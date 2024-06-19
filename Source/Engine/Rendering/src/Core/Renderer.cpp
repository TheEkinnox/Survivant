#include "SurvivantRendering/Core/Renderer.h"

#include "SurvivantRendering/Components/CameraComponent.h"
#include "SurvivantRendering/Components/LightComponent.h"
#include "SurvivantRendering/Components/ModelComponent.h"
#include "SurvivantRendering/RHI/IFrameBuffer.h"
#include "SurvivantRendering/RHI/IRenderAPI.h"

#include <SurvivantCore/ECS/Components/Hierarchy.h>

using namespace LibMath;

using namespace SvCore::ECS;

using namespace SvRendering::Enums;
using namespace SvRendering::Geometry;
using namespace SvRendering::Components;
using namespace SvRendering::Resources;
using namespace SvRendering::RHI;

namespace SvRendering::Core
{
    namespace
    {
        bool CameraOrderFunc(const CameraComponent& p_left, const CameraComponent& p_right)
        {
            return p_left.IsActive() == p_right.IsActive() ? p_left.GetOrder() < p_right.GetOrder() : p_left.IsActive();
        }
    }

    Renderer::Renderer()
        : m_cameraBuffer(IUniformBuffer::Create(EAccessMode::DYNAMIC_DRAW, CAMERA_UBO_INDEX)),
        m_modelBuffer(IUniformBuffer::Create(EAccessMode::DYNAMIC_DRAW, MODEL_UBO_INDEX))
    {
    }

    std::unique_ptr<IShaderStorageBuffer> Renderer::SetupLightSSBO(const Scene* p_scene)
    {
        std::unique_ptr<IShaderStorageBuffer> lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, LIGHT_SSBO_INDEX);
        UpdateLightSSBO(p_scene, *lightsSSBO);
        return lightsSSBO;
    }

    void Renderer::UpdateLightSSBO(const Scene* p_scene, IShaderStorageBuffer& p_lightsSSBO)
    {
        if (!p_scene)
        {
            p_lightsSSBO.SetRawData(nullptr, 0);
            p_lightsSSBO.Bind();
            return;
        }

        SceneView<const LightComponent> view(*p_scene);
        std::vector<Matrix4>            lightMatrices;

        for (const auto entity : view)
        {
            const LightComponent& light = *view.Get<const LightComponent>(entity);
            lightMatrices.emplace_back(light.GetMatrix(p_scene->Get<Transform>(entity)));
        }

        p_lightsSSBO.SetData(lightMatrices.data(), lightMatrices.size());
        p_lightsSSBO.Bind();
    }

    void Renderer::Render(RenderInfo& p_renderPass) const
    {
        ASSERT(p_renderPass.m_scene);

        if (p_renderPass.m_target)
            p_renderPass.m_target->Bind();
        else
            IFrameBuffer::BindDefault();

        if (p_renderPass.m_shaderOverride)
            p_renderPass.m_shaderOverride->Bind();

        m_cameraBuffer->Bind();
        m_modelBuffer->Bind();

        SceneView<const ModelComponent, const Transform> drawables(*p_renderPass.m_scene);

        p_renderPass.m_drawables = &drawables;

        if (p_renderPass.m_camera)
        {
            UpdateCamera(p_renderPass);
        }
        else
        {
            SceneView<CameraComponent> cameras(*p_renderPass.m_scene);
            cameras.Sort<CameraComponent>(&CameraOrderFunc);

            for (const auto camEntity : cameras)
            {
                p_renderPass.m_camera = {
                    .m_camera = cameras.Get<CameraComponent>(camEntity),
                    .m_transform = p_renderPass.m_scene->Get<Transform>(camEntity)
                };

                UpdateCamera(p_renderPass);
            }
        }

        m_cameraBuffer->Unbind();
        m_modelBuffer->Unbind();

        if (p_renderPass.m_material)
            p_renderPass.m_material->GetShader().Unbind();

        if (p_renderPass.m_shaderOverride)
            p_renderPass.m_shaderOverride->Unbind();

        if (p_renderPass.m_target)
            p_renderPass.m_target->Unbind();
    }

    void Renderer::UpdateCamera(RenderInfo& p_renderPass) const
    {
        if (floatEquals(p_renderPass.m_aspect, 0.f))
            return;

        const auto& [cam, transform] = p_renderPass.m_camera;

        ASSERT(cam, "Attempted to render from null camera");

        if (!cam->IsActive())
            return;

        const LayerMask cullingMask = cam->GetCullingMask();

        if (cullingMask == Layer::NONE)
            return;

        p_renderPass.m_cullingMask = cullingMask;

        cam->SetAspect(p_renderPass.m_aspect);

        CameraUBO camUBO;

        if (transform)
        {
            cam->Recalculate(transform->getWorldMatrix().inverse());
            camUBO.m_viewPos = transform->getWorldPosition();
        }
        else
        {
            cam->Recalculate(Matrix4(1.f));
        }

        camUBO.m_viewProjection = (*cam)->GetViewProjection().transposed();
        m_cameraBuffer->SetData(&camUBO, 1);

        p_renderPass.m_frustum = &(*cam)->GetFrustum();

        if (!p_renderPass.m_clearColorOverride && !p_renderPass.m_clearFlagsOverride)
        {
            cam->Clear();
        }
        else
        {
            IRenderAPI& api = IRenderAPI::GetCurrent();

            const Color initialColor = api.GetClearColor();

            api.SetClearColor(p_renderPass.m_clearColorOverride ? *p_renderPass.m_clearColorOverride : cam->GetClearColor());
            bool color, depth, stencil;

            if (p_renderPass.m_clearFlagsOverride)
                CameraComponent::BreakClearMask(*p_renderPass.m_clearFlagsOverride, color, depth, stencil);
            else
                cam->GetClearMask(color, depth, stencil);

            api.Clear(color, depth, stencil).SetClearColor(initialColor);
        }

        DrawModels(p_renderPass);
    }

    void Renderer::DrawModels(RenderInfo& p_renderPass) const
    {
        ASSERT(p_renderPass.m_drawables && p_renderPass.m_frustum && p_renderPass.m_cullingMask != Layer::NONE);

        auto&                                             api       = IRenderAPI::GetCurrent();
        SceneView<const ModelComponent, const Transform>& drawables = *p_renderPass.m_drawables;

        for (const auto& drawableEntity : drawables)
        {
            const auto& [model, material, layerMask] = *drawables.Get<const ModelComponent>(drawableEntity);

            if (!model || !material || !(p_renderPass.m_cullingMask & layerMask))
                continue;

            const Transform& transform = *drawables.Get<const Transform>(drawableEntity);
            const Matrix4    modelMat  = transform.getWorldMatrix();

            if (!p_renderPass.m_frustum->intersects(TransformBoundingBox(model->GetBoundingBox(), modelMat)))
                continue;

            ModelUBO modelUbo
            {
                .m_modelMat = modelMat.transposed(),
                .m_normalMat = modelMat.inverse()
            };

            m_modelBuffer->SetData(&modelUbo, 1);

            DrawInfo drawInfo = {
                .m_model = model,
                .m_material = material,
                .m_modelMat = modelMat,
                .m_layerMask = layerMask,
                .m_entity = drawableEntity,
                .m_renderPass = &p_renderPass
            };

            if (p_renderPass.m_onBeforeDraw)
                p_renderPass.m_onBeforeDraw(drawInfo);

            if (!p_renderPass.m_shaderOverride && p_renderPass.m_material != material)
            {
                material->Bind();
                p_renderPass.m_material = material;
            }

            for (size_t i = 0; i < model->GetMeshCount(); ++i)
            {
                const Mesh& mesh = model->GetMesh(i);

                mesh.Bind();
                api.DrawElements(EPrimitiveType::TRIANGLES, mesh.GetIndexCount());
            }

            if (p_renderPass.m_onAfterDraw)
                p_renderPass.m_onAfterDraw(drawInfo);
        }
    }
}

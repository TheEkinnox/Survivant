//WorldContext.cpp
#include "SurvivantApp/Core/RenderingContext.h"

#include <SurvivantRendering/RHI/IFrameBuffer.h>
#include <SurvivantRendering/RHI/IRenderAPI.h>
#include <SurvivantRendering/RHI/ITexture.h>

using namespace LibMath;

using namespace SvCore::ECS;

using namespace SvRendering::RHI;
using namespace SvRendering::Core;
using namespace SvRendering::Enums;
using namespace SvRendering::Components;

namespace SvApp::Core
{
    RenderingContext::RenderingContext(
        const MainCamera::Cam& p_cam, const Transform& p_trans)
        : m_mainCamera(p_cam, p_trans)
    {
    }

    RenderingContext::RenderingContext(const SvCore::ECS::EntityHandle p_entity)
        : m_mainCamera(p_entity)
    {
    }

    void RenderingContext::Render(Scene* p_scene)
    {
        IRenderAPI::GetCurrent().SetViewport(PosT::zero(), m_viewport);

        Renderer::RenderInfo renderInfo{
            .m_aspect = GetAspect(),
            .m_scene = p_scene
        };

        if (m_frameBuffers.empty())
        {
            m_renderer.Render(renderInfo);
            return;
        }

        for (size_t i = 0; i < m_frameBuffers.size(); i++)
        {
            if (!p_scene)
            {
                m_frameBuffers[i]->Bind();
                IRenderAPI::GetCurrent().SetClearColor(Color::black);
                IRenderAPI::GetCurrent().Clear(true, false, false);
                m_frameBuffers[i]->Unbind();
            }
            else
            {
                renderInfo.m_target = m_frameBuffers[i].get();

                switch (m_renderTypes[i])
                {
                case ERenderType::GAME:
                    GameRender(renderInfo);
                    break;
                case ERenderType::SCENE:
                    SceneRender(renderInfo);
                    break;
                case ERenderType::ID:
                    IdRender(renderInfo);
                    break;
                case ERenderType::DEFAULT:
                default:
                    m_renderer.Render(renderInfo);
                    break;
                }
            }
        }
    }

    void* RenderingContext::GetTextureId(const ETextureType p_renderType)
    {
        ASSERT(!m_frameTextures.empty(), "World has no textures");

        for (size_t i = 0; i < m_textureTypeBuffer.size(); i++)
        {
            if (m_textureTypeBuffer[i] == p_renderType)
                return m_frameTextures[i]->GetHandle();
        }

        ASSERT(false, "Rendering context does not contain render type");
        return m_frameTextures[0]->GetHandle();
    }

    SvCore::ECS::Entity RenderingContext::GetEntityIdValue(const Vec2& p_uv, Scene* p_scene)
    {
        ASSERT(!m_frameTextures.empty(), "World has no textures");

        SvRendering::RHI::IFrameBuffer* idBuff = nullptr;
        for (size_t i = 0; i < m_renderTypes.size(); i++)
        {
            if (m_renderTypes[i] == ERenderType::ID)
            {
                idBuff = m_frameBuffers[i].get();
                break;
            }
        }

        ASSERT(idBuff != nullptr, "Rendering context does not contain render type");

        idBuff->Bind();
        int val;

        IRenderAPI::GetCurrent().ReadPixels(
            p_uv * m_viewport, { 1, 1 }, EPixelDataFormat::RED_INT, EPixelDataType::INT, &val);

        idBuff->Unbind();

        return TextureValueToEntity(val, p_scene);
    }

    void RenderingContext::GameRender(Renderer::RenderInfo& p_renderInfo) const
    {
        m_renderer.Render(p_renderInfo);
    }

    void RenderingContext::IdRender(Renderer::RenderInfo& p_renderInfo)
    {
        const auto [cam, transform] = m_mainCamera.GetCamInfo();
        if (!(cam && transform))
            return;

        p_renderInfo.m_camera = { cam, transform };

        static auto editorSceneShader     = CreateEditorSceneShader();
        p_renderInfo.m_shaderOverride     = editorSceneShader.get();
        p_renderInfo.m_clearColorOverride = &Color::black;
        p_renderInfo.m_onBeforeDraw       = &OnIdDraw;

        const uint8_t clearMask           = CameraComponent::PackClearMask(true, true, false);
        p_renderInfo.m_clearFlagsOverride = &clearMask;

        m_renderer.Render(p_renderInfo);
    }

    void RenderingContext::SceneRender(Renderer::RenderInfo& p_renderInfo)
    {
        const auto [cam, transform] = m_mainCamera.GetCamInfo();
        if (!(cam && transform))
            return;

        p_renderInfo.m_camera       = { cam, transform };
        p_renderInfo.m_onBeforeDraw = &OnBeforeSceneDraw;
        p_renderInfo.m_onAfterDraw  = &OnAfterSceneDraw;

        IRenderAPI::GetCurrent().Clear(true, true, true);
        m_renderer.Render(p_renderInfo);
    }

    void RenderingContext::AddRenderPass(ERenderType p_type)
    {
        switch (p_type)
        {
        case ERenderType::GAME:
            AddColorRenderPass();
            break;
        case ERenderType::SCENE:
            AddColorRenderPass();
            break;
        case ERenderType::ID:
            AddIdRenderPass();
            break;
        default:
            break;
        }

        m_renderTypes.push_back(p_type);
    }

    RenderingContext::CameraInfo RenderingContext::GetCameraInfo()
    {
        return m_mainCamera.GetCamInfo();
    }

    RenderingContext::Vec2& RenderingContext::CameraMoveInput()
    {
        return *m_mainCamera.MoveInput();
    }

    RenderingContext::Vec2& RenderingContext::CameraRotateInput()
    {
        return *m_mainCamera.RotateInput();
    }

    void RenderingContext::UpdateCameraInput()
    {
        m_mainCamera.UpdateInput();
    }

    void RenderingContext::Resize(const Vector2I& p_size)
    {
        //textures
        m_viewport = p_size;

        for (const auto& frameTexture : m_frameTextures)
            frameTexture->Resize(m_viewport.m_x, m_viewport.m_y);

        ResetCameraAspect();
    }

    float RenderingContext::GetAspect() const
    {
        return static_cast<float>(m_viewport.m_x) / static_cast<float>(m_viewport.m_y);
    }

    void RenderingContext::ResetCameraAspect()
    {
        auto [cam, trans] = m_mainCamera.GetCamInfo();

        if (cam)
            cam->SetAspect(GetAspect());
    }

    const Renderer& RenderingContext::GetRenderer() const
    {
        return m_renderer;
    }

    void RenderingContext::AddColorRenderPass()
    {
        std::shared_ptr<ITexture> color = m_frameTextures.emplace_back(CreateTexture(ETextureType::COLOR));
        m_textureTypeBuffer.push_back(ETextureType::COLOR);

        std::shared_ptr<ITexture> depth = m_frameTextures.emplace_back(CreateTexture(ETextureType::DEPTH));
        m_textureTypeBuffer.push_back(ETextureType::DEPTH);

        color->Bind(0);

        const auto& frameBuffer = m_frameBuffers.emplace_back(IFrameBuffer::Create());
        frameBuffer->Attach(*color, EFrameBufferAttachment::COLOR);
        frameBuffer->Attach(*depth, EFrameBufferAttachment::DEPTH);
    }

    void RenderingContext::AddIdRenderPass()
    {
        std::shared_ptr<ITexture> id = m_frameTextures.emplace_back(CreateTexture(ETextureType::ID));
        m_textureTypeBuffer.push_back(ETextureType::ID);

        std::shared_ptr<ITexture> depth = m_frameTextures.emplace_back(CreateTexture(ETextureType::DEPTH));
        m_textureTypeBuffer.push_back(ETextureType::DEPTH);

        id->SetFilters(ETextureFilter::NEAREST, ETextureFilter::NEAREST);
        id->Bind(0);

        const auto frameBuffer = m_frameBuffers.emplace_back(IFrameBuffer::Create()).get();
        frameBuffer->Attach(*id, EFrameBufferAttachment::COLOR);
        frameBuffer->Attach(*depth, EFrameBufferAttachment::DEPTH);
    }

    RenderingContext::TexturePtr RenderingContext::CreateTexture(const ETextureType& p_type) const
    {
        switch (p_type)
        {
        case ETextureType::COLOR:
            return ITexture::Create(m_viewport.m_x, m_viewport.m_y, EPixelDataFormat::RGB);
        case ETextureType::DEPTH:
            return ITexture::Create(m_viewport.m_x, m_viewport.m_y,
                EPixelDataFormat::DEPTH_COMPONENT);
        case ETextureType::ID:
            return ITexture::Create(m_viewport.m_x, m_viewport.m_y,
                EPixelDataFormat::RED_INT_32, EPixelDataFormat::RED_INT,
                EPixelDataType::UNSIGNED_INT);
        default:
            return nullptr;
        }
    }

    std::shared_ptr<IShader> RenderingContext::CreateEditorSceneShader()
    {
        std::shared_ptr<IShader> shader = IShader::Create();

        bool result = shader->Load(EDITOR_SCENE_SHADER_PATH);
        ASSERT(result, "Failed to load shader at path \"%s\"", EDITOR_SCENE_SHADER_PATH);

        result &= shader->Init();
        ASSERT(result, "Failed to initialize shader at path \"%s\"", EDITOR_SCENE_SHADER_PATH);
        (void)result;

        return shader;
    }

    int RenderingContext::EntityToTextureValue(const Entity p_entity)
    {
        uint32_t index = static_cast<uint32_t>(p_entity.GetIndex());
        return (*reinterpret_cast<int*>(&index)) + 1;
    }

    Entity RenderingContext::TextureValueToEntity(const int p_value, Scene* p_scene)
    {
        const uint32_t index = (*reinterpret_cast<const uint32_t*>(&p_value)) - 1;
        return p_scene->Find(static_cast<Entity::Index>(index));
    }

    void RenderingContext::OnIdDraw(const Renderer::DrawInfo& p_drawInfo)
    {
        IShader& shader = *p_drawInfo.m_renderPass->m_shaderOverride;
        shader.SetUniformInt("u_entityID", EntityToTextureValue(p_drawInfo.m_entity));
    }

    void RenderingContext::OnBeforeSceneDraw(const Renderer::DrawInfo& p_drawInfo)
    {
        if (p_drawInfo.m_entity == s_editorSelectedEntity.GetEntity())
        {
            Vector4& colorRef = p_drawInfo.m_material->GetProperty<Vector4>("u_tint");
            colorRef *= SELECTION_TINT;
            p_drawInfo.m_material->Bind();
        }
    }

    void RenderingContext::OnAfterSceneDraw(const Renderer::DrawInfo& p_drawInfo)
    {
        if (p_drawInfo.m_entity == s_editorSelectedEntity.GetEntity())
        {
            Vector4& colorRef = p_drawInfo.m_material->GetProperty<Vector4>("u_tint");
            colorRef /= SELECTION_TINT;
            p_drawInfo.m_material->Bind();
        }
    }
}

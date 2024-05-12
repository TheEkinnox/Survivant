#include "SurvivantTest/TestApp.h"

#include "SurvivantTest/ResourceRegistrations.h"
#include "SurvivantTest/SceneTest.h"

#include <SurvivantCore/ECS/SceneView.h>
#include <SurvivantCore/ECS/Components/Hierarchy.h>
#include <SurvivantCore/Utility/FileSystem.h>
#include <SurvivantCore/Utility/Timer.h>

#include <SurvivantRendering/Components/CameraComponent.h>
#include <SurvivantRendering/Components/LightComponent.h>
#include <SurvivantRendering/Components/ModelComponent.h>
#include <SurvivantRendering/Core/Camera.h>
#include <SurvivantRendering/Enums/ETextureFilter.h>
#include <SurvivantRendering/Enums/ETextureWrapMode.h>
#include <SurvivantRendering/RHI/IRenderAPI.h>
#include <SurvivantRendering/RHI/ITexture.h>
#include <SurvivantRendering/RHI/IUniformBuffer.h>

#include <SurvivantScripting/LuaContext.h>
#include <SurvivantScripting/LuaScriptList.h>

#include <Transform.h>

using namespace LibMath;

using namespace SvApp;

using namespace SvCore::ECS;
using namespace SvCore::Resources;
using namespace SvCore::Utility;

using namespace SvRendering::RHI;
using namespace SvRendering::Core;
using namespace SvRendering::Enums;
using namespace SvRendering::Geometry;
using namespace SvRendering::Resources;
using namespace SvRendering::Components;

using namespace SvScripting;

constexpr const char* UNLIT_SHADER_PATH = "shaders/Unlit.glsl";
constexpr const char* LIT_SHADER_PATH   = "shaders/Lit.glsl";
constexpr const char* TEXTURE_PATH      = "textures/grid.png";

constexpr float  CAM_MOVE_SPEED     = 3.f;
constexpr Radian CAM_ROTATION_SPEED = 90_deg;

constexpr size_t TEST_SCRIPTS_COUNT = 250;

namespace SvTest
{
    void TestApp::Init()
    {
        SvCore::Debug::Logger::GetInstance().SetFile("debug.log");
        ResourceManager::GetInstance().AddSearchPath("assets");

        ASSERT(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");
        SV_LOG("Current working directory: \"%s\"", GetWorkingDirectory().c_str());

        m_window = std::make_unique<Window>("Survivant - Test", 1024, 768, 512, 384);

        IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
        renderAPI.Init(true)
                 .SetCapability(ERenderingCapability::DEPTH_TEST, true)
                 .SetCullFace(ECullFace::BACK);

        SetupInput();

        const ResourceRef<ITexture> texture(TEXTURE_PATH);
        ASSERT(texture, "Failed to load texture at path \"%s\"", TEXTURE_PATH);

        texture->SetFilters(ETextureFilter::NEAREST, ETextureFilter::NEAREST);
        texture->SetWrapModes(ETextureWrapMode::REPEAT, ETextureWrapMode::REPEAT);

        m_lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, 0);
    }

    void TestApp::Run()
    {
        RunSceneTests();

        LuaContext& luaContext = LuaContext::GetInstance();
        luaContext.Init();

        Timer::GetInstance().Reset();
        MakeScene();

        Timer&        timer = Timer::GetInstance();
        InputManager& input = InputManager::GetInstance();

        while (!m_window->ShouldClose())
        {
            input.Update();

            timer.Tick();
            m_window->Update();

            luaContext.Update(timer.GetDeltaTime());

            IRenderAPI::GetCurrent().Clear(true, true, true);
            DrawScene();

            m_window->EndRender();
        }

        luaContext.Stop();
        m_scene.Clear();
        luaContext.Reset();
    }

    void TestApp::SetupInput()
    {
        InputManager& im = InputManager::GetInstance();
        im.InitWindow(m_window.get());
        im.SetInputBindings(std::make_shared<InputManager::InputBindings>());

        auto& keyCallbacks = im.m_bindings->m_keyCallbacks;

        keyCallbacks[{ EKey::W, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            ++m_moveInput.m_y;
        };

        keyCallbacks[{ EKey::W, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            --m_moveInput.m_y;
        };

        keyCallbacks[{ EKey::S, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            --m_moveInput.m_y;
        };

        keyCallbacks[{ EKey::S, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            ++m_moveInput.m_y;
        };

        keyCallbacks[{ EKey::A, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            --m_moveInput.m_x;
        };

        keyCallbacks[{ EKey::A, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            ++m_moveInput.m_x;
        };

        keyCallbacks[{ EKey::D, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            ++m_moveInput.m_x;
        };

        keyCallbacks[{ EKey::D, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            --m_moveInput.m_x;
        };

        keyCallbacks[{ EKey::UP, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            ++m_rotateInput.m_y;
        };

        keyCallbacks[{ EKey::UP, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            --m_rotateInput.m_y;
        };

        keyCallbacks[{ EKey::DOWN, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            --m_rotateInput.m_y;
        };

        keyCallbacks[{ EKey::DOWN, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            ++m_rotateInput.m_y;
        };

        keyCallbacks[{ EKey::LEFT, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            --m_rotateInput.m_x;
        };

        keyCallbacks[{ EKey::LEFT, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            ++m_rotateInput.m_x;
        };

        keyCallbacks[{ EKey::RIGHT, EKeyState::PRESSED, EInputModifier() }] = [this](const char)
        {
            ++m_rotateInput.m_x;
        };

        keyCallbacks[{ EKey::RIGHT, EKeyState::RELEASED, EInputModifier() }] = [this](const char)
        {
            --m_rotateInput.m_x;
        };

        keyCallbacks[{ EKey::R, EKeyState::RELEASED, {} }] = [this](const char)
        {
            MakeScene();
        };
    }

    void TestApp::MakeScene()
    {
        m_scene.Clear();
        LuaContext& luaContext = LuaContext::GetInstance();
        luaContext.Reload();

        EntityHandle camEntity = m_scene.Create();

        CameraComponent& cam = camEntity.Make<CameraComponent>();
        cam.SetClearColor(Color::gray).SetPerspective(90_deg, .01f, 14.f).SetAspect(4.f / 3.f);

        const Vector3 camPos(0.f, 1.8f, 2.f);
        camEntity.Make<Transform>(camPos, Quaternion::identity(), Vector3::one());
        camEntity.Make<LuaScriptList>().Add("scripts.freeLook");

        ResourceRef<Model> cube("models/cube.obj");
        ASSERT(cube, "Failed to load model at path \"%s\"", "models/cube.obj");

        ResourceRef<IShader> unlitShader(UNLIT_SHADER_PATH);
        ASSERT(unlitShader, "Failed to load shader at path \"%s\"", UNLIT_SHADER_PATH);

        ResourceRef<IShader> litShader(LIT_SHADER_PATH);
        ASSERT(litShader, "Failed to load shader at path \"%s\"", LIT_SHADER_PATH);

        ResourceRef<ITexture> texture(TEXTURE_PATH);
        ASSERT(texture, "Failed to load texture at path \"%s\"", TEXTURE_PATH);

        ResourceRef<::Material> whiteMaterial("materials/white.mat");
        ResourceRef<::Material> redMaterial("materials/red.mat");
        ResourceRef<::Material> greenMaterial("materials/green.mat");
        ResourceRef<::Material> blueMaterial("materials/blue.mat");
        ResourceRef<::Material> yellowMaterial("materials/yellow.mat");
        ResourceRef<::Material> magentaMaterial("materials/magenta.mat");
        ResourceRef<::Material> litMaterial("materials/lit.mat");

        EntityHandle whiteCube = m_scene.Create();
        whiteCube.Make<ModelComponent>(cube, whiteMaterial);
        whiteCube.Make<Transform>(Vector3(-.75f, .5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle redCube = m_scene.Create();
        redCube.Make<ModelComponent>(cube, redMaterial);
        redCube.Make<Transform>(Vector3(.75f, .5f, 0.f), Quaternion::identity(), Vector3::one());

        {
            LuaScriptList& scripts = redCube.Make<LuaScriptList>();
            scripts.Add("scripts.rotator");
            scripts.Add("scripts.temporary");
        }

        EntityHandle greenCube = m_scene.Create();
        greenCube.Make<ModelComponent>(cube, greenMaterial);
        greenCube.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());
        greenCube.Make<HierarchyComponent>(redCube.GetEntity());

        EntityHandle yellowCube = m_scene.Create();
        yellowCube.Make<ModelComponent>(cube, yellowMaterial);
        yellowCube.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());
        yellowCube.Make<HierarchyComponent>(greenCube.GetEntity());

        EntityHandle yellowCube2 = m_scene.Create();
        yellowCube2.Make<ModelComponent>(cube, yellowMaterial);
        yellowCube2.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());
        yellowCube2.Make<HierarchyComponent>(yellowCube.GetEntity());

        EntityHandle yellowCube3 = m_scene.Create();
        yellowCube3.Make<ModelComponent>(cube, yellowMaterial);
        yellowCube3.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());
        yellowCube3.Make<HierarchyComponent>(yellowCube2.GetEntity());

        EntityHandle yellowCube4 = m_scene.Create();
        yellowCube4.Make<ModelComponent>(cube, yellowMaterial);
        yellowCube4.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());
        yellowCube4.Make<HierarchyComponent>(yellowCube3.GetEntity());

        EntityHandle greenCube2 = m_scene.Create();
        greenCube2.Make<ModelComponent>(cube, greenMaterial);
        greenCube2.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());
        greenCube2.Make<HierarchyComponent>(yellowCube4.GetEntity());

        EntityHandle magentaCube = m_scene.Create();
        magentaCube.Make<ModelComponent>(cube, magentaMaterial);
        magentaCube.Make<HierarchyComponent>(greenCube.GetEntity());
        magentaCube.Make<Transform>(Vector3(1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle blueCube = m_scene.Create();
        blueCube.Make<ModelComponent>(cube, blueMaterial);
        blueCube.Make<HierarchyComponent>(redCube.GetEntity());
        blueCube.Make<Transform>(Vector3(-1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle magentaCube2 = m_scene.Create();
        magentaCube2.Make<ModelComponent>(cube, magentaMaterial);
        magentaCube2.Make<HierarchyComponent>(yellowCube3.GetEntity());
        magentaCube2.Make<Transform>(Vector3(-1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle blueCube2 = m_scene.Create();
        blueCube2.Make<ModelComponent>(cube, blueMaterial);
        blueCube2.Make<HierarchyComponent>(magentaCube2.GetEntity());
        blueCube2.Make<Transform>(Vector3(-1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle blueCube3 = m_scene.Create();
        blueCube3.Make<ModelComponent>(cube, blueMaterial);
        blueCube3.Make<HierarchyComponent>(greenCube2.GetEntity());
        blueCube3.Make<Transform>(Vector3(1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle magentaCube3 = m_scene.Create();
        magentaCube3.Make<ModelComponent>(cube, magentaMaterial);
        magentaCube3.Make<HierarchyComponent>(blueCube3.GetEntity());
        magentaCube3.Make<Transform>(Vector3(1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle litCube = m_scene.Create();
        litCube.Make<ModelComponent>(cube, litMaterial);
        litCube.Make<Transform>(camPos + Vector3::front(), Quaternion::identity(), Vector3(1.5f, .5f, .1f));

        EntityHandle litCube2 = m_scene.Create();
        litCube2.Make<ModelComponent>(cube, litMaterial);
        litCube2.Make<HierarchyComponent>(blueCube.GetEntity());
        litCube2.Make<Transform>(Vector3(-1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle litCube3 = m_scene.Create();
        litCube3.Make<ModelComponent>(cube, litMaterial);
        litCube3.Make<HierarchyComponent>(litCube2.GetEntity());
        litCube3.Make<Transform>(Vector3(1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle litCube4 = m_scene.Create();
        litCube4.Make<ModelComponent>(cube, litMaterial);
        litCube4.Make<HierarchyComponent>(magentaCube.GetEntity());
        litCube4.Make<Transform>(Vector3(1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle litCube5 = m_scene.Create();
        litCube5.Make<ModelComponent>(cube, litMaterial);
        litCube5.Make<HierarchyComponent>(litCube4.GetEntity());
        litCube5.Make<Transform>(Vector3(-1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        m_scene.Create().Make<LightComponent>(Light(Color::lime));
        m_scene.Create().Make<LightComponent>(DirectionalLight{ Color::magenta, Vector3::back() });
        m_scene.Create().Set<LightComponent>(PointLight{ Color::red, Vector3{ -1, 1, 1 }, Attenuation(16) });

        SpotLight spot{ Color(0.f, 1.f, 0.f, 3.f), camPos, Vector3::front(), Attenuation(10), Cutoff{ cos(0_deg), cos(30_deg) } };
        m_scene.Create().Make<LightComponent>(spot);

        UpdateLightSSBO();

        for (size_t i = 0; i < TEST_SCRIPTS_COUNT; ++i)
        {
            LuaScriptList& scripts = m_scene.Create().Make<LuaScriptList>();
            scripts.Add("scripts/test.lua");

            ASSERT(luaContext.IsValid());
        }

        luaContext.Start();
    }

    void TestApp::UpdateLightSSBO() const
    {
        SceneView<const LightComponent> view(m_scene);
        std::vector<Matrix4>            lightMatrices;

        for (const auto entity : view)
        {
            const LightComponent& light = *view.Get<const LightComponent>(entity);

            switch (light.m_type)
            {
            case ELightType::AMBIENT:
                lightMatrices.emplace_back(light.m_ambient.getMatrix());
                break;
            case ELightType::DIRECTIONAL:
                lightMatrices.emplace_back(light.m_directional.getMatrix());
                break;
            case ELightType::POINT:
                lightMatrices.emplace_back(light.m_point.getMatrix());
                break;
            case ELightType::SPOT:
                lightMatrices.emplace_back(light.m_spot.getMatrix());
                break;
            }
        }

        m_lightsSSBO->Bind();
        m_lightsSSBO->SetData(lightMatrices.data(), lightMatrices.size());
    }

    void TestApp::DrawScene()
    {
        SceneView<CameraComponent>                       cameras(m_scene);
        SceneView<const ModelComponent, const Transform> renderables(m_scene);

        for (const auto camEntity : cameras)
        {
            CameraComponent& cam = *cameras.Get<CameraComponent>(camEntity);

            if (const Transform* transform = m_scene.Get<const Transform>(camEntity))
            {
                cam.Recalculate(transform->getWorldMatrix().inverse());
                BindCamUBO(cam->GetViewProjection(), transform->getWorldPosition());
            }
            else
            {
                cam.Recalculate(Matrix4(1.f));
                BindCamUBO(cam->GetViewProjection(), Vector3::zero());
            }

            cam.Clear();
            const Frustum camFrustum = cam->GetFrustum();

            for (const auto modelEntity : renderables)
            {
                const auto [model, transform] = renderables.Get(modelEntity);
                ASSERT(model->m_model && model->m_material);
                DrawModel(*model->m_model, camFrustum, transform->getWorldMatrix(), *model->m_material);
            }
        }
    }

    void TestApp::BindCamUBO(const Matrix4& p_viewProj, const Vector3& p_viewPos)
    {
        static std::unique_ptr<IUniformBuffer> camBuffer = IUniformBuffer::Create(EAccessMode::DYNAMIC_DRAW, 0);

        struct CameraUBO
        {
            Matrix4 m_viewProjection;
            Vector3 m_viewPos;
        };

        CameraUBO ubo
        {
            p_viewProj.transposed(),
            p_viewPos
        };

        camBuffer->SetData(&ubo, 1);
        camBuffer->Bind();
    }

    void TestApp::BindModelUBO(const Matrix4& p_modelMat)
    {
        static std::unique_ptr<IUniformBuffer> modelBuffer = IUniformBuffer::Create(EAccessMode::DYNAMIC_DRAW, 1);

        struct ModelUBO
        {
            Matrix4 m_modelMat;
            Matrix4 m_normalMat;
        };

        ModelUBO ubo
        {
            p_modelMat.transposed(),
            p_modelMat.inverse()
        };

        modelBuffer->SetData(&ubo, 1);
        modelBuffer->Bind();
    }

    void TestApp::DrawModel(
        const Model& p_model, const Frustum& p_viewFrustum, const Matrix4& p_transform, const Material& p_material)
    {
        if (!p_viewFrustum.intersects(TransformBoundingBox(p_model.GetBoundingBox(), p_transform)))
            return;

        BindModelUBO(p_transform);

        p_material.Bind();
        p_material.GetShader().SetUniformMat4("sv_modelMat", p_transform);
        p_material.GetShader().SetUniformMat4("sv_normalMat", p_transform.transposed().inverse());

        for (size_t i = 0; i < p_model.GetMeshCount(); ++i)
        {
            const Mesh& mesh = p_model.GetMesh(i);

            mesh.Bind();
            IRenderAPI::GetCurrent().DrawElements(EPrimitiveType::TRIANGLES, mesh.GetIndexCount());
        }
    }
}

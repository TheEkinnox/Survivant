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
#include <SurvivantRendering/RHI/IRenderAPI.h>
#include <SurvivantRendering/RHI/IUniformBuffer.h>

#include <SurvivantScripting/LuaContext.h>
#include <SurvivantScripting/LuaScriptList.h>

#include <Transform.h>

using namespace LibMath;

using namespace SvApp;

using namespace SvCore::ECS;
using namespace SvCore::Events;
using namespace SvCore::Resources;
using namespace SvCore::Utility;

using namespace SvRendering::RHI;
using namespace SvRendering::Enums;
using namespace SvRendering::Resources;
using namespace SvRendering::Components;

using namespace SvScripting;

constexpr const char* TEST_SCENE_PATH = "scenes/test_no_scripts.scn";

constexpr float  CAM_MOVE_SPEED     = 3.f;
constexpr Radian CAM_ROTATION_SPEED = 90_deg;

constexpr size_t TEST_SCRIPTS_COUNT = 256;

constexpr int WINDOW_WIDTH  = 1024;
constexpr int WINDOW_HEIGHT = 768;

namespace SvTest
{
    TestApp::TestApp()
    {
        m_resizeListenerId = EventManager::GetInstance().AddListenner<Window::OnFrameBufferSize>(
            [this](const int p_width, const int p_height)
            {
                m_windowSize = { p_width, p_height };
                IRenderAPI::GetCurrent().SetViewport(PosT::zero(), m_windowSize);
            }
        );
    }

    TestApp::~TestApp()
    {
        EventManager::GetInstance().RemoveListenner<Window::OnFrameBufferSize>(m_resizeListenerId);
    }

    void TestApp::Init()
    {
        SvCore::Debug::Logger::GetInstance().SetFile("debug.log");
        ResourceManager::GetInstance().AddSearchPath("assets");

        ASSERT(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");
        SV_LOG("Current working directory: \"%s\"", GetWorkingDirectory().c_str());

        m_window = std::make_unique<Window>("Survivant - Test", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
        m_windowSize = { WINDOW_WIDTH, WINDOW_HEIGHT };

        IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
        renderAPI.Init(true)
                 .SetCapability(ERenderingCapability::DEPTH_TEST, true)
                 .SetCullFace(ECullFace::BACK);

        SetupInput();

        m_lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, 0);
    }

    void TestApp::Run()
    {
        RunSceneTests();

        LuaContext& luaContext = LuaContext::GetInstance();
        luaContext.Init();

        Timer::GetInstance().Refresh();
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
        if (m_scene)
            m_scene->Clear();

        Timer::GetInstance().Refresh();

        LuaContext& luaContext = LuaContext::GetInstance();
        luaContext.Reload();

        m_scene = ResourceManager::GetInstance().Load<Scene>(m_scene.GetPath());

        UpdateLightSSBO();

        for (size_t i = 0; i < TEST_SCRIPTS_COUNT; ++i)
        {
            LuaScriptList& scripts = m_scene->Create().Make<LuaScriptList>();
            scripts.Add("scripts/test.lua");

            ASSERT(luaContext.IsValid());
        }

        luaContext.Start();
    }

    void TestApp::UpdateLightSSBO() const
    {
        SceneView<const LightComponent> view(*m_scene);
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

    void TestApp::DrawScene() const
    {
        SceneView<CameraComponent>                       cameras(*m_scene);
        SceneView<const ModelComponent, const Transform> renderables(*m_scene);

        const float aspect = static_cast<float>(m_windowSize.m_x) / static_cast<float>(m_windowSize.m_y);

        for (const auto camEntity : cameras)
        {
            CameraComponent& cam = *cameras.Get<CameraComponent>(camEntity);
            cam.SetAspect(aspect);

            if (const Transform* transform = m_scene->Get<const Transform>(camEntity))
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

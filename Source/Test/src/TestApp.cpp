#include "SurvivantTest/TestApp.h"

#include "SurvivantTest/ResourceRegistrations.h"
#include "SurvivantTest/RotatorComponent.h"
#include "SurvivantTest/SceneTest.h"
#include "SurvivantTest/TemporaryComponent.h"
#include "SurvivantTest/UserInputComponent.h"

#include <SurvivantCore/ECS/SceneView.h>
#include <SurvivantCore/ECS/Components/Hierarchy.h>
#include <SurvivantCore/Utility/FileSystem.h>

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

            UpdateTemporaries();
            UpdateInput();
            UpdateRotators();

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
        LuaContext& luaContext = LuaContext::GetInstance();
        luaContext.Reload();
        m_scene.Clear();

        EntityHandle camEntity = m_scene.Create();

        CameraComponent& cam = camEntity.Make<CameraComponent>();
        cam.SetClearColor(Color::gray).SetPerspective(90_deg, .01f, 14.f).SetAspect(4.f / 3.f);

        const Vector3 camPos(0.f, 1.8f, 2.f);
        camEntity.Make<Transform>(camPos, Quaternion::identity(), Vector3::one());

        camEntity.Make<UserInput>(CAM_MOVE_SPEED, CAM_ROTATION_SPEED);

        ResourceRef<Model> cube("models/cube.obj");
        ASSERT(cube, "Failed to load model at path \"%s\"", "models/cube.obj");

        ResourceRef<IShader> unlitShader(UNLIT_SHADER_PATH);
        ASSERT(unlitShader, "Failed to load shader at path \"%s\"", UNLIT_SHADER_PATH);

        ResourceRef<IShader> litShader(LIT_SHADER_PATH);
        ASSERT(litShader, "Failed to load shader at path \"%s\"", LIT_SHADER_PATH);

        ResourceRef<ITexture> texture(TEXTURE_PATH);
        ASSERT(texture, "Failed to load texture at path \"%s\"", TEXTURE_PATH);

        ResourceRef whiteMaterial("", new Material(unlitShader));
        whiteMaterial->GetProperty<ResourceRef<ITexture>>("u_diffuse") = texture;
        whiteMaterial->GetProperty<Vector4>("u_tint")                  = Color::white;

        ResourceRef redMaterial("", new Material(*whiteMaterial));
        redMaterial->GetProperty<Vector4>("u_tint") = Color::red;

        ResourceRef greenMaterial("", new Material(*whiteMaterial));
        greenMaterial->GetProperty<Vector4>("u_tint") = Color::green;

        ResourceRef blueMaterial("", new Material(*whiteMaterial));
        blueMaterial->GetProperty<Vector4>("u_tint") = Color::blue;

        ResourceRef yellowMaterial("", new Material(*whiteMaterial));
        yellowMaterial->GetProperty<Vector4>("u_tint") = Color::yellow;

        ResourceRef magentaMaterial("", new Material(*whiteMaterial));
        magentaMaterial->GetProperty<Vector4>("u_tint") = Color::magenta;

        ResourceRef litMaterial("", new Material(litShader));
        litMaterial->GetProperty<ResourceRef<ITexture>>("u_diffuse") = texture;
        litMaterial->GetProperty<Vector4>("u_tint")                  = Color::white;
        litMaterial->GetProperty<Vector4>("u_specularColor")         = Color(.2f, .2f, .2f);
        litMaterial->GetProperty<float>("u_shininess")               = 32.f;

        EntityHandle whiteCube = m_scene.Create();
        whiteCube.Make<ModelComponent>(cube, whiteMaterial);
        whiteCube.Make<Transform>(Vector3(-.75f, .5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle redCube = m_scene.Create();
        redCube.Make<ModelComponent>(cube, redMaterial);
        redCube.Make<Rotator>(Quaternion(36_deg, Vector3::up()));
        redCube.Make<Transform>(Vector3(.75f, .5f, 0.f), Quaternion::identity(), Vector3::one());
        redCube.Make<Temporary>(5.f);

        EntityHandle greenCube = m_scene.Create();
        greenCube.Make<ModelComponent>(cube, greenMaterial);
        greenCube.Make<HierarchyComponent>(redCube.GetEntity());
        greenCube.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle yellowCube = m_scene.Create();
        yellowCube.Make<ModelComponent>(cube, yellowMaterial);
        yellowCube.Make<HierarchyComponent>(greenCube.GetEntity());
        yellowCube.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle magentaCube = m_scene.Create();
        magentaCube.Make<ModelComponent>(cube, magentaMaterial);
        magentaCube.Make<HierarchyComponent>(greenCube.GetEntity());
        magentaCube.Make<Transform>(Vector3(1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle blueCube = m_scene.Create();
        blueCube.Make<ModelComponent>(cube, blueMaterial);
        blueCube.Make<HierarchyComponent>(redCube.GetEntity());
        blueCube.Make<Transform>(Vector3(-1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle litCube = m_scene.Create();
        litCube.Make<ModelComponent>(cube, litMaterial);
        litCube.Make<Transform>(camPos + Vector3::front(), Quaternion::identity(), Vector3(1.5f, .5f, .1f));

        m_scene.Create().Make<LightComponent>(Light(cam.GetClearColor()));
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

    void TestApp::UpdateTemporaries()
    {
        const float deltaTime = Timer::GetInstance().GetDeltaTime();

        SceneView<Temporary> view(m_scene);

        for (const auto entity : view)
        {
            auto& [lifeSpanSec] = *view.Get<Temporary>(entity);
            lifeSpanSec -= deltaTime;

            if (lifeSpanSec <= 0)
                EntityHandle(&view.GetScene(), entity).Destroy();
        }
    }

    void TestApp::UpdateInput()
    {
        const float deltaTime = Timer::GetInstance().GetDeltaTime();

        SceneView<const UserInput, Transform> view(m_scene);

        for (const auto entity : view)
        {
            const auto [moveSpeed, rotationSpeed] = *view.Get<const UserInput>(entity);
            Transform& transform                  = *view.Get<Transform>(entity);
            Vector3    newPos                     = transform.getPosition();
            Quaternion newRot                     = transform.getRotation();

            if (m_moveInput.magnitudeSquared() > 0.f)
            {
                const Vector3 moveDir = m_moveInput.m_x * transform.worldRight() + m_moveInput.m_y * transform.worldBack();
                newPos += moveDir.normalized() * (moveSpeed * deltaTime);
            }

            if (m_rotateInput.magnitudeSquared() > 0.f)
            {
                TVector3<Radian> angles = newRot.toEuler(ERotationOrder::YXZ);

                angles.m_y -= rotationSpeed * m_rotateInput.m_x * deltaTime;
                angles.m_x += rotationSpeed * m_rotateInput.m_y * deltaTime;
                angles.m_x = Degree(clamp(angles.m_x.degree(true), -85.f, 85.f));

                newRot = Quaternion::fromEuler(angles, ERotationOrder::YXZ);
            }

            transform.setAll(newPos, newRot, Vector3::one());
        }
    }

    void TestApp::UpdateRotators()
    {
        const float deltaTime = Timer::GetInstance().GetDeltaTime();

        SceneView<const Rotator, Transform> view(m_scene);

        for (const auto entity : view)
        {
            const auto [rotator, transform] = view.Get(entity);
            transform->rotate(rotator->m_speed.toEuler(ERotationOrder::YXZ) * deltaTime, ERotationOrder::YXZ);
        }
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
        if (!p_viewFrustum.Intersects(TransformBoundingBox(p_model.GetBoundingBox(), p_transform)))
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

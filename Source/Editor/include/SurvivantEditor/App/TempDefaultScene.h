//TempDefaultScene.h
#pragma once

#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/ECS/EntityHandle.h>
#include <SurvivantCore/ECS/Scene.h>
#include <SurvivantCore/ECS/SceneView.h>
#include <SurvivantCore/ECS/Components/Hierarchy.h>
#include <SurvivantCore/Utility/FileSystem.h>
#include <SurvivantCore/Utility/Timer.h>

#include <SurvivantRendering/Components/LightComponent.h>
#include <SurvivantRendering/Components/ModelComponent.h>
#include <SurvivantRendering/Core/Camera.h>
#include <SurvivantRendering/RHI/IFrameBuffer.h>
#include <SurvivantRendering/RHI/IRenderAPI.h>
#include <SurvivantRendering/RHI/IShader.h>
#include <SurvivantRendering/RHI/IShaderStorageBuffer.h>
#include <SurvivantRendering/RHI/ITexture.h>
#include <SurvivantRendering/RHI/IUniformBuffer.h>
#include <SurvivantRendering/RHI/OpenGL/OpenGLTexture.h>

#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantApp/Inputs/KeyboardInputs.h"
#include "SurvivantApp/Inputs/MouseInputs.h"
#include "SurvivantApp/Windows/Window.h"

#include "SurvivantEditor/App/EngineApp.h"

using namespace LibMath;
using namespace SvCore::ECS;
using namespace SvCore::Utility;
using namespace SvRendering::Core;
using namespace SvRendering::Components;
using namespace SvRendering::Enums;
using namespace SvRendering::Geometry;
using namespace SvRendering::Resources;
using namespace SvRendering::RHI;


struct Rotator
{
    Quaternion m_speed;
};

template <>
inline void ComponentTraits::OnAdd<Rotator>(EntityHandle& p_handle, Rotator&)
{
    if (!p_handle.Has<Transform>())
        p_handle.Make<Transform>();
}

namespace ToRemove
{
    constexpr const char* UNLIT_SHADER_PATH = "assets/shaders/Unlit.glsl";
    constexpr const char* LIT_SHADER_PATH = "assets/shaders/Lit.glsl";

    constexpr float  CAM_MOVE_SPEED = 3.f;
    constexpr Radian CAM_ROTATION_SPEED = 90_deg;
    const Vector3 INITIAL_CAM_POS = Vector3(0.0f, 1.8f, 2.f);

    //std::shared_ptr<Scene>                scene;
    //SceneView<Temporary>                  temporariesView(scene);
    //SceneView<const UserInput, Transform> userInputsView(scene);
    //SceneView<const Rotator, Transform>   rotatorsView(scene);
    
    class GameInfo
    {
    public:
        static inline Vector2 moveInput, rotateInput = Vector2::zero();
        static inline EntityHandle gameCamera = EntityHandle();
    };

    //std::unique_ptr<IShaderStorageBuffer> lightsSSBO = SetupLightSSBO(scene);

    struct UserInput
    {
        float  m_moveSpeed;
        Radian m_rotationSpeed;
    };

    struct Temporary
    {
        float m_lifeSpanSec;
    };

    inline std::shared_ptr<ITexture> GetTexture()
    {
        static std::shared_ptr<ITexture> texture = ITexture::Create();
        static bool                      isLoaded = false;

        if (isLoaded)
            return texture;

        ASSERT(texture->Load("assets/textures/grid.png"));
        ASSERT(texture->Init());

        texture->SetFilters(ETextureFilter::NEAREST, ETextureFilter::NEAREST);
        texture->SetWrapModes(ETextureWrapMode::REPEAT, ETextureWrapMode::REPEAT);

        isLoaded = true;

        return texture;
    }

    static inline std::unique_ptr<IFrameBuffer> g_frameBuffer;

    inline ITexture& GetDefaultFrameBuffer()
    {
        static std::shared_ptr<ITexture> color = ITexture::Create(800, 600, EPixelDataFormat::RGB);
        static std::shared_ptr<ITexture> depth = ITexture::Create(800, 600, EPixelDataFormat::DEPTH_COMPONENT);

        static bool isInitialized = false;

        if (isInitialized)
            return *color;

        color->Bind(0);

        g_frameBuffer = IFrameBuffer::Create();
        g_frameBuffer->Attach(*color, EFrameBufferAttachment::COLOR);
        g_frameBuffer->Attach(*depth, EFrameBufferAttachment::DEPTH);

        isInitialized = true;
        return *color;
    }

    std::tuple<int, int> inline AddInputTranslate(char i)
    {
        return { i, 10 };
    }

    std::tuple<int, int> inline AddMouseTranslate(float i, float j)
    {
        return { (int)i, (int)j };
    }

    std::tuple<> inline SpaceTranslate(char /*c*/)
    {
        return { };
    }

    void inline BindCamUBO(const Matrix4& p_viewProj, const Vector3& p_viewPos)
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

    void inline BindModelUBO(const Matrix4& p_modelMat)
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

    void inline DrawModel(const Model& p_model, const Frustum& p_viewFrustum, const Matrix4& p_transform, const Material& p_material)
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

    void inline TestScene()
    {
        const auto onAddEntity = [](const Entity p_entity)
            {
                SV_LOG("Created entity %d:%d", p_entity.GetIndex(), p_entity.GetVersion());
            };

        const auto onRemoveEntity = [](const Entity p_entity)
            {
                SV_LOG("Removed entity %d:%d", p_entity.GetIndex(), p_entity.GetVersion());
            };

        const auto onAddInt = [](const Entity p_owner, const int& p_val)
            {
                SV_LOG("Added int %d to entity %d:%d", p_val, p_owner.GetIndex(), p_owner.GetVersion());
            };

        const auto onBeforeChangeInt = [](const Entity p_owner, const int& p_val)
            {
                SV_LOG("Changing int %d of entity %d:%d", p_owner.GetIndex(), p_owner.GetVersion(), p_val);
            };

        const auto onChangeInt = [](const Entity p_owner, const int& p_val)
            {
                SV_LOG("Changed int of entity %d:%d to %d", p_owner.GetIndex(), p_owner.GetVersion(), p_val);
            };

        const auto onRemoveInt = [](const Entity p_owner, const int& p_val)
            {
                SV_LOG("Removed int %d from entity %d:%d", p_val, p_owner.GetIndex(), p_owner.GetVersion());
            };

        Scene scene;

        scene.GetStorage<Entity>().m_onAdd.AddListener(onAddEntity);
        scene.GetStorage<Entity>().m_onRemove.AddListener(onRemoveEntity);

        scene.GetStorage<int>().m_onAdd.AddListener(onAddInt);
        scene.GetStorage<int>().m_onBeforeChange.AddListener(onBeforeChangeInt);
        scene.GetStorage<int>().m_onChange.AddListener(onChangeInt);
        scene.GetStorage<int>().m_onRemove.AddListener(onRemoveInt);

        scene.Make<int>(scene.Create(), 0);
        scene.Make<int>(scene.Create(), 1);
        scene.Make<int>(scene.Create(), 2);
        scene.Make<int>(scene.Create(), 3);

        EntityHandle lastEntity = scene.Create();

        {
            const int& i = scene.Make<int>(lastEntity, 999);
            CHECK(i == 999, "Constructing a component should return a reference to said component");

            scene.Set<int>(lastEntity, 4);
            CHECK(i == 4, "Setting a component for an existing entity should have updated it's value");
        }

        CHECK(scene.GetStorage<Entity>().GetCount() == 5);
        CHECK(scene.GetStorage<int>().GetCount() == 5);

        scene.Remove<const int>(Entity(1));
        CHECK(scene.GetStorage<int>().GetCount() == 4);

        scene.Remove<int>(*scene.Get<int>(lastEntity));
        CHECK(scene.GetStorage<int>().GetCount() == 3);

        scene.Remove<int>(lastEntity);
        CHECK(scene.GetStorage<int>().GetCount() == 3);

        SceneView<int, char, float> view(scene);

        lastEntity.Set<int>(8);
        lastEntity.Set<char>('j');
        lastEntity.Set<float>(2.5f);

        CHECK(scene.GetStorage<int>().GetCount() == 4);
        CHECK(scene.GetStorage<float>().GetCount() == 1);
        CHECK(scene.GetStorage<char>().GetCount() == 1);

        const char c1 = *view.Get<char>(lastEntity);
        auto [i1, f1] = view.Get<int, float>(lastEntity);

        CHECK(c1 == 'j');
        CHECK(i1 && *i1 == 8);
        CHECK(f1 && floatEquals(*f1, 2.5f));

        auto [i, c, f] = view.Get(lastEntity);

        CHECK(i && *i == 8);
        CHECK(c && *c == 'j');
        CHECK(f && floatEquals(*f, 2.5f));

        int count = 0;
        for (const auto entity : view)
        {
            CHECK(++count == 1, "Only one relevant entity should have been found");

            auto [iComp, fComp, cComp] = view.Get<int, const float, char>(entity);

            CHECK(iComp && *iComp == 8);
            CHECK(cComp && *cComp == 'j');
            CHECK(fComp && floatEquals(*fComp, 2.5f));

            *iComp = 123;
            *cComp = 'z';
        }

        auto [i2, c2, f2] = view.Get(lastEntity);

        CHECK(i2 && *i2 == 123);
        CHECK(c2 && *c2 == 'z');
        CHECK(f2 && floatEquals(*f2, 2.5f));

        lastEntity.Destroy();

        CHECK(scene.GetStorage<Entity>().GetCount() == 4);
        CHECK(scene.GetStorage<int>().GetCount() == 3);
        CHECK(scene.GetStorage<float>().GetCount() == 0);
        CHECK(scene.GetStorage<char>().GetCount() == 0);

        auto [iDestroyed, cDestroyed, fDestroyed] = view.Get(lastEntity);

        CHECK(iDestroyed == nullptr);
        CHECK(cDestroyed == nullptr);
        CHECK(fDestroyed == nullptr);

        for ([[maybe_unused]] const auto _ : view)
            CHECK(false, "The only releveant entity should have been destroyed");

        lastEntity = scene.Create();

        CHECK(scene.GetStorage<Entity>().GetCount() == 5);
        CHECK(scene.GetStorage<int>().GetCount() == 3);
        CHECK(scene.GetStorage<float>().GetCount() == 0);
        CHECK(scene.GetStorage<char>().GetCount() == 0);

        for ([[maybe_unused]] const auto _ : view)
            CHECK(false, "The only releveant entity should have been destroyed");
    }

    void inline MakeScene(Scene& p_scene)
    {
        p_scene.Clear();
        EntityHandle camEntity = p_scene.Create();
        GameInfo::gameCamera = camEntity;

        Camera& cam = camEntity.Make<Camera>(perspectiveProjection(90_deg, 4.f / 3.f, .01f, 14.f));
        cam.SetClearColor(Color::gray);

        const Vector3 camPos(0.f, 1.8f, 2.f);
        camEntity.Make<Transform>(camPos, Quaternion::identity(), Vector3::one());

        camEntity.Make<UserInput>(CAM_MOVE_SPEED, CAM_ROTATION_SPEED);

        std::shared_ptr<Model> cube = std::make_shared<Model>();

        ASSERT(cube->Load("assets/models/cube.obj"), "Failed to load model");
        ASSERT(cube->Init(), "Failed to initialize model");

        std::shared_ptr<IShader> unlitShader = IShader::Create();
        ASSERT(unlitShader->Load(UNLIT_SHADER_PATH), "Failed to load shader at path \"%s\"", UNLIT_SHADER_PATH);
        ASSERT(unlitShader->Init(), "Failed to initialize shader at path \"%s\"", UNLIT_SHADER_PATH);

        std::shared_ptr<IShader> litShader = IShader::Create();
        ASSERT(litShader->Load(LIT_SHADER_PATH), "Failed to load shader at path \"%s\"", LIT_SHADER_PATH);
        ASSERT(litShader->Init(), "Failed to initialize shader at path \"%s\"", LIT_SHADER_PATH);

        std::shared_ptr<Material> whiteMaterial = std::make_shared<Material>(unlitShader);
        whiteMaterial->GetProperty<std::shared_ptr<ITexture>>("u_diffuse") = GetTexture();
        whiteMaterial->GetProperty<Vector4>("u_tint") = Color::white;

        std::shared_ptr<Material> redMaterial = std::make_shared<Material>(*whiteMaterial);
        redMaterial->GetProperty<Vector4>("u_tint") = Color::red;

        std::shared_ptr<Material> greenMaterial = std::make_shared<Material>(*whiteMaterial);
        greenMaterial->GetProperty<Vector4>("u_tint") = Color::green;

        std::shared_ptr<Material> blueMaterial = std::make_shared<Material>(*whiteMaterial);
        blueMaterial->GetProperty<Vector4>("u_tint") = Color::blue;

        std::shared_ptr<Material> yellowMaterial = std::make_shared<Material>(*whiteMaterial);
        yellowMaterial->GetProperty<Vector4>("u_tint") = Color::yellow;

        std::shared_ptr<Material> magentaMaterial = std::make_shared<Material>(*whiteMaterial);
        magentaMaterial->GetProperty<Vector4>("u_tint") = Color::magenta;

        std::shared_ptr<Material> litMaterial = std::make_shared<Material>(litShader);
        litMaterial->GetProperty<std::shared_ptr<ITexture>>("u_diffuse") = GetTexture();
        litMaterial->GetProperty<Vector4>("u_tint") = Color::white;
        litMaterial->GetProperty<Vector4>("u_specularColor") = Color(.2f, .2f, .2f);
        litMaterial->GetProperty<float>("u_shininess") = 32.f;

        EntityHandle whiteCube = p_scene.Create();
        whiteCube.Make<ModelComponent>(cube, whiteMaterial);
        whiteCube.Make<Transform>(Vector3(-.75f, .5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle redCube = p_scene.Create();
        redCube.Make<ModelComponent>(cube, redMaterial);
        redCube.Make<Rotator>(Quaternion(36_deg, Vector3::up()));
        redCube.Make<Transform>(Vector3(.75f, .5f, 0.f), Quaternion::identity(), Vector3::one());
        redCube.Make<Temporary>(5.f);

        EntityHandle greenCube = p_scene.Create();
        greenCube.Make<ModelComponent>(cube, greenMaterial);
        greenCube.Make<HierarchyComponent>(redCube.GetEntity());
        greenCube.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle yellowCube = p_scene.Create();
        yellowCube.Make<ModelComponent>(cube, yellowMaterial);
        yellowCube.Make<HierarchyComponent>(greenCube.GetEntity());
        yellowCube.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle magentaCube = p_scene.Create();
        magentaCube.Make<ModelComponent>(cube, magentaMaterial);
        magentaCube.Make<HierarchyComponent>(greenCube.GetEntity());
        magentaCube.Make<Transform>(Vector3(1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle blueCube = p_scene.Create();
        blueCube.Make<ModelComponent>(cube, blueMaterial);
        blueCube.Make<HierarchyComponent>(redCube.GetEntity());
        blueCube.Make<Transform>(Vector3(-1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle litCube = p_scene.Create();
        litCube.Make<ModelComponent>(cube, litMaterial);
        litCube.Make<Transform>(camPos + Vector3::front(), Quaternion::identity(), Vector3(1.5f, .5f, .1f));

        p_scene.Create().Make<LightComponent>(Light(cam.GetClearColor()));
        p_scene.Create().Make<LightComponent>(DirectionalLight{ Color::magenta, Vector3::back() });
        p_scene.Create().Set<LightComponent>(PointLight{ Color::red, Vector3{ -1, 1, 1 }, Attenuation(16) });

        SpotLight spot{ Color(0.f, 1.f, 0.f, 3.f), camPos, Vector3::front(), Attenuation(10), Cutoff{ cos(0_deg), cos(30_deg) } };
        p_scene.Create().Make<LightComponent>(spot);
    }

    std::unique_ptr<IShaderStorageBuffer> inline SetupLightSSBO(const Scene& p_scene)
    {
        SceneView<const LightComponent> view(p_scene);
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

        std::unique_ptr<IShaderStorageBuffer> lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, 0);
        lightsSSBO->Bind();
        lightsSSBO->SetData(lightMatrices.data(), lightMatrices.size());

        return lightsSSBO;
    }

    void inline DrawScene(Scene& p_scene)
    {
        SceneView<Camera>                                cameras(p_scene);
        SceneView<const ModelComponent, const Transform> renderables(p_scene);

        for (const auto camEntity : cameras)
        {
            Camera& cam = *cameras.Get<Camera>(camEntity);

            if (const Transform* transform = p_scene.Get<const Transform>(camEntity))
            {
                cam.SetView(transform->getWorldMatrix().inverse());
                BindCamUBO(cam.GetViewProjection(), transform->getWorldPosition());
            }
            else
            {
                BindCamUBO(cam.GetViewProjection(), Vector3::zero());
            }

            cam.Clear();
            const Frustum camFrustum = cam.GetFrustum();

            for (const auto modelEntity : renderables)
            {
                const auto [model, transform] = renderables.Get(modelEntity);
                ASSERT(model->m_model && model->m_material);
                DrawModel(*model->m_model, camFrustum, transform->getWorldMatrix(), *model->m_material);
            }
        }
    }

    void inline UpdateTemporaries(SceneView<Temporary>& p_view, const float p_deltaTime)
    {
        for (const auto entity : p_view)
        {
            auto& [lifeSpanSec] = *p_view.Get<Temporary>(entity);
            lifeSpanSec -= p_deltaTime;

            if (lifeSpanSec <= 0)
                EntityHandle(&p_view.GetScene(), entity).Destroy();
        }
    }

    void inline UpdateInput(SceneView<const UserInput, Transform> p_view, const Vector2& p_move, const Vector2& p_rotation, const float p_dt)
    {
        for (const auto entity : p_view)
        {
            const auto [moveSpeed, rotationSpeed] = *p_view.Get<const UserInput>(entity);
            Transform& transform = *p_view.Get<Transform>(entity);
            Vector3    newPos = transform.getPosition();
            Quaternion newRot = transform.getRotation();

            if (p_move.magnitudeSquared() > 0.f)
            {
                const Vector3 moveDir = p_move.m_x * transform.worldRight() + p_move.m_y * transform.worldBack();
                newPos += moveDir.normalized() * (moveSpeed * p_dt);
            }

            if (p_rotation.magnitudeSquared() > 0.f)
            {
                TVector3<Radian> angles = newRot.toEuler(ERotationOrder::YXZ);

                angles.m_y -= rotationSpeed * p_rotation.m_x * p_dt;
                angles.m_x += rotationSpeed * p_rotation.m_y * p_dt;
                angles.m_x = Degree(clamp(angles.m_x.degree(true), -85.f, 85.f));

                newRot = Quaternion::fromEuler(angles, ERotationOrder::YXZ);
            }

            transform.setAll(newPos, newRot, Vector3::one());
        }
    }

    void inline UpdateRotators(SceneView<const Rotator, Transform>& p_view, const float p_deltaTime)
    {
        for (const auto entity : p_view)
        {
            const auto [rotator, transform] = p_view.Get(entity);
            transform->rotate(rotator->m_speed.toEuler(ERotationOrder::YXZ) * p_deltaTime, ERotationOrder::YXZ);
        }
    }

    using namespace LibMath;
    using namespace SvRendering::RHI;
    using namespace SvRendering::Enums;


    static inline Material whiteMaterial;
    static inline Material redMaterial;
    static inline Material litMaterial;

    std::shared_ptr<Scene> inline TestCreateDefaultScene()
    {
        std::shared_ptr<Scene> scene = std::make_shared<Scene>();
        TestScene();
        MakeScene(*scene);
        //m_lightsSSBO = SetupLightSSBO(*scene);

        return scene;
    }

    void inline TestLevelBeginPlay(Scene& p_scene)
    {
        MakeScene(p_scene);
        //GameInfo::gameCamera.Get<Transform>()->setAll(INITIAL_CAM_POS, Quaternion::identity(), Vector3::one());
    }

    void inline SetupGameInputs()
    {
        using namespace SvCore;
        using namespace SvApp;
        using namespace Events;
        using namespace SvEditor::App;

        static bool firstTime = true;
        static std::shared_ptr<InputManager::InputBindings> bindings = std::make_shared<InputManager::InputBindings>();

        InputManager& im = InputManager::GetInstance();

        im.SetInputBindings(bindings);

        if (!firstTime)
            return;
        firstTime = false;

        auto& k = bindings->m_keyCallbacks;
        k.emplace(InputManager::KeyboardKeyType{ EKey::W, EKeyState::PRESSED, EInputModifier() }, [](const char)
            {
                ++GameInfo::moveInput.m_y;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::W, EKeyState::RELEASED, EInputModifier() }, [](const char)
            {
                --GameInfo::moveInput.m_y;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::S, EKeyState::PRESSED, EInputModifier() }, [](const char)
            {
                --GameInfo::moveInput.m_y;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::S, EKeyState::RELEASED, EInputModifier() }, [](const char)
            {
                ++GameInfo::moveInput.m_y;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::A, EKeyState::PRESSED, EInputModifier() }, [](const char)
            {
                --GameInfo::moveInput.m_x;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::A, EKeyState::RELEASED, EInputModifier() }, [](const char)
            {
                ++GameInfo::moveInput.m_x;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::D, EKeyState::PRESSED, EInputModifier() }, [](const char)
            {
                ++GameInfo::moveInput.m_x;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::D, EKeyState::RELEASED, EInputModifier() }, [](const char)
            {
                --GameInfo::moveInput.m_x;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::UP, EKeyState::PRESSED, EInputModifier() }, [](const char)
            {
                ++GameInfo::rotateInput.m_y;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::UP, EKeyState::RELEASED, EInputModifier() }, [](const char)
            {
                --GameInfo::rotateInput.m_y;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::DOWN, EKeyState::PRESSED, EInputModifier() }, [](const char)
            {
                --GameInfo::rotateInput.m_y;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::DOWN, EKeyState::RELEASED, EInputModifier() }, [](const char)
            {
                ++GameInfo::rotateInput.m_y;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::LEFT, EKeyState::PRESSED, EInputModifier() }, [](const char)
            {
                --GameInfo::rotateInput.m_x;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::LEFT, EKeyState::RELEASED, EInputModifier() }, [](const char)
            {
                ++GameInfo::rotateInput.m_x;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::RIGHT, EKeyState::PRESSED, EInputModifier() }, [](const char)
            {
                ++GameInfo::rotateInput.m_x;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::RIGHT, EKeyState::RELEASED, EInputModifier() }, [](const char)
            {
                --GameInfo::rotateInput.m_x;
            });

        k.emplace(InputManager::KeyboardKeyType{ EKey::R, EKeyState::RELEASED, {} }, [](const char)
            {
                GameInfo::gameCamera.Get<Transform>()->setAll(INITIAL_CAM_POS, Quaternion::identity(), Vector3::one());
            });
    }
}

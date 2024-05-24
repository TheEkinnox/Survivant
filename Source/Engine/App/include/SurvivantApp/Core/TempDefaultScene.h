//TempDefaultScene.h
#pragma once

#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/ECS/EntityHandle.h>

#include <SurvivantRendering/Components/LightComponent.h>
#include <SurvivantRendering/Components/ModelComponent.h>

#include "SurvivantApp/Inputs/InputManager.h"

namespace ToRemove
{
    using namespace LibMath;
    using namespace SvCore::ECS;
    using namespace SvRendering::RHI;

    constexpr const char* UNLIT_SHADER_PATH = "assets/shaders/Unlit.glsl";
    constexpr const char* LIT_SHADER_PATH = "assets/shaders/Lit.glsl";
    constexpr const char* EDITORSCENE_SHADER_PATH = "assets/shaders/EditorScene.glsl";

    constexpr float  CAM_MOVE_SPEED = 3.f;
    constexpr Radian CAM_ROTATION_SPEED = 90_deg;
    const Vector3 INITIAL_CAM_POS = Vector3(0.0f, 1.8f, 2.f);

    class GameInfo
    {
    public:
        static inline Vector2 moveInput, rotateInput = Vector2::zero();
        static inline EntityHandle gameCamera = EntityHandle();
    };

    std::shared_ptr<IShader> inline CreateEditorSceneShader()
    {
        std::shared_ptr<IShader> shader = IShader::Create();
        ASSERT(shader->Load(EDITORSCENE_SHADER_PATH), "Failed to load shader at path \"%s\"", EDITORSCENE_SHADER_PATH);
        ASSERT(shader->Init(), "Failed to initialize shader at path \"%s\"", EDITORSCENE_SHADER_PATH);

        return shader;
    }

    int inline EntityToTextureValue(const Entity& p_entity)
    {
        uint32_t index = static_cast<uint32_t>(p_entity.GetIndex());
        return (*reinterpret_cast<int*>(&index)) + 1;
    }

    Entity inline TextureValueToEntity(const int& p_value, Scene* p_scene)
    {
        const uint32_t index = (*reinterpret_cast<const uint32_t*>(&p_value)) - 1;
        return p_scene->Find(static_cast<Entity::Index>(index));
    }

    std::shared_ptr<SvApp::InputManager::InputBindings> inline SetupGameInputs()
    {
        using namespace SvCore;
        using namespace SvApp;
        using namespace Events;

        std::shared_ptr<InputManager::InputBindings> bindings = std::make_shared<InputManager::InputBindings>();

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
                if (Transform* transform = GameInfo::gameCamera.Get<Transform>())
                    transform->setAll(INITIAL_CAM_POS, Quaternion::identity(), Vector3::one());
            });

        return bindings;
    }
}

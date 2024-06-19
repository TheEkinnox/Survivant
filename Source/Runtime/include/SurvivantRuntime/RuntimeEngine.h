//RuntimeEngine.h
#pragma once

#include <SurvivantApp/Core/IEngine.h>
#include <SurvivantApp/Core/GameInstance.h>
#include <SurvivantCore/ECS/EntityHandle.h>
#include <SurvivantCore/Utility/Timer.h>

#include <memory>

namespace SvPhysics
{
    class PhysicsContext;
}

namespace SvAudio
{
    class AudioContext;
}

namespace SvScripting
{
    class LuaContext;
}

namespace SvRuntime
{
    class EditorWindow;

    class RuntimeEngine : public SvApp::Core::IEngine
    {
    public:
        RuntimeEngine();
        ~RuntimeEngine() override;

        // Inherited via IEngine
        void     Init() override;
        void     Update() override;
        void     BakeLights() override;
        SceneRef GetCurrentScene() const override;
        void     ChangeScene(const std::string& p_scenePath) override;
        bool     ChangeCamera(const SvCore::ECS::EntityHandle& p_camera) override;
        float    GetDeltaTime() override;

        bool IsRunning() const;

        void UpdateGame();
        void Render() const;
        void SetViewport(const LibMath::Vector2I& p_size);

    private:
        bool InitializeGameInstance();
        bool ChangeSceneInternal();
        void StartGame();

        using GameInstancePtr = std::unique_ptr<SvApp::Core::GameInstance>;
        using WorldContextPtr = std::shared_ptr<SvApp::Core::WorldContext>;

        WorldContextPtr CreateGameWorld();
        GameInstancePtr CreateGameInstance();
        SceneRef        GetStartScene();

        std::unique_ptr<SvScripting::LuaContext>            m_luaContext;
        std::unique_ptr<SvPhysics::PhysicsContext>          m_physicsContext;
        std::unique_ptr<SvAudio::AudioContext>              m_audioContext;

        SvCore::Utility::Timer m_time;
        bool                   m_isRunning = true;
        WorldContextPtr        m_world;
        GameInstancePtr        m_game;

        std::string m_scenePath;
    };
}

//EditorEngine.h
#pragma once

#include <SurvivantApp/Core/BuildConfig.h>
#include <SurvivantApp/Core/GameInstance.h>
#include <SurvivantApp/Core/IEngine.h>
#include <SurvivantApp/Inputs/InputManager.h>

#include <SurvivantAudio/AudioContext.h>

#include <SurvivantCore/Events/Event.h>
#include <SurvivantCore/Utility/Timer.h>

#include <SurvivantPhysics/PhysicsContext.h>

#include <SurvivantScripting/LuaContext.h>

#include <array>

namespace SvEditor::Core
{
    class EditorWindow;

    class EditorEngine : public SvApp::Core::IEngine
    {
        using WorldContext = SvApp::Core::WorldContext;
        using GameInstance = SvApp::Core::GameInstance;

    public:
        //Events
        class OnCreateBuildGame : public SvCore::Events::Event<
            std::string /*p_buildFileName*/,
            SvApp::Core::BuildConfig /*p_buildInfo*/> {};

        class OnCreateBuildAndRun : public SvCore::Events::Event<
            std::string /*p_buildFileName*/,
            SvApp::Core::BuildConfig /*p_buildInfo*/> {};

        class OnSave : public SvCore::Events::Event<>
        {
        public:
            static inline bool s_saveSucceded = false;
        };

        class OnEditorModifiedScene : public SvCore::Events::Event<> {};

        EditorEngine();
        ~EditorEngine() override;

        // Inherited via IEngine
        void     Init() override;
        void     Update() override;
        void     BakeLights() override;
        SceneRef GetCurrentScene() const override;
        void     ChangeScene(const std::string& p_scenePath) override;
        bool     ChangeCamera(const SvCore::ECS::EntityHandle& p_camera) override;
        float    GetDeltaTime() override;

        void RenderWorlds();

        bool IsPlayInEditor() const;
        bool IsRunning();
        bool IsEditorModifiedScene();
        void TogglePause();
        void SetPaused(bool p_state);
        bool IsPaused() const;
        bool IsGameFocused();

        void SetupUI(Core::EditorWindow* p_window, const std::array<std::function<void()>, 3> p_playPauseFrameCallbacks);

        //create PIE after press play
        std::weak_ptr<GameInstance> CreatePIEGameInstance();
        void                        DestroyGameInstance();

    private:
        using Inputs = SvApp::InputManager::InputBindings;

        void SetupEditorEvents();
        bool ChangeSceneInternal();

        std::string GetTemporaryScenePath() const;
        bool        SaveSceneState() const;
        bool        RestoreSceneState();

        std::shared_ptr<WorldContext> CreateEditorDefaultWorld(const WorldContext::SceneRef& p_inScene);
        std::shared_ptr<WorldContext> CreatePIEWorld();
        std::shared_ptr<Inputs>       CreateEditorInputs();

        SvCore::Utility::Timer m_time;
        SvCore::Utility::Timer m_gameTime;

        bool        m_isRunning             = true;
        bool        m_isPaused              = false;
        bool        m_isEditorModifiedScene = false;
        std::string m_scenePath;

        //always exists
        std::unique_ptr<SvCore::Resources::ResourceManager> m_resourceManager;
        std::unique_ptr<SvScripting::LuaContext>            m_luaContext;
        std::unique_ptr<SvPhysics::PhysicsContext>          m_physicsContext;
        std::unique_ptr<SvAudio::AudioContext>              m_audioContext;

        std::shared_ptr<WorldContext> m_editorWorld;
        WorldContext::SceneRef        m_editorSelectedScene;

        //temporary
        std::shared_ptr<GameInstance>            m_gameInstance;
        std::weak_ptr<WorldContext>              m_PIEWorld;
        std::vector<std::weak_ptr<WorldContext>> m_duplicatedWorlds;
    };
}

//EditorEngine.h
#pragma once

#include "SurvivantApp/Core/IEngine.h"
#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantApp/Core/GameInstance.h"
#include "SurvivantCore/Events/Event.h"
#include <SurvivantCore/Utility/Timer.h>

#include "Transform.h"

#include <array>



using namespace SvApp::Core;
using namespace LibMath;

namespace SvEditor::Core
{
	class EditorWindow;

	class EditorEngine : public SvApp::Core::Engine
	{
	public:
		EditorEngine() = default;
		~EditorEngine() = default;

		// Inherited via Engine
		void Update() override;
		void Init() override;
		void BakeLights() override;

		void RenderWorlds();

		bool IsRunning();

		void SetupUI(Core::EditorWindow* p_window, const std::array<std::function<void()>, 3> p_playPauseFrameCallbacks);

		//bool StartScene(WorldContext& p_worldContext) override;
		bool ChangeScene(const std::string& p_scenePath) override;
		void RedrawViewports() override;
		float GetDeltaTime() override;


		////scene panel, where you mofi
		//void CreateEditorWorld();
		//void CreatePIEWorld();

		//on level switch, will ref new level
		//Scene* GetGameScene();
		//std::shared_ptr<Engine::WorldContext> GetPIEWorldContext();
		
		//create PIE after press play
		std::weak_ptr<GameInstance> CreatePIEGameInstance();
		void DestroyGameInstance();
		
	private:
		static inline const std::string DEFAULT_SCENE_PATH = "assets/scenes/DefaultScene.scn";

		using Inputs = SvApp::InputManager::InputBindings;

		/// <summary>
		/// Changes current level and editorSelectedLevel with levelName, 
		/// make sure that p_scene is in m_allWorlds before browsing to it
		/// </summary>
		/// <param name="p_worldContext">Current world</param>
		/// <param name="p_scene">Scene to go to</param>
		/// <returns>-1 if couldnt, 0 if already there, 1 if properly browsed to</returns>
		int			BrowseToScene(WorldContext& p_worldContext, const std::string& p_path);
		/// <returns>-1 if couldnt, 0 if already there, 1 if properly </returns>
		int			BrowseToDefaultScene(WorldContext& p_worldContext);

		bool InitializePlayInEditorGameInstance(GameInstance& p_instance);

		std::shared_ptr<WorldContext>	CreateEditorDefaultWorld(const WorldContext::SceneRef& p_inScene);
		std::shared_ptr<WorldContext>	CreatePIEWorldByDuplication(const WorldContext& p_context, const WorldContext::SceneRef& p_inScene);
		std::shared_ptr<Inputs>			CreateEditorInputs();

		SvCore::Utility::Timer			m_time;
		bool							m_isRunning = true;

		//always exists
		std::shared_ptr<WorldContext>	m_editorWorld;
		WorldContext::SceneRef			m_editorSelectedScene;

		//temporary
		std::shared_ptr<GameInstance>				m_gameInstance;
		std::weak_ptr<WorldContext>					m_PIEWorld;
		std::vector<std::weak_ptr<WorldContext>>	m_duplicatedWorlds;
	};
}
//RuntimeEngine.h
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

	class EditorEngine : public SvApp::Core::IEngine
	{
	public:
		EditorEngine() = default;
		~EditorEngine() = default;

		// Inherited via IEngine
		void Update() override;
		void Init() override;
		void BakeLights() override;
		bool ChangeScene(const std::string& p_scenePath) override;
		float GetDeltaTime() override;
		bool	IsPlayInEditor()override;


		void RenderWorlds();
		bool IsRunning();

		void SetupUI(Core::EditorWindow* p_window, const std::array<std::function<void()>, 3> p_playPauseFrameCallbacks);

		//bool StartScene(WorldContext& p_worldContext) override;


		////scene panel, where you mofi
		//void CreateEditorWorld();
		//void CreatePIEWorld();

		//on level switch, will ref new level
		//Scene* GetGameScene();
		//std::shared_ptr<IEngine::WorldContext> GetPIEWorldContext();
		
		//create PIE after press play
		std::weak_ptr<GameInstance> CreatePIEGameInstance();
		void DestroyGameInstance();
		
	private:
		using Inputs = SvApp::InputManager::InputBindings;



		bool InitializePlayInEditorGameInstance(GameInstance& p_instance);

		std::shared_ptr<WorldContext>	CreateEditorDefaultWorld(const WorldContext::SceneRef& p_inScene);
		std::shared_ptr<WorldContext>	CreatePIEWorld();
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
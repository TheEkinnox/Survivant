//EditorEngine.h
#pragma once

#include "SurvivantEditor/App/IEngine.h"
#include "SurvivantCore/Events/Event.h"
#include <SurvivantCore/Utility/Timer.h>


#define SV_DELTA_TIME() SvEditor::App::Engine::g_engine->GetDeltaTime()


namespace SvEditor::App
{
	class EditorEngine : public Engine
	{
	public:
		EditorEngine() = default;
		~EditorEngine() = default;

		// Inherited via Engine
		void Update() override;
		void Init() override;
		//bool StartScene(WorldContext& p_worldContext) override;
		bool ChangeScene(const std::string& p_sceneName) override;
		void RedrawViewports() override;
		float GetDeltaTime() override;


		////scene panel, where you mofi
		//void CreateEditorWorld();
		//void CreatePIEWorld();

		//on level switch, will ref new level
		std::shared_ptr<Scene>* GetCurrentScene();
		//std::shared_ptr<Engine::WorldContext> GetPIEWorldContext();
		
		//create PIE after press play
		std::weak_ptr<GameInstance> CreatePIEGameInstance();
		void DestroyGameInstance();
		
	private:
		/// <summary>
		/// Changes current level and editorSelectedLevel with levelName, 
		/// make sure that p_scene is in m_allWorlds before browsing to it
		/// </summary>
		/// <param name="p_worldContext">Current world</param>
		/// <param name="p_scene">Scene to go to</param>
		/// <returns>-1 if couldnt, 0 if already there, 1 if properly browsed to</returns>
		int			BrowseToScene(WorldContext& p_worldContext, std::shared_ptr<Scene> p_scene);
		/// <returns>-1 if couldnt, 0 if already there, 1 if properly </returns>
		int			BrowseToDefaultScene(WorldContext& p_worldContext);

		bool InitializePlayInEditorGameInstance(GameInstance& p_instance);
		std::shared_ptr<WorldContext> CreatePIEWorldByDuplication(WorldContext& p_context, std::shared_ptr<Scene> p_inLevel);
		std::shared_ptr<WorldContext> CreateEditorDefaultWorld();

		SvCore::Utility::Timer			m_time;

		std::shared_ptr<WorldContext>	m_PIEWorld;
		std::shared_ptr<GameInstance>	m_gameInstance;

		std::shared_ptr<Scene>			m_editorSelectedScene;
		std::shared_ptr<WorldContext>	m_editorWorld;
	};
}
//EditorEngine.h
#pragma once

#include "SurvivantEditor/App/IEngine.h"
#include "SurvivantCore/Events/Event.h"



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
		bool LoadNewLevel(WorldContext& p_worldContext, std::shared_ptr<Level> p_level) override;
		void RedrawViewports() override;


		////scene panel, where you mofi
		//void CreateEditorWorld();
		//void CreatePIEWorld();

		//on level switch, will ref new level
		std::shared_ptr<Level>* GetCurrentLevel();
		std::shared_ptr<Engine::WorldContext> GetPIEWorldContext();
		
		//create PIE after press play
		GameInstance* CreatePIEGameInstance();
		void DestroyGameInstance();
		
	private:
		bool InitializePlayInEditorGameInstance(GameInstance& p_instance);
		std::shared_ptr<Engine::WorldContext> CreatePIEWorldByDuplication(WorldContext& p_context, std::shared_ptr<Level> p_inLevel);

		std::shared_ptr<WorldContext>	m_PIEWorld;
		std::unique_ptr<GameInstance>	m_gameInstance;

		std::shared_ptr<Level>			m_editorLevel;
		std::shared_ptr<WorldContext>	m_editorWorld;
	};
}
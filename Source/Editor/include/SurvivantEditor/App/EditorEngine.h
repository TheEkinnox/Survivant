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
		bool StartLevel(WorldContext& p_worldContext) override;
		void RedrawViewports() override;
		float GetDeltaTime() override;


		////scene panel, where you mofi
		//void CreateEditorWorld();
		//void CreatePIEWorld();

		//on level switch, will ref new level
		std::shared_ptr<Level>* GetCurrentLevel();
		std::shared_ptr<Engine::WorldContext> GetPIEWorldContext();
		
		//create PIE after press play
		std::weak_ptr<GameInstance> CreatePIEGameInstance();
		void DestroyGameInstance();
		
	private:
		bool InitializePlayInEditorGameInstance(GameInstance& p_instance);
		std::shared_ptr<Engine::WorldContext> CreatePIEWorldByDuplication(WorldContext& p_context, std::shared_ptr<Level> p_inLevel);
		std::shared_ptr<Engine::WorldContext> CreateEditorDefaultWorld();

		SvCore::Utility::Timer			m_time;

		std::shared_ptr<WorldContext>	m_PIEWorld;
		std::shared_ptr<GameInstance>	m_gameInstance;

		std::shared_ptr<Level>			m_editorLevel;
		std::shared_ptr<WorldContext>	m_editorWorld;


	};
}
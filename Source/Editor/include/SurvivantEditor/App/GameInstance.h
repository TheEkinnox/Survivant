//GameInstance.h
#pragma once

#include "SurvivantApp/Core/IApp.h"
#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/App/IEngine.h"
#include "SurvivantEditor/UI/Core/EditorWindow.h"

namespace SvEditor::App
{
	class GameInstance
	{
	public:
		friend Engine;

		class InitEvent : public SvCore::Events::Event<> {};

		GameInstance() = default;
		~GameInstance() = default;

		void Init();
		void Update();

		//TODO: move to editor Engine
		void InitializeStandalone();

	public:
		Engine* GetEngine();

		std::shared_ptr<Engine::WorldContext> m_worldContext;
	};
}
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

		GameInstance(std::weak_ptr<WorldContext> p_worldContext);
		~GameInstance() = default;

		void Init();
		void Start();
		void Update();

		//TODO: move to editor Engine
		void InitializeStandalone();

	
	private:
		Engine* GetEngine();

		void UpdatePhysics() {}

		std::weak_ptr<WorldContext> m_worldContext;
	};
}
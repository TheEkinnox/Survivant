//GameInstance.h
#pragma once

#include "SurvivantApp/Core/IApp.h"
#include "SurvivantCore/Events/Event.h"
#include "SurvivantApp/Core/IEngine.h"
#include "SurvivantApp/Core/WorldContext.h"

namespace SvApp::Core
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
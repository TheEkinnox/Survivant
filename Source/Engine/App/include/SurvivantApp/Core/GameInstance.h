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
		friend IEngine;

		class InitEvent : public SvCore::Events::Event<> {};

		GameInstance() = default;
		~GameInstance() = default;

		void Init(const std::weak_ptr<WorldContext>& p_worldContext);
		void Start();
		void UpdateScripts();
		void UpdatePhysics();


		void BakeLights();

		//TODO: move to editor IEngine
		void InitializeStandalone();

	
	private:
		IEngine* GetEngine();

		std::weak_ptr<WorldContext> m_worldContext;
	};
}
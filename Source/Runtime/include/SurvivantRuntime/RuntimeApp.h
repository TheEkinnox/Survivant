//RuntimeApp.h
#pragma once

#include "SurvivantRuntime/RuntimeWindow.h"
#include "SurvivantRuntime/RuntimeEngine.h"

#include <SurvivantApp/Core/IApp.h>
#include <SurvivantApp/Core/GameInstance.h>
#include <SurvivantApp/Windows/Window.h>
#include <SurvivantApp/Inputs/InputManager.h>
#include <SurvivantCore/Events/Event.h>
#include <SurvivantRendering/RHI/IRenderAPI.h>
#include <SurvivantRendering/Core/Camera.h>
#include <SurvivantRendering/Resources/Model.h>

#include <Vector/Vector2.h>
#include <Vector/Vector3.h>
#include <Transform.h>


using namespace LibMath;

namespace SvRuntime
{
	class RuntimeApp : public SvApp::Core::IApp
	{
	public:
		RuntimeApp();
		~RuntimeApp() override;

		// Inherited via IApp
		void Init() override;
		void Run() override;

	private:
		void PauseGameInstance();

		std::unique_ptr<SvApp::Window>		m_window;
		bool								m_gameIsPaused;
		RuntimeEngine						m_runEngine;
	};
}
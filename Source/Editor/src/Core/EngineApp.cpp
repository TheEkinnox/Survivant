#include "SurvivantEditor/Core/EngineApp.h"

#include "SurvivantEditor/Core/InspectorItemManager.h"
#include "SurvivantEditor/Panels/HierarchyPanel.h"

#include <SurvivantApp/Inputs/InputManager.h>

#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Debug/Logger.h>
#include <SurvivantCore/Resources/ResourceManager.h>

#include <SurvivantPhysics/PhysicsContext.h>

#include <SurvivantRendering/RHI/IRenderAPI.h>

#include <SurvivantScripting/LuaContext.h>

#include <memory>

using namespace SvRendering::RHI;
using namespace SvRendering::Enums;

using namespace SvCore::Utility;
using namespace SvApp::Core;

namespace SvEditor::Core
{
	EngineApp::EngineApp()
	{
		m_gameInstance = std::weak_ptr<GameInstance>();
	}

	EngineApp::~EngineApp()
	{
		m_window.reset();
	}

	void EngineApp::Init()
	{
		SvCore::Debug::Logger::GetInstance().SetFile("debug.log");
		SvCore::Resources::ResourceManager::GetInstance().AddSearchPath("assets");

		//setup InspectorComponents
		InspectorItemManager::Init();

		//window
		m_window = std::make_unique<Core::EditorWindow>();
		CHECK(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");
		IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
		renderAPI.Init(true)
			.SetCapability(ERenderingCapability::DEPTH_TEST, true)
			.SetCullFace(ECullFace::BACK);

		//TODO: remove this line when draw logo implemented
		//renderAPI.Clear(true, false, false);

		//draw logo
		DrawLogo();

		//engine
		m_editorEngine.Init();
		SvApp::InputManager::GetInstance().InitWindow(m_window.get());

		m_editorEngine.SetupUI(m_window.get(), {
			[this]() { TogglePlayPIE(); },
			[this]() { TogglePausePIE(); },
			[this]() { PressFramePIE(); }
			});
	}

	void EngineApp::Run()
	{
		while (!m_window->ShouldClose() && m_editorEngine.IsRunning())
		{
			m_editorEngine.Update();
			m_window->Update();
			SvApp::InputManager::GetInstance().Update();

			Timer::GetInstance().Tick();
			if (!m_gameInstance.expired() && !m_editorEngine.IsPaused())
				UpdatePIE();

			m_editorEngine.RenderWorlds();	//render worlds
			m_window->RenderUI();			//update UI

			m_window->EndRender();			//render UI with worlds
		}
	}

	void EngineApp::DrawLogo()
	{
		bool check = false;

		do
		{
			m_window->RenderLogo();
			check = !check;

		} while (check); //must have been the wind

		if (bool yes = !check)
			m_window->RenderLogo();
	}

	void EngineApp::TogglePlayPIE()
	{
		const auto currentSelection = RenderingContext::s_editorSelectedEntity.GetEntity().GetIndex();
		HierarchyPanel::ToggleSelectable(SvCore::ECS::NULL_ENTITY.GetIndex());

		if (!m_gameInstance.expired()) //game is running
		{
			m_editorEngine.SetPaused(false);
			m_editorEngine.DestroyGameInstance();
		}
		else //game not running
		{
			m_gameInstance = m_editorEngine.CreatePIEGameInstance();

			if (!m_gameInstance.expired())
			{
				m_gameInstance.lock()->Start();
				m_window->GetUI().ForceGameFocus();
			}
		}

		HierarchyPanel::ToggleSelectable(currentSelection);
	}

	void EngineApp::TogglePausePIE()
	{
		m_editorEngine.TogglePause();
	}

	void EngineApp::PressFramePIE()
	{
		UpdatePIE();
	}

	void EngineApp::UpdatePIE()
	{
		UpdateScripts();
		UpdatePhysics();
	}

	void EngineApp::UpdateScripts()
	{
		SvScripting::LuaContext::GetInstance().Update(Timer::GetInstance().GetDeltaTime());
	}

	void EngineApp::UpdatePhysics()
	{
		SvPhysics::PhysicsContext::GetInstance().Update(Timer::GetInstance().GetDeltaTime());
	}
}


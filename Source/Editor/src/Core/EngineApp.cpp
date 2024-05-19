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

namespace SvEditor::Core
{
	EngineApp::EngineApp() :
		m_gameIsPaused(false)
	{
		m_gameInstance = std::weak_ptr<GameInstance>();
	}

	EngineApp::~EngineApp()
	{
		m_window.reset();
	}

	void EngineApp::Init()
	{
		m_gameIsPaused = false;

		SvCore::Debug::Logger::GetInstance().SetFile("debug.log");
        SvCore::Resources::ResourceManager::GetInstance().AddSearchPath("assets");

		//physics
		SvPhysics::PhysicsContext::GetInstance().Init();

		//scripts
		SvScripting::LuaContext::GetInstance().Init();

		//setup InspectorComponents
		InspectorItemManager::Init();

		//window
		m_window = std::make_unique<Core::EditorWindow>();
		CHECK(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");
		IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
		renderAPI.Init(true)
			.SetCapability(ERenderingCapability::DEPTH_TEST, true)
			.SetCullFace(ECullFace::BACK);

		renderAPI.SetViewport({ 0, 0 }, { 800, 600 });

		//engine
		m_editorEngine.Init();
		LoadAllResources();
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

			if (!m_gameInstance.expired() && !m_gameIsPaused)
			{
				Timer::GetInstance().Tick();
				UpdateScripts();
				UpdatePhysics();
			}

			m_editorEngine.RenderWorlds();

			m_window->RenderUI();
			m_window->EndRender();
		}
	}

	void EngineApp::LoadAllResources()
	{
		//TODO : make/use resource manager init
	}

	void EngineApp::TogglePlayPIE()
	{
		const auto currentSelection = RenderingContext::s_editorSelectedEntity.GetEntity().GetIndex();
		HierarchyPanel::ToggleSelectable(SvCore::ECS::NULL_ENTITY.GetIndex());

		if (!m_gameInstance.expired()) //game is running
		{
			m_editorEngine.DestroyGameInstance();
			m_window->GetUI().ForceSceneFocus();
		}
		else //game not running
		{
			m_gameInstance = m_editorEngine.CreatePIEGameInstance();
			m_gameInstance.lock()->Start();
			m_window->GetUI().ForceGameFocus();
			//this is tmp game
		}

		HierarchyPanel::ToggleSelectable(currentSelection);
	}

	void EngineApp::TogglePausePIE()
	{
		//TODO: toggle pause
	}

	void EngineApp::PressFramePIE()
	{
		//TODO: press frame
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


#include "SurvivantEditor/App/EngineApp.h"


#include "SurvivantApp/Inputs/InputManager.h"
#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Debug/Logger.h>
#include "SurvivantCore/Events/EventManager.h"
#include <SurvivantCore/Utility/FileSystem.h>

#include "SurvivantEditor/App/TempDefaultScene.h"

#include <memory>


namespace SvEditor::App
{
	EngineApp::EngineApp() :
		m_gameIsPaused(false),
		m_isRunning(false)
	{
		m_gameInstance = std::weak_ptr<GameInstance>();
	}

	void EngineApp::Init()
	{
		m_isRunning = true;
		m_gameIsPaused = false;

		SvCore::Debug::Logger::GetInstance().SetFile("debug.log");

		m_window = std::make_unique<UI::Core::EditorWindow>();

		ASSERT(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");
		SV_LOG("Current working directory: \"%s\"", GetWorkingDirectory().c_str());

		IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
		renderAPI.Init(true)
			.SetCapability(ERenderingCapability::DEPTH_TEST, true)
			.SetCullFace(ECullFace::BACK);

		renderAPI.SetViewport({ 0, 0 }, { 800, 600 });

		ToRemove::GetDefaultFrameBuffer();
		m_editorEngine.Init();

		LoadAllResources();
		SetupEditorInputs();
		
		auto defaultTexture = dynamic_cast<OpenGLTexture&>(ToRemove::GetDefaultFrameBuffer()).GetId();

		m_window->SetupUI(
			{
				defaultTexture,
				{
					[this]() { TogglePlayPIE(); },
					[this]() { TogglePausePIE(); },
					[this]() { PressFramePIE(); }
				}
			},
			{
				defaultTexture,
				dynamic_cast<OpenGLTexture&>(*ToRemove::g_idTexture).GetId(),
			});

	}

	void EngineApp::Run()
	{
		while (!m_window->ShouldClose())
		{
			m_editorEngine.Update();
			m_window->Update();

			SvApp::InputManager::GetInstance().Update();

			if (!m_gameInstance.expired() && !m_gameIsPaused)
			{
				//do viewport here 
				//m_gameInstance->m_worldContext->m_viewport;
				ToRemove::g_frameBuffer->Bind();
				m_gameInstance.lock()->Update();
				ToRemove::g_frameBuffer->Unbind();
			}

			m_window->RenderUI();
			m_window->EndRender();
		}
	}

	void EngineApp::LoadAllResources()
	{
		//TODO : make/use resource manager init
	}

	std::tuple<int, int> AddInputTranslate(char i)
	{
		return { i, 10 };
	}

	std::tuple<int, int> AddMouseTranslate(float i, float j)
	{
		return { (int)i, (int)j };
	}

	std::tuple<> SpaceTranslate(char /*c*/)
	{
		return { };
	}

	void EngineApp::SetupEditorInputs()
	{
		using namespace SvCore;
		using namespace SvApp;
		using namespace Events;
		using AddEvent = Events::Event<int, int>;
		class ToggleEvent : public Events::Event<> {};

		InputManager& im = InputManager::GetInstance();
		InputManager::GetInstance().InitWindow(m_window.get());
		auto input = m_window->GetInputs();
		im.SetInputBindings(input);

		auto& k = input->m_keyCallbacks;
		auto& m = input->m_mouseKeyCallbacks;

		EventManager& em = EventManager::GetInstance();
		AddEvent::EventDelegate printAdd = [](int i, int j) { std::cout << "Add = " << i + j << std::endl; };
		//ToggleEvent::EventDelegate toggle = std::bind(&SvApp::Window::ToggleFullScreenMode, &window);
		em.AddListenner<AddEvent>(printAdd);
		//em.AddListenner<ToggleEvent>(toggle);

		//cant put MODS bsc of imgui
		InputManager::KeyboardKeyType   a(EKey::A, EKeyState::RELEASED, EInputModifier());
		InputManager::KeyboardKeyType   b(EKey::B, EKeyState::PRESSED, EInputModifier());
		InputManager::MouseKeyType      mouse(EMouseButton::MOUSE_1, EMouseButtonState::PRESSED, EInputModifier());
		InputManager::KeyboardKeyType   space(EKey::SPACE, EKeyState::PRESSED, EInputModifier());

		auto test = [](char p_c) { return std::tuple<int, int>{ p_c, 10 }; };
		k.emplace(a, im.CreateInputEventBinding<AddEvent>(a, &AddInputTranslate));
		k.emplace(b, im.CreateInputEventBinding<AddEvent>(a, &AddInputTranslate));
		k.emplace(space, im.CreateInputEventBinding<AddEvent>(space, &AddInputTranslate));
		k.emplace(	InputManager::KeyboardKeyType{ EKey::ESCAPE, EKeyState::PRESSED, EInputModifier() },
					[this](char) { m_isRunning = false; });

		m.emplace(mouse, im.CreateInputEventBinding<AddEvent>(mouse, &AddMouseTranslate));
		//stop running on escape
	}

	void EngineApp::TogglePlayPIE()
	{
		if (!m_gameInstance.expired()) //game is running
		{
			m_onStopInEditor.Invoke(*m_gameInstance.lock());
			m_editorEngine.DestroyGameInstance();
		}
		else //game not running
		{
			m_gameInstance = m_editorEngine.CreatePIEGameInstance();
			m_gameInstance.lock()->Start();
			//this is tmp game
			ToRemove::SetupGameInputs();
		}
	}

	void EngineApp::TogglePausePIE()
	{
		//TODO: toggle pause
	}

	void EngineApp::PressFramePIE()
	{
		//TODO: press frame
	}

	void EngineApp::PauseGameInstance()
	{

	}
}

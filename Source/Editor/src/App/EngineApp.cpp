#include "SurvivantEditor/App/EngineApp.h"


#include "SurvivantApp/Inputs/InputManager.h"
#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Debug/Logger.h>
#include "SurvivantCore/Events/EventManager.h"
#include <SurvivantCore/Utility/FileSystem.h>



using namespace LibMath;
using namespace SvCore::Utility;
using namespace SvRendering::Core;
using namespace SvRendering::Enums;
using namespace SvRendering::RHI;

#pragma region ResourcesHeaderToRemove

	#include <SurvivantRendering/Core/Camera.h>
	#include <SurvivantRendering/Core/Color.h>
	#include <SurvivantRendering/Core/Light.h>
	#include <SurvivantRendering/Resources/Material.h>
	#include <SurvivantRendering/Resources/Model.h>
	#include <SurvivantRendering/RHI/IFrameBuffer.h>
	#include <SurvivantRendering/RHI/IShader.h>
	#include <SurvivantRendering/RHI/IShaderStorageBuffer.h>
	#include <SurvivantRendering/RHI/ITexture.h>
	#include <SurvivantRendering/RHI/IUniformBuffer.h>
	#include <SurvivantRendering/RHI/OpenGL/OpenGLTexture.h>

	constexpr const char* UNLIT_SHADER_PATH = "assets/shaders/Unlit.glsl";
	constexpr const char* LIT_SHADER_PATH = "assets/shaders/Lit.glsl";
	namespace ToRemove
	{
		std::shared_ptr<ITexture> GetTextureEngine()
		{
			static std::shared_ptr<ITexture> texture = ITexture::Create();
			static bool                      isLoaded = false;

			if (!isLoaded)
			{
				ASSERT(texture->Load("assets/textures/grid.png"));
				ASSERT(texture->Init());

				texture->SetFilters(ETextureFilter::NEAREST, ETextureFilter::NEAREST);
				texture->SetWrapModes(ETextureWrapMode::REPEAT, ETextureWrapMode::REPEAT);

				isLoaded = true;
			}

			return texture;
		}

		std::unique_ptr<IFrameBuffer> g_frameBuffer;

		ITexture& GetDefaultFrameBuffer()
		{
			static std::shared_ptr<ITexture> color = ITexture::Create(800, 600, EPixelDataFormat::RGB);
			static std::shared_ptr<ITexture> depth = ITexture::Create(800, 600, EPixelDataFormat::DEPTH_COMPONENT);

			static bool isInitialized = false;

			if (isInitialized)
				return *color;

			color->Bind(0);

			g_frameBuffer = IFrameBuffer::Create();
			g_frameBuffer->Attach(*color, EFrameBufferAttachment::COLOR);
			g_frameBuffer->Attach(*depth, EFrameBufferAttachment::DEPTH);

			isInitialized = true;
			return *color;
		}
	}
#pragma endregion

using namespace ToRemove;

namespace SvEditor::App
{
	EngineApp::EngineApp() :
		m_gameIsPaused(false),
		m_isRunning(false)
	{
		m_gameInstance = nullptr;
		m_editorEngine.Init();
	}

	void EngineApp::Init()
	{
		m_isRunning = true;
		m_gameIsPaused = false;

		SvCore::Debug::Logger::GetInstance().SetFile("debug.log");

		m_window = std::make_unique<UI::Core::EditorWindow>(0);

		ASSERT(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");
		SV_LOG("Current working directory: \"%s\"", GetWorkingDirectory().c_str());

		IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
		renderAPI.Init(true)
			.SetCapability(ERenderingCapability::DEPTH_TEST, true)
			.SetCullFace(ECullFace::BACK);


		LoadAllResources();
		SetupEditorInputs();
		
		m_window->SetupUI(dynamic_cast<OpenGLTexture&>(GetDefaultFrameBuffer()).GetId());
	}

	void EngineApp::Run()
	{
		while (!m_window->ShouldClose())
		{
			m_time.tick();
			m_window->Update();

			if (m_gameInstance != nullptr && !m_gameIsPaused)
			{
				m_gameInstance->Update();
			}

			m_window->RenderUI();
			m_window->EndRender();
		}
	}

	void EngineApp::LoadAllResources()
	{
		//TODO : make/use resource manager init


		using namespace SvRendering;
		using namespace Resources;

		Model model;

		ASSERT(model.Load("assets/models/cube.obj"), "Failed to load model");
		ASSERT(model.Init(), "Failed to initialize model");

		std::shared_ptr<IShader> unlitShader = IShader::Create();
		ASSERT(unlitShader->Load(UNLIT_SHADER_PATH), "Failed to load shader at path \"%s\"", UNLIT_SHADER_PATH);
		ASSERT(unlitShader->Init(), "Failed to initialize shader at path \"%s\"", UNLIT_SHADER_PATH);

		std::shared_ptr<IShader> litShader = IShader::Create();
		ASSERT(litShader->Load(LIT_SHADER_PATH), "Failed to load shader at path \"%s\"", LIT_SHADER_PATH);
		ASSERT(litShader->Init(), "Failed to initialize shader at path \"%s\"", LIT_SHADER_PATH);

		Material whiteMaterial(unlitShader);
		whiteMaterial.GetProperty<std::shared_ptr<ITexture>>("u_diffuse") = GetTextureEngine();
		whiteMaterial.GetProperty<Vector4>("u_tint") = Color::white;

		Material redMaterial(whiteMaterial);
		redMaterial.GetProperty<Vector4>("u_tint") = Color::red;

		Material litMaterial(litShader);
		litMaterial.GetProperty<std::shared_ptr<ITexture>>("u_diffuse") = GetTextureEngine();
		litMaterial.GetProperty<Vector4>("u_tint") = Color::white;
		litMaterial.GetProperty<Vector4>("u_specularColor") = Color(.2f, .2f, .2f);
		litMaterial.GetProperty<float>("u_shininess") = 32.f;

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
		im.AddInputEventBinding<AddEvent>(a, &AddInputTranslate);
		im.AddInputEventBinding<AddEvent>(b, &AddInputTranslate);
		im.AddInputEventBinding<AddEvent>(mouse, &AddMouseTranslate);
		im.AddInputEventBinding<ToggleEvent>(space, &SpaceTranslate);

		//stop running on escape
		im.AddInputBinding({ EKey::ESCAPE, EKeyState::PRESSED, EInputModifier() }, [this](char) { m_isRunning = false; });
	}

	IRenderAPI& EngineApp::SetupRenderAPI()
	{
		IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
		renderAPI.Init(true)
			.SetCapability(ERenderingCapability::DEPTH_TEST, true)
			.SetCullFace(ECullFace::BACK);

		return renderAPI;
	}

	void EngineApp::TogglePIE()
	{
		if (m_gameInstance != nullptr) //game is running
		{
			m_onStopInEditor.Invoke(*m_gameInstance);
			m_editorEngine.DestroyGameInstance();
		}
		else //game not running
		{
			m_gameInstance = m_editorEngine.CreatePIEGameInstance();
		}
	}

	void EngineApp::PauseGameInstance()
	{

	}
}


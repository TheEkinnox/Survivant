#include "SurvivantEditor/App/EngineApp.h"


#include "SurvivantApp/Inputs/InputManager.h"
#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Debug/Logger.h>
#include "SurvivantCore/Events/EventManager.h"
#include <SurvivantCore/Utility/FileSystem.h>

#include <memory>

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

#pragma endregion

#pragma region TestGame

#include "Transform.h"

		void SetupTestGame()
		{
			using namespace SvCore;
			using namespace SvApp;
			using namespace Events;

			static bool firstTime = true;

			if (!firstTime)
				return;

			firstTime = false;

			InputManager& im = InputManager::GetInstance();

			using namespace SvEditor::App;

			im.AddInputBinding({ EKey::W, EKeyState::PRESSED, EInputModifier() }, [](const char)
				{
					++SvEditor::App::EngineApp::s_moveInput->m_y;
				});

			im.AddInputBinding({ EKey::W, EKeyState::RELEASED, EInputModifier() }, [](const char)
				{
					--EngineApp::s_moveInput->m_y;
				});

			im.AddInputBinding({ EKey::S, EKeyState::PRESSED, EInputModifier() }, [](const char)
				{
					--EngineApp::s_moveInput->m_y;
				});

			im.AddInputBinding({ EKey::S, EKeyState::RELEASED, EInputModifier() }, [](const char)
				{
					++EngineApp::s_moveInput->m_y;
				});

			im.AddInputBinding({ EKey::A, EKeyState::PRESSED, EInputModifier() }, [](const char)
				{
					--EngineApp::s_moveInput->m_x;
				});

			im.AddInputBinding({ EKey::A, EKeyState::RELEASED, EInputModifier() }, [](const char)
				{
					++EngineApp::s_moveInput->m_x;
				});

			im.AddInputBinding({ EKey::D, EKeyState::PRESSED, EInputModifier() }, [](const char)
				{
					++EngineApp::s_moveInput->m_x;
				});

			im.AddInputBinding({ EKey::D, EKeyState::RELEASED, EInputModifier() }, [](const char)
				{
					--EngineApp::s_moveInput->m_x;
				});

			im.AddInputBinding({ EKey::UP, EKeyState::PRESSED, EInputModifier() }, [](const char)
				{
					++EngineApp::s_rotateInput->m_y;
				});

			im.AddInputBinding({ EKey::UP, EKeyState::RELEASED, EInputModifier() }, [](const char)
				{
					--EngineApp::s_rotateInput->m_y;
				});

			im.AddInputBinding({ EKey::DOWN, EKeyState::PRESSED, EInputModifier() }, [](const char)
				{
					--EngineApp::s_rotateInput->m_y;
				});

			im.AddInputBinding({ EKey::DOWN, EKeyState::RELEASED, EInputModifier() }, [](const char)
				{
					++EngineApp::s_rotateInput->m_y;
				});

			im.AddInputBinding({ EKey::LEFT, EKeyState::PRESSED, EInputModifier() }, [](const char)
				{
					--EngineApp::s_rotateInput->m_x;
				});

			im.AddInputBinding({ EKey::LEFT, EKeyState::RELEASED, EInputModifier() }, [](const char)
				{
					++EngineApp::s_rotateInput->m_x;
				});

			im.AddInputBinding({ EKey::RIGHT, EKeyState::PRESSED, EInputModifier() }, [](const char)
				{
					++EngineApp::s_rotateInput->m_x;
				});

			im.AddInputBinding({ EKey::RIGHT, EKeyState::RELEASED, EInputModifier() }, [](const char)
				{
					--EngineApp::s_rotateInput->m_x;
				});

			im.AddInputBinding({ EKey::R, EKeyState::RELEASED, {} }, [](const char)
				{
					EngineApp::s_camTransform->setAll(*EngineApp::s_camPos, Quaternion::identity(), Vector3::one());
				});

			//im.AddInputBinding({ EKey::ESCAPE, EKeyState::RELEASED, {} }, [&window](const char)
			//{
			//    glfwSetWindowShouldClose(window.GetWindow(), true);
			//});

			std::vector<Matrix4> lightMatrices;
			lightMatrices.emplace_back(Light(EngineApp::s_cam->GetClearColor()).getMatrix());
			lightMatrices.emplace_back(DirectionalLight(Color::magenta, Vector3::back()).getMatrix());
			lightMatrices.emplace_back(SpotLight(Color(0.f, 1.f, 0.f, 3.f), *EngineApp::s_camPos, Vector3::front(), Attenuation(10),
				{ cos(0_deg), cos(30_deg) }).getMatrix());
			lightMatrices.emplace_back(PointLight(Light{ Color::red }, Vector3{ -1, 1, 1 }, Attenuation(16)).getMatrix());

			std::unique_ptr<IShaderStorageBuffer> lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, 0);
			lightsSSBO->Bind();
			lightsSSBO->SetData(lightMatrices.data(), lightMatrices.size());

			SvEditor::App::EngineApp::s_model = std::make_shared<SvRendering::Resources::Model>();
			ASSERT(SvEditor::App::EngineApp::s_model->Load("assets/models/cube.obj"), "Failed to load model");
			ASSERT(SvEditor::App::EngineApp::s_model->Init(), "Failed to initialize model");
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
		m_gameInstance = std::weak_ptr<GameInstance>();
		m_editorEngine.Init();
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


		LoadAllResources();
		SetupEditorInputs();
		
		m_window->SetupUI(
			{
			dynamic_cast<OpenGLTexture&>(GetDefaultFrameBuffer()).GetId(),
			{
				[this]() { TogglePlayPIE(); },
				[this]() { TogglePausePIE(); },
				[this]() { PressFramePIE(); }
			}
			});
	}

	void EngineApp::Run()
	{
		while (!m_window->ShouldClose())
		{
			m_editorEngine.Update();
			m_window->Update();

			if (!m_gameInstance.expired() && !m_gameIsPaused)
			{
				//do viewport here 
				//m_gameInstance->m_worldContext->m_viewport;
				g_frameBuffer->Bind();
				m_gameInstance.lock()->Update();
				g_frameBuffer->Unbind();
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
			m_gameInstance.lock()->m_worldContext->m_currentLevel->BeginPlay();
			//this is tmp game
			SetupTestGame();
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


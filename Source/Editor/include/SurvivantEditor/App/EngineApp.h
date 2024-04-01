//EngineApp.h
#pragma once

#include "SurvivantApp/Core/IApp.h"

#include "SurvivantEditor/UI/Core/EditorWindow.h"
#include "SurvivantEditor/App/GameInstance.h"
#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/App/EditorEngine.h"

#include <SurvivantRendering/RHI/IRenderAPI.h>

#include "Vector/Vector2.h"
#include "Vector/Vector3.h"
#include "Transform.h"
#include "SurvivantRendering/Core/Camera.h"
#include "SurvivantRendering/Resources/Model.h"

using namespace LibMath;

namespace SvEditor::App
{
	class EngineApp : public SvApp::Core::IApp
	{
	public:
		//events
		class OnPIE : public SvCore::Events::Event<GameInstance&> {};
		OnPIE m_onPlayInEditor;
		OnPIE m_onStopInEditor;

		EngineApp();
		~EngineApp() override = default;

		// Inherited via IApp
		void Init() override;
		void Run() override;

		//TODO: remove test default level, move to level
		static inline std::shared_ptr<Vector2>	s_moveInput = std::make_shared<Vector2>(), s_rotateInput = std::make_shared < Vector2>();
		static inline std::shared_ptr<Vector3>	s_camPos = std::make_shared<Vector3>(0.f, 1.8f, 2.f);
		static inline std::shared_ptr<Transform>	s_camTransform = std::make_shared<Transform>(*EngineApp::s_camPos, Quaternion::identity(), Vector3::one());
		static inline std::shared_ptr<SvRendering::Core::Camera> s_cam = std::make_shared<SvRendering::Core::Camera>(perspectiveProjection(90_deg, 4.f / 3.f, .01f, 14.f));
		static inline std::shared_ptr<SvRendering::Resources::Model> s_model;


	private:
		void LoadAllResources();
		void SetupEditorInputs();

		void TogglePlayPIE();
		void TogglePausePIE();
		void PressFramePIE();

		void PauseGameInstance();

		

		//class GameLoopEvent : public SvCore::Events::Event<> {};

		std::unique_ptr<UI::Core::EditorWindow>		m_window;

		bool					m_isRunning;
		bool					m_gameIsPaused;

		std::weak_ptr<GameInstance>		m_gameInstance;
		EditorEngine					m_editorEngine;
	};
}
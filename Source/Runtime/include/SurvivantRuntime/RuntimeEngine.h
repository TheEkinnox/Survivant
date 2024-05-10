//RuntimeEngine.h
#pragma once

#include <SurvivantApp/Core/IEngine.h>
#include <SurvivantApp/Inputs/InputManager.h>
#include <SurvivantApp/Core/GameInstance.h>
#include <SurvivantCore/Events/Event.h>
#include <SurvivantCore/ECS/EntityHandle.h>
#include <SurvivantCore/Utility/Timer.h>

#include <Transform.h>

#include <array>
#include <memory>



using namespace SvApp::Core;
using namespace LibMath;

namespace SvRuntime
{
	class EditorWindow;

	class RuntimeEngine : public SvApp::Core::IEngine
	{
	public:
		RuntimeEngine() = default;
		~RuntimeEngine() = default;

		// Inherited via IEngine
		void	Update() override;
		void	Init() override;
		void	BakeLights() override;
		bool	ChangeScene(const std::string& p_scenePath) override;
		float	GetDeltaTime() override;
		bool	IsPlayInEditor()override;

		bool	IsRunning();
		
		//create PIE after press play
		void	DestroyGameInstance();
		void	UpdateGame();
		void	Render();
		
	private:
		bool InitializeGameInstance();

		using GameInstancePtr = std::unique_ptr<SvApp::Core::GameInstance>;
		using WorldContextPtr = std::shared_ptr<SvApp::Core::WorldContext>;

		WorldContextPtr				CreateGameWorld();
		GameInstancePtr				CreateGameInstance();
		WorldContext::SceneRef		GetStartScene();

		LibMath::TVector2<int>		GetViewportSize();
		void						RedrawViewports(int p_width, int p_height);

		SvCore::Utility::Timer		m_time;
		bool						m_isRunning = true;
		WorldContextPtr				m_world;
		GameInstancePtr				m_game;

		SvCore::ECS::EntityHandle		m_camera;
	};
}
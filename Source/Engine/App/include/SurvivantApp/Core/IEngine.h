//IEngine.h
#pragma once

#include "SurvivantApp/Core/WorldContext.h"
#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantRendering/Core/Camera.h"

#include <string>
#include <memory>
#include <vector>

#define SV_DELTA_TIME() SvApp::Core::IEngine::s_engine->GetDeltaTime()
#define SV_ENGINE() SvApp::Core::IEngine::s_engine

namespace SvApp::Core
{
	using Scene = SvCore::ECS::Scene;
	using Camera = SvRendering::Core::Camera;

	class IEngine 
	{
	public:
		//TODO: add IEngine events

		IEngine() = default;
		~IEngine() = default;

		virtual void	Init() = 0;
		virtual void	Update() = 0;
		//virtual bool	StartScene(WorldContext& p_worldContext) = 0;
		virtual bool	ChangeScene(const std::string& p_scenePath) = 0;
		virtual void	RedrawViewports() = 0;
		virtual float	GetDeltaTime() = 0;
		virtual void	BakeLights() = 0;
		virtual bool	IsPlayInEditor() = 0;


		static std::shared_ptr<WorldContext>	CreateNewWorldContext(WorldContext::EWorldType p_worldType);

		static inline IEngine* s_engine = nullptr;

	protected:
		bool PrepareSceneChange(WorldContext& p_context, WorldContext::SceneRef& p_newLevel, const std::string& p_path);
		bool CommitSceneChange(WorldContext& p_context, const WorldContext::SceneRef& p_newLevel);

		//acces GameInstace members
		std::weak_ptr<WorldContext>&	GetWorldContextRef(GameInstance& p_instance);
	};

	
}
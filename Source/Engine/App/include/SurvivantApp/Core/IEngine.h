//IEngine.h
#pragma once

#include "SurvivantApp/Core/WorldContext.h"
#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantRendering/Core/Camera.h"
#include "SurvivantCore/ECS/EntityHandle.h"

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
		IEngine() = default;
		~IEngine() = default;

		virtual void	Init() = 0;
		virtual void	Update() = 0;
		//virtual bool	StartScene(WorldContext& p_worldContext) = 0;
		virtual bool	ChangeScene(const std::string& p_scenePath) = 0;
		virtual bool	ChangeCamera(const SvCore::ECS::EntityHandle& p_camera) = 0;
		virtual float	GetDeltaTime() = 0;
		virtual void	BakeLights() = 0;
		virtual bool	IsPlayInEditor() = 0;


		static std::shared_ptr<WorldContext>	CreateNewWorldContext(WorldContext::EWorldType p_worldType);

		static inline IEngine* s_engine = nullptr;

	protected:
		static inline std::string DEFAULT_SCENE_PATH = "assets/scenes/test.scn";

		/// <summary>
		/// Changes current level of given world
		/// </summary>
		/// <param name="p_worldContext">Current world</param>
		/// <param name="p_scene">Scene to go to</param>
		/// <returns>true if succesfull, false if not </returns>
		bool			BrowseToScene(WorldContext& p_worldContext, const std::string& p_path);
		/// <returns>-1 if couldnt, 0 if already there, 1 if properly </returns>
		bool			BrowseToDefaultScene(WorldContext& p_worldContext);

		//acces GameInstace members
		std::weak_ptr<WorldContext>&	GetWorldContextRef(GameInstance& p_instance);

	private:
		bool PrepareSceneChange(WorldContext& p_context, WorldContext::SceneRef& p_newLevel, const std::string& p_path);
		bool CommitSceneChange(WorldContext& p_context, const WorldContext::SceneRef& p_newLevel);
	};

	
}
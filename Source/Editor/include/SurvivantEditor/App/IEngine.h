//Engine.h
#pragma once

#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantEditor/App/WorldContext.h"
#include "SurvivantRendering/Core/Camera.h"

#include <string>
#include <memory>
#include <vector>

namespace SvEditor::App
{
	using Scene = SvCore::ECS::Scene;
	using Camera = SvRendering::Core::Camera;

	class Engine 
	{
	public:
		//TODO: add Engine events

		using LevelMap = std::unordered_map<std::string, std::shared_ptr<Scene>>;

		Engine() = default;
		~Engine() = default;

		virtual void	Init() = 0;
		virtual void	Update() = 0;
		//virtual bool	StartScene(WorldContext& p_worldContext) = 0;
		virtual bool	ChangeScene(const std::string& p_sceneName) = 0;
		virtual void	RedrawViewports() = 0;
		virtual float	GetDeltaTime() = 0;


		std::shared_ptr<WorldContext>	CreateNewWorldContext(WorldContext::EWorldType p_worldType);
		Scene&							GetCurrentScene();

		static inline Engine* g_engine = nullptr;

	protected:
		bool PrepareSceneChange(WorldContext& p_context, const std::shared_ptr<Scene>& p_newLevel);
		bool CommitSceneChange(WorldContext& m_context, const std::shared_ptr<Scene>& p_newLevel);

		//acces GameInstace members
		std::weak_ptr<WorldContext>& GetWorldContextRef(GameInstance& p_instance);

		std::shared_ptr<Scene>				m_currentScene;
		LevelMap							m_allLevels;
	};

	
}
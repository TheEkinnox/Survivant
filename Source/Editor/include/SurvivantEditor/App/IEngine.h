//Engine.h
#pragma once

//#include "SurvivantEditor/App/GameInstance.h"

#include <string>
#include <memory>
#include <vector>

namespace SvEditor::App
{
	//foward declaration
	class GameInstance;

	enum class EWorldType
	{
		NONE,
		PIE,
		EDITOR,
		GAME
	};

	//TODO: put actual level

	struct Level
	{
		void InitLevel() { m_bIsInitialized = true; }

		//destroy garbage?
		void DeInitLevel() { m_bIsInitialized = false; }
		void LoadResources() {}
		void BeginPlay() {}


		static std::shared_ptr<Level> CreateLevel() { return std::make_shared<Level>(); }

		std::string				m_name = "";
		//EWorldType	m_worldType = EWorldType::NONE;
		std::shared_ptr<Level>	m_currentScene = nullptr;
		GameInstance*			m_gameInstance = nullptr;

		bool					m_bIsInitialized = false;
	};

	class Engine 
	{
	public:
		//TODO: add Engine events

		struct WorldContext
		{
			EWorldType	m_worldType = EWorldType::NONE;
			int			m_viewport;

			std::vector<std::shared_ptr<Level>> m_levels;
			std::shared_ptr<Level>				m_currentLevel = nullptr;
			GameInstance*						m_owningGameInstance = nullptr;
			std::shared_ptr<Level>				m_persistentLevel = nullptr;
			//size_t								m_PIEInstance;

			std::shared_ptr<Level>* GetCurrentLevelPtr()
			{
				return &m_currentLevel;
			}
		};

		Engine() = default;
		~Engine() = default;

		virtual void	Init() = 0;
		virtual void	Update() = 0;
		virtual bool	LoadNewLevel(WorldContext& p_worldContext, std::shared_ptr<Level> p_level) = 0;
		virtual void	RedrawViewports() = 0;


		std::shared_ptr<Engine::WorldContext>	CreateNewWorldContext(EWorldType p_worldType);
		Level&									GetCurrentLevel();
		/// <summary>
		/// 
		/// </summary>
		/// <param name="p_worldContext"></param>
		/// <returns>-1 if couldnt, 0 if already there, 1 if properly </returns>
		int			BrowseToDefaultScene(WorldContext& p_worldContext);
		/// <returns>-1 if couldnt, 0 if already there, 1 if properly </returns>
		int			BrowseToScene(WorldContext& p_worldContext, const std::string& p_sceneName);


		static inline std::shared_ptr<Engine> g_engine = nullptr;

	protected:
		bool PrepareSceneChange(WorldContext& p_context, const std::string& p_sceneName);
		bool CommitSceneChange(WorldContext& m_context);

		//acces GameInstace members
		std::shared_ptr<Engine::WorldContext>& GetWorldContextRef(GameInstance& p_instance);

		std::shared_ptr<Level>					m_currentLevel;
		std::vector<std::shared_ptr<Level>>		m_allLevels;
	};

	
}
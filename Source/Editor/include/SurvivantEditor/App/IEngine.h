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
		//goes in constructor
		void InitLevel(const std::string& /*p_path*/) {}

		void LoadLevel() { m_bIsLoaded = true; }
		void UnloadLevel() { m_bIsLoaded = false; }
		void LoadResources() {}
		void BeginPlay();

		void UpdateLevel();
		void RenderLevel() {}


		static std::shared_ptr<Level> CreateLevel() { return std::make_shared<Level>(); }

		std::string				m_name = "";
		//EWorldType	m_worldType = EWorldType::NONE;
		std::shared_ptr<Level>	m_currentScene = nullptr;
		GameInstance*			m_gameInstance = nullptr;

		bool					m_bIsLoaded = false;
	};

	class Engine 
	{
	public:
		//TODO: add Engine events

		struct WorldContext
		{
			EWorldType	m_worldType = EWorldType::NONE;
			int			m_viewport = 0;

			std::shared_ptr<Level>				m_currentLevel = nullptr;
			GameInstance*						m_owningGameInstance = nullptr;
			//TODO: deal with persistentLevel
			//std::shared_ptr<Level>				m_persistentLevel = nullptr;

			std::shared_ptr<Level>* GetCurrentLevelPtr()
			{
				return &m_currentLevel;
			}
		};

		Engine() = default;
		~Engine() = default;

		virtual void	Init() = 0;
		virtual void	Update() = 0;
		virtual bool	StartLevel(WorldContext& p_worldContext) = 0;
		virtual void	RedrawViewports() = 0;
		virtual float	GetDeltaTime() = 0;


		std::shared_ptr<Engine::WorldContext>	CreateNewWorldContext(EWorldType p_worldType);
		Level&									GetCurrentLevel();
		/// <summary>
		/// 
		/// </summary>
		/// <param name="p_worldContext"></param>
		/// <returns>-1 if couldnt, 0 if already there, 1 if properly </returns>
		int			BrowseToDefaultLevel(WorldContext& p_worldContext);
		/// <returns>-1 if couldnt, 0 if already there, 1 if properly </returns>
		int			BrowseToLevel(WorldContext& p_worldContext, const std::string& p_levelName);


		static inline Engine* g_engine = nullptr;

	protected:
		bool PrepareLevelChange(WorldContext& p_context, const std::shared_ptr<Level>& p_newLevel);
		bool CommitLevelChange(WorldContext& m_context, const std::shared_ptr<Level>& p_newLevel);

		//acces GameInstace members
		std::shared_ptr<Engine::WorldContext>& GetWorldContextRef(GameInstance& p_instance);

		std::shared_ptr<Level>					m_currentLevel;
		std::vector<std::shared_ptr<Level>>		m_allLevels;
	};

	
}
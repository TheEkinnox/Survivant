//EngineApp.h
#pragma once

#include "SurvivantCore/Debug/Assertion.h"

#include "SurvivantEditor/App/GameInstance.h"
#include "SurvivantEditor/App/IEngine.h"

namespace SvEditor::App
{
    Level& Engine::GetCurrentLevel()
    {
        ASSERT(m_currentLevel != nullptr, "There is no current world");

        return *m_currentLevel;
    }

    int Engine::BrowseToDefaultScene(WorldContext& p_worldContext)
    {
        ASSERT(!p_worldContext.m_levels.empty());
        return BrowseToScene(p_worldContext, p_worldContext.m_levels[0]->m_name);
    }

    int Engine::BrowseToScene(WorldContext& p_worldContext, const std::string& p_sceneName)
    {
        ASSERT(!p_worldContext.m_levels.empty());
        ASSERT(p_worldContext.m_currentLevel != nullptr);

        std::shared_ptr<Level> destination;
        for (auto& scene : p_worldContext.m_levels)
        {
            if (scene->m_name == p_sceneName)
            {
                destination = scene;
                break;
            }
        }
        //not a valid scene name
        if (destination == nullptr)
            return -1;
        
        //alredy in scene
        if (destination == m_currentLevel->m_currentScene)
            return 0;

        if (!PrepareSceneChange(p_worldContext, p_sceneName) ||
            !CommitSceneChange(p_worldContext))
            return -1;

        return 1;
    }

    bool Engine::PrepareSceneChange(WorldContext& /*p_context*/, const std::string& /*p_sceneName*/)
    {
        //load destination scene, switch and the unload old scene

        return true;
    }

    bool Engine::CommitSceneChange(WorldContext& /*m_context*/)
    {
        //load destination scene, switch and the unload old scene

        return true;
    }

    std::shared_ptr<Engine::WorldContext>& Engine::GetWorldContextRef(GameInstance& p_instance)
    {
        return p_instance.m_worldContext;
    }

    std::shared_ptr<Engine::WorldContext> Engine::CreateNewWorldContext(EWorldType p_worldType)
    {
        //TODO: CreateNewWorldContext
        std::shared_ptr<WorldContext> wrdPtr = std::make_unique<WorldContext>();
        WorldContext& world = *wrdPtr;

        switch (p_worldType)
        {
        case SvEditor::App::EWorldType::NONE:
            break;
        case SvEditor::App::EWorldType::PIE:
            world.m_worldType = EWorldType::PIE;
            break;
        case SvEditor::App::EWorldType::EDITOR:
            world.m_worldType = EWorldType::EDITOR;
            break;
        case SvEditor::App::EWorldType::GAME:
            world.m_worldType = EWorldType::GAME;
            break;
        default:
            break;
        }

        return wrdPtr;
    }
}
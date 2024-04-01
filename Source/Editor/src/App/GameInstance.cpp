//EngineApp.h
#pragma once

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantEditor/App/GameInstance.h"
#include "SurvivantEditor/App/EditorEngine.h"

namespace SvEditor::App
{
    void GameInstance::Init()
    {
        //init here
        int i = 0; i;

    }

    void GameInstance::Update()
    {
        m_worldContext->m_currentLevel->UpdateLevel();
        m_worldContext->m_currentLevel->RenderLevel();
    }

    void GameInstance::InitializeStandalone()
    {
        // Creates the world context. This should be the only WorldContext that ever gets created for this GameInstance.
        m_worldContext = GetEngine()->CreateNewWorldContext(EWorldType::GAME);
        m_worldContext->m_owningGameInstance = this;

        // In standalone create a dummy world from the beginning to avoid issues of not having a world until LoadMap gets us our real world
        std::shared_ptr<Level> dummyLevel = Level::CreateLevel();
        dummyLevel->m_gameInstance = this;

        m_worldContext->m_currentLevel = dummyLevel;
    }

    Engine* GameInstance::GetEngine()
    {
        //an engine will exist before any GameInstances
        ASSERT(Engine::g_engine != nullptr, "Global engine not defined before GameInstance");
        return Engine::g_engine;
    }
}
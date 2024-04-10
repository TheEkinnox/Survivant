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
        UpdatePhysics();

        m_worldContext->Update();
        WorldContext::DefaultRender(*m_worldContext);

        //m_worldContext->Render();
    }

    void GameInstance::InitializeStandalone()
    {
        // Creates the world context. This should be the only WorldContext that ever gets created for this GameInstance.
        m_worldContext = GetEngine()->CreateNewWorldContext(WorldContext::EWorldType::GAME);
        m_worldContext->m_owningGameInstance = this;

        // In standalone create a dummy world from the beginning to avoid issues of not having a world until LoadMap gets us our real world
        std::shared_ptr<Scene> dummyLevel = std::make_shared<Scene>();
        //dummyLevel->m_gameInstance = this;

        *m_worldContext->GetCurrentLevelPtr() = dummyLevel;
    }

    void GameInstance::Start()
    {
        m_worldContext->BeginPlay();
    }

    Engine* GameInstance::GetEngine()
    {
        //an engine will exist before any GameInstances
        ASSERT(Engine::g_engine != nullptr, "Global engine not defined before GameInstance");
        return Engine::g_engine;
    }
}
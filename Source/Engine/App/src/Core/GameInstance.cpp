//GameInstance.cpp

#include "SurvivantApp/Core/GameInstance.h"

#include "SurvivantCore/Debug/Assertion.h"

namespace SvApp::Core
{
    void GameInstance::Init(const std::weak_ptr<WorldContext>& p_worldContext)
    {
        m_worldContext = p_worldContext;

    }

    void GameInstance::UpdateScripts()
    {


        //put all updates in gameInstance
        //m_worldContext.lock()->Update();
    }

    void GameInstance::UpdatePhysics()
    {
    }

    void GameInstance::InitializeStandalone()
    {
        // Creates the world context. This should be the only WorldContext that ever gets created for this GameInstance.
        //m_worldContext = GetEngine()->CreateNewWorldContext(WorldContext::EWorldType::GAME);
        //m_worldContext.lock()->m_owningGameInstance = this;

        //// In standalone create a dummy world from the beginning to avoid issues of not having a world until LoadMap gets us our real world
        //std::shared_ptr<Scene> dummyLevel = std::make_shared<Scene>();
        ////dummyLevel->m_gameInstance = this;

        //m_worldContext.lock()->CurrentScene() = dummyLevel;
    }

    void GameInstance::Start()
    {
        m_worldContext.lock()->BeginPlay();
    }

    IEngine* GameInstance::GetEngine()
    {
        //an engine will exist before any GameInstances
        ASSERT(IEngine::s_engine != nullptr, "Global engine not defined before GameInstance");
        return IEngine::s_engine;
    }
}
//EngineApp.cpp

#include "SurvivantCore/Debug/Assertion.h"

#include "SurvivantApp/Core/GameInstance.h"
#include "SurvivantApp/Core/IEngine.h"
#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantRendering/Components/CameraComponent.h"

#include "Transform.h"


namespace SvApp::Core
{
    Scene& Engine::GetGameScene()
    {
        ASSERT(!m_gameScene, "There is no current world");
        return *m_gameScene;
    }

    bool Engine::PrepareSceneChange(WorldContext& /*p_context*/, const WorldContext::SceneRef& p_newLevel)
    {
        //init destination scene
        return p_newLevel->Init();
    }

    bool Engine::CommitSceneChange(WorldContext& p_context, const WorldContext::SceneRef& p_newLevel)
    {
        //switch and the unload old scene
        auto& m_levelToUnload = p_context.CurrentScene();
        p_context.CurrentScene() = p_newLevel;
        p_context.BakeLighting();

        if (m_levelToUnload)
        {
            //m_levelToUnload->UnloadLevel();
        }

        return true;
    }

    std::weak_ptr<WorldContext>& Engine::GetWorldContextRef(GameInstance& p_instance)
    {
        return p_instance.m_worldContext;
    }

    std::shared_ptr<WorldContext> Engine::CreateNewWorldContext(WorldContext::EWorldType p_worldType)
    {
        using namespace LibMath;
        using namespace SvCore::ECS;
        using namespace SvRendering::Components;

        static CameraComponent EditorCameraComponent = CameraComponent();

        std::shared_ptr<WorldContext> wrdPtr = std::make_unique<WorldContext>();
        WorldContext& world = *wrdPtr;
        world.m_worldType = p_worldType;

        switch (p_worldType)
        {
        case WorldContext::EWorldType::NONE:
            break;
        case WorldContext::EWorldType::PIE:
            world.m_renderingContext = std::make_shared<RenderingContext>(EntityHandle());
            world.m_renderingContext->AddRenderPass(RenderingContext::ERenderType::GAME);
            break;
        case WorldContext::EWorldType::EDITOR:
            world.m_renderingContext = std::make_shared<RenderingContext>(CameraComponent(), Transform());
            world.m_renderingContext->AddRenderPass(RenderingContext::ERenderType::SCENE);
            world.m_renderingContext->AddRenderPass(RenderingContext::ERenderType::ID);
            break;
        case WorldContext::EWorldType::GAME:
            world.m_renderingContext = std::make_shared<RenderingContext>(EntityHandle());
            world.m_renderingContext->AddRenderPass(RenderingContext::ERenderType::GAME);
            break;
        default:
            break;
        }

        return wrdPtr;
    }
}
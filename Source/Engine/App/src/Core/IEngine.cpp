//RuntimeApp.cpp

#include "SurvivantCore/Debug/Assertion.h"

#include "SurvivantApp/Core/GameInstance.h"
#include "SurvivantApp/Core/IEngine.h"
#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantCore/Resources/ResourceManager.h"
#include "SurvivantRendering/Components/CameraComponent.h"

#include "Transform.h"


namespace SvApp::Core
{
    bool IEngine::PrepareSceneChange(WorldContext& p_context, WorldContext::SceneRef& p_newLevel, const std::string& p_path)
    {
        //init destination scene
        using namespace SvCore::Resources;

        //switch and the unload old scene
        if (p_context.CurrentScene())
            p_context.CurrentScene()->Clear();

        p_newLevel = ResourceManager::GetInstance().Load<Scene>(p_path);

        if (!p_newLevel)
            return false;

        return p_newLevel->Init();
    }

    bool IEngine::CommitSceneChange(WorldContext& p_context, const WorldContext::SceneRef& p_newLevel)
    {
        p_context.m_currentSceneRef.reset();
        p_context.m_currentSceneRef = std::make_shared<WorldContext::SceneRef>(p_newLevel);
        p_context.BakeLighting();

        return true;
    }

    std::weak_ptr<WorldContext>& IEngine::GetWorldContextRef(GameInstance& p_instance)
    {
        return p_instance.m_worldContext;
    }

    std::shared_ptr<WorldContext> IEngine::CreateNewWorldContext(WorldContext::EWorldType p_worldType)
    {
        using namespace LibMath;
        using namespace SvCore::ECS;
        using namespace SvRendering::Components;

        std::shared_ptr<WorldContext> wrdPtr = std::make_shared<WorldContext>();
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

    bool IEngine::BrowseToDefaultScene(WorldContext& p_worldContext)
    {
        return BrowseToScene(p_worldContext, DEFAULT_SCENE_PATH);
    }

    bool IEngine::BrowseToScene(WorldContext& p_worldContext, const std::string& p_path)
    {
        //update current scene
        WorldContext::SceneRef sceneRef;

        if (!(PrepareSceneChange(p_worldContext, sceneRef, p_path) &&
            CommitSceneChange(p_worldContext, sceneRef)))
            return false;

        return true;
    }
}
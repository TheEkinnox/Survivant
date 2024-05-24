//WorldContext.cpp

#include "SurvivantApp/Core/WorldContext.h"

using namespace SvCore::ECS;
using namespace SvRendering::Core;
using namespace SvRendering::Components;

namespace SvApp::Core
{
    void WorldContext::BeginPlay()
    {
    }

    bool WorldContext::Save(const bool p_pretty)
    {
        return CurrentScene().Export(p_pretty);
    }

    void WorldContext::BakeLighting()
    {
        Renderer::UpdateLightSSBO(CurrentScene().Get(), *m_lightsSSBO);
    }

    EntityHandle WorldContext::GetFirstCamera()
    {
        SceneView<CameraComponent> cameras(*CurrentScene());

        EntityHandle entity;
        if (cameras.begin() != cameras.end())
            entity = EntityHandle(CurrentScene().Get(), *cameras.begin());

        return entity;
    }

    void WorldContext::SetCamera(const CameraComponent& p_cam, const LibMath::Transform& p_trans)
    {
        m_renderingContext->m_mainCamera.SetCamera(p_cam, p_trans);
        m_renderingContext->ResetCameraAspect();
    }

    void WorldContext::SetCamera(const EntityHandle& p_camera)
    {
        m_renderingContext->m_mainCamera.SetEntity(p_camera);
        m_renderingContext->ResetCameraAspect();
    }

    void WorldContext::SetInputs()
    {
        InputManager::GetInstance().SetInputBindings(m_inputs);
    }

    WorldContext::SceneRef& WorldContext::CurrentScene()
    {
        return *m_currentSceneRef;
    }

    std::weak_ptr<WorldContext::SceneRef> WorldContext::CurrentSceneRef()
    {
        return m_currentSceneRef;
    }
}

//WorldContext.cpp

#include "SurvivantApp/Core/WorldContext.h"

#include "SurvivantApp/Core/IEngine.h"
#include "SurvivantApp/Core/TempDefaultScene.h"


namespace SvApp::Core
{
    void WorldContext::BeginPlay()
    {
        using namespace ToRemove;

        TestLevelBeginPlay(*m_currentScene);
    }

    void WorldContext::Update()
    {
        using namespace ToRemove;

        SceneView<Temporary>                  temporariesView(*m_currentScene);
        SceneView<const UserInput, Transform> userInputsView(*m_currentScene);
        SceneView<const Rotator, Transform>   rotatorsView(*m_currentScene);

        UpdateTemporaries(temporariesView, SV_DELTA_TIME());
        UpdateInput(userInputsView, ToRemove::GameInfo::moveInput, ToRemove::GameInfo::rotateInput, SV_DELTA_TIME());
        UpdateRotators(rotatorsView, SV_DELTA_TIME());
    }

    void WorldContext::Render()
    {
        m_renderingContext->Render(*m_currentScene);
    }

    void WorldContext::SetSceneCamera(const EntityHandle& p_entity)
    {
        m_renderingContext->m_mainCamera.SetEntity(p_entity);
    }

    SvCore::ECS::EntityHandle WorldContext::GetDefaultSceneCamera()
    {
        SceneView<Camera> cameras(*m_currentScene);

        ASSERT(cameras.begin() != cameras.end(), "No Cameras In World");
        return EntityHandle(m_currentScene.get(), *cameras.begin());
    }

    void WorldContext::SetOwningCamera(const SvRendering::Core::Camera& p_cam, const LibMath::Transform& p_trans)
    {
        m_renderingContext->m_mainCamera.SetCamera(p_cam, p_trans);
    }

    void WorldContext::SetInputs()
    {
        InputManager::GetInstance().SetInputBindings(m_inputs);
    }
}

//WorldContext.cpp

#include "SurvivantApp/Core/WorldContext.h"

#include "SurvivantApp/Core/IEngine.h"
#include "SurvivantApp/Core/TempDefaultScene.h"


namespace SvApp::Core
{
    void WorldContext::BeginPlay()
    {
        using namespace ToRemove;


        //TODO : Save resource ref in begin play
        //static int i = 0;
        //if (i == 0)
        //{
        //ToRemove::MakeScene(*CurrentScene());
        //CurrentScene()->Save("assets/scenes/DefaultScene.scn");
            //i++;
        //}
        //else
        //{
        //    CurrentScene()->Save("assets/scenes/TestScene1.scn");
        //}

        //CurrentScene()->Clear();
        //CurrentScene()->Save("assets/scenes/EmptyScene.scn");

        TestLevelBeginPlay(*CurrentScene());
    }

    //void WorldContext::Update()
    //{
    //    using namespace ToRemove;

    //    SceneView<Temporary>                  temporariesView(*CurrentScene());
    //    SceneView<const UserInput, Transform> userInputsView(*CurrentScene());
    //    SceneView<const Rotator, Transform>   rotatorsView(*CurrentScene());

    //    UpdateTemporaries(temporariesView, SV_DELTA_TIME());
    //    UpdateInput(userInputsView, ToRemove::GameInfo::moveInput, ToRemove::GameInfo::rotateInput, SV_DELTA_TIME());
    //    UpdateRotators(rotatorsView, SV_DELTA_TIME());
    //}

    void WorldContext::Save()
    {
        CurrentScene()->Save(CurrentScene().GetPath());
    }

    void WorldContext::BakeLighting()
    {
        Renderer::UpdateLightSSBO(CurrentScene().Get(), *m_lightsSSBO);
    }

    SvCore::ECS::EntityHandle WorldContext::GetFirstCamera()
    {
        SceneView<CameraComponent> cameras(*CurrentScene());

        EntityHandle entity;
        if (cameras.begin() != cameras.end())
            entity = EntityHandle(CurrentScene().Get(), *cameras.begin());

        return entity;
    }

    void WorldContext::SetCamera(const SvRendering::Components::CameraComponent& p_cam, const LibMath::Transform& p_trans)
    {
        m_renderingContext->m_mainCamera.SetCamera(p_cam, p_trans);
    }

    void WorldContext::SetCamera(const SvCore::ECS::EntityHandle& p_camera)
    {
        m_renderingContext->m_mainCamera.SetEntity(p_camera);
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

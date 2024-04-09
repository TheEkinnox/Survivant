//WorldContext.cpp

#include "SurvivantEditor/App/WorldContext.h"

#include "SurvivantEditor/App/TempDefaultScene.h"


namespace SvEditor::App
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
        using namespace ToRemove;


        //m_frameBuffer->Bind();

        IRenderAPI::GetCurrent().Clear(true, true, true);
        DrawScene(*m_currentScene);

        //m_frameBuffer->Unbind();
    }
}
//EngineApp.cpp

#include "SurvivantCore/Debug/Assertion.h"

#include "SurvivantEditor/App/GameInstance.h"
#include "SurvivantEditor/App/IEngine.h"



#pragma region TestLevelUpdate

namespace ToRemove
{
    //using namespace LibMath;
    //using namespace SvRendering::RHI;
    //using namespace SvRendering::Enums;


    //Material whiteMaterial;
    //Material redMaterial;
    //Material litMaterial;

    //constexpr const char* UNLIT_SHADER_PATH = "assets/shaders/Unlit.glsl";
    //constexpr const char* LIT_SHADER_PATH = "assets/shaders/Lit.glsl";

    //void Once()
    //{
    //    using namespace SvRendering::Core;
    //    using namespace SvCore::Debug;

    //    static bool firstTime = true;

    //    if (!firstTime)
    //        return;

    //    firstTime = false;


    //    std::vector<Matrix4> lightMatrices;
    //    SvEditor::App::EngineApp::s_cam->SetClearColor(Color::white);
    //    lightMatrices.emplace_back(Light(SvEditor::App::EngineApp::s_cam->GetClearColor()).getMatrix());
    //    lightMatrices.emplace_back(DirectionalLight(Color::magenta, Vector3::back()).getMatrix());
    //    lightMatrices.emplace_back(SpotLight(Color(0.f, 1.f, 0.f, 3.f), *SvEditor::App::EngineApp::s_camPos, Vector3::front(), Attenuation(10),
    //        { cos(0_deg), cos(30_deg) }).getMatrix());
    //    lightMatrices.emplace_back(PointLight(Light{ Color::red }, Vector3{ -1, 1, 1 }, Attenuation(16)).getMatrix());

    //    std::unique_ptr<IShaderStorageBuffer> lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, 0);
    //    lightsSSBO->Bind();
    //    lightsSSBO->SetData(lightMatrices.data(), lightMatrices.size());

    //    SvEditor::App::EngineApp::s_model = std::make_shared<SvRendering::Resources::Model>();
    //    ASSERT(SvEditor::App::EngineApp::s_model->Load("assets/models/cube.obj"), "Failed to load model");
    //    ASSERT(SvEditor::App::EngineApp::s_model->Init(), "Failed to initialize model");

    //    std::shared_ptr<IShader> unlitShader = IShader::Create();
    //    ASSERT(unlitShader->Load(UNLIT_SHADER_PATH), "Failed to load shader at path \"%s\"", UNLIT_SHADER_PATH);
    //    ASSERT(unlitShader->Init(), "Failed to initialize shader at path \"%s\"", UNLIT_SHADER_PATH);

    //    std::shared_ptr<IShader> litShader = IShader::Create();
    //    ASSERT(litShader->Load(LIT_SHADER_PATH), "Failed to load shader at path \"%s\"", LIT_SHADER_PATH);
    //    ASSERT(litShader->Init(), "Failed to initialize shader at path \"%s\"", LIT_SHADER_PATH);

    //    whiteMaterial.SetShader(unlitShader);

    //    whiteMaterial.GetProperty<std::shared_ptr<ITexture>>("u_diffuse") = GetTexture();
    //    whiteMaterial.GetProperty<Vector4>("u_tint") = Color::white;

    //    redMaterial = whiteMaterial;
    //    redMaterial.GetProperty<Vector4>("u_tint") = Color::red;

    //    litMaterial.SetShader(litShader);
    //    litMaterial.GetProperty<std::shared_ptr<ITexture>>("u_diffuse") = GetTexture();
    //    litMaterial.GetProperty<Vector4>("u_tint") = Color::white;
    //    litMaterial.GetProperty<Vector4>("u_specularColor") = Color(.2f, .2f, .2f);
    //    litMaterial.GetProperty<float>("u_shininess") = 32.f;
    //}

    //void TestLevelBeginPlay()
    //{
    //    *SvEditor::App::EngineApp::s_camPos = Vector3(0.f, 1.8f, 2.f);
    //    *SvEditor::App::EngineApp::s_camTransform = Transform(*SvEditor::App::EngineApp::s_camPos, Quaternion::identity(), Vector3::one());
    //}

    //void TestLevelUpdate()
    //{
    //    using namespace LibMath::Literal;
    //    using namespace SvRendering::Core;
    //    using namespace SvEditor::App;

    //    constexpr float  CAM_MOVE_SPEED = 3.f;
    //    constexpr Radian CAM_ROTATION_SPEED = 90_deg;

    //    static Degree angle = 0_deg;

    //    Once();

    //    angle += 20_deg * SvEditor::App::Engine::g_engine->GetDeltaTime();

    //    const Matrix4 modelRot = rotation(angle, Vector3::up());
    //    const Matrix4 modelMat1 = translation(-1.f, .5f, 0.f);
    //    const Matrix4 modelMat2 = translation(1.f, .5f, 0.f) * modelRot;

    //    Vector3    newPos = SvEditor::App::EngineApp::s_camTransform->getPosition();
    //    Quaternion newRot = SvEditor::App::EngineApp::s_camTransform->getRotation();

    //    //already binded
    //    //g_frameBuffer->Bind();
    //    //renderAPI.SetViewport({ 0, 0 }, { 800, 600 });

    //    if (EngineApp::GameInfo::moveInput->magnitudeSquared() > 0.f)
    //    {
    //        const Vector3 moveDir = EngineApp::GameInfo::moveInput->m_x * SvEditor::App::EngineApp::s_camTransform->worldRight() + EngineApp::GameInfo::moveInput->m_y * SvEditor::App::EngineApp::s_camTransform->worldBack();
    //        newPos += moveDir.normalized() * (CAM_MOVE_SPEED * SV_DELTA_TIME());
    //    }

    //    if (EngineApp::GameInfo::rotateInput->magnitudeSquared() > 0.f)
    //    {
    //        TVector3<Radian> angles = newRot.toEuler(ERotationOrder::YXZ);

    //        angles.m_y -= CAM_ROTATION_SPEED * EngineApp::GameInfo::rotateInput->m_x * SV_DELTA_TIME();
    //        angles.m_x += CAM_ROTATION_SPEED * EngineApp::GameInfo::rotateInput->m_y * SV_DELTA_TIME();
    //        angles.m_x = Degree(clamp(angles.m_x.degree(true), -85.f, 85.f));

    //        newRot = Quaternion::fromEuler(angles, ERotationOrder::YXZ);
    //    }

    //    SvEditor::App::EngineApp::s_camTransform->setAll(newPos, newRot, Vector3::one());

    //    EngineApp::s_cam->SetView(SvEditor::App::EngineApp::s_camTransform->getWorldMatrix().inverse());
    //    EngineApp::s_cam->SetClearColor(Color::gray);
    //    EngineApp::s_cam->Clear();

    //    BindCamUBO(EngineApp::s_cam->GetViewProjection(), SvEditor::App::EngineApp::s_camTransform->getWorldPosition());

    //    Frustum camFrustum = EngineApp::s_cam->GetFrustum();

    //    const Vector3 testPos = *EngineApp::s_camPos + Vector3::front();
    //    const Matrix4 testModelMat = translation(testPos) * scaling(1.5f, .5f, .1f);

    //    DrawModel(*SvEditor::App::EngineApp::s_model, camFrustum, modelMat1, whiteMaterial);
    //    DrawModel(*SvEditor::App::EngineApp::s_model, camFrustum, modelMat2, redMaterial);
    //    DrawModel(*SvEditor::App::EngineApp::s_model, camFrustum, testModelMat, litMaterial);

    //    //g_frameBuffer->Unbind();
    //    //renderAPI.SetViewport({ 0, 0 }, { 800, 600 });
    //}
}

#pragma endregion


namespace SvEditor::App
{
    Scene& Engine::GetCurrentScene()
    {
        ASSERT(m_currentScene != nullptr, "There is no current world");

        return *m_currentScene;
    }

    bool Engine::PrepareSceneChange(WorldContext& /*p_context*/, const std::shared_ptr<Scene>& p_newLevel)
    {
        //init destination scene
        return p_newLevel->Init();
    }

    bool Engine::CommitSceneChange(WorldContext& p_context, const std::shared_ptr<Scene>& p_newLevel)
    {
        //switch and the unload old scene
        std::shared_ptr<Scene> m_levelToUnload = p_context.m_currentScene;
        p_context.m_currentScene = p_newLevel;

        if (m_levelToUnload != nullptr)
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
        std::shared_ptr<WorldContext> wrdPtr = std::make_unique<WorldContext>();
        WorldContext& world = *wrdPtr;
        world.m_worldType = p_worldType;

        switch (p_worldType)
        {
        case WorldContext::EWorldType::NONE:
            break;
        case WorldContext::EWorldType::PIE:
            world.AddRenderPass(WorldContext::ERenderType::DEFAULT);
            break;
        case WorldContext::EWorldType::EDITOR:
            world.AddRenderPass(WorldContext::ERenderType::DEFAULT);
            world.AddRenderPass(WorldContext::ERenderType::ID);
            break;
        case WorldContext::EWorldType::GAME:
            world.AddRenderPass(WorldContext::ERenderType::DEFAULT);
            break;
        default:
            break;
        }

        return wrdPtr;
    }
}
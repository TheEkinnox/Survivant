//EngineApp.h
#pragma once

#include "SurvivantCore/Debug/Assertion.h"

#include "SurvivantEditor/App/GameInstance.h"
#include "SurvivantEditor/App/IEngine.h"


#pragma region TestLevelIncludesToTremove
#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Utility/FileSystem.h>
#include <SurvivantCore/Utility/Timer.h>

#include <SurvivantRendering/Core/Camera.h>
#include <SurvivantRendering/Core/Color.h>
#include <SurvivantRendering/Core/Light.h>
#include <SurvivantRendering/Resources/Material.h>
#include <SurvivantRendering/Resources/Model.h>
#include <SurvivantRendering/RHI/IFrameBuffer.h>
#include <SurvivantRendering/RHI/IRenderAPI.h>
#include <SurvivantRendering/RHI/IShader.h>
#include <SurvivantRendering/RHI/IShaderStorageBuffer.h>
#include <SurvivantRendering/RHI/ITexture.h>
#include <SurvivantRendering/RHI/IUniformBuffer.h>
#include <SurvivantRendering/RHI/OpenGL/OpenGLTexture.h>

#include "SurvivantEditor/App/EngineApp.h"
#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantApp/Inputs/KeyboardInputs.h"
#include "SurvivantApp/Inputs/MouseInputs.h"

#include "SurvivantEditor/UI/Core/EditorUI.h"
#include "SurvivantEditor/UI/Core/EditorWindow.h"

#include <Transform.h>
#include "SurvivantEditor/App/IEngine.h"


#pragma endregion



#pragma region TestLevelUpdate

namespace ToRemove
{
    using namespace LibMath;
    using namespace SvRendering::RHI;
    using namespace SvRendering::Enums;

    void BindCamUBO(const Matrix4& p_viewProj, const Vector3& p_viewPos)
    {

        static std::unique_ptr<IUniformBuffer> camBuffer = IUniformBuffer::Create(EAccessMode::DYNAMIC_DRAW, 0);

        struct CameraUBO
        {
            Matrix4 m_viewProjection;
            Vector3 m_viewPos;
        };

        CameraUBO ubo
        {
            p_viewProj.transposed(),
            p_viewPos
        };

        camBuffer->SetData(&ubo, 1);
        camBuffer->Bind();
    }

    void BindModelUBO(const Matrix4& p_modelMat)
    {
        static std::unique_ptr<IUniformBuffer> modelBuffer = IUniformBuffer::Create(EAccessMode::DYNAMIC_DRAW, 1);

        struct ModelUBO
        {
            Matrix4 m_modelMat;
            Matrix4 m_normalMat;
        };

        ModelUBO ubo
        {
            p_modelMat.transposed(),
            p_modelMat.inverse()
        };

        modelBuffer->SetData(&ubo, 1);
        modelBuffer->Bind();
    }

    using namespace SvRendering::Resources;
    using namespace SvRendering::Geometry;

    void DrawModel(const Model& p_model, const Frustum& p_viewFrustum, const Matrix4& p_transform, const Material& p_material)
    {
        if (!p_viewFrustum.Intersects(TransformBoundingBox(p_model.GetBoundingBox(), p_transform)))
            return;

        BindModelUBO(p_transform);

        p_material.Bind();
        p_material.GetShader().SetUniformMat4("sv_modelMat", p_transform);
        p_material.GetShader().SetUniformMat4("sv_normalMat", p_transform.transposed().inverse());

        for (size_t i = 0; i < p_model.GetMeshCount(); ++i)
        {
            const Mesh& mesh = p_model.GetMesh(i);

            mesh.Bind();
            IRenderAPI::GetCurrent().DrawElements(EPrimitiveType::TRIANGLES, mesh.GetIndexCount());
        }
    }

    std::shared_ptr<ITexture> GetTexture()
    {
        static std::shared_ptr<ITexture> texture = ITexture::Create();
        static bool                      isLoaded = false;

        if (!isLoaded)
        {
            ASSERT(texture->Load("assets/textures/grid.png"));
            ASSERT(texture->Init());

            texture->SetFilters(ETextureFilter::NEAREST, ETextureFilter::NEAREST);
            texture->SetWrapModes(ETextureWrapMode::REPEAT, ETextureWrapMode::REPEAT);

            isLoaded = true;
        }

        return texture;
    }

    Material whiteMaterial;
    Material redMaterial;
    Material litMaterial;

    constexpr const char* UNLIT_SHADER_PATH = "assets/shaders/Unlit.glsl";
    constexpr const char* LIT_SHADER_PATH = "assets/shaders/Lit.glsl";

    bool Once()
    {
        using namespace SvRendering::Core;
        using namespace SvCore::Debug;

        std::shared_ptr<IShader> unlitShader = IShader::Create();
        ASSERT(unlitShader->Load(UNLIT_SHADER_PATH), "Failed to load shader at path \"%s\"", UNLIT_SHADER_PATH);
        ASSERT(unlitShader->Init(), "Failed to initialize shader at path \"%s\"", UNLIT_SHADER_PATH);

        std::shared_ptr<IShader> litShader = IShader::Create();
        ASSERT(litShader->Load(LIT_SHADER_PATH), "Failed to load shader at path \"%s\"", LIT_SHADER_PATH);
        ASSERT(litShader->Init(), "Failed to initialize shader at path \"%s\"", LIT_SHADER_PATH);


        whiteMaterial.SetShader(unlitShader);
        redMaterial = whiteMaterial;
        litMaterial.SetShader(litShader);

        whiteMaterial.GetProperty<std::shared_ptr<ITexture>>("u_diffuse") = GetTexture();
        whiteMaterial.GetProperty<Vector4>("u_tint") = Color::white;

        redMaterial.GetProperty<Vector4>("u_tint") = Color::red;

        litMaterial.GetProperty<std::shared_ptr<ITexture>>("u_diffuse") = GetTexture();
        litMaterial.GetProperty<Vector4>("u_tint") = Color::white;
        litMaterial.GetProperty<Vector4>("u_specularColor") = Color(.2f, .2f, .2f);
        litMaterial.GetProperty<float>("u_shininess") = 32.f;

        return true;
    }

    void TestLevelUpdate()
    {
        using namespace LibMath::Literal;
        using namespace SvRendering::Core;
        using namespace SvEditor::App;

        constexpr float  CAM_MOVE_SPEED = 3.f;
        constexpr Radian CAM_ROTATION_SPEED = 90_deg;

        static bool doThisOnce = Once();
        static Degree angle = 0_deg;

        angle += 20_deg * SvEditor::App::Engine::g_engine->GetDeltaTime();

        const Matrix4 modelRot = rotation(angle, Vector3::up());
        const Matrix4 modelMat1 = translation(-1.f, .5f, 0.f);
        const Matrix4 modelMat2 = translation(1.f, .5f, 0.f) * modelRot;

        Vector3    newPos = SvEditor::App::EngineApp::s_camTransform->getPosition();
        Quaternion newRot = SvEditor::App::EngineApp::s_camTransform->getRotation();

        //already binded
        //g_frameBuffer->Bind();
        //renderAPI.SetViewport({ 0, 0 }, { 800, 600 });

        if (EngineApp::s_moveInput->magnitudeSquared() > 0.f)
        {
            const Vector3 moveDir = EngineApp::s_moveInput->m_x * SvEditor::App::EngineApp::s_camTransform->worldRight() + EngineApp::s_moveInput->m_y * SvEditor::App::EngineApp::s_camTransform->worldBack();
            newPos += moveDir.normalized() * (CAM_MOVE_SPEED * SV_DELTA_TIME());
        }

        if (EngineApp::s_rotateInput->magnitudeSquared() > 0.f)
        {
            TVector3<Radian> angles = newRot.toEuler(ERotationOrder::YXZ);

            angles.m_y -= CAM_ROTATION_SPEED * EngineApp::s_rotateInput->m_x * SV_DELTA_TIME();
            angles.m_x += CAM_ROTATION_SPEED * EngineApp::s_rotateInput->m_y * SV_DELTA_TIME();
            angles.m_x = Degree(clamp(angles.m_x.degree(true), -85.f, 85.f));

            newRot = Quaternion::fromEuler(angles, ERotationOrder::YXZ);
        }

        SvEditor::App::EngineApp::s_camTransform->setAll(newPos, newRot, Vector3::one());

        EngineApp::s_cam->SetView(SvEditor::App::EngineApp::s_camTransform->getWorldMatrix().inverse());
        EngineApp::s_cam->SetClearColor(Color::gray);
        EngineApp::s_cam->Clear();

        BindCamUBO(EngineApp::s_cam->GetViewProjection(), SvEditor::App::EngineApp::s_camTransform->getWorldPosition());

        Frustum camFrustum = EngineApp::s_cam->GetFrustum();

        const Vector3 testPos = *EngineApp::s_camPos + Vector3::front();
        const Matrix4 testModelMat = translation(testPos) * scaling(1.5f, .5f, .1f);

        DrawModel(*SvEditor::App::EngineApp::s_model, camFrustum, modelMat1, whiteMaterial);
        DrawModel(*SvEditor::App::EngineApp::s_model, camFrustum, modelMat2, redMaterial);
        DrawModel(*SvEditor::App::EngineApp::s_model, camFrustum, testModelMat, litMaterial);

        //g_frameBuffer->Unbind();
        //renderAPI.SetViewport({ 0, 0 }, { 800, 600 });
    }
}

#pragma endregion


using namespace ToRemove;

namespace SvEditor::App
{
    Level& Engine::GetCurrentLevel()
    {
        ASSERT(m_currentLevel != nullptr, "There is no current world");

        return *m_currentLevel;
    }

    int Engine::BrowseToDefaultLevel(WorldContext& p_worldContext)
    {
        ASSERT(!m_allLevels.empty(), "No levels to brows to");
        return BrowseToLevel(p_worldContext, m_allLevels[0]->m_name);
    }

    int Engine::BrowseToLevel(WorldContext& p_worldContext, const std::string& p_levelName)
    {
        ASSERT(!m_allLevels.empty(), "No levels to brows to");
        //ASSERT(p_worldContext.m_currentLevel != nullptr); can have no current if first browse

        auto destination = std::find_if(m_allLevels.begin(), m_allLevels.end(),
            [p_levelName](const std::shared_ptr<Level>& p_level) { return p_level->m_name == p_levelName; });

        //not a valid scene name
        if (destination == m_allLevels.end())
            return -1;
        
        //alredy in scene
        if (m_currentLevel != nullptr && *destination == m_currentLevel->m_currentScene)
            return 0;

        if (PrepareLevelChange(p_worldContext, *destination) &&
            CommitLevelChange(p_worldContext, *destination))
            return -1;

        return 1;
    }

    bool Engine::PrepareLevelChange(WorldContext& /*p_context*/, const std::shared_ptr<Level>& p_newLevel)
    {
        //load destination scene
        p_newLevel->LoadLevel();

        if (!CHECK(p_newLevel->m_bIsLoaded))
            return false;

        return true;
    }

    bool Engine::CommitLevelChange(WorldContext& p_context, const std::shared_ptr<Level>& p_newLevel)
    {
        //switch and the unload old scene
        std::shared_ptr<Level> m_levelToUnload = p_context.m_currentLevel;
        p_context.m_currentLevel = p_newLevel;

        if (m_levelToUnload != nullptr)
            m_levelToUnload->UnloadLevel();

        return true;
    }

    std::shared_ptr<Engine::WorldContext>& Engine::GetWorldContextRef(GameInstance& p_instance)
    {
        return p_instance.m_worldContext;
    }

    std::shared_ptr<Engine::WorldContext> Engine::CreateNewWorldContext(EWorldType p_worldType)
    {
        //TODO: CreateNewWorldContext
        std::shared_ptr<WorldContext> wrdPtr = std::make_unique<WorldContext>();
        WorldContext& world = *wrdPtr;

        switch (p_worldType)
        {
        case SvEditor::App::EWorldType::NONE:
            break;
        case SvEditor::App::EWorldType::PIE:
            world.m_worldType = EWorldType::PIE;
            break;
        case SvEditor::App::EWorldType::EDITOR:
            world.m_worldType = EWorldType::EDITOR;
            break;
        case SvEditor::App::EWorldType::GAME:
            world.m_worldType = EWorldType::GAME;
            break;
        default:
            break;
        }

        return wrdPtr;
    }

    void Level::UpdateLevel()
    {
        TestLevelUpdate();
    }
}
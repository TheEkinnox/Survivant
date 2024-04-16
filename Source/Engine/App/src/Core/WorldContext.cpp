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
        for (size_t i = 0; i < m_frameBuffers.size(); i++)
        {
            m_frameBuffers[i]->Bind();

            switch (m_renderTypes[i])
            {
            case ERenderType::DEFAULT:  DefaultRender();     break;
            case ERenderType::ID:       IdPassRender();      break;
            default:
                break;
            }

            m_frameBuffers[i]->Unbind();
        }
    }

    intptr_t WorldContext::GetDefaultTextureId()
    {
        ASSERT(!m_frameTextures.empty(), "World has no textures");
        return static_cast<intptr_t>(dynamic_cast<OpenGLTexture&>(*m_frameTextures[0]).GetId());
    }

    void WorldContext::SetSceneCamera()
    {
        SceneView<ProjectionCamera> cameras(*m_currentScene);

        ASSERT(cameras.begin() != cameras.end(), "No Cameras In World");
        SetSceneCamera(EntityHandle(m_currentScene.get(), *cameras.begin()));
    }

    void WorldContext::InitCamera()
    {
        static const Vector3 camPos(0.f, 1.8f, 2.f);

        m_cameraCam.SetClearColor(Color::gray);
        m_cameraTrans.setAll(camPos, Quaternion::identity(), Vector3::one());
    }

    void WorldContext::SetSceneCamera(const EntityHandle& p_entity)
    {
        m_cameraHandle = p_entity;
        m_cameraCam.SetProjection(m_currentScene->Get<ProjectionCamera>(p_entity)->m_projectionMatrix);
        m_cameraTrans = *m_currentScene->Get<Transform>(p_entity);
    }

    void WorldContext::DefaultRender()
    {
        using namespace ToRemove;

        //ASSERT(!m_cameraHandle, "World context has no camera handle");

        if (m_cameraHandle)
        {
            m_cameraCam.SetProjection(m_currentScene->Get<ProjectionCamera>(m_cameraHandle)->m_projectionMatrix);
            m_cameraTrans = *m_currentScene->Get<Transform>(m_cameraHandle);
        }

        IRenderAPI::GetCurrent().Clear(true, true, true);
        DrawMainCameraScene(*m_currentScene, m_cameraCam, m_cameraTrans);
    }

    void WorldContext::AddRenderPass(ERenderType p_type)
    {
        switch (p_type)
        {
        case SvApp::Core::WorldContext::ERenderType::DEFAULT: AddDefaultRenderPass();     break;
        case SvApp::Core::WorldContext::ERenderType::ID:      AddIdRenderPass();          break;
        default:
            break;
        }

        m_renderTypes.push_back(p_type);
    }

    void WorldContext::IdPassRender()
    {
        using namespace ToRemove;

        if (m_cameraHandle)
        {
            m_cameraCam = *m_currentScene->Get<Camera>(m_cameraHandle);
            m_cameraTrans = *m_currentScene->Get<Transform>(m_cameraHandle);
        }

        IRenderAPI::GetCurrent().Clear(true, true, true);
        DrawMainCameraScene(*m_currentScene, m_cameraCam, m_cameraTrans, true);
    }

    void WorldContext::AddDefaultRenderPass()
    {
        std::shared_ptr<ITexture> color = m_frameTextures.emplace_back(
            ITexture::Create(m_viewport.m_x, m_viewport.m_y, EPixelDataFormat::RGB));
        std::shared_ptr<ITexture> depth = m_frameTextures.emplace_back(
            ITexture::Create(m_viewport.m_x, m_viewport.m_y, EPixelDataFormat::DEPTH_COMPONENT));

        color->Bind(0);

        auto frameBuffer = m_frameBuffers.emplace_back(IFrameBuffer::Create()).get();
        frameBuffer->Attach(*color, EFrameBufferAttachment::COLOR);
        frameBuffer->Attach(*depth, EFrameBufferAttachment::DEPTH);
    }

    void WorldContext::AddIdRenderPass()
    {
        std::shared_ptr<ITexture> id = m_frameTextures.emplace_back(
            ITexture::Create(m_viewport.m_x, m_viewport.m_y, EPixelDataFormat::RED_32I, EPixelDataFormat::RED_INT, EPixelDataType::UNSIGNED_INT));
        std::shared_ptr<ITexture> depth = m_frameTextures.emplace_back(
            ITexture::Create(m_viewport.m_x, m_viewport.m_y, EPixelDataFormat::DEPTH_COMPONENT));

        id->SetFilters(ETextureFilter::NEAREST, ETextureFilter::NEAREST);
        id->Bind(0);

        auto frameBuffer = m_frameBuffers.emplace_back(IFrameBuffer::Create()).get();
        frameBuffer->Attach(*id, EFrameBufferAttachment::COLOR);
        frameBuffer->Attach(*depth, EFrameBufferAttachment::DEPTH);
    }
}

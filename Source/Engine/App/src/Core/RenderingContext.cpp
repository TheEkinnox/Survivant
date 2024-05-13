//WorldContext.cpp

#include "SurvivantApp/Core/RenderingContext.h"

#include "SurvivantApp/Core/IEngine.h"
#include "SurvivantApp/Core/TempDefaultScene.h"


namespace SvApp::Core
{
    RenderingContext::RenderingContext(
        const MainCamera::Cam& p_cam, const LibMath::Transform& p_trans):
        m_mainCamera(p_cam, p_trans)
    {
    }
    RenderingContext::RenderingContext(SvCore::ECS::EntityHandle p_entity) :
        m_mainCamera(p_entity)
    {
    }

    void RenderingContext::Render(Scene* p_scene)
    {
        for (size_t i = 0; i < m_frameBuffers.size(); i++)
        {
            m_frameBuffers[i]->Bind();

            if (p_scene == nullptr)
            {
                IRenderAPI::GetCurrent().SetClearColor(Color::black);
                IRenderAPI::GetCurrent().Clear(true, false, false);
            }
            else
            {
                switch (m_renderTypes[i])
                {
                case ERenderType::GAME:     GameRender(*p_scene);     break;
                case ERenderType::SCENE:    SceneRender(*p_scene);     break;
                case ERenderType::ID:       IdRender(*p_scene);      break;
                default:
                    break;
                }
            }


            m_frameBuffers[i]->Unbind();
        }
    }

    intptr_t RenderingContext::GetTextureId(ETextureType p_renderType)
    {
        ASSERT(!m_frameTextures.empty(), "World has no textures");

        for (size_t i = 0; i < m_textureTypeBuffer.size(); i++)
        {
            if (m_textureTypeBuffer[i] == p_renderType)
                return static_cast<intptr_t>(dynamic_cast<OpenGLTexture&>(*m_frameTextures[i]).GetId());
        }

        ASSERT(false, "Rendering context does not contain render type");
        return static_cast<intptr_t>(dynamic_cast<OpenGLTexture&>(*m_frameTextures[0]).GetId());
    }

    SvCore::ECS::Entity RenderingContext::GetEntityIdValue(const Vec2& p_uv, Scene* p_scene)
    {
        ASSERT(!m_frameTextures.empty(), "World has no textures");

        SvRendering::RHI::IFrameBuffer* idBuff = nullptr;
        for (size_t i = 0; i < m_renderTypes.size(); i++)
        {
            if (m_renderTypes[i] == ERenderType::ID)
            {
                idBuff = m_frameBuffers[i].get();
                break;
            }
        }

        ASSERT(idBuff != nullptr, "Rendering context does not contain render type");

        idBuff->Bind();
        int val;

        IRenderAPI::GetCurrent().ReadPixels(
            p_uv * LibMath::Vector2(800, 600), DimensionsT(1, 1), EPixelDataFormat::RED_INT, EPixelDataType::INT, &val);

        idBuff->Unbind();

        return ToRemove::TextureValueToEntity(val, p_scene);
    }

    void RenderingContext::GameRender(Scene& p_scene)
    {
        using namespace ToRemove;

        auto camInfo = m_mainCamera.GetCamInfo();
        if (!(camInfo.first && camInfo.second))
            return;

        IRenderAPI::GetCurrent().Clear(true, true, true);
        DrawMainCameraScene(p_scene, *camInfo.first, *camInfo.second);
    }

    void RenderingContext::SceneRender(Scene& p_scene)
    {
        using namespace ToRemove;

        auto camInfo = m_mainCamera.GetCamInfo();
        if (!(camInfo.first && camInfo.second))
            return;

        IRenderAPI::GetCurrent().Clear(true, true, true);
        DrawSelectedMainCameraScene(p_scene, *camInfo.first, *camInfo.second, s_editorSelectedEntity);
    }

    void RenderingContext::AddRenderPass(ERenderType p_type)
    {
        switch (p_type)
        {
        case ERenderType::GAME:     AddColorRenderPass();     break;
        case ERenderType::SCENE:    AddColorRenderPass();     break;
        case ERenderType::ID:       AddIdRenderPass();          break;
        default:
            break;
        }

        m_renderTypes.push_back(p_type);
    }

    RenderingContext::CameraInfo RenderingContext::GetCameraInfo()
    {
        return m_mainCamera.GetCamInfo();
    }

    RenderingContext::Vec2& RenderingContext::CameraMoveInput()
    {
        return *m_mainCamera.MoveInput();
    }

    RenderingContext::Vec2& RenderingContext::CameraRotateInput()
    {
        return *m_mainCamera.RotateInput();
    }

    void RenderingContext::UpdateCameraInput()
    {
        m_mainCamera.UpdateInput();
    }

    void RenderingContext::Resize(const Vec2& p_size)
    {        
        //textures
        m_viewport = p_size;
        for (size_t i = 0; i < m_frameTextures.size(); i++)
            *m_frameTextures[i] = *CreateTexture(m_textureTypeBuffer[i]);

        //camera
        auto [cam, trans] = m_mainCamera.GetCamInfo();
        cam->SetAspect(static_cast<float>(m_viewport.m_x) / static_cast<float>(m_viewport.m_y));
    }

    void RenderingContext::DefaultFBGameRendering(EntityHandle& p_cameraEntity)
    {
        using namespace ToRemove;

        struct
        {
            CameraComponent*    m_cam;
            Transform*          m_trans;
        }camInfo
        {
            .m_cam = p_cameraEntity.Get<CameraComponent>(),
            .m_trans = p_cameraEntity.Get<Transform>()
        };
        
        IRenderAPI::GetCurrent().Clear(true, true, true);

        if (camInfo.m_cam && camInfo.m_trans)
            DrawMainCameraScene(*p_cameraEntity.GetScene(), *camInfo.m_cam, *camInfo.m_trans);
    }

    void RenderingContext::IdRender(Scene& p_scene)
    {
        using namespace ToRemove;

        auto camInfo = m_mainCamera.GetCamInfo();
        if (!(camInfo.first && camInfo.second))
            return;

        IRenderAPI::GetCurrent().Clear(true, true, true);

        DrawMainCameraScene(p_scene, *camInfo.first, *camInfo.second, true);
    }

    void RenderingContext::AddColorRenderPass()
    {
        std::shared_ptr<ITexture> color = m_frameTextures.emplace_back(CreateTexture(ETextureType::COLOR));
        m_textureTypeBuffer.push_back(ETextureType::COLOR);

        std::shared_ptr<ITexture> depth = m_frameTextures.emplace_back(CreateTexture(ETextureType::DEPTH));
        m_textureTypeBuffer.push_back(ETextureType::DEPTH);

        color->Bind(0);

        auto frameBuffer = m_frameBuffers.emplace_back(IFrameBuffer::Create()).get();
        frameBuffer->Attach(*color, EFrameBufferAttachment::COLOR);
        frameBuffer->Attach(*depth, EFrameBufferAttachment::DEPTH);
    }

    void RenderingContext::AddDefaultRenderPass()
    {
    }

    void RenderingContext::AddIdRenderPass()
    {
        std::shared_ptr<ITexture> id = m_frameTextures.emplace_back(CreateTexture(ETextureType::ID));
        m_textureTypeBuffer.push_back(ETextureType::ID);

        std::shared_ptr<ITexture> depth = m_frameTextures.emplace_back(CreateTexture(ETextureType::DEPTH));
        m_textureTypeBuffer.push_back(ETextureType::DEPTH);

        id->SetFilters(ETextureFilter::NEAREST, ETextureFilter::NEAREST);
        id->Bind(0);

        auto frameBuffer = m_frameBuffers.emplace_back(IFrameBuffer::Create()).get();
        frameBuffer->Attach(*id, EFrameBufferAttachment::COLOR);
        frameBuffer->Attach(*depth, EFrameBufferAttachment::DEPTH);
    }

    RenderingContext::TexturePtr RenderingContext::CreateTexture(const ETextureType& p_type)
    {
        switch (p_type)
        {
        case ETextureType::COLOR:   return ITexture::Create(m_viewport.m_x, m_viewport.m_y, EPixelDataFormat::RGB);
        case ETextureType::DEPTH:   return ITexture::Create(m_viewport.m_x, m_viewport.m_y, 
                                                            EPixelDataFormat::DEPTH_COMPONENT);
        case ETextureType::ID:      return ITexture::Create(m_viewport.m_x, m_viewport.m_y, 
                                                            EPixelDataFormat::RED_INT_32, EPixelDataFormat::RED_INT, 
                                                            EPixelDataType::UNSIGNED_INT);
        default: return nullptr;
        }
        
    }
}
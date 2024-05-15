//WorldContext.cpp

#include "SurvivantApp/Core/RenderingContext.h"
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
        if (!m_renderer)
            m_renderer = std::make_unique<Renderer>();

        IRenderAPI::GetCurrent().SetViewport(PosT::zero(), m_viewport);

        for (size_t i = 0; i < m_frameBuffers.size(); i++)
        {
            if (!p_scene)
            {
                m_frameBuffers[i]->Bind();
                IRenderAPI::GetCurrent().SetClearColor(Color::black);
                IRenderAPI::GetCurrent().Clear(true, false, false);
                m_frameBuffers[i]->Unbind();
            }
            else
            {
                Renderer::RenderInfo renderInfo{
                    .m_aspect = static_cast<float>(m_viewport.m_x) / static_cast<float>(m_viewport.m_y),
                    .m_scene = p_scene,
                    .m_target = m_frameBuffers[i].get()
                };

                switch (m_renderTypes[i])
                {
                case ERenderType::GAME:     GameRender(renderInfo);     break;
                case ERenderType::SCENE:    SceneRender(renderInfo);    break;
                case ERenderType::ID:       IdRender(renderInfo);       break;
                default:
                    break;
                }
            }
        }
    }

    void* RenderingContext::GetTextureId(const ETextureType p_renderType)
    {
        ASSERT(!m_frameTextures.empty(), "World has no textures");

        for (size_t i = 0; i < m_textureTypeBuffer.size(); i++)
        {
            if (m_textureTypeBuffer[i] == p_renderType)
                return m_frameTextures[i]->GetHandle();
        }

        ASSERT(false, "Rendering context does not contain render type");
        return m_frameTextures[0]->GetHandle();
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
            p_uv * m_viewport, { 1, 1 }, EPixelDataFormat::RED_INT, EPixelDataType::INT, &val);

        idBuff->Unbind();

        return ToRemove::TextureValueToEntity(val, p_scene);
    }

    void RenderingContext::GameRender(Renderer::RenderInfo& p_renderInfo) const
    {
        m_renderer->Render(p_renderInfo);
    }

    namespace
    {
        void OnIdDraw(const Renderer::DrawInfo& p_drawInfo)
        {
            IShader& shader = *p_drawInfo.m_renderPass->m_shaderOverride;
            shader.SetUniformInt("u_entityID", ToRemove::EntityToTextureValue(p_drawInfo.m_entity));
        }

        const Vector4 g_darkenColor = Vector4(0.3f, 0.3f, 0.3f, 1);

        void OnBeforeSceneDraw(const Renderer::DrawInfo& p_drawInfo)
        {
            if (p_drawInfo.m_entity == RenderingContext::s_editorSelectedEntity)
            {
                Vector4& colorRef = p_drawInfo.m_material->GetProperty<Vector4>("u_tint");
                colorRef *= g_darkenColor;
            }
        }

        void OnAfterSceneDraw(const Renderer::DrawInfo& p_drawInfo)
        {
            if (p_drawInfo.m_entity == RenderingContext::s_editorSelectedEntity)
            {
                Vector4& colorRef = p_drawInfo.m_material->GetProperty<Vector4>("u_tint");
                colorRef /= g_darkenColor;
            }
        }
    }

    void RenderingContext::IdRender(Renderer::RenderInfo& p_renderInfo)
    {
        const auto [cam, transform] = m_mainCamera.GetCamInfo();
        if (!(cam && transform))
            return;

        p_renderInfo.m_camera = { cam, transform };

        static auto editorSceneShader     = ToRemove::CreateEditorSceneShader();
        p_renderInfo.m_shaderOverride     = editorSceneShader.get();
        p_renderInfo.m_clearColorOverride = &Color::black;
        p_renderInfo.m_onBeforeDraw       = &OnIdDraw;

        const uint8_t clearMask           = CameraComponent::PackClearMask(true, true, false);
        p_renderInfo.m_clearFlagsOverride = &clearMask;

        m_renderer->Render(p_renderInfo);
    }

    void RenderingContext::SceneRender(Renderer::RenderInfo& p_renderInfo)
    {
        const auto [cam, transform] = m_mainCamera.GetCamInfo();
        if (!(cam && transform))
            return;

        p_renderInfo.m_camera       = { cam, transform };
        p_renderInfo.m_onBeforeDraw = &OnBeforeSceneDraw;
        p_renderInfo.m_onAfterDraw  = &OnAfterSceneDraw;

        IRenderAPI::GetCurrent().Clear(true, true, true);
        m_renderer->Render(p_renderInfo);
    }

    void RenderingContext::AddRenderPass(ERenderType p_type)
    {
        switch (p_type)
        {
        case ERenderType::GAME:     AddDefaultRenderPass();     break;
        case ERenderType::SCENE:    AddDefaultRenderPass();     break;
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
        for (const auto& frameTexture : m_frameTextures)
            frameTexture->Resize(m_viewport.m_x, m_viewport.m_y);

        //camera
        auto [cam, _] = m_mainCamera.GetCamInfo();

        if (cam)
            cam->SetAspect(static_cast<float>(m_viewport.m_x) / static_cast<float>(m_viewport.m_y));
    }

    void RenderingContext::AddDefaultRenderPass()
    {
        std::shared_ptr<ITexture> color = m_frameTextures.emplace_back(CreateTexture(ETextureType::COLOR));
        m_textureTypeBuffer.push_back(ETextureType::COLOR);

        std::shared_ptr<ITexture> depth = m_frameTextures.emplace_back(CreateTexture(ETextureType::DEPTH));
        m_textureTypeBuffer.push_back(ETextureType::DEPTH);

        color->Bind(0);

        auto& frameBuffer = m_frameBuffers.emplace_back(IFrameBuffer::Create());
        frameBuffer->Attach(*color, EFrameBufferAttachment::COLOR);
        frameBuffer->Attach(*depth, EFrameBufferAttachment::DEPTH);
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

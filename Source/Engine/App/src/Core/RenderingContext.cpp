//WorldContext.cpp

#include "SurvivantApp/Core/RenderingContext.h"

#include "SurvivantApp/Core/IEngine.h"
#include "SurvivantApp/Core/TempDefaultScene.h"


namespace SvApp::Core
{
    RenderingContext::RenderingContext(
        const SvRendering::Core::Camera& p_cam, const LibMath::Transform& p_trans):
        m_mainCamera(p_cam, p_trans)
    {
    }
    RenderingContext::RenderingContext(SvCore::ECS::EntityHandle p_entity) :
        m_mainCamera(p_entity)
    {
    }

    void RenderingContext::Render(Scene& p_scene)
    {
        for (size_t i = 0; i < m_frameBuffers.size(); i++)
        {
            m_frameBuffers[i]->Bind();

            switch (m_renderTypes[i])
            {
            case ERenderType::DEFAULT:  DefaultRender(p_scene);     break;
            case ERenderType::ID:       IdPassRender(p_scene);      break;
            default:
                break;
            }

            m_frameBuffers[i]->Unbind();
        }
    }

    intptr_t RenderingContext::GetDefaultTextureId()
    {
        ASSERT(!m_frameTextures.empty(), "World has no textures");
        return static_cast<intptr_t>(dynamic_cast<OpenGLTexture&>(*m_frameTextures[0]).GetId());
    }

    void RenderingContext::DefaultRender(Scene& p_scene)
    {
        using namespace ToRemove;

        auto camInfo = m_mainCamera.GetCamInfo();
        if (!(camInfo.first && camInfo.second))
            return;

        IRenderAPI::GetCurrent().Clear(true, true, true);
        DrawMainCameraScene(p_scene, *camInfo.first, *camInfo.second);
    }

    void RenderingContext::AddRenderPass(ERenderType p_type)
    {
        switch (p_type)
        {
        case ERenderType::DEFAULT: AddDefaultRenderPass();     break;
        case ERenderType::ID:      AddIdRenderPass();          break;
        default:
            break;
        }

        m_renderTypes.push_back(p_type);
    }

    void RenderingContext::SetIsDisplayed(bool /*p_isDisplayed*/)
    {
        //m_isDisplayed = p_isDisplayed;
    }

    RenderingContext::CamInfo RenderingContext::GetCameraInfo()
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

    void RenderingContext::IdPassRender(Scene& p_scene)
    {
        using namespace ToRemove;

        auto camInfo = m_mainCamera.GetCamInfo();
        if (!(camInfo.first && camInfo.second))
            return;

        IRenderAPI::GetCurrent().Clear(true, true, true);
        DrawMainCameraScene(p_scene, *camInfo.first, *camInfo.second, true);
    }

    void RenderingContext::AddDefaultRenderPass()
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

    void RenderingContext::AddIdRenderPass()
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

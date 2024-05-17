#include "SurvivantRendering/RHI/IFrameBuffer.h"

#include "SurvivantRendering/RHI/IRenderAPI.h"
#include "SurvivantRendering/RHI/OpenGL/OpenGLFrameBuffer.h"

namespace SvRendering::RHI
{
    void IFrameBuffer::BindDefault()
    {
        switch (IRenderAPI::GetCurrent().GetBackend())
        {
        case EGraphicsAPI::OPENGL:
            return OpenGLFrameBuffer::BindDefault();
        case EGraphicsAPI::NONE:
        default:
            ASSERT(false, "Failed to bind default frame buffer - Unsupported graphics api");
            return;
        }
    }

    std::unique_ptr<IFrameBuffer> IFrameBuffer::Create()
    {
        switch (IRenderAPI::GetCurrent().GetBackend())
        {
        case EGraphicsAPI::OPENGL:
            return std::make_unique<OpenGLFrameBuffer>();
        case EGraphicsAPI::NONE:
        default:
            ASSERT(false, "Failed to create frame buffer - Unsupported graphics api");
            return {};
        }
    }
}

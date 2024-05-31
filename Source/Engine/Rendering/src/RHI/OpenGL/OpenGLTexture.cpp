#include "SurvivantRendering/RHI/OpenGL/OpenGLTexture.h"

#include "SurvivantRendering/RHI/OpenGL/OpenGLAPI.h"

#include <glad/gl.h>

using namespace SvRendering::Enums;

namespace SvRendering::RHI
{
    GLint ToGLInt(const ETextureFilter p_filter)
    {
        switch (p_filter)
        {
        case ETextureFilter::NEAREST:
            return GL_NEAREST;
        case ETextureFilter::LINEAR:
            return GL_LINEAR;
        case ETextureFilter::NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case ETextureFilter::LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        case ETextureFilter::NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case ETextureFilter::LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
            ASSERT(false, "Invalid texture filter");
            return GL_INVALID_ENUM;
        }
    }

    GLint ToGLInt(const ETextureWrapMode p_wrapMode)
    {
        switch (p_wrapMode)
        {
        case ETextureWrapMode::REPEAT:
            return GL_REPEAT;
        case ETextureWrapMode::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case ETextureWrapMode::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case ETextureWrapMode::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
        default:
            ASSERT(false, "Invalid wrap mode");
            return GL_INVALID_ENUM;
        }
    }

    GLenum GetGLFormat(const uint8_t p_channels)
    {
        switch (p_channels)
        {
        case 1:
            return GL_RED;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
        default:
            ASSERT(false, "Invalid channels count. Expected 1-4 but received \"%d\".", p_channels);
            return GL_INVALID_ENUM;
        }
    }

    OpenGLTexture::OpenGLTexture(const int p_width, const int p_height, const EPixelDataFormat p_format)
        : OpenGLTexture(p_width, p_height, p_format, EPixelDataType::FLOAT)
    {
    }

    OpenGLTexture::OpenGLTexture(
        const int p_width, const int p_height, const EPixelDataFormat p_format, const EPixelDataType p_dataType)
        : OpenGLTexture(p_width, p_height, p_format, p_format, p_dataType)
    {
    }

    OpenGLTexture::OpenGLTexture(const int              p_width, const int                       p_height,
                                 const EPixelDataFormat p_internalFormat, const EPixelDataFormat p_format,
                                 const EPixelDataType   p_dataType)
        : ITexture(p_width, p_height, p_internalFormat, p_format, p_dataType)
    {
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        const GLint  internalFormat = static_cast<GLint>(OpenGLAPI::ToGLEnum(m_internalFormat));
        const GLenum dataFormat     = OpenGLAPI::ToGLEnum(m_dataFormat);
        const GLenum dataType       = OpenGLAPI::ToGLEnum(m_dataType);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, dataType, nullptr);
    }

    OpenGLTexture::OpenGLTexture(const OpenGLTexture& p_other)
        : ITexture(p_other)
    {
        if (p_other.m_id == 0)
        {
            m_id = 0;
            return;
        }

        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        const GLint  internalFormat = static_cast<GLint>(OpenGLAPI::ToGLEnum(m_internalFormat));
        const GLenum dataFormat     = OpenGLAPI::ToGLEnum(m_dataFormat);
        const GLenum dataType       = OpenGLAPI::ToGLEnum(m_dataType);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, dataType, nullptr);

        glCopyImageSubData(p_other.m_id, GL_TEXTURE_2D, 0, 0, 0, 0,
            m_id, GL_TEXTURE_2D, 0, 0, 0, 0,
            m_width, m_height, 1
        );
    }

    OpenGLTexture::OpenGLTexture(OpenGLTexture&& p_other) noexcept
        : ITexture(std::forward<ITexture&&>(p_other)), m_id(p_other.m_id)
    {
        p_other.m_id = 0;
    }

    OpenGLTexture::~OpenGLTexture()
    {
        if (m_id != 0)
            glDeleteTextures(1, &m_id);
    }

    OpenGLTexture& OpenGLTexture::GetDefault()
    {
        static OpenGLTexture texture;

        if (texture.m_id == 0)
        {
            texture.m_width          = 1;
            texture.m_height         = 1;
            texture.m_channels       = 3;
            texture.m_internalFormat = EPixelDataFormat::RGB;
            texture.m_dataFormat     = EPixelDataFormat::RGB;
            texture.m_dataType       = EPixelDataType::UNSIGNED_BYTE;

            glGenTextures(1, &texture.m_id);
            glBindTexture(GL_TEXTURE_2D, texture.m_id);

            static constexpr GLubyte white[3] = { 255, 255, 255 };
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.m_width, texture.m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, white);

            texture.SetWrapModes(ETextureWrapMode::REPEAT, ETextureWrapMode::REPEAT);
            texture.SetFilters(ETextureFilter::NEAREST, ETextureFilter::NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        return texture;
    }

    bool OpenGLTexture::Init()
    {
        if (!CHECK(m_data != nullptr, "Unable to initialize opengl texture - no pixels"))
            return false;

        if (m_id == 0)
        {
            glGenTextures(1, &m_id);

            if (!ASSUME(m_id != 0, "Unable to generate opengl texture id."))
                return false;
        }

        const GLint  internalFormat = static_cast<GLint>(OpenGLAPI::ToGLEnum(m_internalFormat));
        const GLenum dataFormat     = OpenGLAPI::ToGLEnum(m_dataFormat);
        const GLenum dataType       = OpenGLAPI::ToGLEnum(m_dataType);

        glBindTexture(GL_TEXTURE_2D, m_id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, dataType, m_data);

        if (m_loadInfo.m_generateMipmap)
            GenerateMipmap();

        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }

    void OpenGLTexture::Bind(const uint8_t p_slot)
    {
        glBindTextureUnit(p_slot, m_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGLInt(m_loadInfo.m_wrapModeU));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGLInt(m_loadInfo.m_wrapModeV));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGLInt(m_loadInfo.m_minFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGLInt(m_loadInfo.m_magFilter));
    }

    void OpenGLTexture::Unbind(const uint8_t p_slot)
    {
        glBindTextureUnit(p_slot, 0);
    }

    void* OpenGLTexture::GetHandle()
    {
        return reinterpret_cast<void*>(static_cast<uintptr_t>(GetId()));
    }

    void OpenGLTexture::Resize(const int p_width, const int p_height)
    {
        ASSERT(!m_data, "Resizing texture resource is not supported");
        m_width  = p_width;
        m_height = p_height;

        const GLint  internalFormat = static_cast<GLint>(OpenGLAPI::ToGLEnum(m_internalFormat));
        const GLenum dataFormat     = OpenGLAPI::ToGLEnum(m_dataFormat);
        const GLenum dataType       = OpenGLAPI::ToGLEnum(m_dataType);

        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, dataType, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLTexture::GenerateMipmap()
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    uint32_t OpenGLTexture::GetId() const
    {
        return m_id;
    }

    void OpenGLTexture::Copy(const ITexture& p_other)
    {
        const OpenGLTexture& other = dynamic_cast<const OpenGLTexture&>(p_other);

        if (this == &other)
            return;

        ITexture::Copy(p_other);

        if (m_id != 0)
            glDeleteTextures(1, &m_id);

        if (other.m_id == 0)
        {
            m_id = 0;
            return;
        }

        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        const GLint  internalFormat = static_cast<GLint>(OpenGLAPI::ToGLEnum(m_internalFormat));
        const GLenum dataFormat     = OpenGLAPI::ToGLEnum(m_dataFormat);
        const GLenum dataType       = OpenGLAPI::ToGLEnum(m_dataType);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, dataType, nullptr);

        glCopyImageSubData(other.m_id, GL_TEXTURE_2D, 0, 0, 0, 0,
            m_id, GL_TEXTURE_2D, 0, 0, 0, 0,
            m_width, m_height, 1
        );

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLTexture::Move(ITexture&& p_other)
    {
        OpenGLTexture& other = dynamic_cast<OpenGLTexture&>(p_other);

        if (m_id != 0)
            glDeleteTextures(1, &m_id);

        m_id = other.m_id;

        other.m_id = 0;

        ITexture::Move(std::move(p_other));
    }
}

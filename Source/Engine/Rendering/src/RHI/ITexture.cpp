#include "SurvivantRendering/RHI/ITexture.h"

#include "SurvivantRendering/RHI/IRenderAPI.h"
#include "SurvivantRendering/RHI/OpenGL/OpenGLTexture.h"

#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Debug/Logger.h>
#include <SurvivantCore/Utility/Copy.h>
#include <SurvivantCore/Utility/FileSystem.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) ASSERT(x)
#include <stb_image.h>

using namespace SvCore::Utility;
using namespace SvCore::Serialization;
using namespace SvRendering::Enums;
using namespace SvRendering::RHI;

namespace SvCore::Resources
{
    template <>
    ITexture* CreateResource<ITexture>()
    {
        switch (IRenderAPI::GetCurrent().GetBackend())
        {
        case EGraphicsAPI::OPENGL:
            return new OpenGLTexture();
        case EGraphicsAPI::NONE:
        default:
            ASSERT(false, "Failed to create texture - Unsupported graphics api");
            return nullptr;
        }
    }

    template <>
    ITexture* GetDefaultResource<ITexture>()
    {
        return &ITexture::GetDefault();
    }
}

namespace SvRendering::RHI
{
    ITexture::ITexture(
        const int p_width, const int p_height, const EPixelDataFormat p_internalFormat, const EPixelDataFormat p_dataFormat,
        const EPixelDataType p_dataType)
        : m_width(p_width), m_height(p_height), m_channels(ToChannelCount(p_dataFormat)),
        m_internalFormat(p_internalFormat), m_dataFormat(p_dataFormat), m_dataType(p_dataType)
    {
    }

    ITexture::ITexture(const ITexture& p_other)
        : IResource(p_other), m_width(p_other.m_width), m_height(p_other.m_height), m_channels(p_other.m_channels),
        m_internalFormat(p_other.m_internalFormat), m_dataFormat(p_other.m_dataFormat), m_dataType(p_other.m_dataType),
        m_loadInfo(p_other.m_loadInfo)
    {
        if (p_other.m_data == nullptr)
        {
            m_data = nullptr;
            return;
        }

        const auto bufferSize = m_width * m_height * m_channels;

        m_data = static_cast<unsigned char*>(stbi__malloc(bufferSize));

        const int result = MemCopy(m_data, bufferSize, p_other.m_data, bufferSize);
        ASSERT(result == 0, "Failed to copy texture");
    }

    ITexture::ITexture(ITexture&& p_other) noexcept
        : IResource(std::forward<IResource&&>(p_other)), m_data(p_other.m_data), m_width(p_other.m_width),
        m_height(p_other.m_height), m_channels(p_other.m_channels), m_internalFormat(p_other.m_internalFormat),
        m_dataFormat(p_other.m_dataFormat), m_dataType(p_other.m_dataType), m_loadInfo(p_other.m_loadInfo)
    {
        p_other.m_data = nullptr;
    }

    ITexture::~ITexture()
    {
        if (m_data != nullptr)
            stbi_image_free(m_data);
    }

    ITexture& ITexture::operator=(const ITexture& p_other)
    {
        Copy(p_other);
        return *this;
    }

    ITexture& ITexture::operator=(ITexture&& p_other) noexcept
    {
        Move(std::move(p_other));
        return *this;
    }

    void ITexture::Copy(const ITexture& p_other)
    {
        if (&p_other == this)
            return;

        m_width          = p_other.m_width;
        m_height         = p_other.m_height;
        m_channels       = p_other.m_channels;
        m_internalFormat = p_other.m_internalFormat;
        m_dataFormat     = p_other.m_dataFormat;
        m_dataType       = p_other.m_dataType;
        m_loadInfo       = p_other.m_loadInfo;

        if (m_data != nullptr)
            stbi_image_free(m_data);

        if (p_other.m_data)
        {
            const auto bufferSize = m_width * m_height * m_channels;

            m_data = static_cast<unsigned char*>(stbi__malloc(bufferSize));

            const int result = MemCopy(m_data, bufferSize, p_other.m_data, bufferSize);
            ASSERT(result == 0, "Failed to copy texture");
        }
        else
        {
            m_data = nullptr;
        }
    }

    void ITexture::Move(ITexture&& p_other)
    {
        if (&p_other == this)
            return;

        if (m_data != nullptr)
            stbi_image_free(m_data);

        m_data           = p_other.m_data;
        m_width          = p_other.m_width;
        m_height         = p_other.m_height;
        m_channels       = p_other.m_channels;
        m_internalFormat = p_other.m_internalFormat;
        m_dataFormat     = p_other.m_dataFormat;
        m_dataType       = p_other.m_dataType;
        m_loadInfo       = p_other.m_loadInfo;

        p_other.m_data = nullptr;
    }

    ITexture& ITexture::GetDefault()
    {
        switch (IRenderAPI::GetCurrent().GetBackend())
        {
        case EGraphicsAPI::OPENGL:
            return OpenGLTexture::GetDefault();
        case EGraphicsAPI::NONE:
        default:
            ASSERT(false, "Unsupported render api");
            static OpenGLTexture unreachable;
            return unreachable;
        }
    }

    bool ITexture::Load(const std::string& p_path)
    {
        if (m_data != nullptr)
            stbi_image_free(m_data);

        stbi_set_flip_vertically_on_load(true);
        m_data = stbi_load(p_path.c_str(), &m_width, &m_height, reinterpret_cast<int*>(&m_channels), 0);

        if (!CHECK(m_data != nullptr, "Unable to load texture from path \"%s\"", p_path.c_str()))
            return false;

        switch (m_channels)
        {
        case 1:
            m_dataFormat = EPixelDataFormat::RED;
            break;
        case 2:
            m_dataFormat = EPixelDataFormat::RG;
            break;
        case 3:
            m_dataFormat = EPixelDataFormat::RGB;
            break;
        case 4:
            m_dataFormat = EPixelDataFormat::RGBA;
            break;
        default:
            ASSERT(false, "Failed to deduce texture format from channel count");
            break;
        }

        m_internalFormat = EPixelDataFormat::RGBA;
        m_dataType       = EPixelDataType::UNSIGNED_BYTE;

        const std::string metaPath = GetMetaPath(p_path);

        if (!PathExists(metaPath))
            return true;

        return m_loadInfo.FromJson(LoadJsonFile(metaPath));
    }

    bool ITexture::Save(const std::string& p_path, const bool p_pretty)
    {
        JsonStringBuffer buffer;

        if (!p_pretty)
        {
            JsonWriter writer(buffer);

            if (!m_loadInfo.ToJson(writer) ||
                !ASSUME(writer.IsComplete(), "Failed to save texture meta data - Generated json is incomplete"))
                return false;
        }
        else
        {
            JsonPrettyWriter writer(buffer);

            if (!m_loadInfo.ToJson(writer) ||
                !ASSUME(writer.IsComplete(), "Failed to save texture meta data - Generated json is incomplete"))
                return false;
        }

        const std::string metaPath = GetMetaPath(p_path);
        std::ofstream     fs(metaPath);

        if (!CHECK(fs.is_open(), "Unable to open texture meta file at path \"%s\"", metaPath.c_str()))
            return false;

        fs << std::string_view(buffer.GetString(), buffer.GetLength());

        return CHECK(!fs.bad(), "Failed to write texture meta data to \"%s\"", metaPath.c_str());
    }

    LibMath::Vector2I ITexture::GetSize() const
    {
        return { m_width, m_height };
    }

    uint8_t ITexture::GetChannelCount() const
    {
        return m_channels;
    }

    void ITexture::SetFilters(const ETextureFilter p_minFilter, const ETextureFilter p_magFilter)
    {
        m_loadInfo.m_minFilter = p_minFilter;
        m_loadInfo.m_magFilter = p_magFilter;
    }

    void ITexture::SetWrapModes(const ETextureWrapMode p_wrapModeU, const ETextureWrapMode p_wrapModeV)
    {
        m_loadInfo.m_wrapModeU = p_wrapModeU;
        m_loadInfo.m_wrapModeV = p_wrapModeV;
    }

    uint8_t ITexture::ToChannelCount(const EPixelDataFormat p_format)
    {
        switch (p_format)
        {
        case EPixelDataFormat::RED:
        case EPixelDataFormat::RED_INT:
        case EPixelDataFormat::RED_INT_32:
        case EPixelDataFormat::RED_UINT_32:
        case EPixelDataFormat::GREEN:
        case EPixelDataFormat::BLUE:
        case EPixelDataFormat::ALPHA:
        case EPixelDataFormat::STENCIL_INDEX:
        case EPixelDataFormat::DEPTH_COMPONENT:
            return 1;
        case EPixelDataFormat::RG:
        case EPixelDataFormat::DEPTH_STENCIL:
            return 2;
        case EPixelDataFormat::RGB:
        case EPixelDataFormat::BGR:
            return 3;
        case EPixelDataFormat::RGBA:
        case EPixelDataFormat::BGRA:
            return 4;
        default:
            return 0;
        }
    }

    std::shared_ptr<ITexture> ITexture::Create()
    {
        switch (IRenderAPI::GetCurrent().GetBackend())
        {
        case EGraphicsAPI::OPENGL:
            return std::make_shared<OpenGLTexture>();
        case EGraphicsAPI::NONE:
        default:
            ASSERT(false, "Failed to create texture - Unsupported graphics api");
            return {};
        }
    }

    std::shared_ptr<ITexture> ITexture::Create(const int p_width, const int p_height, const EPixelDataFormat p_format)
    {
        switch (IRenderAPI::GetCurrent().GetBackend())
        {
        case EGraphicsAPI::OPENGL:
            return std::make_shared<OpenGLTexture>(p_width, p_height, p_format);
        case EGraphicsAPI::NONE:
        default:
            ASSERT(false, "Failed to create texture - Unsupported graphics api");
            return {};
        }
    }

    std::shared_ptr<ITexture> ITexture::Create(int p_width, int p_height, EPixelDataFormat p_format, EPixelDataType p_dataType)
    {
        switch (IRenderAPI::GetCurrent().GetBackend())
        {
        case EGraphicsAPI::OPENGL:
            return std::make_shared<OpenGLTexture>(p_width, p_height, p_format, p_dataType);
        case EGraphicsAPI::NONE:
        default:
            ASSERT(false, "Failed to create texture - Unsupported graphics api");
            return {};
        }
    }

    std::shared_ptr<ITexture> ITexture::Create(
        int p_width, int p_height, EPixelDataFormat p_internalFormat, EPixelDataFormat p_format, EPixelDataType p_dataType)
    {
        switch (IRenderAPI::GetCurrent().GetBackend())
        {
        case EGraphicsAPI::OPENGL:
            return std::make_shared<OpenGLTexture>(p_width, p_height, p_internalFormat, p_format, p_dataType);
        case EGraphicsAPI::NONE:
        default:
            ASSERT(false, "Failed to create texture - Unsupported graphics api");
            return {};
        }
    }
}

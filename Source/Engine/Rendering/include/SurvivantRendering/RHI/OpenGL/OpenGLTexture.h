#pragma once
#include "SurvivantRendering/RHI/ITexture.h"

namespace SvRendering::RHI
{
    class OpenGLTexture final : public ITexture
    {
    public:
        /**
         * \brief Creates a default texture
         */
        OpenGLTexture() = default;

        /**
         * \brief Creates a texture with the given width, height and pixel format
         * \param p_width The texture's width
         * \param p_height The texture's height
         * \param p_format The texture's pixel format
         */
        OpenGLTexture(int p_width, int p_height, Enums::EPixelDataFormat p_format);

        /**
         * \brief Creates a texture with the given width, height, pixel format and data type
         * \param p_width The texture's width
         * \param p_height The texture's height
         * \param p_format The texture's pixel format
         * \param p_dataType The texture's data type
         */
        OpenGLTexture(int p_width, int p_height, Enums::EPixelDataFormat p_format, Enums::EPixelDataType p_dataType);

        /**
         * \brief Creates a texture with the given width, height, internal data format, pixel format and data type
         * \param p_width The texture's width
         * \param p_height The texture's height
         * \param p_internalFormat The texture's internal data format
         * \param p_format The texture's pixel format
         * \param p_dataType The texture's data type
         */
        OpenGLTexture(int p_width, int p_height, Enums::EPixelDataFormat p_internalFormat, Enums::EPixelDataFormat p_format,
                      Enums::EPixelDataType p_dataType);

        /**
         * \brief Creates a copy of the given texture
         * \param p_other The texture to copy
         */
        OpenGLTexture(const OpenGLTexture& p_other);

        /**
         * \brief Creates a move copy of the given texture
         * \param p_other The texture to move
         */
        OpenGLTexture(OpenGLTexture&& p_other) noexcept;

        /**
         * \brief Releases resources allocated for the texture
         */
        ~OpenGLTexture() override;

        /**
         * \brief Assigns a copy of the given texture to this one
         * \param p_other The texture to copy
         * \return A reference to the modified texture
         */
        OpenGLTexture& operator=(const OpenGLTexture& p_other) = default;

        /**
         * \brief Moves the given texture into this one
         * \param p_other The texture to move
         * \return A reference to the modified texture
         */
        OpenGLTexture& operator=(OpenGLTexture&& p_other) noexcept = default;

        /**
         * \brief Gets or creates a white 1x1 texture
         * \return A reference to the default texture
         */
        static OpenGLTexture& GetDefault();

        /**
         * \brief Initializes the texture
         * \return True if the texture was successfully initialized. False otherwise.
         */
        bool Init() override;

        /**
         * \brief Binds the texture to the current OpenGL context
         * \param p_slot The slot the texture should be bound to
         */
        void Bind(uint8_t p_slot) override;

        /**
         * \brief Unbinds the current texture from the OpenGL context
         * \param p_slot The slot the texture is bound to
         */
        void Unbind(uint8_t p_slot) override;

        /**
         * \brief Gets the texture's internal handle
         * \return The texture's handle
         */
        virtual void* GetHandle();

        /**
         * \brief Resizes the texture if the underlying graphics api allows it
         * \param p_width The texture's new width
         * \param p_height The texture's new height
         */
        void Resize(int p_width, int p_height) override;

        /**
         * \brief Generates mipmaps for the texture
         */
        void GenerateMipmap() override;

        /**
         * \brief Gets the texture's id
         * \return The texture's id
         */
        uint32_t GetId() const;

    private:
        uint32_t m_id = 0;

        /**
         * \brief Copies the given texture into this one
         * \param p_other The texture to copy
         */
        void Copy(const ITexture& p_other) override;

        /**
         * \brief Moves the given texture into this one
         * \param p_other The texture to move
         */
        void Move(ITexture&& p_other) override;
    };
}

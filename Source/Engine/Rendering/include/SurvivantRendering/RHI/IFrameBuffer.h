#pragma once
#include "SurvivantRendering/Enums/EFrameBufferAttachment.h"

#include <memory>

namespace SvRendering::RHI
{
    class ITexture;

    class IFrameBuffer
    {
    public:
        /**
         * \brief Disable frame buffer copying
         */
        IFrameBuffer(const IFrameBuffer&) = delete;

        /**
         * \brief Creates a move copy of the given frame buffer
         * \param p_other The moved frame buffer
         */
        IFrameBuffer(IFrameBuffer&& p_other) noexcept = default;

        /**
         * \brief Destroys the given frame buffer
         */
        virtual ~IFrameBuffer() = default;

        /**
         * \brief Disable frame buffer copying
         */
        IFrameBuffer& operator=(const IFrameBuffer&) = delete;

        /**
         * \brief Moves the given frame buffer into this one
         * \param p_other The moved frame buffer
         * \return A reference to the modified frame buffer
         */
        IFrameBuffer& operator=(IFrameBuffer&& p_other) noexcept = default;

        /**
         * \brief Binds the default frame buffer for the current render api
         */
        static void BindDefault();

        /**
         * \brief Binds the frame buffer
         */
        virtual void Bind() = 0;

        /**
         * \brief Unbinds the frame buffer
         */
        virtual void Unbind() = 0;

        /**
         * \brief Attaches the given texture to the frame buffer
         * \param p_texture The texture to attach
         * \param p_attachment The target attachment
         */
        virtual void Attach(const ITexture& p_texture, Enums::EFrameBufferAttachment p_attachment) = 0;

        /**
         * \brief Detaches the given attachment from the frame buffer
         * \param p_attachment The attachment to detach
         */
        virtual void Detach(Enums::EFrameBufferAttachment p_attachment) = 0;

        /**
         * \brief Creates a frame buffer for the current render api
         * \return The created frame buffer
         */
        static std::unique_ptr<IFrameBuffer> Create();

    protected:
        IFrameBuffer() = default;
    };
}

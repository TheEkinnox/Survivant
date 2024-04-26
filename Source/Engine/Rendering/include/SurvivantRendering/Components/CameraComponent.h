#pragma once
#include "SurvivantCore/ECS/ComponentRegistry.h"

#include "SurvivantRendering/Enums/EProjectionType.h"
#include "SurvivantRendering/Core/Camera.h"
#include "SurvivantRendering/Core/Color.h"
#include "SurvivantRendering/Core/Layer.h"

namespace SvRendering::Components
{
    class CameraComponent
    {
    public:
        static constexpr uint8_t SV_CLEAR_COLOR_OFFSET = 0;
        static constexpr uint8_t SV_CLEAR_COLOR_BIT    = 1 << SV_CLEAR_COLOR_OFFSET;

        static constexpr uint8_t SV_CLEAR_DEPTH_OFFSET = SV_CLEAR_COLOR_OFFSET + 1;
        static constexpr uint8_t SV_CLEAR_DEPTH_BIT    = 1 << SV_CLEAR_DEPTH_OFFSET;

        static constexpr uint8_t SV_CLEAR_STENCIL_OFFSET = SV_CLEAR_DEPTH_OFFSET + 1;
        static constexpr uint8_t SV_CLEAR_STENCIL_BIT    = 1 << SV_CLEAR_STENCIL_OFFSET;

        /**
         * \brief Creates a default camera component
         */
        CameraComponent() = default;

        /**
         * \brief Creates a copy of the given camera component
         * \param p_other The camera component to copy
         */
        CameraComponent(const CameraComponent& p_other) = default;

        /**
         * \brief Creates a move copy of the given camera component
         * \param p_other The camera component to move
         */
        CameraComponent(CameraComponent&& p_other) noexcept = default;

        /**
         * \brief Destroys the camera component
         */
        ~CameraComponent() = default;

        /**
         * \brief Assigns a copy of the given camera component to this one
         * \param p_other The camera component to copy
         * \return A reference to the modified component
         */
        CameraComponent& operator=(const CameraComponent& p_other) = default;

        /**
         * \brief Moves the given camera component into this one
         * \param p_other The camera component to move
         * \return A reference to the modified component
         */
        CameraComponent& operator=(CameraComponent&& p_other) noexcept = default;

        /**
         * \brief Converts the camera component into a low level camera
         */
        const Core::Camera& operator*() const;

        /**
         * \brief Converts the camera component into a low level camera
         */
        const Core::Camera* operator->() const;

        /**
         * \brief Serializes the camera component to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise
         */
        bool ToJson(SvCore::Serialization::JsonWriter& p_writer) const;

        /**
         * \brief Deserializes the camera component from json
         * \param p_json The input json value
         * \return True on success. False otherwise
         */
        bool FromJson(const SvCore::Serialization::JsonValue& p_json);

        /**
         * \brief Recalculates the camera's projection matrix
         * \param p_view The camera's view matrix
         */
        void Recalculate(const LibMath::Matrix4& p_view);

        /**
         * \brief Gets the camera's projection type
         * \return The camera's projection type
         */
        Enums::EProjectionType GetProjectionType() const;

        /**
         * \brief Sets the camera's projection type
         * \param p_projectionType The camera's new projection type
         * \return A reference to the modified component
         */
        CameraComponent& SetProjectionType(Enums::EProjectionType p_projectionType);

        /**
         * \brief Sets the camera's clear color
         * \param p_color The new clear color
         * \return A reference to the modified component
         */
        CameraComponent& SetClearColor(const Core::Color& p_color);

        /**
         * \brief Sets the camera's clear color
         * \param p_r The clear color's red component
         * \param p_g The clear color's green component
         * \param p_b The clear color's blue component
         * \param p_a The clear color's alpha component
         * \return A reference to the modified component
         */
        CameraComponent& SetClearColor(float p_r, float p_g, float p_b, float p_a = 1.f);

        /**
         * \brief Gets the camera's current clear color
         * \return The camera's current clear color
         */
        Core::Color GetClearColor() const;

        /**
         * \brief Clears the current frame buffer using the camera's info
         */
        void Clear() const;

        /**
         * \brief Gets the camera's buffer clearing mask
         * \return The camera's clear mask
         */
        uint8_t GetClearMask() const;

        /**
         * \brief Sets the camera's buffer clearing mask
         * \param p_clearMask The buffer clear mask (any of CLEAR_COLOR_BIT, CLEAR_DEPTH_BIT and CLEAR_STENCIL_BIT)
         * \return A reference to the modified component
         */
        CameraComponent& SetClearMask(uint8_t p_clearMask);

        /**
         * \brief Breaks the buffer clearing mask into separate values
         * \param p_clearColor The output target for whether the color buffer should be cleared
         * \param p_clearDepth The output target for whether the depth buffer should be cleared
         * \param p_clearStencil The output target for whether the stencil buffer should be cleared
         */
        void GetClearMask(bool& p_clearColor, bool& p_clearDepth, bool& p_clearStencil) const;

        /**
         * \brief Updates the camera's clear mask from the given values
         * \param p_clearColor Whether the color buffer should be cleared
         * \param p_clearDepth Whether the depth buffer should be cleared
         * \param p_clearStencil Whether the stencil buffer should be cleared
         * \return A reference to the modified component
         */
        CameraComponent& SetClearMask(bool p_clearColor, bool p_clearDepth, bool p_clearStencil);

        /**
         * \brief Gets the camera's culling mask
         * \return The camera's culling layer mask
         */
        Core::LayerMask GetCullingMask() const;

        /**
         * \brief Sets the mask for the layers visible by the camera
         * \param p_cullingMask The camera's culling mask
         * \return A reference to the modified component
         */
        CameraComponent& SetCullingMask(Core::LayerMask p_cullingMask);

        /**
         * \brief Gets the camera's aspect ratio
         * \return The camera's aspect ratio
         */
        float GetAspect() const;

        /**
         * \brief Sets the camera's aspect ratio
         * \param p_aspect The camera's new aspect ratio
         * \return A reference to the modified component
         */
        CameraComponent& SetAspect(float p_aspect);

        /**
         * \brief Gets the camera's vertical field of view
         * \return The camera's vertical field of view
         */
        LibMath::Radian GetFovY() const;

        /**
         * \brief Sets the camera's vertical field of view
         * \param p_fovY The camera's new vertical field of view
         * \return A reference to the modified component
         */
        CameraComponent& SetFovY(const LibMath::Radian& p_fovY);

        /**
         * \brief Gets the camera's perspective near clipping plane
         * \return The camera's perspective near clipping plane
         */
        float GetPerspectiveNear() const;

        /**
         * \brief Sets the camera's perspective near clipping plane
         * \param p_zNear The camera's new perspective near clipping plane
         * \return A reference to the modified component
         */
        CameraComponent& SetPerspectiveNear(float p_zNear);

        /**
         * \brief Gets the camera's perspective far clipping plane
         * \return The camera's perspective far clipping plane
         */
        float GetPerspectiveFar() const;

        /**
         * \brief Sets the camera's perspective far clipping plane
         * \param p_zFar The camera's new perspective far clipping plane
         * \return A reference to the modified component
         */
        CameraComponent& SetPerspectiveFar(float p_zFar);

        /**
         * \brief Sets the camera's field of view and its perspective near and far clipping planes
         * \param p_fovY The camera's new vertical field of view
         * \param p_zNear The camera's new near clipping plane
         * \param p_zFar The camera's new far clipping plane
         * \return A reference to the modified component
         */
        CameraComponent& SetPerspective(const LibMath::Radian& p_fovY, float p_zNear, float p_zFar);

        /**
         * \brief Gets the camera's orthographic size
         * \return The camera's orthographic size
         */
        float GetOrthographicSize() const;

        /**
         * \brief Sets the camera's orthographic size
         * \param p_size The camera's new orthographic size
         * \return A reference to the modified component
         */
        CameraComponent& SetOrthographicSize(float p_size);

        /**
         * \brief Gets the camera's orthographic near clipping plane
         * \return The camera's orthographic near clipping plane
         */
        float GetOrthographicNear() const;

        /**
         * \brief Sets the camera's orthographic near clipping plane
         * \param p_zNear The camera's new orthographic near clipping plane
         * \return A reference to the modified component
         */
        CameraComponent& SetOrthographicNear(float p_zNear);

        /**
         * \brief Gets the camera's orthographic far clipping plane
         * \return The camera's orthographic far clipping plane
         */
        float GetOrthographicFar() const;

        /**
         * \brief Sets the camera's orthographic far clipping plane
         * \param p_zFar The camera's new orthographic far clipping plane
         * \return A reference to the modified component
         */
        CameraComponent& SetOrthographicFar(float p_zFar);

        /**
         * \brief Sets the camera's orthographic size and near/far clipping planes
         * \param p_size The camera's new orthographic size
         * \param p_zNear The camera's new orthographic near clipping plane
         * \param p_zFar The camera's new orthographic far clipping plane
         * \return A reference to the modified component
         */
        CameraComponent& SetOrthographic(float p_size, float p_zNear, float p_zFar);

    private:
        Core::Camera           m_camera;
        Enums::EProjectionType m_projectionType;

        uint8_t         m_clearMask   = SV_CLEAR_COLOR_BIT | SV_CLEAR_DEPTH_BIT | SV_CLEAR_STENCIL_BIT;
        Core::Color     m_clearColor  = Core::Color::black;
        Core::LayerMask m_cullingMask = Core::Layer::ALL;

        LibMath::Radian m_fovY;
        float           m_perspectiveNear;
        float           m_perspectiveFar;

        float m_orthographicSize;
        float m_orthographicNear;
        float m_orthographicFar;

        float m_aspect;
        bool  m_isDirty;
    };
}

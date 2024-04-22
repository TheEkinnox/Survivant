#pragma once
#include "SurvivantCore/ECS/ComponentRegistry.h"

#include "SurvivantRendering/Enums/EProjectionType.h"
#include "SurvivantRendering/Core/Camera.h"

namespace SvRendering::Components
{
    class CameraComponent
    {
    public:
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
        operator Core::Camera() const;

        /**
         * \brief Recalculates the camera's projection matrix
         */
        void Recalculate();

        /**
         * \brief Gets the camera's projection type
         * \return The camera's projection type
         */
        Enums::EProjectionType GetProjectionType() const;

        /**
         * \brief Sets the camera's projection type
         * \param p_projectionType The camera's new projection type
         */
        void SetProjectionType(Enums::EProjectionType p_projectionType);

        /**
         * \brief Gets the camera's aspect ratio
         * \return The camera's aspect ratio
         */
        float GetAspect() const;

        /**
         * \brief Sets the camera's aspect ratio
         * \param p_aspect The camera's new aspect ratio
         */
        void SetAspect(float p_aspect);

        /**
         * \brief Gets the camera's vertical field of view
         * \return The camera's vertical field of view
         */
        LibMath::Radian GetFovY() const;

        /**
         * \brief Sets the camera's vertical field of view
         * \param p_fovY The camera's new vertical field of view
         */
        void SetFovY(const LibMath::Radian& p_fovY);

        /**
         * \brief Gets the camera's perspective near clipping plane
         * \return The camera's perspective near clipping plane
         */
        float GetPerspectiveNear() const;

        /**
         * \brief Sets the camera's perspective near clipping plane
         * \param p_zNear The camera's new perspective near clipping plane
         */
        void SetPerspectiveNear(float p_zNear);

        /**
         * \brief Gets the camera's perspective far clipping plane
         * \return The camera's perspective far clipping plane
         */
        float GetPerspectiveFar() const;

        /**
         * \brief Sets the camera's perspective far clipping plane
         * \param p_zFar The camera's new perspective far clipping plane
         */
        void SetPerspectiveFar(float p_zFar);

        /**
         * \brief Sets the camera's field of view and its perspective near and far clipping planes
         * \param p_fovY The camera's new vertical field of view
         * \param p_zNear The camera's new near clipping plane
         * \param p_zFar The camera's new far clipping plane
         */
        void SetPerspective(const LibMath::Radian& p_fovY, float p_zNear, float p_zFar);

        /**
         * \brief Gets the camera's orthographic size
         * \return The camera's orthographic size
         */
        float GetOrthographicSize() const;

        /**
         * \brief Sets the camera's orthographic size
         * \param p_size The camera's new orthographic size
         */
        void SetOrthographicSize(float p_size);

        /**
         * \brief Gets the camera's orthographic near clipping plane
         * \return The camera's orthographic near clipping plane
         */
        float GetOrthographicNear() const;

        /**
         * \brief Sets the camera's orthographic near clipping plane
         * \param p_zNear The camera's new orthographic near clipping plane
         */
        void SetOrthographicNear(float p_zNear);

        /**
         * \brief Gets the camera's orthographic far clipping plane
         * \return The camera's orthographic far clipping plane
         */
        float GetOrthographicFar() const;

        /**
         * \brief Sets the camera's orthographic far clipping plane
         * \param p_zFar The camera's new orthographic far clipping plane
         */
        void SetOrthographicFar(float p_zFar);

        /**
         * \brief Sets the camera's orthographic size and near/far clipping planes
         * \param p_size The camera's new orthographic size
         * \param p_zNear The camera's new orthographic near clipping plane
         * \param p_zFar The camera's new orthographic far clipping plane
         */
        void SetOrthographic(float p_size, float p_zNear, float p_zFar);

    private:
        friend class SvCore::ECS::ComponentRegistry;

        Core::Camera           m_camera;
        Enums::EProjectionType m_projectionType;

        LibMath::Radian m_fovY;
        float           m_perspectiveNear;
        float           m_perspectiveFar;

        float m_orthographicSize;
        float m_orthographicNear;
        float m_orthographicFar;

        float m_aspect;
    };
}

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const SvRendering::Components::CameraComponent& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer,
        const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(SvRendering::Components::CameraComponent& p_out, const rapidjson::Value& p_json);
}

#pragma once
#include "SurvivantRendering/Geometry/Frustum.h"

#include <Matrix/Matrix4.h>

namespace SvRendering::Core
{
    class Camera
    {
    public:
        /**
         * \brief Creates a default camera
         */
        Camera();

        /**
         * \brief Creates a camera with the given projection and view matrices
         * \param p_projection The camera's projection matrix
         * \param p_view The camera's view matrix
         */
        explicit Camera(LibMath::Matrix4 p_projection, LibMath::Matrix4 p_view = LibMath::Matrix4(1.f));

        /**
         * \brief Creates a copy of the given camera
         * \param p_other The camera to copy
         */
        Camera(const Camera& p_other) = default;

        /**
         * \brief Creates a move copy of the given camera
         * \param p_other The camera to move
         */
        Camera(Camera&& p_other) noexcept = default;

        /**
         * \brief Destroys the camera
         */
        ~Camera() = default;

        /**
         * \brief Assigns a copy of the given camera to this one
         * \param p_other The camera to copy
         * \return A reference to the modified camera
         */
        Camera& operator=(const Camera& p_other) = default;

        /**
         * \brief Moves the given camera into this one
         * \param p_other The camera to move
         * \return A reference to the modified camera
         */
        Camera& operator=(Camera&& p_other) noexcept = default;

        /**
         * \brief Sets the camera's view matrix
         * \param p_view The camera's new view matrix
         */
        void SetView(LibMath::Matrix4 p_view);

        /**
         * \brief Sets the camera's projection matrix
         * \param p_projection The camera's new projection matrix
         */
        void SetProjection(LibMath::Matrix4 p_projection);

        /**
         * \brief Gets the camera's view-projection matrix
         * \return The camera's view-projection matrix
         */
        LibMath::Matrix4 GetViewProjection() const;

        /**
         * \brief Gets the camera's frustum
         * \return The camera's frustum
         */
        Geometry::Frustum GetFrustum() const;

    private:
        LibMath::Matrix4  m_projectionMatrix;
        LibMath::Matrix4  m_viewMatrix;
        LibMath::Matrix4  m_viewProjection;
        Geometry::Frustum m_frustum;

        /**
         * \brief Updates the camera's information
         */
        void OnChange();
    };
}

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
         * \param p_viewProjection The camera's view-projection matrix
         */
        explicit Camera(LibMath::Matrix4 p_viewProjection);

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
         * \brief Assigns a copy of the given camera to the current one
         * \param p_other The camera to copy
         * \return A reference to the current camera
         */
        Camera& operator=(const Camera& p_other) = default;

        /**
         * \brief Moves the given camera into the current one
         * \param p_other The camera to move
         * \return A reference to the current camera
         */
        Camera& operator=(Camera&& p_other) noexcept = default;

        /**
         * \brief Gets the camera's view projection matrix
         * \return The camera's view projection matrix
         */
        const LibMath::Matrix4& GetViewProjection() const;

        /**
         * \brief Sets the camera's view-projection matrix
         * \param p_viewProjection The camera's new view-projection matrix
         * \return A reference to the modified camera
         */
        Camera& SetViewProjection(const LibMath::Matrix4& p_viewProjection);

        /**
         * \brief Gets the camera's frustum
         * \return The camera's frustum
         */
        const Geometry::Frustum& GetFrustum() const;

    private:
        LibMath::Matrix4  m_viewProjectionMatrix;
        Geometry::Frustum m_frustum;
    };
}

#include "SurvivantRendering/Core/Camera.h"

namespace SvRendering::Core
{
    Camera::Camera()
        : Camera(LibMath::Matrix4(1))
    {
    }

    Camera::Camera(LibMath::Matrix4 p_projection, LibMath::Matrix4 p_view)
        : m_projectionMatrix(std::move(p_projection)), m_viewMatrix(std::move(p_view)),
        m_viewProjection(m_projectionMatrix * m_viewMatrix), m_frustum(m_viewProjection)
    {
    }

    void Camera::SetView(LibMath::Matrix4 p_view)
    {
        m_viewMatrix = std::move(p_view);
        OnChange();
    }

    void Camera::SetProjection(LibMath::Matrix4 p_projection)
    {
        m_projectionMatrix = std::move(p_projection);
        OnChange();
    }

    LibMath::Matrix4 Camera::GetViewProjection() const
    {
        return m_viewProjection;
    }

    Geometry::Frustum Camera::GetFrustum() const
    {
        return m_frustum;
    }

    void Camera::OnChange()
    {
        m_viewProjection = m_projectionMatrix * m_viewMatrix;
        m_frustum        = Geometry::Frustum(m_viewProjection);
    }
}

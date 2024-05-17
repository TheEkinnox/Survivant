#include "SurvivantRendering/Core/Camera.h"

using namespace LibMath;

namespace SvRendering::Core
{
    Camera::Camera()
        : Camera(Matrix4(1.f))
    {
    }

    Camera::Camera(Matrix4 p_viewProjection)
        : m_viewProjectionMatrix(std::move(p_viewProjection)), m_frustum(m_viewProjectionMatrix)
    {
    }

    const Matrix4& Camera::GetViewProjection() const
    {
        return m_viewProjectionMatrix;
    }

    Camera& Camera::SetViewProjection(const Matrix4& p_viewProjection)
    {
        m_viewProjectionMatrix = p_viewProjection;
        m_frustum              = Frustum(m_viewProjectionMatrix);

        return *this;
    }

    const Frustum& Camera::GetFrustum() const
    {
        return m_frustum;
    }
}

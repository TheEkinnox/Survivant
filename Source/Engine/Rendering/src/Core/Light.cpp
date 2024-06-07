#include "SurvivantRendering/Core/Light.h"

#include "SurvivantRendering/Enums/ELightType.h"

using namespace LibMath;

using namespace SvRendering::Enums;

namespace SvRendering::Core
{
    Light::Light(const Color& p_color)
        : m_color(p_color)
    {
    }

    Matrix4 Light::GetMatrix(const Transform*) const
    {
        Matrix4 lightMat;

        lightMat(0, 0) = m_color.m_r;
        lightMat(0, 1) = m_color.m_g;
        lightMat(0, 2) = m_color.m_b;
        lightMat(0, 3) = m_color.m_a;

        lightMat(3, 3) = static_cast<float>(ELightType::AMBIENT);

        return lightMat;
    }

    DirectionalLight::DirectionalLight(const Light& p_light, const Vector3& p_direction)
        : Light(p_light), m_direction(p_direction)
    {
    }

    Matrix4 DirectionalLight::GetMatrix(const Transform* p_transform) const
    {
        Matrix4 lightMat(Light::GetMatrix());
        Vector3 dir = m_direction;

        if (p_transform)
            dir.rotate(p_transform->getWorldRotation());

        lightMat(1, 0) = dir.m_x;
        lightMat(2, 0) = dir.m_y;
        lightMat(3, 0) = dir.m_z;

        lightMat(3, 3) = static_cast<float>(ELightType::DIRECTIONAL);

        return lightMat;
    }

    Attenuation::Attenuation(const float p_range)
        : Attenuation(1.f, 4.5f / p_range, 75.f / (p_range * p_range))
    {
    }

    Attenuation::Attenuation(const float p_constant, const float p_linear, const float p_quadratic)
        : m_constant(p_constant), m_linear(p_linear), m_quadratic(p_quadratic)
    {
    }

    PointLight::PointLight(const Light& p_light, const Vector3& p_position, const Attenuation& p_attenuationData)
        : Light(p_light), m_position(p_position), m_attenuationData(p_attenuationData)
    {
    }

    Matrix4 PointLight::GetMatrix(const Transform* p_transform) const
    {
        Matrix4 lightMat(Light::GetMatrix());

        const Vector3 pos = p_transform ? (p_transform->getWorldMatrix() * Vector4(m_position, 1.f)).xyz() : m_position;

        lightMat(1, 1) = pos.m_x;
        lightMat(2, 1) = pos.m_y;
        lightMat(3, 1) = pos.m_z;

        lightMat(1, 2) = m_attenuationData.m_constant;
        lightMat(2, 2) = m_attenuationData.m_linear;
        lightMat(3, 2) = m_attenuationData.m_quadratic;

        lightMat(3, 3) = static_cast<float>(ELightType::POINT);

        return lightMat;
    }

    SpotLight::SpotLight(const Light&       p_light, const Vector3&      p_position, const Vector3& p_direction,
                         const Attenuation& p_attenuation, const Cutoff& p_cutoff)
        : Light(p_light), m_position(p_position), m_direction(p_direction),
        m_attenuationData(p_attenuation), m_cutoff(p_cutoff)
    {
    }

    Matrix4 SpotLight::GetMatrix(const Transform* p_transform) const
    {
        Matrix4 lightMat(Light::GetMatrix());

        Vector3 dir = m_direction;

        if (p_transform)
            dir.rotate(p_transform->getWorldRotation());

        lightMat(1, 0) = dir.m_x;
        lightMat(2, 0) = dir.m_y;
        lightMat(3, 0) = dir.m_z;

        const Vector3 pos = p_transform ? (p_transform->getWorldMatrix() * Vector4(m_position, 1.f)).xyz() : m_position;

        lightMat(1, 1) = pos.m_x;
        lightMat(2, 1) = pos.m_y;
        lightMat(3, 1) = pos.m_z;

        lightMat(1, 2) = m_attenuationData.m_constant;
        lightMat(2, 2) = m_attenuationData.m_linear;
        lightMat(3, 2) = m_attenuationData.m_quadratic;

        lightMat(1, 3) = m_cutoff.m_inner;
        lightMat(2, 3) = m_cutoff.m_outer;

        lightMat(3, 3) = static_cast<float>(ELightType::SPOT);

        return lightMat;
    }
}

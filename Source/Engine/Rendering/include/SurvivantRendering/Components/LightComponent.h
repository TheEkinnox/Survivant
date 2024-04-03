#pragma once
#include "SurvivantRendering/Core/Light.h"
#include "SurvivantRendering/Enums/ELightType.h"

namespace SvRendering::Components
{
    struct LightComponent
    {
        Enums::ELightType m_type;

        Core::Light            m_ambient;
        Core::DirectionalLight m_directional;
        Core::PointLight       m_point;
        Core::SpotLight        m_spot;

        LightComponent(Core::Light p_ambient = Core::Light());
        LightComponent(Core::DirectionalLight p_directional);
        LightComponent(Core::PointLight p_point);
        LightComponent(Core::SpotLight p_spot);

        LightComponent(const LightComponent& p_other)     = default;
        LightComponent(LightComponent&& p_other) noexcept = default;
        ~LightComponent()                                 = default;

        LightComponent& operator=(const LightComponent& p_other)     = default;
        LightComponent& operator=(LightComponent&& p_other) noexcept = default;
    };

    inline LightComponent::LightComponent(Core::Light p_ambient)
        : m_type(Enums::ELightType::AMBIENT), m_ambient(std::move(p_ambient))
    {
    }

    inline LightComponent::LightComponent(Core::DirectionalLight p_directional)
        : m_type(Enums::ELightType::DIRECTIONAL), m_directional(std::move(p_directional))
    {
    }

    inline LightComponent::LightComponent(Core::PointLight p_point)
        : m_type(Enums::ELightType::POINT), m_point(std::move(p_point))
    {
    }

    inline LightComponent::LightComponent(Core::SpotLight p_spot)
        : m_type(Enums::ELightType::SPOT), m_spot(std::move(p_spot))
    {
    }
}

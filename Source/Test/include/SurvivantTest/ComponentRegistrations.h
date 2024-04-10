#pragma once
#include <SurvivantCore/ECS/Components/Hierarchy.h>
#include <SurvivantCore/ECS/Components/TagComponent.h>
#include <SurvivantRendering/Components/LightComponent.h>
#include <SurvivantRendering/Components/ModelComponent.h>

struct Rotator
{
    LibMath::Quaternion m_speed;
};

template <>
inline void SvCore::ECS::ComponentTraits::OnAdd<Rotator>(EntityHandle& p_handle, Rotator&)
{
    if (!p_handle.Has<LibMath::Transform>())
        p_handle.Make<LibMath::Transform>();
}

struct UserInput
{
    float           m_moveSpeed;
    LibMath::Radian m_rotationSpeed;
};

struct Temporary
{
    float m_lifeSpanSec;
};

REGISTER_COMPONENT_TYPE(Hierarchy, SvCore::ECS::HierarchyComponent)
REGISTER_COMPONENT_TYPE(Tag, SvCore::ECS::TagComponent)
REGISTER_COMPONENT_TYPE(Transform, LibMath::Transform)

REGISTER_COMPONENT_TYPE(Light, SvRendering::Components::LightComponent)
REGISTER_COMPONENT_TYPE(Model, SvRendering::Components::ModelComponent)

REGISTER_COMPONENT_TYPE(Rotator, Rotator)
REGISTER_COMPONENT_TYPE(UserInput, UserInput)
REGISTER_COMPONENT_TYPE(Temporary, Temporary)

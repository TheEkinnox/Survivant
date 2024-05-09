#pragma once
#include <SurvivantCore/ECS/ComponentHandle.h>

#include <Vector/Vector3.h>

namespace SvPhysics
{
    struct Contact
    {
        LibMath::Vector3 m_position;
        LibMath::Vector3 m_impulse;
        LibMath::Vector3 m_normal;
        float            m_distance;
    };

    struct CollisionInfo
    {
        SvCore::ECS::ComponentHandle m_colliders[2];
        std::vector<Contact>         m_contacts;
    };
}

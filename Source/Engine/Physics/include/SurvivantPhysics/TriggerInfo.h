#pragma once
#include <SurvivantCore/ECS/ComponentHandle.h>

namespace SvPhysics
{
    struct TriggerInfo
    {
        SvCore::ECS::ComponentHandle m_triggerCollider;
        SvCore::ECS::ComponentHandle m_otherCollider;
    };
}

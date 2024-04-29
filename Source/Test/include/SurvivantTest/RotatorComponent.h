#pragma once
#include <SurvivantCore/ECS/ComponentTraits.h>
#include <SurvivantCore/ECS/EntityHandle.h>

#include <Quaternion.h>
#include <Transform.h>

namespace SvTest
{
    struct Rotator
    {
        LibMath::Quaternion m_speed;
    };
}

template <>
inline void SvCore::ECS::ComponentTraits::OnAdd<SvTest::Rotator>(EntityHandle& p_handle, SvTest::Rotator&)
{
    if (!p_handle.Has<LibMath::Transform>())
        p_handle.Make<LibMath::Transform>();
}

//WorldContext.cpp

#include "SurvivantApp/Core/MainCamera.h"

#include "SurvivantApp/Core/IEngine.h"
#include "SurvivantApp/Core/TempDefaultScene.h"


using namespace LibMath;

namespace SvApp::Core
{
    MainCamera::MainCamera(
        const SvRendering::Core::Camera& p_cam, const LibMath::Transform& p_trans) :
        m_union(p_cam, p_trans),
        m_hasEntity(false)
    {
    }

    MainCamera::MainCamera(SvCore::ECS::EntityHandle p_entity) :
        m_union(p_entity),
        m_hasEntity(false)
    {
    }

    RenderingContext::CamInfo MainCamera::GetCamInfo()
    {
        if (m_hasEntity)
        {
            if (!m_union.m_entity)
                return { nullptr, nullptr };

            return { m_union.m_entity.Get<Camera>(), m_union.m_entity.Get<Transform>() };
        }

        return { &m_union.m_camInfo.m_cam, &m_union.m_camInfo.m_trans };
    }

    void MainCamera::UpdateInput()
    {
        if (m_hasEntity)
            return;

        //m_union.m_camInfo.m_cam;
        m_union.m_camInfo.m_trans = MoveTransformInput(
            m_union.m_camInfo.m_trans, m_moveInput, m_rotateInput, SV_DELTA_TIME());
    }

    void MainCamera::SetEntity(SvCore::ECS::EntityHandle p_entity)
    {
        m_union.m_entity = p_entity;
        m_hasEntity = true;
    }

    void MainCamera::SetCamera(const SvRendering::Core::Camera& p_cam, const LibMath::Transform& p_trans)
    {
        m_union.m_camInfo = { p_cam, p_trans };
        m_hasEntity = false;
    }

    Vector2* MainCamera::MoveInput()
    {
        return &m_moveInput;
    }

    Vector2* MainCamera::RotateInput()
    {
        return &m_rotateInput;
    }


    LibMath::Transform MainCamera::MoveTransformInput(
        const LibMath::Transform& p_trans, 
        const Vec2& p_move, const Vec2& p_rotation, const float p_dt)
    {
        constexpr float  CAM_MOVE_SPEED = 3.f;
        constexpr Radian CAM_ROTATION_SPEED = 90_deg;

        Vector3    newPos = p_trans.getPosition();
        Quaternion newRot = p_trans.getRotation();

        if (p_move.magnitudeSquared() > 0.f)
        {
            const Vector3 moveDir = p_move.m_x * p_trans.worldRight() + p_move.m_y * p_trans.worldBack();
            newPos += moveDir.normalized() * (CAM_MOVE_SPEED * p_dt);
        }

        if (p_rotation.magnitudeSquared() > 0.f)
        {
            TVector3<Radian> angles = newRot.toEuler(ERotationOrder::YXZ);

            angles.m_y -= CAM_ROTATION_SPEED * p_rotation.m_x * p_dt;
            angles.m_x += CAM_ROTATION_SPEED * p_rotation.m_y * p_dt;
            angles.m_x = Degree(clamp(angles.m_x.degree(true), -85.f, 85.f));

            newRot = Quaternion::fromEuler(angles, ERotationOrder::YXZ);
        }

        return Transform(newPos, newRot, Vector3::one());
    }
}

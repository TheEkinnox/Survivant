#include "SurvivantRendering/Components/CameraComponent.h"

#include <Angle.h>

using namespace LibMath;
using namespace SvRendering::Enums;
using namespace SvRendering::Core;

namespace SvRendering::Components
{
    CameraComponent::operator Camera() const
    {
        return m_camera;
    }

    void CameraComponent::Recalculate()
    {
        if (m_projectionType == EProjectionType::PERSPECTIVE)
        {
            m_camera.SetProjection(perspectiveProjection(m_fovY, m_aspect, m_perspectiveNear, m_perspectiveFar));
            return;
        }

        const float halfHeight = m_orthographicSize * .5f;
        const float halfWidth  = halfHeight * m_aspect;

        m_camera.SetProjection(orthographicProjection(-halfWidth, halfWidth, -halfHeight, halfHeight,
            m_orthographicNear, m_orthographicFar));
    }

    EProjectionType CameraComponent::GetProjectionType() const
    {
        return m_projectionType;
    }

    void CameraComponent::SetProjectionType(const EProjectionType p_projectionType)
    {
        m_projectionType = p_projectionType;
        Recalculate();
    }

    float CameraComponent::GetAspect() const
    {
        return m_aspect;
    }

    void CameraComponent::SetAspect(const float p_aspect)
    {
        m_aspect = p_aspect;
        Recalculate();
    }

    Radian CameraComponent::GetFovY() const
    {
        return m_fovY;
    }

    void CameraComponent::SetFovY(const Radian& p_fovY)
    {
        m_fovY = p_fovY;
        Recalculate();
    }

    float CameraComponent::GetPerspectiveNear() const
    {
        return m_perspectiveNear;
    }

    void CameraComponent::SetPerspectiveNear(const float p_zNear)
    {
        m_perspectiveNear = p_zNear;
        Recalculate();
    }

    float CameraComponent::GetPerspectiveFar() const
    {
        return m_perspectiveFar;
    }

    void CameraComponent::SetPerspectiveFar(const float p_zFar)
    {
        m_perspectiveFar = p_zFar;
        Recalculate();
    }

    void CameraComponent::SetPerspective(const Radian& p_fovY, const float p_zNear, const float p_zFar)
    {
        m_projectionType  = EProjectionType::PERSPECTIVE;
        m_perspectiveNear = p_zNear;
        m_perspectiveFar  = p_zFar;
        m_fovY            = p_fovY;

        Recalculate();
    }

    float CameraComponent::GetOrthographicSize() const
    {
        return m_orthographicSize;
    }

    void CameraComponent::SetOrthographicSize(const float p_size)
    {
        m_orthographicSize = p_size;
        Recalculate();
    }

    float CameraComponent::GetOrthographicNear() const
    {
        return m_orthographicNear;
    }

    void CameraComponent::SetOrthographicNear(const float p_zNear)
    {
        m_orthographicNear = p_zNear;
        Recalculate();
    }

    float CameraComponent::GetOrthographicFar() const
    {
        return m_orthographicFar;
    }

    void CameraComponent::SetOrthographicFar(const float p_zFar)
    {
        m_orthographicFar = p_zFar;
        Recalculate();
    }

    void CameraComponent::SetOrthographic(const float p_size, const float p_zNear, const float p_zFar)
    {
        m_projectionType   = EProjectionType::ORTHOGRAPHIC;
        m_orthographicSize = p_size;
        m_orthographicNear = p_zNear;
        m_orthographicFar  = p_zFar;

        Recalculate();
    }
}

namespace SvCore::ECS
{
    using namespace SvRendering::Components;

    template <>
    bool ComponentRegistry::ToJson(
        const CameraComponent& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&)
    {
        p_writer.StartObject();

        const std::string typeString = ProjectionTypeToString(p_component.GetProjectionType());

        p_writer.Key("type");
        p_writer.String(typeString.c_str(), static_cast<rapidjson::SizeType>(typeString.size()));

        p_writer.Key("perspective_fov");
        p_writer.Double(p_component.GetFovY().degree());

        p_writer.Key("perspective_near");
        p_writer.Double(p_component.GetPerspectiveNear());

        p_writer.Key("perspective_far");
        p_writer.Double(p_component.GetPerspectiveFar());

        p_writer.Key("orthographic_size");
        p_writer.Double(p_component.GetOrthographicSize());

        p_writer.Key("orthographic_near");
        p_writer.Double(p_component.GetOrthographicNear());

        p_writer.Key("orthographic_far");
        p_writer.Double(p_component.GetOrthographicFar());

        return CHECK(p_writer.EndObject(), "Unable to serialize camera component");
    }

    template <>
    bool ComponentRegistry::FromJson(CameraComponent& p_out, const rapidjson::Value& p_json, Scene*)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize camera component - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("type");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize camera component's projection type"))
            return false;

        p_out.m_projectionType = StringToProjectionType({ it->value.GetString(), it->value.GetStringLength() });

        it = p_json.FindMember("perspective_fov");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(), "Unable to deserialize camera component's vertical fov"))
            return false;

        p_out.m_fovY = Degree(it->value.Get<float>());

        it = p_json.FindMember("perspective_near");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(),
                "Unable to deserialize camera component's perspective near clipping plane"))
            return false;

        p_out.m_perspectiveNear = it->value.Get<float>();

        it = p_json.FindMember("perspective_far");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(),
                "Unable to deserialize camera component's perspective far clipping plane"))
            return false;

        p_out.m_perspectiveFar = it->value.Get<float>();

        it = p_json.FindMember("orthographic_size");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(), "Unable to deserialize camera component's orthographic size"))
            return false;

        p_out.m_orthographicSize = it->value.Get<float>();

        it = p_json.FindMember("orthographic_near");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(),
                "Unable to deserialize camera component's orthographic near clipping plane"))
            return false;

        p_out.m_orthographicNear = it->value.Get<float>();

        it = p_json.FindMember("orthographic_far");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(),
                "Unable to deserialize camera component's orthographic far clipping plane"))
            return false;

        p_out.m_orthographicFar = it->value.Get<float>();

        p_out.Recalculate();
        return true;
    }
}

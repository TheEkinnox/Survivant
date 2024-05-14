#include "SurvivantRendering/Components/CameraComponent.h"

#include "SurvivantRendering/RHI/IRenderAPI.h"

using namespace LibMath;

using namespace SvCore::Serialization;
using namespace SvRendering::Enums;
using namespace SvRendering::Core;

namespace SvRendering::Components
{
    const Camera& CameraComponent::operator*() const
    {
        return m_camera;
    }

    const Camera* CameraComponent::operator->() const
    {
        return &m_camera;
    }

    bool CameraComponent::ToJson(JsonWriter& p_writer) const
    {
        p_writer.StartObject();

        const std::string typeString = ProjectionTypeToString(m_projectionType);

        p_writer.Key("type");
        p_writer.String(typeString.c_str(), static_cast<rapidjson::SizeType>(typeString.size()));

        p_writer.Key("active");
        p_writer.Bool(m_isActive);

        p_writer.Key("clear_mask");
        p_writer.Uint(m_clearMask);

        const std::string colorString = static_cast<Vector4>(m_clearColor).string();

        p_writer.Key("clear_color");
        p_writer.String(colorString.c_str(), static_cast<rapidjson::SizeType>(colorString.size()));

        p_writer.Key("culling_mask");
        p_writer.Uint(m_cullingMask);

        p_writer.Key("perspective_fov");
        p_writer.Double(m_fovY.degree());

        p_writer.Key("perspective_near");
        p_writer.Double(m_perspectiveNear);

        p_writer.Key("perspective_far");
        p_writer.Double(m_perspectiveFar);

        p_writer.Key("orthographic_size");
        p_writer.Double(m_orthographicSize);

        p_writer.Key("orthographic_near");
        p_writer.Double(m_orthographicNear);

        p_writer.Key("orthographic_far");
        p_writer.Double(m_orthographicFar);

        return CHECK(p_writer.EndObject(), "Unable to serialize camera component");
    }

    bool CameraComponent::FromJson(const JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize camera component - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("type");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize camera component's projection type"))
            return false;

        m_projectionType = StringToProjectionType({ it->value.GetString(), it->value.GetStringLength() });

        it = p_json.FindMember("active");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsBool(), "Unable to deserialize camera component's is active flag"))
            return false;

        m_isActive = it->value.GetBool();

        it = p_json.FindMember("clear_mask");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsUint(), "Unable to deserialize camera component's clear mask"))
            return false;

        m_clearMask = static_cast<uint8_t>(it->value.GetUint());

        it = p_json.FindMember("clear_color");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize camera component's clear color"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));

            Vector4 colorVec;
            iss >> colorVec;
            m_clearColor = colorVec;
        }

        it = p_json.FindMember("culling_mask");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsUint(), "Unable to deserialize camera component's culling mask"))
            return false;

        m_cullingMask = static_cast<uint8_t>(it->value.GetUint());

        it = p_json.FindMember("perspective_fov");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(), "Unable to deserialize camera component's vertical fov"))
            return false;

        m_fovY = Degree(it->value.GetFloat());

        it = p_json.FindMember("perspective_near");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(),
                "Unable to deserialize camera component's perspective near clipping plane"))
            return false;

        m_perspectiveNear = it->value.GetFloat();

        it = p_json.FindMember("perspective_far");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(),
                "Unable to deserialize camera component's perspective far clipping plane"))
            return false;

        m_perspectiveFar = it->value.GetFloat();

        it = p_json.FindMember("orthographic_size");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(), "Unable to deserialize camera component's orthographic size"))
            return false;

        m_orthographicSize = it->value.GetFloat();

        it = p_json.FindMember("orthographic_near");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(),
                "Unable to deserialize camera component's orthographic near clipping plane"))
            return false;

        m_orthographicNear = it->value.GetFloat();

        it = p_json.FindMember("orthographic_far");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsNumber(),
                "Unable to deserialize camera component's orthographic far clipping plane"))
            return false;

        m_orthographicFar = it->value.GetFloat();
        return true;
    }

    bool CameraComponent::IsActive() const
    {
        return m_isActive;
    }

    void CameraComponent::SetActive(const bool p_isActive)
    {
        m_isActive = p_isActive;
    }

    void CameraComponent::Recalculate(const Matrix4& p_view)
    {
        BuildProjection();
        m_camera.SetViewProjection(m_projection * p_view);
    }

    EProjectionType CameraComponent::GetProjectionType() const
    {
        return m_projectionType;
    }

    CameraComponent& CameraComponent::SetProjectionType(const EProjectionType p_projectionType)
    {
        m_projectionType = p_projectionType;
        m_isDirty        = true;
        return *this;
    }

    CameraComponent& CameraComponent::SetClearColor(const Color& p_color)
    {
        m_clearColor = p_color;
        m_isDirty    = true;
        return *this;
    }

    CameraComponent& CameraComponent::SetClearColor(const float p_r, const float p_g, const float p_b, const float p_a)
    {
        SetClearColor({ p_r, p_g, p_b, p_a });
        m_isDirty = true;
        return *this;
    }

    Color CameraComponent::GetClearColor() const
    {
        return m_clearColor;
    }

    void CameraComponent::Clear() const
    {
        RHI::IRenderAPI& renderAPI = RHI::IRenderAPI::GetCurrent();

        const Color old = renderAPI.GetClearColor();
        renderAPI.SetClearColor(m_clearColor);

        bool color, depth, stencil;
        GetClearMask(color, depth, stencil);

        renderAPI.Clear(color, depth, stencil);
        renderAPI.SetClearColor(old);
    }

    uint8_t CameraComponent::GetClearMask() const
    {
        return m_clearMask;
    }

    CameraComponent& CameraComponent::SetClearMask(const uint8_t p_clearMask)
    {
        m_clearMask = p_clearMask;
        m_isDirty   = true;
        return *this;
    }

    void CameraComponent::BreakClearMask(const uint8_t p_mask, bool& p_color, bool& p_depth, bool& p_stencil)
    {
        p_color   = p_mask & SV_CLEAR_COLOR_BIT;
        p_depth   = p_mask & SV_CLEAR_DEPTH_BIT;
        p_stencil = p_mask & SV_CLEAR_STENCIL_BIT;
    }

    void CameraComponent::GetClearMask(bool& p_clearColor, bool& p_clearDepth, bool& p_clearStencil) const
    {
        BreakClearMask(m_clearMask, p_clearColor, p_clearDepth, p_clearStencil);
    }

    CameraComponent& CameraComponent::SetClearMask(bool p_clearColor, bool p_clearDepth, bool p_clearStencil)
    {
        m_clearMask = static_cast<uint8_t>(
            p_clearColor << SV_CLEAR_COLOR_OFFSET |
            p_clearDepth << SV_CLEAR_DEPTH_OFFSET |
            p_clearStencil << SV_CLEAR_STENCIL_OFFSET
        );

        m_isDirty = true;
        return *this;
    }

    LayerMask CameraComponent::GetCullingMask() const
    {
        return m_cullingMask;
    }

    CameraComponent& CameraComponent::SetCullingMask(const LayerMask p_cullingMask)
    {
        m_cullingMask = p_cullingMask;
        m_isDirty     = true;
        return *this;
    }

    float CameraComponent::GetAspect() const
    {
        return m_aspect;
    }

    CameraComponent& CameraComponent::SetAspect(const float p_aspect)
    {
        m_aspect  = p_aspect;
        m_isDirty = true;
        return *this;
    }

    Radian CameraComponent::GetFovY() const
    {
        return m_fovY;
    }

    CameraComponent& CameraComponent::SetFovY(const Radian& p_fovY)
    {
        m_fovY    = p_fovY;
        m_isDirty = true;
        return *this;
    }

    float CameraComponent::GetPerspectiveNear() const
    {
        return m_perspectiveNear;
    }

    CameraComponent& CameraComponent::SetPerspectiveNear(const float p_zNear)
    {
        m_perspectiveNear = p_zNear;
        m_isDirty         = true;
        return *this;
    }

    float CameraComponent::GetPerspectiveFar() const
    {
        return m_perspectiveFar;
    }

    CameraComponent& CameraComponent::SetPerspectiveFar(const float p_zFar)
    {
        m_perspectiveFar = p_zFar;
        m_isDirty        = true;
        return *this;
    }

    CameraComponent& CameraComponent::SetPerspective(const Radian& p_fovY, const float p_zNear, const float p_zFar)
    {
        m_projectionType  = EProjectionType::PERSPECTIVE;
        m_perspectiveNear = p_zNear;
        m_perspectiveFar  = p_zFar;
        m_fovY            = p_fovY;

        return *this;
    }

    float CameraComponent::GetOrthographicSize() const
    {
        return m_orthographicSize;
    }

    CameraComponent& CameraComponent::SetOrthographicSize(const float p_size)
    {
        m_orthographicSize = p_size;
        m_isDirty          = true;
        return *this;
    }

    float CameraComponent::GetOrthographicNear() const
    {
        return m_orthographicNear;
    }

    CameraComponent& CameraComponent::SetOrthographicNear(const float p_zNear)
    {
        m_orthographicNear = p_zNear;
        m_isDirty          = true;
        return *this;
    }

    float CameraComponent::GetOrthographicFar() const
    {
        return m_orthographicFar;
    }

    CameraComponent& CameraComponent::SetOrthographicFar(const float p_zFar)
    {
        m_orthographicFar = p_zFar;
        m_isDirty         = true;
        return *this;
    }

    CameraComponent& CameraComponent::SetOrthographic(const float p_size, const float p_zNear, const float p_zFar)
    {
        m_projectionType   = EProjectionType::ORTHOGRAPHIC;
        m_orthographicSize = p_size;
        m_orthographicNear = p_zNear;
        m_orthographicFar  = p_zFar;

        m_isDirty = true;
        return *this;
    }

    void CameraComponent::BuildProjection()
    {
        if (!m_isDirty)
            return;

        if (m_projectionType == EProjectionType::PERSPECTIVE)
        {
            m_projection = perspectiveProjection(m_fovY, m_aspect, m_perspectiveNear, m_perspectiveFar);
        }
        else
        {
            const float halfHeight = m_orthographicSize * .5f;
            const float halfWidth  = halfHeight * m_aspect;

            m_projection = orthographicProjection(-halfWidth, halfWidth, -halfHeight, halfHeight, m_orthographicNear,
                m_orthographicFar);
        }

        m_isDirty = false;
    }
}

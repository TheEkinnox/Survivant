#include "SurvivantScripting/Bindings/LuaRenderingBinder.h"

#include "SurvivantScripting/LuaTypeRegistry.h"

#include <SurvivantRendering/Components/CameraComponent.h>
#include <SurvivantRendering/Components/ModelComponent.h>

#include <sol/state.hpp>

using namespace LibMath;

using namespace SvCore::Resources;

using namespace SvRendering::Core;
using namespace SvRendering::Enums;
using namespace SvRendering::Components;
using namespace SvRendering::Resources;

namespace SvScripting::Bindings
{
    void LuaRenderingBinder::Bind(sol::state& p_luaState)
    {
        BindColor(p_luaState);
        BindLayers(p_luaState);
        BindProjectionTypes(p_luaState);
        BindModelComponent(p_luaState);
        BindCameraComponent(p_luaState);
    }

    void LuaRenderingBinder::BindColor(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Color";

        sol::constructors<
            Color(),
            Color(float, float, float),
            Color(float, float, float, float),
            Color(const Vector3&),
            Color(const Vector3&, float),
            Color(const Vector4&),
            Color(const Color&)
        > ctor;

        sol::usertype colorType = p_luaState.new_usertype<Color>(
            typeName, ctor,
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const Color& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << p_self;
                    return oss.str();
                },
                [](const Color& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << p_self << p_str;
                    return oss.str();
                }
            ),
            sol::meta_function::addition, &Color::operator+,
            sol::meta_function::subtraction, &Color::operator-,
            sol::meta_function::multiplication, sol::overload(
                sol::resolve<Color(const Color&) const>(&Color::operator*),
                sol::resolve<Color(float) const>(&Color::operator*)
            ),
            sol::meta_function::division, sol::overload(
                sol::resolve<Color(const Color&) const>(&Color::operator/),
                sol::resolve<Color(float) const>(&Color::operator/)
            ),
            "red", sol::readonly_property([]
            {
                return Color::red;
            }),
            "green", sol::readonly_property([]
            {
                return Color::green;
            }),
            "blue", sol::readonly_property([]
            {
                return Color::blue;
            }),
            "cyan", sol::readonly_property([]
            {
                return Color::cyan;
            }),
            "magenta", sol::readonly_property([]
            {
                return Color::magenta;
            }),
            "yellow", sol::readonly_property([]
            {
                return Color::yellow;
            }),
            "lime", sol::readonly_property([]
            {
                return Color::lime;
            }),
            "skyBlue", sol::readonly_property([]
            {
                return Color::skyBlue;
            }),
            "orange", sol::readonly_property([]
            {
                return Color::orange;
            }),
            "black", sol::readonly_property([]
            {
                return Color::black;
            }),
            "darkGray", sol::readonly_property([]
            {
                return Color::darkGray;
            }),
            "gray", sol::readonly_property([]
            {
                return Color::gray;
            }),
            "lightGray", sol::readonly_property([]
            {
                return Color::lightGray;
            }),
            "white", sol::readonly_property([]
            {
                return Color::white;
            }),
            "vec3", sol::readonly_property(
                [](const Color& p_self)
                {
                    return static_cast<Vector3>(p_self);
                }
            ),
            "vec4", sol::readonly_property(
                [](const Color& p_self)
                {
                    return static_cast<Vector4>(p_self);
                }
            ),
            "r", &Color::m_r,
            "g", &Color::m_g,
            "b", &Color::m_b,
            "a", &Color::m_a
        );

        colorType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<Color>(typeName);
        return (void)typeInfo;
    }

    void LuaRenderingBinder::BindLayers(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Layer";

        p_luaState.new_enum<false>(typeName,
            "NONE", Layer::NONE,
            "ALL", Layer::ALL
        );
    }

    void LuaRenderingBinder::BindProjectionTypes(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "EProjectionType";

        p_luaState.new_enum(typeName,
            "PERSPECTIVE", EProjectionType::PERSPECTIVE,
            "ORTHOGRAPHIC", EProjectionType::ORTHOGRAPHIC
        );

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<EProjectionType>(typeName);
        return (void)typeInfo;
    }

    void LuaRenderingBinder::BindClearFlags(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "EClearFlag";

        p_luaState.new_enum(typeName,
            "NONE", 0,
            "COLOR", CameraComponent::SV_CLEAR_COLOR_BIT,
            "DEPTH", CameraComponent::SV_CLEAR_DEPTH_BIT,
            "STENCIL", CameraComponent::SV_CLEAR_STENCIL_BIT
        );
    }

    void LuaRenderingBinder::BindModelComponent(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Model";

        sol::usertype modelType = p_luaState.new_usertype<ModelComponent>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "model", sol::property(
                [](const ModelComponent& p_self)
                {
                    return GenericResourceRef(p_self.m_model);
                },
                [](ModelComponent& p_self, const GenericResourceRef& p_ref)
                {
                    p_self.m_model = p_ref;
                }
            ),
            "material", sol::property(
                [](const ModelComponent& p_self)
                {
                    return GenericResourceRef(p_self.m_material);
                },
                [](ModelComponent& p_self, const GenericResourceRef& p_ref)
                {
                    p_self.m_material = p_ref;
                }
            ),
            "layerMask", &ModelComponent::m_layerMask
        );

        modelType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<ModelComponent>(typeName);
        return (void)typeInfo;
    }

    void LuaRenderingBinder::BindCameraComponent(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Camera";

        sol::usertype cameraType = p_luaState.new_usertype<CameraComponent>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "isActive", sol::property(
                &CameraComponent::IsActive,
                &CameraComponent::SetActive
            ),
            "order", sol::property(
                &CameraComponent::GetOrder,
                &CameraComponent::SetOrder
            ),
            "projectionType", sol::property(
                &CameraComponent::GetProjectionType,
                &CameraComponent::SetProjectionType
            ),
            "clearColor", sol::property(
                &CameraComponent::GetClearColor,
                sol::resolve<CameraComponent&(const Color&)>(&CameraComponent::SetClearColor)
            ),
            "clearMask", sol::property(
                sol::resolve<uint8_t() const>(&CameraComponent::GetClearMask),
                sol::resolve<CameraComponent&(uint8_t)>(&CameraComponent::SetClearMask)
            ),
            "cullingMask", sol::property(
                &CameraComponent::GetCullingMask,
                &CameraComponent::SetCullingMask
            ),
            "aspect", sol::readonly_property(&CameraComponent::GetAspect),
            "fovY", sol::property(
                &CameraComponent::GetFovY,
                &CameraComponent::SetFovY
            ),
            "perspectiveNear", sol::property(
                &CameraComponent::GetPerspectiveNear,
                &CameraComponent::SetPerspectiveNear
            ),
            "perspectiveFar", sol::property(
                &CameraComponent::GetPerspectiveFar,
                &CameraComponent::SetPerspectiveFar
            ),
            "orthographicSize", sol::property(
                &CameraComponent::GetOrthographicSize,
                &CameraComponent::SetOrthographicSize
            ),
            "orthographicNear", sol::property(
                &CameraComponent::GetOrthographicNear,
                &CameraComponent::SetOrthographicNear
            ),
            "orthographicFar", sol::property(
                &CameraComponent::GetOrthographicFar,
                &CameraComponent::SetOrthographicFar
            ),
            "SetClearColor", sol::overload(
                sol::resolve<CameraComponent&(const Color&)>(&CameraComponent::SetClearColor),
                sol::resolve<CameraComponent&(float, float, float, float)>(&CameraComponent::SetClearColor),
                [](CameraComponent& p_self, const float p_r, const float p_g, const float p_b)
                {
                    return p_self.SetClearColor(p_r, p_g, p_b);
                }
            ),
            "Clear", &CameraComponent::Clear,
            "SetPerspective", &CameraComponent::SetPerspective,
            "SetOrthographic", &CameraComponent::SetOrthographic,
            "BreakClearMask", [](const uint8_t p_clearMask)
            -> std::tuple<bool, bool, bool>
            {
                bool color, depth, stencil;
                CameraComponent::BreakClearMask(p_clearMask, color, depth, stencil);
                return { color, depth, stencil };
            },
            "MakeClearMask", &CameraComponent::PackClearMask
        );

        cameraType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<CameraComponent>(typeName);
        return (void)typeInfo;
    }
}

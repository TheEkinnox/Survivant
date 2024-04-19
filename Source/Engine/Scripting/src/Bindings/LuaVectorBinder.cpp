#include "SurvivantScripting/Bindings/LuaMathBinder.h"

#include "SurvivantScripting/LuaTypeRegistry.h"

#include <Vector.h>

#include <sol/state.hpp>

using namespace LibMath;

namespace SvScripting::Bindings
{
    void LuaMathBinder::BindVector2(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Vector2";

        sol::constructors<
            Vector2(),
            Vector2(float),
            Vector2(float, float),
            Vector2(const Vector2&)
        > ctor;

        sol::usertype vectorType = p_luaState.new_usertype<Vector2>(
            typeName, ctor,
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const Vector2& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << p_self;
                    return oss.str();
                },
                [](const Vector2& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << p_self << p_str;
                    return oss.str();
                }
            ),
            sol::meta_function::unary_minus, sol::resolve<Vector2(const Vector2&)>(&operator-),
            sol::meta_function::addition, sol::overload(
                sol::resolve<Vector2(Vector2, const Vector2&)>(&operator+),
                sol::resolve<Vector2(Vector2, float)>(&operator+)
            ),
            sol::meta_function::subtraction, sol::overload(
                sol::resolve<Vector2(Vector2, const Vector2&)>(&operator-),
                sol::resolve<Vector2(Vector2, float)>(&operator-)
            ),
            sol::meta_function::multiplication, sol::overload(
                sol::resolve<Vector2(Vector2, const Vector2&)>(&operator*),
                sol::resolve<Vector2(Vector2, float)>(&operator*)
            ),
            sol::meta_function::division, sol::overload(
                sol::resolve<Vector2(Vector2, const Vector2&)>(&operator/),
                sol::resolve<Vector2(Vector2, float)>(&operator/)
            ),
            "zero", sol::readonly_property(&Vector2::zero),
            "one", sol::readonly_property(&Vector2::one),
            "up", sol::readonly_property(&Vector2::up),
            "down", sol::readonly_property(&Vector2::down),
            "left", sol::readonly_property(&Vector2::left),
            "right", sol::readonly_property(&Vector2::right),
            "AngleFrom", &Vector2::angleFrom<float>,
            "SignedAngleFrom", &Vector2::signedAngleFrom<float>,
            "Cross", &Vector2::cross<float>,
            "DistanceFrom", &Vector2::distanceFrom<float>,
            "DistanceSquaredFrom", &Vector2::distanceSquaredFrom<float>,
            "Dot", &Vector2::dot<float>,
            "isUnit", sol::readonly_property(&Vector2::isUnitVector),
            "magnitude", sol::readonly_property(&Vector2::magnitude),
            "magnitudeSquared", sol::readonly_property(&Vector2::magnitudeSquared),
            "Normalize", &Vector2::normalize,
            "normalized", sol::readonly_property(&Vector2::normalized),
            "ProjectOnto", &Vector2::projectOnto,
            "ReflectOnto", &Vector2::reflectOnto,
            "Rotate", &Vector2::rotate,
            "Rotated", [](Vector2 p_self, const Radian& p_angle)
            {
                p_self.rotate(p_angle);
                return p_self;
            },
            "Scale", &Vector2::scale,
            "Translate", &Vector2::translate,
            "x", &Vector2::m_x,
            "y", &Vector2::m_y
        );

        vectorType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<Vector2>(typeName);
        return (void)typeInfo;
    }

    void LuaMathBinder::BindVector3(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Vector3";

        sol::constructors<
            Vector3(),
            Vector3(float),
            Vector3(const Vector2&),
            Vector3(const Vector3&),
            Vector3(float, float, float)
        > ctor;

        sol::usertype vectorType = p_luaState.new_usertype<Vector3>(
            typeName, ctor,
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const Vector3& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << p_self;
                    return oss.str();
                },
                [](const Vector3& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << p_self << p_str;
                    return oss.str();
                }
            ),
            sol::meta_function::unary_minus, sol::resolve<Vector3(const Vector3&)>(&operator-),
            sol::meta_function::addition, sol::overload(
                sol::resolve<Vector3(Vector3, const Vector3&)>(&operator+),
                sol::resolve<Vector3(Vector3, float)>(&operator+)
            ),
            sol::meta_function::subtraction, sol::overload(
                sol::resolve<Vector3(Vector3, const Vector3&)>(&operator-),
                sol::resolve<Vector3(Vector3, float)>(&operator-)
            ),
            sol::meta_function::multiplication, sol::overload(
                sol::resolve<Vector3(Vector3, const Vector3&)>(&operator*),
                sol::resolve<Vector3(Vector3, float)>(&operator*)
            ),
            sol::meta_function::division, sol::overload(
                sol::resolve<Vector3(Vector3, const Vector3&)>(&operator/),
                sol::resolve<Vector3(Vector3, float)>(&operator/)
            ),
            "zero", sol::readonly_property(&Vector3::zero),
            "one", sol::readonly_property(&Vector3::one),
            "up", sol::readonly_property(&Vector3::up),
            "down", sol::readonly_property(&Vector3::down),
            "left", sol::readonly_property(&Vector3::left),
            "right", sol::readonly_property(&Vector3::right),
            "front", sol::readonly_property(&Vector3::front),
            "back", sol::readonly_property(&Vector3::back),
            "xy", sol::readonly_property([](const Vector3& p_self)
            {
                return Vector2(p_self.m_x, p_self.m_y);
            }),
            "AngleFrom", &Vector3::angleFrom,
            "SignedAngleFrom", &Vector3::signedAngleFrom,
            "Cross", &Vector3::cross,
            "DistanceFrom", &Vector3::distanceFrom,
            "DistanceSquaredFrom", &Vector3::distanceSquaredFrom,
            "Distance2DFrom", &Vector3::distance2DFrom,
            "Distance2DSquaredFrom", &Vector3::distance2DSquaredFrom,
            "Dot", &Vector3::dot<float>,
            "isUnit", sol::readonly_property(&Vector3::isUnitVector),
            "magnitude", sol::readonly_property(&Vector3::magnitude),
            "magnitudeSquared", sol::readonly_property(&Vector3::magnitudeSquared),
            "Normalize", &Vector3::normalize,
            "normalized", sol::readonly_property(&Vector3::normalized),
            "ProjectOnto", &Vector3::projectOnto,
            "ReflectOnto", &Vector3::reflectOnto,
            "Rotate", sol::overload(
                static_cast<void (Vector3::*)(const Radian&, const Vector3&)>(&Vector3::rotate),
                static_cast<void (Vector3::*)(const Quaternion&)>(&Vector3::rotate)
            ),
            "Rotated", sol::overload(
                [](Vector3 p_self, const Radian& p_angle, const Vector3& p_axis)
                {
                    p_self.rotate(p_angle, p_axis);
                    return p_self;
                },
                [](Vector3 p_self, const Quaternion& p_quaternion)
                {
                    p_self.rotate(p_quaternion);
                    return p_self;
                }
            ),
            "Scale", &Vector3::scale,
            "Translate", &Vector3::translate,
            "x", &Vector3::m_x,
            "y", &Vector3::m_y,
            "z", &Vector3::m_z
        );

        vectorType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<Vector3>(typeName);
        return (void)typeInfo;
    }

    void LuaMathBinder::BindVector4(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Vector4";

        sol::constructors<
            Vector4(),
            Vector4(float),
            Vector4(const Vector4&),
            Vector4(const Vector3&, float),
            Vector4(float, float, float, float)
        > ctor;

        sol::usertype vectorType = p_luaState.new_usertype<Vector4>(
            typeName, ctor,
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const Vector4& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << p_self;
                    return oss.str();
                },
                [](const Vector4& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << p_self << p_str;
                    return oss.str();
                }
            ),
            sol::meta_function::unary_minus, sol::resolve<Vector4(const Vector4&)>(&operator-),
            sol::meta_function::addition, sol::overload(
                sol::resolve<Vector4(Vector4, const Vector4&)>(&operator+),
                sol::resolve<Vector4(Vector4, float)>(&operator+)
            ),
            sol::meta_function::subtraction, sol::overload(
                sol::resolve<Vector4(Vector4, const Vector4&)>(&operator-),
                sol::resolve<Vector4(Vector4, float)>(&operator-)
            ),
            sol::meta_function::multiplication, sol::overload(
                sol::resolve<Vector4(Vector4, const Vector4&)>(&operator*),
                sol::resolve<Vector4(Vector4, float)>(&operator*)
            ),
            sol::meta_function::division, sol::overload(
                sol::resolve<Vector4(Vector4, const Vector4&)>(&operator/),
                sol::resolve<Vector4(Vector4, float)>(&operator/)
            ),
            "zero", sol::readonly_property(&Vector4::zero),
            "one", sol::readonly_property(&Vector4::one),
            "up", sol::readonly_property(&Vector4::up),
            "down", sol::readonly_property(&Vector4::down),
            "left", sol::readonly_property(&Vector4::left),
            "right", sol::readonly_property(&Vector4::right),
            "front", sol::readonly_property(&Vector4::front),
            "back", sol::readonly_property(&Vector4::back),
            "xy", sol::readonly_property(
                [](const Vector4& p_self)
                {
                    return Vector2(p_self.m_x, p_self.m_y);
                }
            ),
            "xyz", sol::readonly_property(&Vector4::xyz),
            "DistanceFrom", &Vector4::distanceFrom<float>,
            "DistanceSquaredFrom", &Vector4::distanceSquaredFrom<float>,
            "Dot", &Vector4::dot<float>,
            "isUnit", sol::readonly_property(&Vector4::isUnitVector),
            "magnitude", sol::readonly_property(&Vector4::magnitude),
            "magnitudeSquared", sol::readonly_property(&Vector4::magnitudeSquared),
            "Normalize", &Vector4::normalize,
            "normalized", sol::readonly_property(&Vector4::normalized),
            "Rotate", sol::overload(
                static_cast<void (Vector4::*)(const Radian&, const Vector3&)>(&Vector4::rotate),
                static_cast<void (Vector4::*)(const Quaternion&)>(&Vector4::rotate)
            ),
            "Rotated", sol::overload(
                [](Vector4 p_self, const Radian& p_angle, const Vector3& p_axis)
                {
                    p_self.rotate(p_angle, p_axis);
                    return p_self;
                },
                [](Vector4 p_self, const Quaternion& p_quaternion)
                {
                    p_self.rotate(p_quaternion);
                    return p_self;
                }
            ),
            "Scale", &Vector4::scale,
            "Translate", &Vector4::translate,
            "x", &Vector4::m_x,
            "y", &Vector4::m_y,
            "z", &Vector4::m_z,
            "w", &Vector4::m_w
        );

        vectorType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<Vector4>(typeName);
        return (void)typeInfo;
    }
}

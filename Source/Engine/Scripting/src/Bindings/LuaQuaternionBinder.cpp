#include "SurvivantScripting/LuaTypeRegistry.h"
#include "SurvivantScripting/Bindings/LuaMathBinder.h"

#include <Quaternion.h>

#include <sol/state.hpp>

using namespace LibMath;

namespace SvScripting::Bindings
{
    void LuaMathBinder::BindQuaternion(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Quaternion";

        sol::constructors<
            Quaternion(),
            Quaternion(float),
            Quaternion(float, Vector3),
            Quaternion(Radian, Vector3),
            Quaternion(Degree, Vector3),
            Quaternion(const Quaternion&),
            Quaternion(float, float, float, float)
        > ctor;

        sol::usertype quatType = p_luaState.new_usertype<Quaternion>(
            typeName, ctor,
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const Quaternion& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << p_self;
                    return oss.str();
                },
                [](const Quaternion& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << p_self << p_str;
                    return oss.str();
                }
            ),
            sol::meta_function::multiplication, sol::overload(
                sol::resolve<Quaternion(Quaternion, const Quaternion&)>(&operator*),
                sol::resolve<Quaternion(Quaternion, float)>(&operator*),
                sol::resolve<Quaternion(float, Quaternion)>(&operator*)
            ),
            sol::meta_function::division, sol::overload(
                sol::resolve<Quaternion(Quaternion, const Quaternion&)>(&operator/),
                sol::resolve<Quaternion(Quaternion, float)>(&operator/)
            ),
            "identity", sol::readonly_property(&Quaternion::identity),
            "FromEuler", sol::overload(
                sol::resolve<Quaternion(const Radian&, const Radian&, const Radian&, ERotationOrder)>(&Quaternion::fromEuler),
                [](const Degree& p_x, const Degree& p_y, const Degree& p_z, const ERotationOrder p_order)
                {
                    return Quaternion::fromEuler(p_x, p_y, p_z, p_order);
                }
            ),
            "ToEuler", [](const Quaternion& p_self, const ERotationOrder p_rotationOrder)
            -> std::tuple<Radian, Radian, Radian>
            {
                TVector3<Radian> out = p_self.toEuler(p_rotationOrder);
                return { out.m_x, out.m_y, out.m_z };
            },
            "FromTo", &Quaternion::fromTo<float>,
            "ToAngleAxis", [](const Quaternion& p_self)
            -> std::pair<Radian, Vector3>
            {
                Radian  angle;
                Vector3 axis;
                p_self.toAngleAxis(angle, axis);
                return { angle, axis };
            },
            "isUnit", sol::readonly_property(&Quaternion::isUnit),
            "isIdentity", sol::readonly_property([](const Quaternion& self)
            {
                return self == Quaternion::identity();
            }),
            "magnitude", sol::readonly_property(&Quaternion::magnitude),
            "magnitudeSquared", sol::readonly_property(&Quaternion::magnitudeSquared),
            "Dot", &Quaternion::dot<float>,
            "conjugate", sol::readonly_property(&Quaternion::conjugate),
            "inverse", sol::readonly_property(&Quaternion::inverse),
            "Normalize", &Quaternion::normalize,
            "normalized", sol::readonly_property(&Quaternion::normalized),
            "x", &Quaternion::m_x,
            "y", &Quaternion::m_y,
            "z", &Quaternion::m_z,
            "w", &Quaternion::m_w
        );

        quatType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<Quaternion>(typeName);
        return (void)typeInfo;
    }
}

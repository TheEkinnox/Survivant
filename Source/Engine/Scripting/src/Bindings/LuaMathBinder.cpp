#include "SurvivantScripting/Bindings/LuaMathBinder.h"

#include "SurvivantScripting/LuaTypeRegistry.h"

#include <sol/state.hpp>

using namespace LibMath;

namespace SvScripting::Bindings
{
    void LuaMathBinder::Bind(sol::state& p_luaState)
    {
        BindAngles(p_luaState);
        BindRotationOrder(p_luaState);
        BindMatrix(p_luaState);
        BindVector2(p_luaState);
        BindVector3(p_luaState);
        BindVector4(p_luaState);
        BindQuaternion(p_luaState);
        BindTransform(p_luaState);
        BindGlobals(p_luaState);
    }

    void LuaMathBinder::BindRotationOrder(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "ERotationOrder";

        p_luaState.new_enum(typeName,
            "XYZ", ERotationOrder::XYZ,
            "XZY", ERotationOrder::XZY,
            "YXZ", ERotationOrder::YXZ,
            "YZX", ERotationOrder::YZX,
            "ZXY", ERotationOrder::ZXY,
            "ZYX", ERotationOrder::ZYX
        );

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<ERotationOrder>(typeName);
        return (void)typeInfo;
    }

    void LuaMathBinder::BindGlobals(const sol::state& p_luaState)
    {
        sol::table math = p_luaState.get<sol::table>("math");

        /* Arithmetic */
        math.set_function("round", &LibMath::round<double>);
        math.set_function("clamp", sol::overload(
            &LibMath::clamp<double>,
            &LibMath::clamp<Vector2>,
            &LibMath::clamp<Vector3>,
            &LibMath::clamp<Vector4>,
            &LibMath::clamp<Degree>,
            &LibMath::clamp<Radian>
        ));

        math.set_function("snap", sol::overload(
            &LibMath::snap<double>,
            &LibMath::snap<Vector2>,
            &LibMath::snap<Vector3>,
            &LibMath::snap<Vector4>,
            &LibMath::snap<Degree>,
            &LibMath::snap<Radian>
        ));

        math.set_function("min", sol::overload(
            &LibMath::min<double>,
            &LibMath::min<Vector2>,
            &LibMath::min<Vector3>,
            &LibMath::min<Vector4>,
            &LibMath::min<Degree>,
            &LibMath::min<Radian>
        ));

        math.set_function("max", sol::overload(
            &LibMath::max<double>,
            &LibMath::max<Vector2>,
            &LibMath::max<Vector3>,
            &LibMath::max<Vector4>,
            &LibMath::max<Degree>,
            &LibMath::max<Radian>
        ));

        math.set_function("wrap", &LibMath::wrap<double>);
        math.set_function("sign", &LibMath::sign<double>);

        math.set_function("checkRange", sol::overload(
            &LibMath::isInRange<double>,
            &LibMath::isInRange<Vector2>,
            &LibMath::isInRange<Vector3>,
            &LibMath::isInRange<Vector4>,
            &LibMath::isInRange<Degree>,
            &LibMath::isInRange<Radian>
        ));

        math.set_function("isNear",
            [](const float p_a, const float p_b)
            {
                return floatEquals(p_a, p_b);
            }
        );

        /* Interpolation */
        math.set_function("lerp", sol::overload(
            &LibMath::lerp<double, double>,
            &LibMath::lerp<Vector2, float>,
            &LibMath::lerp<Vector3, float>,
            &LibMath::lerp<Vector4, float>,
            &LibMath::lerp<Quaternion, float>,
            &LibMath::lerp<Degree, float>,
            &LibMath::lerp<Radian, float>
        ));

        math.set_function("slerp", sol::overload(
            &LibMath::slerp<Vector2, float>,
            &LibMath::slerp<Vector3, float>,
            &LibMath::slerp<Vector4, float>,
            &LibMath::slerp<Quaternion, float>
        ));

        /* Trigonometry */
        math.set_function("sin", sol::overload(
            static_cast<double(*)(double)>(&std::sin),
            &LibMath::sin,
            [](const Degree& p_angle)
            {
                return LibMath::sin(p_angle);
            }
        ));

        math.set_function("cos", sol::overload(
            static_cast<double(*)(double)>(&std::cos),
            &LibMath::cos,
            [](const Degree& p_angle)
            {
                return LibMath::cos(p_angle);
            }
        ));

        math.set_function("tan", sol::overload(
            static_cast<double(*)(double)>(&std::tan),
            &LibMath::tan,
            [](const Degree& p_angle)
            {
                return LibMath::tan(p_angle);
            }
        ));

        math.set_function("asin", &LibMath::asin);
        math.set_function("acos", &LibMath::acos);
        math.set_function("atan", sol::overload(
            sol::resolve<Radian(float)>(&LibMath::atan),
            sol::resolve<Radian(float, float)>(&LibMath::atan)
        ));
    }
}

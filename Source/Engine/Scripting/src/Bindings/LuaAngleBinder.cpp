#include "SurvivantScripting/LuaTypeRegistry.h"
#include "SurvivantScripting/Bindings/LuaMathBinder.h"

#include <sol/state.hpp>

using namespace LibMath;

namespace SvScripting::Bindings
{
    template <typename T, typename U>
    void BindAngleType(const std::string& p_typeName, sol::state& p_luaState)
    {
        sol::usertype angleType = p_luaState.new_usertype<T>(
            p_typeName, sol::constructors<T(), T(float), T(const T&), T(const U&)>(),
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const T& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << p_self;
                    return oss.str();
                },
                [](const T& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << p_self << p_str;
                    return oss.str();
                }
            ),
            sol::meta_function::less_than, sol::overload(
                sol::resolve<bool(const T&, const T&)>(&operator<),
                sol::resolve<bool(const T&, const U&)>(&operator<)
            ),
            sol::meta_function::less_than_or_equal_to, sol::overload(
                sol::resolve<bool(const T&, const T&)>(&operator<=),
                sol::resolve<bool(const T&, const U&)>(&operator<=)
            ),
            sol::meta_function::equal_to, sol::overload(
                sol::resolve<bool(const T&, const T&)>(&operator==),
                sol::resolve<bool(const T&, const U&)>(&operator==)
            ),
            sol::meta_function::unary_minus, sol::resolve<T(const T&)>(&operator-),
            sol::meta_function::addition, sol::overload(
                sol::resolve<T(T, const T&)>(&operator+),
                [](const T& p_self, const U& p_other)
                {
                    return p_self + T(p_other);
                }
            ),
            sol::meta_function::subtraction, sol::overload(
                sol::resolve<T(T, const T&)>(&operator-),
                [](const T& p_self, const U& p_other)
                {
                    return p_self - T(p_other);
                }
            ),
            sol::meta_function::multiplication, sol::resolve<T(T, float)>(&operator*),
            sol::meta_function::division, sol::resolve<T(T, float)>(&operator/),
            "Wrap", &T::wrap,
            "raw", sol::readonly_property(&T::raw),
            "rawDegree", sol::readonly_property(
                [](const T& p_self)
                {
                    return Degree(p_self).raw();
                }
            ),
            "rawRadian", sol::readonly_property(
                [](const T& p_self)
                {
                    return Radian(p_self).raw();
                }
            ),
            "GetWrappedDegree", sol::readonly_property(&T::degree),
            "GetWrappedRadian", sol::readonly_property(&T::radian)
        );

        angleType["__type"]["name"] = p_typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<T>(p_typeName);
        return (void)typeInfo;
    }

    void LuaMathBinder::BindAngles(sol::state& p_luaState)
    {
        BindAngleType<Degree, Radian>("Degree", p_luaState);
        BindAngleType<Radian, Degree>("Radian", p_luaState);
    }
}

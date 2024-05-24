#pragma once
#include "SurvivantScripting/LuaTypeRegistry.h"

#include <SurvivantCore/Utility/EnumFlags.h>

#include <sol/state.hpp>

namespace SvScripting::Bindings
{
    template <typename EnumT, typename DataT>
    void BindEnumFlags(const char* p_name, sol::state& p_luaState)
    {
        using EnumFlagsT = SvCore::Utility::EnumFlags<EnumT, DataT>;

        sol::constructors<
            EnumFlagsT(),
            EnumFlagsT(EnumT),
            EnumFlagsT(DataT),
            EnumFlagsT(const EnumFlagsT&)
        > ctor;

        sol::usertype userType = p_luaState.new_usertype<EnumFlagsT>(
            p_name, ctor,
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const EnumFlagsT& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << static_cast<DataT>(p_self);
                    return oss.str();
                },
                [](const EnumFlagsT& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << static_cast<DataT>(p_self) << p_str;
                    return oss.str();
                }
            ),
            sol::meta_function::bitwise_and, sol::overload(
                sol::resolve<EnumFlagsT(EnumT) const>(&EnumFlagsT::operator&),
                sol::resolve<EnumFlagsT(EnumFlagsT) const>(&EnumFlagsT::operator&)
            ),
            sol::meta_function::bitwise_or, sol::overload(
                sol::resolve<EnumFlagsT(EnumT) const>(&EnumFlagsT::operator|),
                sol::resolve<EnumFlagsT(EnumFlagsT) const>(&EnumFlagsT::operator|)
            ),
            sol::meta_function::bitwise_xor, sol::overload(
                sol::resolve<EnumFlagsT(EnumT) const>(&EnumFlagsT::operator^),
                sol::resolve<EnumFlagsT(EnumFlagsT) const>(&EnumFlagsT::operator^)
            ),
            sol::meta_function::bitwise_not, &EnumFlagsT::operator~,
            "raw", sol::property(
                &EnumFlagsT::operator DataT,
                [](EnumFlagsT& p_self, const DataT p_value)
                {
                    return p_self = p_value;
                }
            ),
            "Set", &EnumFlagsT::Set,
            "IsSet", &EnumFlagsT::IsSet
        );

        userType["__type"]["name"] = p_name;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<EnumFlagsT>(p_name);
        return (void)typeInfo;
    }
}

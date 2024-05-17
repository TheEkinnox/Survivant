#include "SurvivantScripting/Bindings/LuaUtilityBinder.h"

#include <SurvivantCore/Debug/Logger.h>
#include <SurvivantCore/Utility/Timer.h>

#include <sol/state.hpp>

using namespace SvCore::Debug;
using namespace SvCore::Utility;

namespace SvScripting::Bindings
{
    void LuaUtilityBinder::Bind(sol::state& p_luaState)
    {
        BindLogger(p_luaState);
        BindTimer(p_luaState);
    }

    void LuaUtilityBinder::BindLogger(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Debug";

        sol::usertype logType = p_luaState.new_usertype<Logger>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "Log", sol::overload(
                [&p_luaState](const sol::object& p_object)
                {
                    Logger::GetInstance().Print("%s\n", ELogType::DEBUG_LOG, p_luaState["tostring"](p_object).get<const char*>());
                },
                [](const std::string& p_message)
                {
                    Logger::GetInstance().Print("%s\n", ELogType::DEBUG_LOG, p_message.c_str());
                }
            ),
            "LogWarning", sol::overload(
                [&p_luaState](const sol::object& p_object)
                {
                    Logger::GetInstance().Print("%s\n", ELogType::WARNING_LOG, p_luaState["tostring"](p_object).get<const char*>());
                },
                [](const std::string& p_message)
                {
                    Logger::GetInstance().Print("%s\n", ELogType::WARNING_LOG, p_message.c_str());
                }
            ),
            "LogError", sol::overload(
                [&p_luaState](const sol::object& p_object)
                {
                    Logger::GetInstance().Print("%s\n", ELogType::ERROR_LOG, p_luaState["tostring"](p_object).get<const char*>());
                },
                [](const std::string& p_message)
                {
                    Logger::GetInstance().Print("%s\n", ELogType::ERROR_LOG, p_message.c_str());
                }
            )
        );

        p_luaState.globals().set_function("print",
            sol::overload(
                [&p_luaState](const sol::object& p_object)
                {
                    Logger::GetInstance().Print("%s\n", ELogType::DEFAULT_LOG, p_luaState["tostring"](p_object).get<const char*>());
                },
                [](const std::string& p_message)
                {
                    Logger::GetInstance().Print("%s\n", ELogType::DEFAULT_LOG, p_message.c_str());
                }
            )
        );
    }

    void LuaUtilityBinder::BindTimer(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Time";

        sol::usertype logType = p_luaState.new_usertype<Timer>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "time", sol::readonly_property(&Timer::GetTime),
            "deltaTime", sol::readonly_property(&Timer::GetDeltaTime),
            "unscaledTime", sol::readonly_property(&Timer::GetUnscaledTime),
            "unscaledDeltaTime", sol::readonly_property(&Timer::GetUnscaledDeltaTime),
            "timeScale", sol::property(&Timer::GetTimeScale, &Timer::SetTimeScale),
            "frameCount", sol::readonly_property(&Timer::GetFrameCount)
        );

        p_luaState[typeName] = &Timer::GetInstance();
    }
}

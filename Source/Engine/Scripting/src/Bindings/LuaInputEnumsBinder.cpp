#include "SurvivantScripting/LuaTypeRegistry.h"
#include "SurvivantScripting/Bindings/LuaInputBinder.h"

#include <SurvivantApp/Inputs/InputSpecifiers/EKey.h>
#include <SurvivantApp/Inputs/InputSpecifiers/EMouseButton.h>
#include <SurvivantApp/Inputs/InputSpecifiers/EInputModifier.h>

#include <sol/state.hpp>

using namespace SvApp;

namespace SvScripting::Bindings
{
    void LuaInputBinder::BindKeys(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "EKey";

        p_luaState.new_enum(typeName,
            "UNKNOWN", EKey::UNKNOWN,
            "SPACE", EKey::SPACE,
            "APOSTROPHE", EKey::APOSTROPHE,
            "COMMA", EKey::COMMA,
            "MINUS", EKey::MINUS,
            "PERIOD", EKey::PERIOD,
            "SLASH", EKey::SLASH,
            "ALPHA_0", EKey::KEY_0,
            "ALPHA_1", EKey::KEY_1,
            "ALPHA_2", EKey::KEY_2,
            "ALPHA_3", EKey::KEY_3,
            "ALPHA_4", EKey::KEY_4,
            "ALPHA_5", EKey::KEY_5,
            "ALPHA_6", EKey::KEY_6,
            "ALPHA_7", EKey::KEY_7,
            "ALPHA_8", EKey::KEY_8,
            "ALPHA_9", EKey::KEY_9,
            "SEMICOLON", EKey::SEMICOLON,
            "EQUAL", EKey::EQUAL,
            "A", EKey::A,
            "B", EKey::B,
            "C", EKey::C,
            "D", EKey::D,
            "E", EKey::E,
            "F", EKey::F,
            "G", EKey::G,
            "H", EKey::H,
            "I", EKey::I,
            "J", EKey::J,
            "K", EKey::K,
            "L", EKey::L,
            "M", EKey::M,
            "N", EKey::N,
            "O", EKey::O,
            "P", EKey::P,
            "Q", EKey::Q,
            "R", EKey::R,
            "S", EKey::S,
            "T", EKey::T,
            "U", EKey::U,
            "V", EKey::V,
            "W", EKey::W,
            "X", EKey::X,
            "Y", EKey::Y,
            "Z", EKey::Z,
            "LEFT_BRACKET", EKey::LEFT_BRACKET,
            "BACKSLASH", EKey::BACKSLASH,
            "RIGHT_BRACKET", EKey::RIGHT_BRACKET,
            "GRAVE_ACCENT", EKey::GRAVE_ACCENT,
            "WORLD_1", EKey::WORLD_1,
            "WORLD_2", EKey::WORLD_2,
            "ESCAPE", EKey::ESCAPE,
            "ENTER", EKey::ENTER,
            "TAB", EKey::TAB,
            "BACKSPACE", EKey::BACKSPACE,
            "INSERT", EKey::INSERT,
            "DELETE", EKey::DEL,
            "RIGHT", EKey::RIGHT,
            "LEFT", EKey::LEFT,
            "DOWN", EKey::DOWN,
            "UP", EKey::UP,
            "PAGE_UP", EKey::PAGE_UP,
            "PAGE_DOWN", EKey::PAGE_DOWN,
            "HOME", EKey::HOME,
            "END", EKey::END,
            "CAPS_LOCK", EKey::CAPS_LOCK,
            "SCROLL_LOCK", EKey::SCROLL_LOCK,
            "NUM_LOCK", EKey::NUM_LOCK,
            "PRINT_SCREEN", EKey::PRINT_SCREEN,
            "PAUSE", EKey::PAUSE,
            "F1", EKey::F1,
            "F2", EKey::F2,
            "F3", EKey::F3,
            "F4", EKey::F4,
            "F5", EKey::F5,
            "F6", EKey::F6,
            "F7", EKey::F7,
            "F8", EKey::F8,
            "F9", EKey::F9,
            "F10", EKey::F10,
            "F11", EKey::F11,
            "F12", EKey::F12,
            "F13", EKey::F13,
            "F14", EKey::F14,
            "F15", EKey::F15,
            "F16", EKey::F16,
            "F17", EKey::F17,
            "F18", EKey::F18,
            "F19", EKey::F19,
            "F20", EKey::F20,
            "F21", EKey::F21,
            "F22", EKey::F22,
            "F23", EKey::F23,
            "F24", EKey::F24,
            "F25", EKey::F25,
            "KP_0", EKey::KP_0,
            "KP_1", EKey::KP_1,
            "KP_2", EKey::KP_2,
            "KP_3", EKey::KP_3,
            "KP_4", EKey::KP_4,
            "KP_5", EKey::KP_5,
            "KP_6", EKey::KP_6,
            "KP_7", EKey::KP_7,
            "KP_8", EKey::KP_8,
            "KP_9", EKey::KP_9,
            "KP_DECIMAL", EKey::KP_DECIMAL,
            "KP_DIVIDE", EKey::KP_DIVIDE,
            "KP_MULTIPLY", EKey::KP_MULTIPLY,
            "KP_SUBTRACT", EKey::KP_SUBTRACT,
            "KP_ADD", EKey::KP_ADD,
            "KP_ENTER", EKey::KP_ENTER,
            "KP_EQUAL", EKey::KP_EQUAL,
            "LEFT_SHIFT", EKey::LEFT_SHIFT,
            "LEFT_CONTROL", EKey::LEFT_CONTROL,
            "LEFT_ALT", EKey::LEFT_ALT,
            "LEFT_SUPER", EKey::LEFT_SUPER,
            "RIGHT_SHIFT", EKey::RIGHT_SHIFT,
            "RIGHT_CONTROL", EKey::RIGHT_CONTROL,
            "RIGHT_ALT", EKey::RIGHT_ALT,
            "RIGHT_SUPER", EKey::RIGHT_SUPER,
            "MENU", EKey::MENU
        );

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<EKey>(typeName);
        return (void)typeInfo;
    }

    void LuaInputBinder::BindMouseButtons(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "EMouseButton";

        p_luaState.new_enum(typeName,
            "MOUSE_1", EMouseButton::MOUSE_1,
            "MOUSE_2", EMouseButton::MOUSE_2,
            "MOUSE_3", EMouseButton::MOUSE_3,
            "MOUSE_4", EMouseButton::MOUSE_4,
            "MOUSE_5", EMouseButton::MOUSE_5,
            "MOUSE_6", EMouseButton::MOUSE_6,
            "MOUSE_7", EMouseButton::MOUSE_7,
            "MOUSE_8", EMouseButton::MOUSE_8,
            "LEFT", EMouseButton::LEFT,
            "RIGHT", EMouseButton::RIGHT,
            "MIDDLE", EMouseButton::MIDDLE
        );

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<EMouseButton>(typeName);
        return (void)typeInfo;
    }

    void LuaInputBinder::BindInputModifiers(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "EInputModifier";

        p_luaState.new_enum(typeName,
            "ANY", EInputModifier::MOD_ANY,
            "SHIFT", EInputModifier::MOD_SHIFT,
            "CONTROL", EInputModifier::MOD_CONTROL,
            "ALT", EInputModifier::MOD_ALT,
            "SUPER", EInputModifier::MOD_SUPER,
            "CAPS_LOCK", EInputModifier::MOD_CAPS_LOCK,
            "NUM_LOCK", EInputModifier::MOD_NUM_LOCK
        );

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<EInputModifier>(typeName);
        return (void)typeInfo;
    }
}

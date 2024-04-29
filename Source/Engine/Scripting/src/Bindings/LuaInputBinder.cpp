#include "SurvivantScripting/Bindings/LuaInputBinder.h"

#include "SurvivantScripting/LuaTypeRegistry.h"

#include <SurvivantApp/Inputs/InputManager.h>

#include <Vector/Vector2.h>

#include <sol/state.hpp>

using namespace LibMath;
using namespace SvApp;

namespace SvScripting::Bindings
{
    void LuaInputBinder::Bind(sol::state& p_luaState)
    {
        BindKeys(p_luaState);
        BindMouseButtons(p_luaState);
        BindInputModifiers(p_luaState);
        BindFunctions(p_luaState);
    }

    void LuaInputBinder::BindFunctions(sol::state& p_luaState)
    {
        using KeyType = InputManager::KeyboardKeyType;
        using MouseType = InputManager::MouseKeyType;

        static constexpr const char* typeName = "Input";

        sol::usertype inputType = p_luaState.new_usertype<InputManager>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "mousePos", sol::readonly_property([]()
                -> Vector2
                {
                    TVector2<double> pos;
                    InputManager::GetInstance().GetMousePos(pos.m_x, pos.m_y);
                    return pos;
                }
            ),
            "IsKeyDown", sol::overload(
                [](const EKey p_key)
                {
                    return InputManager::GetInstance().EvaluateInput(
                        KeyType(p_key, EKeyState::PRESSED, EInputModifier::MOD_ANY));
                },
                [](const EKey p_key, const EInputModifier p_modifier)
                {
                    return InputManager::GetInstance().EvaluateInput(KeyType(p_key, EKeyState::PRESSED, p_modifier));
                }
            ),
            "IsKeyUp", sol::overload(
                [](const EKey p_key)
                {
                    return InputManager::GetInstance().EvaluateInput(KeyType(p_key, EKeyState::RELEASED, EInputModifier::MOD_ANY));
                },
                [](const EKey p_key, const EInputModifier p_modifier)
                {
                    return InputManager::GetInstance().EvaluateInput(KeyType(p_key, EKeyState::RELEASED, p_modifier));
                }
            ),
            "IsMouseButtonDown", sol::overload(
                [](const EMouseButton p_btn)
                {
                    return InputManager::GetInstance().EvaluateInput(
                        MouseType(p_btn, EMouseButtonState::PRESSED, EInputModifier::MOD_ANY));
                },
                [](const EMouseButton p_btn, const EInputModifier p_modifier)
                {
                    return InputManager::GetInstance().EvaluateInput(MouseType(p_btn, EMouseButtonState::PRESSED, p_modifier));
                }
            ),
            "IsMouseButtonUp", sol::overload(
                [](const EMouseButton p_btn)
                {
                    return InputManager::GetInstance().EvaluateInput(
                        MouseType(p_btn, EMouseButtonState::RELEASED, EInputModifier::MOD_ANY));
                },
                [](const EMouseButton p_btn, const EInputModifier p_modifier)
                {
                    return InputManager::GetInstance().EvaluateInput(MouseType(p_btn, EMouseButtonState::RELEASED, p_modifier));
                }
            )
        );

        inputType["__type"]["name"] = typeName;
    }
}

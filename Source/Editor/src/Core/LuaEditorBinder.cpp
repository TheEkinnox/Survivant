//LuaEditorBinder.cpp
#include "SurvivantEditor/Core/LuaEditorBinder.h"

#include "SurvivantEditor/Core/EditorEngine.h"

#include "SurvivantScripting/Bindings/LuaAudioBinder.h"

#include <SurvivantApp/Inputs/InputManager.h>

#include <SurvivantScripting/Bindings/LuaECSBinder.h>
#include <SurvivantScripting/Bindings/LuaInputBinder.h>
#include <SurvivantScripting/Bindings/LuaMathBinder.h>
#include <SurvivantScripting/Bindings/LuaPhysicsBinder.h>
#include <SurvivantScripting/Bindings/LuaRenderingBinder.h>
#include <SurvivantScripting/Bindings/LuaResourceBinder.h>
#include <SurvivantScripting/Bindings/LuaUtilityBinder.h>

#include <sol/state.hpp>

using namespace SvScripting::Bindings;
using namespace SvApp;
using namespace LibMath;

namespace SvEditor::Core
{
	void LuaEditorBinder::EditorUserTypeBindings(sol::state& p_luaState)
	{
        LuaECSBinder::Bind(p_luaState);
        LuaMathBinder::Bind(p_luaState);
        EditorImputBinder(p_luaState);                  //replace with editor ImputBinder
        LuaUtilityBinder::Bind(p_luaState);
        LuaRenderingBinder::Bind(p_luaState);
        LuaResourceBinder::Bind(p_luaState);
        LuaPhysicsBinder::Bind(p_luaState);
        LuaAudioBinder::Bind(p_luaState);
	}

	void LuaEditorBinder::EditorImputBinder(sol::state& p_luaState)
	{
		LuaInputBinder::BindKeys(p_luaState);
		LuaInputBinder::BindMouseButtons(p_luaState);
		LuaInputBinder::BindInputModifiers(p_luaState);
        EditorImputBindFunctions(p_luaState);			//replace with editor BindFunctions
	}
        LuaInputBinder::BindCursorModes(p_luaState);

    void LuaEditorBinder::EditorImputBindFunctions(sol::state& p_luaState)
    {
        using KeyType = InputManager::KeyboardKeyType;
        using MouseType = InputManager::MouseKeyType;

        static constexpr const char* typeName = "Input";

        sol::usertype inputType = p_luaState.new_usertype<InputManager>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "mousePos", sol::property(
                &LuaInputBinder::GetMousePos,
                &LuaInputBinder::SetMousePos
            ),
            "mouseDelta", sol::readonly_property(&LuaInputBinder::GetMouseDelta),
            "cursorMode", sol::property(
                &InputManager::GetCursorMode,
                &InputManager::SetCursorMode
            ),
            "IsKeyDown", sol::overload(
                [](const EKey p_key)
                {
                    return  dynamic_cast<EditorEngine*>(SV_ENGINE())->IsGameFocused() &&
                            InputManager::GetInstance().EvaluateInput(
                                KeyType(p_key, EKeyState::PRESSED, EInputModifier::MOD_ANY));
                },
                [](const EKey p_key, const EInputModifier p_modifier)
                {
                    return  dynamic_cast<EditorEngine*>(SV_ENGINE())->IsGameFocused() &&
                            InputManager::GetInstance().EvaluateInput(
                                KeyType(p_key, EKeyState::PRESSED, p_modifier));
                }
            ),
            "IsKeyUp", sol::overload(
                [](const EKey p_key)
                {
                    return  dynamic_cast<EditorEngine*>(SV_ENGINE())->IsGameFocused() &&
                            InputManager::GetInstance().EvaluateInput(
                                KeyType(p_key, EKeyState::RELEASED, EInputModifier::MOD_ANY));
                },
                [](const EKey p_key, const EInputModifier p_modifier)
                {
                    return  dynamic_cast<EditorEngine*>(SV_ENGINE())->IsGameFocused() &&
                            InputManager::GetInstance().EvaluateInput(
                                KeyType(p_key, EKeyState::RELEASED, p_modifier));
                }
            ),
            "IsMouseButtonDown", sol::overload(
                [](const EMouseButton p_btn)
                {
                    return  dynamic_cast<EditorEngine*>(SV_ENGINE())->IsGameFocused() &&
                            InputManager::GetInstance().EvaluateInput(
                                 MouseType(p_btn, EMouseButtonState::PRESSED, EInputModifier::MOD_ANY));
                },
                [](const EMouseButton p_btn, const EInputModifier p_modifier)
                {
                    return  dynamic_cast<EditorEngine*>(SV_ENGINE())->IsGameFocused() &&
                            InputManager::GetInstance().EvaluateInput(
                                MouseType(p_btn, EMouseButtonState::PRESSED, p_modifier));
                }
            ),
            "IsMouseButtonUp", sol::overload(
                [](const EMouseButton p_btn)
                {
                    return  dynamic_cast<EditorEngine*>(SV_ENGINE())->IsGameFocused() &&
                            InputManager::GetInstance().EvaluateInput(
                                MouseType(p_btn, EMouseButtonState::RELEASED, EInputModifier::MOD_ANY));
                },
                [](const EMouseButton p_btn, const EInputModifier p_modifier)
                {
                    return  dynamic_cast<EditorEngine*>(SV_ENGINE())->IsGameFocused() &&
                            InputManager::GetInstance().EvaluateInput(
                                MouseType(p_btn, EMouseButtonState::RELEASED, p_modifier));
                }
            )
        );

        inputType["__type"]["name"] = typeName;
        p_luaState[typeName]        = &InputManager::GetInstance();
    }
}

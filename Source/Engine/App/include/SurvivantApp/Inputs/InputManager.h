//InputManager.h

#pragma once
#include "SurvivantApp/Inputs/InputType.h"
#include "SurvivantApp/Inputs/KeyboardInputs.h"
#include "SurvivantApp/Inputs/MouseInputs.h"
#include "SurvivantApp/Windows/ECursorMode.h"

#include <SurvivantCore/Events/Event.h>
#include <SurvivantCore/Events/EventManager.h>

#include <Vector/Vector2.h>

#include <memory>
#include <string>
#include <tuple>

namespace SvApp
{
	//foward declaration
	class Window;

	class InputManager
	{
	public:
		//events
		using WindowCloseEvent = SvCore::Events::Event<>;

		using KeyboardKeyType = InputType<EKey, EKeyState, EInputModifier>;
		using KeyCallbackParam = char;
		using KeyCallback = std::function<void(KeyCallbackParam)>;
		using MouseKeyType = InputType<EMouseButton, EMouseButtonState, EInputModifier>;
		using MouseCallback = std::function<void(float, float)>;

		struct InputBindings
		{
			std::unordered_map<KeyboardKeyType, KeyCallback>	m_keyCallbacks;
			std::unordered_map<MouseKeyType, MouseCallback>		m_mouseKeyCallbacks;
		};

		InputManager() {}
		InputManager(InputManager const&) = delete;
		void operator=(InputManager const&) = delete;

		static InputManager& GetInstance();
		static std::string KeyBindingToString(const KeyboardKeyType& p_key);
		static std::string KeyNameToString(const EKey& p_name);
		static std::string KeyModifToString(const EInputModifier& p_modif);
		static EKey GetModifKey(const EInputModifier& p_modif);


		void InitWindow(Window* p_window);

		void Update();

		void CallInput(const KeyboardKeyType& p_type, char p_scancode, bool p_callAtUpdate = false);

		void CallInput(const MouseKeyType& p_type, float p_x, float p_y, bool p_callAtUpdate = false);

		void CallInput(const MouseKeyType& p_type, bool p_callAtUpdate = false);

		void SetInputBindings(const std::shared_ptr<InputBindings>& p_bindings);

		template<class Event, typename ...Args>
		KeyCallback CreateInputEventBinding(const KeyboardKeyType& p_type, std::tuple<Args...> (*p_translate)(KeyCallbackParam));

		template<class Event, typename ...Args>
		MouseCallback CreateInputEventBinding(const MouseKeyType& p_type, std::tuple<Args...>(*p_translate)(float, float));

		void GetMousePos(double& p_x, double& p_y) const;
		void SetMousePos(double p_x, double p_y);

		LibMath::Vector2D GetMouseDelta() const;

		ECursorMode GetCursorMode() const;
		void SetCursorMode(ECursorMode p_mode);

		bool EvaluateInput(const KeyboardKeyType& p_key);
		bool EvaluateInput(const MouseKeyType& p_key);


	public:
		//container peripherique
		std::shared_ptr<InputBindings>		m_bindings;

		std::vector<std::function<void()>>	m_updateCallbacks;

		SvApp::Window* m_window = nullptr;

	private:
		LibMath::Vector2D m_lastMousePos;
		LibMath::Vector2D m_mouseDelta;
		bool m_resetDelta = true;
	};

	template<class T, typename ...Args>
	InputManager::KeyCallback InputManager::CreateInputEventBinding(const KeyboardKeyType& /*p_type*/, std::tuple<Args...>(*p_translate)(KeyCallbackParam))
	{
		static_assert(std::is_base_of_v<SvCore::Events::Event<Args...>, T> || std::is_same_v<SvCore::Events::Event<Args...>, T>);

		//needs to capture a copy of translate ptr
		KeyCallback callback =
			[p_translate](KeyCallbackParam p_1)
			{
				SvCore::Events::EventManager::GetInstance().Invoke<T>(p_translate(p_1));
			};

		return callback;
	}

	template<class T, typename ...Args>
	InputManager::MouseCallback InputManager::CreateInputEventBinding(const MouseKeyType& /*p_type*/, std::tuple<Args...>(*p_translate)(float, float))
	{
		static_assert(std::is_base_of_v<SvCore::Events::Event<Args...>, T> || std::is_same_v<SvCore::Events::Event<Args...>, T>);

		//needs to capture a copy of translate ptr
		MouseCallback callback =
			[p_translate](float p_1, float p_2)
			{
				SvCore::Events::EventManager::GetInstance().Invoke<T>(p_translate(p_1, p_2));
			};

		return callback;
	}

	//glfwSetKeyCallback(window, key_callback);

	//GLFW input callbacks

	//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	//{
	//	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	//		activate_airship();
	//}

}



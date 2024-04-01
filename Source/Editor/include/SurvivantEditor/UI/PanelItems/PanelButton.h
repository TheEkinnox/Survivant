//PanelButton.h
#pragma once

#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/UI/Interfaces/IPanelable.h"

#include <functional>

namespace SvEditor::UI::PanelItems
{
	class PanelButton : public Interfaces::IPanelable
	{
	public:
		class OnButtonPressEvent : public SvCore::Events::Event<> {};

		PanelButton(const std::string& p_name, const OnButtonPressEvent::EventDelegate& p_callback = nullptr) :
			m_name(p_name)
		{
			if (p_callback != nullptr)
				m_callback.AddListener(p_callback);
		}

		PanelButton(const std::string& p_name, const OnButtonPressEvent& p_event) :
			m_name(p_name),
			m_callback(p_event)
		{}

		~PanelButton() = default;

		void DisplayAndUpdatePanel()override;

		void AddCallback(const OnButtonPressEvent::EventDelegate& p_callback)
		{
			m_callback.AddListener(p_callback);
		}

	private:
		std::string	m_name;
		OnButtonPressEvent m_callback;
	};
}
//PanelTextInput.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"

#include <functional>

namespace SvUI::PanelItems
{
	class PanelTextInput : public Interfaces::IPanelable
	{
	public:
		using Callback = std::function<void(PanelTextInput&)>;

		PanelTextInput(
			std::string p_name, 
			std::string& p_buffer, 
			const Callback& p_callback = Callback());
		~PanelTextInput() = default;

		void DisplayAndUpdatePanel()override;
		void Clear();
		const std::string& GetText()const;

		static void ClearPanelText(PanelTextInput& p_panel);

	private:
		//static int TextPanelCallBack(ImGuiInputTextCallbackData* data);

		std::string		m_name;
		std::string&	m_buffer;
		Callback		m_callback;
	};
}

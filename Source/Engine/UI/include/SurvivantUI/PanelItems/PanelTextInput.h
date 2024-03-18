//PanelTextInput.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"

#include <functional>

namespace SvUI::PanelItems
{
	class PanelTextInput : public Interfaces::IPanelable
	{
	public:
		PanelTextInput(std::string p_name, std::function<void(PanelTextInput&)> p_callback);
		~PanelTextInput() = default;

		void DisplayAndUpdatePanel()override;
		void Clear();
		const std::string& GetText()const;

		static void ClearPanelText(PanelTextInput& p_panel);

	private:
		//static int TextPanelCallBack(ImGuiInputTextCallbackData* data);

		std::string								m_name;
		std::string								m_buffer;
		std::function<void(PanelTextInput&)>	m_callback;
	};
}

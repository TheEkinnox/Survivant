//PanelTextInput.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include <functional>
#include <string>

namespace SvEditor::PanelItems
{
	class PanelTextInput : public PanelInputBase<std::string, PanelTextInput*>
	{
	public:
		PanelTextInput(
			const std::string& p_name,
			std::string& p_buffer,
			const Callback& p_callback = Callback());
		PanelTextInput(
			const std::string& p_name,
			const GetRefFunc& p_getRef,
			const Callback& p_callback = Callback());
		PanelTextInput(
			const std::string& p_name,
			const GetCopyFunc& p_getCopy,
			const Callback& p_callback);
		~PanelTextInput() override = default;

		void DisplayAndUpdatePanel()override;
		void Clear();
		const std::string& GetText()const;
		const std::string& GetUpdatedText() const;

		static void ClearPanelText(PanelTextInput& p_panel);

	private:
		//static int TextPanelCallBack(ImGuiInputTextCallbackData* data);

		std::string		m_name;
		std::string		m_string;
	};
}

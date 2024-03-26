//TestPanel.h
#pragma once

#include "SurvivantEditor/UI/Panels/Panel.h"

#include <functional>

namespace SvEditor::UI::Panels
{
	class TestPanel : public Panel
	{
	public:
		TestPanel(const std::string& p_name);
		~TestPanel() = default;

		ERenderFlags Render() override;

		//PanelUniqueSelection								m_unique;
		//PanelMultipleSelection							m_multiple;
	};
}
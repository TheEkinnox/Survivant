//TestPanel.h
#pragma once

#include "SurvivantEditor/Panels/Panel.h"

#include <functional>

namespace SvEditor::Panels
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
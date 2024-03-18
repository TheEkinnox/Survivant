//TestPanel.h
#pragma once

#include "SurvivantUI/Panels/Panel.h"

#include <functional>

namespace SvUI::Panels
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
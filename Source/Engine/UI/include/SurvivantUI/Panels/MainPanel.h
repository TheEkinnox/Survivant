//MainPanel.h
#pragma once

#include "SurvivantUI/Panels/Panel.h"
#include "SurvivantUI/MenuItems/MenuBar.h"

#include <functional>


namespace SvUI::Panels
{
	using namespace MenuItems;

	class MainPanel : public Panel
	{
	public:
		using ChangeLayout = std::function<void(int)>;

		MainPanel();
		MainPanel(MenuBar&& p_menuBar);
		~MainPanel() = default;

		ERenderFlags Render() override;

		void SetMenuBar(MenuBar&& p_menuBar);
		void ChangePanelLayout(const ChangeLayout& p_layout);

	private:
		void SetupLayout(int p_dockspaceId);

		static constexpr char NAME[] = "Main";

		ChangeLayout		m_layout;
		MenuBar				m_menuBar;
		Panel::ERenderFlags m_panelFlags;
	};
}
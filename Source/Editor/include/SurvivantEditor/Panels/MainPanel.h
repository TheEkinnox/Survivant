//MainPanel.h
#pragma once

#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/MenuItems/MenuBar.h"

#include <functional>


namespace SvEditor::Panels
{
	using namespace MenuItems;

	class MainPanel : public Panel
	{
	public:
		using ChangeLayout = std::function<void(int)>;

		MainPanel();
		MainPanel(MenuBar&& p_menuBar);
		~MainPanel() = default;

		void RenderDockSpace();
		ERenderFlags Render() override;

		void SetMenuBar(MenuBar&& p_menuBar);
		void ChangePanelLayout(const ChangeLayout& p_layout);

		void ForceFocus(const std::string& p_name);

		static constexpr char NAME[] = "Main";

	private:
		void SetupLayout(int p_dockspaceId);
		void SetForceFocust();

		MenuBar				m_menuBar;
		Panel::ERenderFlags m_panelFlags;

		std::string			m_forceFocus;
		ChangeLayout		m_layout;
	};
}
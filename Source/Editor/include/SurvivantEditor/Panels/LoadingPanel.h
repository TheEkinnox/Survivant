//LoadingPanel.h
#pragma once

#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelButtonList.h"

#include <functional>


namespace SvEditor::Panels
{
	using namespace PanelItems;

	class LoadingPanel : public Panel
	{
	public:
		LoadingPanel();
		~LoadingPanel() override = default;

		ERenderFlags Render() override;

		static constexpr char NAME[] = "Loading";
	private:
		bool			m_open = true;
	};
}

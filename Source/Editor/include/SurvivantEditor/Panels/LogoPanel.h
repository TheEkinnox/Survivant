//LogoPanel.h
#pragma once

#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelImage.h"

#include <functional>


namespace SvEditor::Panels
{
	class LogoPanel : public Panel
	{
	public:
		LogoPanel() = default;
		~LogoPanel() override = default;

		ERenderFlags Render() override;

		static constexpr char NAME[] = "Logo";

	private:
		PanelItems::PanelImage m_image;
	};
}
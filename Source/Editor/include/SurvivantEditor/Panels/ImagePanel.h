//ImagePanel.h
#pragma once

#include "SurvivantEditor/Panels/Panel.h"

#include <functional>


namespace SvEditor::Panels
{
	class ImagePanel : public Panel
	{
	public:
		ImagePanel(const std::string& p_name, intptr_t p_textureId) : Panel(p_name), m_textureId(p_textureId) {}
		~ImagePanel() = default;

		ERenderFlags Render() override;

	private:
		intptr_t m_textureId;
	};
}
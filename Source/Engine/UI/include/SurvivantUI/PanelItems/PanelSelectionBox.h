//PanelSelectionBox.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"
#include "SurvivantUI/Interfaces/ISelectable.h"

#include <functional>
#include <set>
#include <memory>

namespace SvUI::PanelItems
{
	using namespace Interfaces;

	class PanelSelectionBox : public Interfaces::IPanelable
	{
	public:
		PanelSelectionBox(float p_width = 80, float p_height = 120) : 
			m_width(p_width), m_height(p_height) {}
		~PanelSelectionBox() = default;

		void DisplayAndUpdatePanel() override;

		void SetSelectionSize(float p_width, float p_height);
		void SetSelectionBoxable(const std::vector<std::shared_ptr<ISelectable>>& p_selectionBoxable);

		static void DisplayCenteredText(const std::string& p_text, float p_maxWidth);
	private:
		bool DisplaySelectable(ISelectable* p_item, bool p_doubleClicked);

		std::vector<std::shared_ptr<ISelectable>>	m_elements;
		std::set<ISelectable*>						m_currentSelection;
		float										m_width;
		float										m_height;
	};
}
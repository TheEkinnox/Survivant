//PanelSelectionBox.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/Interfaces/ISelectable.h"

#include <functional>
#include <map>
#include <memory>
#include <set>

namespace SvEditor::PanelItems
{
	using namespace Interfaces;

	class PanelSelectionBox : public Interfaces::IPanelable
	{
	public:
		using SelectableMap = std::map<ISelectable::SelectablePrioKey, std::shared_ptr<ISelectable>>;

		PanelSelectionBox(float p_width = 80, float p_height = 120) : 
			m_width(p_width), m_height(p_height) {}
		~PanelSelectionBox() = default;

		void DisplayAndUpdatePanel() override;

		void SetSelectionSize(float p_width, float p_height);
		void SetSelectionBoxable(const SelectableMap& p_selectionBoxable);

		static void DisplayCenteredText(const std::string& p_text, float p_maxWidth);
	private:
		void DisplaySelectable(ISelectable* p_item);

		SelectableMap				m_elements;
		std::set<ISelectable*>	m_currentSelection;
		float						m_width;
		float						m_height;
	};
}
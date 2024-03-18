//PanelSelectionBox.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"

#include <functional>
#include <set>
#include <memory>

namespace SvUI::PanelItems
{
	class PanelSelectionBox : public Interfaces::IPanelable
	{
	public:
		class ISelectionBoxable
		{
		public:
			virtual ~ISelectionBoxable() = default;

			/// <summary>
			/// 
			/// </summary>
			/// <param name="p_width"></param>
			/// <param name="p_height"></param>
			/// <param name="p_doubleClicked"></param>
			/// <returns>true if need to stop displaying other SelectionBoxable</returns>
			virtual bool DisplayAndUpdateSelection(float& p_width, float& p_height, bool p_doubleClicked) = 0;
			virtual const std::string& GetName() = 0;
		};

		PanelSelectionBox() = default;
		~PanelSelectionBox() = default;

		void DisplayAndUpdatePanel() override;

		void SetSelectionSize(float p_width, float p_height);
		void SetSelectionBoxable(const std::vector<std::shared_ptr<ISelectionBoxable>>& p_selectionBoxable);

		static void DisplayCenteredText(const std::string& p_text, float p_maxWidth);
	private:
		std::vector<std::shared_ptr<ISelectionBoxable>>		m_elements;
		std::set<ISelectionBoxable*>						m_currentSelection;
		float												m_width = 80;
		float												m_height = 120;
	};
}
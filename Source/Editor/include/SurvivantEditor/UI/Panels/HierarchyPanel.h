//HierarchyPanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantEditor/UI/Panels/Panel.h"
#include "SurvivantEditor/UI/PanelItems/PanelTreeBranch.h"


namespace SvEditor::UI::Panels
{
	using namespace PanelItems;

	class HierarchyPanel : public Panel
	{
	public:
		HierarchyPanel();
		~HierarchyPanel();

		// Inherited via Panel
		ERenderFlags Render() override;

		static constexpr char NAME[] = "Hierarchy";
	private:
		void SetupTree();
		
		PanelTreeBranch m_tree;
	};
}
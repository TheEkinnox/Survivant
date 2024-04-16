//HierarchyPanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelTreeBranch.h"


namespace SvEditor::Panels
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
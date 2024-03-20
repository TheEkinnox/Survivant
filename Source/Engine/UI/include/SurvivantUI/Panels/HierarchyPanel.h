//HierarchyPanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantUI/Panels/Panel.h"
#include "SurvivantUI/PanelItems/PanelTreeBranch.h"


namespace SvUI::Panels
{
	using namespace PanelItems;

	class HierarchyPanel : public Panel
	{
	public:
		HierarchyPanel();
		~HierarchyPanel();

		// Inherited via Panel
		ERenderFlags Render() override;


	private:
		void SetupTree();

		static constexpr char NAME[] = "Hierarchy";

		static inline SvCore::Utility::UnusedIdGenerator s_idGenerator;
		
		PanelTreeBranch m_tree;
	};
}
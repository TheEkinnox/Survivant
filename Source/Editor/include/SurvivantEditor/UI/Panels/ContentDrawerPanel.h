//ContentDrawerPanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantEditor/UI/Interfaces/IPanelable.h"
#include "SurvivantEditor/UI/Panels/Panel.h"
#include "SurvivantEditor/UI/PanelItems/PanelSelectionBox.h"
#include "SurvivantEditor/UI/PanelItems/PanelTreeBranch.h"

#include <filesystem>
#include <functional>


namespace SvEditor::UI::Panels
{
	using namespace PanelItems;

	class ContentDrawerPanel : public Panel
	{
	public:
		ContentDrawerPanel();
		~ContentDrawerPanel();

		//Panel
		ERenderFlags Render() override;

		bool SetGridDisplay(PanelTreeBranch& p_branch);
		bool TryOpenFile(PanelTreeBranch& p_branch);

		static constexpr char NAME[] = "ContentDrawer";

	private:
		void SetupTree();
		void SetupBranches(std::shared_ptr<PanelTreeBranch> p_parent, const std::filesystem::path& p_filePath);

		static constexpr char DIRECTORY_PATH[] = "Source";
		static constexpr char BACKSLASH[] = "/";


		std::shared_ptr<PanelTreeBranch>					m_tree;
		PanelSelectionBox									m_grid;
	};
}
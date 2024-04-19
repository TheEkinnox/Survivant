//ContentDrawerPanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantCore/Resources/IResource.h"
#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelSelectionBox.h"
#include "SurvivantEditor/PanelItems/PanelTreeBranch.h"

#include <filesystem>
#include <functional>
#include <string>


namespace SvEditor::Panels
{
	using namespace PanelItems;

	class ContentDrawerPanel : public Panel
	{
	public:
		using ResourceBranch = PanelTreeBranch<std::string>;

		ContentDrawerPanel();
		~ContentDrawerPanel();

		//Panel
		ERenderFlags Render() override;

		bool SetGridDisplay(ResourceBranch& p_branch);
		bool TryOpenFile(ResourceBranch& p_branch);

		static constexpr char NAME[] = "ContentDrawer";

	private:
		void SetupTree();
		void SetupBranches(std::shared_ptr<ResourceBranch> p_parent, const std::filesystem::path& p_filePath);

		static constexpr char DIRECTORY_PATH[] = "Source";
		static constexpr char BACKSLASH[] = "/";


		std::shared_ptr<ResourceBranch>	m_tree;
		PanelSelectionBox				m_grid;
	};
}
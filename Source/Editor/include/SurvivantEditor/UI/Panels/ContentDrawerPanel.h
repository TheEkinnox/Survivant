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

		static size_t GetPanelCount() { return s_idGenerator.GetNumUsedIds(); };

	private:
		void SetupTree();
		void SetupBranches(std::shared_ptr<PanelTreeBranch> p_parent, const std::filesystem::path& p_filePath);

		static constexpr char NAME[] = "ContentDrawer";
		static constexpr char DIRECTORY_PATH[] = "Source";
		static constexpr char BACKSLASH[] = "/";

		static inline SvCore::Utility::UnusedIdGenerator s_idGenerator;

		std::shared_ptr<PanelTreeBranch>					m_tree;
		PanelSelectionBox									m_grid;
	};
}
//ContentDrawerPanel.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelButtonList.h"
#include "SurvivantEditor/PanelItems/PanelSelectionBox.h"
#include "SurvivantEditor/PanelItems/PanelTreeBranch.h"

#include <SurvivantCore/Utility/UnusedIdGenerator.h>
#include <SurvivantCore/Resources/IResource.h>
#include <SurvivantCore/Resources/ResourceRef.h>
#include <SurvivantCore/Utility/TypeRegistry.h>

#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <set>


namespace SvEditor::Panels
{
	using namespace PanelItems;

	class ContentDrawerPanel : public Panel
	{
	public:
		using ResourceBranch = PanelTreeBranch<std::string>;
		using TypeToBranch = std::unordered_map<std::string, std::vector<std::weak_ptr<ResourceBranch>>>;

		ContentDrawerPanel();
		~ContentDrawerPanel() override = default;

		//Panel
		ERenderFlags Render() override;

		bool SetGridDisplay(ResourceBranch& p_branch);
		bool TryOpenFile(ResourceBranch& p_branch);

		static constexpr char NAME[] = "ContentDrawer";

		static const TypeToBranch& GetAllFilePaths();
		static std::vector<std::string> GetAllFilePaths(const std::string& p_type);

	private:
		using TypeExtensions = std::unordered_map<std::string, std::set<std::string>>;

		static constexpr char DIRECTORY_PATH[] = "assets";

		static TypeExtensions CreateExtensions();
		static inline TypeExtensions	s_fileExtensions;
		static inline TypeToBranch		s_typedFiles;

		static std::string GetType(const std::filesystem::path& p_filePath);
		//static std::string FormatPath(const std::filesystem::path& p_filePath);

		void SetupTree();
		void SetupBranches(std::shared_ptr<ResourceBranch> p_parent, const std::filesystem::path& p_filePath);

		std::shared_ptr<ResourceBranch>		m_tree;
		PanelSelectionBox					m_grid;
		PanelButtonList						m_buttonList;
	};
}
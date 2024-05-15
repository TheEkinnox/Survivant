//ContentDrawerPanel.h
#pragma once

#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantCore/Resources/IResource.h"
#include "SurvivantCore/Resources/ResourceRef.h"
#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelSelectionBox.h"
#include "SurvivantEditor/PanelItems/PanelTreeBranch.h"
#include "SurvivantCore/Utility/TypeRegistry.h"

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

		ContentDrawerPanel();
		~ContentDrawerPanel();

		//Panel
		ERenderFlags Render() override;

		bool SetGridDisplay(ResourceBranch& p_branch);
		bool TryOpenFile(ResourceBranch& p_branch);

		static constexpr char NAME[] = "ContentDrawer";

	private:
		using TypedStrings = std::unordered_map<std::string, std::set<std::string>>;

		void SetupTree();
		void SetupBranches(std::shared_ptr<ResourceBranch> p_parent, const std::filesystem::path& p_filePath);

		static constexpr char DIRECTORY_PATH[] = "assets";
		static inline const std::string DOUBLE_SLASH = "\\";
		static inline const std::string SLASH = "/";

		static TypedStrings CreateExtensions();
		static inline TypedStrings s_fileExtensions;
		static inline TypedStrings s_existingPaths;

		static std::string GetType(const std::filesystem::path& p_filePath);
		static SvCore::Resources::GenericResourceRef GetResourceRef(
			const std::string& p_type,
			const std::filesystem::path& p_filePath);

		std::shared_ptr<ResourceBranch>		m_tree;
		PanelSelectionBox					m_grid;
	};
}
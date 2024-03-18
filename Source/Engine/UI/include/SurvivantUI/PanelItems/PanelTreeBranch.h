//PanelTreeBranch.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"
#include "SurvivantUI/PanelItems/PanelSelectionBox.h"

#include <functional>

namespace SvUI::PanelItems
{
	class PanelTreeBranch : public Interfaces::IPanelable, public PanelSelectionBox::ISelectionBoxable
	{
	public:
		using BranchCallback = std::function<bool(PanelTreeBranch&)>;
		using Childreen = std::unordered_map<std::string, std::shared_ptr<PanelTreeBranch>>;

		PanelTreeBranch(const std::string& p_name, bool p_hideLeafs = true);
		PanelTreeBranch(const std::string& p_name, const Childreen& p_branches, bool p_hideLeafs = true);
		~PanelTreeBranch() = default;

		//IPanelable
		void DisplayAndUpdatePanel() override;

		//ISelectionBoxable
		bool				DisplayAndUpdateSelection(float& p_width, float& p_height, bool p_doubleClicked) override;
		const std::string& GetName() override;

		void DisplayTreePanel();

		bool				IsBranch()const;
		const Childreen& GetChildreen()const;
		std::string			GetPathName()const;

		Childreen& SetBranches(const Childreen& p_branches);
		Childreen& SetBranches(const std::set<std::shared_ptr<PanelTreeBranch>>& p_branches);
		void		AddBranch(const std::shared_ptr<PanelTreeBranch>& p_branch);
		void		RemoveBranch(const std::string& p_name);
		void		ForceOpenParents(bool p_openSelf = false);
		void		ForceCloseChildreen(bool p_closeSelf = false);
		void		ForceOpenAll();

		void		SetOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback);
		void		SetAllOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback);
		void		SetAllBranchesOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback);
		void		SetAllLeavesOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback);

	private:
		enum class EForceState
		{
			NOTHING,
			FORCE_OPEN,
			FORCE_CLOSE,
		};

		bool								m_hideLeafs;
		std::string							m_name;
		PanelTreeBranch* m_parent;
		Childreen							m_childreen;
		EForceState							m_forceState;
		std::shared_ptr<BranchCallback>		m_callback;
	};
}
//PanelTreeBranch.h
#pragma once

#include "SurvivantEditor/UI/Interfaces/IPanelable.h"
#include "SurvivantEditor/UI/Interfaces/ISelectable.h"
#include "SurvivantEditor/UI/MenuItems/PopupMenu.h"

#include <functional>
#include <map>
#include <set>
#include <memory>

namespace SvEditor::UI::PanelItems
{
	using namespace Interfaces;

	class PanelTreeBranch : public IPanelable, public ISelectable
	{
	public:
		using BranchCallback = std::function<bool(PanelTreeBranch&)>;
		using PriorityFunc = std::function<size_t(const PanelTreeBranch&)>;
		using Childreen = std::map<ISelectable::SelectablePrioKey, std::shared_ptr<PanelTreeBranch>>;
		
		PanelTreeBranch(const std::string& p_name, bool p_hideLeafs = true);
		PanelTreeBranch(const std::string& p_name, const Childreen& p_branches, bool p_hideLeafs = true);
		~PanelTreeBranch();

		// Inherited via IPanelable
		void DisplayAndUpdatePanel() override;

		// Inherited via ISelectable
		const std::string&	GetIcon() override;
		const std::string&	GetName() override;
		bool				InvokeDoubleClick() override;
		void				DisplayAndUpdatePopupMenu() override;
		bool				GetSelectedState() override;
		void				SetSelectedState(bool p_isSelected)override;

		bool				IsBranch()const;
		const Childreen&	GetChildreen()const;
		std::string			GetPathName()const;

		Childreen&	SetBranches(const Childreen& p_branches);
		Childreen&	SetBranches(const std::vector<std::shared_ptr<PanelTreeBranch>>& p_branches);
		void		AddBranch(const std::shared_ptr<PanelTreeBranch>& p_branch, const PriorityFunc& p_prioFunc = &NoPriority);
		void		RemoveBranch(const std::string& p_name);
		void		ForceOpenParents(bool p_openSelf = false);
		void		ForceCloseChildreen(bool p_closeSelf = false);
		void		ForceOpenAll();

		void		SetOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback);
		void		SetAllOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback);
		void		SetAllBranchesOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback);
		void		SetAllLeavesOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback);

		void		SetAllPriority(const PriorityFunc& p_prioFunc);
		void		SetAllBranchesPriority(const PriorityFunc& p_prioFunc);
		void		SetAllLeavesPriority(const PriorityFunc& p_prioFunc);
		void		SetAllBranchesPriority(size_t p_prio);
		void		SetAllLeavesPriority(size_t p_prio);

		static size_t	NoPriority(const PanelTreeBranch& p_branch);
		static size_t	HasChildreenPriority(const PanelTreeBranch& p_branch);

	private:
		std::vector<std::unique_ptr<IMenuable>>		GetPopupMenuItems();
		void										DisplayTreePanel();

		enum class EForceState
		{
			NOTHING,
			FORCE_OPEN,
			FORCE_CLOSE,
		};

		bool								m_hideLeafs;
		std::string							m_name;
		PanelTreeBranch*					m_parent;
		Childreen							m_childreen;
		EForceState							m_forceState;
		std::shared_ptr<BranchCallback>		m_callback;
		MenuItems::PopupMenu				m_popup;
		bool								m_isSelected;
	};
}
//PanelTreeBranch.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/Interfaces/ISelectable.h"
#include "SurvivantEditor/MenuItems/PopupMenu.h"

#include <functional>
#include <map>
#include <set>
#include <memory>

namespace SvEditor::PanelItems
{
	using namespace Interfaces;

	template<typename T>
	class PanelTreeBranch : public ISelectable
	{
	public:
		using BranchCallback = std::function<bool(PanelTreeBranch&)>;
		using PriorityFunc = std::function<size_t(const PanelTreeBranch&)>;
		using Children = std::map<ISelectable::SelectablePrioKey, std::shared_ptr<PanelTreeBranch>>;
		
		PanelTreeBranch(
			const std::string& p_name,
			bool p_hideLeafs = true,
			const T& p_value = T());

		PanelTreeBranch(
			const std::string& p_name, 
			const Children& p_branches, 
			bool p_hideLeafs = true,
			const T& p_value = T());
		~PanelTreeBranch();

		// Inherited via IPanelable
		void DisplayAndUpdatePanel() override;

		// Inherited via ISelectable
		const std::string&	GetIcon() override;
		const std::string&	GetName() override;
		bool				Open() override;
		bool				Select() override;
		void				DisplayAndUpdatePopupMenu() override;
		bool				GetSelectedState() override;

		bool				IsBranch()const;
		const Children&		GetChildren()const;
		PanelTreeBranch*	GetParent()const;
		std::string			GetPathName()const;
		const T&			SetValue(const T& p_value)const;
		const T&			GetValue()const;

		Children&	SetBranches(const Children& p_branches);
		Children&	SetBranches(
			const std::vector<std::shared_ptr<PanelTreeBranch>>& p_branches = 
			std::vector<std::shared_ptr<PanelTreeBranch>>());
		void			AddBranch(const std::shared_ptr<PanelTreeBranch>& p_branch, size_t p_prio);
		void			AddBranch(const std::shared_ptr<PanelTreeBranch>& p_branch, const PriorityFunc& p_prioFunc = nullptr);
		void			RemoveBranch(const std::string& p_name);
		void			DeleteBranch(std::set<T>* p_deletedElements = nullptr);
		void			ForceOpenParents(bool p_openSelf = false);
		void			ForceCloseChildreen(bool p_closeSelf = false);
		void			ForceOpenAll();

		void			SetAllPriority(const PriorityFunc& p_prioFunc);
		//void			NoPriority(const PriorityFunc& p_prioFunc);

		static size_t	HasChildreenPriority(const PanelTreeBranch& p_branch);

		static inline BranchCallback s_branchesOnSelect = nullptr;
		static inline BranchCallback s_leavesOnSelect = nullptr;
		static inline BranchCallback s_branchesOnOpen = nullptr;
		static inline BranchCallback s_leavesOnOpen = nullptr;

	private:
		std::vector<std::unique_ptr<IMenuable>>		GetPopupMenuItems();
		void										DisplayTreePanel();

		// Inherited via ISelectable
		void				SetSelectedState(bool p_isSelected)override;


		enum class EForceState
		{
			NOTHING,
			FORCE_OPEN,
			FORCE_CLOSE,
		};

		bool					m_hideLeafs;
		std::string				m_name;
		PanelTreeBranch*		m_parent;
		Children				m_children;
		T						m_value;
		EForceState				m_forceState;
		MenuItems::PopupMenu	m_popup;
		bool					m_isSelected;
	};
}

#include "SurvivantEditor/PanelItems/PanelTreeBranch.inl"

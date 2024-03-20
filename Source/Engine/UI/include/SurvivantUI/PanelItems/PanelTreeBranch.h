//PanelTreeBranch.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"
#include "SurvivantUI/Interfaces/ISelectable.h"
#include "SurvivantUI/MenuItems/PopupMenu.h"

#include <functional>
#include <set>
#include <memory>

namespace SvUI::PanelItems
{
	using namespace Interfaces;

	class PanelTreeBranch : public IPanelable, public ISelectable
	{
	public:
		using BranchCallback = std::function<bool(PanelTreeBranch&)>;
		using Childreen = std::unordered_map<std::string, std::shared_ptr<PanelTreeBranch>>;

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
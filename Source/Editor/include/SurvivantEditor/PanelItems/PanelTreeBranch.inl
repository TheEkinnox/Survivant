//PanelTreeBranch.cpp
#pragma once

#include "SurvivantEditor/PanelItems/PanelTreeBranch.h"

#include "SurvivantEditor/Core/IUI.h"
#include "SurvivantEditor/MenuItems/MenuButton.h"

#include <imgui.h>

namespace SvEditor::PanelItems
{
    using namespace MenuItems;

    template<typename T>
    inline PanelTreeBranch<T>::PanelTreeBranch(
        const std::string& p_name, bool p_hideLeafs, const T& p_value) :
        PanelTreeBranch(p_name, Children(), p_hideLeafs, p_value)
    {
    }

    template <typename T>
    PanelTreeBranch<T>::PanelTreeBranch(
            const std::string& p_name,
            const Children & p_branches,
            bool p_hideLeafs,
            const T& p_value) :
        m_name(p_name),
        m_forceState(EForceState::NOTHING),
        m_hideLeafs(p_hideLeafs),
        m_parent(nullptr),
        m_isSelected(false),
        m_value(p_value)
    {
        m_popup.m_items = GetPopupMenuItems();
        SetBranches(p_branches);
    }

    template <typename T>
    PanelTreeBranch<T>::~PanelTreeBranch()
    {
        if (m_isSelected)
            ClearSelection();
    }

    template <typename T>
    inline PanelTreeBranch<T>::Children& PanelTreeBranch<T>::SetBranches(const Children& p_branches)
    {
        m_children = p_branches;

        for (auto& child : p_branches)
            child.second->m_parent = this;

        return m_children;
    }

    template <typename T>
    inline PanelTreeBranch<T>::Children& PanelTreeBranch<T>::SetBranches(const std::vector<std::shared_ptr<PanelTreeBranch>>& p_branches)
    {
        m_children.clear();
        for (auto& child : p_branches)
        {
            child->m_parent = this;
            m_children.insert({ { child.get()->GetName(), 0 }, child });
        }

        return m_children;
    }

    template <typename T>
    inline void PanelTreeBranch<T>::AddBranch(const std::shared_ptr<PanelTreeBranch>& p_branch, const PriorityFunc& p_prio)
    {
        return AddBranch(p_branch, p_prio ? p_prio(*p_branch) : 0);
    }

    template<typename T>
    inline void PanelTreeBranch<T>::AddBranch(const std::shared_ptr<PanelTreeBranch>& p_branch, size_t p_prio)
    {
        auto it = m_children.insert(
            { { p_branch.get()->GetName(), p_prio }, p_branch });

        it.first->second->m_parent = this;
    }

    template <typename T>
    inline void PanelTreeBranch<T>::RemoveBranch(const std::string& p_name)
    {
        for (auto it = m_children.begin(); it != m_children.end(); it++)
        {
            if (it->second.get()->m_name == p_name)
            {
                m_children.erase(it);
                break;
            }
        }
    }

    template<typename T>
    inline void PanelTreeBranch<T>::DeleteBranch(std::set<T>* p_deletedElements)
    {
        for (auto& [prio, child] : m_children)
        {
            child->DeleteBranch(p_deletedElements);

            if (p_deletedElements)
                p_deletedElements->insert(child->GetValue());
        }

        p_deletedElements->insert(GetValue());
    }

    template <typename T>
    inline void PanelTreeBranch<T>::ForceOpenParents(bool p_openSelf)
    {
        if (p_openSelf)
            m_forceState = EForceState::FORCE_OPEN;

        if (m_parent != nullptr)
            m_parent->ForceOpenParents(true);
    }

    template <typename T>
    inline void PanelTreeBranch<T>::ForceCloseChildren(bool p_closeSelf)
    {
        if (p_closeSelf)
            m_forceState = EForceState::FORCE_CLOSE;

        for (auto& child : m_children)
            child.second->ForceCloseChildren(true);
    }

    template <typename T>
    inline void PanelTreeBranch<T>::ForceOpenAll()
    {
        m_forceState = EForceState::FORCE_OPEN;

        for (auto& child : m_children)
            child.second->ForceOpenAll();
    }

    template <typename T>
    inline void PanelTreeBranch<T>::SetAllPriority(const PriorityFunc& p_prioFunc)
    {
        std::vector<Children::node_type> extracted;

        for (auto it = m_children.begin(); it != m_children.end();)
        {
            size_t prio = p_prioFunc(*it->second);

            if (it->first.m_priority != prio)
            {
                auto out = it++;
                auto nodeHandler = m_children.extract(out);
                nodeHandler.key().m_priority = prio;
                extracted.push_back(std::move(nodeHandler));
                //m_children.insert(std::move(nodeHandler));
            }
            else
                ++it;
        }

        for (auto it = extracted.begin(); it != extracted.end();)
        {
            auto out = it++;
            auto val = m_children.insert(std::move(*out));
        }

        for (auto& child : m_children)
            child.second->SetAllPriority(p_prioFunc);
    }

    //template<typename T>
    //void PanelTreeBranch<T>::NoPriority(const PriorityFunc& /*p_prioFunc*/)
    //{
    //}

    template <typename T>
    inline size_t PanelTreeBranch<T>::HasChildrenPriority(const PanelTreeBranch& p_branch)
    {
        return p_branch.GetChildren().empty() ? 0 : 1;
    }

    template <typename T>
    inline std::vector<std::unique_ptr<IMenuable>> PanelTreeBranch<T>::GetPopupMenuItems()
    {
        std::vector<std::unique_ptr<IMenuable>> menu;

        menu.emplace_back(std::make_unique<MenuButton>("Open All", [this](char) { ForceOpenAll(); }));
        menu.emplace_back(std::make_unique<MenuButton>("Close All", [this](char) { ForceCloseChildren(); }));
        menu.emplace_back(std::make_unique<MenuButton>("Test3", [this](char) {}));

        return menu;
    }

    template <typename T>
    inline const std::string& PanelTreeBranch<T>::GetIcon()
    {
        static std::string branch = "Fl";
        static std::string leaf = "Tx";

        if (IsBranch())
            return branch;
        else
            return leaf;
    }

    template <typename T>
    inline const std::string& PanelTreeBranch<T>::GetName()
    {
        return m_name;
    }

    template <typename T>
    inline bool PanelTreeBranch<T>::Open()
    {
        ISelectable::Open();

        if (IsBranch() && s_branchesOnOpen)
            return s_branchesOnOpen(*this);

        else if (s_leavesOnOpen)
            return s_leavesOnOpen(*this);

        return false;
    }

    template<typename T>
    inline bool PanelTreeBranch<T>::Select()
    {
        ISelectable::Select();

        if (IsBranch() && s_branchesOnSelect)
            return s_branchesOnSelect(*this);

        else if (s_leavesOnSelect)
            return s_leavesOnSelect(*this);

        return false;
    }

    template <typename T>
    inline void PanelTreeBranch<T>::DisplayAndUpdatePopupMenu()
    {
        m_popup.DisplayAndUpdatePanel();
    }

    template <typename T>
    inline bool PanelTreeBranch<T>::GetSelectedState()
    {
        return m_isSelected;
    }

    template <typename T>
    inline void PanelTreeBranch<T>::SetSelectedState(bool p_isSelected)
    {
        m_isSelected = p_isSelected;
    }

    template <typename T>
    inline void PanelTreeBranch<T>::DisplayAndUpdatePanel()
    {
        switch (m_forceState)
        {
        case PanelTreeBranch<T>::EForceState::FORCE_OPEN:
            ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Always);
            break;
        case PanelTreeBranch<T>::EForceState::FORCE_CLOSE:
            ForceCloseChildren();
            ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_Always);
            break;
        case PanelTreeBranch<T>::EForceState::NOTHING:
        default:
            break;
        }

        DisplayTreePanel();

        m_forceState = EForceState::NOTHING;
    }

    template <typename T>
    inline void PanelTreeBranch<T>::DisplayTreePanel()
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
        flags |= IsBranch() ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf;
        flags |= m_isSelected ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;

        bool open = ImGui::TreeNodeEx(m_name.c_str(), flags);
        DisplayAndUpdatePopupMenu();

        auto hov = ImGui::IsItemHovered();
        auto cliks = ImGui::GetMouseClickedCount(0);

        if (hov && cliks == 1)
            ToggleSelection();

        else if (hov && cliks == 2)
            Open();

        if (open)
        {
            for (auto& node : m_children)
            {
                if (!(m_hideLeafs && !node.second.get()->IsBranch()))
                    node.second->DisplayAndUpdatePanel();
            }

            ImGui::TreePop();
        }
    }

    template <typename T>
    inline bool PanelTreeBranch<T>::IsBranch() const
    {
        return !m_children.empty();
    }

    template <typename T>
    inline const PanelTreeBranch<T>::Children& PanelTreeBranch<T>::GetChildren() const
    {
        return m_children;
    }

    template<typename T>
    inline PanelTreeBranch<T>* PanelTreeBranch<T>::GetParent() const
    {
        return m_parent;
    }

    template <typename T>
    inline std::string PanelTreeBranch<T>::GetPath() const
    {
        std::vector<const PanelTreeBranch*> parents;
        parents.push_back(this);

        //get all parents
        while (parents.back()->m_parent != nullptr)
            parents.push_back(parents.back()->m_parent);

        std::string path;

        for (auto it = parents.rbegin(); it != parents.rend(); it++)
        {
            path += (*it)->m_name;
            if (!(*it)->GetChildren().empty())
                path += '/';
        }

        return path;
    }

    template<typename T>
    inline void PanelTreeBranch<T>::SetName(const std::string& p_name)
    {
        m_name = p_name;
    }

    template<typename T>
    inline const T& PanelTreeBranch<T>::SetValue(const T& p_value) const
    {
        m_value = p_value;
        return m_value;
    }

    template<typename T>
    inline const T& PanelTreeBranch<T>::GetValue() const
    {
        return m_value;
    }
}

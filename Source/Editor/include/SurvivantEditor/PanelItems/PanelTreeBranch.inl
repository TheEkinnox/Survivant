//PanelTreeBranch.cpp
#pragma once

#include "SurvivantEditor/PanelItems/PanelTreeBranch.h"

#include "SurvivantEditor/Core/IUI.h"
#include "SurvivantEditor/MenuItems/MenuButton.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    using namespace MenuItems;

    template <typename T>
    PanelTreeBranch<T>::PanelTreeBranch(const std::string& p_name, bool p_hideLeafs) :
        m_name(p_name),
        m_forceState(EForceState::NOTHING),
        m_hideLeafs(p_hideLeafs),
        m_parent(nullptr),
        m_isSelected(false)
    {
        m_popup.m_items = GetPopupMenuItems();
    }

    template <typename T>
    PanelTreeBranch<T>::PanelTreeBranch(const std::string& p_name, const Childreen& p_branches, bool p_hideLeafs) :
        m_name(p_name),
        m_forceState(EForceState::NOTHING),
        m_hideLeafs(p_hideLeafs),
        m_parent(nullptr),
        m_isSelected(false)
    {
        m_popup.m_items = GetPopupMenuItems();
        SetBranches(p_branches);
    }

    template <typename T>
    PanelTreeBranch<T>::~PanelTreeBranch()
    {
        if (m_isSelected)
            InvokeClearSelected();
    }

    template <typename T>
    inline PanelTreeBranch<T>::Childreen& PanelTreeBranch<T>::SetBranches(const Childreen& p_branches)
    {
        m_childreen = p_branches;

        for (auto& child : p_branches)
            child.second->m_parent = this;

        return m_childreen;
    }

    template <typename T>
    inline PanelTreeBranch<T>::Childreen& PanelTreeBranch<T>::SetBranches(const std::vector<std::shared_ptr<PanelTreeBranch>>& p_branches)
    {
        m_childreen.clear();
        for (auto& child : p_branches)
        {
            child->m_parent = this;
            m_childreen.insert({ { child.get()->GetName(), 0 }, child });
        }

        return m_childreen;
    }

    template <typename T>
    inline void PanelTreeBranch<T>::AddBranch(const std::shared_ptr<PanelTreeBranch>& p_branch, const PriorityFunc& p_prio)
    {
        auto it = m_childreen.insert(
            { { p_branch.get()->GetName(), p_prio? p_prio(*p_branch) : 0 }, p_branch });

        it.first->second->m_parent = this;
    }

    template <typename T>
    inline void PanelTreeBranch<T>::RemoveBranch(const std::string& p_name)
    {
        for (auto it = m_childreen.begin(); it != m_childreen.end(); it++)
        {
            if (it->second.get()->m_name == p_name)
            {
                m_childreen.erase(it);
                break;
            }
        }
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
    inline void PanelTreeBranch<T>::ForceCloseChildreen(bool p_closeSelf)
    {
        if (p_closeSelf)
            m_forceState = EForceState::FORCE_CLOSE;

        for (auto& child : m_childreen)
            child.second->ForceCloseChildreen(true);
    }

    template <typename T>
    inline void PanelTreeBranch<T>::ForceOpenAll()
    {
        m_forceState = EForceState::FORCE_OPEN;

        for (auto& child : m_childreen)
            child.second->ForceOpenAll();
    }

    template <typename T>
    inline void PanelTreeBranch<T>::SetAllPriority(const PriorityFunc& p_prioFunc)
    {
        std::vector<Childreen::node_type> extracted;

        for (auto it = m_childreen.begin(); it != m_childreen.end();)
        {
            size_t prio = p_prioFunc(*it->second);

            if (it->first.m_priority != prio)
            {
                auto out = it++;
                auto nodeHandler = m_childreen.extract(out);
                nodeHandler.key().m_priority = prio;
                extracted.push_back(std::move(nodeHandler));
                //m_childreen.insert(std::move(nodeHandler));
            }
            else
                ++it;
        }

        for (auto it = extracted.begin(); it != extracted.end();)
        {
            auto out = it++;
            auto val = m_childreen.insert(std::move(*out));
        }

        for (auto& child : m_childreen)
            child.second->SetAllPriority(p_prioFunc);
    }

    //template<typename T>
    //void PanelTreeBranch<T>::NoPriority(const PriorityFunc& /*p_prioFunc*/)
    //{
    //}

    template <typename T>
    inline size_t PanelTreeBranch<T>::HasChildreenPriority(const PanelTreeBranch& p_branch)
    {
        return p_branch.GetChildreen().empty() ? 0 : 1;
    }

    template <typename T>
    inline std::vector<std::unique_ptr<IMenuable>> PanelTreeBranch<T>::GetPopupMenuItems()
    {
        std::vector<std::unique_ptr<IMenuable>> menu;

        menu.emplace_back(std::make_unique<MenuButton>("Open All", [this](char) { ForceOpenAll(); }));
        menu.emplace_back(std::make_unique<MenuButton>("Close All", [this](char) { ForceCloseChildreen(); }));
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
    inline bool PanelTreeBranch<T>::InvokeOpen()
    {
        ISelectable::InvokeOpen();

        if (IsBranch() && s_branchesOnOpen)
            return s_branchesOnOpen(*this);

        else if (s_leavesOnOpen)
            return s_leavesOnOpen(*this);

        return false;
    }

    template<typename T>
    inline bool PanelTreeBranch<T>::InvokeSelected()
    {
        ISelectable::InvokeSelected();

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
            ForceCloseChildreen();
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
        flags |= IsBranch() ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Bullet;
        flags |= m_isSelected ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;

        bool open = ImGui::TreeNodeEx(m_name.c_str(), flags);
        DisplayAndUpdatePopupMenu();

        auto hov = ImGui::IsItemHovered();
        auto cliks = ImGui::GetMouseClickedCount(0);

        if (hov && cliks == 1)
            InvokeSelected();

        else if (hov && cliks == 2)
            InvokeOpen();

        if (open)
        {
            for (auto& node : m_childreen)
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
        return !m_childreen.empty();
    }

    template <typename T>
    inline const PanelTreeBranch<T>::Childreen& PanelTreeBranch<T>::GetChildreen() const
    {
        return m_childreen;
    }

    template <typename T>
    inline std::string PanelTreeBranch<T>::GetPathName() const
    {
        std::vector<const PanelTreeBranch*> parents;
        parents.push_back(this);

        //get all parents
        while (parents.back()->m_parent != nullptr)
        {
            parents.push_back(parents.back()->m_parent);
        }

        std::string path;

        for (auto it = parents.rbegin(); it != parents.rend(); it++)
            path += (*it)->m_name + "$";

        return path;
    }
}

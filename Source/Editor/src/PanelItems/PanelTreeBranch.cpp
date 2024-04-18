//PanelTreeBranch.cpp

#include "SurvivantEditor/PanelItems/PanelTreeBranch.h"
#include "SurvivantEditor/Core/IUI.h"
#include "SurvivantEditor/MenuItems/MenuButton.h"


#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    using namespace MenuItems;

    PanelTreeBranch::PanelTreeBranch(const std::string& p_name, bool p_hideLeafs) :
        m_name(p_name),
        m_forceState(EForceState::NOTHING),
        m_hideLeafs(p_hideLeafs),
        m_parent(nullptr),
        m_callback(nullptr),
        m_isSelected(false)
    {
        m_popup.m_items = GetPopupMenuItems();
    }

    PanelTreeBranch::PanelTreeBranch(const std::string& p_name, const Childreen& p_branches, bool p_hideLeafs) :
        m_name(p_name),
        m_forceState(EForceState::NOTHING),
        m_hideLeafs(p_hideLeafs),
        m_parent(nullptr),
        m_callback(nullptr),
        m_isSelected(false)
    {
        m_popup.m_items = GetPopupMenuItems();
        SetBranches(p_branches);
    }

    PanelTreeBranch::~PanelTreeBranch()
    {
        if (m_isSelected)
            SV_CURRENT_UI()->SetSelected();
    }


    PanelTreeBranch::Childreen& PanelTreeBranch::SetBranches(const Childreen& p_branches)
    {
        m_childreen = p_branches;

        for (auto& child : p_branches)
            child.second->m_parent = this;

        return m_childreen;
    }

    PanelTreeBranch::Childreen& PanelTreeBranch::SetBranches(const std::vector<std::shared_ptr<PanelTreeBranch>>& p_branches)
    {
        m_childreen.clear();
        for (auto& child : p_branches)
        {
            child->m_parent = this;
            m_childreen.insert({ { child.get()->GetName(), 0 }, child });
        }

        return m_childreen;
    }

    void PanelTreeBranch::AddBranch(const std::shared_ptr<PanelTreeBranch>& p_branch, const PriorityFunc& p_prio)
    {
        auto it = m_childreen.insert( 
            { { p_branch.get()->GetName(), p_prio(*p_branch) }, p_branch});

        it.first->second->m_parent = this;
    }

    void PanelTreeBranch::RemoveBranch(const std::string& p_name)
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

    void PanelTreeBranch::ForceOpenParents(bool p_openSelf)
    {
        if (p_openSelf)
            m_forceState = EForceState::FORCE_OPEN;

        if (m_parent != nullptr)
            m_parent->ForceOpenParents(true);
    }

    void PanelTreeBranch::ForceCloseChildreen(bool p_closeSelf)
    {
        if (p_closeSelf)
            m_forceState = EForceState::FORCE_CLOSE;

        for (auto& child : m_childreen)
            child.second->ForceCloseChildreen(true);
    }

    void PanelTreeBranch::ForceOpenAll()
    {
        m_forceState = EForceState::FORCE_OPEN;

        for (auto& child : m_childreen)
            child.second->ForceOpenAll();
    }

    void PanelTreeBranch::SetOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback)
    {
        m_callback = p_callback;
    }

    void PanelTreeBranch::SetAllOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback)
    {
        m_callback = p_callback;

        for (auto& child : m_childreen)
            child.second->SetAllOnClickCallback(p_callback);
    }

    void PanelTreeBranch::SetAllBranchesOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback)
    {
        if (IsBranch())
            m_callback = p_callback;

        for (auto& child : m_childreen)
            child.second->SetAllBranchesOnClickCallback(p_callback);
    }

    void PanelTreeBranch::SetAllLeavesOnClickCallback(const std::shared_ptr<BranchCallback>& p_callback)
    {
        if (!IsBranch())
            m_callback = p_callback;

        for (auto& child : m_childreen)
            child.second->SetAllLeavesOnClickCallback(p_callback);
    }

    void PanelTreeBranch::SetAllPriority(const PriorityFunc& p_prioFunc)
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
            child.second->SetAllBranchesPriority(p_prioFunc);
    }

    void PanelTreeBranch::SetAllBranchesPriority(const PriorityFunc& /*p_prioFunc*/)
    {
    }

    void PanelTreeBranch::SetAllLeavesPriority(const PriorityFunc& /*p_prioFunc*/)
    {
    }

    void PanelTreeBranch::SetAllBranchesPriority(size_t /*p_prio*/)
    {
    }

    void PanelTreeBranch::SetAllLeavesPriority(size_t /*p_prio*/)
    {
    }

    size_t PanelTreeBranch::NoPriority(const PanelTreeBranch& /*p_branch*/)
    {
        return 0;
    }

    size_t PanelTreeBranch::HasChildreenPriority(const PanelTreeBranch& p_branch)
    {
        return p_branch.GetChildreen().empty()? 0 : 1;
    }

    std::vector<std::unique_ptr<IMenuable>> PanelTreeBranch::GetPopupMenuItems()
    {
        std::vector<std::unique_ptr<IMenuable>> menu;

        menu.emplace_back(std::make_unique<MenuButton>("Open All",  [this](char) { ForceOpenAll(); }));
        menu.emplace_back(std::make_unique<MenuButton>("Close All", [this](char) { ForceCloseChildreen(); }));
        menu.emplace_back(std::make_unique<MenuButton>("Test3",     [this](char) {} ));

        return menu;
    }

    const std::string& PanelTreeBranch::GetIcon()
    {
        static std::string branch = "Fl";
        static std::string leaf = "Tx";

        if (IsBranch())
            return branch;
        else
            return leaf;
    }

    const std::string& PanelTreeBranch::GetName()
    {
        return m_name;
    }

    bool PanelTreeBranch::InvokeDoubleClick()
    {
        if (m_callback)
            return (*m_callback)(*this);

        return false;
    }

    void PanelTreeBranch::DisplayAndUpdatePopupMenu()
    {
        m_popup.DisplayAndUpdatePanel();
    }

    bool PanelTreeBranch::GetSelectedState()
    {
        return m_isSelected;
    }

    void PanelTreeBranch::SetSelectedState(bool p_isSelected)
    {
        m_isSelected = p_isSelected;
    }

    void PanelTreeBranch::DisplayAndUpdatePanel()
    {
        switch (m_forceState)
        {
        case PanelTreeBranch::EForceState::FORCE_OPEN:
            ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Always);
            break;
        case PanelTreeBranch::EForceState::FORCE_CLOSE:
            ForceCloseChildreen();
            ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_Always);
            break;
        case PanelTreeBranch::EForceState::NOTHING:
        default:
            break;
        }

        DisplayTreePanel();

        m_forceState = EForceState::NOTHING;
    }

    void PanelTreeBranch::DisplayTreePanel()
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
        flags |= IsBranch() ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Bullet;
        flags |= m_isSelected ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;

        bool open = ImGui::TreeNodeEx(m_name.c_str(), flags);
        DisplayAndUpdatePopupMenu();

        if (ImGui::GetMouseClickedCount(0) == 1 && ImGui::IsItemHovered() )
        {
            SV_CURRENT_UI()->SetSelected(this);
            m_isSelected = true;

            if (m_callback)
                (*m_callback)(*this);
        }

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

    bool PanelTreeBranch::IsBranch() const
    {
        return !m_childreen.empty();
    }

    const PanelTreeBranch::Childreen& PanelTreeBranch::GetChildreen() const
    {
        return m_childreen;
    }

    std::string PanelTreeBranch::GetPathName() const
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

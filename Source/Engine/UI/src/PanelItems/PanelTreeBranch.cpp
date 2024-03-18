//PanelTreeBranch.cpp

#include "SurvivantUI/PanelItems/PanelTreeBranch.h"
#include "SurvivantUI/Core/EditorUI.h"


#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::PanelItems
{
    PanelTreeBranch::PanelTreeBranch(const std::string& p_name, bool p_hideLeafs) :
        m_name(p_name),
        m_forceState(EForceState::NOTHING),
        m_hideLeafs(p_hideLeafs),
        m_parent(nullptr),
        m_callback(nullptr)
    {}

    PanelTreeBranch::PanelTreeBranch(const std::string& p_name, const Childreen& p_branches, bool p_hideLeafs) :
        m_name(p_name),
        m_forceState(EForceState::NOTHING),
        m_hideLeafs(p_hideLeafs),
        m_parent(nullptr),
        m_callback(nullptr)
    {
        SetBranches(p_branches);
    }

    PanelTreeBranch::Childreen& PanelTreeBranch::SetBranches(const Childreen& p_branches)
    {
        m_childreen = p_branches;
        for (auto& child : p_branches)
            child.second->m_parent = this;

        return m_childreen;
    }

    PanelTreeBranch::Childreen& PanelTreeBranch::SetBranches(const std::set<std::shared_ptr<PanelTreeBranch>>& p_branches)
    {
        m_childreen.clear();
        for (auto& child : p_branches)
        {
            child->m_parent = this;
            m_childreen.insert({ child.get()->GetName(), child });
        }

        return m_childreen;
    }

    void PanelTreeBranch::AddBranch(const std::shared_ptr<PanelTreeBranch>& p_branch)
    {
        auto it = m_childreen.insert(m_childreen.end(), { p_branch.get()->GetName(), p_branch });
        it->second.get()->m_parent = this;
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

    bool PanelTreeBranch::DisplayAndUpdateSelection(float& p_width, float& /*p_height*/, bool p_doubleClicked)
    {
        static auto font = Core::EditorUI::GetIconFont();
        auto cursorPos = ImGui::GetCursorPos();
        std::string iconTxt;

        if (IsBranch())
            iconTxt = "Fl";
        else
            iconTxt = "Tx";

        ImGui::PushFont(font);
        ImVec2 sz = ImGui::CalcTextSize(iconTxt.c_str());
        ImGui::PopFont();
        //float canvasWidth = ImGui::GetWindowContentRegionWidth();
        float canvasWidth = p_width - 16;
        float origScale = font->Scale;
        font->Scale = canvasWidth / sz.x;
        ImGui::SetCursorPos({ cursorPos.x + 8, cursorPos.y + 8 });

        ImGui::PushFont(font);
        ImGui::Text("%s", iconTxt.c_str());
        ImGui::PopFont();
        font->Scale = origScale;

        cursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos({ cursorPos.x + 4, cursorPos.y + 4 });

        ImGui::PushTextWrapPos(p_width - 4);
        ImGui::TextWrapped(m_name.c_str());
        ImGui::PopTextWrapPos();


        if (p_doubleClicked && m_callback != nullptr)
            return (*m_callback)(*this);

        //can display other items
        return false;
    }

    const std::string& PanelTreeBranch::GetName()
    {
        return m_name;
    }

    void PanelTreeBranch::DisplayTreePanel()
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
        flags |= IsBranch() ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Bullet;

        bool open = ImGui::TreeNodeEx(m_name.c_str(), flags);

        if (ImGui::GetMouseClickedCount(0) == 1 && ImGui::IsItemHovered() &&
            m_callback != nullptr)
            (*m_callback)(*this);

        if (open)
        {
            for (auto& node : m_childreen)
            {
                if (m_hideLeafs && node.second.get()->IsBranch())
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

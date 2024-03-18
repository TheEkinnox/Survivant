//ContentDrawerPanel.cpp

#include "SurvivantUI/Panels/ContentDrawerPanel.h"

#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantUI/Core/EditorUI.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::Panels
{
    using namespace Interfaces;

    ContentDrawerPanel::ContentDrawerPanel() :
        m_tree()
    {
        m_name = GetUniqueName(NAME, s_idGenerator.GetUnusedId());
        //m_tree = std::make_shared<PanelTreeBranch>("TREE");

      /*  auto& branches = m_tree->SetBranches({ std::make_shared<PanelTreeBranch>("0"), std::make_shared<PanelTreeBranch>("1"), std::make_shared<PanelTreeBranch>("2") });
        branches["0"]->SetBranches({
            std::make_shared<PanelTreeBranch>("A-123456789"),
            std::make_shared<PanelTreeBranch>("B-45678"),
            std::make_shared<PanelTreeBranch>("B-45678"),
            std::make_shared<PanelTreeBranch>("B-45678"),
            std::make_shared<PanelTreeBranch>("B-45678"),
            std::make_shared<PanelTreeBranch>("B-45678"),
            std::make_shared<PanelTreeBranch>("B-45678"),
            std::make_shared<PanelTreeBranch>("B-45678"),
            std::make_shared<PanelTreeBranch>("B-45678"),
            std::make_shared<PanelTreeBranch>("C")
            });
        branches["1"]->SetBranches({std::make_shared<PanelTreeBranch>("D-123456789"), std::make_shared<PanelTreeBranch>("E-45678"), std::make_shared<PanelTreeBranch>("F")});
        branches["2"]->SetBranches({std::make_shared<PanelTreeBranch>("G-123456789"), std::make_shared<PanelTreeBranch>("H-45678"), std::make_shared<PanelTreeBranch>("I")});*/

        SetupTree();

        auto setGridDisplayFunc = std::make_shared<PanelTreeBranch::BranchCallback>(
            std::bind(&ContentDrawerPanel::SetGridDisplay, this, std::placeholders::_1));
        m_tree.get()->SetAllBranchesOnClickCallback(setGridDisplayFunc);

        auto tryOpenFileFunc = std::make_shared<PanelTreeBranch::BranchCallback>(
            std::bind(&ContentDrawerPanel::TryOpenFile, this, std::placeholders::_1));
        m_tree.get()->SetAllLeavesOnClickCallback(tryOpenFileFunc);

    }

    ContentDrawerPanel::~ContentDrawerPanel()
    {
        s_idGenerator.RemoveId(GetUniqueIdInName());
    }

    Panel::ERenderFlags ContentDrawerPanel::Render()
    {
        bool open = true;
        static float treeWidth = ImGui::GetContentRegionAvail().x * 0.5f;

        auto panelSize = ImGui::GetContentRegionAvail();
        ImGuiWindowFlags    window_flags = ImGuiWindowFlags_None;
        ImGuiChildFlags     child_flags = ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY;

        if (!ImGui::Begin(m_name.c_str(), &open))
        {
            ImGui::End();
            return Panel::ERenderFlags();
        }

        //tree
        {
            if (ImGui::BeginChild("ChildL", ImVec2(treeWidth, ImGui::GetContentRegionAvail().y), child_flags, window_flags))
                m_tree.get()->DisplayAndUpdatePanel();

            ImGui::EndChild();
        }

        ImGui::SameLine();
        auto pos = ImGui::GetCursorPos();
        ImGui::Button("##", ImVec2(5, ImGui::GetContentRegionAvail().y));
        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        if (ImGui::IsItemActive())
        {
            treeWidth += ImGui::GetIO().MouseDelta.x;
            treeWidth = std::clamp(treeWidth, 10.0f, 2 * panelSize.x);
        }

        ImGui::SameLine();

        //grid
        {
            if (ImGui::BeginChild("ChildR", ImVec2(0, ImGui::GetContentRegionAvail().y), child_flags, window_flags))
                m_grid.DisplayAndUpdatePanel();
            ImGui::EndChild();
        }

        ImGui::End();

        auto flags = ERenderFlags();
        if (!open)
            flags = ERenderFlags::CLOSE;

        return flags;
    }

    bool ContentDrawerPanel::SetGridDisplay(PanelTreeBranch& p_branch)
    {
        p_branch.ForceOpenParents();
        auto& childreen = p_branch.GetChildreen();

        //"cast" from PanelTreeBranch to ISelectionBoxable
        std::vector<std::shared_ptr<PanelSelectionBox::ISelectionBoxable>> gridItems;
        gridItems.reserve(childreen.size());

        for (auto& pair : childreen)
            gridItems.emplace_back(pair.second);

        m_grid.SetSelectionBoxable(gridItems);

        //cannt display other items bcs changes them
        return true;
    }

    void ContentDrawerPanel::SetupTree()
    {

        auto root = std::filesystem::current_path();
        m_tree = std::make_shared<PanelTreeBranch>(root.filename().string());

        //auto files = directory_iterator(root);

        SetupBranches(m_tree, root);

    }

    void ContentDrawerPanel::SetupBranches(
        std::shared_ptr<PanelTreeBranch> p_parent, const std::filesystem::path& p_filePath)
    {
        using directory_iterator = std::filesystem::directory_iterator;
        using directory_ptr_and_path = std::pair<std::shared_ptr<PanelTreeBranch>, std::filesystem::path>;

        std::vector<directory_ptr_and_path> directories;

        for (const auto& dirEntry : directory_iterator(p_filePath))
        {
            const auto& path = dirEntry.path();
            std::string name = path.filename().string();

            auto ptrBranch = std::make_shared<PanelTreeBranch>(path.filename().string());
            p_parent.get()->AddBranch(ptrBranch);

            if (dirEntry.is_directory())
                directories.push_back({ ptrBranch, path });
        }

        for (auto& pair : directories)
            SetupBranches(pair.first, pair.second);

        //for (auto& child : files)
        //    p_parent.get()->AddBranch(std::make_shared<PanelTreeBranch>(child));

        //for (size_t i = 0; i < directories.size(); i++)
        //    SetupBranches(p_parent.get()->GetChildreens, path);
    }

    bool ContentDrawerPanel::TryOpenFile(PanelTreeBranch& p_branch)
    {
        std::string message = "Try to open file, path : " + p_branch.GetPathName();
        ::Core::EventManager::GetInstance().Invoke<Core::EditorUI::DebugEvent>(message.c_str());

        //can display other items
        return false;
    }
}
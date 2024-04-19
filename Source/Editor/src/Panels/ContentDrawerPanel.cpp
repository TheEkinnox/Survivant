//ContentDrawerPanel.cpp

#include "SurvivantEditor/Panels/ContentDrawerPanel.h"

#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/Core/EditorUI.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::Panels
{
    using namespace Interfaces;

    ContentDrawerPanel::ContentDrawerPanel() :
        m_tree()
    {
        m_name = NAME;

        SetupTree();

        // setup folder callbacks
        ResourceBranch::s_branchesOnSelect =
            std::bind(&ContentDrawerPanel::SetGridDisplay, this, std::placeholders::_1);

        // setup file callback
        ResourceBranch::s_leavesOnOpen =
            std::bind(&ContentDrawerPanel::TryOpenFile, this, std::placeholders::_1);

        m_tree->SetAllPriority(&ResourceBranch::HasChildreenPriority);
    }

    ContentDrawerPanel::~ContentDrawerPanel()
    {
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

    bool ContentDrawerPanel::SetGridDisplay(ResourceBranch& p_branch)
    {
        p_branch.ForceOpenParents();
        auto& childreen = p_branch.GetChildreen();

        //"cast" from PanelTreeBranch to ISelectionBoxable
        PanelSelectionBox::SelectableMap gridItems;

        for (auto& pair : childreen)
            gridItems.insert(pair);

        m_grid.SetSelectionBoxable(gridItems);

        //cannt display other items bcs changes them
        return true;
    }

    void ContentDrawerPanel::SetupTree()
    {
        auto root = std::filesystem::current_path();
        m_tree = std::make_shared<ResourceBranch>(root.filename().string());

        SetupBranches(m_tree, root);
    }

    void ContentDrawerPanel::SetupBranches(
        std::shared_ptr<ResourceBranch> p_parent, const std::filesystem::path& p_filePath)
    {
        using directory_iterator = std::filesystem::directory_iterator;
        using directory_ptr_and_path = std::pair<std::shared_ptr<ResourceBranch>, std::filesystem::path>;

        std::vector<directory_ptr_and_path> directories;

        for (const auto& dirEntry : directory_iterator(p_filePath))
        {
            const auto& path = dirEntry.path();
            std::string name = path.filename().string();

            auto ptrBranch = std::make_shared<ResourceBranch>(path.filename().string());
            p_parent->AddBranch(ptrBranch);

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

    bool ContentDrawerPanel::TryOpenFile(ResourceBranch& p_branch)
    {
        std::string message = "Try to open file, path : " + p_branch.GetPathName();
        SvCore::Events::EventManager::GetInstance().Invoke<Core::EditorUI::DebugEvent>(message.c_str());

        //can display other items
        return false;
    }
}
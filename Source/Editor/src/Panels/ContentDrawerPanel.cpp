//ContentDrawerPanel.cpp
#include "SurvivantEditor/Panels/ContentDrawerPanel.h"

#include "SurvivantCore/Resources/ResourceRegistry.h"

#include "SurvivantEditor/Core/EditorUI.h"
#include "SurvivantEditor/Core/InspectorItemManager.h"
#include "SurvivantEditor/Panels/InspectorPanel.h"

#include <SurvivantApp/Core/IEngine.h>

#include <SurvivantCore/ECS/Scene.h>

#include <SurvivantRendering/Resources/Material.h>
#include <SurvivantRendering/Resources/Model.h>
#include <SurvivantRendering/RHI/IShader.h>
#include <SurvivantRendering/RHI/ITexture.h>

#include <SurvivantScripting/LuaScript.h>

#include <imgui.h>

#include "SurvivantCore/Resources/ResourceManager.h"

namespace SvEditor::Panels
{
    using namespace Interfaces;
    using namespace SvCore::Resources;

    ContentDrawerPanel::ContentDrawerPanel()
    {
        m_name = NAME;

        m_buttonList.m_buttons.reserve(1);
        m_buttonList.m_buttons.emplace_back(PanelButton("Refresh", [this]() {  
                SetupTree(); 
                m_tree->ForceCloseChildreen(true);
                m_tree->Select();
            }));

        SetupTree();

        // setup folder callbacks
        ResourceBranch::s_branchesOnSelect =
            std::bind(&ContentDrawerPanel::SetGridDisplay, this, std::placeholders::_1);

        // setup file callback
        ResourceBranch::s_leavesOnOpen =
            std::bind(&ContentDrawerPanel::TryOpenFile, this, std::placeholders::_1);

        ResourceBranch::s_leavesOnSelect =
            [](ResourceBranch& p_branch)
            {
                auto panel = Core::InspectorItemManager::GetPanelableResource(
                        ResourceManager::GetInstance().GetOrCreate(p_branch.GetValue(), p_branch.GetPath()));

                if (panel)
                    InspectorPanel::SetInpectorInfo(panel, p_branch.GetName());

                return true;
            };
    }

    Panel::ERenderFlags ContentDrawerPanel::Render()
    {
        bool open = true;
        static float treeWidth = ImGui::GetContentRegionAvail().x * 0.5f;

        auto             panelSize    = ImGui::GetContentRegionAvail();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        ImGuiChildFlags  child_flags  = ImGuiChildFlags_AutoResizeY;

        if (!ImGui::Begin(m_name.c_str(), &open))
        {
            ImGui::End();
            return Panel::ERenderFlags();
        }

        //tree
        {
            if (ImGui::BeginChild("ChildL", ImVec2(treeWidth, ImGui::GetContentRegionAvail().y), child_flags, window_flags))
            {
                m_buttonList.DisplayAndUpdatePanel();
                m_tree.get()->DisplayAndUpdatePanel();
            }

            ImGui::EndChild();
        }

        ImGui::SameLine();
        auto pos = ImGui::GetCursorPos();
        ImGui::Button("##", ImVec2(5, ImGui::GetContentRegionAvail().y));

        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

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
        auto& childreen = p_branch.GetChildren();

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

        for (const auto& dirEntry : std::filesystem::directory_iterator(root))
        {
            if (dirEntry.path().filename() == DIRECTORY_PATH)
            {
                root = dirEntry.path();
                break;
            }
        }

        s_typedFiles.clear();
        s_fileExtensions = CreateExtensions();
        m_tree           = std::make_shared<ResourceBranch>(root.filename().string());

        SetupBranches(m_tree, root);
        m_tree->SetAllPriority(&ResourceBranch::HasChildreenPriority);
    }

    void ContentDrawerPanel::SetupBranches(std::shared_ptr<ResourceBranch> p_parent, const std::filesystem::path& p_filePath)
    {
        using directory_iterator = std::filesystem::directory_iterator;
        using directory_ptr_and_path = std::pair<std::shared_ptr<ResourceBranch>, std::filesystem::path>;

        std::vector<directory_ptr_and_path> directories;

        for (const auto& dirEntry : directory_iterator(p_filePath))
        {
            const auto& path = dirEntry.path();

            auto type      = GetType(path);
            auto ptrBranch = std::make_shared<ResourceBranch>(path.filename().string(), true, type);
            p_parent->AddBranch(ptrBranch);

            if (!type.empty())
                s_typedFiles[type].emplace_back(std::weak_ptr<ResourceBranch>(ptrBranch));

            if (dirEntry.is_directory())
                directories.emplace_back(directory_ptr_and_path(ptrBranch, path));
        }

        for (auto& [branchPtr, path] : directories)
            SetupBranches(branchPtr, path);

        //for (auto& child : files)
        //    p_parent.get()->AddBranch(std::make_shared<PanelTreeBranch>(child));

        //for (size_t i = 0; i < directories.size(); i++)
        //    SetupBranches(p_parent.get()->GetChildreens, path);
    }

    //std::string ContentDrawerPanel::FormatPath(const std::filesystem::path& p_filePath)
    //{
    //    std::string path = p_filePath.string();

    //    //erase before dirPath
    //    auto it = path.find(DIRECTORY_PATH);
    //    if (it != std::string::npos)
    //        path = path.substr(it);

    //    return path;
    //}

    ContentDrawerPanel::TypeExtensions ContentDrawerPanel::CreateExtensions()
    {
        using namespace SvCore::Resources;
        using namespace SvCore::ECS;
        using namespace SvRendering::Resources;
        using namespace SvRendering::RHI;
        using namespace SvScripting;

        auto& rr = ResourceRegistry::GetInstance();

        TypeExtensions extensions;
        {
            auto& set = extensions[rr.GetRegisteredTypeName<Model>()];
            set.insert(".obj");
        }
        {
            auto& set = extensions[rr.GetRegisteredTypeName<Material>()];
            set.insert(".mat");
        }
        {
            auto& set = extensions[rr.GetRegisteredTypeName<IShader>()];
            set.insert(".glsl");
            //set.insert(".frag");
            //set.insert(".vert");
        }
        {
            auto& set = extensions[rr.GetRegisteredTypeName<LuaScript>()];
            set.insert(".lua");
        }
        {
            auto& set = extensions[rr.GetRegisteredTypeName<ITexture>()];
            set.insert(".png");
        }
        {
            auto& set = extensions[rr.GetRegisteredTypeName<Scene>()];
            set.insert(".scn");
        }

        return extensions;
    }

    std::string ContentDrawerPanel::GetType(const std::filesystem::path& p_filePath)
    {
        using namespace SvCore::Resources;
        using namespace SvCore::Utility;
        const std::string extension = p_filePath.extension().string();

        if (extension.empty())
            return {};

        for (auto& [type, set] : s_fileExtensions)
        {
            if (set.contains(extension))
                return type;
        }

        return {};
    }

    bool ContentDrawerPanel::TryOpenFile(ResourceBranch& p_branch)
    {
        using namespace SvCore::ECS;
        using namespace SvCore::Resources;

        auto path = p_branch.GetName();

        const auto it = path.find_last_of('.');

        if (it == std::string::npos)
            return false;

        const auto extension = path.substr(it);

        path = p_branch.GetPath();
        SV_LOG(std::string("Try to open file, path : " + path).c_str());

        const auto& rr = ResourceRegistry::GetInstance();

        if (s_fileExtensions.at(rr.GetRegisteredTypeName<Scene>()).contains(extension)
            && !(SV_ENGINE()->IsPlayInEditor()))
        {
            SV_ENGINE()->ChangeScene(path);
        }

        //can display other items
        return false;
    }

    std::vector<std::string> ContentDrawerPanel::GetAllFilePaths(const std::string& p_type)
    {
        auto it = s_typedFiles.find(p_type);
        if (it == s_typedFiles.end())
            return {};

        auto& [type, branches] = *it;
        std::vector<std::string> paths;
        paths.reserve(branches.size());

        for (auto& branch : branches)
            paths.emplace_back(std::string(branch.lock()->GetPath()));

        return paths;
    }
}
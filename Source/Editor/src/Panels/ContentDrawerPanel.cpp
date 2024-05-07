//ContentDrawerPanel.cpp
#include "SurvivantEditor/Panels/ContentDrawerPanel.h"


#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/Core/InspectorComponentManager.h"
#include "SurvivantEditor/Core/EditorUI.h"
#include "SurvivantEditor/Panels/InspectorPanel.h"
#include "SurvivantApp/Core/IEngine.h"

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

        ResourceBranch::s_leavesOnSelect =
            [](ResourceBranch& p_branch) 
            { 
                auto panel = Core::InspectorItemManager::GetPanelableResource(p_branch.GetValue());
                if (panel)
                    InspectorPanel::SetInpectorInfo(panel, p_branch.GetName());

                return true; 
            };

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
        ImGuiChildFlags     child_flags = ImGuiChildFlags_AutoResizeY;

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
        using directory_iterator = std::filesystem::directory_iterator;

        auto root = std::filesystem::current_path();

        for (const auto& dirEntry : std::filesystem::directory_iterator(root))
        {
            if (dirEntry.path().filename() == DIRECTORY_PATH)
            {
                root = dirEntry.path();
                break;
            }
        }

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

            auto t = CreateResourceRef(path);
            auto tmp = ResourceBranch(path.filename().string(), true, t);
            auto ptrBranch = std::make_shared<ResourceBranch>(tmp);
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

    std::unordered_map<std::string, std::set<std::string>> ContentDrawerPanel::CreateExtensions()
    {
        std::unordered_map<std::string, std::set<std::string>> extensions;
        {
            auto& set = extensions.insert({"Model", std::set<std::string>() }).first->second;
            set.insert(".obj");
        }
        {
            /*auto& set = */extensions.insert({"Material", std::set<std::string>() }).first->second;
        }
        {
            auto& set = extensions.insert({"Shader", std::set<std::string>() }).first->second;
            set.insert(".glsl");
            //set.insert(".frag");
            //set.insert(".vert");
        }
        {
            auto& set = extensions.insert({"Script", std::set<std::string>() }).first->second;
            set.insert(".lua");
        }
        {
            auto& set = extensions.insert({"Texture", std::set<std::string>() }).first->second;
            set.insert(".png");
        }
        {
            auto& set = extensions.insert({ "Scene", std::set<std::string>() }).first->second;
            set.insert(".scn");
        }

        return extensions;
    }

    SvCore::Resources::GenericResourceRef ContentDrawerPanel::CreateResourceRef(const std::filesystem::path& p_filePath)
    {
        using namespace SvCore::Resources;
        std::string extension = p_filePath.extension().string();

        if (extension.empty())
            return GenericResourceRef();

        std::string resType;
        for (auto& [type, set] : FileExtensions)
        {
            if (set.contains(extension))
            {
                resType = type;
                break;
            }
        }

        if (resType.empty())
            return GenericResourceRef();

        std::string path = p_filePath.string();

        //erase before dirPath
        auto it = path.find(DIRECTORY_PATH);
        if (it != std::string::npos)
            path = path.substr(it);

        //replace all DOUBLE_SLASH for SLASH;
        size_t start_pos = 0;
        while ((start_pos = path.find(DOUBLE_SLASH, start_pos)) != std::string::npos)
        {
            path.replace(start_pos, DOUBLE_SLASH.length(), SLASH);
            start_pos += SLASH.length();
        }

        //"assets/shaders/Lit.glsl"


        return GenericResourceRef(resType, path);
    }

    bool ContentDrawerPanel::TryOpenFile(ResourceBranch& p_branch)
    {
        auto path = p_branch.GetPathName();

        auto it = path.find_last_of('.');

        if (it == path.npos)
            return false;

        auto extension = path.substr(it);
        SV_LOG(std::string("Try to open file, path : " + path).c_str());

        if (FileExtensions["Scene"].contains(extension))
            SV_ENGINE()->ChangeScene(path);

        //can display other items
        return false;
    }
}
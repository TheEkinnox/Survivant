//EditorUI.cpp

#include "SurvivantEditor/Core/EditorUI.h"

#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantApp/Windows/Window.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantCore/Utility/Utility.h"
#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantEditor/Core/IUI.h"
#include "SurvivantEditor/MenuItems/MenuButton.h"
#include "SurvivantEditor/Panels/ConsolePanel.h"
#include "SurvivantEditor/Panels/ContentDrawerPanel.h"
#include "SurvivantEditor/Panels/GamePanel.h"
#include "SurvivantEditor/Panels/HierarchyPanel.h"
#include "SurvivantEditor/Panels/ImagePanel.h"
#include "SurvivantEditor/Panels/InspectorPanel.h"
#include "SurvivantEditor/Panels/SavePanel.h"
#include "SurvivantEditor/Panels/ScenePanel.h"
#include "SurvivantEditor/Panels/TestPanel.h"
#include "SurvivantEditor/PanelItems/PanelButton.h"


#include "Vector/Vector2.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui_internal.h"

namespace SvEditor::Core
{
    using namespace MenuItems;
    using namespace PanelItems;

    EditorUI::EditorUI() :
        m_main(std::make_shared<MainPanel>()),
        m_currentPanels(),
        m_selected(nullptr)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Dockspace

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        //setup Main panel
        MainPanel::ChangeLayout l = std::bind(&EditorUI::Layout1, this, std::placeholders::_1);
        m_main->ChangePanelLayout(l);

        //TODO : add spawn save m_panel on event close request
        SvCore::Events::EventManager::GetInstance().AddListenner<SvApp::Window::WindowCloseRequest>(
            SvApp::Window::WindowCloseRequest::EventDelegate(std::bind(&EditorUI::TryCreateSavePanel, this)));

        //fonts
        ImFontConfig config;
        config.SizePixels = DEFAULT_FONT_SIZE;
        config.OversampleH = config.OversampleV = 1;
        config.PixelSnapH = true;
        m_fonts.push_back(io.Fonts->AddFontDefault(&config));

        config.SizePixels = ICON_FONT_SIZE;
        m_fonts.push_back(io.Fonts->AddFontDefault(&config));

        //UIManager
        SvEditor::Core::IUI::m_currentUI = this;

        //Propagate selection
        ISelectable::m_onSelected.AddListener([this](ISelectable* p_selected) 
            { 
            if (m_selected)
                m_selected->InvokeClearSelected(); 
            m_selected = p_selected; 
            });
        ISelectable::m_onClearSelected.AddListener([this]() { m_selected = nullptr; });
    }

    EditorUI::~EditorUI()
    {
    }

    void EditorUI::InitEditorUi(SvApp::Window* p_window)
    {
        ImGui_ImplGlfw_InitForOpenGL(p_window->GetWindow(), true);
        //#ifdef __EMSCRIPTEN__
        //    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
        //#endif
        ImGui_ImplOpenGL3_Init(SvApp::GLSL_Version);
    }

    void EditorUI::InitGamePanel(const WorldContext::WorldCreator& p_worldCreator, const std::array<std::function<void()>, 3> p_playPauseFrameCallbacks)
    {
        GamePanel::SetGameWorldCreator(p_worldCreator);
        GamePanel::AddPlayListenner(p_playPauseFrameCallbacks[0]);
        GamePanel::AddPauseListenner(p_playPauseFrameCallbacks[1]);
        GamePanel::AddFrameListenner(p_playPauseFrameCallbacks[2]);
    }

    void EditorUI::InitScenePanel(std::weak_ptr<WorldContext> p_world)
    {
        using namespace SvApp::Core;
        using namespace SvRendering::RHI;
        using namespace SvRendering::Enums;

        ScenePanel::SetSceneWorld(p_world);

        //                p_world.lock()->m_renderingContext->GetTexture(RenderingContext::ERenderType::ID);


        ScenePanel::AddClickSceneListenner(
            [p_world](const LibMath::Vector2& p_uv)
            { 
                auto id = p_world.lock()->
                    m_renderingContext->GetIdTextureValue(p_uv);


            });
        
        ScenePanel::AddResizeListenner(
            [](const LibMath::Vector2& p_size)
            { SV_EVENT_MANAGER().Invoke<EditorUI::DebugEvent>(SvCore::Utility::FormatString("Size = %f, %f", p_size.m_x, p_size.m_y).c_str()); });

        m_inputs = p_world.lock()->m_inputs;
        m_main->SetMenuBar(CreateMenuBar());
    }

    void EditorUI::StartFrameUpdate()
    {
        // Update the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool b = true;
        ImGui::ShowDemoWindow(&b);
    }

    void EditorUI::RenderPanels()
    {
        struct PanelFlags
        {
            std::string         m_name;
            Panel::ERenderFlags m_flags = Panel::ERenderFlags();
        };

        std::vector<PanelFlags> pfArray;

        {//main always first
            auto flags = m_main->Render();
            if (flags != Panel::ERenderFlags())
                pfArray.push_back({ m_main->GetName(), flags});
        }

        for (auto& [name, panel] : m_currentPanels)
        {
            auto flags = panel->Render();
            if (flags != Panel::ERenderFlags())
                pfArray.push_back({ panel->GetName(), flags});
        }

        //handle m_flags after
        for (auto& pf : pfArray)
            HandlePanelFlags(pf.m_name, pf.m_flags);
    }

    void EditorUI::EndFrameUpdate()
    {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        for (auto& callback : m_endFrameCallbacks)
        {
            //magic!
            (this->*callback)();
        }

        m_endFrameCallbacks.clear();
    }

    MenuBar EditorUI::CreateMenuBar()
    {
#undef MOD_ALT
#undef MOD_CONTROL

        using namespace SvApp;

        MenuBar menuBar;
        auto& menuList = menuBar.m_menus;

        //add menu 'File' to menu list
        Menu& menu1 = menuList.emplace_back("File");
        //add buttons to menu that triggers an event
        menu1.m_items.emplace_back(std::make_unique<MenuButton>(
            "New",
            [](char) {}));
        //add buton with a keyboard shortcut
        menu1.m_items.emplace_back(std::make_unique<MenuButton>(
            "Exit",
            [](char) { SvCore::Events::EventManager::GetInstance().Invoke<SvApp::Window::WindowCloseRequest>(); },
            InputManager::KeyboardKeyType(
                EKey::F11,
                EKeyState::PRESSED,
                EInputModifier::MOD_ALT),
            *m_inputs
        ));

        Menu& menu2 = menuList.emplace_back("Edit");
        menu2.m_items.emplace_back(std::make_unique<MenuButton>(
            "Button1",
            [](char) {},
            InputManager::KeyboardKeyType(
                EKey::F11,
                EKeyState::PRESSED,
                EInputModifier(EInputModifier::MOD_ALT | EInputModifier::MOD_CONTROL)),
            *m_inputs
        ));
        menu2.m_items.emplace_back(std::make_unique<MenuButton>(
            "Cut",
            [](char) {}));
        menu2.m_items.emplace_back(std::make_unique<MenuButton>(
            "Copy",
            [](char) {}));
        menu2.m_items.emplace_back(std::make_unique<MenuButton>(
            "Paste",
            [](char) {}));
        menu2.m_items.emplace_back(std::make_unique<MenuButton>(
            "Undo",
            [](char) {}));

        //Add a menu to the menu
        {
            auto menu3 = std::make_unique<Menu>("View");
            menu3->m_items.emplace_back(std::make_unique<MenuButton>(
                "Test",
                [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateNewTestPanel); }));
            menu3->m_items.emplace_back(std::make_unique<MenuButton>(
                "Console",
                [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateConsolePanel); }));
            menu3->m_items.emplace_back(std::make_unique<MenuButton>(
                "Save",
                [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateSavePanel); }));
            menu3->m_items.emplace_back(std::make_unique<MenuButton>(
                "Content",
                [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateContentPanel); }));
            menu3->m_items.emplace_back(std::make_unique<MenuButton>(
                "Inspector",
                [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateInspectorPanel); }));

            menu2.m_items.emplace_back(std::move(menu3));
        }

        Menu& menu4 = menuList.emplace_back(menu2);
        menu4.SetName("Copy");

        return menuBar;
    }

    void EditorUI::HandlePanelFlags(const std::string& p_name, Panel::ERenderFlags p_flags)
    {
        if (p_flags & Panel::CLOSE)
            m_currentPanels.erase(p_name);

        if (p_flags & Panel::DefaultInputs)
            SvApp::InputManager::GetInstance().SetInputBindings(m_inputs);
    }

    //ISelectable* EditorUI::GetSelected()
    //{
    //    return m_selected;
    //}

    //void EditorUI::SetSelected(ISelectable* p_selected)
    //{
    //    //remose selection
    //    if (m_selected != nullptr)
    //        m_selected->SetSelectedState(false);

    //    //set new selection
    //    m_selected = p_selected;
    //}

    ImFont* EditorUI::GetFontDefault()
    {
        return m_fonts[0];
    }

    ImFont* EditorUI::GetIconFont()
    {
        return m_fonts[1];
    }

    std::shared_ptr<Panel> EditorUI::CreateNewTestPanel()
    {
        static int i = 0;

        //TODO: remove debug message "Created Test Panel"
        SvCore::Events::EventManager::GetInstance().Invoke<EditorUI::DebugEvent>("Created Test Panel");
        auto name = std::string("test-") + std::to_string(i++);

        return m_currentPanels.insert({ name, std::make_shared<TestPanel>(name) }).first->second;
    }

    void EditorUI::TryCreateSavePanel()
    {
        //TODO: add save boolean here
        std::srand((int)std::time(0));
        auto val = std::rand();
        if (val % 20 == 0)
        {
            CreateSavePanel();
            SvApp::Window::WindowCloseRequest::InterceptCloseRequest();
        }
    }

    std::shared_ptr<Panel> EditorUI::CreateSavePanel()
    {
        //if (m_currentPanels.contains(SavePanel::NAME))
            //focus

        return m_currentPanels.insert(
            { SavePanel::NAME, std::make_shared<SavePanel>() }).first->second;
    }

    std::shared_ptr<Panel> EditorUI::CreateConsolePanel()
    {
        auto panel = m_currentPanels.find(SavePanel::NAME);
        if (panel != m_currentPanels.end())
        {
            m_main->ForceFocus(SavePanel::NAME);
            return panel->second;
        }

        return m_currentPanels.insert(
            { ConsolePanel::NAME, std::make_shared<ConsolePanel>() }).first->second;
    }

    std::shared_ptr<Panel> EditorUI::CreateContentPanel()
    {
        auto panel = m_currentPanels.find(ContentDrawerPanel::NAME);
        if (panel != m_currentPanels.end())
        {
            m_main->ForceFocus(ContentDrawerPanel::NAME);
            return panel->second;
        }

        return m_currentPanels.insert(
            { ContentDrawerPanel::NAME, std::make_shared<ContentDrawerPanel>() }).first->second;
    }

    std::shared_ptr<Panel> EditorUI::CreateInspectorPanel()
    {
        auto panel = m_currentPanels.find(InspectorPanel::NAME);
        if (panel != m_currentPanels.end())
        {
            m_main->ForceFocus(InspectorPanel::NAME);
            return panel->second;
        }

        return m_currentPanels.insert(
            { InspectorPanel::NAME, std::make_shared<InspectorPanel>() }).first->second;
    }

    std::shared_ptr<Panel> EditorUI::CreateGamePanel()
    {
        auto panel = m_currentPanels.find(GamePanel::NAME);
        if (panel != m_currentPanels.end())
        {
            m_main->ForceFocus(GamePanel::NAME);
            return panel->second;
        }

        return m_currentPanels.insert(
            { GamePanel::NAME, std::make_shared<GamePanel>() }).first->second;
    }

    std::shared_ptr<Panel> Core::EditorUI::CreateScenePanel()
    {
        auto panel = m_currentPanels.find(ScenePanel::NAME);
        if (panel != m_currentPanels.end())
        {
            m_main->ForceFocus(ScenePanel::NAME);
            return panel->second;
        }

        return m_currentPanels.insert(
            { ScenePanel::NAME, std::make_shared<ScenePanel>() }).first->second;
    }

    std::shared_ptr<Panel> EditorUI::CreateHierarchyPanel()
    {
        auto panel = m_currentPanels.find(HierarchyPanel::NAME);
        if (panel != m_currentPanels.end())
        {
            m_main->ForceFocus(HierarchyPanel::NAME);
            return panel->second;
        }

        return m_currentPanels.insert(
            { HierarchyPanel::NAME, std::make_shared<HierarchyPanel>() }).first->second;
    }

    void EditorUI::Layout1(int p_dockspaceId)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGuiID id = p_dockspaceId;

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        ImGui::DockBuilderRemoveNode(id); // clear any previous layout
        ImGui::DockBuilderAddNode(id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(id, viewport->Size);

        auto dock_id_down = ImGui::DockBuilderSplitNode(id, ImGuiDir_Down, 0.35f, nullptr, &id);
        auto dock_id_left = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 0.25f, nullptr, &id);
        auto dock_id_right = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.25f, nullptr, &id);
        //auto dock_id_top = ImGui::DockBuilderSplitNode(id, ImGuiDir_Up, 0.2f, nullptr, &id);

        // we now dock our windows into the docking node we made above
        ImGui::DockBuilderDockWindow(CreateGamePanel()->GetName().c_str(), id);
        ImGui::DockBuilderDockWindow(CreateScenePanel()->GetName().c_str(), id);
        ImGui::DockBuilderDockWindow(CreateInspectorPanel()->GetName().c_str(), dock_id_right);
        ImGui::DockBuilderDockWindow(CreateHierarchyPanel()->GetName().c_str(), dock_id_left);
        ImGui::DockBuilderDockWindow(CreateConsolePanel()->GetName().c_str(), dock_id_down);
        ImGui::DockBuilderDockWindow(CreateContentPanel()->GetName().c_str(), dock_id_down);

        ImGui::DockBuilderFinish(id);
    }
}

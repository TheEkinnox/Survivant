//EditorUI.cpp

#include "SurvivantEditor/UI/Core/EditorUI.h"

#include "SurvivantApp/Windows/Window.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantCore/Utility/Utility.h"
#include "SurvivantEditor/UI/Core/UIManager.h"
#include "SurvivantEditor/UI/MenuItems/MenuButton.h"
#include "SurvivantEditor/UI/Panels/ConsolePanel.h"
#include "SurvivantEditor/UI/Panels/ContentDrawerPanel.h"
#include "SurvivantEditor/UI/Panels/GamePanel.h"
#include "SurvivantEditor/UI/Panels/HierarchyPanel.h"
#include "SurvivantEditor/UI/Panels/ImagePanel.h"
#include "SurvivantEditor/UI/Panels/InspectorPanel.h"
#include "SurvivantEditor/UI/Panels/SavePanel.h"
#include "SurvivantEditor/UI/Panels/ScenePanel.h"
#include "SurvivantEditor/UI/Panels/TestPanel.h"
#include "SurvivantEditor/UI/PanelItems/PanelButton.h"

#include "Vector/Vector2.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui_internal.h"

namespace SvEditor::UI::Core
{
    using namespace MenuItems;
    using namespace PanelItems;

    EditorUI::EditorUI(SvApp::InputManager::InputBindings& p_inputs) :
        m_inputs(p_inputs),
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
        m_main->SetMenuBar(CreateMenuBar());

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
        SvEditor::UI::Core::UIManager::GetInstance().SetCurrentUI(this);
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

    void EditorUI::InitGamePanel(intptr_t p_textureId, const std::function<void()> p_playPauseFrameCallbacks[3])
    {
        GamePanel::SetGameTexture(p_textureId);
        GamePanel::AddPlayListenner(p_playPauseFrameCallbacks[0]);
        GamePanel::AddPauseListenner(p_playPauseFrameCallbacks[1]);
        GamePanel::AddFrameListenner(p_playPauseFrameCallbacks[2]);
    }

    void EditorUI::InitScenePanel(intptr_t p_sceneTextureId, intptr_t p_idTextureId)
    {
        ScenePanel::SetSceneTexture(p_sceneTextureId);
        ScenePanel::SetIdTexture(p_idTextureId);
        /*ScenePanel::AddClickSceneListenner(
            [](const LibMath::Vector2& p_uv)
            { SV_EVENT_MANAGER().Invoke<EditorUI::DebugEvent>(SvCore::Utility::FormatString("UV = %f, %f", p_uv.m_x, p_uv.m_y).c_str());});
        */
        ScenePanel::AddResizeListenner(
            [](const LibMath::Vector2& p_size)
            { SV_EVENT_MANAGER().Invoke<EditorUI::DebugEvent>(SvCore::Utility::FormatString("Size = %f, %f", p_size.m_x, p_size.m_y).c_str()); });

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
            std::shared_ptr<Panel> m_panel;
            Panel::ERenderFlags m_flags = Panel::ERenderFlags();
        };

        std::vector<PanelFlags> pfArray;

        {//main always first
            auto flags = m_main->Render();
            if (flags != Panel::ERenderFlags())
                pfArray.push_back({ m_main, flags });
        }

        for (auto& panel : m_currentPanels)
        {
            auto flags = panel->Render();
            if (flags != Panel::ERenderFlags())
                pfArray.push_back({ panel, flags });
        }

        //DisplayPopupMenu();

        //handle m_flags after
        for (auto& pf : pfArray)
            HandlePanelFlags(pf.m_panel, pf.m_flags);
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
            m_inputs
        ));

        Menu& menu2 = menuList.emplace_back("Edit");
        menu2.m_items.emplace_back(std::make_unique<MenuButton>(
            "Button1",
            [](char) {},
            InputManager::KeyboardKeyType(
                EKey::F11,
                EKeyState::PRESSED,
                EInputModifier(EInputModifier::MOD_ALT | EInputModifier::MOD_CONTROL)),
            m_inputs
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

    void EditorUI::HandlePanelFlags(std::shared_ptr<Panel> p_panel, Panel::ERenderFlags p_flags)
    {
        if (p_flags & Panel::CLOSE)
            m_currentPanels.erase(p_panel);
    }

    ISelectable* EditorUI::GetSelected()
    {
        return m_selected;
    }

    void EditorUI::SetSelected(ISelectable* p_selected)
    {
        //remose selection
        if (m_selected != nullptr)
            m_selected->SetSelectedState(false);

        //set new selection
        m_selected = p_selected;
    }

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

        return *m_currentPanels.insert(std::make_shared<TestPanel>(std::string("test-") + std::to_string(i++))).first;
    }

    void EditorUI::TryCreateSavePanel()
    {
        //TODO: add save boolean here
        std::srand((int)std::time(0));
        auto val = std::rand();
        if (val % 20 == 0 || SavePanel::GetPanelCount() != 0)
        {
            CreateSavePanel();
            SvApp::Window::WindowCloseRequest::InterceptCloseRequest();
        }
    }

    std::shared_ptr<Panel> EditorUI::CreateSavePanel()
    {
        if (SavePanel::GetPanelCount() == 0)
            return *m_currentPanels.insert(std::make_shared<SavePanel>()).first;

        return nullptr;
    }

    std::shared_ptr<Panel> EditorUI::CreateConsolePanel()
    {
        return *m_currentPanels.insert(std::make_shared<ConsolePanel>()).first;
    }

    std::shared_ptr<Panel> EditorUI::CreateContentPanel()
    {
        return *m_currentPanels.insert(std::make_shared<ContentDrawerPanel>()).first;
    }

    std::shared_ptr<Panel> EditorUI::CreateInspectorPanel()
    {
        return *m_currentPanels.insert(std::make_shared<InspectorPanel>()).first;
    }

    std::shared_ptr<Panel> EditorUI::CreateGamePanel()
    {
        return *m_currentPanels.insert(std::make_shared<GamePanel>()).first;
    }

    std::shared_ptr<Panel> Core::EditorUI::CreateScenePanel()
    {
        return *m_currentPanels.insert(std::make_shared<ScenePanel>()).first;
    }

    std::shared_ptr<Panel> EditorUI::CreateHierarchyPanel()
    {
        return *m_currentPanels.insert(std::make_shared<HierarchyPanel>()).first;
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

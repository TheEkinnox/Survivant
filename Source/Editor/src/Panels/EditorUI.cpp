//EditorUI.cpp

#include "SurvivantEditor/Core/EditorUI.h"

#include "SurvivantEditor/Core/EditorEngine.h"
#include "SurvivantEditor/Panels/BuildPanel.h"
#include "SurvivantEditor/Panels/ConsolePanel.h"
#include "SurvivantEditor/Panels/GamePanel.h"
#include "SurvivantEditor/Panels/HierarchyPanel.h"
#include "SurvivantEditor/Panels/InspectorPanel.h"
#include "SurvivantEditor/Panels/SavePanel.h"
#include "SurvivantEditor/Panels/ScenePanel.h"
#include "SurvivantEditor/Panels/TestPanel.h"

#include <SurvivantApp/Core/IEngine.h>
#include <SurvivantApp/Inputs/InputManager.h>
#include <SurvivantApp/Windows/Window.h>

#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Events/EventManager.h>
#include <SurvivantCore/Utility/Utility.h>

#include <SurvivantRendering/Components/ModelComponent.h>

#include <Vector/Vector2.h>

#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <ImGuizmo.h>

namespace SvEditor::Core
{
    using namespace MenuItems;
    using namespace PanelItems;

    using namespace LibMath;
    using namespace SvCore::ECS;
    using namespace SvRendering::Components;

    EditorUI::EditorUI() :
        m_main(std::make_shared<MainPanel>()),
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
        m_currentUI = this;

        //Propagate selection
        ISelectable::s_onSelected.AddListener([this](ISelectable* p_selected)
            {
            if (m_selected)
                m_selected->ClearSelection();
            m_selected = p_selected;
            });
        ISelectable::s_onClearSelected.AddListener([this]() { m_selected = nullptr; });
    }

    void EditorUI::InitWindow(SvApp::Window* p_window)
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
        using namespace SvCore::ECS;
        using namespace SvRendering::RHI;
        using namespace SvRendering::Enums;

        ScenePanel::SetSceneWorld(p_world);

        ScenePanel::AddClickSceneListenner(
            [p_world](const Vector2& p_uv)
            {
                auto& scene = p_world.lock()->CurrentScene();
                auto entity = p_world.lock()->
                    m_renderingContext->GetEntityIdValue(p_uv, scene.Get());

                SV_LOG(SvCore::Utility::FormatString("ID = %d", entity.GetIndex()).c_str());

                //auto& currentSelected = p_world.lock()->m_renderingContext->s_editorSelectedEntity;
                //if (currentSelected == entity)
                //    currentSelected = {};
                //else
                //    currentSelected = EntityHandle(p_world.lock()->CurrentScene().Get(), entity);

                HierarchyPanel::ToggleSelectable(entity.GetIndex());

                //auto entityPanel = InspectorItemManager::GetPanelableEntity(
                //    SvCore::ECS::EntityHandle(p_world.lock()->CurrentScene().Get(), currentSelected));

                //does this in ToggleSelectable
                //InspectorPanel::SetInpectorInfo(entityPanel, "Entity");
            });

        ScenePanel::AddResizeListenner(
            [p_world](const Vector2& p_size)
            {
                p_world.lock()->m_renderingContext->Resize(p_size);
                SV_LOG(SvCore::Utility::FormatString("Size = %f, %f", p_size.m_x, p_size.m_y).c_str());
            });

        m_inputs = p_world.lock()->m_inputs;
        m_main->SetMenuBar(CreateMenuBar(p_world));
    }

    void EditorUI::InitHierarchyPanel(std::weak_ptr<WorldContext> p_world)
    {
        HierarchyPanel::SetCurrentSceneGetter(
            [p_world]() { return p_world.lock()->CurrentSceneRef(); });
    }

    void EditorUI::StartFrameUpdate()
    {
        // UpdateScripts the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
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

    void EditorUI::ForceGameFocus()
    {
        m_main->ForceFocus(GamePanel::NAME);
    }

    void EditorUI::ForceSceneFocus()
    {
        m_main->ForceFocus(ScenePanel::NAME);
    }

    namespace
    {
        void SetFocusPerspective(
            const CameraComponent& p_cam, Transform& p_camTransform, const EntityHandle& p_entity, const float p_spacing = 0.f)
        {
            const Transform* entityTransform = p_entity.Get<Transform>();
            const Vector3 entityPos = entityTransform ? entityTransform->getWorldPosition() : Vector3::zero();

            float boundsSize;

            if (const ModelComponent* model = p_entity.Get<ModelComponent>(); model && model->m_model)
            {
                const auto [min, max] = entityTransform ?
                    TransformBoundingBox(model->m_model->GetBoundingBox(), entityTransform->getWorldMatrix())
                    : model->m_model->GetBoundingBox();

                boundsSize = min.distanceFrom(max);
            }
            else if (entityTransform)
            {
                boundsSize = entityTransform->getWorldScale().magnitude();
            }
            else
            {
                return; // No model and no transform - Nothing to focus
            }

            const float minDistance = (boundsSize + p_spacing) / sin(p_cam.GetFovY() * .5f);
            p_camTransform.setPosition(entityPos + p_camTransform.forward() * minDistance);
        }
    }

    MenuBar EditorUI::CreateMenuBar(std::weak_ptr<WorldContext> p_world)
    {
        using namespace SvApp;

        MenuBar menuBar;
        auto& menuList = menuBar.m_menus;

        m_inputs = Inputs(p_world.lock()->m_inputs);
        auto inputs = m_inputs.lock();

        //add menu 'File' to menu list
        Menu& menu1 = menuList.emplace_back("Test");
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
            *inputs
        ));

        Menu& menu2 = menuList.emplace_back("Edit");
        menu2.m_items.emplace_back(std::make_unique<MenuButton>(
            "Button1",
            [](char) {},
            InputManager::KeyboardKeyType(
                EKey::F11,
                EKeyState::PRESSED,
                EInputModifier(EInputModifier::MOD_ALT | EInputModifier::MOD_CONTROL)),
            *inputs
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

            menu2.m_items.emplace_back(std::move(menu3));
        }

        Menu& sceneMenu = menuList.emplace_back("Scene");

        sceneMenu.m_items.emplace_back(std::make_unique<MenuButton>(MenuButton(
            "Save",
            [](char)
            {
                SV_EVENT_MANAGER().Invoke<EditorEngine::OnSave>();
            },
            InputManager::KeyboardKeyType(EKey::S, EKeyState::PRESSED, EInputModifier::MOD_CONTROL),
            *inputs
        )));

        sceneMenu.m_items.emplace_back(std::make_unique<MenuButton>(MenuButton(
            "Reload",
            [p_world](char)
            {
                auto engine = dynamic_cast<Core::EditorEngine*>(SV_ENGINE());
                if (engine->IsPlayInEditor())
                {
                    SV_LOG_WARNING("Can't reload scene during play mode");
                    return;
                }

                engine->ChangeScene(p_world.lock()->CurrentScene().GetPath());
                SV_LOG("Scene reloaded successfully");
            },
            InputManager::KeyboardKeyType(EKey::R, EKeyState::PRESSED, EInputModifier::MOD_CONTROL),
            *inputs
        )));

        Menu& entityMenu = menuList.emplace_back("Entity");

        entityMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Add New",
            [p_world](char)
            {
                if (const auto& scene = p_world.lock()->CurrentScene())
                    scene->Create();
            },
            InputManager::KeyboardKeyType(
                EKey::N, EKeyState::PRESSED,
                static_cast<EInputModifier>(EInputModifier::MOD_CONTROL | EInputModifier::MOD_SHIFT)
            ),
            *inputs
        ));

        entityMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Add New Child",
            [p_world](char)
            {
                const auto world = p_world.lock();

                if (Scene* scene = world->CurrentScene().Get())
                {
                    const EntityHandle entity(scene, world->m_renderingContext->s_editorSelectedEntity);
                    (void)(entity ? entity.AddChild() : scene->Create());
                }
            },
            InputManager::KeyboardKeyType(
                EKey::N, EKeyState::PRESSED,
                static_cast<EInputModifier>(EInputModifier::MOD_CONTROL | EInputModifier::MOD_ALT)
            ),
            *inputs
        ));

        entityMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Add New Parent",
            [p_world](char)
            {
                const auto world = p_world.lock();

                if (Scene* scene = world->CurrentScene().Get())
                {
                    const EntityHandle parent = scene->Create();
                    EntityHandle       entity(scene, world->m_renderingContext->s_editorSelectedEntity);

                    if (entity)
                        entity.SetParent(parent, true);
                }
            },
            InputManager::KeyboardKeyType(
                EKey::N, EKeyState::PRESSED,
                static_cast<EInputModifier>(EInputModifier::MOD_ALT | EInputModifier::MOD_SHIFT)
            ),
            *inputs
        ));

        entityMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Focus selected",
            [p_world](char)
            {
                using namespace LibMath;
                using namespace SvCore::ECS;
                using namespace SvRendering::Components;

                const auto world               = p_world.lock();
                const auto [cam, camTransform] = world->m_renderingContext->GetCameraInfo();

                if (!cam || !camTransform)
                    return;

                const EntityHandle entity(world->CurrentScene().Get(), world->m_renderingContext->s_editorSelectedEntity);
                SetFocusPerspective(*cam, *camTransform, entity);
            },
            InputManager::KeyboardKeyType(EKey::F, EKeyState::PRESSED, EInputModifier()),
            *inputs
        ));

        entityMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Duplicate selected",
            [p_world](char)
            {
                const auto         world = p_world.lock();
                Scene*             scene = world->CurrentScene().Get();
                const EntityHandle entity(scene, world->m_renderingContext->s_editorSelectedEntity);
                (void)entity.Copy();
            },
            InputManager::KeyboardKeyType(EKey::D, EKeyState::PRESSED, EInputModifier::MOD_CONTROL),
            *inputs
        ));

        entityMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Remove selected",
            [p_world](char)
            {
                const auto   world = p_world.lock();
                Scene*       scene = world->CurrentScene().Get();
                EntityHandle entity(scene, world->m_renderingContext->s_editorSelectedEntity);
                entity.Destroy();
            },
            InputManager::KeyboardKeyType(EKey::DEL, EKeyState::PRESSED, EInputModifier()),
            *inputs
        ));

        Menu& panelsMenu = menuList.emplace_back("Panels");

        panelsMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Build",
            [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateBuildPanel); },
            InputManager::KeyboardKeyType(EKey::B, EKeyState::PRESSED, EInputModifier::MOD_ALT),
            *inputs
        ));

        panelsMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Console",
            [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateConsolePanel); },
            InputManager::KeyboardKeyType(EKey::C, EKeyState::PRESSED, EInputModifier::MOD_ALT),
            *inputs
        ));

        panelsMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Assets",
            [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateContentPanel); },
            InputManager::KeyboardKeyType(EKey::A, EKeyState::PRESSED, EInputModifier::MOD_ALT),
            *inputs
        ));

        panelsMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Game",
            [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateGamePanel); },
            InputManager::KeyboardKeyType(EKey::G, EKeyState::PRESSED, EInputModifier::MOD_ALT),
            *inputs
        ));

        panelsMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Hierarchy",
            [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateHierarchyPanel); },
            InputManager::KeyboardKeyType(EKey::H, EKeyState::PRESSED, EInputModifier::MOD_ALT),
            *inputs
        ));

        panelsMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Inspector",
            [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateInspectorPanel); },
            InputManager::KeyboardKeyType(EKey::I, EKeyState::PRESSED, EInputModifier::MOD_ALT),
            *inputs
        ));

        panelsMenu.m_items.emplace_back(std::make_unique<MenuButton>(
            "Scene",
            [this](char) { m_endFrameCallbacks.push_back(&EditorUI::CreateScenePanel); },
            InputManager::KeyboardKeyType(EKey::S, EKeyState::PRESSED, EInputModifier::MOD_ALT),
            *inputs
        ));

        return menuBar;
    }

    void EditorUI::HandlePanelFlags(const std::string& p_name, Panel::ERenderFlags p_flags)
    {
        if (p_flags & Panel::CLOSE)
            m_currentPanels.erase(p_name);

        if (p_flags & Panel::DefaultInputs)
            SvApp::InputManager::GetInstance().SetInputBindings(m_inputs.lock());
    }

    ISelectable* EditorUI::GetSelected()
    {
        return m_selected;
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

        auto name = std::string("test-") + std::to_string(i++);

        return m_currentPanels.insert({ name, std::make_shared<TestPanel>(name) }).first->second;
    }

    void EditorUI::TryCreateSavePanel()
    {
        if (dynamic_cast<EditorEngine*>(SV_ENGINE())->IsEditorModifiedScene())
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

    std::shared_ptr<Panel> EditorUI::CreateScenePanel()
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

    std::shared_ptr<Panel> EditorUI::CreateBuildPanel()
    {
        auto panel = m_currentPanels.find(BuildPanel::NAME);
        if (panel != m_currentPanels.end())
        {
            m_main->ForceFocus(BuildPanel::NAME);
            return panel->second;
        }

        return m_currentPanels.insert(
            { BuildPanel::NAME, std::make_shared<BuildPanel>() }).first->second;
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
        ImGui::DockBuilderDockWindow(CreateScenePanel()->GetName().c_str(), id);
        ImGui::DockBuilderDockWindow(CreateGamePanel()->GetName().c_str(), id);
        ImGui::DockBuilderDockWindow(CreateInspectorPanel()->GetName().c_str(), dock_id_right);
        ImGui::DockBuilderDockWindow(CreateHierarchyPanel()->GetName().c_str(), dock_id_left);
        ImGui::DockBuilderDockWindow(CreateConsolePanel()->GetName().c_str(), dock_id_down);
        ImGui::DockBuilderDockWindow(CreateContentPanel()->GetName().c_str(), dock_id_down);

        ImGui::DockBuilderFinish(id);
    }
}

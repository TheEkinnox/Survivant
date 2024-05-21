//EditorUI.h
#pragma once

#include "SurvivantApp/Core/WorldContext.h"
#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/Core/IUI.h"
#include "SurvivantEditor/Interfaces/ISelectable.h"
#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/Panels/MainPanel.h"
#include "SurvivantEditor/MenuItems/Menu.h"
#include "SurvivantEditor/MenuItems/MenuBar.h"

#include <unordered_set>
#include <memory>
#include <array>

//foward declaration
struct ImFont;

namespace SvApp {
	class Window;
}

namespace SvEditor::Core
{
	using namespace Panels;
	using namespace MenuItems;
	using namespace Interfaces;

	class EditorUI: public IUI
	{
	public:
		using Panels = std::unordered_map<std::string, std::shared_ptr<Panel>>;
		using WorldContext = SvApp::Core::WorldContext;

		EditorUI();
		~EditorUI() override = default;

		void InitWindow(SvApp::Window* p_window);

		void InitGamePanel(
			const WorldContext::WorldCreator& p_worldCreator,
			const std::array<std::function<void()>, 3> p_playPauseFrameCallbacks);
		void InitScenePanel(std::weak_ptr<WorldContext> p_world);
		void InitHierarchyPanel(std::weak_ptr<WorldContext> p_world);

		void StartFrameUpdate() override;
		void RenderPanels();
		void EndFrameUpdate() override;

		void ForceGameFocus();
		void ForceSceneFocus();

		std::shared_ptr<Panel> CreateNewTestPanel();
		std::shared_ptr<Panel> CreateSavePanel();
		std::shared_ptr<Panel> CreateConsolePanel();
		std::shared_ptr<Panel> CreateContentPanel();
		std::shared_ptr<Panel> CreateInspectorPanel();
		std::shared_ptr<Panel> CreateGamePanel();
		std::shared_ptr<Panel> CreateScenePanel();
		std::shared_ptr<Panel> CreateHierarchyPanel();
		std::shared_ptr<Panel> CreateBuildPanel();

		void TryCreateSavePanel();

		void Layout1(int p_dockspaceId);

		// Inherited via IUI
		ImFont* GetFontDefault() override;
		ImFont* GetIconFont() override;
		ISelectable* GetSelected() override;
		//void SetSelected(ISelectable* p_selected) override;

	private:
		typedef std::shared_ptr<Panel> (EditorUI::* CreatePanelCallback)();
		using Inputs = std::shared_ptr<SvApp::InputManager::InputBindings>;

		MenuBar CreateMenuBar(std::weak_ptr<WorldContext> p_world);
		void HandlePanelFlags(const std::string& p_name, Panel::ERenderFlags p_flags);

		//TODO : add ratio vfont sizes
		static constexpr int DEFAULT_FONT_SIZE = 16;
		static constexpr int ICON_FONT_SIZE = 64;

		std::vector<ImFont*> m_fonts;

		Panels								m_currentPanels;
		std::shared_ptr<MainPanel>			m_main;
		std::vector<CreatePanelCallback>	m_endFrameCallbacks;
		ISelectable*						m_selected;
		Inputs								m_inputs;
	};
}

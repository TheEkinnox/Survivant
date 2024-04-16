//EditorUI.h
#pragma once

#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/UI/Core/IUI.h"
#include "SurvivantEditor/UI/Interfaces/ISelectable.h"
#include "SurvivantEditor/UI/Panels/Panel.h"
#include "SurvivantEditor/UI/Panels/MainPanel.h"
#include "SurvivantEditor/UI/MenuItems/Menu.h"
#include "SurvivantEditor/UI/MenuItems/MenuBar.h"
#include "SurvivantEditor/App/WorldContext.h"
#include "SurvivantApp/Inputs/InputManager.h"


#include <unordered_set>
#include <memory>
#include <array>

//foward declaration
struct ImFont;

namespace SvApp {
	class Window;
}

namespace SvEditor::App
{
	struct WorldContext;
}

namespace SvEditor::UI::Core
{
	using namespace Panels;
	using namespace MenuItems;
	using namespace Interfaces;

	class EditorUI : public IUI
	{
	public:
		using Panels = std::unordered_map<std::string, std::shared_ptr<Panel>>;

		EditorUI(SvApp::InputManager::InputBindings& p_inputs);
		~EditorUI(); 

		void InitEditorUi(SvApp::Window* p_window);

		void InitGamePanel(const App::WorldContext::WorldCreator& p_worldCreator, const std::array<std::function<void()>, 3> p_playPauseFrameCallbacks);
		void InitScenePanel(std::weak_ptr<App::WorldContext> p_world);

		void StartFrameUpdate() override;
		void RenderPanels();
		void EndFrameUpdate() override;

		std::shared_ptr<Panel> CreateNewTestPanel();
		std::shared_ptr<Panel> CreateSavePanel();
		std::shared_ptr<Panel> CreateConsolePanel();
		std::shared_ptr<Panel> CreateContentPanel();
		std::shared_ptr<Panel> CreateInspectorPanel();
		std::shared_ptr<Panel> CreateGamePanel();
		std::shared_ptr<Panel> CreateScenePanel();
		std::shared_ptr<Panel> CreateHierarchyPanel();

		void TryCreateSavePanel();

		void Layout1(int p_dockspaceId);

		// Inherited via IUI
		ImFont* GetFontDefault() override;
		ImFont* GetIconFont() override;
		ISelectable* GetSelected() override;
		void SetSelected(ISelectable* p_selected) override;

	private:
		typedef std::shared_ptr<Panel> (EditorUI::* CreatePanelCallback)();

		MenuBar CreateMenuBar();
		//void DisplayPopupMenu();
		void HandlePanelFlags(const std::string& p_name, Panel::ERenderFlags p_flags);

		//TODO : add ratio vfont sizes
		static constexpr int DEFAULT_FONT_SIZE = 16;
		static constexpr int ICON_FONT_SIZE = 64;

		std::vector<ImFont*> m_fonts;

		Panels									m_currentPanels;
		std::shared_ptr<MainPanel>				m_main;
		std::vector<CreatePanelCallback>		m_endFrameCallbacks;
		ISelectable*							m_selected;
		SvApp::InputManager::InputBindings&		m_inputs;
		bool									m_hasChangedInputs;
	};
}

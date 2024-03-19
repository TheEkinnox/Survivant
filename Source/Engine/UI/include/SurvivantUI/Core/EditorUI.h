//EditorUI.h
#pragma once

#include "SurvivantCore/Events/Event.h"
#include "SurvivantUI/Interfaces/IUI.h"
#include "SurvivantUI/Interfaces/ISelectable.h"
#include "SurvivantUI/Panels/Panel.h"
#include "SurvivantUI/Panels/MainPanel.h"
#include "SurvivantUI/MenuItems/Menu.h"
#include "SurvivantUI/MenuItems/MenuBar.h"

#include <unordered_set>
#include <memory>

//foward declaration
struct ImFont;

namespace App {
	class Window;
}

namespace SvUI::Core
{
	using namespace Panels;
	using namespace MenuItems;
	using namespace Interfaces;

	class EditorUI : public IUI
	{
	public:
		EditorUI();
		~EditorUI(); 

		void InitEditorUi(App::Window* p_window);

		void AddImageWindow(intptr_t p_textureId);

		void StartFrameUpdate() override;
		void RenderPanels();
		void EndFrameUpdate() override;

		void CreateNewTestPanel();
		void TryCreateSavePanel();
		void CreateSavePanel();
		void CreateConsolePanel();
		void CreateContentPanel();
		void CreateInspectorPanel();

		void Layout1(int p_dockspaceId);

		// Inherited via IUI
		ImFont* GetFontDefault() override;
		ImFont* GetIconFont() override;
		ISelectable* GetSelected() override;
		void SetSelected(ISelectable* p_selected) override;

	private:
		typedef void (EditorUI::* EndFrameCallback)();

		MenuBar CreateMenuBar();
		//void DisplayPopupMenu();
		void HandlePanelFlags(std::shared_ptr<Panel> p_id, Panel::ERenderFlags p_flags);

		//TODO : add ratio vfont sizes
		static constexpr int DEFAULT_FONT_SIZE = 16;
		static constexpr int ICON_FONT_SIZE = 64;

		std::vector<ImFont*> m_fonts;

		std::unordered_set<std::shared_ptr<Panel>>		m_currentPanels;
		std::shared_ptr<MainPanel>						m_main;
		std::vector<EndFrameCallback>					m_endFrameCallbacks;
		ISelectable*									m_selected;
	};
}

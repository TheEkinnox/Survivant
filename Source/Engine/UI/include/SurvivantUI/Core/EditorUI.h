//EditorUI.h
#pragma once


#include "SurvivantCore/Events/Event.h"
#include "SurvivantUI/Interfaces/IUI.h"

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

	class EditorUI : public Interfaces::IUI
	{
	public:

		EditorUI();
		~EditorUI(); 

		static ImFont* GetIconFont();

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


	private:
		typedef void (EditorUI::* EndFrameCallback)();

		MenuBar CreateMenuBar();
		void HandlePanelFlags(std::shared_ptr<Panel> p_id, Panel::ERenderFlags p_flags);

		static constexpr int ICON_FONT_SIZE = 200;
		static constexpr int DEFAULT_FONT_SIZE = 15;

		static inline ImFont* s_iconFont = nullptr;

		std::unordered_set<std::shared_ptr<Panel>>		m_currentPanels;
		std::shared_ptr<MainPanel>						m_main;
		std::vector<EndFrameCallback>					m_endFrameCallbacks;
	};
}

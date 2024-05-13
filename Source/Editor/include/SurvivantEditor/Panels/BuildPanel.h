//BuildPanel.h
#pragma once

#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelButtonList.h"
#include "SurvivantEditor/PanelItems/PanelResourceSelector.h"
#include "SurvivantEditor/PanelItems/PanelTextInput.h"
#include "SurvivantEditor/PanelItems/PanelTextBox.h"

#include <SurvivantCore/ECS/Scene.h>

#include <functional>
#include <vector>

namespace SvEditor::Panels
{
	using namespace PanelItems;

	class BuildPanel : public Panel
	{
	public:
		BuildPanel();
		~BuildPanel() = default;

		ERenderFlags Render()override;

		static constexpr char NAME[] = "Build";
	private:
		using SceneSelect = PanelResourceSelector<SvCore::ECS::Scene>;
		using SelectedScenes = std::vector<SceneSelect::Value>;

		void AddScene(const SceneSelect::Value& p_scene);
		void RemoveScene(const SceneSelect::Value& p_scene);

		PanelButtonList		m_buttons;

		SelectedScenes		m_scenes;
		SceneSelect			m_addScene;
		SceneSelect			m_removeScene;
		PanelTextBox		m_scenePaths;

		std::string			m_buildName; //needs to be before m_selectBuildName bcs of constructor
		PanelTextInput		m_selectBuildName;
	};
}
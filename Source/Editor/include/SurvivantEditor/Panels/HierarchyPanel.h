//HierarchyPanel.h
#pragma once

#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelTreeBranch.h"
#include "SurvivantEditor/PanelItems/PanelButton.h"

#include <SurvivantApp/Core/WorldContext.h>
#include <SurvivantCore/Events/Event.h>
#include <SurvivantCore/ECS/EntityHandle.h>
#include <SurvivantCore/ECS/Scene.h>
#include <SurvivantCore/ECS/Components/Hierarchy.h>
#include <SurvivantCore/ECS/Components/TagComponent.h>
#include <SurvivantCore/Utility/UnusedIdGenerator.h>

#include <array>

namespace SvEditor::Panels
{
	using namespace PanelItems;

	class HierarchyPanel : public Panel
	{
	public:
		using HierarchyBranch = PanelTreeBranch<SvCore::ECS::Entity::Index>;
		using EntityBranchMap = std::unordered_map<SvCore::ECS::Entity::Index, std::weak_ptr<HierarchyBranch>>;
		using SceneRefPtr = std::weak_ptr<SvApp::Core::WorldContext::SceneRef>;
		using CurrentSceneGetter = std::function<SceneRefPtr()>;

		HierarchyPanel();
		~HierarchyPanel();

		// Inherited via Panel
		ERenderFlags Render() override;

		static void SetCurrentSceneGetter(const CurrentSceneGetter& p_getCurrentScene);
		static void ToggleSelectable(const SvCore::ECS::Entity::Index& p_entity);

		bool m_isDirty = false;
		static constexpr char NAME[] = "Hierarchy";

	private:
		using EntityListenerId = SvCore::Events::Event<SvCore::ECS::EntityHandle>::ListenerId;
		using HierarchyListenerId = SvCore::Events::Event<
			SvCore::ECS::EntityHandle, SvCore::ECS::HierarchyComponent>::ListenerId;
		using TagComponentListenerId = SvCore::Events::Event<
			SvCore::ECS::EntityHandle, SvCore::ECS::TagComponent>::ListenerId;

		static std::shared_ptr<HierarchyBranch> CreateEntityBranch(const SvCore::ECS::EntityHandle& p_childEntity);

		static inline CurrentSceneGetter	s_getCurrentScene = nullptr;
		static inline EntityBranchMap		s_entities;

		void	RemoveListeners(SvCore::ECS::Scene& p_scene);
		void	SetScene();
		void	SetSceneName();
		void	SetupTree();
		void	SetupEntityBranch(HierarchyBranch& p_parent, const SvCore::ECS::EntityHandle& p_entity);
		void				AddEntityBranch(HierarchyBranch& p_parent, std::shared_ptr<HierarchyBranch> p_childBranch);
		SvCore::ECS::Scene& GetScene();

		std::string			m_sceneName;
		HierarchyBranch		m_tree;
		SceneRefPtr			m_scene;
		PanelButton			m_addEntity;

		std::array<EntityListenerId, 2>			m_onModifEntity;
		std::array<HierarchyListenerId, 3>		m_onModifHierarchy;
		std::array<TagComponentListenerId, 3>	m_onModifTag;

		SvCore::ECS::Entity	m_currentSelected;
	};
}
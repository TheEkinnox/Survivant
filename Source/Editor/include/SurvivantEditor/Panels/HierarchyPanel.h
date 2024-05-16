//HierarchyPanel.h
#pragma once

#include "SurvivantApp/Core/WorldContext.h"
#include "SurvivantCore/Events/Event.h"
#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantCore/ECS/Components/Hierarchy.h"
#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelTreeBranch.h"
#include "SurvivantEditor/PanelItems/PanelButton.h"

#include <array>

namespace SvEditor::Panels
{
	using namespace PanelItems;

	class HierarchyPanel : public Panel
	{
	public:
		using HierarchyBranch = PanelTreeBranch<SvCore::ECS::Entity::Id>;
		using EntityBranchMap = std::unordered_map<SvCore::ECS::Entity::Id, std::shared_ptr<HierarchyBranch>>;
		using SceneRefPtr = std::weak_ptr<SvApp::Core::WorldContext::SceneRef>;
		using CurrentSceneGetter = std::function<SceneRefPtr()>;

		HierarchyPanel();
		~HierarchyPanel();

		// Inherited via Panel
		ERenderFlags Render() override;

		static void SetCurrentSceneGetter(const CurrentSceneGetter& p_getCurrentScene);
		static void ToggleSelectable(const SvCore::ECS::Entity::Id& p_entity);

		bool m_isDirty = false;
		static constexpr char NAME[] = "Hierarchy";

	private:
		using EntityListenerId = SvCore::Events::Event<SvCore::ECS::EntityHandle>::ListenerId;
		using HierarchyListenerId = SvCore::Events::Event<
			SvCore::ECS::EntityHandle, SvCore::ECS::HierarchyComponent>::ListenerId;

		void	UpdateScene();
		void	SetupTree();
		void	SetupEntityBranch(HierarchyBranch& p_parent, const SvCore::ECS::EntityHandle& p_entity);

		void				AddEntityBranch(HierarchyBranch& p_parent, std::shared_ptr<HierarchyBranch> p_childBranch);
		void				RemoveEntity(const SvCore::ECS::EntityHandle& p_entity);
		SvCore::ECS::Scene& GetScene();

		static std::shared_ptr<HierarchyBranch> CreateEntityBranch(const SvCore::ECS::EntityHandle& p_childEntity);

		static inline CurrentSceneGetter	s_getCurrentScene = nullptr;
		static inline EntityBranchMap		s_entities;
		
		HierarchyBranch		m_tree;
		SceneRefPtr			m_scene;
		PanelButton			m_addEntity;

		std::array<EntityListenerId, 2>			m_onModifEntity;
		std::array<HierarchyListenerId, 3>		m_onModifHierarchy;

		std::weak_ptr<HierarchyBranch>	m_currentSelected;
	};
}
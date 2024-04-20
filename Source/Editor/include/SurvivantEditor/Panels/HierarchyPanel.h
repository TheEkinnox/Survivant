//HierarchyPanel.h
#pragma once

#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantCore/Utility/UnusedIdGenerator.h"
#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantEditor/PanelItems/PanelTreeBranch.h"



namespace SvEditor::Panels
{
	using namespace PanelItems;

	class HierarchyPanel : public Panel
	{
	public:
		using HierarchyBranch = PanelTreeBranch<SvCore::ECS::Entity::Id>;
		using EntityBranchMap = std::unordered_map<SvCore::ECS::Entity::Id, std::shared_ptr<HierarchyBranch>>;
		using SceneRef = std::weak_ptr<std::shared_ptr<SvCore::ECS::Scene>>;
		using CurrentSceneGetter = std::function<SceneRef()>;

		HierarchyPanel();
		~HierarchyPanel();

		// Inherited via Panel
		ERenderFlags Render() override;

		static void SetCurrentSceneGetter(CurrentSceneGetter p_getCurrentScene);
		static void SelectSelectable(const SvCore::ECS::Entity::Id& p_entity);

		static constexpr char NAME[] = "Hierarchy";
	private:
		void	UpdateScene();
		void	SetupTree();
		void	SetupEntityBranch(HierarchyBranch& p_parent, const SvCore::ECS::EntityHandle& p_entity);

		void	AddEntityBranch(HierarchyBranch& p_parent, std::shared_ptr<HierarchyBranch> p_childBranch);
		void	RemoveEntity(const SvCore::ECS::EntityHandle& p_entity);

		static std::shared_ptr<HierarchyBranch> CreateEntityBranch(const SvCore::ECS::EntityHandle& p_childEntity);

		static inline CurrentSceneGetter	s_getCurrentScene = nullptr;
		static inline EntityBranchMap		s_entities;

		SvCore::ECS::Scene& GetScene();
		
		HierarchyBranch		m_tree;
		SceneRef			m_scene;
	};
}
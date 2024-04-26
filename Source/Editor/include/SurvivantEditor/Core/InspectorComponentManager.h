//InspectorComponentManager.h
#pragma once

#include "SurvivantCore/ECS/ComponentRegistry.h"
#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/PanelItems/PanelComponent.h"
#include "SurvivantEditor/PanelItems/PanelEntity.h"

#include <memory>

namespace SvEditor::Core
{
	class InspectorComponentManager
	{
	public:
		using PanelableComponent = PanelItems::PanelComponent;
		using PanelableEntity = std::shared_ptr<PanelItems::PanelEntity>;

		InspectorComponentManager() = delete;
		~InspectorComponentManager() = delete;

		static void Init();

		template <class Component>
		static bool AddComponentToPanelable(
			PanelableComponent(*p_toPanelable)(const SvCore::ECS::EntityHandle& p_entity));

		static PanelableEntity GetPanelableEntity(const SvCore::ECS::EntityHandle& p_entity);

		template <class Component>
		static PanelableComponent GetPanelableComponent(const SvCore::ECS::EntityHandle& p_entity);

	private:
		static inline const std::string CREATE_PANELABLE = "CreatePanelable";

		//	REGISTER_COMPONENT_TYPE(Hierarchy, SvCore::ECS::HierarchyComponent)
		//	REGISTER_COMPONENT_TYPE(Tag, SvCore::ECS::TagComponent)
		//	REGISTER_COMPONENT_TYPE(Transform, LibMath::Transform)

		//	REGISTER_COMPONENT_TYPE(Light, SvRendering::Components::LightComponent)
		//	REGISTER_COMPONENT_TYPE(Model, SvRendering::Components::ModelComponent)

		//	REGISTER_COMPONENT_TYPE(Rotator, SvTest::Rotator)
		//	REGISTER_COMPONENT_TYPE(Temporary, SvTest::Temporary)
		//	REGISTER_COMPONENT_TYPE(UserInput, SvTest::UserInput)

		static PanelableComponent AddComponentTransform(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentHierarchy(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentTag(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentLight(const SvCore::ECS::EntityHandle& p_entity);
		//static PanelableComponent AddComponentModel(const SvCore::ECS::EntityHandle& p_entity);
		//static PanelableComponent AddComponentRotator(const SvCore::ECS::EntityHandle& p_entity);
		//static PanelableComponent AddComponentTemporary(const SvCore::ECS::EntityHandle& p_entity);
		//static PanelableComponent AddComponentUserInput(const SvCore::ECS::EntityHandle& p_entity);

		static std::string GetEntityName(const SvCore::ECS::EntityHandle& p_entity);
	};

	template<class Component>
	inline InspectorComponentManager::PanelableComponent InspectorComponentManager::GetPanelableComponent(
		const SvCore::ECS::EntityHandle& p_entity)
	{}

	template<class Component>
	inline bool SvEditor::Core::InspectorComponentManager::AddComponentToPanelable(
		PanelableComponent(*p_toPanelable)(const SvCore::ECS::EntityHandle& p_entity))
	{
		using namespace SvCore::ECS;

		auto& registry = ComponentRegistry::GetInstance();
		registry.RegisterType<Component>(typeid(Component).name());

		auto func = [p_toPanelable](const void* p_payload) {
				return p_toPanelable(*static_cast<const EntityHandle*>(p_payload));
			};

		return registry.GetTypeInfo<Component>().Add(CREATE_PANELABLE, func);
	}
}

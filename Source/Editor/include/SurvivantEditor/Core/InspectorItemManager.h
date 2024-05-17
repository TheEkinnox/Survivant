//InspectorItemManager.h
#pragma once

#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantCore/ECS/ComponentRegistry.h"
#include "SurvivantCore/Resources/ResourceRef.h"
#include "SurvivantEditor/PanelItems/PanelComponent.h"
#include "SurvivantEditor/PanelItems/PanelEntity.h"
#include "SurvivantEditor/PanelItems/PanelResourceDisplay.h"

#include <memory>

namespace SvEditor::Core
{
	class InspectorItemManager
	{
	public:
		using PanelableComponent = std::shared_ptr<PanelItems::PanelComponent>;
		using PanelableEntity = std::shared_ptr<PanelItems::PanelEntity>;
		using PanelableResource = std::shared_ptr<PanelItems::PanelResourceDisplay>;
		using ComponentFunc = PanelableComponent(*)(const SvCore::ECS::EntityHandle& p_entity);
		using ComponentInfo = SvCore::Utility::DynamicTypeInfo<SvCore::ECS::ComponentTypeInfo>;

		InspectorItemManager() = delete;
		~InspectorItemManager() = delete;

		static void Init();

		template <class Component>
		static bool AddComponentToPanelable(
			ComponentFunc p_toPanelable,
			const std::string& p_name = std::string());

		template <class Resource>
		static bool AddResourceToPanelable(
			PanelableResource(*p_toPanelable)(const SvCore::Resources::GenericResourceRef& p_resource),
			const std::string& p_name = std::string());

		static PanelableEntity		GetPanelableEntity(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableResource	GetPanelableResource(const SvCore::Resources::GenericResourceRef& p_resource);
		static PanelableComponent	GetPanelableComponent(const SvCore::Utility::TypeId& p_type, const SvCore::ECS::EntityHandle& p_entity);

		static PanelableComponent	AddPanelableComponent(const ComponentInfo& p_typeInfo, const SvCore::ECS::EntityHandle& p_entity);

	private:
		static inline const std::string CREATE_COMPONENT = "CreateComponentPanelable";
		static inline const std::string ADD_COMPONENT = "AddComponentInEditor";
		static inline const std::string CREATE_RESOURCE = "CreateResourcePanelable";

		static PanelableComponent AddComponentTransform(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentHierarchy(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentCamera(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentTag(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentLight(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentModel(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentScriptList(const SvCore::ECS::EntityHandle& p_entity);

		static PanelableComponent AddComponentRigidBody(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentBoxCollider(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentSphereCollider(const SvCore::ECS::EntityHandle& p_entity);
		static PanelableComponent AddComponentCapsuleCollider(const SvCore::ECS::EntityHandle& p_entity);

		static PanelableResource AddResourceDefault(const SvCore::Resources::GenericResourceRef& p_resource);
		static PanelableResource AddResourceMaterial(const SvCore::Resources::GenericResourceRef& p_resource);

	};

}

#include "SurvivantCore/Resources/ResourceRegistry.h"
#include "SurvivantCore/Resources/ResourceManager.h"
#include "SurvivantEditor/Interfaces/IPanelable.h"

namespace SvEditor::Core
{
	template<class Component>
	inline bool SvEditor::Core::InspectorItemManager::AddComponentToPanelable(
		ComponentFunc p_toPanelable,
		const std::string& p_name)
	{
		using namespace SvCore::ECS;

		auto& registry = ComponentRegistry::GetInstance();
		registry.RegisterType<Component>(p_name.empty() ? typeid(Component).name() : p_name);
		auto& type = registry.GetTypeInfo<Component>();

		auto func1 = [p_toPanelable, type](const void* p_payload) {
			auto entity = *static_cast<const EntityHandle*>(p_payload);
			entity.Make<Component>();
			return InspectorItemManager::GetPanelableComponent(type.m_typeId, entity);
			};

		auto func2 = [p_toPanelable](const void* p_payload) {
				return p_toPanelable(*static_cast<const EntityHandle*>(p_payload));
			};

		bool res =	type.Add(ADD_COMPONENT, func1);
		res &=		type.Add(CREATE_COMPONENT, func2);
		return res;
	}

	template<class Resource>
	inline bool InspectorItemManager::AddResourceToPanelable(
		PanelableResource(*p_toPanelable)(const SvCore::Resources::GenericResourceRef& p_resource),
		const std::string& p_name)
	{
		using namespace SvCore::Resources;

		auto& registry = ResourceRegistry::GetInstance();
		registry.RegisterType<Resource>(p_name.empty() ? typeid(Resource).name() : p_name);

		auto func = [p_toPanelable](const void* p_payload) {
			return p_toPanelable(*static_cast<const GenericResourceRef*>(p_payload));
			};

		return registry.GetTypeInfo<Resource>().Add(CREATE_RESOURCE, func);
	}
}

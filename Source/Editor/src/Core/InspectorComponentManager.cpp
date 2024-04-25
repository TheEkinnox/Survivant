//InspectorComponentManager.cpp
#include "SurvivantEditor/Core/InspectorComponentManager.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/ECS/Components/Hierarchy.h"
#include "SurvivantCore/ECS/Components/TagComponent.h"
#include "SurvivantRendering/Components/CameraComponent.h"
#include "SurvivantRendering/Components/LightComponent.h"
#include "SurvivantRendering/Components/ModelComponent.h"

#include "SurvivantEditor/PanelItems/PanelComponent.h"
#include "SurvivantEditor/PanelItems/PanelTransformInput.h"

#include "Transform.h"

using namespace LibMath;
using namespace SvCore::ECS;
using namespace SvEditor::PanelItems;
using namespace SvRendering::Components;



namespace SvEditor::Core
{
	void InspectorComponentManager::Init()
	{
		AddComponentToPanelable<Transform>(&AddComponentTransform);
		//AddComponent(HierarchyComponent, &AddComponentTransform);
	}

	void InspectorComponentManager::TestImplementComponent()
	{
		const auto printHierarchy = [](const void* p_payload)
			{
				ASSERT(p_payload);

				const HierarchyComponent& hierarchy = *static_cast<const HierarchyComponent*>(p_payload);
				SV_LOG("Called injected func for child of {%u:%u}",
					hierarchy.GetParent().GetIndex(), hierarchy.GetParent().GetVersion());

				return true;
			};

		CHECK(ComponentRegistry::GetInstance().GetTypeInfo<HierarchyComponent>().Add("Print", printHierarchy));
	}

	InspectorComponentManager::PanelableEntity InspectorComponentManager::GetPanelableEntity(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		auto& registry = ComponentRegistry::GetInstance();
		auto components = p_entity.GetComponents();

		std::vector<PanelableComponent> panelables;
		panelables.reserve(components.size());

		for (auto& [type, component] : components)
		{
			if (!registry.Contains(type))
				continue;

			auto& info = registry.GetTypeInfo(type);

			if (info.Has(CREATE_PANELABLE))
			{
				auto val = info.Call<PanelableComponent>(CREATE_PANELABLE, (void*)&p_entity);
				
				if (val.has_value())
					panelables.emplace_back(PanelableComponent(val.value()));
			}
		}

		return std::make_shared<PanelItems::PanelEntity>(std::move(PanelItems::PanelEntity(
			p_entity, panelables)));
	}

	InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentTransform(
		const SvCore::ECS::EntityHandle& p_entity)
	{

		auto component = PanelComponent("Transform",
			PanelComponent::Items({
					std::make_shared<PanelTransformInput>(
						PanelTransformInput::GetRefFunc(
							[e = p_entity]() mutable -> Transform&
							{
								return *e.Get<Transform>();
							})
						//does callback already in PanelTransformInput with ref
						/*PanelTransformInput::Callback( 
							[entity](PanelTransformInput::CallbackParams p_params) mutable
							{
								auto& trans = *entity.Get<Transform>();
								auto& [pos, rot, scl] = p_params;

								if (pos)
									trans.setPosition(*pos);
								if (rot)
									trans.setRotation(*rot);
								if (scl)
									trans.setScale(*scl);
							})*/	
				)}));

		return PanelItems::PanelComponent(std::move(component));
	}

	//InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentHierarchy(const SvCore::ECS::EntityHandle& p_entity)
	//{


	//	//auto component = PanelComponent("",
	//	//	PanelComponent::Items({
	//	//			std::make_shared<Panel>(
	//	//				PanelTransformInput::GetRefFunc(
	//	//					[e = p_entity]() mutable -> HierarchyComponent&
	//	//					{
	//	//						return *e.Get<HierarchyComponent>();
	//	//					})
	//	//				//does callback already in PanelTransformInput with ref
	//	//				/*PanelTransformInput::Callback(
	//	//					[entity](PanelTransformInput::CallbackParams p_params) mutable
	//	//					{
	//	//						auto& trans = *entity.Get<Transform>();
	//	//						auto& [pos, rot, scl] = p_params;

	//	//						if (pos)
	//	//							trans.setPosition(*pos);
	//	//						if (rot)
	//	//							trans.setRotation(*rot);
	//	//						if (scl)
	//	//							trans.setScale(*scl);
	//	//					})*/
	//	//		) }));

	//	//return PanelableComponent();
	//}

	//InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentTag(const SvCore::ECS::EntityHandle& p_entity)
	//{
	//	return PanelableComponent();
	//}

	//InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentLight(const SvCore::ECS::EntityHandle& p_entity)
	//{
	//	return PanelableComponent();
	//}

	//InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentModel(const SvCore::ECS::EntityHandle& p_entity)
	//{
	//	return PanelableComponent();
	//}

	//InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentRotator(const SvCore::ECS::EntityHandle& p_entity)
	//{
	//	return PanelableComponent();
	//}

	//InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentTemporary(const SvCore::ECS::EntityHandle& p_entity)
	//{
	//	return PanelableComponent();
	//}

	//InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentUserInput(const SvCore::ECS::EntityHandle& p_entity)
	//{
	//	return PanelableComponent();
	//}
}


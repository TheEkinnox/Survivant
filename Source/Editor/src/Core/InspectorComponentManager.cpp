//InspectorComponentManager.cpp
#include "SurvivantEditor/Core/InspectorComponentManager.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/ECS/Components/Hierarchy.h"
#include "SurvivantCore/ECS/Components/TagComponent.h"
#include "SurvivantRendering/Components/CameraComponent.h"
#include "SurvivantRendering/Components/LightComponent.h"
#include "SurvivantRendering/Components/ModelComponent.h"

#include "SurvivantEditor/PanelItems/PanelButton.h"
#include "SurvivantEditor/PanelItems/PanelColorInput.h"
#include "SurvivantEditor/PanelItems/PanelComponent.h"
#include "SurvivantEditor/PanelItems/PanelIntInput.h"
#include "SurvivantEditor/PanelItems/PanelSelectionDisplay.h"
#include "SurvivantEditor/PanelItems/PanelTextInput.h"
#include "SurvivantEditor/PanelItems/PanelUInt32Input.h"
#include "SurvivantEditor/PanelItems/PanelTransformInput.h"
#include "SurvivantEditor/PanelItems/PanelVec2Input.h"
#include "SurvivantEditor/PanelItems/PanelVec3Input.h"

#include "Transform.h"

using namespace LibMath;
using namespace SvCore::ECS;
using namespace SvEditor::PanelItems;
using namespace SvRendering::Components;
using namespace SvRendering::Enums;



namespace SvEditor::Core
{
	void InspectorComponentManager::Init()
	{
		AddComponentToPanelable<Transform>(&AddComponentTransform);
		AddComponentToPanelable<HierarchyComponent>(&AddComponentHierarchy);
		AddComponentToPanelable<TagComponent>(&AddComponentTag);
		AddComponentToPanelable<LightComponent>(&AddComponentLight);
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
		static size_t Prio = 2;

		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<Transform>(),
			PanelComponent::Items({
					std::make_shared<PanelTransformInput>(PanelTransformInput(
						PanelTransformInput::GetRefFunc(
							[e = p_entity]() mutable -> Transform&
							{
								return *e.Get<Transform>();
							}))
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
				)}),
			Prio);

		return component;
	}

	InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentHierarchy(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<HierarchyComponent>(),
			PanelComponent::Items({
					std::make_shared<PanelUInt32Input>(PanelUInt32Input(
						"Parent      ",
						PanelUInt32Input::GetCopyFunc([entity = p_entity]() mutable -> uint32_t { return
							static_cast<uint32_t>(entity.Get<HierarchyComponent>()->GetParent().GetIndex()); }),
						PanelUInt32Input::Callback([entity = p_entity](const uint32_t& /*p_index*/) mutable {
							/*entity.GetScene().*/
							/*entity.SetParent();*/ })
					)),
				//std::make_shared<PanelTextInput>(PanelTextInput(
				//	"Parent        ",
				//	PanelTextInput::GetRefFunc([entity = p_entity]() mutable -> std::string& {
				//		static std::string val;

				//		auto parent = entity.Get<HierarchyComponent>()->GetParent();
				//		if (!parent || parent == NULL_ENTITY)
				//			val = "None";
				//		else
				//			val = GetEntityName(EntityHandle(entity.GetScene(), parent));
				//		return val; }),
				//	PanelTextInput::Callback()
				//	)),
				std::make_shared<PanelUInt32Input>(PanelUInt32Input(
					"Child Count ",
					PanelIntInput::GetCopyFunc([entity = p_entity]() mutable -> uint32_t { return
						static_cast<uint32_t>(entity.Get<HierarchyComponent>()->GetChildCount()); }),
					PanelIntInput::Callback()
				)),
				std::make_shared<PanelButton>(PanelButton(
					"Remove Parent",
					PanelButton::OnButtonPressEvent::EventDelegate([entity = p_entity]() mutable {
						entity.SetParent(EntityHandle()); })
					))
				}));

		return component;
	}

	InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentTag(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		static size_t Prio = 3;

		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<TagComponent>(),
			PanelComponent::Items({
					std::make_shared<PanelTextInput>(PanelTextInput(
						"Name ",
						[entity = p_entity]() mutable -> std::string& { return 
							entity.Get<TagComponent>()->m_tag; }
					))
				}),
			Prio);

		return component;
	}

	InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentLight(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		using namespace SvRendering::Core;

		std::vector<std::string> enumNames;
		for (int i = 0; i <= static_cast<int>(ELightType::SPOT); i++)
			enumNames.emplace_back(std::string(LightTypeToString(ELightType(i))));

		std::vector<PanelSelectionDisplay::SelectedDisplay> display;

		//Core::Light            m_ambient;
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
				std::make_shared<PanelColorInput>(PanelColorInput(
					"Color ", 
					[entity = p_entity]() mutable -> Vector4& { return //cast from (float[4])[0] to &Vec4
						*(Vector4*)&(entity.Get<LightComponent>()->m_ambient.m_color.m_r); }
				))
			}));

		//Core::DirectionalLight m_directional;
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
				std::make_shared<PanelColorInput>(PanelColorInput(
					"Color     ",
					[entity = p_entity]() mutable -> Vector4& { return //cast from (float[4])[0] to &Vec4
						*(Vector4*)&(entity.Get<LightComponent>()->m_directional.m_color); }
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Direction ",
					[entity = p_entity]() mutable -> Vector3& { return
						entity.Get<LightComponent>()->m_directional.m_direction; }
				))
		}));

		//Core::PointLight       m_point;
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
				std::make_shared<PanelColorInput>(PanelColorInput(
					"Color       ",
					[entity = p_entity]() mutable -> Vector4&{ return //cast from (float[4])[0] to &Vec4
						*(Vector4*)&(entity.Get<LightComponent>()->m_point.m_color); }
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Position    ",
					[entity = p_entity]() mutable -> Vector3& { return
						entity.Get<LightComponent>()->m_point.m_position; }
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Attenuation ",
					[entity = p_entity]() mutable -> LibMath::Vector3& { return //cast from (float[3])[0] to &Vec3
						*(Vector3*)&(entity.Get<LightComponent>()->m_point.m_attenuationData.m_constant); }
				))
			}));

		//Core::SpotLight        m_spot;
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
				std::make_shared<PanelColorInput>(PanelColorInput(
					"Color       ",
					[entity = p_entity]() mutable -> Vector4& { return //cast from (float[4])[0] to &Vec4
						*(Vector4*)&(entity.Get<LightComponent>()->m_spot.m_color); }
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Position    ",
					[entity = p_entity]() mutable -> Vector3& { return
						entity.Get<LightComponent>()->m_spot.m_position; }
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Direction   ",
					[entity = p_entity]() mutable -> Vector3& { return
						entity.Get<LightComponent>()->m_spot.m_direction; }
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Attenuation ",
					[entity = p_entity]() mutable -> LibMath::Vector3& { return //cast from (float[3])[0] to &Vec3
						*(Vector3*)&(entity.Get<LightComponent>()->m_spot.m_attenuationData.m_constant); }
				)),
				std::make_shared<PanelVec2Input>(PanelVec2Input(
					"Cutoff      ",
					[entity = p_entity]() mutable -> LibMath::Vector2& { return //cast from (float[2])[0] to &Vec2
						*(Vector2*)&(entity.Get<LightComponent>()->m_spot.m_cutoff.m_inner); }
				))
			}));

		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<LightComponent>(),
			PanelComponent::Items({
					std::make_shared<PanelSelectionDisplay>(PanelSelectionDisplay(
						"Type ", enumNames, display, 
						[p_entity]() -> int { return static_cast<int>( //copy enum to int
							p_entity.Get<LightComponent>()->m_type); },
						[entity = p_entity] (const int& p_val) mutable { //set enum
							entity.Get<LightComponent>()->m_type = static_cast<ELightType>(p_val); })
				) }));

		return component;
	}

	//InspectorComponentManager::PanelableComponent InspectorComponentManager::AddComponentModel(
	//	const SvCore::ECS::EntityHandle& /*p_entity*/)
	//{
	//	//auto component = PanelComponent("Light",
	//	//	PanelComponent::Items({
	//	//			std::make_shared<PanelSelectionDisplay>(PanelSelectionDisplay(
	//	//				"Type ", enumNames, display,
	//	//				[p_entity]() -> int { return static_cast<int>( //copy enum to int
	//	//					p_entity.Get<LightComponent>()->m_type); },
	//	//				[entity = p_entity](const int& p_val) mutable { //set enum
	//	//					entity.Get<LightComponent>()->m_type = static_cast<ELightType>(p_val); })
	//	//		) }));

	//	//return component;
	//}

	std::string InspectorComponentManager::GetEntityName(const SvCore::ECS::EntityHandle& p_entity)
	{
		auto val = p_entity.Get<TagComponent>();
		if (val)
			return val->m_tag + ("(" + std::to_string(p_entity.GetEntity().GetIndex()) + ')');

		return "(" + std::to_string(p_entity.GetEntity().GetIndex()) + ')';
	}
}


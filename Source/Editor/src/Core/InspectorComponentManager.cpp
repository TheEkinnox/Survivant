//InspectorItemManager.cpp
#include "SurvivantEditor/Core/InspectorComponentManager.h"

#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/ECS/Components/Hierarchy.h"
#include "SurvivantCore/ECS/Components/TagComponent.h"
#include "SurvivantRendering/Components/CameraComponent.h"
#include "SurvivantRendering/Components/LightComponent.h"
#include "SurvivantRendering/Components/ModelComponent.h"
#include "SurvivantRendering/Resources/Material.h"
#include "SurvivantRendering/Resources/Mesh.h"
#include "SurvivantRendering/Resources/Model.h"
#include "SurvivantRendering/RHI/IShader.h"
#include "SurvivantRendering/RHI/ITexture.h"
#include "SurvivantScripting/LuaScript.h"

#include "SurvivantEditor/PanelItems/PanelButton.h"
#include "SurvivantEditor/PanelItems/PanelColorInput.h"
#include "SurvivantEditor/PanelItems/PanelComponent.h"
#include "SurvivantEditor/PanelItems/PanelIntInput.h"
#include "SurvivantEditor/PanelItems/PanelResourceSelector.h"
#include "SurvivantEditor/PanelItems/PanelSelectionDisplay.h"
#include "SurvivantEditor/PanelItems/PanelTextBox.h"
#include "SurvivantEditor/PanelItems/PanelTextInput.h"
#include "SurvivantEditor/PanelItems/PanelTransformInput.h"
#include "SurvivantEditor/PanelItems/PanelUInt32Input.h"
#include "SurvivantEditor/PanelItems/PanelVec2Input.h"
#include "SurvivantEditor/PanelItems/PanelVec3Input.h"

#include "Transform.h"

using namespace LibMath;
using namespace SvCore::ECS;
using namespace SvEditor::PanelItems;
using namespace SvRendering::Components;
using namespace SvRendering::Enums;
using namespace SvRendering::Resources;
using namespace SvRendering::RHI;
using namespace SvScripting;

namespace SvEditor::Core
{
	void InspectorItemManager::Init()
	{
		//components
		CHECK(AddComponentToPanelable<Transform>(&AddComponentTransform, "Transform"),
			"Couldn't add ComponentToPanelable callback to type : Transform");
		CHECK(AddComponentToPanelable<HierarchyComponent>(&AddComponentHierarchy, "Hierarchy"),
			"Couldn't add ComponentToPanelable callback to type : Hierarchy");
		CHECK(AddComponentToPanelable<TagComponent>(&AddComponentTag, "Tag"),
			"Couldn't add ComponentToPanelable callback to type : Tag");
		CHECK(AddComponentToPanelable<LightComponent>(&AddComponentLight, "Light"),
			"Couldn't add ComponentToPanelable callback to type : Light");
		CHECK(AddComponentToPanelable<ModelComponent>(&AddComponentModel, "Model"),
			"Couldn't add ComponentToPanelable callback to type : Model");
	
		CHECK(AddResourceToPanelable<Model>(&AddResourceDefault, "Model"),
			"Couldn't add ResourceToPanelable callback to type : Model");
		CHECK(AddResourceToPanelable<Material>(&AddResourceDefault, "Material"),
			"Couldn't add ResourceToPanelable callback to type : Material");
		CHECK(AddResourceToPanelable<LuaScript>(&AddResourceDefault, "Script"),
			"Couldn't add ResourceToPanelable callback to type : Script");
		CHECK(AddResourceToPanelable<IShader>(&AddResourceDefault, "Shader"),
			"Couldn't add ResourceToPanelable callback to type : Shader");
		CHECK(AddResourceToPanelable<ITexture>(&AddResourceDefault, "Texture"),
			"Couldn't add ResourceToPanelable callback to type : Texture");
	}

	InspectorItemManager::PanelableEntity InspectorItemManager::GetPanelableEntity(
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

			if (info.Has(CREATE_COMPONENT))
			{
				auto val = info.Call<PanelableComponent>(CREATE_COMPONENT, (void*)&p_entity);
				
				if (val.has_value())
					panelables.emplace_back(val.value());
			}
		}

		return std::make_shared<PanelItems::PanelEntity>(std::move(PanelItems::PanelEntity(
			p_entity, panelables)));
	}

	InspectorItemManager::PanelableResource InspectorItemManager::GetPanelableResource(
		const SvCore::Resources::GenericResourceRef& p_resource)
	{
		using namespace SvCore::Resources;
		auto& registry = ResourceRegistry::GetInstance();
		//auto& rm = ResourceManager::GetInstance();

		if (!registry.Contains(p_resource.GetType()))
			return AddResourceDefault(p_resource);

		auto& info = registry.GetTypeInfo(p_resource.GetType());
		PanelableResource panel = nullptr;

		if (info.Has(CREATE_RESOURCE))
			panel = info.Call<PanelableResource>(CREATE_COMPONENT, (void*)&p_resource).value_or(nullptr);

		return panel;
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentTransform(
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
				)}),
			Prio);

		return std::make_shared<PanelComponent>(std::move(component));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentHierarchy(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<HierarchyComponent>(),
			PanelComponent::Items({
					std::make_shared<PanelUInt32Input>(PanelUInt32Input(
						"Parent      ",
						PanelUInt32Input::GetCopyFunc([entity = p_entity]() mutable -> uint32_t { return
							static_cast<uint32_t>(entity.Get<HierarchyComponent>()->GetParent().GetIndex()); }),
						PanelUInt32Input::Callback([entity = p_entity](const uint32_t& p_index) mutable {
							entity.SetParent(entity.GetScene()->Find(static_cast<Entity::Index>(p_index)));  })
					)),
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

		return std::make_shared<PanelComponent>(std::move(component));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentTag(
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

		return std::make_shared<PanelComponent>(std::move(component));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentLight(
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

		return std::make_shared<PanelComponent>(std::move(component));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentModel(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		using namespace SvRendering::Resources;
		using namespace SvCore::Resources;

		//auto& ref = *p_entity.Get<ModelComponent>();

		auto component = PanelComponent("Model",
			PanelComponent::Items({
					std::make_shared<PanelResourceSelector<Model>>(PanelResourceSelector<Model>(
						"Model    ", [entity = p_entity]() mutable -> ResourceRef<Model>& { return
							entity.Get<ModelComponent>()->m_model; }
					)),
					std::make_shared<PanelResourceSelector<Material>>(PanelResourceSelector<Material>(
						"Material ", [entity = p_entity]() mutable -> ResourceRef<Material>&{ return
							entity.Get<ModelComponent>()->m_material; }
					))
				}));

		return std::make_shared<PanelComponent>(std::move(component));
	}

	InspectorItemManager::PanelableResource InspectorItemManager::AddResourceMaterial(
		const SvCore::Resources::GenericResourceRef& p_resource)
	{
		using namespace SvRendering::Resources;
		using namespace SvRendering::RHI;
		using namespace SvCore::Resources;

		ResourceRef<Material> resource = p_resource;

		auto component = PanelResourceDisplay(p_resource, "Ma",
			PanelResourceDisplay::Items({
					std::make_shared<PanelResourceSelector<IShader>>(PanelResourceSelector<IShader>(
						"Material ", [resource]() mutable -> ResourceRef<IShader>&{
							static ResourceRef<IShader> ref;
							*ref = resource->GetShader();
							return ref; },
						[resource](PanelResourceSelector<IShader>::CallbackParams p_params) {
							resource->SetShader(p_params);
						}
					))
				}));

		return std::make_shared<PanelResourceDisplay>(std::move(component));
	}

	InspectorItemManager::PanelableResource InspectorItemManager::AddResourceDefault(
		const SvCore::Resources::GenericResourceRef& p_resource)
	{
		using namespace SvRendering::Resources;
		using namespace SvRendering::RHI;
		using namespace SvCore::Resources;
		using namespace SvCore::Serialization;

		rapidjson::StringBuffer buffer;
		JsonWriter writer(buffer);
		p_resource.ToJson(writer);

		std::string rawJson = buffer.GetString();

		auto component = PanelResourceDisplay(p_resource, "Re",
			PanelResourceDisplay::Items({
					std::make_shared<PanelTextDisplay>(PanelTextDisplay(
						std::make_shared<PanelTextDisplay::DefaultText>(PanelTextDisplay::DefaultText(
							rawJson
						))
					))
				}));

		return std::make_shared<PanelResourceDisplay>(std::move(component));
	}

	std::string InspectorItemManager::GetEntityName(const SvCore::ECS::EntityHandle& p_entity)
	{
		auto val = p_entity.Get<TagComponent>();
		if (val)
			return val->m_tag + ("(" + std::to_string(p_entity.GetEntity().GetIndex()) + ')');

		return "(" + std::to_string(p_entity.GetEntity().GetIndex()) + ')';
	}
}


//InspectorItemManager.cpp
#include "SurvivantEditor/Core/InspectorItemManager.h"

#include "SurvivantApp/Core/IEngine.h"
#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/ECS/Components/Hierarchy.h"
#include "SurvivantCore/ECS/Components/TagComponent.h"
#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantRendering/Components/CameraComponent.h"
#include "SurvivantRendering/Components/LightComponent.h"
#include "SurvivantRendering/Components/ModelComponent.h"
#include "SurvivantRendering/Resources/Material.h"
#include "SurvivantRendering/Resources/Model.h"
#include "SurvivantRendering/RHI/IShader.h"
#include "SurvivantRendering/RHI/ITexture.h"
#include "SurvivantScripting/LuaScriptList.h"

#include "SurvivantEditor/Panels/HierarchyPanel.h"
#include "SurvivantEditor/PanelItems/PanelButton.h"
#include "SurvivantEditor/PanelItems/PanelColorInput.h"
#include "SurvivantEditor/PanelItems/PanelComponent.h"
#include "SurvivantEditor/PanelItems/PanelFloatInput.h"
#include "SurvivantEditor/PanelItems/PanelIntInput.h"
#include "SurvivantEditor/PanelItems/PanelMultipleSelection.h"
#include "SurvivantEditor/PanelItems/PanelResourceSelector.h"
#include "SurvivantEditor/PanelItems/PanelSelectionDisplay.h"
#include "SurvivantEditor/PanelItems/PanelTextBox.h"
#include "SurvivantEditor/PanelItems/PanelTextInput.h"
#include "SurvivantEditor/PanelItems/PanelTransformInput.h"
#include "SurvivantEditor/PanelItems/PanelUInt32Input.h"
#include "SurvivantEditor/PanelItems/PanelVec2Input.h"
#include "SurvivantEditor/PanelItems/PanelVec3Input.h"
#include "SurvivantEditor/PanelItems/PanelCheckbox.h"
#include "SurvivantEditor/PanelItems/PanelDoubleInput.h"
#include "SurvivantEditor/PanelItems/PanelSeparator.h"

#include "SurvivantScripting/LuaScriptList.h"
#include "SurvivantScripting/LuaTypeRegistry.h"

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
		//resources
		CHECK(AddResourceToPanelable<Scene>(&AddResourceDefault, "Scene"),			"Couldn't init resource type : Scene");
		CHECK(AddResourceToPanelable<Model>(&AddResourceDefault, "Model"),			"Couldn't init resource type : Model");
		CHECK(AddResourceToPanelable<Material>(&AddResourceDefault, "Material"),	"Couldn't init resource type : Material");
		CHECK(AddResourceToPanelable<LuaScript>(&AddResourceDefault, "Script"),		"Couldn't init resource type : Script");
		CHECK(AddResourceToPanelable<IShader>(&AddResourceDefault, "Shader"),		"Couldn't init resource type : Shader");
		CHECK(AddResourceToPanelable<ITexture>(&AddResourceDefault, "Texture"),		"Couldn't init resource type : Texture");

		//components
		CHECK(AddComponentToPanelable<Transform>(&AddComponentTransform, "Transform"),				"Couldn't init component type : Transform");
		CHECK(AddComponentToPanelable<HierarchyComponent>(&AddComponentHierarchy, "Hierarchy"),		"Couldn't init component type : Hierarchy");
		CHECK(AddComponentToPanelable<TagComponent>(&AddComponentTag, "Tag"),						"Couldn't init component type : Tag");
		CHECK(AddComponentToPanelable<LightComponent>(&AddComponentLight, "Light"),					"Couldn't init component type : Light");
		CHECK(AddComponentToPanelable<ModelComponent>(&AddComponentModel, "Model"),					"Couldn't init component type : Model");
		CHECK(AddComponentToPanelable<CameraComponent>(&AddComponentCamera, "Camera"),				"Couldn't init component type : Camera");
		CHECK(AddComponentToPanelable<LuaScriptList>(&AddComponentScriptList, "ScriptList"),		"Couldn't init component type : ScriptList");
	}

	InspectorItemManager::PanelableEntity InspectorItemManager::GetPanelableEntity(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		//auto& registry = ComponentRegistry::GetInstance();
		auto components = p_entity.GetComponents();

		std::vector<PanelableComponent> panelables;
		panelables.reserve(components.size());

		for (auto& [type, component] : components)
		{
			auto panel = GetPanelableComponent(type, p_entity);
			if (panel)
				panelables.emplace_back(panel);
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
			panel = info.Call<PanelableResource>(CREATE_RESOURCE, (void*)&p_resource).value_or(nullptr);

		return panel;
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddPanelableComponent(
		const ComponentInfo& p_typeInfo, const SvCore::ECS::EntityHandle& p_entity)
	{
		PanelableComponent panel = nullptr;

		if (p_typeInfo.Has(ADD_COMPONENT))
			panel = p_typeInfo.Call<PanelableComponent>(ADD_COMPONENT, (void*)&p_entity).value_or(nullptr);

		return panel;
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::GetPanelableComponent(const SvCore::Utility::TypeId& p_type, const SvCore::ECS::EntityHandle& p_entity)
	{
		auto& registry = ComponentRegistry::GetInstance();

		if (!registry.Contains(p_type))
			return nullptr;

		auto& info = registry.GetTypeInfo(p_type);
		PanelableComponent panel = nullptr;

		if (info.Has(CREATE_COMPONENT))
			panel = info.Call<PanelableComponent>(CREATE_COMPONENT, (void*)&p_entity).value_or(nullptr);

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
		using namespace SvEditor::Panels;

		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<HierarchyComponent>(),
			PanelComponent::Items({
				std::make_shared<PanelUInt32Input>(PanelUInt32Input(
					"Parent      ",
					PanelUInt32Input::GetCopyFunc([entity = p_entity]() mutable -> uint32_t { return
						static_cast<uint32_t>(entity.Get<HierarchyComponent>()->GetParent().GetIndex()); }),
					PanelUInt32Input::Callback([entity = p_entity](const uint32_t& p_index) mutable {
						entity.SetParent(entity.GetScene()->Find(static_cast<Entity::Index>(p_index)));
						//HierarchyPanel::s_isDirty = true;
						})
				)),
				std::make_shared<PanelButton>(PanelButton(
					"Remove Parent",
					PanelButton::OnButtonPressEvent::EventDelegate([entity = p_entity]() mutable {
						entity.SetParent(EntityHandle());
						//HierarchyPanel::s_isDirty = true;
						})
					))
				}));

		return std::make_shared<PanelComponent>(std::move(component));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentCamera(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		std::vector<std::string> enumNames;
		for (int i = 0; i <= static_cast<int>(EProjectionType::ORTHOGRAPHIC); i++)
			enumNames.emplace_back(std::string(ProjectionTypeToString(EProjectionType(i))));

		std::vector<PanelSelectionDisplay::SelectedDisplay> display;

		//Perspective
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
				std::make_shared<PanelFloatInput>(PanelFloatInput(
					"FovY     ",
					[entity = p_entity]() mutable -> float { return
						entity.Get<CameraComponent>()->GetFovY().degree(true); },
					[entity = p_entity](const float& p_val) mutable {
						entity.Get<CameraComponent>()->SetFovY(LibMath::Degree(std::fmodf(p_val, 360))); }
				)),
				std::make_shared<PanelVec2Input>(PanelVec2Input(
					"Near/Far ",
					[entity = p_entity]() mutable -> Vector2 { return Vector2(
						entity.Get<CameraComponent>()->GetPerspectiveNear(),
						entity.Get<CameraComponent>()->GetPerspectiveFar()); },
					[entity = p_entity](const Vector2& p_val) mutable {
						entity.Get<CameraComponent>()->SetPerspectiveNear(p_val.m_x);
						entity.Get<CameraComponent>()->SetPerspectiveFar(p_val.m_y); }
				))
			}));

		//Orthographic
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
		std::make_shared<PanelFloatInput>(PanelFloatInput(
			"Size     ",
			[entity = p_entity]() mutable -> float { return
				entity.Get<CameraComponent>()->GetOrthographicSize(); },
			[entity = p_entity](const float& p_val) mutable {
				entity.Get<CameraComponent>()->SetOrthographicSize(p_val); }
		)),
		std::make_shared<PanelVec2Input>(PanelVec2Input(
			"Near/Far ",
			[entity = p_entity]() mutable -> Vector2 { return Vector2(
				entity.Get<CameraComponent>()->GetOrthographicNear(),
				entity.Get<CameraComponent>()->GetOrthographicFar()); },
			[entity = p_entity](const Vector2& p_val) mutable {
				entity.Get<CameraComponent>()->SetOrthographicNear(p_val.m_x);
				entity.Get<CameraComponent>()->SetOrthographicFar(p_val.m_y); }
		))
			}));

		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<CameraComponent>(),
			PanelComponent::Items({
				std::make_shared<PanelMultipleSelection>(PanelMultipleSelection(
					"Clear Mask   ", { "Color", "Depth", "Stencil" },
					[entity = p_entity]() -> int {
							return static_cast<int>(entity.Get<CameraComponent>()->GetClearMask());
					},
					[entity = p_entity](const int& p_val) mutable {
							entity.Get<CameraComponent>()->SetClearMask(static_cast<uint8_t>(p_val));
					}
				)),
				std::make_shared<PanelColorInput>(PanelColorInput(
					"Color        ",
					[entity = p_entity]() mutable -> Vector4 { return static_cast<Vector4>(
						entity.Get<CameraComponent>()->GetClearColor()); },
					[entity = p_entity](const Vector4& p_value) mutable {
						entity.Get<CameraComponent>()->SetClearColor(p_value); }
				)),
				std::make_shared<PanelUInt32Input>(PanelUInt32Input(
					"Culling Mask ",
					[entity = p_entity]() mutable -> uint32_t { return static_cast<uint32_t>(
						entity.Get<CameraComponent>()->GetCullingMask()); },
					[entity = p_entity](const uint32_t& p_value) mutable {
						entity.Get<CameraComponent>()->SetCullingMask(p_value); }
				)),
				std::make_shared<PanelSelectionDisplay>(PanelSelectionDisplay(
					"Type         ", enumNames, display,
					[p_entity]() -> int { return static_cast<int>( //copy enum to int
							p_entity.Get<CameraComponent>()->GetProjectionType()); },
					[entity = p_entity](const int& p_val) mutable { //set enum
						entity.Get<CameraComponent>()->SetProjectionType(static_cast<EProjectionType>(p_val)); }
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
					std::make_shared<PanelButton>(PanelButton(
						"Bake Lighting",
						[]() { SV_ENGINE()->BakeLights(); }
					)),
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

		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<ModelComponent>(),
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

	namespace
	{
		bool AddLuaUserTypeToPanel(
			PanelComponent::Items& p_items, sol::table p_table, const std::string& p_key, const std::string& p_displayName)
		{
			using namespace SvCore::Utility;
			const std::string typeString = p_table[p_key]["__type"]["name"];

			if (typeString.empty())
				return false;

			const auto&  luaTypes = LuaTypeRegistry::GetInstance();
			const TypeId typeId   = luaTypes.GetRegisteredTypeId(typeString);

			if (typeId == 0)
				return false;

			if (typeId == LuaTypeRegistry::GetTypeId<Vector2>())
			{
				p_items.emplace_back(std::make_shared<PanelVec2Input>(p_displayName,
					[p_table, p_key]() mutable -> Vector2&
					{
						return *p_table[p_key].get<Vector2*>();
					}
				));

				return true;
			}

			if (typeId == LuaTypeRegistry::GetTypeId<Vector3>())
			{
				p_items.emplace_back(std::make_shared<PanelVec3Input>(p_displayName,
					[p_table, p_key]() mutable -> Vector3&
					{
						return *p_table[p_key].get<Vector3*>();
					}
				));

				return true;
			}

			if (typeId == LuaTypeRegistry::GetTypeId<Transform>())
			{
				p_items.emplace_back(std::make_shared<PanelTransformInput>(
					[p_table, p_key]() mutable -> Transform& {
						return *p_table[p_key].get<Transform*>();
					}
				));

				return true;
			}

			return false;
		}

		void AddLuaTableToPanel(PanelComponent::Items& p_items, sol::table p_table, const std::string& p_prefix = {})
		{
			using namespace SvCore::Serialization;
			using namespace SvCore::Utility;

			for (auto [key, value] : p_table)
			{
				if (key.get_type() != sol::type::string)
					continue;

				const std::string keyStr = key.as<const std::string&>();

				if (keyStr.empty() || keyStr[0] == '_' || LuaScriptList::s_ignoredFields.contains(keyStr))
					continue;

				switch (value.get_type())
				{
				case sol::type::string:
				{
					p_items.emplace_back(std::make_shared<PanelTextInput>(p_prefix + keyStr,
						[p_table, keyStr]() mutable
						{
							return p_table[keyStr].get<std::string&>();
						},
						[p_table, keyStr](const std::tuple<PanelTextInput*>& p_textInput) mutable
						{
							const std::string& str = std::get<0>(p_textInput)->GetText();
							p_table[keyStr]		= str;
						}
					));

					break;
				}
				case sol::type::number:
				{
					if (value.is<int>())
						p_items.emplace_back(std::make_shared<PanelIntInput>(p_prefix + keyStr,
							[p_table, keyStr]() mutable
							{
								return p_table[keyStr].get<int>();
							},
							[p_table, keyStr](const int p_value) mutable
							{
								p_table[keyStr] = p_value;
							}
						));
					else
						p_items.emplace_back(std::make_shared<PanelDoubleInput>(p_prefix + keyStr,
							[p_table, keyStr]() mutable
							{
								return p_table[keyStr].get<double>();
							},
							[p_table, keyStr](const double p_value) mutable
							{
								p_table[keyStr] = p_value;
							}
						));

					break;
				}
				case sol::type::boolean:
				{
					p_items.emplace_back(std::make_shared<PanelCheckbox>(p_prefix + keyStr,
						[p_table, keyStr]() mutable
						{
							return p_table[keyStr].get<bool>();
						},
						[p_table, keyStr](const bool p_value) mutable
						{
							p_table[keyStr] = p_value;
						}
					));

					break;
				}
				case sol::type::table:
				{
					AddLuaTableToPanel(p_items, value.as<sol::table>(), p_prefix + keyStr + ".");
					break;
				}
				case sol::type::userdata:
				{
					if (!AddLuaUserTypeToPanel(p_items, p_table, keyStr, p_prefix + keyStr))
						AddLuaTableToPanel(p_items, value.as<sol::table>(), p_prefix + keyStr + ".");
					break;
				}
				case sol::type::lightuserdata:
				case sol::type::none:
				case sol::type::lua_nil:
				case sol::type::thread:
				case sol::type::function:
				case sol::type::poly:
				default:
					break;
				}
			}
		}

		std::shared_ptr<PanelComponent> MakeScriptPanelComponent(LuaScriptHandle p_script)
		{
			using namespace SvCore::ECS;

			PanelComponent::Items items;

			items.emplace_back(std::make_shared<PanelButton>("Remove Script",
				[p_script]() mutable
				{
					p_script.m_owner.Get<LuaScriptList>()->Remove(p_script.m_name);
				}
			));

			ComponentRegistry::EntitiesMap entitiesMap;
			AddLuaTableToPanel(items, p_script.m_table);

			return std::make_shared<PanelComponent>(p_script.m_name, items);
		}
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentScriptList(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		using namespace SvCore::Resources;

		PanelComponent::Items items;

		if (const LuaScriptList* scripts = p_entity.Get<LuaScriptList>())
		{
			items.reserve(scripts->size() + 1);

			for (const auto& name : *scripts)
				items.emplace_back(MakeScriptPanelComponent(scripts->Get(name)));

		    items.emplace_back(std::make_shared<PanelSeparator>());

			items.emplace_back((std::make_shared<PanelResourceSelector<LuaScript>>(
				"Add Script ", []() -> ResourceRef<LuaScript>& {
					static ResourceRef<LuaScript> empty;
				    empty = {};
					return empty;
				},
				[p_entity](const ResourceRef<LuaScript>& p_script)
				{
					LuaScriptList* list = const_cast<LuaScriptList*>(p_entity.Get<LuaScriptList>());
					list->Add(p_script.GetPath());
				}
			)));
		}

		return std::make_shared<PanelComponent>("Scripts", std::move(items));
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
					std::make_shared<PanelResourceSelector<IShader>>(
						"Material ", [resource]() mutable -> ResourceRef<IShader>&{
							static ResourceRef<IShader> ref = resource->GetShaderRef();
							return ref; },
						[resource](PanelResourceSelector<IShader>::CallbackParams p_params) {
							resource->SetShader(p_params);
						}
					)
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

		JsonStringBuffer buffer;
		JsonWriter writer(buffer);
		p_resource.ToJson(writer);

		std::string rawJson = buffer.GetString();

		auto component = PanelResourceDisplay(p_resource, "Re",
			PanelResourceDisplay::Items({
					std::make_shared<PanelTextDisplay>(PanelTextDisplay(
						std::make_shared<DefaultText>(DefaultText(
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


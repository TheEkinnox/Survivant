//InspectorItemManager.cpp
#include "SurvivantEditor/Core/InspectorItemManager.h"

#include "SurvivantEditor/Core/EditorEngine.h"
#include "SurvivantEditor/Panels/HierarchyPanel.h"
#include "SurvivantEditor/PanelItems/PanelButton.h"
#include "SurvivantEditor/PanelItems/PanelCheckbox.h"
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
#include "SurvivantEditor/PanelItems/PanelScriptList.h"

#include <SurvivantApp/Core/IEngine.h>

#include <SurvivantCore/Debug/Assertion.h>

#include <SurvivantCore/ECS/Components/Hierarchy.h>
#include <SurvivantCore/ECS/Components/TagComponent.h>
#include <SurvivantCore/ECS/Scene.h>
#include <SurvivantRendering/Components/CameraComponent.h>
#include <SurvivantRendering/Components/LightComponent.h>
#include <SurvivantRendering/Components/ModelComponent.h>
#include <SurvivantRendering/Enums/EShaderDataType.h>
#include <SurvivantRendering/Resources/Material.h>
#include <SurvivantRendering/Resources/Model.h>
#include <SurvivantRendering/RHI/IShader.h>
#include <SurvivantRendering/RHI/ITexture.h>

#include <SurvivantPhysics/RigidBody.h>
#include <SurvivantPhysics/Collider/BoxCollider.h>
#include <SurvivantPhysics/Collider/CapsuleCollider.h>
#include <SurvivantPhysics/Collider/SphereCollider.h>

#include <SurvivantScripting/LuaScriptList.h>

#include <Transform.h>

using namespace LibMath;
using namespace SvCore::ECS;
using namespace SvEditor::PanelItems;
using namespace SvRendering::Components;
using namespace SvRendering::Enums;
using namespace SvRendering::Resources;
using namespace SvRendering::RHI;
using namespace SvScripting;
using namespace SvPhysics;

using Material = SvRendering::Resources::Material;
using PhysicsMaterial = SvPhysics::Material;
using namespace SvRendering::Resources;
using namespace SvRendering::RHI;
using namespace SvCore::Resources;

namespace SvEditor::Core
{
	void InspectorItemManager::Init()
	{
		//resources
		CHECK(AddResourceToPanelable<Scene>(&AddResourceScene, "Scene"),			"Couldn't init resource type : Scene");
		CHECK(AddResourceToPanelable<Model>(&AddResourceDefault, "Model"),			"Couldn't init resource type : Model");
		CHECK(AddResourceToPanelable<::Material>(&AddResourceMaterial, "Material"),	"Couldn't init resource type : Material");
		CHECK(AddResourceToPanelable<LuaScript>(&AddResourceDefault, "Script"),		"Couldn't init resource type : Script");
		CHECK(AddResourceToPanelable<IShader>(&AddResourceDefault, "Shader"),		"Couldn't init resource type : Shader");
		CHECK(AddResourceToPanelable<ITexture>(&AddResourceDefault, "Texture"),		"Couldn't init resource type : Texture");

		CHECK(AddResourceToPanelable<PhysicsMaterial>(&AddResourceDefault, "PhysicsMaterial"),	"Couldn't init resource type : Physics Material");

		//components
		CHECK(AddComponentToPanelable<Transform>(&AddComponentTransform, "Transform"),				"Couldn't init component type : Transform");
		CHECK(AddComponentToPanelable<HierarchyComponent>(&AddComponentHierarchy, "Hierarchy"),		"Couldn't init component type : Hierarchy");
		CHECK(AddComponentToPanelable<TagComponent>(&AddComponentTag, "Tag"),						"Couldn't init component type : Tag");

		CHECK(AddComponentToPanelable<LightComponent>(&AddComponentLight, "Light"),					"Couldn't init component type : Light");
		CHECK(AddComponentToPanelable<ModelComponent>(&AddComponentModel, "Model"),					"Couldn't init component type : Model");
		CHECK(AddComponentToPanelable<CameraComponent>(&AddComponentCamera, "Camera"),				"Couldn't init component type : Camera");

		CHECK(AddComponentToPanelable<LuaScriptList>(&AddComponentScriptList, "ScriptList"),		"Couldn't init component type : ScriptList");

		CHECK(AddComponentToPanelable<RigidBody>(&AddComponentRigidBody, "RigidBody"),						"Couldn't init component type : RigidBody");
		CHECK(AddComponentToPanelable<BoxCollider>(&AddComponentBoxCollider, "BoxCollider"),				"Couldn't init component type : BoxCollider");
		CHECK(AddComponentToPanelable<SphereCollider>(&AddComponentSphereCollider, "SphereCollider"),		"Couldn't init component type : SphereCollider");
		CHECK(AddComponentToPanelable<CapsuleCollider>(&AddComponentCapsuleCollider, "CapsuleCollider"),	"Couldn't init component type : CapsuleCollider");
	}

	InspectorItemManager::PanelableEntity InspectorItemManager::GetPanelableEntity(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		//auto& registry = ComponentRegistry::GetInstance();
		auto components = p_entity.GetComponents();

		std::vector<PanelableComponent> panelables;
		panelables.reserve(components.size());

		for (auto& type : components | std::views::keys)
		{
			// Scripts are handled by the entity's panel
			if (type == ComponentRegistry::GetTypeId<LuaScriptList>())
				continue;

			if (auto panel = GetPanelableComponent(type, p_entity))
				panelables.emplace_back(panel);
		}

		return std::make_shared<PanelEntity>(p_entity, panelables);
	}

	InspectorItemManager::PanelableResource InspectorItemManager::GetPanelableResource(
		const SvCore::Resources::GenericResourceRef& p_resource)
	{
		auto& registry = ResourceRegistry::GetInstance();

		PanelableResource panel = nullptr;
		if (!registry.Contains(p_resource.GetType()))
			return panel;

		auto& info = registry.GetTypeInfo(p_resource.GetType());
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

		return std::make_shared<PanelComponent>(PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<Transform>(),
			PanelComponent::Items({
					std::make_shared<PanelTransformInput>(PanelTransformInput(
						PanelTransformInput::GetRefFunc(
							[e = p_entity]() mutable -> Transform&
							{
								return *e.Get<Transform>();
							}),
						PanelTransformInput::Callback([](PanelTransformInput::CallbackParams)
							{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
					)
				) }),
			Prio));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentHierarchy(
		const SvCore::ECS::EntityHandle& p_entity)
	{
		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<HierarchyComponent>(),
			PanelComponent::Items({
				std::make_shared<PanelUInt32Input>(PanelUInt32Input(
					"Parent      ",
					PanelUInt32Input::GetCopyFunc([entity = p_entity]() mutable -> uint32_t {
					    if(HierarchyComponent* hierarchy = entity.Get<HierarchyComponent>())
					        return static_cast<uint32_t>(hierarchy->GetParent().GetIndex());

					    return static_cast<uint32_t>(NULL_ENTITY.GetIndex());
				    }),
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
						entity.Get<CameraComponent>()->SetPerspectiveFar(p_val.m_y); },
					0.001f, FLT_MAX
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
						PanelTextInput::GetRefFunc([entity = p_entity]() mutable -> std::string& { return
							entity.Get<TagComponent>()->m_tag; }),
						PanelTextInput::Callback([entity = p_entity](PanelTextInput::CallbackParams) mutable {
							auto tag = entity.Get<TagComponent>(); //refresh
							entity.Set<TagComponent>(*tag);
						})
					))
				}),
			Prio);

		return std::make_shared<PanelComponent>(std::move(component));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentLight(
		const SvCore::ECS::EntityHandle& p_entity)
	{
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
						*(Vector3*)&(entity.Get<LightComponent>()->m_spot.m_attenuationData); }
				)),
				std::make_shared<PanelVec2Input>(PanelVec2Input(
					"Cutoff      ",
					[entity = p_entity]() mutable -> LibMath::Vector2& { return //cast from (float[2])[0] to &Vec2
						*(Vector2*)&(entity.Get<LightComponent>()->m_spot.m_cutoff); }
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
		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<ModelComponent>(),
			PanelComponent::Items({
					std::make_shared<PanelResourceSelector<Model>>(PanelResourceSelector<Model>(
						"Model     ", [entity = p_entity]() mutable -> ResourceRef<Model>& { return
							entity.Get<ModelComponent>()->m_model; }
					)),
					std::make_shared<PanelResourceSelector<::Material>>(PanelResourceSelector<::Material>(
						"Material  ", [entity = p_entity]() mutable -> ResourceRef<::Material>&{ return
							entity.Get<ModelComponent>()->m_material; }
					)),
					std::make_shared<PanelUInt32Input>(PanelUInt32Input(
						"Layer Mask ",[entity = p_entity]() mutable -> PanelUInt32Input::Value&{ return
							entity.Get<ModelComponent>()->m_layerMask; }
					)),
					std::make_shared<PanelButton>(PanelButton(
						"All Layers",
						PanelButton::OnButtonPressEvent::EventDelegate([entity = p_entity]() mutable {
							entity.Get<ModelComponent>()->m_layerMask = SvRendering::Core::Layer::ALL; })
					))
				}));

		return std::make_shared<PanelComponent>(std::move(component));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentScriptList(
		const EntityHandle& p_entity)
	{
		return std::make_shared<PanelScriptList>(p_entity);
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentRigidBody(const EntityHandle& p_entity)
	{
		const std::string typeName = ComponentRegistry::GetInstance().GetRegisteredTypeName<RigidBody>();

		return std::make_shared<PanelComponent>(typeName, PanelComponent::Items({
			std::make_shared<PanelFloatInput>("Mass\t\t",
				[p_entity]
				{
					return p_entity.Get<RigidBody>()->GetMass();
				},
				[entity = p_entity](const float p_value) mutable
				{
					entity.Get<RigidBody>()->SetMass(p_value);
				}
			),
			std::make_shared<PanelCheckbox>("Is Kinematic\t\t",
				[p_entity]
				{
					return p_entity.Get<RigidBody>()->IsKinematic();
				},
				[entity = p_entity](const bool p_value) mutable
				{
					entity.Get<RigidBody>()->SetKinematic(p_value);
				}
			),
			std::make_shared<PanelCheckbox>("Use Gravity\t\t",
				[p_entity]
				{
					return p_entity.Get<RigidBody>()->CanUseGravity();
				},
				[entity = p_entity](const bool p_value) mutable
				{
					entity.Get<RigidBody>()->SetUseGravity(p_value);
				}
			),
			std::make_shared<PanelUniqueSelection>("Collision Detection\t",
				std::vector<std::string>({ "Discrete", "Continuous", "Continuous Speculative" }),
				[p_entity]
				{
					return static_cast<int>(p_entity.Get<RigidBody>()->GetCollisionDetectionMode());
				},
				[entity = p_entity](const int p_value) mutable
				{
					ECollisionDetectionMode detectionMode;
					switch (p_value)
					{
					case 0:
						detectionMode = ECollisionDetectionMode::DISCRETE;
						break;
					case 1:
						detectionMode = ECollisionDetectionMode::CONTINUOUS;
						break;
					case 2:
						detectionMode = ECollisionDetectionMode::CONTINUOUS_SPECULATIVE;
						break;
					default:
						ASSERT(false, "Unsupported collision detection mode");
						return;
					}

					entity.Get<RigidBody>()->SetCollisionDetectionMode(detectionMode);
				}
			)
		}));
	}

	namespace
	{
		template <typename T>
		InspectorItemManager::PanelableComponent MakePanelCollider(const EntityHandle& p_entity, PanelComponent::Items&& p_items)
		{
			static_assert(std::is_base_of_v<ICollider, T>);

			p_items.emplace(p_items.begin(), std::make_shared<PanelVec3Input>("Offset\t\t",
				[entity = p_entity]() mutable -> Vector3& {
					return static_cast<ICollider*>(entity.Get<T>())->m_offset;
				}
			));

			p_items.emplace(p_items.begin(), std::make_shared<PanelCheckbox>("Is Trigger\t",
				[entity = p_entity]() mutable -> bool& {
					return static_cast<ICollider*>(entity.Get<T>())->m_isTrigger;
				}
			));

			p_items.emplace(p_items.begin(), std::make_shared<PanelResourceSelector<PhysicsMaterial>>("Material\t",
				[entity = p_entity]() mutable -> SvCore::Resources::ResourceRef<PhysicsMaterial>& {
					return static_cast<ICollider*>(entity.Get<T>())->m_material;
				}
			));

			const std::string typeName = ComponentRegistry::GetInstance().GetRegisteredTypeName<T>();
			return std::make_shared<PanelComponent>(typeName, std::move(p_items));
		}
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentBoxCollider(
		const EntityHandle& p_entity)
	{
		PanelComponent::Items items
		({
			std::make_shared<PanelVec3Input>("Size\t\t", [entity = p_entity]() mutable -> Vector3& {
				return entity.Get<BoxCollider>()->m_size;
			})
		});

		return MakePanelCollider<BoxCollider>(p_entity, std::move(items));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentSphereCollider(
		const EntityHandle& p_entity)
	{
		PanelComponent::Items items
		({
			std::make_shared<PanelFloatInput>("Radius\t\t", [entity = p_entity]() mutable -> float& {
				return entity.Get<SphereCollider>()->m_radius;
			})
		});

		return MakePanelCollider<SphereCollider>(p_entity, std::move(items));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentCapsuleCollider(
		const EntityHandle& p_entity)
	{
		PanelComponent::Items items
		({
			std::make_shared<PanelVec3Input>("Axis\t\t", [entity = p_entity]() mutable -> Vector3& {
				return entity.Get<CapsuleCollider>()->m_axis;
			}),
			std::make_shared<PanelFloatInput>("Height\t\t", [entity = p_entity]() mutable -> float& {
				return entity.Get<CapsuleCollider>()->m_height;
			}),
			std::make_shared<PanelFloatInput>("Radius\t\t", [entity = p_entity]() mutable -> float& {
				return entity.Get<CapsuleCollider>()->m_radius;
			})
		});

		return MakePanelCollider<CapsuleCollider>(p_entity, std::move(items));
	}
#define PROPERTY_TO_PANELABLE(mat, name, panelType) \
	std::make_shared<panelType>(panelType( \
		name, \
		panelType::GetRefFunc([mat = p_material, name]() mutable -> panelType::Value& { \
			return std::any_cast<panelType::Value&>(mat->GetProperty(name).m_value); }), \
		panelType::Callback([mat = p_material, name](panelType::Value p_val) { \
			mat->GetProperty(name).m_value.emplace<panelType::Value>(p_val); \
			mat.Export(true); })) \
		)

	namespace
	{
		PanelResourceDisplay::Items GetPropertyItems(
			const ResourceRef<::Material>& p_material)
		{
			PanelResourceDisplay::Items items;
			for (auto& [name, property] : p_material->GetProperties())
			{
				auto& [type, value] = property;
				switch (type)
				{
				case SvRendering::Enums::EShaderDataType::UNKNOWN:
					break;
				case SvRendering::Enums::EShaderDataType::BOOL:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelCheckbox));
					break;
				case SvRendering::Enums::EShaderDataType::INT:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelIntInput));
					break;
				case SvRendering::Enums::EShaderDataType::UNSIGNED_INT:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelUInt32Input));
					break;
				case SvRendering::Enums::EShaderDataType::FLOAT:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelFloatInput));
					break;
				case SvRendering::Enums::EShaderDataType::VEC2:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelVec2Input));
					break;
				case SvRendering::Enums::EShaderDataType::VEC3:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelVec3Input));
					break;
				case SvRendering::Enums::EShaderDataType::VEC4: //Assums to be a color
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelColorInput));
					break;
				case SvRendering::Enums::EShaderDataType::TEXTURE:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelResourceSelector<ITexture>));
					break;
				case SvRendering::Enums::EShaderDataType::MAT3:
				case SvRendering::Enums::EShaderDataType::MAT4:
				default:
					ASSERT(false, "Unsuported EShaderDataType");
					break;
				}
			}

			return items;
		}

		void CreateResourceItems(
			const std::weak_ptr<PanelResourceDisplay>& p_resourceDisplay,
			const ResourceRef<::Material>& p_mat)
		{
			PanelResourceDisplay::Items items = PanelResourceDisplay::Items({
				std::make_shared<PanelResourceSelector<IShader>>(
					"Shader ", [p_mat]() mutable -> ResourceRef<IShader>&{
						static ResourceRef<IShader> ref = p_mat->GetShaderRef();
						return ref; 
					},
					[p_mat, p_resourceDisplay](PanelResourceSelector<IShader>::CallbackParams p_params) {
						p_mat->SetShader(p_params); 
						p_mat.Export(true);
						p_resourceDisplay.lock()->SetItems(GetPropertyItems(p_mat), 1);
					})
				});

			p_resourceDisplay.lock()->SetItems(items);
			p_resourceDisplay.lock()->SetItems(GetPropertyItems(p_mat), 1);
		}
	}

	InspectorItemManager::PanelableResource InspectorItemManager::AddResourceMaterial(
		const SvCore::Resources::GenericResourceRef& p_resource)
	{
		ResourceRef<::Material> mat = p_resource;
		
		auto componentPtr = std::make_shared<PanelResourceDisplay>(PanelResourceDisplay(
			p_resource, "Ma",
			PanelResourceDisplay::Items()));

		CreateResourceItems(std::weak_ptr<PanelResourceDisplay>(componentPtr), mat);

		return componentPtr;
	}

	InspectorItemManager::PanelableResource InspectorItemManager::AddResourceScene(
		const SvCore::Resources::GenericResourceRef& /*p_resource*/)
	{
		ASSERT(false, "Do not Inspect scene bcs needs to load ref");
		return {};
	}

	InspectorItemManager::PanelableResource InspectorItemManager::AddResourceDefault(
		const SvCore::Resources::GenericResourceRef& p_resource)
	{
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
}


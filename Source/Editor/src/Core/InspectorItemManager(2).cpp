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
#include "SurvivantEditor/PanelItems/PanelScriptList.h"
#include "SurvivantEditor/PanelItems/PanelSeparator.h"

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

#define PROPERTY_TO_PANELABLE(mat, name, panelType) \
	std::make_shared<panelType>(panelType( \
		name, \
		panelType::GetRefFunc([mat = p_material, name]() mutable -> panelType::Value& { \
			return std::any_cast<panelType::Value&>(mat->GetProperty(name).m_value); }), \
		panelType::Callback([mat = p_material, name](panelType::Value p_val) { \
			mat->GetProperty(name).m_value.emplace<panelType::Value>(p_val); \
			mat.Export(true); })) \
		)

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
	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentTransform(
		const EntityHandle& p_entity)
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
		const EntityHandle& p_entity)
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
		const EntityHandle& p_entity)
	{
		std::vector<std::string> enumNames;
		for (int i = 0; i <= static_cast<int>(EProjectionType::ORTHOGRAPHIC); i++)
			enumNames.emplace_back(std::string(ProjectionTypeToString(EProjectionType(i))));

		std::vector<PanelSelectionDisplay::SelectedDisplay> display;

		//Perspective
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
				std::make_shared<PanelFloatInput>(PanelFloatInput(
					"FovY         ",
					[entity = p_entity]() mutable -> float { return
						entity.Get<CameraComponent>()->GetFovY().degree(true); },
					[entity = p_entity](const float& p_val) mutable {
						entity.Get<CameraComponent>()->SetFovY(Degree(std::fmodf(p_val, 360))); 
						SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); }
				)),
				std::make_shared<PanelVec2Input>(PanelVec2Input(
					"Near/Far     ",
					[p_entity]() -> Vector2 {
						const CameraComponent* cam = p_entity.Get<CameraComponent>();
						return { cam->GetPerspectiveNear(), cam->GetPerspectiveFar() }; },
					[entity = p_entity](const Vector2& p_val) mutable {
						CameraComponent* cam = entity.Get<CameraComponent>();
					    cam->SetPerspectiveNear(p_val.m_x).SetPerspectiveFar(p_val.m_y); 
						SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); },
                    0.001f, FLT_MAX
				))
			}));

		//Orthographic
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
		std::make_shared<PanelFloatInput>(PanelFloatInput(
			"Size        ",
			[entity = p_entity]() mutable -> float { return
				entity.Get<CameraComponent>()->GetOrthographicSize(); },
			[entity = p_entity](const float& p_val) mutable {
				entity.Get<CameraComponent>()->SetOrthographicSize(p_val); 
				SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); }
		)),
		std::make_shared<PanelVec2Input>(PanelVec2Input(
			"Near/Far     ",
			[entity = p_entity]() mutable -> Vector2 { return Vector2(
				entity.Get<CameraComponent>()->GetOrthographicNear(),
				entity.Get<CameraComponent>()->GetOrthographicFar()); },
			[entity = p_entity](const Vector2& p_val) mutable {
				entity.Get<CameraComponent>()->SetOrthographicNear(p_val.m_x);
				entity.Get<CameraComponent>()->SetOrthographicFar(p_val.m_y); 
				SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); }
		))
			}));

		auto component = PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<CameraComponent>(),
			PanelComponent::Items({
				std::make_shared<PanelCheckbox>(
					"Is Active    ",
					PanelCheckbox::GetCopyFunc([p_entity] { return p_entity.Get<CameraComponent>()->IsActive(); }),
					PanelCheckbox::Callback([entity = p_entity](const bool p_val) mutable {
						entity.Get<CameraComponent>()->SetActive(p_val); 
						SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				),
				std::make_shared<PanelSeparator>("Rendering"),
				std::make_shared<PanelIntInput>(
					"Order        ",
					[p_entity] { return p_entity.Get<CameraComponent>()->GetOrder(); },
					[entity = p_entity](const int p_val) mutable {
						entity.Get<CameraComponent>()->SetOrder(p_val); 
						SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); }
				),
				std::make_shared<PanelMultipleSelection>(PanelMultipleSelection(
					"Clear Mask   ", { "Color", "Depth", "Stencil" },
					[p_entity]() -> int {
							return static_cast<int>(p_entity.Get<CameraComponent>()->GetClearMask());
					},
					[entity = p_entity](const int p_val) mutable {
							entity.Get<CameraComponent>()->SetClearMask(static_cast<uint8_t>(p_val));
							SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
					}
				)),
				std::make_shared<PanelColorInput>(PanelColorInput(
					"Color        ",
					[p_entity]() -> Vector4 { return static_cast<Vector4>(
						p_entity.Get<CameraComponent>()->GetClearColor()); },
					[entity = p_entity](const Vector4& p_value) mutable {
						entity.Get<CameraComponent>()->SetClearColor(p_value); 
						SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); }
				)),
				std::make_shared<PanelUInt32Input>(PanelUInt32Input(
					"Culling Mask ",
					[p_entity]() -> uint32_t { return static_cast<uint32_t>(
						p_entity.Get<CameraComponent>()->GetCullingMask()); },
					[entity = p_entity](const uint32_t& p_value) mutable {
						entity.Get<CameraComponent>()->SetCullingMask(p_value); 
						SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); }
				)),
				std::make_shared<PanelSeparator>("Projection"),
				std::make_shared<PanelSelectionDisplay>(PanelSelectionDisplay(
					"Type         ", enumNames, display,
					[p_entity]() -> int { return static_cast<int>( //copy enum to int
							p_entity.Get<CameraComponent>()->GetProjectionType()); },
					[entity = p_entity](const int& p_val) mutable { //set enum
						entity.Get<CameraComponent>()->SetProjectionType(static_cast<EProjectionType>(p_val)); 
						SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); }
				))
			}));

		return std::make_shared<PanelComponent>(std::move(component));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentTag(
		const EntityHandle& p_entity)
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
		const EntityHandle& p_entity)
	{
		std::vector<std::string> enumNames;
		for (int i = 0; i <= static_cast<int>(ELightType::SPOT); i++)
			enumNames.emplace_back(std::string(LightTypeToString(ELightType(i))));

		std::vector<PanelSelectionDisplay::SelectedDisplay> display;

		//Core::Light            m_ambient;
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
				std::make_shared<PanelColorInput>(PanelColorInput(
					"Color ",
					PanelColorInput::GetRefFunc([entity = p_entity]() mutable -> Vector4& //cast from (float[4])[0] to &Vec4
						{ return *(Vector4*)&(entity.Get<LightComponent>()->m_ambient.m_color.m_r); }),
					PanelColorInput::Callback([](PanelColorInput::CallbackParams)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				))
			}));

		//Core::DirectionalLight m_directional;
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
				std::make_shared<PanelColorInput>(PanelColorInput(
					"Color     ",
					PanelColorInput::GetRefFunc([entity = p_entity]() mutable -> Vector4& //cast from (float[4])[0] to &Vec4
						{ return *(Vector4*)&(entity.Get<LightComponent>()->m_directional.m_color.m_r); }),
					PanelColorInput::Callback([](PanelColorInput::CallbackParams)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Direction ",
					PanelVec3Input::GetRefFunc([entity = p_entity]() mutable -> Vector3& { return
						entity.Get<LightComponent>()->m_directional.m_direction; }),
					PanelVec3Input::Callback([](PanelVec3Input::CallbackParams)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				))
		}));

		//Core::PointLight       m_point;
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
				std::make_shared<PanelColorInput>(PanelColorInput(
					"Color       ",
					PanelColorInput::GetRefFunc([entity = p_entity]() mutable -> Vector4& //cast from (float[4])[0] to &Vec4
						{ return *(Vector4*)&(entity.Get<LightComponent>()->m_point.m_color.m_r); }),
					PanelColorInput::Callback([](PanelColorInput::CallbackParams)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Position    ",
					PanelVec3Input::GetRefFunc([entity = p_entity]() mutable -> Vector3& { return
						entity.Get<LightComponent>()->m_point.m_position; }),
					PanelVec3Input::Callback([](const PanelVec3Input::CallbackParams&)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Attenuation ",
					PanelVec3Input::GetRefFunc([entity = p_entity]() mutable -> Vector3& //cast from (float[3])[0] to &Vec3
						{ return *(Vector3*)&(entity.Get<LightComponent>()->m_point.m_attenuationData.m_constant); }),
					PanelVec3Input::Callback([](const PanelVec3Input::CallbackParams&)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				))
			}));

		//Core::SpotLight        m_spot;
		display.emplace_back(PanelSelectionDisplay::SelectedDisplay({
				std::make_shared<PanelColorInput>(PanelColorInput(
					"Color       ",
					PanelColorInput::GetRefFunc([entity = p_entity]() mutable -> Vector4& //cast from (float[4])[0] to &Vec4
						{ return *(Vector4*)&(entity.Get<LightComponent>()->m_spot.m_color.m_r); }),
					PanelColorInput::Callback([](PanelColorInput::CallbackParams)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Position    ",
					PanelVec3Input::GetRefFunc([entity = p_entity]() mutable -> Vector3& { return
						entity.Get<LightComponent>()->m_spot.m_position; }),
					PanelVec3Input::Callback([](const PanelVec3Input::CallbackParams&)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Direction   ",
					PanelVec3Input::GetRefFunc([entity = p_entity]() mutable -> Vector3& { return
						entity.Get<LightComponent>()->m_spot.m_direction; }),
					PanelVec3Input::Callback([](PanelVec3Input::CallbackParams)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				)),
				std::make_shared<PanelVec3Input>(PanelVec3Input(
					"Attenuation ",
					PanelVec3Input::GetRefFunc([entity = p_entity]() mutable -> Vector3& //cast from (float[3])[0] to &Vec3
						{ return *(Vector3*)&(entity.Get<LightComponent>()->m_spot.m_attenuationData.m_constant); }),
					PanelVec3Input::Callback([](const PanelVec3Input::CallbackParams&)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				)),
				std::make_shared<PanelVec2Input>(PanelVec2Input(
					"Cutoff      ",
					PanelVec2Input::GetRefFunc([entity = p_entity]() mutable -> Vector2& { return //cast from (float[2])[0] to &Vec2
						reinterpret_cast<Vector2&>(entity.Get<LightComponent>()->m_spot.m_cutoff); }),
					PanelVec2Input::Callback([](const PanelVec2Input::CallbackParams&)
						{ SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
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
							entity.Get<LightComponent>()->m_type = static_cast<ELightType>(p_val); 
							SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
				) }));

		return std::make_shared<PanelComponent>(std::move(component));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentModel(
		const EntityHandle& p_entity)
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
			std::make_shared<PanelFloatInput>("Mass           ",
				[p_entity]
				{
					return p_entity.Get<RigidBody>()->GetMass();
				},
				[entity = p_entity](const float p_value) mutable
				{
					entity.Get<RigidBody>()->SetMass(p_value);
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
				}
			),
			std::make_shared<PanelCheckbox>("Is Kinematic   ",
				[p_entity]
				{
					return p_entity.Get<RigidBody>()->IsKinematic();
				},
				[entity = p_entity](const bool p_value) mutable
				{
					entity.Get<RigidBody>()->SetKinematic(p_value);
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
				}
			),
			std::make_shared<PanelCheckbox>("Use Gravity    ",
				[p_entity]
				{
					return p_entity.Get<RigidBody>()->CanUseGravity();
				},
				[entity = p_entity](const bool p_value) mutable
				{
					entity.Get<RigidBody>()->SetUseGravity(p_value);
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
				}
			),
			std::make_shared<PanelUniqueSelection>("Collision Mode ",
				std::vector<std::string>({ "Discrete", "Continuous", "Continuous Speculative" }),
				[p_entity]
				{
					return static_cast<int>(p_entity.Get<RigidBody>()->GetCollisionDetectionMode());
				},
				[entity = p_entity](const int p_value) mutable
				{
					entity.Get<RigidBody>()->SetCollisionDetectionMode(static_cast<ECollisionDetectionMode>(p_value));
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
				}
			),
			std::make_shared<PanelMultipleSelection>("Axis Locks     ",
				std::vector<std::string>({ "X Position", "Y Position", "Z Position", "X Rotation", "Y Rotation", "Z Rotation" }),
				[p_entity]() -> EAxisLockFlags::DataType
				{
					return p_entity.Get<RigidBody>()->GetAxisLocks();
				},
				[entity = p_entity](const int p_value) mutable
				{
					entity.Get<RigidBody>()->SetAxisLocks(static_cast<EAxisLockFlags::DataType>(p_value));
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
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

			p_items.emplace(p_items.begin(), std::make_shared<PanelVec3Input>("Offset     ",
				PanelVec3Input::GetRefFunc([entity = p_entity]() mutable -> Vector3& {
					return static_cast<ICollider*>(entity.Get<T>())->m_offset; }),
				PanelVec3Input::Callback([](PanelVec3Input::CallbackParams) {
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
			));

			p_items.emplace(p_items.begin(), std::make_shared<PanelCheckbox>("Is Trigger ",
				PanelCheckbox::GetRefFunc([entity = p_entity]() mutable -> bool& {
					return static_cast<ICollider*>(entity.Get<T>())->m_isTrigger;
				}),
				PanelCheckbox::Callback([](PanelCheckbox::CallbackParams) {
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
			));

			p_items.emplace(p_items.begin(), std::make_shared<PanelResourceSelector<PhysicsMaterial>>(" Material ",
				PanelResourceSelector<PhysicsMaterial>::GetRefFunc([entity = p_entity]() mutable -> ResourceRef<PhysicsMaterial>& {
					return static_cast<ICollider*>(entity.Get<T>())->m_material;
				}),
				PanelResourceSelector<PhysicsMaterial>::Callback([](PanelResourceSelector<PhysicsMaterial>::CallbackParams) {
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); })
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
			std::make_shared<PanelVec3Input>("Size       ", 
				PanelVec3Input::GetRefFunc([entity = p_entity]() mutable -> Vector3& {
					return entity.Get<BoxCollider>()->m_size;
				}),
				PanelVec3Input::Callback([](PanelVec3Input::CallbackParams) {
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>(); 
				}))
		});

		return MakePanelCollider<BoxCollider>(p_entity, std::move(items));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentSphereCollider(
		const EntityHandle& p_entity)
	{
		PanelComponent::Items items
		({
			std::make_shared<PanelFloatInput>("Radius     ", 
				PanelFloatInput::GetRefFunc([entity = p_entity]() mutable -> float& {
					return entity.Get<SphereCollider>()->m_radius;
				}),
				PanelFloatInput::Callback([](PanelFloatInput::CallbackParams) {
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
				}))
		});

		return MakePanelCollider<SphereCollider>(p_entity, std::move(items));
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::AddComponentCapsuleCollider(
		const EntityHandle& p_entity)
	{
		PanelComponent::Items items
		({
			std::make_shared<PanelVec3Input>("Axis       ", 
				PanelVec3Input::GetRefFunc([entity = p_entity]() mutable -> Vector3& {
					return entity.Get<CapsuleCollider>()->m_axis;
				}),
				PanelVec3Input::Callback([](PanelVec3Input::CallbackParams) {
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
				})),
			std::make_shared<PanelFloatInput>("Height     ", 
				PanelFloatInput::GetRefFunc([entity = p_entity]() mutable -> float& {
					return entity.Get<CapsuleCollider>()->m_height;
				}),
				PanelFloatInput::Callback([](PanelFloatInput::CallbackParams) {
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
				})),
			std::make_shared<PanelFloatInput>("Radius     ", 
				PanelFloatInput::GetRefFunc([entity = p_entity]() mutable -> float& {
					return entity.Get<CapsuleCollider>()->m_radius;
				}),
				PanelFloatInput::Callback([](PanelFloatInput::CallbackParams) {
					SV_EVENT_MANAGER().Invoke<Core::EditorEngine::OnEditorModifiedScene>();
				}))
		});

		return MakePanelCollider<CapsuleCollider>(p_entity, std::move(items));
	}

	namespace
	{
		PanelResourceDisplay::Items GetPropertyItems(const ResourceRef<::Material>& p_material)
		{
			PanelResourceDisplay::Items items;
			for (auto& [name, property] : p_material->GetProperties())
			{
				auto& [type, value] = property;
				switch (type)
				{
				case EShaderDataType::UNKNOWN:
					break;
				case EShaderDataType::BOOL:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelCheckbox));
					break;
				case EShaderDataType::INT:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelIntInput));
					break;
				case EShaderDataType::UNSIGNED_INT:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelUInt32Input));
					break;
				case EShaderDataType::FLOAT:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelFloatInput));
					break;
				case EShaderDataType::VEC2:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelVec2Input));
					break;
				case EShaderDataType::VEC3:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelVec3Input));
					break;
				case EShaderDataType::VEC4: //Assume to be a color
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelColorInput));
					break;
				case EShaderDataType::TEXTURE:
					items.emplace_back(PROPERTY_TO_PANELABLE(p_material, name, PanelResourceSelector<ITexture>));
					break;
				case EShaderDataType::MAT3:
				case EShaderDataType::MAT4:
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
		const GenericResourceRef& p_resource)
	{
		ResourceRef<::Material> mat = p_resource;

		auto componentPtr = std::make_shared<PanelResourceDisplay>(PanelResourceDisplay(
			p_resource, "Ma",
			PanelResourceDisplay::Items()));

		CreateResourceItems(std::weak_ptr(componentPtr), mat);

		return componentPtr;
	}

	InspectorItemManager::PanelableResource InspectorItemManager::AddResourceDefault(
		const GenericResourceRef& p_resource)
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

#undef PROPERTY_TO_PANELABLE

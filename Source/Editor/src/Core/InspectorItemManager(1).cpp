//InspectorItemManager.cpp
#include "SurvivantEditor/Core/InspectorItemManager.h"

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

#include <SurvivantAudio/AudioClip.h>

#include <SurvivantScripting/LuaScriptList.h>

#include <Transform.h>

using namespace LibMath;
using namespace SvAudio;
using namespace SvCore::ECS;
using namespace SvEditor::PanelItems;
using namespace SvRendering::Components;
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
		CHECK(AddResourceToPanelable<Scene>(&AddResourceScene, "Scene"),						"Couldn't init resource type : Scene");
		CHECK(AddResourceToPanelable<Model>(&AddResourceDefault, "Model"),						"Couldn't init resource type : Model");
		CHECK(AddResourceToPanelable<::Material>(&AddResourceMaterial, "Material"),				"Couldn't init resource type : Material");
		CHECK(AddResourceToPanelable<LuaScript>(&AddResourceDefault, "Script"),					"Couldn't init resource type : Script");
		CHECK(AddResourceToPanelable<IShader>(&AddResourceDefault, "Shader"),					"Couldn't init resource type : Shader");
		CHECK(AddResourceToPanelable<ITexture>(&AddResourceDefault, "Texture"),					"Couldn't init resource type : Texture");
		CHECK(AddResourceToPanelable<AudioClip>(&AddResourceDefault, "AudioClip"),				"Couldn't init resource type : AudioClip");
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
		const EntityHandle& p_entity)
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
		const GenericResourceRef& p_resource)
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
		const ComponentInfo& p_typeInfo, const EntityHandle& p_entity)
	{
		PanelableComponent panel = nullptr;

		if (p_typeInfo.Has(ADD_COMPONENT))
			panel = p_typeInfo.Call<PanelableComponent>(ADD_COMPONENT, (void*)&p_entity).value_or(nullptr);

		return panel;
	}

	InspectorItemManager::PanelableComponent InspectorItemManager::GetPanelableComponent(const SvCore::Utility::TypeId& p_type, const EntityHandle& p_entity)
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

	InspectorItemManager::PanelableResource InspectorItemManager::AddResourceScene(
		const GenericResourceRef& /*p_resource*/)
	{
		ASSERT(false, "Do not Inspect scene bcs needs to load ref");
		return {};
	}
}

#undef PROPERTY_TO_PANELABLE

#pragma once

#include <SurvivantCore/ECS/Components/Hierarchy.h>
#include <SurvivantCore/ECS/Components/TagComponent.h>

#include <SurvivantPhysics/RigidBody.h>
#include <SurvivantPhysics/Collider/BoxCollider.h>
#include <SurvivantPhysics/Collider/CapsuleCollider.h>
#include <SurvivantPhysics/Collider/SphereCollider.h>

#include <SurvivantRendering/Components/CameraComponent.h>
#include <SurvivantRendering/Components/LightComponent.h>
#include <SurvivantRendering/Components/ModelComponent.h>

#include <SurvivantScripting/LuaScriptList.h>

REGISTER_COMPONENT_TYPE(Transform, LibMath::Transform)
REGISTER_COMPONENT_TYPE(Hierarchy, SvCore::ECS::HierarchyComponent)
REGISTER_COMPONENT_TYPE(Tag, SvCore::ECS::TagComponent)

REGISTER_COMPONENT_TYPE(Light, SvRendering::Components::LightComponent)
REGISTER_COMPONENT_TYPE(Model, SvRendering::Components::ModelComponent)
REGISTER_COMPONENT_TYPE(Camera, SvRendering::Components::CameraComponent)

REGISTER_COMPONENT_TYPE(ScriptList, SvScripting::LuaScriptList)

REGISTER_COMPONENT_TYPE(RigidBody, SvPhysics::RigidBody)
REGISTER_COMPONENT_TYPE(BoxCollider, SvPhysics::BoxCollider)
REGISTER_COMPONENT_TYPE(SphereCollider, SvPhysics::SphereCollider)
REGISTER_COMPONENT_TYPE(CapsuleCollider, SvPhysics::CapsuleCollider)

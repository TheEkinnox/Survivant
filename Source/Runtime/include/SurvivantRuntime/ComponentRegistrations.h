#pragma once

#include <SurvivantCore/ECS/Components/Hierarchy.h>
#include <SurvivantCore/ECS/Components/TagComponent.h>

#include <SurvivantRendering/Components/CameraComponent.h>
#include <SurvivantRendering/Components/LightComponent.h>
#include <SurvivantRendering/Components/ModelComponent.h>

REGISTER_COMPONENT_TYPE(Hierarchy, SvCore::ECS::HierarchyComponent)
REGISTER_COMPONENT_TYPE(Tag, SvCore::ECS::TagComponent)
REGISTER_COMPONENT_TYPE(Transform, LibMath::Transform)

REGISTER_COMPONENT_TYPE(Camera, SvRendering::Components::CameraComponent)
REGISTER_COMPONENT_TYPE(Light, SvRendering::Components::LightComponent)
REGISTER_COMPONENT_TYPE(Model, SvRendering::Components::ModelComponent)

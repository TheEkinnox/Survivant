#pragma once
#include "SurvivantTest/RotatorComponent.h"
#include "SurvivantTest/TemporaryComponent.h"
#include "SurvivantTest/UserInputComponent.h"

#include <SurvivantCore/ECS/Components/Hierarchy.h>
#include <SurvivantCore/ECS/Components/TagComponent.h>

#include <SurvivantRendering/Components/LightComponent.h>
#include <SurvivantRendering/Components/ModelComponent.h>

REGISTER_COMPONENT_TYPE(Hierarchy, SvCore::ECS::HierarchyComponent)
REGISTER_COMPONENT_TYPE(Tag, SvCore::ECS::TagComponent)
REGISTER_COMPONENT_TYPE(Transform, LibMath::Transform)

REGISTER_COMPONENT_TYPE(Light, SvRendering::Components::LightComponent)
REGISTER_COMPONENT_TYPE(Model, SvRendering::Components::ModelComponent)

REGISTER_COMPONENT_TYPE(Rotator, SvTest::Rotator)
REGISTER_COMPONENT_TYPE(Temporary, SvTest::Temporary)
REGISTER_COMPONENT_TYPE(UserInput, SvTest::UserInput)

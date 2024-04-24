#pragma once
#include "SurvivantRendering/Resources/Material.h"
#include "SurvivantRendering/Resources/Model.h"

#include <SurvivantCore/ECS/ComponentRegistry.h>

namespace SvRendering::Components
{
    struct ModelComponent
    {
        SvCore::Resources::ResourceRef<Resources::Model>    m_model;
        SvCore::Resources::ResourceRef<Resources::Material> m_material;
    };
}

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const SvRendering::Components::ModelComponent&, SvCore::Serialization::JsonWriter&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(SvRendering::Components::ModelComponent&, const SvCore::Serialization::JsonValue&, Scene*);
}

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
    bool ComponentRegistry::ToJson<SvRendering::Components::ModelComponent>(
        const SvRendering::Components::ModelComponent&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson<SvRendering::Components::ModelComponent>(
        SvRendering::Components::ModelComponent&, const rapidjson::Value&);
}

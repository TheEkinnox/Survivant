#pragma once
#include "SurvivantRendering/Core/Layer.h"
#include "SurvivantRendering/Resources/Material.h"
#include "SurvivantRendering/Resources/Model.h"

#include <SurvivantCore/Serialization/Serializer.h>

namespace SvRendering::Components
{
    struct ModelComponent
    {
        SvCore::Resources::ResourceRef<Resources::Model>    m_model;
        SvCore::Resources::ResourceRef<Resources::Material> m_material;
        Core::LayerMask                                     m_layerMask = Core::Layer::ALL;
    };
}

namespace SvCore::Serialization
{
    template <>
    bool ToJson(const SvRendering::Components::ModelComponent&, JsonWriter&);

    template <>
    bool FromJson(SvRendering::Components::ModelComponent&, const JsonValue&);
}

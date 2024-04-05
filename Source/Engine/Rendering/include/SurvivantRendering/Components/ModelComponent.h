#pragma once
#include "SurvivantRendering/Resources/Material.h"
#include "SurvivantRendering/Resources/Model.h"

namespace SvRendering::Components
{
    struct ModelComponent
    {
        SvCore::Resources::ResourceRef<Resources::Model>    m_model;
        SvCore::Resources::ResourceRef<Resources::Material> m_material;
    };
}

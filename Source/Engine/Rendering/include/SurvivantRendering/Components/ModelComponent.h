#pragma once
#include "SurvivantRendering/Resources/Material.h"
#include "SurvivantRendering/Resources/Model.h"

namespace SvRendering::Components
{
    struct ModelComponent
    {
        std::shared_ptr<Resources::Model>    m_model;
        std::shared_ptr<Resources::Material> m_material;
    };
}

#pragma once
#include "SurvivantRendering/Resources/Material.h"
#include "SurvivantRendering/Resources/Model.h"

namespace SvCore::ECS
{
    struct ModelComponent
    {
        std::shared_ptr<SvRendering::Resources::Model>    m_model;
        std::shared_ptr<SvRendering::Resources::Material> m_material;
    };
}

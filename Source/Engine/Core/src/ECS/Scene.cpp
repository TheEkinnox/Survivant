#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantCore/ECS/EntityHandle.h"

#include <ranges>

namespace SvCore::ECS
{
    Scene::Scene()
        : m_entities(this)
    {
    }

    bool Scene::Load(const std::string& /*fileName*/)
    {
        // TODO: Load scene from file
        return CHECK(false, "TODO: Load scene from file");
    }

    EntityHandle Scene::Create()
    {
        return { this, m_entities.Add() };
    }

    EntityHandle Scene::Create(const Entity p_source)
    {
        if (!IsValid(p_source))
            return Create();

        Entity entity = m_entities.Add();

        for (const auto& componentStorage : m_components | std::views::values)
            componentStorage->Copy(p_source, entity);

        return { this, entity };
    }

    void Scene::Destroy(const Entity p_entity)
    {
        for (const auto& componentStorage : m_components | std::views::values)
            componentStorage->Remove(p_entity);

        m_entities.Remove(p_entity);
    }

    bool Scene::IsValid(const Entity p_entity) const
    {
        return p_entity != NULL_ENTITY && m_entities.Has(p_entity);
    }

    void Scene::Clear()
    {
        for (auto& storage : m_components | std::views::values)
        {
            if (storage)
                storage->Clear();
        }

        m_entities.Clear();
    }

    bool Scene::Contains(const Entity p_owner) const
    {
        return m_entities.Has(p_owner);
    }
}

#include "SurvivantCore/ECS/Scene.h"

#include "SurvivantCore/ECS/ComponentHandle.h"
#include "SurvivantCore/ECS/ComponentRegistry.h"

#include <ranges>

using namespace SvCore::Serialization;
using namespace SvCore::Utility;

namespace SvCore::ECS
{
    Scene::Scene()
        : m_entities(this)
    {
    }

    bool Scene::Load(const std::string& p_fileName)
    {
        return FromJson(LoadJsonFile(p_fileName));
    }

    bool Scene::Save(const std::string& p_fileName)
    {
        JsonStringBuffer buffer;
        JsonWriter       writer(buffer);

        if (!ToJson(writer) || !ASSUME(writer.IsComplete(), "Failed to save scene - Generated json is incomplete"))
            return false;

        std::ofstream fs(p_fileName);

        if (!CHECK(fs.is_open(), "Unable to open scene file at path \"%s\"", p_fileName.c_str()))
            return false;

        fs << std::string_view(buffer.GetString(), buffer.GetLength());

        return CHECK(!fs.bad(), "Failed to write scene to \"%s\"", p_fileName.c_str());
    }

    bool Scene::ToJson(JsonWriter& p_writer) const
    {
        p_writer.StartObject();

        p_writer.Key("entities");
        p_writer.Uint64(m_entities.size());

        p_writer.Key("components");
        p_writer.StartArray();

        IComponentStorage::EntitiesMap entitiesMap;
        Entity::Index                  index = 0;

        for (const auto entity : m_entities)
            entitiesMap[entity] = Entity(index++);

        for (const auto& [typeId, storage] : m_components)
        {
            if (!storage || storage->size() == 0)
                continue;

            p_writer.StartObject();
            p_writer.Key("type");

            const std::string& typeName = ComponentRegistry::GetInstance().GetRegisteredTypeName(typeId);
            if (!CHECK(p_writer.String(typeName.c_str(), static_cast<rapidjson::SizeType>(typeName.size())),
                    "Unable to serialize scene component storage - Failed to write type"))
                return false;

            p_writer.Key("data");
            if (!storage->ToJson(p_writer, entitiesMap))
                return false;

            p_writer.EndObject();
        }

        p_writer.EndArray();

        return p_writer.EndObject();
    }

    bool Scene::FromJson(const JsonValue& p_json)
    {
        Clear();

        if (!CHECK(p_json.IsObject(), "Unable to deserialize scene - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("entities");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsUint64(), "Unable to deserialize scene - Invalid entities count"))
            return false;

        const Entity::Index entityCount = static_cast<Entity::Index>(it->value.GetUint64());
        m_entities.Reserve(entityCount);

        for (Entity::Index id = 0; id < entityCount; ++id)
        {
            [[maybe_unused]] Entity entity = Create();
            if (!ASSUME(entity.GetIndex() == id))
                return false;
        }

        it = p_json.FindMember("components");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsArray(),
                "Unable to deserialize scene - Invalid components storage array"))
            return false;

        const auto componentStorages = it->value.GetArray();
        for (const auto& jsonStorage : componentStorages)
        {
            if (!DeserializeStorage(jsonStorage))
                return false;
        }

        return true;
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

    EntityHandle Scene::Find(const Entity::Index p_index)
    {
        return { this, m_entities.Find(p_index) };
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

    IComponentStorage& Scene::GetStorage(const TypeId p_id)
    {
        const auto it = m_components.find(p_id);

        if (it != m_components.end())
            return *it->second;

        return *(m_components[p_id] = ComponentRegistry::GetInstance().GetTypeInfo(p_id).MakeStorage(this));
    }

    const IComponentStorage& Scene::GetStorage(const TypeId p_id) const
    {
        return const_cast<Scene*>(this)->GetStorage(p_id);
    }

    std::vector<TypeId> Scene::GetComponentIds() const
    {
        const auto view = m_components | std::views::keys;
        return { view.begin(), view.end() };
    }

    Entity::Id Scene::GetComponentCount(const Entity p_entity) const
    {
        if (!Contains(p_entity))
            return 0;

        Entity::Id count = 0;

        for (const auto& storage : m_components | std::views::values)
        {
            if (storage && storage->Contains(p_entity))
                ++count;
        }

        return count;
    }

    std::vector<TypeId> Scene::GetComponentIds(const Entity p_owner) const
    {
        std::vector<TypeId> ids;
        ids.reserve(m_components.size());

        for (const auto& [id, storage] : m_components)
        {
            if (storage->Contains(p_owner))
                ids.emplace_back(id);
        }

        return ids;
    }

    std::vector<std::pair<TypeId, void*>> Scene::GetComponents(const Entity p_owner) const
    {
        std::vector<std::pair<TypeId, void*>> components;
        components.reserve(m_components.size());

        for (const auto& [id, storage] : m_components)
        {
            if (auto component = storage->FindRaw(p_owner))
                components.emplace_back(id, component);
        }

        return components;
    }

    std::vector<ComponentHandle> Scene::GetComponentHandles(const Entity p_owner) const
    {
        std::vector<ComponentHandle> components;
        components.reserve(m_components.size());

        EntityHandle handle(const_cast<Scene*>(this), p_owner);

        for (const auto& [id, storage] : m_components)
        {
            if (storage->Contains(p_owner))
                components.emplace_back(handle, id);
        }

        return components;
    }

    bool Scene::DeserializeStorage(const JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize scene component storage - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("type");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(),
                "Unable to deserialize component storage - Invalid type string"))
            return false;

        const std::string type(it->value.GetString(), it->value.GetStringLength());

        it = p_json.FindMember("data");
        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize component storage - Data not found"))
            return false;

        const ComponentTypeInfo& typeInfo = ComponentRegistry::GetInstance().GetTypeInfo(type);
        IComponentStorage&       storage  = *(m_components[typeInfo.m_typeId] = typeInfo.MakeStorage(this));

        if (!storage.FromJson(it->value))
            return false;

        return true;
    }
}

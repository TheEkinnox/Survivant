#include "SurvivantCore/ECS/Scene.h"

#include "SurvivantCore/ECS/ComponentRegistry.h"

#include <ranges>

#include <rapidjson/istreamwrapper.h>

namespace SvCore::ECS
{
    Scene::Scene()
        : m_entities(this)
    {
    }

    bool Scene::Load(const std::string& p_fileName)
    {
        std::ifstream fs(p_fileName);

        if (!fs.is_open())
        {
            SV_LOG_ERROR("Unable to open scene file at path \"%s\"", p_fileName.c_str());
            return false;
        }

        rapidjson::IStreamWrapper isw(fs);

        rapidjson::Document json;
        json.ParseStream(isw);

        if (json.HasParseError())
        {
            SV_LOG_ERROR("Unable to parse scene from file - Parse error %d", json.GetParseError());
            return false;
        }

        return FromJson(json);
    }

    bool Scene::ToJson(rapidjson::Writer<rapidjson::StringBuffer>& p_writer) const
    {
        p_writer.StartObject();

        p_writer.Key("entities");
        p_writer.Uint64(m_entities.GetCount());

        p_writer.Key("components");
        p_writer.StartArray();

        IComponentStorage::EntitiesMap entitiesMap;
        Entity::Id                     index = 0;

        for (const auto entity : m_entities)
            entitiesMap[entity] = Entity(index++);

        for (const auto& [typeId, storage] : m_components)
        {
            if (!storage || storage->GetCount() == 0)
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

    bool Scene::FromJson(const rapidjson::Value& p_json)
    {
        Clear();

        if (!CHECK(p_json.IsObject(), "Unable to deserialize scene - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("entities");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsUint64(), "Unable to deserialize scene - Invalid entities count"))
            return false;

        const Entity::Id entityCount = it->value.GetUint64();
        m_entities.Reserve(entityCount);

        for (Entity::Id id = 0; id < entityCount; ++id)
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

    bool Scene::DeserializeStorage(const rapidjson::Value& p_json)
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

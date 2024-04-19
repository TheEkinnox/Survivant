#pragma once
#include "SurvivantCore/ECS/ComponentRegistry.h"
#include "SurvivantCore/ECS/ComponentStorage.h"

#include "SurvivantCore/Debug/Assertion.h"

namespace SvCore::ECS
{
    inline ComponentRegistry& ComponentRegistry::GetInstance()
    {
        static ComponentRegistry instance;
        return instance;
    }

    template <typename T>
    void ComponentRegistry::RegisterType(const std::string& p_name)
    {
        const ComponentTypeInfo typeInfo
        {
            .m_typeId = GetTypeId<T>(),
            .MakeStorage = [](Scene* p_scene)
            {
                std::unique_ptr<IComponentStorage> storage = std::make_unique<ComponentStorage<T>>(p_scene);
                return storage;
            }
        };

        TypeRegistry::RegisterType<T>(p_name, typeInfo);
    }

    template <typename T>
    bool ComponentRegistry::ToJson(
        const T& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap& p_toSerialized)
    {
        constexpr bool hasToJsonWithMap = requires
        {
            p_component.ToJson(p_writer, p_toSerialized);
        };

        constexpr bool hasToJson = requires
        {
            p_component.ToJson(p_writer);
        };

        if constexpr (hasToJsonWithMap)
            return p_component.ToJson(p_writer, p_toSerialized);
        else if constexpr (hasToJson)
            return p_component.ToJson(p_writer);
        else
            return ASSUME(false, "Json serialization is not defined for \"%s\"", typeid(T).name()) && false;
    }

    template <typename T>
    bool ComponentRegistry::FromJson(T& p_out, const rapidjson::Value& p_json, Scene* p_scene)
    {
        constexpr bool hasFromJsonWithScene = requires
        {
            p_out.FromJson(p_json, p_scene);
        };

        constexpr bool hasFromJson = requires
        {
            p_out.FromJson(p_json);
        };

        if constexpr (hasFromJsonWithScene)
            return p_out.FromJson(p_json, p_scene);
        else if constexpr (hasFromJson)
            return p_out.FromJson(p_json);
        else
            return ASSUME(false, "Json deserialization is not defined for \"%s\"", typeid(T).name()) && false;
    }
}

#pragma once
#include "SurvivantCore/ECS/ComponentRegistry.h"
#include "SurvivantCore/ECS/ComponentStorage.h"

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
                std::shared_ptr<IComponentStorage> storage = std::make_shared<ComponentStorage<T>>(p_scene);
                return storage;
            }
        };
        
        TypeRegistry::RegisterType<T>(p_name, typeInfo);
    }

    template <typename T>
    bool ComponentRegistry::ToJson(const T& p_value, Serialization::JsonWriter& p_writer, const EntitiesMap& p_toSerialized)
    {
        constexpr bool hasToJsonWithMap = requires
        {
            p_value.ToJson(p_writer, p_toSerialized);
        };

        if constexpr (hasToJsonWithMap)
            return p_value.ToJson(p_writer, p_toSerialized);
        else
            return Serialization::ToJson(p_value, p_writer);
    }

    template <typename T>
    bool ComponentRegistry::FromJson(T& p_out, const Serialization::JsonValue& p_json, Scene* p_scene)
    {
        constexpr bool hasFromJsonWithScene = requires
        {
            p_out.FromJson(p_json, p_scene);
        };

        if constexpr (hasFromJsonWithScene)
            return p_out.FromJson(p_json, p_scene);
        else
            return Serialization::FromJson(p_out, p_json);
    }
}

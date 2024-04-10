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
    bool ComponentRegistry::ToJson(const T&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&)
    {
        return ASSUME(false, "Json serialization is not defined for \"%s\"", typeid(T).name()) && false;
    }

    template <typename T>
    bool ComponentRegistry::FromJson(T&, const rapidjson::Value&)
    {
        return ASSUME(false, "Json deserialization is not defined for \"%s\"", typeid(T).name()) && false;
    }
}

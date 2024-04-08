#pragma once
#include "SurvivantCore/ECS/Entity.h"
#include "SurvivantCore/Utility/DynamicTypeInfo.h"
#include "SurvivantCore/Utility/TypeRegistry.h"

#include <cstdint>
#include <memory>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#define REGISTER_COMPONENT_TYPE(Name, Type)                                                                  \
static uint8_t compReg_##Name = (SvCore::ECS::ComponentRegistry::GetInstance().RegisterType<Type>(#Name), 0);

namespace SvCore::ECS
{
    class IComponentStorage;
    class Scene;

    struct ComponentTypeInfo
    {
        using TypeId = size_t;

        TypeId m_typeId;

        std::unique_ptr<IComponentStorage> (*MakeStorage)(Scene*);
    };

#ifdef SV_EDITOR
    class ComponentRegistry final : public Utility::TypeRegistry<Utility::DynamicTypeInfo<ComponentTypeInfo>>
#else
    class ComponentRegistry final : public Utility::TypeRegistry<ComponentTypeInfo>
#endif
    {
    public:
        using EntitiesMap = std::unordered_map<Entity::Id, Entity>;

        /**
         * \brief Gets the current component registry instance
         * \return A reference to the current component registry
         */
        static ComponentRegistry& GetInstance();

        /**
         * \brief Registers the given component type with the given name
         * \tparam T The component type to register
         */
        template <typename T>
        void RegisterType(const std::string& p_name);

        /**
         * \brief Serializes the given component to json
         * \tparam T The serialized component's type
         * \param p_component The component to serialize
         * \param p_writer The output json writer
         * \param p_toSerialized The entity id to serialized entity map
         * \return True on success. False otherwise
         */
        template <typename T>
        static bool ToJson(
            const T& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap& p_toSerialized);

        /**
         * \brief Deserializes the given component from json
         * \tparam T The component type to deserialize
         * \param p_out The output component
         * \param p_json The input json data
         * \return True on success. False otherwise
         */
        template <typename T>
        static bool FromJson(T& p_out, const rapidjson::Value& p_json);
    };
}

#include "SurvivantCore/ECS/ComponentRegistry.inl"

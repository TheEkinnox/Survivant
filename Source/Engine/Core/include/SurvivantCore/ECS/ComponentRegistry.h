#pragma once
#include "SurvivantCore/ECS/Entity.h"
#include "SurvivantCore/Serialization/Serializer.h"
#include "SurvivantCore/Utility/TypeRegistry.h"

#ifdef SV_EDITOR
#include "SurvivantCore/Utility/DynamicTypeInfo.h"
#endif

#include <cstdint>
#include <memory>

#define REGISTER_COMPONENT_TYPE(Name, Type)                                                                  \
static uint8_t compReg_##Name = (SvCore::ECS::ComponentRegistry::GetInstance().RegisterType<Type>(#Name), 0);

namespace SvCore::ECS
{
    class IComponentStorage;
    class Scene;

    struct ComponentTypeInfo
    {
        Utility::TypeId m_typeId;

        std::shared_ptr<IComponentStorage> (*MakeStorage)(Scene*);
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
         * \param p_value The component to serialize
         * \param p_writer The output json writer
         * \param p_toSerialized The entity id to serialized entity map
         * \return True on success. False otherwise
         */
        template <typename T>
        static bool ToJson(const T& p_value, Serialization::JsonWriter& p_writer, const EntitiesMap& p_toSerialized);

        /**
         * \brief Deserializes the given component from json
         * \tparam T The component type to deserialize
         * \param p_out The output component
         * \param p_json The input json data
         * \param p_scene The component's target scene
         * \return True on success. False otherwise
         */
        template <typename T>
        static bool FromJson(T& p_out, const Serialization::JsonValue& p_json, Scene* p_scene);
    };
}

#include "SurvivantCore/ECS/ComponentRegistry.inl"

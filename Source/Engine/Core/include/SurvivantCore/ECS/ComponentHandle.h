#pragma once
#include <SurvivantCore/ECS/EntityHandle.h>

namespace SvCore::ECS
{
    struct ComponentHandle
    {
        EntityHandle    m_owner;
        Utility::TypeId m_typeId;

        /**
         * \brief Checks whether the given handle references the same component as this one or not
         * \param p_other The component handle to compare against
         * \return True if the given handle references the same component. False otherwise
         */
        bool operator==(const ComponentHandle& p_other) const;

        /**
         * \brief Checks whether the component handle is valid or not
         */
        operator bool() const;

        /**
         * \brief Removes the component from its owner
         */
        void Destroy();

        /**
         * \brief Gets the handled component cast to the given type
         * \tparam T The expected component type
         * \return The found component cast to the given type on success. Nullptr otherwise
         */
        template <typename T = void>
        T* Get() const
        {
            if (!m_owner || m_typeId == 0)
                return nullptr;

            return static_cast<T*>(m_owner.GetScene()->GetStorage(m_typeId).FindRaw(m_owner));
        }

        /**
         * \brief Gets or creates the handled component cast to the given type
         * \tparam T The expected component type
         * \return The found or created component cast to the given type on success. Nullptr otherwise
         */
        template <typename T = void>
        T* GetOrCreate() const
        {
            if (!m_owner || m_typeId == 0)
                return nullptr;

            return static_cast<T*>(m_owner.GetScene()->GetStorage(m_typeId).GetOrCreateRaw(m_owner));
        }
    };

    template <>
    bool ComponentRegistry::ToJson(
        const ComponentHandle& p_value, Serialization::JsonWriter& p_writer, const EntitiesMap& p_toSerialized);

    template <>
    bool ComponentRegistry::FromJson(ComponentHandle& p_out, const Serialization::JsonValue& p_json, Scene* p_scene);
}

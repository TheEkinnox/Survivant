#pragma once
#include <string>
#include <unordered_map>

namespace SvCore::Utility
{
    using TypeId = size_t;

    template <class TypeInfo>
    class TypeRegistry
    {
    public:
        /**
         * \brief Creates an empty type registry
         */
        TypeRegistry() = default;

        /**
         * \brief Creates a copy of the given type registry
         * \param p_other The type registry to copy
         */
        TypeRegistry(const TypeRegistry& p_other) = default;

        /**
         * \brief Creates a move copy of the given type registry
         * \param p_other The type registry to move
         */
        TypeRegistry(TypeRegistry&& p_other) noexcept = default;

        /**
         * \brief Destroys the type registry
         */
        ~TypeRegistry() = default;

        /**
         * \brief Assigns a copy of the given type registry to this one
         * \param p_other The type registry to copy
         * \return A reference to the modified type registry
         */
        TypeRegistry& operator=(const TypeRegistry& p_other) = default;

        /**
         * \brief Moves the given type registry into this one
         * \param p_other The type registry to move
         * \return A reference to the modified type registry
         */
        TypeRegistry& operator=(TypeRegistry&& p_other) noexcept = default;

        /**
         * \brief Gets the given type's id
         * \tparam T The target type
         * \return The given type's id
         */
        template <typename T>
        static TypeId GetTypeId();

        /**
         * \brief Registers the given type with the given info and name
         * \tparam T The registered type
         * \param p_name The registered type's name
         * \param p_info The registered type's info
         */
        template <typename T>
        TypeInfo& RegisterType(const std::string& p_name, const TypeInfo& p_info);

        /**
         * \brief Checks whether the given type has been registered or not
         * \param p_name The searched type name
         * \return True if the given type has been registered. False otherwise
         */
        bool Contains(const std::string& p_name) const;

        /**
         * \brief Checks whether the given type has been registered or not
         * \param p_id The searched type id
         * \return True if the given type has been registered. False otherwise
         */
        bool Contains(const TypeId& p_id) const;

        /**
         * \brief Checks whether the given type has been registered or not
         * \tparam T The searched type
         * \return True if the given type has been registered. False otherwise
         */
        template <typename T>
        bool Contains() const;

        /**
         * \brief Gets the registered type information for the given component type
         * \tparam T The component type
         * \return The registered type information for the given component type
         */
        TypeInfo& GetTypeInfo(const std::string& p_type);

        /**
         * \brief Gets the registered type information for the given component type
         * \tparam T The component type
         * \return The registered type information for the given component type
         */
        const TypeInfo& GetTypeInfo(const std::string& p_type) const;

        /**
         * \brief Gets the registered type information for the given type id
         * \param p_typeId The component type's id
         * \return The registered type information for the given component type
         */
        TypeInfo& GetTypeInfo(TypeId p_typeId);

        /**
         * \brief Gets the registered type information for the given type id
         * \param p_typeId The component type's id
         * \return The registered type information for the given component type
         */
        const TypeInfo& GetTypeInfo(TypeId p_typeId) const;

        /**
         * \brief Gets the registered type information for the given type id
         * \tparam T The component type
         * \return The registered type information for the given component type
         */
        template <typename T>
        TypeInfo& GetTypeInfo();

        /**
         * \brief Gets the registered type information for the given type id
         * \tparam T The component type
         * \return The registered type information for the given component type
         */
        template <typename T>
        const TypeInfo& GetTypeInfo() const;

        /**
         * \brief Gets the registered name for the given type id
         * \param p_typeId The component type's id
         * \return The registered name for the given component type
         */
        const std::string& GetRegisteredTypeName(TypeId p_typeId) const;

        /**
         * \brief Gets the registered name for the given component type
         * \tparam T The component type
         * \return The registered name for the given component type
         */
        template <typename T>
        const std::string& GetRegisteredTypeName() const;

    private:
        using TypeMap = std::unordered_map<TypeId, TypeInfo>;
        using TypeNameMap = std::unordered_map<TypeId, std::string>;
        using TypeIdMap = std::unordered_map<std::string, TypeId>;

        TypeMap     m_typeInfos;
        TypeNameMap m_typeNames;
        TypeIdMap   m_typeIds;
    };
}

#include "SurvivantCore/Utility/TypeRegistry.inl"

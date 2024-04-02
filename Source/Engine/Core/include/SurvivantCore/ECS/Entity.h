#pragma once
#include <climits>
#include <cstdint>

namespace SvCore::ECS
{
    class Entity
    {
    public:
        using Id = uint64_t;
        using Version = uint32_t;

        static constexpr uint8_t VERSION_BITS = 24;
        static constexpr uint8_t INDEX_BITS   = sizeof(Id) * CHAR_BIT - VERSION_BITS;

        static constexpr Version VERSION_MASK = (Version{ 1 } << VERSION_BITS) - 1;
        static constexpr Id      INDEX_MASK   = (Id{ 1 } << INDEX_BITS) - 1;

        /**
         * \brief Creates a default entity
         */
        constexpr Entity() = default;

        /**
         * \brief Creates an entity with the given id
         * \param p_id The entity's id
         */
        explicit constexpr Entity(Id p_id);

        /**
         * \brief Creates an entity with the given index and version
         * \param p_index The entity's index
         * \param p_version The entity's version
         */
        constexpr Entity(Id p_index, Version p_version);

        /**
         * \brief Implicitly converts an entity to it's index
         */
        constexpr operator Id() const;

        /**
         * \brief Gets the entity's version
         * \return The entity's version
         */
        constexpr Version GetVersion() const;

        /**
         * \brief Gets the entity's index
         * \return The entity's index
         */
        constexpr Id GetIndex() const;

        /**
         * \brief Increments the entity's version
         */
        void BumpVersion();

    private:
        Id m_id = 0;

        /**
         * \brief Creates an entity identifier from the given index and version
         * \param p_index The target index
         * \param p_version The target version
         * \return The entity identifier composed of the given index and version
         */
        static constexpr Id Make(Id p_index, Id p_version);
    };
}

#include "SurvivantCore/ECS/Entity.inl"

namespace SvCore::ECS
{
    static constexpr Entity NULL_ENTITY{ Entity::INDEX_MASK, Entity::VERSION_MASK };
}

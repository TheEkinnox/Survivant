#pragma once
#include "SurvivantCore/Utility/TypeTraits.h"

#include <climits>
#include <cstdint>
#include <ostream>

namespace SvCore::ECS
{
    class Entity
    {
    public:
        using Id = uint64_t;

        static constexpr uint8_t VERSION_BITS = 32;
        static constexpr uint8_t INDEX_BITS   = sizeof(Id) * CHAR_BIT - VERSION_BITS;

        using Version = Utility::SmallestUnsigned<VERSION_BITS>;
        using Index = Utility::SmallestUnsigned<INDEX_BITS>;

        static constexpr Version VERSION_MASK = (Id{ 1 } << VERSION_BITS) - 1;
        static constexpr Index   INDEX_MASK   = (Id{ 1 } << INDEX_BITS) - 1;

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
        constexpr Entity(Index p_index, Version p_version);

        /**
         * \brief Implicitly converts an entity to it's id
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
        constexpr Index GetIndex() const;

        /**
         * \brief Increments the entity's version
         */
        void BumpVersion();

        /**
         * \brief Creates a string representation of the entity
         * \return A string representation of the entity
         */
        std::string GetString() const;

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

    /**
     * \brief Adds an entity's string representation to the given output stream
     * \param p_stream The output stream
     * \param p_entity The output entity
     * \return The modified stream
     */
    std::ostream& operator<<(std::ostream& p_stream, const Entity& p_entity);
}

#include "SurvivantCore/ECS/Entity.inl"

namespace SvCore::ECS
{
    static constexpr Entity NULL_ENTITY{ Entity::INDEX_MASK, Entity::VERSION_MASK };
}

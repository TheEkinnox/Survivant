#pragma once
#include "SurvivantCore/ECS/Entity.h"
#include "SurvivantCore/Events/Event.h"

namespace SvCore::ECS
{
    class EntityHandle;
    class Scene;

    class EntityStorage
    {
    public:
        using iterator = std::vector<Entity>::iterator;
        using const_iterator = std::vector<Entity>::const_iterator;

        Events::Event<EntityHandle> m_onAdd;
        Events::Event<EntityHandle> m_onRemove;

        /**
         * \brief Creates an empty entity storage
         * \param p_scene The storage's owning scene
         */
        explicit EntityStorage(Scene* p_scene = nullptr);

        /**
         * \brief Creates a copy of the given entity storage
         * \param p_other The entity storage to copy
         */
        EntityStorage(const EntityStorage& p_other) = default;

        /**
         * \brief Creates a move copy of the given entity storage
         * \param p_other The entity storage to move
         */
        EntityStorage(EntityStorage&& p_other) noexcept = default;

        /**
         * \brief Destroys the entity storage
         */
        ~EntityStorage() = default;

        /**
         * \brief Assigns a copy of the given entity storage to this one
         * \param p_other The entity storage to copy
         * \return A reference to the modified entity storage
         */
        EntityStorage& operator=(const EntityStorage& p_other) = default;

        /**
         * \brief Moves the given entity storage into this one
         * \param p_other The entity storage to move
         * \return A reference to the modified entity storage
         */
        EntityStorage& operator=(EntityStorage&& p_other) noexcept = default;

        /**
         * \brief Adds an entity to the manager
         * \return The added entity
         */
        Entity Add();

        /**
         * \brief Removes the given entity from the manager
         * \param p_entity The entity to remove
         */
        void Remove(Entity p_entity);

        /**
         * \brief Checks whether the given entity is present in the manager or not
         * \param p_entity The entity to search for
         * \return True if the entity was found. False otherwise
         */
        bool Has(Entity p_entity) const;

        /**
         * \brief Finds the entity with the given index
         * \param p_index The entity's index
         * \return The entity with the given index on success. NULL_ENTITY otherwise
         */
        Entity Find(Entity::Index p_index) const;

        /**
         * \brief Removes all entities from the manager
         */
        void Clear();

        /**
         * \brief Reserves the given number of entities
         * \param p_count The number of entities to reserve
         */
        void Reserve(Entity::Index p_count);

        /**
         * \brief Gets an iterator to the start of the entities array
         * \return An iterator to the start of the entities array
         */
        iterator begin();

        /**
         * \brief Gets an iterator to the end of the entities array
         * \return An iterator to the end of the entities array
         */
        iterator end();

        /**
         * \brief Gets a constant iterator to the start of the entities array
         * \return A constant iterator to the start of the entities array
         */
        const_iterator begin() const;

        /**
         * \brief Gets a constant iterator to the end of the entities array
         * \return A constant iterator to the end of the entities array
         */
        const_iterator end() const;

        /**
         * \brief Gets the current number of entities
         * \return The current number of entities
         */
        Entity::Index size() const;

    private:
        std::vector<Entity> m_entities;
        Entity::Index       m_count = 0;
        Scene*              m_scene = nullptr;
    };
}

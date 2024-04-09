#pragma once
#include "SurvivantCore/ECS/Entity.h"
#include "SurvivantCore/Events/Event.h"

#include <unordered_map>

namespace SvCore::ECS
{
    class EntityHandle;
    class Scene;

    class IComponentStorage
    {
    public:
        using EntitiesMap = std::unordered_map<Entity::Id, Entity>;

        /**
         * \brief Creates a copy of the given component storage
         * \param p_other The component storage to copy
         */
        IComponentStorage(const IComponentStorage& p_other) = default;

        /**
         * \brief Creates a move copy of the given component storage
         * \param p_other The component storage to move
         */
        IComponentStorage(IComponentStorage&& p_other) noexcept = default;

        /**
         * \brief Destroys the component storage
         */
        virtual ~IComponentStorage() = default;

        /**
         * \brief Assigns a copy of the given component storage to this one
         * \param p_other The component storage to copy
         * \return A reference to the modified component storage
         */
        IComponentStorage& operator=(const IComponentStorage& p_other) = default;

        /**
         * \brief Moves the given component storage into this one
         * \param p_other The component storage to move
         * \return A reference to the modified component storage
         */
        IComponentStorage& operator=(IComponentStorage&& p_other) noexcept = default;

        /**
         * \brief Checks if the given entity owns a component in the storage
         * \param p_entity The entity to check for
         * \return True if the entity owns a component in this storage. False otherwise
         */
        virtual bool Contains(Entity p_entity) const = 0;

        /**
         * \brief Assigns a copy of the source entity's component to the target entity
         * \param p_source The entity from which the component should be copied
         * \param p_target The entity to which the component should be assigned
         * \return True on success. False otherwise
         */
        virtual bool Copy(Entity p_source, Entity p_target) = 0;

        /**
         * \brief Removes the component owned by the given entity
         * \param p_owner The removed component's owner
         */
        virtual void Remove(Entity p_owner) = 0;

        /**
         * \brief Removes all stored components
         */
        virtual void Clear() = 0;

        /**
         * \brief Reserves the given number of components
         * \param p_count The number of components to reserve
         */
        virtual void Reserve(Entity::Id p_count) = 0;

        /**
         * \brief Gets the current number of entities
         * \return The current number of entities
         */
        virtual Entity::Id GetCount() const = 0;

    protected:
        /**
         * \brief Creates a default component storage
         */
        IComponentStorage() = default;
    };

    template <class T>
    class ComponentStorage final : public IComponentStorage
    {
    public:
        using ComponentT = std::remove_const_t<T>;
        using iterator = typename std::vector<ComponentT>::iterator;
        using const_iterator = typename std::vector<ComponentT>::const_iterator;

        SvCore::Events::Event<EntityHandle, T&> m_onAdd;
        SvCore::Events::Event<EntityHandle, T&> m_onRemove;
        SvCore::Events::Event<EntityHandle, T&> m_onBeforeChange;
        SvCore::Events::Event<EntityHandle, T&> m_onChange;

        /**
         * \brief Creates an empty component storage
         * \param p_scene The storage's owning scene
         */
        explicit ComponentStorage(Scene* p_scene = nullptr);

        /**
         * \brief Creates a copy of the given component storage
         * \param p_other The component storage to copy
         */
        ComponentStorage(const ComponentStorage& p_other) = default;

        /**
         * \brief Creates a move copy of the given component storage
         * \param p_other The component storage to move
         */
        ComponentStorage(ComponentStorage&& p_other) noexcept = default;

        /**
         * \brief Destroys the component storage
         */
        ~ComponentStorage() override = default;

        /**
         * \brief Assigns a copy of the given component storage to this one
         * \param p_other The component storage to copy
         * \return A reference to the modified component storage
         */
        ComponentStorage& operator=(const ComponentStorage& p_other) = default;

        /**
         * \brief Moves the given component storage into this one
         * \param p_other The component storage to move
         * \return A reference to the modified component storage
         */
        ComponentStorage& operator=(ComponentStorage&& p_other) noexcept = default;

        /**
         * \brief Checks if the given entity owns a component in the storage
         * \param p_entity The entity to check for
         * \return True if the entity owns a component in this storage. False otherwise
         */
        bool Contains(Entity p_entity) const override;

        /**
         * \brief Assigns a copy of the source entity's component to the target entity
         * \param p_source The entity from which the component should be copied
         * \param p_target The entity to which the component should be assigned
         * \return True on success. False otherwise
         */
        bool Copy(Entity p_source, Entity p_target) override;

        /**
         * \brief Assigns the given component instance to the given entity
         * \param p_owner The component's owner
         * \param p_instance The added component instance
         * \return A reference to the created or modified component
         */
        T& Set(Entity p_owner, const ComponentT& p_instance);

        /**
         * \brief Creates or modifies the given entity's component of the given type
         * \tparam Args The component's construction parameters
         * \param p_owner The component's owner
         * \param p_args The component's construction parameters
         * \return A reference to the created or modified component
         */
        template <typename... Args>
        T& Construct(Entity p_owner, Args&&... p_args);

        /**
         * \brief Removes the component owned by the given entity
         * \param p_owner The removed component's owner
         */
        void Remove(Entity p_owner) override;

        /**
         * \brief Removes the given component instance from the storage
         * \param p_component The component instance to remove
         */
        void Remove(const T& p_component);

        /**
         * \brief Removes all stored components
         */
        void Clear() override;

        /**
         * \brief Reserves the given number of entities
         * \param p_count The number of entities to reserve
         */
        void Reserve(Entity::Id p_count) override;

        /**
         * \brief Gets the current number of entities
         * \return The current number of entities
         */
        Entity::Id GetCount() const override;

        /**
         * \brief Checks if the given entity owns a component in the storage
         * \param p_owner The searched component's owner
         * \return True if the entity owns a component of in the storage. False otherwise
         */
        bool Has(Entity p_owner) const;

        /**
         * \brief Finds the component owned by the given entity
         * \param p_owner The searched component's owner
         * \return A pointer to the found component on success. Nullptr otherwise
         */
        T* Find(Entity p_owner);

        /**
         * \brief Finds the component owned by the given entity
         * \param p_owner The searched component's owner
         * \return A constant pointer to the found component on success. Nullptr otherwise
         */
        const T* Find(Entity p_owner) const;

        /**
         * \brief Gets the given component instance's owner
         * \param p_component The component who's owner to find
         * \return The component's owner on success. NULL_ENTITY otherwise
         */
        Entity GetOwner(const T& p_component) const;

        /**
         * \brief Gets an iterator to the start of the components array
         * \return An iterator to the start of the components array
         */
        iterator begin();

        /**
         * \brief Gets an iterator to the end of the components array
         * \return An iterator to the end of the components array
         */
        iterator end();

        /**
         * \brief Gets a constant iterator to the start of the components array
         * \return A constant iterator to the start of the components array
         */
        const_iterator begin() const;

        /**
         * \brief Gets a constant iterator to the end of the components array
         * \return A constant iterator to the end of the components array
         */
        const_iterator end() const;

    private:
        std::vector<ComponentT>                m_components;
        std::unordered_map<Entity::Id, size_t> m_entityToComponent;
        std::unordered_map<size_t, Entity>     m_componentToEntity;
        Scene*                                 m_scene;
    };
}

#include "SurvivantCore/ECS/ComponentStorage.inl"
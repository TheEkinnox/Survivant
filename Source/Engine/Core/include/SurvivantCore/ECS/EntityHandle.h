#pragma once
#include "SurvivantCore/ECS/Entity.h"
#include "SurvivantCore/Utility/TypeRegistry.h"

namespace SvCore::ECS
{
    struct ComponentHandle;
    class Scene;

    class EntityHandle final
    {
    public:
        enum class EComponentSearchOrigin : uint8_t
        {
            ROOT,
            PARENT,
            CHILDREN
        };

        /**
         * \brief Creates a handle for the given entity
         * \param p_scene The linked entity's scene
         * \param p_entity The linked entity
         */
        EntityHandle(Scene* p_scene = nullptr, Entity p_entity = NULL_ENTITY);

        /**
         * \brief Creates a copy of the given entity handle
         * \param p_other The entity handle to copy
         */
        EntityHandle(const EntityHandle& p_other) = default;

        /**
         * \brief Creates a move copy of the given entity handle
         * \param p_other The entity handle to move
         */
        EntityHandle(EntityHandle&& p_other) noexcept = default;

        /**
         * \brief Destroys the entity handle
         */
        ~EntityHandle() = default;

        /**
         * \brief Assigns a copy of the given entity handle to this one
         * \param p_other The entity handle to copy
         * \return A reference to the modified handle
         */
        EntityHandle& operator=(const EntityHandle& p_other) = default;

        /**
         * \brief Moves the given entity handle into this one
         * \param p_other The entity handle to move
         * \return A reference to the modified handle
         */
        EntityHandle& operator=(EntityHandle&& p_other) noexcept = default;

        /**
         * \brief Checks whether the given handle references the same entity as this one
         * \param p_other The compared entity handle
         * \return True if the other handle references the same entity. False otherwise
         */
        bool operator==(const EntityHandle& p_other) const;

        /**
         * \brief Checks whether the entity is valid or not
         * \return True if the entity is valid. False otherwise.
         */
        operator bool() const;

        /**
         * \brief Implicitly converts the handle to it's linked entity
         */
        operator Entity() const;

        /**
         * \brief Gets the handle's linked entity
         * \return The handle's linked entity
         */
        Entity GetEntity() const;

        /**
         * \brief Gets the entity's scene
         * \return A pointer to the entity's scene
         */
        Scene* GetScene() const;

        /**
         * \brief Gets the root of the entity's hierarchy
         * \return A handle to the root of the entity's hierarchy
         */
        EntityHandle GetRoot() const;

        /**
         * \brief Gets the entity's parent
         * \return A handle to the entity's parent if found or to NULL_ENTITY otherwise
         */
        EntityHandle GetParent() const;

        /**
         * \brief Gets the entity's display name
         * \return The entity's display name
         */
        std::string GetDisplayName() const;

        /**
         * \brief Sets the linked entity's parent
         * \param p_parent The entity's new parent
         */
        void SetParent(EntityHandle p_parent);

        /**
         * \brief Gets the entity's next sibling
         * \return A handle to the entity's next sibling if found or to NULL_ENTITY otherwise
         */
        EntityHandle GetNextSibling() const;

        /**
         * \brief Gets the entity's previous sibling
         * \return A handle to the entity's previous sibling if found or to NULL_ENTITY otherwise
         */
        EntityHandle GetPreviousSibling() const;

        /**
         * \brief Gets the entity's child count
         * \return The entity's child count
         */
        size_t GetChildCount() const;

        /**
         * \brief Gets the entity's child at the given index
         * \param p_index The target child's index
         * \return A handle to the child if found or to NULL_ENTITY otherwise
         */
        EntityHandle GetChild(size_t p_index) const;

        /**
         * \brief Gets the entity's children
         * \return A vector of the entity's children
         */
        std::vector<EntityHandle> GetChildren() const;

        /**
         * \brief Creates a copy of the linked component
         * \return A handle to the entity's copy
         */
        EntityHandle Copy() const;

        /**
         * \brief Destroys the linked entity
         */
        void Destroy();

        /**
         * \brief Checks if the linked entity owns a component of the given type
         * \tparam T The component's type
         * \return True if the entity owns a component of the given type. False otherwise
         */
        template <typename T>
        bool Has() const;

        /**
         * \brief Finds the component owned by the linked entity
         * \tparam T The component's type
         * \return A pointer to the found component on success. Nullptr otherwise
         */
        template <typename T>
        T* Get();

        /**
         * \brief Finds the component owned by the linked entity
         * \tparam T The component's type
         * \return A constant pointer to the found component on success. Nullptr otherwise
         */
        template <typename T>
        const T* Get() const;

        /**
         * \brief Finds the component of the given type owned by the entity or one of it's parents
         * \tparam T The component's type
         * \return A pointer to the found component on success. Nullptr otherwise
         */
        template <typename T>
        T* GetInParent();

        /**
         * \brief Finds the component of the given type owned by the entity or one of it's parents
         * \tparam T The component's type
         * \return A constant pointer to the found component on success. Nullptr otherwise
         */
        template <typename T>
        const T* GetInParent() const;

        /**
         * \brief Finds the component of the given type owned by the entity or one of it's children
         * \tparam T The component's type
         * \return A pointer to the found component on success. Nullptr otherwise
         */
        template <typename T>
        T* GetInChildren();

        /**
         * \brief Finds the component of the given type owned by the entity or one of it's children
         * \tparam T The component's type
         * \return A constant pointer to the found component on success. Nullptr otherwise
         */
        template <typename T>
        const T* GetInChildren() const;

        /**
         * \brief Finds the component of the given type owned by an entity in the linked entity's hierarchy
         * \tparam T The component's type
         * \param p_searchOrigin The point at which the search should start
         * \return A pointer to the found component on success. Nullptr otherwise
         */
        template <typename T>
        T* GetInHierarchy(EComponentSearchOrigin p_searchOrigin);

        /**
         * \brief Finds the component of the given type owned by an entity in the linked entity's hierarchy
         * \tparam T The component's type
         * \param p_searchOrigin The point at which the search should start
         * \return A constant pointer to the found component on success. Nullptr otherwise
         */
        template <typename T>
        const T* GetInHierarchy(EComponentSearchOrigin p_searchOrigin) const;

        /**
         * \brief Assigns the given component instance to the linked entity
         * \tparam T The component's type
         * \param p_instance The added component instance
         * \return A reference to the created or modified component
         */
        template <typename T>
        T& Set(const T& p_instance);

        /**
         * \brief Creates or modifies the linked entity's component of the given type
         * \tparam T The component's type
         * \tparam Args The component's construction parameters
         * \param p_args The component's construction parameters
         * \return A reference to the created or modified component
         */
        template <typename T, typename... Args>
        T& Make(Args&&... p_args);

        /**
         * \brief Removes the component of the given type from the linked entity
         * \tparam T The component's type
         */
        template <typename T>
        void Remove();

        /**
         * \brief Removes the given component instance from the linked entity
         * \tparam T The component's type
         * \param p_instance The component instance to remove
         */
        template <typename T>
        void Remove(const T& p_instance);

        /**
         * \brief Gets the number of components owned by the linked entity
         * \return The number of components owned by the entity
         */
        Entity::Id GetComponentCount() const;

        /**
         * \brief Gets the ids of all the component types owned by the linked entity
         * \return The ids of all the component types owned by the entity
         */
        std::vector<Utility::TypeId> GetComponentIds() const;

        /**
         * \brief Gets all the components owned by the linked entity
         * \return The components owned by the entity
         */
        std::vector<std::pair<Utility::TypeId, void*>> GetComponents() const;

        /**
         * \brief Gets handles to all the components owned by the linked entity
         * \return Handles to the components owned by the entity
         */
        std::vector<ComponentHandle> GetComponentHandles() const;

    private:
        Scene* m_scene;
        Entity m_entity;
    };

    /**
     * \brief Adds an entity handle's string representation to the given output stream
     * \param p_stream The output stream
     * \param p_handle The output entity handle
     * \return The modified stream
     */
    std::ostream& operator<<(std::ostream& p_stream, const EntityHandle& p_handle);
}

#include "SurvivantCore/ECS/EntityHandle.inl"

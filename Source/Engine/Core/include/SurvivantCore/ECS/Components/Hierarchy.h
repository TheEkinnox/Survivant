#pragma once
#include "SurvivantCore/ECS/ComponentTraits.h"
#include "SurvivantCore/ECS/ComponentRegistry.h"
#include "SurvivantCore/ECS/Entity.h"
#include "SurvivantCore/Serialization/MathSerializers.h"

namespace SvCore::ECS
{
    class HierarchyComponent
    {
    public:
        /**
         * \brief Creates a hierarchy component with the given parent entity
         * \param p_parent The entity's parent entity
         */
        explicit HierarchyComponent(Entity p_parent = NULL_ENTITY);

        /**
         * \brief Creates a copy of the given hierarchy component
         * \param p_other The hierarchy component to copy
         */
        HierarchyComponent(const HierarchyComponent& p_other) = default;

        /**
         * \brief Creates a move copy of the given hierarchy component
         * \param p_other The hierarchy component to move
         */
        HierarchyComponent(HierarchyComponent&& p_other) noexcept = default;

        /**
         * \brief Destroys the hierarchy component
         */
        ~HierarchyComponent() = default;

        /**
         * \brief Assigns a copy of the given hierarchy component to this one
         * \param p_other The hierarchy component to copy
         * \return A reference to the modified component
         */
        HierarchyComponent& operator=(const HierarchyComponent& p_other) = default;

        /**
         * \brief Moves the given hierarchy component into this one
         * \param p_other The hierarchy component to move
         * \return A reference to the modified component
         */
        HierarchyComponent& operator=(HierarchyComponent&& p_other) noexcept = default;

        /**
         * \brief Gets the hierarchy owner's parent entity
         * \return The owner's parent
         */
        Entity GetParent() const;

        /**
         * \brief Sets the hierarchy owner's parent entity
         * \param p_parent The owner's new parent
         */
        void SetParent(Entity p_parent);

        /**
         * \brief Gets the hierarchy owner's first child
         * \return The owner's first child
         */
        Entity GetFirstChild() const;

        /**
         * \brief Gets the hierarchy owner's previous sibling
         * \return The owner's previous sibling
         */
        Entity GetPreviousSibling() const;

        /**
         * \brief Gets the hierarchy owner's next sibling
         * \return The owner's next sibling
         */
        Entity GetNextSibling() const;

        /**
         * \brief Gets the hierarchy owner's number of child
         * \return The owner's child count
         */
        size_t GetChildCount() const;

    private:
        friend struct ComponentTraits;
        friend class ComponentRegistry;

        Entity m_parent          = NULL_ENTITY;
        Entity m_firstChild      = NULL_ENTITY;
        Entity m_previousSibling = NULL_ENTITY;
        Entity m_nextSibling     = NULL_ENTITY;
        size_t m_childCount      = 0;
    };

    std::vector<LibMath::Transform*> GetChildTransforms(const EntityHandle& p_entity);

    /**
     * \brief Links the relevant transforms to this entity
     * \param p_entity The entity for which the transforms should be linked
     */
    void LinkTransforms(EntityHandle& p_entity);

    /**
     * \brief Unlinks the relevant transforms from this entity
     * \param p_entity The entity for which the transforms should be unlinked
     */
    void UnlinkTransforms(const EntityHandle& p_entity);

    template <>
    void ComponentTraits::OnAdd(EntityHandle&, HierarchyComponent&);

    template <>
    void ComponentTraits::OnRemove(EntityHandle&, HierarchyComponent&);

    template <>
    void ComponentTraits::OnBeforeChange(EntityHandle&, HierarchyComponent&, HierarchyComponent&);

    template <>
    void ComponentTraits::OnChange(EntityHandle&, HierarchyComponent&);

    template <>
    HierarchyComponent ComponentTraits::Copy(EntityHandle&, HierarchyComponent&, EntityHandle&);

    template <>
    void ComponentTraits::OnAdd(EntityHandle&, LibMath::Transform&);

    template <>
    void ComponentTraits::OnRemove(EntityHandle&, LibMath::Transform&);

    template <>
    void ComponentTraits::OnChange(EntityHandle&, LibMath::Transform&);

    template <>
    bool ComponentRegistry::ToJson(const HierarchyComponent&, Serialization::JsonWriter&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson<HierarchyComponent>(HierarchyComponent&, const Serialization::JsonValue&, Scene*);
}

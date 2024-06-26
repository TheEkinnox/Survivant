#pragma once

namespace SvCore::ECS
{
    class EntityHandle;

    struct ComponentTraits
    {
        /**
         * \brief The action to perform when a component of the given type is added
         * \tparam T The added entity's type
         * \param p_entity The component's owner
         * \param p_component The added component
         */
        template <class T>
        static void OnAdd([[maybe_unused]] EntityHandle& p_entity, [[maybe_unused]] T& p_component)
        {
        }

        /**
         * \brief The action to perform before a component of the given type is removed
         * \tparam T The removed component's type
         * \param p_entity The component's owner
         * \param p_component The component to remove
         */
        template <class T>
        static void OnRemove([[maybe_unused]] EntityHandle& p_entity, [[maybe_unused]] T& p_component)
        {
        }

        /**
         * \brief The action to perform before a component of the given type is updated
         * \tparam T The updated component's type
         * \param p_entity The component's owner
         * \param p_component The updated component
         * \param p_newValue The component's new value
         */
        template <class T>
        static void OnBeforeChange(
            [[maybe_unused]] EntityHandle& p_entity, [[maybe_unused]] T& p_component, [[maybe_unused]] T& p_newValue)
        {
        }

        /**
         * \brief The action to perform after a component of the given type was updated
         * \tparam T The updated component's type
         * \param p_entity The component's owner
         * \param p_component The updated component
         */
        template <class T>
        static void OnChange([[maybe_unused]] EntityHandle& p_entity, [[maybe_unused]] T& p_component)
        {
        }

        /**
         * \brief The action to perform before a component of the given type is copied
         * \tparam T The updated component's type
         * \param p_from The source component's owner
         * \param p_source The copied component
         * \param p_to The copied component's owner
         * \return The component copy
         */
        template <class T>
        static T Copy([[maybe_unused]] EntityHandle& p_from, [[maybe_unused]] T& p_source, [[maybe_unused]] EntityHandle& p_to)
        {
            return p_source;
        }
    };
}

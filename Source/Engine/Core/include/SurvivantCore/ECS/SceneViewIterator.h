#pragma once
#include "SurvivantCore/ECS/ComponentStorage.h"
#include "SurvivantCore/ECS/EntityStorage.h"
#include "SurvivantCore/Utility/TypeTraits.h"

namespace SvCore::ECS
{
    template <bool IsConst, class... Components>
    class SceneViewIterator
    {
        static_assert(sizeof...(Components) > 0);
        static_assert(!Utility::HasDuplicates<Components...>);

        using iterator_type = std::conditional_t<IsConst, EntityStorage::const_iterator, EntityStorage::iterator>;
        using StorageTuple = std::tuple<ComponentStorage<Components>*...>;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename iterator_type::value_type;
        using difference_type = typename iterator_type::difference_type;
        using pointer = typename iterator_type::pointer;
        using reference = typename iterator_type::reference;

        /**
         * \brief Creates a default scene view iterator
         */
        SceneViewIterator() = default;

        /**
         * \brief Creates a scene view iterator starting and ending at the given positions for the given storages
         * \param p_current The start iterator
         * \param p_end The end iterator
         * \param p_storages The storages this iterator cares about
         */
        SceneViewIterator(iterator_type p_current, iterator_type p_end, const StorageTuple& p_storages);

        /**
         * \brief Creates a copy of the given scene view iterator
         * \param p_other The scene view iterator to copy
         */
        SceneViewIterator(const SceneViewIterator& p_other) = default;

        /**
         * \brief Creates a move copy of the given scene view iterator
         * \param p_other The scene view iterator to move
         */
        SceneViewIterator(SceneViewIterator&& p_other) noexcept = default;

        /**
         * \brief Destroys the scene view iterator
         */
        ~SceneViewIterator() = default;

        /**
         * \brief Assigns a copy of the given scene view iterator to this one
         * \param p_other The scene view iterator to copy
         * \return A reference to the modified scene view iterator
         */
        SceneViewIterator& operator=(const SceneViewIterator& p_other) = default;

        /**
         * \brief Moves the given scene view iterator into this one
         * \param p_other The scene view iterator to move
         * \return A reference to the modified scene view iterator
         */
        SceneViewIterator& operator=(SceneViewIterator&& p_other) noexcept = default;

        /**
         * \brief Checks if the given iterator is equal to this one
         * \param p_other The scene view iterator to compare against
         * \return True if the iterators are equal. False otherwise
         */
        bool operator==(const SceneViewIterator& p_other) const;

        /**
         * \brief Checks if the given iterator is not equal to this one
         * \param p_other The scene view iterator to compare against
         * \return True if the iterators are not equal. False otherwise
         */
        bool operator!=(const SceneViewIterator& p_other) const;

        /**
         * \brief Dereferences the scene view iterator
         * \return A reference to the iterated entity
         */
        reference operator*() const;

        /**
         * \brief Dereferences the scene view iterator
         * \return A pointer to the iterated entity
         */
        pointer operator->() const;

        /**
         * \brief Pre-increments the scene view iterator
         * \return A reference to the incremented iterator
         */
        SceneViewIterator& operator++();

        /**
         * \brief Post-increments the scene view iterator
         * \return A copy of the iterator before the increment
         */
        SceneViewIterator operator++(int);

    private:
        const StorageTuple* m_storages;
        iterator_type       m_iterator;
        iterator_type       m_end;

        /**
         * \brief Checks if the given entity is valid (not NULL_ENTITY & has all of the requested components)
         * \tparam Index The checked type's index
         * \param p_entity The checked entity
         * \return True if the entity is valid. False otherwise
         */
        template <size_t Index = 0>
        bool IsValid(value_type p_entity);
    };
}

#include "SurvivantCore/ECS/SceneViewIterator.inl"

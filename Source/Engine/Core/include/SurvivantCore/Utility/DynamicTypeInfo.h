#pragma once
#include <any>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

namespace SvCore::Utility
{
    template <typename T>
    using DynamicCallResult = std::conditional_t<std::is_same_v<void, T>, bool, std::optional<T>>;

    template <class TypeInfo>
    class DynamicTypeInfo : public TypeInfo
    {
    public:
        using Callable = std::function<std::any(void* /*payload*/)>;

        /**
         * \brief Creates empty dynamic type info and initializes the base type with the given parameters
         * \tparam Args The base type's construction parameter types
         * \param p_args The base type's construction parameters
         */
        template <typename... Args>
        DynamicTypeInfo(Args&&... p_args);

        /**
         * \brief Creates a copy of the given dynamic type info
         * \param p_other The dynamic type info to copy
         */
        DynamicTypeInfo(const DynamicTypeInfo& p_other) = default;

        /**
         * \brief Creates a move copy of the given dynamic type info
         * \param p_other The dynamic type info to move
         */
        DynamicTypeInfo(DynamicTypeInfo&& p_other) = default;

        /**
         * \brief Destroys the dynamic type info
         */
        ~DynamicTypeInfo() = default;

        /**
         * \brief Assigns a copy of the given dynamic type info to this one
         * \param p_other The dynamic type info to copy
         * \return A reference to the modified type info
         */
        DynamicTypeInfo& operator=(const DynamicTypeInfo& p_other) = default;

        /**
         * \brief Moves the given dynamic type info into this one
         * \param p_other The dynamic type info to move
         * \return A reference to the modified type info
         */
        DynamicTypeInfo& operator=(DynamicTypeInfo&& p_other) = default;

        /**
         * \brief Adds a callable with the given name to the dynamic type info
         * \param p_name The added callable's name
         * \param p_callable The added callable
         * \return True on success. False otherwise (i.e.: The name was already in use)
         */
        bool Add(const std::string& p_name, const Callable& p_callable);

        /**
         * \brief Adds a callable with the given name to the dynamic type info
         * \param p_name The added callable's name
         * \param p_callable The added callable
         * \return True on success. False otherwise (i.e.: The name was already in use)
         */
        void Replace(const std::string& p_name, const Callable& p_callable);

        /**
         * \brief Removes the callable with the given name from the dynamic type info
         * \param p_name The removed callable's name
         * \return True on success. False otherwise (i.e.: The name wasn't found)
         */
        bool Remove(const std::string& p_name);

        /**
         * \brief Checks whether a callable with the given name has been registered or not
         * \param p_name The searched callable's name
         * \return True if a callable with the given name was found. False otherwise
         */
        bool Has(const std::string& p_name) const;

        /**
         * \brief Calls the callable with the given name
         * \param p_name The callable's name
         * \param p_payload The callable's payload
         * \return The result of the call
         */
        template <typename T = void>
        DynamicCallResult<T> Call(const std::string& p_name, void* p_payload = nullptr) const;

    private:
        std::unordered_map<std::string /*name*/, Callable> m_callables;
    };
}

#include "SurvivantCore/Utility/DynamicTypeInfo.inl"

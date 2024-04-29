#pragma once
#include "SurvivantCore/Resources/IResource.h"
#include "SurvivantCore/Serialization/Serializer.h"

#include <type_traits>

namespace SvCore::Resources
{
    class ResourceRefBase
    {
    };

    template <class T>
    class ResourceRef : public ResourceRefBase
    {
        static_assert(std::is_same_v<IResource, T> || std::is_base_of_v<IResource, T>);

    public:
        using RefCountT = uint32_t;

        /**
         * \brief Creates an empty resource ref
         */
        ResourceRef() = default;

        /**
         * \brief Creates a handle to the resource with the given path
         * \param p_path The resource's path
         * \param p_resource A pointer to the referenced resource
         */
        ResourceRef(std::string p_path, T* p_resource);

        /**
         * \brief Creates a handle to the resource with the given path
         * \param p_path The resource's path
         */
        ResourceRef(const std::string& p_path);

        /**
         * \brief Creates a copy of the given resource reference
         * \param p_other The resource reference to copy
         */
        ResourceRef(const ResourceRef& p_other);

        /**
         * \brief Creates a move copy of the given resource reference
         * \param p_other The resource reference to move
         */
        ResourceRef(ResourceRef&& p_other) noexcept;

        /**
         * \brief Creates a copy of the given resource reference
         * \tparam U The other resource type
         * \param p_other The resource reference to copy
         */
        template <typename U>
        ResourceRef(const ResourceRef<U>& p_other);

        /**
         * \brief Creates a move copy of the given resource reference
         * \tparam U The other resource type
         * \param p_other The resource reference to move
         */
        template <typename U>
        ResourceRef(ResourceRef<U>&& p_other) noexcept;

        /**
         * \brief Destroys the resource reference
         */
        virtual ~ResourceRef();

        /**
         * \brief Assigns a copy of the given resource reference to this one
         * \param p_other The resource reference to copy
         * \return A reference to the modified resource reference
         */
        ResourceRef& operator=(const ResourceRef& p_other);

        /**
         * \brief Moves the given resource reference into this one
         * \param p_other The resource reference to move
         * \return A reference to the modified resource reference
         */
        ResourceRef& operator=(ResourceRef&& p_other) noexcept;

        /**
         * \brief Checks whether the given resource reference is equivalent to this one
         * \param p_other The compared resource reference
         * \return True if the other resource reference is equivalent to this one. False otherwise
         */
        bool operator==(const ResourceRef& p_other) const;

        /**
         * \brief Gets a reference to the referenced resource
         * \return A reference to the referenced resource
         */
        T& operator*() const;

        /**
         * \brief Gets a pointer to the referenced resource
         * \return A pointer to the referenced resource
         */
        T* operator->() const;

        /**
         * \brief Checks whether the resource reference has been set or not.
         * \return True if the resource ref is set. False otherwise.
         */
        virtual operator bool() const;

        /**
         * \brief Checks whether the resource can be converted to the given type
         * \tparam U The target resource type
         * \return True if the resource can be cast to the given type. False otherwise
         */
        template <typename U>
        bool CanCastTo() const;

        /**
         * \brief Gets a pointer to the referenced resource
         * \return A pointer to the referenced resource
         */
        T* Get() const;

        /**
         * \brief Gets a pointer to the referenced or default resource
         * \return A pointer to the referenced or default resource
         */
        T* GetOrDefault() const;

        /**
         * \brief Gets the number of active references to the resource
         * \return The number of active references to the resource
         */
        RefCountT GetReferenceCount() const;

        /**
         * \brief Gets the referenced resource's path
         * \return The referenced resource's path
         */
        const std::string& GetPath() const;

        /**
         * \brief Resets the resource reference
         */
        void Reset();

        /**
         * \brief Serializes the resource reference to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise.
         */
        virtual bool ToJson(Serialization::JsonWriter& p_writer) const;

        /**
         * \brief Deserializes the resource reference from json
         * \param p_json The input json data
         * \return True on success. False otherwise.
         */
        virtual bool FromJson(const Serialization::JsonValue& p_json);

    protected:
        template <typename U>
        friend class ResourceRef;

        std::string m_path;
        T*          m_resource = nullptr;
        RefCountT*  m_refCount = nullptr;
    };

    class GenericResourceRef final : public ResourceRef<IResource>
    {
    public:
        using TypeSizeT = uint8_t;

        /**
         * \brief Creates an empty generic resource ref
         */
        GenericResourceRef() = default;

        /**
         * \brief Creates a handle to the resource of the given type with the given path
         * \param p_type The resource's type
         * \param p_path The resource's path
         * \param p_resource A pointer to the referenced resource
         */
        GenericResourceRef(std::string p_type, const std::string& p_path, IResource* p_resource);

        /**
         * \brief Creates a handle to the resource of the given type with the given path
         * \param p_type The resource's type
         * \param p_path The resource's path
         */
        GenericResourceRef(const std::string& p_type, const std::string& p_path);

        /**
         * \brief Creates a copy of the given generic resource reference
         * \param p_other The generic resource reference to copy
         */
        GenericResourceRef(const GenericResourceRef& p_other) = default;

        /**
         * \brief Creates a move copy of the given generic resource reference
         * \param p_other The generic resource reference to move
         */
        GenericResourceRef(GenericResourceRef&& p_other) noexcept = default;

        /**
         * \brief Creates a generic copy of the given resource reference
         * \param p_other The resource reference to copy
         */
        template <typename T>
        GenericResourceRef(const ResourceRef<T>& p_other);

        /**
         * \brief Creates a generic move copy of the given resource reference
         * \param p_other The resource reference to move
         */
        template <typename T>
        GenericResourceRef(ResourceRef<T>&& p_other) noexcept;

        /**
         * \brief Creates a generic copy of the given resource reference with the given type
         * \param p_other The resource reference to copy
         * \param p_type The target type of the created generic resource reference
         */
        template <typename T>
        GenericResourceRef(const ResourceRef<T>& p_other, std::string p_type);

        /**
         * \brief Creates a generic move copy of the given resource reference with the given type
         * \param p_other The resource reference to move
         * \param p_type The target type of the created generic resource reference
         */
        template <typename T>
        GenericResourceRef(ResourceRef<T>&& p_other, std::string p_type) noexcept;

        /**
         * \brief Destroys the generic resource reference
         */
        ~GenericResourceRef() override = default;

        /**
         * \brief Assigns a copy of the given generic resource reference to this one
         * \param p_other The generic resource reference to copy
         * \return A reference to the modified generic resource reference
         */
        GenericResourceRef& operator=(const GenericResourceRef& p_other) = default;

        /**
         * \brief Moves the given generic resource reference into this one
         * \param p_other The generic resource reference to move
         * \return A reference to the modified generic resource reference
         */
        GenericResourceRef& operator=(GenericResourceRef&& p_other) noexcept = default;

        /**
         * \brief Checks whether the generic resource reference has been set or not.
         * \return True if the resource ref is set. False otherwise.
         */
        operator bool() const override;

        /**
         * \brief Checks whether the resource can be converted to the given type
         * \tparam U The target resource type
         * \return True if the resource can be cast to the given type. False otherwise
         */
        template <typename U>
        bool CanCastTo() const;

        /**
         * \brief Checks whether the resource can be converted to the given type
         * \param p_type The target resource type
         * \return True if the resource can be cast to the given type. False otherwise
         */
        bool CanCastTo(const std::string& p_type) const;

        /**
         * \brief Gets the referenced resource's type
         * \return The referenced resource's type
         */
        std::string GetType() const;

        /**
         * \brief Serializes the generic resource reference to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise.
         */
        bool ToJson(Serialization::JsonWriter& p_writer) const override;

        /**
         * \brief Deserializes the generic resource reference from json
         * \param p_json The input json data
         * \return True on success. False otherwise.
         */
        bool FromJson(const Serialization::JsonValue& p_json) override;

    private:
        std::string m_type;
    };
}

#include "SurvivantCore/Resources/ResourceRef.inl"

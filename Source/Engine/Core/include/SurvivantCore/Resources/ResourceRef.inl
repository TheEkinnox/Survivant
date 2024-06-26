#pragma once
#include "SurvivantCore/Resources/ResourceManager.h"
#include "SurvivantCore/Resources/ResourceRef.h"
#include "SurvivantCore/Resources/ResourceRegistry.h"
#include "SurvivantCore/Utility/FileSystem.h"

namespace SvCore::Resources
{
    template <class T>
    ResourceRef<T>::ResourceRef(std::string p_path, T* p_resource)
        : m_path(std::move(p_path)), m_resource(p_resource), m_refCount(p_resource ? new RefCountT(1) : nullptr)
    {
        Utility::ReplaceInPlace(m_path, "\\", "/");
    }

    template <class T>
    ResourceRef<T>::ResourceRef(const std::string& p_path)
        : ResourceRef(p_path, nullptr)
    {
    }

    template <class T>
    ResourceRef<T>::ResourceRef(const ResourceRef& p_other)
        : m_path(p_other.m_path), m_resource(p_other.m_resource), m_refCount(p_other.m_refCount)
    {
        if (m_refCount)
            ++(*m_refCount);
    }

    template <class T>
    ResourceRef<T>::ResourceRef(ResourceRef&& p_other) noexcept
        : m_path(std::move(p_other.m_path)), m_resource(p_other.m_resource), m_refCount(p_other.m_refCount)
    {
        p_other.m_resource = nullptr;
        p_other.m_refCount = nullptr;
    }

    template <class T>
    template <typename U>
    ResourceRef<T>::ResourceRef(const ResourceRef<U>& p_other)
        : m_path(p_other.m_path), m_resource(dynamic_cast<T*>(p_other.m_resource)),
        m_refCount(m_resource ? p_other.m_refCount : nullptr)
    {
        static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U> || std::is_base_of_v<U, T>,
            "Attempted to convert to an incompatible resource type");

        ASSERT((void*)p_other.m_resource == (void*)m_resource, "Attempted to convert to an incompatible resource type");

        if (m_refCount)
            ++(*m_refCount);
    }

    template <class T>
    template <typename U>
    ResourceRef<T>::ResourceRef(ResourceRef<U>&& p_other) noexcept
        : m_path(std::move(p_other.m_path)), m_resource(dynamic_cast<T*>(p_other.m_resource)),
        m_refCount(m_resource ? p_other.m_refCount : nullptr)
    {
        static_assert(std::is_same_v<T, U> || std::is_base_of_v<T, U> || std::is_base_of_v<U, T>,
            "Attempted to convert to an incompatible resource type");

        // Required for invalid conversions from IResource (can't just check for nullptr since it's a valid value)
        ASSERT((void*)p_other.m_resource == (void*)m_resource, "Attempted to convert to an incompatible resource type");

        p_other.m_resource = nullptr;
        p_other.m_refCount = nullptr;
    }

    template <class T>
    ResourceRef<T>::~ResourceRef()
    {
        Reset();
    }

    template <class T>
    ResourceRef<T>& ResourceRef<T>::operator=(const ResourceRef& p_other)
    {
        if (this == &p_other)
            return *this;

        if (p_other.m_refCount != m_refCount)
        {
            Reset();

            if (p_other.m_refCount)
                ++(*p_other.m_refCount);
        }

        m_path     = p_other.m_path;
        m_resource = p_other.m_resource;
        m_refCount = p_other.m_refCount;

        return *this;
    }

    template <class T>
    ResourceRef<T>& ResourceRef<T>::operator=(ResourceRef&& p_other) noexcept
    {
        if (this == &p_other)
            return *this;

        if (p_other.m_refCount != m_refCount)
            Reset();

        m_path     = std::move(p_other.m_path);
        m_resource = p_other.m_resource;
        m_refCount = p_other.m_refCount;

        p_other.m_resource = nullptr;
        p_other.m_refCount = nullptr;

        return *this;
    }

    template <class T>
    bool ResourceRef<T>::operator==(const ResourceRef& p_other) const
    {
        return m_path.empty() && p_other.m_path.empty() ? m_resource == p_other.m_resource : m_path == p_other.m_path;
    }

    template <class T>
    T& ResourceRef<T>::operator*() const
    {
        return *GetOrDefault();
    }

    template <class T>
    T* ResourceRef<T>::operator->() const
    {
        return GetOrDefault();
    }

    template <class T>
    ResourceRef<T>::operator bool() const
    {
        return GetOrDefault();
    }

    template <class T>
    template <typename U>
    bool ResourceRef<T>::CanCastTo() const
    {
        // DON'T check if T is a base of U (refs to IResource would evaluate to true even for incompatible resource types)
        if constexpr (std::is_same_v<T, U> || std::is_base_of_v<U, T>)
            return true;
        else
            return (void*)dynamic_cast<U*>(m_resource) == (void*)m_resource;
    }

    template <class T>
    T* ResourceRef<T>::Get() const
    {
        if (!m_resource && !m_path.empty())
            const_cast<ResourceRef&>(*this).Reload();

        return m_resource;
    }

    template <class T>
    T* ResourceRef<T>::GetOrDefault() const
    {
        return m_resource || !m_path.empty() ? Get() : GetDefaultResource<T>();
    }

    template <class T>
    typename ResourceRef<T>::RefCountT ResourceRef<T>::GetReferenceCount() const
    {
        return m_refCount ? *m_refCount : 0;
    }

    template <class T>
    const std::string& ResourceRef<T>::GetPath() const
    {
        return m_path;
    }

    template <class T>
    std::string ResourceRef<T>::GetFullPath() const
    {
        return ResourceManager::GetInstance().GetFullPath(m_path);
    }

    template <class T>
    bool ResourceRef<T>::Export(const bool p_pretty, const std::string& p_path) const
    {
        if (!m_resource)
            return false;

        std::error_code err;

        const ResourceManager& resourceManager = ResourceManager::GetInstance();
        std::filesystem::path  fullPath        = std::filesystem::absolute(resourceManager.GetFullPath(m_path), err);

        if (!CHECK(err.value() == 0, "Failed to get absolute resource path from \"%s\" - %s",
                m_path.c_str(), err.message().c_str()))
            return false;

        if (!p_path.empty())
        {
            const std::filesystem::path path = std::filesystem::absolute(resourceManager.GetFullPath(p_path), err);

            if (!CHECK(err.value() == 0, "Failed to get absolute target path from \"%s\" - %s",
                    p_path.c_str(), err.message().c_str()))
                return false;

            bool shouldCopy = fullPath != path && exists(fullPath, err);

            if (!CHECK(err.value() == 0, "Failed to check if path exists at \"%s\" - %s",
                    fullPath.string().c_str(), err.message().c_str()))
                return false;

            if (shouldCopy && !(
                CHECK(std::filesystem::create_directories(path.parent_path()) || err.value() == 0,
                    "Failed to create directories for \"%s\" - %s", path.string().c_str(), err.message().c_str()) &&
                CHECK(std::filesystem::copy_file(fullPath, path, err),
                    "Failed to copy resource from \"%s\" to \"%s\":\n%s", fullPath.c_str(), p_path.c_str(), err.message().c_str())))
                return false;

            fullPath = std::move(path);
        }
        else if (!CHECK(std::filesystem::create_directories(fullPath.parent_path(), err) || err.value() == 0,
                "Failed to create directories for \"%s\" - %s", fullPath.c_str(), err.message().c_str()))
        {
            return false;
        }

        return static_cast<IResource*>(m_resource)->Save(fullPath.string(), p_pretty);
    }

    template <class T>
    bool ResourceRef<T>::Reload()
    {
        if constexpr (!std::is_same_v<IResource, T>)
            return (*this) = ResourceManager::GetInstance().GetOrCreate<T>(m_path);
        else
            return (*this) = ResourceManager::GetInstance().Get<T>(m_path);
    }

    template <class T>
    void ResourceRef<T>::Reset()
    {
        if (m_refCount && --(*m_refCount) == 0)
        {
            delete m_refCount;
            delete m_resource;
        }

        m_refCount = nullptr;
        m_resource = nullptr;
    }

    template <class T>
    bool ResourceRef<T>::ToJson(Serialization::JsonWriter& p_writer) const
    {
        p_writer.StartObject();

        p_writer.Key("path");
        p_writer.String(m_path.c_str(), static_cast<rapidjson::SizeType>(m_path.size()));

        return p_writer.EndObject();
    }

    template <class T>
    bool ResourceRef<T>::FromJson(const Serialization::JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize resource ref - Json value should be an object"))
            return false;

        const auto it = p_json.FindMember("path");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize resource ref - Invalid resource path"))
            return false;

        m_path = std::string(it->value.GetString(), it->value.GetStringLength());

        const std::string basePath = m_path;

        if constexpr (!std::is_same_v<T, IResource>)
            (*this) = { m_path };

        return CHECK(basePath.empty() == m_path.empty(), "Unable to deserialize resource ref - Failed to load resource");
    }

    inline GenericResourceRef::GenericResourceRef(std::string p_type, const std::string& p_path, IResource* p_resource)
        : ResourceRef(p_path, p_resource), m_type(std::move(p_type))
    {
        ASSERT(!m_resource || m_resource->GetTypeName() == m_type,
            "Attempted to make generic resource ref with resource of a different type");
    }

    inline GenericResourceRef::GenericResourceRef(const std::string& p_type, const std::string& p_path)
        : GenericResourceRef(p_type, p_path, nullptr)
    {
    }

    template <typename T>
    GenericResourceRef::GenericResourceRef(const ResourceRef<T>& p_other)
        : ResourceRef(p_other), m_type(m_resource ? m_resource->GetTypeName() : std::string())
    {
    }

    template <typename T>
    GenericResourceRef::GenericResourceRef(ResourceRef<T>&& p_other) noexcept
        : ResourceRef(std::move(p_other)), m_type(m_resource ? m_resource->GetTypeName() : std::string())
    {
    }

    template <typename T>
    GenericResourceRef::GenericResourceRef(const ResourceRef<T>& p_other, std::string p_type)
        : ResourceRef(p_other), m_type(std::move(p_type))
    {
        ASSERT(!m_resource || m_resource->GetTypeName() == m_type,
            "Attempted to convert resource ref to generic resource ref of a different type");
    }

    template <typename T>
    GenericResourceRef::GenericResourceRef(ResourceRef<T>&& p_other, std::string p_type) noexcept
        : ResourceRef(std::move(p_other)), m_type(std::move(p_type))
    {
        ASSERT(!m_resource || m_resource->GetTypeName() == m_type,
            "Attempted to convert resource ref to generic resource ref of a different type");
    }

    inline GenericResourceRef::operator bool() const
    {
        return ResourceRef::operator bool() && !m_type.empty();
    }

    template <typename T>
    bool GenericResourceRef::CanCastTo() const
    {
        if (!m_resource)
            return true;

        const ResourceRegistry& resourceReg = ResourceRegistry::GetInstance();
        return (resourceReg.Contains<T>() && resourceReg.GetRegisteredTypeName<T>() == m_type) || ResourceRef().CanCastTo<T>();
    }

    inline bool GenericResourceRef::CanCastTo(const std::string& p_type) const
    {
        return !m_resource || m_resource->GetTypeName() == p_type;
    }

    inline IResource* GenericResourceRef::Get() const
    {
        if (!m_resource && !m_type.empty() && !m_path.empty())
            const_cast<GenericResourceRef&>(*this).Reload();

        return m_resource;
    }

    inline bool GenericResourceRef::Reload()
    {
        return (*this) = ResourceManager::GetInstance().GetOrCreate(m_type, m_path);
    }

    inline const std::string& GenericResourceRef::GetType() const
    {
        return m_type;
    }

    inline bool GenericResourceRef::ToJson(Serialization::JsonWriter& p_writer) const
    {
        p_writer.StartObject();

        p_writer.Key("type");
        p_writer.String(m_type.c_str(), static_cast<rapidjson::SizeType>(m_type.size()));

        p_writer.Key("path");
        p_writer.String(m_path.c_str(), static_cast<rapidjson::SizeType>(m_path.size()));

        return p_writer.EndObject();
    }

    inline bool GenericResourceRef::FromJson(const Serialization::JsonValue& p_json)
    {
        const auto it = p_json.FindMember("type");
        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize resource ref - Invalid resource type"))
            return false;

        m_type = std::string(it->value.GetString(), it->value.GetStringLength());

        if (!ResourceRef::FromJson(p_json))
            return false;

        const std::string basePath = m_path;

        (*this) = { m_type, m_path };
        return CHECK(basePath.empty() == m_path.empty(), "Unable to deserialize resource ref - Failed to load resource");
    }
}

#pragma once
#include "SurvivantCore/Resources/IResource.h"
#include "SurvivantCore/Resources/ResourceManager.h"
#include "SurvivantCore/Resources/ResourceRef.h"

namespace SvCore::Resources
{
    template <typename T>
    ResourceRef<T> ResourceManager::Load(const std::string& p_path)
    {
        static_assert(std::is_same_v<IResource, T> || std::is_base_of_v<IResource, T>);

        if (p_path.empty())
            return {};

        const std::string key = GetRelativePath(p_path);

        T*         resource = nullptr;
        const auto it       = m_resources.find(key);

        if (it != m_resources.end() && it->second)
        {
            IResource* savedResource = it->second->Get();

            resource = dynamic_cast<T*>(savedResource);

            if (!CHECK(!savedResource || resource || it->second->GetReferenceCount() <= 1,
                    "Unsafe reloading of resource at path \"%s\"", key.c_str()))
                return {};
        }

        const bool canReuse = resource;

        if (!canReuse)
            resource = CreateResource<T>();

        if (!LoadResource(resource, key))
        {
            if (it != m_resources.end())
                m_resources.erase(it);

            return {};
        }

        return canReuse ? *it->second : *(m_resources[key] = std::make_unique<ResourceRef<IResource>>(key, resource));
    }

    template <typename T>
    ResourceRef<T> ResourceManager::Get(const std::string& p_path) const
    {
        static_assert(std::is_same_v<IResource, T> || std::is_base_of_v<IResource, T>);

        if (p_path.empty())
            return {};

        const auto it = m_resources.find(GetRelativePath(p_path));

        if (it != m_resources.end())
            return *it->second;

        return ResourceRef<T>();
    }

    template <typename T>
    ResourceRef<T> ResourceManager::GetOrCreate(const std::string& p_path)
    {
        ResourceRef<T> resource = Get<T>(p_path);
        return resource ? resource : Load<T>(p_path);
    }

    template <typename T>
    std::vector<ResourceRef<T>> ResourceManager::GetAll() const
    {
        std::vector<ResourceRef<T>> resources;

        for (const auto& resource : m_resources | std::ranges::views::values)
        {
            if (!resource)
                continue;

            if (resource->CanCastTo<T>())
                resources.emplace_back(*resource);
        }

        return resources;
    }

    template <typename T>
    void ResourceManager::ReloadAll()
    {
        for (const auto& resource : m_resources | std::ranges::views::values)
        {
            if (resource && resource->CanCastTo<T>())
                Load<T>(resource->GetPath());
        }
    }

    template <typename T>
    void ResourceManager::RemoveAll()
    {
        for (auto it = m_resources.begin(); it != m_resources.end(); ++it)
        {
            if (it->second && it->second->CanCastTo<T>())
                it = m_resources.erase(it);
        }
    }
}

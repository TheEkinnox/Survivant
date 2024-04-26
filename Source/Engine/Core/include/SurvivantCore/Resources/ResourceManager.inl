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

        T*         resource = nullptr;
        const auto it       = m_resources.find(p_path);

        if (it != m_resources.end() && it->second)
        {
            IResource* savedResource = it->second->Get();

            resource = dynamic_cast<T*>(savedResource);

            if (!CHECK(!savedResource || resource || it->second->GetReferenceCount() <= 1,
                    "Unsafe reloading of resource at path \"%s\"", p_path.c_str()))
                return {};
        }

        const bool canReuse = resource;

        if (!resource)
            resource = CreateResource<T>();

        if (!LoadResource(resource, p_path))
        {
            if (it != m_resources.end())
                m_resources.erase(it);

            return {};
        }

        return canReuse ? *it->second : *(m_resources[p_path] = std::make_unique<ResourceRef<IResource>>(p_path, resource));
    }

    template <typename T>
    ResourceRef<T> ResourceManager::Get(const std::string& p_path) const
    {
        static_assert(std::is_same_v<IResource, T> || std::is_base_of_v<IResource, T>);

        if (p_path.empty())
            return {};

        const auto it = m_resources.find(p_path);

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

            if (ResourceRef<T> castResource = *resource)
                resources.push_back(castResource);
        }

        return resources;
    }
}

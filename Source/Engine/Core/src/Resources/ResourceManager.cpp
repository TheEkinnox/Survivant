#include "SurvivantCore/Resources/ResourceManager.h"

#include "SurvivantCore/Debug/Logger.h"
#include "SurvivantCore/Resources/IResource.h"
#include "SurvivantCore/Resources/ResourceRegistry.h"
#include "SurvivantCore/Utility/FileSystem.h"

#include <ranges>

namespace SvCore::Resources
{
    ResourceManager::ResourceManager(std::vector<std::string> p_searchPaths)
        : m_searchPaths(std::move(p_searchPaths))
    {
    }

    ResourceManager::~ResourceManager()
    {
        Clear();
    }

    ResourceManager& ResourceManager::operator=(const ResourceManager& p_other)
    {
        if (this == &p_other)
            return *this;

        m_searchPaths = p_other.m_searchPaths;

        return *this;
    }

    ResourceManager& ResourceManager::operator=(ResourceManager&& p_other) noexcept
    {
        if (this == &p_other)
            return *this;

        m_resources   = std::move(p_other.m_resources);
        m_searchPaths = std::move(p_other.m_searchPaths);

        return *this;
    }

    ResourceManager& ResourceManager::GetInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    GenericResourceRef ResourceManager::Create(const std::string& p_type, const std::string& p_path, const bool p_shouldLoad)
    {
        if (p_type.empty() || p_path.empty())
            return {};

        IResource* resource = nullptr;
        const auto it       = m_resources.find(p_path);

        if (it != m_resources.end() && it->second)
        {
            IResource* savedResource = it->second->Get();

            resource = !savedResource || savedResource->GetTypeName() == p_type ? savedResource : nullptr;

            if (!CHECK(!savedResource || resource || it->second->GetReferenceCount() <= 1,
                    "Unsafe reloading of resource at path \"%s\"", p_path.c_str()))
                return {};
        }

        const bool canReuse = resource;

        if (!resource)
            resource = ResourceRegistry::GetInstance().Create(p_type);

        if (p_shouldLoad && !LoadResource(resource, p_path))
        {
            m_resources.erase(it);
            return {};
        }

        return canReuse ? *it->second : *(m_resources[p_path] = std::make_unique<GenericResourceRef>(p_type, p_path, resource));
    }

    GenericResourceRef ResourceManager::Get(const std::string& p_type, const std::string& p_path) const
    {
        if (p_type.empty() || p_path.empty())
            return {};

        const auto it = m_resources.find(p_path);

        if (it == m_resources.end())
            return {};

        GenericResourceRef* resource = dynamic_cast<GenericResourceRef*>(it->second.get());

        if (!resource)
            return { *it->second, p_type };

        return resource->GetType() == p_type ? *resource : GenericResourceRef();
    }

    GenericResourceRef ResourceManager::GetOrCreate(const std::string& p_type, const std::string& p_path)
    {
        GenericResourceRef resource = Get(p_type, p_path);
        return resource ? resource : Create(p_type, p_path, true);
    }

    std::vector<char> ResourceManager::ReadFile(const std::string& p_path) const
    {
        if (p_path.empty())
            return {};

        std::vector<char> resourceData;

        std::ifstream fileStream(GetFullPath(p_path), std::ios::binary | std::ios::ate);

        if (!fileStream.is_open())
            return {};

        const std::ifstream::pos_type length = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);

        resourceData.resize(length);
        fileStream.read(resourceData.data(), length);
        fileStream.close();

        return resourceData;
    }

    void ResourceManager::Remove(const std::string& p_path)
    {
        m_resources.erase(p_path);
    }

    void ResourceManager::Clear()
    {
        m_resources.clear();
    }

    std::vector<std::string> ResourceManager::GetSearchPaths() const
    {
        return m_searchPaths;
    }

    void ResourceManager::SetSearchPaths(std::vector<std::string> p_searchPaths)
    {
        m_searchPaths = std::move(p_searchPaths);
    }

    void ResourceManager::AddSearchPath(const std::string& p_path)
    {
        const auto it = std::ranges::find(m_searchPaths, p_path);

        if (it == m_searchPaths.end())
            m_searchPaths.emplace_back(p_path);
    }

    void ResourceManager::RemoveSearchPath(const std::string& p_path)
    {
        m_searchPaths.erase(std::ranges::find(m_searchPaths, p_path));
    }

    std::string ResourceManager::GetFullPath(const std::string& p_path) const
    {
        if (Utility::PathExists(p_path))
            return p_path;

        for (const auto& searchPath : m_searchPaths)
        {
            const std::string fullPath = Utility::AppendPath(searchPath, p_path);

            if (Utility::PathExists(fullPath))
                return fullPath;
        }

        return p_path;
    }

    bool ResourceManager::LoadResource(IResource* p_resource, const std::string& p_path) const
    {
        return p_resource->Load(GetFullPath(p_path)) && p_resource->Init();
    }
}
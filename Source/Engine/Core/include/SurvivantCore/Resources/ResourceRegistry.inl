#pragma once
#include "SurvivantCore/Resources/ResourceRegistry.h"

namespace SvCore::Resources
{
    template <typename T, typename... Args>
    T* CreateResource(Args&&... p_args)
    {
        return new T(std::forward<Args>(p_args)...);
    }

    template <typename T>
    T* GetDefaultResource()
    {
        return nullptr;
    }

    inline ResourceRegistry& ResourceRegistry::GetInstance()
    {
        static ResourceRegistry instance;
        return instance;
    }

    inline IResource* ResourceRegistry::Create(const std::string& p_type) const
    {
        return ASSUME(Contains(p_type)) ? GetTypeInfo(p_type).Allocate() : nullptr;
    }

    inline IResource* ResourceRegistry::GetDefault(const std::string& p_type) const
    {
        return ASSUME(Contains(p_type)) ? GetTypeInfo(p_type).GetDefault() : nullptr;
    }

    template <typename T>
    void ResourceRegistry::RegisterType(const std::string& p_name)
    {
        const ResourceTypeInfo typeInfo
        {
            .Allocate = []
            {
                return static_cast<IResource*>(CreateResource<T>());
            },
            .GetDefault = []
            {
                return static_cast<IResource*>(GetDefaultResource<T>());
            }
        };

        TypeRegistry::RegisterType<T>(p_name, typeInfo);
    }
}

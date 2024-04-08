#pragma once
#include "SurvivantCore/Utility/DynamicTypeInfo.h"
#include "SurvivantCore/Utility/TypeRegistry.h"

#include <cstdint>

#define REGISTER_RESOURCE_TYPE(Name, Type)                                                                        \
static uint8_t resReg_##Name = (SvCore::Resources::ResourceRegistry::GetInstance().RegisterType<Type>(#Name), 0);

namespace SvCore::Resources
{
    class IResource;

    template <typename T, typename... Args>
    T* CreateResource(Args&&... p_args);

    template <typename T>
    T* GetDefaultResource();

    struct ResourceTypeInfo
    {
        decltype(&CreateResource<IResource>)     Allocate;
        decltype(&GetDefaultResource<IResource>) GetDefault;
    };

#ifdef SV_EDITOR
    class ResourceRegistry final : public Utility::TypeRegistry<Utility::DynamicTypeInfo<ResourceTypeInfo>>
#else
    class ResourceRegistry final : public Utility::TypeRegistry<ResourceTypeInfo>
#endif
    {
    public:
        /**
         * \brief Gets the current resource registry instance
         * \return A reference to the current resource registry
         */
        static ResourceRegistry& GetInstance();

        /**
         * \brief Registers the given component type with the given name
         * \tparam T The component type to register
         */
        template <typename T>
        void RegisterType(const std::string& p_name);

        /**
         * \brief Tries to allocate a resource of the given registered resource type.
         * \param p_type The type of the resource to create
         * \return A pointer to the allocated resource on success, nullptr otherwise
         */
        IResource* Create(const std::string& p_type) const;

        /**
         * \brief Gets the default resource for the given registered resource type
         * \param p_type The target resource type
         * \return The default resource of the given type
         */
        IResource* GetDefault(const std::string& p_type) const;
    };
}

#include "SurvivantCore/Resources/ResourceRegistry.inl"

#pragma once
#include <unordered_map>

#include "SurvivantCore/Debug/Assertion.h"

#define SV_SERVICE(Type) SvCore::Utility::ServiceLocator::Get<Type>()

namespace SvCore::Utility
{
    class ServiceLocator
    {
    public:
        /**
         * \brief Provides a service of the given type to the service locator.
         * \tparam T The provided service's type
         * \param p_service The provided service
         */
        template <typename T>
        static void Provide(T& p_service)
        {
            s_services[typeid(T).hash_code()] = &p_service;
        }

        /**
         * \brief Gets the service of the given type.
         * \tparam T The service's type
         * \return A reference to the previously provided service.
         */
        template <typename T>
        static T& Get()
        {
            ASSERT(s_services.contains(typeid(T).hash_code()));
            return *static_cast<T*>(s_services[typeid(T).hash_code()]);
        }

    private:
        inline static std::unordered_map<size_t, void*> s_services;
    };
}

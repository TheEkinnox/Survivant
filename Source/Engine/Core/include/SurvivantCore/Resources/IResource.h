#pragma once
#include "SurvivantCore/Resources/ResourceRegistry.h"

#include <string>

#define REGISTERED_RESOURCE_BODY()                                                                      \
public:                                                                                                 \
inline const std::string& GetTypeName() const                                                           \
{                                                                                                       \
    return SvCore::Resources::ResourceRegistry::GetInstance().GetRegisteredTypeName<decltype(*this)>(); \
}                                                                                                       \
private:

namespace SvCore::Resources
{
    class IResource
    {
    public:
        /**
         * \brief Destroys the resource
         */
        virtual ~IResource() = default;

        /**
         * \brief Gets the resource's registered type name
         * \return The resource's registered type name
         */
        virtual const std::string& GetTypeName() const = 0;

        /**
         * \brief Loads the resource from the given file
         * \param p_fileName The resource file's path
         * \return True if the resource was successfully loaded. False otherwise.
         */
        virtual bool Load(const std::string& p_fileName) = 0;

        /**
         * \brief Initializes the resource
         * \return True on success. False otherwise.
         */
        virtual bool Init() = 0;
    };
}

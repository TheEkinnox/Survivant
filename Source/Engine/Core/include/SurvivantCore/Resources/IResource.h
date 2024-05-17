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

        /**
         * \brief Saves the necessary info to load the resource at the given path
         * \param p_path The resource file's path
         * \param p_pretty Whether the output should be human-friendly
         * \return True on success. False otherwise
         */
        virtual bool Save(const std::string& p_path, bool p_pretty = false) = 0;

    protected:
        /**
         * \brief Gets the path to the meta file associated with the given resource
         * \param p_fileName The resource file's path
         * \return The path to the meta file associated with the given resource
         */
        static std::string GetMetaPath(const std::string& p_fileName)
        {
            return !p_fileName.empty() ? p_fileName + META_EXTENSION : p_fileName;
        }

    private:
        static constexpr const char* META_EXTENSION = ".meta";

        friend auto operator<=>(const IResource&, const IResource&) = default;
    };
}

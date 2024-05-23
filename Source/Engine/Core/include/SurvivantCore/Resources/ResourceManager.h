#pragma once
#include <memory>
#include <string>
#include <unordered_map>

namespace SvCore::Resources
{
    class IResource;

    template <class T>
    class ResourceRef;
    class GenericResourceRef;

    class ResourceManager
    {
        using ResourceMap = std::unordered_map<std::string, std::unique_ptr<ResourceRef<IResource>>>;

    public:
        /**
         * \brief Creates a default resource manager
         */
        ResourceManager() = default;

        /**
         * \brief Creates a resource manager with the given search paths
         * \param p_searchPaths The resource manager's search paths
         */
        ResourceManager(std::vector<std::string> p_searchPaths);

        /**
         * \brief Creates a copy of the given resource manager
         * \param p_other A reference to the resource manager to copy
         */
        ResourceManager(const ResourceManager& p_other) = default;

        /**
         * \brief Creates a move copy of the given resource manager
         * \param p_other The resource manager to move
         */
        ResourceManager(ResourceManager&& p_other) noexcept = default;

        /**
         * \brief Destroys the resource manager and frees the memory allocated for it's loaded resources
         */
        ~ResourceManager();

        /**
         * \brief Copies the given resource manager's data into the current one
         * \param p_other A reference to the resource manager to copy
         */
        ResourceManager& operator=(const ResourceManager& p_other);

        /**
         * \brief Moves the given resource manager's data into the current one
         * \param p_other The resource manager to move
         */
        ResourceManager& operator=(ResourceManager&& p_other) noexcept;

        /**
         * \brief Gets a reference to the current resource manager instance
         * \return A reference to the current resource manager instance
         */
        static ResourceManager& GetInstance();

        /**
         * \brief Tries to load the resource with the given path
         * \tparam T The resource's type
         * \param p_path The path of the resource's file
         * \return A pointer to the resource on success, nullptr otherwise
         */
        template <typename T>
        ResourceRef<T> Load(const std::string& p_path);

        /**
         * \brief Tries to create an empty resource of the given type
         * \param p_type The resource's type
         * \param p_path The resource's path
         * \return A pointer to the created resource on success, nullptr otherwise
         */
        GenericResourceRef Create(const std::string& p_type, const std::string& p_path);

        /**
         * \brief Tries to find the resource with the given path.
         * \tparam T The resource's type
         * \param p_path The resource's path
         * \return A pointer to the resource on success, nullptr otherwise.
         */
        template <typename T>
        ResourceRef<T> Get(const std::string& p_path) const;

        /**
         * \brief Tries to find the resource of the given type with the given path.
         * \param p_type The resource's type
         * \param p_path The resource's path
         * \return A pointer to the resource on success, nullptr otherwise.
         */
        GenericResourceRef Get(const std::string& p_type, const std::string& p_path) const;

        /**
         * \brief Tries to find the resource with the given path. If not found, tries to load it
         * \tparam T The resource's type
         * \param p_path The resource's path
         * \return A pointer to the resource on success, nullptr otherwise.
         */
        template <typename T>
        ResourceRef<T> GetOrCreate(const std::string& p_path);

        /**
         * \brief Tries to find the resource with the given path. If not found, tries to load it
         * \param p_type The resource's type
         * \param p_path The resource's path
         * \return A pointer to the resource on success, nullptr otherwise.
         */
        GenericResourceRef GetOrCreate(const std::string& p_type, const std::string& p_path);

        /**
         * \brief Gets all the loaded resources of the given type
         * \tparam T The target resource type
         * \return The list of all loaded resources of the given type
         */
        template <typename T>
        std::vector<ResourceRef<T>> GetAll() const;

        /**
         * \brief Gets all the loaded resources of the given type
         * \param p_type The target resource type
         * \return The list of all loaded resources of the given type
         */
        std::vector<GenericResourceRef> GetAll(const std::string& p_type) const;

        /**
         * \brief Reloads all the loaded resources of the given type
         * \tparam T The target resource type
         */
        template <typename T>
        void ReloadAll();

        /**
         * \brief Reloads all the loaded resources of the given type
         * \param p_type The target resource type
         */
        void ReloadAll(const std::string& p_type);

        /**
         * \brief Reads the content of the resource at the given path
         * \param p_path The path of the resource file to read
         * \return The resource's file content
         */
        std::vector<char> ReadFile(const std::string& p_path) const;

        /**
         * \brief Removes the resource with the given path from the manager
         * \param p_path The resource's path
         */
        void Remove(const std::string& p_path);

        /**
         * \brief Removes all the loaded resources of the given type from the manager
         * \tparam T The target resource type
         */
        template <typename T>
        void RemoveAll();

        /**
         * \brief Removes all the loaded resources of the given type from the manager
         * \param p_type The target resource type
         */
        void RemoveAll(const std::string& p_type);

        /**
         * \brief Removes all loaded resources from the manager
         */
        void Clear();

        /**
         * \brief Gets the resource manager's search paths
         * \return The resource manager's search paths
         */
        std::vector<std::string> GetSearchPaths() const;

        /**
         * \brief Sets the resource manager's search paths
         * \param p_searchPaths The resource manager's new search paths
         */
        void SetSearchPaths(std::vector<std::string> p_searchPaths);

        /**
         * \brief Adds the given path to the resource manager's search paths
         * \param p_path The search path to add
         */
        void AddSearchPath(const std::string& p_path);

        /**
         * \brief Removes the given path from the resource manager's search paths
         * \param p_path The search path to remove
         */
        void RemoveSearchPath(const std::string& p_path);

        /**
         * \brief Gets the full path of a resource from its relative path
         * \param p_path The resource's relative path
         * \return The resource's path including the search path
         */
        std::string GetFullPath(const std::string& p_path) const;

        /**
         * \brief Gets the relative path of a resource from its full path
         * \param p_path The resource's full path
         * \return The resource's relative path
         */
        std::string GetRelativePath(std::string p_path) const;

    private:
        ResourceMap m_resources;

        std::vector<std::string> m_searchPaths;

        /**
         * \brief Loads and initializes the given resource
         * \param p_resource The resource to load
         * \param p_path The resource's path
         * \return True on success. False otherwise.
         */
        bool LoadResource(IResource* p_resource, const std::string& p_path) const;
    };
}

#include "SurvivantCore/Resources/ResourceManager.inl"

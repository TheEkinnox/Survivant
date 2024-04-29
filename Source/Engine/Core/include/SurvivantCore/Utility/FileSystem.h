#pragma once
#include <string>

namespace SvCore::Utility
{
    /**
     * \brief Gets the calling application's directory
     * \return The calling application's directory
     */
    const char* GetApplicationDirectory();

    /**
     * \brief Gets the current working directory
     * \return The current working directory
     */
    std::string GetWorkingDirectory();

    /**
     * \brief Sets the application's working directory
     * \param p_directory The new working directory
     * \return True on success. False otherwise
     */
    bool SetWorkingDirectory(const std::string& p_directory);

    /**
     * \brief Converts the given path's separator to the system's preferred one
     * \param p_path The path to convert
     * \return The converted path
     */
    std::string MakePreferred(const std::string& p_path);

    /**
     * \brief Appends the given path to the given root
     * \param p_root The root search path
     * \param p_path The path to append to the root
     * \return The concatenated paths
     */
    std::string AppendPath(const std::string& p_root, const std::string& p_path);

    /**
     * \brief Converts the given path to an absolute path
     * \param p_path The path to convert to an absolute path
     * \return The absolute path
     */
    std::string GetAbsolutePath(const std::string& p_path);

    /**
     * \brief Checks whether the given file path exists
     * \param p_path The path to check
     * \return True if the path exists. False otherwise
     */
    bool PathExists(const std::string& p_path);
}

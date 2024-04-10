#pragma once

#include <string>
#include <vector>

namespace SvCore::Utility
{
    /**
     * \brief Formats a string using the printf syntax
     * \tparam Args The arguments to insert into the format string
     * \param p_format The format string (follows printf syntax)
     * \param p_args The arguments to insert into the format string
     * \return The formatted string
     */
    template <class... Args>
    std::string FormatString(const std::string& p_format, Args&&... p_args);

    /**
     * \brief Splits the given string using the given delimiter
     * \param p_str The string to split
     * \param p_delimiter The delimiter to use to split the string
     * \param p_includeEmpty Whether empty substrings should be included in the result
     * \return A vector containing the sub-strings
     */
    std::vector<std::string> SplitString(const std::string& p_str, const char* p_delimiter, bool p_includeEmpty);

    /**
     * \brief Trims the beginning of the given string using the given compare func
     * \param p_str The string to trim
     * \param p_compareFunc The function used to check if a character should be trimmed
     */
    template <class CompareFunc>
    void TrimStringStart(std::string& p_str, CompareFunc p_compareFunc = &isspace);

    /**
     * \brief Trims the end of the given string using the given compare func
     * \param p_str The string to trim
     * \param p_compareFunc The function used to check if a character should be trimmed
     */
    template <class CompareFunc>
    void TrimStringEnd(std::string& p_str, CompareFunc p_compareFunc = &isspace);

    /**
     * \brief Trims the given string using the given compare func
     * \param p_str The string to trim
     * \param p_compareFunc The function used to check if a character should be trimmed
     */
    template <class CompareFunc>
    void TrimString(std::string& p_str, CompareFunc p_compareFunc = &isspace);

    /**
     * \brief Converts a string to upper case
     * \param p_str The string to convert to upper case
     */
    void ToUpperInPlace(std::string& p_str);

    /**
     * \brief Converts a string to lower case
     * \param p_str The string to convert to lower case
     */
    void ToLowerInPlace(std::string& p_str);

    /**
     * \brief Converts a string to upper case
     * \param p_str The string to convert to upper case
     * \return The string converted to upper case
     */
    std::string ToUpper(std::string p_str);

    /**
     * \brief Converts a string to lower case
     * \param p_str The string to convert to lower case
     * \return The string converted to lower case
     */
    std::string ToLower(std::string p_str);

    /**
     * \brief Replaces the given substring by another one in the given string
     * \param p_str The source string
     * \param p_from The substring to replace
     * \param p_to The substring to replace by
     */
    void ReplaceInPlace(std::string& p_str, const std::string& p_from, const std::string& p_to);

    /**
     * \brief Replaces the given substring by another one in the given string
     * \param p_str The source string
     * \param p_from The substring to replace
     * \param p_to The substring to replace by
     */
    std::string Replace(std::string p_str, const std::string& p_from, const std::string& p_to);

    /// <summary>
    /// Compare string without while ignoring case sensitivity
    /// </summary>
    /// <param name="p_str1"> First string </param>
    /// <param name="p_str2"> secon setring </param>
    /// <returns>-1 if less, 0 if equals, 1 if greater</returns>
    int CompareAlphabeticly(std::string p_str1, std::string p_str2);
}

#include "SurvivantCore/Utility/Utility.inl"

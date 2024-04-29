#include "SurvivantCore/Utility/Utility.h"

#include <sstream>

namespace SvCore::Utility
{
    std::vector<std::string> SplitString(const std::string& p_str, const char* p_delimiter, const bool p_includeEmpty)
    {
        std::vector<std::string> result{};

        if (p_str.empty())
        {
            if (p_includeEmpty)
                result.push_back(p_str);

            return result;
        }

        size_t start = 0;
        size_t end   = p_str.find(p_delimiter, start);

        while (start < p_str.size())
        {
            std::string token = p_str.substr(start, end - start);

            if (p_includeEmpty || !token.empty())
                result.push_back(std::move(token));

            if (end == std::string::npos)
                break;

            start = end + strlen(p_delimiter);
            end   = p_str.find(p_delimiter, start);
        }

        return result;
    }

    void ToUpperInPlace(std::string& p_str)
    {
        std::ranges::transform(p_str, p_str.begin(), [](const char p_c)
        {
            return static_cast<char>(std::toupper(p_c));
        });
    }

    void ToLowerInPlace(std::string& p_str)
    {
        std::ranges::transform(p_str, p_str.begin(), [](const char p_c)
        {
            return static_cast<char>(std::tolower(p_c));
        });
    }

    std::string ToUpper(std::string p_str)
    {
        ToUpperInPlace(p_str);
        return p_str;
    }

    std::string ToLower(std::string p_str)
    {
        ToLowerInPlace(p_str);
        return p_str;
    }

    void ReplaceInPlace(std::string& p_str, const std::string& p_from, const std::string& p_to)
    {
        if (p_str.empty() || p_from.empty() || p_from.size() > p_str.size())
            return;

        size_t startPos = 0;
        while ((startPos = p_str.find(p_from, startPos)) != std::string::npos)
        {
            p_str.replace(startPos, p_from.length(), p_to);
            startPos += p_to.length();
        }
    }

    std::string Replace(std::string p_str, const std::string& p_from, const std::string& p_to)
    {
        ReplaceInPlace(p_str, p_from, p_to);
        return p_str;
    }

    int CompareAlphabeticly(std::string p_str1, std::string p_str2)
    {
        size_t len = p_str1.size() < p_str2.size() ? p_str1.size() : p_str2.size();

        for (size_t i = 0; i < len; i++)
        {
            auto a = std::tolower(static_cast<unsigned char>(p_str1[i]));
            auto b = std::tolower(static_cast<unsigned char>(p_str2[i]));

            if (a != b)
                return a < b ? -1 : 1;
        }

        return 0;
    }
}

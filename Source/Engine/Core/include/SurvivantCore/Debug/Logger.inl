#pragma once
#include <cassert>
#include <fstream>
#include <iostream>

#include "SurvivantCore/Debug/Logger.h"
#include "SurvivantCore/Utility/Utility.h"

namespace SvCore::Debug
{
    inline void Logger::SetFile(const std::filesystem::path& p_filePath)
    {
        m_filePath = p_filePath;
    }

    inline Logger& Logger::GetInstance()
    {
        static Logger instance;
        return instance;
    }

    template <typename... Args>
    void Logger::Print(const char* p_format, const ELogType p_type, Args... p_args)
    {
        const std::string message = Utility::FormatString(p_format, p_args...);

        m_onPrint.Invoke({ p_type, message });

        (p_type == ELogType::ERROR_LOG ? std::cerr : std::cout) << message << std::flush;

        if (m_filePath.empty())
            return;

        std::ofstream file(m_filePath, std::ios::app);
        assert(file.is_open());

        switch (p_type)
        {
        case ELogType::DEBUG_LOG:
            file << "[INFO] " << message << std::flush;
            break;
        case ELogType::WARNING_LOG:
            file << "[WARNING] " << message << std::flush;
            break;
        case ELogType::ERROR_LOG:
            file << "[ERROR] " << message << std::flush;
            break;
        case ELogType::DEFAULT_LOG:
        default:
            file << message << std::flush;
            break;
        }
    }

    template <typename... Args>
    void Logger::DebugLog([[maybe_unused]] const char*  p_file,
                          [[maybe_unused]] const size_t p_line, const char* p_format, const ELogType p_type, Args... p_args)
    {
        std::string message = Utility::FormatString(p_format, p_args...);

#if defined(_DEBUG) || defined(SV_VERBOSE_LOG)
        message = Utility::FormatString("%s(%d): %s\n", p_file, p_line, message.c_str());
#else
        message += '\n';
#endif // _DEBUG || SV_VERBOSE_LOG

        Print(message.c_str(), p_type);
    }
}

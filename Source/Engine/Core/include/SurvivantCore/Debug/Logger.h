#pragma once

#include "SurvivantCore/Debug/ELogType.h"
#include "SurvivantCore/Events/Event.h"

#include <filesystem>

#ifndef SV_LOG
#define SV_LOG(format, ...) SvCore::Debug::Logger::GetInstance().DebugLog(__FILE__, __LINE__, format, SvCore::Debug::ELogType::DEBUG_LOG __VA_OPT__(,) __VA_ARGS__)
#define SV_LOG_WARNING(format, ...) SvCore::Debug::Logger::GetInstance().DebugLog(__FILE__, __LINE__, format, SvCore::Debug::ELogType::WARNING_LOG __VA_OPT__(,) __VA_ARGS__)
#define SV_LOG_ERROR(format, ...) SvCore::Debug::Logger::GetInstance().DebugLog(__FILE__, __LINE__, format, SvCore::Debug::ELogType::ERROR_LOG __VA_OPT__(,) __VA_ARGS__)
#endif //SV_LOG

namespace SvCore::Debug
{
    struct LogInfo
    {
        ELogType    m_type;
        std::string m_message;
    };

    class Logger
    {
    public:
        Events::Event<LogInfo> m_onPrint;

        Logger()                          = default;
        Logger(const Logger& p_other)     = default;
        Logger(Logger&& p_other) noexcept = default;
        ~Logger()                         = default;

        Logger& operator=(const Logger& p_other)     = default;
        Logger& operator=(Logger&& p_other) noexcept = default;

        /**
         * \brief Sets the given file as the log output.
         * \param p_filePath The log file's path
         */
        inline void SetFile(const std::filesystem::path& p_filePath);

        /**
         * \brief Logs a message with the given format following printf's syntax.
         * \tparam Args The arguments to insert into the format string
         * \param p_format The format of the message
         * \param p_type The type of message getting logged
         * \param p_args Additional arguments to insert into the message
         */
        template <typename... Args>
        void Print(const char* p_format, ELogType p_type, Args... p_args);

        /**
         * \brief Logs a message with the given format following printf's syntax.
         * Appends the given file path and line at the beginning of the message
         * \tparam Args The arguments to insert into the format string
         * \param p_file The file for which the function was called
         * \param p_line The line for which the function was called
         * \param p_format The format of the message
         * \param p_type The type of message getting logged
         * \param p_args Additional arguments to insert into the message
         */
        template <typename... Args>
        void DebugLog(const char* p_file, size_t p_line, const char* p_format, ELogType p_type, Args... p_args);

        /**
        * \brief Accessor to the Logger singleton
        * \return A reference to the current Logger instance
        */
        inline static Logger& GetInstance();

    private:
        std::filesystem::path m_filePath;
    };
}

#include "SurvivantCore/Debug/Logger.inl"

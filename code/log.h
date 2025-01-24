#pragma once
#include <cstdio>
#include <ctime>

// NOTE(sbalse): Not meant to be used outside of this file.
namespace Internal
{
    enum class LogLevel
    {
        INFO,
        ERROR
    };

    constexpr const char* LOG_COLOR_RESET = "\033[0m";
    constexpr const char* LOG_COLOR_GREEN = "\033[32m";
    constexpr const char* LOG_COLOR_RED = "\033[31m";

    void GetTimestamp(char* const buffer, const size_t size)
    {
        const std::time_t now = std::time(nullptr);
        std::tm localTime = {};
        localtime_s(&localTime, &now);
        std::strftime(buffer, size, "%Y-%m-%d %H:%M:%S", &localTime);
    }

    template<typename... Args>
    void LogMessage(
        const LogLevel level,
        const char* color,
        const char* format,
        const Args... args)
    {
        constexpr size_t timestampSize = 32;
        char timestamp[timestampSize] = {};
        GetTimestamp(timestamp, timestampSize);

        constexpr size_t messageSize = 1024;
        char message[messageSize] = {};
        if constexpr (sizeof...(args) > 0)
        {
            std::snprintf(message, messageSize, format, args...);
        }
        else
        {
            std::snprintf(message, messageSize, "%s", format);
        }

        std::FILE* outStream = nullptr;
        const char* logLevelStr = nullptr;

        switch (level)
        {
        case LogLevel::ERROR:
        {
            outStream = stderr;
            logLevelStr = "ERROR";
        } break;

        case LogLevel::INFO:
        {
            outStream = stdout;
            logLevelStr = "INFO";
        } break;

        default:
        {
            outStream = stdout;
            logLevelStr = "UNKNOWN";
        } break;
        }

        std::fprintf(
            outStream,
            "%s[%s] %s: %s%s\n",
            color,
            logLevelStr,
            timestamp,
            message,
            LOG_COLOR_RESET);
    }
} // namespace Internal

  // NOTE(sbalse): Macros for logging.
#define LOG_INFO(...) \
do \
{ \
    LogMessage(Internal::LogLevel::INFO, Internal::LOG_COLOR_GREEN, __VA_ARGS__); \
} while (0);

#define LOG_ERROR(...) \
do \
{ \
    LogMessage(Internal::LogLevel::ERROR, Internal::LOG_COLOR_RED, __VA_ARGS__); \
} while (0);

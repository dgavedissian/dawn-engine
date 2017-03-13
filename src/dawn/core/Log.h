/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#if defined(DW_MSVC)
#pragma warning(push)
#pragma warning(disable : 4127 4100)
#endif

#define TINYFORMAT_USE_VARIADIC_TEMPLATES
#include <tinyformat.h>

#if defined(DW_MSVC)
#pragma warning(pop)
#endif

namespace dw {

enum class LogLevel { Debug, Info, Warning, Error };

class DW_API LogMessageHandler {
public:
    LogMessageHandler() = default;
    virtual ~LogMessageHandler() = default;

    virtual void onMessage(LogLevel level, const String& message) = 0;
};

class DW_API Logger : public Object {
public:
    DW_OBJECT(Logger);

    Logger(Context* context);
    Logger(Logger& other) = delete;
    ~Logger() = default;

    Logger& operator=(const Logger& other) = delete;

    void addLogMessageHandler(UniquePtr<LogMessageHandler>&& handler);

    template <typename... Args> String formatMessage(const String& format, const Args&... args);
    template <typename... Args> void log(LogLevel level, const String& format, const Args&... args);
    template <typename... Args> void debug(const String& format, const Args&... args);
    template <typename... Args> void info(const String& format, const Args&... args);
    template <typename... Args> void warn(const String& format, const Args&... args);
    template <typename... Args> void error(const String& format, const Args&... args);

private:
    void dispatchLogMessage(LogLevel level, const String& message);
    Vector<UniquePtr<LogMessageHandler>> handlers_;
};

template <typename... Args>
String Logger::formatMessage(const String& format, const Args&... args) {
    return tfm::format(format.c_str(), args...);
}

template <typename... Args>
void Logger::log(LogLevel level, const String& format, const Args&... args) {
    dispatchLogMessage(level, formatMessage(format, args...));
}

template <typename... Args> void Logger::debug(const String& format, const Args&... args) {
    log(LogLevel::Debug, format, args...);
}

template <typename... Args> void Logger::info(const String& format, const Args&... args) {
    log(LogLevel::Info, format, args...);
}

template <typename... Args> void Logger::warn(const String& format, const Args&... args) {
    log(LogLevel::Warning, format, args...);
}

template <typename... Args> void Logger::error(const String& format, const Args&... args) {
    log(LogLevel::Error, format, args...);
}
}

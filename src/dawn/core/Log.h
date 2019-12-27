/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "core/StringUtils.h"

namespace dw {
namespace detail {
void DisplayFatalError(String error_message);
}

enum class LogLevel { Debug, Info, Warning, Error };

class DW_API LogMessageHandler {
public:
    LogMessageHandler() = default;
    virtual ~LogMessageHandler() = default;

    virtual void onMessage(LogLevel level, const String& message) = 0;
};

class DW_API Logger : public Module {
public:
    DW_OBJECT(Logger)

    Logger(Context* context);
    Logger(Logger& other) = delete;
    ~Logger() = default;

    Logger& operator=(const Logger& other) = delete;

    void addLogMessageHandler(UniquePtr<LogMessageHandler> handler);

    void log(LogLevel level, const String& format) {
        dispatchLogMessage(level, format);
    }

    template <typename... Args> void debug(const String& format, const Args&... args);
    template <typename... Args> void info(const String& format, const Args&... args);
    template <typename... Args> void warn(const String& format, const Args&... args);
    template <typename... Args> void error(const String& format, const Args&... args);

    Logger& withObjectName(const String& name);

private:
    void dispatchLogMessage(LogLevel level, const String& message);
    Vector<UniquePtr<LogMessageHandler>> handlers_;
    String object_name_;
};

template <typename... Args> void Logger::debug(const String& format, const Args&... args) {
#ifdef DW_DEBUG
    log(LogLevel::Debug, str::format(format, args...));
#endif
}

template <typename... Args> void Logger::info(const String& format, const Args&... args) {
    log(LogLevel::Info, str::format(format, args...));
}

template <typename... Args> void Logger::warn(const String& format, const Args&... args) {
    log(LogLevel::Warning, str::format(format, args...));
}

template <typename... Args> void Logger::error(const String& format, const Args&... args) {
    log(LogLevel::Error, str::format(format, args...));
}
}  // namespace dw

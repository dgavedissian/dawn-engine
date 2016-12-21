/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include <tinyformat.h>

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
    ~Logger() = default;

    void addLogMessageHandler(UniquePtr<LogMessageHandler>&& handler);

    template <typename... T> String formatMessage(const String& format, const T&... args) {
        return tfm::format(format.c_str(), args...);
    }

    template <typename... T> void log(LogLevel level, const String& format, const T&... args) {
        dispatchLogMessage(level, formatMessage(format, args...));
    }

    template <typename... T> void debug(const String& format, const T&... args) {
        log(LogLevel::Debug, format, args...);
    }

    template <typename... T> void info(const String& format, const T&... args) {
        log(LogLevel::Info, format, args...);
    }

    template <typename... T> void warn(const String& format, const T&... args) {
        log(LogLevel::Warning, format, args...);
    }

    template <typename... T> void error(const String& format, const T&... args) {
        log(LogLevel::Error, format, args...);
    }

private:
    void dispatchLogMessage(LogLevel level, const String& message);
    Vector<UniquePtr<LogMessageHandler>> _handlers;
};
}
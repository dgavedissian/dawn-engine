/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "core/Singleton.h"

namespace dw {

enum LogLevel
{
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

class DW_API LogListener
{
public:
    LogListener();
    virtual ~LogListener();

    // TODO: Override this
    virtual void logWrite(const String& message) = 0;
};

// Outputs to the platform specific log
class DW_API PlatformLog : public LogListener
{
public:
    virtual void logWrite(const String& message) override;
};

class DW_API Log : public Singleton<Log>
{
private:
    class DW_API Stream
    {
    public:
        Stream(Log* log, LogLevel level, const String& message);
        ~Stream();

        template <class T>
        inline Stream& operator<<(T val)
        {
            std::ostringstream writer;
            writer << val;
            mMessage += writer.str();
            return *this;
        }

    private:
        Log* mLogger;
        LogLevel mLevel;
        String mMessage;
    };

public:
    Log(const String& filename);
    ~Log();

    // Writes a new line to the output stream
    void write(const String& message, LogLevel level);

    // Returns a new output stream object which writes a line when it gets out of scope
    Stream getStream(LogLevel level);

    // Add a Listener
    void addListener(LogListener* Listener);

    // Remove a Listener
    void removeListener(LogListener* Listener);

    // Get the current log buffer
    const Vector<String>& getBuffer() const;

private:
    std::ofstream mLogFile;
    Vector<String> mLogBuffer;

    Vector<LogListener*> mListeners;

    PlatformLog mPlatformLog;
};

template <> inline Log::Stream& Log::Stream::operator<<<String>(String val)
{
    mMessage += val;
    return *this;
}

}

// Macros
#define LOG Log::inst().getStream(LOG_INFO)
#define LOGWARN Log::inst().getStream(LOG_WARN)
#define LOGERR Log::inst().getStream(LOG_ERROR)


/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

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
    virtual void LogWrite(const String& message) = 0;
};

// Outputs to the platform specific log
class PlatformLog : public LogListener
{
public:
    virtual void LogWrite(const String& message) override;
};

class DW_API Log : public Singleton<Log>
{
private:
    class Stream
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
    void Write(const String& message, LogLevel level);

    // Returns a new output stream object which writes a line when it gets out of scope
    Stream GetStream(LogLevel level);

    // Add a Listener
    void AddListener(LogListener* Listener);

    // Remove a Listener
    void RemoveListener(LogListener* Listener);

    // Get the current log buffer
    const Vector<String>& GetLogBuffer() const;

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

NAMESPACE_END

// Macros
#define LOG Log::inst().GetStream(LOG_INFO)
#define LOGWARN Log::inst().GetStream(LOG_WARN)
#define LOGERR Log::inst().GetStream(LOG_ERROR)


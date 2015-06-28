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
    virtual void LogWrite(const string& message) = 0;
};

// Outputs to the platform specific log
class PlatformLog : public LogListener
{
public:
    virtual void LogWrite(const string& message) override;
};

class DW_API Log : public Singleton<Log>
{
private:
    class Stream
    {
    public:
        Stream(Log* log, LogLevel level, const string& message);
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
        string mMessage;
    };

public:
    Log(const string& filename);
    ~Log();

    // Writes a new line to the output stream
    void Write(const string& message, LogLevel level);

    // Returns a new output stream object which writes a line when it gets out of scope
    Stream GetStream(LogLevel level);

    // Add a listener
    void AddListener(LogListener* listener);

    // Remove a listener
    void RemoveListener(LogListener* listener);

    // Get the current log buffer
    const vector<string>& GetLogBuffer() const;

private:
    std::ofstream mLogFile;
    vector<string> mLogBuffer;

    vector<LogListener*> mListeners;

    PlatformLog mPlatformLog;
};

template <> inline Log::Stream& Log::Stream::operator<<<string>(string val)
{
    mMessage += val;
    return *this;
}

NAMESPACE_END

// Macros
#define LOG Log::inst().GetStream(LOG_INFO)
#define LOGWARN Log::inst().GetStream(LOG_WARN)
#define LOGERR Log::inst().GetStream(LOG_ERROR)


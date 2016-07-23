/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "core/Utils.h"

NAMESPACE_BEGIN

LogListener::LogListener()
{
    Log::inst().addListener(this);
}

LogListener::~LogListener()
{
    Log::inst().removeListener(this);
}

void PlatformLog::logWrite(const String& message)
{
    // Output to stdout
    std::cout << message << std::endl;

    // Output to VS debug screen
#if DW_PLATFORM == DW_WIN32 && defined(DW_DEBUG)
    String debugLine = message + "\n";
    OutputDebugStringA(debugLine.c_str());
#endif
}

Log::Stream::Stream(Log* log, LogLevel level, const String& message)
    : mLogger(log),
      mLevel(level),
      mMessage(message)
{
}

Log::Stream::~Stream()
{
    mLogger->write(mMessage, mLevel);
}

Log::Log(const String& filename) : mLogFile(filename)
{
    mLogFile << "Dawn Engine " << DW_VERSION_STR << std::endl;
    mLogFile << "-------------------------------------" << std::endl;
}

Log::~Log()
{
    mLogFile.close();
}

void Log::write(const String& message, LogLevel level)
{
    // Get the time of day
    time_t t = ::time(nullptr);
    tm* now = localtime(&t);
    std::stringstream ss;
    ss << "[" << (now->tm_hour < 10 ? "0" : "") << now->tm_hour << ":"
       << (now->tm_min < 10 ? "0" : "") << now->tm_min << ":" << (now->tm_sec < 10 ? "0" : "")
       << now->tm_sec << "]";
    String timeStr = ss.str();

    Vector<String> lines;
    splitString(message, '\n', lines);

    // TODO: threading - add lock here
    for (uint i = 0; i < lines.size(); ++i)
    {
        String levelStr = "";
        switch (level)
        {
        case LOG_WARN:
            levelStr = "[warning] ";
            break;

        case LOG_ERROR:
            levelStr = "[error] ";
            break;

        default:
            break;
        }

        String line = timeStr + " " + levelStr + " " + lines[i];

        // Convert tab characters into spaces
        const int tabSize = 4;
        for (uint i = 0; i < line.size(); ++i)
        {
            int noSpaces = tabSize - (i % tabSize);
            if (line[i] == '\t')
                line.replace(i, 1, String(noSpaces, ' '));
        }

        // Output to file
        mLogFile << line << std::endl;

        // Be sure that the log file is up to date in case of a crash
        mLogFile.flush();

        // Output to Listeners
        for (auto i = mListeners.begin(); i != mListeners.end(); ++i)
            (*i)->logWrite(line);

        // Add to the log buffer
        mLogBuffer.push_back(line);
    }
}

Log::Stream Log::getStream(LogLevel level)
{
    return Stream(this, level, "");
}

void Log::addListener(LogListener* Listener)
{
    mListeners.push_back(Listener);
}

void Log::removeListener(LogListener* Listener)
{
    mListeners.erase(std::find(mListeners.begin(), mListeners.end(), Listener));
}

const Vector<String>& Log::getBuffer() const
{
    return mLogBuffer;
}

NAMESPACE_END

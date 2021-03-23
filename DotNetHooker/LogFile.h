#ifndef _LOG_FILE_H_
#define _LOG_FILE_H_

#include <Windows.h>
#include <string>
#include <mutex>

class LogFile
{
public:
    LogFile() : LogFile(512) {}

    LogFile(LONG BufferSize) : bufferSize{BufferSize} {}

    HRESULT StartLog(const std::wstring& Path);

    // only one thread should access this object when FlushLog is called.
    void FlushLog();

    // returns the current line number, where Line was logged
    LONG Log(const std::wstring& Line);

    // returns the current line number, where Line was logged
    LONG LogError(const std::wstring& FailedFunction, HRESULT ErrorStatus);

    // copy constructor not used
    LogFile(const LogFile& other) = delete;

    // copy operator not used
    LogFile& operator=(const LogFile& other) = delete;

    // move constructor not used
    LogFile(LogFile&& other) = delete;

    // move assignment not used
    LogFile& operator=(LogFile&& other) = delete;

    ~LogFile();

    static HRESULT DumpDataToFile(const std::wstring& FilePath, PVOID Data, SIZE_T DataSize);

private:
    LONG LogInternal(const std::wstring& Line);

    mutable std::mutex logMutex;
    volatile LONG lineNumber = 0;
    std::wstringstream stringStream;
    HANDLE logHandle = INVALID_HANDLE_VALUE;
    LONG bufferSize;
};

#endif // !_LOG_FILE_H_


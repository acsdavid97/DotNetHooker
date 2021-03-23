#include "pch.h"
#include "LogFile.h"

HRESULT LogFile::StartLog(const std::wstring& Path)
{
    logHandle = CreateFileW(Path.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_DELETE | FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (logHandle == INVALID_HANDLE_VALUE)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}

void LogFile::FlushLog()
{
    std::wstring str = stringStream.str();
    stringStream.str(std::wstring());
    stringStream.clear();
    DWORD bytesToWrite = (DWORD)str.length() * sizeof(WCHAR);
    DWORD bytesWritten = 0;
    WriteFile(logHandle, str.c_str(), bytesToWrite, &bytesWritten, nullptr);
}

LONG LogFile::Log(const std::wstring& Line)
{
    return LogInternal(Line);
}

LONG LogFile::LogInternal(const std::wstring& Line)
{
    std::unique_lock<std::mutex> lck(logMutex);
    stringStream << Line << L'\n';
    LONG newLineNumber =  InterlockedIncrement(&lineNumber);
    if (newLineNumber >= bufferSize)
    {
        FlushLog();
    }

    return newLineNumber;
}

LONG LogFile::LogError(const std::wstring& FailedFunction, HRESULT ErrorStatus)
{
    std::wstringstream errorLine;
    errorLine << L"[ERROR] " << FailedFunction << L" failed with" << std::hex << L" 0x" << ErrorStatus;
    return LogInternal(errorLine.str());
}

LogFile::~LogFile()
{
    if (logHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(logHandle);
    }
}

HRESULT
LogFile::DumpDataToFile(const std::wstring& FilePath, PVOID Data, SIZE_T DataSize)
{
    HANDLE fileHandle = CreateFileW(FilePath.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_DELETE | FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    DWORD bytesWritten = 0;
    if (!WriteFile(fileHandle, Data, (DWORD)DataSize, &bytesWritten, nullptr))
    {
        CloseHandle(fileHandle);
        return HRESULT_FROM_WIN32(GetLastError());
    }

    CloseHandle(fileHandle);

    return S_OK;
}
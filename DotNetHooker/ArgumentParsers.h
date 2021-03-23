#ifndef _ARGUMENT_PARSERS_H_
#define _ARGUMENT_PARSERS_H_

#include <cor.h>
#include <memory>

// defines intresting argument types for our profiler
enum class ArgumentType
{
    Unknown = 0,
    String = 1,                 // System.String
    ByteArray = 2,              // byte[]
    Class = 3,                  // any Class, except Object, argument extractor can use ICorProfilerInfo::GetClassFromObject to get classId

    DummyPtr = 4,
    DummyByte = 5,
    DummyWord = 6,
    DummyDword = 7,
    DummyQword = 8,

    Max = DummyQword,                // entries above this are invalid
};

typedef struct _PARSED_ARGUMENT
{
    PVOID DataStart;
    SIZE_T DataSize;
}PARSED_ARGUMENT;

class IArgumentParser
{
public:
    // each parser is responsible to advance the ArgumentDataStart pointer with as many bytes as consumed.
    virtual HRESULT ParseArgument(
        _In_ PBYTE& ArgumentDataStart,
        _In_ PBYTE ArgumentDataEnd,
        _Out_ PARSED_ARGUMENT& ParsedArgument
    ) = 0;

    static std::shared_ptr<IArgumentParser> Make(
        _In_ ICorProfilerInfo2* ProfilerInfo,
        _In_ ArgumentType ArgType
    );
};


class StringArgumentParser : public IArgumentParser
{
public:
    StringArgumentParser(_In_ ICorProfilerInfo2* profilerInfo);

    HRESULT ParseArgument(
        _In_ PBYTE& ArgumentDataStart,
        _In_ PBYTE ArgumentDataEnd,
        _Out_ PARSED_ARGUMENT& ParsedArgument
    ) override;
private:
    ULONG strBufSizeOffset = 0;
    ULONG strLengthOffset = 0;
    ULONG strBufferOffset = 0;
};

class ByteArrayArgumentParser : public IArgumentParser
{
public:
    ByteArrayArgumentParser(_In_ ICorProfilerInfo2* ProfilerInfo);
    ~ByteArrayArgumentParser();

    HRESULT ParseArgument(
        _In_ PBYTE& ArgumentDataStart,
        _In_ PBYTE ArgumentDataEnd,
        _Out_ PARSED_ARGUMENT& ParsedArgument
    ) override;
private:
    ICorProfilerInfo2* profilerInfo;
};

class PointerSizeDummyParser : public IArgumentParser
{
public:
    PointerSizeDummyParser(_In_ ICorProfilerInfo2*) {}

    HRESULT ParseArgument(
        _In_ PBYTE& ArgumentDataStart,
        _In_ PBYTE ArgumentDataEnd,
        _Out_ PARSED_ARGUMENT&
    ) override
    {
        if (ArgumentDataStart + sizeof(PVOID) > ArgumentDataEnd)
        {
            return E_NOT_SUFFICIENT_BUFFER;
        }

        ArgumentDataStart += sizeof(PVOID);
        return E_NOTIMPL;
    }
};

class ByteSizeDummyParser : public IArgumentParser
{
public:
    ByteSizeDummyParser(_In_ ICorProfilerInfo2*) {}

    HRESULT ParseArgument(
        _In_ PBYTE& ArgumentDataStart,
        _In_ PBYTE ArgumentDataEnd,
        _Out_ PARSED_ARGUMENT&
    ) override
    {
        if (ArgumentDataStart + sizeof(BYTE) > ArgumentDataEnd)
        {
            return E_NOT_SUFFICIENT_BUFFER;
        }

        ArgumentDataStart += sizeof(BYTE);
        return E_NOTIMPL;
    }
};

class WordSizeDummyParser : public IArgumentParser
{
public:
    WordSizeDummyParser(_In_ ICorProfilerInfo2*) {}

    HRESULT ParseArgument(
        _In_ PBYTE& ArgumentDataStart,
        _In_ PBYTE ArgumentDataEnd,
        _Out_ PARSED_ARGUMENT&
    ) override
    {
        if (ArgumentDataStart + sizeof(WORD) > ArgumentDataEnd)
        {
            return E_NOT_SUFFICIENT_BUFFER;
        }

        ArgumentDataStart += sizeof(WORD);
        return E_NOTIMPL;
    }
};

class DwordSizeDummyParser : public IArgumentParser
{
public:
    DwordSizeDummyParser(_In_ ICorProfilerInfo2*) {}

    HRESULT ParseArgument(
        _In_ PBYTE& ArgumentDataStart,
        _In_ PBYTE ArgumentDataEnd,
        _Out_ PARSED_ARGUMENT&
    ) override
    {
        if (ArgumentDataStart + sizeof(DWORD) > ArgumentDataEnd)
        {
            return E_NOT_SUFFICIENT_BUFFER;
        }

        ArgumentDataStart += sizeof(DWORD);
        return E_NOTIMPL;
    }
};

class QwordSizeDummyParser : public IArgumentParser
{
public:
    QwordSizeDummyParser(_In_ ICorProfilerInfo2*) {}

    HRESULT ParseArgument(
        _In_ PBYTE& ArgumentDataStart,
        _In_ PBYTE ArgumentDataEnd,
        _Out_ PARSED_ARGUMENT&
    ) override
    {
        if (ArgumentDataStart + sizeof(LARGE_INTEGER) > ArgumentDataEnd)
        {
            return E_NOT_SUFFICIENT_BUFFER;
        }

        ArgumentDataStart += sizeof(LARGE_INTEGER);
        return E_NOTIMPL;
    }
};

#endif // !_ARGUMENT_PARSERS_H_
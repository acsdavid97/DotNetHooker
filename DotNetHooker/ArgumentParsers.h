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
    Max = Class,                // entries above this are invalid
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

#endif // !_ARGUMENT_PARSERS_H_
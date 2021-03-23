#ifndef _FUNCTION_INFO_H_
#define _FUNCTION_INFO_H_

#include <vector>
#include <string>
#include <cor.h>

#include "ArgumentParsers.h"

class FunctionInfo
{
public:
    FunctionInfo(_In_ const std::wstring& functionName) :
        functionName(functionName)
    {
    }

    const std::wstring& GetFunctionName()
    {
        return functionName;
    }

    const std::vector<std::shared_ptr<IArgumentParser>>& GetArgumentParsers()
    {
        return argumentParsers;
    }

    HRESULT ParseFunctionSignature(
        _In_ PCCOR_SIGNATURE Signature,
        _In_ ULONG SignatureSize,
        _In_ ICorProfilerInfo2* ProfilerInfo,
        _In_ IMetaDataImport* MetaData
    );

private:
    HRESULT ParseType(
        _In_ PCCOR_SIGNATURE& Signature,
        _In_ SIZE_T SignatureSize,
        _In_ IMetaDataImport* MetaData,
        _Out_ ArgumentType& ArgType
    );

    void AddArgumentType(
        _In_ ArgumentType ArgumentType,
        _In_ ICorProfilerInfo2* ProfilerInfo
    )
    {
        // TODO: do not allocate a new parser for each argument, instead have global (singleton) parsers for each type.
        auto parser = IArgumentParser::Make(ProfilerInfo, ArgumentType);
        argumentParsers.push_back(parser);
    }

    std::wstring functionName;
    std::vector<std::shared_ptr<IArgumentParser>> argumentParsers;
};


#endif // !_FUNCTION_INFO_H_


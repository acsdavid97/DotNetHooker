#include "pch.h"
#include "ArgumentParsers.h"
#include <exception>

std::shared_ptr<IArgumentParser> IArgumentParser::Make(
	_In_ ICorProfilerInfo2* ProfilerInfo,
	_In_ ArgumentType ArgType
)
{
	switch (ArgType)
	{
	case ArgumentType::String:
		return std::make_shared<StringArgumentParser>(ProfilerInfo);
	case ArgumentType::ByteArray:
	case ArgumentType::Class:
	default:
		throw std::exception("Invalid ArgumentType");
	}
}

StringArgumentParser::StringArgumentParser(_In_ ICorProfilerInfo2* profilerInfo)
{
	HRESULT hres = profilerInfo->GetStringLayout(&strBufSizeOffset, &strLengthOffset, &strBufferOffset);
	if (FAILED(hres))
	{
		throw std::exception("GetStringLayout failed");
	}
}

HRESULT StringArgumentParser::ParseArgument(
	_In_ PBYTE& ArgumentDataStart,
	_In_ PBYTE ArgumentDataEnd,
	_Out_ PARSED_ARGUMENT& ParsedArgument
)
{
	if (ArgumentDataStart >= ArgumentDataEnd)
	{
		return E_NOT_SUFFICIENT_BUFFER;
	}

	PBYTE stringObject = (PBYTE) *(PVOID*)ArgumentDataStart;
	ArgumentDataStart += sizeof(PVOID);

	ULONG32 stringLength = *(PULONG32)(stringObject + strLengthOffset);
	ParsedArgument.DataSize = stringLength * sizeof(WCHAR);
	ParsedArgument.DataStart = stringObject + strBufferOffset;
	return S_OK;
}
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
		return std::make_shared<ByteArrayArgumentParser>(ProfilerInfo);
	case ArgumentType::Class:
	case ArgumentType::DummyPtr:
		return std::make_shared<PointerSizeDummyParser>(ProfilerInfo);
	case ArgumentType::DummyByte:
		return std::make_shared<ByteSizeDummyParser>(ProfilerInfo);
	case ArgumentType::DummyWord:
		return std::make_shared<WordSizeDummyParser>(ProfilerInfo);
	case ArgumentType::DummyDword:
		return std::make_shared<DwordSizeDummyParser>(ProfilerInfo);
	case ArgumentType::DummyQword:
		return std::make_shared<QwordSizeDummyParser>(ProfilerInfo);
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
	if (ArgumentDataStart + sizeof(PVOID) > ArgumentDataEnd)
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

ByteArrayArgumentParser::ByteArrayArgumentParser(_In_ ICorProfilerInfo2* ProfilerInfo)
{
	ProfilerInfo->AddRef();
	profilerInfo = ProfilerInfo;
}

ByteArrayArgumentParser::~ByteArrayArgumentParser()
{
	profilerInfo->Release();
}

HRESULT ByteArrayArgumentParser::ParseArgument(
	_In_ PBYTE& ArgumentDataStart,
	_In_ PBYTE ArgumentDataEnd,
	_Out_ PARSED_ARGUMENT& ParsedArgument
)
{
	if (ArgumentDataStart + sizeof(PVOID) > ArgumentDataEnd)
	{
		return E_NOT_SUFFICIENT_BUFFER;
	}

	PBYTE arrayObject = (PBYTE) * (PVOID*)ArgumentDataStart;
	ArgumentDataStart += sizeof(PVOID);

	// expect a single dimension for byte[]
	ULONG32 dimension = 0;
	int lowerbound = 0;
	PBYTE data = nullptr;
	HRESULT hres = profilerInfo->GetArrayObjectInfo((ObjectID)arrayObject, 1, &dimension, &lowerbound, &data);
	if (FAILED(hres))
	{
		return hres;
	}

	ParsedArgument.DataSize = dimension;
	ParsedArgument.DataStart = data;

	return S_OK;
}
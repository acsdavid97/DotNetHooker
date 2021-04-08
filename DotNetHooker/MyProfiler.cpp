// MyProfiler.cpp : Implementation of CMyProfiler

#include "pch.h"
#include "MyProfiler.h"
#include "trampoline.h"
#include <string>
#include <windows.h>
#include <string.h>

const WCHAR* DNH_ARGUMENT_FILTER_ENV_VAR = L"DNH_ARGUMENT_FILTER";
const WCHAR* DNH_FUNC_FILTER_INCLUDE_ENV_VAR = L"DNH_FUNC_FILTER_INCLUDE";
const WCHAR* DNH_FUNC_FILTER_EXCLUDE_ENV_VAR = L"DNH_FUNC_FILTER_EXCLUDE";

CMyProfiler* gMyProfiler = nullptr;


thread_local static DWORD gCurrentThreadFunctionCallDepth = 0;
thread_local static bool gCurrentThreadTailCall = false;

extern "C" void __stdcall FunctionEnterHook(
    _In_ FunctionID funcId,
    _In_ UINT_PTR clientData,
    _In_ COR_PRF_FRAME_INFO func,
    _In_ COR_PRF_FUNCTION_ARGUMENT_INFO * argumentInfo
)
{
    gMyProfiler->OnEnter(funcId, clientData, func, argumentInfo);
}

void CMyProfiler::OnEnter(
    _In_ FunctionID funcId,
    _In_ UINT_PTR clientData,
    _In_ COR_PRF_FRAME_INFO func,
    _In_ COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo
)
{
    UNREFERENCED_PARAMETER(func);
    UNREFERENCED_PARAMETER(funcId);

    if (!clientData)
    {
        // function marked as not monitored by OnFunctionMap yet we still received a callback
        // should never happen.
        __debugbreak();
        return;
    }

    ThreadID threadId = 0;
    HRESULT hres = profilerInfo->GetCurrentThreadID(&threadId);
    if (FAILED(hres))
    {
        // threads should be managed that execute functions. should never return CORPROF_E_NOT_MANAGED_THREAD
        __debugbreak();
        return;
    }

    std::wstringstream lineToWrite;
    lineToWrite << L"TID: 0x" << std::hex << (DWORD)threadId << L" ";
    for (DWORD i = 0; i < gCurrentThreadFunctionCallDepth; i++)
    {
        lineToWrite << L" ";
    }

    std::shared_ptr<FunctionInfo> funcInfo = GetFunctionById(clientData);

    lineToWrite << funcInfo->GetFunctionName();
    if (gCurrentThreadTailCall)
    {
        lineToWrite << L" (Tail called from prev)";
        gCurrentThreadTailCall = false;
    }
    else
    {
        // tail call does not increase call depth, since there is no leave for it
        gCurrentThreadFunctionCallDepth++;
    }

    std::wstring finalLine = lineToWrite.str();

    LONG currentLineNumber = logFile.Log(finalLine);

    if (funcInfo->GetShouldDumpArgs())
    {
        hres = DumpFunctionArguments(funcInfo, argumentInfo, currentLineNumber);
        if (FAILED(hres))
        {
            logFile.LogError(L"DumpFunctionArguments", hres);
        }
    }
}

extern "C" void __stdcall FunctionLeaveHook(
    _In_ FunctionID funcId,
    _In_ UINT_PTR clientData,
    _In_ COR_PRF_FRAME_INFO func,
    _In_ COR_PRF_FUNCTION_ARGUMENT_RANGE * retvalRange
)
{
    gMyProfiler->OnLeave(funcId, clientData, func, retvalRange);
}

void CMyProfiler::OnLeave(
    _In_ FunctionID funcId,
    _In_ UINT_PTR clientData,
    _In_ COR_PRF_FRAME_INFO func,
    _In_ COR_PRF_FUNCTION_ARGUMENT_RANGE* retvalRange
)
{
    UNREFERENCED_PARAMETER(funcId);
    UNREFERENCED_PARAMETER(clientData);
    UNREFERENCED_PARAMETER(func);
    UNREFERENCED_PARAMETER(retvalRange);

    gCurrentThreadFunctionCallDepth--;
}

extern "C" void __stdcall FunctionTailcallHook(
    _In_ FunctionID funcId,
    _In_ UINT_PTR clientData,
    _In_ COR_PRF_FRAME_INFO func
)
{
    gMyProfiler->OnTailcall(funcId, clientData, func);
}

void CMyProfiler::OnTailcall(
    _In_ FunctionID funcId,
    _In_ UINT_PTR clientData,
    _In_ COR_PRF_FRAME_INFO func
)
{
    UNREFERENCED_PARAMETER(funcId);
    UNREFERENCED_PARAMETER(clientData);
    UNREFERENCED_PARAMETER(func);

    gCurrentThreadTailCall = true;
}

UINT_PTR __stdcall FunctionIDMapperCallback(
    _In_ FunctionID funcId,
    _Out_ BOOL* pbHookFunction
)
{
    return gMyProfiler->OnFunctionMap(funcId, pbHookFunction);
}

UINT_PTR CMyProfiler::OnFunctionMap(
    _In_ FunctionID funcId,
    _Out_ BOOL* pbHookFunction
)
{
    // in case of error we ignore the function
    *pbHookFunction = false;

    ClassID classId = 0;
    ModuleID moduleId = 0;
    mdToken metadataToken = 0;
    HRESULT hres = profilerInfo->GetFunctionInfo(funcId, &classId, &moduleId, &metadataToken);
    if (FAILED(hres))
    {
        return 0;
    }

    IMetaDataImport* metaDataImport = nullptr;
    hres = profilerInfo->GetModuleMetaData(moduleId, ofRead, IID_IMetaDataImport, reinterpret_cast<IUnknown**>(&metaDataImport));
    if (FAILED(hres))
    {
        return 0;
    }

    constexpr ULONG32 METHOD_NAME_MAX_SIZE = 512;

    mdTypeDef classType = 0;
    WCHAR methodNameBuf[METHOD_NAME_MAX_SIZE];
    ULONG realMethodNameSize = 0;
    PCCOR_SIGNATURE methodSignature = nullptr;
    ULONG methodSignatureSize = 0;
    hres = metaDataImport->GetMethodProps(metadataToken, &classType, methodNameBuf, METHOD_NAME_MAX_SIZE, &realMethodNameSize, nullptr, &methodSignature, &methodSignatureSize, nullptr, nullptr);
    if (FAILED(hres))
    {
        metaDataImport->Release();
        return 0;
    }

    if (METHOD_NAME_MAX_SIZE < realMethodNameSize)
    {
        // array too small should realloc
        metaDataImport->Release();
        return 0;
    }

    std::wstring fullmethodName = GetClassNameById(classId);
    fullmethodName += L".";
    fullmethodName += methodNameBuf;

    if (!IsFunctionMonitored(fullmethodName))
    {
        // user does not want monitoring of this function
        return 0;
    }

    bool shouldDumpArgs = ShouldDumpArgsForFunction(fullmethodName);
    auto funcInfo = std::make_shared<FunctionInfo>(fullmethodName, shouldDumpArgs);

    if (shouldDumpArgs)
    {
        hres = funcInfo->ParseFunctionSignature(methodSignature, methodSignatureSize, profilerInfo, metaDataImport);
        metaDataImport->Release();
        if (FAILED(hres))
        {
            return 0;
        }
    }
    else
    {
        metaDataImport->Release();
    }

    // all good, we will monitor the function
    *pbHookFunction = true;

    // BUG: for same funcionID different mapping is returned, in case OnFunctionMap is called from different threads.
    // Profilers should be tolerant of cases where multiple threads of a profiled application are calling the same method / function simultaneously.
    // In such cases, the profiler may receive multiple FunctionIDMapper callbacks for the same FunctionID.The profiler should be certain to return the same values from this callback when it is called multiple times with the same FunctionID.
    // https://docs.microsoft.com/en-us/previous-versions/dotnet/netframework-4.0/aa964957(v=vs.100)

    return AddFunctionName(funcInfo);
}

HRESULT CMyProfiler::DumpFunctionArguments(
    _In_ const std::shared_ptr<FunctionInfo> FunctionInfo,
    _In_ COR_PRF_FUNCTION_ARGUMENT_INFO* ArgumentInfo,
    _In_ LONG CurrentLineNumber
)
{
    ULONG currentRangeIndex = 0;
    PBYTE currentArgumentPointer = nullptr;
    PBYTE currentRangeEnd = nullptr;
    DWORD argumentIndex = 0;
    for (const auto& parser : FunctionInfo->GetArgumentParsers())
    {
        argumentIndex++;
        if (currentArgumentPointer >= currentRangeEnd)
        {
            // first iteration or we are at the end of a range.
            if (currentRangeIndex >= ArgumentInfo->numRanges)
            {
                logFile.LogError(L"<outside of argument range>", 0x0);
                return E_INVALIDARG;
            }

            currentArgumentPointer = (PBYTE)ArgumentInfo->ranges[currentRangeIndex].startAddress;
            currentRangeEnd = currentArgumentPointer + ArgumentInfo->ranges[currentRangeIndex].length;
            currentRangeIndex++;
        }


        PARSED_ARGUMENT parsedArgument = { 0 };
        HRESULT hres = parser->ParseArgument(currentArgumentPointer, currentRangeEnd, parsedArgument);

        if (hres == E_NOTIMPL)
        {
            // dummy parsers return this, not need to log error.
            continue;
        }

        if (FAILED(hres))
        {
            logFile.LogError(L"ParseArgument", hres);
            continue;
        }

        std::wstringstream fileName;
        fileName << L"PID_" << pid << L"_Line_" << CurrentLineNumber << L"_Argc_" << argumentIndex << L".dnh";
        std::wstring fileNameStr = fileName.str();
        hres = LogFile::DumpDataToFile(fileNameStr.c_str(), parsedArgument.DataStart, parsedArgument.DataSize);
        if (FAILED(hres))
        {
            logFile.LogError(L"DumpDataToFile", hres);
            continue;
        }
    }

    return S_OK;
}

UINT_PTR CMyProfiler::AddFunctionName(
    _In_ const std::shared_ptr<FunctionInfo>& FunctionInfo
)
{
    std::unique_lock<std::shared_mutex> lock(functionMapLock);
    currentFunctionId++;
    functionIdToInfo[currentFunctionId] = FunctionInfo;

    return currentFunctionId;
}

std::shared_ptr<FunctionInfo> CMyProfiler::GetFunctionById(
    _In_ UINT_PTR FunctionId
)
{
    std::shared_lock<std::shared_mutex> lck(functionMapLock);
    auto result = functionIdToInfo.find(FunctionId);
    if (result == functionIdToInfo.end())
    {
        // function name not resolved, yet marked as monitored by OnFunctionMap
        // should never happen.
        __debugbreak();
        throw std::runtime_error("invalid function ID");
    }

    return result->second;
}

void CMyProfiler::AddClassIdToNameMapping(
    _In_ ClassID ClassId,
    _In_ const std::wstring& ClassName
)
{
    std::unique_lock<std::shared_mutex> lck(classMapLock);
    classIdToName[ClassId] = ClassName;
}

std::wstring CMyProfiler::GetClassNameById(
    _In_ ClassID ClassId
)
{
    auto result = classIdToName.find(ClassId);
    if (result == classIdToName.end())
    {
        return std::wstring(L"<NOT_FOUND_CLASS>");
    }

    return result->second;
}

HRESULT CMyProfiler::ResolveClassName(
    _In_ ClassID ClassId,
    _Out_ std::wstring& ClassName
)
{
    ModuleID moduleId = 0;
    mdTypeDef classTypeDef = 0;
    HRESULT hres = profilerInfo->GetClassIDInfo(ClassId, &moduleId, &classTypeDef);
    if (FAILED(hres))
    {
        return hres;
    }

    IMetaDataImport* metaDataImport = nullptr;
    hres = profilerInfo->GetModuleMetaData(moduleId, ofRead, IID_IMetaDataImport, reinterpret_cast<IUnknown**>(&metaDataImport));
    if (FAILED(hres))
    {
        return hres;
    }

    constexpr ULONG32 CLASS_NAME_MAX_SIZE = 512;

    WCHAR classNameBuf[CLASS_NAME_MAX_SIZE];
    ULONG realClassNameSize = 0;
    hres = metaDataImport->GetTypeDefProps(classTypeDef, classNameBuf, CLASS_NAME_MAX_SIZE, &realClassNameSize, nullptr, nullptr);
    metaDataImport->Release();
    if (FAILED(hres))
    {
        return hres;
    }

    if (CLASS_NAME_MAX_SIZE < realClassNameSize)
    {
        // array too small should realloc
        return STATUS_BUFFER_OVERFLOW;
    }

    ClassName = classNameBuf;
    return S_OK;
}


HRESULT STDMETHODCALLTYPE CMyProfiler::Initialize(
    /* [in] */ IUnknown* pICorProfilerInfoUnk)
{
    gMyProfiler = this;

    std::wstringstream logNameStream;
    pid = GetCurrentProcessId();
    logNameStream << L"DotNetHooker_" << pid << L".log";

    HRESULT hres = logFile.StartLog(logNameStream.str());
    if (FAILED(hres))
    {
        return hres;
    }

    hres = pICorProfilerInfoUnk->QueryInterface(&profilerInfo);
    if (FAILED(hres))
    {
        return hres;
    }

    hres = profilerInfo->SetEventMask(COR_PRF_MONITOR_ENTERLEAVE | COR_PRF_ENABLE_FUNCTION_ARGS | COR_PRF_MONITOR_CLASS_LOADS);
    if (FAILED(hres))
    {
        profilerInfo->Release();
        return hres;
    }

    hres = profilerInfo->SetFunctionIDMapper(FunctionIDMapperCallback);
    if (FAILED(hres))
    {
        profilerInfo->Release();
        return hres;
    }

    hres = profilerInfo->SetEnterLeaveFunctionHooks2(FunctionEnterTrampoline, FunctionLeaveTrampoline, FunctionTailcallTrampoline);
    if (FAILED(hres))
    {
        profilerInfo->Release();
        return hres;
    }

    ParseFunctionNameFilters();
    ParseArgumentDumpingFilters();

    DeleteProfilerEnvirionmentVars();

    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::Shutdown(void)
{
    profilerInfo->Release();

    logFile.FlushLog();

    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ClassLoadFinished(
    /* [in] */ ClassID classId,
    /* [in] */ HRESULT hrStatus)
{
    if (FAILED(hrStatus))
    {
        return S_OK;
    }

    std::wstring className;
    HRESULT hres = ResolveClassName(classId, className);
    if (FAILED(hres))
    {
        return S_OK;
    }

    AddClassIdToNameMapping(classId, className);

    return S_OK;
};

void CMyProfiler::ParseFunctionNameFilters()
{
    TokenizeEnvVar(DNH_FUNC_FILTER_INCLUDE_ENV_VAR, functionIncludeFilters);
    TokenizeEnvVar(DNH_FUNC_FILTER_EXCLUDE_ENV_VAR, functionExcludeFilters);
}

bool CMyProfiler::IsFunctionMonitored(
    _In_ const std::wstring& FunctionName
)
{
    if (FindFunctionNameInArray(FunctionName, functionExcludeFilters))
    {
        // explicit deny by exclude filters.
        return false;
    }

    if (functionIncludeFilters.empty())
    {
        // empty include filters means monitor all
        return true;
    }

    // not empty include filters means monitor only the explicitly allowed.
    return FindFunctionNameInArray(FunctionName, functionIncludeFilters);
}

void CMyProfiler::ParseArgumentDumpingFilters()
{
    TokenizeEnvVar(DNH_ARGUMENT_FILTER_ENV_VAR, argDumpingFilters);
}

void CMyProfiler::TokenizeEnvVar(
    LPCWSTR EnvVar,
    std::vector<std::wstring>& Tokens
)
{
    WCHAR envVarBuf[1024];
    DWORD size = GetEnvironmentVariableW(EnvVar, envVarBuf, 1024);
    if (size == 0)
    {
        return;
    }

    WCHAR* separator = L";";
    WCHAR* context = nullptr;
    WCHAR* currentToken = wcstok_s(envVarBuf, separator, &context);
    while (currentToken)
    {
        Tokens.emplace_back(currentToken);
        currentToken = wcstok_s(nullptr, separator, &context);
    }
}

bool CMyProfiler::ShouldDumpArgsForFunction(
    _In_ const std::wstring& FunctionName
)
{
    return FindFunctionNameInArray(FunctionName, argDumpingFilters);
}

bool CMyProfiler::FindFunctionNameInArray(
    _In_ const std::wstring& FunctionName,
    const std::vector<std::wstring>& Array
)
{
    for (const std::wstring& monitoredFunction : Array)
    {
        if (FunctionName.find(monitoredFunction) != std::wstring::npos)
        {
            return true;
        }
    }

    return false;
}

void CMyProfiler::DeleteProfilerEnvirionmentVars()
{
    // delete profiler related env vars so malware has harder time finding if it runs under profiler
    SetEnvironmentVariableW(L"COR_ENABLE_PROFILING", nullptr);
    SetEnvironmentVariableW(L"COR_PROFILER", nullptr);
    SetEnvironmentVariableW(L"COMPlus_ProfAPI_ProfilerCompatibilitySetting", nullptr);
    SetEnvironmentVariableW(DNH_ARGUMENT_FILTER_ENV_VAR, nullptr);
    SetEnvironmentVariableW(DNH_FUNC_FILTER_INCLUDE_ENV_VAR, nullptr);
    SetEnvironmentVariableW(DNH_FUNC_FILTER_EXCLUDE_ENV_VAR, nullptr);
}

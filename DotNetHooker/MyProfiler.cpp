// MyProfiler.cpp : Implementation of CMyProfiler

#include "pch.h"
#include "MyProfiler.h"
#include "trampoline.h"
#include <string>
#include <windows.h>

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
    UNREFERENCED_PARAMETER(argumentInfo);
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
        // threads should be managed that execute functions should never return CORPROF_E_NOT_MANAGED_THREAD
        // really Microsoft?
        __debugbreak();
        return;
    }

    std::wstringstream lineToWrite;
    lineToWrite << L"TID: 0x" << std::hex << (DWORD)threadId << L" ";
    for (DWORD i = 0; i < gCurrentThreadFunctionCallDepth; i++)
    {
        lineToWrite << L" ";
    }

    const std::wstring functionName = GetFunctionById(clientData);

    lineToWrite << functionName;
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

    lineToWrite << L"\n";

    if (functionName.find(L"SomethingMethod") != std::wstring::npos)
    {
        ULONG strBufSizeOffset = 0;
        ULONG strLengthOffset = 0;
        ULONG strBufferOffset = 0;
        profilerInfo->GetStringLayout(&strBufSizeOffset, &strLengthOffset, &strBufferOffset);

        // TODO: read string from args 
        __debugbreak();
    }

    std::wstring finalLine = lineToWrite.str();

    DWORD sizeToWrite = (DWORD)finalLine.length() * sizeof(WCHAR);
    DWORD bytesWritten = 0;
    // TODO: improve perf by not writing to file on every function call,
    // instead write in batches, e.g. after 1000 function calls.
    WriteFile(logHandle, finalLine.c_str(), sizeToWrite, &bytesWritten, nullptr);
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
    hres = metaDataImport->GetMethodProps(metadataToken, &classType, methodNameBuf, METHOD_NAME_MAX_SIZE, &realMethodNameSize, nullptr, nullptr, nullptr, nullptr, nullptr);
    metaDataImport->Release();
    if (FAILED(hres))
    {
        return 0;
    }

    if (METHOD_NAME_MAX_SIZE < realMethodNameSize)
    {
        // array too small should realloc
        return 0;
    }

    std::wstring fullmethodName = GetClassNameById(classId);
    fullmethodName += L".";
    fullmethodName += methodNameBuf;

    // all good, we will monitor the function
    *pbHookFunction = true;

    return AddFunctionName(fullmethodName);
}

UINT_PTR CMyProfiler::AddFunctionName(
    _In_ const std::wstring& FunctionName
)
{
    std::unique_lock<std::shared_mutex> lock(functionMapLock);
    currentFunctionId++;
    functionIdToName[currentFunctionId] = FunctionName;

    return currentFunctionId;
}

std::wstring CMyProfiler::GetFunctionById(
    _In_ UINT_PTR FunctionId
)
{
    std::shared_lock<std::shared_mutex> lck(functionMapLock);
    auto result = functionIdToName.find(FunctionId);
    if (result == functionIdToName.end())
    {
        // function name not resolved, yet marked as monitored by OnFunctionMap
        // should never happen.
        __debugbreak();
        return std::wstring(L"<NOT_FOUND_FUNCTION>");
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
        // should never happen.
        __debugbreak();
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


// CMyProfiler

HRESULT STDMETHODCALLTYPE CMyProfiler::Initialize(
    /* [in] */ IUnknown* pICorProfilerInfoUnk)
{
    gMyProfiler = this;

    logHandle = CreateFileW(L"DotNetHooker.log",
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

    HRESULT hres = pICorProfilerInfoUnk->QueryInterface(&profilerInfo);
    if (FAILED(hres))
    {
        return hres;
    }

    hres = profilerInfo->SetEventMask(COR_PRF_MONITOR_ENTERLEAVE | COR_PRF_ENABLE_FUNCTION_ARGS | COR_PRF_ENABLE_FUNCTION_RETVAL | COR_PRF_MONITOR_CLASS_LOADS | COR_PRF_ENABLE_FRAME_INFO);
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

    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::Shutdown(void)
{
    profilerInfo->Release();

    if (logHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(logHandle);
    }

    return S_OK;
};

#pragma warning(push)
#pragma warning(disable:4100) // disable unreferenced formal parameter as there is a lot of it here :)

HRESULT STDMETHODCALLTYPE CMyProfiler::AppDomainCreationStarted(
    /* [in] */ AppDomainID appDomainId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::AppDomainCreationFinished(
    /* [in] */ AppDomainID appDomainId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::AppDomainShutdownStarted(
    /* [in] */ AppDomainID appDomainId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::AppDomainShutdownFinished(
    /* [in] */ AppDomainID appDomainId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::AssemblyLoadStarted(
    /* [in] */ AssemblyID assemblyId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::AssemblyLoadFinished(
    /* [in] */ AssemblyID assemblyId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::AssemblyUnloadStarted(
    /* [in] */ AssemblyID assemblyId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::AssemblyUnloadFinished(
    /* [in] */ AssemblyID assemblyId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ModuleLoadStarted(
    /* [in] */ ModuleID moduleId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ModuleLoadFinished(
    /* [in] */ ModuleID moduleId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ModuleUnloadStarted(
    /* [in] */ ModuleID moduleId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ModuleUnloadFinished(
    /* [in] */ ModuleID moduleId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ModuleAttachedToAssembly(
    /* [in] */ ModuleID moduleId,
    /* [in] */ AssemblyID AssemblyId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ClassLoadStarted(
    /* [in] */ ClassID classId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ClassLoadFinished(
    /* [in] */ ClassID classId,
    /* [in] */ HRESULT hrStatus)
{
    std::wstring className;
    HRESULT hres = ResolveClassName(classId, className);
    if (FAILED(hres))
    {
        return S_OK;
    }

    AddClassIdToNameMapping(classId, className);

    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ClassUnloadStarted(
    /* [in] */ ClassID classId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ClassUnloadFinished(
    /* [in] */ ClassID classId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::FunctionUnloadStarted(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::JITCompilationStarted(
    /* [in] */ FunctionID functionId,
    /* [in] */ BOOL fIsSafeToBlock)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::JITCompilationFinished(
    /* [in] */ FunctionID functionId,
    /* [in] */ HRESULT hrStatus,
    /* [in] */ BOOL fIsSafeToBlock)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::JITCachedFunctionSearchStarted(
    /* [in] */ FunctionID functionId,
    /* [out] */ BOOL* pbUseCachedFunction)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::JITCachedFunctionSearchFinished(
    /* [in] */ FunctionID functionId,
    /* [in] */ COR_PRF_JIT_CACHE result)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::JITFunctionPitched(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::JITInlining(
    /* [in] */ FunctionID callerId,
    /* [in] */ FunctionID calleeId,
    /* [out] */ BOOL* pfShouldInline)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ThreadCreated(
    /* [in] */ ThreadID threadId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ThreadDestroyed(
    /* [in] */ ThreadID threadId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ThreadAssignedToOSThread(
    /* [in] */ ThreadID managedThreadId,
    /* [in] */ DWORD osThreadId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RemotingClientInvocationStarted(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RemotingClientSendingMessage(
    /* [in] */ GUID* pCookie,
    /* [in] */ BOOL fIsAsync)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RemotingClientReceivingReply(
    /* [in] */ GUID* pCookie,
    /* [in] */ BOOL fIsAsync)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RemotingClientInvocationFinished(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RemotingServerReceivingMessage(
    /* [in] */ GUID* pCookie,
    /* [in] */ BOOL fIsAsync)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RemotingServerInvocationStarted(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RemotingServerInvocationReturned(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RemotingServerSendingReply(
    /* [in] */ GUID* pCookie,
    /* [in] */ BOOL fIsAsync)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::UnmanagedToManagedTransition(
    /* [in] */ FunctionID functionId,
    /* [in] */ COR_PRF_TRANSITION_REASON reason)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ManagedToUnmanagedTransition(
    /* [in] */ FunctionID functionId,
    /* [in] */ COR_PRF_TRANSITION_REASON reason)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RuntimeSuspendStarted(
    /* [in] */ COR_PRF_SUSPEND_REASON suspendReason)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RuntimeSuspendFinished(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RuntimeSuspendAborted(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RuntimeResumeStarted(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RuntimeResumeFinished(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RuntimeThreadSuspended(
    /* [in] */ ThreadID threadId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RuntimeThreadResumed(
    /* [in] */ ThreadID threadId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::MovedReferences(
    /* [in] */ ULONG cMovedObjectIDRanges,
    /* [size_is][in] */ ObjectID oldObjectIDRangeStart[],
    /* [size_is][in] */ ObjectID newObjectIDRangeStart[],
    /* [size_is][in] */ ULONG cObjectIDRangeLength[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ObjectAllocated(
    /* [in] */ ObjectID objectId,
    /* [in] */ ClassID classId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ObjectsAllocatedByClass(
    /* [in] */ ULONG cClassCount,
    /* [size_is][in] */ ClassID classIds[],
    /* [size_is][in] */ ULONG cObjects[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ObjectReferences(
    /* [in] */ ObjectID objectId,
    /* [in] */ ClassID classId,
    /* [in] */ ULONG cObjectRefs,
    /* [size_is][in] */ ObjectID objectRefIds[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RootReferences(
    /* [in] */ ULONG cRootRefs,
    /* [size_is][in] */ ObjectID rootRefIds[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionThrown(
    /* [in] */ ObjectID thrownObjectId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionSearchFunctionEnter(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionSearchFunctionLeave(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionSearchFilterEnter(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionSearchFilterLeave(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionSearchCatcherFound(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionOSHandlerEnter(
    /* [in] */ UINT_PTR __unused)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionOSHandlerLeave(
    /* [in] */ UINT_PTR __unused)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionUnwindFunctionEnter(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionUnwindFunctionLeave(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionUnwindFinallyEnter(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionUnwindFinallyLeave(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionCatcherEnter(
    /* [in] */ FunctionID functionId,
    /* [in] */ ObjectID objectId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionCatcherLeave(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::COMClassicVTableCreated(
    /* [in] */ ClassID wrappedClassId,
    /* [in] */ REFGUID implementedIID,
    /* [in] */ void* pVTable,
    /* [in] */ ULONG cSlots)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::COMClassicVTableDestroyed(
    /* [in] */ ClassID wrappedClassId,
    /* [in] */ REFGUID implementedIID,
    /* [in] */ void* pVTable)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionCLRCatcherFound(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ExceptionCLRCatcherExecute(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::ThreadNameChanged(
    /* [in] */ ThreadID threadId,
    /* [in] */ ULONG cchName,
    /* [annotation][in] */
    _In_reads_opt_(cchName)  WCHAR name[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::GarbageCollectionStarted(
    /* [in] */ int cGenerations,
    /* [size_is][in] */ BOOL generationCollected[],
    /* [in] */ COR_PRF_GC_REASON reason)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::SurvivingReferences(
    /* [in] */ ULONG cSurvivingObjectIDRanges,
    /* [size_is][in] */ ObjectID objectIDRangeStart[],
    /* [size_is][in] */ ULONG cObjectIDRangeLength[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::GarbageCollectionFinished(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::FinalizeableObjectQueued(
    /* [in] */ DWORD finalizerFlags,
    /* [in] */ ObjectID objectID)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::RootReferences2(
    /* [in] */ ULONG cRootRefs,
    /* [size_is][in] */ ObjectID rootRefIds[],
    /* [size_is][in] */ COR_PRF_GC_ROOT_KIND rootKinds[],
    /* [size_is][in] */ COR_PRF_GC_ROOT_FLAGS rootFlags[],
    /* [size_is][in] */ UINT_PTR rootIds[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::HandleCreated(
    /* [in] */ GCHandleID handleId,
    /* [in] */ ObjectID initialObjectId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMyProfiler::HandleDestroyed(
    /* [in] */ GCHandleID handleId)
{
    return S_OK;
};

#pragma warning(pop)
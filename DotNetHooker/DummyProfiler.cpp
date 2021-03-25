#include "pch.h"
#include "DummyProfiler.h"

#pragma warning(push)
#pragma warning(disable:4100) // disable unreferenced formal parameter as there is a lot of it here :)

HRESULT STDMETHODCALLTYPE DummyProfiler::Initialize(
    /* [in] */ IUnknown* pICorProfilerInfoUnk)
{
    return S_OK;
}


HRESULT STDMETHODCALLTYPE DummyProfiler::Shutdown(void)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DummyProfiler::AppDomainCreationStarted(
    /* [in] */ AppDomainID appDomainId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::AppDomainCreationFinished(
    /* [in] */ AppDomainID appDomainId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::AppDomainShutdownStarted(
    /* [in] */ AppDomainID appDomainId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::AppDomainShutdownFinished(
    /* [in] */ AppDomainID appDomainId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::AssemblyLoadStarted(
    /* [in] */ AssemblyID assemblyId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::AssemblyLoadFinished(
    /* [in] */ AssemblyID assemblyId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::AssemblyUnloadStarted(
    /* [in] */ AssemblyID assemblyId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::AssemblyUnloadFinished(
    /* [in] */ AssemblyID assemblyId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ModuleLoadStarted(
    /* [in] */ ModuleID moduleId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ModuleLoadFinished(
    /* [in] */ ModuleID moduleId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ModuleUnloadStarted(
    /* [in] */ ModuleID moduleId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ModuleUnloadFinished(
    /* [in] */ ModuleID moduleId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ModuleAttachedToAssembly(
    /* [in] */ ModuleID moduleId,
    /* [in] */ AssemblyID AssemblyId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ClassLoadStarted(
    /* [in] */ ClassID classId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ClassUnloadStarted(
    /* [in] */ ClassID classId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ClassLoadFinished(
    /* [in] */ ClassID classId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ClassUnloadFinished(
    /* [in] */ ClassID classId,
    /* [in] */ HRESULT hrStatus)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::FunctionUnloadStarted(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::JITCompilationStarted(
    /* [in] */ FunctionID functionId,
    /* [in] */ BOOL fIsSafeToBlock)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::JITCompilationFinished(
    /* [in] */ FunctionID functionId,
    /* [in] */ HRESULT hrStatus,
    /* [in] */ BOOL fIsSafeToBlock)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::JITCachedFunctionSearchStarted(
    /* [in] */ FunctionID functionId,
    /* [out] */ BOOL* pbUseCachedFunction)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::JITCachedFunctionSearchFinished(
    /* [in] */ FunctionID functionId,
    /* [in] */ COR_PRF_JIT_CACHE result)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::JITFunctionPitched(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::JITInlining(
    /* [in] */ FunctionID callerId,
    /* [in] */ FunctionID calleeId,
    /* [out] */ BOOL* pfShouldInline)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ThreadCreated(
    /* [in] */ ThreadID threadId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ThreadDestroyed(
    /* [in] */ ThreadID threadId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ThreadAssignedToOSThread(
    /* [in] */ ThreadID managedThreadId,
    /* [in] */ DWORD osThreadId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RemotingClientInvocationStarted(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RemotingClientSendingMessage(
    /* [in] */ GUID* pCookie,
    /* [in] */ BOOL fIsAsync)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RemotingClientReceivingReply(
    /* [in] */ GUID* pCookie,
    /* [in] */ BOOL fIsAsync)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RemotingClientInvocationFinished(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RemotingServerReceivingMessage(
    /* [in] */ GUID* pCookie,
    /* [in] */ BOOL fIsAsync)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RemotingServerInvocationStarted(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RemotingServerInvocationReturned(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RemotingServerSendingReply(
    /* [in] */ GUID* pCookie,
    /* [in] */ BOOL fIsAsync)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::UnmanagedToManagedTransition(
    /* [in] */ FunctionID functionId,
    /* [in] */ COR_PRF_TRANSITION_REASON reason)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ManagedToUnmanagedTransition(
    /* [in] */ FunctionID functionId,
    /* [in] */ COR_PRF_TRANSITION_REASON reason)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RuntimeSuspendStarted(
    /* [in] */ COR_PRF_SUSPEND_REASON suspendReason)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RuntimeSuspendFinished(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RuntimeSuspendAborted(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RuntimeResumeStarted(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RuntimeResumeFinished(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RuntimeThreadSuspended(
    /* [in] */ ThreadID threadId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RuntimeThreadResumed(
    /* [in] */ ThreadID threadId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::MovedReferences(
    /* [in] */ ULONG cMovedObjectIDRanges,
    /* [size_is][in] */ ObjectID oldObjectIDRangeStart[],
    /* [size_is][in] */ ObjectID newObjectIDRangeStart[],
    /* [size_is][in] */ ULONG cObjectIDRangeLength[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ObjectAllocated(
    /* [in] */ ObjectID objectId,
    /* [in] */ ClassID classId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ObjectsAllocatedByClass(
    /* [in] */ ULONG cClassCount,
    /* [size_is][in] */ ClassID classIds[],
    /* [size_is][in] */ ULONG cObjects[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ObjectReferences(
    /* [in] */ ObjectID objectId,
    /* [in] */ ClassID classId,
    /* [in] */ ULONG cObjectRefs,
    /* [size_is][in] */ ObjectID objectRefIds[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RootReferences(
    /* [in] */ ULONG cRootRefs,
    /* [size_is][in] */ ObjectID rootRefIds[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionThrown(
    /* [in] */ ObjectID thrownObjectId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionSearchFunctionEnter(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionSearchFunctionLeave(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionSearchFilterEnter(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionSearchFilterLeave(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionSearchCatcherFound(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionOSHandlerEnter(
    /* [in] */ UINT_PTR __unused)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionOSHandlerLeave(
    /* [in] */ UINT_PTR __unused)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionUnwindFunctionEnter(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionUnwindFunctionLeave(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionUnwindFinallyEnter(
    /* [in] */ FunctionID functionId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionUnwindFinallyLeave(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionCatcherEnter(
    /* [in] */ FunctionID functionId,
    /* [in] */ ObjectID objectId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionCatcherLeave(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::COMClassicVTableCreated(
    /* [in] */ ClassID wrappedClassId,
    /* [in] */ REFGUID implementedIID,
    /* [in] */ void* pVTable,
    /* [in] */ ULONG cSlots)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::COMClassicVTableDestroyed(
    /* [in] */ ClassID wrappedClassId,
    /* [in] */ REFGUID implementedIID,
    /* [in] */ void* pVTable)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionCLRCatcherFound(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ExceptionCLRCatcherExecute(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::ThreadNameChanged(
    /* [in] */ ThreadID threadId,
    /* [in] */ ULONG cchName,
    /* [annotation][in] */
    _In_reads_opt_(cchName)  WCHAR name[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::GarbageCollectionStarted(
    /* [in] */ int cGenerations,
    /* [size_is][in] */ BOOL generationCollected[],
    /* [in] */ COR_PRF_GC_REASON reason)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::SurvivingReferences(
    /* [in] */ ULONG cSurvivingObjectIDRanges,
    /* [size_is][in] */ ObjectID objectIDRangeStart[],
    /* [size_is][in] */ ULONG cObjectIDRangeLength[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::GarbageCollectionFinished(void)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::FinalizeableObjectQueued(
    /* [in] */ DWORD finalizerFlags,
    /* [in] */ ObjectID objectID)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::RootReferences2(
    /* [in] */ ULONG cRootRefs,
    /* [size_is][in] */ ObjectID rootRefIds[],
    /* [size_is][in] */ COR_PRF_GC_ROOT_KIND rootKinds[],
    /* [size_is][in] */ COR_PRF_GC_ROOT_FLAGS rootFlags[],
    /* [size_is][in] */ UINT_PTR rootIds[])
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::HandleCreated(
    /* [in] */ GCHandleID handleId,
    /* [in] */ ObjectID initialObjectId)
{
    return S_OK;
};

HRESULT STDMETHODCALLTYPE DummyProfiler::HandleDestroyed(
    /* [in] */ GCHandleID handleId)
{
    return S_OK;
};

#pragma warning(pop)
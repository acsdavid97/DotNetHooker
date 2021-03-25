#ifndef _DUMMY_PROFILER_H_
#define _DUMMY_PROFILER_H_

#include <cor.h>
#include <corprof.h>

class DummyProfiler : public ICorProfilerCallback2
{
public:
    HRESULT STDMETHODCALLTYPE Initialize(
        /* [in] */ IUnknown* pICorProfilerInfoUnk) override;

    HRESULT STDMETHODCALLTYPE Shutdown(void) override;

    HRESULT STDMETHODCALLTYPE AppDomainCreationStarted(
        /* [in] */ AppDomainID appDomainId) override;

    HRESULT STDMETHODCALLTYPE AppDomainCreationFinished(
        /* [in] */ AppDomainID appDomainId,
        /* [in] */HRESULT hrStatus) override;

    HRESULT STDMETHODCALLTYPE AppDomainShutdownStarted(
        /* [in] */ AppDomainID appDomainId) override;

    HRESULT STDMETHODCALLTYPE AppDomainShutdownFinished(
        /* [in] */ AppDomainID appDomainId,
        /* [in] */HRESULT hrStatus) override;

    HRESULT STDMETHODCALLTYPE AssemblyLoadStarted(
        /* [in] */ AssemblyID assemblyId) override;

    HRESULT STDMETHODCALLTYPE AssemblyLoadFinished(
        /* [in] */ AssemblyID assemblyId,
        /* [in] */HRESULT hrStatus) override;

    HRESULT STDMETHODCALLTYPE AssemblyUnloadStarted(
        /* [in] */ AssemblyID assemblyId) override;

    HRESULT STDMETHODCALLTYPE AssemblyUnloadFinished(
        /* [in] */ AssemblyID assemblyId,
        /* [in] */HRESULT hrStatus) override;

    HRESULT STDMETHODCALLTYPE ModuleLoadStarted(
        /* [in] */ ModuleID moduleId) override;

    HRESULT STDMETHODCALLTYPE ModuleLoadFinished(
        /* [in] */ ModuleID moduleId,
        /* [in] */HRESULT hrStatus) override;

    HRESULT STDMETHODCALLTYPE ModuleUnloadStarted(
        /* [in] */ ModuleID moduleId) override;

    HRESULT STDMETHODCALLTYPE ModuleUnloadFinished(
        /* [in] */ ModuleID moduleId,
        /* [in] */HRESULT hrStatus) override;

    HRESULT STDMETHODCALLTYPE ModuleAttachedToAssembly(
        /* [in] */ ModuleID moduleId,
        /* [in] */ AssemblyID AssemblyId) override;

    HRESULT STDMETHODCALLTYPE ClassLoadStarted(
        /* [in] */ ClassID classId) override;

    HRESULT STDMETHODCALLTYPE ClassLoadFinished(
        /* [in] */ ClassID classId,
        /* [in] */HRESULT hrStatus) override;

    HRESULT STDMETHODCALLTYPE ClassUnloadStarted(
        /* [in] */ ClassID classId) override;

    HRESULT STDMETHODCALLTYPE ClassUnloadFinished(
        /* [in] */ ClassID classId,
        /* [in] */HRESULT hrStatus) override;

    HRESULT STDMETHODCALLTYPE FunctionUnloadStarted(
        /* [in] */ FunctionID functionId) override;

    HRESULT STDMETHODCALLTYPE JITCompilationStarted(
        /* [in] */ FunctionID functionId,
        /* [in] */ BOOL fIsSafeToBlock) override;

    HRESULT STDMETHODCALLTYPE JITCompilationFinished(
        /* [in] */ FunctionID functionId,
        /* [in] */HRESULT hrStatus,
        /* [in] */ BOOL fIsSafeToBlock) override;

    HRESULT STDMETHODCALLTYPE JITCachedFunctionSearchStarted(
        /* [in] */ FunctionID functionId,
        /* [out] */ BOOL* pbUseCachedFunction) override;

    HRESULT STDMETHODCALLTYPE JITCachedFunctionSearchFinished(
        /* [in] */ FunctionID functionId,
        /* [in] */ COR_PRF_JIT_CACHE result) override;

    HRESULT STDMETHODCALLTYPE JITFunctionPitched(
        /* [in] */ FunctionID functionId) override;

    HRESULT STDMETHODCALLTYPE JITInlining(
        /* [in] */ FunctionID callerId,
        /* [in] */ FunctionID calleeId,
        /* [out] */ BOOL* pfShouldInline) override;

    HRESULT STDMETHODCALLTYPE ThreadCreated(
        /* [in] */ ThreadID threadId) override;

    HRESULT STDMETHODCALLTYPE ThreadDestroyed(
        /* [in] */ ThreadID threadId) override;

    HRESULT STDMETHODCALLTYPE ThreadAssignedToOSThread(
        /* [in] */ ThreadID managedThreadId,
        /* [in] */ DWORD osThreadId) override;

    HRESULT STDMETHODCALLTYPE RemotingClientInvocationStarted(void) override;

    HRESULT STDMETHODCALLTYPE RemotingClientSendingMessage(
        /* [in] */ GUID* pCookie,
        /* [in] */ BOOL fIsAsync) override;

    HRESULT STDMETHODCALLTYPE RemotingClientReceivingReply(
        /* [in] */ GUID* pCookie,
        /* [in] */ BOOL fIsAsync) override;

    HRESULT STDMETHODCALLTYPE RemotingClientInvocationFinished(void) override;

    HRESULT STDMETHODCALLTYPE RemotingServerReceivingMessage(
        /* [in] */ GUID* pCookie,
        /* [in] */ BOOL fIsAsync) override;

    HRESULT STDMETHODCALLTYPE RemotingServerInvocationStarted(void) override;

    HRESULT STDMETHODCALLTYPE RemotingServerInvocationReturned(void) override;

    HRESULT STDMETHODCALLTYPE RemotingServerSendingReply(
        /* [in] */ GUID* pCookie,
        /* [in] */ BOOL fIsAsync) override;

    HRESULT STDMETHODCALLTYPE UnmanagedToManagedTransition(
        /* [in] */ FunctionID functionId,
        /* [in] */ COR_PRF_TRANSITION_REASON reason) override;

    HRESULT STDMETHODCALLTYPE ManagedToUnmanagedTransition(
        /* [in] */ FunctionID functionId,
        /* [in] */ COR_PRF_TRANSITION_REASON reason) override;

    HRESULT STDMETHODCALLTYPE RuntimeSuspendStarted(
        /* [in] */ COR_PRF_SUSPEND_REASON suspendReason) override;

    HRESULT STDMETHODCALLTYPE RuntimeSuspendFinished(void) override;

    HRESULT STDMETHODCALLTYPE RuntimeSuspendAborted(void) override;

    HRESULT STDMETHODCALLTYPE RuntimeResumeStarted(void) override;

    HRESULT STDMETHODCALLTYPE RuntimeResumeFinished(void) override;

    HRESULT STDMETHODCALLTYPE RuntimeThreadSuspended(
        /* [in] */ ThreadID threadId) override;

    HRESULT STDMETHODCALLTYPE RuntimeThreadResumed(
        /* [in] */ ThreadID threadId) override;

    HRESULT STDMETHODCALLTYPE MovedReferences(
        /* [in] */ ULONG cMovedObjectIDRanges,
        /* [size_is][in] */ ObjectID oldObjectIDRangeStart[],
        /* [size_is][in] */ ObjectID newObjectIDRangeStart[],
        /* [size_is][in] */ ULONG cObjectIDRangeLength[]) override;

    HRESULT STDMETHODCALLTYPE ObjectAllocated(
        /* [in] */ ObjectID objectId,
        /* [in] */ ClassID classId) override;

    HRESULT STDMETHODCALLTYPE ObjectsAllocatedByClass(
        /* [in] */ ULONG cClassCount,
        /* [size_is][in] */ ClassID classIds[],
        /* [size_is][in] */ ULONG cObjects[]) override;

    HRESULT STDMETHODCALLTYPE ObjectReferences(
        /* [in] */ ObjectID objectId,
        /* [in] */ ClassID classId,
        /* [in] */ ULONG cObjectRefs,
        /* [size_is][in] */ ObjectID objectRefIds[]) override;

    HRESULT STDMETHODCALLTYPE RootReferences(
        /* [in] */ ULONG cRootRefs,
        /* [size_is][in] */ ObjectID rootRefIds[]) override;

    HRESULT STDMETHODCALLTYPE ExceptionThrown(
        /* [in] */ ObjectID thrownObjectId) override;

    HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionEnter(
        /* [in] */ FunctionID functionId) override;

    HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionLeave(void) override;

    HRESULT STDMETHODCALLTYPE ExceptionSearchFilterEnter(
        /* [in] */ FunctionID functionId) override;

    HRESULT STDMETHODCALLTYPE ExceptionSearchFilterLeave(void) override;

    HRESULT STDMETHODCALLTYPE ExceptionSearchCatcherFound(
        /* [in] */ FunctionID functionId) override;

    HRESULT STDMETHODCALLTYPE ExceptionOSHandlerEnter(
        /* [in] */ UINT_PTR __unused) override;

    HRESULT STDMETHODCALLTYPE ExceptionOSHandlerLeave(
        /* [in] */ UINT_PTR __unused) override;

    HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionEnter(
        /* [in] */ FunctionID functionId) override;

    HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionLeave(void) override;

    HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyEnter(
        /* [in] */ FunctionID functionId) override;

    HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyLeave(void) override;

    HRESULT STDMETHODCALLTYPE ExceptionCatcherEnter(
        /* [in] */ FunctionID functionId,
        /* [in] */ ObjectID objectId) override;

    HRESULT STDMETHODCALLTYPE ExceptionCatcherLeave(void) override;

    HRESULT STDMETHODCALLTYPE COMClassicVTableCreated(
        /* [in] */ ClassID wrappedClassId,
        /* [in] */ REFGUID implementedIID,
        /* [in] */ void* pVTable,
        /* [in] */ ULONG cSlots) override;

    HRESULT STDMETHODCALLTYPE COMClassicVTableDestroyed(
        /* [in] */ ClassID wrappedClassId,
        /* [in] */ REFGUID implementedIID,
        /* [in] */ void* pVTable) override;

    HRESULT STDMETHODCALLTYPE ExceptionCLRCatcherFound(void) override;

    HRESULT STDMETHODCALLTYPE ExceptionCLRCatcherExecute(void) override;

    HRESULT STDMETHODCALLTYPE ThreadNameChanged(
        /* [in] */ ThreadID threadId,
        /* [in] */ ULONG cchName,
        /* [annotation][in] */
        _In_reads_opt_(cchName)  WCHAR name[]) override;

    HRESULT STDMETHODCALLTYPE GarbageCollectionStarted(
        /* [in] */ int cGenerations,
        /* [size_is][in] */ BOOL generationCollected[],
        /* [in] */ COR_PRF_GC_REASON reason) override;

    HRESULT STDMETHODCALLTYPE SurvivingReferences(
        /* [in] */ ULONG cSurvivingObjectIDRanges,
        /* [size_is][in] */ ObjectID objectIDRangeStart[],
        /* [size_is][in] */ ULONG cObjectIDRangeLength[]) override;

    HRESULT STDMETHODCALLTYPE GarbageCollectionFinished(void) override;

    HRESULT STDMETHODCALLTYPE FinalizeableObjectQueued(
        /* [in] */ DWORD finalizerFlags,
        /* [in] */ ObjectID objectID) override;

    HRESULT STDMETHODCALLTYPE RootReferences2(
        /* [in] */ ULONG cRootRefs,
        /* [size_is][in] */ ObjectID rootRefIds[],
        /* [size_is][in] */ COR_PRF_GC_ROOT_KIND rootKinds[],
        /* [size_is][in] */ COR_PRF_GC_ROOT_FLAGS rootFlags[],
        /* [size_is][in] */ UINT_PTR rootIds[]) override;

    HRESULT STDMETHODCALLTYPE HandleCreated(
        /* [in] */ GCHandleID handleId,
        /* [in] */ ObjectID initialObjectId) override;

    HRESULT STDMETHODCALLTYPE HandleDestroyed(
        /* [in] */ GCHandleID handleId) override;

};

#endif // !_DUMMY_PROFILER_H_

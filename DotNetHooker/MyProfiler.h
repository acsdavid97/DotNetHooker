// MyProfiler.h : Declaration of the CMyProfiler

#pragma once

#include "resource.h"       // main symbols
#include <map>
#include <vector>
#include <shared_mutex>


#include "DotNetHooker_i.h"
#include "DummyProfiler.h"
#include "FunctionInfo.h"
#include "LogFile.h"


#ifdef _WIN32_WCE
#error "Neutral-threaded COM objects are not supported on Windows CE."
#endif

using namespace ATL;

#ifndef STATUS_BUFFER_OVERFLOW
#define STATUS_BUFFER_OVERFLOW 0x80000005
#endif // !STATUS_BUFFER_OVERFLOW


// CMyProfiler

class ATL_NO_VTABLE CMyProfiler :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CMyProfiler, &CLSID_MyProfiler>,
	public DummyProfiler
{
public:
	CMyProfiler()
	{
	}

DECLARE_REGISTRY_RESOURCEID(106)

DECLARE_NOT_AGGREGATABLE(CMyProfiler)

BEGIN_COM_MAP(CMyProfiler)
    COM_INTERFACE_ENTRY(ICorProfilerCallback2)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

    HRESULT STDMETHODCALLTYPE Initialize(
        /* [in] */ IUnknown* pICorProfilerInfoUnk) override;

    HRESULT STDMETHODCALLTYPE Shutdown(void) override;

    HRESULT STDMETHODCALLTYPE ClassLoadFinished(
        /* [in] */ ClassID classId,
        /* [in] */HRESULT hrStatus) override;

    void OnEnter(
        _In_ FunctionID funcId,
        _In_ UINT_PTR clientData,
        _In_ COR_PRF_FRAME_INFO func,
        _In_ COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo
    );

    void OnLeave(
        _In_ FunctionID funcId,
        _In_ UINT_PTR clientData,
        _In_ COR_PRF_FRAME_INFO func,
        _In_ COR_PRF_FUNCTION_ARGUMENT_RANGE* retvalRange
    );

    void OnTailcall(
        _In_ FunctionID funcId,
        _In_ UINT_PTR clientData,
        _In_ COR_PRF_FRAME_INFO func
    );

    UINT_PTR OnFunctionMap(
        _In_ FunctionID funcId,
        _Out_ BOOL* pbHookFunction
    );

private:
    HRESULT DumpFunctionArguments(
        _In_ const std::shared_ptr<FunctionInfo> FunctionInfo,
        _In_ COR_PRF_FUNCTION_ARGUMENT_INFO* ArgumentInfo,
        _In_ LONG CurrentLineNumber
    );

    HRESULT ResolveClassName(
        _In_ ClassID ClassId,
        _Out_ std::wstring& ClassName
    );

    UINT_PTR AddFunctionName(
        _In_ const std::shared_ptr<FunctionInfo>& FunctionName
    );

    std::shared_ptr<FunctionInfo> GetFunctionById(
        _In_ UINT_PTR FunctionId
    );

    void AddClassIdToNameMapping(
        _In_ ClassID ClassId,
        _In_ const std::wstring& ClassName
    );

    std::wstring GetClassNameById(
        _In_ ClassID ClassId
    );

    void ParseArgumentDumpingFilters();

    bool ShouldDumpArgsForFunction(
        _In_ const std::wstring& FunctionName
    );
    
    void DeleteProfilerEnvirionmentVars();


    ICorProfilerInfo2* profilerInfo = nullptr;
    
    mutable std::shared_mutex functionMapLock;
    UINT_PTR currentFunctionId = 0;
    std::map<UINT_PTR, std::shared_ptr<FunctionInfo>> functionIdToInfo;

    mutable std::shared_mutex classMapLock;
    std::map<ClassID, std::wstring> classIdToName;

    std::vector<std::wstring> argDumpingFilters;

    LogFile logFile;
};

OBJECT_ENTRY_AUTO(__uuidof(MyProfiler), CMyProfiler)

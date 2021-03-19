#ifndef _TRAMPOLINE_H_
#define _TRAMPOLINE_H_

extern "C" void __stdcall FunctionEnterTrampoline(
    _In_ FunctionID funcId,
    _In_ UINT_PTR clientData,
    _In_ COR_PRF_FRAME_INFO func,
    _In_ COR_PRF_FUNCTION_ARGUMENT_INFO * argumentInfo
);

extern "C" void __stdcall FunctionLeaveTrampoline(
    _In_ FunctionID funcId,
    _In_ UINT_PTR clientData,
    _In_ COR_PRF_FRAME_INFO func,
    _In_ COR_PRF_FUNCTION_ARGUMENT_RANGE * retvalRange
);

extern "C" void __stdcall FunctionTailcallTrampoline(
    _In_ FunctionID funcId,
    _In_ UINT_PTR clientData,
    _In_ COR_PRF_FRAME_INFO func
);

#endif // !_TRAMPOLINE_H_

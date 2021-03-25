.model flat, stdcall
option casemap:none

.code

FunctionEnterHook PROTO STDCALL :DWORD, :DWORD, :DWORD, :DWORD
FunctionLeaveHook PROTO :DWORD, :DWORD, :DWORD, :DWORD
FunctionTailcallHook PROTO :DWORD, :DWORD, :DWORD

;; masm 32 is smart enough to generate correct prologue and epilogue based on signature
;; can double check it with: dumpbin trampoline32.obj /DISASM from developer command prompt

FunctionEnterTrampoline proc STDCALL funcId:DWORD, clientData:DWORD, func:DWORD, argumentInfo:DWORD
    PUSHAD

    MOV EAX, argumentInfo
    PUSH EAX
    MOV EAX, func
    PUSH EAX
    MOV EAX, clientData
    PUSH EAX
    MOV EAX, funcId
    PUSH EAX
    CALL FunctionEnterHook

    POPAD
    ret
FunctionEnterTrampoline endp


FunctionLeaveTrampoline proc STDCALL funcId:DWORD, clientData:DWORD, func:DWORD, retvalRange:DWORD
    PUSHAD

    MOV EAX, retvalRange
    PUSH EAX
    MOV EAX, func
    PUSH EAX
    MOV EAX, clientData
    PUSH EAX
    MOV EAX, funcId
    PUSH EAX
    CALL FunctionLeaveHook

    POPAD
    ret
FunctionLeaveTrampoline endp


FunctionTailcallTrampoline proc STDCALL funcId:DWORD, clientData:DWORD, func:DWORD
    PUSHAD

    MOV EAX, func
    PUSH EAX
    MOV EAX, clientData
    PUSH EAX
    MOV EAX, funcId
    PUSH EAX
    CALL FunctionTailcallHook

    POPAD
    ret
FunctionTailcallTrampoline endp

end
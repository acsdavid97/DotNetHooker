.code

FunctionEnterHook PROTO
FunctionLeaveHook PROTO
FunctionTailcallHook PROTO

SAVE_REGISTERS MACRO
    ;; saving volatile registers
    PUSH RAX
    PUSH RCX
    PUSH RDX
    PUSH R8
    PUSH R9
    PUSH R10
    PUSH R11
ENDM

RESTORE_REGISTERS MACRO
    ;; restore volatile registers in reverse order
    POP R11
    POP R10
    POP R9
    POP R8
    POP RDX
    POP RCX
    POP RAX
ENDM

FunctionEnterTrampoline proc
    SAVE_REGISTERS
    
    CALL FunctionEnterHook

    RESTORE_REGISTERS

    ret
FunctionEnterTrampoline endp


FunctionLeaveTrampoline proc
    SAVE_REGISTERS
    
    CALL FunctionLeaveHook

    RESTORE_REGISTERS

    ret
FunctionLeaveTrampoline endp


FunctionTailcallTrampoline proc
    SAVE_REGISTERS
    
    CALL FunctionTailcallHook

    RESTORE_REGISTERS

    ret
FunctionTailcallTrampoline endp

end
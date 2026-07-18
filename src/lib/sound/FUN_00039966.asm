; FUN_00039966 @ 00039966  (46 bytes) -- hand-written assembly (fully commented).
;
; FUN_00039966: unregister a driver slot -- clear g_driver_ptr[slot] back to 0, freeing
; the entry that FUN_000398d7 filled in. The slot index is bounds-checked (must be < 16);
; out-of-range indices are ignored.
;
; Args (stack, cdecl):  [ebp+8] = slot index
; No return value. Runs inside a cli critical section.
; Globals:  0xbcfa g_driver_ptr[16]
;
FUN_00039966:
        push    ebp                              ; 55
        mov     ebp, esp                          ; 8bec
        push    esi                               ; 56
        push    edi                               ; 57
        pushfd                                    ; 9c    save caller FLAGS
        cli                                      ; fa    enter critical section
        mov     ebx, dword ptr [ebp + 8]          ; 8b5d08       ebx = slot index (arg)
        cmp     ebx, 0x10                          ; 83fb10      slot in range 0..15?
        jae     0x39983                            ; 730e       no -> .exit (ignore)
        shl     ebx, 1                             ; d1e3        ebx = slot*2
        shl     ebx, 1                             ; d1e3        ebx = slot*4 (dword index)
        mov     dword ptr [ebx + 0xbcfa], 0        ; c783fabc000000000000  g_driver_ptr[slot] = 0
; .exit (0x39983): critical-section exit
        push    ebp                               ; 55
        mov     ebp, esp                          ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502      restore caller IF?
        cli                                      ; fa
        je      0x3998e                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3

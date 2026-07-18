; FUN_00039994 @ 0x39994  (31 bytes) -- hand-written assembly (fully commented).
;
; FUN_00039994: a thin wrapper that invokes driver command 0x64 on a voice, passing a
; fixed image pointer (0x2c176) as the command's extra argument. It marshals the two
; arguments and hands off to sound_dispatch_trampoline, which resolves the voice's
; 0x64 handler and tail-calls it; the handler's return value (in eax) is returned as-is.
;
; From the callers, command 0x64 looks up / prepares the driver object for a voice: the
; returned eax is used by FUN_000399bd as a pointer whose field [+0x20] it reads.
; (The exact semantics of 0x64 and of the 0x2c176 data live in the driver handler, not
; here -- this routine only builds the call.)
;
; Args (cdecl):  [ebp+8] = voice index (0..15).
; Returns:       eax = whatever the voice's 0x64 handler returns (a driver object pointer).
; Globals:       0x2c176 = fixed pointer into the game image handed to the handler.

FUN_00039994:
        push    ebp
        mov     ebp, esp
        push    esi                          ; preserved for caller
        push    edi
        mov     eax, 0x2c176                 ; extra arg = fixed image pointer
        push    eax                          ;   (arg 2)
        push    dword ptr [ebp + 8]          ; arg 1 = voice index (also the dispatch selector)
        mov     eax, 0x64                    ; command code 0x64
        call    0x392ac                      ; -> sound_dispatch_trampoline (resolve + tail-call handler)
        add     esp, 8                        ; drop the two pushed args
        pop     edi
        pop     esi
        leave
        ret                                  ; eax = handler result

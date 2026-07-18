; FUN_00039a82 @ 00039a82  (61 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039a82.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039a82:
        mov     ebx, esp                         ; 8bdc
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0x10                        ; 83fb10
        jae     0x39abe                          ; 7332
        shl     ebx, 1                           ; d1e3
        shl     ebx, 1                           ; d1e3
        mov     edx, 0                           ; ba00000000
        xchg    dword ptr [ebx + 0xbd7a], edx    ; 87937abd0000
        cmp     edx, 0                           ; 83fa00
        je      0x39abe                          ; 741e
        mov     edx, dword ptr [ebx + 0xbd3a]    ; 8b933abd0000
        cmp     edx, -1                          ; 83faff
        je      0x39ab4                          ; 7409
        push    edx                              ; 52
        call    0x396d5                          ; e824fcffff     -> FUN_000396d5
        add     esp, 4                           ; 83c404
        mov     eax, 0x68                        ; b868000000
        jmp     0x392ac                          ; e9eef7ffff     -> sound_dispatch_trampoline
        ret                                      ; c3

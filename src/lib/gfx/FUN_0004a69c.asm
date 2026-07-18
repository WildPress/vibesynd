; FUN_0004a69c @ 0004a69c  (44 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004a69c.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004a69c:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        mov     ax, word ptr [ebp + 8]           ; 668b4508
        mov     bx, word ptr [ebp + 0xc]         ; 668b5d0c
        mov     cl, byte ptr [ebp + 0x10]        ; 8a4d10
        mov     ch, byte ptr [ebp + 0x14]        ; 8a6d14
        mov     dx, 0                            ; 66ba0000
        mov     edi, dword ptr [ebp + 0x1c]      ; 8b7d1c
        mov     esi, dword ptr [ebp + 0x20]      ; 8b7520
        call    0x4b073                          ; e8b2090000     -> FUN_0004b073
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3

; FUN_0004a66b @ 0004a66b  (49 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004a66b.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004a66b:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        mov     edi, dword ptr [ebp + 0x10]      ; 8b7d10
        mov     ax, word ptr [ebp + 8]           ; 668b4508
        mov     bx, word ptr [ebp + 0xc]         ; 668b5d0c
        mov     cl, byte ptr [edi + 4]           ; 8a4f04
        mov     ch, byte ptr [edi + 5]           ; 8a6f05
        mov     esi, dword ptr [edi]             ; 8b37
        mov     dx, word ptr [ebp + 0x14]        ; 668b5514
        mov     edi, dword ptr [0x5374]          ; 8b3d74530000
        call    0x4b073                          ; e8de090000     -> FUN_0004b073
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3

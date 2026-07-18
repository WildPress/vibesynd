; FUN_0004d04b @ 0004d04b  (30 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d04b.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d04b:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ecx                              ; 51
        push    edi                              ; 57
        push    esi                              ; 56
        mov     ecx, 0x1c632                     ; b932c60100
        sub     ecx, 0x108                       ; 81e908010000
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        mov     edi, dword ptr [ebp + 0xc]       ; 8b7d0c
        rep movsb byte ptr es:[edi], byte ptr [esi] ; f3a4
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     ecx                              ; 59
        leave                                    ; c9
        ret                                      ; c3

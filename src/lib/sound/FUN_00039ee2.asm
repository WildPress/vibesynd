; FUN_00039ee2 @ 00039ee2  (71 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039ee2.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039ee2:
        movzx   eax, word ptr [0xbddc]           ; 0fb705dcbd0000
        sub     eax, 6                           ; 83e806
        push    eax                              ; 50
        push    dword ptr [0x5370]               ; ff3570530000   0x5370=g_back_buf
        push    dword ptr [0xbdd0]               ; ff35d0bd0000
        call    0x3a7c4                          ; e8c6080000     -> FUN_0003a7c4
        add     esp, 0xc                         ; 83c40c
        mov     edi, dword ptr [0x5370]          ; 8b3d70530000   0x5370=g_back_buf
        mov     ax, word ptr [edi]               ; 668b07
        mov     word ptr [0xbdd6], ax            ; 66a3d6bd0000
        add     edi, 2                           ; 83c702
        mov     ax, word ptr [edi]               ; 668b07
        mov     word ptr [0xbdd8], ax            ; 66a3d8bd0000
        add     edi, 2                           ; 83c702
        mov     ax, word ptr [edi]               ; 668b07
        mov     word ptr [0xbdda], ax            ; 66a3dabd0000
        ret                                      ; c3

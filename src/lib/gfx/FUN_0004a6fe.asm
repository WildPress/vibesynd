; FUN_0004a6fe @ 0004a6fe  (54 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004a6fe.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004a6fe:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        push    ebp                              ; 55
        mov     esi, dword ptr [ebp + 0x10]      ; 8b7510
        movsx   ebx, word ptr [ebp + 8]          ; 0fbf5d08
        movsx   ecx, word ptr [ebp + 0xc]        ; 0fbf4d0c
        mov     edi, dword ptr [0x536c]          ; 8b3d6c530000
        mov     dl, byte ptr [esi + 4]           ; 8a5604
        mov     dh, byte ptr [esi + 5]           ; 8a7605
        mov     eax, 0                           ; b800000000
        mov     esi, dword ptr [esi]             ; 8b36
        call    0x4a734                          ; e809000000     -> draw_sprite_rle
        pop     ebp                              ; 5d
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3

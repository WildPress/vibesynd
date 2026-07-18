; FUN_00039495 @ 00039495  (49 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039495.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039495:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        mov     eax, 0                           ; b800000000
        cmp     dword ptr [ebp + 8], 0xd68d      ; 817d088dd60000
        jae     0x394b9                          ; 7311
        mov     eax, dword ptr [ebp + 8]         ; 8b4508
        mov     ebx, 0x20bc                      ; bbbc200000
        mov     ecx, 0x2710                      ; b910270000
        mul     ecx                              ; f7e1
        div     ebx                              ; f7f3
        push    eax                              ; 50
        call    0x39467                          ; e8a8ffffff     -> reprogram_pit_ch0
        add     esp, 4                           ; 83c404
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3

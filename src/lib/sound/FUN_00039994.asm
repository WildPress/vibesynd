; FUN_00039994 @ 00039994  (31 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039994.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039994:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        mov     eax, 0x2c176                     ; b876c10200
        push    eax                              ; 50
        push    dword ptr [ebp + 8]              ; ff7508
        mov     eax, 0x64                        ; b864000000
        call    0x392ac                          ; e800f9ffff     -> sound_dispatch_trampoline
        add     esp, 8                           ; 83c408
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3

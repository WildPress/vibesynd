; FUN_00039280 @ 00039280  (44 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039280.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039280:
        cmp     ebx, 0x10                        ; 83fb10
        jae     0x392a2                          ; 731d
        shl     ebx, 1                           ; d1e3
        shl     ebx, 1                           ; d1e3
        mov     ebx, dword ptr [ebx + 0xbcfa]    ; 8b9bfabc0000
        cmp     ebx, 0                           ; 83fb00
        je      0x392a2                          ; 740e
        mov     ecx, dword ptr [ebx]             ; 8b0b
        cmp     ecx, eax                         ; 3bc8
        je      0x392a8                          ; 740e
        add     ebx, 8                           ; 83c308
        cmp     ecx, -1                          ; 83f9ff
        jne     0x39294                          ; 75f2
        mov     eax, 0                           ; b800000000
        ret                                      ; c3
        mov     eax, dword ptr [ebx + 4]         ; 8b4304
        ret                                      ; c3

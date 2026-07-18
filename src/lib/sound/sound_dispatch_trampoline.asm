; sound_dispatch_trampoline @ 000392ac  (18 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses sound_dispatch_trampoline.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
sound_dispatch_trampoline:
        mov     ebx, esp                         ; 8bdc
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        call    0x39280                          ; e8caffffff     -> FUN_00039280
        cmp     eax, 0                           ; 83f800
        je      0x392bd                          ; 7402
        jmp     eax                              ; ffe0
        ret                                      ; c3

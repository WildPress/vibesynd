; FUN_000396d5 @ 000396d5  (77 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_000396d5.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_000396d5:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     ebx, dword ptr [ebp + 8]         ; 8b5d08
        cmp     ebx, -1                          ; 83fbff
        je      0x39711                          ; 742d
        shl     ebx, 1                           ; d1e3
        cmp     word ptr [ebx + 0xbc38], 0       ; 6683bb38bc000000
        je      0x39711                          ; 7421
        mov     word ptr [ebx + 0xbc38], 0       ; 66c78338bc00000000
        dec     word ptr [0xbbf0]                ; 66ff0df0bb0000
        jne     0x39711                          ; 750f
        push    0                                ; 6a00
        call    0x39467                          ; e85efdffff     -> reprogram_pit_ch0
        add     esp, 4                           ; 83c404
        call    0x3942f                          ; e81efdffff     -> FUN_0003942f
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x3971c                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3

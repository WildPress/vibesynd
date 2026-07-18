; FUN_00039879 @ 00039879  (69 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039879.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039879:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        cmp     dword ptr [ebp + 0xc], 0         ; 837d0c00
        jne     0x3988d                          ; 7507
        mov     eax, 0xd68d                      ; b88dd60000
        jmp     0x398a1                          ; eb14
        mov     eax, 0x2710                      ; b810270000
        mov     edx, 0                           ; ba00000000
        mov     ebx, 0x2e9c                      ; bb9c2e0000
        mul     dword ptr [ebp + 0xc]            ; f7650c
        div     ebx                              ; f7f3
        push    eax                              ; 50
        push    dword ptr [ebp + 8]              ; ff7508
        call    0x397f1                          ; e847ffffff     -> FUN_000397f1
        add     esp, 8                           ; 83c408
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x398b8                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3

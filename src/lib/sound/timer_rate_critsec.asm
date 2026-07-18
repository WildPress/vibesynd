; timer_rate_critsec @ 00039846  (51 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses timer_rate_critsec.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
timer_rate_critsec:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     edx, 0                           ; ba00000000
        mov     eax, 0xf4240                     ; b840420f00
        mov     ebx, dword ptr [ebp + 0xc]       ; 8b5d0c
        div     ebx                              ; f7f3
        push    eax                              ; 50
        push    dword ptr [ebp + 8]              ; ff7508
        call    0x397f1                          ; e88cffffff     -> FUN_000397f1
        add     esp, 8                           ; 83c408
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x39873                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3

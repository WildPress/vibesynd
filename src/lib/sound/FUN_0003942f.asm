; FUN_0003942f @ 0003942f  (56 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0003942f.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0003942f:
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     dword ptr [0xbcee], 0xffffffff   ; c705eebc0000ffffffff
        mov     eax, 8                           ; b808000000
        mov     edx, dword ptr [0xbce6]          ; 8b15e6bc0000
        mov     ebx, dword ptr [0xbcea]          ; 8b1deabc0000
        mov     ah, 0x25                         ; b425
        push    ds                               ; 1e
        mov     ds, bx                           ; 668edb
        int     0x21                             ; cd21
        pop     ds                               ; 1f
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x39462                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        ret                                      ; c3

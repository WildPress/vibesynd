; install_timer_isr @ 000393e0  (79 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses install_timer_isr.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
install_timer_isr:
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     eax, 8                           ; b808000000
        mov     ah, 0x35                         ; b435
        push    es                               ; 06
        int     0x21                             ; cd21
        mov     dx, es                           ; 668cc2
        pop     es                               ; 07
        mov     dword ptr [0xbce6], ebx          ; 891de6bc0000
        mov     dword ptr [0xbcea], edx          ; 8915eabc0000
        mov     ebx, 0x2bc2b                     ; bb2bbc0200
        mov     dword ptr [0xbc34], ebx          ; 891d34bc0000
        mov     eax, 8                           ; b808000000
        mov     edx, 0x2bb76                     ; ba76bb0200
        mov     bx, cs                           ; 668ccb
        mov     ah, 0x25                         ; b425
        push    ds                               ; 1e
        mov     ds, bx                           ; 668edb
        int     0x21                             ; cd21
        pop     ds                               ; 1f
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x3942a                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        ret                                      ; c3

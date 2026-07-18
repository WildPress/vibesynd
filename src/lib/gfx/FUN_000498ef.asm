; FUN_000498ef @ 000498ef  (51 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_000498ef.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_000498ef:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        mov     ax, word ptr [ebp + 8]           ; 668b4508
        mov     ah, 0                            ; b400
        int     0x10                             ; cd10
        mov     ax, 7                            ; 66b80700
        xor     cx, cx                           ; 6633c9
        mov     dx, 0x27e                        ; 66ba7e02
        int     0x33                             ; cd33
        mov     ax, 8                            ; 66b80800
        xor     cx, cx                           ; 6633c9
        mov     dx, 0x18e                        ; 66ba8e01
        int     0x33                             ; cd33
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3

; FUN_00039625 @ 00039625  (176 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00039625.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00039625:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     ebx, 0                           ; bb00000000
        cmp     word ptr [ebx + 0xbc38], 0       ; 6683bb38bc000000
        je      0x3964a                          ; 740f
        add     ebx, 2                           ; 83c302
        cmp     ebx, 0x20                        ; 83fb20
        jb      0x39631                          ; 72ee
        mov     eax, 0xffffffff                  ; b8ffffffff
        jmp     0x396c4                          ; eb7a
        mov     eax, ebx                         ; 8bc3
        shr     eax, 1                           ; d1e8
        mov     word ptr [ebx + 0xbc38], 1       ; 66c78338bc00000100
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        shl     ebx, 1                           ; d1e3
        mov     dword ptr [ebx + 0xbbf4], esi    ; 89b3f4bb0000
        mov     dword ptr [ebx + 0xbc9e], 0xffffffff ; c7839ebc0000ffffffff
        inc     word ptr [0xbbf0]                ; 66ff05f0bb0000
        cmp     word ptr [0xbbf0], 1             ; 66833df0bb000001
        jne     0x396c4                          ; 7547
        push    eax                              ; 50
        call    0x39393                          ; e810fdffff     -> clear_voice_tables
        mov     word ptr [0xbc58], 1             ; 66c70558bc00000100
        call    0x393e0                          ; e84ffdffff     -> install_timer_isr
        push    0xd68d                           ; 688dd60000
        push    0x10                             ; 6a10
        call    0x397f1                          ; e854010000     -> FUN_000397f1
        add     esp, 8                           ; 83c408
        push    0x10                             ; 6a10
        call    0x39747                          ; e8a0000000     -> FUN_00039747
        add     esp, 4                           ; 83c404
        pop     eax                              ; 58
        mov     ebx, eax                         ; 8bd8
        shl     ebx, 1                           ; d1e3
        mov     word ptr [ebx + 0xbc38], 1       ; 66c78338bc00000100
        shl     ebx, 1                           ; d1e3
        mov     dword ptr [ebx + 0xbc9e], 0xffffffff ; c7839ebc0000ffffffff
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x396cf                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3

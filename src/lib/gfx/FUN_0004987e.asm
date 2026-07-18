; FUN_0004987e @ 0004987e  (113 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004987e.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004987e:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        test    byte ptr [0x105], 2              ; f6050501000002
        jne     0x49899                          ; 7509
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x498cb                          ; 7432
        mov     bx, 0                            ; 66bb0000
        mov     cx, 0x10                         ; 66b91000
        push    bx                               ; 6653
        push    cx                               ; 6651
        mov     ax, 0x1000                       ; 66b80010
        int     0x10                             ; cd10
        pop     cx                               ; 6659
        pop     bx                               ; 665b
        inc     bh                               ; fec7
        inc     bl                               ; fec3
        loop    0x498a1                          ; 67e2eb
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        mov     dx, 0x3c8                        ; 66bac803
        mov     al, 0                            ; b000
        out     dx, al                           ; ee
        mov     dl, 0xc9                         ; b2c9
        mov     ecx, 0x30                        ; b930000000
        rep outsb dx, byte ptr [esi]             ; f36e
        jmp     0x498e7                          ; eb1c
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x498e7                          ; 7413
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        mov     dx, 0x3c8                        ; 66bac803
        mov     al, 0                            ; b000
        out     dx, al                           ; ee
        mov     dl, 0xc9                         ; b2c9
        mov     ecx, 0x300                       ; b900030000
        rep outsb dx, byte ptr [esi]             ; f36e
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3

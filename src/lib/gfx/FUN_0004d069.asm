; FUN_0004d069 @ 0004d069  (75 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d069.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d069:
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        mov     esi, dword ptr [0x5360]          ; 8b3560530000
        mov     edi, 0xe39c                      ; bf9ce30000
        mov     dx, 0xff                         ; 66baff00
        mov     byte ptr [edi], 0                ; c60700
        mov     cl, 0                            ; b100
        mov     eax, dword ptr [0x5360]          ; a160530000
        add     eax, dword ptr [esi]             ; 0306
        add     esi, 4                           ; 83c604
        mov     ch, 0xf                          ; b50f
        cmp     dword ptr [eax], 0               ; 833800
        jne     0x4d0a1                          ; 750d
        add     eax, 0x14                        ; 83c014
        dec     ch                               ; fecd
        jge     0x4d08f                          ; 7df4
        mov     bl, 1                            ; b301
        shl     bl, cl                           ; d2e3
        or      byte ptr [edi], bl               ; 081f
        inc     cl                               ; fec1
        cmp     cl, 6                            ; 80f906
        jne     0x4d083                          ; 75db
        inc     edi                              ; 47
        dec     dx                               ; 664a
        jge     0x4d07e                          ; 7dd1
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        ret                                      ; c3

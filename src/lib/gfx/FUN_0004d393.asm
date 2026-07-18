; FUN_0004d393 @ 0004d393  (54 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004d393.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004d393:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        mov     ecx, dword ptr [ebp + 8]         ; 8b4d08
        or      ecx, ecx                         ; 0bc9
        je      0x4d3c2                          ; 7422
        bsr     eax, ecx                         ; 0fbdc1
        movzx   ebx, word ptr [eax*2 + 0x3fc81]  ; 0fb71c4581fc0300
        mov     eax, ecx                         ; 8bc1
        xor     edx, edx                         ; 33d2
        div     ebx                              ; f7f3
        cmp     eax, ebx                         ; 3bc3
        jge     0x4d3bb                          ; 7d06
        add     ebx, eax                         ; 03d8
        shr     ebx, 1                           ; d1eb
        jmp     0x4d3ab                          ; ebf0
        mov     eax, ebx                         ; 8bc3
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
        xor     eax, eax                         ; 33c0
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3

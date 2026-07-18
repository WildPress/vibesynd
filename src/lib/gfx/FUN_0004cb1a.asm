; FUN_0004cb1a @ 0004cb1a  (78 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004cb1a.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004cb1a:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        mov     eax, dword ptr [ebp + 8]         ; 8b4508
        mov     edi, 0xe384                      ; bf84e30000
        call    0x4d451                          ; e822090000     -> FUN_0004d451
        mov     byte ptr [edi], 0                ; c60700
        movzx   eax, word ptr [ebp + 0x1c]       ; 0fb7451c
        neg     eax                              ; f7d8
        add     eax, 0xe384                      ; 0584e30000
        add     eax, 0xa                         ; 83c00a
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x18]            ; 66ff7518
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x14]            ; 66ff7514
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x10]            ; 66ff7510
        push    dword ptr [ebp + 0xc]            ; ff750c
        push    eax                              ; 50
        call    0x4cb68                          ; e80a000000     -> FUN_0004cb68
        add     esp, 0x14                        ; 83c414
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3

; FUN_0004cacc @ 0004cacc  (78 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004cacc.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004cacc:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        mov     eax, dword ptr [ebp + 8]         ; 8b4508
        mov     edi, 0xe384                      ; bf84e30000
        call    0x4d513                          ; e8320a0000     -> FUN_0004d513
        mov     byte ptr [edi], 0                ; c60700
        movzx   eax, word ptr [ebp + 0x1c]       ; 0fb7451c
        neg     eax                              ; f7d8
        add     eax, 0xe384                      ; 0584e30000
        add     eax, 8                           ; 83c008
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x18]            ; 66ff7518
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x14]            ; 66ff7514
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x10]            ; 66ff7510
        push    dword ptr [ebp + 0xc]            ; ff750c
        push    eax                              ; 50
        call    0x4cb68                          ; e858000000     -> FUN_0004cb68
        add     esp, 0x14                        ; 83c414
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3

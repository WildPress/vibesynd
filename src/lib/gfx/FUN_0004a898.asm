; FUN_0004a898 @ 0004a898  (57 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004a898.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004a898:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        push    ebp                              ; 55
        push    es                               ; 06
        mov     ax, ds                           ; 668cd8
        mov     es, ax                           ; 668ec0
        mov     esi, dword ptr [ebp + 0x14]      ; 8b7514
        mov     ax, word ptr [ebp + 8]           ; 668b4508
        mov     bx, word ptr [ebp + 0xc]         ; 668b5d0c
        mov     cl, byte ptr [esi + 4]           ; 8a4e04
        mov     ch, byte ptr [esi + 5]           ; 8a6e05
        mov     edi, dword ptr [esi]             ; 8b3e
        mov     esi, dword ptr [0x536c]          ; 8b356c530000
        call    0x4b073                          ; e8ac070000     -> FUN_0004b073
        pop     es                               ; 07
        pop     ebp                              ; 5d
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3

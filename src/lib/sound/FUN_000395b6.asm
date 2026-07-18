; FUN_000395b6 @ 000395b6  (111 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_000395b6.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_000395b6:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     dword ptr [0xbdbe], 0            ; c705bebd000000000000
        mov     esi, dword ptr [0xbdbe]          ; 8b35bebd0000
        shl     esi, 2                           ; c1e602
        mov     edx, dword ptr [esi + 0xbd3a]    ; 8b963abd0000
        mov     eax, dword ptr [esi + 0xbcfa]    ; 8b86fabc0000
        cmp     eax, 0                           ; 83f800
        je      0x39600                          ; 741f
        cmp     edx, -1                          ; 83faff
        je      0x395ef                          ; 7409
        push    edx                              ; 52
        call    0x396d5                          ; e8e9000000     -> FUN_000396d5
        add     esp, 4                           ; 83c404
        push    dword ptr [ebp + 8]              ; ff7508
        push    dword ptr [0xbdbe]               ; ff35bebd0000
        call    0x39a82                          ; e885040000     -> FUN_00039a82
        add     esp, 8                           ; 83c408
        inc     dword ptr [0xbdbe]               ; ff05bebd0000
        cmp     dword ptr [0xbdbe], 0x10         ; 833dbebd000010
        jne     0x395c7                          ; 75b8
        call    0x39722                          ; e80e010000     -> FUN_00039722
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x3961f                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3

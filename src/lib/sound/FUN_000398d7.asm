; FUN_000398d7 @ 000398d7  (143 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_000398d7.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_000398d7:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    esi                              ; 56
        push    edi                              ; 57
        pushfd                                   ; 9c
        cli                                      ; fa
        mov     dword ptr [0xbdbe], 0            ; c705bebd000000000000
        mov     esi, dword ptr [0xbdbe]          ; 8b35bebd0000
        shl     esi, 1                           ; d1e6
        shl     esi, 1                           ; d1e6
        mov     eax, dword ptr [esi + 0xbcfa]    ; 8b86fabc0000
        cmp     eax, 0                           ; 83f800
        je      0x39913                          ; 7416
        inc     dword ptr [0xbdbe]               ; ff05bebd0000
        cmp     dword ptr [0xbdbe], 0x10         ; 833dbebd000010
        jne     0x398e8                          ; 75dc
        mov     eax, 0xffffffff                  ; b8ffffffff
        jmp     0x39955                          ; eb42
        mov     edi, dword ptr [ebp + 8]         ; 8b7d08
        mov     eax, 0xffffffff                  ; b8ffffffff
        cmp     dword ptr [edi + 4], 0x79706f43  ; 817f04436f7079
        jne     0x39955                          ; 7531
        mov     edi, dword ptr [edi]             ; 8b3f
        mov     dword ptr [esi + 0xbcfa], edi    ; 89befabc0000
        push    dword ptr [0xbdbe]               ; ff35bebd0000
        call    0x39994                          ; e85d000000     -> FUN_00039994
        add     esp, 4                           ; 83c404
        mov     edi, eax                         ; 8bf8
        cmp     eax, 0                           ; 83f800
        mov     eax, 0xffffffff                  ; b8ffffffff
        je      0x39955                          ; 740f
        mov     edx, dword ptr [edi]             ; 8b17
        cmp     edx, dword ptr [0xbdcc]          ; 3b15ccbd0000
        ja      0x39955                          ; 7705
        mov     eax, dword ptr [0xbdbe]          ; a1bebd0000
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        test    byte ptr [ebp + 5], 2            ; f6450502
        cli                                      ; fa
        je      0x39960                          ; 7401
        sti                                      ; fb
        pop     ebp                              ; 5d
        popfd                                    ; 9d
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        leave                                    ; c9
        ret                                      ; c3

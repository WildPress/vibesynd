; FUN_0004993b @ 0004993b  (67 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004993b.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004993b:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x49962                          ; 7415
        push    0x1f400                          ; 6800f40100
        push    dword ptr [ebp + 0xc]            ; ff750c
        push    dword ptr [ebp + 8]              ; ff7508
        call    0x4d199                          ; e83c380000     -> fill_bytes
        add     esp, 0xc                         ; 83c40c
        jmp     0x49976                          ; eb14
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4996d                          ; 7402
        jmp     0x49976                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x49976                          ; 7400
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3

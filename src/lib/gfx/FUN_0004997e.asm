; FUN_0004997e @ 0004997e  (179 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004997e.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004997e:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        cld                                      ; fc
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x49a15                          ; 0f8480000000
        mov     dx, 0x3ce                        ; 66bace03
        mov     ax, 0xff08                       ; 66b808ff
        out     dx, ax                           ; 66ef
        mov     ax, 5                            ; 66b80500
        out     dx, ax                           ; 66ef
        mov     dl, 0xc4                         ; b2c4
        mov     ax, 0x102                        ; 66b80201
        out     dx, ax                           ; 66ef
        push    0x9600                           ; 6800960000
        push    dword ptr [ebp + 8]              ; ff7508
        push    0xa0000                          ; 6800000a00
        call    0x4d199                          ; e8da370000     -> FUN_0004d199
        add     esp, 0xc                         ; 83c40c
        mov     ax, 0x202                        ; 66b80202
        out     dx, ax                           ; 66ef
        push    0x9600                           ; 6800960000
        push    dword ptr [ebp + 8]              ; ff7508
        push    0xa0000                          ; 6800000a00
        call    0x4d199                          ; e8bf370000     -> FUN_0004d199
        add     esp, 0xc                         ; 83c40c
        mov     ax, 0x402                        ; 66b80204
        out     dx, ax                           ; 66ef
        push    0x9600                           ; 6800960000
        push    dword ptr [ebp + 8]              ; ff7508
        push    0xa0000                          ; 6800000a00
        call    0x4d199                          ; e8a4370000     -> FUN_0004d199
        add     esp, 0xc                         ; 83c40c
        mov     ax, 0x802                        ; 66b80208
        out     dx, ax                           ; 66ef
        push    0x9600                           ; 6800960000
        push    dword ptr [ebp + 8]              ; ff7508
        push    0xa0000                          ; 6800000a00
        call    0x4d199                          ; e889370000     -> FUN_0004d199
        add     esp, 0xc                         ; 83c40c
        jmp     0x49a29                          ; eb14
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x49a20                          ; 7402
        jmp     0x49a29                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x49a29                          ; 7400
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3

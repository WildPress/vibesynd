; FUN_00040f4e @ 00040f4e  (301 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00040f4e.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00040f4e:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -0xc                        ; 83c4f4
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x4105f                          ; 0f84f8000000
        mov     ax, word ptr [ebp + 0x1c]        ; 668b451c
        sar     ax, 5                            ; 66c1f805
        mov     cx, word ptr [ebp + 0x24]        ; 668b4d24
        sar     cx, 5                            ; 66c1f905
        mov     bx, word ptr [ebp + 0x20]        ; 668b5d20
        sar     bx, 4                            ; 66c1fb04
        mov     dx, word ptr [ebp + 0x28]        ; 668b5528
        sar     dx, 4                            ; 66c1fa04
        sub     cx, ax                           ; 662bc8
        inc     cx                               ; 6641
        mov     word ptr [ebp - 0xa], cx         ; 66894df6
        sub     dx, bx                           ; 662bd3
        inc     dx                               ; 6642
        mov     word ptr [ebp - 0xc], dx         ; 668955f4
        movsx   eax, ax                          ; 0fbfc0
        movsx   ebx, bx                          ; 0fbfdb
        shl     eax, 2                           ; c1e002
        imul    ebx, ebx, 0x500                  ; 69db00050000
        mov     dword ptr [ebp - 4], eax         ; 8945fc
        mov     dword ptr [ebp - 8], ebx         ; 895df8
        mov     bx, word ptr [ebp - 0xc]         ; 668b5df4
        movzx   edi, bx                          ; 0fb7fb
        inc     edi                              ; 47
        mov     ax, word ptr [ebp + 0x18]        ; 668b4518
        shl     bx, 7                            ; 66c1e307
        add     bx, ax                           ; 6603d8
        sub     bx, 0x100                        ; 6681eb0001
        movzx   cx, byte ptr [ebp + 8]           ; 660fb64d08
        movzx   dx, byte ptr [ebp + 0x10]        ; 660fb65510
        add     cx, dx                           ; 6603ca
        cmp     cx, 0x100                        ; 6681f90001
        jl      0x40fdb                          ; 7c01
        dec     edi                              ; 4f
        sub     word ptr [ebp + 8], bx           ; 66295d08
        sub     word ptr [ebp + 0x10], bx        ; 66295d10
        add     ax, 0x7f                         ; 6683c07f
        sar     ax, 7                            ; 66c1f807
        shl     ax, 4                            ; 66c1e004
        add     di, ax                           ; 6603f8
        mov     ax, word ptr [ebp + 0xc]         ; 668b450c
        mov     bx, word ptr [ebp + 0x14]        ; 668b5d14
        add     word ptr [ebp + 8], ax           ; 66014508
        add     word ptr [ebp + 0x10], bx        ; 66015d10
        movzx   ax, byte ptr [edi + 0xa7dc]      ; 660fb687dca70000
        dec     edi                              ; 4f
        cmp     dword ptr [ebp - 8], 0           ; 837df800
        jl      0x41044                          ; 7c33
        cmp     dword ptr [ebp - 8], 0x7d00      ; 817df8007d0000
        jge     0x41073                          ; 7d59
        push    edi                              ; 57
        push    dword ptr [ebp - 8]              ; ff75f8
        push    dword ptr [ebp - 4]              ; ff75fc
        sub     esp, 2                           ; 83ec02
        push    ax                               ; 6650
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp - 0xa]             ; 66ff75f6
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 0x10]            ; 66ff7510
        sub     esp, 2                           ; 83ec02
        push    word ptr [ebp + 8]               ; 66ff7508
        call    0x4107b                          ; e83b000000     -> FUN_0004107b
        add     esp, 0x18                        ; 83c418
        pop     edi                              ; 5f
        add     word ptr [ebp + 8], 0x80         ; 668145088000
        add     word ptr [ebp + 0x10], 0x80      ; 668145108000
        add     dword ptr [ebp - 8], 0x500       ; 8145f800050000
        dec     word ptr [ebp - 0xc]             ; 66ff4df4
        jg      0x41002                          ; 7fa5
        jmp     0x41073                          ; eb14
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4106a                          ; 7402
        jmp     0x41073                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x41073                          ; 7400
        pop     edi                              ; 5f
        pop     esi                              ; 5e
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b
        pop     eax                              ; 58
        leave                                    ; c9
        ret                                      ; c3

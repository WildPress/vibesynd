; FUN_0004cb68 @ 0004cb68  (227 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004cb68.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004cb68:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -4                          ; 83c4fc
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        test    byte ptr [0x105], 2              ; f6050501000002
        jne     0x4cb89                          ; 750d
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4cc3d                          ; 0f84b4000000
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        test    byte ptr [0x105], 2              ; f6050501000002
        jne     0x4cb9d                          ; 7508
        shr     word ptr [ebp + 0x10], 1         ; 66d16d10
        shr     word ptr [ebp + 0x14], 1         ; 66d16d14
        mov     ax, word ptr [ebp + 0x10]        ; 668b4510
        mov     word ptr [ebp - 2], ax           ; 668945fe
        mov     ax, word ptr [ebp + 0x18]        ; 668b4518
        mov     word ptr [ebp + 0x18], ax        ; 66894518
        movzx   edi, byte ptr [esi]              ; 0fb63e
        inc     esi                              ; 46
        cmp     edi, 0                           ; 83ff00
        je      0x4cc46                          ; 0f848c000000
        cmp     edi, 0xd                         ; 83ff0d
        je      0x4cc19                          ; 745a
        imul    edi, edi, 5                      ; 6bff05
        add     edi, dword ptr [ebp + 0xc]       ; 037d0c
        mov     ax, word ptr [ebp + 0x10]        ; 668b4510
        mov     bx, word ptr [ebp + 0x14]        ; 668b5d14
        movzx   cx, byte ptr [edi + 4]           ; 660fb64f04
        add     bx, cx                           ; 6603d9
        movzx   cx, byte ptr [edi + 3]           ; 660fb64f03
        movzx   dx, byte ptr [edi + 2]           ; 660fb65702
        add     word ptr [ebp + 0x10], dx        ; 66015510
        add     word ptr [ebp + 0x10], 1         ; 6683451001
        movzx   edi, word ptr [edi]              ; 0fb73f
        cmp     edi, 0                           ; 83ff00
        jle     0x4cc17                          ; 7e27
        cmp     cx, 0                            ; 6683f900
        jle     0x4cc17                          ; 7e21
        add     edi, dword ptr [ebp + 0xc]       ; 037d0c
        sar     dx, 3                            ; 66c1fa03
        cmp     dx, 0                            ; 6683fa00
        je      0x4cc0e                          ; 740b
        mov     dx, word ptr [ebp + 0x18]        ; 668b5518
        call    0x4cc4d                          ; e841000000     -> FUN_0004cc4d
        jmp     0x4cc17                          ; eb09
        mov     dx, word ptr [ebp + 0x18]        ; 668b5518
        call    0x4cd19                          ; e802010000     -> FUN_0004cd19
        jmp     0x4cbad                          ; eb94
        mov     ax, word ptr [ebp - 2]           ; 668b45fe
        mov     word ptr [ebp + 0x10], ax        ; 66894510
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x4cc31                          ; 7407
        add     word ptr [ebp + 0x14], 0xc       ; 668345140c
        jmp     0x4cc36                          ; eb05
        add     word ptr [ebp + 0x14], 6         ; 6683451406
        jmp     0x4cbad                          ; e972ffffff
        jmp     0x4cc46                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4cc46                          ; 7400
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b

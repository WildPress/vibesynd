; FUN_0004cde3 @ 0004cde3  (162 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004cde3.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004cde3:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -0x18                       ; 83c4e8
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    edi                              ; 57
        push    esi                              ; 56
        mov     esi, dword ptr [ebp + 8]         ; 8b7508
        mov     eax, dword ptr [0x10abc]         ; a1bc0a0100
        mov     dword ptr [ebp - 0x10], eax      ; 8945f0
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x4ce6c                          ; 746a
        movzx   eax, word ptr [ebp + 0xc]        ; 0fb7450c
        movzx   ebx, word ptr [ebp + 0x10]       ; 0fb75d10
        mov     cl, al                           ; 8ac8
        and     cl, 7                            ; 80e107
        shr     eax, 3                           ; c1e803
        imul    ebx, ebx, 0x50                   ; 6bdb50
        add     ebx, eax                         ; 03d8
        add     ebx, dword ptr [0x5368]          ; 031d68530000   0x5368=g_screen_buf
        mov     dword ptr [ebp - 0xc], ebx       ; 895df4
        mov     dword ptr [ebp - 8], ebx         ; 895df8
        movzx   eax, word ptr [ebp + 0x14]       ; 0fb74514
        imul    eax, eax, 0x2800                 ; 69c000280000
        add     dword ptr [ebp - 0x10], eax      ; 0145f0
        movzx   edx, byte ptr [esi]              ; 0fb616
        inc     esi                              ; 46
        cmp     edx, 0                           ; 83fa00
        je      0x4ce80                          ; 7447
        cmp     edx, 0xd                         ; 83fa0d
        je      0x4ce5a                          ; 741c
        sub     edx, 0x20                        ; 83ea20
        shl     edx, 6                           ; c1e206
        add     edx, dword ptr [ebp - 0x10]      ; 0355f0
        mov     edi, dword ptr [ebp - 8]         ; 8b7df8
        call    0x4ce87                          ; e838000000     -> FUN_0004ce87
        inc     dword ptr [ebp - 8]              ; ff45f8
        dec     word ptr [ebp + 0x18]            ; 66ff4d18
        jne     0x4ce30                          ; 75d8
        jmp     0x4ce80                          ; eb26
        mov     eax, dword ptr [ebp - 0xc]       ; 8b45f4
        add     eax, 0x280                       ; 0580020000
        mov     dword ptr [ebp - 0xc], eax       ; 8945f4
        mov     dword ptr [ebp - 8], eax         ; 8945f8
        jmp     0x4ce52                          ; ebe8
        jmp     0x4ce80                          ; eb14
        test    byte ptr [0x105], 4              ; f6050501000004
        je      0x4ce77                          ; 7402
        jmp     0x4ce80                          ; eb09
        test    byte ptr [0x105], 1              ; f6050501000001
        je      0x4ce80                          ; 7400
        pop     esi                              ; 5e
        pop     edi                              ; 5f
        pop     edx                              ; 5a
        pop     ecx                              ; 59
        pop     ebx                              ; 5b

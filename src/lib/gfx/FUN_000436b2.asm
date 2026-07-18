; FUN_000436b2 @ 000436b2  (4491 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_000436b2.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_000436b2:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -4                          ; 83c4fc
        mov     byte ptr [ebp - 1], 0            ; c645ff00
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c6c110000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xc14                       ; 81c1140c0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4373a                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x4373a                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xc14]     ; 8b9e140c0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x4373a                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x4373a                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xc14                       ; 81c1140c0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4373a                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4373a                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43721                          ; 7505
        call    0x46188                          ; e8672a0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e87ce1ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8efc100000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cee100000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x437b8                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x437b8                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xa14]     ; 8b9e140a0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x437b8                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x437b8                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x437b8                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x437b8                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4379f                          ; 7505
        call    0x46188                          ; e8e9290000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8fee0ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e7e100000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c70100000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43836                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43836                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xa10]     ; 8b9e100a0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x43836                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x43836                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43836                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43836                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4381d                          ; 7505
        call    0x46188                          ; e86b290000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e880e0ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e00100000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cf20f0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x438b3                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x438b3                          ; 7c5b
        mov     ebx, dword ptr [esi + 0xa14]     ; 8b9e140a0000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x438b3                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x438b3                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x438b3                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x438b3                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4389a                          ; 7505
        call    0x46188                          ; e8ee280000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e803e0ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e830f0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c750f0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43931                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43931                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xa10]     ; 8b9e100a0000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x43931                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x43931                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43931                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43931                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43918                          ; 7505
        call    0x46188                          ; e870280000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e885dfffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e050f0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cf70e0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x439af                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x439af                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x439af                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x439af                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x439af                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x439af                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43996                          ; 7505
        call    0x46188                          ; e8f2270000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e807dfffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e870e0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c790e0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43a2d                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43a2d                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xa10]     ; 8b9e100a0000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x43a2d                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x43a2d                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa10                       ; 81c1100a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43a2d                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43a2d                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43a14                          ; 7505
        call    0x46188                          ; e874270000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e889deffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e090e0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cfb0d0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43aab                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43aab                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x43aab                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x43aab                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43aab                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43aab                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43a92                          ; 7505
        call    0x46188                          ; e8f6260000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e80bdeffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e8b0d0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c7d0d0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43b29                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43b29                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x80c]     ; 8b9e0c080000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x43b29                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x43b29                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43b29                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43b29                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43b10                          ; 7505
        call    0x46188                          ; e878260000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e88dddffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e0d0d0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cff0c0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43ba6                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x43ba6                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x43ba6                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x43ba6                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43ba6                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43ba6                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43b8d                          ; 7505
        call    0x46188                          ; e8fb250000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e810ddffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e900c0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c820c0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43c24                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43c24                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x80c]     ; 8b9e0c080000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x43c24                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x43c24                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43c24                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43c24                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43c0b                          ; 7505
        call    0x46188                          ; e87d250000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e892dcffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e120c0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c040c0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43ca2                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43ca2                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x43ca2                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x43ca2                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43ca2                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43ca2                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43c89                          ; 7505
        call    0x46188                          ; e8ff240000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e814dcffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e940b0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c860b0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43d20                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43d20                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x80c]     ; 8b9e0c080000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x43d20                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x43d20                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x80c                       ; 81c10c080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43d20                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43d20                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43d07                          ; 7505
        call    0x46188                          ; e881240000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e896dbffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e160b0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c080b0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43d9e                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43d9e                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x43d9e                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x43d9e                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43d9e                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43d9e                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43d85                          ; 7505
        call    0x46188                          ; e803240000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e818dbffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e980a0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c8a0a0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43e1c                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43e1c                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x608]     ; 8b9e08060000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x43e1c                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x43e1c                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43e1c                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43e1c                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43e03                          ; 7505
        call    0x46188                          ; e885230000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89adaffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e1a0a0000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c0c0a0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43e99                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x43e99                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x43e99                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x43e99                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43e99                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43e99                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43e80                          ; 7505
        call    0x46188                          ; e808230000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e81ddaffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e9d090000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c8f090000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43f17                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43f17                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x608]     ; 8b9e08060000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x43f17                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x43f17                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43f17                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43f17                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43efe                          ; 7505
        call    0x46188                          ; e88a220000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89fd9ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e1f090000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c11090000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43f95                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x43f95                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x43f95                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x43f95                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x43f95                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x43f95                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43f7c                          ; 7505
        call    0x46188                          ; e80c220000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e821d9ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8ea1080000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c93080000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44013                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44013                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x608]     ; 8b9e08060000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x44013                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x44013                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x608                       ; 81c108060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44013                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44013                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43ffa                          ; 7505
        call    0x46188                          ; e88e210000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a3d8ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e23080000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c15080000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44091                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44091                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x44091                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x44091                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44091                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44091                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44078                          ; 7505
        call    0x46188                          ; e810210000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e825d8ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8ea5070000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c97070000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4410f                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x4410f                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x404]     ; 8b9e04040000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x4410f                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x4410f                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4410f                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4410f                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x440f6                          ; 7505
        call    0x46188                          ; e892200000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a7d7ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e27070000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c19070000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4418c                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x4418c                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x4418c                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x4418c                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4418c                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4418c                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44173                          ; 7505
        call    0x46188                          ; e815200000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e82ad7ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8eaa060000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c9c060000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4420a                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x4420a                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x404]     ; 8b9e04040000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x4420a                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x4420a                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4420a                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4420a                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x441f1                          ; 7505
        call    0x46188                          ; e8971f0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8acd6ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e2c060000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c1e060000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44288                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44288                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x44288                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x44288                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44288                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44288                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4426f                          ; 7505
        call    0x46188                          ; e8191f0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e82ed6ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8eae050000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8ca0050000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44306                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44306                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x404]     ; 8b9e04040000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x44306                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x44306                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x404                       ; 81c104040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44306                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44306                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x442ed                          ; 7505
        call    0x46188                          ; e89b1e0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8b0d5ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e30050000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c22050000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44384                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44384                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x44384                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x44384                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44384                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44384                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4436b                          ; 7505
        call    0x46188                          ; e81d1e0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e832d5ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8eb2040000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8ca4040000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44402                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44402                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x200]     ; 8b9e00020000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x44402                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x44402                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44402                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44402                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x443e9                          ; 7505
        call    0x46188                          ; e89f1d0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8b4d4ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e34040000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c26040000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4447f                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x4447f                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x4447f                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x4447f                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4447f                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4447f                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44466                          ; 7505
        call    0x46188                          ; e8221d0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e837d4ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8eb7030000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8ca9030000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x444fd                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x444fd                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x200]     ; 8b9e00020000
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x444fd                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x444fd                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x444fd                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x444fd                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x444e4                          ; 7505
        call    0x46188                          ; e8a41c0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8b9d3ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e39030000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c2b030000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44571                          ; 7d5a
        cmp     ecx, 0                           ; 83f900
        jl      0x44571                          ; 7c55
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x44571                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x44571                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44571                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44571                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44558                          ; 7505
        call    0x46188                          ; e8301c0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e845d3ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8ec5020000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cb7020000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x445ef                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x445ef                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x200]     ; 8b9e00020000
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x445ef                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x445ef                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x200                       ; 81c100020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x445ef                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x445ef                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x445d6                          ; 7505
        call    0x46188                          ; e8b21b0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8c7d2ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e47020000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c39020000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44663                          ; 7d5a
        cmp     ecx, 0                           ; 83f900
        jl      0x44663                          ; 7c55
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x44663                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x44663                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44663                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44663                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4464a                          ; 7505
        call    0x46188                          ; e83e1b0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e853d2ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8ed3010000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cc5010000
        mov     ecx, esi                         ; 8bce
        add     ecx, -4                          ; 83c1fc
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x446d8                          ; 7d5b
        cmp     ecx, 0                           ; 83f900
        jl      0x446d8                          ; 7c56
        mov     ebx, dword ptr [esi - 4]         ; 8b5efc
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x446d8                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x446d8                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, -4                          ; 83c1fc
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x446d8                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x446d8                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x446bf                          ; 7505
        call    0x46188                          ; e8c91a0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8ded1ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8e5e010000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8c50010000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4474a                          ; 7d58
        cmp     ecx, 0                           ; 83f900
        jl      0x4474a                          ; 7c53
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x4474a                          ; 7e49
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x4474a                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4474a                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4474a                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44731                          ; 7505
        call    0x46188                          ; e8571a0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e86cd1ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 0f8eec000000
        dec     edx                              ; 4a
        jl      0x4482f                          ; 0f8cde000000
        mov     ecx, esi                         ; 8bce
        add     ecx, -4                          ; 83c1fc
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x447ba                          ; 7d56
        cmp     ecx, 0                           ; 83f900
        jl      0x447ba                          ; 7c51
        mov     ebx, dword ptr [esi - 4]         ; 8b5efc
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x447ba                          ; 7e46
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x447ba                          ; 7435
        mov     ecx, esi                         ; 8bce
        add     ecx, -4                          ; 83c1fc
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x447ba                          ; 7428
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x447ba                          ; 7420
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x447a5                          ; 7505
        call    0x46188                          ; e8e3190000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8f8d0ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 7e7c
        dec     edx                              ; 4a
        jl      0x4482f                          ; 7c72
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffdfc                  ; 81c1fcfdffff
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4482f                          ; 7d5c
        cmp     ecx, 0                           ; 83f900
        jl      0x4482f                          ; 7c57
        mov     ebx, dword ptr [esi - 0x204]     ; 8b9efcfdffff
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x4482f                          ; 7e49
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x4482f                          ; 7438
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffdfc                  ; 81c1fcfdffff
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4482f                          ; 7428
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4482f                          ; 7420
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4481a                          ; 7505
        call    0x46188                          ; e86e190000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e883d0ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x44836                          ; 7e07
        mov     al, byte ptr [ebp - 1]           ; 8a45ff
        mov     ah, 0                            ; b400
        leave                                    ; c9
        ret                                      ; c3
        mov     al, byte ptr [ebp - 1]           ; 8a45ff
        mov     ah, 1                            ; b401
        leave                                    ; c9
        ret                                      ; c3

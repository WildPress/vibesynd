; FUN_0004483d @ 0004483d  (4482 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004483d.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004483d:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -4                          ; 83c4fc
        mov     byte ptr [ebp - 1], 0            ; c645ff00
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c63110000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa18                       ; 81c1180a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x448c4                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x448c4                          ; 7c5b
        mov     ebx, dword ptr [esi + 0xa18]     ; 8b9e180a0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x448c4                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x448c4                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa18                       ; 81c1180a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x448c4                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x448c4                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x448ab                          ; 7505
        call    0x46188                          ; e8dd180000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8f2cfffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8ef4100000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8ce6100000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44942                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44942                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xa14]     ; 8b9e140a0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x44942                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x44942                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44942                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44942                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44929                          ; 7505
        call    0x46188                          ; e85f180000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e874cfffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e76100000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c68100000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x814                       ; 81c114080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x449c0                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x449c0                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x814]     ; 8b9e14080000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x449c0                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x449c0                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x814                       ; 81c114080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x449c0                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x449c0                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x449a7                          ; 7505
        call    0x46188                          ; e8e1170000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8f6ceffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8ef80f0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8cea0f0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44a3e                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44a3e                          ; 7c5c
        mov     ebx, dword ptr [esi + 0xa14]     ; 8b9e140a0000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x44a3e                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x44a3e                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44a3e                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44a3e                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44a25                          ; 7505
        call    0x46188                          ; e863170000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e878ceffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e7a0f0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c6c0f0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x814                       ; 81c114080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44abc                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44abc                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x814]     ; 8b9e14080000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x44abc                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x44abc                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x814                       ; 81c114080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44abc                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44abc                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44aa3                          ; 7505
        call    0x46188                          ; e8e5160000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8facdffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8efc0e0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8cee0e0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44b3a                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44b3a                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x44b3a                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x44b3a                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44b3a                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44b3a                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44b21                          ; 7505
        call    0x46188                          ; e867160000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e87ccdffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e7e0e0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c700e0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x814                       ; 81c114080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44bb7                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x44bb7                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x814]     ; 8b9e14080000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x44bb7                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x44bb7                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x814                       ; 81c114080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44bb7                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44bb7                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44b9e                          ; 7505
        call    0x46188                          ; e8ea150000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8ffccffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e010e0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8cf30d0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44c35                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44c35                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x44c35                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x44c35                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44c35                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44c35                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44c1c                          ; 7505
        call    0x46188                          ; e86c150000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e881ccffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e830d0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c750d0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x610                       ; 81c110060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44cb3                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44cb3                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x610]     ; 8b9e10060000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x44cb3                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x44cb3                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x610                       ; 81c110060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44cb3                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44cb3                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44c9a                          ; 7505
        call    0x46188                          ; e8ee140000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e803ccffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e050d0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8cf70c0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44d31                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44d31                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x44d31                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x44d31                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44d31                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44d31                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44d18                          ; 7505
        call    0x46188                          ; e870140000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e885cbffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e870c0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c790c0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x610                       ; 81c110060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44daf                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44daf                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x610]     ; 8b9e10060000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x44daf                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x44daf                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x610                       ; 81c110060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44daf                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44daf                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44d96                          ; 7505
        call    0x46188                          ; e8f2130000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e807cbffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e090c0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8cfb0b0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44e2d                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44e2d                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x44e2d                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x44e2d                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44e2d                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44e2d                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44e14                          ; 7505
        call    0x46188                          ; e874130000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e889caffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e8b0b0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c7d0b0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x610                       ; 81c110060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44eaa                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x44eaa                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x610]     ; 8b9e10060000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x44eaa                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x44eaa                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x610                       ; 81c110060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44eaa                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44eaa                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44e91                          ; 7505
        call    0x46188                          ; e8f7120000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e80ccaffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e0e0b0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c000b0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44f28                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44f28                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x44f28                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x44f28                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44f28                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44f28                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44f0f                          ; 7505
        call    0x46188                          ; e879120000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e88ec9ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e900a0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c820a0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x40c                       ; 81c10c040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x44fa6                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x44fa6                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x40c]     ; 8b9e0c040000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x44fa6                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x44fa6                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x40c                       ; 81c10c040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x44fa6                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x44fa6                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x44f8d                          ; 7505
        call    0x46188                          ; e8fb110000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e810c9ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e120a0000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c040a0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45024                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x45024                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x45024                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x45024                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45024                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45024                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4500b                          ; 7505
        call    0x46188                          ; e87d110000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e892c8ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e94090000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c86090000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x40c                       ; 81c10c040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x450a2                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x450a2                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x40c]     ; 8b9e0c040000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x450a2                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x450a2                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x40c                       ; 81c10c040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x450a2                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x450a2                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x45089                          ; 7505
        call    0x46188                          ; e8ff100000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e814c8ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e16090000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c08090000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45120                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x45120                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x45120                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x45120                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45120                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45120                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x45107                          ; 7505
        call    0x46188                          ; e881100000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e896c7ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e98080000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c8a080000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x40c                       ; 81c10c040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4519d                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x4519d                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x40c]     ; 8b9e0c040000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x4519d                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x4519d                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x40c                       ; 81c10c040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4519d                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4519d                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x45184                          ; 7505
        call    0x46188                          ; e804100000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e819c7ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e1b080000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c0d080000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4521b                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x4521b                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x4521b                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x4521b                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4521b                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4521b                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x45202                          ; 7505
        call    0x46188                          ; e8860f0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89bc6ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e9d070000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c8f070000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x208                       ; 81c108020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45299                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x45299                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x208]     ; 8b9e08020000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x45299                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x45299                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x208                       ; 81c108020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45299                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45299                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x45280                          ; 7505
        call    0x46188                          ; e8080f0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e81dc6ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e1f070000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c11070000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45317                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x45317                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x45317                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x45317                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45317                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45317                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x452fe                          ; 7505
        call    0x46188                          ; e88a0e0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89fc5ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8ea1060000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c93060000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x208                       ; 81c108020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45395                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x45395                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x208]     ; 8b9e08020000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x45395                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x45395                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x208                       ; 81c108020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45395                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45395                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4537c                          ; 7505
        call    0x46188                          ; e80c0e0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e821c5ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e23060000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c15060000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45413                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x45413                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x45413                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x45413                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45413                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45413                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x453fa                          ; 7505
        call    0x46188                          ; e88e0d0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a3c4ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8ea5050000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c97050000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x208                       ; 81c108020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45490                          ; 7d60
        cmp     ecx, 0                           ; 83f900
        jl      0x45490                          ; 7c5b
        mov     ebx, dword ptr [esi + 0x208]     ; 8b9e08020000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x45490                          ; 7e4c
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x45490                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x208                       ; 81c108020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45490                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45490                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x45477                          ; 7505
        call    0x46188                          ; e8110d0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e826c4ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e28050000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c1a050000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4550e                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x4550e                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x4550e                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x4550e                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4550e                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4550e                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x454f5                          ; 7505
        call    0x46188                          ; e8930c0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a8c3ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8eaa040000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c9c040000
        mov     ecx, esi                         ; 8bce
        add     ecx, 4                           ; 83c104
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45583                          ; 7d5b
        cmp     ecx, 0                           ; 83f900
        jl      0x45583                          ; 7c56
        mov     ebx, dword ptr [esi + 4]         ; 8b5e04
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x45583                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x45583                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 4                           ; 83c104
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45583                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45583                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4556a                          ; 7505
        call    0x46188                          ; e81e0c0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e833c3ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e35040000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c27040000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45601                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x45601                          ; 7c5c
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x45601                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x45601                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45601                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45601                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x455e8                          ; 7505
        call    0x46188                          ; e8a00b0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8b5c2ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8eb7030000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8ca9030000
        mov     ecx, esi                         ; 8bce
        add     ecx, 4                           ; 83c104
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45676                          ; 7d5b
        cmp     ecx, 0                           ; 83f900
        jl      0x45676                          ; 7c56
        mov     ebx, dword ptr [esi + 4]         ; 8b5e04
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x45676                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x45676                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 4                           ; 83c104
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45676                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45676                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4565d                          ; 7505
        call    0x46188                          ; e82b0b0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e840c2ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e42030000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c34030000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x456ea                          ; 7d5a
        cmp     ecx, 0                           ; 83f900
        jl      0x456ea                          ; 7c55
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x456ea                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x456ea                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x456ea                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x456ea                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x456d1                          ; 7505
        call    0x46188                          ; e8b70a0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8ccc1ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8ece020000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8cc0020000
        mov     ecx, esi                         ; 8bce
        add     ecx, 4                           ; 83c104
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4575e                          ; 7d5a
        cmp     ecx, 0                           ; 83f900
        jl      0x4575e                          ; 7c55
        mov     ebx, dword ptr [esi + 4]         ; 8b5e04
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x4575e                          ; 7e49
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x4575e                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 4                           ; 83c104
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4575e                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4575e                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x45745                          ; 7505
        call    0x46188                          ; e8430a0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e858c1ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e5a020000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c4c020000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x457d2                          ; 7d5a
        cmp     ecx, 0                           ; 83f900
        jl      0x457d2                          ; 7c55
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x457d2                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x457d2                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x457d2                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x457d2                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x457b9                          ; 7505
        call    0x46188                          ; e8cf090000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8e4c0ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8ee6010000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8cd8010000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffe00                  ; 81c100feffff
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x45850                          ; 7d61
        cmp     ecx, 0                           ; 83f900
        jl      0x45850                          ; 7c5c
        mov     ebx, dword ptr [esi - 0x200]     ; 8b9e00feffff
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x45850                          ; 7e4d
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x45850                          ; 743c
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffe00                  ; 81c100feffff
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x45850                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x45850                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x45837                          ; 7505
        call    0x46188                          ; e851090000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e866c0ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8e68010000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8c5a010000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x458c3                          ; 7d59
        cmp     ecx, 0                           ; 83f900
        jl      0x458c3                          ; 7c54
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x458c3                          ; 7e4a
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x458c3                          ; 7439
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x458c3                          ; 742c
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x458c3                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x458aa                          ; 7505
        call    0x46188                          ; e8de080000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8f3bfffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 0f8ef5000000
        dec     edx                              ; 4a
        jl      0x459b1                          ; 0f8ce7000000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffe00                  ; 81c100feffff
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4593c                          ; 7d5c
        cmp     ecx, 0                           ; 83f900
        jl      0x4593c                          ; 7c57
        mov     ebx, dword ptr [esi - 0x200]     ; 8b9e00feffff
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x4593c                          ; 7e49
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x4593c                          ; 7438
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffe00                  ; 81c100feffff
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x4593c                          ; 7428
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x4593c                          ; 7420
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x45927                          ; 7505
        call    0x46188                          ; e861080000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e876bfffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 7e7c
        dec     edx                              ; 4a
        jl      0x459b1                          ; 7c72
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffdfc                  ; 81c1fcfdffff
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x459b1                          ; 7d5c
        cmp     ecx, 0                           ; 83f900
        jl      0x459b1                          ; 7c57
        mov     ebx, dword ptr [esi - 0x204]     ; 8b9efcfdffff
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x459b1                          ; 7e49
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x459b1                          ; 7438
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffdfc                  ; 81c1fcfdffff
        cmp     ecx, dword ptr [0x38952]         ; 3b0d52890300
        je      0x459b1                          ; 7428
        cmp     ecx, dword ptr [0x38956]         ; 3b0d56890300
        je      0x459b1                          ; 7420
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4599c                          ; 7505
        call    0x46188                          ; e8ec070000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e801bfffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x459b8                          ; 7e07
        mov     al, byte ptr [ebp - 1]           ; 8a45ff
        mov     ah, 0                            ; b400
        leave                                    ; c9
        ret                                      ; c3
        mov     al, byte ptr [ebp - 1]           ; 8a45ff
        mov     ah, 1                            ; b401
        leave                                    ; c9
        ret                                      ; c3

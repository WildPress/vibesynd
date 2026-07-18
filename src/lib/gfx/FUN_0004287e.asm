; FUN_0004287e @ 0004287e  (3636 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004287e.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004287e:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        add     esp, -4                          ; 83c4fc
        mov     byte ptr [ebp - 1], 0            ; c645ff00
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c150e0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa18                       ; 81c1180a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x428ed                          ; 7d48
        cmp     ecx, 0                           ; 83f900
        jl      0x428ed                          ; 7c43
        mov     ebx, dword ptr [esi + 0xa18]     ; 8b9e180a0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x428ed                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x428ed                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x428d4                          ; 7505
        call    0x46188                          ; e8b4380000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8c9efffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ebe0d0000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cb00d0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42953                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42953                          ; 7c44
        mov     ebx, dword ptr [esi + 0xa14]     ; 8b9e140a0000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x42953                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x42953                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4293a                          ; 7505
        call    0x46188                          ; e84e380000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e863efffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e580d0000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c4a0d0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x814                       ; 81c114080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x429b9                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x429b9                          ; 7c44
        mov     ebx, dword ptr [esi + 0x814]     ; 8b9e14080000
        movzx   ebx, byte ptr [ebx + 0xb]        ; 0fb65b0b
        cmp     ebx, 4                           ; 83fb04
        jle     0x429b9                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x429b9                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x429a0                          ; 7505
        call    0x46188                          ; e8e8370000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8fdeeffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ef20c0000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8ce40c0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xa14                       ; 81c1140a0000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42a1f                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42a1f                          ; 7c44
        mov     ebx, dword ptr [esi + 0xa14]     ; 8b9e140a0000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x42a1f                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x42a1f                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42a06                          ; 7505
        call    0x46188                          ; e882370000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e897eeffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e8c0c0000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c7e0c0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x814                       ; 81c114080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42a85                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42a85                          ; 7c44
        mov     ebx, dword ptr [esi + 0x814]     ; 8b9e14080000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x42a85                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x42a85                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42a6c                          ; 7505
        call    0x46188                          ; e81c370000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e831eeffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e260c0000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c180c0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42aeb                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42aeb                          ; 7c44
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 0xa]        ; 0fb65b0a
        cmp     ebx, 4                           ; 83fb04
        jle     0x42aeb                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x42aeb                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42ad2                          ; 7505
        call    0x46188                          ; e8b6360000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8cbedffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ec00b0000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cb20b0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x814                       ; 81c114080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42b50                          ; 7d48
        cmp     ecx, 0                           ; 83f900
        jl      0x42b50                          ; 7c43
        mov     ebx, dword ptr [esi + 0x814]     ; 8b9e14080000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x42b50                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x42b50                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42b37                          ; 7505
        call    0x46188                          ; e851360000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e866edffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e5b0b0000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c4d0b0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42bb6                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42bb6                          ; 7c44
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x42bb6                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x42bb6                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42b9d                          ; 7505
        call    0x46188                          ; e8eb350000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e800edffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ef50a0000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8ce70a0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x610                       ; 81c110060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42c1c                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42c1c                          ; 7c44
        mov     ebx, dword ptr [esi + 0x610]     ; 8b9e10060000
        movzx   ebx, byte ptr [ebx + 9]          ; 0fb65b09
        cmp     ebx, 4                           ; 83fb04
        jle     0x42c1c                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x42c1c                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42c03                          ; 7505
        call    0x46188                          ; e885350000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89aecffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e8f0a0000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c810a0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x810                       ; 81c110080000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42c82                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42c82                          ; 7c44
        mov     ebx, dword ptr [esi + 0x810]     ; 8b9e10080000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x42c82                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x42c82                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42c69                          ; 7505
        call    0x46188                          ; e81f350000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e834ecffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e290a0000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c1b0a0000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x610                       ; 81c110060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42ce8                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42ce8                          ; 7c44
        mov     ebx, dword ptr [esi + 0x610]     ; 8b9e10060000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x42ce8                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x42ce8                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42ccf                          ; 7505
        call    0x46188                          ; e8b9340000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8ceebffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ec3090000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cb5090000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42d4e                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42d4e                          ; 7c44
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 8]          ; 0fb65b08
        cmp     ebx, 4                           ; 83fb04
        jle     0x42d4e                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x42d4e                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42d35                          ; 7505
        call    0x46188                          ; e853340000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e868ebffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e5d090000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c4f090000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x610                       ; 81c110060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42db3                          ; 7d48
        cmp     ecx, 0                           ; 83f900
        jl      0x42db3                          ; 7c43
        mov     ebx, dword ptr [esi + 0x610]     ; 8b9e10060000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x42db3                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x42db3                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42d9a                          ; 7505
        call    0x46188                          ; e8ee330000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e803ebffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ef8080000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cea080000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42e19                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42e19                          ; 7c44
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x42e19                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x42e19                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42e00                          ; 7505
        call    0x46188                          ; e888330000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e89deaffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e92080000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c84080000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x40c                       ; 81c10c040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42e7f                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42e7f                          ; 7c44
        mov     ebx, dword ptr [esi + 0x40c]     ; 8b9e0c040000
        movzx   ebx, byte ptr [ebx + 7]          ; 0fb65b07
        cmp     ebx, 4                           ; 83fb04
        jle     0x42e7f                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x42e7f                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42e66                          ; 7505
        call    0x46188                          ; e822330000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e837eaffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e2c080000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c1e080000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x60c                       ; 81c10c060000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42ee5                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42ee5                          ; 7c44
        mov     ebx, dword ptr [esi + 0x60c]     ; 8b9e0c060000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x42ee5                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x42ee5                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42ecc                          ; 7505
        call    0x46188                          ; e8bc320000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8d1e9ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ec6070000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cb8070000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x40c                       ; 81c10c040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42f4b                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42f4b                          ; 7c44
        mov     ebx, dword ptr [esi + 0x40c]     ; 8b9e0c040000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x42f4b                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x42f4b                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42f32                          ; 7505
        call    0x46188                          ; e856320000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e86be9ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e60070000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c52070000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x42fb1                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x42fb1                          ; 7c44
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 6]          ; 0fb65b06
        cmp     ebx, 4                           ; 83fb04
        jle     0x42fb1                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x42fb1                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42f98                          ; 7505
        call    0x46188                          ; e8f0310000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e805e9ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8efa060000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cec060000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x40c                       ; 81c10c040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43016                          ; 7d48
        cmp     ecx, 0                           ; 83f900
        jl      0x43016                          ; 7c43
        mov     ebx, dword ptr [esi + 0x40c]     ; 8b9e0c040000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x43016                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x43016                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x42ffd                          ; 7505
        call    0x46188                          ; e88b310000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a0e8ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e95060000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c87060000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4307c                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x4307c                          ; 7c44
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x4307c                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x4307c                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43063                          ; 7505
        call    0x46188                          ; e825310000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e83ae8ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e2f060000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c21060000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x208                       ; 81c108020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x430e2                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x430e2                          ; 7c44
        mov     ebx, dword ptr [esi + 0x208]     ; 8b9e08020000
        movzx   ebx, byte ptr [ebx + 5]          ; 0fb65b05
        cmp     ebx, 4                           ; 83fb04
        jle     0x430e2                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x430e2                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x430c9                          ; 7505
        call    0x46188                          ; e8bf300000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8d4e7ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ec9050000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cbb050000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x408                       ; 81c108040000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43148                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x43148                          ; 7c44
        mov     ebx, dword ptr [esi + 0x408]     ; 8b9e08040000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x43148                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x43148                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4312f                          ; 7505
        call    0x46188                          ; e859300000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e86ee7ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e63050000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c55050000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x208                       ; 81c108020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x431ae                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x431ae                          ; 7c44
        mov     ebx, dword ptr [esi + 0x208]     ; 8b9e08020000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x431ae                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x431ae                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43195                          ; 7505
        call    0x46188                          ; e8f32f0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e808e7ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8efd040000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cef040000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43214                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x43214                          ; 7c44
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 4]          ; 0fb65b04
        cmp     ebx, 4                           ; 83fb04
        jle     0x43214                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x43214                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x431fb                          ; 7505
        call    0x46188                          ; e88d2f0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8a2e6ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e97040000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c89040000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x208                       ; 81c108020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43279                          ; 7d48
        cmp     ecx, 0                           ; 83f900
        jl      0x43279                          ; 7c43
        mov     ebx, dword ptr [esi + 0x208]     ; 8b9e08020000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x43279                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x43279                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43260                          ; 7505
        call    0x46188                          ; e8282f0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e83de6ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e32040000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c24040000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x432df                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x432df                          ; 7c44
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x432df                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x432df                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x432c6                          ; 7505
        call    0x46188                          ; e8c22e0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8d7e5ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ecc030000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cbe030000
        mov     ecx, esi                         ; 8bce
        add     ecx, 4                           ; 83c104
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x4333f                          ; 7d46
        cmp     ecx, 0                           ; 83f900
        jl      0x4333f                          ; 7c41
        mov     ebx, dword ptr [esi + 4]         ; 8b5e04
        movzx   ebx, byte ptr [ebx + 3]          ; 0fb65b03
        cmp     ebx, 4                           ; 83fb04
        jle     0x4333f                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x4333f                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43326                          ; 7505
        call    0x46188                          ; e8622e0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e877e5ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e6c030000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c5e030000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0x204                       ; 81c104020000
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x433a5                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x433a5                          ; 7c44
        mov     ebx, dword ptr [esi + 0x204]     ; 8b9e04020000
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x433a5                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x433a5                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4338c                          ; 7505
        call    0x46188                          ; e8fc2d0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e811e5ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e06030000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cf8020000
        mov     ecx, esi                         ; 8bce
        add     ecx, 4                           ; 83c104
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43405                          ; 7d46
        cmp     ecx, 0                           ; 83f900
        jl      0x43405                          ; 7c41
        mov     ebx, dword ptr [esi + 4]         ; 8b5e04
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x43405                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x43405                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x433ec                          ; 7505
        call    0x46188                          ; e89c2d0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8b1e4ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ea6020000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c98020000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43464                          ; 7d45
        cmp     ecx, 0                           ; 83f900
        jl      0x43464                          ; 7c40
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx + 2]          ; 0fb65b02
        cmp     ebx, 4                           ; 83fb04
        jle     0x43464                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x43464                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4344b                          ; 7505
        call    0x46188                          ; e83d2d0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e852e4ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e47020000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c39020000
        mov     ecx, esi                         ; 8bce
        add     ecx, 4                           ; 83c104
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x434c3                          ; 7d45
        cmp     ecx, 0                           ; 83f900
        jl      0x434c3                          ; 7c40
        mov     ebx, dword ptr [esi + 4]         ; 8b5e04
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x434c3                          ; 7e34
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx]             ; 8b1b
        cmp     ebx, 0                           ; 83fb00
        je      0x434c3                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x434aa                          ; 7505
        call    0x46188                          ; e8de2c0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8f3e3ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ee8010000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cda010000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43522                          ; 7d45
        cmp     ecx, 0                           ; 83f900
        jl      0x43522                          ; 7c40
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x43522                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x10]      ; 8b5b10
        cmp     ebx, 0                           ; 83fb00
        je      0x43522                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43509                          ; 7505
        call    0x46188                          ; e87f2c0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e894e3ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e89010000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c7b010000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffe00                  ; 81c100feffff
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43588                          ; 7d49
        cmp     ecx, 0                           ; 83f900
        jl      0x43588                          ; 7c44
        mov     ebx, dword ptr [esi - 0x200]     ; 8b9e00feffff
        movzx   ebx, byte ptr [ebx + 1]          ; 0fb65b01
        cmp     ebx, 4                           ; 83fb04
        jle     0x43588                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 8]         ; 8b5b08
        cmp     ebx, 0                           ; 83fb00
        je      0x43588                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4356f                          ; 7505
        call    0x46188                          ; e8192c0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e82ee3ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8e23010000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8c15010000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0                           ; 83c100
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x435e6                          ; 7d44
        cmp     ecx, 0                           ; 83f900
        jl      0x435e6                          ; 7c3f
        mov     ebx, dword ptr [esi]             ; 8b1e
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x435e6                          ; 7e35
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0xc]       ; 8b5b0c
        cmp     ebx, 0                           ; 83fb00
        je      0x435e6                          ; 7424
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x435cd                          ; 7505
        call    0x46188                          ; e8bb2b0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e8d0e2ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 0f8ec5000000
        dec     edx                              ; 4a
        jl      0x436a4                          ; 0f8cb7000000
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffe00                  ; 81c100feffff
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x43647                          ; 7d44
        cmp     ecx, 0                           ; 83f900
        jl      0x43647                          ; 7c3f
        mov     ebx, dword ptr [esi - 0x200]     ; 8b9e00feffff
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x43647                          ; 7e31
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 4]         ; 8b5b04
        cmp     ebx, 0                           ; 83fb00
        je      0x43647                          ; 7420
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x43632                          ; 7505
        call    0x46188                          ; e8562b0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e86be2ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 7e64
        dec     edx                              ; 4a
        jl      0x436a4                          ; 7c5a
        mov     ecx, esi                         ; 8bce
        add     ecx, 0xfffffdfc                  ; 81c1fcfdffff
        sub     ecx, dword ptr [0x5358]          ; 2b0d58530000   0x5358=g_map_cols
        cmp     ecx, 0xc000                      ; 81f900c00000
        jge     0x436a4                          ; 7d44
        cmp     ecx, 0                           ; 83f900
        jl      0x436a4                          ; 7c3f
        mov     ebx, dword ptr [esi - 0x204]     ; 8b9efcfdffff
        movzx   ebx, byte ptr [ebx]              ; 0fb61b
        cmp     ebx, 4                           ; 83fb04
        jle     0x436a4                          ; 7e31
        imul    ebx, ebx, 0x18                   ; 6bdb18
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        mov     ebx, dword ptr [ebx + 0x14]      ; 8b5b14
        cmp     ebx, 0                           ; 83fb00
        je      0x436a4                          ; 7420
        cmp     byte ptr [ebp - 1], 0            ; 807dff00
        jne     0x4368f                          ; 7505
        call    0x46188                          ; e8f92a0000     -> FUN_00046188
        add     byte ptr [ebp - 1], 1            ; 8045ff01
        add     ebx, dword ptr [0x5360]          ; 031d60530000
        call    0x418ac                          ; e80ee2ffff     -> FUN_000418ac
        cmp     cx, 0                            ; 6683f900
        jle     0x436ab                          ; 7e07
        mov     al, byte ptr [ebp - 1]           ; 8a45ff
        mov     ah, 0                            ; b400
        leave                                    ; c9
        ret                                      ; c3
        mov     al, byte ptr [ebp - 1]           ; 8a45ff
        mov     ah, 1                            ; b401
        leave                                    ; c9
        ret                                      ; c3

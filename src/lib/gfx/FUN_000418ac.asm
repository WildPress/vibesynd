; FUN_000418ac @ 000418ac  (408 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_000418ac.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_000418ac:
        mov     edi, 0xe144                      ; bf44e10000
        mov     cx, 0x10                         ; 66b91000
        mov     eax, dword ptr [edi]             ; 8b07
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x418c4                          ; 7408
        mov     eax, dword ptr [ebx]             ; 8b03
        not     eax                              ; f7d0
        or      dword ptr [edi], eax             ; 0907
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0x14]      ; 8b4714
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x418d8                          ; 740a
        mov     eax, dword ptr [ebx + 0x14]      ; 8b4314
        not     eax                              ; f7d0
        or      dword ptr [edi + 0x14], eax      ; 094714
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0x28]      ; 8b4728
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x418ec                          ; 740a
        mov     eax, dword ptr [ebx + 0x28]      ; 8b4328
        not     eax                              ; f7d0
        or      dword ptr [edi + 0x28], eax      ; 094728
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0x3c]      ; 8b473c
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x41900                          ; 740a
        mov     eax, dword ptr [ebx + 0x3c]      ; 8b433c
        not     eax                              ; f7d0
        or      dword ptr [edi + 0x3c], eax      ; 09473c
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0x50]      ; 8b4750
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x41914                          ; 740a
        mov     eax, dword ptr [ebx + 0x50]      ; 8b4350
        not     eax                              ; f7d0
        or      dword ptr [edi + 0x50], eax      ; 094750
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0x64]      ; 8b4764
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x41928                          ; 740a
        mov     eax, dword ptr [ebx + 0x64]      ; 8b4364
        not     eax                              ; f7d0
        or      dword ptr [edi + 0x64], eax      ; 094764
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0x78]      ; 8b4778
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x4193c                          ; 740a
        mov     eax, dword ptr [ebx + 0x78]      ; 8b4378
        not     eax                              ; f7d0
        or      dword ptr [edi + 0x78], eax      ; 094778
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0x8c]      ; 8b878c000000
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x41959                          ; 7410
        mov     eax, dword ptr [ebx + 0x8c]      ; 8b838c000000
        not     eax                              ; f7d0
        or      dword ptr [edi + 0x8c], eax      ; 09878c000000
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0xa0]      ; 8b87a0000000
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x41976                          ; 7410
        mov     eax, dword ptr [ebx + 0xa0]      ; 8b83a0000000
        not     eax                              ; f7d0
        or      dword ptr [edi + 0xa0], eax      ; 0987a0000000
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0xb4]      ; 8b87b4000000
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x41993                          ; 7410
        mov     eax, dword ptr [ebx + 0xb4]      ; 8b83b4000000
        not     eax                              ; f7d0
        or      dword ptr [edi + 0xb4], eax      ; 0987b4000000
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0xc8]      ; 8b87c8000000
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x419b0                          ; 7410
        mov     eax, dword ptr [ebx + 0xc8]      ; 8b83c8000000
        not     eax                              ; f7d0
        or      dword ptr [edi + 0xc8], eax      ; 0987c8000000
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0xdc]      ; 8b87dc000000
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x419cd                          ; 7410
        mov     eax, dword ptr [ebx + 0xdc]      ; 8b83dc000000
        not     eax                              ; f7d0
        or      dword ptr [edi + 0xdc], eax      ; 0987dc000000
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0xf0]      ; 8b87f0000000
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x419ea                          ; 7410
        mov     eax, dword ptr [ebx + 0xf0]      ; 8b83f0000000
        not     eax                              ; f7d0
        or      dword ptr [edi + 0xf0], eax      ; 0987f0000000
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0x104]     ; 8b8704010000
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x41a07                          ; 7410
        mov     eax, dword ptr [ebx + 0x104]     ; 8b8304010000
        not     eax                              ; f7d0
        or      dword ptr [edi + 0x104], eax     ; 098704010000
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0x118]     ; 8b8718010000
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x41a24                          ; 7410
        mov     eax, dword ptr [ebx + 0x118]     ; 8b8318010000
        not     eax                              ; f7d0
        or      dword ptr [edi + 0x118], eax     ; 098718010000
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        mov     eax, dword ptr [edi + 0x12c]     ; 8b872c010000
        xor     eax, 0xffffffff                  ; 83f0ff
        je      0x41a41                          ; 7410
        mov     eax, dword ptr [ebx + 0x12c]     ; 8b832c010000
        not     eax                              ; f7d0
        or      dword ptr [edi + 0x12c], eax     ; 09872c010000
        inc     cx                               ; 6641
        dec     cx                               ; 6649
        ret                                      ; c3

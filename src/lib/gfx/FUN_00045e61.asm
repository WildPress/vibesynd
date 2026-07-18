; FUN_00045e61 @ 00045e61  (151 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00045e61.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00045e61:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    eax                              ; 50
        push    ebx                              ; 53
        push    ecx                              ; 51
        push    edx                              ; 52
        push    esi                              ; 56
        push    edi                              ; 57
        mov     ax, word ptr [ebp + 8]           ; 668b4508
        mov     bx, word ptr [ebp + 0xc]         ; 668b5d0c
        mov     cx, ax                           ; 668bc8
        mov     dx, bx                           ; 668bd3
        sar     ax, 1                            ; 66d1f8
        sar     bx, 1                            ; 66d1fb
        shl     bx, 7                            ; 66c1e307
        mov     si, ax                           ; 668bf0
        add     si, bx                           ; 6603f3
        and     esi, 0xffff                      ; 81e6ffff0000
        shl     esi, 2                           ; c1e602
        and     cx, 1                            ; 6683e101
        and     dx, 1                            ; 6683e201
        mov     bx, dx                           ; 668bda
        add     bx, cx                           ; 6603d9
        mov     eax, 0                           ; b800000000
        cmp     bx, 2                            ; 6683fb02
        jl      0x45ead                          ; 7c03
        add     eax, 8                           ; 83c008
        cmp     cx, dx                           ; 663bca
        jl      0x45eb5                          ; 7c03
        add     eax, 4                           ; 83c004
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x45f6e                          ; 0f84ac000000
        imul    ebx, dword ptr [ebp + 0x18], 0x500 ; 695d1800050000
        mov     dword ptr [ebp + 0x18], ebx      ; 895d18
        shl     dword ptr [ebp + 0x14], 2        ; c1651402
        add     esi, dword ptr [0x5358]          ; 033558530000   0x5358=g_map_cols
        jmp     dword ptr [eax + 0x38794]        ; ffa094870300
        mov     cl, 0x87                         ; b187
        add     eax, dword ptr [eax]             ; 0300
        in      al, dx                           ; ec
        xchg    dword ptr [ebx], eax             ; 8703
        add     byte ptr [edi + eax*4 - 0x7853fffd], ah ; 00a4870300ac87
        add     eax, dword ptr [eax]             ; 0300
        add     esi, 0x200                       ; 81c600020000
        jmp     0x45f34                          ; eb40
        add     esi, 4                           ; 83c604

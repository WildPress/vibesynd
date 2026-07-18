; FUN_0004b073 @ 0004b073  (64 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004b073.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004b073:
        test    byte ptr [0x105], 2              ; f6050501000002
        je      0x4cab7                          ; 0f84371a0000
        and     edx, 3                           ; 83e203
        shl     edx, 3                           ; c1e203
        push    edi                              ; 57
        add     cl, 7                            ; 80c107
        and     cl, 0xf8                         ; 80e1f8
        sar     cl, 3                            ; c0f903
        movzx   edi, cl                          ; 0fb6f9
        dec     edi                              ; 4f
        add     edx, edi                         ; 03d7
        pop     edi                              ; 5f
        jmp     dword ptr [edx*4 + 0x3d956]      ; ff249556d90300
        salc                                     ; d6
        fld     dword ptr [ebx]                  ; d903
        add     byte ptr [eax - 0x27fffc26], ch  ; 00a8da0300d8
        fild    dword ptr [ebx]                  ; db03
        add     cl, bl                           ; 00d9
        fild    dword ptr [ebx]                  ; db03
        add     byte ptr [ebp - 0x41fffc23], bh  ; 00bddd0300be

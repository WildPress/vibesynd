; FUN_0004a909 @ 0004a909  (22 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_0004a909.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_0004a909:
        sar     cl, 3                            ; c0f903
        dec     cl                               ; fec9
        jl      0x4b072                          ; 0f8c5e070000
        movzx   edx, cl                          ; 0fb6d1
        jmp     dword ptr [edx*4 + 0x3d1d6]      ; ff2495d6d10300

; FUN_00046188 @ 00046188  (144 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_00046188.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_00046188:
        mov     eax, 0xe144                      ; b844e10000
        mov     dword ptr [eax], 0               ; c70000000000
        mov     dword ptr [eax + 0x14], 0        ; c7401400000000
        mov     dword ptr [eax + 0x28], 0        ; c7402800000000
        mov     dword ptr [eax + 0x3c], 0        ; c7403c00000000
        mov     dword ptr [eax + 0x50], 0        ; c7405000000000
        mov     dword ptr [eax + 0x64], 0        ; c7406400000000
        mov     dword ptr [eax + 0x78], 0        ; c7407800000000
        mov     dword ptr [eax + 0x8c], 0        ; c7808c00000000000000
        mov     dword ptr [eax + 0xa0], 0        ; c780a000000000000000
        mov     dword ptr [eax + 0xb4], 0        ; c780b400000000000000
        mov     dword ptr [eax + 0xc8], 0        ; c780c800000000000000
        mov     dword ptr [eax + 0xdc], 0        ; c780dc00000000000000
        mov     dword ptr [eax + 0xf0], 0        ; c780f000000000000000
        mov     dword ptr [eax + 0x104], 0       ; c7800401000000000000
        mov     dword ptr [eax + 0x118], 0       ; c7801801000000000000
        mov     dword ptr [eax + 0x12c], 0       ; c7802c01000000000000
        ret                                      ; c3

; FUN_00046188 @ 00046188  (144 bytes) -- hand-written assembly (fully commented).
;
; Clear the running visibility/occlusion mask table at 0xe144: zero the first dword
; of each of its 16 entries. The table has 16 slots on a 0x14-byte (20-byte) stride;
; only the leading dword of each slot -- the accumulated 32-bit mask -- is reset, the
; rest of each slot is left untouched.
;
; This is the per-pass "start with everything clear" step. The scene walkers
; (FUN_00041a44 / FUN_000436b2 / FUN_0004483d) call it once, the first time they find
; a cell to process, then OR each cell's mask into the same table with FUN_000418ac.
; FUN_000418ac reads these leading dwords back on the same 0x14 stride, which is what
; ties the two routines together.
;
; No args. Writes: 0xe144 .. 0xe144 + 15*0x14 (the 16 mask slots). Saves nothing
; (it only touches eax). The build uses FUN_00046188.c (a db-transcription of the raw
; bytes); this .asm is the readable companion. See docs/game-vs-library.md.
;
FUN_00046188:
        mov     eax, 0xe144                      ; eax = base of the 16-slot mask table
        mov     dword ptr [eax], 0               ; slot 0  mask = 0
        mov     dword ptr [eax + 0x14], 0        ; slot 1  (stride 0x14)
        mov     dword ptr [eax + 0x28], 0        ; slot 2
        mov     dword ptr [eax + 0x3c], 0        ; slot 3
        mov     dword ptr [eax + 0x50], 0        ; slot 4
        mov     dword ptr [eax + 0x64], 0        ; slot 5
        mov     dword ptr [eax + 0x78], 0        ; slot 6
        mov     dword ptr [eax + 0x8c], 0        ; slot 7
        mov     dword ptr [eax + 0xa0], 0        ; slot 8
        mov     dword ptr [eax + 0xb4], 0        ; slot 9
        mov     dword ptr [eax + 0xc8], 0        ; slot 10
        mov     dword ptr [eax + 0xdc], 0        ; slot 11
        mov     dword ptr [eax + 0xf0], 0        ; slot 12
        mov     dword ptr [eax + 0x104], 0       ; slot 13
        mov     dword ptr [eax + 0x118], 0       ; slot 14
        mov     dword ptr [eax + 0x12c], 0       ; slot 15
        ret

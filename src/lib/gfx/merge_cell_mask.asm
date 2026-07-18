; merge_cell_mask @ 000418ac  (408 bytes) -- hand-written assembly (fully commented).
;
; OR-merge one cell's visibility mask into the running mask table at 0xe144. The
; table has 16 slots on a 0x14-byte (20-byte) stride; the leading dword of each slot
; is a 32-bit accumulated mask (clear_occlusion_mask zeroes them at the start of a pass).
;
; For each of the 16 slots the routine does:
;       accumulator = [0xe144 + k*0x14]
;       if accumulator == 0xffffffff: skip           ; slot already fully set
;       accumulator |= NOT [ebx + k*0x14]             ; fold in the source, inverted
;
; The source table at ebx uses the same 16-slot / 0x14 stride, so slot k of the
; source lines up with slot k of the accumulator. The loop is fully unrolled 16
; times (one block per slot) -- a classic speed-over-size hand-asm shape.
;
; The `xor eax, 0xffffffff` only provides the "already all-ones" test: it inverts the
; accumulator to set ZF when the original was 0xffffffff, then eax is reloaded from
; the source and the inverted-source value is what actually gets OR'd in. Slots that
; are already all-ones are left alone, saving the read/not/or.
;
; cx counting: cx is loaded with 16 on entry. Every slot runs `dec cx`; a slot that
; actually merges also runs `inc cx` first (the "already all-ones" branch jumps past
; the `inc cx` straight to the `dec cx`). So cx ends as 16 - (slots skipped), i.e. the
; count of slots that were merged. It is a by-product of the unrolled control flow;
; nothing here consumes it.
;
; Args:    ebx = source mask table (16 slots, 0x14 stride)
; Writes:  0xe144 .. 0xe144 + 15*0x14   (the 16 accumulator dwords)
; Saves nothing; callers preserve what they need.
;
; Called from the scene walkers (occlusion_walk / occlusion_walk_excl / occlusion_walk_excl2), once
; per drawable cell. The build uses merge_cell_mask.c (a db-transcription of the raw
; bytes); this .asm is the readable companion. See docs/game-vs-library.md.
;
merge_cell_mask:
        mov     edi, 0xe144                      ; edi = base of the running mask table
        mov     cx, 0x10                         ; cx = 16 (see "cx counting" above)

; --- slot 0 ---
        mov     eax, dword ptr [edi]             ; accumulator for slot 0
        xor     eax, 0xffffffff                  ; ZF=1 iff it was already all-ones
        je      skip0                            ;   -> leave it, jump past the merge + inc
        mov     eax, dword ptr [ebx]             ; source mask, slot 0
        not     eax                              ; invert it
        or      dword ptr [edi], eax             ; fold into the accumulator
        inc     cx                               ; merged: count it back up
skip0:  dec     cx                               ; every slot decrements cx

; --- slot 1 (stride 0x14) ---
        mov     eax, dword ptr [edi + 0x14]
        xor     eax, 0xffffffff
        je      skip1
        mov     eax, dword ptr [ebx + 0x14]
        not     eax
        or      dword ptr [edi + 0x14], eax
        inc     cx
skip1:  dec     cx

; --- slot 2 ---
        mov     eax, dword ptr [edi + 0x28]
        xor     eax, 0xffffffff
        je      skip2
        mov     eax, dword ptr [ebx + 0x28]
        not     eax
        or      dword ptr [edi + 0x28], eax
        inc     cx
skip2:  dec     cx

; --- slot 3 ---
        mov     eax, dword ptr [edi + 0x3c]
        xor     eax, 0xffffffff
        je      skip3
        mov     eax, dword ptr [ebx + 0x3c]
        not     eax
        or      dword ptr [edi + 0x3c], eax
        inc     cx
skip3:  dec     cx

; --- slot 4 ---
        mov     eax, dword ptr [edi + 0x50]
        xor     eax, 0xffffffff
        je      skip4
        mov     eax, dword ptr [ebx + 0x50]
        not     eax
        or      dword ptr [edi + 0x50], eax
        inc     cx
skip4:  dec     cx

; --- slot 5 ---
        mov     eax, dword ptr [edi + 0x64]
        xor     eax, 0xffffffff
        je      skip5
        mov     eax, dword ptr [ebx + 0x64]
        not     eax
        or      dword ptr [edi + 0x64], eax
        inc     cx
skip5:  dec     cx

; --- slot 6 ---
        mov     eax, dword ptr [edi + 0x78]
        xor     eax, 0xffffffff
        je      skip6
        mov     eax, dword ptr [ebx + 0x78]
        not     eax
        or      dword ptr [edi + 0x78], eax
        inc     cx
skip6:  dec     cx

; --- slot 7 ---
        mov     eax, dword ptr [edi + 0x8c]
        xor     eax, 0xffffffff
        je      skip7
        mov     eax, dword ptr [ebx + 0x8c]
        not     eax
        or      dword ptr [edi + 0x8c], eax
        inc     cx
skip7:  dec     cx

; --- slot 8 ---
        mov     eax, dword ptr [edi + 0xa0]
        xor     eax, 0xffffffff
        je      skip8
        mov     eax, dword ptr [ebx + 0xa0]
        not     eax
        or      dword ptr [edi + 0xa0], eax
        inc     cx
skip8:  dec     cx

; --- slot 9 ---
        mov     eax, dword ptr [edi + 0xb4]
        xor     eax, 0xffffffff
        je      skip9
        mov     eax, dword ptr [ebx + 0xb4]
        not     eax
        or      dword ptr [edi + 0xb4], eax
        inc     cx
skip9:  dec     cx

; --- slot 10 ---
        mov     eax, dword ptr [edi + 0xc8]
        xor     eax, 0xffffffff
        je      skip10
        mov     eax, dword ptr [ebx + 0xc8]
        not     eax
        or      dword ptr [edi + 0xc8], eax
        inc     cx
skip10: dec     cx

; --- slot 11 ---
        mov     eax, dword ptr [edi + 0xdc]
        xor     eax, 0xffffffff
        je      skip11
        mov     eax, dword ptr [ebx + 0xdc]
        not     eax
        or      dword ptr [edi + 0xdc], eax
        inc     cx
skip11: dec     cx

; --- slot 12 ---
        mov     eax, dword ptr [edi + 0xf0]
        xor     eax, 0xffffffff
        je      skip12
        mov     eax, dword ptr [ebx + 0xf0]
        not     eax
        or      dword ptr [edi + 0xf0], eax
        inc     cx
skip12: dec     cx

; --- slot 13 ---
        mov     eax, dword ptr [edi + 0x104]
        xor     eax, 0xffffffff
        je      skip13
        mov     eax, dword ptr [ebx + 0x104]
        not     eax
        or      dword ptr [edi + 0x104], eax
        inc     cx
skip13: dec     cx

; --- slot 14 ---
        mov     eax, dword ptr [edi + 0x118]
        xor     eax, 0xffffffff
        je      skip14
        mov     eax, dword ptr [ebx + 0x118]
        not     eax
        or      dword ptr [edi + 0x118], eax
        inc     cx
skip14: dec     cx

; --- slot 15 ---
        mov     eax, dword ptr [edi + 0x12c]
        xor     eax, 0xffffffff
        je      skip15
        mov     eax, dword ptr [ebx + 0x12c]
        not     eax
        or      dword ptr [edi + 0x12c], eax
        inc     cx
skip15: dec     cx
        ret

; FUN_0004d221 @ 0004d221  (305 bytes) -- hand-written assembly (fully commented).
;
; vec_to_angle (atan2): turn a 2-D vector (dx, dy) into a heading expressed as a
; byte angle, 0..255 for a full turn (64 units per quadrant).
;
; The routine picks one of eight octants from the signs of the two components and
; from which one is larger in magnitude, divides the smaller magnitude by the larger
; to get a 0..255 slope index, and reads a sub-angle (0..0x40) out of the arctangent
; table at 0xa95e. It then adds the octant base (0x40 / 0x80 / 0xc0) and negates
; within the octant where the direction runs backwards, so the eight cases together
; cover the whole circle. Both components zero returns 0.
;
; y is negated on entry (screen y grows downward), so the returned angle is measured
; in the usual screen sense. The game reaches this through a wrapper that sign-extends
; two shorts before calling (see manifest/names.json).
;
; Args (stack / cdecl):  [ebp+8] (word) = dx     [ebp+0xc] (word) = dy
; Returns: ax = angle 0..255.   Globals: 0xa95e = arctangent lookup (words, one per
; slope index, each 0..0x40).  The build uses FUN_0004d221.c (db-transcription).
;
FUN_0004d221:
        push    ebp                              ; 55
        mov     ebp, esp                         ; 8bec
        push    ebx                              ; 53
        push    edx                              ; 52
        xor     eax, eax                         ; 33c0
        mov     ax, word ptr [ebp + 8]           ; 668b4508   -- ax = dx
        or      ax, ax                           ; 660bc0
        jne     0x4d241                          ; 7510       -- dx != 0 -> classify (-> load_y)
        mov     bx, word ptr [ebp + 0xc]         ; 668b5d0c   -- dx == 0: bx = dy
        or      bx, bx                           ; 660bdb
        jne     0x4d245                          ; 750b       -- dy != 0 -> classify (-> classify)
        xor     ax, ax                           ; 6633c0     -- dx == dy == 0 -> angle 0
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
load_y:                                          ;            <- (0x4d241)
        mov     bx, word ptr [ebp + 0xc]         ; 668b5d0c   -- bx = dy
classify:                                        ;            <- (0x4d245)
        neg     bx                               ; 66f7db     -- bx = -dy  (flip to screen-up sense)
        xor     dh, dh                           ; 32f6
        or      ax, ax                           ; 660bc0
        js      0x4d2cb                          ; 787c       -- dx < 0 -> left half (-> x_neg)
        or      bx, bx                           ; 660bdb
        js      0x4d290                          ; 783c       -- -dy < 0 (dy > 0) -> lower-right (-> quad_lr)
; --- quadrant: dx >= 0, -dy >= 0  (upper-right) ---
        cmp     ax, bx                           ; 663bc3
        jl      0x4d274                          ; 7c1b       -- |dx| < |dy| -> steep octant (-> oct_ur_steep)
; octant: shallow, base 0x40, direction reversed
        xchg    bx, ax                           ; 6693       -- put larger magnitude in the divisor
        mov     dl, ah                           ; 8ad4       -- build dx_ax:00 as dividend numerator
        mov     ah, al                           ; 8ae0       --   (value << 8) so the quotient is a 0..255 slope
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3     -- ax = (smaller<<8) / larger  = slope index
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000  -- ax = arctan[slope]  (0..0x40)
        add     ax, 0x40                         ; 6683c040   -- + octant base 0x40
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
oct_ur_steep:                                    ;            <- (0x4d274) base 0x80, reversed
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3     -- slope = (dx<<8)/(-dy)
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        neg     ax                               ; 66f7d8     -- run the sub-angle backwards
        add     ax, 0x80                         ; 66058000   -- + octant base 0x80
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
quad_lr:                                         ;            <- (0x4d290) dx >= 0, dy > 0 (lower-right)
        neg     bx                               ; 66f7db     -- bx = |dy|
        cmp     ax, bx                           ; 663bc3
        jl      0x4d2b6                          ; 7c1e       -- |dx| < |dy| -> steep (-> oct_lr_steep)
; octant: shallow, base 0x40
        xchg    bx, ax                           ; 6693
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        neg     ax                               ; 66f7d8
        add     ax, 0x40                         ; 6683c040   -- + octant base 0x40
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
oct_lr_steep:                                    ;            <- (0x4d2b6) base 0x00
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000  -- angle straight from the table (base 0)
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
x_neg:                                           ;            <- (0x4d2cb) dx < 0 (left half)
        neg     ax                               ; 66f7d8     -- ax = |dx|
        or      bx, bx                           ; 660bdb
        js      0x4d30f                          ; 783c       -- -dy < 0 (dy > 0) -> lower-left (-> quad_ll)
; --- quadrant: dx < 0, -dy >= 0  (upper-left) ---
        cmp     ax, bx                           ; 663bc3
        jl      0x4d2f6                          ; 7c1e       -- |dx| < |dy| -> steep (-> oct_ul_steep)
; octant: shallow, base 0xc0, reversed
        xchg    bx, ax                           ; 6693
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        neg     ax                               ; 66f7d8
        add     ax, 0xc0                         ; 6605c000   -- + octant base 0xc0
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
oct_ul_steep:                                    ;            <- (0x4d2f6) base 0x80
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        add     ax, 0x80                         ; 66058000   -- + octant base 0x80
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
quad_ll:                                         ;            <- (0x4d30f) dx < 0, dy > 0 (lower-left)
        neg     bx                               ; 66f7db     -- bx = |dy|
        cmp     ax, bx                           ; 663bc3
        jl      0x4d332                          ; 7c1b       -- |dx| < |dy| -> steep (-> oct_ll_steep)
; octant: shallow, base 0xc0
        xchg    bx, ax                           ; 6693
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        add     ax, 0xc0                         ; 6605c000   -- + octant base 0xc0
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3
oct_ll_steep:                                    ;            <- (0x4d332) base 0x100, wrapped to byte
        mov     dl, ah                           ; 8ad4
        mov     ah, al                           ; 8ae0
        xor     al, al                           ; 32c0
        div     bx                               ; 66f7f3
        mov     ax, word ptr [eax*2 + 0xa95e]    ; 668b04455ea90000
        neg     ax                               ; 66f7d8
        add     ax, 0x100                        ; 66050001   -- + 0x100 then mask -> wraps toward 0
        and     ax, 0xff                         ; 6625ff00   -- keep angle in 0..255
        pop     edx                              ; 5a
        pop     ebx                              ; 5b
        leave                                    ; c9
        ret                                      ; c3

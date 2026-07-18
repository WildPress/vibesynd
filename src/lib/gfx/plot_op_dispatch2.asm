; plot_op_dispatch2 @ 000404b8  (7 bytes) -- hand-written assembly (fully commented).
;
; A bare jump-table dispatcher, the twin of plot_op_dispatch but with its own table at
; 0x32d77. Transfer control to entry edx of a table of 32-bit code pointers; the
; selected handler returns to plot_op_dispatch2's caller. No prologue, no return of its
; own.
;
; Same shape as plot_op_dispatch (the mask-plot operation dispatch used by plot_point):
; a caller loads an operation/case index into edx and falls in here to reach the
; matching handler through one indexed jump.
;
; Args:    edx = table index
; Reads:   0x32d77  base of a jump table of 32-bit code pointers
;
; The build uses plot_op_dispatch2.c (the raw bytes as a db-transcription); this .asm is
; the readable companion. See docs/game-vs-library.md.
;
plot_op_dispatch2:
        jmp     dword ptr [edx*4 + 0x32d77]      ; tail-jump to handler edx (no return here)

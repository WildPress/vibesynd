; FUN_000402e0 @ 000402e0  (7 bytes) -- hand-written assembly (fully commented).
;
; A bare jump-table dispatcher: transfer control to entry edx of a table of code
; pointers based at 0x32b9f. There is no prologue and no return of its own -- it
; jumps straight into the selected handler, which returns to FUN_000402e0's caller.
;
; This is the "apply the chosen op" tail used by plot_point (see plot_point.asm):
; plot_point puts an operation code in edx (set / clear / toggle the mask bit) and
; falls in here so the same point-plot routine can drive several mask operations
; through one indexed jump. edx is the operation index; the four bytes at
; [edx*4 + 0x32b9f] are the handler address.
;
; Args:    edx = table index (operation code)
; Reads:   0x32b9f  base of a jump table of 32-bit code pointers
;
; The build uses FUN_000402e0.c (the raw bytes as a db-transcription); this .asm is
; the readable companion. See docs/game-vs-library.md.
;
FUN_000402e0:
        jmp     dword ptr [edx*4 + 0x32b9f]      ; tail-jump to handler edx (no return here)

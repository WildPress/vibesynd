; blit_width_dispatch @ 0x4a909  (22 bytes) -- hand-written assembly (fully commented).
;
; blit_width_dispatch: width-dispatch stub for a family of unrolled blit routines. It turns
; the sprite width (in pixels, cl) into a width-in-bytes index (cl >> 3), and jumps to
; the matching specialised routine through the jump table at 0x3d1d6. A width of less
; than 8 pixels (index would be 0 after the dec, i.e. negative) exits via 0x4b072, a
; shared return outside this listing.
;
; This is the counterpart of span_op_dispatch's tail: both pick an unrolled loop by sprite
; width so there is no per-column branch in the inner copy. Called by FUN_0004a8d1.
;
; Registers in:  cl = width in pixels (other args -- ax,bx,ch,esi,edi -- set by caller)
; Table:  0x3d1d6  per-width jump table (indexed by width-in-bytes minus 1)
;
blit_width_dispatch:
        sar     cl, 3                            ; cl = width / 8 (bytes across)
        dec     cl                               ; index = widthbytes - 1
        jl      0x4b072                          ; width < 8 px -> nothing to draw (shared exit)
        movzx   edx, cl                          ; zero-extend the table index
        jmp     dword ptr [edx*4 + 0x3d1d6]      ; dispatch to the per-width blit routine

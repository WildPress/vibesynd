; FUN_0004b073 @ 0004b073  (64 bytes) -- hand-written assembly (fully commented).
;
; span_op_dispatch (inferred): choose and jump to a specialized handler for a planar
; drawing operation, indexed by an operation code and by how many bytes the span
; covers. It runs only when render-mode bit 1 (planar) is set; otherwise it jumps
; away to a shared exit at 0x4cab7 in the neighbouring text/draw code.
;
; The dispatch index is built as:
;     edx = (value & 3) * 8                      -- operation code, 4 ops, spaced by 8
;     bytes = ((cl + 7) & ~7) / 8                -- byte span covered (cl treated as a
;                                                   bit width/position, rounded up)
;     edx = edx + (bytes - 1)
; then it jumps through the table at 0x3d956: jmp [edx*4 + 0x3d956]. The handlers this
; selects are elsewhere; this routine only computes the index and dispatches. The exact
; per-op meaning (looks like set/clear/xor/copy variants of a horizontal span) is not
; pinned down from this fragment alone.
;
; Registers in (private convention):  cl = span width/position     edx = value (op in
; the low bits).  Globals: 0x105 render-mode flags; 0x3d956 handler jump table.
; The pragma marks it `aborts` (it never returns normally -- it tail-jumps).
; The build uses FUN_0004b073.c (db-transcription); this is the readable companion.
;
FUN_0004b073:
        test    byte ptr [0x105], 2              ; f6050501000002  -- planar mode?
        je      0x4cab7                          ; 0f84371a0000  -- no -> tail-jump to shared exit (0x4cab7)
        and     edx, 3                           ; 83e203     -- edx = value & 3 (operation code)
        shl     edx, 3                           ; c1e203     -- edx *= 8 (table stride per op)
        push    edi                              ; 57
        add     cl, 7                            ; 80c107     -- round the span up ...
        and     cl, 0xf8                         ; 80e1f8     --   ... to a whole number of bytes
        sar     cl, 3                            ; c0f903     -- cl = byte span = (cl+7)/8
        movzx   edi, cl                          ; 0fb6f9
        dec     edi                              ; 4f         -- edi = byte span - 1
        add     edx, edi                         ; 03d7       -- index = op*8 + (bytes-1)
        pop     edi                              ; 5f
        jmp     dword ptr [edx*4 + 0x3d956]      ; ff249556d90300  -- dispatch to the chosen handler

; --- trailing data (not executed): a little-endian dword pointer table stored after
;     the function body. The bytes below are shown as the disassembler decoded them,
;     but they are actually the dwords 0x0003d9d6, 0x0003daa8, 0x0003dbd8, 0x0003dbd9,
;     0x0003ddbd, followed by one byte 0xbe (handler code addresses in the 0x3d9xx..
;     0x3ddxx range). Control never falls here; the jmp above always leaves.
        salc                                     ; d6
        fld     dword ptr [ebx]                  ; d903
        add     byte ptr [eax - 0x27fffc26], ch  ; 00a8da0300d8
        fild    dword ptr [ebx]                  ; db03
        add     cl, bl                           ; 00d9
        fild    dword ptr [ebx]                  ; db03
        add     byte ptr [ebp - 0x41fffc23], bh  ; 00bddd0300be

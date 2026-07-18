; present_frame_dispatch @ 0x4a574  (52 bytes) -- hand-written assembly (fully commented).
;
; present_frame_dispatch: present-the-frame dispatcher. Picks the copy-to-display routine that
; matches the current render mode:
;   bit 1 (planar/mode-X) -> present_frame_planar, the string-op planar present
;   bit 2 (discard layer)  -> FUN_0004a639, which is an empty stub (does nothing)
;   bit 0 (8bpp)           -> nothing
;
; No args of its own; the callees read the global buffers directly.
; Globals:  0x105  render-mode flags
; Calls:    present_frame_planar (planar present), FUN_0004a639 (no-op)
;
present_frame_dispatch:
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        test    byte ptr [0x105], 2              ; planar mode?
        je      try_bit2
        call    0x4a5a8                          ; present_frame_planar: planar present
        jmp     done
try_bit2:
        test    byte ptr [0x105], 4              ; discard-layer mode?
        je      try_bit0
        call    0x4a639                          ; FUN_0004a639: no-op stub
        jmp     done
try_bit0:
        test    byte ptr [0x105], 1              ; 8bpp mode?
        je      done                             ;   (no present for this mode)
done:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        ret

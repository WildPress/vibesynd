; clear_offscreen @ 0x4993b  (67 bytes) -- hand-written assembly (fully commented).
;
; clear_offscreen: clear a whole planar offscreen buffer. In planar render mode it calls
; fill_bytes(dst, value, 0x1f400). 0x1f400 = 128000 = 4 planes * 0x7d00 (32000), the
; full size of a mode-X offscreen buffer, so this fills every plane in one linear
; sweep. The other render modes do nothing (they fall through the shared mode tail).
;
; Args (stack / cdecl):
;   [ebp+8]    dst   -- buffer base to clear
;   [ebp+0xc]  value -- fill byte (passed as a dword to fill_bytes)
; Globals:  0x105  render-mode flags
; Calls:    fill_bytes @ 0x4d199
;
clear_offscreen:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        test    byte ptr [0x105], 2              ; planar mode?
        je      try_bit2                         ;   no -> check the other modes
        push    0x1f400                          ; count = 128000 (whole 4-plane buffer)
        push    dword ptr [ebp + 0xc]            ; value
        push    dword ptr [ebp + 8]              ; dst
        call    0x4d199                          ; fill_bytes(dst, value, 0x1f400)
        add     esp, 0xc
        jmp     done
try_bit2:
        test    byte ptr [0x105], 4              ; discard-layer mode?
        je      try_bit0
        jmp     done                             ;   yes -> nothing to clear
try_bit0:
        test    byte ptr [0x105], 1              ; 8bpp mode?
        je      done                             ;   (this routine only clears the planar buffer)
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        ret

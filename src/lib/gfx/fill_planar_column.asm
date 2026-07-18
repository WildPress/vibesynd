; fill_planar_column @ 0x49a31  (70 bytes) -- hand-written assembly (fully commented).
;
; fill_planar_column: fill a 16-byte-wide column down a planar buffer. In planar render
; mode it writes the dword `value` into the first four dwords (offsets 0,4,8,0xc) of
; every 0x50 (80) byte row, for 0x640 (1600) rows. 1600 * 0x50 = 0x1f400, the full
; four-plane buffer, so this touches the leftmost 16 bytes of every row across the
; whole buffer -- i.e. it appears to fill a narrow left-hand column band with a
; repeating 4-byte pattern. Other render modes do nothing.
;
; Args (stack / cdecl):
;   [ebp+8]    dst   -- buffer base
;   [ebp+0xc]  value -- 32-bit fill pattern
; Globals:  0x105  render-mode flags
;
fill_planar_column:
        push    ebp
        mov     ebp, esp
        push    eax
        push    ecx
        push    esi
        test    byte ptr [0x105], 2              ; planar mode?
        je      try_bit2                         ;   no -> other modes
        mov     esi, dword ptr [ebp + 8]         ; esi = dst
        mov     eax, dword ptr [ebp + 0xc]       ; eax = fill pattern
        mov     cx, 0x640                        ; 1600 rows
fill_row:
        mov     dword ptr [esi], eax             ; row bytes 0..3
        mov     dword ptr [esi + 4], eax         ; row bytes 4..7
        mov     dword ptr [esi + 8], eax         ; row bytes 8..11
        mov     dword ptr [esi + 0xc], eax       ; row bytes 12..15
        add     esi, 0x50                        ; next row (80-byte pitch)
        dec     cx
        jne     fill_row                         ; loop over all 1600 rows
        jmp     done
try_bit2:
        test    byte ptr [0x105], 4              ; discard-layer mode?
        je      try_bit0
        jmp     done
try_bit0:
        test    byte ptr [0x105], 1              ; 8bpp mode?
        je      done
        pop     esi
        pop     ecx
        pop     eax
        leave
        ret

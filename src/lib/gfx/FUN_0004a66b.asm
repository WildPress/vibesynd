; FUN_0004a66b @ 0x4a66b  (49 bytes) -- hand-written assembly (fully commented).
;
; FUN_0004a66b: draw a sprite from a descriptor into the buffer pointed to by global
; 0x5374. Identical to FUN_0004a63a except for the destination: instead of
; g_screen_buf (0x5368) it targets the buffer at 0x5374 (a separate draw surface).
;
; The descriptor (pointer in arg2) is: +0 = sprite pixel data, +4 = width byte,
; +5 = height byte. Register set-up matches the FUN_0004b073 convention.
;
; Args (stack / cdecl):
;   [ebp+8]    x (word)          [ebp+0xc]  y (word)
;   [ebp+0x10] descriptor ptr    [ebp+0x14] mode/operation selector (-> dx)
; Globals:  0x5374  destination buffer pointer
; Calls:    FUN_0004b073 @ 0x4b073
;
FUN_0004a66b:
        push    ebp
        mov     ebp, esp
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        mov     edi, dword ptr [ebp + 0x10]      ; edi = descriptor ptr
        mov     ax, word ptr [ebp + 8]           ; ax = x
        mov     bx, word ptr [ebp + 0xc]         ; bx = y
        mov     cl, byte ptr [edi + 4]           ; cl = width
        mov     ch, byte ptr [edi + 5]           ; ch = height
        mov     esi, dword ptr [edi]             ; esi = sprite pixel data
        mov     dx, word ptr [ebp + 0x14]        ; dx = mode/operation selector
        mov     edi, dword ptr [0x5374]          ; edi = destination buffer (global 0x5374)
        call    0x4b073                          ; FUN_0004b073: planar masked blit
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        leave
        ret

; FUN_0004a69c @ 0x4a69c  (44 bytes) -- hand-written assembly (fully commented).
;
; FUN_0004a69c: fully-parameterised planar blit. Same worker as FUN_0004a63a/66b
; (FUN_0004b073) but every input is passed explicitly on the stack rather than pulled
; from a descriptor or a global -- the caller supplies the source data pointer and the
; destination buffer directly. The operation selector dx is forced to 0.
;
; Args (stack / cdecl):
;   [ebp+8]    x (word)         [ebp+0xc]  y (word)
;   [ebp+0x10] width (byte)     [ebp+0x14] height (byte)
;   [ebp+0x1c] dest buffer      [ebp+0x20] source pixel data
; Registers passed to FUN_0004b073:
;   ax=x  bx=y  cl=width  ch=height  dx=0  edi=dest  esi=source
; Calls:    FUN_0004b073 @ 0x4b073
;
FUN_0004a69c:
        push    ebp
        mov     ebp, esp
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        mov     ax, word ptr [ebp + 8]           ; ax = x
        mov     bx, word ptr [ebp + 0xc]         ; bx = y
        mov     cl, byte ptr [ebp + 0x10]        ; cl = width
        mov     ch, byte ptr [ebp + 0x14]        ; ch = height
        mov     dx, 0                            ; dx = operation 0
        mov     edi, dword ptr [ebp + 0x1c]      ; edi = destination buffer
        mov     esi, dword ptr [ebp + 0x20]      ; esi = source pixel data
        call    0x4b073                          ; FUN_0004b073: planar masked blit
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        leave
        ret

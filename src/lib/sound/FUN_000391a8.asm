; FUN_000391a8 @ 000391a8  (216 bytes) -- hand-written assembly, reconstructed listing.
; Original bytes disassembled from the game image; call targets and known globals
; resolved to names. The build uses FUN_000391a8.c (db-transcription); this listing is the
; readable companion. See docs/game-vs-library.md for why these are hand asm.
;
FUN_000391a8:
        push    ebx                              ; 53
        push    esi                              ; 56
        mov     si, word ptr [0x5398]            ; 668b3598530000
        mov     ebx, dword ptr [esp + 0xc]       ; 8b5c240c
        movsx   eax, si                          ; 0fbfc6
        lea     eax, [eax*4]                     ; 8d048500000000
        add     eax, ebx                         ; 01d8
        mov     cx, word ptr [0xbde4]            ; 668b0de4bd0000
        cmp     cx, word ptr [eax]               ; 663b08
        jne     0x39276                          ; 0f85a5000000
        mov     al, byte ptr [eax + 2]           ; 8a4002
        cmp     al, 0x53                         ; 3c53
        jb      0x391f6                          ; 721e
        jbe     0x39206                          ; 762c
        cmp     al, 0x6d                         ; 3c6d
        jb      0x391ed                          ; 720f
        jbe     0x39251                          ; 0f866d000000
        cmp     al, 0x73                         ; 3c73
        je      0x39206                          ; 741e
        jmp     0x39270                          ; e983000000
        cmp     al, 0x61                         ; 3c61
        je      0x39232                          ; 7441
        jmp     0x39270                          ; e97a000000
        cmp     al, 0x41                         ; 3c41
        jb      0x39270                          ; 0f8272000000
        jbe     0x39232                          ; 7632
        cmp     al, 0x4d                         ; 3c4d
        je      0x39251                          ; 744d
        jmp     0x39270                          ; eb6a
        cmp     byte ptr [0x10b4a], 0            ; 803d4a0b010000 0x10b4a=g_sound_enabled
        je      0x39270                          ; 7461
        movsx   eax, si                          ; 0fbfc6
        lea     eax, [eax*4]                     ; 8d048500000000
        add     eax, ebx                         ; 01d8
        mov     al, byte ptr [eax + 3]           ; 8a4003
        and     eax, 0xff                        ; 25ff000000
        push    eax                              ; 50
        mov     word ptr [0x5398], si            ; 66893598530000
        call    0x35f78                          ; e848cdffff     -> FUN_00035f78
        jmp     0x39266                          ; eb34
        movsx   eax, si                          ; 0fbfc6
        lea     eax, [eax*4]                     ; 8d048500000000
        add     eax, ebx                         ; 01d8
        mov     al, byte ptr [eax + 3]           ; 8a4003
        and     eax, 0xff                        ; 25ff000000
        mov     dword ptr [0xbdf8], eax          ; a3f8bd0000
        inc     esi                              ; 46
        jmp     0x391b5                          ; e964ffffff
        movsx   eax, si                          ; 0fbfc6
        movsx   eax, byte ptr [ebx + eax*4 + 3]  ; 0fbe448303
        push    eax                              ; 50
        mov     word ptr [0x5398], si            ; 66893598530000
        call    0x38fe8                          ; e882fdffff     -> sound_channel_select
        mov     si, word ptr [0x5398]            ; 668b3598530000
        add     esp, 4                           ; 83c404
        inc     esi                              ; 46
        jmp     0x391b5                          ; e93fffffff
        mov     word ptr [0x5398], si            ; 66893598530000
        pop     esi                              ; 5e
        pop     ebx                              ; 5b
        ret                                      ; c3

; FUN_000391a8 @ 000391a8  (216 bytes) -- hand-written assembly (fully commented).
;
; FUN_000391a8: walk a table of 4-byte command records and execute each one, for as
; long as the record's tag matches the current tag g_bde4. Looks like the driver's
; little command/parameter interpreter: it reads a persistent cursor, steps through
; consecutive records that belong to the current tag, dispatches each by a one-letter
; command code, and stops at the first record whose tag no longer matches.
;
; Args (stack, cdecl-ish):  [esp+0xc] = base of the record table (after the two
;                           entry pushes; = [esp+4] on entry).
; Cursor:  0x5398  g_5398 -- signed 16-bit index into the table, persisted across calls.
;
; Record layout (4 bytes):  word[0] = tag (matched against g_bde4)
;                           byte[2] = command code, an ASCII letter
;                           byte[3] = parameter
;
; Commands (case-insensitive):
;   'S'/'s' : if g_sound_enabled (0x10b4a) is set, call FUN_00035f78(param)
;   'a'/'A' : store param into g_bdf8 (0xbdf8)
;   'm'/'M' : call sound_channel_select(signed param)  (0x38fe8)
;   anything else : skip the record
; After handling (or skipping) a record the cursor advances and the loop repeats.
;
; Globals:  0x5398  g_5398 (record cursor)     0xbde4  g_bde4 (tag to match)
;           0xbdf8  g_bdf8 ('a' target)        0x10b4a g_sound_enabled
; Calls:    0x35f78 FUN_00035f78 ('S')         0x38fe8 sound_channel_select ('m')
;
FUN_000391a8:
        push    ebx                              ; 53   save ebx (holds table base)
        push    esi                              ; 56   save esi (holds cursor)
        mov     si, word ptr [0x5398]            ; 668b3598530000  si = g_5398 (cursor)
        mov     ebx, dword ptr [esp + 0xc]       ; 8b5c240c        ebx = record table base (arg)
; .next_record (0x391b5): test the record at cursor, run it while its tag matches
        movsx   eax, si                          ; 0fbfc6          eax = sign-extend cursor
        lea     eax, [eax*4]                     ; 8d048500000000  eax = cursor*4 (record size 4)
        add     eax, ebx                         ; 01d8            eax -> current record
        mov     cx, word ptr [0xbde4]            ; 668b0de4bd0000  cx = g_bde4 (tag to match)
        cmp     cx, word ptr [eax]               ; 663b08          record.tag == g_bde4 ?
        jne     0x39276                          ; 0f85a5000000    no  -> .done
        mov     al, byte ptr [eax + 2]           ; 8a4002          al = record.cmd (ASCII code)
; --- dispatch on the command letter (binary search over the character) ---
        cmp     al, 0x53                         ; 3c53            'S' ?
        jb      0x391f6                          ; 721e            al < 'S'  -> .test_low ('A'/'M')
        jbe     0x39206                          ; 762c            al == 'S' -> .cmd_S
        cmp     al, 0x6d                         ; 3c6d            'm' ?
        jb      0x391ed                          ; 720f            'S' < al < 'm' -> .test_a
        jbe     0x39251                          ; 0f866d000000    al == 'm' -> .cmd_m
        cmp     al, 0x73                          ; 3c73            's' ?
        je      0x39206                          ; 741e            al == 's' -> .cmd_S
        jmp     0x39270                          ; e983000000      other -> .advance (skip)
; .test_a (0x391ed): 'S' < al < 'm'
        cmp     al, 0x61                          ; 3c61            'a' ?
        je      0x39232                          ; 7441            al == 'a' -> .cmd_a
        jmp     0x39270                          ; e97a000000      other -> .advance (skip)
; .test_low (0x391f6): al < 'S'
        cmp     al, 0x41                          ; 3c41            'A' ?
        jb      0x39270                          ; 0f8272000000    al < 'A'  -> .advance (skip)
        jbe     0x39232                          ; 7632            al == 'A' -> .cmd_a
        cmp     al, 0x4d                          ; 3c4d            'M' ?
        je      0x39251                          ; 744d            al == 'M' -> .cmd_m
        jmp     0x39270                          ; eb6a            other -> .advance (skip)
; .cmd_S (0x39206): 'S'/'s' -- gated sound command
        cmp     byte ptr [0x10b4a], 0            ; 803d4a0b010000  g_sound_enabled set? (0x10b4a)
        je      0x39270                          ; 7461            no -> .advance (nothing pushed yet)
        movsx   eax, si                          ; 0fbfc6          recompute record address
        lea     eax, [eax*4]                     ; 8d048500000000
        add     eax, ebx                         ; 01d8
        mov     al, byte ptr [eax + 3]           ; 8a4003          al = record.param
        and     eax, 0xff                        ; 25ff000000      zero-extend param
        push    eax                              ; 50              arg = param
        mov     word ptr [0x5398], si            ; 66893598530000  publish cursor before the call
        call    0x35f78                          ; e848cdffff     -> FUN_00035f78(param)
        jmp     0x39266                          ; eb34            -> .cmd_cleanup (drop arg, advance)
; .cmd_a (0x39232): 'a'/'A' -- store param into g_bdf8
        movsx   eax, si                          ; 0fbfc6
        lea     eax, [eax*4]                     ; 8d048500000000
        add     eax, ebx                         ; 01d8            eax -> record
        mov     al, byte ptr [eax + 3]           ; 8a4003          al = record.param
        and     eax, 0xff                        ; 25ff000000      zero-extend
        mov     dword ptr [0xbdf8], eax          ; a3f8bd0000      g_bdf8 = param
        inc     esi                              ; 46              advance cursor
        jmp     0x391b5                          ; e964ffffff     -> .next_record
; .cmd_m (0x39251): 'm'/'M' -- select channel with a signed param
        movsx   eax, si                          ; 0fbfc6
        movsx   eax, byte ptr [ebx + eax*4 + 3]  ; 0fbe448303      eax = sign-extend record.param
        push    eax                              ; 50              arg = signed param
        mov     word ptr [0x5398], si            ; 66893598530000  publish cursor before the call
        call    0x38fe8                          ; e882fdffff     -> sound_channel_select(param)
; .cmd_cleanup (0x39266): shared tail for 'S' and 'm' (both pushed one arg)
        mov     si, word ptr [0x5398]            ; 668b3598530000  reload cursor (callee may have moved it)
        add     esp, 4                           ; 83c404          drop the pushed arg
; .advance (0x39270): step to the next record and loop
        inc     esi                              ; 46              cursor++
        jmp     0x391b5                          ; e93fffffff     -> .next_record
; .done (0x39276): first non-matching tag ends the run
        mov     word ptr [0x5398], si            ; 66893598530000  save cursor back to g_5398
        pop     esi                              ; 5e
        pop     ebx                              ; 5b
        ret                                      ; c3

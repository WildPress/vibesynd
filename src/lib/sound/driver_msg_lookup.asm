; driver_msg_lookup @ 00039280  (44 bytes) -- hand-written assembly (fully commented).
;
; driver_msg_lookup: the driver's message-dispatch table lookup. Given a driver slot in
; ebx and a message id in eax, find the {id,handler} entry for that message in the
; slot's driver record and return its handler address (0 if the slot is empty or the
; message is not handled). sound_dispatch_trampoline is the usual caller: it loads
; ebx from the stack, sets eax to the message id, calls here, and jmps to the result.
;
; Args (registers, private convention):
;   ebx = driver slot index (0..15)     eax = message id to look up
; Returns:
;   eax = handler address, or 0 if slot empty / message not found
;
; Driver record (g_driver_ptr[slot] at 0xbcfa): an array of 8-byte entries
;   dword[0] = message id      dword[4] = handler address
; terminated by an entry whose id == -1 (0xffffffff).
;
; Globals:  0xbcfa  g_driver_ptr[16] -- per-slot driver-record pointers
;
driver_msg_lookup:
        cmp     ebx, 0x10                        ; 83fb10          slot in range 0..15 ?
        jae     0x392a2                          ; 731d            no -> .not_found (return 0)
        shl     ebx, 1                           ; d1e3            ebx *= 2
        shl     ebx, 1                           ; d1e3            ebx *= 2  (now slot*4, dword index)
        mov     ebx, dword ptr [ebx + 0xbcfa]    ; 8b9bfabc0000    ebx = g_driver_ptr[slot]
        cmp     ebx, 0                            ; 83fb00         slot registered ?
        je      0x392a2                          ; 740e            no -> .not_found (return 0)
; .scan (0x39294): walk the {id,handler} entries until id matches or terminator
        mov     ecx, dword ptr [ebx]             ; 8b0b            ecx = entry.id
        cmp     ecx, eax                          ; 3bc8           entry.id == requested id ?
        je      0x392a8                          ; 740e            yes -> .found
        add     ebx, 8                            ; 83c308         advance to next entry
        cmp     ecx, -1                           ; 83f9ff         was that the -1 terminator ?
        jne     0x39294                          ; 75f2            no -> keep scanning (.scan)
; .not_found (0x392a2): empty slot, bad slot, or exhausted table
        mov     eax, 0                            ; b800000000     return 0
        ret                                       ; c3
; .found (0x392a8):
        mov     eax, dword ptr [ebx + 4]          ; 8b4304         eax = entry.handler
        ret                                       ; c3

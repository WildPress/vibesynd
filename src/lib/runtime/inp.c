/* C runtime: inp @ 0x3b2aa (CLIB3S, Watcom 9.5). Read a byte from an I/O port:
   framed (push ebp; mov ebp,esp), loads the port from [ebp+8] into EDX, zeroes
   EAX (sub eax,eax) and `in al,dx`. Sibling of outp 0x3b22d. Whole-function
   db-transcription into a frameless wrapper (the memset 0x3aaf8 pattern): the
   body carries its own ebp frame, the trailing RET is supplied by the wrapper. */
extern unsigned char __inp(void);
#pragma aux __inp = "db 85" "db 137" "db 229" "db 139" "db 85" "db 8" "db 41" "db 192" "db 236" "db 93" parm [] value [al] modify exact [eax ecx edx];
unsigned char inp(unsigned port)
{
    return __inp();
}

/* C runtime helper @ 0x3b1d3 (CLIB3S region). Writes the NUL-terminated string
   pointed to by EAX to a DOS file/handle: opens the fixed path at ds:0x2d8d2
   (DOS int 21h AH=0x3D, AL=1 open-for-write), strlen's the string via LODSB, then
   int 21h AH=0x40 write. The function has NO ret -- it falls straight through into
   _exit (0x3b1f6): a "write message then terminate" abort path. Whole-function
   db-transcription into a frameless wrapper; `aborts` suppresses the wrapper's
   trailing RET so the fall-through is preserved. */
extern void __dbgwrite(void);
#pragma aux __dbgwrite = "db 82" "db 80" "db 186" "db 210" "db 216" "db 2" "db 0" "db 102" "db 184" "db 1" "db 61" "db 205" "db 33" "db 102" "db 139" "db 216" "db 90" "db 139" "db 242" "db 252" "db 172" "db 60" "db 0" "db 117" "db 251" "db 139" "db 206" "db 43" "db 202" "db 73" "db 180" "db 64" "db 205" "db 33" "db 88" parm [] modify exact [eax ecx edx] aborts;
void FUN_0003b1d3(void);
#pragma aux FUN_0003b1d3 aborts;
void FUN_0003b1d3(void)
{
    __dbgwrite();
}

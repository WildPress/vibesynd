# Function map

Semantic names + descriptions over the `FUN_<addr>` anchors (source filename stays `FUN_<addr>`; this is the readable layer). Full machine-readable map in `manifest/names.json`.

## Intro path (boot -> Bullfrog logo -> main menu), 132 functions that execute

| addr | name | subsystem | what it does |
|---|---|---|---|
| `0xd928` | ***(unnamed)*** |  |  |
| `0xe568` | ***(unnamed)*** |  |  |
| `0x17998` | ***(unnamed)*** |  |  |
| `0x179f8` | ***(unnamed)*** |  |  |
| `0x17b48` | ***(unnamed)*** |  |  |
| `0x180f8` | ***(unnamed)*** |  |  |
| `0x18158` | ***(unnamed)*** |  |  |
| `0x18338` | ***(unnamed)*** |  |  |
| `0x18458` | ***(unnamed)*** |  |  |
| `0x184b8` | ***(unnamed)*** |  |  |
| `0x18828` | ***(unnamed)*** |  |  |
| `0x18878` | ***(unnamed)*** | unclassified | Decomp target: stack-calling forwarding wrapper. original @ 0x00018878 (14 bytes) * * 8b 54 24 04 mov edx,[esp |
| `0x188a8` | ***(unnamed)*** | unclassified | forward three args to a callee (g uses ebx as scratch). |
| `0x18958` | ***(unnamed)*** |  |  |
| `0x20c88` | ***(unnamed)*** |  |  |
| `0x20fc8` | ***(unnamed)*** |  |  |
| `0x24b08` | ***(unnamed)*** |  |  |
| `0x24be8` | ***(unnamed)*** |  |  |
| `0x25238` | ***(unnamed)*** |  |  |
| `0x25338` | ***(unnamed)*** |  |  |
| `0x253a8` | ***(unnamed)*** |  |  |
| `0x254a8` | ***(unnamed)*** |  |  |
| `0x28b88` | ***(unnamed)*** |  |  |
| `0x34c28` | ***(unnamed)*** |  |  |
| `0x34d48` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x35d08` | ***(unnamed)*** |  |  |
| `0x35e68` | ***(unnamed)*** |  |  |
| `0x38c28` | ***(unnamed)*** | unclassified | guarded alloc/read helper @ 0x38c28. * * NEAR-MISS (NOT matched). Register-ROLE wall: the target's ESI/EDI ass |
| `0x38cf8` | ***(unnamed)*** |  |  |
| `0x39280` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x392ac` | ***(unnamed)*** |  |  |
| `0x39393` | ***(unnamed)*** |  |  |
| `0x393e0` | ***(unnamed)*** |  |  |
| `0x39467` | ***(unnamed)*** |  |  |
| `0x39495` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x394c6` | ***(unnamed)*** |  |  |
| `0x3954c` | ***(unnamed)*** |  |  |
| `0x39625` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x39747` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x397f1` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x39846` | ***(unnamed)*** |  |  |
| `0x398d7` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x39994` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x399b3` | ***(unnamed)*** | lib | dispatch stub @ 000399b3 (10B): mov eax,0x65; jmp sound_dispatch_trampoline (tail call) |
| `0x399bd` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x39af1` | ***(unnamed)*** | lib | dispatch stub @ 00039af1 (10B): mov eax,0x85; jmp sound_dispatch_trampoline (tail call) |
| `0x39b55` | ***(unnamed)*** | lib | dispatch stub @ 00039b55 (10B): mov eax,0x96; jmp sound_dispatch_trampoline (tail call) |
| `0x39b5f` | ***(unnamed)*** | lib | dispatch stub @ 00039b5f (10B): mov eax,0x97; jmp sound_dispatch_trampoline (tail call) |
| `0x39b73` | ***(unnamed)*** | lib | dispatch stub @ 00039b73 (10B): mov eax,0x99; jmp sound_dispatch_trampoline (tail call) |
| `0x39b7d` | ***(unnamed)*** | lib | dispatch stub @ 00039b7d (10B): mov eax,0x9a; jmp sound_dispatch_trampoline (tail call) |
| `0x39b87` | ***(unnamed)*** | lib | dispatch stub @ 00039b87 (10B): mov eax,0x9b; jmp sound_dispatch_trampoline (tail call) |
| `0x39b91` | ***(unnamed)*** | lib | dispatch stub @ 00039b91 (10B): mov eax,0x9c; jmp sound_dispatch_trampoline (tail call) |
| `0x3a1ec` | ***(unnamed)*** | lib | GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Top-level graphics decompression driver @ 0x3a1ec. TRUE SIZE 3 |
| `0x3a37a` | ***(unnamed)*** | lib | GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Big-endian dword reader @ 0x3a37a. Frameless 9-byte leaf: LODS |
| `0x3a383` | ***(unnamed)*** | lib | GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Huffman symbol lookup @ 0x3a383. Non-leaf (calls the bit-reade |
| `0x3a3c6` | ***(unnamed)*** | lib | GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Bitstream bit-extractor @ 0x3a3c6. Leaf. Reads CL (=AL param)  |
| `0x3a449` | ***(unnamed)*** | lib | GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Huffman decode-table builder @ 0x3a449. Non-leaf (calls the bi |
| `0x3a526` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3a579` | ***(unnamed)*** | lib | C runtime: open @ 0x3a579 (CLIB3S). Variadic wrapper: va_arg the mode, then call the real open (path, oflag, 0 |
| `0x3a598` | ***(unnamed)*** | lib | C runtime: open/sopen core @ 0x3a598 (CLIB3S). Skips leading spaces in the name, masks the share/access flags  |
| `0x3a7c4` | ***(unnamed)*** | lib | C runtime: read (text/binary) @ 0x3a7c4 (CLIB3S). Handle lookup (0x3c529), then a raw binary DOS read (AH=0x3f |
| `0x3a89d` | ***(unnamed)*** | lib | C runtime: close @ 0x3a89d (CLIB3S, Watcom 9.5). DOS close (AH=0x3E, BX=handle). int 21h; fold carry into the  |
| `0x3a900` | ***(unnamed)*** | lib | GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Word-wise string append @ 0x3a900. Custom frame (PUSH ESI/EDI/ |
| `0x3a93b` | ***(unnamed)*** | lib | C runtime: lseek @ 0x3a93b (CLIB3S). DOS seek (AH=0x42), 64-bit->32-bit result combine, error path calls the D |
| `0x3a97c` | ***(unnamed)*** | lib | C runtime: tell @ 0x3a97c (CLIB3S). DOS seek-cur (AH=0x42, method 1) to read the file position; 64-bit->32-bit |
| `0x3a9c8` | ***(unnamed)*** | lib | C runtime: strcmp @ 0x3a9c8 (CLIB3S). 386 hand-asm dword-at-a-time compare with the 0xfefefeff/0x80808080 zero |
| `0x3aa74` | ***(unnamed)*** | lib | framed 1-arg forwarder @ 0x3aa74: push [ebp+8]; call 0x3aa84 |
| `0x3aa84` | ***(unnamed)*** | lib | C runtime: malloc / _nmalloc core @ 0x3aa84 (CLIB3S, small model). Saves ES/FS/GS, reads DS as the near select |
| `0x3aaf8` | ***(unnamed)*** | lib | C runtime: memset @ 0x3aaf8 (CLIB3S, Watcom 9.5). Hand-asm, FRAMELESS, stack-calling. Loads dst/c/len from [es |
| `0x3ab1a` | ***(unnamed)*** | lib | C runtime: filelength @ 0x3ab1a (CLIB3S). Saves current pos (lseek SEEK_CUR), seeks to end (SEEK_END) for the  |
| `0x3ab59` | ***(unnamed)*** | lib | framed 1-arg forwarder @ 0x3ab59: push [ebp+8]; call 0x3ab69 |
| `0x3ab69` | ***(unnamed)*** | lib | GAME hand-asm (NOT CLIB -- 0% RTL fingerprint). Thin wrapper @ 0x3ab69 that forwards its 3rd stack arg ([EBP+0 |
| `0x3adb2` | ***(unnamed)*** |  |  |
| `0x3aea6` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3aef9` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3af38` | ***(unnamed)*** | lib | C runtime: system @ 0x3af38 (CLIB3S, Watcom 9.5). Runs a command through the shell. Looks up the COMSPEC env s |
| `0x3b239` | ***(unnamed)*** | lib | C runtime: d_getvec @ 0x3b239 (CLIB3S). DOS get-interrupt-vector (AH=0x35, with a DOS-version gate on the 0xc2 |
| `0x3b273` | ***(unnamed)*** | lib | C runtime: d_setvec @ 0x3b273 (CLIB3S). DOS set-interrupt-vector (AH=0x25) with the DOS-version gate. Entire b |
| `0x3b3b9` | ***(unnamed)*** |  |  |
| `0x3b3e6` | ***(unnamed)*** | lib | C runtime: int386x @ 0x3b3e6 (CLIB3S, Watcom 9.5). Register-setup wrapper for a software interrupt: saves ebx/ |
| `0x3b407` | ***(unnamed)*** | lib | framed field-mask+call @ 0x3b407: p=[ebp+8]; p[0xc]&=0xcf; g(p,0,0) |
| `0x3b420` | ***(unnamed)*** | lib | C runtime: fread @ 0x3b420 (CLIB3S). size*count buffered read (_filbuf/qread core); regs-first prologue -> ful |
| `0x3b594` | ***(unnamed)*** | lib | C runtime: fseek @ 0x3b594 (CLIB3S). buffered seek via lseek/tell/ftell/flush; regs-first prologue -> full db- |
| `0x3b739` | ***(unnamed)*** | lib | C runtime: fopen mode-string parser @ 0x3b739 (CLIB3S). Decodes r/w/a/+/b/t to stream flag bits (tolower, 0xc2 |
| `0x3b7e8` | ***(unnamed)*** | lib | C runtime: _fopen core @ 0x3b7e8 (CLIB3S). Parse mode, open/create via 0x3a598, append-truncate via fseek, ini |
| `0x3b8cd` | ***(unnamed)*** | lib | framed call-then-cond-call @ 0x3b8cd: p=g1(0); if(p) p=g2(a,b,c,p); return p; |
| `0x3b8f8` | ***(unnamed)*** | lib | framed forwarder @ 0x3b8f8: push 0; push [ebp+c]; push [ebp+8]; call 0x3b8cd |
| `0x3b99e` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3b9ca` | ***(unnamed)*** | lib | C runtime: @ 0x3b9ca (CLIB3S). r=FUN_3ba80(a,b); FUN_3dae1(a); return r; regs-first prologue -> full db-transc |
| `0x3ba80` | ***(unnamed)*** | lib | C runtime: stream teardown / _fclose core @ 0x3ba80 (CLIB3S, Watcom 9.5). FILE* at [ebp+0x10], flag [ebp+0x14] |
| `0x3c44d` | ***(unnamed)*** | lib | C runtime: isatty @ 0x3c44d (CLIB3S). DOS IOCTL 0x4400 get-device-info, test the char-device bit -> 0/1. int21 |
| `0x3c46d` | ***(unnamed)*** | lib | Decomp target #1 - original @ 0x0003c46d (6 bytes) * * Original machine code: B8 3C 1E 01 00 C3 * Disassembly: |
| `0x3c491` | ***(unnamed)*** | lib | @ 0x3c491 -- framed CLIB helper: if(flag) p = FUN_0003c4b9(p); return p. |
| `0x3c529` | ***(unnamed)*** | lib | C runtime: fd->stream lookup @ 0x3c529 (CLIB3S, Watcom 9.5). Returns the stream table entry ds:0xc07c[fd*4] af |
| `0x3c57b` | ***(unnamed)*** | lib | C runtime: handle-release helper @ 0x3c57b (CLIB3S, Watcom 9.5). Stores the handle (arg1) OR'd with 0x40 in it |
| `0x3c594` | ***(unnamed)*** |  |  |
| `0x3c63f` | ***(unnamed)*** |  |  |
| `0x3c772` | ***(unnamed)*** | lib | C runtime: DPMI/DOS paragraph allocator @ 0x3c772 (CLIB3S, Watcom 9.5). Framed (push ebx/esi/edi/ebp; mov ebp, |
| `0x3c816` | ***(unnamed)*** | lib | C runtime: heap-grow / sbrk-style allocator core @ 0x3c816 (CLIB3S, Watcom 9.5). Rounds the requested size (ar |
| `0x3ca18` | ***(unnamed)*** | lib | C runtime: byte-fill core @ 0x3ca18 (CLIB3S, Watcom 9.5). The byte-granular front of memset: if ECX!=0, aligns |
| `0x3ca4f` | ***(unnamed)*** | lib | C runtime: aligned dword-fill core @ 0x3ca4f (CLIB3S, Watcom 9.5). The inner loop of memset: fills ECX dwords  |
| `0x3cacb` | ***(unnamed)*** | lib | C runtime: __STK stack-check thunk @ 0x3cacb (CLIB3S, Watcom 9.5). Preserves the caller's EAX by XCHG'ing it w |
| `0x3cade` | ***(unnamed)*** | lib | C runtime: __STK stack-limit check core @ 0x3cade (CLIB3S, Watcom 9.5). Given a proposed new stack top in EAX: |
| `0x3cba4` | ***(unnamed)*** | lib | C runtime: getenv @ 0x3cba4 (CLIB3S, Watcom 9.5). Framed (push ebx/esi/edi/ebp; mov ebp,esp). Walks the enviro |
| `0x3cbf9` | ***(unnamed)*** | lib | C runtime: switch-char/cmd-prefix helper @ 0x3cbf9 (CLIB3S; heuristic mislabeled spawnve). If arg3!=0 store '/ |
| `0x3cc45` | ***(unnamed)*** | lib | framed two-call @ 0x3cc45: g1(b, e, c, 0); g2(a, b, c, d, e); params a=+8 b=+c c=+10 d=+14 e=+18 |
| `0x3cc74` | ***(unnamed)*** | lib | C runtime: system() / spawn-command @ 0x3cc74 (CLIB3S, Watcom 9.5). Large framed hand-asm: stack-probe prologu |
| `0x3cfce` | ***(unnamed)*** | lib | framed forwarder @ 0x3cfce: lea eax,[ebp+10]; push eax; push [ebp+c]; push [ebp+8]; call 0x3e48e |
| `0x3d40f` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3d4bc` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3d4f3` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3d894` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3d90b` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3d935` | ***(unnamed)*** | lib | C runtime: qread @ 0x3d935 (CLIB3S). DOS read (AH=0x3f); error path calls the DOS-error handler. Full body via |
| `0x3da03` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3da37` | ***(unnamed)*** | lib | C runtime: tolower @ 0x3da37 (CLIB3S). -d2 forces the leaf frame. |
| `0x3da4c` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3dae1` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3db36` | ***(unnamed)*** | lib | C runtime: chktty @ 0x3db36 (CLIB3S). If the tty-checked flag (0x20) is unset and the fd is a tty (isatty), se |
| `0x3dc1b` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3deee` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3dfcf` | ***(unnamed)*** | lib | C runtime helper @ 0x3dfcf (CLIB3S): string copy incl NUL (heuristic mislabeled 'cenvarg'). |
| `0x3dfe8` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3e143` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3e1af` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3e27a` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3e2bb` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3e361` | ***(unnamed)*** | lib | C runtime helper @ 0x3e361 (CLIB3S): path-separator normalize (mislabeled makepath). |
| `0x3e381` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x3e48e` | ***(unnamed)*** | lib | framed forwarder @ 0x3e48e: push [c2f4]; push [ebp+10]; push [ebp+c]; push [ebp+8]; call 0x3e6ab |
| `0x3e6ab` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |
| `0x4d199` | ***(unnamed)*** | lib | -- db-transcription (hand-asm/library). |

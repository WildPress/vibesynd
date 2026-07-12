/* switch dispatcher @ 0x18d18.  TRUE extent 0x18d18-0x1930b = ~1523B (0x5f3);
   manifest size 89 covers ONLY the clamp+dispatch prologue up to the jump table.

   WALL (playbook Section 0: inline jump-table / switch = not auto-verifiable).
   The dispatch is
       JMP dword ptr CS:[EBX*0x4 + 0xb5ac]
   i.e. a 7-entry jump table living in a FAR (CS-relative) data segment at
   absolute 0xb5ac, with the shipped function's on-disk .text left clean.  When
   WE compile the equivalent switch, Watcom 9.5b co-locates the jump table (plus
   entry-alignment padding) in the SAME object .text BEFORE the code, so
   match_reloc compares (table+code) against the target's code-only window and the
   lengths can NEVER be equal -- even if every code byte is identical.  No C
   spelling moves the table out of the compiled object.  Effectively-matchable
   code that we cannot auto-verify; documented, NOT marked matched.

   Decoded structure (disassembly-authoritative):
     signature ~ FUN_00018d18(int p1@EAX, int p2@EDI, short n@EBX, char p4@DL)
     clamp:  if (n <= 1) n = 2;  else if (n > 8) n = 8;   n -= 2;
             if ((unsigned short)n > 6) goto end;         // default -> return
             switch (n) { case 0..6 }
     Each case k (original height n = k+2, k = 0..6) emits (k+2) unrolled calls to
     fn_3f636(a, b, c, d) -- a tile/row draw helper -- with the a/b column pair and
     c row index stepping by 1 per call:
       case @0x18d81 (n=2): 2 calls   case @0x18f33 (n=5): 5 calls
       case @0x18de4 (n=3): 3 calls   case @0x19019 (n=6): 6 calls
       case @0x18e76 (n=4): 4 calls   case @0x1912c (n=7): 7 calls
                                      case @0x1926f (n=8): 8 calls
     shared tail @0x19304: add esp,0x64; pop ebp/edi/esi/ebx; ret.

   No source form is attempted here because the jump-table co-location wall makes a
   masked byte match structurally impossible to confirm under match_reloc; the
   above is the ground-truth reconstruction for the record. */

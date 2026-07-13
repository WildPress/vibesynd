/* FUN_00039ca0 -- SKIP-ASM-IDIOM (music/SFX file player main loop, 418B).
 * disasm: push ebp; mov ebp,esp; push eax;ebx;ecx;edx;edi;esi   ; save ALL regs
 *   (caller-saves-all convention -- the whole 0x39xxx driver relies on this).
 *   g_bdf2=1; g_537e=0x80; g_5398=0; g_bdf4=[ebp+0xc];
 *   loop g_bdf2 frames:
 *     FUN_0003a598(0xbdfc, 0x200, 0x40) -> open handle g_bdd0; if <=0 break;
 *     INT 21h / AH=2Ch : read DOS time -> seed g_bdec;
 *     FUN_0003a7c4 reads chunk headers; per tag 0xaf12 -> FUN_00039ee2,
 *       0xf1fa -> advance + FUN_000391a8([ebp+0x10]) + FUN_00039e42(), then
 *       blit: rep movsd 0x3e80 dwords g_5368 -> 0xa0000 (VGA), FUN_0003a19a;
 *     INT 10h / AX=1200h video calls; loop until end / g_bdf2 clears;
 *     FUN_0003a89d(g_bdd0) close;
 * INT 21h/INT 10h, REP MOVSD to video memory, the all-register entry save and
 * direct screen blit are hand asm; not Watcom C. Parked. */

/* dosint_win.c -- Windows adapter for the DOS/BIOS int emulator (core: dosint_core.c).
 *
 * The Windows equivalent of dosint.c. Where POSIX traps the game's raw int/port-I/O instructions
 * with a SIGSEGV/SIGILL handler, Windows uses a Vectored Exception Handler. The instructions the
 * game issues surface as three exception codes:
 *   - int 0x21 / 0x10 / 0x16 / 0x33 (CD nn)     -> STATUS_ACCESS_VIOLATION
 *   - in / out / cli / sti / mov Sreg (ring 0)  -> STATUS_PRIVILEGED_INSTRUCTION
 *   - a bad jump into unmapped code             -> STATUS_ACCESS_VIOLATION (fault addr == EIP)
 * All route to the same portable syn_dos_trap(), which inspects the bytes at EIP and services or
 * skips the instruction, then we resume with the updated context.
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "dosint_core.h"

/* marshal the Win32 CONTEXT <-> our portable RG_* file */
static void ctx_to_rg(CONTEXT *c, long *r) {
    r[RG_EAX] = c->Eax; r[RG_EBX] = c->Ebx; r[RG_ECX] = c->Ecx; r[RG_EDX] = c->Edx;
    r[RG_ESI] = c->Esi; r[RG_EDI] = c->Edi; r[RG_EBP] = c->Ebp; r[RG_ESP] = c->Esp;
    r[RG_EIP] = c->Eip; r[RG_EFL] = c->EFlags;
}
static void rg_to_ctx(long *r, CONTEXT *c) {
    c->Eax = r[RG_EAX]; c->Ebx = r[RG_EBX]; c->Ecx = r[RG_ECX]; c->Edx = r[RG_EDX];
    c->Esi = r[RG_ESI]; c->Edi = r[RG_EDI]; c->Ebp = r[RG_EBP]; c->Esp = r[RG_ESP];
    c->Eip = r[RG_EIP]; c->EFlags = r[RG_EFL];
}

static LONG CALLBACK on_veh(EXCEPTION_POINTERS *ep) {
    DWORD code = ep->ExceptionRecord->ExceptionCode;
    const char *name;
    int have_fault = 0;
    unsigned fault = 0;
    long r[RG_N];

    switch (code) {
    case EXCEPTION_ACCESS_VIOLATION:
        name = "ACCESS_VIOLATION";
        /* ExceptionInformation[1] is the referenced address (used for bad-jump detection) */
        have_fault = 1;
        fault = (unsigned)ep->ExceptionRecord->ExceptionInformation[1];
        break;
    case EXCEPTION_PRIV_INSTRUCTION:   name = "PRIV_INSTRUCTION";    break;
    case EXCEPTION_ILLEGAL_INSTRUCTION: name = "ILLEGAL_INSTRUCTION"; break;
    default:
        return EXCEPTION_CONTINUE_SEARCH;   /* not ours -- let the OS handle it */
    }

    ctx_to_rg(ep->ContextRecord, r);
    syn_dos_trap(r, fault, have_fault, name);   /* services or _exit()s */
    rg_to_ctx(r, ep->ContextRecord);
    return EXCEPTION_CONTINUE_EXECUTION;
}

void dosint_install(void) {
    /* back the VGA memory window (0xa0000..0xc0000) so the game's direct-to-VGA writes land in
     * real memory instead of faulting. 0xa0000 is 64KB-aligned (allocation granularity). */
    void *vga = VirtualAlloc((void *)0xa0000, 0x20000, MEM_RESERVE | MEM_COMMIT,
                             PAGE_EXECUTE_READWRITE);
    if (vga != (void *)0xa0000)
        fprintf(stderr, "[dosint] warning: VGA window at 0xa0000 not mapped (got %p); "
                        "direct-to-VGA writes will fault\n", vga);
    /* first (TRUE) so we see the fault before the OS default handler */
    AddVectoredExceptionHandler(1, on_veh);
}

/* dosint_core.h -- platform-independent core of the DOS/BIOS int emulator.
 *
 * The trap logic (int21/int10/int16/int33, DPMI, port I/O, privileged-instruction skips) works
 * on a flat register array indexed by the RG_* constants below, so it is identical on every OS.
 * A thin per-platform adapter (dosint.c = POSIX signals, dosint_win.c = Windows VEH) marshals the
 * native fault context into that array, calls syn_dos_trap(), and marshals the result back before
 * resuming the faulting thread.
 */
#ifndef DOSINT_CORE_H
#define DOSINT_CORE_H

/* our own register indices -- deliberately NOT glibc's REG_* (ucontext.h), so the core never
 * depends on a platform context layout. The adapters copy named registers in and out. */
enum { RG_EAX, RG_EBX, RG_ECX, RG_EDX, RG_ESI, RG_EDI, RG_EBP, RG_ESP, RG_EIP, RG_EFL, RG_N };

/* Service one fault. `r` is the RG_N-entry register file (updated in place: on a handled trap
 * RG_EIP is advanced past the instruction). `fault_addr` is the accessed address for a memory
 * fault (have_fault_addr != 0); it lets the core detect a bad instruction-fetch without touching
 * the faulting page. `signame` labels the fatal report. Handled traps return; unrecoverable ones
 * print a diagnosis and _exit(), so this can return void. */
void syn_dos_trap(long *r, unsigned fault_addr, int have_fault_addr, const char *signame);

/* captured VGA DAC palette (256 * r,g,b, 6-bit) */
void dosint_get_dac(unsigned char *out768);

#endif

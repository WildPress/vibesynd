/* shims_mem.c -- native replacements for the game's DOS/DPMI memory allocators.
 * The game is -4s (stack calling), so these are plain cdecl. */
#include <stdlib.h>

/* os_getmem(size) -- DPMI/DOS block alloc, page-rounded. Return a native block. */
void *shim_os_getmem(int size) {
    if (size <= 0) size = 0x1000;
    size = (size + 0xfff) & ~0xfff;
    return malloc((size_t)size);
}

/* dpmi_dos_alloc_paras(paras) -- allocate `paras` * 16 bytes of "DOS" memory. */
void *shim_dpmi_dos_alloc_paras(int paras) {
    return malloc((size_t)(paras > 0 ? paras : 1) * 16);
}

/* brk / heap_grow -- the CLIB heap extends via DOS; back it with malloc-per-call. */
void *shim_brk(void *addr) { (void)addr; return (void *)-1; }
void *shim_heap_grow(int size) { return malloc((size_t)(size > 0 ? size : 0x10000)); }

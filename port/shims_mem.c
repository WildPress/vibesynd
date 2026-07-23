/* shims_mem.c -- native replacements for the game's DOS/DPMI memory allocators.
 * The game is -4s (stack calling), so these are plain cdecl.
 *
 * The game loads code overlays (gamedg.dll / gamefm.dll) into allocated buffers and CALLS
 * into them, so allocations must be EXECUTABLE. We back every allocation with an RWX mmap
 * (a 16-byte header stores the size for free()).
 */
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
static void *xalloc(size_t sz) {
    size_t total = sz + 16;
    void *m = VirtualAlloc(NULL, total, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!m) return NULL;
    *(size_t *)m = total;
    return (char *)m + 16;
}
static void xfree(void *p) {
    if (p) { char *m = (char *)p - 16; VirtualFree(m, 0, MEM_RELEASE); }
}
#else
#include <sys/mman.h>
static void *xalloc(size_t sz) {
    size_t total = sz + 16;
    void *m = mmap(NULL, total, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (m == MAP_FAILED) return NULL;
    *(size_t *)m = total;
    return (char *)m + 16;
}
static void xfree(void *p) {
    if (p) { char *m = (char *)p - 16; munmap(m, *(size_t *)m); }
}
#endif

/* os_getmem(size) -- DPMI/DOS block alloc, page-rounded. */
void *shim_os_getmem(int size) {
    if (size <= 0) size = 0x1000;
    return xalloc((size_t)((size + 0xfff) & ~0xfff));
}
void *shim_dpmi_dos_alloc_paras(int paras) {
    return xalloc((size_t)(paras > 0 ? paras : 1) * 16);
}
void *shim_brk(void *addr) { (void)addr; return (void *)-1; }
void *shim_heap_grow(int size) { return xalloc((size_t)(size > 0 ? size : 0x10000)); }

/* the malloc family -> the RWX allocator (bypasses the DOS/4GW heap) */
void *shim_malloc(unsigned size)  { return xalloc(size ? size : 1); }
void *shim_nmalloc(unsigned size) { return xalloc(size ? size : 1); }
void  shim_free(void *p)          { xfree(p); }
void  shim_nfree(void *p)         { xfree(p); }

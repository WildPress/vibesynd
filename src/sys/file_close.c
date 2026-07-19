/* Decomp target: stack-calling forwarding wrapper.  original @ 0x00018878 (14 bytes)
 *
 *   8b 54 24 04   mov edx,[esp+4]     ; arg
 *   52            push edx
 *   e8 <rel32>    call 0x0003a89d     ; <-- relocation (masked when diffing)
 *   83 c4 04      add esp,4           ; caller cleanup (cdecl/stack calling)
 *   c3            ret
 *
 * Frameless, stack calling (-5s). Forwards its arg to another function.
 * First non-leaf match -> exercises relocation-aware diffing.
 */
extern void close(int);

void file_close(int a)
{
    close(a);
}

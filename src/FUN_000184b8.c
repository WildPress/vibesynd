/* @ 0x000184b8 (803B): (re)allocate a memory-block descriptor.
 *
 * param_1 = descriptor `d` (pointer). Layout used:
 *   d[0x00]  byte  tag; '*' (0x2a) => block already exists (resize), else fresh.
 *   d[0x1c]  int*  slot holding the block's linear address (*slot).
 *   d[0x20]  int*  optional slot for the block end address.
 *   d[0x24]  int   block size (bytes).
 *   d[0x28]  byte  flags; bit0 => use conventional/DOS memory (DPMI int 0x31).
 *   d[0x2a]  u16   DPMI selector for the DOS block.
 *
 * DPMI is driven through FUN_0003adb2(0x31,&in,&out) (union REGS = 7 ints each):
 *   in.eax(msg[0]) 0x101 free / 0x100 alloc DOS mem; in.ebx(msg[1]) paragraphs;
 *   in.edx(msg[3] low word) selector; out.eax(scratch[0]) err/seg; out.edx(scratch[3]) selector.
 *
 * Returns 1 ok, 0 open failed, -1/-2/-3 DPMI errors (8 / 7 / 0x8011) or alloc failure.
 * Sibling: FUN_000183e8 (same DPMI free idiom).
 */
extern void FUN_0003adb2(int a, void *in, void *out);
extern void FUN_00018488(int a, int b);
extern int  FUN_00018458(int a);
extern int  FUN_00018828(void *a, int b);
extern int  FUN_00018958(void *a);
extern void FUN_00018878(int a);
extern int  FUN_000188a8(int a, int b, int c);
extern void FUN_0003a1ec(int a, int b);

int FUN_000184b8(unsigned char *p)
{
    int scratch[7];
    int msg[7];
    int addr;
    int err;
    unsigned int a;
    int h;

    if (p[0] == 0x2a) {
        if (p[0x28] & 1) {
            /* resize existing DOS block */
            if (**(int **)(p + 0x1c) != 0) {
                msg[0] = 0x101;
                *(unsigned short *)((char *)msg + 0xc) = *(unsigned short *)(p + 0x2a);
                FUN_0003adb2(0x31, msg, scratch);
                **(int **)(p + 0x1c) = 0;
            }
            msg[0] = 0x100;
            msg[1] = (int)((*(unsigned int *)(p + 0x24) + 0x10) >> 4);
            FUN_0003adb2(0x31, msg, scratch);
            err = scratch[0];
            if (err == 8) {
ret_m1:
                return -1;
            }
            if (err == 7) return -2;
            if (err == 0x8011) return -3;
            addr = err << 4;
            **(int **)(p + 0x1c) = addr;
            *(unsigned short *)(p + 0x2a) = (unsigned short)scratch[3];
            if (*(int **)(p + 0x20) != 0)
                **(int **)(p + 0x20) = *(int *)(p + 0x24) + addr;
            for (a = addr; a < *(int *)(p + 0x24) + addr; a++)
                *(char *)a = 0;
        } else {
            /* resize existing normal block */
            if (**(int **)(p + 0x1c) != 0) {
                FUN_00018488(**(int **)(p + 0x1c), *(int *)(p + 0x24));
                **(int **)(p + 0x1c) = 0;
            }
            addr = FUN_00018458(*(int *)(p + 0x24));
            if (addr == 0) goto ret_m1;
            **(int **)(p + 0x1c) = addr;
            if (*(int **)(p + 0x20) != 0)
                **(int **)(p + 0x20) = addr + *(int *)(p + 0x24);
            for (a = addr; a < addr + *(int *)(p + 0x24); a++)
                *(char *)a = 0;
        }
        return 1;
    }

    /* fresh block: open the source */
    h = FUN_00018828(p, 0x200);
    if (h == -1) goto ret0;

    if (p[0x28] & 1) {
        /* fresh DOS block */
        if (**(int **)(p + 0x1c) != 0) {
            msg[0] = 0x101;
            *(unsigned short *)((char *)msg + 0xc) = *(unsigned short *)(p + 0x2a);
            FUN_0003adb2(0x31, msg, scratch);
            **(int **)(p + 0x1c) = 0;
        }
        *(int *)(p + 0x24) = FUN_00018958(p);
        msg[0] = 0x100;
        msg[1] = (int)((*(unsigned int *)(p + 0x24) + 0x110) >> 4);
        FUN_0003adb2(0x31, msg, scratch);
        err = scratch[0];
        if (err == 8) return -1;
        if (err == 7) return -2;
        if (err == 0x8011) return -3;
        addr = err << 4;
        **(int **)(p + 0x1c) = addr;
        *(unsigned short *)(p + 0x2a) = (unsigned short)scratch[3];
        if (*(int **)(p + 0x20) != 0)
            **(int **)(p + 0x20) = *(int *)(p + 0x24) + addr;
        for (a = addr; a < *(int *)(p + 0x24) + addr; a++)
            *(char *)a = 0;
    } else {
        /* fresh normal block */
        if (**(int **)(p + 0x1c) != 0) {
            FUN_00018488(**(int **)(p + 0x1c), *(int *)(p + 0x24));
            **(int **)(p + 0x1c) = 0;
        }
        *(int *)(p + 0x24) = FUN_00018958(p);
        addr = FUN_00018458(*(int *)(p + 0x24));
        if (addr != 0) {
            **(int **)(p + 0x1c) = addr;
            if (*(int **)(p + 0x20) != 0)
                **(int **)(p + 0x20) = addr + *(int *)(p + 0x24);
        } else {
            FUN_00018878(h);
            return -1;
        }
    }

    if (FUN_000188a8(h, addr, *(int *)(p + 0x24)) != *(int *)(p + 0x24)) {
        FUN_00018878(h);
        FUN_0003a1ec(**(int **)(p + 0x1c), **(int **)(p + 0x1c));
        return 1;
    }
    FUN_00018878(h);
    return 1;

ret0:
    return 0;
}

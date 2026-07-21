/* dgroup.s -- the DGROUP data model for the native all-asm build.
 * The code blob references data as __obj<N> + offset. This lays the initialised DGROUP image
 * (OBJECT2 | zero | OBJECT4 | zero, from tools/port_data.py) and defines the per-object bases
 * at their DGROUP offsets (cumulative LE object vsizes: obj2@0, obj3@0x13e60, obj4@0x14a60).
 */
    .data
    .globl __dgroup
__dgroup:
    .incbin "port/gen/dgroup.bin"

    .globl __obj2
    .set __obj2, __dgroup + 0x0
    .globl __obj3
    .set __obj3, __dgroup + 0x13e60
    .globl __obj4
    .set __obj4, __dgroup + 0x14a60

    .section .note.GNU-stack,"",@progbits

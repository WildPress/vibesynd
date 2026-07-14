#!/usr/bin/env python3
"""linearize.py -- reconstruct the TRUE runtime image of object1 from the LE data pages
of inputs/SYNDICAT_MAIN.EXE, and emit the code prefix our decomp is missing.

WHY: SYNDICAT_MAIN_OBJECT1.linear.bin (the whole decomp's basis) was extracted 0x28b8 bytes
(= e_lfanew, the MZ stub size) INTO object1 -- its first 0x28b8 bytes of code are absent, and
every manifest address is uniformly (true_obj1_offset + 0x10000 - 0x28b8). Functions in that
prefix (e.g. FUN_0000d928 @ manifest 0xd928 = obj1 offset 0x1e0) are called by the startup
(FUN_00024be8: `CALL 0x0000d928`) but have no source, so buildgame.py stubs them to `ret` and
the game inits then exits with NO render. This tool recovers them.

VERIFIED: the LE data pages are stored SEQUENTIALLY from Data-Pages-Offset (header+0x80); the
reconstructed obj1[0x28b8:] matches the current linear.bin at 2493/2493 sampled bytes, and
__x386_start (eb 78 "WATCOM C/C++32") lands at obj1 offset 0x2d85c (runtime 0x3d85c) as the LE
header's entry point requires.

Coordinates:
  obj1 file offset F  ->  runtime  = 0x10000 + F
                          manifest = 0x10000 + F - 0x28b8   (what Ghidra/our src use)
  prefix = obj1[0 : 0x28b8]  ->  manifest [0xd748, 0x10000)   (the missing region)

Outputs (build/):
  obj1_full.bin    real object1 image, runtime base 0x10000, vsize 0x3fdf4
  obj1_prefix.bin  obj1[0:0x28b8]  -> load as a Ghidra memory block at MANIFEST 0xd748
"""
import struct, os, sys

EXE = "inputs/SYNDICAT_MAIN.EXE"
STUB = 0x28b8            # e_lfanew: manifest = runtime - 0x28b8; prefix length
OBJ1_VSIZE = 0x3fdf4


def reconstruct_obj1():
    d = open(EXE, "rb").read()
    H = struct.unpack_from("<I", d, 0x3c)[0]
    assert d[H:H+2] in (b"LE", b"LX"), "no LE/LX header"
    u32 = lambda o: struct.unpack_from("<I", d, H+o)[0]
    page_size = u32(0x28)
    objtab = u32(0x40) + H
    # object1: pages pidx..pidx+pcnt-1
    vsize, base, flags, pidx, pcnt = struct.unpack_from("<IIIII", d, objtab)
    data_pages_off = u32(0x80)          # from START of file; pages stored sequentially
    start = data_pages_off + (pidx - 1) * page_size
    img = bytearray(d[start:start + pcnt * page_size])
    return img[:OBJ1_VSIZE], base, vsize


def main():
    img, base, vsize = reconstruct_obj1()
    os.makedirs("build", exist_ok=True)
    open("build/obj1_full.bin", "wb").write(img)
    open("build/obj1_prefix.bin", "wb").write(img[:STUB])
    print("obj1 runtime base=0x%x vsize=0x%x  -> build/obj1_full.bin (0x%x bytes)"
          % (base, vsize, len(img)))
    print("prefix obj1[0:0x%x] -> build/obj1_prefix.bin  (Ghidra block @ manifest 0x%x)"
          % (STUB, 0x10000 - STUB))
    # sanity: __x386_start at obj1 offset 0x2d85c
    hx = lambda o, n=8: " ".join("%02x" % x for x in img[o:o+n])
    print("check __x386_start @obj1:0x2d85c :", hx(0x2d85c), "(expect eb 78 57 41 ..)")
    print("check FUN_0000d928  @obj1:0x1e0  :", hx(0x1e0))


if __name__ == "__main__":
    main()

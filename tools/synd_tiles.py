#!/usr/bin/env python3
"""Decode and render Syndicate map tiles (HBLK) to a PNG contact sheet.

Format (per freesynd tilemanager): a tile is 64x48, built from 6 subtiles of
32x16. The file starts with a 256-entry index (6 little-endian u32 subtile offsets
each, 24 bytes per tile). A subtile is planar: each 32-pixel row is 20 bytes laid
out as 4 mask bytes then 4 bytes per colour plane (4 planes -> 4-bit index 0..15),
mask bit set = transparent. Subtile rows and tile subrows are stored bottom-up.

  python3 tools/synd_tiles.py <HBLK.DAT> <PAL> <out.png>
"""
import os, struct, sys
import rnc
import synd_sprites as sp

TW, TH, SW, SH, HDR = 64, 48, 32, 16, 6144


def decode_tile(hblk, tid):
    t = [255] * (TW * TH)
    for i in range(2):
        for j in range(3):
            off = struct.unpack_from("<I", hblk, tid * 24 + (i * 3 + j) * 4)[0]
            if off < HDR or off + 20 * SH > len(hblk):
                continue
            p = off
            dest = (2 - j) * SH * TW + i * SW
            for row in range(SH):
                rb = hblk[p:p + 20]; p += 20
                outrow = dest + (SH - 1 - row) * TW
                for blk in range(4):
                    mask = rb[blk]
                    for px in range(8):
                        b = 7 - px
                        if not ((mask >> b) & 1):
                            v = ((rb[4 + blk] >> b) & 1) | (((rb[8 + blk] >> b) & 1) << 1) \
                                | (((rb[12 + blk] >> b) & 1) << 2) | (((rb[16 + blk] >> b) & 1) << 3)
                            t[outrow + blk * 8 + px] = v
    return t


def main():
    hblk_path, pal_path, out = sys.argv[1:4]
    d = open(hblk_path, "rb").read()
    hblk = rnc.unpack(d) if d[:3] == b"RNC" else d
    pal = sp.load_palette(pal_path)
    cols, rows, pad = 16, 16, 2
    W, H = cols * (TW + pad), rows * (TH + pad)
    bg = [(30, 30, 36)] * (W * H)
    for tid in range(256):
        t = decode_tile(hblk, tid)
        cx, cy = (tid % cols) * (TW + pad), (tid // cols) * (TH + pad)
        for y in range(TH):
            for x in range(TW):
                v = t[(TH - 1 - y) * TW + x]        # flip: tile data is bottom-up
                if v != 255:
                    bg[(cy + y) * W + (cx + x)] = pal[v]
    sp.write_png(out, W, H, bg)
    print("wrote %s (256 tiles, %dx%d)" % (out, W, H))


if __name__ == "__main__":
    main()

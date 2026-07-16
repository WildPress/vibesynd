#!/usr/bin/env python3
"""Export each Syndicate map tile as an individual PNG for LoRA training.

Nearest-neighbour upscales each non-empty 64x48 tile onto a consistent dark canvas
and writes it as tile_NNN.png. Caption .txt files are written separately once the
tiles are described. Copyrighted game data: output stays local, never committed.

  python3 tools/synd_tile_dataset.py <HBLK.DAT> <PAL> <outdir> [scale]
"""
import os, struct, sys
import rnc
import synd_sprites as sp
import synd_tiles as st

BG = (26, 26, 38)


def main():
    hblk_path, pal_path, outdir = sys.argv[1:4]
    scale = int(sys.argv[4]) if len(sys.argv) > 4 else 8
    os.makedirs(outdir, exist_ok=True)
    d = open(hblk_path, "rb").read()
    hblk = rnc.unpack(d) if d[:3] == b"RNC" else d
    pal = sp.load_palette(pal_path)
    TW, TH = st.TW, st.TH
    SWc, SHc = TW * scale, TH * scale
    kept = []
    for tid in range(256):
        t = st.decode_tile(hblk, tid)
        if all(v == 255 for v in t):        # empty tile
            continue
        rgb = [None] * (SWc * SHc)
        for y in range(SHc):
            sy = TH - 1 - (y // scale)       # flip (tile data is bottom-up)
            for x in range(SWc):
                v = t[sy * TW + (x // scale)]
                rgb[y * SWc + x] = BG if v == 255 else pal[v]
        sp.write_png(os.path.join(outdir, "tile_%03d.png" % tid), SWc, SHc, rgb)
        kept.append(tid)
    print("wrote %d non-empty tiles to %s (%dx%d each)" % (len(kept), outdir, SWc, SHc))
    print("ids:", kept)


if __name__ == "__main__":
    main()

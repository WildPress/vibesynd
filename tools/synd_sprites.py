#!/usr/bin/env python3
"""Decode and render Syndicate HSPR sprites to a PNG contact sheet.

Format worked out from the files: the .TAB is 6-byte records (u32 offset, u8 width,
u8 height) into the .DAT. Each sprite row is 5-plane planar, 8 pixels per 5 bytes,
MSB first, giving a 5-bit palette index (0..31), value 0 = transparent. The palette
is a 256-colour VGA 6-bit table (multiply by 4 for 8-bit RGB).

Reads straight from the GOG data files, RNC-unpacking as needed. Output PNGs go to a
local (gitignored) folder, never committed.

  python3 tools/synd_sprites.py <TAB> <DAT> <PAL> <out.png> [start] [count] [cols] [scale]
"""
import os, struct, sys, zlib
import rnc


def _maybe_unpack(path):
    d = open(path, "rb").read()
    return rnc.unpack(d) if d[:3] == b"RNC" else d


def load_palette(path):
    p = _maybe_unpack(path)          # 768 bytes, 6-bit VGA
    return [((p[i * 3] << 2), (p[i * 3 + 1] << 2), (p[i * 3 + 2] << 2)) for i in range(256)]


def load_sprites(tab_path, dat_path):
    tab = _maybe_unpack(tab_path)
    dat = _maybe_unpack(dat_path)
    ents = [struct.unpack_from("<IBB", tab, i * 6) for i in range(len(tab) // 6)]
    return ents, dat


TRANSPARENT = -1


def decode(ents, dat, i, fmt="planar"):
    """Decode sprite i. Two formats, chosen by bank (per freesynd loadSprite):

    planar (HSPR game sprites): 5 bytes encode 8 pixels. byte0 = transparency mask
    (bit set -> transparent), bytes1-4 = 4 colour planes -> 4-bit index (0..15),
    MSB leftmost.

    rle (MSPR menu sprites): 8-bit indexed, run-length per row. A control byte b
    gives run = b if b < 128 else b-256. run > 0 copies that many literal pixels,
    run < 0 is that many transparent pixels, run == 0 ends the row."""
    off, w, h = ents[i]
    nb = (ents[i + 1][0] if i + 1 < len(ents) else len(dat)) - off
    if w == 0 or h == 0 or nb <= 0:
        return w, h, []
    if fmt == "rle":
        px = []
        p = off
        for _ in range(h):
            row = []
            while p < len(dat):
                b = dat[p]; p += 1
                run = b if b < 128 else b - 256
                if run == 0:
                    break
                if run > 0:
                    for _ in range(run):
                        row.append(dat[p] if p < len(dat) else 0); p += 1
                else:
                    row.extend([TRANSPARENT] * (-run))
            px.append((row + [TRANSPARENT] * w)[:w])
        return w, h, px
    bpr = nb // h
    groups = bpr // 5
    px = []
    for r in range(h):
        base = off + r * bpr
        row = []
        for g in range(groups):
            d0, d1, d2, d3, d4 = (dat[base + g * 5 + k] if base + g * 5 + k < len(dat) else 0 for k in range(5))
            for bit in range(8):
                b = 7 - bit
                if (d0 >> b) & 1:
                    row.append(TRANSPARENT)
                else:
                    row.append(((d1 >> b) & 1) | (((d2 >> b) & 1) << 1) | (((d3 >> b) & 1) << 2) | (((d4 >> b) & 1) << 3))
        px.append(row[:w] if w else row)
    return w, h, px


def write_png(path, w, h, rgb):
    def chunk(typ, data):
        c = struct.pack(">I", len(data)) + typ + data
        return c + struct.pack(">I", zlib.crc32(typ + data) & 0xFFFFFFFF)
    raw = bytearray()
    for y in range(h):
        raw.append(0)
        for x in range(w):
            raw += bytes(rgb[y * w + x])
    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(raw), 9))
    png += chunk(b"IEND", b"")
    open(path, "wb").write(png)


def main():
    tab, dat, pal, out = sys.argv[1:5]
    start = int(sys.argv[5]) if len(sys.argv) > 5 else 0
    count = int(sys.argv[6]) if len(sys.argv) > 6 else 120
    cols = int(sys.argv[7]) if len(sys.argv) > 7 else 12
    scale = int(sys.argv[8]) if len(sys.argv) > 8 else 2
    palette = load_palette(pal)
    ents, data = load_sprites(tab, dat)
    fmt = "rle" if "MSPR" in os.path.basename(tab).upper() else "planar"
    end = min(start + count, len(ents) - 1)
    items = [(i, decode(ents, data, i, fmt)) for i in range(start, end)]
    # tight flow layout: each sprite takes only its own size, wrapping to new rows
    pad = 3
    W = max(cols * 40, 400)
    x, y, row_h = pad, pad, 0
    pos = []
    for _, (w, h, px) in items:
        w, h = max(w, 1), max(h, 1)
        if x + w + pad > W and x > pad:
            x, y, row_h = pad, y + row_h + pad, 0
        pos.append((x, y))
        x += w + pad
        row_h = max(row_h, h)
    H = y + row_h + pad
    bg = [(40, 40, 48) if ((px // 8 + py // 8) & 1) else (28, 28, 34) for py in range(H) for px in range(W)]
    for (idx, (w, h, spx)), (cx, cy) in zip(items, pos):
        for yy in range(len(spx)):
            row = spx[yy]
            for xx in range(len(row)):
                v = row[xx]
                if v < 0:
                    continue
                px_, py_ = cx + xx, cy + yy
                if 0 <= px_ < W and 0 <= py_ < H:
                    bg[py_ * W + px_] = palette[v]
    if scale > 1:
        SW, SH = W * scale, H * scale
        big = [bg[(y // scale) * W + (x // scale)] for y in range(SH) for x in range(SW)]
        write_png(out, SW, SH, big)
    else:
        write_png(out, W, H, bg)
    print("wrote %s  (%d sprites %d..%d, %dx%d)" % (out, len(items), start, end, W * scale, H * scale))


if __name__ == "__main__":
    main()

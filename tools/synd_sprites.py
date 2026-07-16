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
import struct, sys, zlib
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


TRANSPARENT = 255


def decode(ents, dat, i):
    """HSPR/MSPR format (per freesynd unpackBlocks1): 5 bytes encode 8 pixels.
    byte0 = transparency mask (bit set -> transparent), bytes1-4 = 4 colour planes
    giving a 4-bit palette index (0..15). MSB is the leftmost pixel."""
    off, w, h = ents[i]
    nb = (ents[i + 1][0] if i + 1 < len(ents) else len(dat)) - off
    if w == 0 or h == 0 or nb <= 0:
        return w, h, []
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
    end = min(start + count, len(ents) - 1)
    items = [(i, decode(ents, data, i)) for i in range(start, end)]
    cell_w = max((w for _, (w, h, p) in items), default=8) + 4
    cell_h = max((h for _, (w, h, p) in items), default=8) + 6
    rows = (len(items) + cols - 1) // cols
    W, H = cols * cell_w, rows * cell_h
    # checkerboard background so transparency reads
    bg = [(40, 40, 48) if ((x // 8 + y // 8) & 1) else (28, 28, 34) for y in range(H) for x in range(W)]
    for n, (idx, (w, h, px)) in enumerate(items):
        cx = (n % cols) * cell_w + 2
        cy = (n // cols) * cell_h + 2
        for y in range(len(px)):
            row = px[y]
            for x in range(len(row)):
                v = row[x]
                if v == TRANSPARENT:
                    continue
                col = palette[v]
                py = cy + y
                px_ = cx + x
                if 0 <= px_ < W and 0 <= py < H:
                    bg[py * W + px_] = col
    if scale > 1:
        SW, SH = W * scale, H * scale
        big = [bg[(y // scale) * W + (x // scale)] for y in range(SH) for x in range(SW)]
        write_png(out, SW, SH, big)
    else:
        write_png(out, W, H, bg)
    print("wrote %s  (%d sprites %d..%d, %dx%d)" % (out, len(items), start, end, W * scale, H * scale))


if __name__ == "__main__":
    main()

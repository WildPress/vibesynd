#!/usr/bin/env python3
"""Turn a generated tile image into a clean game-resolution tile.

Downscales the generated PNG to tile size, then snaps every pixel to a fixed
palette so the output stays on-scheme and off-colour noise is removed. Optionally
keys the flat background out to transparency. Uses PIL + numpy (present in the AI
environment). Default palette is the 16 colours the Syndicate tiles actually use.

  python3 pixelate_tile.py <in.png> <out.png> [--size WxH] [--palette pal.json]
                           [--bg R,G,B] [--scale N]

  --size     output tile size (default 64x48)
  --palette  JSON list of [r,g,b] to quantise to (default: the game tile palette)
  --bg       a background colour to make transparent (e.g. 26,26,38)
  --scale    also write an N-times nearest-neighbour upscale for easy viewing
"""
import sys, json
import numpy as np
from PIL import Image

# HPALETTE indices 0-15: the palette the 4-bit Syndicate tiles use
GAME_PALETTE = [
    (0, 0, 0), (172, 84, 44), (48, 64, 56), (252, 164, 92), (192, 156, 156),
    (116, 80, 36), (168, 36, 0), (76, 88, 120), (28, 32, 44), (200, 124, 56),
    (100, 108, 92), (224, 216, 40), (220, 252, 252), (184, 148, 32), (252, 60, 8), (0, 140, 184),
]


def quantise(rgb, pal):
    pal = np.array(pal, dtype=np.int32)
    flat = rgb.reshape(-1, 3).astype(np.int32)
    dist = ((flat[:, None, :] - pal[None, :, :]) ** 2).sum(2)
    return pal[dist.argmin(1)].reshape(rgb.shape).astype(np.uint8)


def arg(name, default=None):
    return sys.argv[sys.argv.index(name) + 1] if name in sys.argv else default


def main():
    inp, out = sys.argv[1], sys.argv[2]
    w, h = (int(v) for v in arg("--size", "64x48").split("x"))
    pal = GAME_PALETTE
    if arg("--palette"):
        pal = [tuple(c) for c in json.load(open(arg("--palette")))]
    bg = tuple(int(v) for v in arg("--bg").split(",")) if arg("--bg") else None
    scale = int(arg("--scale", "0"))

    im = Image.open(inp).convert("RGB")
    small = im.resize((w, h), Image.BOX)        # area-average downscale
    q = quantise(np.array(small), pal)

    if bg:
        bg_q = quantise(np.array([[bg]], dtype=np.uint8), pal)[0, 0]
        alpha = np.where((q == bg_q).all(2), 0, 255).astype(np.uint8)
        img = Image.fromarray(np.dstack([q, alpha]), "RGBA")
    else:
        img = Image.fromarray(q, "RGB")
    img.save(out)
    print("wrote %s (%dx%d, %d-colour%s)" % (out, w, h, len(pal), ", bg keyed" if bg else ""))

    if scale > 1:
        big = img.resize((w * scale, h * scale), Image.NEAREST)
        p = out.rsplit(".", 1)
        big.save("%s_x%d.%s" % (p[0], scale, p[1]))
        print("  preview: %s_x%d.%s" % (p[0], scale, p[1]))


if __name__ == "__main__":
    main()

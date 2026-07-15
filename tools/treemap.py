#!/usr/bin/env python3
"""treemap.py -- a STATIC squarified treemap of decomp progress, as an SVG for the README.

decomp.dev draws its treemap on a <canvas>, which GitHub READMEs can't run (no JS, no canvas). A
committed .svg referenced as an image DOES render on the repo homepage, so this emits a self-contained
SVG: a two-level squarified treemap (subsystem boxes, functions within), each rectangle's AREA
proportional to that function's code size, coloured by how far we have it:
    matched (green) = byte-exact · parked (amber) = decoded, on a codegen wall · undecoded (grey)

    python tools/treemap.py               # -> docs/treemap.svg   (embed with ![](docs/treemap.svg))

Standalone: host Python, no deps. The card is dark-themed so it reads on both light and dark READMEs.
"""
import json, os, glob, html

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MAN = os.path.join(ROOT, "manifest", "functions.json")
OUT = os.path.join(ROOT, "docs", "treemap.svg")

W, H = 1200, 800
HEAD = 74                      # header band height
PAD = 14                      # card padding
GAP = 2                       # inset between subsystem boxes
LABEL_H = 15                  # subsystem label strip

BG = "#0d1117"; CARD = "#0d1117"; STROKE = "#0d1117"
INK = "#e6edf3"; MUTED = "#8b949e"
COL = {"matched": "#2ea043", "parked": "#d29922", "no-c": "#545d68"}


# ---- squarified treemap (Bruls/Huizing/van Wijk; port of the `squarify` package) -------------
def _layoutrow(sizes, x, y, dy):
    w = sum(sizes) / dy
    out = []
    for s in sizes:
        out.append((x, y, w, s / w)); y += s / w
    return out

def _layoutcol(sizes, x, y, dx):
    h = sum(sizes) / dx
    out = []
    for s in sizes:
        out.append((x, y, s / h, h)); x += s / h
    return out

def _layout(sizes, x, y, dx, dy):
    return _layoutrow(sizes, x, y, dy) if dx >= dy else _layoutcol(sizes, x, y, dx)

def _worst(sizes, x, y, dx, dy):
    r = _layout(sizes, x, y, dx, dy)
    return max(max(w / h, h / w) for (_, _, w, h) in r if w > 0 and h > 0)

def squarify(sizes, x, y, dx, dy):
    """sizes: areas (already normalised to dx*dy), DESCENDING. Returns (x,y,w,h) per size, in order."""
    sizes = [float(s) for s in sizes]
    if not sizes:
        return []
    if len(sizes) == 1:
        return _layout(sizes, x, y, dx, dy)
    i = 1
    while i < len(sizes) and _worst(sizes[:i], x, y, dx, dy) >= _worst(sizes[:i + 1], x, y, dx, dy):
        i += 1
    cur, rest = sizes[:i], sizes[i:]
    rects = _layout(cur, x, y, dx, dy)
    area = sum(cur)
    if dx >= dy:
        w = area / dy
        nx, ny, ndx, ndy = x + w, y, dx - w, dy
    else:
        h = area / dx
        nx, ny, ndx, ndy = x, y + h, dx, dy - h
    return rects + squarify(rest, nx, ny, ndx, ndy)


def normalize(sizes, dx, dy):
    tot = sum(sizes)
    if tot <= 0:
        return [0.0 for _ in sizes]
    area = dx * dy
    return [s * area / tot for s in sizes]


def rect(x, y, w, h, fill, stroke=STROKE, sw=1, rx=0):
    if w <= 0 or h <= 0:
        return ""
    return (f'<rect x="{x:.1f}" y="{y:.1f}" width="{w:.1f}" height="{h:.1f}" fill="{fill}" '
            f'stroke="{stroke}" stroke-width="{sw}"{f" rx={rx}" if rx else ""}/>')


def text(x, y, s, fill=INK, size=11, weight="normal", anchor="start", outline=False):
    o = (f'stroke="{BG}" stroke-width="3" paint-order="stroke" ' if outline else "")
    return (f'<text x="{x:.1f}" y="{y:.1f}" fill="{fill}" font-size="{size}" font-weight="{weight}" '
            f'text-anchor="{anchor}" font-family="ui-monospace,Menlo,Consolas,monospace" '
            f'{o}>{html.escape(s)}</text>')


def main():
    man = json.load(open(MAN))["functions"]
    SRC = {}
    for p in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        rel = os.path.relpath(p, os.path.join(ROOT, "src")).replace("\\", "/")
        SRC[os.path.basename(p)[:-2]] = os.path.dirname(rel) or "unclassified"

    def klass(f):
        if f.get("status") == "matched":
            return "matched"
        return "parked" if (f["name"] in SRC or ("FUN_" + f["addr"]) in SRC) else "no-c"

    subs = {}
    for f in man:
        sub = SRC.get(f["name"]) or SRC.get("FUN_" + f["addr"]) or "unclassified"
        subs.setdefault(sub, []).append(f)

    tot_by = sum(f["size"] for f in man)
    mby = sum(f["size"] for f in man if klass(f) == "matched")
    mfn = sum(1 for f in man if klass(f) == "matched")
    pfn = sum(1 for f in man if klass(f) == "parked")
    nfn = sum(1 for f in man if klass(f) == "no-c")
    covb = 100.0 * mby / tot_by if tot_by else 0

    # areas
    x0, y0 = PAD, HEAD
    tw, th = W - 2 * PAD, H - HEAD - PAD
    sub_items = sorted(subs.items(), key=lambda kv: -sum(f["size"] for f in kv[1]))
    sub_sizes = normalize([sum(f["size"] for f in fs) for _, fs in sub_items], tw, th)
    sub_rects = squarify(sub_sizes, x0, y0, tw, th)

    body = []
    for (name, fs), (sx, sy, sw, sh) in zip(sub_items, sub_rects):
        # inset each subsystem box, reserve a label strip when it's tall enough
        ix, iy, iw, ih = sx + GAP, sy + GAP, sw - 2 * GAP, sh - 2 * GAP
        if iw <= 1 or ih <= 1:
            continue
        show_label = iw > 46 and ih > LABEL_H + 6
        fy = iy + (LABEL_H if show_label else 0)
        fh = ih - (LABEL_H if show_label else 0)
        fns = sorted(fs, key=lambda f: -f["size"])
        fsz = normalize([f["size"] for f in fns], iw, fh)
        for f, (fx, fyy, fw, fhh) in zip(fns, squarify(fsz, ix, fy, iw, fh)):
            body.append(rect(fx, fyy, fw, fhh, COL[klass(f)]))
        body.append(rect(ix, iy, iw, ih, "none", stroke="#161b22", sw=1))   # subsystem outline
        if show_label:
            mtc = sum(1 for f in fs if klass(f) == "matched")
            lbl = f"{name}  {mtc}/{len(fs)}"
            body.append(text(ix + 4, iy + 11, lbl, fill=INK, size=10, weight="bold", outline=True))

    # header: title, big %, legend
    hdr = [
        rect(0, 0, W, H, CARD),
        text(PAD, 28, "Syndicate — matching decompilation", fill=INK, size=17, weight="bold"),
        text(PAD, 50, f"{mfn} of {len(man)} functions byte-matched · "
                      f"treemap area = code size · grouped by subsystem", fill=MUTED, size=11),
        text(W - PAD, 30, f"{covb:.1f}%", fill=COL["matched"], size=26, weight="bold", anchor="end"),
        text(W - PAD, 48, "of code bytes matched", fill=MUTED, size=11, anchor="end"),
    ]
    # legend (on the title row, centred — clear of the subtitle line and the right-aligned %)
    lx = W / 2 - 150
    for i, (k, lab, n) in enumerate([("matched", "matched", mfn), ("parked", "parked", pfn),
                                     ("no-c", "undecoded", nfn)]):
        cx = lx + i * 105
        hdr.append(rect(cx, 18, 11, 11, COL[k], stroke="none"))
        hdr.append(text(cx + 16, 27, f"{lab} {n}", fill=MUTED, size=11))

    svg = (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" width="{W}" height="{H}" '
           f'font-family="ui-monospace,Menlo,Consolas,monospace" role="img" '
           f'aria-label="Syndicate decompilation progress treemap, {covb:.1f}% of code bytes matched">'
           + "".join(hdr) + "".join(body) + "</svg>\n")

    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    open(OUT, "w", encoding="utf-8").write(svg)
    print(f"wrote {OUT}  ({len(svg):,} bytes)")
    print(f"  {mfn} matched / {pfn} parked / {nfn} undecoded across {len(subs)} subsystems, "
          f"{covb:.1f}% bytes")
    print("  embed in README:  ![Decompilation progress](docs/treemap.svg)")


if __name__ == "__main__":
    main()

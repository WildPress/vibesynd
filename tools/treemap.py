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
COL = {"matched": "#2ea043", "equivalent": "#22d3ee", "near": "#388bfd", "parked": "#d29922", "no-c": "#545d68"}


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

    # equivalence.json (from classify_equiv.py): register-normalised verdict per unmatched fn.
    #   equivalent (cyan) = REGISTER-ROLE/PURE-ALLOC/MATCH: same instructions, only a register/slot
    #     differs -> PROVABLY zero behavioural difference, just not byte-identical.
    #   near (blue)       = STRUCTURAL but >=0.90 similar: 1-2 encoding idioms (e.g. xor;mov16 vs mov;and),
    #     almost surely semantically equivalent.
    #   parked (amber)    = STRUCTURAL <0.90: a real instruction-shape difference (a source change to find).
    eqf = os.path.join(ROOT, "manifest", "equivalence.json")
    EQUIV, NEAR = set(), set()
    if os.path.exists(eqf):
        for n, v in json.load(open(eqf)).items():
            if not v:
                continue
            if v.get("verdict") in ("MATCH", "PURE-ALLOC", "REGISTER-ROLE"):
                EQUIV.add(n)
            elif v.get("verdict") == "STRUCTURAL" and (v.get("score") or 0) >= 0.90:
                NEAR.add(n)

    def klass(f):
        if f.get("status") == "matched":
            return "matched"
        has_src = f["name"] in SRC or ("FUN_" + f["addr"]) in SRC
        if not has_src:
            return "no-c"
        if f["name"] in EQUIV:
            return "equivalent"
        if f["name"] in NEAR:
            return "near"
        return "parked"

    subs = {}
    for f in man:
        sub = SRC.get(f["name"]) or SRC.get("FUN_" + f["addr"]) or "unclassified"
        subs.setdefault(sub, []).append(f)

    tot_by = sum(f["size"] for f in man)
    mby = sum(f["size"] for f in man if klass(f) == "matched")
    mfn = sum(1 for f in man if klass(f) == "matched")
    efn = sum(1 for f in man if klass(f) == "equivalent")
    nefn = sum(1 for f in man if klass(f) == "near")
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
            mtc = sum(1 for f in fs if klass(f) in ("matched", "equivalent"))
            lbl = f"{name}  {mtc}/{len(fs)}"
            body.append(text(ix + 4, iy + 11, lbl, fill=INK, size=10, weight="bold", outline=True))

    # header: title, big %, legend
    hdr = [
        rect(0, 0, W, H, CARD),
        text(PAD, 28, "Syndicate — matching decompilation", fill=INK, size=17, weight="bold"),
        text(PAD, 50, f"{mfn} byte-matched + {efn} register-only + {nefn} near-identical = "
                      f"{mfn+efn+nefn} of {len(man)} behaviourally complete · treemap area = code size",
             fill=MUTED, size=11),
        text(W - PAD, 30, f"{covb:.1f}%", fill=COL["matched"], size=26, weight="bold", anchor="end"),
        text(W - PAD, 48, "of code bytes byte-exact", fill=MUTED, size=11, anchor="end"),
    ]
    # legend (on the title row, centred — clear of the subtitle line and the right-aligned %)
    lx = W / 2 - 215
    for i, (k, lab, n) in enumerate([("matched", "byte-exact", mfn), ("equivalent", "register-only", efn),
                                     ("near", "near-identical", nefn), ("parked", "structural", pfn)]):
        cx = lx + i * 120
        hdr.append(rect(cx, 18, 11, 11, COL[k], stroke="none"))
        hdr.append(text(cx + 16, 27, f"{lab} {n}", fill=MUTED, size=11))

    svg = (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" width="{W}" height="{H}" '
           f'font-family="ui-monospace,Menlo,Consolas,monospace" role="img" '
           f'aria-label="Syndicate decompilation progress treemap, {covb:.1f}% of code bytes matched">'
           + "".join(hdr) + "".join(body) + "</svg>\n")

    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    open(OUT, "w", encoding="utf-8").write(svg)
    print(f"wrote {OUT}  ({len(svg):,} bytes)")
    print(f"  {mfn} matched / {efn} register-only / {nefn} near-identical / {pfn} structural across {len(subs)} subsystems, "
          f"{covb:.1f}% bytes")
    print("  embed in README:  ![Decompilation progress](docs/treemap.svg)")


if __name__ == "__main__":
    main()

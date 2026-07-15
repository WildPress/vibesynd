#!/usr/bin/env python3
"""progress.py -- a decomp.dev-style progress page for the Syndicate matching decomp.

Standalone (plain host Python, NO container needed): reads manifest/functions.json, resolves each
function's subsystem from the src/<subsystem>/ tree, walks the git history of the manifest to
reconstruct the matched-over-time curve, and emits a SELF-CONTAINED dashboard/progress.html
(data + SVG charts inlined, no external fetch, no JS libraries -- opens straight off disk).

Inspired by decomp.dev's project view: the centrepiece is the MOSAIC -- one cell per function,
width proportional to code bytes, coloured by how far we have it:
    matched  (green)  = byte-exact C exists; this IS the original's machine code
    parked   (amber)  = decoded C exists but not yet byte-exact  (our analog of decomp.dev "fuzzy")
    no-C     (grey)   = no source yet (undecoded)
Our "units" are the src/ subsystem directories (startup, combat, mission, render, lib/runtime, ...),
the natural analog of decomp.dev's per-object-file breakdown.

    python tools/progress.py                 # -> dashboard/progress.html
    python tools/progress.py --no-history     # skip the git walk (fast; flat curve)

Kept LOCAL by design (see AGENTS.md): this is not published as a claude.ai Artifact.
"""
import json, os, sys, glob, subprocess, html
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MAN = os.path.join(ROOT, "manifest", "functions.json")
OUTDIR = os.path.join(ROOT, "dashboard")
SEG_BYTES = 261620   # OBJECT1 code segment size, for "% of segment attributed" context

# ---- palette (matches tools/dashboard.py; theme-aware) ----------------------------------------
MATCHED, EQUIV, PARKED, NOC = "matched", "equivalent", "parked", "no-c"
# parked fns whose only diff is register/encoding (from classify_equiv.py) -> "equivalent" (complete).
_eqf = os.path.join(ROOT, "manifest", "equivalence.json")
EQSET = ({n for n, v in json.load(open(_eqf)).items()
          if v and v.get("verdict") in ("MATCH", "PURE-ALLOC", "REGISTER-ROLE")}
         if os.path.exists(_eqf) else set())


def load_manifest_at(ref=None):
    """Parse manifest/functions.json either from the working tree (ref=None) or a git commit."""
    if ref is None:
        txt = open(MAN, encoding="utf-8").read()
    else:
        txt = subprocess.run(["git", "-C", ROOT, "show", f"{ref}:manifest/functions.json"],
                             capture_output=True, text=True).stdout
    return json.loads(txt)["functions"]


def subsystem_map():
    """basename(FUN_xxxx / semantic name) -> subsystem dir, from the src/ tree (same resolution as
    coverage.py: the manifest's per-fn "src" field goes stale, so classify by where the .c lives)."""
    m = {}
    for p in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        rel = os.path.relpath(p, os.path.join(ROOT, "src")).replace("\\", "/")
        sub = os.path.dirname(rel) or "unclassified"      # e.g. "lib/runtime", "combat"
        m[os.path.basename(p)[:-2]] = sub
    return m


def klass(f, has_src):
    if f.get("status") == "matched":
        return MATCHED
    if not has_src:
        return NOC
    return EQUIV if f["name"] in EQSET else PARKED


def build_history():
    """Walk every commit that touched the manifest -> [(unix_ts, matched_fns, total_fns,
    matched_bytes, total_bytes)], oldest first. Schema drift (older manifests, missing status)
    is tolerated: a function with no status just isn't 'matched'."""
    out = []
    log = subprocess.run(["git", "-C", ROOT, "log", "--format=%H %ct",
                          "--", "manifest/functions.json"], capture_output=True, text=True).stdout
    rows = [ln.split() for ln in log.splitlines() if ln.strip()]
    for h, ts in reversed(rows):                          # oldest -> newest
        try:
            fns = load_manifest_at(h)
        except Exception:
            continue
        mfn = sum(1 for f in fns if f.get("status") == "matched")
        mby = sum(f.get("size", 0) for f in fns if f.get("status") == "matched")
        tby = sum(f.get("size", 0) for f in fns)
        out.append((int(ts), mfn, len(fns), mby, tby))
    return out


# ---- SVG progress-over-time chart -------------------------------------------------------------
def history_svg(hist, W=1100, H=200):
    if len(hist) < 2:
        return '<div class="empty">No manifest history available.</div>'
    pad_l, pad_r, pad_t, pad_b = 44, 12, 14, 26
    iw, ih = W - pad_l - pad_r, H - pad_t - pad_b
    t0, t1 = hist[0][0], hist[-1][0]
    span = max(t1 - t0, 1)
    pct = [100.0 * mby / tby if tby else 0 for _, _, _, mby, tby in hist]
    ymax = max(5.0, min(100.0, (int(max(pct) / 10) + 1) * 10))       # round up to next 10%

    def X(t): return pad_l + iw * (t - t0) / span
    def Y(p): return pad_t + ih * (1 - p / ymax)

    pts = [(X(h[0]), Y(pc)) for h, pc in zip(hist, pct)]
    line = " ".join(f"{x:.1f},{y:.1f}" for x, y in pts)
    area = f"{pad_l},{pad_t+ih} " + line + f" {pad_l+iw:.1f},{pad_t+ih}"
    # y gridlines / labels
    grid = []
    for i in range(0, int(ymax) + 1, max(10, int(ymax // 5 // 10) * 10 or 10)):
        y = Y(i)
        grid.append(f'<line x1="{pad_l}" y1="{y:.1f}" x2="{pad_l+iw}" y2="{y:.1f}" class="grid"/>'
                    f'<text x="{pad_l-6}" y="{y+3:.1f}" class="ylab">{i}%</text>')
    # x date labels (first, middle, last)
    import datetime
    def dlab(ts): return datetime.datetime.fromtimestamp(ts, datetime.timezone.utc).strftime("%b %d")
    xlabs = []
    for h in (hist[0], hist[len(hist)//2], hist[-1]):
        x = X(h[0])
        xlabs.append(f'<text x="{x:.1f}" y="{H-8}" class="xlab" text-anchor="middle">{dlab(h[0])}</text>')
    last = pct[-1]
    return f'''<svg viewBox="0 0 {W} {H}" class="hist" role="img"
      aria-label="Matched code bytes over time, currently {last:.1f} percent">
      {''.join(grid)}
      <polygon points="{area}" class="harea"/>
      <polyline points="{line}" class="hline"/>
      <circle cx="{pts[-1][0]:.1f}" cy="{pts[-1][1]:.1f}" r="3.5" class="hdot"/>
      {''.join(xlabs)}
    </svg>'''


# ---- MOSAIC ----------------------------------------------------------------------------------
def cell(f, sub):
    st = f["_k"]
    t = (f'{f["name"]}  ·  0x{f["addr"]}  ·  {f["size"]} B  ·  {st}'
         f'{"  ·  "+sub if sub else ""}')
    # data-* feed the JS hover readout; flex-grow encodes byte mass.
    return (f'<i class="c {st}" style="flex-grow:{max(f["size"],1)}" title="{html.escape(t)}" '
            f'data-n="{html.escape(f["name"])}" data-a="{f["addr"]}" data-s="{f["size"]}" '
            f'data-k="{st}" data-sub="{html.escape(sub)}"></i>')


def main():
    fns = load_manifest_at()
    submap = subsystem_map()
    for f in fns:
        sub = submap.get(f["name"]) or submap.get("FUN_" + f["addr"]) or ""
        f["_sub"] = sub
        f["_k"] = klass(f, bool(sub))

    tot = len(fns)
    tby = sum(f["size"] for f in fns)
    mfn = sum(1 for f in fns if f["_k"] == MATCHED)
    efn = sum(1 for f in fns if f["_k"] == EQUIV)
    pfn = sum(1 for f in fns if f["_k"] == PARKED)
    nfn = sum(1 for f in fns if f["_k"] == NOC)
    mby = sum(f["size"] for f in fns if f["_k"] == MATCHED)
    pby = sum(f["size"] for f in fns if f["_k"] == PARKED)
    covb = 100.0 * mby / tby if tby else 0
    covf = 100.0 * mfn / tot if tot else 0

    # per-subsystem aggregation, ordered by total bytes desc
    subs = defaultdict(lambda: {"m": 0, "p": 0, "n": 0, "mby": 0, "tby": 0, "fns": []})
    for f in fns:
        s = subs[f["_sub"] or "unclassified"]
        s["fns"].append(f)
        s["tby"] += f["size"]
        if f["_k"] == MATCHED: s["m"] += 1; s["mby"] += f["size"]
        elif f["_k"] == PARKED: s["p"] += 1
        else: s["n"] += 1
    sub_order = sorted(subs.items(), key=lambda kv: -kv[1]["tby"])

    # whole-binary mosaic: functions grouped by subsystem (colours cluster), then by addr
    ordered = sorted(fns, key=lambda f: (f["_sub"] or "~", int(f["addr"], 16)))
    mosaic = "".join(cell(f, f["_sub"] or "unclassified") for f in ordered)

    # per-subsystem mini mosaics + bars
    sub_html = []
    for name, s in sub_order:
        cells = "".join(cell(f, name) for f in sorted(s["fns"], key=lambda f: int(f["addr"], 16)))
        pct = 100.0 * s["mby"] / s["tby"] if s["tby"] else 0
        sub_html.append(f'''<div class="srow">
          <div class="shead"><span class="sname">{html.escape(name)}</span>
            <span class="sstat">{s["m"]}/{len(s["fns"])} fns · {pct:.0f}% bytes</span></div>
          <div class="mosaic mini">{cells}</div></div>''')

    hist = [] if "--no-history" in sys.argv else build_history()
    chart = history_svg(hist)

    os.makedirs(OUTDIR, exist_ok=True)
    doc = PAGE.format(
        covb=f"{covb:.1f}", covf=f"{covf:.1f}", mfn=mfn, tot=tot, efn=efn, pfn=pfn, nfn=nfn,
        mby=f"{mby:,}", tby=f"{tby:,}", pby=f"{pby:,}", nsub=len(subs),
        segpct=f"{100.0*tby/SEG_BYTES:.0f}",
        mosaic=mosaic, subrows="\n".join(sub_html), chart=chart,
        css=CSS, js=JS)
    outp = os.path.join(OUTDIR, "progress.html")
    open(outp, "w", encoding="utf-8").write(doc)
    print(f"wrote {outp}")
    print(f"  {mfn}/{tot} matched ({covf:.1f}% of fns, {covb:.1f}% of bytes), "
          f"{pfn} parked, {nfn} no-C, {len(subs)} subsystems, {len(hist)} history points")
    print("  open it: double-click the file, or  python -m http.server 8777 -d dashboard  -> :8777/progress.html")


CSS = """
:root{--bg:#0e1316;--surface:#161d21;--surface2:#1d262b;--border:#26323a;--ink:#dbe2e7;
  --muted:#7f8d98;--faint:#5a6771;--accent:#e2a24a;--ok:#59bd8c;--wip:#e2a24a;--none:#3a4650;--blue:#388bfd;
  --mono:ui-monospace,"Cascadia Code","JetBrains Mono",Menlo,Consolas,monospace;
  --ui:system-ui,-apple-system,"Segoe UI",Roboto,sans-serif;}
@media (prefers-color-scheme:light){:root{--bg:#e8edf0;--surface:#fff;--surface2:#f1f5f7;
  --border:#d5dde2;--ink:#16222b;--muted:#5c6a75;--faint:#8b98a2;--accent:#a86713;
  --ok:#2c8f65;--wip:#c98a1e;--none:#c2ccd3;--blue:#2f6fdb;}}
:root[data-theme=dark]{--bg:#0e1316;--surface:#161d21;--surface2:#1d262b;--border:#26323a;
  --ink:#dbe2e7;--muted:#7f8d98;--faint:#5a6771;--accent:#e2a24a;--ok:#59bd8c;--wip:#e2a24a;--none:#3a4650;--blue:#388bfd;}
:root[data-theme=light]{--bg:#e8edf0;--surface:#fff;--surface2:#f1f5f7;--border:#d5dde2;
  --ink:#16222b;--muted:#5c6a75;--faint:#8b98a2;--accent:#a86713;--ok:#2c8f65;--wip:#c98a1e;--none:#c2ccd3;--blue:#2f6fdb;}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--ink);font-family:var(--ui);font-size:14px;line-height:1.5}
.wrap{max-width:1180px;margin:0 auto;padding:0 20px 64px}
header{border-bottom:1px solid var(--border);background:var(--surface)}
.bar{max-width:1180px;margin:0 auto;padding:14px 20px;display:flex;gap:16px;align-items:center}
.brand{font-family:var(--mono);font-weight:600;font-size:15px}.brand b{color:var(--accent)}
.brand span{color:var(--faint);font-weight:400}
.tgl{margin-left:auto;font-family:var(--mono);font-size:12px;color:var(--muted);background:var(--surface2);
  border:1px solid var(--border);border-radius:8px;padding:5px 10px;cursor:pointer}
h1{font-family:var(--mono);font-size:24px;margin:30px 0 2px}
.head{display:flex;align-items:baseline;gap:14px;flex-wrap:wrap}
.big{font-family:var(--mono);font-size:44px;font-variant-numeric:tabular-nums;color:var(--ok);line-height:1}
.big small{font-size:16px;color:var(--muted)}
.lede{color:var(--muted);margin:6px 0 22px;max-width:70ch}
.tiles{display:grid;grid-template-columns:repeat(auto-fit,minmax(140px,1fr));gap:12px;margin-bottom:26px}
.tile{background:var(--surface);border:1px solid var(--border);border-radius:12px;padding:13px 15px}
.tile .k{font-size:11px;text-transform:uppercase;letter-spacing:.08em;color:var(--faint)}
.tile .v{font-family:var(--mono);font-size:24px;font-variant-numeric:tabular-nums;margin-top:5px;line-height:1}
.tile .v small{font-size:12px;color:var(--muted)}
.sec{background:var(--surface);border:1px solid var(--border);border-radius:12px;padding:16px 18px;margin-bottom:22px}
.sec h2{font-family:var(--mono);font-size:14px;text-transform:uppercase;letter-spacing:.06em;
  color:var(--muted);margin:0 0 4px}
.sec .cap{color:var(--faint);font-size:12px;margin:0 0 14px}
.legend{display:flex;gap:16px;flex-wrap:wrap;font-family:var(--mono);font-size:12px;color:var(--muted);margin-top:12px}
.legend span{display:inline-flex;align-items:center;gap:6px}
.sw{width:11px;height:11px;border-radius:3px;display:inline-block}
.sw.matched{background:var(--ok)}.sw.parked{background:var(--wip)}.sw.no-c{background:var(--none)}.sw.equivalent{background:var(--blue)}
.mosaic{display:flex;width:100%;height:38px;border-radius:6px;overflow:hidden;background:var(--surface2);
  border:1px solid var(--border);gap:1px}
.mosaic.mini{height:16px;border-radius:4px}
.mosaic .c{min-width:1px;height:100%;display:block;transition:opacity .1s}
.mosaic .c.matched{background:var(--ok)}.mosaic .c.parked{background:var(--wip)}.mosaic .c.no-c{background:var(--none)}.mosaic .c.equivalent{background:var(--blue)}
.mosaic .c:hover{opacity:.65}
.readout{font-family:var(--mono);font-size:12.5px;color:var(--ink);margin-top:10px;min-height:1.4em}
.readout .a{color:var(--accent)}.readout .m{color:var(--faint)}
.srow{margin-bottom:12px}
.shead{display:flex;justify-content:space-between;align-items:baseline;font-family:var(--mono);
  font-size:12px;margin-bottom:4px}
.sname{color:var(--ink)}.sstat{color:var(--faint);font-variant-numeric:tabular-nums}
.hist{width:100%;height:auto;display:block}
.hist .grid{stroke:var(--border);stroke-width:1}
.hist .ylab,.hist .xlab{fill:var(--faint);font-family:var(--mono);font-size:10px}
.hist .ylab{text-anchor:end}
.hist .harea{fill:color-mix(in srgb,var(--ok) 18%,transparent)}
.hist .hline{fill:none;stroke:var(--ok);stroke-width:2;stroke-linejoin:round}
.hist .hdot{fill:var(--ok)}
.empty{color:var(--muted);font-size:13px;padding:12px 0}
footer{margin-top:24px;color:var(--faint);font-size:12px;font-family:var(--mono)}
"""

JS = r"""
const root=document.documentElement;
const tg=document.getElementById('tgl');
if(tg)tg.onclick=()=>{const d=root.getAttribute('data-theme')==='light'?'dark':'light';
  root.setAttribute('data-theme',d);tg.textContent=d==='light'?'◑ dark':'◐ light';};
const ro=document.getElementById('readout');
document.querySelectorAll('.mosaic').forEach(m=>m.addEventListener('mousemove',e=>{
  const c=e.target.closest('.c');if(!c||!ro)return;
  ro.innerHTML=`<span class="a">${c.dataset.n}</span> · 0x${c.dataset.a} · `
    +`${(+c.dataset.s).toLocaleString()} B · ${c.dataset.k}`
    +`<span class="m"> · ${c.dataset.sub}</span>`;}));
"""

PAGE = """<!doctype html><meta charset="utf-8">
<title>Syndicate decomp — progress</title>
<style>{css}</style>
<header><div class="bar">
  <div class="brand"><b>SYNDICATE</b> <span>// matching decomp · progress · local</span></div>
  <button class="tgl" id="tgl">◐ light</button>
</div></header>
<div class="wrap">
  <h1>Decompilation progress</h1>
  <div class="head"><div class="big">{covb}%<small> of code bytes byte-matched</small></div></div>
  <p class="lede">Byte-for-byte parity of the original <b>Syndicate</b> (Watcom / DOS4GW) code segment,
    one function at a time. {mfn} of {tot} functions are byte-exact; the rest are decoded and parked on
    a compiler-codegen wall. Units are the <b>src/</b> subsystem tree.</p>

  <div class="tiles">
    <div class="tile"><div class="k">Functions matched</div><div class="v">{mfn}<small>/{tot}</small></div></div>
    <div class="tile"><div class="k">Code bytes matched</div><div class="v">{mby}<small> / {tby}</small></div></div>
    <div class="tile"><div class="k">Equivalent <small>(reg-only)</small></div><div class="v">{efn}</div></div>
    <div class="tile"><div class="k">Parked (decoded)</div><div class="v">{pfn}<small> · {pby} B</small></div></div>
    <div class="tile"><div class="k">Undecoded</div><div class="v">{nfn}</div></div>
    <div class="tile"><div class="k">Subsystems</div><div class="v">{nsub}</div></div>
  </div>

  <div class="sec">
    <h2>The binary at a glance</h2>
    <p class="cap">Every function, one cell, width proportional to its code size — grouped by subsystem.
      Hover for detail.</p>
    <div class="mosaic">{mosaic}</div>
    <div class="readout" id="readout">Hover a cell…</div>
    <div class="legend">
      <span><i class="sw matched"></i>matched — byte-exact</span>
      <span><i class="sw equivalent"></i>equivalent — complete, register/encoding-only diff</span>
      <span><i class="sw parked"></i>parked — decoded, on a codegen wall</span>
      <span><i class="sw no-c"></i>undecoded</span>
    </div>
  </div>

  <div class="sec">
    <h2>Matched code over time</h2>
    <p class="cap">Reconstructed from the git history of manifest/functions.json.</p>
    {chart}
  </div>

  <div class="sec">
    <h2>By subsystem</h2>
    <p class="cap">Each subsystem's functions, ordered by address; sorted by total code size.</p>
    {subrows}
  </div>

  <footer>Local tool · regenerate with <b>python tools/progress.py</b> · not published — stays on your machine</footer>
</div>
<script>{js}</script>
"""


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Generate the LOCAL decomp cockpit (data.json + static index.html shell).

Runs inside the synd-decomp container (needs objdump + the code segment).
  python3 tools/dashboard.py

Writes:
  dashboard/data.json   payload (functions, disasm, stats, activity notes)
  dashboard/index.html  static shell that fetches data.json and live-refreshes

Serve it locally (see tools/serve.sh) and open http://localhost:8777/. Regenerate
data.json as functions are matched; the page updates itself without a reload.
"""
import json, subprocess, re, os

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
MANIFEST = "manifest/functions.json"
OUTDIR = "dashboard"
SEG_BYTES = 261620
DISASM_MAX = 300

seg = open(SEG, "rb").read()
man = json.load(open(MANIFEST))
fns = man["functions"]
base = int(man.get("image_base", "0"), 16)


def disasm(b):
    open("/tmp/d.bin", "wb").write(b)
    out = subprocess.run(
        ["objdump", "-D", "-b", "binary", "-m", "i386", "-M", "intel", "/tmp/d.bin"],
        capture_output=True, text=True).stdout
    rows = []
    for line in out.splitlines():
        m = re.match(r'^\s*([0-9a-f]+):\t([0-9a-f ]+?)\t(.*)$', line)
        if m:
            rows.append([int(m.group(1), 16), m.group(2).strip(), m.group(3).strip()])
    return rows


records, dismap = [], {}
matched_bytes = code_bytes = leaves = framed = 0
counts = {"matched": 0, "wip": 0, "unmatched": 0, "equivalent": 0}
for f in fns:
    off = int(f["addr"], 16) - base
    b = seg[off:off + f["size"]]
    is_frame = b[:3] == bytes([0x55, 0x89, 0xe5])
    is_leaf = f["calls"] == 0 and f["data_refs"] == 0
    code_bytes += f["size"]
    leaves += is_leaf
    framed += is_frame
    st = f.get("status", "unmatched")
    counts[st] = counts.get(st, 0) + 1
    if st in ("matched", "equivalent"):
        matched_bytes += f["size"]
    records.append({
        "addr": f["addr"], "name": f["name"], "size": f["size"],
        "calls": f["calls"], "data_refs": f["data_refs"], "incoming": f["incoming"],
        "status": st, "leaf": is_leaf, "frame": is_frame,
        "summary": f.get("summary", ""),
    })
    if f["size"] <= DISASM_MAX:
        dismap[f["addr"]] = disasm(b)

cov = 100.0 * matched_bytes / code_bytes if code_bytes else 0
seg_cov = 100.0 * code_bytes / SEG_BYTES
stats = {
    "total": len(fns), "matched": counts.get("matched", 0), "wip": counts.get("wip", 0),
    "leaves": leaves, "framed": framed, "code_bytes": code_bytes,
    "seg_bytes": SEG_BYTES, "matched_bytes": matched_bytes, "coverage_pct": cov,
    "phase": "Inventory & analysis",
    "notes": [
        f"{len(fns)} functions identified · {seg_cov:.1f}% of the code segment "
        f"attributed to functions",
        "Inventory is a LOWER BOUND — LE fixups (relocations) not yet applied, so "
        "jump tables / function pointers stay unresolved and hide more functions",
        f"{counts.get('matched', 0)} matched · {counts.get('wip', 0)} in progress · "
        f"{cov:.2f}% of code byte-matched",
        "Toolchain live: Open Watcom (DOS/4GW) · Ghidra 12.1.2 · objdiff · in-container",
    ],
}

os.makedirs(OUTDIR, exist_ok=True)
json.dump({"records": records, "disasm": dismap, "stats": stats},
          open(f"{OUTDIR}/data.json", "w", encoding="utf-8"))

CSS = """
:root{
  --bg:#0e1316; --surface:#161d21; --surface2:#1d262b; --border:#26323a;
  --ink:#dbe2e7; --muted:#7f8d98; --faint:#5a6771; --accent:#e2a24a;
  --accent-ink:#160e02; --ok:#59bd8c; --wip:#e2a24a; --none:#54626e; --skip:#a97fa0;
  --mono:ui-monospace,"Cascadia Code","JetBrains Mono","SF Mono",Menlo,Consolas,monospace;
  --ui:system-ui,-apple-system,"Segoe UI",Roboto,sans-serif;
}
@media (prefers-color-scheme:light){:root{
  --bg:#e8edf0; --surface:#ffffff; --surface2:#f1f5f7; --border:#d5dde2;
  --ink:#16222b; --muted:#5c6a75; --faint:#8b98a2; --accent:#a86713;
  --accent-ink:#fff7ec; --ok:#2c8f65; --wip:#9a6a12; --none:#849098; --skip:#8a5c82;
}}
:root[data-theme="dark"]{
  --bg:#0e1316; --surface:#161d21; --surface2:#1d262b; --border:#26323a;
  --ink:#dbe2e7; --muted:#7f8d98; --faint:#5a6771; --accent:#e2a24a;
  --accent-ink:#160e02; --ok:#59bd8c; --wip:#e2a24a; --none:#54626e; --skip:#a97fa0;
}
:root[data-theme="light"]{
  --bg:#e8edf0; --surface:#ffffff; --surface2:#f1f5f7; --border:#d5dde2;
  --ink:#16222b; --muted:#5c6a75; --faint:#8b98a2; --accent:#a86713;
  --accent-ink:#fff7ec; --ok:#2c8f65; --wip:#9a6a12; --none:#849098; --skip:#8a5c82;
}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--ink);font-family:var(--ui);
  font-size:14px;line-height:1.5;-webkit-font-smoothing:antialiased}
.wrap{max-width:1180px;margin:0 auto;padding:0 20px 64px}
header{position:sticky;top:0;z-index:5;background:color-mix(in srgb,var(--bg) 88%,transparent);
  backdrop-filter:blur(8px);border-bottom:1px solid var(--border)}
.bar{max-width:1180px;margin:0 auto;padding:14px 20px;display:flex;gap:16px;
  align-items:center;flex-wrap:wrap}
.brand{font-family:var(--mono);font-weight:600;letter-spacing:.02em;font-size:15px}
.brand b{color:var(--accent)} .brand span{color:var(--faint);font-weight:400}
.live{margin-left:auto;display:flex;gap:8px;align-items:center;font-family:var(--mono);
  font-size:11.5px;color:var(--muted)}
.live .dot{width:8px;height:8px;border-radius:50%;background:var(--ok);
  box-shadow:0 0 0 0 color-mix(in srgb,var(--ok) 70%,transparent);animation:pulse 2s infinite}
@keyframes pulse{0%{box-shadow:0 0 0 0 color-mix(in srgb,var(--ok) 55%,transparent)}
  70%{box-shadow:0 0 0 7px transparent}100%{box-shadow:0 0 0 0 transparent}}
@media (prefers-reduced-motion:reduce){.live .dot{animation:none}}
h1{font-family:var(--mono);font-size:22px;letter-spacing:.01em;margin:30px 0 4px;text-wrap:balance}
.lede{color:var(--muted);margin:0 0 22px;max-width:64ch}
.status{background:var(--surface);border:1px solid var(--border);border-left:3px solid var(--accent);
  border-radius:10px;padding:14px 16px;margin-bottom:22px}
.status .ph{font-size:11px;text-transform:uppercase;letter-spacing:.09em;color:var(--accent);
  margin-bottom:8px;font-weight:600}
.status ul{margin:0;padding-left:18px} .status li{margin:3px 0;color:var(--ink)}
.tiles{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:12px;margin-bottom:14px}
.tile{background:var(--surface);border:1px solid var(--border);border-radius:12px;padding:14px 16px}
.tile .k{font-size:11px;text-transform:uppercase;letter-spacing:.08em;color:var(--faint)}
.tile .v{font-family:var(--mono);font-size:26px;font-variant-numeric:tabular-nums;margin-top:6px;line-height:1}
.tile .v small{font-size:13px;color:var(--muted)}
.meterbox{background:var(--surface);border:1px solid var(--border);border-radius:12px;
  padding:16px 18px;margin-bottom:26px}
.meterhead{display:flex;justify-content:space-between;align-items:baseline;margin-bottom:10px}
.meterhead .lab{font-size:11px;text-transform:uppercase;letter-spacing:.08em;color:var(--faint)}
.meterhead .num{font-family:var(--mono);font-variant-numeric:tabular-nums;color:var(--muted)}
.meter{height:10px;background:var(--surface2);border-radius:999px;overflow:hidden;border:1px solid var(--border)}
.meter i{display:block;height:100%;background:linear-gradient(90deg,var(--accent),var(--ok));
  border-radius:999px;min-width:2px;transition:width .4s ease}
.toolbar{display:flex;gap:10px;align-items:center;flex-wrap:wrap;margin-bottom:12px}
input[type=search]{font-family:var(--mono);font-size:13px;background:var(--surface);
  border:1px solid var(--border);border-radius:9px;padding:8px 12px;color:var(--ink);min-width:220px;flex:1}
input[type=search]:focus{outline:2px solid var(--accent);outline-offset:1px}
.filters{display:flex;gap:6px;flex-wrap:wrap}
.fbtn{font-family:var(--mono);font-size:12px;padding:6px 11px;border:1px solid var(--border);
  background:var(--surface);color:var(--muted);border-radius:8px;cursor:pointer}
.fbtn[aria-pressed=true]{border-color:var(--accent);color:var(--accent);
  background:color-mix(in srgb,var(--accent) 12%,transparent)}
.fbtn:focus-visible{outline:2px solid var(--accent);outline-offset:1px}
.layout{display:grid;grid-template-columns:1fr;gap:18px}
@media(min-width:900px){.layout{grid-template-columns:1.35fr 1fr}}
.tablewrap{background:var(--surface);border:1px solid var(--border);border-radius:12px;overflow:auto;max-height:70vh}
table{border-collapse:collapse;width:100%;font-family:var(--mono);font-size:12.5px}
thead th{position:sticky;top:0;background:var(--surface2);text-align:left;padding:9px 12px;
  font-weight:600;color:var(--muted);border-bottom:1px solid var(--border);cursor:pointer;
  white-space:nowrap;font-variant-numeric:tabular-nums}
thead th.num,tbody td.num{text-align:right;font-variant-numeric:tabular-nums}
tbody td{padding:7px 12px;border-bottom:1px solid color-mix(in srgb,var(--border) 55%,transparent);white-space:nowrap}
tbody tr{cursor:pointer} tbody tr:hover{background:var(--surface2)}
tbody tr[aria-selected=true]{background:color-mix(in srgb,var(--accent) 14%,transparent)}
.addr{color:var(--accent)}
.pill{font-size:10.5px;padding:2px 8px;border-radius:999px;text-transform:uppercase;
  letter-spacing:.04em;border:1px solid transparent}
.pill.unmatched{color:var(--none);border-color:color-mix(in srgb,var(--none) 45%,transparent)}
.pill.wip{color:var(--wip);border-color:color-mix(in srgb,var(--wip) 55%,transparent)}
.pill.matched{color:var(--ok);border-color:color-mix(in srgb,var(--ok) 55%,transparent);
  background:color-mix(in srgb,var(--ok) 12%,transparent)}
.tag{font-size:10px;color:var(--faint);border:1px solid var(--border);border-radius:5px;padding:1px 5px;margin-left:5px}
.detail{background:var(--surface);border:1px solid var(--border);border-radius:12px;padding:18px;
  position:sticky;top:78px;max-height:70vh;overflow:auto}
.detail h2{font-family:var(--mono);font-size:16px;margin:0 0 3px;color:var(--accent)}
.detail .sub{color:var(--muted);font-family:var(--mono);font-size:12px;margin-bottom:12px}
.detail .summary{background:var(--surface2);border:1px solid var(--border);border-radius:8px;
  padding:10px 12px;margin-bottom:14px;color:var(--ink)}
.detail .summary.none{color:var(--faint);font-style:italic}
.kv{display:grid;grid-template-columns:auto 1fr;gap:4px 14px;font-family:var(--mono);font-size:12.5px;margin-bottom:16px}
.kv dt{color:var(--faint)} .kv dd{margin:0;text-align:right;font-variant-numeric:tabular-nums}
.asm{font-family:var(--mono);font-size:12px;line-height:1.65;background:var(--surface2);
  border:1px solid var(--border);border-radius:9px;padding:12px;overflow-x:auto}
.asm .row{display:grid;grid-template-columns:44px 128px 1fr;gap:8px;white-space:nowrap}
.asm .o{color:var(--faint)} .asm .b{color:var(--muted)} .asm .m{color:var(--ink)}
.empty{color:var(--muted);font-size:13px;padding:20px 0}
.lbl{font-size:11px;text-transform:uppercase;letter-spacing:.08em;color:var(--faint);margin-bottom:8px}
footer{margin-top:30px;color:var(--faint);font-size:12px;font-family:var(--mono)}
"""

JS = r"""
const root=document.documentElement, $=s=>document.querySelector(s);
let D=null, filter='all', sortK='addr', sortDir=1, q='', sel=null, built=false;

async function load(){
  try{
    const r=await fetch('data.json?'+Date.now());
    D=await r.json();
    if(!built) buildStatic();
    renderAll();
    const t=new Date();
    $('#updated').textContent='updated '+t.toLocaleTimeString();
  }catch(e){ $('#updated').textContent='data.json not reachable'; }
}
function buildStatic(){
  const FILT=[['all','all'],['unmatched','unmatched'],['wip','in progress'],
    ['matched','matched'],['leaf','leaves']];
  $('#filters').innerHTML=FILT.map(([k,l])=>
    `<button class="fbtn" data-f="${k}" aria-pressed="${k==='all'}">${l}</button>`).join('');
  $('#filters').addEventListener('click',e=>{const b=e.target.closest('.fbtn');if(!b)return;
    filter=b.dataset.f;[...$('#filters').children].forEach(x=>x.setAttribute('aria-pressed',x.dataset.f===filter));
    renderTable();});
  $('#q').addEventListener('input',e=>{q=e.target.value.toLowerCase();renderTable();});
  document.querySelectorAll('#tbl thead th').forEach(th=>th.addEventListener('click',()=>{
    const k=th.dataset.k; if(sortK===k)sortDir*=-1;else{sortK=k;sortDir=1;} renderTable();}));
  $('#rows').addEventListener('click',e=>{const tr=e.target.closest('tr');if(!tr)return;
    sel=tr.dataset.a; renderTable(); detail(sel);});
  built=true;
}
function renderAll(){ renderStats(); renderTable(); if(sel) detail(sel); }
function renderStats(){
  const s=D.stats;
  $('#tiles').innerHTML=[
    ['Functions',s.total,''],['Matched',s.matched,'byte-identical'],
    ['In progress',s.wip,''],['Pure leaves',s.leaves,'no calls/data'],
    ['Framed fns',s.framed,'of '+s.total],
  ].map(([k,v,x])=>`<div class="tile"><div class="k">${k}</div>
    <div class="v">${v}${x?` <small>${x}</small>`:''}</div></div>`).join('');
  $('#covbar').style.width=Math.max(s.coverage_pct,0.4)+'%';
  $('#covnum').textContent=s.matched_bytes.toLocaleString()+' / '+s.code_bytes.toLocaleString()+
    ' B  ('+s.coverage_pct.toFixed(2)+'%)';
  $('#phase').textContent=s.phase;
  $('#notes').innerHTML=s.notes.map(n=>`<li>${n}</li>`).join('');
}
function visible(){
  return D.records.filter(r=>{
    if(filter==='leaf'&&!r.leaf)return false;
    if(['unmatched','wip','matched'].includes(filter)&&r.status!==filter)return false;
    if(q&&!(r.name.toLowerCase().includes(q)||r.addr.includes(q)))return false;
    return true;
  }).sort((a,b)=>{let x=a[sortK],y=b[sortK];
    if(sortK==='addr'){x=parseInt(x,16);y=parseInt(y,16);}
    if(typeof x==='string')return sortDir*x.localeCompare(y);
    return sortDir*(x-y);});
}
function renderTable(){
  $('#rows').innerHTML=visible().map(r=>`<tr data-a="${r.addr}" aria-selected="${sel===r.addr}">
    <td class="addr">${r.addr}</td>
    <td>${r.name}${r.leaf?'<span class="tag">leaf</span>':''}${r.frame?'<span class="tag">frame</span>':''}</td>
    <td class="num">${r.size}</td><td class="num">${r.calls}</td>
    <td class="num">${r.data_refs}</td><td class="num">${r.incoming}</td>
    <td><span class="pill ${r.status}">${r.status}</span></td></tr>`).join('');
}
function detail(addr){
  const r=D.records.find(x=>x.addr===addr); if(!r)return;
  const dis=D.disasm[addr];
  const asm=dis?`<div class="asm">`+dis.map(([o,b,m])=>
    `<div class="row"><span class="o">${o.toString(16).padStart(4,'0')}</span>
      <span class="b">${b}</span><span class="m">${m.replace(/</g,'&lt;')}</span></div>`).join('')+`</div>`
    :`<div class="empty">Disassembly not embedded (function over the inline cap). View it in Ghidra.</div>`;
  const sum=r.summary?`<div class="summary">${r.summary}</div>`
    :`<div class="summary none">No description yet — added as this function is analyzed.</div>`;
  $('#detail').innerHTML=`<h2>${r.name}</h2>
    <div class="sub">${r.addr} · ${r.size} bytes · ${r.leaf?'leaf':'non-leaf'}</div>${sum}
    <dl class="kv"><dt>outgoing calls</dt><dd>${r.calls}</dd>
      <dt>data references</dt><dd>${r.data_refs}</dd><dt>callers (xrefs)</dt><dd>${r.incoming}</dd>
      <dt>frame pointer</dt><dd>${r.frame?'yes':'omitted'}</dd>
      <dt>status</dt><dd><span class="pill ${r.status}">${r.status}</span></dd></dl>
    <div class="lbl">Original disassembly</div>${asm}`;
}
load(); setInterval(load, 5000);
"""

doc = f"""<meta charset="utf-8">
<title>Syndicate decomp cockpit</title>
<style>{CSS}</style>
<header><div class="bar">
  <div class="brand"><b>SYNDICATE</b> <span>// matching decomp · local</span></div>
  <div class="live"><span class="dot"></span><span id="updated">loading…</span></div>
</div></header>
<div class="wrap">
<h1>Matching-decompilation cockpit</h1>
<p class="lede">Every function in the original <b>Syndicate</b> code segment, tracked toward
byte-for-byte parity with Watcom-compiled C. Live view — refreshes itself as work lands.</p>
<div class="status"><div class="ph" id="phase"></div><ul id="notes"></ul></div>
<div class="tiles" id="tiles"></div>
<div class="meterbox"><div class="meterhead"><span class="lab">Coverage — bytes matched</span>
  <span class="num" id="covnum"></span></div><div class="meter"><i id="covbar"></i></div></div>
<div class="toolbar"><input type="search" id="q" placeholder="filter by name or address…"
  aria-label="filter functions"><div class="filters" id="filters"></div></div>
<div class="layout">
  <div class="tablewrap"><table id="tbl"><thead><tr>
    <th data-k="addr">addr</th><th data-k="name">name</th><th data-k="size" class="num">size</th>
    <th data-k="calls" class="num">calls</th><th data-k="data_refs" class="num">data</th>
    <th data-k="incoming" class="num">xrefs</th><th data-k="status">status</th>
  </tr></thead><tbody id="rows"></tbody></table></div>
  <aside class="detail" id="detail"><div class="empty">Select a function to inspect its
    original disassembly.</div></aside>
</div>
<footer>Local tool · data.json regenerated by tools/dashboard.py · disassembly is raw linear
  objdump (branch targets approximate) · segment OBJECT1</footer>
</div>
<script>{JS}</script>
"""

open(f"{OUTDIR}/index.html", "w", encoding="utf-8").write(doc)
print(f"wrote {OUTDIR}/index.html + data.json  ({len(dismap)} fns with disasm, "
      f"{len(records)} total)")

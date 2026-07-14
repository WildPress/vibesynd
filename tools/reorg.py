#!/usr/bin/env python3
"""reorg.py -- reorganize the flat src/FUN_<addr>.c pile into a subsystem directory tree that
reads like the developers' game project, WITHOUT losing the address anchor.

Each function keeps its FUN_<addr>.c filename (the ground-truth link to the binary/Ghidra/manifest);
only its DIRECTORY changes, by subsystem. Library / non-game-logic code is separated under lib/.
Classification: explicit per-function assignments extracted from docs/architecture.md (priority),
then library address ranges, else 'unclassified'. Writes a per-dir _index.md and updates the
manifest "src" path for matched functions (format-preserving, line-by-line -- never json.dump).

  python3 tools/reorg.py            # do the reorg
  python3 tools/reorg.py --plan     # print the classification counts only, move nothing
"""
import json, os, re, sys, glob, subprocess

MAN = "manifest/functions.json"
ARCH = "docs/architecture.md"

# explicit addr -> subsystem, from architecture.md sections (assigned in ORDER; first claim wins)
EXPLICIT = {
    "startup":     [0x24be8, 0x12ca8, 0x252d8, 0x22858, 0x20fc8],
    "sys":         [0x184b8, 0x18158, 0x180f8, 0x18828, 0x179f8, 0x17b48, 0x188e8, 0x18958, 0x17998, 0x27e78, 0x28728],
    "input":       [0x254a8, 0x20c88, 0x28b88, 0x2ad58],
    "entity":      [0x13bc8, 0x376f8, 0x37738, 0x37778, 0x37a48, 0x36c28, 0x36c78, 0x37658, 0x37878, 0x26e18, 0x26da8, 0x26c78, 0x2fbc8, 0x37918, 0x1c178, 0x22b38, 0x22ba8],
    "map":         [0x33c38, 0x33cf8, 0x33fb8, 0x2d5b8, 0x2d468, 0x34368, 0x28ec8, 0x20d18, 0x1a8c8, 0x1a918, 0x19608],
    "combat":      [0x34858, 0x34198, 0x34118, 0x34168, 0x34088, 0x34608, 0x34048, 0x2d738, 0x2d6c8, 0x2d358, 0x2e4f8, 0x2e5f8, 0x2e808, 0x30868, 0x2def8, 0x2d3b8, 0x30508, 0x30708, 0x2d7a8, 0x2d808, 0x2d868],
    "mission":     [0x23158, 0x2bca8, 0x2bee8, 0x2c218, 0x37ad8, 0x37d08, 0x149e8, 0x1a458, 0x2d0d8],
    "economy":     [0x223c8, 0x12da8, 0x15f58, 0x33568, 0x35b68, 0x164c8, 0x264a8, 0x165f8, 0x16638],
    "render":      [0x35538, 0x35588, 0x355d8, 0x263f8, 0x1ff98, 0x19318, 0x26778],
    "ui":          [0x36698, 0x363d8, 0x365e8, 0x36648, 0x361a8, 0x36208, 0x36298, 0x36338, 0x205f8, 0x20728, 0x20018, 0x20158, 0x25d58, 0x35638, 0x36808, 0x1bc28, 0x29c58, 0x29ad8, 0x2a288, 0x265d8],
    "multiplayer": [0x27428, 0x272b8, 0x284a8, 0x28558, 0x28228, 0x28368, 0x27d88, 0x279f8, 0x28878, 0x288f8, 0x14078],
    "anim":        [0x2d228, 0x2bbe8, 0x13ac8, 0x13b38],
    "sound":       [0x35d08, 0x38cf8, 0x38fe8],
}
ORDER = ["startup", "sys", "input", "entity", "map", "combat", "mission",
         "economy", "render", "ui", "multiplayer", "anim", "sound"]

DESC = {
    "startup": "Program & session startup: command-line/main, session init, timer & keyboard install.",
    "sys": "Memory, file I/O, decompression, DPMI/DOS services (game infrastructure).",
    "input": "Keyboard & mouse input; mission cursor / target-action resolver.",
    "entity": "Entity/agent/vehicle pool + spatial-grid threading, spawns, chain walks.",
    "map": "Map / tile / isometric passability, column lookup, minimap & radar render.",
    "combat": "Weapons, damage, projectiles, line-of-sight, targeting (the 0x34xxx cluster).",
    "mission": "Command / mission / orders interpreters (jump-table dispatchers, scripting layer).",
    "economy": "Economy, equipment, research, funding, target claiming, save-game.",
    "render": "Low-level drawing primitives: blit, VGA, masked copy, gauge, lines.",
    "ui": "Menus, panels, HUD, text engine (glyph/word-wrap), widgets.",
    "multiplayer": "NetBIOS-over-DPMI session setup, NCB send/receive, sync barriers.",
    "anim": "Animation tick / frame counters, palette-flash effects.",
    "sound": "Game-side sound system: driver load/init, XMIDI init, channel select.",
    "lib/sound": "LIBRARY: low-level sound-driver dispatch stubs (hand-asm, AIL-style).",
    "lib/runtime": "LIBRARY: linked-in Watcom C runtime (CLIB3S) -- NOT game code.",
    "lib/gfx": "LIBRARY: low-level graphics/math primitives (different toolchain, 0x40000+).",
    "unclassified": "Not yet understood well enough to place; classified only by call-graph position.",
}


def range_subsys(a):
    if 0x39000 <= a < 0x3a000: return "lib/sound"
    if 0x3a000 <= a < 0x40000: return "lib/runtime"
    if a >= 0x40000:           return "lib/gfx"
    return None


def classify(addr):
    for sub in ORDER:
        if addr in EXPLICIT[sub]:
            return sub
    r = range_subsys(addr)
    return r if r else "unclassified"


def arch_role(addr):
    """grab a short role snippet for addr from architecture.md, if present."""
    hexs = "%x" % addr
    for ln in open(ARCH, encoding="utf-8", errors="replace"):
        if hexs in ln.lower():
            m = re.search(r"0x%s\*?\*?\s*([^.;*]+)" % hexs, ln, re.I)
            if m:
                return m.group(1).strip().strip("—-* ")[:70]
    return ""


def main():
    plan = "--plan" in sys.argv
    man = json.load(open(MAN))
    fns = man["functions"]
    buckets = {}
    assign = {}   # name -> (subsys, new_relpath)
    for f in fns:
        a = int(f["addr"], 16); nm = f["name"]
        sub = classify(a)
        buckets.setdefault(sub, []).append(f)
        assign[nm] = sub

    print("classification (%d functions):" % len(fns))
    for sub in sorted(buckets, key=lambda s: -len(buckets[s])):
        print("  %-14s %3d" % (sub, len(buckets[sub])))
    if plan:
        return

    # move files (git mv where tracked) + build new path map
    moved, newpath = 0, {}
    for f in fns:
        nm = f["name"]; sub = assign[nm]
        old = "src/%s.c" % nm
        d = "src/%s" % sub
        os.makedirs(d, exist_ok=True)
        new = "%s/%s.c" % (d, nm)
        if os.path.exists(old) and not os.path.exists(new):
            r = subprocess.run(["git", "mv", old, new], capture_output=True, text=True)
            if r.returncode != 0:
                os.replace(old, new)
            moved += 1
        newpath[nm] = new

    # stray non-FUN files -> unclassified
    for stray in glob.glob("src/*.c"):
        os.makedirs("src/unclassified", exist_ok=True)
        base = os.path.basename(stray)
        subprocess.run(["git", "mv", stray, "src/unclassified/" + base], capture_output=True, text=True)

    # update manifest "src" path for matched fns (format-preserving line edit; never json.dump)
    lines = open(MAN, encoding="utf-8").read().split("\n")
    for i, ln in enumerate(lines):
        m = re.search(r'"name": "(FUN_[0-9a-fA-F]+)"', ln)
        if m and '"src": "src/' in ln:
            nm = m.group(1)
            if nm in newpath:
                lines[i] = re.sub(r'"src": "src/[^"]*"', '"src": "%s"' % newpath[nm], ln)
    open(MAN, "w", encoding="utf-8", newline="\n").write("\n".join(lines))

    # per-dir _index.md
    by = {f["name"]: f for f in fns}
    for sub, items in buckets.items():
        d = "src/%s" % sub
        os.makedirs(d, exist_ok=True)
        rows = sorted(items, key=lambda f: int(f["addr"], 16))
        out = ["# %s\n" % sub, DESC.get(sub, "") + "\n",
               "| addr | size | status | role |", "|------|-----:|--------|------|"]
        for f in rows:
            role = arch_role(int(f["addr"], 16))
            st = "matched" if f["status"] == "matched" else "decoded" if f.get("src") else f["status"]
            out.append("| `%s` | %d | %s | %s |" % (f["addr"], f["size"], st, role))
        open(d + "/_index.md", "w", encoding="utf-8", newline="\n").write("\n".join(out) + "\n")

    print("\nmoved %d files into %d subsystem dirs; wrote _index.md each." % (moved, len(buckets)))


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""apply_names.py -- RENAME functions from FUN_<addr> to their semantic names (from tools/names.py
LABELS), everywhere consistently, so the SOURCE reads with real names. The anchor becomes the manifest
`name` (== filename == the name tools take); FUN_<addr> stops being special.

For each (addr -> new_name) that has a source file:
  * git mv src/<sub>/FUN_<addr>.c  ->  src/<sub>/<new_name>.c
  * global token replace \\bFUN_<addr>\\b -> <new_name> across all src/**/*.c (definition, externs, calls,
    header comments; the __db_FUN_<addr>_N helper labels are left alone -- not word-bounded)
  * manifest: name -> new_name, src -> new path
  * recipes.json: key FUN_<addr> -> new_name
Symbols/relocs don't affect the byte match, and GAMEO uses the whole original image, so nothing about
the running build changes -- only the byte-match verification cares, and it keys on the manifest name.

  python3 tools/apply_names.py [--dry]
"""
import json, os, glob, re, sys, subprocess, importlib.util

spec = importlib.util.spec_from_file_location("names", "tools/names.py")
names_mod = importlib.util.module_from_spec(spec); spec.loader.exec_module(names_mod)
LABELS = names_mod.LABELS

DRY = "--dry" in sys.argv
man = json.load(open("manifest/functions.json"))
by_addr = {f["addr"]: f for f in man["functions"]}
recipes = json.load(open("manifest/recipes.json"))
srcmap = {os.path.basename(p)[:-2]: p for p in glob.glob("src/**/*.c", recursive=True)}

renames = []
for addr, new in LABELS.items():
    f = by_addr.get(addr)
    if not f:
        continue
    old = f["name"]
    if old == new:
        continue
    src = srcmap.get(old) or (f.get("src") if f.get("src") and os.path.exists(f["src"]) else None)
    if not src or not os.path.exists(src):
        print("skip (no src): %s -> %s" % (old, new)); continue
    if new in srcmap:
        print("skip (name collision): %s -> %s" % (old, new)); continue
    newsrc = os.path.join(os.path.dirname(src), new + ".c").replace("\\", "/")
    renames.append((addr, old, new, src.replace("\\", "/"), newsrc))

print("%d functions to rename%s" % (len(renames), " (dry run)" if DRY else ""))
for addr, old, new, src, newsrc in renames:
    print("  %-14s -> %-24s  (%s)" % (old, new, newsrc))
if DRY or not renames:
    sys.exit(0)

# 1+2. single pass: global token replace across all source, WRITING renamed files to their new path
# directly (write-new + remove-old, no git mv -> avoids the mount's overwrite-permission race).
rmap = {old: new for _, old, new, _, _ in renames}
pat = re.compile(r"\b(" + "|".join(re.escape(o) for o in rmap) + r")\b")
path_rename = {src: newsrc for _, _, _, src, newsrc in renames}
for p in glob.glob("src/**/*.c", recursive=True):
    p = p.replace("\\", "/")
    t = open(p, encoding="utf-8", errors="replace").read()
    nt = pat.sub(lambda m: rmap[m.group(1)], t)
    target = path_rename.get(p, p)
    if target != p:
        open(target, "w", encoding="utf-8", newline="\n").write(nt)
        os.remove(p)
    elif nt != t:
        open(target, "w", encoding="utf-8", newline="\n").write(nt)

# 3. manifest name + src (preserve the compact one-fn-per-line format)
for addr, old, new, src, newsrc in renames:
    by_addr[addr]["name"] = new
    by_addr[addr]["src"] = newsrc
with open("manifest/functions.json", "w", newline="\n") as f:
    f.write("{\n")
    for k in ("binary", "segment", "image_base", "language"):
        f.write('  "%s": %s,\n' % (k, json.dumps(man[k])))
    f.write('  "functions": [\n')
    rows = [json.dumps(fn, separators=(", ", ": ")) for fn in man["functions"]]
    f.write(",\n".join("    " + r for r in rows))
    f.write("\n  ]\n}\n")

# 4. recipes key rename
for addr, old, new, src, newsrc in renames:
    if old in recipes:
        recipes[new] = recipes.pop(old)
with open("manifest/recipes.json", "w", newline="\n") as f:
    json.dump(recipes, f, indent=1)

print("renamed %d functions (files + symbols + manifest + recipes)." % len(renames))

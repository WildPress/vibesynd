#!/usr/bin/env python3
"""apply_match.py -- apply permuter wins safely. For each src/**/<name>.c.match produced by cpermute.py,
install it as <name>.c, recompile with the recipe flags and VERIFY a reloc-aware byte match. On success:
set the manifest status=matched, record the recipe in manifest/recipes.json, and delete the .match. On
failure: REVERT the .c to its previous content and report (never leave a non-matching source installed).

Run in-container (needs the period Watcom compiler):
  docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/apply_match.py
Env MATCH_FLAGS overrides the verify flags (default the standard game recipe).
"""
import json, os, glob, subprocess, shutil, sys

FLAGS = os.environ.get("MATCH_FLAGS", "-4s -oneatx -zp8 -s -zq")
man = json.load(open("manifest/functions.json"))
by_name = {f["name"]: f for f in man["functions"]}
recipes = json.load(open("manifest/recipes.json"))


def verify(name, flags):
    r = subprocess.run(["bash", "tools/match95.sh", name, flags], capture_output=True, text=True)
    return ("RELOC-AWARE match (masked): YES" in r.stdout or
            "JUMP-TABLE-AWARE match    : YES" in r.stdout)


applied, failed = [], []
for mf in sorted(glob.glob("src/**/*.c.match", recursive=True)):
    c = mf[:-6]                                  # strip ".match" -> the .c path
    name = os.path.basename(c)[:-2]
    prev = open(c, encoding="utf-8", errors="replace").read() if os.path.exists(c) else None
    shutil.copyfile(mf, c)
    if verify(name, FLAGS):
        f = by_name.get(name)
        if f:
            f["status"] = "matched"
            recipes[name] = {"addr": f["addr"], "flags": FLAGS}
        os.remove(mf)
        applied.append(name)
        print("APPLIED  %s (byte-match verified)" % name)
    else:
        if prev is not None:
            open(c, "w", encoding="utf-8", newline="\n").write(prev)
        failed.append(name)
        print("REJECTED %s (.match did not verify; reverted)" % name)

if applied:
    # manifest: compact one-fn-per-line (same writer as apply_names.py)
    with open("manifest/functions.json", "w", newline="\n") as fp:
        fp.write("{\n")
        for k in ("binary", "segment", "image_base", "language"):
            if k in man:
                fp.write('  "%s": %s,\n' % (k, json.dumps(man[k])))
        fp.write('  "functions": [\n')
        rows = [json.dumps(fn, separators=(", ", ": ")) for fn in man["functions"]]
        fp.write(",\n".join("    " + r for r in rows))
        fp.write("\n  ]\n}\n")
    with open("manifest/recipes.json", "w", newline="\n") as fp:
        json.dump(recipes, fp, indent=1)

print("\napplied %d, rejected %d" % (len(applied), len(failed)))

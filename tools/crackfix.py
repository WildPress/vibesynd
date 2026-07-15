#!/usr/bin/env python3
"""crackfix.py -- crack "dead callee-save" parked functions by declaring their callees
`#pragma aux <callee> modify [reg]` (the mechanism proven on 0x36168).

`modify [reg]` only adds reg to the callee's clobber set -- it does NOT change arg
passing -- so it's safe to add to every called extern. Watcom then saves/restores that
callee-saved reg around the whole body (to honor this function's own callee-save
contract), reproducing the phantom push/pop the target has.

For each NAME on the command line (or all detected CALLEE-SAVE fns from a list file):
  1. read source, find every called extern function name
  2. inject a `#pragma aux <name> modify [<regs>];` block for each
  3. recompile + reloc-verify; on success mark matched + record recipe; else revert.

Run in-container:
  docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/crackfix.py NAME:regs [NAME:regs ...]
    e.g. tools/crackfix.py FUN_00037818:ebx FUN_00034048:esi+edi
"""
import os, sys, re, json, glob, subprocess

FLAGS = "-4s -oneatx -zp8 -s -zq"
man = json.load(open("manifest/functions.json"))
by_name = {f["name"]: f for f in man["functions"]}
recipes = json.load(open("manifest/recipes.json"))
srcmap = {os.path.basename(p)[:-2]: p for p in glob.glob("src/**/*.c", recursive=True)}

# names this file declares/defines but that are NOT callees to tag
CALL_RE = re.compile(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(")
DECL_RE = re.compile(r"^\s*extern\b.*?\b([A-Za-z_][A-Za-z0-9_]*)\s*\(", re.M)


def verify(name):
    r = subprocess.run(["bash", "tools/match95.sh", name], capture_output=True, text=True)
    return "RELOC-AWARE match (masked): YES" in r.stdout or "JUMP-TABLE-AWARE match    : YES" in r.stdout


def callees(txt, self_name):
    """External function names that are both declared extern AND called in the body."""
    declared = set(DECL_RE.findall(txt))
    called = set(CALL_RE.findall(txt))
    return sorted((declared & called) - {self_name, "if", "while", "for", "switch", "return", "sizeof"})


def inject(txt, names, regs):
    modblock = "\n".join("#pragma aux %s modify [%s];" % (n, regs) for n in names)
    # insert after the last extern declaration line (before the function body)
    lines = txt.splitlines(keepends=True)
    last_extern = 0
    for i, ln in enumerate(lines):
        if ln.lstrip().startswith("extern") or ln.lstrip().startswith("#pragma"):
            last_extern = i + 1
    # avoid duplicate pragmas
    if "modify [%s]" % regs in txt and all(("aux %s modify" % n) in txt for n in names):
        return txt
    return "".join(lines[:last_extern]) + modblock + "\n" + "".join(lines[last_extern:])


def main():
    specs = []
    for a in sys.argv[1:]:
        if ":" in a:
            n, r = a.split(":", 1); specs.append((n, r.replace("+", " ")))
    ok, bad = [], []
    for name, regs in specs:
        p = srcmap.get(name)
        if not p:
            bad.append((name, "no source")); continue
        orig = open(p, encoding="utf-8", errors="replace").read()
        cs = callees(orig, name)
        if not cs:
            bad.append((name, "no callees found")); continue
        new = inject(orig, cs, regs)
        open(p, "w", newline="\n").write(new)
        if verify(name):
            f = by_name[name]; f["status"] = "matched"; f["match_pct"] = 100.0
            recipes[name] = {"addr": f["addr"], "flags": FLAGS}
            ok.append((name, regs, cs)); print("MATCHED  %s  modify[%s] on %s" % (name, regs, cs))
        else:
            open(p, "w", newline="\n").write(orig)
            bad.append((name, "did not verify")); print("FAIL     %s  (reverted)" % name)
    if ok:
        with open("manifest/functions.json", "w", newline="\n") as fp:
            fp.write("{\n")
            for k in ("binary", "segment", "image_base", "language"):
                if k in man: fp.write('  "%s": %s,\n' % (k, json.dumps(man[k])))
            fp.write('  "functions": [\n')
            fp.write(",\n".join("    " + json.dumps(fn, separators=(", ", ": ")) for fn in man["functions"]))
            fp.write("\n  ]\n}\n")
        with open("manifest/recipes.json", "w", newline="\n") as fp:
            json.dump(recipes, fp, indent=1)
    print("\nmatched %d, failed %d" % (len(ok), len(bad)))
    for n, why in bad: print("  FAIL %s: %s" % (n, why))

if __name__ == "__main__":
    main()

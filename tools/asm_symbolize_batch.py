#!/usr/bin/env python3
"""Round-trip verify tools/asm_symbolize.py across all db-transcription functions and
report the pass rate (how much of the asm the native-relocation pipeline handles).

A function is a db-transcription if its src file contains "db " pragmas. Usage:
  python tools/asm_symbolize_batch.py [limit]
"""
import json, os, sys, subprocess, glob

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def is_asm(srcpath):
    p = os.path.join(ROOT, srcpath.replace("/", os.sep))
    try:
        return '"db ' in open(p, encoding="utf-8", errors="ignore").read()
    except OSError:
        return False

def main():
    limit = int(sys.argv[1]) if len(sys.argv) > 1 else 100000
    fns = json.load(open(os.path.join(ROOT, "manifest", "functions.json")))["functions"]
    asm = [f for f in fns if f.get("src") and is_asm(f["src"]) and int(f.get("size", 0)) > 0]
    asm.sort(key=lambda f: int(f["addr"], 16))
    asm = asm[:limit]
    npass = nfail = 0
    fails = []
    for f in asm:
        r = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "asm_symbolize.py"),
                            "--verify", f["name"], f["addr"], str(f["size"])],
                           capture_output=True, text=True)
        line = (r.stdout + r.stderr).strip().splitlines()
        ok = r.returncode == 0
        if ok:
            npass += 1
        else:
            nfail += 1
            reason = line[0] if line else "?"
            fails.append((f["name"], f["addr"], f["size"], reason))
    print("=" * 60)
    print("asm functions tested: %d   PASS %d   FAIL %d   (%.1f%%)"
          % (len(asm), npass, nfail, 100.0 * npass / max(1, len(asm))))
    if fails:
        print("\nfailures (first 30):")
        from collections import Counter
        cats = Counter()
        for name, addr, size, reason in fails:
            short = reason.split(":")[1].strip()[:40] if ":" in reason else reason[:40]
            cats[short] += 1
        for name, addr, size, reason in fails[:30]:
            print("  %-28s @%s %5sB  %s" % (name, addr, size, reason[:70]))
        print("\nfailure categories:")
        for cat, n in cats.most_common():
            print("  %3d  %s" % (n, cat))

if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""dbgen.py -- auto-generate a db-transcription src/<name>.c for a hand-asm / library
function, byte-matching it mechanically.

WHY: match_reloc.py compares our compiled .obj positionally against linear.bin and masks only
the fixups present in OUR object. A function body emitted as PURE literal `db` bytes has zero
fixups, and every call/jmp displacement + abs32 operand read straight from linear.bin already
equals the resolved bytes there. So a literal-db transcription is an EXACT byte match with no
masking and no symbolic call decls needed (cont. 25 finding). This makes transcription of the
hand-asm sound driver (0x39xxx) and the regs-first-prologue CLIB runtime (0x3a000+) purely
mechanical -- exactly what this tool does.

The emitted src is a `void FUN(void){ __body(); }` wrapper whose body is the transcription across
sequential `#pragma aux` helpers (split for the ~1024-char DOS wcc386 line limit). The trailing
`c3` (ret) is dropped -- the frameless `-3s` wrapper's own epilogue supplies it. A `modify exact`
on each helper + `modify` on the wrapper suppress any wrapper push/pop.

USAGE (in-container):
  python3 tools/dbgen.py <FUN_NAME> [<FUN_NAME> ...]     # generate src for each
  python3 tools/dbgen.py --verify <FUN_NAME> ...          # generate + compile-check each
Only writes a file if it does NOT already exist, UNLESS --force (so it never clobbers a
hand-written game-code reconstruction). Preserves an existing file's top comment as the header.

⚠ This is for HAND-ASM and LIBRARY code (byte-parity is the goal, C isn't meaningful). Do NOT use
it to shortcut a game-logic C reconstruction -- game code should read as decompiled C.
"""
import json, os, sys, subprocess, glob

MAN = "manifest/functions.json"
SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
RECIPE = "-3s -oneatx -zp8 -s -zq"
# db-items per helper pragma. TWO hard wcc386 limits bound this (cont.25 debug):
#  (1) a compilation unit tolerates at most ~18-19 inline `#pragma aux` helper symbols;
#      beyond that wcc386 SILENTLY emits an empty _TEXT (obj compiles, function = 0 bytes).
#      NOT name-length driven -- it is a raw pragma-symbol count ceiling.
#  (2) a source LINE over ~1000 chars HARD-fails the DOS compile (no OBJ); ~800 is safe.
#      Each helper must also stay <~100 bytes to be INLINED (else Watcom tail-jmps to it).
# So we size helpers to keep count <= ~16 while the line stays ~800 chars and the pragma
# body stays inline-able. Ceiling on matchable size: ~16 * ~88 ~= 1400B (hard cap ~1600B).
MAX_HELPERS = 16
MAX_ITEMS = 88                        # ~800-char line, still inline-able (< ~100B body)
MIN_ITEMS = 48


def helper_items(n):
    """Pick db-items/helper so helper count stays <= MAX_HELPERS without over-long lines."""
    it = max(MIN_ITEMS, -(-n // MAX_HELPERS))     # ceil(n / MAX_HELPERS)
    return min(it, MAX_ITEMS)


def fn_bytes(f, base):
    addr = int(f["addr"], 16)
    if addr < base and os.path.exists("build/obj1_full.bin"):     # prefix fn: not in linear.bin
        return open("build/obj1_full.bin", "rb").read()[addr - 0xd748:addr - 0xd748 + f["size"]]
    off = addr - base
    return open(SEG, "rb").read()[off:off + f["size"]]


def gen_src(name, body, header, aborts=False):
    """body = the function bytes to emit as db. If aborts, the wrapper is marked `aborts`
    so Watcom appends NO trailing ret (for no-ret / RET-N / tail-jmp / borrowed-epilogue fns);
    caller must pass the FULL body. Otherwise the trailing `c3` was dropped and the wrapper's
    epilogue supplies the ret."""
    li = helper_items(len(body))
    parts = [body[i:i + li] for i in range(0, len(body), li)]
    lines = [header.rstrip() + "\n"]
    for i, p in enumerate(parts):
        db = " ".join(f'"db {b}"' for b in p)
        lines.append(f"extern void __db_{name}_{i}(void);")
        lines.append(f"#pragma aux __db_{name}_{i} = {db} "
                     f"modify exact [eax ebx ecx edx esi edi ebp];")
    ab = " aborts" if aborts else ""
    lines.append(f"#pragma aux {name} modify [eax ebx ecx edx esi edi ebp]{ab};")
    lines.append(f"void {name}(void) {{")
    for i in range(len(parts)):                 # one call per line (huge fns: call-list would
        lines.append(f"    __db_{name}_{i}();")  # otherwise blow the ~560-char source-line limit)
    lines.append("}")
    return "\n".join(lines) + "\n"


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    verify = "--verify" in sys.argv
    force = "--force" in sys.argv
    man = json.load(open(MAN)); base = int(man["image_base"], 16)
    by = {f["name"]: f for f in man["functions"]}
    done, fail = [], []
    for name in args:
        f = by.get(name)
        if not f:
            print(f"{name}: not in manifest"); fail.append(name); continue
        # src files live in subsystem subdirs; reuse the existing location, else default to lib/runtime
        _hits = glob.glob(f"src/**/{name}.c", recursive=True)
        path = _hits[0] if _hits else f"src/lib/runtime/{name}.c"
        os.makedirs(os.path.dirname(path), exist_ok=True)
        header = f"/* @ 0x{int(f['addr'],16):x} ({f['size']}B) -- db-transcription (hand-asm/library). */"
        if os.path.exists(path) and not force:
            # preserve the existing header comment (a prior semantic decode)
            txt = open(path, encoding="utf-8", errors="replace").read()
            if txt.lstrip().startswith("/*"):
                header = txt[:txt.index("*/") + 2]
        b = fn_bytes(f, base)
        # Try the plain form first (drop trailing c3); if it doesn't verify, retry with
        # `aborts` + the FULL body (for no-ret / RET-N / tail-jmp / borrowed-epilogue fns).
        attempts = []
        if b and b[-1] == 0xc3:
            attempts.append((b[:-1], False))    # drop ret, wrapper supplies it
        attempts.append((b, True))              # keep all, aborts wrapper (no ret)
        matched = False
        for body, aborts in attempts:
            open(path, "w", newline="\n").write(gen_src(name, body, header, aborts))
            if not verify:
                print(f"{name}: wrote {path} ({f['size']}B, aborts={aborts})")
                matched = True; break
            r = subprocess.run(["bash", "tools/match95.sh", name, RECIPE],
                               capture_output=True, text=True)
            if "RELOC-AWARE match (masked): YES" in r.stdout or "EXACT byte match          : YES" in r.stdout:
                print(f"{name}: MATCH  ({f['size']}B, aborts={aborts})"); matched = True; break
        if verify:
            (done if matched else fail).append(name)
            if not matched:
                print(f"{name}: NO-MATCH  ({f['size']}B)")
    if verify:
        print(f"\n{len(done)} matched, {len(fail)} failed" + (f": {fail}" if fail else ""))


if __name__ == "__main__":
    main()

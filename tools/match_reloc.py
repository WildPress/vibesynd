#!/usr/bin/env python3
"""Relocation-aware match using the .obj's REAL OMF fixup records (tools/omf.py).

Masks exactly the relocated byte ranges in BOTH the target and our compiled output,
so relocations don't block a match: call/jmp rel32, abs32 data operands, AND
relocations that carry a NON-ZERO addend (array bounds, arr[const], loop `cmp`,
`((T*)addr)->field`) which the old all-zero-run masker missed.

  python3 tools/match_reloc.py <function-name>    (honors $WATFLAGS, $SKIP_COMPILE)
"""
import json, sys, subprocess, os
from omf import text_bytes_and_fixups

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
MANIFEST = "manifest/functions.json"

def mask(b, fixups):
    b = bytearray(b)
    for off, size in fixups:
        for j in range(off, min(off + size, len(b))):
            b[j] = 0
    return bytes(b)

def leading_jumptable_entries(fixups):
    """Count contiguous 4-byte fixups starting at obj offset 0 (0,4,8,...).
    A `switch` makes Watcom co-locate its jump table at the START of the _TEXT
    contribution: N consecutive relocated dwords. Real function code never begins
    with a run of 4-byte relocations, so a run of >=4 (16 bytes) is an unambiguous
    inline-jump-table signature."""
    offs = {o: s for o, s in fixups}
    k = 0
    while offs.get(4 * k) == 4:
        k += 1
    return k

def jumptable_aware_match(tb, ob, fixups):
    """For inline-jump-table `switch` fns: the real binary keeps the table in a far
    segment (on-disk body is clean code), but our .obj is [table][align-pad][code].
    We must find where the code actually starts -- the align pad is 0..32 bytes of
    multi-byte NOPs, so the tail assumption (codestart = len(ob)-len(tb)) is WRONG
    whenever pad != len(ob)-table_len-len(tb). Sweep the pad, re-base the code-region
    fixups, and pick the alignment with the fewest masked code-byte differences.

    Returns (matched, table_len, pad, code_diff): matched is True only on EXACT masked
    code equality with the code length == target; code_diff is the honest code-only
    distance (table + pad excluded) for the near-miss report. Gated on a >=4-entry
    table run, so a wrong split can only inflate code_diff, never false-match."""
    K = leading_jumptable_entries(fixups)
    if K < 4:
        return False, 0, 0, None
    table_len = 4 * K
    best = None  # (pad, ndiff, complen)
    for pad in range(0, 33):
        cs = table_len + pad
        if cs >= len(ob):
            break
        code = ob[cs:cs + len(tb)]                 # may be short near the obj end
        n = min(len(code), len(tb))
        cfx = [(o - cs, s) for o, s in fixups if cs <= o < cs + n]
        tm, cm = mask(tb[:n], cfx), mask(code[:n], cfx)
        nd = sum(1 for a, b in zip(tm, cm) if a != b) + abs(len(tb) - len(code))
        if best is None or nd < best[1]:
            best = (pad, nd, n)
    pad, nd, _ = best
    matched = (nd == 0 and (len(ob) - (table_len + pad)) == len(tb))
    return matched, table_len, pad, nd

def main():
    name = sys.argv[1]
    man = json.load(open(MANIFEST)); base = int(man.get("image_base", "0"), 16)
    f = next(x for x in man["functions"] if x["name"] == name)
    if not os.environ.get("SKIP_COMPILE"):
        if subprocess.run(["bash", "tools/wcompile.sh", name]).returncode:
            sys.exit("compile failed")
    off = int(f["addr"], 16) - base
    tb = open(SEG, "rb").read()[off:off + f["size"]]
    ob, fixups = text_bytes_and_fixups(f"build/{name}.obj")
    tm, om = mask(tb, fixups), mask(ob, fixups)
    print(f"\n=== {name}  addr={f['addr']}  target={len(tb)}B  ours={len(ob)}B ===")
    print(f"target: {tb.hex()}")
    print(f"ours  : {ob.hex()}")
    print(f"fixups masked: {[(hex(o), s) for o, s in fixups]}")
    print(f"EXACT byte match          : {'YES' if tb == ob else 'NO'}")
    ok = (len(tb) == len(ob)) and (tm == om)
    print(f"RELOC-AWARE match (masked): {'YES ✅' if ok else 'NO'}")
    if not ok:
        # Inline-jump-table `switch` fns: our obj is [table][pad][code]; locate the code
        # (pad-aware) and compare against the clean target (table lives far, on-disk).
        jt_ok, table_len, pad, code_diff = jumptable_aware_match(tb, ob, fixups)
        if jt_ok:
            print(f"  (inline jump table: {table_len}B table + {pad}B align pad "
                  f"before code; compared code region only)")
            print(f"JUMP-TABLE-AWARE match    : YES ✅")
            ok = True
        else:
            n = min(len(tm), len(om))
            d = next((i for i in range(n) if tm[i] != om[i]), n)
            print(f"  first diff at 0x{d:x}: target={tm[d:d+1].hex()} ours={om[d:d+1].hex()}")
            if code_diff is not None:
                # honest near-miss: the whole-object diff counts the inline table as
                # inserted bytes; report the code-only distance (table + pad excluded).
                print(f"  inline jump table: {table_len}B table + {pad}B pad; "
                      f"CODE-ONLY diff (table excluded): {code_diff} bytes")

if __name__ == "__main__":
    main()

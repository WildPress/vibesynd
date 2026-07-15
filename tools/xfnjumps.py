#!/usr/bin/env python3
"""xfnjumps.py -- find parked functions whose TARGET has a jcc/jmp landing OUTSIDE the
function's own [addr, addr+size) range: a cross-function shared block / tail-merge (like
0x37818 jumping back into 0x377e8's return-0 stub). These are candidates for a whole-module
build (merge with the neighbour that owns the branch target). Prints the branch target and
which function owns it."""
import json, capstone
IMG = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
man = json.load(open("manifest/functions.json"))["functions"]
byaddr = sorted((int(f["addr"], 16), f) for f in man)


def owner(addr):
    lo = None
    for a, f in byaddr:
        if a <= addr:
            lo = (a, f)
        else:
            break
    if lo and lo[0] <= addr < lo[0] + lo[1].get("size", 0):
        return lo[1]["name"], lo[0]
    return None, None


def main():
    parked = [f for f in man if f.get("status") != "matched"]
    hits = []
    for f in sorted(parked, key=lambda x: x.get("size", 0)):
        addr, size = int(f["addr"], 16), f.get("size", 0)
        code = IMG[addr - 0x10000: addr - 0x10000 + size]
        for ins in md.disasm(code, addr):
            if ins.group(capstone.CS_GRP_JUMP):     # jcc/jmp (not call)
                try:
                    tgt = int(ins.op_str, 16)
                except ValueError:
                    continue
                if not (addr <= tgt < addr + size):  # lands outside this function
                    onm, oaddr = owner(tgt)
                    hits.append((f["name"], addr, size, ins.mnemonic, tgt, onm, oaddr))
    print("=== parked fns with cross-function branches (whole-module candidates) ===")
    for nm, a, sz, mn, tgt, onm, oaddr in hits:
        rel = "SELF" if onm == nm else (f"-> {onm}" if onm else "-> (gap/none)")
        print(f"{nm} @ {a:#x} ({sz}B): {mn} {tgt:#x}  {rel}"
              + (f" @ {oaddr:#x}" if oaddr else ""))
    print(f"\n{len(hits)} cross-fn branches across {len(set(h[0] for h in hits))} parked fns")

if __name__ == "__main__":
    main()

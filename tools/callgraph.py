#!/usr/bin/env python3
"""Static call graph for OBJECT1. Scans the code segment for near-CALL/JMP
(E8/E9 rel32) whose target lands exactly on a known function start, and builds
caller<->callee maps. Naive (no full disassembly) but the "lands on a function
start" filter kills almost all false positives, which is enough to navigate the
call tree for subsystem work.

Usage:
  callgraph.py callers <addr>     who calls <addr>
  callgraph.py callees <addr>     what <addr> calls
  callgraph.py tree <addr> [depth] callee tree from <addr> (default depth 2)
"""
import json, sys, struct

m = json.load(open("manifest/functions.json"))
base = int(m["image_base"], 16)
data = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()

funcs = {int(f["addr"], 16): f for f in m["functions"]}
starts = sorted(funcs)

def containing(addr):
    """function whose range contains addr"""
    import bisect
    i = bisect.bisect_right(starts, addr) - 1
    if i < 0:
        return None
    fa = starts[i]
    if fa <= addr < fa + funcs[fa]["size"]:
        return fa
    return None

# build edges: (call_site_addr, caller_func, target_func)
edges = []
for i in range(len(data) - 4):
    op = data[i]
    if op in (0xE8, 0xE9):  # call rel32 / jmp rel32
        rel = struct.unpack_from("<i", data, i + 1)[0]
        target = base + i + 5 + rel
        if target in funcs:
            site = base + i
            cf = containing(site)
            if cf is not None and cf != target:  # ignore self / stray
                edges.append((site, cf, target, op))

callers = {}
callees = {}
for site, cf, tgt, op in edges:
    callers.setdefault(tgt, set()).add(cf)
    callees.setdefault(cf, set()).add(tgt)

def name(a):
    f = funcs.get(a)
    st = "*" if f and f["status"] == "matched" else " "
    return f'{st}{a:#08x} {f["name"] if f else "?"} (sz={f["size"] if f else "?"})'

def show(title, aset):
    print(title)
    for a in sorted(aset):
        print("   ", name(a))
    if not aset:
        print("    (none)")

def tree(a, depth, seen, indent=0):
    print("  " * indent + name(a).strip())
    if depth <= 0 or a in seen:
        return
    seen.add(a)
    for c in sorted(callees.get(a, [])):
        tree(c, depth - 1, seen, indent + 1)

if __name__ == "__main__":
    cmd = sys.argv[1]
    addr = int(sys.argv[2], 16)
    if cmd == "callers":
        show(f"callers of {name(addr).strip()}:", callers.get(addr, set()))
    elif cmd == "callees":
        show(f"callees of {name(addr).strip()}:", callees.get(addr, set()))
    elif cmd == "tree":
        depth = int(sys.argv[3]) if len(sys.argv) > 3 else 2
        tree(addr, depth, set())

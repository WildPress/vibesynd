#!/usr/bin/env python3
"""Search case orderings of a switch to reproduce the original's tree shape.

Watcom's sparse-switch codegen builds a comparison tree whose balance depends on
the order the cases appear in the source. When a reconstruction is correct except
for that tree, the fix is to find the ordering the developers used. This permutes
the case-groups, compiles each (Watcom 9.5), and reports any byte-identical match
plus the size distribution (so you can see whether the target tree is reachable at
all under the current flags/C).

  docker run --rm -v "$PWD":/work -w /work synd-decomp \
      python3 tools/permute.py FUN_00020d98 "-4s -oneatx -zp8 -s -zq" [--max 400]

The switch body is found between the line containing 'switch' and its closing '}'.
Each case-group runs from a line starting with 'case' through the line with 'break'.
The original file is restored on exit.
"""
import json, subprocess, sys, re, itertools, random, os, time

def parse_args(argv):
    name = argv[1]
    flags = argv[2]
    mx = 400
    if "--max" in argv:
        mx = int(argv[argv.index("--max") + 1])
    return name, flags, mx

def split_switch(lines):
    """return (pre, indent, groups, post). groups is a list of line-lists."""
    si = next(i for i, l in enumerate(lines) if re.match(r"\s*switch\s*\(", l))
    # find the closing brace of the switch by brace counting from the '{'
    depth = 0
    started = False
    ci = None
    for i in range(si, len(lines)):
        depth += lines[i].count("{") - lines[i].count("}")
        if "{" in lines[i]:
            started = True
        if started and depth == 0:
            ci = i
            break
    body = lines[si + 1:ci]
    groups, cur = [], []
    for l in body:
        if re.match(r"\s*case\b", l) and cur and "break" in cur[-1]:
            groups.append(cur); cur = []
        cur.append(l)
    if cur:
        groups.append(cur)
    return lines[:si + 1], groups, lines[ci:]

DEBUG = "--debug" in sys.argv

# Best run INSIDE the container (docker run ... python3 tools/permute.py ...): the
# write and the compile then share one /work mount, so an fsync makes each change
# visible. On the host, DOSBox reads can race the drvfs write. Auto-detect: if the
# docker CLI is absent we're already inside, so call bash directly.
INSIDE = subprocess.run(["which", "docker"], capture_output=True).returncode != 0
PREFIX = [] if INSIDE else ["docker", "run", "--rm", "-v", os.getcwd() + ":/work",
                            "-w", "/work", "synd-decomp"]

def write_settled(path, content):
    """write and flush so the compile reliably sees the new content (drvfs lag)."""
    with open(path, "w") as f:
        f.write(content)
        f.flush()
        os.fsync(f.fileno())
    time.sleep(0.2)

def compile_size_match(name, flags):
    r = subprocess.run(PREFIX + ["bash", "tools/match95.sh", name, flags],
                       capture_output=True, text=True)
    out = r.stdout
    m = re.search(r"ours=(\d+)B", out)
    size = int(m.group(1)) if m else -1
    matched = "RELOC-AWARE match (masked): YES" in out
    tgt = re.search(r"target=(\d+)B", out)
    target = int(tgt.group(1)) if tgt else -1
    if size == -1 and DEBUG:
        print("---- compile failed; stdout+stderr ----")
        print((out + r.stderr)[-800:])
        print("---------------------------------------")
    return size, matched, target

def main():
    name, flags, mx = parse_args(sys.argv)
    src = f"src/{name}.c"
    original = open(src).read()
    lines = original.split("\n")
    pre, groups, post = split_switch(lines)
    n = len(groups)
    total = 1
    for k in range(2, n + 1):
        total *= k
    print(f"{name}: {n} case-groups, {total} orderings; sampling up to {mx}")

    if total <= mx:
        orders = list(itertools.permutations(range(n)))
    else:
        seen, orders = set(), []
        base = tuple(range(n))
        orders.append(base); seen.add(base)
        while len(orders) < mx:
            p = list(range(n)); random.shuffle(p); p = tuple(p)
            if p not in seen:
                seen.add(p); orders.append(p)

    best = None
    sizes = {}
    try:
        for idx, order in enumerate(orders):
            body = []
            for gi in order:
                body.extend(groups[gi])
            content = "\n".join(pre + body + post)
            write_settled(src, content)
            size, matched, target = compile_size_match(name, flags)
            if size == -1:  # tolerate a transient /mnt/c or DOSBox hiccup: retry once
                write_settled(src, content)
                size, matched, target = compile_size_match(name, flags)
            sizes[size] = sizes.get(size, 0) + 1
            if best is None or abs(size - target) < abs(best[1] - target):
                best = (order, size, target)
            if matched:
                print(f"\n*** MATCH at ordering #{idx}: {order} ***")
                open(src + ".match", "w").write("\n".join(pre + body + post))
                print(f"    saved to {src}.match")
                return
            if idx % 25 == 0:
                print(f"  [{idx}/{len(orders)}] sizes so far: "
                      f"{dict(sorted(sizes.items()))}")
    finally:
        open(src, "w").write(original)
    print(f"\nno exact match. target={best[2]}B. size histogram:")
    for s, c in sorted(sizes.items()):
        flag = "  <- TARGET SIZE" if s == best[2] else ""
        print(f"   {s}B : {c}{flag}")
    print(f"closest ordering {best[0]} -> {best[1]}B")

if __name__ == "__main__":
    main()

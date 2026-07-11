#!/usr/bin/env python3
"""PARALLEL case-order search (Watcom 9.5, DOSBox). Fans the orderings across N
worker processes, each with its own DOSBox work dir + temp src/obj, so a 32-core
box can exhaust the whole permutation space. Run INSIDE the container:

  docker run --rm -v "$PWD":/work -w /work synd-decomp \
      python3 tools/permute_par.py FUN_00020d98 "-4s -oneatx -zp8 -s -zq" [--workers 24] [--max N]

Reports any byte-identical ordering (saves src/<name>.c.match) and a size
histogram. Restores the original source on exit.
"""
import json, subprocess, sys, re, itertools, random, os
import multiprocessing as mp
from omf import text_bytes_and_fixups

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
MAN = "manifest/functions.json"

def split_switch(lines):
    si = next(i for i, l in enumerate(lines) if re.match(r"\s*switch\s*\(", l))
    depth, started, ci = 0, False, None
    for i in range(si, len(lines)):
        depth += lines[i].count("{") - lines[i].count("}")
        if "{" in lines[i]:
            started = True
        if started and depth == 0:
            ci = i; break
    body = lines[si + 1:ci]
    groups, cur = [], []
    for l in body:
        if re.match(r"\s*case\b", l) and cur and "break" in cur[-1]:
            groups.append(cur); cur = []
        cur.append(l)
    if cur:
        groups.append(cur)
    return lines[:si + 1], groups, lines[ci:]

def mask(b, fixups):
    b = bytearray(b)
    for off, size in fixups:
        for j in range(off, min(off + size, len(b))):
            b[j] = 0
    return bytes(b)

# globals set per-worker
G = {}

def init_worker(pre, groups, post, flags, target):
    wid = (mp.current_process()._identity or [0])[0]
    work = f"/tmp/dosw_{wid}"   # native VM fs, NOT the slow /mnt/c drvfs mount
    os.makedirs(work, exist_ok=True)
    G.update(pre=pre, groups=groups, post=post, flags=flags, target=target,
             wid=wid, work=work)

def render(order):
    body = []
    for gi in order:
        body.extend(G["groups"][gi])
    return "\n".join(G["pre"] + body + G["post"])

def try_batch(batch):
    """compile a whole batch in ONE DOSBox session (amortises startup)."""
    W = G["work"]
    subprocess.run(f"rm -f {W}/SRC*.C {W}/O*.OBJ", shell=True)
    for j, order in enumerate(batch):
        with open(f"{W}/SRC{j:02d}.C", "w") as f:
            f.write(render(order)); f.flush(); os.fsync(f.fileno())
    subprocess.run(["bash", "tools/wcc95_batch.sh", W, G["flags"]], capture_output=True)
    tb, out = G["target"], []
    for j, order in enumerate(batch):
        objp = f"{W}/O{j:02d}.OBJ"
        if not os.path.exists(objp):
            out.append((order, -1, False)); continue
        try:
            ob, fx = text_bytes_and_fixups(objp)
        except Exception:
            out.append((order, -1, False)); continue
        out.append((order, len(ob), len(tb) == len(ob) and mask(tb, fx) == mask(ob, fx)))
    return out

def main():
    name = sys.argv[1]
    flags = sys.argv[2]
    workers = int(sys.argv[sys.argv.index("--workers") + 1]) if "--workers" in sys.argv else 24
    mx = int(sys.argv[sys.argv.index("--max") + 1]) if "--max" in sys.argv else 0
    B = int(sys.argv[sys.argv.index("--batch") + 1]) if "--batch" in sys.argv else 50

    man = json.load(open(MAN)); base = int(man["image_base"], 16)
    f = next(x for x in man["functions"] if x["name"] == name)
    off = int(f["addr"], 16) - base
    target = open(SEG, "rb").read()[off:off + f["size"]]

    src = f"src/{name}.c"
    original = open(src).read()
    pre, groups, post = split_switch(original.split("\n"))
    n = len(groups)
    total = 1
    for k in range(2, n + 1):
        total *= k
    orders = list(itertools.permutations(range(n)))
    if mx and mx < len(orders):
        random.seed(0); random.shuffle(orders); orders = orders[:mx]
    # stage the compiler tree on the native fs so DOSBox isn't reading it over
    # drvfs on every compile; workers pick it up via WAT_ROOT.
    if not os.path.isdir("/tmp/wat"):
        subprocess.run("cp -r /work/toolchain/watcom95 /tmp/wat", shell=True)
    os.environ["WAT_ROOT"] = "/tmp/wat"

    batches = [orders[i:i + B] for i in range(0, len(orders), B)]
    print(f"{name}: {n} groups, {total} total orderings, testing {len(orders)} in "
          f"{len(batches)} batches of {B} on {workers} workers (target={len(target)}B)",
          flush=True)

    sizes, best, done = {}, None, 0
    try:
        pool = mp.Pool(workers, initializer=init_worker,
                       initargs=(pre, groups, post, flags, target))
        for results in pool.imap_unordered(try_batch, batches):
            for order, size, matched in results:
                done += 1
                sizes[size] = sizes.get(size, 0) + 1
                if size != -1 and (best is None or abs(size - len(target)) < abs(best[1] - len(target))):
                    best = (order, size)
                if matched:
                    body = []
                    for gi in order:
                        body.extend(groups[gi])
                    open(src + ".match", "w").write("\n".join(pre + body + post))
                    print(f"\n*** MATCH: ordering {order} -> saved {src}.match ***", flush=True)
                    pool.terminate(); return
            print(f"  [{done}/{len(orders)}] best={best[1] if best else '-'}B "
                  f"sizes={dict(sorted(sizes.items()))}", flush=True)
        pool.close(); pool.join()
    finally:
        pass
    print(f"\nno exact match in {done}. size histogram (target={len(target)}B):", flush=True)
    for s, c in sorted(sizes.items()):
        print(f"   {s}B : {c}{'  <- TARGET' if s == len(target) else ''}")
    print(f"closest: {best}")

if __name__ == "__main__":
    main()

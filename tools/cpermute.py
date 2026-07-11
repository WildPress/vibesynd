#!/usr/bin/env python3
"""AST-based C permuter for our Watcom-9.5 setup. Parses a function with pycparser,
enumerates semantics-preserving mutations (commutative operand swaps), regenerates
each variant, and sweeps them through the fast batched compiler, scored by how many
bytes match the target. A lightweight decomp-permuter tailored to our backend.

  docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/cpermute.py \
      FUN_00033fb8 "-4s -oneatx -zp8 -s -zq" [--workers 20] [--max 4096]

Needs pycparser (pip-installs it with --break-system-packages if missing). Saves a
byte-identical winner to src/<name>.c.match; leaves src/<name>.c untouched.
"""
import json, subprocess, sys, re, os, itertools
import multiprocessing as mp
try:
    from pycparser import c_parser, c_generator, c_ast
except ImportError:
    subprocess.run("pip install -q --break-system-packages pycparser", shell=True)
    from pycparser import c_parser, c_generator, c_ast
from omf import text_bytes_and_fixups

SEG, MAN = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "manifest/functions.json"
COMMUTATIVE = {"+", "*", "==", "!=", "&", "|", "^", "&&", "||"}

def strip_comments(s):
    s = re.sub(r"/\*.*?\*/", "", s, flags=re.S)
    return re.sub(r"//[^\n]*", "", s)

class Collect(c_ast.NodeVisitor):
    def __init__(self): self.sites = []
    def visit_BinaryOp(self, n):
        if n.op in COMMUTATIVE: self.sites.append(n)
        self.generic_visit(n)

def mask(b, fx):
    b = bytearray(b)
    for off, size in fx:
        for j in range(off, min(off + size, len(b))): b[j] = 0
    return bytes(b)

def score(tb, ob, fx):
    """leading matching bytes (works across sizes) -- the hill-climb signal."""
    tm, om = mask(tb, fx), mask(ob, fx)
    n = min(len(tm), len(om))
    lead = 0
    for i in range(n):
        if tm[i] == om[i]: lead += 1
        else: break
    return lead

G = {}
def init_worker(flags, target):
    wid = (mp.current_process()._identity or [0])[0]
    work = f"/tmp/dosw_{wid}"; os.makedirs(work, exist_ok=True)
    G.update(flags=flags, target=target, work=work)

def try_batch(batch):
    """batch = list of (idx, source_string)."""
    W = G["work"]
    subprocess.run(f"rm -f {W}/SRC*.C {W}/O*.OBJ", shell=True)
    for j, (idx, src) in enumerate(batch):
        with open(f"{W}/SRC{j:02d}.C", "w") as f:
            f.write(src); f.flush(); os.fsync(f.fileno())
    subprocess.run(["bash", "tools/wcc95_batch.sh", W, G["flags"]], capture_output=True)
    tb, out = G["target"], []
    for j, (idx, src) in enumerate(batch):
        p = f"{W}/O{j:02d}.OBJ"
        if not os.path.exists(p): out.append((idx, -1, False)); continue
        try: ob, fx = text_bytes_and_fixups(p)
        except Exception: out.append((idx, -1, False)); continue
        matched = len(tb) == len(ob) and mask(tb, fx) == mask(ob, fx)
        out.append((idx, score(tb, ob, fx), matched))
    return out

def main():
    name, flags = sys.argv[1], sys.argv[2]
    workers = int(sys.argv[sys.argv.index("--workers")+1]) if "--workers" in sys.argv else 20
    mx = int(sys.argv[sys.argv.index("--max")+1]) if "--max" in sys.argv else 4096
    B = 40

    man = json.load(open(MAN)); base = int(man["image_base"], 16)
    f = next(x for x in man["functions"] if x["name"] == name)
    off = int(f["addr"], 16) - base
    target = open(SEG, "rb").read()[off:off + f["size"]]

    import random
    ast = c_parser.CParser().parse(strip_comments(open(f"src/{name}.c").read()))
    c = Collect(); c.visit(ast); sites = c.sites          # commutative-swap axes
    fn = next(x for x in ast.ext if isinstance(x, c_ast.FuncDef))
    body = fn.body                                        # statement-reorder axis
    stmts = list(body.block_items or [])
    k = len(stmts)
    gen = c_generator.CGenerator()

    # commutative combos (each site: swap / no-swap)
    comm = list(itertools.product([0, 1], repeat=len(sites)))
    # statement orderings: exhaustive if small, else identity + random samples
    if k <= 6:
        orders = list(itertools.permutations(range(k)))
    else:
        random.seed(0)
        orders = [tuple(range(k))] + [tuple(random.sample(range(k), k)) for _ in range(720)]
    variants = [(cc, so) for so in orders for cc in comm]
    random.seed(1); random.shuffle(variants)
    if len(variants) > mx:
        variants = variants[:mx]
    print(f"{name}: {len(sites)} commutative sites x {len(orders)} statement orders "
          f"-> {len(comm)*len(orders)} variants (testing {len(variants)}), "
          f"target={len(target)}B", flush=True)

    def render(v):
        cc, so = v
        for i, s in enumerate(cc):
            if s: sites[i].left, sites[i].right = sites[i].right, sites[i].left
        body.block_items = [stmts[j] for j in so]
        out = gen.visit(ast)
        body.block_items = stmts
        for i, s in enumerate(cc):
            if s: sites[i].left, sites[i].right = sites[i].right, sites[i].left
        return out
    combos = variants                                    # keep name for the reporting code
    sources = [(i, render(v)) for i, v in enumerate(variants)]

    if not os.path.isdir("/tmp/wat"):
        subprocess.run("cp -r /work/toolchain/watcom95 /tmp/wat", shell=True)
    os.environ["WAT_ROOT"] = "/tmp/wat"
    batches = [sources[i:i+B] for i in range(0, len(sources), B)]
    best = (-1, None); done = 0
    pool = mp.Pool(workers, initializer=init_worker, initargs=(flags, target))
    for results in pool.imap_unordered(try_batch, batches):
        for idx, sc, matched in results:
            done += 1
            if sc > best[0]: best = (sc, combos[idx])
            if matched:
                open(f"src/{name}.c.match", "w").write(render(combos[idx]))
                print(f"\n*** MATCH: swaps={combos[idx]} -> src/{name}.c.match ***", flush=True)
                pool.terminate(); return
        print(f"  [{done}/{len(combos)}] best={best[0]}/{len(target)} bytes match", flush=True)
    pool.close(); pool.join()
    print(f"\nno exact match. best {best[0]}/{len(target)} bytes via swaps={best[1]}", flush=True)

if __name__ == "__main__":
    main()

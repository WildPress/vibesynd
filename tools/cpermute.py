#!/usr/bin/env python3
"""AST-based C permuter for our Watcom-9.5 setup -- the decomp-permuter idea on our
backend. Parses a function with pycparser and applies randomized semantics-preserving
mutations, then sweeps the variants through the fast batched compiler, scored by how
many leading bytes match the target.

Transforms:
  * commutative operand swaps   (a+b -> b+a, a==b -> b==a, ...)
  * statement / declaration reordering
  * temporary introduction      (hoist a subexpression into `int __tN = ...;`)

Randomized search (like the real decomp-permuter): each variant deep-copies the AST,
applies a random subset of mutations, and renders. Invalid variants just fail to
compile and are skipped.

  docker run --rm -v "$PWD":/work -w /work synd-decomp python3 tools/cpermute.py \
      FUN_00033fb8 "-4s -oneatx -zp8 -s -zq" [--workers 24] [--n 4000] [--seed 0]

Needs pycparser (auto-installed). Saves a byte-identical winner to src/<name>.c.match.
"""
import json, subprocess, sys, re, os, copy, random
import multiprocessing as mp
try:
    from pycparser import c_parser, c_generator, c_ast
except ImportError:
    subprocess.run("pip install -q --break-system-packages pycparser", shell=True)
    from pycparser import c_parser, c_generator, c_ast
from omf import text_bytes_and_fixups

SEG, MAN = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "manifest/functions.json"
COMMUTATIVE = {"+", "*", "==", "!=", "&", "|", "^", "&&", "||"}
ARITH = {"+", "-", "*", "/", "%", "<<", ">>", "&", "|", "^"}
# types to try for introduced temps and for swapping existing scalar locals -- the
# signedness/width choice drives movsx/movzx, sar/shr, mov al vs eax, etc.
TYPES = ["int", "unsigned int", "unsigned char", "unsigned short",
         "char", "short", "long", "unsigned"]
PARSER = c_parser.CParser()
GEN = c_generator.CGenerator()

def strip_comments(s):
    s = re.sub(r"/\*.*?\*/", "", s, flags=re.S)
    return re.sub(r"//[^\n]*", "", s)

def body_of(ast):
    fn = next(x for x in ast.ext if isinstance(x, c_ast.FuncDef))
    return fn.body

def parent_map(node, parent=None, pmap=None):
    if pmap is None: pmap = {}
    pmap[id(node)] = parent
    for _, ch in node.children():
        parent_map(ch, node, pmap)
    return pmap

def replace_child(parent, old, new):
    for nm, ch in parent.children():
        if ch is old:
            if nm.endswith("]"):
                attr, idx = nm[:-1].split("["); getattr(parent, attr)[int(idx)] = new
            else:
                setattr(parent, nm, new)
            return True
    return False

def comm_sites(ast):
    out = []
    class V(c_ast.NodeVisitor):
        def visit_BinaryOp(self, n):
            if n.op in COMMUTATIVE: out.append(n)
            self.generic_visit(n)
    V().visit(ast)
    return out

def hoist_sites(body):
    """(stmt_index, node) for hoistable subexpressions (ArrayRef / arithmetic op)."""
    out = []
    for si, stmt in enumerate(body.block_items or []):
        stack = [stmt]
        while stack:
            n = stack.pop()
            for _, ch in n.children():
                if isinstance(ch, c_ast.ArrayRef) or \
                   (isinstance(ch, c_ast.BinaryOp) and ch.op in ARITH):
                    out.append((si, ch))
                stack.append(ch)
    return out

def make_decl(name, expr, typ):
    d = PARSER.parse(f"void __f(void){{ {typ} {name} = {GEN.visit(expr)}; }}")
    return d.ext[0].body.block_items[0]

def type_sites(body):
    """IdentifierType nodes of scalar (non-pointer) local declarations -- swappable."""
    out = []
    for stmt in (body.block_items or []):
        if isinstance(stmt, c_ast.Decl) and isinstance(stmt.type, c_ast.TypeDecl) \
           and isinstance(stmt.type.type, c_ast.IdentifierType):
            out.append(stmt.type.type)
    return out

def apply_mutations(ast, comm_flags, hoist_types, local_types, order_seed):
    body = body_of(ast)
    cs = comm_sites(ast)
    for i, on in enumerate(comm_flags):
        if on and i < len(cs):
            cs[i].left, cs[i].right = cs[i].right, cs[i].left
    # swap existing scalar local types
    ts = type_sites(body)
    for i, t in enumerate(local_types):
        if t and i < len(ts):
            ts[i].names = t.split()
    # temp hoists (each site: None, or a type string to hoist as)
    hs = hoist_sites(body)
    pm = parent_map(ast)
    pre = {}
    tn = 0
    for i, typ in enumerate(hoist_types):
        if not typ or i >= len(hs):
            continue
        si, node = hs[i]
        parent = pm.get(id(node))
        if parent is None:
            continue
        name = f"__t{tn}"; tn += 1
        decl = make_decl(name, node, typ)
        replace_child(parent, node, c_ast.ID(name=name))
        pre.setdefault(si, []).append(decl)
    items = body.block_items or []
    spliced = []
    for si, stmt in enumerate(items):
        spliced.extend(pre.get(si, []))
        spliced.append(stmt)
    # statement/declaration reorder
    if order_seed is not None:
        idx = list(range(len(spliced)))
        random.Random(order_seed).shuffle(idx)
        spliced = [spliced[j] for j in idx]
    body.block_items = spliced

def mask(b, fx):
    b = bytearray(b)
    for off, size in fx:
        for j in range(off, min(off + size, len(b))): b[j] = 0
    return bytes(b)

def score(tb, ob, fx):
    tm, om = mask(tb, fx), mask(ob, fx)
    n = min(len(tm), len(om)); lead = 0
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
        out.append((idx, score(tb, ob, fx), len(tb) == len(ob) and mask(tb, fx) == mask(ob, fx)))
    return out

def main():
    name, flags = sys.argv[1], sys.argv[2]
    workers = int(sys.argv[sys.argv.index("--workers")+1]) if "--workers" in sys.argv else 24
    N = int(sys.argv[sys.argv.index("--n")+1]) if "--n" in sys.argv else 4000
    seed = int(sys.argv[sys.argv.index("--seed")+1]) if "--seed" in sys.argv else 0
    B = 40

    man = json.load(open(MAN)); base = int(man["image_base"], 16)
    f = next(x for x in man["functions"] if x["name"] == name)
    off = int(f["addr"], 16) - base
    target = open(SEG, "rb").read()[off:off + f["size"]]

    src0 = strip_comments(open(f"src/{name}.c").read())
    ast0 = PARSER.parse(src0)
    ncomm = len(comm_sites(ast0)); nhoist = len(hoist_sites(body_of(ast0)))
    ntype = len(type_sites(body_of(ast0)))
    print(f"{name}: {ncomm} commutative, {nhoist} hoist, {ntype} type sites; "
          f"sampling {N} variants, target={len(target)}B", flush=True)

    rng = random.Random(seed)
    seen, specs = set(), []
    # always include the identity variant first
    specs.append((tuple([0]*ncomm), tuple([None]*nhoist), tuple([None]*ntype), None))
    while len(specs) < N:
        cf = tuple(rng.randint(0, 1) for _ in range(ncomm))
        hf = tuple(rng.choice(TYPES) if rng.random() < 0.2 else None for _ in range(nhoist))
        lt = tuple(rng.choice(TYPES) if rng.random() < 0.25 else None for _ in range(ntype))
        od = rng.randint(0, 10**9) if rng.random() < 0.5 else None
        key = (cf, hf, lt, od)
        if key in seen: continue
        seen.add(key); specs.append(key)

    def render(spec):
        a = copy.deepcopy(ast0)
        try:
            apply_mutations(a, spec[0], spec[1], spec[2], spec[3])
            return GEN.visit(a)
        except Exception:
            return "void __broken(void){}"
    sources = [(i, render(s)) for i, s in enumerate(specs)]

    if not os.path.isdir("/tmp/wat"):
        subprocess.run("cp -r /work/toolchain/watcom95 /tmp/wat", shell=True)
    os.environ["WAT_ROOT"] = "/tmp/wat"
    batches = [sources[i:i+B] for i in range(0, len(sources), B)]
    best, done = (-1, 0), 0
    pool = mp.Pool(workers, initializer=init_worker, initargs=(flags, target))
    for results in pool.imap_unordered(try_batch, batches):
        for idx, sc, matched in results:
            done += 1
            if sc > best[0]: best = (sc, idx)
            if matched:
                open(f"src/{name}.c.match", "w").write(sources[idx][1])
                print(f"\n*** MATCH (variant {idx}) -> src/{name}.c.match ***", flush=True)
                pool.terminate(); return
        print(f"  [{done}/{len(specs)}] best={best[0]}/{len(target)} bytes match", flush=True)
    pool.close(); pool.join()
    print(f"\nno exact match. best {best[0]}/{len(target)} bytes. winning C:\n"
          f"{'-'*40}\n{sources[best[1]][1]}\n{'-'*40}", flush=True)

if __name__ == "__main__":
    main()

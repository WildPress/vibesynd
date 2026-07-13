#!/usr/bin/env python3
"""
cpermute.py -- an AST-based C "permuter" (fuzzer) for our Watcom-9.5b matching setup.

=============================================================================
WHAT PROBLEM THIS SOLVES
=============================================================================
We are doing a *matching* decompilation of Syndicate: for each function we hand-write
C that, compiled with the period compiler (Watcom C/C++ 9.5b), produces machine code
*byte-for-byte identical* to the original binary. Behavioural equivalence is not enough
-- the exact bytes must match (modulo relocations, see "MASKING" below).

The hard part is that a single behaviour has many valid C spellings, and Watcom's code
generator makes internal choices -- which register holds a value, the order it schedules
independent operations, whether it folds/hoists a subexpression, how it lays out a switch
-- that we cannot dictate from C directly. Two source spellings that are semantically
identical can compile to different (but equivalent) byte sequences. When our reconstruction
is *logically correct* but diverges from the target on one of these compiler-internal
choices, we have a "near-miss": the right instructions in a slightly different form.

A permuter attacks near-misses mechanically. Instead of us hand-guessing which of the
thousands of equivalent C spellings happens to nudge Watcom into the target's exact
choices, we enumerate semantics-preserving *mutations* of our source, compile hundreds of
them per second, and keep the one whose bytes match. This is the same idea as the
decomp-permuter used by the N64/PS milk-scmatching community, specialised to our backend.

WHAT IT CANNOT DO: if the target's byte sequence is not reachable from ANY C spelling
under our compiler+flags -- a pure register-allocation tie-break, a structural switch
layout, a library object built with different flags -- no permutation will find it. In
that case the tool converges to a best near-miss and stops, which is itself the useful
signal ("this is a genuine wall, stop hand-grinding it"). See AGENTS.md for the catalogue
of walls we've mapped this way (0x33fb8 register tie-break, 0x26e18 register-role, etc.).

=============================================================================
HOW IT WORKS, END TO END
=============================================================================
1. Read the target bytes for the function from the linear image (manifest gives addr+size).
2. Parse our reconstruction src/<name>.c into a pycparser AST (comments stripped first;
   pycparser can't tokenise them). This is the "seed".
3. Enumerate MUTATION SITES in the seed AST -- places a semantics-preserving transform
   could apply (a commutative operator, a while-loop, a pow2 multiply, a local variable...).
4. Sample N random "specs". A spec is a tuple of flags/choices, one slot per site, saying
   which transforms to apply in this variant. Spec 0 is always the identity (no mutation),
   so a seed that already matches is caught immediately.
5. For each spec, deep-copy the seed AST, apply its mutations, and render back to C text.
6. Compile the variants in large batches through the period Watcom compiler and diff each
   resulting object's .text against the target, relocation-aware. Score by matching bytes.
7. First exact (masked) match wins -> written to src/<name>.c.match. Otherwise report the
   best near-miss and its source, so a human can see exactly where/why it diverges.

WHY IT'S FAST (this is what makes brute force viable):
  * The compiler is DOS-hosted WCC386.EXE under DOSBox; a cold start costs ~0.85s. We
    amortise that by compiling a whole BATCH of variants (default 40) in ONE DOSBox
    session -- see tools/wcc95_batch.sh. Net throughput ~200 compiles/sec.
  * All compiler I/O happens on the container's native /tmp, never the /mnt/c drvfs mount
    (drvfs write-visibility lag made batches flaky and slow). Each worker gets its own
    /tmp/dosw_<n> scratch dir.
  * Variants fan out across CPU cores with multiprocessing.

MASKING (relocation-aware compare):
  A call/global reference compiles to an instruction with a 4-byte field the LINKER fills
  in later; the object file records these as "fixups". Our object and the original binary
  will disagree on those bytes (different link addresses) even when the code is identical,
  so we ZERO the fixup ranges in BOTH before comparing. tools/omf.py extracts the .text
  bytes and the fixup list from a Watcom OMF .OBJ; mask() blanks those ranges.

=============================================================================
KEY INSIGHTS BAKED INTO THE TRANSFORMS  (why each family exists)
=============================================================================
Every transform family below corresponds to a compiler-choice we observed drive a real
near-miss. They are the vocabulary of "equivalent C spellings that flip one codegen knob":

  * loop form (while <-> do-while): loop rotation. A `while` whose body always runs once
    can be a `do-while`; the two compile differently and only one matches. (The classic
    trap: writing `while` where the original was `do-while` makes -oneatx rotate the loop
    and falsely look opt-dependent -- see AGENTS.md.)
  * commutative swap (a+b <-> b+a, a==b <-> b==a, ...): which operand Watcom evaluates
    first / which register becomes the destination of the op.
  * codegen-equivalent rewrite: strength reduction (x*2^n <-> x<<n, x/2^n <-> x>>n,
    x%2^n <-> x&(2^n-1)), relational swap (a<b <-> b>a), and range-check folding
    ((a>=lo && a<=hi) <-> (unsigned)(a-lo) <= (hi-lo)). Same result, different instructions.
  * type permutation: signedness/width of temps and scalar locals. Drives movsx vs movzx,
    sar vs shr, `mov al` vs `mov eax`, byte vs word memory access.
  * temp introduction (hoist): pull a subexpression into `T __tN = <expr>;`. Materialises a
    value into its own register/lifetime; can change scheduling and register assignment.
  * variable inlining (the INVERSE of the above): drop `T name = init;` and paste `init`
    at each use. This one is subtle and high-value: a NAMED local lets Watcom keep a
    16-bit value in EAX and zero-extend in place (`and eax,0xffff`); the REPEATED inline
    subexpression makes Watcom CSE it into a callee-saved register (EBX) and zero-extend
    via `xor eax,eax; mov ax,bx`. In effect: inlining a value's uses <-> forcing it into a
    persistent register. This cracked 0x37738 and the previously-parked 0x34118.
  * statement / declaration reorder: shuffle independent statements; changes scheduling.

=============================================================================
USAGE
=============================================================================
  docker run --rm -v "$PWD":/work -w /work synd-decomp \
      python3 tools/cpermute.py <FUNC> "<wcc386 flags>" [--workers 24] [--n 4000] [--seed 0]

  e.g.  python3 tools/cpermute.py FUN_00033fb8 "-4s -oneatx -zp8 -s -zq"

Reads src/<FUNC>.c (the seed), leaves it untouched, and on success writes the winning
variant to src/<FUNC>.c.match. pycparser is auto-installed if missing.
"""
import json, subprocess, sys, re, os, copy, random, difflib
import multiprocessing as mp
try:
    from pycparser import c_parser, c_generator, c_ast
except ImportError:
    # PEP 668 environments refuse a bare pip install; --break-system-packages is required
    # inside the throwaway container.
    subprocess.run("pip install -q --break-system-packages pycparser", shell=True)
    from pycparser import c_parser, c_generator, c_ast
from omf import text_bytes_and_fixups   # .text bytes + relocation fixups from a Watcom OBJ

# Linear (flat, un-relocated) image of the original code segment, and the manifest that
# maps each function name -> {addr, size, status}. These are the ground truth we match to.
SEG, MAN = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "manifest/functions.json"

# Operators whose operands may be swapped without changing the result.
COMMUTATIVE = {"+", "*", "==", "!=", "&", "|", "^", "&&", "||"}
# Arithmetic ops we consider "worth hoisting into a temp" (a subexpression carrying work).
ARITH = {"+", "-", "*", "/", "%", "<<", ">>", "&", "|", "^"}
# Candidate types for introduced temps and for swapping the type of an existing scalar
# local. The signedness/width choice is what drives movsx/movzx, sar/shr, mov al vs eax,
# byte vs word memory operands -- i.e. it moves real bytes in the output.
TYPES = ["int", "unsigned int", "unsigned char", "unsigned short",
         "char", "short", "long", "unsigned"]

PARSER = c_parser.CParser()
GEN = c_generator.CGenerator()   # AST -> C text


# ---------------------------------------------------------------------------
# AST plumbing
# ---------------------------------------------------------------------------
def strip_comments(s):
    """pycparser is a strict C parser with no preprocessor and cannot handle comments,
    so we remove /*...*/ and // before parsing. (Our seed sources carry explanatory
    header comments that must not reach the parser.)"""
    s = re.sub(r"/\*.*?\*/", "", s, flags=re.S)
    return re.sub(r"//[^\n]*", "", s)

def body_of(ast):
    """The Compound (statement list) of the single function definition in the file."""
    fn = next(x for x in ast.ext if isinstance(x, c_ast.FuncDef))
    return fn.body

def parent_map(node, parent=None, pmap=None):
    """id(node) -> parent node, for the whole tree. pycparser nodes don't carry a parent
    pointer, but several transforms need to replace a node inside its parent, so we build
    this map on demand. Rebuilt after structural edits (node identities change)."""
    if pmap is None: pmap = {}
    pmap[id(node)] = parent
    for _, ch in node.children():
        parent_map(ch, node, pmap)
    return pmap

def replace_child(parent, old, new):
    """Swap `old` for `new` wherever it sits under `parent`. pycparser exposes children as
    (name, node) pairs where list elements look like 'args[2]'; handle both the plain-attr
    and the indexed-list cases."""
    for nm, ch in parent.children():
        if ch is old:
            if nm.endswith("]"):
                attr, idx = nm[:-1].split("["); getattr(parent, attr)[int(idx)] = new
            else:
                setattr(parent, nm, new)
            return True
    return False


# ---------------------------------------------------------------------------
# Mutation-site enumeration + application, one family at a time.
# Each `*_sites(...)` returns the list of places the transform could apply, in a stable
# order; the matching per-variant flag tuple says which of them to actually apply.
# ---------------------------------------------------------------------------
def comm_sites(ast):
    """All commutative BinaryOp nodes (candidates for an operand swap)."""
    out = []
    class V(c_ast.NodeVisitor):
        def visit_BinaryOp(self, n):
            if n.op in COMMUTATIVE: out.append(n)
            self.generic_visit(n)
    V().visit(ast)
    return out

def loop_sites(ast):
    """While / DoWhile nodes -- each can be swapped to the other form. Only valid when the
    body is guaranteed to run at least once; if not, the variant simply mismatches / fails
    to compile and is dropped by the search, so we don't need to prove it here."""
    out = []
    class V(c_ast.NodeVisitor):
        def visit_While(self, n): out.append(n); self.generic_visit(n)
        def visit_DoWhile(self, n): out.append(n); self.generic_visit(n)
    V().visit(ast)
    return out

# --- helpers for the codegen-equivalent rewrite family ---
def _pow2(v): return v is not None and v > 0 and (v & (v - 1)) == 0
def _const(node):
    """Integer value of an `int` Constant node, else None."""
    if isinstance(node, c_ast.Constant) and node.type == "int":
        try: return int(node.value, 0)
        except Exception: return None
    return None
REL_SWAP = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}

def parse_expr(text):
    """Parse a C expression string into an AST expression node (via a throwaway wrapper)."""
    d = PARSER.parse(f"void __f(void){{ int __x = ({text}); }}")
    return d.ext[0].body.block_items[0].init

def range_check(n):
    """Recognise `a >= lo && a <= hi` (operands in either order, lo/hi integer constants)
    and return (a_node, lo, hi); else None. This is the pattern foldable to the unsigned
    single-compare trick `(unsigned)(a-lo) <= (hi-lo)`, which Watcom emits for range tests
    and which we therefore sometimes need to write explicitly to match."""
    if n.op != "&&":
        return None
    def part(x):
        if isinstance(x, c_ast.BinaryOp) and x.op in (">=", "<=") and _const(x.right) is not None:
            return (x.op, x.left, _const(x.right))
        return None
    a, b = part(n.left), part(n.right)
    if not a or not b or a[0] == b[0]:
        return None
    ge, le = (a, b) if a[0] == ">=" else (b, a)
    if GEN.visit(ge[1]) != GEN.visit(le[1]):   # both halves must test the same expression
        return None
    return (ge[1], ge[2], le[2])

def rewrite_sites(ast):
    """BinaryOp nodes that have a codegen-equivalent alternate spelling:
       *  x * 2^n            <-> x << n
       *  x / 2^n            <-> x >> n
       *  x % 2^n            <-> x & (2^n - 1)
       *  a <rel> b          <-> b <swapped-rel> a
       *  a>=lo && a<=hi     <-> (unsigned)(a-lo) <= (hi-lo)
    Whether the shift form or the mul/div form matches depends on the compiler; offering
    both lets the search pick."""
    out = []
    class V(c_ast.NodeVisitor):
        def visit_BinaryOp(self, n):
            self.generic_visit(n)
            if n.op == "*" and (_pow2(_const(n.left)) or _pow2(_const(n.right))):
                out.append(n)
            elif n.op in ("/", "%") and _pow2(_const(n.right)):
                out.append(n)
            elif n.op in REL_SWAP:
                out.append(n)
            elif n.op == "&&" and range_check(n):
                out.append(n)
    V().visit(ast)
    return out

def apply_rewrite(n):
    """Rewrite BinaryOp `n` in place to its equivalent form. Re-inspects the operands each
    time (rather than caching from rewrite_sites), so it composes correctly with a
    commutative swap that may have already moved the constant to the other side."""
    if n.op == "*":
        cs = "right" if _pow2(_const(n.right)) else "left"          # side holding the pow2
        k = _const(getattr(n, cs)).bit_length() - 1                 # 2^k -> shift by k
        n.op = "<<"; setattr(n, cs, c_ast.Constant("int", str(k)))
    elif n.op == "/":
        k = _const(n.right).bit_length() - 1
        n.op = ">>"; n.right = c_ast.Constant("int", str(k))
    elif n.op == "%":
        m = _const(n.right) - 1                                     # x % 2^n == x & (2^n-1)
        n.op = "&"; n.right = c_ast.Constant("int", hex(m))
    elif n.op in REL_SWAP:
        n.op = REL_SWAP[n.op]; n.left, n.right = n.right, n.left
    elif n.op == "&&":
        rc = range_check(n)
        if rc:
            a, lo, hi = rc
            n.op = "<="
            n.left = parse_expr(f"(unsigned)({GEN.visit(a)} - {lo})")
            n.right = parse_expr(str(hi - lo))

def hoist_sites(body):
    """(stmt_index, node) for each subexpression worth hoisting into a temp: array
    references and arithmetic ops, anywhere in each top-level statement's subtree. The
    stmt_index records which statement the temp declaration must be spliced in front of."""
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
    """Build a `typ name = <expr>;` declaration node by parsing it (simpler and more robust
    than constructing the Decl/TypeDecl/IdentifierType nodes by hand)."""
    d = PARSER.parse(f"void __f(void){{ {typ} {name} = {GEN.visit(expr)}; }}")
    return d.ext[0].body.block_items[0]

def type_sites(body):
    """IdentifierType nodes of scalar (non-pointer) top-level local declarations. Mutating
    `.names` on one of these changes the declared type of that local (see TYPES)."""
    out = []
    for stmt in (body.block_items or []):
        if isinstance(stmt, c_ast.Decl) and isinstance(stmt.type, c_ast.TypeDecl) \
           and isinstance(stmt.type.type, c_ast.IdentifierType):
            out.append(stmt.type.type)
    return out

def _count_id(root, name):
    """Number of ID references to `name` anywhere under `root`."""
    c = [0]
    class V(c_ast.NodeVisitor):
        def visit_ID(self, n):
            if n.name == name: c[0] += 1
    V().visit(root)
    return c[0]

def inline_sites(body):
    """Top-level local declarations `T name = init;` whose name is used in a LATER
    statement -- candidates to INLINE (drop the decl, paste `init` at every use).

    This is the inverse of temp-introduction and the highest-value transform we have for
    register-role near-misses. Rationale: a named local lets Watcom keep e.g. a 16-bit
    field in EAX and zero-extend it in place (`and eax,0xffff`), whereas the repeated
    inline subexpression forces a CSE into a callee-saved register (EBX/ESI) with a
    `xor eax,eax; mov ax,bx` zero-extend -- exactly what targets do when they keep a value
    live across a test. Cracked 0x37738 and the parked register-wall 0x34118.

    LIMITATION: only enumerates declarations directly in the function's top-level block,
    not ones nested inside an if/loop compound. Extend to recurse if a nested-decl case
    needs it."""
    items = body.block_items or []
    out = []
    for si, stmt in enumerate(items):
        if isinstance(stmt, c_ast.Decl) and stmt.init is not None and stmt.name:
            if sum(_count_id(s, stmt.name) for s in items[si + 1:]) >= 1:
                out.append(stmt)
    return out


def apply_mutations(ast, loop_flags, comm_flags, rewrite_flags, hoist_types,
                    local_types, order_seed, inline_flags=()):
    """Apply one variant's chosen mutations to `ast` in place. Each *_flags tuple is
    parallel to the corresponding *_sites list. Order of application matters and is chosen
    so earlier structural edits don't invalidate later site lookups: loop-form swaps and
    inlining (which change the statement list) happen before we enumerate type/hoist sites.
    Any flag index beyond its (possibly now-shorter) site list is a harmless no-op, which
    is why the stochastic search tolerates the seed/variant site-count drift."""
    # (1) loop-form swaps first -- they replace whole While/DoWhile nodes.
    ls = loop_sites(ast)
    for i, on in enumerate(loop_flags):
        if on and i < len(ls):
            n = ls[i]
            parent = parent_map(ast).get(id(n))
            if parent is None:
                continue
            new = c_ast.DoWhile(cond=n.cond, stmt=n.stmt) if isinstance(n, c_ast.While) \
                  else c_ast.While(cond=n.cond, stmt=n.stmt)
            replace_child(parent, n, new)

    body = body_of(ast)

    # (2) variable inlining (inverse of temp-intro): for each selected `T name=init;`,
    # replace every ID use of `name` with a fresh copy of `init`, then delete the decl.
    # Done before type/hoist enumeration so those see the reduced body.
    isites = inline_sites(body)
    for i, on in enumerate(inline_flags):
        if not on or i >= len(isites):
            continue
        d = isites[i]
        if d not in (body.block_items or []):     # already removed by an earlier index
            continue
        name, init = d.name, d.init
        uses = []
        class _U(c_ast.NodeVisitor):
            def visit_ID(self, n):
                if n.name == name: uses.append(n)
        _U().visit(body)
        pm = parent_map(ast)
        for u in uses:
            par = pm.get(id(u))
            if par is not None:
                replace_child(par, u, copy.deepcopy(init))   # copy: each use gets its own tree
        body.block_items = [s for s in (body.block_items or []) if s is not d]

    # (3) commutative operand swaps.
    cs = comm_sites(ast)
    for i, on in enumerate(comm_flags):
        if on and i < len(cs):
            cs[i].left, cs[i].right = cs[i].right, cs[i].left

    # (4) codegen-equivalent rewrites (strength reduction / relational swap / range fold).
    rs = rewrite_sites(ast)
    for i, on in enumerate(rewrite_flags):
        if on and i < len(rs):
            apply_rewrite(rs[i])

    # (5) retype existing scalar locals (signedness/width).
    ts = type_sites(body)
    for i, t in enumerate(local_types):
        if t and i < len(ts):
            ts[i].names = t.split()

    # (6) temp hoists: replace each selected subexpression with a fresh `__tN` ID and remember
    # the `T __tN = <expr>;` decl to splice in just before that subexpression's statement.
    hs = hoist_sites(body)
    pm = parent_map(ast)
    pre = {}          # stmt_index -> [decls to insert before it]
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

    # (7) statement/declaration reorder: deterministic shuffle keyed by order_seed. A
    # broken ordering (use-before-def) just fails to compile and is dropped.
    if order_seed is not None:
        idx = list(range(len(spliced)))
        random.Random(order_seed).shuffle(idx)
        spliced = [spliced[j] for j in idx]
    body.block_items = spliced


# ---------------------------------------------------------------------------
# Scoring: relocation-aware byte comparison against the target.
# ---------------------------------------------------------------------------
def mask(b, fx):
    """Zero out the fixup (relocation) ranges so link-address differences don't count as
    mismatches. `fx` is [(offset, size), ...] from the object's relocation records."""
    b = bytearray(b)
    for off, size in fx:
        for j in range(off, min(off + size, len(b))): b[j] = 0
    return bytes(b)

def score(tb, ob, fx):
    """Total ALIGNED matching bytes (difflib), NOT just the leading prefix.

    A leading-byte score is capped by the FIRST diverging byte -- and that first divergence
    is often just a jump displacement that differs only because some later block is a couple
    bytes longer. That hides the fact that the body is otherwise ~95% correct (it reported
    10/130 for 0x26e18 when the true masked match was ~110/130). Aligning the two masked
    byte strings with difflib and summing the matching runs credits the matching tail too,
    so the search can climb through length-shifted near-misses instead of plateauing."""
    tm, om = mask(tb, fx), mask(ob, fx)
    sm = difflib.SequenceMatcher(None, tm, om, autojunk=False)
    return sum(b.size for b in sm.get_matching_blocks())


# ---------------------------------------------------------------------------
# Parallel batched compile + diff.
# Each worker owns a private /tmp scratch dir and shares the read-only target bytes.
# ---------------------------------------------------------------------------
G = {}
def init_worker(flags, target):
    """Per-worker setup: a unique native-/tmp scratch dir (drvfs is too slow/laggy) plus
    the compile flags and target bytes stashed in a module global for try_batch()."""
    wid = (mp.current_process()._identity or [0])[0]
    work = f"/tmp/dosw_{wid}"; os.makedirs(work, exist_ok=True)
    G.update(flags=flags, target=target, work=work)

def table_ok(codestart, table_len):
    """Plausible [table][<=32B pad][code] split (see match_reloc.py)."""
    return codestart >= table_len and codestart <= table_len + 32

def try_batch(batch):
    """Compile a batch of variants in ONE DOSBox session and diff each against the target.
    `batch` is [(variant_index, c_source), ...]. Returns [(variant_index, score, matched)].
    Writing all sources first and invoking the compiler once amortises DOSBox startup."""
    W = G["work"]
    subprocess.run(f"rm -f {W}/SRC*.C {W}/O*.OBJ", shell=True)
    for j, (idx, src) in enumerate(batch):
        with open(f"{W}/SRC{j:02d}.C", "w") as f:
            f.write(src); f.flush(); os.fsync(f.fileno())    # ensure DOS side sees the file
    # wcc95_batch.sh compiles every SRC*.C in W to O*.OBJ in a single Watcom/DOSBox run.
    subprocess.run(["bash", "tools/wcc95_batch.sh", W, G["flags"]], capture_output=True)
    tb, out = G["target"], []
    for j, (idx, src) in enumerate(batch):
        p = f"{W}/O{j:02d}.OBJ"
        if not os.path.exists(p):                        # variant failed to compile
            out.append((idx, -1, False)); continue
        try: ob, fx = text_bytes_and_fixups(p)
        except Exception: out.append((idx, -1, False)); continue
        # Jump-table-aware split (mirrors match_reloc.jumptable_aware_match): a switch
        # makes Watcom co-locate its table at the START of the obj .text as a run of
        # >=4 consecutive 4-byte fixups at offsets 0,4,8,... The on-disk target is
        # clean code, so score/compare only the code TAIL with re-based fixups.
        offs = {o: sz for o, sz in fx}
        k = 0
        while offs.get(4 * k) == 4:
            k += 1
        if k >= 4:
            codestart = len(ob) - len(tb)
            if table_ok(codestart, 4 * k):
                ob = ob[codestart:]
                fx = [(o - codestart, sz) for o, sz in fx if o >= codestart]
        matched = len(tb) == len(ob) and mask(tb, fx) == mask(ob, fx)   # exact (masked) hit
        out.append((idx, score(tb, ob, fx), matched))
    return out


def main():
    # ---- args ----
    name, flags = sys.argv[1], sys.argv[2]
    workers = int(sys.argv[sys.argv.index("--workers")+1]) if "--workers" in sys.argv else 24
    N = int(sys.argv[sys.argv.index("--n")+1]) if "--n" in sys.argv else 4000   # variants to try
    seed = int(sys.argv[sys.argv.index("--seed")+1]) if "--seed" in sys.argv else 0
    B = 40                                                                       # batch size

    # ---- target bytes ----
    man = json.load(open(MAN)); base = int(man["image_base"], 16)
    f = next(x for x in man["functions"] if x["name"] == name)
    off = int(f["addr"], 16) - base
    target = open(SEG, "rb").read()[off:off + f["size"]]

    # ---- seed AST + site counts ----
    src0 = strip_comments(open(f"src/{name}.c").read())
    ast0 = PARSER.parse(src0)
    nloop = len(loop_sites(ast0)); ncomm = len(comm_sites(ast0)); nrw = len(rewrite_sites(ast0))
    nhoist = len(hoist_sites(body_of(ast0))); ntype = len(type_sites(body_of(ast0)))
    ninline = len(inline_sites(body_of(ast0)))
    print(f"{name}: {nloop} loop, {ncomm} commutative, {nrw} rewrite, {nhoist} hoist, "
          f"{ntype} type, {ninline} inline sites; sampling {N} variants, "
          f"target={len(target)}B", flush=True)

    # ---- sample N distinct specs (spec = per-family choice tuples) ----
    # Probabilities keep most sites OFF in any given variant, so we explore combinations of
    # a FEW simultaneous mutations rather than always-everything (which rarely matches).
    rng = random.Random(seed)
    seen, specs = set(), []
    specs.append((tuple([0]*nloop), tuple([0]*ncomm), tuple([0]*nrw),           # identity first:
                  tuple([None]*nhoist), tuple([None]*ntype), None, tuple([0]*ninline)))  # catches an
                                                                                         # already-matching seed
    while len(specs) < N:
        lf = tuple(rng.randint(0, 1) for _ in range(nloop))                            # loop swaps
        cf = tuple(rng.randint(0, 1) for _ in range(ncomm))                            # comm swaps
        rw = tuple(rng.randint(0, 1) for _ in range(nrw))                              # rewrites
        hf = tuple(rng.choice(TYPES) if rng.random() < 0.2 else None for _ in range(nhoist))  # hoists
        lt = tuple(rng.choice(TYPES) if rng.random() < 0.25 else None for _ in range(ntype))  # retypes
        od = rng.randint(0, 10**9) if rng.random() < 0.5 else None                     # reorder seed
        inf = tuple(rng.randint(0, 1) for _ in range(ninline))                         # inlines
        key = (lf, cf, rw, hf, lt, od, inf)
        if key in seen: continue
        seen.add(key); specs.append(key)

    # ---- render every spec to C text (done single-threaded; cheap vs compiling) ----
    def render(spec):
        a = copy.deepcopy(ast0)                       # never mutate the shared seed
        try:
            apply_mutations(a, spec[0], spec[1], spec[2], spec[3], spec[4], spec[5], spec[6])
            return GEN.visit(a)
        except Exception:
            return "void __broken(void){}"            # unrenderable -> will fail to match
    sources = [(i, render(s)) for i, s in enumerate(specs)]

    # ---- stage the compiler on native /tmp once, then fan batches across workers ----
    if not os.path.isdir("/tmp/wat"):
        subprocess.run("cp -r /work/toolchain/watcom95 /tmp/wat", shell=True)
    os.environ["WAT_ROOT"] = "/tmp/wat"
    batches = [sources[i:i+B] for i in range(0, len(sources), B)]
    best, done = (-1, 0), 0                            # (score, variant_index)
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
    # No exact match: print the best near-miss and its source so a human can read off the
    # remaining divergence (usually a register/scheduling choice worth recording as a wall).
    print(f"\nno exact match. best {best[0]}/{len(target)} bytes. winning C:\n"
          f"{'-'*40}\n{sources[best[1]][1]}\n{'-'*40}", flush=True)

if __name__ == "__main__":
    main()

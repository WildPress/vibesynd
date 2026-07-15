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
import json, subprocess, sys, re, os, copy, random, difflib, itertools, time
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


# Watcom keyword <-> stock-C-qualifier round-trip. pycparser is strict C99 and chokes on Watcom
# extensions (__far/__near/__huge), so functions using them were never permuted at all (they crashed).
# These are POINTER QUALIFIERS, semantically leaves on the type; we map each to a standard qualifier
# pycparser understands FOR PARSING/MUTATION, then map it back FOR COMPILATION so the emitted bytes still
# carry the real extension. Safe because our sources never use volatile/restrict/const in these slots.
WATCOM_QUALS = [("__far", "volatile"), ("__near", "restrict")]

def watcom_in(s):
    for kw, ph in WATCOM_QUALS:
        s = re.sub(r"\b%s\b" % re.escape(kw), ph, s)
    return s

def watcom_out(s):
    for kw, ph in WATCOM_QUALS:
        s = re.sub(r"\b%s\b" % re.escape(ph), kw, s)
    # CGenerator emits the qualifier BEFORE the type ("__far void *"); Watcom's grammar wants it
    # BETWEEN the type and the star ("void __far *"). Move it (type may be multi-word, e.g. "unsigned char").
    s = re.sub(r"\b(__far|__near)\s+([A-Za-z_][\w ]*?)\s*\*", r"\2 \1 *", s)
    return s

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


# Types for stack-padding pads. Scalars shift a slot by their width; arrays force a stack home (harder
# for Watcom to elide) and shift by their whole size, so the search can hit several frame deltas.
PAD_TYPES = ["char", "short", "int", "long", "char[2]", "char[4]", "char[8]", "char[16]"]

def make_pad(name, typ):
    """Build an UNUSED local declaration `typ name;` (scalar) or `base name[N];` (array). Inserting one
    into the decl block is decomp-permuter's perm_pad_var_decl: a dead local that shifts Watcom's
    esp-relative spill-slot / register assignment for the REAL locals -- the one lever that can move a
    slot-TRANSPOSE near-miss (0x2e5f8 y<->i, 0x338d8 counters) that decl REORDER leaves byte-inert.
    Watcom may elide a truly-dead local; array/larger pads resist elision and shift by their full size.
    The search discovers empirically which pad (if any) lands the target layout -- an elided pad just
    scores as identity and costs nothing."""
    if "[" in typ:
        base, rest = typ.split("[", 1)                 # "char[4]" -> base="char", rest="4]"
        decl = f"{base.strip()} {name}[{rest};"        # -> "char __padN[4];"
    else:
        decl = f"{typ} {name};"
    d = PARSER.parse(f"void __f(void){{ {decl} }}")
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


def declblock_len(body):
    """Length of the leading CONTIGUOUS run of local declarations at the top of the function
    body. These are the `T name;` / `T name = init;` statements Watcom lays out into esp-relative
    spill slots / assigns registers to; PERMUTING them is the one lever that moves slot and
    register assignment (decl-order is otherwise byte-inert only because the whole-body shuffle
    breaks compilation before it can help). This is the surgical version of order_seed: reorder
    just the declaration block, leave every executable statement in place. Cracks the length-exact
    spill-slot / register-role parks (0x2e5f8 y<->i transpose, 0x338d8 counter slots, etc.)."""
    n = 0
    for stmt in (body.block_items or []):
        if isinstance(stmt, c_ast.Decl) and not isinstance(stmt.type, c_ast.FuncDecl):
            n += 1
        else:
            break
    return n


def apply_mutations(ast, loop_flags, comm_flags, rewrite_flags, hoist_types,
                    local_types, order_seed, inline_flags=(), decl_perm=None, pads=()):
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

    # (8) SURGICAL declaration-block permutation: reorder ONLY the leading contiguous run of
    # Decl statements by decl_perm (a permutation of range(k)), leaving executable statements
    # in place. This is what moves Watcom's spill-slot / register assignment; an invalid perm
    # (e.g. an initializer that now precedes its dependency) just fails to compile and is dropped.
    if decl_perm is not None:
        items = body.block_items or []
        k = declblock_len(body)
        if len(decl_perm) == k and sorted(decl_perm) == list(range(k)):
            head = [items[j] for j in decl_perm]
            body.block_items = head + items[k:]

    # (9) STACK PADDING (decomp-permuter perm_pad_var_decl): insert dead local decls to shift Watcom's
    # spill-slot / register assignment for the real locals. Each pad is (pos, typ); inserted in order at
    # its index within the (post-perm) leading decl block, so several pads keep a stable relative order.
    # This is the lever that reaches slot-TRANSPOSE walls that decl-reorder (step 8) leaves byte-inert.
    if pads:
        items = body.block_items or []
        k = declblock_len(body)
        for j, (pos, typ) in enumerate(pads):
            try:
                d = make_pad(f"__pad{j}", typ)
            except Exception:
                continue
            items.insert(max(0, min(pos, k)), d)
            k += 1
        body.block_items = items


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
    # include the PID so CONCURRENT cpermute processes (parallel functions) get distinct scratch dirs --
    # otherwise both pools use /tmp/dosw_1.. and clobber each other's SRC*.C/O*.OBJ mid-compile.
    work = f"/tmp/dosw_{os.getpid()}_{wid}"; os.makedirs(work, exist_ok=True)
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


def rand_pads(rng, ndecl):
    """0-2 stack pads at random decl-block positions with random widths. Kept sparse (most variants get
    none) so padding composes with the other levers rather than dominating the sample."""
    if rng.random() >= 0.35:
        return ()
    return tuple((rng.randint(0, ndecl), rng.choice(PAD_TYPES)) for _ in range(rng.randint(1, 2)))


def rand_spec(rng, nloop, ncomm, nrw, nhoist, ntype, ninline, ndecl, ID):
    """One random variant spec: a FEW simultaneous mutations (most sites off)."""
    return (tuple(rng.randint(0, 1) for _ in range(nloop)),
            tuple(rng.randint(0, 1) for _ in range(ncomm)),
            tuple(rng.randint(0, 1) for _ in range(nrw)),
            tuple(rng.choice(TYPES) if rng.random() < 0.2 else None for _ in range(nhoist)),
            tuple(rng.choice(TYPES) if rng.random() < 0.25 else None for _ in range(ntype)),
            rng.randint(0, 10**9) if rng.random() < 0.5 else None,
            tuple(rng.randint(0, 1) for _ in range(ninline)),
            tuple(rng.sample(range(ndecl), ndecl)) if (ndecl >= 2 and rng.random() < 0.5) else ID,
            rand_pads(rng, ndecl))


def perturb(spec, rng):
    """A NEIGHBOUR of `spec`: tweak exactly ONE mutation dimension. This is the move operator for the
    simulated-annealing hill-climb -- small steps from the best-known variant converge on near-misses
    that blind random sampling stalls on (register/scheduling ties an inch away from matching)."""
    lf, cf, rw, hf, lt, od, inf = (list(spec[0]), list(spec[1]), list(spec[2]), list(spec[3]),
                                   list(spec[4]), spec[5], list(spec[6]))
    dp = list(spec[7]) if spec[7] is not None else None
    pads = list(spec[8]) if len(spec) > 8 else []
    ndecl = len(dp) if dp is not None else 0
    dims = ["od", "pad"]                                # 'pad' always available (can add the first pad)
    for nm, seq in (("lf", lf), ("cf", cf), ("rw", rw), ("hf", hf), ("lt", lt), ("inf", inf)):
        if seq: dims.append(nm)
    if dp and len(dp) >= 2: dims.append("dp")
    d = rng.choice(dims)
    if d == "lf": i = rng.randrange(len(lf)); lf[i] ^= 1
    elif d == "cf": i = rng.randrange(len(cf)); cf[i] ^= 1
    elif d == "rw": i = rng.randrange(len(rw)); rw[i] ^= 1
    elif d == "inf": i = rng.randrange(len(inf)); inf[i] ^= 1
    elif d == "hf": i = rng.randrange(len(hf)); hf[i] = None if rng.random() < 0.4 else rng.choice(TYPES)
    elif d == "lt": i = rng.randrange(len(lt)); lt[i] = None if rng.random() < 0.4 else rng.choice(TYPES)
    elif d == "od": od = None if rng.random() < 0.3 else rng.randint(0, 10**9)
    elif d == "dp": i, j = rng.sample(range(len(dp)), 2); dp[i], dp[j] = dp[j], dp[i]
    elif d == "pad":
        # add / remove / retype / move a stack pad -- the move that walks through frame layouts
        if pads and rng.random() < 0.4:
            i = rng.randrange(len(pads))
            if rng.random() < 0.5:                      # retype or move this pad
                pos, typ = pads[i]
                pads[i] = ((rng.randint(0, max(ndecl, 0)), typ) if rng.random() < 0.5
                           else (pos, rng.choice(PAD_TYPES)))
            else:
                pads.pop(i)                             # remove
        else:
            pads.append((rng.randint(0, max(ndecl, 0)), rng.choice(PAD_TYPES)))   # add
    return (tuple(lf), tuple(cf), tuple(rw), tuple(hf), tuple(lt), od, tuple(inf),
            tuple(dp) if dp is not None else None, tuple(pads))


def main():
    # ---- args ----
    name, flags = sys.argv[1], sys.argv[2]
    workers = int(sys.argv[sys.argv.index("--workers")+1]) if "--workers" in sys.argv else 24
    N = int(sys.argv[sys.argv.index("--n")+1]) if "--n" in sys.argv else 4000   # variants to try
    seed = int(sys.argv[sys.argv.index("--seed")+1]) if "--seed" in sys.argv else 0
    # --status DIR: emit a per-function progress JSON (DIR/<name>.json) at each anneal step, for the
    # live monitor (tools/permwatch.py) to read. Written atomically; cadence == the print cadence, so
    # it adds no measurable cost to the search.
    status_dir = sys.argv[sys.argv.index("--status")+1] if "--status" in sys.argv else None
    B = 40                                                                       # batch size

    # ---- target bytes ----
    man = json.load(open(MAN)); base = int(man["image_base"], 16)
    f = next(x for x in man["functions"] if x["name"] == name)
    off = int(f["addr"], 16) - base
    target = open(SEG, "rb").read()[off:off + f["size"]]

    # ---- seed AST + site counts ----  (src files live in subsystem subdirs)
    import glob as _glob
    _sp = _glob.glob(f"src/**/{name}.c", recursive=True)
    srcfile = _sp[0] if _sp else f"src/{name}.c"
    src0 = watcom_in(strip_comments(open(srcfile).read()))   # __far/__near -> stock quals for parsing
    try:
        ast0 = PARSER.parse(src0)
    except Exception as e:
        # Un-parseable even after the Watcom-qualifier swap (unusual construct). Don't crash the whole
        # run: report and exit cleanly so the harness records it as an un-permutable wall.
        print(f"{name}: UNPARSEABLE ({type(e).__name__}: {str(e)[:80]}) -- skipping (no variants)", flush=True)
        return
    nloop = len(loop_sites(ast0)); ncomm = len(comm_sites(ast0)); nrw = len(rewrite_sites(ast0))
    nhoist = len(hoist_sites(body_of(ast0))); ntype = len(type_sites(body_of(ast0)))
    ninline = len(inline_sites(body_of(ast0)))
    ndecl = declblock_len(body_of(ast0))
    print(f"{name}: {nloop} loop, {ncomm} commutative, {nrw} rewrite, {nhoist} hoist, "
          f"{ntype} type, {ninline} inline, {ndecl} decl sites, +stack-pad; sampling {N} variants, "
          f"target={len(target)}B", flush=True)

    # ---- PHASE 1: broad seed sample (identity + decl-block perms + random combos) ----
    # Reserve most of the budget for annealing; the seed just needs to find a good starting point.
    import math
    ID = tuple(range(ndecl))
    rng = random.Random(seed)
    SEED = min(N, max(400, N // 3))
    seen, specs = set(), []
    ident = (tuple([0]*nloop), tuple([0]*ncomm), tuple([0]*nrw),
             tuple([None]*nhoist), tuple([None]*ntype), None, tuple([0]*ninline), ID, ())
    specs.append(ident); seen.add(ident)
    base = ident[:7]                                   # decl-block perms with everything else identity
    if 2 <= ndecl <= 7:
        for perm in itertools.permutations(range(ndecl)):
            if len(specs) >= SEED: break
            key = base + (perm,)
            if key not in seen: seen.add(key); specs.append(key)
    elif ndecl >= 8:
        cap = min(SEED, 8000); tries = 0
        while len(specs) < cap and tries < cap * 20:
            tries += 1
            key = base + (tuple(rng.sample(range(ndecl), ndecl)),)
            if key not in seen: seen.add(key); specs.append(key)
    while len(specs) < SEED:
        key = rand_spec(rng, nloop, ncomm, nrw, nhoist, ntype, ninline, ndecl, ID)
        if key not in seen: seen.add(key); specs.append(key)

    def render(spec):
        a = copy.deepcopy(ast0)                        # never mutate the shared seed
        try:
            apply_mutations(a, spec[0], spec[1], spec[2], spec[3], spec[4], spec[5], spec[6],
                            spec[7] if len(spec) > 7 else None,
                            spec[8] if len(spec) > 8 else ())
            return watcom_out(GEN.visit(a))            # restore __far/__near for compilation
        except Exception:
            return "void __broken(void){}"             # unrenderable -> will fail to match

    if not os.path.isdir("/tmp/wat"):
        subprocess.run("cp -r /work/toolchain/watcom95 /tmp/wat", shell=True)
    os.environ["WAT_ROOT"] = "/tmp/wat"
    pool = mp.Pool(workers, initializer=init_worker, initargs=(flags, target))

    reg = {}                                           # variant idx -> (spec, source)
    ctr = [0]
    best = [(-1, None, None)]                          # (score, spec, source)

    t_start = time.time()
    def emit(phase, cur=None, T=None, matched=False, done=False):
        """Write DIR/<name>.json (atomically) so tools/permwatch.py can render this run live.
        No-op unless --status DIR was given. Called on the print cadence, so it's free."""
        if not status_dir:
            return
        try:
            os.makedirs(status_dir, exist_ok=True)
            el = max(time.time() - t_start, 1e-6)
            rec = {"name": name, "target": len(target), "best": best[0][0], "cur": cur,
                   "iters": ctr[0], "total": N, "phase": phase, "T": T,
                   "rate": ctr[0] / el, "matched": matched, "done": done, "ts": time.time()}
            p = os.path.join(status_dir, name + ".json")
            with open(p + ".tmp", "w") as fh:
                fh.write(json.dumps(rec))
            os.replace(p + ".tmp", p)
        except Exception:
            pass
    emit("start")

    class _Hit(Exception):
        pass

    def run(spec_list):
        """Render + compile a list of specs; update the global best; write .match & raise on an exact hit.
        Returns [(idx, score), ...] so the annealer can drive its acceptance decisions."""
        items = []
        for sp in spec_list:
            i = ctr[0]; ctr[0] += 1
            src = render(sp); reg[i] = (sp, src); items.append((i, src))
        out = []
        batches = [items[k:k+B] for k in range(0, len(items), B)]
        for results in pool.imap_unordered(try_batch, batches):
            for idx, sc, matched in results:
                if matched:
                    open(f"{srcfile}.match", "w").write(reg[idx][1])
                    print(f"\n*** MATCH (variant {idx}) -> {srcfile}.match ***", flush=True)
                    pool.terminate(); raise _Hit()
                if sc > best[0][0]:
                    best[0] = (sc, reg[idx][0], reg[idx][1])
                out.append((idx, sc))
        return out

    try:
        # PHASE 1 -- seed
        run(specs)
        print(f"  [seed {ctr[0]}] best={best[0][0]}/{len(target)}", flush=True)
        emit("seed", cur=best[0][0])
        # PHASE 2 -- simulated-annealing hill-climb from the best seed. Small single-dimension moves
        # from the current point converge on register/scheduling near-misses that random sampling can't
        # reach; occasional downhill moves (Boltzmann-accepted) escape local optima.
        cur_spec, cur_s = best[0][1], best[0][0]
        T = max(2.0, len(target) * 0.06)
        budget = N - ctr[0]
        while budget > 0 and cur_spec is not None:
            m = min(B * 4, budget); budget -= m
            neigh = [perturb(cur_spec, rng) for _ in range(m)]
            for idx, sc in run(neigh):
                if sc >= cur_s or (T > 0 and rng.random() < math.exp((sc - cur_s) / T)):
                    cur_spec, cur_s = reg[idx][0], sc   # accept the move (uphill, or downhill by temp)
            T *= 0.95
            print(f"  [anneal {ctr[0]}] best={best[0][0]}/{len(target)} cur={cur_s} T={T:.1f}", flush=True)
            emit("anneal", cur=cur_s, T=T)
    except _Hit:
        emit("done", matched=True, done=True)
        return
    emit("done", cur=best[0][0], done=True)
    pool.close(); pool.join()
    # No exact match: print the best near-miss + source so a human can read off the remaining divergence.
    print(f"\nno exact match. best {best[0][0]}/{len(target)} bytes. winning C:\n"
          f"{'-'*40}\n{best[0][2]}\n{'-'*40}", flush=True)

if __name__ == "__main__":
    main()

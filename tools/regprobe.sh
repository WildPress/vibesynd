#!/usr/bin/env bash
# regprobe.sh -- show what C source can and cannot steer in Watcom 9.5b codegen.
# Runs inside the pipeline container:  ./run.sh bash tools/regprobe.sh
#
# Companion to docs/register-allocation.md. Compiles four tiny functions and
# disassembles them so you can watch the bytes shift:
#   A vs B  -- register ALLOCATION. Same code, two statements reordered. Which
#              register backs each store flips with the source order. The lever is real.
#   C vs D  -- a memory compare written a==b vs b==a. Source order picks which operand
#              loads first, so the bytes differ too. The wall only appears when both
#              operands are already pinned in registers (see the collision-query byte).
set -u
pip install --quiet --break-system-packages capstone 2>/dev/null
W=/tmp/regprobe; rm -rf "$W"; mkdir -p "$W"

cat > "$W/SRC00.C" <<'EOF'
extern int g(int);
extern int gx, gy;
void p(int a, int b){ int x, y; x = g(a); y = g(b); gx = x; gy = y; }
EOF
cat > "$W/SRC01.C" <<'EOF'
extern int g(int);
extern int gx, gy;
void p(int a, int b){ int x, y; y = g(b); x = g(a); gx = x; gy = y; }
EOF
cat > "$W/SRC02.C" <<'EOF'
extern int gr;
void c(int a, int b){ gr = (a == b); }
EOF
cat > "$W/SRC03.C" <<'EOF'
extern int gr;
void c(int a, int b){ gr = (b == a); }
EOF

bash tools/wcc95_batch.sh "$W" "-4s -oneatx -zp8 -s -zq" >/dev/null 2>&1

python3 - "$W" <<'PY'
import sys, os
sys.path.insert(0, "tools")
from omf import text_bytes_and_fixups
import capstone
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
labels = {0: "A: x=g(a) first, y=g(b) second",
          1: "B: y=g(b) first, x=g(a) second  (statements reordered)",
          2: "C: gr = (a == b)",
          3: "D: gr = (b == a)   (operands swapped)"}
W = sys.argv[1]
for i in range(4):
    p = "%s/O%02d.OBJ" % (W, i)
    print("=" * 62); print(labels[i])
    if not os.path.exists(p):
        print("  (no OBJ -- compile failed)"); continue
    ob, _ = text_bytes_and_fixups(p)
    for ins in md.disasm(bytes(ob), 0):
        print("  %-6s %-28s ; %s" % (ins.mnemonic, ins.op_str, ins.bytes.hex()))
PY

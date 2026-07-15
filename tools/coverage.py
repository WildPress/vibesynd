#!/usr/bin/env python3
"""coverage.py -- DYNAMIC coverage of the decompilation. Reads an execution trace (a stream of
game-relative instruction offsets logged by the patched DOSBox, build/tr_orig.bin), maps each executed
offset to the manifest function that contains it (manifest_addr = offset + 0xd748), and classifies every
EXECUTED function by how well we have it reconstructed:

  matched-C   : status==matched  -> byte-exact C exists; this IS the running code
  unmatched-C : status==unmatched but a source file exists -> decoded, not yet byte-exact
  no-C        : no source at all (the prefix / main loop / undecoded) -> only original bytes

This turns "how much of the game have we decompiled" from a static file count into "how much of the code
that actually runs on this path is provably ours", and prints the prioritized decode backlog: the
executed functions we still have no C for, ranked by how many instructions they execute.

  python3 tools/coverage.py [build/tr_orig.bin]
"""
import json, os, sys, struct, bisect, glob
from collections import Counter

TRACE = sys.argv[1] if len(sys.argv) > 1 else "build/tr_orig.bin"
BASE = 0xd748

man = json.load(open("manifest/functions.json"))["functions"]
# source presence by BASENAME (== manifest name), like buildgame.py/apply_names.py -- the manifest's
# per-fn "src" field is unreliable (goes stale), so never trust it for classification.
SRC = {os.path.basename(p)[:-2] for p in glob.glob("src/**/*.c", recursive=True)}
funcs = sorted(((int(f["addr"], 16), int(f["addr"], 16) + f["size"], f) for f in man), key=lambda t: t[0])
starts = [t[0] for t in funcs]


def find(addr):
    i = bisect.bisect_right(starts, addr) - 1
    if i >= 0 and funcs[i][0] <= addr < funcs[i][1]:
        return funcs[i][2]
    return None


def klass(f):
    if f.get("status") == "matched":
        return "matched-C"
    if f["name"] in SRC or ("FUN_%s" % f["addr"]) in SRC:
        return "unmatched-C"
    return "no-C"


# stream the trace, counting instruction executions per function
data = open(TRACE, "rb").read()
n = len(data) // 4
offs = struct.unpack("<%dI" % n, data[:n * 4])
hits = Counter()       # fn addr -> instruction executions
outside = 0
for o in offs:
    f = find(o + BASE)
    if f is None:
        outside += 1
    else:
        hits[f["addr"]] += 1

by_addr = {int(f["addr"], 16): f for f in man}
executed = [by_addr[int(a, 16)] for a in hits]  # addr keys are hex strings? no -> fix below
# hits keyed by f["addr"] (hex string). rebuild:
by_hex = {f["addr"]: f for f in man}
executed = [by_hex[a] for a in hits]

kcount = Counter(klass(f) for f in executed)
kbytes = Counter()
kinstr = Counter()
for f in executed:
    kbytes[klass(f)] += f["size"]
    kinstr[klass(f)] += hits[f["addr"]]

tot_fn = len(executed)
tot_by = sum(f["size"] for f in executed)
tot_in = sum(hits.values())
print("trace: %s  (%d instruction records, %d outside any manifest fn)" % (TRACE, n, outside))
print("executed functions: %d of %d in manifest\n" % (tot_fn, len(man)))
print("%-12s %8s %10s %14s" % ("class", "funcs", "code-bytes", "instr-executed"))
for k in ("matched-C", "unmatched-C", "no-C"):
    print("%-12s %8d %10d %13.1f%%" % (k, kcount[k], kbytes[k], 100.0 * kinstr[k] / max(tot_in, 1)))
print("%-12s %8d %10d %14d" % ("TOTAL", tot_fn, tot_by, tot_in))
print("\n=> %.1f%% of executed code-bytes are byte-exact our-C; %.1f%% of executed instructions.\n"
      % (100.0 * kbytes["matched-C"] / max(tot_by, 1), 100.0 * kinstr["matched-C"] / max(tot_in, 1)))

# prioritized work backlog: executed functions that are NOT byte-exact yet (unmatched-C + any no-C),
# ranked by how much they run. These are the functions to get byte-matching to reach 100% our-C on the path.
backlog = sorted((f for f in executed if klass(f) != "matched-C"), key=lambda f: -hits[f["addr"]])
print("BYTE-MATCH BACKLOG -- executed functions not yet byte-exact (ranked by instructions run):")
print("%-10s %8s %7s %-12s  %s" % ("addr", "instr", "bytes", "class", "name"))
for f in backlog[:30]:
    print("0x%-8s %8d %7d %-12s  %s" % (f["addr"].lstrip("0") or "0", hits[f["addr"]], f["size"],
                                        klass(f), f["name"]))
print("\n(%d executed functions still need byte-matching)" % len(backlog))

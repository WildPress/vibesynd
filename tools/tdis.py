#!/usr/bin/env python3
"""tdis.py <name> -- disassemble one target function from the linear.bin, in manifest coords."""
import json, sys, capstone
name = sys.argv[1]
man = json.load(open("manifest/functions.json"))
base = int(man["image_base"], 16)
f = [x for x in man["functions"] if x["name"] == name][0]
seg = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()
off = int(f["addr"], 16) - base
tb = seg[off:off + f["size"]]
print("addr", f["addr"], "size", f["size"], "calls", f.get("calls"), "data_refs", f.get("data_refs"))
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for i in md.disasm(tb, int(f["addr"], 16)):
    print("%08x %-24s %s %s" % (i.address, " ".join("%02x" % b for b in i.bytes), i.mnemonic, i.op_str))

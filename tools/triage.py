import json, struct
from collections import Counter
m = json.load(open("manifest/functions.json"))
base = int(m["image_base"], 16)
seg = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()
cats = Counter(); ex = {}
for f in m["functions"]:
    if f["status"] != "unmatched": continue
    off = int(f["addr"], 16) - base; b = seg[off:off + f["size"]]; sz = f["size"]
    if sz < 2: c = "tiny(<2)"
    elif b[0] == 0x55 and b[1] == 0x89: c = "FRAMED(55 89)"           # blocked class
    elif b[0] == 0xB8 and sz == 6 and b[5] == 0xC3: c = "ret_const(b8..c3)"
    elif b[0] == 0xA1 and sz == 6 and b[5] == 0xC3: c = "getter_int(a1)"
    elif b[0] == 0xA3 and sz == 6 and b[5] == 0xC3: c = "setter_int(a3)"
    elif b[0] == 0xA0 and sz == 6: c = "getter_byte(a0)"
    elif b[0] == 0x0F and b[1] in (0xB6, 0xBE) and b[2] == 0x05: c = "getter_zx(0fb6/be)"
    elif b[0] == 0xB8 and sz >= 10 and 0xE9 in b[5:7]: c = "tailthunk(b8..e9)"
    elif b[0] == 0xE9: c = "jmp_thunk(e9)"
    elif b[0] in (0x8B, 0x0F) and 0xE8 in b: c = "fwd_call(mov..call)"
    elif b[0] == 0xFF and b[1] in (0x74, 0x24): c = "push_mem/jmptbl(ff)"
    else: c = f"other({b[0]:02x})"
    cats[c] += 1; ex.setdefault(c, f'{f["name"]} sz={sz}')
tot = sum(cats.values())
print(f"unmatched={tot}")
for c, n in cats.most_common(30):
    print(f"  {n:4}  {c:22} e.g. {ex[c]}")

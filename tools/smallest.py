import json
m = json.load(open("manifest/functions.json"))
base = int(m["image_base"], 16)
seg = open("inputs/SYNDICAT_MAIN_OBJECT1.linear.bin", "rb").read()
u = [f for f in m["functions"] if f["status"] == "unmatched" and f["size"] >= 5]
def firstbytes(f):
    off = int(f["addr"], 16) - base
    return seg[off:off + min(f["size"], 12)].hex()
u.sort(key=lambda f: f["size"])
shown = 0
for f in u:
    b = firstbytes(f)
    if b.startswith("5589e5"):   # framed lean-epilogue class -> skip
        continue
    print(f'  {f["addr"]} sz={f["size"]:>3} calls={f["calls"]} data={f["data_refs"]} {f["name"]}  {b}')
    shown += 1
    if shown >= 22:
        break

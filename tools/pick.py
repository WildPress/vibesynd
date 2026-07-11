import json, sys
m = json.load(open("manifest/functions.json"))
fns = [f for f in m["functions"] if f["status"] == "unmatched"]
maxsize = int(sys.argv[1]) if len(sys.argv) > 1 else 40
maxcalls = int(sys.argv[2]) if len(sys.argv) > 2 else 1
c = [f for f in fns if f["size"] <= maxsize and f["calls"] <= maxcalls and f["data_refs"] <= 2]
c.sort(key=lambda f: (f["size"], f["calls"]))
matched = sum(1 for f in m["functions"] if f["status"] == "matched")
print(f"matched={matched}/{len(m['functions'])}  unmatched={len(fns)}  candidates(size<={maxsize})={len(c)}")
for f in c[:40]:
    print(f'  {f["addr"]} sz={f["size"]:>3} calls={f["calls"]} data={f["data_refs"]} inc={f["incoming"]} {f["name"]}')

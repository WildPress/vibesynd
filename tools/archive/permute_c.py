#!/usr/bin/env python3
"""C-variation search. Takes a template with inline alternatives and sweeps every
combination through the fast batched compiler, looking for a byte-identical match.
For functions that are correct except for a compiler-choice divergence (register,
schedule, fold) where SOME C spelling flips it but we can't find it by hand.

Alternatives are written  $[ optionA $| optionB $| optionC $]  inline in the
template (the $ delimiters don't clash with C). Example:

    int index = $[col + row*128 $| row*128 + col $];
    unsigned $[short$|int$] tile = ...;

Run in-container (uses native /tmp + batched DOSBox, ~200 compiles/sec):
  docker run --rm -v "$PWD":/work -w /work synd-decomp \
      python3 tools/permute_c.py FUN_00033fb8 tmpl/FUN_00033fb8.c "-4s -oneatx -zp8 -s -zq"

Saves the winning C to src/<name>.c.match. The real src/<name>.c is untouched.
"""
import json, subprocess, sys, re, os, itertools
import multiprocessing as mp
from omf import text_bytes_and_fixups

SEG = "inputs/SYNDICAT_MAIN_OBJECT1.linear.bin"
MAN = "manifest/functions.json"
RX = re.compile(r"\$\[(.*?)\$\]", re.S)

def parse(tmpl):
    """-> (literal_parts, choices) so render = interleave(parts, chosen)."""
    parts, choices, last = [], [], 0
    for m in RX.finditer(tmpl):
        parts.append(tmpl[last:m.start()])
        choices.append([o.strip() for o in m.group(1).split("$|")])
        last = m.end()
    parts.append(tmpl[last:])
    return parts, choices

def mask(b, fx):
    b = bytearray(b)
    for off, size in fx:
        for j in range(off, min(off + size, len(b))):
            b[j] = 0
    return bytes(b)

G = {}
def init_worker(parts, choices, flags, target):
    wid = (mp.current_process()._identity or [0])[0]
    work = f"/tmp/dosw_{wid}"; os.makedirs(work, exist_ok=True)
    G.update(parts=parts, choices=choices, flags=flags, target=target, work=work)

def render(combo):
    s = G["parts"][0]
    for i, opt in enumerate(combo):
        s += opt + G["parts"][i + 1]
    return s

def try_batch(batch):
    W = G["work"]
    subprocess.run(f"rm -f {W}/SRC*.C {W}/O*.OBJ", shell=True)
    for j, combo in enumerate(batch):
        with open(f"{W}/SRC{j:02d}.C", "w") as f:
            f.write(render(combo)); f.flush(); os.fsync(f.fileno())
    subprocess.run(["bash", "tools/wcc95_batch.sh", W, G["flags"]], capture_output=True)
    tb, out = G["target"], []
    for j, combo in enumerate(batch):
        p = f"{W}/O{j:02d}.OBJ"
        if not os.path.exists(p):
            out.append((combo, -1, False)); continue
        try:
            ob, fx = text_bytes_and_fixups(p)
        except Exception:
            out.append((combo, -1, False)); continue
        out.append((combo, len(ob), len(tb) == len(ob) and mask(tb, fx) == mask(ob, fx)))
    return out

def main():
    name, tmpl_path, flags = sys.argv[1], sys.argv[2], sys.argv[3]
    workers = int(sys.argv[sys.argv.index("--workers")+1]) if "--workers" in sys.argv else 24
    B = int(sys.argv[sys.argv.index("--batch")+1]) if "--batch" in sys.argv else 40

    man = json.load(open(MAN)); base = int(man["image_base"], 16)
    f = next(x for x in man["functions"] if x["name"] == name)
    off = int(f["addr"], 16) - base
    target = open(SEG, "rb").read()[off:off + f["size"]]
    parts, choices = parse(open(tmpl_path).read())
    combos = list(itertools.product(*choices))
    print(f"{name}: {len(choices)} choice points "
          f"({'x'.join(str(len(c)) for c in choices)}) = {len(combos)} combos, "
          f"target={len(target)}B", flush=True)

    if not os.path.isdir("/tmp/wat"):
        subprocess.run("cp -r /work/toolchain/watcom95 /tmp/wat", shell=True)
    os.environ["WAT_ROOT"] = "/tmp/wat"
    batches = [combos[i:i+B] for i in range(0, len(combos), B)]
    sizes, best, done = {}, None, 0
    pool = mp.Pool(workers, initializer=init_worker, initargs=(parts, choices, flags, target))
    for results in pool.imap_unordered(try_batch, batches):
        for combo, size, matched in results:
            done += 1
            sizes[size] = sizes.get(size, 0) + 1
            if size != -1 and (best is None or abs(size-len(target)) < abs(best[1]-len(target))):
                best = (combo, size)
            if matched:
                open(f"src/{name}.c.match", "w").write(render(combo))
                print(f"\n*** MATCH: {combo} -> saved src/{name}.c.match ***", flush=True)
                pool.terminate(); return
        print(f"  [{done}/{len(combos)}] best={best[1] if best else '-'}B "
              f"sizes={dict(sorted(sizes.items()))}", flush=True)
    pool.close(); pool.join()
    print(f"\nno match. target={len(target)}B  closest={best}", flush=True)

if __name__ == "__main__":
    main()

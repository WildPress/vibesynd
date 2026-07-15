#!/usr/bin/env python3
"""permwatch.py -- live TUI for cpermute permuter runs.

Run cpermute with  --status dashboard/permute  (each run writes DIR/<name>.json every anneal step),
then in another terminal:

    python tools/permwatch.py                      # watches dashboard/permute/, redraws every 1s
    python tools/permwatch.py <dir> --once          # one snapshot, then exit (for scripts/CI)

Shows one row per function being permuted: an iteration progress bar, the best masked-byte score vs
the target size (with a bar), throughput (compiles/sec), annealing temperature, and status. A row
goes green on an exact match. Reads only the small JSON status files -- zero load on the search, and
it can watch container runs from the Windows host because the repo dir is the shared mount.

Pure stdlib, no deps. Enables ANSI on Windows terminals automatically.
"""
import json, os, sys, time, glob

ESC = "\033["
def c(s, col): return f"{ESC}{col}m{s}{ESC}0m"
GREEN, AMBER, GREY, CYAN, DIM, BOLD, RED = "32", "33", "90", "36", "2", "1", "31"

BAR_W = 22
BLOCKS = " ▏▎▍▌▋▊▉█"


def bar(frac, w=BAR_W, col=GREEN):
    frac = 0.0 if frac < 0 else 1.0 if frac > 1 else frac
    full = int(frac * w)
    rem = frac * w - full
    partial = BLOCKS[int(rem * 8)] if full < w else ""
    s = "█" * full + partial
    s = s + " " * (w - len(s))
    return c(s, col)


def load(d):
    rows = []
    for p in glob.glob(os.path.join(d, "*.json")):
        try:
            with open(p) as fh:
                rows.append(json.load(fh))
        except Exception:
            continue
    return rows


def render(rows, d):
    now = time.time()
    out = []
    out.append(c("  permuter monitor", BOLD) + c(f"   {d}   {time.strftime('%H:%M:%S')}", DIM))
    out.append("")
    hdr = f"  {'function':<22}{'iterations':<26}{'best / target':<24}{'rate':>9}  {'T':>6}  status"
    out.append(c(hdr, DIM))
    out.append(c("  " + "─" * 96, GREY))

    # running first (most recent ts), then matched, then done; within, by best% desc
    def sortkey(r):
        idle = now - r.get("ts", 0) > 8 and not r.get("done")
        stage = 0 if (not r.get("done") and not idle) else (1 if r.get("matched") else 2)
        tgt = max(r.get("target", 1), 1)
        return (stage, -(r.get("best", 0) / tgt))
    rows = sorted(rows, key=sortkey)

    n_match = n_run = 0
    tot_rate = 0.0
    for r in rows:
        name = r.get("name", "?")[:22]
        it, tot = r.get("iters", 0), max(r.get("total", 1), 1)
        best, tgt = r.get("best", 0), max(r.get("target", 1), 1)
        rate = r.get("rate", 0.0)
        T = r.get("T")
        matched = r.get("matched")
        done = r.get("done")
        idle = now - r.get("ts", 0) > 8 and not done

        ipct = it / tot
        spct = best / tgt if best >= 0 else 0
        ibar = bar(ipct, col=CYAN)
        scol = GREEN if matched else (AMBER if spct > 0.6 else RED)
        sbar = bar(spct, w=12, col=scol)

        if matched:
            status = c("● MATCH", GREEN); n_match += 1
        elif done:
            status = c("done", DIM)
        elif idle:
            status = c("idle", GREY)
        else:
            status = c(f"{r.get('phase','?')}", AMBER); n_run += 1; tot_rate += rate

        nm = c(f"{name:<22}", GREEN if matched else BOLD)
        itcol = f"{ibar} {it:>5}/{tot:<6}"
        sccol = f"{sbar} {best:>4}/{tgt:<5}"
        Ts = f"{T:6.1f}" if isinstance(T, (int, float)) else "   -- "
        out.append(f"  {nm}{itcol}  {sccol}{rate:8.0f}/s  {Ts}  {status}")

    out.append(c("  " + "─" * 96, GREY))
    out.append(c(f"  {len(rows)} functions   ", DIM)
               + c(f"{n_run} running", AMBER) + c("  ·  ", DIM)
               + c(f"{n_match} matched", GREEN) + c("  ·  ", DIM)
               + c(f"{tot_rate:.0f} compiles/s total", CYAN))
    return "\n".join(out)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    d = args[0] if args else os.path.join("dashboard", "permute")
    once = "--once" in sys.argv
    if os.name == "nt":
        os.system("")                       # enable ANSI/VT processing on Windows terminals
    try:
        sys.stdout.reconfigure(encoding="utf-8")   # block/● glyphs aren't in the cp1252 console codepage
    except Exception:
        pass

    if once:
        rows = load(d)
        print(render(rows, d) if rows else f"  no status files in {d}/ yet")
        return

    sys.stdout.write(ESC + "2J")            # clear once
    try:
        while True:
            rows = load(d)
            frame = render(rows, d) if rows else c(f"  waiting for status files in {d}/ …", DIM)
            # home cursor, redraw with clear-to-end-of-line per row to avoid flicker
            sys.stdout.write(ESC + "H")
            for line in frame.split("\n"):
                sys.stdout.write(line + ESC + "K\n")
            sys.stdout.write(ESC + "J")     # clear anything below
            sys.stdout.flush()
            time.sleep(1.0)
    except KeyboardInterrupt:
        sys.stdout.write("\n")


if __name__ == "__main__":
    main()

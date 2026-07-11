# Function manifest

`functions.json` is the single source of truth for the decompilation's coverage.
It is generated from Ghidra's analysis of the original binary and updated by the
matching harness as functions are matched.

## Schema

```jsonc
{
  "binary": "SYNDICAT_MAIN.EXE",
  "segment": "OBJECT1",          // code segment the addresses live in
  "image_base": "0x10000",       // base the addresses are relative to
  "generated": "<ghidra build tag>",
  "functions": [
    {
      "addr": "0x00012a40",      // start address (Ghidra image space)
      "name": "sub_12a40",       // Ghidra name, renamed as we learn intent
      "size": 42,                // bytes
      "calls": 0,                // number of outgoing calls (0 = leaf)
      "data_refs": 0,            // referenced globals/constants
      "incoming": 3,             // xref count (callers)
      "status": "unmatched",     // unmatched | wip | matched | equivalent
      "match_pct": 0.0,          // 0..100, last diff result
      "src": null,               // path under src/ once drafted, e.g. "src/sub_12a40.c"
      "notes": ""
    }
  ]
}
```

## Status values

- `unmatched` — identified, no C written yet.
- `wip` — C drafted, not yet byte-identical.
- `matched` — compiles to byte-identical assembly (the goal).
- `equivalent` — behaviourally correct but a known non-matching codegen
  difference we accept for now (documented in `notes`).

## Picking the next function

Leaf functions (`calls == 0`, `data_refs == 0`, small `size`) are the cheapest
first matches. The harness sorts by these to suggest the next target.

# Function manifest

`functions.json` is the single source of truth for the decompilation's coverage.
It's generated from Ghidra's analysis of the original binary and updated by the
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

- `unmatched`: identified, no C written yet.
- `wip`: C drafted, not yet byte-identical.
- `matched`: compiles to byte-identical assembly, which is the goal.
- `equivalent`: behaviourally correct, but with a known non-matching codegen
  difference we accept for now (documented in `notes`).

## Picking the next function

Leaf functions (`calls == 0`, `data_refs == 0`, small `size`) are the cheapest first
matches. The harness sorts by these to suggest the next target.

## Runtime library

The framed functions near the top of the code segment (`0x3a000` and up) are the
linked-in Watcom C runtime, not game code. They're identified and named in
`library_functions.md`, and tagged `LIBRARY` in Ghidra. Skip them when picking
decompilation targets.

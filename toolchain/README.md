# Toolchain: period Watcom C/C++ (preserved locally, not in git)

The compilers that a version of the original *Syndicate* was built with. They're
abandonware, kept locally for the matching decompilation. The binaries are
git-ignored (`toolchain/*`). Only this provenance file is tracked, so the identity
of each download survives even if the local bits or the upstream mirror are lost.

Two compilers are staged. **10.0a** is the primary one we match against. **9.5b** is
kept for A/B tests (its provenance and the download steps are in `AGENTS.md`, session
log cont. 8).

## Canonical artifact (preserve this one)

| | |
|---|---|
| File | `Watcom_CPP_10.0a.7z` |
| Size | 138,125,608 bytes (132 MiB) |
| sha256 | `37f2ac13b414e441fe2a76df372263af433b142cf1de72ec431d4a7a2622568a` |
| Contains | `WATCOM_C10A.ISO` (317,562,880 B, sha256 `f51ed9358e7eaf6a0d292268bbbc15832530489dd8239a98f6938c244f1736fc`) |
| Compiler | "WATCOM C32 Optimizing Compiler Version 10.0a" |

Everything else under `toolchain/` (`iso_out/`, `watcom10a_iso/`, `watcom10a/`,
`doswork/`) is re-derivable from this .7z and can be deleted to reclaim space.

## Source

- WinWorldPC product page: <https://winworldpc.com/product/watcom-c-c/100>
- Download entry: *Watcom CPP 10.0a (1994) (ISO)*, `/download/46c3bfc2-ade2-809c-18c3-9a11c3a4efbf`
- Mirror hit: `https://dl-alt1.winworldpc.com/.../Watcom CPP 10.0a.7z`
- Obtained: 2026-07-10.
- Mirrors can disappear, so verify by the sha256 above. If it's gone, try archive.org,
  the WinWorldPC library, or another Watcom 10.0/10.0a preservation copy.

## Re-stage from the .7z (what the pipeline needs)

```bash
# in the container (needs p7zip):
7z x Watcom_CPP_10.0a.7z -oiso_out
7z x "iso_out/Watcom CPP 10.0a/WATCOM_C10A.ISO" -owatcom10a_iso
mkdir -p watcom10a/WATCOM
cp -r watcom10a_iso/WATCOM/{BINB,BIN,H,LIB386,LIB286,BINW} watcom10a/WATCOM/
```

Then compile with `tools/wcc_dos.sh <name>` (DOSBox plus WCC386.EXE via W32RUN).

## Notes

- Watcom 10.0 has no `-6` CPU level (it rejects `-6r` with E1073), so use `-5r`, `-4r`,
  or `-3r`.
- `BINB/WCC386.EXE` is a Win32 app that needs `W32RUN.EXE`, so put `C:\WATCOM\BIN` on
  the PATH.
- Open Watcom v2 only agrees with 10.0a on functions that make no register-allocation
  choices. On real code its register allocator diverges, so 10.0a is the compiler we
  match against, not OW v2 (see `AGENTS.md`).
- This is the only durable copy, so keep an off-machine backup. Git won't hold it.

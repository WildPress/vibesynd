# Conventions

How we commit and version this decompilation. It isn't a normal software project,
but a bit of structure makes the progress readable.

## Commits

Conventional Commits, with types adapted for a matching decompilation.

- `match`: one or more functions matched byte-identical. This is the core progress.
- `lib`: runtime-library identification, naming, or tagging.
- `tools`: harness and analysis scripts.
- `docs`: documentation and notes.
- `chore`: repo maintenance, toolchain, config.
- `fix`: a correction, for example a wrong match, a tool bug, or a stale fact.
- `refactor`: restructuring with no change in behaviour.

Format is `type: short summary`, or `type(scope): summary` where a scope helps (a
subsystem name or an address range). Keep the summary in the imperative and under
about 70 characters.

Example: `match: 6 framed forwarders via -3s -of (0x3aa74..0x3e48e)`.

The body explains what and why when it isn't obvious. End commits with the
Co-Authored-By trailer.

## Versioning

SemVer, read as decompilation progress rather than as an API.

- `0.MINOR.PATCH` while the decompilation is incomplete.
- `1.0.0` is reserved for a byte-complete match of the game's own code.
- Bump PATCH for ordinary work: match batches, tooling, docs.
- Bump MINOR at a milestone, such as a subsystem fully matched, or another 10% of
  game byte coverage.
- Tag milestones as `vMAJOR.MINOR.PATCH`.

The version marks milestones. The continuous progress number is the completion
score, not the version.

## Completion score

`tools/score.py` reports coverage from the manifest. Byte coverage is the honest
headline, because early matches are tiny and function count flatters progress.
Runtime-library functions are split out, since they'll be linked from the original
library rather than decompiled, so the real target is the game's own code.

```bash
python3 tools/score.py           # human summary
python3 tools/score.py --json    # machine-readable
```

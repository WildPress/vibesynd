# Conventions

This project uses **[Conventional Commits](https://www.conventionalcommits.org/)** and
**[Semantic Versioning](https://semver.org/)**.

## Commits

Format: `type(scope): summary`

**Types** (the only valid ones):

| type | for |
|---|---|
| `feat` | a new capability |
| `fix` | a bug fix |
| `docs` | documentation only |
| `refactor` | code change that neither fixes a bug nor adds a feature |
| `perf` | a performance improvement |
| `test` | tests only |
| `build` | build system, tooling, or link/data pipeline |
| `ci` | CI configuration |
| `chore` | housekeeping (deps, ignores, tags) |
| `revert` | reverts a previous commit |

**Scopes** (optional, this repo's common ones): `decomp`, `port`, `tools`, `data`, `docs`,
plus a subsystem where useful (`render`, `sound`, `mission`, …).

Breaking changes: add `!` after the type/scope (`feat(port)!: …`) or a `BREAKING CHANGE:` footer.

Examples:
- `feat(port): SDL2 video backend with GPU-textured framebuffer`
- `build(port): place globals at DGROUP offsets so field-views alias`
- `fix(decomp): find_projectile_step dir arg is int, not char`
- `docs: acknowledge FreeSynd as inspiration`
- `chore: delete stale fix branch`

The matching-decompilation work on `main` should treat a byte-neutral naming/comment change as
`refactor` or `docs`; anything that changes emitted bytes is not allowed on `main` (see
`BRANCHES.md`).

## Versioning

Semantic Versioning, `MAJOR.MINOR.PATCH`, via annotated git tags.

- **`main` (the decompilation)** — `v1.0.0` is "the engine plays the game". Fidelity and
  runtime improvements bump `MINOR`; corrections bump `PATCH`. A change to what "matched" means
  or the build contract bumps `MAJOR`.
- **`port` (the native build)** — versioned independently with a `port-` prefix while it is
  pre-1.0, e.g. `port-v0.1.0` for the first release that renders, `port-v1.0.0` when it plays a
  mission natively. Pre-release milestones may use `-alpha`/`-beta` (`port-v0.1.0-alpha`).

Tag from the branch the release belongs to. Both branches require the user's own *Syndicate*
data files at runtime; no assets are ever distributed.

#!/usr/bin/env python3
"""Turn function references in a wiki markdown file into links to the source file.

Runs at wiki-sync time (see .github/workflows/wiki.yml), so the docs/ source stays
clean, portable markdown while the wiki mirror gets clickable function links.

For every function that has a source file in the manifest, this links:
  - an address like 0x2fe68 (bare, or in backticks), matched to the function START
  - a function name or label like persuade_capture or FUN_0002fe68 (in backticks)
to the source file on GitHub. Fenced code blocks and mermaid diagrams are left alone.

  python3 tools/wiki_autolink.py <file.md> [<file.md> ...]
"""
import json, os, re, sys

REPO = os.environ.get("GITHUB_REPOSITORY", "WildPress/vibesynd")
BRANCH = os.environ.get("WIKI_LINK_BRANCH", "main")
BASE = "https://github.com/%s/blob/%s/" % (REPO, BRANCH)


def norm(hexstr):
    return hexstr.lower().lstrip("0") or "0"


def load_maps():
    """addr (normalised hex) -> url, and name/label -> url, for functions with a src file."""
    man = json.load(open("manifest/functions.json"))["functions"]
    by_addr = {}
    addr_map, name_map = {}, {}
    for f in man:
        src = f.get("src")
        if not src:
            continue
        url = BASE + src.replace("\\", "/")
        addr_map[norm(f["addr"])] = url
        name_map[f["name"]] = url
        by_addr[f["addr"]] = url
    # add semantic labels (names.json) that differ from the manifest name
    try:
        names = json.load(open("manifest/names.json"))
        for fn, info in names.items():
            label = info.get("label")
            url = by_addr.get(info.get("addr"))
            if label and url:
                name_map[label] = url
    except (FileNotFoundError, ValueError):
        pass
    return addr_map, name_map


TOKEN = re.compile(r"`([^`]+)`|(0x[0-9a-fA-F]+)\b")
ADDR_ONLY = re.compile(r"0x([0-9a-fA-F]+)$")
NAME_ONLY = re.compile(r"(?:FUN_[0-9a-fA-F]+|[A-Za-z_][A-Za-z0-9_]*)$")


def linkify(text, addr_map, name_map):
    def repl(m):
        if m.group(1) is not None:                 # `backticked` content
            c = m.group(1).strip()
            url = None
            a = ADDR_ONLY.match(c)
            if a:
                url = addr_map.get(norm(a.group(1)))
            elif NAME_ONLY.match(c):
                url = name_map.get(c)
            return "[`%s`](%s)" % (c, url) if url else m.group(0)
        c = m.group(2)                             # bare 0xADDR
        url = addr_map.get(norm(c[2:]))
        return "[%s](%s)" % (c, url) if url else c
    return TOKEN.sub(repl, text)


def process(path, addr_map, name_map):
    out, incode = [], False
    n = 0
    for ln in open(path, encoding="utf-8").read().split("\n"):
        if ln.strip().startswith("```"):
            incode = not incode
            out.append(ln)
            continue
        if incode:
            out.append(ln)
            continue
        new = linkify(ln, addr_map, name_map)
        n += new.count("](https://github.com/") - ln.count("](https://github.com/")
        out.append(new)
    open(path, "w", encoding="utf-8", newline="\n").write("\n".join(out))
    return n


def main():
    addr_map, name_map = load_maps()
    for p in sys.argv[1:]:
        added = process(p, addr_map, name_map)
        print("%s: %d function links" % (p, added))


if __name__ == "__main__":
    main()

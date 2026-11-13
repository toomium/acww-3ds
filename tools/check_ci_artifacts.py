#!/usr/bin/env python3
"""Fail if a CI workflow uploads anything but the allowed files.

Usage:
    python tools/check_ci_artifacts.py [WORKFLOW.yml ...]

A build contains the game's code and the web builder handles the player's ROM, so nothing from them may leave a
run except a release's files (the program and its checksum) and the website (web/ and the release's program).
This reads every actions/upload-artifact and actions/upload-pages-artifact step of the workflows (default:
.github/workflows) and checks each uploaded path against ALLOWED. Add to the list only paths with no game data.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

ALLOWED = [
    "dist/release/",  # a release's files: acww.3dsx, acww.cia and SHA256SUMS.txt (build.yml)
    "_site",          # the website: web/ and the release's program (pages.yml, tools/build_site.py)
]


def uploaded_paths(text: str) -> list[str]:
    """The `path:` of every upload step, as written (a step without one gives a placeholder that fails)."""
    paths = []
    # A step starts at a "- " item of a steps list; its keys follow, indented deeper.
    for step in re.split(r"\n\s*- (?=name:|uses:|id:)", text):
        if not re.search(r"uses:\s*actions/upload-(pages-)?artifact@", step):
            continue
        m = re.search(r"^[ \t]*path:[ \t]*(\|)?[ \t]*(\S.*?)?[ \t]*$((?:\n[ \t]+[^\s#].*)*)", step, re.M)
        if not m:
            paths.append("<no path>")
            continue
        # a block (path: |) lists one path per line; keep only its lines, not the keys after it
        lines = [m.group(2)] if m.group(2) else []
        if m.group(1):
            lines += [l.strip() for l in m.group(3).splitlines() if l.strip() and not re.match(r"\s*[\w-]+:", l)]
        paths += lines or ["<empty path>"]
    return paths


def check(name: str, text: str) -> list[str]:
    return [f"{name}: uploads {p!r}" for p in uploaded_paths(text) if p not in ALLOWED]


def main() -> int:
    files = [Path(a) for a in sys.argv[1:]] or sorted((ROOT / ".github" / "workflows").glob("*.y*ml"))
    bad = [line for f in files for line in check(f.name, f.read_text(encoding="utf-8"))]
    if bad:
        print("CI uploads something not on the allowed list (tools/check_ci_artifacts.py):")
        print("\n".join(f"  {line}" for line in bad))
        return 1
    print(f"CI uploads: only allowed paths ({len(files)} workflows)")
    return 0


if __name__ == "__main__":
    sys.exit(main())

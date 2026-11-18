#!/usr/bin/env python3
"""Put the web builder's site together: web/, and a release's program if there is one.

Usage:
    python tools/build_site.py --out _site                              # the site alone (no build on it)
    python tools/build_site.py --out _site --release-dir dist --tag v0.1.0

--release-dir is a folder holding a release's acww.3dsx (the Pages workflow downloads it with `gh`). The
site then carries it as release/acww.3dsx, with release/release.json (the tag, the size, the SHA-256),
and the page offers it. A release's acww.cia (the HOME Menu version, no game code), if it is in the folder,
goes along as release/acww.cia, and release.json says so ("cia": true); the same for the Switch's program,
acww.nro ("nro": true). Without them the page asks the visitor for their own build.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PROGRAM = "acww.3dsx"
CIA = "acww.cia"
NRO = "acww.nro"


def build(out: Path, release_dir: Path | None, tag: str) -> None:
    if out.exists():
        shutil.rmtree(out)
    shutil.copytree(ROOT / "web", out)
    if release_dir is None:
        return
    program = release_dir / PROGRAM
    if not program.is_file():
        sys.exit(f"{program}: not found")
    data = program.read_bytes()
    (out / "release").mkdir()
    (out / "release" / PROGRAM).write_bytes(data)
    info = {"tag": tag, "size": len(data), "sha256": hashlib.sha256(data).hexdigest()}
    # (a release from before the .cia, or the Switch, has none)
    for name, key in ((CIA, "cia"), (NRO, "nro")):
        if (release_dir / name).is_file():
            shutil.copy(release_dir / name, out / "release" / name)
            info[key] = True
    (out / "release" / "release.json").write_text(json.dumps(info) + "\n", encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--release-dir", type=Path)
    parser.add_argument("--tag", default="")
    args = parser.parse_args()
    if args.release_dir is not None and not args.tag:
        sys.exit("--release-dir needs --tag")
    build(args.out, args.release_dir, args.tag)
    print(f"Site: {args.out}" + (f" (with {args.tag})" if args.release_dir else " (no build on it)"))


if __name__ == "__main__":
    main()

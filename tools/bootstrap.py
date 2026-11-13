#!/usr/bin/env python3
"""Fetch the pinned decompilation, apply our patches and link in our own files.

Usage:
    python tools/bootstrap.py                    # fetch (if needed), patch, link
    python tools/bootstrap.py --clean            # delete build/upstream and start over
    python tools/bootstrap.py --source PATH|URL  # fetch from here instead of the url in upstream.lock

Result:
    build/upstream/     7ucina/acww-decomp at the commit in upstream.lock, with patches/acww-decomp/*.patch
                        applied and every file of 3ds_port/ linked in (symlink) at its path.

Run it again without --clean to refresh the links after adding a file to 3ds_port/.
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LOCK_FILE = ROOT / "upstream.lock"
UPSTREAM_DIR = ROOT / "build" / "upstream"
PATCH_DIR = ROOT / "patches" / "acww-decomp"
PORT_DIR = ROOT / "3ds_port"

# Written into build/upstream/.git once the patches are applied: tells a second run that they are.
DONE_MARK = "acww3ds-bootstrapped"
EXCLUDE_BEGIN = "# acww-3ds links begin"
EXCLUDE_END = "# acww-3ds links end"


def read_lock(path: Path) -> dict[str, str]:
    """Parse key=value lines, ignoring blanks and # comments."""
    values: dict[str, str] = {}
    for line_no, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        if "=" not in line:
            sys.exit(f"{path.name}:{line_no}: expected key=value, got {raw!r}")
        key, value = line.split("=", 1)
        values[key.strip()] = value.strip()
    for required in ("url", "commit"):
        if required not in values:
            sys.exit(f"{path.name}: missing '{required}='")
    return values


def git(*args: str, cwd: Path | None = None, capture: bool = False) -> str:
    result = subprocess.run(["git", *args], cwd=cwd, check=False, text=True, capture_output=capture)
    if result.returncode != 0:
        detail = (result.stderr or "").strip() if capture else ""
        sys.exit(f"git {' '.join(args)} failed{': ' + detail if detail else ''}")
    return (result.stdout or "").strip() if capture else ""


def fetch(dest: Path, source: str, commit: str) -> None:
    """Fetch exactly one commit (no history). A local path is fetched through file:// so that --depth works."""
    if Path(source).is_dir():
        source = Path(source).resolve().as_uri()
    print(f"Fetching {source} @ {commit[:12]} ...")
    dest.mkdir(parents=True, exist_ok=True)
    git("init", "--quiet", cwd=dest)
    git("remote", "add", "origin", source, cwd=dest)
    git("fetch", "--quiet", "--depth", "1", "origin", commit, cwd=dest)
    git("checkout", "--quiet", "--detach", "FETCH_HEAD", cwd=dest)
    got = git("rev-parse", "HEAD", cwd=dest, capture=True)
    if got != commit:
        sys.exit(f"Fetched {got} but upstream.lock says {commit}.")


def apply_patches(dest: Path, patch_dir: Path) -> None:
    for patch in sorted(patch_dir.glob("*.patch")):
        print(f"  applying {patch.name}")
        result = subprocess.run(["git", "apply", str(patch)], cwd=dest, text=True, capture_output=True, check=False)
        if result.returncode != 0:
            sys.exit(f"Patch {patch.name} does not apply:\n{result.stderr.strip()}")


def port_files(port_dir: Path) -> list[Path]:
    """The files of 3ds_port/, as paths relative to it."""
    return sorted(p.relative_to(port_dir) for p in port_dir.rglob("*") if p.is_file() and "__pycache__" not in p.parts)


def link_port(dest: Path, port_dir: Path) -> int:
    """Symlink every file of port_dir into dest at its path; drop links to files that are gone."""
    for link in dest.rglob("*"):
        if ".git" in link.relative_to(dest).parts or not link.is_symlink():
            continue
        target = Path(os.path.realpath(link))
        if port_dir.resolve() in target.parents and not target.exists():
            link.unlink()
    files = port_files(port_dir)
    for rel in files:
        link = dest / rel
        link.parent.mkdir(parents=True, exist_ok=True)
        if link.is_symlink():
            link.unlink()
        elif link.exists():
            sys.exit(f"{rel} exists in the decompilation; 3ds_port/ must hold only files that do not.")
        link.symlink_to(os.path.relpath(port_dir / rel, link.parent))
    # Keep the links out of `git status` and `git add -A` inside build/upstream.
    exclude = dest / ".git" / "info" / "exclude"
    exclude.parent.mkdir(exist_ok=True)
    old = exclude.read_text(encoding="utf-8") if exclude.exists() else ""
    if EXCLUDE_BEGIN in old:
        old = old[:old.index(EXCLUDE_BEGIN)] + old[old.index(EXCLUDE_END) + len(EXCLUDE_END) + 1:]
    block = "\n".join([EXCLUDE_BEGIN, *("/" + rel.as_posix() for rel in files), EXCLUDE_END]) + "\n"
    exclude.write_text(old + block, encoding="utf-8")
    return len(files)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--clean", action="store_true", help="delete build/upstream first")
    parser.add_argument("--source", help="fetch from this path or url instead of the one in upstream.lock")
    args = parser.parse_args()

    if shutil.which("git") is None:
        sys.exit("Git is not installed or not on PATH.")
    if args.clean and UPSTREAM_DIR.exists():
        print("Removing build/upstream")
        shutil.rmtree(UPSTREAM_DIR)

    lock = read_lock(LOCK_FILE)
    commit = lock["commit"]
    if not (UPSTREAM_DIR / ".git").exists():
        fetch(UPSTREAM_DIR, args.source or lock["url"], commit)
        apply_patches(UPSTREAM_DIR, PATCH_DIR)
        (UPSTREAM_DIR / ".git" / DONE_MARK).write_text(commit + "\n", encoding="utf-8")
    else:
        have = git("rev-parse", "HEAD", cwd=UPSTREAM_DIR, capture=True)
        if have != commit:
            sys.exit(f"build/upstream is at {have[:12]}, but {commit[:12]} is wanted.\nRun with --clean to re-fetch.")
        if not (UPSTREAM_DIR / ".git" / DONE_MARK).exists():
            sys.exit("build/upstream is incomplete (an earlier run stopped half way).\nRun with --clean to start over.")
        print(f"build/upstream: already at {commit[:12]} with the patches applied; refreshing the links only")

    print(f"Linked {link_port(UPSTREAM_DIR, PORT_DIR)} files of 3ds_port/")
    print("Done: build/upstream")


if __name__ == "__main__":
    main()

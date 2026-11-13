#!/usr/bin/env python3
"""Make patches/ and 3ds_port/ from a branch of the fork.

Usage:
    python tools/export_from_fork.py --fork /path/to/acww-decomp --base upstream/main --branch old3ds-o14

What it does, with D = the difference between --base (upstream's state) and --branch (the port's state):
  - a file D adds (it does not exist upstream) is copied to 3ds_port/<path>, with its file mode.
  - a file D changes or deletes (it exists upstream) goes into one patch under patches/acww-decomp/,
    chosen by the table AREAS below. Each such file is in exactly one patch.
  - files in SKIP are dropped on purpose (the fork's README.md and its screenshots; this repository has
    its own README and no pictures of the game).

The export stops with an error, and writes nothing, if a changed upstream file matches no area or two
areas, if a file is a symlink or changes its type, or if a file looks like game content (FORBIDDEN).
Only git's tracked content is read, so untracked files and the fork's symlinks to private notes never
get here. Everything is read with git; the fork is not modified.
"""

from __future__ import annotations

import argparse
import fnmatch
import io
import os
import shutil
import subprocess
import sys
import tarfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PATCH_DIR = ROOT / "patches" / "acww-decomp"
PORT_DIR = ROOT / "3ds_port"

# Files of the fork that are not exported (patterns).
SKIP = ["README.md", "docs/images/*"]

# Never copied or patched, whatever the area table says.
FORBIDDEN = ["*.nds", "*.srl", "*.sav", "*.cdc", "*.3dsx", "*.elf", "*.o", "build/*"]

# The area table: (patch name, short title, path patterns). A pattern is an fnmatch pattern on the
# path in the source tree ("*" also matches "/"). Every changed upstream file must match exactly one
# pattern of exactly one area; patterns therefore must not overlap. The patch number is the position
# in this list. To put a file somewhere new, add its pattern to one area.
AREAS: list[tuple[str, str, list[str]]] = [
    ("build-system-and-tools", "Build system, helper tools and ignore rules",
     [".gitignore", "tools/*"]),
    ("documentation", "Documentation of the runtime",
     ["docs/*"]),
    ("runtime-headers", "Shared headers of the runtime",
     ["include/*"]),
    ("runtime-core", "Runtime core: hardware registers, OS, SDK hooks, memory, coroutines, VRAM",
     ["modern/host/hw.c", "modern/host/os.c", "modern/host/sdk.c", "modern/host/host.h",
      "modern/host/main.c", "modern/host/ptr32.c", "modern/host/coro.c", "modern/host/coro_switch.S",
      "modern/host/vram.c"]),
    ("runtime-card-pxi-wireless", "Runtime: ROM card, ARM7 side (PXI) and wireless",
     ["modern/host/card.c", "modern/host/pxi7.c", "modern/host/wm7.c"]),
    ("renderer-2d", "The 2D engines",
     ["modern/host/gpu2d/*"]),
    ("renderer-3d", "The 3D geometry engine and rasteriser",
     ["modern/host/gpu3d/*"]),
    ("render-driver", "Render driver, self-tests and the melonDS reference",
     ["modern/host/render.c", "modern/host/render.h", "modern/host/render_selftest.c",
      "modern/host/render_selftest3d.c", "modern/host/melon.cpp", "modern/host/melon_none.c"]),
    ("sound", "Sound (the ARM7's sound driver)",
     ["modern/host/snd7/*"]),
    ("frontend-and-platform", "Frontends and platform layers",
     ["modern/host/frontend.c", "modern/host/frontend.h", "modern/host/frontend_viewer.c",
      "modern/host/plat.h", "modern/host/plat_posix.c", "modern/host/plat_win32.c"]),
    ("game-sdk-libraries", "The game's SDK libraries (autoload_2, itcm): card, sound streams, VRAM, input",
     ["src/autoload_2/*", "src/itcm/*"]),
    ("game-main", "The game's main program (src/main)",
     ["src/main/*"]),
    ("game-overlays", "The game's overlays (src/ov*)",
     ["src/ov*"]),
]


def git(fork: Path, *args: str, binary: bool = False) -> str | bytes:
    result = subprocess.run(["git", "--literal-pathspecs", "-C", str(fork), "-c", "core.quotepath=off", *args],
                            capture_output=True, check=False)
    if result.returncode != 0:
        sys.exit(f"git {' '.join(args)} failed: {result.stderr.decode(errors='replace').strip()}")
    return result.stdout if binary else result.stdout.decode()


def matches(path: str, patterns: list[str]) -> bool:
    return any(fnmatch.fnmatchcase(path, p) for p in patterns)


def area_of(path: str) -> str:
    """The one area a changed upstream file belongs to; exits if it is none or several."""
    found = [name for name, _title, patterns in AREAS if matches(path, patterns)]
    if len(found) != 1:
        sys.exit(f"{path}: matches {len(found)} areas ({', '.join(found) or 'none'}); "
                 "fix the table AREAS in tools/export_from_fork.py")
    return found[0]


def changed_files(fork: Path, base: str, branch: str) -> list[tuple[str, str, str]]:
    """(status, mode in the branch, path) for every file that differs; no rename detection."""
    out = git(fork, "diff", "--raw", "--no-renames", "-z", "--no-abbrev", base, branch)
    fields = out.split("\0")
    entries = []
    i = 0
    while i + 1 < len(fields):
        meta = fields[i].lstrip(":").split()  # old mode, new mode, old sha, new sha, status
        path = fields[i + 1]
        entries.append((meta[4][0], meta[1], meta[0], path))
        i += 2
    return [(status, new_mode, path) if status != "M" or new_mode == old_mode else ("T", new_mode, path)
            for status, new_mode, old_mode, path in entries]


def plan(files: list[tuple[str, str, str]]) -> tuple[list[str], dict[str, list[str]]]:
    """Split the changed files into the added ones (3ds_port, docs) and the patch contents."""
    added: list[str] = []
    patches: dict[str, list[str]] = {name: [] for name, _t, _p in AREAS}
    for status, mode, path in files:
        if matches(path, FORBIDDEN):
            sys.exit(f"{path}: looks like game content or a build product; refusing to export it")
        if matches(path, SKIP):
            continue
        if mode == "120000":
            sys.exit(f"{path}: is a symlink; symlinks are not exported")
        if status == "T":
            sys.exit(f"{path}: changes its file type or mode; handle it by hand")
        if status == "A":
            added.append(path)
        elif status in ("M", "D"):
            patches[area_of(path)].append(path)
        else:
            sys.exit(f"{path}: unexpected change status {status}")
    return added, patches


def write_patch(fork: Path, base: str, branch: str, number: int, name: str, title: str, paths: list[str]) -> Path:
    body = git(fork, "diff", "--no-color", "--no-ext-diff", "--no-renames", base, branch, "--", *sorted(paths))
    header = (f"From: acww-3ds export <noreply@invalid>\n"
              f"Subject: [PATCH] {title}\n\n"
              f"Changes {len(paths)} files of the decompilation for the 3DS port (area: {name}).\n"
              f"Made by tools/export_from_fork.py; do not edit by hand.\n\n")
    path = PATCH_DIR / f"{number:04d}-{name}.patch"
    path.write_text(header + body, encoding="utf-8")
    return path


def copy_added(fork: Path, branch: str, paths: list[str]) -> None:
    """Extract the added files from the branch's tree, keeping file modes."""
    data = git(fork, "archive", "--format=tar", branch, "--", *paths, binary=True)
    with tarfile.open(fileobj=io.BytesIO(data), mode="r:") as tar:
        for member in tar:
            if not member.isfile():
                continue
            dest = PORT_DIR / member.name
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_bytes(tar.extractfile(member).read())
            dest.chmod(0o755 if member.mode & 0o100 else 0o644)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--fork", required=True, type=Path, help="the fork's git repository (only read)")
    parser.add_argument("--base", required=True, help="upstream's state: a ref of the fork")
    parser.add_argument("--branch", required=True, help="the port's state: a ref of the fork")
    parser.add_argument("--out", type=Path, default=ROOT, help="the repository to write (default: this one)")
    args = parser.parse_args()

    global PATCH_DIR, PORT_DIR
    PATCH_DIR = args.out / "patches" / "acww-decomp"
    PORT_DIR = args.out / "3ds_port"

    for ref in (args.base, args.branch):
        git(args.fork, "rev-parse", "--verify", "--quiet", ref + "^{commit}")
    added, patches = plan(changed_files(args.fork, args.base, args.branch))

    # Everything checked: only now is anything written.
    shutil.rmtree(PATCH_DIR, ignore_errors=True)
    shutil.rmtree(PORT_DIR, ignore_errors=True)
    PATCH_DIR.mkdir(parents=True)
    copy_added(args.fork, args.branch, added)

    number = 0
    for name, title, _patterns in AREAS:
        if not patches[name]:
            continue
        number += 1
        path = write_patch(args.fork, args.base, args.branch, number, name, title, patches[name])
        print(f"{path.relative_to(args.out)}: {len(patches[name])} files, {len(path.read_text().splitlines())} lines")
    print(f"3ds_port/: {len(added)} files")


if __name__ == "__main__":
    main()

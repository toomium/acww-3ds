#!/usr/bin/env python3
"""Start the PC program on a made-up ROM that holds no game data, and say how far it gets.

Usage:
    python tools/boot_smoke.py /path/to/build/upstream/build/host64_usa/acww

The ROM is made in a temp folder: zeros, but for a file table with one file, a sound archive with no sounds
(the game's start-up opens that one first and crashes without it). The game's start-up then runs for real:
the runtime's hardware, the SDK's start-up, the heaps, the file system, the overlay loader, the graphics and
sound set-up. It stops at the first file it needs that the ROM doesn't have, with the game's own panic
("File can't open"), exit code 2. That is a pass, and the log says which file. Reaching the frame limit
is a pass too. A crash (signal, or the runtime's crash handler: exit code 5), a hang, or any other stop is
a failure.
"""

from __future__ import annotations

import os
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

TIMEOUT = 60
SOUND = b"sound_dataENG.sdat"   # the file Snd_Init opens (src/main/unk_020039ec.cpp)


def make_rom() -> bytes:
    """A 64 KB ROM: the header's file name table (FNT) and file allocation table (FAT) pointers, the tables,
    and the one file, a sound archive (SDAT) with empty blocks. No game data."""
    rom = bytearray(0x10000)
    fnt, fat, sdat = 0x1000, 0x2000, 0x3000
    names = bytes([len(SOUND)]) + SOUND + b"\0"
    struct.pack_into("<IHH", rom, fnt, 8, 0, 1)                    # the root directory: names at +8, first file 0
    rom[fnt + 8:fnt + 8 + len(names)] = names
    struct.pack_into("<IIII", rom, 0x40, fnt, 8 + len(names), fat, 8)   # the header: FNT, its size, FAT, its size
    info = b"INFO" + struct.pack("<I", 0x28) + bytes(0x20)         # eight tables, all absent
    fat_block = b"FAT " + struct.pack("<II", 0xC, 0)               # no entries
    head = bytearray(0x40)
    head[:4] = b"SDAT"
    struct.pack_into("<IIHH", head, 4, 0x0100FEFF, 0x40 + len(info) + len(fat_block), 0x40, 3)
    struct.pack_into("<IIII", head, 0x10, 0, 0, 0x40, len(info))   # symbols (none), info
    struct.pack_into("<II", head, 0x20, 0x40 + len(info), len(fat_block))
    struct.pack_into("<II", head, 0x28, 0x40 + len(info) + len(fat_block), 0)
    data = bytes(head) + info + fat_block
    rom[sdat:sdat + len(data)] = data
    struct.pack_into("<II", rom, fat, sdat, sdat + len(data))      # file 0
    return bytes(rom)


def classify(returncode: int | None, log: str) -> tuple[bool, str]:
    """(passed, what happened) from the program's exit status (None: still running at the time limit) and log."""
    lines = [line for line in log.splitlines() if line.strip()]
    last = lines[-1] if lines else "(no output)"
    if returncode is None:
        return False, f"no end after {TIMEOUT} s: {last}"
    if returncode < 0:
        return False, f"killed by signal {-returncode}: {last}"
    if "card: the ROM image" not in log:
        return False, f"the ROM wasn't read (exit {returncode}): {last}"
    if returncode == 0 and "(ACWW_EXIT_FRAME)" in log:
        return True, "ran to the frame limit"
    panics = [l for l in lines if "game panic: File can't open" in l]
    if returncode == 2 and panics:
        return True, "stopped at the first file it needs: " + panics[-1].split("game panic: ", 1)[1]
    crash = [l for l in lines if "signal" in l]
    return False, f"unexpected stop (exit {returncode}): {(crash or [last])[0]}"


def run(program: str) -> tuple[bool, str]:
    with tempfile.TemporaryDirectory() as tmp:
        rom = Path(tmp) / "made_up.nds"
        rom.write_bytes(make_rom())
        env = dict(os.environ, ACWW_HEADLESS="1", ACWW_FAST="1", ACWW_EXIT_FRAME="60")
        try:
            r = subprocess.run([program, str(rom)], env=env, capture_output=True, text=True,
                               errors="replace", timeout=TIMEOUT)
        except subprocess.TimeoutExpired as e:
            out = e.stderr or b""
            return classify(None, out.decode(errors="replace") if isinstance(out, bytes) else out)
        return classify(r.returncode, r.stdout + r.stderr)


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__)
        return 2
    ok, what = run(sys.argv[1])
    print(("PASS: " if ok else "FAIL: ") + what)
    if os.environ.get("GITHUB_ACTIONS"):
        print(f"::{'notice' if ok else 'error'} title=Boot smoke test::{what}")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())

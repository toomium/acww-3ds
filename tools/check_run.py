#!/usr/bin/env python3
"""Local regression check: did a change alter what the game does on the PC build?

    python3 tools/check_run.py ROM.nds              # run, compare with build/check/baseline.txt; exit 1 if different
    python3 tools/check_run.py --baseline ROM.nds   # run, store the result as the baseline

One headless, deterministic run of the PC build (the decompilation's `save` play script: a new game to the first save,
fresh save file) writes screenshots, the sound, the save file to build/check/run/; their SHA-256
hashes go to build/check/run.txt. Covers the PC build only (game code and shared runtime), not the 3DS-only code.
The ROM is only read; nothing of the game is stored in the repository (build/ is ignored).
"""

import hashlib
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
UP = ROOT / "build" / "upstream"
BINARY = UP / "build" / "host64_usa" / "acww"
CHECK = ROOT / "build" / "check"
BASELINE = CHECK / "baseline.txt"
SHOTS = (2500, 8000, 21000, 25000, 33000, 33999)  # the intro, the town, Nook's talk, the save prompt
EXIT_FRAME = 34000  # the `save` script ends at about frame 33500
# (no RAM hash: ACWW_RAMHASH holds host pointers, so it differs from run to run with ASLR and from build to build)
TIMEOUT = 1800  # seconds of real time


def read_hashes(path):
    return dict(line.split(None, 1)[::-1] for line in Path(path).read_text().splitlines() if line.strip())


def compare(base, new):
    """Lines describing every item that differs (empty: identical). Both are {item: hash}."""
    out = [f"DIFFERS: {k}" for k in sorted(base.keys() & new.keys()) if base[k] != new[k]]
    out += [f"MISSING in the run: {k}" for k in sorted(base.keys() - new.keys())]
    out += [f"NOT in the baseline: {k}" for k in sorted(new.keys() - base.keys())]
    return out


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def run(rom, work):
    shutil.rmtree(work, ignore_errors=True)
    work.mkdir(parents=True)
    exports = subprocess.run([sys.executable, UP / "tools" / "host_play_script.py", "save"], check=True,
                             capture_output=True, text=True).stdout
    env = dict(os.environ, ACWW_HEADLESS="1", ACWW_FAST="1", ACWW_RENDERER3D="own", ACWW_EXIT_FRAME=str(EXIT_FRAME),
               ACWW_SHOT=";".join(f"{f}:{work}/shot-{f}.ppm" for f in SHOTS), ACWW_WAV=str(work / "sound.wav"),
               ACWW_SAVE=str(work / "game.sav"))
    env.update(re.findall(r"^export (\w+)='(.*)'$", exports, re.M))
    with open(work / "log.txt", "wb") as log:
        r = subprocess.run([BINARY, rom], env=env, stdin=subprocess.DEVNULL, stdout=log, stderr=log, timeout=TIMEOUT)
    if r.returncode:
        sys.exit(f"error: the run failed (exit {r.returncode}); see {work / 'log.txt'}")
    names = [f"shot-{f}.ppm" for f in SHOTS] + ["sound.wav", "game.sav"]
    missing = [n for n in names if not (work / n).is_file()]
    if missing:
        sys.exit(f"error: the run did not write {', '.join(missing)}; see {work / 'log.txt'}")
    return {n: sha(work / n) for n in names}


def main(argv):
    args = argv[1:]
    baseline = "--baseline" in args
    args = [a for a in args if a != "--baseline"]
    if len(args) != 1 or not args[0]:
        sys.exit("error: no ROM given: make check ROM=path/to/rom.nds")
    if not Path(args[0]).is_file():
        sys.exit(f"error: ROM file not found: {args[0]}")
    if not BINARY.is_file():
        sys.exit(f"error: PC build missing ({BINARY}): run make pc")
    hashes = run(args[0], CHECK / "run")
    text = "".join(f"{h}  {n}\n" for n, h in hashes.items())
    (CHECK / "run.txt").write_text(text)
    if baseline:
        shutil.copyfile(CHECK / "run.txt", BASELINE)
        print(f"Baseline stored: {BASELINE}")
        return 0
    if not BASELINE.is_file():
        print("No baseline yet (build/check/baseline.txt). Make one, on main before your change:\n"
              "  make check-baseline ROM=path/to/rom.nds")
        return 1
    diff = compare(read_hashes(BASELINE), hashes)
    if diff:
        print("\n".join(diff))
        print(f"{len(diff)} difference(s) from the baseline (the run's files: {CHECK / 'run'})")
        return 1
    print("No differences from the baseline.")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))

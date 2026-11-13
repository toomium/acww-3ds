# Contributing

## The one rule that can't be broken

**Never commit game content.** No ROMs, no files extracted from a ROM, no save files, no `dspfirm.cdc`, and no
built binaries. The `.gitignore` blocks the common file types, but check the list of changed files before every
commit. If a `.nds`, `.sav` or `.cdc` shows up there, stop.

Builds contain Nintendo's code too: the `.3dsx` and the PC binary are never committed or attached to an issue. The
only build that is published is a release's `acww.3dsx`, made by CI from a version tag.

## How changes get in

`main` always holds a version that has been tested in Azahar.

1. **Branch.** Every change goes on its own branch, named by what it does:
   `port/gpu-lists`, `perf/old-3ds-2d`, `fix/pocket-menu`, `docs/readme`, `tools/bootstrap`, `ci/build`.
2. **Pull request.** Open a pull request into `main` describing what changed and how to test it.
3. **CI.** GitHub Actions runs the tools' tests and builds the 3DS and PC programs on every push. A pull request
   with a red build doesn't get merged. CI uploads a build only as a release's file (a version tag).
4. **Test.** Check out the branch, build it with `make game` and try it in Azahar. Changes that affect performance
   also get measured on a real Old 3DS and, where it matters, a New 3DS: Azahar shows whether the picture is the
   same, not how fast the console is, and it does not show a GPU hang (a changed vertex shader is tried on the
   console before anything is built on it).
5. **Merge.** The repository owner merges after testing.

## Changes to the port's own files

New files that upstream does not have go in `3ds_port/`, at the path they have in the source tree
(`3ds_port/modern/host/pica_display.c`). `bootstrap.py` links them into `build/upstream`; after adding one, run
`python tools/bootstrap.py` again (no `--clean`) to make its link. Edit them in `3ds_port/`: the links in
`build/upstream` point there.

## Changes to upstream's files

The decompilation is never committed here. `tools/bootstrap.py` downloads it into `build/upstream` and applies
`patches/acww-decomp/*.patch`. To change one of its files:

```sh
python tools/bootstrap.py --clean               # a fresh copy with the existing patches
cd build/upstream
git add -A && git commit -m "Existing patches"  # so your change stands alone (set user.name and user.email if git asks)
# ... edit, build with make from the top directory, then commit ...
git format-patch -1 -o ../../patches/acww-decomp/
```

Rename the new file to the next number (`0015-<area>.patch`) and commit it in this repository. Keep each patch to one
change. Wrap changes to the game's own sources in `#ifdef N3DS_PORT` so the other builds stay as they were.

## Bringing work across from the fork

While the port is still developed on a branch of the fork, `tools/export_from_fork.py` rebuilds `patches/` and
`3ds_port/` from it:

```sh
python tools/export_from_fork.py --fork /path/to/acww-decomp --base upstream/main --branch <branch>
```

It sorts every changed upstream file into one patch by the table `AREAS` in the script, and stops with an error if
a file matches no area or two areas. Add its pattern to the table, then run it again. It replaces `patches/` and
`3ds_port/` completely, and only adds to `docs/images/`. To move to a newer upstream, change the commit in
`upstream.lock` and make a fresh export from a branch that is based on it.

## Checks

```sh
make test      # the tools' unit tests, the PC build, a start-up smoke test of it
```

The smoke test (`tools/boot_smoke.py`) starts the PC program on a made-up ROM with no game data; the game's start-up
runs for real up to the first file it needs, where it must stop with its own "File can't open" message. It proves
that the build starts and its start-up doesn't crash; it does not prove anything about the game itself.

```sh
make check-baseline ROM=path/to/rom.nds   # on main, before your change
make check ROM=path/to/rom.nds            # after it: lists what differs, exit 1 if anything
```

`make check` plays the PC build headless and deterministic from a new game to the first save and compares SHA-256
hashes of screenshots, sound and save file with `build/check/baseline.txt` (one run, a few minutes). It covers the
game's code and the shared runtime with our patches on the PC, not the 3DS-only code (`N3DS_PORT`): that still needs
Azahar or a console. It needs a ROM, so it is not part of `make test` or CI.

The game's own checks (screenshots, sound and save file against a baseline) need a ROM and are described in
`docs/host.md` of the decompilation.

## Code style

- C: match the surrounding code. Comments say what and why in plain sentences.
- Python: 3.10+, standard library only, with unittest tests in `tools/test_*.py`. Tools must never modify or upload a
  ROM.

## Commit messages

A short summary line that names the area and what changed ("Performance: the GPU lists' runs out of view left
out", "README: how to build", "CI: retry the fetch"), a blank line, then what and why if it isn't obvious, and what
was measured. Commits written with AI help end with a `Co-Authored-By: Claude` line (AI_DISCLOSURE.md).

# Notice

## What this project licenses

Only the original code, tools and documentation written for this project are
covered by this project's license, the GNU General Public License v3.0 (see
[`LICENSE`](LICENSE)): the files in `3ds_port/`, `tools/` and `web/`, the
documentation, and the changes the patches in `patches/` make.

## What it does not

- **The game.** Animal Crossing: Wild World and all of its content belong to
  Nintendo. None of it is included here. Players supply their own legally
  dumped cartridge.
- **The decompilation.** 7ucina/acww-decomp is a separate project with its own
  terms (MIT). It is downloaded at build time, not redistributed here.
- **Third-party libraries and tools.** devkitARM, libctru and citro3d keep
  their own licenses, as do makerom and bannertool, which `make cia` fetches
  and builds.

## Adapted from other projects

`cia/acww.rsf` and `tools/build_cia_tools.sh` are adapted from
[GabeSud0/pokeplatinum-3ds](https://github.com/GabeSud0/pokeplatinum-3ds)
(GPL-3.0), whose forwarder is also the model for `cia/forwarder.c`.

## Rules for contributors

- Never commit ROMs, extracted assets, save files or built game binaries.
- Never add code or assets copied from leaked source code, or from ports that
  bundle the game's assets.
- Keep changes to the decompilation as patch files in `patches/`.

## License

The original code in this repository is licensed under the GNU General Public
License v3.0. Anyone may use, modify and share it, but distributed modified
versions must also be released as source code under GPL-3.0. By contributing,
you agree that your contribution is licensed the same way. A change to the
decompilation's own files that is useful to it as well may also be offered to
that project under its MIT terms by its author.

## Releases and the web builder

The releases carry the 3DS program, built from this project's code and the
decompilation: never a ROM or anything taken from one. The web builder
(`web/`) puts that program and the player's own ROM dump together for the SD
card in the player's browser; the ROM is never uploaded.

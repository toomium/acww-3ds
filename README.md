<h1 align="center">Animal Crossing: Wild World 3DS</h1>

<p align="center">
  <strong>Your town, across both screens.</strong><br>
  A native Nintendo 3DS port of Animal Crossing: Wild World.
</p>

<p align="center">
  <a href="https://github.com/toomium/acww-3ds/actions/workflows/build.yml"><img src="https://img.shields.io/github/actions/workflow/status/toomium/acww-3ds/build.yml?branch=main&amp;style=for-the-badge&amp;label=Build&amp;logo=github" alt="Build status"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-GPL--3.0-4B5563?style=for-the-badge" alt="GPL-3.0 licence"></a>
</p>

<p align="center">
  <a href="#features">Features</a> &nbsp;·&nbsp;
  <a href="#status">Status</a> &nbsp;·&nbsp;
  <a href="#getting-started">Getting started</a> &nbsp;·&nbsp;
  <a href="#for-developers">For developers</a> &nbsp;·&nbsp;
  <a href="#credits-and-legal">Credits and legal</a>
</p>

---

Animal Crossing: Wild World 3DS is built from the
[7ucina/acww-decomp](https://github.com/7ucina/acww-decomp) decompilation. It
isn't an emulator and it isn't a remake: **the game's own code runs on the
3DS**, with the DS's hardware rebuilt on the 3DS's own: the 3D on its GPU, the
sound on its DSP, the save and the ROM on the SD card.

## Features

| | What to expect |
| :--- | :--- |
| **Wide screens** | The town is drawn across the 3DS's full 400×240 upper and 320×240 lower screens instead of the DS's 256×192, and the game's interface moves to the screens' edges. |
| **60 frames a second as the target** | The game draws a 3D picture every third frame; the port draws the pictures in between. 60 isn't reached everywhere yet. |
| **Analog Circle Pad** | The player walks in any direction, not only the D-pad's eight, and as fast as the Circle Pad is tilted. |
| **An options page** | A new tab in the pause menu, left of the pockets: L from the pockets, or touch its button. Its settings are kept in `acww.cfg`. |
| **Stacked fruit** | Optional, off by default: up to 9 of a fruit in one pocket slot. |
| **Other languages** | English, French, German, Italian or Spanish text, taken from your own copy of the European version. Only German has been tested so far. |
| **Built from your own cartridge** | The game's data comes from your own dump. No game content is distributed. |

## Status

| ✅ Works | 🛠️ Work in progress | 💭 Future wishlist |
| :--- | :--- | :--- |
| Playable on a New 3DS and in Azahar: the town, the houses, the menus, saving | The Old 3DS: it runs, slower than the game should (see below) | Stereoscopic 3D |
| Wide screens, the options page, the Circle Pad | 60 frames a second everywhere; a HOME Menu version (`.cia`: the web builder puts the icon from your ROM on it; the banner is a placeholder) | |
| Sound, the save file, the touch screen | Languages other than German tested | Wireless play |

On an **Old 3DS** the game currently runs at about three fifths of its speed in
town. `ACWW_FRAMESKIP=auto` in `acww.cfg` lets it leave pictures out to keep the
game's pace; making it faster is what is being worked on.

> [!IMPORTANT]
> **This repository contains no game content.** No ROM, graphics, music,
> text or any other data from the game is stored here. The releases carry the
> port's program; the game's data is **your own cartridge's**, on your computer
> or in your browser, and never leaves it. Don't share the files you make: they
> hold your cartridge's data.

## Getting started

### What you need

- A **3DS family console** with custom firmware (Luma3DS) and the Homebrew
  Launcher. Setup guide: <https://3ds.hacks.guide>
- Your own dump of **Animal Crossing: Wild World (USA, Rev 1)**: game code
  `ADME`, SHA-1 `77fde3e30e1e6068395d1f96ea63be569b61c351`.
- For sound, the 3DS's DSP firmware at `sd:/3ds/dspfirm.cdc`, dumped from your
  own console with the DSP1 homebrew. Without it the game runs silently.
- Only to build it yourself: [devkitPro](https://devkitpro.org/wiki/Getting_Started)
  with the `3ds-dev` package group, Python 3.10+, Git, ninja and make.
- Optional: the [Azahar](https://azahar-emu.org) emulator for testing on a PC.
  It doesn't show a console's speed: judge performance on the console.

### 1. Make the game's files

**The easy way: the web builder.** Choose your ROM, press
**Build**, and download a ZIP laid out for the SD card. It all happens in your
browser: your ROM is never uploaded. (The page is this repository's `web/`
folder, published with each release.)

**Or build it yourself**, from the source:

```sh
python tools/bootstrap.py     # once: fetch the decompilation, apply the patches
make game                     # -> build/upstream/build/host3ds_usa/acww.3dsx
```

### 2. Copy to the SD card

The web builder's ZIP has these in its `3ds` folder: copy it to the root of the
SD card.

```
sdmc:/3ds/acww/acww.3dsx      (the program, for the Homebrew Launcher)
sdmc:/3ds/acww/game.pak       (your ROM's files without the game's code: the web builder's, or `make pak ROM=...`)
sdmc:/3ds/acww/acww.cfg       (optional: the settings)

Without a `game.pak` the port reads the ROM itself, as `game.nds`.
```

In Azahar, these go in its emulated SD card folder.

### 3. Play

Run `acww.3dsx` from the Homebrew Launcher, or open it in Azahar. The game
saves to `sdmc:/3ds/acww/game.sav`.

### While playing

Every button is the game's, and the touch screen is the DS's. The Circle Pad
walks at any angle and speed. The port writes a log to
`sdmc:/3ds/acww/log.txt`: send it along when you report a problem (its first line names the port's version).

The options page writes `sdmc:/3ds/acww/acww.cfg`, a file of `NAME=value`
lines that can also be edited by hand:

| Option | Default | Meaning |
| :--- | :--- | :--- |
| `ACWW_PICA_INTERP` | `1` | 60 frames a second: the pictures between the game's |
| `ACWW_WIDE` | `1` | the wide screens (after a restart) |
| `ACWW_STACK_FRUIT` | `0` | stacked fruit |
| `ACWW_ANALOG` | `1` | the Circle Pad's angle and tilt (`0`: the D-pad's eight directions) |
| `ACWW_FRAMESKIP` | `0` | `auto`: pictures are left out when the console can't keep the game's pace |
| `ACWW_LANG` | | `ENG`, `FRN`, `GER`, `ITA` or `SPN`: that language's text... |
| `ACWW_LANG_ROM` | | ...from the European ROM (`ADMP`) at this path, e.g. `sdmc:/3ds/acww/eur.nds` |

Stacked fruit uses item codes the DS game doesn't have: a save with stacks in
it belongs to this port.

> [!TIP]
> **Need help?** Open an issue, and include your console or emulator and
> `log.txt`.

## For developers

<details>
<summary><strong>How it works</strong></summary>

<br>

```
your ROM ──► the web builder, or `make pak` ──► game.pak on the SD card

7ucina/acww-decomp @ pinned commit ──► + patches/ ──► + 3ds_port/ ──► acww.3dsx
```

- The game's decompiled C and C++ is compiled with GCC for the 3DS's CPU. The
  decompilation is pinned in [`upstream.lock`](upstream.lock); the port's
  changes to it live as patch files.
- The decompilation brings a runtime that rebuilds the DS's hardware in
  software (it is also a PC port). The 3DS port adds its own parts beside it
  ([`3ds_port/`](3ds_port)): the screens put together on the 3DS's GPU, the
  DS's 3D commands handed to the GPU, sound through the DSP, the 3DS's buttons,
  touch screen and SD card.
- The port (`make game`) replaces the DS hardware model piece by piece with
  code made for the 3DS, and adds what a DS can't do.

</details>

<details>
<summary><strong>Building and testing</strong></summary>

<br>

```sh
python tools/bootstrap.py   # once: fetch the decompilation, apply patches/, link 3ds_port/ in
make test                   # the tools' and the web builder's tests, and the PC build
make game                   # the port for the 3DS
make cia                    # the HOME Menu version: build/cia/acww.cia, a forwarder to the .3dsx (no game code)
make pak ROM=game.nds       # the data pack from your ROM: build/game.pak (docs/pak-format.md)
make cia ROM=game.nds       # that with the game's icon from your ROM (for your console, not to pass on)
make pc                     # the same game code as a PC program
make check ROM=game.nds     # local regression test on the PC build (needs a ROM; make check-baseline first)
```

`make game NINJAFLAGS=-j16` passes options to ninja;
`python tools/bootstrap.py --clean` starts `build/upstream` over. The
runtime, its `ACWW_*` options and how changes are checked are described in
`build/upstream/docs/host.md`.

</details>

<details>
<summary><strong>What this repository holds</strong></summary>

<br>

- the port's own code (`3ds_port/`),
- patches applied on top of the decompilation (`patches/`),
- tools that fetch the decompilation and build the port (`tools/`),
- the web builder (`web/`).

CI builds and tests everything on every push and uploads a build only as a
release's file. Never commit a ROM, an extracted asset, a save or a build of
the game. The `.gitignore` blocks the common cases, but check `git status`
before you commit.

```
3ds_port/          the files that are the port's alone, at their paths in the source tree
patches/           changes applied on top of the decompilation, numbered by area
tools/             bootstrap, the export from the fork, the site's assembly, tests
web/               the web builder's website (GitHub Pages; tools/build_site.py)
docs/releases/     each version's notes
Makefile           `make game`, `make pc`, `make test`, `make check`, `make clean`
LICENSE            GPL-3.0, for this project's own code
upstream.lock      the exact 7ucina/acww-decomp commit we build against
build/             everything fetched and built; never committed
```

</details>

### Roadmap

- **Done:** the game's code built and running on the 3DS; 2D, 3D, sound,
  input, saving; the wide screens; the options page; the Circle Pad; stacked
  fruit; languages from the European version.
- **Work in progress:** the Old 3DS's speed, 60 frames a second everywhere.
- **Future wishlist:** stereoscopic 3D, wireless play.

### Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for how changes are made and tested,
and [`AI_DISCLOSURE.md`](AI_DISCLOSURE.md) for how AI assistance is used. Found
a bug? Open an issue with your `log.txt`.

## Credits and legal

The original code in this repository is licensed under the
[GNU General Public License v3.0](LICENSE). That covers only this project's own
code, not the game or the decompilation. See [`NOTICE.md`](NOTICE.md).

Built on [acww-decomp](https://github.com/7ucina/acww-decomp) by Lucina and its
contributors: the decompilation and the PC port this 3DS port started from. The
repository's layout and way of working follow
[pokeplatinum-3ds](https://github.com/GabeSud0/pokeplatinum-3ds). The 3DS side
uses devkitPro's libctru and citro3d.

This is an unofficial fan project, not affiliated with or endorsed by Nintendo.
Animal Crossing is a trademark of Nintendo.

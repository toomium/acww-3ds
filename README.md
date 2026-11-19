<h1 align="center">acww-port</h1>

<p align="center">
  <strong>Your town, on the 3DS and the Switch.</strong><br>
  Native ports of Animal Crossing: Wild World for the Nintendo 3DS and the Nintendo Switch.
</p>

<p align="center">
  <a href="https://github.com/toomium/acww-port/actions/workflows/build.yml"><img src="https://img.shields.io/github/actions/workflow/status/toomium/acww-port/build.yml?branch=main&amp;style=for-the-badge&amp;label=Build&amp;logo=github" alt="Build status"></a>
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

acww-port is built from the
[7ucina/acww-decomp](https://github.com/7ucina/acww-decomp) decompilation. It
isn't an emulator and it isn't a remake: **the game's own code runs on the
console**, with the DS's hardware rebuilt on the console's own: the 3D on its
GPU, the sound on its audio hardware, the save and the ROM on the SD card. The
3DS and the Switch share most of the port; each has only its own display,
input and system parts.

## Features

| | What to expect |
| :--- | :--- |
| **Wide screens** | The town is drawn across the 3DS's full 400×240 upper and 320×240 lower screens, or 16:9 on the Switch, instead of the DS's 256×192, and the game's interface moves to the screens' edges. |
| **One screen on the Switch** | The town fills the display in 720p; the touch screen appears as a panel when it has something to show. Six other layouts (side by side, stacked, ...) are a press of the left stick away. |
| **60 frames a second** | The game draws a 3D picture every third frame; the port draws the pictures in between. The Switch holds 60; on the 3DS it isn't reached everywhere yet. |
| **Analog stick** | The player walks in any direction, not only the D-pad's eight, and as fast as the Circle Pad or left stick is tilted. |
| **An options page** | A new tab in the pause menu, left of the pockets: L from the pockets, or touch its button. Its settings are kept in `acww.cfg`. |
| **Main screen on top** | 3DS: normally you play on the bottom screen, with this option you put your town on the top screen. |
| **Stacked fruit** | Optional, off by default: up to 9 of a fruit in one pocket slot. |
| **New Leaf controls** | Optional, off by default: Y picks up items and pulls weeds, B only runs, X opens the pockets. |
| **Other languages** | English, French, German, Italian or Spanish text, taken from your own copy of the European version. Only German has been tested so far. |
| **Built from your own cartridge** | The game's data comes from your own dump. No game content is distributed. |

## Status

| ✅ Works | 🛠️ Work in progress | 💭 Future wishlist |
| :--- | :--- | :--- |
| Playable on a New 3DS and in Azahar: the town, the houses, the menus, saving | The Old 3DS: it runs, slower than the game should (see below) | Stereoscopic 3D |
| Playable on a Switch (Atmosphère) at 60 frames a second, docked or handheld; in the web builder and the releases | [Visiting another town](#visiting-another-town), 3DS and Switch together: works in Azahar and Eden; on the consoles being tested | Nintendo Wi-Fi Connection |
| Wide screens, the options page, the analog stick | 60 frames a second everywhere on the 3DS; a HOME Menu version (`.cia`: the web builder puts the icon from your ROM on it; the banner is a placeholder) | |
| Sound, the save file, the touch screen | Languages other than German tested | |

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
- Or a **Switch** with custom firmware (Atmosphère) and the Homebrew Menu.
  Setup guide: <https://switch.hacks.guide>
- Your own dump of **Animal Crossing: Wild World (USA, Rev 1)**: game code
  `ADME`, SHA-1 `77fde3e30e1e6068395d1f96ea63be569b61c351`.
- 3DS, for sound: the 3DS's DSP firmware at `sd:/3ds/dspfirm.cdc`, dumped from
  your own console with the DSP1 homebrew. Without it the game runs silently.
- Only to build it yourself: [devkitPro](https://devkitpro.org/wiki/Getting_Started)
  with the `3ds-dev` package group (3DS) or `switch-dev`, `switch-sdl2` and
  `switch-mesa` (Switch), Python 3.10+, Git, ninja and make.
- Optional: the [Azahar](https://azahar-emu.org) emulator for testing on a PC.
  It doesn't show a console's speed: judge performance on the console.

### 1. Make the game's files

**The easy way: the web builder.** Go to [Here](https://toomium.github.io/acww-port/), choose your ROM and your
console (3DS or Switch), press **Build**, and download a ZIP laid out for the SD card. It all happens in your
browser: your ROM is never uploaded.

**Or build it yourself**, from the source:

```sh
python tools/bootstrap.py     # once: fetch the decompilation, apply the patches
make game                     # 3DS:    build/upstream/build/host3ds_usa/acww.3dsx
make switch                   # Switch: build/upstream/build/hostswitch_usa/acww.nro
make pak ROM=game.nds         # the game's data from your ROM: build/game.pak
```

### 2. Copy to the SD card

The web builder's ZIP has these in its `3ds` folder (or `switch` for the
Switch): copy it to the root of the SD card.

```
sdmc:/3ds/acww/acww.3dsx      (the program, for the Homebrew Launcher)
sdmc:/3ds/acww/game.pak       (your ROM's files without the game's code: the web builder's, or `make pak ROM=...`)
sdmc:/3ds/acww/acww.cfg       (optional: the settings)

Without a `game.pak` the port reads the ROM itself, as `game.nds`.
```

For the Switch, the same files go in `sdmc:/switch/acww/`, with `acww.nro` as
the program.

In an emulator, these go in its emulated SD card folder.

### 3. Play

Run `acww.3dsx` from the Homebrew Launcher (3DS) or `acww.nro` from the
Homebrew Menu (Switch). The game saves to `game.sav` beside the program.

### While playing

Every button is the game's, and the touch screen is the DS's. The Circle Pad
or left stick walks at any angle and speed. On the Switch, a few buttons the DS
doesn't have do more:

| Switch | What it does |
| :--- | :--- |
| Press the left stick (L3) | the next screen layout (kept in `acww.cfg`) |
| Press the right stick (R3) | show or hide the touch screen in the one-screen layout |
| Right stick, ZR | move a pointer and touch with it: the touch screen when docked |
| The touch screen | handheld: touches where the game's touch screen is shown |

The port writes a log, `log.txt`, beside the program: send it along when you
report a problem (its first line names the port's version).

The options page writes `acww.cfg` beside the program, a file of `NAME=value`
lines that can also be edited by hand:

| Option | Default | Meaning |
| :--- | :--- | :--- |
| `ACWW_PICA_INTERP` | `1` | 60 frames a second: the pictures between the game's |
| `ACWW_WIDE` | `1` | the wide screens (after a restart) |
| `ACWW_LAYOUT` | `0` | Switch: the screen layout, `0` one screen to `6` |
| `ACWW_SWITCH_SCALE` | `3` | Switch: the resolution, `1` to `4` times the 3DS's (`3`: 720p; after a restart) |
| `ACWW_MAIN_TOP` | `0` | 3DS: the town and the rooms always on the upper screen: the pause menu opens and closes on the lower screen alone, over the sky (the touch screen then shows the sky and takes no touches there) |
| `ACWW_STACK_FRUIT` | `0` | stacked fruit |
| `ACWW_NL_CONTROLS` | `0` | New Leaf controls: Y picks up items and pulls weeds, B only runs, X opens the pockets instead of the map |
| `ACWW_ANALOG` | `1` | the Circle Pad's angle and tilt (`0`: the D-pad's eight directions) |
| `ACWW_FRAMESKIP` | `0` | 3DS: `auto`: pictures are left out when the console can't keep the game's pace |
| `ACWW_LANG` | | `ENG`, `FRN`, `GER`, `ITA` or `SPN`: that language's text... |
| `ACWW_LANG_ROM` | | ...from the European ROM (`ADMP`) at this path, e.g. `sdmc:/3ds/acww/eur.nds` |

Stacked fruit uses item codes the DS game doesn't have: a save with stacks in
it belongs to this port.

### Visiting another town

The DS's local wireless play (the gate: inviting a friend or visiting their
town) goes over the console's own internet connection, and a 3DS and a Switch
can visit each other. The consoles must be connected to the same network in
System Settings and run the same version of the port. The radio's traffic is
UDP on ports 31470-31477; each console finds the others on the network by
itself. As on the DS, the gate opens only after Nook's part-time job.

| Option | Default | Meaning |
| :--- | :--- | :--- |
| `ACWW_NET_PEER` | | `host[:port]`: the host's address, if the host's town isn't found by itself (another network: the host's port, 31470 by default, forwarded to it) |
| `ACWW_NET_LAN` | `1` | `0`: no beacons on the local network, only to `ACWW_NET_PEER` |
| `ACWW_MAC` | the console's | `xx:xx:xx:xx:xx:xx`: how the game tells the players apart; made from the console's ID or serial number, so set it in one of two emulator windows that share one |
| `ACWW_NET_PORT` | | a fixed UDP port instead of the first free one of 31470-31477; two emulators on one computer: `31471` in one of them |
| `ACWW_WM_LOG` | `0` | `1`: the wireless traffic in `log.txt` once a second, `2`: every packet |

If the connection breaks, the game resets as on the DS, which on the consoles
ends the program (back to the Homebrew Launcher or the menu; the save is
written first). Nintendo Wi-Fi Connection (playing over the internet through
Nintendo's servers, which are gone) isn't supported.

> [!TIP]
> **Need help?** Open an issue, and include your console or emulator and
> `log.txt`.

## For developers

<details>
<summary><strong>How it works</strong></summary>

<br>

```
your ROM ──► the web builder, or `make pak` ──► game.pak on the SD card

7ucina/acww-decomp @ pinned commit ──► + patches/ ──► + ports/ ──► acww.3dsx / acww.nro
```

- The game's decompiled C and C++ is compiled with GCC for the 3DS's CPU. The
  decompilation is pinned in [`upstream.lock`](upstream.lock); the port's
  changes to it live as patch files.
- The decompilation brings a runtime that rebuilds the DS's hardware in
  software (it is also a PC port). The console ports add their own parts beside
  it ([`ports/`](ports)): the screens put together on the GPU and the DS's 3D
  commands handed to it, sound, buttons, touch screen and SD card.
- What the consoles share is in `ports/common/`: the compositing of the screens
  and the wide view (`comp2d.c`, drawn by citro3d on the 3DS and OpenGL ES on
  the Switch), the 3D's display lists and the pictures in between, the options
  page. Each console has only its backends: `ports/3ds/`, `ports/switch/`.
- The ports replace the DS hardware model piece by piece with code made for the
  console, and add what a DS can't do.

</details>

<details>
<summary><strong>Building and testing</strong></summary>

<br>

```sh
python tools/bootstrap.py   # once: fetch the decompilation, apply patches/, link ports/*/overlay/ in
make test                   # the tools' and the web builder's tests, and the PC build
make game                   # the port for the 3DS
make switch                 # the port for the Switch
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

- the port's own code (`ports/`),
- patches applied on top of the decompilation (`patches/`),
- tools that fetch the decompilation and build the port (`tools/`),
- the web builder (`web/`).

CI builds and tests everything on every push and uploads a build only as a
release's file. Never commit a ROM, an extracted asset, a save or a build of
the game. The `.gitignore` blocks the common cases, but check `git status`
before you commit.

```
ports/common/      the files the console ports share (overlay/: at their paths in the source tree)
ports/3ds/         the 3DS's own files (overlay/ as above; cia/: the HOME Menu forwarder)
ports/switch/      the Switch's own files (overlay/ as above)
patches/           changes applied on top of the decompilation, numbered by area
tools/             bootstrap, the data pack, the site's assembly, tests
web/               the web builder's website (GitHub Pages; tools/build_site.py)
docs/releases/     each version's notes
Makefile           `make game`, `make switch`, `make pc`, `make test`, `make check`, `make clean`
LICENSE            GPL-3.0, for this project's own code
upstream.lock      the exact 7ucina/acww-decomp commit we build against
build/             everything fetched and built; never committed
```

</details>

### Roadmap

- **Done:** the game's code built and running on the 3DS and the Switch; 2D,
  3D, sound, input, saving; the wide screens; the options page; the analog
  stick; stacked fruit; languages from the European version; the Switch's one
  screen at 720p and 60 frames a second.
- **Work in progress:** the Old 3DS's speed, 60 frames a second everywhere on
  the 3DS; visiting another town, 3DS and Switch together.
- **Future wishlist:** stereoscopic 3D, Nintendo Wi-Fi Connection.

### Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for how changes are made and tested,
and [`AI_DISCLOSURE.md`](AI_DISCLOSURE.md) for how AI assistance is used. Found
a bug? Open an issue with your `log.txt`.

## Credits and legal

The original code in this repository is licensed under the
[GNU General Public License v3.0](LICENSE). That covers only this project's own
code, not the game or the decompilation. See [`NOTICE.md`](NOTICE.md).

Built on [acww-decomp](https://github.com/7ucina/acww-decomp) by Lucina and its
contributors: the decompilation and the PC port these ports started from. The
repository's layout and way of working follow
[pokeplatinum-3ds](https://github.com/GabeSud0/pokeplatinum-3ds). The 3DS side
uses devkitPro's libctru and citro3d, the Switch side libnx, SDL2 and Mesa.

This is an unofficial fan project, not affiliated with or endorsed by Nintendo.
Animal Crossing is a trademark of Nintendo.

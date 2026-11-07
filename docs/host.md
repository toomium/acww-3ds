# Toward a native PC build (plan)

## The modern branch is PC-only (2026-10-07)

This branch no longer builds the matching DS ROM (nor the GCC-built DS ROM of docs/modern.md); the decomp's own
branches do. The code here is the PC port's, and it is rewritten freely toward plain, portable C/C++: real shared
types and member access instead of DS-offset views, padding arrays and per-site host branches. The `HOST` / `MODERN`
/ `NONMATCHING` conditionals were resolved (`tools/host_resolve_ifdefs.py`: their DS-only alternatives deleted);
`HOST64` (`__SIZEOF_POINTER__ == 8`) remains while the 32-bit build serves as the reference. The helpers left from
the dual build (`HOST_AT`, `HOST_AT_A`, `HOST_SIZED`: `include/uptr.h`) go away as their users are rewritten.

Checking changes without the matching ROM: the behaviour must not change. Both builds are deterministic and, since
docs/portability.md 3.4, give the same output (screenshots, sound, save files, register traffic frame for frame); the
x86-64 build is the reference. `tools/host_oracle.sh <binary> quick|play` compares a build's screenshots and sound
with the captures in `~/acww/ref/oracle/captures_v3` (not in the repository; recorded from the x86-64 build with the
own renderers, docs/portability.md 5.3, with `tools/host_oracle.sh build/host64_usa/acww record`, which writes only
that directory - `CAPDIR=` another; `captures_v2`, the same runs as melonDS draws them, are checked with the oracle
build: `CAPDIR=~/acww/ref/oracle/captures_v2 ACWW_RENDERER=melon tools/host_oracle.sh build/host64_oracle_usa/acww`): `quick` - the attract demo and title, screenshots at frames 2500 and 8000 and the sound; `play` - scripted
play through the intro (`tools/host_play_script.py play`: A every second), screenshots to frame 25000. The earlier
captures (`~/acww/ref/oracle/captures`, the i386 build with the access-counting virtual time) stay as they were;
`~/acww/ref/oracle/oracle.sh <binary> quick|play` still compares with them when the run uses that time:
`ACWW_TIMING=legacy ~/acww/ref/oracle/oracle.sh <binary> play` (or `CAPDIR=~/acww/ref/oracle/captures
ACWW_TIMING=legacy tools/host_oracle.sh <binary> play`). Under the legacy time the i386 and x86-64 builds are
identical too (the 64-bit drift was 64-bit bugs: docs/portability.md 3.4).

`ACWW_EXIT_FRAME=<n>` ends a run after frame n (the sound file closed as on an interrupt): oracle.sh uses it, so a
check takes as long as the frames it compares (quick ~40 s, play ~3 min).

Checking a rewrite of one file: `tools/host_objcmp.sh OLD.cpp NEW.cpp DIR` compiles both versions as the builds do and
compares the code per function (`tools/host_fndiff.py DIR/A32.dis DIR/B32.dis -v`): every difference left should have
an explanation (inlining, stack layout, a fixed bug).

Finding where two builds part: `ACWW_RAMHASH=<file>` writes a hash of main RAM (and DTCM and the system work area)
every frame and `ACWW_RAMDUMP=f1,f2,...:<prefix>` dumps main RAM at frames: 4 MB as the DS's main RAM from
0x02000000, assembled from the game's data and bss, the main RAM block (the root heap on 32-bit builds) and the system
work area (include/host_mem.h; 32-bit builds: the game's data and heaps live there). Diff
the dumps, ignoring the bytes that differ at an early frame (code addresses in static data), then watch the first
address that differs. Code that reads memory nothing wrote (a lost argument, an uninitialized buffer) shows up as a
difference that depends only on code shape.

Shared declarations: every source file has a generated header, `include/proto/<dir>/<file>.h`, with the
declarations of the functions and global variables it defines, exactly as the definitions write them
(`tools/host_protos.py`; regenerate after changing a definition's signature). Units include these headers instead of
declaring what they use themselves. A unit that still declares something itself defines `PROTO_LOCAL_<name>`
before the includes (the header then leaves that declaration out): its uses don't match the real type yet - each
one is a call or an access to fix (a blob passed for a typed object, a pointer kept in an integer, an argument
count that differs, an array declared for a scalar). `grep -rn 'define PROTO_LOCAL_' src` lists them; the goal is
none. include/proto/.excluded lists the definitions left out of the headers: prototypes that do not compile alone
(types local to one source file: moving them to headers comes with the rewrite of their files) and prototypes that
conflict with a unit that uses them (the unit's own declaration or calls differ: each one a call to fix). Full
regeneration: `python3 tools/host_decls.py --json D.json && python3 tools/host_protos.py --decls D.json
--headers-only` (it compiles every unit against the new headers); `--only SRC...` regenerates single files' headers.

Declarations: `tools/host_decls.py` (libclang; `pip install --user clang==21.*`) compares every declaration written
in a source file with the function's definition; `tools/host_method_calls.py` rewrote the calls of methods through
their mangled names into member calls (non-virtual where the method is virtual, as the direct call was; pointers
passed unchanged). Both builds compile without -fpermissive (since 2026-10-08): a conversion between a pointer and an
integer is an error, so a pointer can no longer be cut to 32 bits silently; `tools/host_strict_errors.sh` (64-bit
syntax check of every C++ unit) reports any that come back.

Portability check: `python3 tools/host_portcheck.py` parses every game unit with libclang for x86_64-linux (the
baseline: what clang rejects although GCC accepts it) and for arm64-apple-macos and x86_64-w64-windows-gnu with
`-mno-ms-bitfields` (`--targets` for others), and reports the baseline's errors and, per target, only the diagnostics
beyond the baseline (`-v` lists them; exit status 0 when all are 0, as they are since 2026-10-08). A clang build:
`python3 tools/configure_host.py --arch x86_64 --cc clang && ninja -f build_host64_clang.ninja` (build/host64_clang_usa;
clang from the PATH, else the newest of ~/acww/toolchains/LLVM-*/bin and ~/acww/toolchains/llvm-mingw-*/bin, or
`--cc <path>`): the game and the host runtime's C by clang, melonDS and the link by g++. It passes the oracle
(`tools/host_oracle.sh build/host64_clang_usa/acww quick|play`) and plays the town, save and continue scripts like
the GCC build (docs/portability.md 2.1). With `--sanitize` too (and `--suffix _clang_asan`) it links with GCC's
sanitizer runtimes; clang's UBSan also checks calls through mistyped function pointers (`-fsanitize=function`).

The modern build (docs/modern.md) compiles the game with GCC and plays like the original in an emulator. The next
step is a host build: the same sources compiled for the PC, with the DS hardware replaced. This note records what
that involves and the choice still open.

## What already holds

- All 867 sources compile for 32-bit x86 (`gcc -m32 -c -DMODERN`, freestanding) without changes.
- Everything is built from source except 6 small functions (~3 KB, docs/unmatched.md) and ~70 assembly units
  (6,700 lines: startup, interrupt and thread-switch code, the C library's soft-float and math routines). On a host
  these are replaced by C or by the host's own facilities.
- The code depends on the DS's 32-bit layout (mwcc's class layout, `(u8 *)this + 0x824`, fixed addresses in RAM,
  I/O and VRAM). A 32-bit host process can map memory at the DS addresses (main RAM 0x02000000, DTCM 0x027e0000,
  ITCM 0x01ff8000, I/O 0x04000000, VRAM 0x06000000...) so that those addresses keep working unchanged. A 64-bit build
  needs the view classes and hardcoded offsets replaced by real member accesses first.
- Overlays: on the host they are all linked into one program; loading an overlay becomes "reset its data and bss and
  run its constructors" (done: `modern/host/overlay.c`, see the status below). Calls to addresses several overlays
  share go through typed C dispatchers (modern/ds/overlays.c, tools/host_shared_calls.py).

## What has to be provided

1. **Process and memory:** fixed mappings at the DS addresses; a main thread plus the game's own threads (the SDK's
   OS_Thread is cooperative and switches stacks in assembly: on the host, one host thread per game thread with a
   baton, or ucontext).
2. **ARM9 hardware the game touches directly:** interrupts (VBlank/HBlank, timers, DMA completion), DMA (copy/fill
   into VRAM), timers, the divider/sqrt units, cache maintenance (no-ops), the 2D engines (registers + VRAM),
   and the 3D engine (geometry commands written to 0x04000400..., then rasterisation).
3. **The ARM7 side**, reached through the PXI FIFO by tag: NVRAM (firmware user settings), RTC, touch panel, sound,
   power management, microphone, wireless (WM), FS/card (ROM reads, backup memory = the save file), cartridge.
4. **A frontend:** window, input (keys, mouse as touch), audio output, save files.

## Status (experimental)

The game runs natively as a 32-bit Linux program: title screen with the 3D town demo, touch to start, the intro
(Kapp'n's taxi, name entry), with sound, and melonDS's renderer drawing both screens (decision 2026-10-05: reuse
melonDS's hardware code for now, replace it with our own once everything works; the goal is a fully native port,
so sound was done natively right away). It holds 60 fps paced (160-200 fps unpaced).

Register accesses are explicit: the game's sources access hardware registers only through `IO_R` / `IO_W` /
`IO_OR` ... (`include/nitro/io_access.h`; rewritten by `tools/host_io_rewrite.py`), which are the plain volatile
accesses on the DS (the matching build is unchanged) and calls to `HostIo_Read` / `HostIo_Write` (hw.c) on the
host. The I/O registers are plain host arrays; nothing is mapped at the DS's I/O addresses (the SIGSEGV trap
and the instruction decoding that used to catch missed accesses are gone since 2026-10-08: they had caught none
in the play and save/continue scripts).

Threads switch stacks with `coro_switch.S` (i386, x86-64 System V, x86-64 Windows, arm64; `coro.h`), not
`ucontext`. The Windows variant (rdi, rsi and xmm6-15 saved too, the TIB's stack bounds switched with the stack) runs
the Windows build's game threads (under Wine, below); `ninja -f build_host64.ninja coro-win64-test` also runs the
same code on Linux, called with GCC's `ms_abi` and a stand-in TIB (`modern/host/tests/coro_win64_test.c`). The arm64 variant runs the aarch64 build
under qemu (below), and `ninja -f build_host_a64.ninja coro-a64-test` checks it alone (`modern/host/tests/coro_a64_test.c`:
x19-x29 and d8-d15 survive a switch, arguments, stack alignment). Interrupts are
delivered only at safe points: register accesses, and when the game enables interrupts, halts or waits. No signal
is involved in running the game (docs/portability.md phase 1): the integer divisions with a variable divisor go
through `DS_DIV` / `DS_MOD` (`include/ds_div.h`, the DS's results for a zero divisor; `tools/host_div_rewrite.py`),
so a hardware divide error is a crash. A watchdog thread reports (only reports) when virtual time stands still
for 2 s of real time - the game spinning on memory without a register access or a wait, which would want an
explicit wait at that site; none is known (`ACWW_WATCHDOG=<ms>`, 0 = off). Runs are exactly repeatable (same
screenshots and sound). The debugging aids that use POSIX signals (crash report, the SIGINT dump, `ACWW_PROFILE`,
`ACWW_WATCH`) are in `debug_posix.c` behind `HOST_DEBUG_POSIX` (on by default except on Windows).

Build (README.md): `python3 tools/configure_host.py && ninja -f build_host64.ninja` (x86-64; SDL2's headers from
pkg-config or `/usr/include/SDL2`, `--sdl2` otherwise; wgpu-native from `toolchains/`, which
`tools/host_fetch_wgpu.py` fills); the 32-bit build: `--arch i386` and `build_host.ninja` (needs the 32-bit sysroot,
`--sysroot`; melonDS only for `--oracle`: `--melonds`, a tree patched with `tools/host/melonds.patch`). Run:
`build/host64_usa/acww <rom.nds>` (`ACWW_FIRMWARE=<firmware image>`: the user settings from a firmware image, else a
built-in one; the save file `<rom>.sav` beside the ROM unless `ACWW_SAVE` or `ACWW_HEADLESS`): a window
shows both screens; keys (by position, so the same keys on other layouts): arrows, X = A, Z = B, S = X, A = Y, Q/W =
L/R, Enter = Start, Right Shift = Select; the mouse on the bottom screen touches; M mutes. Sound goes to the default
audio device (SDL), about 70 ms behind. The 64-bit builds have the frontend in the game's process
(`frontend_sdl.c`, SDL2, linked with the system's libSDL2; docs/portability.md 4.1): also game controllers (D-pad or
left stick; the face buttons by position as on the DS: right = A, bottom = B, top = X, left = Y; shoulders or
triggers = L/R; Start; Back = Select), a finger on the bottom screen as the touch, F11 the window size 1x/2x/3x,
Alt+Enter full screen, and a key binding screen (`frontend_ui.c`): hidden until F1 (or a controller's Back +
Start) opens it, the game paused while it is open, every DS button's key and controller button changeable, kept in
`bindings.txt` in SDL's preference directory (`~/.local/share/acww/`; `ACWW_BINDINGS=<file>`: another);
`ACWW_UI_SCRIPT="frame:F1,Down,Return,K,shot:file.ppm,Escape;..."` scripts it for tests (keys by SDL's names,
`pad:<button>`, `shot:`). The i386 build, which can't load the system's 64-bit SDL, starts the viewer program
(`acww-viewer`, `frontend_viewer.c` + `viewer/`, sharing memory with the game) instead. Without a display (testing):
`SDL_VIDEODRIVER=dummy` (or `offscreen`) and `SDL_AUDIODRIVER=dummy` (or `disk`); `ACWW_WINSHOT="frame:file.ppm;..."`
writes the window's picture as drawn. Window runs use the host's clock (see `ACWW_RTC`), so they differ from headless
runs unless `ACWW_RTC` pins it; with the same clock a window run gives the same screenshots and sound as a headless
one.

Environment: `ACWW_SAVE=<file>` (the save memory - the card's 256 KB backup flash - is loaded from that file at the
game's first access and every write goes through to it; a missing file is a blank memory, created at the first
write. Without `ACWW_SAVE` every run starts blank and nothing is written to disk), `ACWW_RTC` (the clock: the host's
local time, years clamped to 2000-2099; `ACWW_RTC="2026-10-08 12:00:00"` pins a fixed date and time that doesn't
advance. Headless runs default to the fixed 2026-04-01 12:00:00, the clock the oracle's captures were taken with, so
that scripted runs stay reproducible; `ACWW_RTC=host` gives them the local time), `ACWW_HEADLESS=1` (no window),
`ACWW_FAST=1` (not paced to 60 Hz; `ACWW_FAST=<frame>`: not paced until that frame), `ACWW_SHOT="frame:file.ppm;..."`, `ACWW_KEYS="frame:frames:mask;..."`, `ACWW_TOUCH="frame:frames:x,y;..."`;
`ACWW_WAV=file.wav` (the sound, also headless; ACWW_KEYS masks are hex KEYINPUT bits: 1 A, 2 B, 8 Start, 10 Right,
20 Left, 40 Up, 80 Down, 400 X, 800 Y), `ACWW_MUTE=1`, `ACWW_VOLUME=<percent>` (the 3DS: the sound's volume, default 100), `ACWW_3DS_SCALE=1.25` (the 3DS: the screens 1.25 times, 320x240, instead of 1:1), `ACWW_WIDE=1` (the 3DS: the 3D world beyond the DS's screen, to the 3DS screen's edges; on by default in the `--direct` build, `ACWW_WIDE=0` off), `ACWW_SHOW_FPS=1` (the 3DS: the game's frames and the pictures shown a second, "60/30", in the top screen's corner); debugging: `ACWW_STATS`, `ACWW_FPS` (every 120 frames: frames a second and the slowest frame, the renderers' time,
and a sampled profile: the milliseconds a frame in the game, the registers, DMA, the geometry engine, the renderers,
sound, the frontend and the pacing - also where `ACWW_PROFILE`'s signals aren't), `ACWW_IOLOG=addr`,
`ACWW_IOHIST` (register accesses by address and calling code, to `acww_iohist.txt`; written at exit, on SIGINT and
at `ACWW_EXIT_FRAME`), `ACWW_IOTRACE=<file>` and `ACWW_IOTRACE_FRAMES=<from>-<to>` (the register traffic frame by frame: "Virtual time"), `ACWW_TIMING=legacy` (the earlier access-counting virtual time), `ACWW_POSLOG=<n>` (the local player's
position every n frames: for writing input scripts that walk somewhere), `ACWW_WATCH=addr:len` (x86 Linux),
`ACWW_PROFILE` (to `acww_profile.txt`, at the same points as `ACWW_IOHIST`), `ACWW_WATCHDOG`, `ACWW_HIGHMEM` (64-bit hosts: see below), `ACWW_PROCLOG`, `ACWW_HEAPCHECK`, `ACWW_SNDTRACE` (sound commands),
`ACWW_SNDSTATE` (the sound driver's players and channels once a second), `ACWW_AUDIO_INFO` (the frontend's audio
device, underruns); window runs: `ACWW_WINSHOT` (above), `ACWW_VSYNC=1` (also wait for the display's vsync; the pacing
is the runtime's either way), `ACWW_AUDIO_SAMPLES` (the audio device's buffer, default 512 frames).

Local wireless (the gate's DS-to-DS play between copies of the program, `wm7.c`): every copy on a machine and the LAN
finds the others by itself (UDP ports 31470-31477, the first free one; `ACWW_NET_PORT=<port>`: that one). Over the
internet the visitor names the host: `ACWW_NET_PEER=<host>[:<port>][,...]` (the host's port reachable: forwarded, by
default 31470); `ACWW_NET_LAN=0` keeps a copy's beacons off the LAN. Each copy's MAC address (how the game tells the
players apart) is made from the machine's name and the save file's path, `ACWW_MAC=xx:xx:xx:xx:xx:xx` sets it
(headless runs: a fixed one; two headless copies need two). `ACWW_WM_LOG=1` logs the requests, the connections and
the MP traffic once a second, `=2` every packet. For scripted tests: `ACWW_TEST_JOB_DONE=1` keeps the player's
"Nook's part-time job" flag clear (the gate opens only after the job; the save script's save hasn't done it) and
`ACWW_FAST=<frame>` gets there quickly before both copies run in real time (they wait for each other's packets);
`tools/host_wireless_test.sh <binary> <save file>` runs a whole visit with two copies. A connection that breaks
makes the game reset the system (on the DS: back to its title); the program then starts again in place
(`host_reset_system`; a headless run ends there).

The runtime (`modern/host/`):

- `wm7.c`: the ARM7's wireless manager (PXI tag 10): the WM requests answered, the radio's traffic as UDP datagrams
  between copies (docs/portability.md 33).
- `hw.c`: nothing mapped at a DS address (the game's variables are ordinary data and bss); the register model behind `HostIo_Read` / `HostIo_Write` (the registers in host arrays);
  virtual time (scanlines, timers, VBlank), DMA, the divider and square root; interrupts taken after register
  accesses; 60 Hz pacing; the watchdog.
- `plat.h`, `plat_posix.c`, `plat_win32.c`, `plat_common.c`: the operating-system services (docs/portability.md
  4.2): monotonic time, sleeping until a deadline, the local time, a background thread, coroutine stacks (with a
  guard page), zeroed page-aligned blocks (the root heap, the blocks of `HOST_MEM_RUNTIME_ALIGN`), the program
  image's bounds, reading a file into memory (the ROM), a backtrace. The rest of the runtime calls no POSIX function
  except the `ACWW_HIGHMEM` harness (`hw.c`, x86-64 Linux only), the debugging aids (`debug_posix.c`, `guards.c`) and
  the i386 viewer link (`frontend_viewer.c`). `plat_win32.c` is the Windows build's (below).
- `debug_posix.c`: the signal-based debugging aids (`HOST_DEBUG_POSIX`).
- `mem.c`: the DS memory the game reaches by address, as host objects (include/host_mem.h): the system work area
  (`gHostSysWork`, `HW_SYSWORK(T, member)`), DTCM and ITCM (16 / 32 KB blocks), palettes and OAM (`HW_PLTT`,
  `HW_OAM`), the main RAM arena (`HostMainRam_Ptr`). Each block keeps the low bits of the DS's addresses.
  `tools/host_hwaddr_rewrite.py` turns a literal DS address cast in the sources into these (`--check` lists any).
- `vram.c`: VRAM banking without mappings: the banks are host memory, the CPU sees VRAM through five window arrays
  (`HostVram_CpuPtr(dsaddr)`) that follow VRAMCNT - written back to the banks before the renderer reads them and at
  every VRAMCNT change, then filled from the banks' new places, so a pointer kept across a change sees what the DS
  shows there.
- `render.c` / `render.h`: the display interface the rest of the runtime uses (`render_init`, `render_write8/16/32`,
  `render_read32`, `render_frame`, `render_screen`, `render_gx_fifo_irq_level`), and the choice of renderer:
  `ACWW_RENDERER=own` (default) or `melon`, `ACWW_RENDERER3D` (the same choice for the 3D engine, plus `wgpu`, the default where the build and the machine
  have WebGPU (a hardware adapter; else `own`; `own` when comparing with melonDS; tools/host_oracle.sh sets `own`,
  as the captures are the CPU renderers'): the
  own engine's polygons drawn by the GPU, gpu3d/gpu3d_wgpu.c, and the screens composited and shown by it,
  wgpu_display.c, docs/portability.md 5.4; `ACWW_SCALE3D=<n>`: the 3D picture at n times the DS's resolution (default 2); `ACWW_MSAA=1`: no 4x MSAA
  for the 3D edges; the 2D layers drawn by the GPU too (`ACWW_GPU2D=0`: by the CPU), `ACWW_GPU2D_CHECK=1` checked against the
  CPU's every frame, `ACWW_GPU2D_SELFTEST=<n>` on random scenes;
  `ACWW_WGPU_READBACK=1`: the 3D picture read back and composited on the CPU instead; default: the 2D one's), `ACWW_RENDER_COMPARE=1` (both every frame, the first differing pixel of
  each differing frame reported with the layers and effect the own renderer used there; a summary at exit),
  `ACWW_RENDER_SELFTEST=<n>` (`render_selftest.c`: n random scenes through both renderers before the game starts,
  then exit; `_SEED`, `_MASK`, `_OBJ`, `_OBJS`, `_SHOTS` narrow it down), `ACWW_GPU2D_FEATURES=1` (the 2D features the
  frames used, at exit).
- `gpu2d/`: the project's own renderer of the two 2D engines (docs/portability.md 5.1), plain C with no host or
  emulator dependencies (`gpu2d.h`): registers as written, VRAM banks, palettes, OAM and the 3D picture in, both
  screens out. `gpu2d.c` (VRAM mapping with the banks' mirrors, windows, compositing and effects, display modes,
  master brightness), `gpu2d_bg.c` (text, affine, extended and large bitmap BGs, mosaic), `gpu2d_obj.c` (OBJs).
  Compiled with -O2 (about 2 ms a frame on this machine).
- `melon.cpp`: melonDS's GPU (GPL-3.0; register forwarding, 3D commands run at once, VRAM sync), in the oracle build
  only (`tools/configure_host.py --oracle`: `build_host64_oracle.ninja`, `build/host64_oracle_usa`; any --arch) as the
  reference for `ACWW_RENDERER=melon` / `ACWW_RENDERER3D=melon`, the comparisons and the render self-tests. The other
  builds compile `melon_none.c` instead (asking for melonDS stops with a message) and link no GPL code.
- `os.c`: interrupt masking per thread, thread contexts (a host stack each, `coro.h`), lock ids, BIOS waits.
- `pxi7.c`: the ARM7 services - start-up handshake, WVR, NVRAM (firmware image), RTC (the host's local time;
  `ACWW_RTC`), touch panel, card backup (the save memory, `ACWW_SAVE`; replies delayed like the DS's), power management (PMIC registers kept in the host:
  backlights, sound amp, LED, battery never low; power-off ends the run); sound commands go to `snd7/`.
- `snd7/`: the ARM7's sound driver, native: NitroSDK's driver (commands, sequence player, channel allocation,
  envelopes, LFOs, alarms; ported from pret/pokediamond's decompilation of the same SDK driver, its command set and
  shared structures checked against ACWW's ARM9 side, tables from ACWW's ARM7 program) in `driver.c`, `seq.c`,
  `exchannel.c`, `channel.c`; the sound hardware in software (`spu.c`: 16 channels with PCM8/PCM16/IMA-ADPCM, square
  waves and noise, the mixer, the two capture units that the game's effect path uses) clocked by the virtual time;
  `output.c` to the frontend's audio ring and `ACWW_WAV`. Checked against melonDS running the original game: the logo
  jingle comes out with a waveform correlation of 0.998 at the same level, the title music at 0.97-0.997 once its
  fade-in (run by the ARM9 per frame) is over.
- `card.c` (ROM reads), `sdk.c` (the SDK's assembly routines in C), `gaps_code.c` (undecompiled functions),
  `frontend.c` (the frame out, the input in, through `frontend.h`'s AcwwShm block) with `frontend_sdl.c` (64-bit:
  SDL2 in-process, its calls on a stack of their own; an audio callback reads the ring) or `frontend_viewer.c` +
  `viewer/` (i386); both hold the audio queue at a target level by playing up to 0.5% faster or slower.

The build (`tools/configure_host.py`): a plain link of the game's and the runtime's objects (docs/portability.md
3.3; x86-64: `g++ -m64 -pie -o acww <objects> <melonDS libraries> -lstdc++ -lpthread -lm`, a position-independent
executable; i386 a fixed-address one, `-no-pie`), the game's variables ordinary data and bss where the compiler and
linker put them (no code depends on the DS's variable order since 2.8; modern/link/game_begin.c and game_end.c,
linked first and last of the game's objects, mark where they are for the debug dumps and `HostMem_IsGameRam`),
DTCM's variables ordinary variables (DTCM is a host block), its C library's
names renamed to `ds_*` (`include/host_libc_rename.h`, included first in every game unit; the program's global
operator new/delete are the host's, `modern/host/newdelete.cpp`: the game allocates from its heaps by name,
`new (kGameHeap) T` / `Mem_AllocForNew`, and blocks of its heaps go back to them on delete), calls to addresses
several overlays share through typed C dispatchers (`Shared_<name>`, include/host_shared_calls.h and
modern/gen/shared_*.c, generated by `tools/host_shared_calls.py`, the calls renamed in the sources; regenerate after
changing such a function's signature or callers: `--check` lists what is out of date), gap data as typed C (`tools/host_gaps.py`, `modern/gen/gaps.c`, the same for
every architecture; the zero-initialised gap variables are defined in the sources with their types). The DS linker
script's absolute symbols are constants (`include/sdk_linker_symbols.h`); the names symbols.txt gives to places
inside variables are pointers defined at the end of the variables' units (`HOST_LABEL_DEFINE`, include/host_labels.h;
a unit declares such a label as `#define gSavePlayers HOST_LABEL(u8 (*)[], gSavePlayers)`), and second names of
functions are the functions themselves in the sources: no linker script supplies names. `-fpcc-struct-return` (small structs
returned in memory, as mwcc) is kept for the 32-bit build only, where it is the i386 Linux ABI's default anyway.

Differences from the DS that the sources handle under `HOST` (or MODERN):

- The ARM C++ ABI's constructors return `this`, x86's don't: code using a constructor's result keeps the pointer;
  `void X_Create() { new X; }` create functions return the object.
- Pointers to virtual member functions: ARM `{offset, 2*adj+1}`, x86 `{offset+1, adj}`: the compiler's, written as
  `&Class::member` (ProcBase's phase dispatch); member functions used as such must be 2-byte aligned
  (`-falign-functions=4`).
- PXI messages that carry an address (the sound command list, the card request block, the NVRAM buffer, a WM
  request): the ARM7 side is the host's, so the pointer is handed over whole before the message goes out
  (`PXI_PointerWord`, include/nitro/pxi_ptr.h); the message itself is sent as on the DS. The DMA functions hand the
  whole source and destination to the DMA model the same way (include/nitro/dma_host.h).
- Reading through NULL is harmless on the DS (address 0 is mapped) but not on the host.
- Code that ran over one variable into the next (a clear spanning two arrays, a loop ending at the next variable's
  address) is fixed (docs/portability.md 2.8). The guard variant finds new cases: `python3 tools/configure_host.py
  --guards [--arch x86_64] && ninja -f build_host[64]_guards.ninja` puts 64 poisoned bytes after every game variable
  (`tools/host_guards.py`); `ACWW_GUARDS=1` reports every guard that changed, once per frame (the variable, the frame
  and an address for `watch *(int *)<addr>` in gdb), `ACWW_GUARDS_FILL=<byte>` changes the poison (a run that
  changes with it reads past an end), `host_guards.py --only LO-HI` (by hand on the map) bisects such a read. The guards don't see reads: one table split
  into variables that the code indexes as one (ov068's field villager state table, three 8-entry pieces indexed
  0..0x17) showed up only as a date-dependent crash (both builds). The in-game date and time change what the game does
  (villagers' plans and items, events, the random seed `Clock_GetTimeSeed`): a sweep of headless runs with many
  pinned `ACWW_RTC` values (`ACWW_EXIT_FRAME=3000`; every hour of a day, the 1st and 15th of each month, holidays,
  random minutes and seconds) reaches what the default clock doesn't.
- The sanitizer variant sees reads too: `python3 tools/configure_host.py --arch x86_64 --sanitize
  address,bounds,undefined && ninja -f build_host64_asan.ninja` (build/host64_asan_usa; x86-64 only) compiles the
  game and the runtime with ASan (redzones around globals, stack and heap objects: a read past an end is reported as
  well as a write), UBSan's array bounds (`bounds`: indexes against the declared size, also arrays inside structs) and
  the rest of UBSan (`undefined`, without `alignment` - the DS heaps' 4-byte aligned blocks - and `shift-base`; shift
  counts past the width stay checked: ARM gives 0 for them, x86 shifts by the count modulo 32, `include/ds_shift.h`).
  Every check recovers (`-fsanitize-recover=all`): a run goes on and reports each place once. Run it with
  `ASAN_OPTIONS=halt_on_error=0:detect_leaks=0:log_path=<prefix> UBSAN_OPTIONS=halt_on_error=0:print_stacktrace=1:log_path=<prefix>`.
  The runtime's stack switches tell ASan which stack runs (`coro.c`: `__sanitizer_start_switch_fiber` /
  `__sanitizer_finish_switch_fiber`, `HOST_ASAN`). The game's heaps are carved out of big blocks, so the NNS expanded
  and frame heap allocators poison what isn't allocated (free blocks, a frame heap's free middle, a block's bytes past
  the size asked for; `include/host_asan.h`) and are themselves compiled without ASan's checks, as are the C library's
  word-at-a-time string functions (`ASAN_UNCHECKED` in configure_host.py); an overlay's reset unpoisons its variables
  (a heap may have lived in one), and the ARM7 sequence reader (`snd7/seq.c` `SeqCacheFetch`) isn't checked (it reads
  16 aligned bytes, and may read a sequence the ARM9 has just freed, as the SDK's heap protocol allows). It runs the
  oracle's scripts with the same pictures and sound as the plain x86-64 build (about 4x slower). Its findings and the
  fixes: docs/portability.md 2.8, "sanitizer sweep".
- Integer division by zero: the DS's division routine (`_s32_div_f`) gives the dividend as the quotient and 0 as
  the remainder; x86 traps (hw.c gives the DS's result and reports the site), arm64 gives 0. The game does it (e.g.
  `spl_scl_in_out` at the end of a particle animation).

### Virtual time (docs/portability.md 3.4)

The clock is a cycle counter (`sCycles`, the ARM9's 33.5 MHz bus clock: 2130 cycles a line, 263 lines a frame) that
the game sees only through: the VBlank (and the once-a-frame HBlank) interrupt and DISPSTAT / VCOUNT; the timers -
timer 0 is `OS_GetTick`'s counter (prescaler 64, its overflow interrupt extends it), timer 1 runs the OS alarms
(`OS_Sleep`, the SDK's alarms); the replies the host delays (the card's backup transfers, `pxi7_send_after`); the ARM7
sound driver's ticks and alarms (snd7, clocked by the counter); busy waits (`OS_SpinWait`, `SVC_WaitByLoop`); and
the polling loops that wait for one of these. When the game halts or waits for an interrupt (`OS_Halt`, the idle
thread, `VBlankIntrWait`), the clock jumps to the next event.

What moves the clock between waits (hw.c, `io_read_cost`):

- a read of DISPSTAT / VCOUNT or of a timer's counter: 64 cycles (one OS tick);
- a polling loop - the same register read again with no other register access in between: from the fifth read on,
  4096 cycles a read;
- `OS_SpinWait(n)`: n / 2 cycles (n is in 67 MHz CPU cycles), `SVC_WaitByLoop(n)`: 2n;
- nothing else. Any other register access (the interrupt master enable saved and restored around every critical
  section, DMA, the divider and square root units, the 3D command ports, the PXI FIFO, ...) takes no time.

So the clock depends on what the game does that it can observe, not on how many register accesses the code makes
on the way: an extra interrupt-mask save, a copy done by DMA or by the CPU, an SDK routine rewritten in C, a
renderer that answers status reads differently once (but not one that keeps the geometry engine busy longer: a
polling loop's length counts), or the build's architecture. Before (until 2026-10-08; `ACWW_TIMING=legacy` brings it
back) every register read took 64 cycles and a read of the register read last 4096: time inside a frame counted the
reads of the interrupt mask, of DMA channel 1's control after each DMA (read, write, read: counted as a polling loop
since writes didn't break it), of the divider... - ov004's `FX_Div` constructor at the overlay's load once moved the
attract demo's town by a frame, and any change to such code shifted every later frame.

What the game uses time for, measured (oracle play script, 25000 frames, legacy model, `ACWW_IOHIST`): 6.68 million
register reads, of them the interrupt master enable 29 %, DMA channel 1's control 10 %, GXSTAT 10 %, IE 8.5 %, IF
6 %, the PXI FIFO control 5.5 %, the divider 7.6 %, the square root 4.5 %; the clock reads are 0.6 %: VCOUNT 41,296
(`HBlank_Handler` once a frame, and the VRAM upload queues `VramQueueTex_Run` / `VramQueue2d_Run`, which run queued
uploads while `VCOUNT + cost` fits the VBlank), DISPSTAT 4, the tick counter 77 (`OS_GetTick`: the 40 ms budget of
`FieldScene` creation and of two loaders that then continue in the next frame, the communication icon's blink,
`Backup_CancelAndWait`'s timeout, the crash screen's clock). With the new model (the same script): 43,425 clock
reads, 1,595 polling reads in 15 frames - all of them `G3X_ClearFifo` waiting for the geometry engine at a scene's
start (melonDS's GPU reports it busy until its frame has run: 137 reads, one frame), 167,020 waits.

Consequences: a frame's work takes (almost) no virtual time, so the game always reaches its VBlank wait in time (no
slow frames), the VRAM queues upload everything queued in one VBlank, the 40 ms creation budget is never used up (a
scene is created in one go) and loading is instant: the title screen is up at frame 250 (the legacy model: 1550, after
1000 black frames; the DS about 900). `OS_GetTick` advances one tick per call within a frame and 8752 ticks a frame.
Both builds give the same register traffic frame for frame: `ACWW_IOTRACE=<file>` writes a line per frame (reads,
writes, a hash of every access's address and value - but for the DMA address registers and the PXI send FIFO, which can hold host addresses -, the clock, the clock reads, polls and waits;
`ACWW_IOTRACE_FRAMES=<from>-<to>` lists every access in those frames, with the calling code), and the i386 and
x86-64 traces of the play script are byte-identical (25002 lines), as are those of the town, save and continue
scripts. The traces are how the 64-bit bugs that made the builds part were found (docs/portability.md 3.4): the first
frame whose line differs, then the first access that differs in it, then gdb at that cycle in both builds.

A spin on memory with only timeless register accesses (an interrupt-mask save and restore around a check of a flag
an interrupt handler sets) would hang; after a million accesses without time passing, time moves to the next event and
the site is reported once ("register accesses without time passing"). None is known (it never fired in the quick,
play, town, save and continue scripts); the watchdog (`ACWW_WATCHDOG`) reports a spin without any register access.

### 64-bit hosts (in progress)

`python3 tools/configure_host.py --arch x86_64 && ninja -f build_host64.ninja` builds `build/host64_usa/acww`, a
native x86-64 program (status: plays like the i386 build through the attract demo, the title (logo, clouds, talk
text) into the town scene with villagers, insects, fish and particle effects; screenshots, sound, save files and register traffic identical to the i386 build's in the quick, play, town, save
and continue scripts: docs/portability.md 3.4). It is a
PIE (loaded anywhere, ASLR) with nothing at a DS address: the game's variables are ordinary data and bss, the rest
of the DS memory host objects (docs/portability.md 3.2, 3.3); the root heap and the threads' stacks are anonymous
mappings anywhere. Everything is above 4 GB. The crash report gives each code address also less the load bias, the
address `addr2line` and gdb take for the program file.

**aarch64 Linux** (docs/portability.md 4.4): `python3 tools/configure_host.py --arch aarch64 && ninja -f
build_host_a64.ninja` cross-compiles `build/hosta64_usa/acww` with ARM's GCC 14.3 toolchain (`--cross`, default
`~/acww/toolchains/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-linux-gnu`, its glibc sysroot built in). The
game keeps `-fsigned-char` (Linux arm64's char is unsigned) and gets `-ffp-contract=off` (GCC fuses a * b + c on
arm64 by default; x86-64 has no FMA without -mfma), as do the runtime and melonDS. No SDL2 for arm64 here: the
frontend is `frontend_none.c` (`HOST_FRONTEND_NONE`, always headless; any 64-bit build without SDL2 headers gets it).
melonDS's core is cross-built once, outside the repository, with the CMake toolchain file in tools/host:

    cd ~/acww/ref/melonDS && cmake -S . -B build-core-a64 -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE=<repo>/tools/host/aarch64-linux.toolchain.cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS=-ffp-contract=off -DCMAKE_CXX_FLAGS=-ffp-contract=off -DBUILD_QT_SDL=OFF -DENABLE_JIT=OFF \
      -DENABLE_GDBSTUB=OFF -DENABLE_OGLRENDERER=OFF -DENABLE_LTO=OFF -DENABLE_LTO_RELEASE=OFF -DUSE_VCPKG=OFF \
      -DMELONDS_EMBED_BUILD_INFO=OFF -DTEAKRA_BUILD_TOOLS=OFF -DTEAKRA_BUILD_UNIT_TESTS=OFF
    ninja -C build-core-a64 src/libcore.a src/teakra/src/libteakra.a

(no JIT and no GDB stub: the runtime's melon.cpp is then compiled without `JIT_ENABLED` / `GDBSTUB_ENABLED`, which
change melonDS's class layouts). It runs under qemu user mode (`~/acww/toolchains/qemu/usr/bin/qemu-aarch64 -L
<toolchain>/aarch64-none-linux-gnu/libc build/hosta64_usa/acww <rom>`), at 15-35 frames a second;
`tools/host_oracle.sh` runs the binary through `$ACWW_RUNNER` (that qemu command line) with `QUICK_TIMEOUT` /
`PLAY_TIMEOUT` raised (quick takes about 10 minutes, play about 30). Status: quick (both screenshots and the whole
WAV) and play identical to captures_v2; the save and continue scripts give the x86-64 build's screenshots (70 and
16, every 500 frames), save files and `ACWW_IOTRACE` files byte for byte. No source change was needed for that
beyond the build: the runtime's x86-only bits were already guarded (debug_posix.c now also reads the arm64
registers for its crash report; `ACWW_WATCH` stays x86), and `host_panic` prints only the format on Linux arm64
(its `void *` argument list can't be a `va_list` there).

**Windows x86-64** (docs/portability.md 4.4): `python3 tools/configure_host.py --arch x86_64 --os windows && ninja -f
build_host_win64.ninja` cross-compiles `build/hostwin64_usa/acww.exe` with llvm-mingw (clang, the UCRT, libc++ and
lld: `--mingw`, default the newest `~/acww/toolchains/llvm-mingw-*`), linked statically (only system DLLs needed),
with an 8 MB stack for the process's own thread. MinGW keeps the Itanium C++ ABI; the game and the runtime's C get
`-mno-ms-bitfields` (GCC's bitfield layout, which the save data's `static_assert`s need); the runtime's C++ units
(melonDS's headers) keep the platform's. No frontend yet (`frontend_none.c`: headless). The runtime builds without
POSIX: `plat_win32.c`, no `frontend_sdl.c` / `frontend_viewer.c` / `plat_posix.c`; `debug_posix.c` and `guards.c`
compile to their stubs (`HOST_DEBUG_POSIX` 0). COFF objects can't be aligned beyond 8 KB, so the host blocks aligned
more (DTCM, ITCM, the main RAM block, VRAM's banks and windows) are allocated at start-up there
(`HOST_MEM_RUNTIME_ALIGN`, include/host_mem.h; `HostMem_Check` verifies the alignment as before). melonDS's core is
cross-built once, outside the repository:

    cd ~/acww/ref/melonDS && cmake -S . -B build-core-win64 -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE=<repo>/tools/host/win64-mingw.toolchain.cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS=-ffp-contract=off -DBUILD_QT_SDL=OFF -DENABLE_JIT=OFF -DENABLE_GDBSTUB=OFF \
      -DENABLE_OGLRENDERER=OFF -DENABLE_LTO_RELEASE=OFF -DUSE_VCPKG=OFF
    ninja -C build-core-win64 core

It runs under Wine here: `tools/host_wine.sh build/hostwin64_usa/acww.exe <rom>` (no display, `WINEDEBUG=-all`, a
prefix of its own: `$WINEPREFIX`, default `~/.cache/acww-wine`; Unix paths work as they are, through Wine's Z:
drive), and `ACWW_RUNNER=tools/host_wine.sh tools/host_oracle.sh build/hostwin64_usa/acww.exe quick|play` (as fast as
the Linux build: quick about 1.5 minutes, play about 4). Status: quick (both screenshots and the whole WAV) and play
identical to captures_v2; the save and continue scripts give the x86-64 build's screenshots (70 and 16, every 500
frames), save files and `ACWW_IOTRACE` files byte for byte. The runtime writes its text files (`ACWW_IOTRACE`,
`ACWW_RAMHASH`, `ACWW_IOHIST`) in binary mode, so they have LF line ends on Windows too.

`ACWW_HIGHMEM=1` (x86-64 Linux) is the low-4 GB poisoning harness (docs/portability.md 3.0, 3.3): the whole low 4 GB
(from 64 KB up) reserved without access (`PROT_NONE`, as macOS's `__PAGEZERO`); the PIE program, the C library's
heap, the root heap and the stacks are all above it, so any DS address used as a pointer faults. A pointer
cut to 32 bits then faults, and the crash report says where (its frames as addresses: `addr2line -f -i -C -e
build/host64_usa/acww <addresses>` names them, with the addresses less the load bias that the report prints in parentheses). The quick, play, town and save/continue scripts run through under it, with the same
pictures and sound as without it. Pointers that must live in a 32-bit slot of the DS's data formats (a sound bank's
wave archive links, an NFTR font's block links, a wave loaded on its own) go through `include/host_ptr32.h`: the
pointer itself when it fits, else a handle into a host table.

What changes with 64-bit pointers, and how the sources handle it (the DS build unchanged, every step checked with the
matching build and with the i386 build's screenshots and sound staying bit-identical):

- Integers holding pointers: `uptr` / `sptr` (`include/uptr.h`; u32 on the DS). `typedef unsigned long u32`-style
  typedefs and fx32 became int under HOST (long is 64-bit). `HOST64` marks layouts for the 64-bit host only.
- Objects sized by their DS size: `HOST_SIZED(n, type)` (n on the DS, at least the real size on the host),
  `HOST_ALIGN8`. Tools find the sites: `tools/host_alloc_sizes.py` (`(T *)Heap_Alloc(h, N)`, `count * N`),
  `tools/host_ctor_buffers.py` (buffers, also members, built by a mangled constructor call),
  `tools/host_placement_sizes.py` (`p = Heap_Alloc(h, N); new (p) T`). Strides and offsets written as numbers
  (`cell += 0x28`, `heap + 0x24`, `(u8 *)task + 0x1c`) get names (`MAPBLOCK_SIZE`, the member's offset).
- One object, two declarations (a typed class and a DS-offset view): the views agree on the DS only.
  `tools/host_view_conflicts.py` finds shared variables (and with `--types`, same-named types) whose views disagree;
  `tools/host_view_layout.py` generates the 64-bit layout of a view from the class it views (DWARF of both builds:
  each member at the place of the same byte in the real class) with `static_assert` checks
  (example: `TalkWindowState`).
- Labels inside variables: C pointers to a member path (`HOST_LABEL_DEFINE(gSndBgmHandle, (char *)&gSndMgr.bgmHandle.p)`,
  written once by `tools/host_labels.py` from the 32-bit build's DWARF), so they are at the right place on every host.
  Gap data is typed C (pointers 8 bytes on 64-bit hosts).
- SDK objects the sources kept as byte blobs (OSThread, OSMutex, FSFile, FSArchive, NNSFndArchive, sound handles,
  the NNS G3D render / animation objects: `include/nnsys/g3d_rt.h`) have their real types on the host.
- Data in the ROM's file formats keeps its 32-bit slots (the sound banks' wave archive links, offsets in G3D
  resources).
- Hand-written varargs: `DS_VA_ARG` (`include/ds_stdarg.h`).
- The ROM's overlay table: 32-byte records whose two .sinit pointers are 8 bytes each in the host's
  `FSOverlayInfoHeader`; `FS_LoadOverlayInfo` copies the fields one by one (copied whole, the file id was left
  unset and some overlays - 2, 3, 6 in the title demo - failed to load, unnoticed while constructors ran at start-up).
- The save data (`gSaveData`, include/save/SaveData.h) is written to and read from the save file as it is in memory,
  so its records are plain data with the DS layout on every host, checked by `static_assert`s of their sizes and key
  offsets (SaveData, PlayerData, VillagerData / SaveVillagers, Letter and the letter containers, Pattern, TownState,
  PlayerErrands). No vtable or pointer member inside a save record: `Letter` had a virtual destructor only (no
  letter is deleted through a base pointer), so it keeps the DS's vtable pointer as a data word (`dsVtable`, the
  constructor writes the DS value 0x020dd458, `Letter_Clear` zeroes it); 64-bit integers in save records are `s64`
  / `u64` (4-byte aligned, as on the DS: a `long long` member made PlayerData 8-byte aligned and its array stride
  0x2290). Walks over the records use `sizeof` of the stored type (`sizeof(PlayerData)`, `sizeof(Letter)`, ...).
  Checked by cutting `gSaveData` out of `ACWW_RAMDUMP`s of both builds at the same frames of the scripted intro: the
  images differ only in content (random ids once the two runs' random numbers part, the bulletin board's default
  posts: the mail text builder's members are still DS-sized blobs, `include/talk/MailTextBuilder.h`), not in
  layout. The i386 build crashed on arriving in town (ov009's
  `sBuildingResources` still held the title demo's freed resources); with overlays reset on load it arrives in town
  and goes through the town hall's tutorial. It then stalled in `PMi_WaitBusy`: the game polls the battery every 20
  frames once the player can move (`LowBattery_Poll` -> `PM_GetBattery`, a PMIC register read through the PXI's PM
  tag 8), and the host didn't answer the PM tag at all; `pxi7.c` now does. With the script below extended (legacy time: it
  stops at frame 21050, then Down from 21100 to 21500) the player leaves the town hall (town at ~frame 22000) and
  walks around town (checked to frame 34000: villagers, beach, no stop). With A and Start kept going after the
  tutorial the player only talks to Pelly again and again (Start opens the save menu, "I can't save right now" in a
  new town until the first save). How a new game first saves: the town hall's tutorial ends, the player goes into
  their house (scene 1, a house room) and out again; Tom Nook then waits in front of the house (a visitor:
  `Visitor_CheckTomNook` wants the previous scene to be a house room), tells about the loan, walks off and clears the
  new-town state (`SpNpcNookIntro::mainAct04` -> `GameStart_Clear`); from then on Start opens "Do you want to quit
  playing for now?" with "Save and end" first: the save (both copies of the town data, 0x0-0x2bfc0 in 256-byte
  writes, each verified) and the title screen. The scripts that get there are `tools/host_play_script.py town|save|continue|menu` (one script for
  every build since docs/portability.md 3.4; `eval "$(python3 tools/host_play_script.py save)"` sets `ACWW_KEYS` and
  `ACWW_TOUCH`; the oracle's firmware image, the default pinned clock, a fresh `ACWW_SAVE`). With the frame-based time
  the game reaches each point sooner and makes a different town (its random seeds come from the clock): `town` - the
  touch at (128,96) at 2200, A every 60 frames from 2400 and Start every 60 from 7650 to 21050, a touch at (218,180)
  every 600 frames from 7630 (the name entries; the town at ~12000, Kapp'n's warp to the town hall), then B every 60
  frames to 21700 (closes Pelly's last talk) and Down 21800-22200: out of the town hall in front of it at (113, 56),
  frame ~22150. `save` adds the walk to the house (Down 545 frames past the house's west side, Right 65, Up 150: the
  door at (119, 109), frame ~23250; the town and the walk since `NpcAnimCtrl::isAnimFinished` was fixed, which
  changed Kapp'n's animations and with them the random numbers the new town is made from), A
  at 24000-24240 every 60 (into the house), Down at 24900 (back out: Tom Nook), A every 60 frames 25300-31840 (his
  talk), Start at 32300 and A at 32500, 32700, ..., 33500 (Save and end; "Come back and see us again" at ~32750), the
  title at ~34500. `continue` (A every 60 frames from 2400 with that save file): the greeting, the player waking up in
  the house, in front of the house in town from ~4300. `menu` (2026-11-04): A every 60 frames 2400-3500 with that
  save file, the player out of the house at ~4100, Y at 4400: the pause menu on the pockets from ~4450 (the PC
  build; R / L at later frames go through its tabs). Under `ACWW_TIMING=legacy` the earlier i386 script
  applied before the isAnimFinished fix (since then **unverified**: the fix changes that town too) (A and Start to 21050, Down 21100-21500, the walk 23200:60:80 23300:200:10 23520:220:80 23750:620:10
  24380:120:80 24510:400:10 24970:175:40 25150:64:10 25220:100:40 25330:14:20 25350:150:40 to the door at (151, 109), A
  at 25700-25940, 26600:150:80, A 27000-33540, Start 34000, A 34200-35200: the save at 34786-34907). With
  `ACWW_RTC="2026-10-08 18:30:00"` the continue shows 10/8 (Thursday) 6:30 PM, the evening sky and the weeds of the
  days that passed. `ACWW_POSLOG`, the town map (X) and `Town_FindPlayerHouse` / `Town_FindTownHall` called from gdb
  were how the walk was found. The bugs on the way (the `spl_calc` crash reported earlier
  no longer occurs): the PMIC register-read table (`PMRegInfo`, its destination read through a DS label at the DS
  stride, the table larger than its gap object), and three views of objects at DS offsets: `Unk_02005e7c` and
  `Unk_02007694`, the PlayerActor's net-event and action handlers, declared the actor's members at their DS
  offsets (a pending field event read from other bytes: a pick-up fanfare at unit (-1183, 968) crashed in the town
  hall; the player and Pelly not drawn); they now derive from PlayerActor without members of their own. The BGM
  manager reached as bytes (`data_021c1b3c + 0x248` / `0x1c4` / `0x2e4`) is a `BgmManager *`. A script that gets through the
  name entries: the oracle's play script (A every second from frame 2400) plus, every 600 frames from 7630, a touch
  on the keyboard's Confirm button (218,180) (it only dismisses the "no room" message: touches on the keys don't
  register) and Start every 60 frames from 7650 (Start confirms a name); presses and touches 6 frames each.
- A C-style cast from a base pointer to a derived class is a static downcast (it subtracts the base's offset):
  converting a derived pointer to its base subobject's address needs `reinterpret_cast` after the `static_cast`.
- Members reached by a DS offset from a pointer: `tools/host_this_offsets.py` rewrites `(u8 *)this + N` (and
  `p + N` for a `u8 *p` / a typed or base-class pointer listed in `tools/host_offset_assumes.txt` with its real type)
  to `HOST_AT` / `HOST_AT_A`; a unit reaching `this`'s members through a macro (`#define M(T, o)`) gets a
  compile-time map of the offsets it uses (`tools/host_offset_map.py`, e.g. the camera); an actor's position from
  an untyped pointer is `ACTOR_POS_AT(p)`; views of whole objects are generated (`tools/host_view_layout.py
  --remove` regenerates one after the real class changed; a size check fails the build when a layout is stale).
- Tables of pointers to member functions: an entry is 16 bytes on x86-64; code that reads the second member at the
  DS offset `+ 8` (state tables: title screen, NPC behaviour, talk scripts) reads half of the first. The same for
  `{id, member function}` handler tables walked with the DS stride (`(u32)tbl + i * 12`: the special NPCs' talk
  handlers): index the array.
- Records that start with a smaller record: an actor's profile (`ActorProfile`) is a `ProcProfile` (16 bytes on
  x86-64) followed by the actor flags and cull box; read through `u32 *` (`e[2]`) it gives the priorities instead
  (the actor culled away: Kapp'n missing in the taxi).
- The communication manager (`CommManager`) reached by DS offsets (`gCommManager + 0x64` for `myAid`, `0x68`
  `localSlot`, `0x6c` `memberCount`, views padded to them): on x86-64 these land in other members (the intro's
  player was never spawned: `localSlot` stayed 4).
- Heaps are sized for the DS's objects: on 64-bit hosts every heap the game creates with a size is twice that
  (`ExpHeap_Create` / `FrameHeap_Create`), and the root heap is a block of its own (`host_root_heap_region`,
  8 MB, an anonymous mapping) instead of the DS's main RAM arena.
- Stack buffers sized to what the DS's stack slot padding absorbed: `Clock_GetDate` writes four bytes into
  three-byte locals (on x86-64 the fourth hit a saved register).
- ARM idioms that don't carry over: an empty function's "result" is its first argument (still in r0 on the DS).

Debugging: gdb works directly (no signals to pass through any more); SIGINT prints every game thread's backtrace. Most crashes so far were heap blocks
overwritten past their end: walk the heap's free list at the crash, then watch the damaged block header from
`Main_Init` on to find the writer. Comparing with the i386 build: `ACWW_STATS=<frames>` (display registers, VRAM bank
contents, 3D command and register access counts per interval), `ACWW_VRAMDUMP=<frame>:<file>` (VRAM banks, palettes,
OAM, display I/O), `ACWW_PROCLOG=1` (processes created), screenshots at the same frames; the random number calls
(a breakpoint on `Random_NextBelow` logging the callers) show where the two runs' logic first differs.

Overlays are reset when they are loaded, as on the DS (which reads a fresh image from the ROM, clears the bss and
runs the static initialisers), in plain C: every overlay unit (`src/ovNNN/*.cpp`) ends with the list of the
variables it defines (`HOST_OVERLAY_VARS`, include/host_overlay_vars.h) and may have a `StaticInit_<unit>` with what
the DS's `__sinit` did that isn't constant data (tables of member function pointers, values read at the load, the
default constructors of its objects: `HOST_STATIC_INIT_ARRAY`, `HOST_STATIC_CONSTRUCT`); modern/gen/overlay_table.c
collects them per overlay. Both are generated by `tools/host_overlay_vars.py` from both builds' objects: run it after
adding, removing or renaming a variable of an overlay unit (`--check` lists what is out of date, and reports a
function-local static - move it out of its function, with an explicit guard if it is initialised at run time - or a
static initialiser the compiler still emits for an overlay unit, which would run only at start-up: make the object
constant-initialised (constexpr constructor) or initialise it in StaticInit). `modern/host/overlay.c` copies the
variables in `HostOverlay_Init()`, first thing in `main()`; `FS_StartOverlay` restores overlay N's variables and
runs its StaticInits in link order. Objects of classes with default constructors are constructed at start-up too
(their constructors only set their members) and again at every load. Overlays that share an address range on the
DS keep separate variables here; each is reset on its own load. 332 units with 2956 variables, 15 StaticInits.
(The tool reads the writable data and bss of each object, `objdump -t`: constant data with relocations - tables of
pointers, vtables, the lists themselves - is in `.data.rel.ro*` in the position-independent x86-64 objects, which nm
calls data too; until 2026-10-08 `--check` reported every unit out of date and 231 function-local constant tables as
"can't be reset" for that reason, since step 3.3 made the x86-64 build a PIE.)
Debugging: `ACWW_OVLOG=1` lists each load (the bytes the reset undoes, by variable), `ACWW_OVRESET=0` restores the
earlier behaviour (every overlay's StaticInits once at start-up, no reset), `ACWW_OVRESET=-4,9` that for the overlays
listed. ov004's information points use the divider registers (`MuseumInfoPoint_InitFromDef`: `FX_Div`), so virtual
time passes for them at the overlay's load.

The game reaches each scene sooner (in frames) than on the DS: loading takes no virtual time, so the title is up at
frame 250 instead of about 900 (with the earlier access-counting time, `ACWW_TIMING=legacy`, at 1550: see "Virtual
time").

## The choice: how to provide the hardware (decided: A for the display for now, B for sound)

| | A. Reuse melonDS's hardware code | B. Write it for the port |
|---|---|---|
| 2D/3D rendering | melonDS's GPU2D/GPU3D (software and OpenGL renderers) fed from the mapped VRAM and the command FIFO | own 2D compositor and a 3D rasteriser / GL renderer for the DS geometry engine |
| ARM7 services | run the original ARM7 binary in melonDS's ARM7 interpreter with its SPU, RTC, SPI, Wifi | HLE each PXI service on the host (ROM/save files, host clock, mouse); reimplement the SDK sound driver (sequence player + mixer) |
| Effort | smaller: glue between the native ARM9 code and the emulated parts | large: renderer and sound driver are the big pieces |
| Fidelity | as good as melonDS | depends on the reimplementation |
| Licence | melonDS is GPL-3.0: the port as distributed becomes GPL | free to choose |

Either way the game code runs natively; only the peripherals differ. A mix is possible: melonDS's renderer first to
get a picture, replaced later.

Until this is decided, the work that both options need goes first: the host runtime (memory map, threads,
interrupts, DMA, timers), a headless host build that boots to the title's main loop with stub ARM7 services, and
replacing the assembly units.

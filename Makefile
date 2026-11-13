# ---------------------------------------------------------------------------
# One place to build and test everything locally.
#
#   make game         the port for the 3DS (needs devkitPro with 3ds-dev):
#                     build/upstream/build/host3ds_usa/acww.3dsx
#                     (runs with your own ROM dump: README.md)
#   make cia          the HOME Menu version: build/cia/acww.cia, a forwarder that opens
#                     sdmc:/3ds/acww/acww.3dsx (no game code in it; needs devkitPro, git,
#                     gcc, g++; builds makerom and bannertool once; placeholder art in cia/)
#   make cia ROM=...  that with the game's icon, taken from your ROM (not for release)
#   make pak ROM=...  the data pack from your ROM, as the web builder makes it:
#                     build/game.pak (game content: for your SD card only)
#   make pc           the PC build (x86-64, gcc-14)
#   make test         run the tools' unit tests and build for the PC
#   make check ROM=path/to/rom.nds
#                     local regression test: one headless, deterministic run of
#                     the PC build (new game to the first save), its screenshots,
#                     sound and save file compared by SHA-256 with
#                     build/check/baseline.txt; prints what differs, exit 1 if any
#   make check-baseline ROM=...   store the run as that baseline (do this on main,
#                     before a change). Covers the game's code and the shared
#                     runtime on the PC, NOT the 3DS-only code (N3DS_PORT: that
#                     needs Azahar or a console). Needs a ROM: not for CI.
#   make clean
#   (make with no target: make game)
#
# Run `python tools/bootstrap.py` once first (it fetches the decompilation,
# applies patches/ and links 3ds_port/ in). Builds contain game code: they
# are for your own use and must never be committed or shared (CONTRIBUTING.md).
#
# NINJAFLAGS passes options to ninja, for example: make game NINJAFLAGS=-j16
# ---------------------------------------------------------------------------

UP := build/upstream
# The version the program writes as its log's first line: PORT_VERSION from the environment (CI: a release's tag),
# else dev- and this repository's commit.
PORT_VERSION ?= dev-$(shell git rev-parse --short HEAD 2>/dev/null || echo unknown)
VERSION_H := $(UP)/modern/host/port_version.h
NINJAFLAGS ?=

.PHONY: all game pak cia pc test clean check-bootstrap check check-baseline

all: game

check-bootstrap:
	@test -f $(UP)/tools/configure_host.py || \
	    { echo "Run first: python tools/bootstrap.py"; exit 1; }

game: check-bootstrap
	@echo '#define PORT_VERSION "$(PORT_VERSION)"' | cmp -s - $(VERSION_H) || \
	    echo '#define PORT_VERSION "$(PORT_VERSION)"' > $(VERSION_H)
	cd $(UP) && python3 tools/configure_host.py --os 3ds
	cd $(UP) && ninja -f build_host_3ds.ninja $(NINJAFLAGS)
	@echo "Built $(UP)/build/host3ds_usa/acww.3dsx (contains game code: don't share it)"
	@echo "It needs game.pak beside it in sdmc:/3ds/acww/ (make pak ROM=..., or the web builder; see README.md)"

# The HOME Menu version. Nothing of the game is in it, so it needs no bootstrap: the forwarder (libctru only),
# the icon and the banner from cia/'s pictures, a moment of silence as the banner's sound.
DKP ?= $(or $(DEVKITPRO),/opt/devkitpro)
CIA := build/cia
CIA_TOOLS := build/cia-tools

cia:
	@bash tools/build_cia_tools.sh $(CIA_TOOLS)
	@mkdir -p $(CIA)
	$(DKP)/devkitARM/bin/arm-none-eabi-gcc -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft \
	    -mword-relocations -ffunction-sections -D__3DS__ -O2 -Wall -I$(DKP)/libctru/include -specs=3dsx.specs \
	    -Wl,--gc-sections -L$(DKP)/libctru/lib cia/forwarder.c -lctru -o $(CIA)/forwarder.elf
	$(DKP)/tools/bin/smdhtool --create "Animal Crossing: Wild World 3DS" "The 3DS port (HOME Menu version)" \
	    "acww-3ds" cia/icon.png $(CIA)/acww.smdh
	python3 -c "import wave; w = wave.open('$(CIA)/silence.wav', 'wb'); w.setnchannels(1); w.setsampwidth(2); w.setframerate(32000); w.writeframes(bytes(6400)); w.close()"
	$(CIA_TOOLS)/bin/bannertool makebanner -i cia/banner.png -a $(CIA)/silence.wav -o $(CIA)/acww.bnr
	$(CIA_TOOLS)/bin/makerom -f cia -o $(CIA)/acww.cia -target t -exefslogo -rsf cia/acww.rsf \
	    -elf $(CIA)/forwarder.elf -icon $(CIA)/acww.smdh -banner $(CIA)/acww.bnr
ifdef ROM
	node tools/cia_icon.mjs $(CIA)/acww.cia "$(ROM)"
	@echo "Built $(CIA)/acww.cia with the icon from your ROM (for your console only: do not pass it on)"
else
	@echo "Built $(CIA)/acww.cia (no game content in it; install it with FBI)"
endif

pc: check-bootstrap
	cd $(UP) && python3 tools/configure_host.py --cc gcc-14
	cd $(UP) && ninja -f build_host64.ninja $(NINJAFLAGS)
	@echo "Built $(UP)/build/host64_usa/acww (contains game code: don't share it)"
	@echo "Run it as: $(UP)/build/host64_usa/acww path/to/your/rom.nds"

# The data pack: your ROM's files without the game's code (docs/pak-format.md).
pak:
	@test -n "$(ROM)" || { echo "error: no ROM given: make pak ROM=path/to/rom.nds"; exit 1; }
	node tools/make_pak.mjs "$(ROM)" build/game.pak "$(PORT_VERSION)"
	@echo "Built build/game.pak: copy it to sdmc:/3ds/acww/ (game content: don't share it)"

# The game's own checks compare screenshots and sound against a run with a ROM
# (docs/host.md in the decompilation); they are not run here. The PC build is
# the build check.
test: check-bootstrap
	python3 -m unittest discover -s tools -p 'test_*.py'
	python3 tools/check_ci_artifacts.py
	@if command -v node >/dev/null; then node tools/test_web.mjs; else echo "(node not found: the web builder's tests left out)"; fi
	$(MAKE) --no-print-directory pc
	python3 tools/boot_smoke.py $(UP)/build/host64_usa/acww
	@echo "Tests passed. The PC build $(UP)/build/host64_usa/acww contains game code: don't share it"

check: check-bootstrap
	@python3 tools/check_run.py "$(ROM)"

check-baseline: check-bootstrap
	@python3 tools/check_run.py --baseline "$(ROM)"

# Removes build output only; build/upstream's sources and any edits in them stay
# (python tools/bootstrap.py --clean starts those over).
clean:
	rm -rf $(UP)/build $(UP)/build_host*.ninja $(CIA)

#!/usr/bin/env python3
"""Write build_host.ninja: the game compiled for 32-bit x86 Linux with the host runtime in modern/host/ (see
docs/host.md). Experimental; the DS hardware is being replaced piece by piece.

The game's sources are compiled freestanding, as for the DS (-nostdinc, mwcc's type rules), with -DMODERN -DHOST.
The runtime (modern/host/) uses the host's C library. No linker script supplies names: the labels symbols.txt
gives to places inside variables are pointers defined in C (include/host_labels.h, tools/host_labels.py), the second
names of functions are the functions themselves in the sources.

Needs a 32-bit sysroot (memory: acww-host-toolchain): --sysroot <dir>.
"""
import argparse
import glob
import os
import re
import shlex
import shutil
import subprocess
import sys
from pathlib import Path

import ninja_syntax

parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument("--sysroot", default=os.path.expanduser("~/acww/toolchains/i386-sysroot"))
parser.add_argument("--cc", default="gcc",
                    help="the C compiler: gcc, or clang or a path to one (a separate build: suffix _clang; when no "
                         "clang is on the PATH, the newest of ~/acww/toolchains/LLVM-*/bin and "
                         "~/acww/toolchains/llvm-mingw-*/bin, a full clang that also targets Linux)")
parser.add_argument("--oracle", action="store_true",
                    help="the oracle variant (a separate build: suffix _oracle): melonDS (GPL-3.0, --melonds) linked in "
                         "as a reference renderer for ACWW_RENDERER=melon, ACWW_RENDERER3D=melon, ACWW_RENDER_COMPARE "
                         "and the render self-tests. The other builds have no melonDS (modern/host/melon_none.c)")
parser.add_argument("--melonds", default=os.path.expanduser("~/acww/ref/melonDS"),
                    help="--oracle: melonDS source tree patched with tools/host/melonds.patch and built for the "
                         "target (build-core32, build-core, build-core-a64, build-core-win64)")
parser.add_argument("--wgpu", default=None,
                    help="wgpu-native (include/, lib/libwgpu_native.a) for the GPU renderer (ACWW_RENDERER3D=wgpu, the "
                         "default: modern/host/wgpu_display.c); default for x86-64 Linux: toolchains/wgpu-native-v29.0.1.1/"
                         "linux-x86_64 in the repository (tools/host_fetch_wgpu.py downloads it), else the same in "
                         "~/acww/toolchains, when it is there; 'none': without")
parser.add_argument("--sdl2", default=None,
                    help="a directory with SDL2 development headers (usr/include/SDL2) for the window (the 64-bit "
                         "builds' frontend, linked with the system's libSDL2) and the i386 build's viewer; default: "
                         "pkg-config's sdl2, /usr/include/SDL2, /usr/local/include/SDL2, ~/acww/toolchains/sdl2-dev")
parser.add_argument("--arch", choices=["i386", "x86_64", "aarch64"], default="x86_64",
                    help="x86_64 (default): build_host64.ninja, build/host64_<v>; i386: build_host.ninja, build/host_<v> "
                         "(32-bit, --sysroot); aarch64: build_host_a64.ninja, "
                         "build/hosta64_<v> (64-bit ARM Linux, cross-compiled with --cross; run under qemu-aarch64: "
                         "docs/host.md)")
parser.add_argument("--cross", default=os.path.expanduser(
                        "~/acww/toolchains/arm-gnu-toolchain-14.3.rel1-x86_64-aarch64-none-linux-gnu"),
                    help="--arch aarch64: the cross toolchain (bin/aarch64-none-linux-gnu-gcc, its glibc sysroot "
                         "built in)")
parser.add_argument("--qemu", default=os.path.expanduser("~/acww/toolchains/qemu/usr/bin/qemu-aarch64"),
                    help="--arch aarch64: qemu-aarch64 (user mode) for running the tests (the game: ACWW_RUNNER, "
                         "tools/host_oracle.sh)")
parser.add_argument("--os", choices=["linux", "windows", "macos", "3ds"], default="macos" if sys.platform == "darwin" else "linux",
                    help="windows (with --arch x86_64): a cross build for x86-64 Windows with llvm-mingw (clang, UCRT, "
                         "libc++, lld; --mingw): build_host_win64.ninja, build/hostwin64_<v>/acww.exe, headless only "
                         "(no frontend yet), run with Wine here (ACWW_RUNNER; docs/host.md); macos (the default on a "
                         "Mac): a native build for the Mac's own architecture with Apple's clang, SDL2 from pkg-config "
                         "(Homebrew): build_host64.ninja, build/host64_<v>; 3ds: the Nintendo 3DS with devkitPro's devkitARM and "
                         "libctru ($DEVKITPRO, default /opt/devkitpro): build_host_3ds.ninja, build/host3ds_<v>/acww.3dsx")
parser.add_argument("--mingw", default="",
                    help="the llvm-mingw directory for --os windows (default: the newest ~/acww/toolchains/llvm-mingw-*)")
parser.add_argument("--extra-cflags", default="",
                    help="more compiler flags for the game code (a debugging build, e.g. -fstack-protector-all; "
                         "use with --suffix)")
parser.add_argument("--suffix", default="",
                    help="a separate build: build_host64<suffix>.ninja, build/host64<suffix>_<v>")
parser.add_argument("--strict-c", action="store_true",
                    help="the game's C units with -Werror=pointer-to-int-cast -Werror=int-to-pointer-cast (and without "
                         "-w, which would hide them): every pointer kept in an integer is an error (docs/portability.md "
                         "0.3; meant for the x86-64 build, with --suffix)")
parser.add_argument("--guards", action="store_true",
                    help="the guard variant (a separate build: suffix _guards): poisoned bytes after every game variable "
                         "(tools/host_guards.py), checked every frame with ACWW_GUARDS=1 (modern/host/guards.c; "
                         "docs/portability.md 2.8)")
parser.add_argument("--sanitize", default="",
                    help="the sanitizer variant (x86-64 only; a separate build: suffix _asan): a comma list of "
                         "address (ASan: redzones around globals, stack and heap objects - catches reads too - and "
                         "the game's own heaps poisoned where unallocated, include/host_asan.h), bounds (array indexes "
                         "against the declared size, also arrays inside structs) and undefined (the rest of UBSan); "
                         "every report recovers (the run goes on), docs/host.md")
parser.add_argument("--direct", action="store_true",
                    help="--os 3ds: the enhanced port (a separate build: suffix _direct): "
                         "N3DS_DIRECT defined, the hardware model replaced where it has been")
parser.add_argument("--regen-gen", action="store_true",
                    help="regenerate modern/gen/gaps.c from the DS build's delinking (build/usa/delinks)")
parser.add_argument("version", nargs="?", default="usa")
args = parser.parse_args()

root = Path(__file__).resolve().parent.parent
WIN = args.os == "windows"
MAC = args.os == "macos"
N3DS = args.os == "3ds"
DKP = os.environ.get("DEVKITPRO", "/opt/devkitpro")
if N3DS:
    if args.guards or args.oracle or args.sanitize or args.cc != "gcc":
        sys.exit("--os 3ds: no --guards, --oracle, --sanitize or --cc (devkitARM's GCC)")
    DKA = os.environ.get("DEVKITARM", f"{DKP}/devkitARM")
    if not os.path.exists(f"{DKA}/bin/arm-none-eabi-gcc") or not os.path.exists(f"{DKP}/libctru/lib/libctru.a"):
        sys.exit(f"--os 3ds: no devkitARM and libctru in {DKP} (DEVKITPRO, DEVKITARM; `dkp-pacman -S 3ds-dev`)")
    args.cc = f"{DKA}/bin/arm-none-eabi-gcc"
    if args.direct and not args.suffix:
        args.suffix = "_direct"
elif args.direct:
    sys.exit("--direct: --os 3ds only")
if MAC:
    if args.guards or args.oracle:
        sys.exit("--os macos: no --guards or --oracle (GNU ld / melonDS builds)")
    # (Apple's gcc is clang; the build is native, for the machine's own architecture: arm64 or x86-64)
    if args.cc == "gcc":
        args.cc = "clang"
    args.arch = "x86_64" if os.uname().machine == "x86_64" else "aarch64"
if WIN:
    if args.arch != "x86_64":
        sys.exit("--os windows: x86_64 only")
    if args.guards or args.sanitize:
        sys.exit("--os windows: no --guards or --sanitize (GNU ld / Linux runtimes)")
    if not args.mingw:
        found = sorted(glob.glob(os.path.expanduser("~/acww/toolchains/llvm-mingw-*/bin/x86_64-w64-mingw32-clang")
                                     + (".exe" if os.name == "nt" else "")))
        if not found:
            sys.exit("configure_host.py: no llvm-mingw in ~/acww/toolchains (--mingw)")
        args.mingw = str(Path(found[-1]).parent.parent)
    args.cc = f"{args.mingw}/bin/x86_64-w64-mingw32-clang"
CLANG = os.path.basename(args.cc).startswith("clang") or WIN
if CLANG and not WIN and not MAC:
    if not args.suffix:
        args.suffix = "_clang"
    if not shutil.which(args.cc):
        found = sorted(glob.glob(os.path.expanduser(f"~/acww/toolchains/LLVM-*/bin/{args.cc}"))
                       + glob.glob(os.path.expanduser(f"~/acww/toolchains/llvm-mingw-*/bin/{args.cc}")))
        if not found:
            sys.exit(f"configure_host.py: no {args.cc} on the PATH or in ~/acww/toolchains")
        args.cc = found[-1]
if args.guards and not args.suffix:
    args.suffix = "_guards"
if args.oracle and not args.suffix:
    args.suffix = "_oracle"
A64 = args.arch == "aarch64"
if A64 and CLANG and not MAC:
    sys.exit("configure_host.py: --arch aarch64 builds with the GCC cross toolchain (--cross) only so far")
SANITIZE = [x for x in args.sanitize.split(",") if x]
if SANITIZE:
    if args.arch != "x86_64":
        sys.exit("--sanitize: x86-64 only (the i386 sysroot has no sanitizer runtimes)")
    if set(SANITIZE) - {"address", "bounds", "undefined"}:
        sys.exit("--sanitize: address, bounds, undefined")
    if args.guards:
        sys.exit("--sanitize and --guards don't combine")
    if not args.suffix:
        args.suffix = "_asan"
X64 = args.arch == "x86_64" and not N3DS
HOST64 = X64 or A64  # a 64-bit build (the build directory's and the ninja file's names apart)
CXX = "g++"
if MAC:
    CXX = "clang++"
    build = Path("build") / f"host64{args.suffix}_{args.version}"
    NINJA_FILE = f"build_host64{args.suffix}.ninja"
elif A64:
    # the cross compiler (C and C++; its sysroot built in)
    args.cc = f"{args.cross}/bin/aarch64-none-linux-gnu-gcc"
    CXX = f"{args.cross}/bin/aarch64-none-linux-gnu-g++"
    build = Path("build") / f"hosta64{args.suffix}_{args.version}"
    NINJA_FILE = f"build_host_a64{args.suffix}.ninja"
elif N3DS:
    CXX = f"{DKA}/bin/arm-none-eabi-g++"
    build = Path("build") / f"host3ds{args.suffix}_{args.version}"
    NINJA_FILE = f"build_host_3ds{args.suffix}.ninja"
elif WIN:
    # llvm-mingw's C++ compiler for the runtime's C++ units and the link (its libc++)
    CXX = f"{args.mingw}/bin/x86_64-w64-mingw32-clang++"
    build = Path("build") / f"hostwin64{args.suffix}_{args.version}"
    NINJA_FILE = f"build_host_win64{args.suffix}.ninja"
else:
    build = Path("build") / (f"host64{args.suffix}_{args.version}" if X64 else f"host{args.suffix}_{args.version}")
    NINJA_FILE = f"build_host64{args.suffix}.ninja" if X64 else f"build_host{args.suffix}.ninja"
R = args.sysroot
G = f"{R}/usr/lib/gcc/x86_64-linux-gnu/15/32"
# (the 3DS: devkitPro's flags for its ARM11; -mword-relocations: what 3dsxtool can relocate)
M_FLAG = "-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft -mword-relocations" if N3DS else \
    "" if MAC or A64 else "-m64" if X64 else "-m32"
# the compiler command; clang is told its target (llvm-mingw's clang is a cross compiler for Windows too; on Linux it
# finds the system's GCC installation for the host runtime's headers and libraries)
CC = f"{args.cc} --target={'x86_64' if X64 else 'i386'}-linux-gnu" if CLANG and not WIN and not MAC else args.cc
LD_EMU = "elf_x86_64" if X64 else "aarch64linux" if A64 else "elf_i386"

# the 64-bit builds: position-independent executables (loaded anywhere, ASLR; docs/portability.md 3.3); i386, the
# frozen reference, stays a fixed-address executable (its captures were taken so)
PIE = "" if WIN or N3DS else "-fPIE" if HOST64 else "-fno-pie"
GAME_FLAGS = " ".join([
    *([M_FLAG] if M_FLAG else []), *([PIE] if PIE else []), "-fno-short-enums", "-fsigned-char", "-fshort-wchar", "-ffreestanding", "-nostdinc",
    "-fno-strict-aliasing", "-fno-builtin", "-O1", "-g", "-w", "-fno-omit-frame-pointer", "-falign-functions=4",
    "-DMODERN", "-DHOST", f"-DGAME_VERSION_{args.version.upper()}", "-Iinclude",
    # the game's C library names renamed to ds_* (its own libc, apart from the host's)
    "-include", "include/host_libc_rename.h",
    # local variables without an initializer start as 0 instead of garbage left on the stack
    # (caused a crash on the 3DS in function "runChosenTopic")
    "-ftrivial-auto-var-init=zero",
])
if N3DS:
    # the game optimised further on the 3DS's slow CPU, without frame pointers (no backtraces there: plat_3ds.c)
    GAME_FLAGS = GAME_FLAGS.replace(" -O1 ", " -O2 ").replace(" -fno-omit-frame-pointer ", " -fomit-frame-pointer ")
if WIN:
    # GCC's bitfield layout, not Microsoft's: the save data's layout (its static_asserts) needs it
    GAME_FLAGS += " -mno-ms-bitfields"
if not HOST64 and not N3DS:
    # small structs returned in memory, as mwcc does: kept while the 32-bit build is the reference (its captures were
    # taken with it); the 64-bit builds use the platform ABI (clang supports the flag only on i386)
    GAME_FLAGS += " -fpcc-struct-return"
C_FLAGS = "-std=gnu11"
CXX_FLAGS = ("-std=gnu++14 -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit "
             "-fno-sized-deallocation")
# every unit with the full class definitions, in both builds: the layout tools (tools/host_view_layout.py) read the
# types from the 32-bit and the 64-bit debug information (debug information only: the code is the same)
CXX_FLAGS += " -femit-class-debug-always"
if args.guards:
    # a section per variable: tools/host_guards.py puts a guard after each
    GAME_FLAGS += " -fdata-sections"
if A64 or MAC:
    # no fused multiply-adds (GCC contracts a * b + c by default on arm64, x86-64 has no FMA without -mfma): the
    # same floating-point results as the x86 builds
    GAME_FLAGS += " -ffp-contract=off"
if CLANG:
    # GCC-only game flag: -fpcc-struct-return (clang: i386 only); clang's name for the class debug information
    if HOST64:
        GAME_FLAGS = GAME_FLAGS.replace(" -fpcc-struct-return", "")
    CXX_FLAGS = CXX_FLAGS.replace("-femit-class-debug-always", "-fstandalone-debug")
if MAC:
    # the base-object constructors and destructors (C2/D2) with the complete-object ones as their aliases, as on ELF:
    # Darwin's default emits only the variants a unit uses, and modern/gen/shared_table.c names the C2/D2 ones
    CXX_FLAGS += " -Xclang -mconstructor-aliases"
if args.extra_cflags:
    GAME_FLAGS += " " + args.extra_cflags
if args.direct:
    GAME_FLAGS += " -DN3DS_DIRECT"
# the sanitizer variant: the game, the runtime and the link with the same -fsanitize options; HOST_ASAN tells the
# runtime's stack switches (coro.c) and the game's heaps (include/host_asan.h) to annotate for ASan
SAN_FLAGS = ""
if SANITIZE:
    SAN_FLAGS = " -fsanitize=" + ",".join(SANITIZE) + " -fsanitize-recover=all"
    if "undefined" in SANITIZE:
        # not memory errors, and everywhere: members of 8-byte aligned types in the DS heaps' 4-byte aligned blocks
        # (x86 and arm64 don't mind), left shifts of negative values (the DS code's idiom). Shift counts past the width
        # stay checked: ARM gives 0 for them, x86 shifts by the count modulo the width.
        SAN_FLAGS += " -fno-sanitize=alignment,shift-base"
    if "address" in SANITIZE:
        SAN_FLAGS += " -DHOST_ASAN"
    GAME_FLAGS += SAN_FLAGS
# the NNS heap allocators (expanded and frame heap): they mark free memory as poisoned (include/host_asan.h) and walk
# their block headers in it, so they are compiled without ASan's checks
ASAN_UNCHECKED = {"unk_02100234", "unk_02100708", "unk_02100890", "unk_0210092c", "unk_02100a24", "unk_02100bb0",
                  "unk_02100d44", "unk_02101008", "unk_02101048",
                  # the C library's string functions (strlen, strcpy, strcmp...): they read aligned words, also past
                  # the terminator within the word, by design
                  "unk_0212a120"}
M = args.melonds
GUARD_FLAGS = " -DHOST_GUARDS" if args.guards else ""
# the frontend: SDL2 in the game's process (modern/host/frontend_sdl.c) when its headers are there (x86-64: --sdl2,
# linked with the system's libSDL2); otherwise none (modern/host/frontend_none.c: headless only)
def find_sdl2():
    """The -I directories for SDL2's headers (SDL.h's own directory first), or None."""
    def tree(d):  # a Debian-style tree: usr/include/SDL2, its configuration in the multiarch directory
        return [f"{d}/usr/include/SDL2", f"{d}/usr/include/x86_64-linux-gnu"] \
            if os.path.exists(f"{d}/usr/include/SDL2/SDL.h") else None
    if args.sdl2:
        return tree(args.sdl2)
    try:
        out = subprocess.run(["pkg-config", "--cflags-only-I", "sdl2"], capture_output=True, text=True, timeout=10)
        dirs = [f[2:] for f in out.stdout.split() if f.startswith("-I")]
        sdl = [d for d in dirs if os.path.exists(f"{d}/SDL.h")]
        if out.returncode == 0 and sdl:
            return sdl + [d for d in dirs if d not in sdl]
    except (OSError, subprocess.SubprocessError):
        pass
    for d in ("/usr/include/SDL2", "/usr/local/include/SDL2"):
        if os.path.exists(f"{d}/SDL.h"):
            return [d]
    return tree(os.path.expanduser("~/acww/toolchains/sdl2-dev"))
SDL2_DIRS = find_sdl2() if (X64 or MAC) and not WIN else None
SDL2 = bool(SDL2_DIRS)
FRONTEND_FLAG = " -DHOST_FRONTEND_3DS=1" if N3DS else " -DHOST_FRONTEND_SDL=1" if SDL2 else \
    " -DHOST_FRONTEND_NONE=1" if HOST64 else ""
# (SDL_syswm.h, for the window's native handles: modern/host/x11shim stands in for the X11 headers)
SDL_CFLAGS = " ".join(f"-I{d}" for d in SDL2_DIRS) + " -Imodern/host/x11shim" if SDL2 else ""
SDL_LIBS = "-l:libSDL2-2.0.so.0" if SDL2 else ""
if MAC and SDL2:
    SDL_LIBS = subprocess.run(["pkg-config", "--libs", "sdl2"], capture_output=True, text=True).stdout.strip() or "-lSDL2"
if MAC:
    # Apple's clang and ld64: a PIE (always, on macOS); a native build, no -m64. The default 8 MB stack is the main
    # thread's (the game's threads have stacks of their own, os.c).
    HOST_FLAGS = ("-O1 -g -fno-omit-frame-pointer -ffp-contract=off -Wall -std=gnu11 -DHOST -Iinclude" + FRONTEND_FLAG
                  + SAN_FLAGS)
    LINK_FLAGS = SAN_FLAGS.replace(" -DHOST_ASAN", "")
    HOST_CXX_FLAGS = "-O1 -g -ffp-contract=off -std=c++17 -DHOST" + SAN_FLAGS
    MELON_LIBS = ""
elif N3DS:
    # newlib and libctru (devkitPro); no POSIX signals for the debugging aids (debug_posix.c). The runtime shares
    # structures with the game, so it takes the game's 32-bit enums too (arm-none-eabi's default is the smallest
    # size), except the files that use libctru's structures (N3DS_NATIVE: the library's own default); the linker's
    # notes about the mix are expected
    # (-O2: the 3DS's CPU is slow, the runtime's loops are much of its time)
    HOST_FLAGS = (f"{M_FLAG} -O3 -g -fomit-frame-pointer -Wall -std=gnu11 -DHOST -Iinclude -D__3DS__ "
                  f"-I{DKP}/libctru/include -DHOST_DEBUG_POSIX=0 -fno-short-enums" + FRONTEND_FLAG)
    LINK_FLAGS = f"{M_FLAG} -specs=3dsx.specs -L{DKP}/libctru/lib -Wl,--no-enum-size-warning,--no-wchar-size-warning"
    HOST_CXX_FLAGS = f"{M_FLAG} -O1 -g -std=c++17 -DHOST"
    if args.direct:
        HOST_FLAGS += " -DN3DS_DIRECT"
        HOST_CXX_FLAGS += " -DN3DS_DIRECT"
    MELON_LIBS = ""
elif A64:
    # the cross toolchain's glibc; melonDS's core cross-built for aarch64 (build-core-a64: without the JIT and the GDB
    # stub, -ffp-contract=off; tools/host/aarch64-linux.toolchain.cmake, docs/host.md)
    HOST_FLAGS = ("-fPIE -O1 -g -fno-omit-frame-pointer -ffp-contract=off -Wall -std=gnu11 -DHOST -Iinclude"
                  + FRONTEND_FLAG + GUARD_FLAGS)
    LINK_FLAGS = "-pie"
    HOST_CXX_FLAGS = f"-fPIE -O1 -g -ffp-contract=off -std=c++17 -I{M}/src -I{M}/build-core-a64/src -DHOST"
    MELON_LIBS = f"{M}/build-core-a64/src/libcore.a {M}/build-core-a64/src/teakra/src/libteakra.a"
elif WIN:
    # llvm-mingw's own libraries (UCRT, libc++, winpthreads), linked statically: the program needs only system DLLs.
    # melonDS's core cross-built for Windows in build-core-win64 (tools/host/win64-mingw.toolchain.cmake; no JIT, no
    # GDB stub). The runtime's C units share the game's structures, so they take -mno-ms-bitfields too; the C++ units
    # include melonDS's headers and keep the platform layout of its library.
    HOST_FLAGS = ("-m64 -O1 -g -fno-omit-frame-pointer -Wall -std=gnu11 -DHOST -Iinclude -mno-ms-bitfields"
                  + FRONTEND_FLAG)
    # (an 8 MB stack for the process's own thread: the game's threads have stacks of their own, os.c)
    LINK_FLAGS = "-m64 -static -Wl,--stack,8388608"
    HOST_CXX_FLAGS = f"-m64 -O1 -g -std=c++17 -I{M}/src -I{M}/build-core-win64/src -DHOST"
    MELON_LIBS = f"{M}/build-core-win64/src/libcore.a {M}/build-core-win64/src/teakra/src/libteakra.a"
elif X64:
    # the system's own 64-bit libraries; melonDS's 64-bit core (build-core, configured with the JIT and the GDB stub)
    HOST_FLAGS = ("-m64 -fPIE -O1 -g -fno-omit-frame-pointer -Wall -std=gnu11 -DHOST -Iinclude" + FRONTEND_FLAG
                  + GUARD_FLAGS + SAN_FLAGS)
    LINK_FLAGS = "-m64 -pie" + SAN_FLAGS.replace(" -DHOST_ASAN", "")
    HOST_CXX_FLAGS = (f"-m64 -fPIE -O1 -g -std=c++17 -I{M}/src -I{M}/build-core/src -DJIT_ENABLED -DGDBSTUB_ENABLED "
                      "-DHOST" + SAN_FLAGS)
    MELON_LIBS = f"{M}/build-core/src/libcore.a {M}/build-core/src/teakra/src/libteakra.a"
else:
    HOST_FLAGS = (f"-m32 -fno-pie -O1 -g -fno-omit-frame-pointer -Wall -std=gnu11 --sysroot={R} -isystem {R}/usr/include "
                  f"-isystem /usr/include/x86_64-linux-gnu -isystem /usr/include -DHOST -Iinclude") + GUARD_FLAGS
    LINK_FLAGS = (f"-m32 -no-pie --sysroot={R} -B{G} -L{G} -Wl,-dynamic-linker,{R}/usr/lib32/ld-linux.so.2 "
                  f"-Wl,--disable-new-dtags -Wl,-rpath,{R}/usr/lib32")
    HOST_CXX_FLAGS = (f"-m32 -fno-pie -O1 -g -std=c++17 --sysroot={R} -isystem {R}/usr/include/x86_64-linux-gnu/c++/15/32 "
                      f"-idirafter {R}/usr/include -idirafter /usr/include/x86_64-linux-gnu -idirafter /usr/include "
                      f"-I{M}/src -I{M}/build-core32/src -DHOST")
    MELON_LIBS = f"{M}/build-core32/src/libcore.a {M}/build-core32/src/teakra/src/libteakra.a"
    # the frontend: the 64-bit viewer process (modern/host/frontend_viewer.c, viewer/)
if not args.oracle:  # (no melonDS: its headers and libraries aren't needed)
    MELON_LIBS = ""
    HOST_CXX_FLAGS = re.sub(f" -I{re.escape(M)}/\\S+| -DJIT_ENABLED| -DGDBSTUB_ENABLED", "", HOST_CXX_FLAGS)

# WebGPU (wgpu-native, linked statically): ACWW_RENDERER3D=wgpu
WGPU = args.wgpu
if WGPU is None:
    WGPU = "none"
    if (X64 or MAC) and not WIN:  # (the repository's toolchains/, then ~/acww/toolchains)
        # (tools/host_fetch_wgpu.py's directory names: the platform and machine as Python names them)
        plat = f"darwin-{os.uname().machine}" if MAC else "linux-x86_64"
        for d in (str(root / "toolchains"), os.path.expanduser("~/acww/toolchains")):
            if os.path.exists(f"{d}/wgpu-native-v29.0.1.1/{plat}/lib/libwgpu_native.a"):
                WGPU = f"{d}/wgpu-native-v29.0.1.1/{plat}"
                break
if WGPU == "none" or not os.path.exists(f"{WGPU}/lib/libwgpu_native.a"):
    if args.wgpu not in (None, "none"):
        sys.exit(f"--wgpu: no {args.wgpu}/lib/libwgpu_native.a")
    WGPU = None
# (quoted: the repository's own path, where host_fetch_wgpu.py puts it, may have spaces)
WGPU_CFLAGS = f"-O2 -DHOST_WGPU -I{shlex.quote(WGPU + '/include')}" if WGPU else "-O2"
WGPU_LIBS = f" {shlex.quote(WGPU + '/lib/libwgpu_native.a')} -ldl" if WGPU else ""
if WGPU and MAC:
    # (wgpu-native's Metal backend: the system frameworks it calls)
    WGPU_LIBS = (f" {shlex.quote(WGPU + '/lib/libwgpu_native.a')} -framework Metal -framework QuartzCore -framework CoreGraphics"
                 " -framework Foundation -framework IOKit -lobjc")

# the 3DS runtime's files that use libctru's structures (built with its enum size: HOST_FLAGS)
N3DS_NATIVE = ("plat_3ds.c", "frontend_3ds.c", "gpu3d_pica.c", "pica_display.c")

MWCC_FLAG_MAP = {("-char", "unsigned"): "-funsigned-char"}


def source_flags(source: Path) -> list[str]:
    with open(source, encoding="utf-8", errors="replace") as f:
        for _, line in zip(range(10), f):
            m = re.match(r"\s*//\s*mwcc-flags:\s*(.+?)\s*$", line)
            if m:
                words = m.group(1).split()
                return [g for k, g in MWCC_FLAG_MAP.items()
                        if any(tuple(words[i:i + len(k)]) == k for i in range(len(words)))]
    return []


def sources(base: Path, suffixes):
    for r, _, files in os.walk(base):
        for f in sorted(files):
            p = Path(r) / f
            if p.suffix in suffixes:
                yield p


def main():
    os.chdir(root)
    with open(NINJA_FILE, "w") as file:
        n = ninja_syntax.Writer(file)
        cc_flags = f"{GAME_FLAGS} {C_FLAGS}"
        if args.strict_c:
            cc_flags = cc_flags.replace(" -w ", " ") + " -Werror=pointer-to-int-cast -Werror=int-to-pointer-cast"
        n.rule("cc", f"{CC} {cc_flags} $flags -MMD -MF $out.d -c $in -o $out",
               depfile="$out.d", deps="gcc", description="CC $in")
        n.rule("cxx", f"{CC} -x c++ {GAME_FLAGS} {CXX_FLAGS} $flags -MMD -MF $out.d -c $in -o $out",
               depfile="$out.d", deps="gcc", description="CXX $in")
        n.rule("hostcc", f"{CC} {HOST_FLAGS} $flags -MMD -MF $out.d -c $in -o $out", depfile="$out.d", deps="gcc",
               description="HOSTCC $in")
        n.rule("hostas_S", " ".join(x for x in (CC, M_FLAG, PIE) if x) + " -c $in -o $out", description="AS $in")
        n.rule("hostcxx", f"{CXX} {HOST_CXX_FLAGS} -MMD -MF $out.d -c $in -o $out", depfile="$out.d", deps="gcc",
               description="HOSTCXX $in")
        libs = " -lws2_32" if WIN else f" {SDL_LIBS} -lc++ -lpthread -lm" if MAC else " -lcitro3d -lctru -lm" if N3DS else \
            f" {SDL_LIBS} -lstdc++ -lpthread -lm -lc -lgcc"  # (ws2_32: wm7.c's sockets)
        n.rule("link", f"{CXX} {LINK_FLAGS} -o $out @$objects {MELON_LIBS}{WGPU_LIBS}{libs}", description="LINK $out")
        objs = []
        for s in sources(Path("src"), (".c", ".cpp")):
            o = str(build / s.with_suffix(".o"))
            fl = source_flags(s)
            if "address" in SANITIZE and s.stem in ASAN_UNCHECKED:
                fl.append("-fno-sanitize=address")
            n.build(o, "cxx" if s.suffix == ".cpp" else "cc", str(s), variables={"flags": " ".join(fl)} if fl else None)
            objs.append(o)
        for s in sources(Path("modern/host"), (".c", ".cpp", ".S")):
            if "viewer" in s.parts or "tests" in s.parts:
                continue
            if WIN and s.name in ("frontend_sdl.c", "frontend_ui.c", "frontend_viewer.c", "plat_posix.c"):
                continue  # (debug_posix.c and guards.c are empty there: HOST_DEBUG_POSIX 0, no HOST_GUARDS)
            if N3DS and s.name in ("frontend_sdl.c", "frontend_ui.c", "frontend_viewer.c", "plat_posix.c",
                                   "plat_win32.c"):
                continue
            if not N3DS and s.name in ("plat_3ds.c", "frontend_3ds.c"):
                continue
            if s.name in (("melon_none.c",) if args.oracle else ("melon.cpp", "melon_platform.cpp")):
                continue  # (melonDS, or the stand-in that refuses it: --oracle)
            o = str(build / s.with_suffix(".o"))
            rule = {".cpp": "hostcxx", ".S": "hostas_S"}.get(s.suffix, "hostcc")
            sdl = s.name in ("frontend_sdl.c", "frontend_ui.c") and SDL_CFLAGS
            implicit = []
            if s.name == "wgpu_display.c":  # (the 2D renderer's shader, embedded: modern/host/gpu2d.wgsl)
                wgsl_h = str(build / "gen" / "gpu2d_wgsl.h")
                n.rule("embed", "python3 tools/host_embed.py $in $out $name", description="EMBED $out")
                n.build(wgsl_h, "embed", "modern/host/gpu2d.wgsl", implicit=["tools/host_embed.py"],
                        variables={"name": "kShader2d"})
                implicit = [wgsl_h]
            # the 2D and 3D renderers' per-pixel loops: optimised further than the rest of the runtime (-O1)
            fl = "-fshort-enums" if N3DS and s.name in N3DS_NATIVE else SDL_CFLAGS if sdl else WGPU_CFLAGS + f" -I{build}/gen" if s.name == "wgpu_display.c" else \
                WGPU_CFLAGS if s.name == "gpu3d_wgpu.c" else \
                "-O3" if N3DS and s.name == "gpu3d_geom.c" else \
                "-O2" if "gpu2d" in s.parts or "gpu3d" in s.parts else None
            n.build(o, rule, str(s), variables={"flags": fl} if fl else None, implicit=implicit)
            objs.append(o)
        if N3DS:
            # the vertex shaders (gpu3d_pica.c, pica_display.c): assembled by picasso, embedded by bin2s (<name>_shbin,
            # <name>_shbin_size)
            n.rule("picasso", f"{DKP}/tools/bin/picasso -o $out $in", description="PICASSO $out")
            n.rule("bin2s", f"{DKP}/tools/bin/bin2s -a 4 $in > $out", description="BIN2S $out")
            for s in sources(Path("modern/host"), (".pica",)):
                shbin = str(build / "gen" / s.name.replace(".v.pica", ".shbin"))
                n.build(shbin, "picasso", str(s))
                n.build(shbin + ".s", "bin2s", shbin)
                n.build(shbin + ".o", "hostas_S", shbin + ".s")
                objs.append(shbin + ".o")
        # The data between the decomp's units (gap objects of the DS build's delinking): generated once from that
        # build, which this branch no longer runs, and kept in modern/gen/. --regen-gen rebuilds it from
        # build/usa/delinks.
        n.rule("gaps", "python3 tools/host_gaps.py $in $out", description="GAPS $out")
        gen_gaps = "modern/gen/gaps.c"  # typed C, the same for every architecture
        if args.regen_gen:
            n.build(gen_gaps, "gaps", "tools/host_objects_aliases.txt", implicit=["tools/host_gaps.py"])
        gaps_c = gen_gaps
        n.build(str(build / "gaps.o"), "hostcc", gaps_c)
        objs.append(str(build / "gaps.o"))
        # game-side runtime pieces: the loaded overlays and the dispatchers of calls to addresses several overlays share
        # (generated C, committed: tools/host_shared_calls.py)
        for s in list(sources(Path("modern/ds"), (".c",))) + [Path("modern/gen/shared_calls.c"),
                                                                Path("modern/gen/shared_table.c")]:
            o = str(build / s.with_suffix(".o"))
            n.build(o, "cc", str(s))
            objs.append(o)
        # The game (and its own C library): ordinary objects, its variables in the program's .data and .bss
        # (docs/portability.md 3.3), between the variables of modern/link/game_begin.c and game_end.c, linked first
        # and last of the game's objects (linkers keep the input order within .data and .bss): the range the debug
        # dumps and HostMem_IsGameRam use (include/host_mem.h)
        marks = []
        for s in (Path("modern/link/game_begin.c"), Path("modern/link/game_end.c")):
            marks.append(str(build / s.with_suffix(".o")))
            n.build(marks[-1], "cc", str(s))
        # (forward slashes, on Windows too: the tests below, the object lists' readers, and the linker, which reads
        # @objects.txt with GNU quoting, where a backslash escapes)
        objs, marks = [Path(o).as_posix() for o in objs], [Path(o).as_posix() for o in marks]
        host_objs = [o for o in objs if "/modern/host/" in o or o.endswith("/gaps.o")]
        game_objs = [marks[0]] + [o for o in objs if o not in host_objs] + [marks[1]]
        # (DTCM's variables, src/dtcm, are ordinary game variables; DTCM itself is a host block: include/host_mem.h)
        build.mkdir(parents=True, exist_ok=True)
        game_list = build / "game_objects.txt"  # (also for tools/host_shared_calls.py)
        game_list.write_text("".join(f"{o}\n" for o in game_objs))
        # each overlay's units, their variables and static initialisers, for modern/host/overlay.c (generated C,
        # committed: tools/host_overlay_vars.py)
        n.build(str(build / "overlay_table.o"), "hostcc", "modern/gen/overlay_table.c",
                variables={"flags": "-Imodern/host"})
        host_objs.append(str(build / "overlay_table.o"))
        if args.guards:
            # the guard variant (a debugging build; GNU ld): the game linked into one object first (its data,
            # read-only data and bss as the sections gdata, .rodata and gbss), whose map tools/host_guards.py reads,
            # then the same layout again with a guard after every variable
            game_ld = str(build / "game.ld")
            Path(game_ld).write_text("SECTIONS {\n    gdata : { *(.data .data.*) }\n"
                                     "    .rodata : { *(.rodata .rodata.*) }\n"
                                     "    gbss : { *(.bss .bss.* COMMON) }\n}\n")
            n.rule("partial", f"ld -m {LD_EMU} -r -T $order -o $out @$list", description="LD -r $out")
            n.rule("partial_map", f"ld -m {LD_EMU} -r -T $order -Map=$map -o $out @$list", description="LD -r $out")
            n.build([str(build / "game_plain.o"), str(build / "game_plain.map")], "partial_map", game_objs,
                    implicit=[str(game_list), game_ld],
                    variables={"list": str(game_list), "order": game_ld, "map": str(build / "game_plain.map")})
            n.rule("guards", "python3 tools/host_guards.py $in $out", description="GUARDS $out")
            guard_ld, guard_c = str(build / "guards.ld"), str(build / "guard_table.c")
            n.build([guard_ld, guard_c], "guards", str(build / "game_plain.map"), implicit=["tools/host_guards.py"])
            n.build(str(build / "game_all.o"), "partial", game_objs, implicit=[str(game_list), guard_ld],
                    variables={"list": str(game_list), "order": guard_ld})
            n.build(str(build / "guard_table.o"), "hostcc", guard_c, variables={"flags": "-Imodern/host"})
            host_objs.append(str(build / "guard_table.o"))
            link_objs = [str(build / "game_all.o")] + host_objs
        else:
            link_objs = game_objs + host_objs
        # (the game's C library names don't clash with the host's: include/host_libc_rename.h)
        objects_file = build / "objects.txt"
        objects_file.write_text("".join(f"{Path(o).as_posix()}\n" for o in link_objs))
        exe = str(build / ("acww.exe" if WIN else "acww.elf" if N3DS else "acww"))
        n.build(exe, "link", link_objs, implicit=[str(objects_file)], variables={"objects": str(objects_file)})
        if N3DS:
            # the homebrew launcher's program: the .3dsx with its title and icon (the .smdh), as $DEVKITARM/3ds_rules
            smdh, tdsx = str(build / "acww.smdh"), str(build / "acww.3dsx")
            n.rule("smdh", f'{DKP}/tools/bin/smdhtool --create "Animal Crossing WW" "Native port" '
                           f'"acww-decomp" $in $out', description="SMDH $out")
            n.build(smdh, "smdh", f"{DKP}/libctru/default_icon.png")
            # (and the relocations checked: tools/host_3dsx_check.py)
            n.rule("3dsx", f"{DKP}/tools/bin/3dsxtool $in $out --smdh=$smdh && python3 tools/host_3dsx_check.py $out "
                           "|| (rm -f $out; exit 1)", description="3DSX $out")
            n.build(tdsx, "3dsx", exe, implicit=[smdh, "tools/host_3dsx_check.py"], variables={"smdh": smdh})
        # the viewer: a 64-bit SDL2 program (SDL2's headers as for the frontend: <SDL2/SDL.h>, so their parent
        # directory; the system's libSDL2 at run time)
        vdirs = find_sdl2() or []
        vflags = " ".join(f"-I{os.path.dirname(d) if os.path.exists(d + '/SDL.h') else d}" for d in vdirs)
        n.rule("viewer", f"gcc -O2 {vflags} $in -o $out -l:libSDL2-2.0.so.0", description="VIEWER $out")
        if N3DS:
            n.default([str(build / "acww.3dsx")])
        elif WIN or MAC:
            n.default([exe])
        elif A64:
            n.default([exe])
            # the arm64 stack switch (coro_switch.S): `ninja -f build_host_a64.ninja coro-a64-test`, run under --qemu
            # (modern/host/tests/coro_a64_test.c)
            test = str(build / "coro_a64_test")
            n.rule("coro_test", f"{args.cc} -O1 -g -fno-omit-frame-pointer -Wall -Imodern/host $in -o $out",
                   description="CC $out")
            n.build(test, "coro_test", ["modern/host/tests/coro_a64_test.c", "modern/host/coro.c",
                                         "modern/host/coro_switch.S"], implicit=["modern/host/coro.h"])
            n.rule("run", f"{args.qemu} -L {args.cross}/aarch64-none-linux-gnu/libc $in", description="RUN $in")
            n.build("coro-a64-test", "run", test)
        elif X64:
            n.default([exe])
            # the Windows x64 stack switch (coro_switch.S), tested here with GCC's ms_abi:
            # `ninja -f build_host64.ninja coro-win64-test` (modern/host/tests/coro_win64_test.c)
            test = str(build / "coro_win64_test")
            n.rule("coro_test", "gcc -O0 -g -fno-omit-frame-pointer -Wall -DHOST_CORO_WIN64_TEST -Imodern/host $in -o $out",
                   description="CC $out")
            n.build(test, "coro_test", ["modern/host/tests/coro_win64_test.c", "modern/host/coro.c",
                                         "modern/host/coro_switch.S"], implicit=["modern/host/coro.h"])
            n.rule("run", "$in", description="RUN $in")
            n.build("coro-win64-test", "run", test)
        else:
            viewer = str(build / "acww-viewer")
            n.build(viewer, "viewer", "modern/host/viewer/viewer.c", implicit=["modern/host/frontend.h"])
            n.default([exe, viewer])


def summary():
    """What the build has, and what to do about what it hasn't."""
    print(f"configure_host.py: {NINJA_FILE}: build with `ninja -f {NINJA_FILE}`, the game: {build}/"
          f"{'acww.exe' if WIN else 'acww.3dsx' if N3DS else 'acww'}")
    if (HOST64 or MAC) and not WIN:
        print("  window: " + ("SDL2" if SDL2 else
              "none, headless only - install SDL2's development files (e.g. libsdl2-dev) or give --sdl2"))
        print("  GPU renderer: " + ("wgpu-native" if WGPU else
              "none, the CPU renderers - `python3 tools/host_fetch_wgpu.py` downloads wgpu-native"))


if __name__ == "__main__":
    main()
    summary()

#!/usr/bin/env python3
"""Checks a .3dsx's relocations (the 3DS build): every relocated word must name a place in the program.

The format keeps a relocation's target as an offset into the program, with the top 4 bits a sub-type (absolute: 0;
relative: 0 = 32-bit, 1 = 31-bit), so it can't express an address outside the program - such as the constant
`array - 0x04000000` the compiler folds `array + (addr - 0x04000000)` into. The loaders refuse such a program (Azahar:
"A generic load error"). Exit status 1, with the words listed, if there are any.

    python3 tools/host_3dsx_check.py build/host3ds_usa/acww.3dsx
"""
import struct
import sys


def bad_relocations(d):
    """(segment, word offset, value) of each relocated word the loaders refuse."""
    hs, rhs, _, _, code, ro, data, bss = struct.unpack_from("<HHIIIIII", d, 4)
    tables = rhs // 4
    pos = hs
    counts = [struct.unpack_from(f"<{tables}I", d, pos + i * rhs) for i in range(3)]
    pos += 3 * rhs
    sizes = [code, ro, data - bss]
    segs = []
    for size in sizes:
        segs.append(d[pos:pos + size])
        pos += size
    bad = []
    for s in range(3):
        for t in range(tables):
            word = 0
            for _ in range(counts[s][t]):
                skip, patch = struct.unpack_from("<HH", d, pos)
                pos += 4
                word += skip
                for _ in range(patch):
                    if t < 2 and word * 4 + 4 <= len(segs[s]):
                        v = struct.unpack_from("<I", segs[s], word * 4)[0]
                        if v >> 28 > t:  # (absolute: sub-type 0 only; relative: 0 or 1)
                            bad.append((s, word * 4, v))
                    word += 1
    return bad


def main():
    with open(sys.argv[1], "rb") as f:
        d = f.read()
    assert d[:4] == b"3DSX", f"{sys.argv[1]}: not a .3dsx"
    bad = bad_relocations(d)
    for s, off, v in bad:
        print(f"{sys.argv[1]}: segment {s} (code, rodata, data) offset {off:#x}: relocated word {v:#010x} is outside "
              "the program (an address the compiler folded: see modern/host/hw.c, opaque)")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())

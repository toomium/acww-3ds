# The data pack (`game.pak`), version 1

`game.pak` is one file for the SD card (`sdmc:/3ds/acww/game.pak`): **the player's own ROM's files, without the
game's code**. It is made from a ROM dump by the web builder (`web/js/pak.mjs`) or by `make pak ROM=...`, and read
by the port in place of the ROM (`3ds_port/modern/host/card_pak.h`). It is game content: never commit it or pass it
on (`*.pak` is in `.gitignore`). The layout follows GabeSud0/pokeplatinum-3ds's data pack.

## Layout

All numbers are unsigned and little-endian; offsets count from the start of the file.

| Offset | Size | Meaning |
|---:|---:|---|
| `0x00` | 8 | `ACWWPAK` and a zero byte |
| `0x08` | 4 | the format's version: **1** |
| `0x0C` | 32 | the port's version the pack was made for (text, zero-padded) |
| `0x2C` | 20 | the source ROM's SHA-1 (the 20 bytes, not text) |
| `0x40` | 4 | the number of entries `N` |
| `0x44` | `12 × N` | the index, sorted by number |
| after it | | the entries' bytes, in the index's order, each starting at a multiple of 4 (zero bytes between) |

An index entry is 12 bytes: the entry's number, its offset, its size.

## The entries

Numbers below `0x10000000` are the ROM's files, by their number in the ROM's file table. The 148 files that are the
game's code (the overlays) are left out, and so are the two programs (ARM9, ARM7), the banner and the icon.

| Number | What |
|---|---|
| `0x10000000` | a card header (0x200 bytes): the ROM's first 0x20 bytes (the game's name and code), and at 0x40, 0x48 and 0x50 the offsets and sizes of the next three entries **in the pack** |
| `0x10000001` | the ROM's file name table, unchanged |
| `0x10000002` | the file table as the game reads it: for each file number, the offsets in the pack of its first byte and of the byte after its last (zero for a file left out) |
| `0x10000003` | the ROM's list of overlays, unchanged |

The game finds its files by name, through the name table and the file table, and reads them from the card by
offset. As both tables' offsets are the pack's, the port reads the pack as it reads a ROM image; only the header
the game is shown comes from entry `0x10000000`.

## What the port checks

The first 8 bytes (anything else is read as a ROM image), the version (another one: "make it again"), that the
index and the header entry lie inside the file. The ROM's SHA-1 is checked where the pack is made: the web builder
makes one only from a dump it knows (`web/js/rom.mjs`). The version text is for the log; a pack made for an
older version of the port stays good as long as the format's version is the same.

## Other ROMs

Only the USA ROM (Rev 1) is supported. Another dump is added by its SHA-1 once the game is tested with a pack made
from it: what differs (other file names, another language's files in the place of the English ones) is the
builder's to put right, so that the port only ever sees a pack.

// The data pack (game.pak): the ROM's files without the game's code, in the layout docs/pak-format.md describes.
// Made here, in the browser (or by tools/make_pak.mjs), from the visitor's own ROM; the port reads it in place of
// the ROM (modern/host/card_pak.h).

export const PAK_VERSION = 1;
// The entries that are no file of the ROM's (those are numbered from 0): what the game's file system starts from.
export const PAK_HEADER = 0x10000000, PAK_NAMES = 0x10000001, PAK_TABLE = 0x10000002, PAK_OVERLAYS = 0x10000003;

const align4 = (n) => (n + 3) & ~3;

// rom: the ROM image; sha1: its SHA-1 as 20 bytes; buildId: the port's version, 1 to 32 bytes of text
export function makePak(rom, sha1, buildId) {
  const bad = () => new Error("This is not a DS ROM image.");
  if (rom.length < 0x200) throw bad();
  const v = new DataView(rom.buffer, rom.byteOffset, rom.byteLength);
  const part = (at) => {  // an offset and a size in the ROM's header
    const start = v.getUint32(at, true), size = v.getUint32(at + 4, true);
    if (start + size > rom.length) throw bad();
    return rom.subarray(start, start + size);
  };
  const names = part(0x40), table = part(0x48), overlays = part(0x50);
  const tv = new DataView(table.buffer, table.byteOffset, table.byteLength);
  const ov = new DataView(overlays.buffer, overlays.byteOffset, overlays.byteLength);
  const count = table.length >> 3;
  // the overlays' files are the game's code: left out
  const code = new Set();
  for (let at = 0; at + 32 <= overlays.length; at += 32) code.add(ov.getUint32(at + 24, true));

  const entries = [];  // [id, data]
  for (let id = 0; id < count; id++) {
    const start = tv.getUint32(id * 8, true), end = tv.getUint32(id * 8 + 4, true);
    if (end < start || end > rom.length) throw bad();
    if (!code.has(id)) entries.push([id, rom.subarray(start, end)]);
  }
  // the header: the game's name and code, and where the three tables are in the pack (written below)
  const header = new Uint8Array(0x200), hv = new DataView(header.buffer);
  header.set(rom.subarray(0, 0x20));
  const newTable = new Uint8Array(count * 8), nv = new DataView(newTable.buffer);
  entries.push([PAK_HEADER, header], [PAK_NAMES, names], [PAK_TABLE, newTable], [PAK_OVERLAYS, overlays]);

  let at = align4(0x44 + 12 * entries.length);
  const offsets = entries.map(([, data]) => {
    const here = at;
    at = align4(at + data.length);
    return here;
  });
  if (at > 0xffffffff) throw bad();
  // the file table as the game reads it: each file's first byte and the one after its last, in the pack
  entries.forEach(([id, data], i) => {
    if (id < count) nv.setUint32(id * 8, offsets[i], true), nv.setUint32(id * 8 + 4, offsets[i] + data.length, true);
  });
  [0x40, 0x48, 0x50].forEach((field, i) => {
    hv.setUint32(field, offsets[entries.length - 3 + i], true);
    hv.setUint32(field + 4, entries[entries.length - 3 + i][1].length, true);
  });

  const id = new TextEncoder().encode(buildId);
  if (!id.length || id.length > 32 || id.includes(0) || sha1.length !== 20) throw new Error("Bad build ID or SHA-1.");
  const out = new Uint8Array(at), o = new DataView(out.buffer);
  out.set(new TextEncoder().encode("ACWWPAK"));
  o.setUint32(0x08, PAK_VERSION, true);
  out.set(id, 0x0c);
  out.set(sha1, 0x2c);
  o.setUint32(0x40, entries.length, true);
  entries.forEach(([entry, data], i) => {
    o.setUint32(0x44 + i * 12, entry, true);
    o.setUint32(0x48 + i * 12, offsets[i], true);
    o.setUint32(0x4c + i * 12, data.length, true);
    out.set(data, offsets[i]);
  });
  return out;
}

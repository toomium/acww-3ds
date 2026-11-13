// The game's own icon on the HOME Menu version: the .cia's placeholder icon replaced by the one in the visitor's
// ROM. The release's .cia has no game content; the icon gets in here, in the browser (or in `make cia ROM=...`).
import { subtle } from "./rom.mjs";

const SMALL = 0x480, LARGE = 0x1200;  // an SMDH's 24x24 and 48x48 pictures, at 0x2040 and 0x24c0
const SMDH_ICONS = 0x2040;

// The ROM's 32x32 icon (16 colours, colour 0 transparent: white here) as an SMDH's two pictures, one after the
// other: RGB565, in 8x8 tiles whose pixels are in Z order. Scaled by the nearest pixel.
export function smdhIcons(rom) {
  const v = new DataView(rom.buffer, rom.byteOffset, rom.byteLength);
  const banner = rom.length >= 0x6c ? v.getUint32(0x68, true) : 0;
  if (!banner || banner + 0x240 > rom.length) throw new Error("The ROM has no icon.");
  const pixel = (x, y) => {
    const tile = (y >> 3) * 4 + (x >> 3);
    const c = (rom[banner + 0x20 + tile * 32 + (y & 7) * 4 + ((x & 7) >> 1)] >> ((x & 1) * 4)) & 15;
    if (!c) return 0xffff;
    const p = v.getUint16(banner + 0x220 + c * 2, true), g = (p >> 5) & 31;  // the DS's colour: 5 bits each, red lowest
    return ((p & 31) << 11) | (((g << 1) | (g >> 4)) << 5) | ((p >> 10) & 31);
  };
  const out = new Uint8Array(SMALL + LARGE), o = new DataView(out.buffer);
  let at = 0;
  for (const size of [24, 48])
    for (let ty = 0; ty < size; ty += 8)
      for (let tx = 0; tx < size; tx += 8)
        for (let m = 0; m < 64; m++, at += 2) {
          let x = 0, y = 0;
          for (let b = 0; b < 3; b++) {
            x |= ((m >> (2 * b)) & 1) << b;
            y |= ((m >> (2 * b + 1)) & 1) << b;
          }
          o.setUint16(at, pixel(Math.floor((tx + x) * 32 / size), Math.floor((ty + y) * 32 / size)), true);
        }
  return out;
}

// A copy of this .cia with the ROM's icon. The icon is a file in the program's ExeFS, and five SHA-256 hashes lead
// from it up to the title's description (TMD); each is checked before the change (so a .cia laid out another way,
// or an encrypted one, is refused, not broken) and written again after it. The signatures are not made again:
// a homebrew .cia's are not valid on a console anyway, custom firmware does not check them.
export async function ciaWithIcon(cia, rom) {
  const icons = smdhIcons(rom);
  const out = cia.slice(), v = new DataView(out.buffer);
  const bad = () => new Error("This .cia is not laid out as expected.");
  const align = (n) => (n + 63) & ~63;
  const text = (at) => String.fromCharCode(...out.subarray(at, at + 4));
  if (out.length < 0x2040) throw bad();
  const tmd = align(align(align(v.getUint32(0, true)) + v.getUint32(8, true)) + v.getUint32(12, true));
  const content = align(tmd + v.getUint32(16, true));
  const meta = align(content + Number(v.getBigUint64(24, true))), metaSize = v.getUint32(20, true);
  if (content + 0x200 > out.length || v.getUint32(tmd) !== 0x10004 || text(content + 0x100) !== "NCCH") throw bad();

  const exefs = content + v.getUint32(content + 0x1a0, true) * 0x200;
  if (exefs + 0x200 > out.length) throw bad();
  let file = 0;
  while (file < 10 && text(exefs + file * 16) !== "icon") file++;
  if (file === 10) throw bad();
  const icon = exefs + 0x200 + v.getUint32(exefs + file * 16 + 8, true);
  const chunks = tmd + 0xb04, infos = tmd + 0x204;  // the TMD's list of contents, and the 64 records over it
  // [first byte, length, where its hash is], from the icon outwards
  const chain = [
    [icon, v.getUint32(exefs + file * 16 + 12, true), exefs + 0x200 - 0x20 * (file + 1)],
    [exefs, v.getUint32(content + 0x1a8, true) * 0x200, content + 0x1c0],
    [content, Number(v.getBigUint64(chunks + 8)), chunks + 0x10],
    [chunks, 0x30 * v.getUint16(infos + 2), infos + 4],
    [infos, 0x900, tmd + 0x140 + 0xa4],
  ];
  if (chain[0][1] < SMDH_ICONS + icons.length || text(icon) !== "SMDH") throw bad();
  const hash = async ([at, length]) => new Uint8Array(await subtle.digest("SHA-256", out.subarray(at, at + length)));
  for (const link of chain) {
    if (link[0] + link[1] > out.length) throw bad();
    const h = await hash(link);
    if (!h.every((b, i) => b === out[link[2] + i])) throw bad();
  }
  out.set(icons, icon + SMDH_ICONS);
  for (const link of chain) out.set(await hash(link), link[2]);
  // the copy of the icon after the content, which the installer (FBI) shows; nothing hashes it
  if (metaSize >= 0x400 + SMDH_ICONS + icons.length && text(meta + 0x400) === "SMDH")
    out.set(icons, meta + 0x400 + SMDH_ICONS);
  return out;
}

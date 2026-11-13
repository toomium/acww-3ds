// The web builder's parts that need no browser: node tools/test_web.mjs (made-up bytes only, no game content).
import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdtempSync, writeFileSync } from "node:fs";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { crc32, zipParts } from "../web/js/zip.mjs";
import { createHash } from "node:crypto";
import { ciaWithIcon, smdhIcons } from "../web/js/cia.mjs";
import { makePak } from "../web/js/pak.mjs";
import { checkRom, gameCode, sdFiles, sha1Hex } from "../web/js/rom.mjs";

const enc = new TextEncoder();

// the standard check value
assert.equal(crc32(enc.encode("123456789")), 0xcbf43926);
assert.equal(await sha1Hex(enc.encode("abc")), "a9993e364706816aba3e25717850c26c9cd0d89d");

// a made-up "ROM": the right game code, the wrong content
const fake = new Uint8Array(64);
fake.set(enc.encode("ADME"), 0x0c);
assert.equal(gameCode(fake), "ADME");
let r = await checkRom(fake);
assert.equal(r.ok, false);
assert.match(r.why, /right game/);
r = await checkRom(new Uint8Array(64));
assert.equal(r.ok, false);
assert.match(r.why, /not Animal Crossing/);

const files = sdFiles({ program: enc.encode("program"), pak: fake });
assert.deepEqual(files.map((f) => f.name), ["3ds/acww/acww.3dsx", "3ds/acww/game.pak"]);
assert.deepEqual(sdFiles({ program: fake, pak: fake, cia: fake }).map((f) => f.name),
  ["3ds/acww/acww.3dsx", "3ds/acww/game.pak", "cias/acww.cia"]);

// the archive is one other tools read: Python's zipfile checks every CRC and gives the bytes back
const zip = Buffer.concat(zipParts(files).map((p) => Buffer.from(p)));
const dir = mkdtempSync(join(tmpdir(), "acww-web-"));
writeFileSync(join(dir, "sd.zip"), zip);
const out = execFileSync("python3", ["-c", `
import sys, zipfile
z = zipfile.ZipFile(sys.argv[1])
assert z.testzip() is None
print(",".join(z.namelist()), z.read("3ds/acww/acww.3dsx").decode(), len(z.read("3ds/acww/game.pak")))
`, join(dir, "sd.zip")]).toString().trim();
assert.equal(out, "3ds/acww/acww.3dsx,3ds/acww/game.pak program 64");

// the game's icon into a .cia: a made-up ROM whose icon is all red, a made-up .cia with the parts that matter
// (the TMD with one content, in it an ExeFS with the file "icon"), every hash right
const sha = (b) => createHash("sha256").update(b).digest();
const iconRom = new Uint8Array(0x80 + 0x240);
new DataView(iconRom.buffer).setUint32(0x68, 0x80, true);
iconRom.fill(0x11, 0x80 + 0x20, 0x80 + 0x220);                    // every pixel colour 1
new DataView(iconRom.buffer).setUint16(0x80 + 0x220 + 2, 0x001f, true);  // colour 1: red
assert.equal(smdhIcons(iconRom).length, 0x480 + 0x1200);
assert.throws(() => smdhIcons(new Uint8Array(64)), /no icon/);

const TMD = 0x2040, CONTENT = 0x2b80, EXEFS = CONTENT + 0x200, ICON = EXEFS + 0x200, SIZE = 0x36c0;
const fakeCia = new Uint8Array(ICON + SIZE), cv = new DataView(fakeCia.buffer);
cv.setUint32(0, 0x2020, true);
cv.setUint32(16, 0xb34, true);
cv.setBigUint64(24, BigInt(fakeCia.length - CONTENT), true);
cv.setUint32(TMD, 0x10004);
fakeCia.set(enc.encode("NCCH"), CONTENT + 0x100);
cv.setUint32(CONTENT + 0x1a0, 1, true);
cv.setUint32(CONTENT + 0x1a8, 1, true);
fakeCia.set(enc.encode("icon"), EXEFS);
cv.setUint32(EXEFS + 12, SIZE, true);
fakeCia.set(enc.encode("SMDH"), ICON);
cv.setBigUint64(TMD + 0xb04 + 8, BigInt(fakeCia.length - CONTENT));
cv.setUint16(TMD + 0x204 + 2, 1);
const links = [[ICON, SIZE, EXEFS + 0x1e0], [EXEFS, 0x200, CONTENT + 0x1c0],
  [CONTENT, fakeCia.length - CONTENT, TMD + 0xb14], [TMD + 0xb04, 0x30, TMD + 0x208], [TMD + 0x204, 0x900, TMD + 0x1e4]];
const hashesRight = (c) => links.every(([at, n, h]) => sha(c.subarray(at, at + n)).equals(Buffer.from(c.subarray(h, h + 32))));
for (const [at, n, h] of links) fakeCia.set(sha(fakeCia.subarray(at, at + n)), h);

const withIcon = await ciaWithIcon(fakeCia, iconRom);
assert.deepEqual([...withIcon.subarray(ICON + 0x2040, ICON + 0x2044)], [0x00, 0xf8, 0x00, 0xf8]);  // red, RGB565
assert.deepEqual([...withIcon.subarray(ICON + SIZE - 2)], [0x00, 0xf8]);
assert.ok(hashesRight(withIcon));
assert.equal(fakeCia[ICON + 0x2040], 0);                             // the one given is not changed
const broken = fakeCia.slice();
broken[ICON + 8] ^= 1;
await assert.rejects(ciaWithIcon(broken, iconRom), /not laid out/);
await assert.rejects(ciaWithIcon(enc.encode("not a .cia"), iconRom), /not laid out/);

// the data pack: a made-up ROM with three files, the first an overlay's (code: left out)
const packRom = new Uint8Array(0x400), pr = new DataView(packRom.buffer);
packRom.set(enc.encode("ADME"), 0x0c);
[[0x40, 0x200, 5], [0x48, 0x210, 24], [0x50, 0x240, 32]].forEach(([at, start, size]) => {
  pr.setUint32(at, start, true), pr.setUint32(at + 4, size, true);
});
packRom.set(enc.encode("names"), 0x200);
[[0x300, 0x310], [0x310, 0x313], [0x320, 0x320]].forEach(([a, b], i) => {
  pr.setUint32(0x210 + i * 8, a, true), pr.setUint32(0x214 + i * 8, b, true);
});
pr.setUint32(0x240 + 24, 0, true);  // the overlay's file: number 0
packRom.set(enc.encode("abc"), 0x310);
const pak = makePak(packRom, new Uint8Array(20).fill(7), "v9");
const pv = new DataView(pak.buffer);
assert.equal(String.fromCharCode(...pak.subarray(0, 7)), "ACWWPAK");
assert.deepEqual([pv.getUint32(8, true), pak[0x0c], pak[0x0d], pak[0x0e], pak[0x2c], pv.getUint32(0x40, true)], [1, 0x76, 0x39, 0, 7, 6]);
const entry = (i) => [0, 4, 8].map((k) => pv.getUint32(0x44 + i * 12 + k, true));
assert.deepEqual([0, 1, 2, 3, 4, 5].map((i) => entry(i)[0]), [1, 2, 0x10000000, 0x10000001, 0x10000002, 0x10000003]);
const [, abcAt, abcSize] = entry(0), [, headAt, headSize] = entry(2), [, , namesSize] = entry(3), [, tableAt] = entry(4);
assert.equal(abcAt % 4, 0);
assert.equal(String.fromCharCode(...pak.subarray(abcAt, abcAt + abcSize)), "abc");
assert.equal(entry(1)[2], 0);                                        // the empty file
assert.deepEqual([headSize, namesSize, pak.length % 4], [0x200, 5, 0]);
assert.equal(String.fromCharCode(...pak.subarray(headAt + 0x0c, headAt + 0x10)), "ADME");
assert.deepEqual([pv.getUint32(headAt + 0x48, true), pv.getUint32(headAt + 0x4c, true)], [tableAt, 24]);
// the file table: the overlay's file is nowhere, "abc" is where the index says
assert.deepEqual([0, 4, 8, 12].map((k) => pv.getUint32(tableAt + k, true)), [0, 0, abcAt, abcAt + 3]);
assert.throws(() => makePak(new Uint8Array(64), new Uint8Array(20), "v9"), /not a DS ROM/);

console.log("web builder tests passed");

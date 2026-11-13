// What the builder knows about the files it is given. Nothing here leaves the browser.

// The dumps the port runs: the game code in the header and the whole file's SHA-1.
export const ROMS = [
  { name: "USA (Rev 1)", code: "ADME", sha1: "77fde3e30e1e6068395d1f96ea63be569b61c351" },
];

export const subtle = globalThis.crypto?.subtle ?? (await import("node:crypto")).webcrypto.subtle;

export async function sha1Hex(bytes) {
  const digest = new Uint8Array(await subtle.digest("SHA-1", bytes));
  return [...digest].map((b) => b.toString(16).padStart(2, "0")).join("");
}

// The four letters at 0x0c of a DS ROM's header.
export function gameCode(bytes) {
  return bytes.length < 0x10 ? "" : String.fromCharCode(...bytes.subarray(0x0c, 0x10));
}

// {ok, rom, code, sha1, why}: whether these bytes are a dump the port runs.
export async function checkRom(bytes) {
  const code = gameCode(bytes), sha1 = await sha1Hex(bytes);
  const rom = ROMS.find((r) => r.sha1 === sha1);
  if (rom) return { ok: true, rom, code, sha1 };
  const sameGame = ROMS.some((r) => r.code === code);
  return {
    ok: false, code, sha1,
    why: sameGame ? "This is the right game, but not a dump the port knows (another revision, a trimmed or a patched file)."
                  : "This is not Animal Crossing: Wild World (USA).",
  };
}

// The SD card's files: [{name, data}] for zipParts.
export function sdFiles({ program, pak, cia }) {
  const files = [{ name: "3ds/acww/acww.3dsx", data: program }, { name: "3ds/acww/game.pak", data: pak }];
  if (cia) files.push({ name: "cias/acww.cia", data: cia });  // (the HOME Menu version, for FBI to install)
  return files;
}

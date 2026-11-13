// The page: the ROM checked, the program fetched from this site (or chosen), the settings, the ZIP.
import { zipParts } from "./zip.mjs";
import { checkRom, sdFiles } from "./rom.mjs";
import { ciaWithIcon } from "./cia.mjs";
import { makePak } from "./pak.mjs";

const $ = (id) => document.getElementById(id);
const state = { rom: null, sha1: "", program: null, cia: null, tag: "own build" };

function say(id, text, kind = "") {
  $(id).textContent = text;
  $(id).className = "status " + kind;
}
async function bytesOf(file) {
  return new Uint8Array(await file.arrayBuffer());
}
function refresh() {
  $("build").disabled = !(state.rom && state.program);
}

$("rom").addEventListener("change", async () => {
  state.rom = null;
  const file = $("rom").files[0];
  if (file) {
    say("rom-status", "Checking…");
    const bytes = await bytesOf(file);
    const r = await checkRom(bytes);
    if (r.ok) {
      state.rom = bytes;
      state.sha1 = r.sha1;
      say("rom-status", `${r.rom.name}: the right dump.`, "ok");
    } else {
      say("rom-status", `${r.why} (game code ${r.code || "?"}, SHA-1 ${r.sha1})`, "bad");
    }
  }
  refresh();
});

$("program").addEventListener("change", async () => {
  const file = $("program").files[0];
  state.program = file ? await bytesOf(file) : null;
  refresh();
});

$("build").addEventListener("click", async () => {
  let cia = $("cia").checked ? state.cia : null;
  // the game's icon on it, from the ROM; a .cia this can't read keeps its placeholder
  if (cia) cia = await ciaWithIcon(cia, state.rom).catch((e) => (console.warn(e), cia));
  // the data pack: the ROM's files without the game's code, for this build
  const sha1 = Uint8Array.from(state.sha1.match(/../g), (h) => parseInt(h, 16));
  const files = sdFiles({ program: state.program, pak: makePak(state.rom, sha1, state.tag), cia });
  const url = URL.createObjectURL(new Blob(zipParts(files), { type: "application/zip" }));
  const a = document.createElement("a");
  a.href = url;
  a.download = "acww-3ds-sd.zip";
  a.click();
  setTimeout(() => URL.revokeObjectURL(url), 60000);
  say("build-status", "Done: acww-3ds-sd.zip.", "ok");
});

// This site's build (tools/build_site.py puts a release's files in release/); without one, the visitor's own.
try {
  const info = await (await fetch("release/release.json")).json();
  const res = await fetch("release/acww.3dsx");
  if (!res.ok) throw new Error(String(res.status));
  state.program = new Uint8Array(await res.arrayBuffer());
  state.tag = info.tag;
  say("program-status", `This site's build: ${info.tag}.`, "ok");
  $("release").textContent = `Build ${info.tag}, SHA-256 ${info.sha256}.`;
  // The HOME Menu version, if the release has one: offered only when it is here.
  if (info.cia) {
    const c = await fetch("release/acww.cia");
    if (c.ok) {
      state.cia = new Uint8Array(await c.arrayBuffer());
      $("cia-row").hidden = false;
    }
  }
} catch {
  say("program-status", "");
  $("program-own").hidden = false;
}
refresh();

// The page: the ROM checked, the console chosen, the program fetched from this site (or chosen), the ZIP.
import { zipParts } from "./zip.mjs";
import { checkRom, programConsole, sdFiles } from "./rom.mjs";
import { ciaWithIcon } from "./cia.mjs";
import { makePak } from "./pak.mjs";

const $ = (id) => document.getElementById(id);
// site: this site's programs by console (release/), own: the visitor's own build and its console
const state = { rom: null, sha1: "", console: "3ds", site: {}, own: null, cia: null, tag: "", siteTag: "" };
const PROGRAM = { "3ds": "acww.3dsx", switch: "acww.nro" };

function say(id, text, kind = "") {
  $(id).textContent = text;
  $(id).className = "status " + kind;
}
async function bytesOf(file) {
  return new Uint8Array(await file.arrayBuffer());
}
function program() {
  return state.own?.console === state.console ? state.own.data : state.site[state.console] ?? null;
}
// what the page shows for the chosen console
function refresh() {
  const c = state.console, fromSite = !!state.site[c];
  state.tag = fromSite ? state.siteTag : "own build";
  if (fromSite) say("program-status", `This site's build: ${state.siteTag}.`, "ok");
  else if (state.own && state.own.console !== c) say("program-status", `That file is not a ${PROGRAM[c]}.`, "bad");
  else say("program-status", state.own ? `Your own ${PROGRAM[c]}.` : "", state.own ? "ok" : "");
  $("program-own").hidden = fromSite;
  $("cia-row").hidden = !(c === "3ds" && state.cia);
  for (const el of document.querySelectorAll("[data-console]")) el.hidden = el.dataset.console !== c;
  $("build").disabled = !(state.rom && program());
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

for (const radio of document.querySelectorAll("input[name=console]")) {
  radio.addEventListener("change", () => {
    state.console = radio.value;
    refresh();
  });
}

$("program").addEventListener("change", async () => {
  const file = $("program").files[0];
  const data = file ? await bytesOf(file) : null;
  state.own = data ? { data, console: programConsole(data) } : null;
  refresh();
});

$("build").addEventListener("click", async () => {
  const c = state.console;
  let cia = c === "3ds" && $("cia").checked ? state.cia : null;
  // the game's icon on it, from the ROM; a .cia this can't read keeps its placeholder
  if (cia) cia = await ciaWithIcon(cia, state.rom).catch((e) => (console.warn(e), cia));
  // the data pack: the ROM's files without the game's code, for this build
  const sha1 = Uint8Array.from(state.sha1.match(/../g), (h) => parseInt(h, 16));
  const files = sdFiles({ console: c, program: program(), pak: makePak(state.rom, sha1, state.tag), cia });
  const url = URL.createObjectURL(new Blob(zipParts(files), { type: "application/zip" }));
  const name = `acww-${c}-sd.zip`;
  const a = document.createElement("a");
  a.href = url;
  a.download = name;
  a.click();
  setTimeout(() => URL.revokeObjectURL(url), 60000);
  say("build-status", `Done: ${name}.`, "ok");
});

// This site's builds (tools/build_site.py puts a release's files in release/); without one, the visitor's own.
try {
  const info = await (await fetch("release/release.json")).json();
  const res = await fetch("release/acww.3dsx");
  if (!res.ok) throw new Error(String(res.status));
  state.site["3ds"] = new Uint8Array(await res.arrayBuffer());
  state.siteTag = info.tag;
  $("release").textContent = `Build ${info.tag}, SHA-256 ${info.sha256}.`;
  // The Switch's program and the HOME Menu version, if the release has them: offered only when they are here.
  if (info.nro) {
    const n = await fetch("release/acww.nro");
    if (n.ok) state.site.switch = new Uint8Array(await n.arrayBuffer());
  }
  if (info.cia) {
    const c = await fetch("release/acww.cia");
    if (c.ok) state.cia = new Uint8Array(await c.arrayBuffer());
  }
} catch {
  // (no build on this site: refresh() asks for the visitor's own)
}
refresh();

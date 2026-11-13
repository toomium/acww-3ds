// The web builder's page in a real browser: node tools/test_web_browser.mjs [firefox|chrome|chromium|PATH]
// Builds the site (tools/build_site.py) with a made-up program, serves it on localhost, opens a test page in a
// headless browser that chooses made-up files in the real page (in an iframe), clicks Build and hands back the
// ZIP the page made; this script then checks the ZIP's names and bytes. No game data: every input is made up here.
//
// The page accepts only the real dump (SHA-1 in web/js/rom.mjs), so a made-up ROM is refused: the test shows
// that on the site as it is (/real/), then serves a copy of the site whose rom.mjs lists the made-up ROM's SHA-1
// (/test/) for the build. The files on disk and the production page are never changed; only that served copy is.
// No driver or package is needed: the browser runs the test page, which reports to this server.
import assert from "node:assert/strict";
import { execFileSync, spawn } from "node:child_process";
import { createHash } from "node:crypto";
import { createServer } from "node:http";
import { mkdirSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from "node:fs";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { fileURLToPath } from "node:url";

const root = fileURLToPath(new URL("..", import.meta.url));
const enc = new TextEncoder();
const work = mkdtempSync(join(tmpdir(), "acww-browser-"));
let browser, server;
const cleanup = () => { browser?.kill(); server?.close(); rmSync(work, { recursive: true, force: true }); };

try {
  // made-up inputs: a "ROM" with the right game code, and a "program"
  const rom = new Uint8Array(0x200).map((_, i) => i * 7 % 251);
  rom.set(enc.encode("ADME"), 0x0c);
  rom.fill(0, 0x40, 0x58);  // (no files: the data pack made from it is its header alone)
  const program = enc.encode("made-up program\n");
  const sha1 = createHash("sha1").update(rom).digest("hex");

  mkdirSync(join(work, "release"));
  writeFileSync(join(work, "release", "acww.3dsx"), program);
  writeFileSync(join(work, "release", "acww.cia"), enc.encode("made-up cia\n"));
  execFileSync("python3", [join(root, "tools/build_site.py"), "--out", join(work, "site"),
    "--release-dir", join(work, "release"), "--tag", "v0.0.0-test"], { stdio: "inherit" });

  const romJs = readFileSync(join(work, "site/js/rom.mjs"), "utf8");
  const patched = romJs.replace(/sha1: "[0-9a-f]{40}"/, `sha1: "${sha1}"`);
  assert.notEqual(patched, romJs, "rom.mjs has no sha1 to replace");

  const harness = `<!doctype html><meta charset="utf-8"><body><script type="module">
const wait = async (what, cond) => { for (let i = 0; i < 200 && !cond(); i++) await new Promise((r) => setTimeout(r, 50)); if (!cond()) throw new Error("timed out: " + what); };
const open = async (src) => { const f = document.createElement("iframe"); f.src = src; document.body.append(f); const w = f.contentWindow;
  await wait(src + " fetched the program", () => w.document.getElementById("program-status")?.textContent.includes("This site's build")); return w; };
const choose = (w, id, bytes) => { const dt = new w.DataTransfer(); dt.items.add(new w.File([new Uint8Array(bytes)], "x.bin"));
  const el = w.document.getElementById(id); el.files = dt.files; el.dispatchEvent(new w.Event("change")); };
try {
  const rom = ${JSON.stringify([...rom])};
  const out = {};
  let w = await open("/real/index.html");
  choose(w, "rom", rom);
  await wait("rom status", () => w.document.getElementById("rom-status").textContent.includes("SHA-1"));
  out.realRom = w.document.getElementById("rom-status").textContent;
  out.realDisabled = w.document.getElementById("build").disabled;
  out.release = w.document.getElementById("release").textContent;
  w = await open("/test/index.html");
  const blobs = [], make = w.URL.createObjectURL;
  w.URL.createObjectURL = (b) => (blobs.push(b), make(b));
  choose(w, "rom", rom);
  await wait("rom accepted", () => w.document.getElementById("rom-status").textContent.includes("right dump"));
  await wait("the .cia offered", () => !w.document.getElementById("cia-row").hidden);
  await wait("build enabled", () => !w.document.getElementById("build").disabled);
  w.document.getElementById("build").click();
  await wait("zip", () => blobs.length);
  await fetch("/zip", { method: "POST", body: blobs[0] });
  out.buildStatus = w.document.getElementById("build-status").textContent;
  await fetch("/result", { method: "POST", body: JSON.stringify(out) });
} catch (e) { await fetch("/result", { method: "POST", body: JSON.stringify({ error: String(e) }) }); }
</script>`;

  let zip, finish;
  const done = new Promise((resolve) => (finish = resolve));
  server = createServer(async (req, res) => {
    const chunks = [];
    for await (const c of req) chunks.push(c);
    const body = Buffer.concat(chunks);
    const url = new URL(req.url, "http://localhost");
    if (req.method === "POST") {
      if (url.pathname === "/zip") zip = body; else finish(JSON.parse(body.toString()));
      return res.end("ok");
    }
    if (url.pathname === "/") return res.setHeader("content-type", "text/html").end(harness);
    const m = url.pathname.match(/^\/(real|test)\/(.*)$/);
    const rel = m?.[2] || "index.html";
    if (!m || rel.includes("..")) return res.writeHead(404).end();
    const type = { html: "text/html", mjs: "text/javascript", css: "text/css", json: "application/json" }[rel.split(".").pop()] ?? "application/octet-stream";
    try {
      const data = m[1] === "test" && rel === "js/rom.mjs" ? patched : readFileSync(join(work, "site", rel));
      res.setHeader("content-type", type).end(data);
    } catch { res.writeHead(404).end(); }
  }).listen(0, "127.0.0.1");
  await new Promise((r) => server.once("listening", r));
  const url = `http://127.0.0.1:${server.address().port}/`;

  const want = process.argv[2] ?? ["firefox", "google-chrome", "chromium"].find((b) => {
    try { execFileSync("which", [b], { stdio: "ignore" }); return true; } catch { return false; }
  });
  assert.ok(want, "no browser found (firefox, google-chrome, chromium)");
  const profile = join(work, "profile");
  const args = /firefox/.test(want) ? ["--headless", "--no-remote", "--profile", profile, url]
    : ["--headless=new", "--no-sandbox", "--disable-gpu", `--user-data-dir=${profile}`, url];
  mkdirSync(profile);
  browser = spawn(want, args, { stdio: "ignore" });
  const result = await Promise.race([done, new Promise((_, no) => setTimeout(() => no(new Error("no answer from the browser")), 90000))]);
  assert.equal(result.error, undefined, result.error);

  // the page as it is refuses the made-up ROM and says what it is
  assert.match(result.realRom, /right game, but not a dump/);
  assert.ok(result.realRom.includes(sha1) && result.realRom.includes("ADME"));
  assert.equal(result.realDisabled, true);
  assert.ok(result.release.includes("v0.0.0-test"));
  assert.match(result.buildStatus, /Done/);

  // the ZIP the page made: names in order, every CRC right, the data pack made for this build from this ROM
  writeFileSync(join(work, "sd.zip"), zip);
  const listing = execFileSync("python3", ["-c", `
import sys, zipfile
z = zipfile.ZipFile(sys.argv[1])
assert z.testzip() is None
pak = z.read("3ds/acww/game.pak")
assert pak[:8] == b"ACWWPAK\\0" and pak[12:23] == b"v0.0.0-test" and pak[44:64].hex() == sys.argv[2]
print(",".join(z.namelist()), repr(z.read("3ds/acww/acww.3dsx")), repr(z.read("cias/acww.cia")))
`, join(work, "sd.zip"), sha1]).toString().trim();
  assert.equal(listing, `3ds/acww/acww.3dsx,3ds/acww/game.pak,cias/acww.cia b'made-up program\\n' b'made-up cia\\n'`);
  console.log("web builder browser test passed (" + want + ")");
} finally {
  cleanup();
}

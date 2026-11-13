// node tools/make_pak.mjs <rom.nds> <game.pak> [build id]: the data pack from your own ROM, as the web builder
// makes it (`make pak ROM=...`). The pack is game content: never commit or pass it on.
import { createHash } from "node:crypto";
import { readFileSync, writeFileSync } from "node:fs";
import { makePak } from "../web/js/pak.mjs";

const [rom, out, buildId = "local"] = process.argv.slice(2);
const bytes = new Uint8Array(readFileSync(rom));
writeFileSync(out, makePak(bytes, createHash("sha1").update(bytes).digest(), buildId));

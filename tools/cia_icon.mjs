// node tools/cia_icon.mjs <acww.cia> <rom.nds>: the .cia's icon replaced by the ROM's, in place (`make cia ROM=...`).
// The result has game content (the icon) in it: for your own console, not for a release.
import { readFileSync, writeFileSync } from "node:fs";
import { ciaWithIcon } from "../web/js/cia.mjs";

const [cia, rom] = process.argv.slice(2);
writeFileSync(cia, await ciaWithIcon(new Uint8Array(readFileSync(cia)), new Uint8Array(readFileSync(rom))));

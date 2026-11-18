// The console ports' texture VRAM versions (the 3DS's gpu3d_pica.c, the Switch's gpu3d_gl.c each include this once:
// its statics are the backend's): a copy of the texture image and palette slots and a version per FINE bytes, bumped
// when they change, and a texture's newest version from the bytes it reads, so that a texture is decoded again only
// when what it reads changed. The backend defines IMG_SIZE (0x80000), PAL_SIZE (0x20000) and PIECE (0x1000).
// (Moved out of gpu3d_pica.c, 2026-11-18, as it was.)
// a version per FINE bytes of the texture image and palette slots, bumped when they change: small, so that a
// palette or texture the game animates doesn't make its neighbours in VRAM look changed (their decoding again, 4 KB
// pieces before: a palette's five colours had six 4x4 textures decoded again, 30 ms on core 2)
enum { FINE = 64 };
static u8 sImgCopy[IMG_SIZE], sPalCopy[PAL_SIZE];
static u32 sImgVer[IMG_SIZE / FINE], sPalVer[PAL_SIZE / FINE], sVer;

static void track(const u8 *src, u8 *copy, u32 *ver, u32 size) {
    for (u32 o = 0; o < size; o += PIECE)  // (PIECE at a time, then the FINE bytes that differ)
        if (memcmp(copy + o, src + o, PIECE))
            for (u32 f = o; f < o + PIECE; f += FINE)
                if (memcmp(copy + f, src + f, FINE)) {
                    memcpy(copy + f, src + f, FINE);
                    ver[f / FINE] = ++sVer;
                }
}
static u32 newest(const u32 *ver, u32 size, u32 start, u32 len) {
    u32 v = 0;
    if (!len) return 0;
    for (u32 p = start / FINE, n = 0; n <= len / FINE + 1 && n < size / FINE; p = (p + 1) % (size / FINE), n++)
        if (ver[p] > v) v = ver[p];
    return v;
}
static u32 slot1_4x4(u32 base) { return 0x20000 + ((base & 0x1ffff) >> 1) + ((base >> 17) == 2 ? 0x10000 : 0); }
// a 4x4 texture's palette bytes: from its palette base, the lowest to the highest block's offset (4 bytes a unit;
// a block uses up to 4 colours: 8 bytes)
static void palette_4x4(u32 tp, u32 tpal, u32 *lo, u32 *len) {
    int w = 8 << (tp >> 20 & 7), h = 8 << (tp >> 23 & 7);
    u32 slot1 = slot1_4x4((tp & 0xffff) << 3 & (IMG_SIZE - 1)), mn = 0x3fff, mx = 0;
    for (int b = 0; b < w * h / 16; b++) {
        u32 a = (slot1 + 2 * (u32)b) & (IMG_SIZE - 1), off = (sImgCopy[a] | sImgCopy[a + 1] << 8) & 0x3fff;
        if (off < mn) mn = off;
        if (off > mx) mx = off;
    }
    *lo = (((tpal & 0x1fff) << 4) + mn * 4) & (PAL_SIZE - 1);
    *len = (mx - mn) * 4 + 8;
}
// the newest version of the VRAM a texture reads (GBATEK "DS 3D Texture Formats": the image, 4x4's second slot,
// the palette)
static u32 texture_version(u32 tp, u32 tpal) {
    int fmt = tp >> 26 & 7, w = 8 << (tp >> 20 & 7), h = 8 << (tp >> 23 & 7);
    static const u8 bits[8] = {0, 8, 2, 4, 8, 2, 8, 16};
    static const u16 colours[8] = {0, 32, 4, 16, 256, 0, 8, 0};
    u32 base = (tp & 0xffff) << 3 & (IMG_SIZE - 1);
    u32 v = newest(sImgVer, IMG_SIZE, base, (u32)(w * h * bits[fmt] / 8));
    if (fmt == 5) {  // (the 4x4 blocks' palette info in slot 1; the palette bytes they use: palette_4x4)
        u32 i = newest(sImgVer, IMG_SIZE, slot1_4x4(base), (u32)(w * h / 8));
        return v > i ? v : i;
    }
    if (colours[fmt]) {
        u32 pb = fmt == 2 ? (tpal & 0x1fff) << 3 : (tpal & 0x1fff) << 4;
        u32 p = newest(sPalVer, PAL_SIZE, pb & (PAL_SIZE - 1), colours[fmt] * 2u);
        if (p > v) v = p;
    }
    return v;
}

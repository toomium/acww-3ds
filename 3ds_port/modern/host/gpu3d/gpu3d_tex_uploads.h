// The port's texture uploads copied piecemeal (N3DS_PORT): included once in gpu3d_tex.h, before
// g3_tex_setup.

int gG3TexParts = -1;
u32 gG3TexPart[G3_TEX_PARTS][2];
// The port: only the uploads since the last call copied into the flat images, while no remap came between
// (the slots' banks as before) and their slot has that bank alone (none OR-ed); a texture upload changed 640 KB
// of copies and comparisons before (gpu3d_pica.c track). 0: not possible, the slots taken in full.
static int g3_tex_uploads(void) {
    unsigned up[G3_TEX_PARTS][3];
    int n = host_vram_take_uploads(up, G3_TEX_PARTS), parts = 0;
    if (n < 0 || !gG3Banks) return 0;
    for (int k = 0; k < n; k++) {
        unsigned b = up[k][0], off = up[k][1], len = up[k][2];
        u8 c = b < 7 ? gG3VramCnt[b] : 0;
        if (!(c & 0x80) || (c & 7) != 3 || !gG3Banks[b] || !len) continue;  // (not in a texture slot)
        int ofs = c >> 3 & 3;
        u32 at;  // (in the image slots, or | 1u << 31 the palette slots)
        if (b < 4) {
            if (sNImg[ofs] != 1) return 0;
            at = (u32)ofs * IMG_SLOT + off;
        } else if (b == 4) {  // (bank E: palette slots 0-3, as its offset)
            for (unsigned s = off / PAL_SLOT; s <= (off + len - 1) / PAL_SLOT; s++)
                if (sNPal[s] != 1) return 0;
            at = off | 1u << 31;
        } else {
            int s = (ofs & 1) + (ofs >> 1) * 4;
            if (sNPal[s] != 1) return 0;
            at = ((u32)s * PAL_SLOT + off) | 1u << 31;
        }
        memcpy((at >> 31 ? sPalFlat : sImgFlat) + (at & 0x7fffffff), gG3Banks[b] + off, len);
        gG3TexPart[parts][0] = at, gG3TexPart[parts++][1] = len;
    }
    gG3TexParts = parts;
    return 1;
}

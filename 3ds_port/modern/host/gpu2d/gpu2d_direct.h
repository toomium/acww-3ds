// The port's additions to the recorded frame (N3DS_PORT): the margins' affine parameters, the iris, the
// menu wipe, the options page and the anchors of OBJs and BGs. Included once in gpu2d.c, after the Record struct.

static Gpu2dMarginAffine sMargin[2];  // (given for the next frame: recorded at its start)
void gpu2d_margin_affine(int engine, int bg, int line, int pa, int32_t x, int32_t y) {
    int k = line < 0 ? line + GPU2D_MARGIN : line - SCREEN_H + GPU2D_MARGIN;
    if (engine < 0 || engine > 1 || k < 0 || k >= 2 * GPU2D_MARGIN || (line >= 0 && line < SCREEN_H)) return;
    Gpu2dMarginAffine *m = &sMargin[engine];
    m->bg = bg, m->pa[k] = (int16_t)pa, m->x[k] = x, m->y[k] = y;
}
static int sIris = -1;  // (kept until changed)
void gpu2d_iris(int progress) { sIris = progress; }
static int sMenuWipe = -1;  // (kept while engine A's window 1 is on)
void gpu2d_menu_wipe(int pos) { sMenuWipe = pos; }
static const uint32_t *sOptPic;  // (the options page: kept until changed)
static uint32_t sOptSerial;
static int sOptPageY = 192, sOptBtnY = GPU2D_NO_BTN;
static int sOptBtnNext = GPU2D_NO_BTN;
void gpu2d_options(const uint32_t *pic, uint32_t serial, int pageY) { sOptPic = pic, sOptSerial = serial, sOptPageY = pageY; }
void gpu2d_opt_button(int y) { sOptBtnNext = y; }
static u8 sObjAnchor[2][128], sObjAnchorOam[2][128][6];  // (each entry's anchor, and what it was given for)
static Gpu2dBgAnchor sBgAnchor[2];
void gpu2d_obj_anchors(int engine, const void *oam, const uint8_t *anchors) {
    if (engine < 0 || engine > 1) return;
    memcpy(sObjAnchor[engine], anchors, 128);
    for (int i = 0; i < 128; i++) memcpy(sObjAnchorOam[engine][i], (const u8 *)oam + 8 * i, 6);
    if (engine == 1) sOptBtnY = sOptBtnNext, sOptBtnNext = GPU2D_NO_BTN;  // (the options button goes with the OAM, as an OBJ)
}
void gpu2d_bg_anchor(int engine, int bgs, int windows, int anchor) {
    if (engine < 0 || engine > 1) return;
    Gpu2dBgAnchor *a = &sBgAnchor[engine];
    *a = (Gpu2dBgAnchor){(u8)bgs, (u8)windows, (u8)(bgs ? anchor : 0), {0}};
    for (int n = 0; n < 4; n++) {  // (another screen's BG with these numbers isn't moved: a menu opened meanwhile)
        u32 v = gpu2d_read32(0x04000008u + 0x1000u * (u32)engine + 4u * (u32)(n / 2));
        a->bgcnt[n] = (u16)(n & 1 ? v >> 16 : v);
    }
}

// The port's display lists (N3DS_PORT): a block of 64 words or more for the GX FIFO (the game's lists of
// models and map pieces, the same words every frame) is kept in a cache and converted once into a form that is run
// again and again without decoding it: the vertex commands of a stretch worked out into vertices, the lighting done
// once for each different normal, the polygons made as lists of vertex numbers. Included once in gpu3d_geom.c, after
// execute(): it needs the file's statics. See docs/old-3ds.md 6.6.
//
// The game sends a model's list as a block of words after a packed command word written on its own (in town the
// word of BEGIN_VTXS and the first texture coordinates, normal and vertex): the commands' parameters then follow in
// the block. The list is the two together: that command word (the commands it has left) and the block.
//
// Convertible is a list of only NOP, MTX_RESTORE, COLOR, NORMAL, TEXCOORD, the vertex commands, BEGIN_VTXS and
// END_VTXS, with a BEGIN_VTXS before its first vertex, whose first vertex command is an absolute one (VTX_16 or
// VTX_10), and with no primitive of more than DL_MAXPRIM vertices. Anything else takes the old path, every time.
//
// A converted list is a row of steps: "run command" (MTX_RESTORE and BEGIN_VTXS, through execute() as before: the
// matrices, the latch of the polygon attributes and the counters of lighting's inputs stay exactly the old code's) and
// "piece" (everything between two such commands).

enum { DL_SLOTS = 2048, DL_MAXPRIM = 1024 };
enum { DL_OK, DL_CMD, DL_CUT, DL_NOBEGIN, DL_RELATIVE, DL_BIG, DL_CHANGING, DL_WHYS };
static const char *const kDlWhy[DL_WHYS] = {"", "command", "cut", "no BEGIN", "relative first", "big primitive", "changing"};

typedef struct {
    s16 xyz[3];  // the object coordinates (the relative commands worked out)
    u8 ck, tk;   // the colour's source: 0 the one before the piece (sVtxColor as it is then), 1 a COLOR word (cw),
                 // 2 a normal (cw: its number in the piece's normals); the texture coordinates': 0 the one before
                 // the piece, 1 a TEXCOORD word (tw)
    u32 cw, tw;
} DlVtx;
typedef struct { u32 nv, v[4]; } DlPoly;  // a polygon: its vertices' numbers in the list in submit_polygon's order
typedef struct {
    u32 first, nvtx, nnormal, npoly;  // the vertices' numbers in the list: first, first + 1, ...
    DlVtx *vtx;
    u32 *normal;   // the piece's different normals (30 bits)
    DlPoly *poly;  // the polygons that are complete after the piece's last vertex, in order
    u32 nnormcmd;  // NORMAL commands (sTickNormals)
    u32 gfirst, gcount;  // ACWW_G3D_GPU: its triangles' indices in the GPU's memory (dl_gpu_store)
    s16 bb[6];           // its vertices' box in object coordinates: the least x, y, z, then the greatest;
    u8 run;              // in a run's first piece (run: the first after a command that isn't BEGIN_VTXS) the run's
    u8 glit;             // its vertices there have normals (lit by the shader), not colours
    u8 fck, ftk, hasNormal;  // what is left in sVtxColor / sTexRaw and sTexCoord / sNormal after the piece: the last
    u32 fcw, ftw, lastNormal;  // colour source (ck, cw), the last TEXCOORD word, the last normal's word
    int inpoly, strip;  // sVtxInPoly and sStripPolys after the piece
} DlPiece;
typedef struct { u8 cmd; u32 param, piece; } DlStep;  // cmd 0: the piece
typedef struct {
    const u32 *ptr;
    u32 n, *words;  // the list as sent (a copy)
    u8 pre;         // the list starts with a command word that was written on its own (words[0]); the block is the rest
    u8 state;       // 0 unused, 1 not convertible (why), 2 converted
    u8 why;
    s8 gpu;         // ACWW_G3D_GPU: its pieces are in the GPU's memory (1), can't be (-1), not tried yet (0)
    u8 changed;     // times the words at ptr were others than the copy (3: taken as a buffer written anew, not a list)
    DlStep *step;
    DlPiece *piece;
    u32 nstep, npiece, nvtx, maxnormal;
    u8 perm[4];     // sTemp[i] is, after the list, the pointer sTemp[perm[i]] was before
    s32 lastw[4];   // the vertex last written through the pointer sTemp[i] was before (-1 none)
} DlEntry;
static DlEntry sDl[DL_SLOTS];
static struct { unsigned long seen, hits, converted, replaced, why[DL_WHYS], old, ran, checked, diffs; } sDlStat;
static int sDlStats = -1, sDlOn, sDlCheck, sDlBusy;  // (the options; sDlBusy: the old path being run again by the check)
static int sDlGpu, sDlGpuNow;  // ACWW_G3D_GPU=1; the list being run is drawn by the GPU
static unsigned long sDlGpuWhy[3];  // (lists not drawn by the GPU: the check, the texture matrix, the list itself)
static unsigned long sDlGpuNot;  // (lists that can't be: dl_gpu_store's rules, or no room)
static int sDlCull;  // ACWW_G3D_CULL (on unless 0): dl_box_out
static unsigned long sDlBoxOut, sDlBoxOutV, sDlBoxV;  // (dl_box_out's counts: pieces out, their vertices, all vertices)
static unsigned long sDlGpuDraws, sDlGpuCpu;  // (pieces drawn by the GPU; pieces of such lists the CPU made after all)

// ACWW_G3D_BENCH's last run: the ticks of a list's parts (0 the lookup, 1 the list's start, 2 the commands between
// the pieces, 3 a draw's values, 4 g3p_gpu_draw, 5 the piece's rest in a list the GPU draws, 6 the list's end, 7 the
// pieces of a list under the texture matrix, 8 those of a list the GPU can't draw), how many of each command, and the
// pieces and vertices of the three kinds of list (sDlKind: 0 the GPU's, 1 texture matrix, 2 can't)
static int sDlSplit, sDlKind;
static u64 sDlT[9], sDlMark;
static unsigned long sDlCmds[256], sDlLists, sDlPieces[3], sDlVerts[3];
#define DL_T(i) do { if (sDlSplit) { u64 n_ = host_plat_ticks(); sDlT[i] += n_ - sDlMark, sDlMark = n_; } } while (0)

static void dl_options(void) {
    if (sDlStats >= 0) return;
    sDlStats = getenv("ACWW_G3D_DLSTATS") && atoi(getenv("ACWW_G3D_DLSTATS"));
    sDlOn = !getenv("ACWW_G3D_DL") || atoi(getenv("ACWW_G3D_DL"));  // (on unless ACWW_G3D_DL=0)
    sDlCheck = getenv("ACWW_G3D_DLCHECK") && atoi(getenv("ACWW_G3D_DLCHECK"));
    sDlGpu = getenv("ACWW_G3D_GPU") ? atoi(getenv("ACWW_G3D_GPU")) : 0;
    sDlCull = !getenv("ACWW_G3D_CULL") || atoi(getenv("ACWW_G3D_CULL"));
}

static void dl_free(DlEntry *e) {
    for (u32 i = 0; i < e->npiece && e->piece; i++) free(e->piece[i].vtx), free(e->piece[i].normal), free(e->piece[i].poly);
    free(e->piece), free(e->step), free(e->words);
    memset(e, 0, sizeof *e);
}

// the list's words as they are made into steps and pieces
typedef struct {
    DlStep *step;
    DlPiece *piece, cur;  // cur: the piece being made
    DlVtx *tv;
    u32 *tn;
    DlPoly *tp;
    u32 nstep, npiece, nvtx, pv0, nn, pp0, np;  // (pv0, nn, pp0: where the piece being made began)
    int any;                                    // it has a vertex or an event
    int inpoly, strip;
} DlBuild;

static void dl_piece_free(DlPiece *p) { free(p->vtx), free(p->normal), free(p->poly); }

// the piece being made ends: kept if it has anything; 0 if out of memory
static int dl_flush(DlBuild *b) {
    DlPiece *c = &b->cur;
    if (b->any) {
        c->first = b->pv0, c->nvtx = b->nvtx - b->pv0, c->nnormal = b->nn, c->npoly = b->np - b->pp0;
        c->inpoly = b->inpoly, c->strip = b->strip;
        c->vtx = malloc((size_t)c->nvtx * sizeof *c->vtx + 1);
        c->normal = malloc((size_t)c->nnormal * sizeof *c->normal + 1);
        c->poly = malloc((size_t)c->npoly * sizeof *c->poly + 1);
        if (!c->vtx || !c->normal || !c->poly) { dl_piece_free(c); memset(c, 0, sizeof *c); return 0; }
        memcpy(c->vtx, b->tv + b->pv0, c->nvtx * sizeof *c->vtx);
        memcpy(c->normal, b->tn, c->nnormal * sizeof *c->normal);
        memcpy(c->poly, b->tp + b->pp0, c->npoly * sizeof *c->poly);
        b->step[b->nstep++] = (DlStep){0, 0, b->npiece};
        b->piece[b->npiece++] = *c;
    }
    memset(c, 0, sizeof *c);
    b->any = 0, b->pv0 = b->nvtx, b->nn = 0, b->pp0 = b->np;
    return 1;
}

// the list's words into the converted form; the reason it can't be, or DL_OK
static int dl_convert(DlEntry *e, const u32 *v, u32 n) {
    DlBuild b = {0};
    b.step = malloc((4 * (size_t)n + 2) * sizeof *b.step);
    b.piece = malloc((4 * (size_t)n + 2) * sizeof *b.piece);
    b.tv = malloc((size_t)n * sizeof *b.tv);
    b.tn = malloc((size_t)n * sizeof *b.tn);
    b.tp = malloc((size_t)n * sizeof *b.tp);
    int why = DL_OK;
    if (!b.step || !b.piece || !b.tv || !b.tn || !b.tp) why = DL_CMD;
    int begun = 0, haveVtx = 0, type = 0;
    u32 prim = 0, vi[4] = {0}, i = 0;
    u8 pt[4] = {0, 1, 2, 3};  // submit_vertex's sTemp as ids: the pointer at each place
    s32 lastw[4] = {-1, -1, -1, -1};
    s16 xyz[3] = {0};
    while (i < n && !why) {
        u32 cmds = v[i], need = 0;
        for (u32 c = cmds; c; c >>= 8) need += kParams[c & 0xff];
        if (i + 1 + need > n) { why = DL_CUT; break; }
        i++;
        for (; cmds && !why; cmds >>= 8) {
            u8 c = cmds & 0xff;
            const u32 *p = &v[i];
            i += kParams[c];
            if (!c || c == 0x41) continue;
            if (c == 0x14 || c == 0x40) {  // a command run as it is: the piece before ends here
                if (!dl_flush(&b)) { why = DL_CMD; break; }
                b.step[b.nstep++] = (DlStep){c, p[0], 0};
                if (c == 0x40) type = p[0] & 3, b.inpoly = b.strip = 0, prim = 0, begun = 1;
                continue;
            }
            DlPiece *cur = &b.cur;
            if (c == 0x20) { cur->fck = 1, cur->fcw = p[0] & 0x7fff, b.any = 1; continue; }
            if (c == 0x21) {
                u32 key = p[0] & 0x3fffffff, k = 0;
                while (k < b.nn && b.tn[k] != key) k++;
                if (k == b.nn) b.tn[b.nn++] = key;
                cur->fck = 2, cur->fcw = k, cur->hasNormal = 1, cur->lastNormal = key, cur->nnormcmd++, b.any = 1;
                continue;
            }
            if (c == 0x22) { cur->ftk = 1, cur->ftw = p[0], b.any = 1; continue; }
            if (c < 0x23 || c > 0x28) { why = DL_CMD; break; }
            if (!begun) { why = DL_NOBEGIN; break; }
            if (!haveVtx && c != 0x23 && c != 0x24) { why = DL_RELATIVE; break; }
            haveVtx = 1;
            // the vertex commands as execute() has them
            switch (c) {
            case 0x23: xyz[0] = (s16)p[0]; xyz[1] = (s16)(p[0] >> 16); xyz[2] = (s16)p[1]; break;
            case 0x24:
                xyz[0] = (s16)((p[0] & 0x3ff) << 6);
                xyz[1] = (s16)((p[0] >> 10 & 0x3ff) << 6);
                xyz[2] = (s16)((p[0] >> 20 & 0x3ff) << 6);
                break;
            case 0x25: xyz[0] = (s16)p[0]; xyz[1] = (s16)(p[0] >> 16); break;
            case 0x26: xyz[0] = (s16)p[0]; xyz[2] = (s16)(p[0] >> 16); break;
            case 0x27: xyz[1] = (s16)p[0]; xyz[2] = (s16)(p[0] >> 16); break;
            case 0x28:
                xyz[0] += (s16)((p[0] & 0x3ff) << 6) >> 6;
                xyz[1] += (s16)((p[0] >> 10 & 0x3ff) << 6) >> 6;
                xyz[2] += (s16)((p[0] >> 20 & 0x3ff) << 6) >> 6;
                break;
            }
            if (++prim > DL_MAXPRIM) { why = DL_BIG; break; }
            // (what the vertex takes: the colour and texture coordinates the commands before it in the piece left)
            DlVtx *d = &b.tv[b.nvtx];
            memcpy(d->xyz, xyz, sizeof xyz);
            d->ck = cur->fck, d->cw = cur->fcw, d->tk = cur->ftk, d->tw = cur->ftw;
            b.any = 1;
            // submit_vertex's polygon bookkeeping, on vertex numbers
            lastw[pt[b.inpoly]] = (s32)b.nvtx;
            vi[b.inpoly] = b.nvtx++;
            b.inpoly++;
            if (b.inpoly != (type & 1 ? 4 : 3)) continue;
            DlPoly *pl = &b.tp[b.np++];
            pl->nv = type & 1 ? 4 : 3;
            if (type == 2 && (b.strip & 1)) pl->v[0] = vi[1], pl->v[1] = vi[0], pl->v[2] = vi[2];
            else if (type == 3) pl->v[0] = vi[0], pl->v[1] = vi[1], pl->v[2] = vi[3], pl->v[3] = vi[2];
            else for (u32 k = 0; k < pl->nv; k++) pl->v[k] = vi[k];
            if (type == 2) {
                b.strip++;
                u8 t = pt[0]; pt[0] = pt[1], pt[1] = pt[2], pt[2] = t;
                u32 u = vi[0]; vi[0] = vi[1], vi[1] = vi[2], vi[2] = u;
                b.inpoly = 2;
            } else if (type == 3) {
                b.strip++;
                u8 t0 = pt[0], t1 = pt[1];
                pt[0] = pt[2], pt[1] = pt[3], pt[2] = t0, pt[3] = t1;
                u32 u0 = vi[0], u1 = vi[1];
                vi[0] = vi[2], vi[1] = vi[3], vi[2] = u0, vi[3] = u1;
                b.inpoly = 2;
            } else b.inpoly = 0;
        }
    }
    if (!why && !dl_flush(&b)) why = DL_CMD;  // (the last piece)
    if (!why) {
        e->step = malloc((size_t)b.nstep * sizeof *b.step + 1);
        e->piece = malloc((size_t)b.npiece * sizeof *b.piece + 1);
        if (!e->step || !e->piece) why = DL_CMD;
        else {
            memcpy(e->step, b.step, b.nstep * sizeof *b.step), memcpy(e->piece, b.piece, b.npiece * sizeof *b.piece);
            e->nstep = b.nstep, e->npiece = b.npiece, e->nvtx = b.nvtx;
            for (u32 k = 0; k < b.npiece; k++)
                if (b.piece[k].nnormal > e->maxnormal) e->maxnormal = b.piece[k].nnormal;
            for (int k = 0; k < 4; k++) e->perm[k] = pt[k], e->lastw[k] = lastw[k];
        }
    }
    if (why) {  // (the pieces made are not the entry's)
        for (u32 k = 0; k < b.npiece && b.piece; k++) dl_piece_free(&b.piece[k]);
        dl_piece_free(&b.cur);
        free(e->step), free(e->piece);
        e->step = 0, e->piece = 0;
    }
    free(b.step), free(b.piece), free(b.tv), free(b.tn), free(b.tp);
    return why;
}

// the entry for a list sent (a hit: the same words at the same place), made again if not; NULL if not convertible.
// pre: the command word written before the block, 0 none
static DlEntry *dl_entry(const u32 *v, u32 n, u32 pre) {
    sDlStat.seen++;
    // (the pointer's own entry among four places from its hash, else a free one, else the first: with one place each
    // the town's 160 lists pushed each other out, one list in eight converted again)
    u32 h = ((uintptr_t)v >> 2) * 2654435761u >> 21;
    DlEntry *e = 0;
    for (u32 k = 0; k < 4; k++) {
        DlEntry *c = &sDl[(h + k) & (DL_SLOTS - 1)];
        if (c->ptr == v) { e = c; break; }
        if (!e && !c->state) e = c;
    }
    if (!e) e = &sDl[h & (DL_SLOTS - 1)];
    u32 skip = pre != 0;
    // (a buffer filled anew for every send, the SDK's command buffer: converting it each time costs more than running it)
    if (e->state == 1 && e->why == DL_CHANGING && e->ptr == v) { sDlStat.why[DL_CHANGING]++; return 0; }
    // shortcut: with ACWW_G3D_GPU four words of the list are compared with the copy, not all (the whole compare was a
    // sixth of the lists' time on the Old 3DS); a list changed only elsewhere is drawn as it was. All, if that shows.
    int same = e->state && e->ptr == v && e->n == n + skip && e->pre == skip && (!skip || e->words[0] == pre);
    if (same && sDlGpu && sDlGpu != 3) {
        for (u32 k = 0; k < 4; k++) same &= e->words[skip + k * (n - 1) / 3] == v[k * (n - 1) / 3];
    } else if (same) same = !memcmp(e->words + skip, v, n * sizeof *v);
    if (same)
        sDlStat.hits++;
    else {
        if (e->state) sDlStat.replaced++;
        u8 changed = e->state && e->ptr == v ? e->changed + 1 : 0;
        dl_free(e);
        if (changed >= 3) { e->ptr = v, e->state = 1, e->why = DL_CHANGING, e->changed = changed; sDlStat.why[DL_CHANGING]++; return 0; }
        e->words = malloc((size_t)(n + skip) * sizeof *v);
        if (!e->words) return 0;
        if (skip) e->words[0] = pre;
        memcpy(e->words + skip, v, n * sizeof *v);
        e->ptr = v, e->n = n + skip, e->pre = (u8)skip, e->changed = changed;
        e->why = (u8)dl_convert(e, e->words, n + skip);
        e->state = e->why ? 1 : 2;
        if (!e->why) sDlStat.converted++;
    }
    if (e->state == 1) { sDlStat.why[e->why]++; return 0; }
    return e;
}

// ACWW_G3D_DLSTATS=1 (or ACWW_G3D_DLCHECK=1): every 600 swaps what happened to the lists
static void dl_swap(void) {
    if (!(sDlStats || sDlCheck) || gG3Swaps % 600) return;
    fprintf(stderr, "g3d dl: swap %lu: lists %lu, cache hits %lu, converted %lu, replaced %lu; not convertible:",
            (unsigned long)gG3Swaps, sDlStat.seen, sDlStat.hits, sDlStat.converted, sDlStat.replaced);
    for (int k = 1; k < DL_WHYS; k++) fprintf(stderr, " %s %lu", kDlWhy[k], sDlStat.why[k]);
    fprintf(stderr, "; old path by the engine's state %lu; run converted %lu", sDlStat.old, sDlStat.ran);
    if (sDlGpu)
        fprintf(stderr, "; pieces drawn by the GPU %lu, by the CPU in its lists %lu; lists not: texture matrix %lu, the list's "
                "kind or no room %lu (%lu different lists); the GPU's pieces left out, their box outside the view, %lu, %lu of %lu "
                "vertices", sDlGpuDraws, sDlGpuCpu, sDlGpuWhy[1], sDlGpuWhy[2], sDlGpuNot, sDlBoxOut, sDlBoxOutV, sDlBoxV);
    if (sDlCheck) fprintf(stderr, "; checked %lu, differences %lu", sDlStat.checked, sDlStat.diffs);
    fprintf(stderr, "\n");
}

static void write32_n_impl(uint32_t addr, const uint32_t *v, uint32_t n);

// ---- running a converted list. The same integers as the old path (submit_vertex, submit_polygon's direct part,
// execute's cases 0x20-0x28 and g3p_polygon), in loops: what the commands of a piece leave changed, the vertices of
// the whole list in sLv at their numbers (a strip's polygon takes the vertices of the one before from there).
static G3LVertex *sLv;
static u32 sLvN;
static u8 (*sLit)[3];  // the colours of a piece's different normals
static u32 sLitN;

// room for the list's vertices and a piece's different normals' colours; 0 if there is none
static int dl_scratch(const DlEntry *e) {
    if (e->nvtx > sLvN) {
        G3LVertex *p = realloc(sLv, (size_t)e->nvtx * sizeof *p);
        if (!p) return 0;
        sLv = p, sLvN = e->nvtx;
    }
    if (e->maxnormal > sLitN) {
        void *p = realloc(sLit, (size_t)e->maxnormal * sizeof *sLit);
        if (!p) return 0;
        sLit = p, sLitN = e->maxnormal;
    }
    return g3p_list_begin(e->nvtx);
}

// the texture coordinates a TEXCOORD word gives (execute's case 0x22; the mode is 0 or 1 here)
static void dl_texcoord(u32 w, s16 raw[2], s16 coord[2]) {
    raw[0] = (s16)w;
    raw[1] = (s16)(w >> 16);
    if ((sTexParam >> 30) == 1) {
        for (int i = 0; i < 2; i++)
            coord[i] = (s16)(((s64)raw[0] * sTex.m[i] + (s64)raw[1] * sTex.m[4 + i] + sTex.m[8 + i] + sTex.m[12 + i]) >> 12);
    } else {
        coord[0] = raw[0];
        coord[1] = raw[1];
    }
}

// the polygon's part of submit_polygon (the port's) on the converted vertices
static void dl_polygon(const DlPoly *pl) {
    sPolyDone = 1;
    sPolySerial++;
    int nv = (int)pl->nv;
    const G3LVertex *v[4];
    for (int i = 0; i < nv; i++) v[i] = &sLv[pl->v[i]];
    s64 ax = v[1]->pos[0] - v[0]->pos[0], ay = v[1]->pos[1] - v[0]->pos[1], aw = v[1]->pos[3] - v[0]->pos[3];
    s64 bx = v[2]->pos[0] - v[0]->pos[0], by = v[2]->pos[1] - v[0]->pos[1], bw = v[2]->pos[3] - v[0]->pos[3];
    s64 nx = ay * bw - aw * by, ny = aw * bx - ax * bw, nw = ax * by - ay * bx;
    s64 dot = nx * v[0]->pos[0] + ny * v[0]->pos[1] + nw * v[0]->pos[3];
    int facing = dot >= 0;
    u32 attr = sCurPolyAttr;
    int culled = facing ? !(attr & 0x80) : !(attr & 0x40);
    sPolyOutcome = 0;
    sPolyNOut = 0;
    if (culled) {
        sStripValid = 0;
        sPolyOutcome = 1;
        return;
    }
    int far = 0, near = 0;
    for (int i = 0; i < nv; i++) {
        far += v[i]->pos[2] > v[i]->pos[3];
        near += v[i]->pos[2] < -v[i]->pos[3];
    }
    if ((far && !(attr & 0x1000)) || far == nv || near == nv) {
        sPolyOutcome = far ? 3 : 2;
        return;
    }
    if (sNumPolys >= MAX_POLYGONS) { sPolyOutcome = 5; return; }
    u32 alpha = attr >> 16 & 31;
    int fmt = sTexParam >> 26 & 7;
    g3p_list_polygon(pl->v, nv, attr, sTexParam, sTexPal, (alpha > 0 && alpha < 31) || fmt == 1 || fmt == 6,
                     sFlushParam >> 1 & 1);
    sNumPolys++;
    sNumVerts += nv;
}

// ---- ACWW_G3D_GPU=1: a converted list drawn by the GPU. Its pieces' vertices (object coordinates, the normal or
// the colour, the texture coordinates as sent) and triangles are put into the GPU's memory once (g3p_gpu_store);
// each time the list is sent a piece is one draw with the clip matrix and the lighting's values of that moment
// (g3p_gpu_draw), and the vertex shader transforms and lights. Not the DS's integers: the colours are not cut to 5
// bits, the positions and depths are the GPU's floats. Only for a list whose every piece has all its vertices lit or
// all coloured by COLOR commands, their texture coordinates and colours given in the list, and its polygons' vertices in the piece
// (not a strip across a MTX_RESTORE: a character's joints); a piece takes the CPU's way when a light's specular
// term isn't 0.
// shortcut: the polygons aren't counted (sNumPolys, the 2048 limit), the facing is the GPU's (the triangle's on the
// screen), and the positions of an unfinished primitive's vertices aren't kept; right while the lists are whole
// primitives, as the game's are.
// (what the list's pieces so far left for a vertex without its own: the last TEXCOORD word, and the last COLOR word
// (kind 1) or normal (kind 2); 0: from before the list, not known)
typedef struct { u32 tw, cw; u8 tk, kind; } DlCarry;
static int dl_gpu_store(DlPiece *pc, DlCarry *cy) {
    u32 ni = 0, bad = 0;
    for (u32 k = 0; k < pc->npoly; k++) {
        ni += (pc->poly[k].nv - 2) * 3;
        for (u32 i = 0; i < pc->poly[k].nv; i++) bad |= pc->poly[k].v[i] - pc->first >= pc->nvtx;
    }
    G3GVertex *gv = malloc(pc->nvtx * sizeof *gv + 1);
    u16 *ix = malloc(ni * sizeof *ix + 1);
    if (!gv || !ix) bad = 1;
    for (u32 j = 0; !bad && j < pc->nvtx; j++) {
        const DlVtx *d = &pc->vtx[j];
        G3GVertex *o = &gv[j];
        if (d->tk) cy->tw = d->tw, cy->tk = 1;
        if (d->ck) cy->kind = d->ck, cy->cw = d->ck == 2 ? pc->normal[d->cw] : d->cw;
        if (!j) pc->glit = cy->kind == 2;
        if (!cy->tk || !cy->kind || pc->glit != (cy->kind == 2)) { bad = 1; break; }
        memcpy(o->pos, d->xyz, sizeof d->xyz);
        o->pos[3] = o->cn[3] = 1;
        for (int c = 0; c < 3; c++) {
            if (!j || d->xyz[c] < pc->bb[c]) pc->bb[c] = (s16)d->xyz[c];
            if (!j || d->xyz[c] > pc->bb[3 + c]) pc->bb[3 + c] = (s16)d->xyz[c];
        }
        if (pc->glit) o->cn[0] = sx10(cy->cw), o->cn[1] = sx10(cy->cw >> 10), o->cn[2] = sx10(cy->cw >> 20);
        else
            for (int c = 0; c < 3; c++) o->cn[c] = (cy->cw >> 5 * c & 31) ? (s16)((cy->cw >> 5 * c & 31) * 2 + 1) : 0;
        o->tc[0] = (s16)cy->tw, o->tc[1] = (s16)(cy->tw >> 16);
    }
    for (u32 k = 0, n = 0; !bad && k < pc->npoly; k++)
        for (u32 i = 1; i + 1 < pc->poly[k].nv; i++) {
            ix[n++] = (u16)(pc->poly[k].v[0] - pc->first);
            ix[n++] = (u16)(pc->poly[k].v[i] - pc->first);
            ix[n++] = (u16)(pc->poly[k].v[i + 1] - pc->first);
        }
    pc->gcount = ni;
    int ok = !bad && (!ni || g3p_gpu_store(gv, pc->nvtx, ix, ni, &pc->gfirst));
    free(gv), free(ix);
    return ok;
}

// whether the list is in the GPU's memory (put there the first time)
static int dl_gpu_entry(DlEntry *e) {
    if (!e->gpu) {
        e->gpu = 1;
        DlCarry cy = {0};
        DlPiece *run = 0;  // (the run's first piece: it gets the box of all the run's vertices)
        for (u32 k = 0; k < e->nstep && e->gpu > 0; k++) {
            // (a normal's colour from under another matrix isn't the shader's for this one)
            if (e->step[k].cmd == 0x14 && cy.kind == 2) cy.kind = 0;
            if (e->step[k].cmd) {
                if (e->step[k].cmd != 0x40) run = 0;
                continue;
            }
            DlPiece *pc = &e->piece[e->step[k].piece];
            if (!dl_gpu_store(pc, &cy)) e->gpu = -1;
            int first = !run;
            if (first) run = pc, pc->run = 1;
            if (!pc->nvtx) {
                if (first) memcpy(pc->bb, (s16[6]){32767, 32767, 32767, -32768, -32768, -32768}, sizeof pc->bb);
                continue;
            }
            for (int c = 0; c < 3; c++) {
                if (pc->bb[c] < run->bb[c]) run->bb[c] = pc->bb[c];
                if (pc->bb[3 + c] > run->bb[3 + c]) run->bb[3 + c] = pc->bb[3 + c];
            }
        }
        if (e->gpu < 0) sDlGpuNot++;
    }
    return e->gpu > 0;
}

// Whether a run's box (its first piece's bb) is outside the view: beyond one clip plane with all of it (then so is every
// vertex, clip coordinates being linear in a vertex's), so the run's draws are left out (ACWW_G3D_CULL=0: drawn all
// the same). A run, not a piece: one test for the pieces that are one draw, and a draw is not cut in two by a piece
// left out in its middle (the test for every piece cost in the geometry what it saved in the draws). The planes are
// the wide view's at the sides (sClipNum / sClipDen); the test is the box's middle and half size against each.
// shortcut: the side planes an eighth of w further out for the stereoscopic shift, which is not worked out here.
static int sDlRunOut;  // (the run being run is left out)
static int dl_box_out(const DlPiece *pc) {
    double sDlPlane[6][4], sDlPlaneAbs[6][3];
    if (pc->bb[0] > pc->bb[3]) return 1;  // (a run without a vertex)
    {
        update_clip();
        for (int i = 0; i < 3; i++) {
            double f = (double)sClipNum[i] / sClipDen[i] + (i == 0 ? 0.125 : 0);
            for (int side = 0; side < 2; side++) {
                double *pl = sDlPlane[2 * i + side], sg = side ? -1 : 1;
                for (int j = 0; j < 3; j++) {
                    pl[j] = sg * sClip.m[4 * j + i] + f * sClip.m[4 * j + 3];
                    sDlPlaneAbs[2 * i + side][j] = pl[j] < 0 ? -pl[j] : pl[j];
                }
                pl[3] = 4096.0 * (sg * sClip.m[12 + i] + f * sClip.m[15]);
            }
        }
    }
    double c[3], e[3];
    for (int j = 0; j < 3; j++) c[j] = (pc->bb[j] + pc->bb[3 + j]) * 0.5, e[j] = (pc->bb[3 + j] - pc->bb[j]) * 0.5;
    for (int k = 0; k < 6; k++)
        if (sDlPlane[k][0] * c[0] + sDlPlane[k][1] * c[1] + sDlPlane[k][2] * c[2] + sDlPlane[k][3] +
            sDlPlaneAbs[k][0] * e[0] + sDlPlaneAbs[k][1] * e[1] + sDlPlaneAbs[k][2] * e[2] < 0) return 1;
    return 0;
}

// the piece as a draw; 0: the CPU's way
static int dl_piece_gpu(const DlPiece *pc) {
    u32 attr = sCurPolyAttr, pa = attr >> 16 & 31;
    if (!pc->gcount || !(attr & 0xc0)) return 1;  // (nothing to draw, or neither side is)
    G3GDraw d = {0};
    const float alpha = (float)(pa ? pa : 31) * (1 / 31.0f);
    if (pc->glit) {  // lighting()'s terms with the vector matrix taken into the lights' directions
        for (int k = 0; k < 3; k++) d.base[k] = (float)sEmission[k];
        for (int l = 0; l < 4; l++) {
            if (!(attr & (1u << l))) continue;
            for (int k = 0; k < 3; k++) {
                if (sSpecular[k] * sLightColor[l][k]) return 0;
                d.base[k] += (float)(sAmbient[k] * sLightColor[l][k]) * (1 / 32.0f);
                d.lcol[l][k] = (float)(sDiffuse[k] * sLightColor[l][k]) * (1 / (32.0f * 31));
            }
            for (int j = 0; j < 3; j++)  // (the direction 9 fractional bits, the matrix and the normal as kept 12)
                d.ldir[l][j] = -((float)sLightDir[l][0] * (float)sVec.m[4 * j] + (float)sLightDir[l][1] * (float)sVec.m[4 * j + 1] +
                                 (float)sLightDir[l][2] * (float)sVec.m[4 * j + 2]) * (1 / (512.0f * 4096 * 4096));
        }
        for (int k = 0; k < 3; k++) d.base[k] *= 1 / 31.0f;
        d.base[3] = alpha;
    } else {
        d.cscale[0] = d.cscale[1] = d.cscale[2] = 1 / 63.0f;
        d.cscale[3] = alpha;
    }
    if (sTexParam >> 30) {  // (dl_texcoord's sum; shortcut: not wrapped to 16 bits as the DS does, a texture's repeat hides it)
        if (sTex.m[1] || sTex.m[4]) return 0;  // (turned or sheared: the shader has a scale and a shift only)
        for (int i = 0; i < 2; i++)
            d.tex[i][0] = (float)sTex.m[5 * i] * (1 / 4096.0f), d.tex[i][1] = (float)(sTex.m[8 + i] + sTex.m[12 + i]) * (1 / 4096.0f);
    }
    update_clip();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) d.clip[4 * i + j] = (float)sClip.m[4 * j + i] * (1 / 16777216.0f);
        d.clip[4 * i + 3] = (float)sClip.m[12 + i] * (1 / 4096.0f);
    }
    int fmt = sTexParam >> 26 & 7;
    DL_T(3);
    int r = g3p_gpu_draw(pc->gfirst, pc->gcount, &d, attr, sTexParam, sTexPal, (pa > 0 && pa < 31) || fmt == 1 || fmt == 6,
                         sFlushParam >> 1 & 1);
    DL_T(4);
    return r;
}

// In a list drawn by the GPU: sDlMore, the piece before was drawn and only BEGIN_VTXS came since, so the next piece of
// the same kind is the same draw going on (to index sDlMoreEnd); sDlOwed, sVtxColor is still to be made from sNormal
// (the piece's last normal lit: needed only by what reads the colour, so made before the next command that isn't
// BEGIN_VTXS, before a piece the CPU makes, and at the list's end)
static int sDlMore, sDlMoreLit, sDlOwed;
static u32 sDlMoreEnd;
static void dl_owed(void) {
    if (sDlOwed) lighting(), sDlOwed = 0;
}

static void dl_piece(const DlPiece *pc) {
    int gpu = 0;
    if (sDlGpuNow) {
        sDlBoxV += pc->nvtx;
        if (pc->run) sDlRunOut = sDlCull && dl_box_out(pc);
        if (sDlRunOut) {  // (nothing of the run in the view: no draw)
            sDlBoxOut++, sDlBoxOutV += pc->nvtx;
            gpu = 1;
        } else if (sDlMore && pc->gfirst == sDlMoreEnd && pc->glit == sDlMoreLit) {
            g3p_gpu_more(pc->gcount);
            sDlMoreEnd += pc->gcount, gpu = 1;
        } else {
            gpu = dl_piece_gpu(pc);
            sDlMore = gpu && pc->gcount && (sCurPolyAttr & 0xc0);
            sDlMoreEnd = pc->gfirst + pc->gcount, sDlMoreLit = pc->glit;
        }
        gpu ? sDlGpuDraws++ : sDlGpuCpu++;
    }
    if (!gpu) dl_owed();
    u8 col0[3];  // (what the colour and texture coordinates were before the piece)
    s16 tc0[2] = {sTexCoord[0], sTexCoord[1]};
    memcpy(col0, sVtxColor, 3);
    for (u32 k = gpu ? pc->nnormal : 0; k < pc->nnormal; k++) {  // (each different normal lit once: lighting's inputs don't change in a piece)
        u32 w = pc->normal[k];
        sNormal[0] = sx10(w);
        sNormal[1] = sx10(w >> 10);
        sNormal[2] = sx10(w >> 20);
        lighting();
        memcpy(sLit[k], sVtxColor, 3);
    }
    if (gpu && pc->nvtx) memcpy(sVtx, pc->vtx[pc->nvtx - 1].xyz, sizeof sVtx);
    else if (pc->nvtx) {
        update_clip();
        if (sSeg < 0) sSeg = g3p_matrix(sClip.m);
        u32 lastTw = 0;
        s16 raw[2] = {0}, coord[2] = {0};
        int haveTw = 0;
        for (u32 j = 0; j < pc->nvtx; j++) {
            const DlVtx *d = &pc->vtx[j];
            G3LVertex *o = &sLv[pc->first + j];
            s64 x = d->xyz[0], y = d->xyz[1], z = d->xyz[2];
            for (int i = 0; i < 4; i++)
                o->pos[i] = (s32)((x * sClip.m[i] + y * sClip.m[4 + i] + z * sClip.m[8 + i] + 0x1000 * (s64)sClip.m[12 + i]) >> 12);
            u8 cw[3];
            const u8 *c = col0;
            if (d->ck == 1) cw[0] = d->cw & 31, cw[1] = d->cw >> 5 & 31, cw[2] = d->cw >> 10 & 31, c = cw;
            else if (d->ck == 2) c = sLit[d->cw];
            for (int i = 0; i < 3; i++) o->color[i] = c[i] ? c[i] << 4 | 0xf : 0;
            if (d->tk) {
                if (!haveTw || lastTw != d->tw) dl_texcoord(d->tw, raw, coord), lastTw = d->tw, haveTw = 1;
                o->tex[0] = coord[0], o->tex[1] = coord[1];
            } else {
                o->tex[0] = tc0[0], o->tex[1] = tc0[1];
            }
            o->obj[0] = d->xyz[0], o->obj[1] = d->xyz[1], o->obj[2] = d->xyz[2];
            o->seg = (u16)sSeg;
        }
        g3p_list_vertices(pc->first, &sLv[pc->first], pc->nvtx, sCurPolyAttr, sFlushParam >> 1 & 1);
        for (u32 k = 0; k < pc->npoly; k++) dl_polygon(&pc->poly[k]);
        memcpy(sVtx, pc->vtx[pc->nvtx - 1].xyz, sizeof sVtx);
    }
    // what the piece's commands leave in the engine
    sVtxCount += (int)pc->nvtx;
    sTickVerts += pc->nvtx;
    sTickNormals += pc->nnormcmd;
    if (pc->fck == 0) memcpy(sVtxColor, col0, 3);
    else if (pc->fck == 1) sVtxColor[0] = pc->fcw & 31, sVtxColor[1] = pc->fcw >> 5 & 31, sVtxColor[2] = pc->fcw >> 10 & 31, sDlOwed = 0;
    else if (gpu) sDlOwed = 1;  // (the last normal is the one left in sNormal below)
    else memcpy(sVtxColor, sLit[pc->fcw], 3);
    if (pc->ftk) dl_texcoord(pc->ftw, sTexRaw, sTexCoord);
    if (pc->hasNormal) {
        sNormal[0] = sx10(pc->lastNormal);
        sNormal[1] = sx10(pc->lastNormal >> 10);
        sNormal[2] = sx10(pc->lastNormal >> 20);
    }
    sVtxInPoly = pc->inpoly;
    sStripPolys = pc->strip;
    sDlPieces[sDlKind]++, sDlVerts[sDlKind] += pc->nvtx;
    DL_T(sDlKind ? 6 + sDlKind : 5);
}

static void dl_run(const DlEntry *e) {
    G3Vertex *old[4];
    memcpy(old, sTemp, sizeof old);
    // (ACWW_G3D_GPU=2: the lists under the texture matrix left to the CPU; 3: dl_entry compares the whole list)
    sDlGpuNow = sDlGpu && !sDlCheck && !(sDlGpu == 2 && sTexParam >> 30) && dl_gpu_entry((DlEntry *)e);
    if (sDlGpu && !sDlGpuNow) sDlGpuWhy[sDlCheck ? 0 : sTexParam >> 30 ? 1 : 2]++;
    sDlLists++;
    sDlKind = sDlGpuNow ? 0 : sTexParam >> 30 ? 1 : 2;
    sDlMore = 0, sDlRunOut = 0;
    DL_T(1);
    for (u32 k = 0; k < e->nstep; k++) {
        const DlStep *st = &e->step[k];
        if (st->cmd) {
            if (st->cmd != 0x40) dl_owed(), sDlMore = 0;
            execute(st->cmd, &st->param);
            sDlCmds[st->cmd & 255]++;
            DL_T(2);
        } else dl_piece(&e->piece[st->piece]);
    }
    dl_owed();
    // the vertices of an unfinished polygon, and the other buffers' last vertices, as submit_vertex leaves them
    for (int i = 0; i < 4; i++) {
        sTemp[i] = old[e->perm[i]];
        if (e->lastw[i] < 0) continue;
        G3Vertex *g = old[i];
        const G3LVertex *l = &sLv[e->lastw[i]];
        memcpy(g->pos, l->pos, sizeof g->pos);
        for (int c = 0; c < 3; c++) g->color[c] = l->color[c], g->obj[c] = l->obj[c];
        g->tex[0] = l->tex[0], g->tex[1] = l->tex[1];
        g->seg = l->seg;
        g->clipped = 0, g->farAxis = 0, g->dbgClips = 0, g->screenDone = 0;
    }
    if (e->pre) sPackCmds = 0, sPackLeft = 0, sPackParamsLeft = 0;  // (the command word's commands all done)
    DL_T(6);
}

// ---- ACWW_G3D_DLCHECK=1: every list run on the new path, then again on the old path from the same state; the
// engine's state and the GPU's list after each compared. Both run, the old path's result kept.
#define DL_STATE(X) \
    X(sVtx) X(sVtxColor) X(sTexRaw) X(sTexCoord) X(sNormal) X(sTemp) X(sTempBuf) X(sPolyType) X(sCurPolyAttr) \
    X(sInBegin) X(sVtxInPoly) X(sStripPolys) X(sStripValid) X(sStripRam) X(sPos) X(sVec) X(sProj) X(sTex) X(sClip) \
    X(sClipDirty) X(sSeg) X(sMtxMode) X(sProjSP) X(sTexSP) X(sPosSP) X(sGxStat) X(sLightGen) X(sNormGen) X(sNumPolys) \
    X(sNumVerts) X(sNumOpaque) X(sPolyDone) X(sPolyOutcome) X(sPolyNOut) X(sPolyClipped) X(sFarHidden) X(sPolySerial) \
    X(sVtxCount) X(sTickVerts) X(sTickNormals) X(sPackCmds) X(sPackLeft) X(sPackParamsLeft) X(sRamOverflow) X(sPolyAttr) \
    X(sTexParam) X(sTexPal)
typedef struct {
#define X(f) typeof(f) f;
    DL_STATE(X)
#undef X
} DlState;
static void dl_take(DlState *s) {
#define X(f) memcpy(&s->f, &f, sizeof f);
    DL_STATE(X)
#undef X
}
static void dl_put(const DlState *s) {
#define X(f) memcpy(&f, &s->f, sizeof f);
    DL_STATE(X)
#undef X
}

static void dl_report(const DlEntry *e, const char *what, const char *name, long at) {
    if (sDlStat.diffs++ < 20)
        fprintf(stderr, "g3d dlcheck: list %p, %lu words: %s %s %ld\n", (const void *)e->ptr, (unsigned long)e->n, what, name, at);
}

static void dl_check(const DlEntry *e, const u32 *v, u32 n) {
    static DlState s0, s1, s2;
    G3pMark mk;
    g3p_list_mark(&mk);
    dl_take(&s0);
    dl_run(e);
    dl_take(&s1);
    int nv1, nb1;
    void *d1 = g3p_list_since(&mk, &nv1, &nb1);
    dl_put(&s0);
    g3p_list_reset(&mk);
    sDlBusy = 1;
    write32_n_impl(0x04000400, v, n);
    sDlBusy = 0;
    dl_take(&s2);
    int nv2, nb2;
    void *d2 = g3p_list_since(&mk, &nv2, &nb2);
    sDlStat.checked++;
    int d = g3p_list_diff(d1, nv1, nb1, d2, nv2, nb2);
    if (d >= 0x20000) dl_report(e, "list counts or last count", "differ", d - 0x20000);
    else if (d >= 0x10000) dl_report(e, "batch", "differs", d - 0x10000);
    else if (d >= 0) dl_report(e, "vertex", "differs", d);
#define X(f) \
    if (memcmp(&s1.f, &s2.f, sizeof s1.f)) { \
        long at = 0; \
        while (((const char *)&s1.f)[at] == ((const char *)&s2.f)[at]) at++; \
        dl_report(e, "state", #f, at); \
    }
    DL_STATE(X)
#undef X
    free(d1), free(d2);
}

// whether the block is a list of the converted kind, and if so run: 1 (the block is done), else 0 (the old path
// decodes it). pre: the command word written before the block, 0 none.
static int dl_try(const u32 *v, u32 n, u32 pre) {
    dl_options();
    if ((!sDlOn && !sDlStats) || sDlBusy) return 0;
    if (sBenchSkip & 16) { if (pre) sPackCmds = 0, sPackLeft = 0, sPackParamsLeft = 0; return 1; }  // (ACWW_G3D_BENCH: the lists left out, to time what is outside them)
    if (sBenchSkip || (sTexParam >> 30) >= 2 || g3p_recorded()) { sDlStat.old++; return 0; }
    if (sDlSplit) sDlMark = host_plat_ticks();
    DlEntry *e = dl_entry(v, n, pre);
    if (!e || !sDlOn || !dl_scratch(e)) return 0;
    DL_T(0);
    if (sDlCheck) dl_check(e, v, n);
    else dl_run(e);
    sDlStat.ran++;
    return 1;
}

// The console ports' GPU vertex lists (gpu3d_internal.h g3p_*), shared by the 3DS's gpu3d_pica.c and the Switch's
// gpu3d_gl.c, each of which includes this once (its statics are the backend's): the polygons as the geometry engine
// makes them, in batches of one state; each vertex's object coordinates and clip matrix kept, so that a list can be
// drawn again from other matrices: the pictures between the game's (the game draws a 3D picture every third frame);
// the display lists drawn by the GPU (ACWW_G3D_GPU). The backend defines PVertex (24 bytes: clip position as floats,
// colour and alpha as bytes, the DS's texture coordinates as shorts), MAX_POLYGONS' list sizes come from
// gpu3d_internal.h, and G3_LINEAR_ALLOC (memory the GPU reads); G3_NO_GPU_LISTS: no display lists drawn by the GPU.
// (Moved out of gpu3d_pica.c, 2026-11-18, as it was.)
// ---- the port's vertex lists: the polygons as the geometry engine makes them
// (g3p_polygon, on the game's thread), in batches of the same state, drawn opaque first, then translucent, each in
// the order made (the game sorts its translucent polygons itself: manual sort). Three lists in turn: one filled, one
// drawn, and the one drawn before it.
// shortcut: the list filled next is the one drawn two swaps ago, taken as done by the GPU (a swap is a frame or more,
// a 3D picture 2-4 ms of the GPU's); wait for the GPU there if it ever isn't.
// Each vertex also keeps its object coordinates and its clip matrix (Rec, Seg: normal memory), so that the picture
// can be drawn again from other matrices (replay: the pictures between the game's).
enum { LISTS = 3, LIST_VERTS = MAX_POLYGONS * 6, MAX_SEGS = 1024, NO_SEG = 0xffff };  // (a quad is two triangles)
enum { GPU_VERTS = 12288, GPU_INDICES = 49152, GPU_DRAWS = 512 };  // (the display lists drawn by the GPU: below)
typedef struct {
    u32 attr, texparam, texpal;
    u8 trans, wbuffer;
    int first, count;  // its vertices in the list
    float vx[2], vy[2];  // its viewport (sVx, sVy)
    int gpu;  // a display list's draw (g3p_gpu_draw): its number in the list's draws + 1; first, count: its indices
} Batch;
typedef struct {
    s32 obj[3];
    u16 seg;  // its clip matrix (NO_SEG: the list had too many; replayed as drawn)
} Rec;
typedef struct {
    s32 m[16];       // a clip matrix (20.12)
    s32 m0[16];      // the matrix it is paired with in the list before (pair; m: none), the pictures between from it
    s32 anchor[3];   // the object coordinates of the first vertex under it
    u32 sig;         // what is drawn with it: a hash of its first vertices' object coordinates, n of them
    int n;
} Seg;
typedef struct {
    PVertex *v;
    Rec *rec;  // (a vertex's, as v)
    int nv, npoly, nbatch, nseg;
    int recorded;  // its matrices (Seg) and vertices' object coordinates (Rec) kept: recording()
    Batch batch[MAX_POLYGONS + GPU_DRAWS];
    Seg *seg;  // MAX_SEGS
    G3GDraw *draw;  // GPU_DRAWS (made with the first one)
    int ndraw;
} List;
static List sList[LISTS];
static int sFill;
// the replayed vertices drawn (linear memory), two in turn: one written while the GPU may still read the other
static PVertex *sReplay[2];
static int sReplayBuf;
// the viewport: the DS screen's clip coordinates from the geometry engine's (x' = x * sVx[0] + w * sVx[1], and y')
static float sVx[2] = {1, 0}, sVy[2] = {1, 0};

void g3p_viewport(const u32 vp[6]) {
    // (pixels: x = (x / w + 1) * width / 2 + x1, y = (1 - y / w) * height / 2 + top; the screen's clip space has
    // x / w = x / 128 - 1, y / w = 1 - y / 96)
    sVx[0] = (float)vp[4] / 256, sVx[1] = (float)(vp[4] + 2 * vp[0]) / 256 - 1;
    sVy[0] = (float)vp[5] / 192, sVy[1] = 1 - (float)vp[5] / 192 - (float)vp[3] / 96;
}

// what the pictures between the game's and the replay's check need, kept only for them (decided at a list's start;
// without them the matrices' copies and the vertices' records were a tenth of the vertices' work: an Old 3DS)
static int interp_on(void);
static int recording(void) {
    static int check = -1;  // (ACWW_PICA_REPLAY, prepare)
    if (check < 0) check = getenv("ACWW_PICA_REPLAY") && atoi(getenv("ACWW_PICA_REPLAY"));
    return check || interp_on();
}

int g3p_matrix(const s32 clip[16]) {
    List *l = &sList[sFill];
    if (!l->recorded || !l->seg || l->nseg >= MAX_SEGS) return NO_SEG;
    Seg *s = &l->seg[l->nseg];
    memcpy(s->m, clip, sizeof s->m);
    s->sig = 2166136261u, s->n = 0;
    return l->nseg++;
}

void g3p_vertex(int seg, const s32 obj[3]) {
    if (seg == NO_SEG) return;
    Seg *s = &sList[sFill].seg[seg];
    if (s->n >= 4) return;
    if (!s->n) memcpy(s->anchor, obj, sizeof s->anchor);
    for (int i = 0; i < 3; i++) s->sig = (s->sig ^ (u32)obj[i]) * 16777619u;  // (FNV-1a)
    s->n++;
}

// a vertex's clip coordinates (the geometry engine's, x 4096) as the GPU's: the viewport applied, the depth as the
// DS's (Z or W); eq: the depth test "equal"'s margin
static void clip_pos(float o[4], const s32 p[4], const float vx[2], const float vy[2], int wbuffer, float eq) {
    const float k = 1 / 4096.0f, w = (float)p[3] * k;
    o[0] = (float)p[0] * k * vx[0] + w * vx[1];
    o[1] = (float)p[1] * k * vy[0] + w * vy[1];
    if (wbuffer) {  // (the depth W itself, 24 bits: not divided by w, C3D_DepthMap; clipped at w > 0 only)
        float d = (float)p[3] * (1 / 16777215.0f) - eq;
        o[2] = d < 0 ? 0 : d > 1 ? -1 : -d;
    } else {  // (z / w from -1 to 1 into the GPU's 0 to -1: the near and far planes the DS's)
        o[2] = ((float)p[2] * k + w) * -0.5f + eq * w;
    }
    o[3] = w;
}
// (depth test "equal" passes within 0x200 on the DS, gpu3d_raster.c depth_pass: moved nearer by that, then less or
// equal)
static float depth_eq(u32 attr) { return attr & 0x4000 ? 0x200 / 16777215.0f : 0; }

// a vertex through m, as the geometry engine (gpu3d_geom.c submit_vertex)
static void transform(s32 p[4], const s32 m[16], const s32 obj[3]) {
    s64 x = obj[0], y = obj[1], z = obj[2];
    for (int c = 0; c < 4; c++)
        p[c] = (s32)((x * m[c] + y * m[4 + c] + z * m[8 + c] + 0x1000 * (s64)m[12 + c]) >> 12);
}

// the list's vertices again into out at t (16 bits) between their seg's pair: each from its object coordinates
// through the paired matrix (m0) and its own (m), the two positions interpolated, the colours and texture coordinates
// as drawn; returns the vertices whose position differs from the list's (t = 1: none). The positions, not the
// matrices, are interpolated: vertices the game made the same in both pictures (a seam between two pieces, a decal on
// a surface) stay the same between them, where a matrix rounded element by element moved them apart.
static int replay(const List *l, s64 t, PVertex *out) {
    int differ = 0;
    for (int k = 0; k < l->nbatch; k++) {
        const Batch *b = &l->batch[k];
        float eq = depth_eq(b->attr);
        // (the last three vertices made: a polygon's triangles share its first vertex and an edge, and polygons in a
        // row share vertices; the same object coordinates and matrix give the same position)
        int recent[3] = {-1, -1, -1}, next = 0;
        for (int i = b->first; i < b->first + b->count; i++) {
            const Rec *r = &l->rec[i];
            out[i] = l->v[i];
            if (r->seg == NO_SEG) continue;
            int j = -1;
            for (int c = 0; c < 3 && j < 0; c++) {
                const Rec *q = recent[c] < 0 ? 0 : &l->rec[recent[c]];
                if (q && q->seg == r->seg && q->obj[0] == r->obj[0] && q->obj[1] == r->obj[1] && q->obj[2] == r->obj[2])
                    j = recent[c];
            }
            if (j >= 0) {
                memcpy(out[i].pos, out[j].pos, sizeof out[i].pos);
                differ += memcmp(out[i].pos, l->v[i].pos, sizeof out[i].pos) != 0;
                continue;
            }
            recent[next] = i, next = (next + 1) % 3;
            const Seg *sg = &l->seg[r->seg];
            s32 p[4], p0[4];
            transform(p, sg->m, r->obj);
            if (t < 0x10000) {
                transform(p0, sg->m0, r->obj);
                for (int c = 0; c < 4; c++) p[c] = p0[c] + (s32)(((s64)(p[c] - p0[c]) * t) >> 16);
            }
            clip_pos(out[i].pos, p, b->vx, b->vy, b->wbuffer, eq);
            differ += memcmp(out[i].pos, l->v[i].pos, sizeof out[i].pos) != 0;
        }
    }
    return differ;
}

// ---- the pictures between the game's: the game draws a 3D picture every third frame
// (gVBlanksPerFrame = 3); the frames between show the list again with each clip matrix moved from its pair in the
// list before towards its own, so the newest picture is shown in full one game frame late (t = 1 on its last frame).
// The pairs: the matrices under which the same vertices are drawn (Seg.sig), the nearest on the screen of those;
// none (not interpolated) when none is within a quarter of the screen (for a vertex off the screen, a quarter of
// its distance from the middle). Each matrix of the list before goes to the nearest only (and to those as near: the
// same drawn again): one new in the picture, as a tree coming in at the wide screen's edge or the horizon, else took
// its neighbour's and was drawn between the two, gone from its place for a frame. Fewer than half paired: a cut, none.
static u16 sHead[1024], sNext[MAX_SEGS], sClaim[MAX_SEGS];  // sClaim: a matrix of the list before's nearest, + 1 (0: none)
static s16 sFound[MAX_SEGS];
static float sBest[MAX_SEGS];
static u32 sInterpLists, sInterpSegs, sInterpPaired, sInterpCuts, sInterpBatches, sInterpVerts;

// obj's position on the screen (x / w, y / w) and its depth (w) through m; 0 behind the eye
static int screen_of(const s32 m[16], const s32 obj[3], float xyw[3]) {
    s64 x = obj[0], y = obj[1], z = obj[2], p[4];
    for (int c = 0; c < 4; c++) p[c] = x * m[c] + y * m[4 + c] + z * m[8 + c] + 0x1000 * (s64)m[12 + c];
    if (p[3] <= 0) return 0;
    float iw = 1.0f / (float)p[3];
    xyw[0] = (float)p[0] * iw, xyw[1] = (float)p[1] * iw, xyw[2] = (float)p[3];
    return 1;
}

static void pair(List *l, const List *pl) {
    int paired = 0;
    memset(sHead, 0xff, sizeof sHead);
    for (int i = 0; i < pl->nseg; i++) {
        u32 h = pl->seg[i].sig & 1023;
        sNext[i] = sHead[h], sHead[h] = (u16)i;
    }
    for (int i = 0; i < l->nseg; i++) {
        Seg *sg = &l->seg[i];
        memcpy(sg->m0, sg->m, sizeof sg->m);
        sFound[i] = -1;
        float a[3], b[3];
        int found = -1, tries = 0;
        if (!sg->n || !screen_of(sg->m, sg->anchor, a)) continue;
        // (a vertex off the screen gets a quarter of its distance from the middle: one near the eye, as a large piece of
        // ground's first, moves a screen or more from one picture to the next)
        float r = fmaxf(1, fmaxf(fabsf(a[0]), fabsf(a[1]))), radius = 0.25f * 0.25f * r * r, best = INFINITY;
        for (u16 j = sHead[sg->sig & 1023]; j != 0xffff && tries < 32; j = sNext[j]) {
            const Seg *q = &pl->seg[j];
            if (q->sig != sg->sig || q->n != sg->n) continue;
            tries++;  // (32 at most: many of the same, as particles, cost nothing much)
            if (!screen_of(q->m, sg->anchor, b)) continue;
            // (the depth too, relative: of two alike one behind the other on the screen, as two trees, the one in front
            // took the one behind's and was drawn between, smaller)
            float d = (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]), e = (a[2] - b[2]) / a[2];
            if (d < radius && d + e * e < best) best = d + e * e, found = j;
        }
        sFound[i] = (s16)found, sBest[i] = best;
        if (found >= 0 && (!sClaim[found] || best < sBest[sClaim[found] - 1])) sClaim[found] = (u16)(i + 1);
    }
    for (int i = 0; i < l->nseg; i++) {
        int j = sFound[i];
        if (j >= 0 && sBest[i] == sBest[sClaim[j] - 1]) memcpy(l->seg[i].m0, pl->seg[j].m, sizeof l->seg[i].m0), paired++;
    }
    for (int i = 0; i < l->nseg; i++)
        if (sFound[i] >= 0) sClaim[sFound[i]] = 0;
    sInterpLists++, sInterpSegs += (u32)l->nseg, sInterpPaired += (u32)paired;
    sInterpBatches += (u32)l->nbatch, sInterpVerts += (u32)l->nv;
    if (paired * 2 < l->nseg) {
        for (int i = 0; i < l->nseg; i++) memcpy(l->seg[i].m0, l->seg[i].m, sizeof l->seg[i].m0);
        sInterpCuts++;
    }
    static int stats = -1;  // (ACWW_PICA_STATS=1: every 300 lists)
    if (stats < 0) stats = getenv("ACWW_PICA_STATS") && atoi(getenv("ACWW_PICA_STATS"));
    if (stats && sInterpLists % 300 == 0) {
        fprintf(stderr, "pica interp: %u lists, %u matrices, %u paired, %u cuts; a list %u draws (batches), %u vertices\n",
                (unsigned)sInterpLists, (unsigned)sInterpSegs, (unsigned)sInterpPaired, (unsigned)sInterpCuts,
                (unsigned)(sInterpBatches / 300), (unsigned)(sInterpVerts / 300));
        sInterpSegs = sInterpPaired = sInterpCuts = sInterpBatches = sInterpVerts = 0;
    }
}

// the phase, counted on the game's thread: frames (g3p_between, every frame) since the list shown was first drawn,
// and the frames between it and the one before
static int sInterp = -1, sShownList = -1, sPeriod;
static u32 sFrames, sListFrame;

static int interp_on(void) {  // ACWW_PICA_INTERP=0: off
    if (sInterp < 0) sInterp = !getenv("ACWW_PICA_INTERP") || atoi(getenv("ACWW_PICA_INTERP"));
    return sInterp;
}
// the replayed vertices' buffers, made when the pictures between or the replay's check are first wanted (590 KB of
// linear memory an Old 3DS, where they are off, has no room for); NULL: drawn as made
static void replay_alloc(void) {
    if (sReplay[0]) return;
    PVertex *p = G3_LINEAR_ALLOC(LIST_VERTS * sizeof(PVertex));
    if ((sReplay[1] = p)) sReplay[0] = G3_LINEAR_ALLOC(LIST_VERTS * sizeof(PVertex));  // ([0] last: it is what is asked)
}
void g3p_set_interp(int on) {  // (the options page: the game's thread, like interp_on)
    sInterp = on != 0;
    if (on) replay_alloc();
}
int g3p_between(const G3RenderState *r) {
    // shortcut: counts g3_render's frames, every frame only with ACWW_FRAMESKIP=0 (the port's default)
    sFrames++;
    if (!interp_on()) return 0;
    return r->list == sShownList && sPeriod >= 2 && sPeriod <= 4 && (int)(sFrames - sListFrame) < sPeriod;
}

// the texture coordinates' scale: 1 / (16 * size) by the texture's size field
static const float kSize[8] = {1 / 128.0f, 1 / 256.0f, 1 / 512.0f, 1 / 1024.0f,
                               1 / 2048.0f, 1 / 4096.0f, 1 / 8192.0f, 1 / 16384.0f};

// the batch the next polygon of this state joins: the list's last if its state is the same, else a new one
static Batch *choose_batch(List *l, u32 attr, u32 texparam, u32 texpal, int translucent, int wbuffer) {
    Batch *b = l->nbatch ? &l->batch[l->nbatch - 1] : 0;
    if (!b || b->gpu || b->attr != attr || b->texparam != texparam || b->texpal != texpal || b->wbuffer != wbuffer ||
        memcmp(b->vx, sVx, sizeof sVx) || memcmp(b->vy, sVy, sizeof sVy)) {
        b = &l->batch[l->nbatch++];
        b->attr = attr, b->texparam = texparam, b->texpal = texpal, b->trans = (u8)translucent;
        b->wbuffer = (u8)wbuffer, b->first = l->nv, b->count = 0, b->gpu = 0;
        memcpy(b->vx, sVx, sizeof sVx), memcpy(b->vy, sVy, sizeof sVy);
    }
    return b;
}

// whether the list being filled keeps its matrices and vertices' object coordinates (recording)
int g3p_recorded(void) { return sList[sFill].recorded; }

void g3p_polygon(G3Vertex *const *v, int n, u32 attr, u32 texparam, u32 texpal, int translucent, int wbuffer) {
    List *l = &sList[sFill];
    if (!l->v || !l->rec || l->npoly >= MAX_POLYGONS || l->nv + (n - 2) * 3 > LIST_VERTS) return;
    l->npoly++;
    Batch *b = choose_batch(l, attr, texparam, texpal, translucent, wbuffer);
    const float eq = depth_eq(attr);
    u32 pa = attr >> 16 & 31;
    u8 alpha = (u8)(pa ? pa : 31);
    PVertex pv[10];
    Rec rv[10];
    for (int i = 0; i < n; i++) {
        const G3Vertex *x = v[i];
        PVertex *o = &pv[i];
        clip_pos(o->pos, x->pos, b->vx, b->vy, wbuffer, eq);
        if (l->recorded) memcpy(rv[i].obj, x->obj, sizeof rv[i].obj), rv[i].seg = x->seg;
        for (int c = 0; c < 3; c++) o->col[c] = (u8)(x->color[c] >> 3);
        o->col[3] = alpha;
        o->tc[0] = x->tex[0], o->tc[1] = x->tex[1];
    }
    PVertex *out = l->v + l->nv;
    for (int i = 1; i + 1 < n; i++) *out++ = pv[0], *out++ = pv[i], *out++ = pv[i + 1];
    Rec *ro = l->rec + l->nv;
    for (int i = 1; i + 1 < n && l->recorded; i++) *ro++ = rv[0], *ro++ = rv[i], *ro++ = rv[i + 1];
    l->nv += (n - 2) * 3;
    b->count += (n - 2) * 3;
}

// ---- the display lists run converted (gpu3d_geom_list.h): the vertices of a whole list are made first, into sLpv at
// their numbers in the list, then each polygon takes its vertices from there (as g3p_polygon, for a list that isn't
// recorded)
static PVertex *sLpv;
static u32 sLpvN;
int g3p_list_begin(u32 n) {
    if (n > sLpvN) {
        PVertex *p = realloc(sLpv, (size_t)n * sizeof *p);
        if (!p) return 0;
        sLpv = p, sLpvN = n;
    }
    return 1;
}
void g3p_list_vertices(u32 first, const G3LVertex *v, u32 n, u32 attr, int wbuffer) {
    const float eq = depth_eq(attr);
    u32 pa = attr >> 16 & 31;
    u8 alpha = (u8)(pa ? pa : 31);
    for (u32 i = 0; i < n; i++) {
        PVertex *o = &sLpv[first + i];
        clip_pos(o->pos, v[i].pos, sVx, sVy, wbuffer, eq);
        for (int c = 0; c < 3; c++) o->col[c] = (u8)(v[i].color[c] >> 3);
        o->col[3] = alpha;
        o->tc[0] = v[i].tex[0], o->tc[1] = v[i].tex[1];
    }
}
void g3p_list_polygon(const u32 *idx, int n, u32 attr, u32 texparam, u32 texpal, int translucent, int wbuffer) {
    List *l = &sList[sFill];
    if (!l->v || !l->rec || l->npoly >= MAX_POLYGONS || l->nv + (n - 2) * 3 > LIST_VERTS) return;
    l->npoly++;
    Batch *b = choose_batch(l, attr, texparam, texpal, translucent, wbuffer);
    PVertex *out = l->v + l->nv;
    for (int i = 1; i + 1 < n; i++) *out++ = sLpv[idx[0]], *out++ = sLpv[idx[i]], *out++ = sLpv[idx[i + 1]];
    l->nv += (n - 2) * 3;
    b->count += (n - 2) * 3;
}

// ---- the display lists drawn by the GPU (ACWW_G3D_GPU, gpu3d_geom_list.h): a list's vertices in object coordinates
// with their normals are put once into linear memory, with the indices of its triangles; each time the list is sent
// it is one draw from there (a batch of its own, in the order made), with the matrix and the lighting's values of
// that moment for the vertex shader.
// shortcut: the room is never given back (a list whose words change, or more lists than fit, stay on the CPU's path);
// a cache that frees old lists when a game area needs more than GPU_VERTS.
static G3GVertex *sGv;
static u16 *sGi;
static u32 sGvN, sGiN;
int g3p_gpu_store(const G3GVertex *v, u32 nv, u16 *idx, u32 ni, u32 *first) {
#ifdef G3_NO_GPU_LISTS  // (a backend that doesn't draw them: the lists stay on the geometry engine's path)
    (void)v, (void)nv, (void)idx, (void)ni, (void)first;
    return 0;
#endif
    static int failed;
    if (!sGv && !failed) {  // (failed 1: no memory; 2: full)
        sGv = G3_LINEAR_ALLOC(GPU_VERTS * sizeof *sGv), sGi = G3_LINEAR_ALLOC(GPU_INDICES * sizeof *sGi);
        if (!sGv || !sGi) {
            failed = 1;
            fprintf(stderr, "pica: no linear memory for the display lists' vertices: they stay on the CPU's path\n");
        }
    }
    if (!failed && (sGvN + nv > GPU_VERTS || sGiN + ni > GPU_INDICES)) {
        failed = 2;  // (the lists so far stay)
        fprintf(stderr, "pica: the display lists' memory is full (%lu vertices, %lu indices): the others stay on the CPU's path\n",
                (unsigned long)sGvN, (unsigned long)sGiN);
    }
    if (failed) return 0;
    memcpy(sGv + sGvN, v, nv * sizeof *v);
    for (u32 i = 0; i < ni; i++) sGi[sGiN + i] = (u16)(idx[i] + sGvN);
    *first = sGiN;
    sGvN += nv, sGiN += ni;  // (after the data: render flushes what these count)
    return 1;
}
int g3p_gpu_draw(u32 first, u32 count, const G3GDraw *d, u32 attr, u32 texparam, u32 texpal, int translucent, int wbuffer) {
    List *l = &sList[sFill];
    if (!l->draw) l->draw = malloc(GPU_DRAWS * sizeof *l->draw);
    if (!l->draw) return 0;
    Batch *b = l->nbatch ? &l->batch[l->nbatch - 1] : 0;
    // (a list's next stretch after a BEGIN_VTXS, nothing changed: the draw before goes on)
    if (b && b->gpu && b->first + b->count == (int)first && b->attr == attr && b->texparam == texparam &&
        b->texpal == texpal && b->wbuffer == wbuffer && !memcmp(b->vx, sVx, sizeof sVx) && !memcmp(b->vy, sVy, sizeof sVy) &&
        !memcmp(&l->draw[b->gpu - 1], d, sizeof *d)) {
        b->count += (int)count;
        return 1;
    }
    if (l->ndraw >= GPU_DRAWS) return 0;
    b = &l->batch[l->nbatch++];
    b->attr = attr, b->texparam = texparam, b->texpal = texpal, b->trans = (u8)translucent, b->wbuffer = (u8)wbuffer;
    b->first = (int)first, b->count = (int)count;
    memcpy(b->vx, sVx, sizeof sVx), memcpy(b->vy, sVy, sizeof sVy);
    l->draw[l->ndraw] = *d;
    b->gpu = ++l->ndraw;
    return 1;
}

// the draw g3p_gpu_draw made last goes on with the next count indices (the caller knows nothing changed since)
void g3p_gpu_more(u32 count) {
    List *l = &sList[sFill];
    l->batch[l->nbatch - 1].count += (int)count;
}

// ACWW_G3D_DLCHECK: a list run twice, from the same counts: what the first run added kept to be compared with the
// second's
void g3p_list_mark(G3pMark *m) {
    const List *l = &sList[sFill];
    m->nv = l->nv, m->npoly = l->npoly, m->nbatch = l->nbatch;
    m->lastCount = l->nbatch ? l->batch[l->nbatch - 1].count : 0;
}
void g3p_list_reset(const G3pMark *m) {
    List *l = &sList[sFill];
    l->nv = m->nv, l->npoly = m->npoly, l->nbatch = m->nbatch;
    if (m->nbatch) l->batch[m->nbatch - 1].count = m->lastCount;
}
// (the vertices, the batches added, then the last old batch's count)
void *g3p_list_since(const G3pMark *m, int *nv, int *nb) {
    const List *l = &sList[sFill];
    *nv = l->nv - m->nv, *nb = l->nbatch - m->nbatch;
    size_t vs = (size_t)*nv * sizeof(PVertex), bs = (size_t)*nb * sizeof(Batch);
    char *p = malloc(vs + bs + sizeof(int));
    if (!p) return 0;
    memcpy(p, l->v + m->nv, vs);
    memcpy(p + vs, l->batch + m->nbatch, bs);
    int last = m->nbatch ? l->batch[m->nbatch - 1].count : 0;
    memcpy(p + vs + bs, &last, sizeof last);
    return p;
}
int g3p_list_diff(const void *a, int nva, int nba, const void *b, int nvb, int nbb) {
    if (!a || !b) return 0x30000;
    if (nva != nvb) return 0x20000;  // (the counts)
    if (nba != nbb) return 0x20001;
    const char *pa = a, *pb = b;
    for (int i = 0; i < nva; i++)
        if (memcmp(pa + i * sizeof(PVertex), pb + i * sizeof(PVertex), sizeof(PVertex))) return i;  // a vertex
    pa += (size_t)nva * sizeof(PVertex), pb += (size_t)nva * sizeof(PVertex);
    for (int i = 0; i < nba; i++)
        if (memcmp(pa + i * sizeof(Batch), pb + i * sizeof(Batch), sizeof(Batch))) return 0x10000 + i;  // a batch
    pa += (size_t)nba * sizeof(Batch), pb += (size_t)nba * sizeof(Batch);
    return memcmp(pa, pb, sizeof(int)) ? 0x20002 : -1;  // (the last old batch's count)
}

// (ACWW_G3D_BENCH: the list being filled emptied again)
void g3p_bench_reset(void) { sList[sFill].nv = sList[sFill].npoly = sList[sFill].nbatch = sList[sFill].nseg = sList[sFill].ndraw = 0; }

// ACWW_G3D_LISTHASH=1: at each swap one line with a hash (FNV-1a) of the list handed over: its vertices and each
// batch's fields (not the structs' bytes: padding). For comparing two runs' lists.
static u32 fnv(u32 h, const void *p, size_t n) {
    for (const u8 *b = p; n--; b++) h = (h ^ *b) * 16777619u;
    return h;
}
static void list_hash(const List *l) {
    static int on = -1;
    static u32 swaps;
    if (on < 0) on = getenv("ACWW_G3D_LISTHASH") && atoi(getenv("ACWW_G3D_LISTHASH"));
    if (!on) return;
    u32 h = fnv(2166136261u, l->v, (size_t)l->nv * sizeof *l->v);
    for (int i = 0; i < l->nbatch; i++) {
        const Batch *b = &l->batch[i];
        h = fnv(h, &b->attr, 4), h = fnv(h, &b->texparam, 4), h = fnv(h, &b->texpal, 4);
        h = fnv(h, &b->trans, 1), h = fnv(h, &b->wbuffer, 1), h = fnv(h, &b->first, 4), h = fnv(h, &b->count, 4);
        h = fnv(h, b->vx, sizeof b->vx), h = fnv(h, b->vy, sizeof b->vy);
    }
    fprintf(stderr, "g3d listhash %lu %d %d %08lx\n", (unsigned long)++swaps, l->nv, l->nbatch, (unsigned long)h);
}

int g3p_swap(void) {
    int handed = sFill;
    list_hash(&sList[handed]);
    sFill = (sFill + 1) % LISTS;
    sList[sFill].nv = sList[sFill].npoly = sList[sFill].nbatch = sList[sFill].nseg = sList[sFill].ndraw = 0;
    sList[sFill].recorded = recording();
    return handed;
}

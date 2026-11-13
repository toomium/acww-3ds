// The port's benchmark of the geometry engine (N3DS_PORT, ACWW_G3D_BENCH): one frame's writes to the GX
// FIFO recorded and run again. Included once in gpu3d_geom.c, after write32_n_impl: it needs the file's statics.

// ACWW_G3D_BENCH=<frame>: one game frame's writes to the GX FIFO recorded (the first frame begun at or after that
// one, from one swap to the next), then run again BENCH_RUNS times with nothing of the game in between: what a
// vertex costs in the game against what the geometry engine alone takes for the same commands. Printed once.
// shortcut: the frame's commands run again leave the engine's state as the frame's end did only if the frame sets
// its own state up (the game's frames do); the picture after may be wrong once. A measurement, not for playing.
enum { BENCH_BLOCKS = 16384, BENCH_WORDS = 65536, BENCH_RUNS = 20 };
typedef struct { u32 addr, n; } BenchBlock;  // (n's top bit: a single write, gpu3d_write32)
static long sBenchAt = -2;
static int sBenchState;  // 0 waiting for the frame, 1 recording, 2 recorded (the swap is pending), 3 done
static BenchBlock *sBenchBlock;
static u32 *sBenchWord, sBenchBlocks, sBenchWords;
static u64 sBenchTicks, sBenchVerts;

// a write while recording: run and timed as ACWW_G3D_TICKS does, kept unless it brought the swap (the frame's end)
static void bench_record(uint32_t addr, const uint32_t *v, uint32_t n, int single) {
    u64 t = host_plat_ticks();
    if (single) write32_impl(addr, v[0]);
    else write32_n_impl(addr, v, n);
    t = host_plat_ticks() - t;
    if (sFlushPending) { sBenchState = 2; return; }
    if (sBenchBlocks == BENCH_BLOCKS || sBenchWords + n > BENCH_WORDS) {
        fprintf(stderr, "g3d bench: the frame is too long to record\n");
        sBenchState = 3;
        return;
    }
    sBenchBlock[sBenchBlocks++] = (BenchBlock){addr, n | (single ? 0x80000000u : 0)};
    memcpy(sBenchWord + sBenchWords, v, n * sizeof *v);
    sBenchWords += n;
    sBenchTicks += t;
}

// the recorded frame run BENCH_RUNS times, each run into an emptied list: the ticks of all, and in *each (if given)
// those timed block by block as in the game (two tick calls a block)
static u64 bench_runs(u64 *each) {
    u64 t0 = host_plat_ticks();
    for (int k = 0; k < BENCH_RUNS; k++) {
        const u32 *w = sBenchWord;
        for (u32 b = 0; b < sBenchBlocks; b++) {
            u32 n = sBenchBlock[b].n & 0x7fffffffu;
            u64 t = each ? host_plat_ticks() : 0;
            if (sBenchBlock[b].n >> 31) write32_impl(sBenchBlock[b].addr, w[0]);
            else write32_n_impl(sBenchBlock[b].addr, w, n);
            if (each) *each += host_plat_ticks() - t;
            w += n;
        }
        sNumPolys = sNumVerts = 0, sStripValid = 0, sSeg = -1;
        g3p_bench_reset();
    }
    return host_plat_ticks() - t0;
}

// after a swap: the recording begun, or the recorded frame run again (twice BENCH_RUNS times: timed block by block
// as in the game, then in one go, then in one go with parts left out: bench_runs)
static void bench_frame(void) {
    if (sBenchAt == -2) sBenchAt = getenv("ACWW_G3D_BENCH") ? atol(getenv("ACWW_G3D_BENCH")) : -1;
    if (sBenchAt < 0 || sBenchState == 3) return;
    if (sBenchState == 0 || sPipeN || sFifoN) {  // (commands waiting behind the swap: this frame not, the next)
        if ((long)sFrame < sBenchAt) return;
        if (!sBenchBlock) sBenchBlock = malloc(BENCH_BLOCKS * sizeof *sBenchBlock);
        if (!sBenchWord) sBenchWord = malloc(BENCH_WORDS * sizeof *sBenchWord);
        if (!sBenchBlock || !sBenchWord) { sBenchState = 3; return; }
        sBenchBlocks = sBenchWords = 0, sBenchTicks = 0, sBenchVerts = sTickVerts;
        sBenchState = 1;
        return;
    }
    if (sBenchState != 2) return;
    u64 verts = sTickVerts - sBenchVerts, keepV = sTickVerts, keepN = sTickNormals, each = 0, whole, part[4];
    static const int kSkip[4] = {1, 2, 4, 2 | 8};  // (without the GPU's list; lighting; polygons; lighting and vertices)
    bench_runs(&each);
    whole = bench_runs(0);
    for (int i = 0; i < 4; i++) {
        sBenchSkip = kSkip[i];
        part[i] = bench_runs(0);
    }
    sBenchSkip = 32;  // (32: nothing left out, but the display lists on the old path; 16: the lists left out)
    u64 oldPath = bench_runs(0);
    sBenchSkip = 16;
    u64 keepV2 = sTickVerts;
    u64 noLists = bench_runs(0);
    sCmdMark = host_plat_ticks(), sCmdLast = 0;  // (command 0, a NOP: also what comes before a run's first command)
    sCmdStat = 1;
    bench_runs(0);
    sCmdStat = 0;
    u64 outside = (sTickVerts - keepV2) / BENCH_RUNS;
    sBenchSkip = 16 | 2 | 8;
    u64 noListsBare = bench_runs(0);
    sBenchSkip = 0;
    if (!verts) verts = 1;
    fprintf(stderr, "g3d bench: frame %llu: %lu blocks, %lu words, %llu vertices; in the game %llu ticks (%llu a vertex); "
            "run again %d times: block by block %llu ticks a run (%llu a vertex), in one go %llu (%llu)\n",
            (unsigned long long)sFrame, (unsigned long)sBenchBlocks, (unsigned long)sBenchWords, (unsigned long long)verts,
            (unsigned long long)sBenchTicks, (unsigned long long)(sBenchTicks / verts), BENCH_RUNS,
            (unsigned long long)(each / BENCH_RUNS), (unsigned long long)(each / BENCH_RUNS / verts),
            (unsigned long long)(whole / BENCH_RUNS), (unsigned long long)(whole / BENCH_RUNS / verts));
    fprintf(stderr, "g3d bench: ticks a vertex in one go without the GPU's list %llu, without lighting %llu, without "
            "polygons %llu, without lighting and vertices %llu\n",
            (unsigned long long)(part[0] / BENCH_RUNS / verts), (unsigned long long)(part[1] / BENCH_RUNS / verts),
            (unsigned long long)(part[2] / BENCH_RUNS / verts), (unsigned long long)(part[3] / BENCH_RUNS / verts));
    fprintf(stderr, "g3d bench: ticks a run in one go: %llu, with the display lists on the old path %llu, without the "
            "lists %llu (%llu of the vertices are outside them)\n", (unsigned long long)(whole / BENCH_RUNS),
            (unsigned long long)(oldPath / BENCH_RUNS), (unsigned long long)(noLists / BENCH_RUNS),
            (unsigned long long)outside);
    fprintf(stderr, "g3d bench outside the lists: %llu ticks a run without lighting and vertices; commands (with a "
            "timing each, see below):", (unsigned long long)(noListsBare / BENCH_RUNS));
    for (int c = 0; c < 256; c++)
        if (sCmdN[c] || sCmdT[c])
            fprintf(stderr, " %02x: %lu (%llu ticks)", c, sCmdN[c] / BENCH_RUNS, (unsigned long long)(sCmdT[c] / BENCH_RUNS));
    fprintf(stderr, "\n");
    memset(sDlT, 0, sizeof sDlT), memset(sDlCmds, 0, sizeof sDlCmds), sDlLists = 0;
    memset(sDlPieces, 0, sizeof sDlPieces), memset(sDlVerts, 0, sizeof sDlVerts);
    sDlSplit = 1;
    bench_runs(0);
    u64 t1 = host_plat_ticks(), keepT = sDlT[0];
    for (int i = 0; i < 1000; i++) DL_T(0);  // (what the timing itself costs)
    t1 = host_plat_ticks() - t1, sDlT[0] = keepT;
    sDlSplit = 0;
    fprintf(stderr, "g3d bench lists: a run: %lu lists; pieces (vertices) the GPU's %lu (%lu), texture matrix %lu (%lu), "
            "can't %lu (%lu); ticks: lookup %llu, list's start %llu, commands %llu, draw's values %llu, g3p_gpu_draw %llu, "
            "the GPU's pieces' rest %llu, list's end %llu, texture matrix pieces %llu, can't pieces %llu (a timing costs "
            "%llu); commands:", sDlLists / BENCH_RUNS, sDlPieces[0] / BENCH_RUNS, sDlVerts[0] / BENCH_RUNS,
            sDlPieces[1] / BENCH_RUNS, sDlVerts[1] / BENCH_RUNS, sDlPieces[2] / BENCH_RUNS, sDlVerts[2] / BENCH_RUNS,
            (unsigned long long)(sDlT[0] / BENCH_RUNS), (unsigned long long)(sDlT[1] / BENCH_RUNS),
            (unsigned long long)(sDlT[2] / BENCH_RUNS), (unsigned long long)(sDlT[3] / BENCH_RUNS),
            (unsigned long long)(sDlT[4] / BENCH_RUNS), (unsigned long long)(sDlT[5] / BENCH_RUNS),
            (unsigned long long)(sDlT[6] / BENCH_RUNS), (unsigned long long)(sDlT[7] / BENCH_RUNS),
            (unsigned long long)(sDlT[8] / BENCH_RUNS), (unsigned long long)(t1 / 1000));
    for (int c = 0; c < 256; c++)
        if (sDlCmds[c]) fprintf(stderr, " %02x: %lu", c, sDlCmds[c] / BENCH_RUNS);
    fprintf(stderr, "\n");
    // ACWW_G3D_PMU=1 (with Luma3DS: plat_3ds.c): the frame run again under the CPU's counters, two events a time,
    // every event number of the ARM11 MPCore's list (0x00-0x13: cache misses, stalls, branches, instructions)
    if (getenv("ACWW_G3D_PMU") && atoi(getenv("ACWW_G3D_PMU"))) {
        fprintf(stderr, "g3d bench pmu: a run's counts (%llu vertices):", (unsigned long long)verts);
        for (int ev = 0; ev < 0x14; ev += 2) {
            uint32_t c[3];
            if (!host_plat_pmu_start(ev, ev + 1)) { fprintf(stderr, " the counters can't be reached"); break; }
            bench_runs(0);
            host_plat_pmu_stop(c);
            if (!ev) fprintf(stderr, " cycles %lu;", (unsigned long)(c[0] / BENCH_RUNS));
            fprintf(stderr, " %02x: %lu, %02x: %lu,", ev, (unsigned long)(c[1] / BENCH_RUNS), ev + 1,
                    (unsigned long)(c[2] / BENCH_RUNS));
        }
        fprintf(stderr, "\n");
        // (the same per part, as the ticks above: instructions, data dependency stalls, instruction cache misses, loads)
        static const int kPart[5] = {0, 1, 2, 4, 2 | 8};
        fprintf(stderr, "g3d bench pmu: a run's instructions (08), stall cycles (02), instruction cache misses (00), "
                "loads (0a): whole, without the GPU's list, lighting, polygons, lighting and vertices:");
        for (int i = 0; i < 5; i++) {
            uint32_t a[3] = {0}, b[3] = {0};
            sBenchSkip = kPart[i];
            if (host_plat_pmu_start(0x08, 0x02)) { bench_runs(0); host_plat_pmu_stop(a); }
            if (host_plat_pmu_start(0x00, 0x0a)) { bench_runs(0); host_plat_pmu_stop(b); }
            fprintf(stderr, " %lu %lu %lu %lu;", (unsigned long)(a[1] / BENCH_RUNS), (unsigned long)(a[2] / BENCH_RUNS),
                    (unsigned long)(b[1] / BENCH_RUNS), (unsigned long)(b[2] / BENCH_RUNS));
        }
        sBenchSkip = 0;
        fprintf(stderr, "\n");
    }
    sTickVerts = keepV, sTickNormals = keepN;  // (ACWW_G3D_TICKS's counts: the game's only)
    free(sBenchBlock), free(sBenchWord);
    sBenchBlock = 0, sBenchWord = 0;
    sBenchState = 3;
}

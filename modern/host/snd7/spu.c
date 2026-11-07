// Host build: the DS sound hardware in software. 16 channels (PCM8, PCM16, IMA-ADPCM on all; square waves on 8-13,
// noise on 14-15), each stepped by its own timer at 16.76 MHz; a mixer with per-channel volume, divider and pan,
// the master volume and the output selectors; two capture units that record the mixer into memory (the effects
// that play back through channels 1 and 3). The output is a stereo sample every 1024 bus cycles (32728 Hz).
//
// Channel samples are held between timer steps (no interpolation), as on the DS. Sample data is read from DS
// memory at mix time, so the game's buffers must stay valid while a channel plays them, as on the DS.

#include <string.h>

#include "snd7.h"

#define SAMPLE_CYCLES 1024u  // bus cycles per output sample
#define TIMER_STEP 512u      // the channel timers' ticks (16.76 MHz) per output sample

SpuChannelRegs spu_ch[16];
u16 spu_soundcnt;
u8 spu_capcnt[2];
uintptr_t spu_capdad[2];
u16 spu_caplen[2];

typedef struct {
    u32 timer;      // counts up from tmr to 0x10000: the next sample
    s32 pos;        // samples (nibbles for ADPCM, header included) since the start; negative: the start delay
    s32 cur;        // the sample being output
    s32 adpcm, adpcm_idx, loop_adpcm, loop_adpcm_idx;
    u16 lfsr;
} ChState;
static ChState sCh[16];

static struct {
    u32 timer;
    u32 pos;  // bytes written
} sCap[2];

static u64 sMixed;  // bus cycle the output has been generated up to

static const s8 kAdpcmIndex[8] = {-1, -1, -1, -1, 2, 4, 6, 8};
static const u16 kAdpcmStep[89] = {
    7,     8,     9,     10,    11,    12,    13,    14,    16,    17,    19,    21,    23,    25,    28,
    31,    34,    37,    41,    45,    50,    55,    60,    66,    73,    80,    88,    97,    107,   118,
    130,   143,   157,   173,   190,   209,   230,   253,   279,   307,   337,   371,   408,   449,   494,
    544,   598,   658,   724,   796,   876,   963,   1060,  1166,  1282,  1411,  1552,  1707,  1878,  2066,
    2272,  2499,  2749,  3024,  3327,  3660,  4026,  4428,  4871,  5358,  5894,  6484,  7132,  7845,  8630,
    9493,  10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767};
static const u8 kVolShift[4] = {4, 3, 2, 0};  // the divider: /1, /2, /4, /16 of a sample shifted up by 4

// sample data: the channel's buffer, from SOUNDxSAD to the end of its loop ((SOUNDxPNT + SOUNDxLEN) words; the
// IMA-ADPCM header at least); anything else (no buffer, past the end) reads as silence
static inline int readable(const SpuChannelRegs *r, u32 off, u32 n) {
    u32 extent = (u32)r->pnt * 4 + r->len * 4;
    return r->sad && off + n <= (extent < 4 ? 4 : extent);
}
static inline s8 rd8(const SpuChannelRegs *r, u32 off) { return readable(r, off, 1) ? *(const s8 *)(r->sad + off) : 0; }
static inline s16 rd16(const SpuChannelRegs *r, u32 off) { return readable(r, off, 2) ? *(const s16 *)(r->sad + off) : 0; }

static inline u32 format_of(int ch) { return spu_ch[ch].cnt >> 29 & 3; }
static inline u32 repeat_of(int ch) { return spu_ch[ch].cnt >> 27 & 3; }
static inline void stop(int ch) { spu_ch[ch].cnt &= ~0x80000000u; }

void spu_cnt_written(int ch, u32 old) {
    if (!(old & 0x80000000u) && (spu_ch[ch].cnt & 0x80000000u)) {
        ChState *s = &sCh[ch];
        s->timer = spu_ch[ch].tmr;
        s->pos = format_of(ch) == 3 ? -1 : -3;
        s->cur = 0;
        s->lfsr = 0x7fff;
    }
}

void spu_capcnt_written(int idx, u8 old) {
    if (!(old & 0x80) && (spu_capcnt[idx] & 0x80)) {
        sCap[idx].timer = spu_ch[idx * 2 + 1].tmr;  // capture 0 runs on channel 1's timer, capture 1 on channel 3's
        sCap[idx].pos = 0;
    }
}

// the end of the data: loop or stop; returns 0 when the channel stopped
static int at_end(int ch) {
    if (repeat_of(ch) & 1) return 1;  // loop (the caller rewinds)
    stop(ch);
    return 0;
}

static void next_sample(int ch) {
    SpuChannelRegs *r = &spu_ch[ch];
    ChState *s = &sCh[ch];
    u32 loop_bytes = (u32)r->pnt * 4, total_bytes = loop_bytes + r->len * 4;
    s->pos++;
    if (s->pos < 0) return;
    switch (format_of(ch)) {
    case 0:  // PCM8
        if ((u32)s->pos >= total_bytes) {
            if (!at_end(ch)) return;
            s->pos = (s32)loop_bytes;
        }
        s->cur = rd8(r, (u32)s->pos) << 8;
        break;
    case 1:  // PCM16
        if ((u32)s->pos * 2 >= total_bytes) {
            if (!at_end(ch)) return;
            s->pos = (s32)(loop_bytes / 2);
        }
        s->cur = rd16(r, (u32)s->pos * 2);
        break;
    case 2: {  // IMA-ADPCM: a 4-byte header (first value, step index), then 4-bit codes, low nibble first
        if (s->pos < 8) {
            if (s->pos == 0) {
                s->adpcm = rd16(r, 0);
                s->adpcm_idx = (u8)rd8(r, 2) & 0x7f;
                if (s->adpcm_idx > 88) s->adpcm_idx = 88;
                s->loop_adpcm = s->adpcm;
                s->loop_adpcm_idx = s->adpcm_idx;
            }
            return;
        }
        if ((u32)s->pos >= total_bytes * 2) {
            if (!at_end(ch)) return;
            s->pos = (s32)(loop_bytes * 2);
            s->adpcm = s->loop_adpcm;
            s->adpcm_idx = s->loop_adpcm_idx;
        } else if ((u32)s->pos == loop_bytes * 2) {
            s->loop_adpcm = s->adpcm;
            s->loop_adpcm_idx = s->adpcm_idx;
        }
        u32 code = (u8)rd8(r, (u32)s->pos >> 1);
        if (s->pos & 1) code >>= 4;
        u32 step = kAdpcmStep[s->adpcm_idx];
        s32 diff = (s32)(step >> 3);
        if (code & 1) diff += (s32)(step >> 2);
        if (code & 2) diff += (s32)(step >> 1);
        if (code & 4) diff += (s32)step;
        if (code & 8) {
            s->adpcm -= diff;
            if (s->adpcm < -0x7fff) s->adpcm = -0x7fff;
        } else {
            s->adpcm += diff;
            if (s->adpcm > 0x7fff) s->adpcm = 0x7fff;
        }
        s->adpcm_idx += kAdpcmIndex[code & 7];
        if (s->adpcm_idx < 0) s->adpcm_idx = 0;
        else if (s->adpcm_idx > 88) s->adpcm_idx = 88;
        s->cur = s->adpcm;
        break;
    }
    case 3:
        if (ch >= 8 && ch <= 13) {  // square wave: duty (n+1)/8 high, 7 = always low
            u32 duty = r->cnt >> 24 & 7;
            s->cur = duty != 7 && (u32)(7 - (s->pos & 7)) <= duty ? 0x7fff : -0x7fff;
        } else if (ch >= 14) {  // noise: a 15-bit LFSR
            if (s->lfsr & 1) {
                s->lfsr = (u16)(s->lfsr >> 1 ^ 0x6000);
                s->cur = -0x7fff;
            } else {
                s->lfsr >>= 1;
                s->cur = 0x7fff;
            }
        } else {
            s->cur = 0;
        }
        break;
    }
}

// one output sample's worth of a channel: its output before pan (sample << shift * volume), 0 when off
static s32 run_channel(int ch) {
    SpuChannelRegs *r = &spu_ch[ch];
    if (!(r->cnt & 0x80000000u)) return 0;
    if (format_of(ch) != 3 && ((u32)r->pnt + r->len) * 4 < 16) return 0;
    ChState *s = &sCh[ch];
    s->timer += TIMER_STEP;
    while (s->timer >= 0x10000) {
        s->timer = r->tmr + (s->timer - 0x10000);
        next_sample(ch);
        if (!(r->cnt & 0x80000000u)) break;
    }
    return (s->cur << kVolShift[r->cnt >> 8 & 3]) * (s32)(r->cnt & 0x7f);
}

static inline void pan_out(int ch, s32 v, s64 *l, s64 *r) {
    s32 pan = (s32)(spu_ch[ch].cnt >> 16 & 0x7f);
    *l += ((s64)v * (128 - pan)) >> 10;
    *r += ((s64)v * pan) >> 10;
}

static inline s32 clamp16(s64 v) { return v < -0x8000 ? -0x8000 : v > 0x7fff ? 0x7fff : (s32)v; }

static void capture(int idx, s32 v) {
    if (!(spu_capcnt[idx] & 0x80)) return;
    sCap[idx].timer += TIMER_STEP;
    u32 len = (u32)(spu_caplen[idx] ? spu_caplen[idx] : 1) * 4;
    while (sCap[idx].timer >= 0x10000) {
        sCap[idx].timer = spu_ch[idx * 2 + 1].tmr + (sCap[idx].timer - 0x10000);
        // (the capture buffer: SNDCAPxDAD, SNDCAPxLEN words)
        uintptr_t a = spu_capdad[idx] + sCap[idx].pos;
        if (spu_capcnt[idx] & 8) {  // PCM8
            if (spu_capdad[idx] && sCap[idx].pos + 1 <= len) *(s8 *)a = (s8)(v >> 8);
            sCap[idx].pos += 1;
        } else {
            if (spu_capdad[idx] && sCap[idx].pos + 2 <= len) *(s16 *)a = (s16)v;
            sCap[idx].pos += 2;
        }
        if (sCap[idx].pos >= len) {
            sCap[idx].pos = 0;
            if (spu_capcnt[idx] & 4) {  // one-shot
                spu_capcnt[idx] &= ~0x80;
                break;
            }
        }
    }
}

static void mix_one(s16 out[2]) {
    if (!(spu_soundcnt & 0x8000)) {
        out[0] = out[1] = 0;
        return;
    }
    s64 l = 0, r = 0;
    s32 v[16];
    for (int i = 0; i < 16; i++) v[i] = run_channel(i);
    pan_out(0, v[0], &l, &r);
    pan_out(2, v[2], &l, &r);
    if (!(spu_soundcnt & 0x1000)) pan_out(1, v[1], &l, &r);
    if (!(spu_soundcnt & 0x2000)) pan_out(3, v[3], &l, &r);
    for (int i = 4; i < 16; i++) pan_out(i, v[i], &l, &r);
    // capture: the mixer (or channel 0 / 2 alone), 16 bits
    capture(0, spu_capcnt[0] & 2 ? clamp16(v[0] >> 11) : clamp16(l >> 8));
    capture(1, spu_capcnt[1] & 2 ? clamp16(v[2] >> 11) : clamp16(r >> 8));
    s64 lo = 0, ro = 0, dummy = 0;
    switch (spu_soundcnt >> 8 & 3) {
    case 0: lo = l; break;
    case 1: pan_out(1, v[1], &lo, &dummy); break;
    case 2: pan_out(3, v[3], &lo, &dummy); break;
    case 3: pan_out(1, v[1], &lo, &dummy); pan_out(3, v[3], &lo, &dummy); break;
    }
    switch (spu_soundcnt >> 10 & 3) {
    case 0: ro = r; break;
    case 1: pan_out(1, v[1], &dummy, &ro); break;
    case 2: pan_out(3, v[3], &dummy, &ro); break;
    case 3: pan_out(1, v[1], &dummy, &ro); pan_out(3, v[3], &dummy, &ro); break;
    }
    s32 mv = spu_soundcnt & 0x7f;
    out[0] = (s16)clamp16((lo * mv >> 7) >> 8);
    out[1] = (s16)clamp16((ro * mv >> 7) >> 8);
}

void spu_mix_until(u64 cycles) {
#ifndef N3DS_DIRECT  // (the enhanced port mixes on a thread of its own: the game's thread's profile mark isn't its)
    HOST_PROF(PROF_SOUND);
#endif
    s16 buf[512 * 2];
    u32 n = 0;
    while (sMixed + SAMPLE_CYCLES <= cycles) {
        mix_one(&buf[n * 2]);
        sMixed += SAMPLE_CYCLES;
        if (++n == 512) {
            snd7_output(buf, n);
            n = 0;
        }
    }
    if (n) snd7_output(buf, n);
}

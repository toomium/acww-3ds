// Host build: the ARM7 sound driver's main loop, commands, alarms, bank lookup and helpers (NitroSDK
// SND_main.c / SND_command.c / SND_work.c / SND_alarm.c / SND_bank.c / SND_util.c). See snd7.h.
//
// The DS runs the driver in an ARM7 thread that wakes every 2728 OS ticks (5.2 ms) from an alarm (a pass that
// steps the sequences, envelopes and LFOs) and when the ARM9 asks it to process commands (a pass that only
// applies them). Here both are passes of snd7_pass(): the periodic one on the host's virtual clock
// (snd7_run, from the event loop), the other one as soon as a command list arrives.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snd7.h"
#ifdef N3DS_DIRECT
#include "../plat.h"
#endif

#define OS_TICK_CYCLES 64u               // the ARM7's OS tick: the 33.5 MHz bus clock / 64
#define SND_INTERVAL (2728u * OS_TICK_CYCLES)
#define SND_START_DELAY (0x10000u * OS_TICK_CYCLES)

struct SNDWork snd7_work;
struct SNDSharedWork *snd7_shared;

static u64 sNextTick = SND_START_DELAY;  // SND_StartIntervalTimer at the ARM7's start
static int sTimerStopped;

#ifdef N3DS_DIRECT
// The enhanced port: the driver on a thread of its own (snd7_thread), clocked by the sound
// it mixes for the DSP, as the DS's ARM7 runs beside the ARM9: the music goes on while the game's thread is held up
// (a load, a slow frame). The game's thread hands over the command lists (snd7_receive) and passes the alarms'
// messages on to the ARM9 (snd7_run); everything else here is the thread's.
static u64 sClock;  // the thread's bus cycles: the sound mixed so far
#define NOW() sClock
#else
#define NOW() host_cycles()
#endif

// ---- tables (ACWW's ARM7 program, 0x0238f1a8-0x0238f2f3; the BIOS pitch table is computed)
static const s8 sLfoSinTable[33] = {0,  6,  12, 19, 25, 31,  37,  43,  49,  54,  60,  65,  71,  76,  81,  85, 90,
                                    94, 98, 102, 106, 109, 112, 115, 117, 120, 122, 123, 125, 126, 126, 127, 127};
const s16 snd7_decibel_square_table[128] = {
    -723, -722, -721, -651, -601, -562, -530, -503, -480, -460, -442, -425, -410, -396, -383, -371,
    -360, -349, -339, -330, -321, -313, -305, -297, -289, -282, -276, -269, -263, -257, -251, -245,
    -239, -234, -229, -224, -219, -214, -210, -205, -201, -196, -192, -188, -184, -180, -176, -173,
    -169, -165, -162, -158, -155, -152, -149, -145, -142, -139, -136, -133, -130, -127, -125, -122,
    -119, -116, -114, -111, -109, -106, -103, -101, -99,  -96,  -94,  -91,  -89,  -87,  -85,  -82,
    -80,  -78,  -76,  -74,  -72,  -70,  -68,  -66,  -64,  -62,  -60,  -58,  -56,  -54,  -52,  -50,
    -49,  -47,  -45,  -43,  -42,  -40,  -38,  -36,  -35,  -33,  -31,  -30,  -28,  -27,  -25,  -23,
    -22,  -20,  -19,  -17,  -16,  -14,  -13,  -11,  -10,  -8,   -7,   -6,   -4,   -3,   -1,   0};
// the BIOS volume table (SVC_GetVolumeTable): the ARM9 SDK has its own copy for its SND_CalcChannelVolume
extern const u8 data_0213a0b4[724];
static u16 sPitchTable[768];  // SVC_GetPitchTable: 0x10000 * (2^(i/768) - 1)

// ---- util
u16 snd7_calc_timer(int timer, int pitch) {
    int octave = 0;
    int p = -pitch;
    while (p < 0) { octave--; p += 768; }
    while (p >= 768) { octave++; p -= 768; }
    u64 result = sPitchTable[p];
    result += 0x10000;
    result *= (u32)timer;
    int shift = octave - 16;
    if (shift <= 0) {
        result >>= -shift;
    } else if (shift < 32) {
        if (result & ~0ull << (32 - shift)) return 0xffff;
        result <<= shift;
    } else {
        return 0xffff;
    }
    if (result < 0x10) result = 0x10;
    else if (result > 0xffff) result = 0xffff;
    return (u16)result;
}

u16 snd7_calc_channel_volume(int value) {
    if (value < SND_VOL_DB_MIN) value = SND_VOL_DB_MIN;
    else if (value > 0) value = 0;
    int result = data_0213a0b4[value - SND_VOL_DB_MIN];
    int div = value < -240 ? 3 : value < -120 ? 2 : value < -60 ? 1 : 0;
    return (u16)(result | div << 8);
}

s8 snd7_sin_idx(int x) {
    if (x < 0x20) return sLfoSinTable[x];
    if (x < 0x40) return sLfoSinTable[0x40 - x];
    if (x < 0x60) return (s8)-sLfoSinTable[x - 0x40];
    return (s8)-sLfoSinTable[0x20 - (x - 0x60)];
}

u16 snd7_calc_random(void) {
    static u32 state = 0x12345678;
    state = state * 1664525u + 1013904223u;
    return (u16)(state >> 16);
}

// ---- bank
BOOL snd7_read_inst_data(const struct SNDBankData *bank, s32 program, s32 midiKey, struct SNDInstData *inst) {
    if (program < 0 || (u32)program >= bank->instCount) return FALSE;
    u32 off = bank->instOffsets[program];
    inst->type = (u8)off;
    switch (inst->type) {
    case SND_INST_PCM:
    case SND_INST_PSG:
    case SND_INST_NOISE:
    case SND_INST_DIRECTPCM:
    case SND_INST_DUMMY:
        inst->param = *(const struct SNDInstParam *)((const u8 *)bank + (off >> 8));
        return TRUE;
    case SND_INST_DRUM_TABLE: {
        const struct SNDDrumSet *d = (const struct SNDDrumSet *)((const u8 *)bank + (off >> 8));
        if (midiKey < d->minKey || midiKey > d->maxKey) return FALSE;
        *inst = d->instruments[midiKey - d->minKey];
        return TRUE;
    }
    case SND_INST_KEY_SPLIT: {
        const struct SNDKeySplit *k = (const struct SNDKeySplit *)((const u8 *)bank + (off >> 8));
        int i = 0;
        while (midiKey > k->key[i]) {
            if (++i >= SND_INST_MAX_KEYSPLIT) return FALSE;
        }
        *inst = k->instruments[i];
        return TRUE;
    }
    default:
        return FALSE;
    }
}

static const struct SNDWaveData *wave_data_address(const struct SNDWaveArc *arc, s32 wave) {
    u32 p = arc->waveOffsets[wave];
    if (p == 0) return NULL;
    if (p < 0x02000000) return (const struct SNDWaveData *)((const u8 *)arc + p);  // an offset in the archive
    return HostPtr32_Get(p);  // a wave loaded on its own (SND_SetWaveDataAddress)
}

static const struct SNDWaveData *wave_data(const struct SNDBankData *bank, s32 waveArc, s32 wave) {
    const struct SNDWaveArc *arc = HostPtr32_Get(bank->waveArcLinks[waveArc].waveArc);
    if (arc == NULL || (u32)wave >= arc->waveCount) return NULL;
    return wave_data_address(arc, wave);
}

BOOL snd7_note_on(struct SNDExChannel *chn, s32 midiKey, s32 velocity, s32 length, const struct SNDBankData *bank,
                  const struct SNDInstData *inst) {
    u8 release = inst->param.envRelease;
    BOOL ok;
    if (release == 0xff) {
        length = -1;
        release = 0;
    }
    switch (inst->type) {
    case SND_INST_PCM:
    case SND_INST_DIRECTPCM: {
        const struct SNDWaveData *w =
            inst->type == SND_INST_PCM
                ? wave_data(bank, inst->param.wave[1], inst->param.wave[0])
                : (const struct SNDWaveData *)(uintptr_t)((u32)inst->param.wave[1] << 16 | inst->param.wave[0]);
        ok = w != NULL && snd7_start_exchannel_pcm(chn, &w->param, w->sampleData, length);
        break;
    }
    case SND_INST_PSG: ok = snd7_start_exchannel_psg(chn, inst->param.wave[0], length); break;
    case SND_INST_NOISE: ok = snd7_start_exchannel_noise(chn, length); break;
    default: ok = FALSE; break;
    }
    if (!ok) return FALSE;
    chn->midiKey = (u8)midiKey;
    chn->rootMidiKey = inst->param.rootKey;
    chn->velocity = (u8)velocity;
    snd7_set_exchannel_attack(chn, inst->param.envAttack);
    snd7_set_exchannel_decay(chn, inst->param.envDecay);
    snd7_set_exchannel_sustain(chn, inst->param.envSustain);
    snd7_set_exchannel_release(chn, release);
    chn->initPan = (s8)(inst->param.pan - 0x40);
    return TRUE;
}

// ---- work
static void update_shared_work(void) {
    if (snd7_shared == NULL) return;
    u16 active = 0, capture = 0;
    for (int i = 0; i < SND_CHANNEL_COUNT; i++)
        if (snd7_is_channel_active(i)) active |= 1 << i;
    if (snd7_is_capture_active(0)) capture |= 1;
    if (snd7_is_capture_active(1)) capture |= 2;
    snd7_shared->channelStatus = active;
    snd7_shared->captureStatus = capture;
}

// ---- alarms: SND_SetupAlarm / SND_StartAlarm; when one fires the ARM9 gets (idx | id << 8) on tag 7
static u64 sAlarmDue[SND_ALARM_COUNT];

static void alarm_setup(int idx, u32 tick, u32 period, u32 id) {
    struct SNDAlarm *a = &snd7_work.alarms[idx];
    a->enable = 0;
    a->tick = tick;
    a->period = period;
    a->id = (u8)id;
}

static void alarm_start(int idx) {
    struct SNDAlarm *a = &snd7_work.alarms[idx];
    // OS_SetAlarm(tick) fires after `tick`; OS_SetPeriodicAlarm(tick + now, period) first at tick + now
    sAlarmDue[idx] = NOW() + (a->tick ? a->tick : 1) * OS_TICK_CYCLES;
    a->enable = 1;
}

static void alarm_stop(int idx) {
    struct SNDAlarm *a = &snd7_work.alarms[idx];
    if (a->enable) {
        a->id++;
        a->enable = 0;
    }
}

#ifdef N3DS_DIRECT
// (the messages to the ARM9 and the command lists from it, between the threads: a lock held for a copy)
static int sLock;
static void lock(void) { while (__atomic_exchange_n(&sLock, 1, __ATOMIC_ACQUIRE)) {} }
static void unlock(void) { __atomic_store_n(&sLock, 0, __ATOMIC_RELEASE); }
static u32 sOutbox[64];
static int sOutboxCount;
static void send_to_arm9(u32 word) {
    lock();
    if (sOutboxCount < 64) sOutbox[sOutboxCount++] = word;
    unlock();
}
#else
static void send_to_arm9(u32 word) { pxi7_send(word); }
#endif

static void alarms_run(u64 now) {
    for (int i = 0; i < SND_ALARM_COUNT; i++) {
        struct SNDAlarm *a = &snd7_work.alarms[i];
        while (a->enable && sAlarmDue[i] <= now) {
            send_to_arm9(7 | (u32)(i | a->id << 8) << 6);
            if (a->period == 0) { sAlarmDue[i] = ~0ull; break; }
            sAlarmDue[i] += a->period * OS_TICK_CYCLES;
        }
    }
}

// ---- commands
static struct SNDCommand *sMsg[64];  // command lists waiting (the DS's message queue holds 8)
static int sMsgCount;

static void start_timer(u32 channelMask, u32 captureMask, u32 alarmMask) {
    for (int i = 0; i < SND_CHANNEL_COUNT && channelMask; i++, channelMask >>= 1) {
        if (channelMask & 1) {
            u32 old = spu_ch[i].cnt;
            spu_ch[i].cnt |= 0x80000000u;
            spu_cnt_written(i, old);
        }
    }
    for (int i = 0; i < 2; i++) {
        if (captureMask & (1 << i)) {
            u8 old = spu_capcnt[i];
            spu_capcnt[i] |= 0x80;
            spu_capcnt_written(i, old);
        }
    }
    for (int i = 0; i < SND_ALARM_COUNT && alarmMask; i++, alarmMask >>= 1)
        if (alarmMask & 1) alarm_start(i);
    update_shared_work();
}

static void stop_timer(u32 channelMask, u32 captureMask, u32 alarmMask, s32 hold) {
    for (int i = 0; i < SND_ALARM_COUNT && alarmMask; i++, alarmMask >>= 1)
        if (alarmMask & 1) alarm_stop(i);
    for (int i = 0; i < SND_CHANNEL_COUNT && channelMask; i++, channelMask >>= 1)
        if (channelMask & 1) snd7_stop_channel(i, hold);
    for (int i = 0; i < 2; i++) {
        if (captureMask & (1 << i)) {
            u8 old = spu_capcnt[i];
            spu_capcnt[i] = 0;
            spu_capcnt_written(i, old);
        }
    }
    update_shared_work();
}

// READ_DRIVER_INFO: the driver's state in the DS layout. Its pointers to the driver's own objects (a track's
// channels, a channel's next) are relative to workPtr, a stand-in DS address (the ARM9 only subtracts it); the
// others (wave data, banks, sequence positions) are the ARM9's own addresses.
#define DS_WORK 0x03800000u
static u32 ds_channel(const struct SNDExChannel *c) { return c ? DS_WORK + (u32)(c - snd7_work.channels) * 0x54 : 0; }
static void put32(u8 *p, u32 v) { memcpy(p, &v, 4); }
static void write_ds_work(u8 *w) {
    for (int i = 0; i < SND_CHANNEL_COUNT; i++) {
        const struct SNDExChannel *c = &snd7_work.channels[i];
        u8 *d = w + i * 0x54;
        memcpy(d, c, 0x44);  // up to the wave pointer: the same layout
        put32(d + 0x44, c->type == SND_CHN_TYPE_PCM ? HostPtr32_Of(c->waveDataPtr) : (u32)c->dutyCycle);
        put32(d + 0x48, HostPtr32_Of((const void *)(uintptr_t)c->callback));
        put32(d + 0x4c, HostPtr32_Of(c->callbackUserData));
        put32(d + 0x50, ds_channel(c->channelLLNext));
    }
    for (int i = 0; i < SND_PLAYER_COUNT; i++) {
        const struct SNDPlayer *p = &snd7_work.players[i];
        u8 *d = w + 0x540 + i * 0x24;
        memcpy(d, p, 0x20);
        put32(d + 0x20, HostPtr32_Of(p->bank));
    }
    for (int i = 0; i < SND_TRACK_COUNT; i++) {
        const struct SNDTrack *t = &snd7_work.tracks[i];
        u8 *d = w + 0x780 + i * 0x40;
        memcpy(d, t, 0x24);  // up to the sequence pointers
        put32(d + 0x24, HostPtr32_Of(t->base));
        put32(d + 0x28, HostPtr32_Of(t->cur));
        for (int k = 0; k < SND_TRACK_MAX_CALL; k++) put32(d + 0x2c + 4 * k, HostPtr32_Of(t->posCallStack[k]));
        memcpy(d + 0x38, t->loopCount, SND_TRACK_MAX_CALL);
        d[0x3b] = t->callStackDepth;
        put32(d + 0x3c, ds_channel(t->channelLLHead));
    }
    memcpy(w + 0xf80, snd7_work.alarms, sizeof snd7_work.alarms);
}

static void read_driver_info(struct SNDDriverInfo *info) {
    write_ds_work(info->work);
    info->workPtr = DS_WORK;
    for (int i = 0; i < SND_CHANNEL_COUNT; i++) info->channelControls[i] = snd7_get_channel_control(i);
    info->lockedChannels = snd7_get_locked_channel(0);
}

static void each_channel(u32 mask, void (*fn)(int ch, int a, int b), int a, int b) {
    for (int i = 0; i < SND_CHANNEL_COUNT && mask; i++, mask >>= 1)
        if (mask & 1) fn(i, a, b);
}
static void set_timer2(int ch, int timer, int unused) { (void)unused; snd7_set_channel_timer(ch, timer); }
static void set_pan2(int ch, int pan, int unused) { (void)unused; snd7_set_channel_pan(ch, pan); }

static int sTrace = -1;

static void command_proc(void) {
    if (sTrace < 0) sTrace = getenv("ACWW_SNDTRACE") != NULL;
    for (int m = 0; m < sMsgCount; m++) {
        for (struct SNDCommand *p = sMsg[m]; p;) {
            struct SNDCommand c = *p;
            const uintptr_t *a = c.arg;
            if (sTrace)
                fprintf(stderr, "snd7: frame %llu cmd %d %08llx %08llx %08llx %08llx\n", (unsigned long long)host_frame(), c.id,
                        (unsigned long long)a[0], (unsigned long long)a[1], (unsigned long long)a[2], (unsigned long long)a[3]);
            switch (c.id) {
            case 0x00: snd7_start_seq((int)a[0], (const void *)(uintptr_t)a[1], a[2], (struct SNDBankData *)(uintptr_t)a[3]); break;
            case 0x01: snd7_stop_seq((int)a[0]); break;
            case 0x02: snd7_prepare_seq((int)a[0], (const void *)(uintptr_t)a[1], a[2], (struct SNDBankData *)(uintptr_t)a[3]); break;
            case 0x03: snd7_start_prepared_seq((int)a[0]); break;
            case 0x04: snd7_pause_seq((int)a[0], (BOOL)a[1]); break;
            case 0x05: snd7_skip_seq((int)a[0], a[1]); break;
            case 0x06: snd7_set_player_param((int)a[0], a[1], a[2], (int)a[3]); break;
            case 0x07: snd7_set_track_param((int)(a[0] & 0xffffff), a[1], a[2], a[3], (u8)(a[0] >> 24)); break;
            case 0x08: snd7_set_track_mute((int)a[0], a[1], (int)a[2]); break;
            case 0x09: snd7_set_track_allocatable_channel((int)a[0], a[1], a[2]); break;
            case 0x0a: if (snd7_shared) snd7_shared->players[a[0]].localVars[a[1]] = (s16)a[2]; break;
            case 0x0b: if (snd7_shared) snd7_shared->globalVars[a[0]] = (s16)a[1]; break;
            case 0x0c: start_timer(a[0], a[1], a[2]); break;
            case 0x0d: stop_timer(a[0], a[1], a[2], (s32)a[3]); break;
            case 0x0e:
                snd7_setup_channel_pcm((int)(a[0] & 0xffff), (const void *)a[1], (int)(a[3] >> 24) & 3,
                                       (int)(a[3] >> 26) & 3, (int)(a[3] & 0xffff), (int)(a[2] & 0x3fffff),
                                       (int)(a[2] >> 24) & 0x7f, (int)(a[2] >> 22) & 3, (int)(a[0] >> 16) & 0xffff,
                                       (int)(a[3] >> 16) & 0x7f);
                break;
            case 0x0f:
                snd7_setup_channel_psg((int)a[0], (int)a[3], (int)a[1] & 0x7f, (int)(a[1] >> 8) & 3,
                                       (int)(a[2] >> 8) & 0xffff, (int)a[2] & 0x7f);
                break;
            case 0x10:
                snd7_setup_channel_noise((int)a[0], (int)a[1] & 0x7f, (int)(a[1] >> 8) & 3, (int)(a[2] >> 8) & 0xffff,
                                         (int)a[2] & 0x7f);
                break;
            case 0x11:
                snd7_setup_capture((int)(a[2] >> 31) & 1, (int)(a[2] >> 30) & 1, (void *)(uintptr_t)a[0], (int)a[1],
                                   (BOOL)(a[2] >> 29) & 1, (int)(a[2] >> 28) & 1, (int)(a[2] >> 27) & 1);
                break;
            case 0x12: alarm_setup((int)a[0], a[1], a[2], a[3]); break;
            case 0x13: each_channel(a[0], set_timer2, (int)a[1], 0); break;
            case 0x14:
                for (int i = 0; i < SND_CHANNEL_COUNT; i++)
                    if (a[0] >> i & 1) snd7_set_channel_volume(i, (int)a[1], (int)a[2]);
                break;
            case 0x15: each_channel(a[0], set_pan2, (int)a[1], 0); break;
            case 0x16: snd7_set_surround_decay((int)a[0]); break;
            case 0x17: spu_soundcnt = (u16)((spu_soundcnt & ~0x7fu) | (a[0] & 0x7f)); break;
            case 0x18: snd7_set_master_pan((int)a[0]); break;
            case 0x19:
                spu_soundcnt = (u16)((spu_soundcnt & 0x807f) | (a[0] & 3) << 8 | (a[1] & 3) << 10 | (a[2] & 1) << 12 |
                                     (a[3] & 1) << 13);
                break;
            case 0x1a: snd7_lock_channel(a[0], a[1]); break;
            case 0x1b: snd7_unlock_channel(a[0], a[1]); break;
            case 0x1c: snd7_stop_unlocked_channel(a[0], a[1]); break;
            case 0x1d: snd7_shared = (struct SNDSharedWork *)(uintptr_t)a[0]; break;
            case 0x1e: snd7_invalidate_seq((const void *)(uintptr_t)a[0], (const void *)(uintptr_t)a[1]); break;
            case 0x1f: snd7_invalidate_bank((const void *)(uintptr_t)a[0], (const void *)(uintptr_t)a[1]); break;
            case 0x20: snd7_invalidate_wave((const void *)(uintptr_t)a[0], (const void *)(uintptr_t)a[1]); break;
            case 0x21: read_driver_info((struct SNDDriverInfo *)(uintptr_t)a[0]); break;
            default: fprintf(stderr, "snd7: unknown command %d\n", c.id); break;
            }
            p = c.llNext;
        }
        if (snd7_shared) snd7_shared->finishedCommandTag++;
    }
    sMsgCount = 0;
}

// ---- the driver thread's loop body (SndThread)
// at: the virtual time of the pass (the sound so far is mixed with the registers as they were)
static void snd7_pass(BOOL update, u64 at) {
    spu_mix_until(at);
    snd7_update_exchannel();
    command_proc();
    snd7_seq_main(update);
    snd7_exchannel_main(update);
    update_shared_work();
    snd7_calc_random();
}

void snd7_skip_pause(void) { sTimerStopped = 1; }
void snd7_skip_resume(void) {
    sTimerStopped = 0;
    sNextTick = NOW() + SND_START_DELAY;
}

static int sInit;
static void snd7_init(void) {
    sInit = 1;
    for (int i = 0; i < 768; i++) sPitchTable[i] = (u16)lround(65536.0 * (pow(2.0, i / 768.0) - 1.0));
    snd7_exchannel_init();
    snd7_seq_init();
    for (int i = 0; i < SND_ALARM_COUNT; i++) {
        snd7_work.alarms[i].enable = 0;
        snd7_work.alarms[i].id = 0;
        sAlarmDue[i] = ~0ull;
    }
    spu_soundcnt = 0x8000 | 0x7f;  // SND_Enable, SND_SetOutputSelector(0, 0, 0, 0), SND_SetMasterVolume(0x7f)
}

#ifdef N3DS_DIRECT
static struct SNDCommand *sInbox[64];  // (the lists handed over, for the thread's next pass)
static int sInboxCount, sStarted;
int frontend_audio_pump(void);  // (frontend_3ds.c: the DSP fed from the ring; the frames waiting to be played)
enum { CHUNK = 256, LEVEL = 3 * 1024 };  // (output frames mixed at a time; mixed ahead of the DSP's playing)
static void snd7_thread(void *arg) {
    (void)arg;
    while (!host_threads_quitting()) {
        while (frontend_audio_pump() < LEVEL) {
            lock();
            int n = sInboxCount;
            for (int i = 0; i < n && sMsgCount < 64; i++) sMsg[sMsgCount++] = sInbox[i];
            sInboxCount = 0;
            unlock();
            if (n) snd7_pass(FALSE, sClock);
            u64 end = sClock + CHUNK * 1024u;  // (1024 bus cycles an output frame: spu.c)
            while (!sTimerStopped && sNextTick <= end) {
                alarms_run(sNextTick);
                sClock = sNextTick;
                sNextTick += SND_INTERVAL;
                snd7_pass(TRUE, sClock);
            }
            alarms_run(end);
            spu_mix_until(end);
            sClock = end;
        }
        host_plat_sleep_ms(2);
    }
    host_thread_stopped();
}
#endif

// PXI tag 7 from the ARM9: a command list, or NULL = process now
void snd7_receive(struct SNDCommand *list) {
    if (!sInit) snd7_init();
#ifdef N3DS_DIRECT
    if (list) {
        lock();
        if (sInboxCount < 64) sInbox[sInboxCount++] = list;
        else fprintf(stderr, "snd7: command queue full\n");
        unlock();
    }
    if (!sStarted) {
        sStarted = 1;
        // (ACWW_SOUND_CORE=1: on the 3DS's system core 1 first, off the GPU thread's core 2; not the default: it
        // didn't pay on hardware)
        int core1 = getenv("ACWW_SOUND_CORE") && atoi(getenv("ACWW_SOUND_CORE")) == 1;
        if (!host_thread_start(snd7_thread, 0, core1 ? 2 : 1)) fprintf(stderr, "snd7: no thread for the sound\n");
    }
    return;
#endif
    if (list) {
        if (sMsgCount < 64) sMsg[sMsgCount++] = list;
        else fprintf(stderr, "snd7: command queue full\n");
    }
    // the driver thread runs right away (the DS: at its next wakeup, within 5.2 ms)
    snd7_pass(FALSE, host_cycles());
}

u64 snd7_next_due(void) {
#ifdef N3DS_DIRECT
    return ~0ull;  // (the thread's own clock)
#endif
    u64 d = sTimerStopped ? ~0ull : sNextTick;
    for (int i = 0; i < SND_ALARM_COUNT; i++)
        if (snd7_work.alarms[i].enable && sAlarmDue[i] < d) d = sAlarmDue[i];
    return d;
}

// ACWW_SNDSTATE: the driver's state once a second (players, their tracks, the channels)
static void dump_state(void) {
    static u64 last = ~0ull;
    u64 f = host_frame();
    if (f % 60 || f == last) return;
    last = f;
    fprintf(stderr, "snd7: frame %llu soundcnt %04x cap %02x %02x\n", (unsigned long long)f, spu_soundcnt, spu_capcnt[0], spu_capcnt[1]);
    for (int p = 0; p < SND_PLAYER_COUNT; p++) {
        struct SNDPlayer *ply = &snd7_work.players[p];
        if (!ply->flags.active) continue;
        fprintf(stderr, "  player %d prepared %d paused %d vol %d fader %d tempo %d ratio %d prio %d tracks", p,
                ply->flags.prepared, ply->flags.paused, ply->volume, ply->extFader, ply->tempo, ply->tempoRatio, ply->prio);
        for (int i = 0; i < 16; i++) {
            if (ply->tracks[i] == 0xff) continue;
            struct SNDTrack *t = &snd7_work.tracks[ply->tracks[i]];
            fprintf(stderr, " [%d cur %08x wait %d vol %d expr %d fader %d mute %d mask %04x]", i, (u32)(uintptr_t)t->cur, t->wait,
                    t->volume, t->expression, t->extFader, t->flags.muted, t->channelMask);
        }
        fprintf(stderr, "\n");
    }
    for (int i = 0; i < 16; i++) {
        struct SNDExChannel *c = &snd7_work.channels[i];
        if (!c->flags.active && !(spu_ch[i].cnt >> 31)) continue;
        fprintf(stderr, "  ch %d active %d env %d att %d vol %04x timer %04x pan %d prio %d | cnt %08x sad %08x tmr %04x pnt %04x len %06x\n",
                i, c->flags.active, c->envStatus, c->envAttenuation, c->volume, c->timer, c->pan, c->priority, spu_ch[i].cnt,
                (u32)spu_ch[i].sad, spu_ch[i].tmr, spu_ch[i].pnt, spu_ch[i].len);
    }
}

// from the host's event loop: the periodic passes and alarms due by now, and the audio up to now
void snd7_run(u64 now) {
#ifdef N3DS_DIRECT
    (void)now;
    u32 out[64];
    lock();
    int n = sOutboxCount;
    memcpy(out, sOutbox, (size_t)n * sizeof *out);
    sOutboxCount = 0;
    unlock();
    for (int i = 0; i < n; i++) pxi7_send(out[i]);
    return;
#endif
    HOST_PROF(PROF_SOUND);
    if (!sInit) snd7_init();
    while (!sTimerStopped && sNextTick <= now) {
        alarms_run(sNextTick);
        u64 t = sNextTick;
        sNextTick += SND_INTERVAL;
        snd7_pass(TRUE, t);
    }
    alarms_run(now);
    spu_mix_until(now);
    static int dump = -1;
    if (dump < 0) dump = getenv("ACWW_SNDSTATE") != NULL;
    if (dump) dump_state();
}

// The port's sound thread and the lists handed to it (ENHANCED_PORT): included once in driver.c, after
// snd7_init, before snd7_receive; it needs the file's statics and the lock of driver_lock.h.

static struct SNDCommand *sInbox[64];  // (the lists handed over, for the thread's next pass)
static int sInboxCount, sStarted;
int frontend_audio_pump(void);  // (frontend_3ds.c: the DSP fed from the ring; the frames waiting to be played)
enum { CHUNK = 256, LEVEL = 3 * 1024 };  // (output frames mixed at a time; mixed ahead of the DSP's playing)
// ACWW_FPS or ACWW_SOUND_STATS: the thread's time a second of sound, the driver (sequencer, alarms) and the mixer
static u64 sTicksDriver, sTicksMix, sMixedFrames;
static void snd7_thread_report(void) {
    static int on = -1;
    if (on < 0) on = getenv("ACWW_FPS") || getenv("ACWW_SOUND_STATS");
    if (!on || sMixedFrames < 5 * 32728) return;
    double secs = (double)sMixedFrames / 32728;
    fprintf(stderr, "sound: the thread's time a second of sound: driver %.1f ms, mixer %.1f ms\n",
            (double)sTicksDriver / 268111.856 / secs, (double)sTicksMix / 268111.856 / secs);
    sTicksDriver = sTicksMix = sMixedFrames = 0;
}
static void snd7_thread(void *arg) {
    (void)arg;
    while (!host_threads_quitting()) {
        while (frontend_audio_pump() < LEVEL) {
            lock();
            int n = sInboxCount;
            for (int i = 0; i < n && sMsgCount < 64; i++) sMsg[sMsgCount++] = sInbox[i];
            sInboxCount = 0;
            unlock();
            u64 t0 = host_plat_ticks(), mixed = 0;  // (after the lock: not its waiting)
            // (each pass mixes up to its time first, snd7_pass: done here, timed, so that the pass's own is empty)
#define MIX_TO(at) do { u64 m_ = host_plat_ticks(); spu_mix_until(at); mixed += host_plat_ticks() - m_; } while (0)
            if (n) {
                MIX_TO(sClock);
                snd7_pass(FALSE, sClock);
            }
            u64 end = sClock + CHUNK * 1024u;  // (1024 bus cycles an output frame: spu.c)
            while (!sTimerStopped && sNextTick <= end) {
                alarms_run(sNextTick);
                sClock = sNextTick;
                sNextTick += SND_INTERVAL;
                MIX_TO(sClock);
                snd7_pass(TRUE, sClock);
            }
            alarms_run(end);
            MIX_TO(end);
#undef MIX_TO
            sClock = end;
            sTicksMix += mixed, sTicksDriver += host_plat_ticks() - t0 - mixed, sMixedFrames += CHUNK;
        }
        snd7_thread_report();
        host_plat_sleep_ms(2);
    }
    host_thread_stopped();
}

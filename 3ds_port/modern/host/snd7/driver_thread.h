// The port's sound thread and the lists handed to it (N3DS_PORT): included once in driver.c, after
// snd7_init, before snd7_receive; it needs the file's statics and the lock of driver_lock.h.

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

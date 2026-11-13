// The port's lock and mailbox to the ARM9 (N3DS_PORT): included once in driver.c, in place of the
// send_to_arm9 that calls pxi7_send directly.

// (the messages to the ARM9 and the command lists from it, between the threads: a lock held for a copy. Taken, it is
// waited for asleep after a short spin: with ACWW_SOUND_CORE=0 the sound's thread is on the game's core before the
// game's thread, and a spin there would never let the holder go on)
static int sLock;
static void lock(void) {
    for (int i = 0; __atomic_exchange_n(&sLock, 1, __ATOMIC_ACQUIRE); i++)
        if (i >= 1000) host_plat_sleep_ms(1);
}
static void unlock(void) { __atomic_store_n(&sLock, 0, __ATOMIC_RELEASE); }
static u32 sOutbox[64];
static int sOutboxCount;
static void send_to_arm9(u32 word) {
    lock();
    if (sOutboxCount < 64) sOutbox[sOutboxCount++] = word;
    unlock();
}

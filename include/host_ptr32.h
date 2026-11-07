#ifndef HOST_PTR32_H
#define HOST_PTR32_H

// A pointer in a 32-bit slot of the DS's data formats: the sound files' links (a bank's wave archives, a wave
// archive's banks, the address of a wave loaded on its own), which keep their file layout, so the slot stays 4 bytes.
// Values below 0x02000000 in these slots are file offsets, as on the DS. A pointer from 0x02000000 to below
// HOST_PTR32_HANDLE is stored as itself (DS memory; a 32-bit PC's heap); any other (a 64-bit host's, the 3DS's
// program and data from 0x00100000 on) as a handle >= HOST_PTR32_HANDLE: its index in a table of the pointers stored
// this way (modern/host/ptr32.c; each distinct pointer once, never released).
// Self-contained (no includes): the game's units with their own basic types include it too.
#define HOST_PTR32_HANDLE 0xf0000000u
#ifdef __cplusplus
extern "C" {
#endif
unsigned int HostPtr32_Handle(const void *p);  // (modern/host/ptr32.c)
void *HostPtr32_Lookup(unsigned int handle);
#ifdef __cplusplus
}
#endif
static inline unsigned int HostPtr32_Of(const void *p) {
    __UINTPTR_TYPE__ a = (__UINTPTR_TYPE__)p;
    return a == 0 || (a >= 0x02000000u && a < HOST_PTR32_HANDLE) ? (unsigned int)a : HostPtr32_Handle(p);
}
static inline void *HostPtr32_Get(unsigned int v) {
    return v < HOST_PTR32_HANDLE ? (void *)(__UINTPTR_TYPE__)v : HostPtr32_Lookup(v);
}

#endif

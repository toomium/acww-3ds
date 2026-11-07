// Host build: pointers in the 32-bit slots of the DS's data formats (include/host_ptr32.h). A pointer that isn't
// stored as itself gets a handle: HOST_PTR32_HANDLE + its index in sPtrs. Each distinct pointer is stored
// once (a hash from pointer to index), so the table grows only with the number of distinct addresses ever stored
// (the sound data's links: a few hundred in a session).

#include <stdlib.h>

#include "host.h"
#include "host_ptr32.h"

static const void **sPtrs;   // index -> pointer
static u32 sCount, sCap;
static u32 *sHash;           // open addressing: index + 1, 0 = empty
static u32 sHashCap;         // a power of two, at least twice sCount

static u32 hash_of(const void *p) {
    u64 a = (u64)(uintptr_t)p;
    a ^= a >> 33;
    a *= 0xff51afd7ed558ccdull;
    a ^= a >> 33;
    return (u32)a;
}

static void rehash(u32 cap) {
    free(sHash);
    sHash = calloc(cap, sizeof *sHash);
    if (!sHash) host_fatal("HostPtr32: no memory");
    sHashCap = cap;
    for (u32 i = 0; i < sCount; i++) {
        u32 h = hash_of(sPtrs[i]) & (cap - 1);
        while (sHash[h]) h = (h + 1) & (cap - 1);
        sHash[h] = i + 1;
    }
}

unsigned int HostPtr32_Handle(const void *p) {
    if (sHashCap == 0) rehash(1024);
    u32 h = hash_of(p) & (sHashCap - 1);
    for (; sHash[h]; h = (h + 1) & (sHashCap - 1))
        if (sPtrs[sHash[h] - 1] == p) return HOST_PTR32_HANDLE + sHash[h] - 1;
    if (sCount >= ~HOST_PTR32_HANDLE) host_fatal("HostPtr32: too many pointers");
    if (sCount == sCap) {
        sCap = sCap ? sCap * 2 : 1024;
        sPtrs = realloc(sPtrs, sCap * sizeof *sPtrs);
        if (!sPtrs) host_fatal("HostPtr32: no memory");
    }
    sPtrs[sCount] = p;
    sHash[h] = ++sCount;
    if (sCount * 2 > sHashCap) rehash(sHashCap * 2);
    return HOST_PTR32_HANDLE + sCount - 1;
}

void *HostPtr32_Lookup(unsigned int handle) {
    u32 i = handle - HOST_PTR32_HANDLE;
    if (i >= sCount) host_fatal("HostPtr32: not a handle");
    return (void *)sPtrs[i];
}

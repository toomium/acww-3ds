// Host build: C versions of the SDK and runtime routines the DS has as assembly (src/**/*.s) - memory fills and
// copies, matrix helpers, cache maintenance (nothing to do on the host), mwcc's helper functions called by name.
// Interrupts and thread contexts are in os.c.

#include <stdint.h>
#include <string.h>

#include "ds_div.h"
#include "host_mem.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;
typedef int64_t s64;
typedef uint64_t u64;
typedef s32 fx32;

// ---- caches, protection regions: nothing to do
void DC_FlushAll(void) {}
void DC_FlushRange(void *p, u32 n) { (void)p; (void)n; }
void DC_InvalidateAll(void) {}
void DC_InvalidateRange(void *p, u32 n) { (void)p; (void)n; }
void DC_StoreAll(void) {}
void DC_StoreRange(void *p, u32 n) { (void)p; (void)n; }
void DC_WaitWriteBufferEmpty(void) {}
void IC_InvalidateAll(void) {}
void IC_InvalidateRange(void *p, u32 n) { (void)p; (void)n; }
void OS_SetProtectionRegion1(u32 v) { (void)v; }
void OS_SetProtectionRegion2(u32 v) { (void)v; }
void OS_EnableDTCM(void) {}
void *OS_GetDTCMAddress(void) { return HW_DTCM_BLOCK; }  // (the host's DTCM block)

// ---- MI: fills and copies (sizes in bytes; the 16/32-bit versions move whole units). A register on either side
// (the 3D command FIFO, the matrix results, a DMA's registers) goes through the register model, unit by unit.
unsigned long long HostIo_Read(uintptr_t addr, unsigned int size);
unsigned long long HostIo_Write(uintptr_t addr, unsigned int size, unsigned long long value);
// (the whole pointer: a host pointer above 4 GB whose low word falls in the register range isn't a register)
static int is_reg(const volatile void *p) { uintptr_t a = (uintptr_t)p; return a >= 0x04000000 && a < 0x05000000; }
static void units(const volatile void *src, volatile void *dst, u32 n, u32 size, int fill, u32 v, int fixed_dst) {
    uintptr_t s = (uintptr_t)src, d = (uintptr_t)dst;
    for (n /= size; n--; d += fixed_dst ? 0 : size) {
        u32 x = fill ? v : is_reg(src) ? (u32)HostIo_Read(s, size) : size == 2 ? *(const volatile u16 *)s : *(const volatile u32 *)s;
        if (!fill) s += size;
        if (is_reg(dst)) HostIo_Write(d, size, x);
        else if (size == 2) *(volatile u16 *)(uintptr_t)d = (u16)x;
        else *(volatile u32 *)(uintptr_t)d = x;
    }
}
void MIi_CpuClear16(u32 v, void *dst, u32 n) {
    if (is_reg(dst)) { units(0, dst, n, 2, 1, v, 0); return; }
    u16 *d = dst;
    for (n /= 2; n--;) *d++ = (u16)v;
}
void MIi_CpuCopy16(const void *src, void *dst, u32 n) {
    if (is_reg(src) || is_reg(dst)) { units(src, dst, n, 2, 0, 0, 0); return; }
    const u16 *s = src;
    u16 *d = dst;
    for (n /= 2; n--;) *d++ = *s++;
}
void MIi_CpuClear32(u32 v, void *dst, u32 n) {
    if (is_reg(dst)) { units(0, dst, n, 4, 1, v, 0); return; }
    u32 *d = dst;
    for (n /= 4; n--;) *d++ = v;
}
void MIi_CpuCopy32(const void *src, void *dst, u32 n) {
    if (is_reg(src) || is_reg(dst)) { units(src, dst, n, 4, 0, 0, 0); return; }
    const u32 *s = src;
    u32 *d = dst;
    for (n /= 4; n--;) *d++ = *s++;
}
void HostIo_Send32(uintptr_t addr, const u32 *src, u32 n);  // (hw.c)
void MIi_CpuSend32(const void *src, volatile void *dst, u32 n) {
    if (is_reg(dst) && !is_reg(src)) { HostIo_Send32((uintptr_t)dst, src, n / 4); return; }  // the 3D command FIFO
    if (is_reg(dst)) { units(src, dst, n, 4, 0, 0, 1); return; }  // (every word to one port)
    const u32 *s = src;
    volatile u32 *d = dst;
    for (n /= 4; n--;) *d = *s++;
}
void MIi_CpuClearFast(u32 v, void *dst, u32 n) { MIi_CpuClear32(v, dst, n); }
void MIi_CpuCopyFast(const void *src, void *dst, u32 n) { MIi_CpuCopy32(src, dst, n); }
void MI_CpuFill8(void *dst, u32 v, u32 n) { u8 *d = dst; while (n--) *d++ = (u8)v; }
void MI_CpuCopy8(const void *src, void *dst, u32 n) { memmove(dst, src, n); }
void MI_Copy32B(const void *src, void *dst) { MIi_CpuCopy32(src, dst, 32); }
void MI_Copy36B(const void *src, void *dst) { MIi_CpuCopy32(src, dst, 36); }
void MI_Copy48B(const void *src, void *dst) { MIi_CpuCopy32(src, dst, 48); }
void MI_Copy64B(const void *src, void *dst) { MIi_CpuCopy32(src, dst, 64); }
void MI_Zero36B(void *dst) { MIi_CpuClear32(0, dst, 36); }
void GX_SendFifo48B(const void *src, void *dst) { MIi_CpuSend32(src, dst, 48); }
void GXi_NopClearFifo128_(void *dst) { MIi_CpuSend32((const u32[32]){0}, dst, 128); }

// BIOS CpuSet: mode bits 0-20 count, bit 24 fill, bit 26 32-bit units
void CpuSet(const void *src, void *dst, u32 mode) {
    u32 n = mode & 0x1fffff;
    if (mode & (1u << 26)) {
        const u32 *s = src; u32 *d = dst;
        for (u32 i = 0; i < n; i++) d[i] = (mode & (1u << 24)) ? *s : s[i];
    } else {
        const u16 *s = src; u16 *d = dst;
        for (u32 i = 0; i < n; i++) d[i] = (mode & (1u << 24)) ? *s : s[i];
    }
}

s32 MI_SwapWord(u32 v, volatile u32 *p) { u32 old = *p; *p = v; return (s32)old; }

// LZ77 (type 0x10): header word = size << 8 | 0x10
void MI_UncompressLZ8(const void *srcp, void *dstp) {
    const u8 *s = srcp; u8 *d = dstp;
    u32 size = (s[1] | s[2] << 8 | s[3] << 16); s += 4;
    while (size > 0) {
        u8 flags = *s++;
        for (int i = 0; i < 8 && size > 0; i++, flags <<= 1) {
            if (flags & 0x80) {
                u32 len = (s[0] >> 4) + 3, off = ((s[0] & 15) << 8 | s[1]) + 1; s += 2;
                for (; len && size; len--, size--, d++) *d = d[-(s32)off];
            } else { *d++ = *s++; size--; }
        }
    }
}

// the overlay "backward" compression of the ROM; overlays aren't compressed on the host
void MIi_UncompressBackward(u32 bottom) { (void)bottom; }

// streaming LZ8: the assembly decoder (src/autoload_2/unk_021162b0.s) in C. The context is the one
// MI_InitUncompContextLZ (src/autoload_2/unk_021163b0.c) sets up: destination, remaining count, the current flag
// byte and its remaining bits, a reference's first byte and whether it has been read. Returns the remaining count
// (0: done); File_LoadAlloc feeds it the file a heap-sized piece at a time when the whole file doesn't fit.
typedef struct {
    u8 *destp;
    int destCount;
    u16 u8_;
    u8 u10;
    u8 flags, flagIndex, length, lengthFlg;
} HostUncompContextLZ;

s32 MI_ReadUncompLZ8(void *st, const void *p, s32 n) {
    HostUncompContextLZ *c = st;
    const u8 *s = p;
    u8 *d = c->destp;
    int count = c->destCount;
    while (count > 0) {
        if (c->flagIndex == 0) {
            if (n == 0) break;
            c->flags = *s++; n--;
            c->flagIndex = 8;
            continue;
        }
        if (n == 0) break;
        if (!(c->flags & 0x80)) {
            *d++ = *s++; n--; count--;
        } else {
            if (!c->lengthFlg) { c->length = *s++; n--; c->lengthFlg = 1; }
            if (n == 0) break;
            u32 off = ((c->length & 0xf) << 8 | *s++) + 1; n--;
            c->lengthFlg = 0;
            for (s32 len = (c->length >> 4) + 3; len > 0; len--) { *d = d[-(s32)off]; d++; count--; }
        }
        if (count == 0) break;
        c->flags <<= 1;
        c->flagIndex--;
    }
    c->destp = d;
    c->destCount = count;
    return count;
}

// ---- matrices (fx32, 1.0 = 0x1000): 33 = 3x3, 43 = 3x3 + translation row, 44 = 4x4 (row-major)
#define ONE 0x1000
static void set9(fx32 *m, fx32 a, fx32 b, fx32 c, fx32 d, fx32 e, fx32 f, fx32 g, fx32 h, fx32 i) {
    m[0] = a; m[1] = b; m[2] = c; m[3] = d; m[4] = e; m[5] = f; m[6] = g; m[7] = h; m[8] = i;
}
void MTX_Identity33_(fx32 *m) { set9(m, ONE, 0, 0, 0, ONE, 0, 0, 0, ONE); }
void MTX_Identity43_(fx32 *m) { MTX_Identity33_(m); m[9] = m[10] = m[11] = 0; }
void MTX_Identity44_(fx32 *m) { memset(m, 0, 64); m[0] = m[5] = m[10] = m[15] = ONE; }
void MTX_RotX33_(fx32 *m, fx32 s, fx32 c) { set9(m, ONE, 0, 0, 0, c, s, 0, -s, c); }
void MTX_RotY33_(fx32 *m, fx32 s, fx32 c) { set9(m, c, 0, -s, 0, ONE, 0, s, 0, c); }
void MTX_RotZ33_(fx32 *m, fx32 s, fx32 c) { set9(m, c, s, 0, -s, c, 0, 0, 0, ONE); }
void MTX_RotX43_(fx32 *m, fx32 s, fx32 c) { MTX_RotX33_(m, s, c); m[9] = m[10] = m[11] = 0; }
void MTX_RotY43_(fx32 *m, fx32 s, fx32 c) { MTX_RotY33_(m, s, c); m[9] = m[10] = m[11] = 0; }
void MTX_RotZ43_(fx32 *m, fx32 s, fx32 c) { MTX_RotZ33_(m, s, c); m[9] = m[10] = m[11] = 0; }
void MTX_Scale43_(fx32 *m, fx32 x, fx32 y, fx32 z) { set9(m, x, 0, 0, 0, y, 0, 0, 0, z); m[9] = m[10] = m[11] = 0; }
void MTX_Copy33To43_(const fx32 *a, fx32 *b) { memcpy(b, a, 36); b[9] = b[10] = b[11] = 0; }
void MTX_Copy43To44_(const fx32 *a, fx32 *b) {
    for (int r = 0; r < 4; r++) { b[r * 4] = a[r * 3]; b[r * 4 + 1] = a[r * 3 + 1]; b[r * 4 + 2] = a[r * 3 + 2]; b[r * 4 + 3] = r == 3 ? ONE : 0; }
}
void MTX_Copy44To43_(const fx32 *a, fx32 *b) {
    for (int r = 0; r < 4; r++) { b[r * 3] = a[r * 4]; b[r * 3 + 1] = a[r * 4 + 1]; b[r * 3 + 2] = a[r * 4 + 2]; }
}

// ---- bit counting
u32 OsCountZeroBits(u32 x) { return x ? (u32)__builtin_clz(x) : 32; }
u32 WifiLink_CountLeadingZeros(u32 x) { return OsCountZeroBits(x); }
u32 WifiLink_CountBits(u32 x) { return (u32)__builtin_popcount(x); }

// ---- mwcc runtime helpers called by name (soft float in IEEE single/double bit patterns, division)
static float f_of(u32 v) { float f; memcpy(&f, &v, 4); return f; }
static u32 u_of(float f) { u32 v; memcpy(&v, &f, 4); return v; }
u32 _fadd(u32 a, u32 b) { return u_of(f_of(a) + f_of(b)); }
u32 _fsub(u32 a, u32 b) { return u_of(f_of(a) - f_of(b)); }
s32 _ffix(u32 a) { return (s32)f_of(a); }
u32 _fflt(s32 a) { return u_of((float)a); }
double _f2d(u32 a) { return (double)f_of(a); }
double _dsqrt(double x) { return __builtin_sqrt(x); }
s64 _ll_mul(s64 a, s64 b) { return a * b; }
// (the quotient only: the callers by name use r0; a / 0 = a as on the DS)
s32 _s32_div_f(s32 a, s32 b) { return DS_DIV(a, b); }
u32 _u32_div_f(u32 a, u32 b) { return DS_UDIV(a, b); }

// ---- waits
void host_spend_cycles(u64 n);  // (hw.c)
// (busy waits of a given length: they take that much virtual time - the ARM9's 67 MHz CPU cycles are half as many
// bus cycles; SVC_WaitByLoop's loop takes 4 CPU cycles a turn)
void OS_SpinWait(u32 n) { host_spend_cycles(n / 2); }
void WaitByLoop(s32 n) { if (n > 0) host_spend_cycles((u64)n * 2); }
void RtcWaitBusy(void) {}

// ---- Metrowerks runtime: 64-bit logical shift right (lo, hi, n)
u64 func_02133540(u32 lo, u32 hi, u32 n) { return (((u64)hi << 32) | lo) >> (n & 63); }

// ---- NitroSDK alarm timer interrupt wrapper
void OSi_ArrangeTimer(void);
void OSi_AlarmHandler(void) { OSi_ArrangeTimer(); }

// ---- SHA-1 block transform (DGT): ctx->h[0..4] at offset 0; len is a multiple of 64
static u32 rol(u32 x, int n) { return x << n | x >> (32 - n); }
void DGTi_hash2_arm4_small(u32 *h, const u8 *data, u32 len) {
    for (; len >= 64; len -= 64, data += 64) {
        u32 w[80], a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
        for (int i = 0; i < 16; i++) w[i] = (u32)data[i * 4] << 24 | data[i * 4 + 1] << 16 | data[i * 4 + 2] << 8 | data[i * 4 + 3];
        for (int i = 16; i < 80; i++) w[i] = rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        for (int i = 0; i < 80; i++) {
            u32 f, k;
            if (i < 20) { f = (b & c) | (~b & d); k = 0x5a827999; }
            else if (i < 40) { f = b ^ c ^ d; k = 0x6ed9eba1; }
            else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8f1bbcdc; }
            else { f = b ^ c ^ d; k = 0xca62c1d6; }
            u32 t = rol(a, 5) + f + e + k + w[i];
            e = d; d = c; c = rol(b, 30); b = a; a = t;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
    }
}

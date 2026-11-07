// Host build: the ARM9's game card registers. The SDK reads the ROM itself (CARD_ReadRom): an 8-byte command at
// 0x040001a8, ROMCTRL (0x040001a4) bit 31 starts it, the data comes 4 bytes at a time from 0x04100010 (by the CPU
// or by DMA in card mode). Command 0xB7 reads 0x200 bytes at a ROM address; the others return 0xffffffff.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host.h"
#include "plat.h"

#define ROMCTRL 0x040001a4u
static FILE *sRom;
static u32 sRomSize;
static u8 sHeader[0x200];
static const char *sRomPath;  // (host_rom_path: pxi7.c's default save file)
static u32 sAddr, sLeft, sIsData;

// The image read from the file as the game asks for it, every read of the 3DS's SD card being slow. When the image
// fits in memory beside what the program needs later (ROM_SPARE), it is held whole: each 64 KB piece read once, at
// its first access (larger pieces would read much the game never asks for). Else (the Old 3DS: ACWW's image is
// 32 MB) through a cache of 32 KB blocks (1 MB, the least recently used one replaced); the game reads
// its files in runs of 512-byte pages, often from a few at a time.
enum { BLOCK = 0x8000, BLOCKS = 32, PIECE = 0x10000, ROM_SPARE = 12 << 20 };
static u8 sBlock[BLOCKS][BLOCK];
static u32 sBlockAddr[BLOCKS], sBlockUsed[BLOCKS], sUse;
static int sLast;
static u8 *sImage, *sPieceRead;  // the whole image; per piece: read
static void read_at(u32 addr, u8 *dst, u32 n) {
    memset(dst, 0xff, n);
    if (fseek(sRom, (long)addr, SEEK_SET) != 0 || (fread(dst, 1, n, sRom) == 0 && ferror(sRom)))
        host_fatal("the ROM file can't be read");
}
// (the flags atomic: the direct build reads the image in the background too, preload)
static int piece_read(u32 p) { return __atomic_load_n(&sPieceRead[p], __ATOMIC_ACQUIRE); }
static void image_load(u32 a, u32 n) {  // (the pieces under [a, a + n))
    for (u32 p = a / PIECE; p <= (a + n - 1) / PIECE; p++)
        if (!piece_read(p)) {
            u32 at = p * PIECE;
            read_at(at, sImage + at, sRomSize - at < PIECE ? sRomSize - at : PIECE);
            __atomic_store_n(&sPieceRead[p], 1, __ATOMIC_RELEASE);
        }
}
#ifdef N3DS_DIRECT
// The enhanced port: the rest of the image read in the background, by a thread with its
// own file handle that runs while the game's waits, so that the game's loads find their files in memory. A piece the
// game asks for first it still reads itself; one read by both at once gets the same bytes twice.
static void preload(void *arg) {
    (void)arg;
    FILE *f = fopen(sRomPath, "rb");
    for (u32 p = 0; f && p <= (sRomSize - 1) / PIECE; p++) {
        u32 at = p * PIECE, n = sRomSize - at < PIECE ? sRomSize - at : PIECE;
        if (!piece_read(p) && fseek(f, (long)at, SEEK_SET) == 0 && fread(sImage + at, 1, n, f) == n)
            __atomic_store_n(&sPieceRead[p], 1, __ATOMIC_RELEASE);
    }
    if (f) fclose(f);
}
#endif
static u8 rom_byte(u32 a) {
    if (a >= sRomSize) return 0xff;
    if (sImage) {
        if (!piece_read(a / PIECE)) image_load(a, 1);
        return sImage[a];
    }
    u32 addr = a & ~(u32)(BLOCK - 1);
    if (sBlockAddr[sLast] != addr) {
        int b = 0;
        for (int i = 0; i < BLOCKS; i++) {
            if (sBlockAddr[i] == addr) { b = i; goto found; }
            if (sBlockUsed[i] < sBlockUsed[b]) b = i;
        }
        sBlockAddr[b] = addr;
        read_at(addr, sBlock[b], BLOCK);
    found:
        sLast = b;
    }
    sBlockUsed[sLast] = ++sUse;
    return sBlock[sLast][a & (BLOCK - 1)];
}

void card_init(const char *path) {
    sRomPath = path;
    sRom = fopen(path, "rb");
    if (!sRom) { perror(path); exit(1); }
    if (fseek(sRom, 0, SEEK_END) != 0) { perror(path); exit(1); }
    long n = ftell(sRom);
    if (n < (long)sizeof sHeader) host_fatal("the ROM file is too small");
    sRomSize = (u32)n;
    for (int i = 0; i < BLOCKS; i++) sBlockAddr[i] = ~0u;  // (none held)
    void *spare = malloc(sRomSize + (size_t)ROM_SPARE);  // (the image only if this much fits)
    if (spare) {
        free(spare);
        sImage = malloc(sRomSize);
        sPieceRead = calloc(sRomSize / PIECE + 1, 1);
        if (!sImage || !sPieceRead) free(sImage), free(sPieceRead), sImage = 0, sPieceRead = 0;
    }
    fprintf(stderr, "card: the ROM image %s\n", sImage ? "held in memory" : "through a 1 MB cache");
    for (u32 i = 0; i < sizeof sHeader; i++) sHeader[i] = rom_byte(i);
#ifdef N3DS_DIRECT
    if (sImage && !host_plat_thread_start(preload, 0)) fprintf(stderr, "card: no thread to read the image ahead\n");
#endif
}

#ifdef N3DS_DIRECT
// The enhanced port's languages: ACWW_LANG (ENG, FRN, GER, ITA or SPN) takes that
// language's files from the EUR ROM in ACWW_LANG_ROM, the game's second archive (modern/ds/n3ds_lang.c). Its file
// name and allocation tables are held in memory: every file the game opens is looked up there first.
static FILE *sLang;
static u8 *sLangTab;  // the image's bytes [sLangAt, sLangEnd): both tables
static u32 sLangAt, sLangEnd;
static void lang_file(u32 at, void *dst, u32 n) {
    memset(dst, 0xff, n);
    if (fseek(sLang, (long)at, SEEK_SET) != 0 || (fread(dst, 1, n, sLang) == 0 && ferror(sLang)))
        host_fatal("ACWW_LANG_ROM can't be read");
}
// the language, or 0 (none chosen); tab: the FAT's offset and size, the FNT's
const char *host_lang_init(u32 tab[4]) {
    const char *lang = getenv("ACWW_LANG"), *path = getenv("ACWW_LANG_ROM");
    if (!lang || !*lang) return 0;
    if (strlen(lang) != 3 || !path) host_fatal("ACWW_LANG: ENG, FRN, GER, ITA or SPN, with the EUR ROM in ACWW_LANG_ROM");
    sLang = fopen(path, "rb");
    if (!sLang) { perror(path); exit(1); }
    u8 h[0x50];
    lang_file(0, h, sizeof h);
    if (memcmp(h + 0x0c, "ADMP", 4)) host_fatal("ACWW_LANG_ROM is not Animal Crossing: Wild World's EUR ROM (ADMP)");
    u32 fnt, fnt_size, fat, fat_size;
    memcpy(&fnt, h + 0x40, 4), memcpy(&fnt_size, h + 0x44, 4), memcpy(&fat, h + 0x48, 4), memcpy(&fat_size, h + 0x4c, 4);
    sLangAt = fnt < fat ? fnt : fat;
    sLangEnd = fnt + fnt_size > fat + fat_size ? fnt + fnt_size : fat + fat_size;
    if (sLangEnd - sLangAt > (4u << 20) || !(sLangTab = malloc(sLangEnd - sLangAt)))
        host_fatal("ACWW_LANG_ROM: no memory for its tables");
    lang_file(sLangAt, sLangTab, sLangEnd - sLangAt);
    tab[0] = fat, tab[1] = fat_size, tab[2] = fnt, tab[3] = fnt_size;
    fprintf(stderr, "card: the language %s from %s\n", lang, path);
    return lang;
}
void host_lang_read(void *dst, u32 at, u32 n) {
    if (at >= sLangAt && at <= sLangEnd && n <= sLangEnd - at) memcpy(dst, sLangTab + (at - sLangAt), n);
    else lang_file(at, dst, n);
}
#endif

const u8 *host_rom(u32 *size) { *size = sizeof sHeader; return sHeader; }  // (the header: main.c)
const char *host_rom_path(void) { return sRomPath; }

void host_card_dma(void);

void card_control_written(u32 ctrl) {
    if (!(ctrl >> 31)) return;
    const u8 *cmd = host_io(0x040001a8);
    u32 blk = ctrl >> 24 & 7;
    sLeft = blk == 0 ? 0 : blk == 7 ? 4 : 0x100u << blk;
    sIsData = cmd[0] == 0xb7;
    sAddr = (u32)cmd[1] << 24 | cmd[2] << 16 | cmd[3] << 8 | cmd[4];
    if (sLeft == 0) { *(u32 *)host_io(ROMCTRL) = ctrl & ~0x80800000u; return; }
    *(u32 *)host_io(ROMCTRL) = ctrl | 0x00800000;  // data ready
    host_card_dma();
}

// CARDi_ReadCard's fast path (HOST): a 512-byte page straight from the image
void host_card_read(u32 addr, void *dst, u32 n) {
    HOST_PROF(PROF_ROM);
    if (sImage && n && addr < sRomSize && n <= sRomSize - addr) {
        image_load(addr, n);
        memcpy(dst, sImage + addr, n);
        return;
    }
    for (u32 i = 0; i < n; i++) ((u8 *)dst)[i] = rom_byte(addr + i);
}

u32 card_read_data(void) {
    HOST_PROF(PROF_ROM);
    u32 v = 0xffffffff;
    if (sLeft) {
        if (sIsData) {
            v = 0;
            for (int i = 0; i < 4; i++) {
                u32 a = sAddr + i;
                if (a < 0x8000) a = 0x8000 + (a & 0x1ff);  // secure area reads mirror (unused by the SDK)
                v |= (u32)rom_byte(a) << (i * 8);
            }
        }
        sAddr += 4;
        sLeft -= 4;
        if (sLeft == 0) {
            u32 c = *(u32 *)host_io(ROMCTRL) & ~0x80800000u;
            *(u32 *)host_io(ROMCTRL) = c;
            if (*(u16 *)host_io(0x040001a0) & 0x4000) host_irq_raise(1u << 19);  // transfer complete
        }
    }
    return v;
}

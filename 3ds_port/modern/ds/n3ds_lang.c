// The enhanced port's languages: the user's EUR ROM as a second archive, "eur", beside
// the USA ROM's "rom", and the files the game opens by name taken from it where it has them in the chosen language:
// "/script/ENG/x" as "eur:/script/GER/x", "/item_info/x" as "eur:/item_info/GER/x", "/font/fontC_img.bin" as
// "eur:/font/fontC_img_ger.bin". Every other file stays the USA ROM's. The ROM itself is read by the runtime (card.c).
#ifdef N3DS_PORT
#include "nitro/fs.h"
#include "proto/autoload_2/unk_02119020.h"
#include "proto/autoload_2/unk_02119434.h"

// (src/autoload_2/unk_02119020.c, unk_02119434.c)
BOOL FS_RegisterArchiveName(FSArc *arc, const char *name, int len);
BOOL FS_LoadArchive(FSArc *arc, uptr base, u32 fat, u32 fat_size, u32 fnt, u32 fnt_size, FSIoFunc rd, FSIoFunc wr);
BOOL FSi_FindPath(FSFile *file, const char *path, FSFileID *id, FSDirPos *pos);

const char *host_lang_init(u32 tab[4]);       // modern/host/card.c
void host_lang_read(void *dst, u32 at, u32 n);

static FSArc sArc;
static char sLang[3], sLower[3];  // "GER", "ger"

static int ReadCallback(FSArc *arc, void *dst, u32 at, u32 n) {
    (void)arc;
    host_lang_read(dst, at, n);
    return 0;  // (done)
}
static int WriteCallback(FSArc *arc, void *src, u32 at, u32 n) {
    (void)arc, (void)src, (void)at, (void)n;
    return 1;  // (failed: the ROM can't be written)
}

// FSi_InitRom, after the USA ROM's archive (src/autoload_2/unk_02119de4.c)
void N3ds_LangInit(void) {
    u32 t[4];  // the FAT's offset and size, the FNT's
    const char *lang = host_lang_init(t);
    if (!lang) return;
    for (int i = 0; i < 3; i++) sLang[i] = lang[i], sLower[i] = lang[i] | 0x20;
    FS_InitArchive(&sArc);
    if (FS_RegisterArchiveName(&sArc, "eur", 3)) FS_LoadArchive(&sArc, 0, t[0], t[1], t[2], t[3], ReadCallback, WriteCallback);
}

int N3ds_LangEur(void) { return sLang[0] != 0; }  // (n3ds_options.h)

// p: "eur:" and path's first `keep` characters, then `ins` (len characters), then the rest of path
static BOOL Find(FSFileID *id, const char *path, int keep, const char *ins, int len, int skip) {
    char p[300];
    int n = 0;
    p[n++] = 'e', p[n++] = 'u', p[n++] = 'r', p[n++] = ':';
    for (int i = 0; i < keep; i++) p[n++] = path[i];
    for (int i = 0; i < len; i++) p[n++] = ins[i];
    for (const char *s = path + keep + skip; *s; s++) p[n++] = *s;
    p[n] = 0;
    FSFile f;
    FS_InitFile(&f);
    return FSi_FindPath(&f, p, id, 0);
}

// FS_ConvertPathToFileID (src/autoload_2/unk_02119434.c): the file's id in the EUR ROM, if it has the file in the
// language
BOOL N3ds_LangFileID(FSFileID *id, const char *path) {
    if (!sLang[0] || path[0] != '/') return 0;
    int n = 0, slash = 0;
    for (; path[n]; n++) {
        if (n >= 200) return 0;
        if (path[n] == '/') slash = n;
    }
    for (int i = 0; i + 5 <= n; i++)  // "/ENG/" -> "/GER/"
        if (path[i] == '/' && path[i + 1] == 'E' && path[i + 2] == 'N' && path[i + 3] == 'G' && path[i + 4] == '/')
            return Find(id, path, i + 1, sLang, 3, 3);
    char dir[4] = {'/', sLang[0], sLang[1], sLang[2]};  // "dir/name" -> "dir/GER/name"
    if (slash > 0 && Find(id, path, slash, dir, 4, 0)) return 1;
    char suffix[4] = {'_', sLower[0], sLower[1], sLower[2]};  // "_ger" before one of the name's '_' or '.'
    for (int k = slash + 1; k < n; k++)
        if ((path[k] == '_' || path[k] == '.') && Find(id, path, k, suffix, 4, 0)) return 1;
    return 0;
}
#endif

// The port's languages from a second ROM (N3DS_PORT): included once in card.c, after card_init.

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

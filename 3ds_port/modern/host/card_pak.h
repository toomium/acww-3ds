// A data pack in place of the ROM image: included once in card.c, used by card_init.
//
// game.pak (docs/pak-format.md in the port's repository; made by its web builder or `make pak`) holds the ROM's
// files without the game's code: a header, an index by file number, the files. Three of its entries are the tables
// the game's file system starts from (the file names, each file's place, the overlays' list), and one is a card
// header that says where those three are, all as offsets in the pack. So the pack is read as the card's image is,
// byte for byte, and only the header the game is shown is another: that entry instead of the pack's first bytes.
enum { PAK_ENTRIES = 0x44, PAK_HEADER = 0x10000000 };
static u32 pak_u32(u32 at) {
    return rom_byte(at) | rom_byte(at + 1) << 8 | rom_byte(at + 2) << 16 | (u32)rom_byte(at + 3) << 24;
}
// h: the file's first 0x200 bytes; if they are a pack's, the card header from it
static void pak_header(u8 *h) {
    if (memcmp(h, "ACWWPAK", 8)) return;  // (a ROM image)
    if (pak_u32(0x08) != 1) host_fatal("game.pak is of a version this build can't read: make it again");
    u32 count = pak_u32(0x40);
    if (count > (sRomSize - PAK_ENTRIES) / 12) host_fatal("game.pak is damaged (its index)");
    // the index is sorted by number, the port's own entries after the ROM's files
    for (u32 i = count; i-- > 0;) {
        u32 at = PAK_ENTRIES + i * 12, id = pak_u32(at), start = pak_u32(at + 4), size = pak_u32(at + 8);
        if (id < PAK_HEADER) break;
        if (id != PAK_HEADER) continue;
        if (size != 0x200 || start > sRomSize - size) break;
        fprintf(stderr, "card: a data pack, made for %.32s\n", (const char *)h + 0x0c);
        for (u32 k = 0; k < size; k++) h[k] = rom_byte(start + k);
        return;
    }
    host_fatal("game.pak is damaged (no header)");
}

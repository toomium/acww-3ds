// The HOME Menu version (acww.cia): a forwarder. It opens the Homebrew Launcher version,
// sdmc:/3ds/acww/acww.3dsx, the way the Homebrew Launcher does, so the two are one program: an update is the new
// .3dsx alone, and the .cia holds no game code.
//
// How: Luma3DS's loader runs a .3dsx in place of one title, its homebrew title (the Homebrew Launcher Loader's,
// 000400000D921E00, unless Rosalina was told another). Its port hb:ldr is told which .3dsx (command 2) and the
// program's arguments (command 3), as 3ds-hbmenu's source/loaders/rosalina.c does. Then this title is made the
// homebrew title for one start (Luma3DS's shared page: the title "selected", which becomes the homebrew title as
// this program closes, as Rosalina's "Switch the hb. title to the current app" does) and this program closes with
// a jump to itself (libctru's chainloader): Luma3DS loads the game in this title's place, so the HOME Menu shows
// this title as the one running. The game puts Luma3DS's own homebrew title back as the selected one as it starts
// (plat_3ds.c), so the next start runs this program again. All of it is GabeSud0/pokeplatinum-3ds's
// forwarder's way.
//
// Where the shared page isn't laid out as known (another Luma3DS): a jump to the homebrew title, in which the
// game then runs (the HOME button shows that title's icon).
// shortcut: a missing homebrew title is not noticed on that second way (the jump just fails); ask AM for the
// title first if someone meets it.
#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define PROGRAM "sdmc:/3ds/acww/acww.3dsx"
#define HOMEBREW_TITLE 0x000400000D921E00ULL  // (Luma3DS's default)

// Luma3DS's shared page, its config part (Luma3DS's own layout): the homebrew title now, and the one it becomes
// when the running program closes.
typedef struct {
    u64 hbldr3dsxTid;
    u64 selectedHbldr3dsxTid;
    bool useHbldr;
} LumaSharedConfig;
#define LUMA_SHARED_CONFIG ((volatile LumaSharedConfig *)(OS_SHAREDCFG_VADDR + 0x800))

static Handle hbldr;
static u32 args[0x400 / 4];  // Luma3DS's loader's argument buffer: argc, then the arguments one after the other

static Result hbldr_send(u16 command, const void *data, u32 size, unsigned buffer) {
    u32 *cmd = getThreadCommandBuffer();
    cmd[0] = IPC_MakeHeader(command, 0, 2);
    cmd[1] = IPC_Desc_StaticBuffer(size, buffer);
    cmd[2] = (u32)data;
    Result rc = svcSendSyncRequest(hbldr);
    return R_SUCCEEDED(rc) ? (Result)cmd[1] : rc;
}

static void problem(const char *what, const char *help) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    printf("\n  Animal Crossing: Wild World 3DS\n\n  %s\n\n%s\n\n  Press any button to close.\n", what, help);
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown()) break;
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    gfxExit();
}

int main(void) {
    struct stat st;
    if (stat(PROGRAM, &st) != 0) {
        problem("The game isn't on the SD card.",
                "  The HOME Menu version opens\n  sd:/3ds/acww/acww.3dsx: copy it\n  there, with game.pak beside it\n"
                "  (the web builder makes both).");
        return 0;
    }
    if (R_FAILED(svcConnectToPort(&hbldr, "hb:ldr"))) {
        problem("This needs Luma3DS.",
                "  The HOME Menu version opens the\n  game through Luma3DS's homebrew\n  loader. Update Luma3DS, or open\n"
                "  acww.3dsx from the Homebrew\n  Launcher.");
        return 0;
    }
    // Luma3DS's homebrew title, and whether this title can take its place for the game's start: Luma3DS, its
    // loader on, its shared page laid out as known (the homebrew title there the one its kernel extension says).
    s64 version = 0, title = 0;
    u64 self = 0;
    if (R_FAILED(svcGetSystemInfo(&title, 0x10000, 0x100)) || title == 0) title = (s64)HOMEBREW_TITLE;
    bool own = R_SUCCEEDED(svcGetSystemInfo(&version, 0x10000, 0)) && LUMA_SHARED_CONFIG->useHbldr &&
               LUMA_SHARED_CONFIG->hbldr3dsxTid == (u64)title && R_SUCCEEDED(APT_GetProgramID(&self)) && self != 0;
    // argv[0]: the program's path, as the Homebrew Launcher gives it (and nothing more: the game's main takes any
    // other argument as its ROM's path)
    args[0] = 1;
    strcpy((char *)&args[1], PROGRAM);
    // (the loader takes the path without "sdmc:")
    Result rc = hbldr_send(2, PROGRAM + 5, sizeof PROGRAM - 5, 0);
    if (R_SUCCEEDED(rc)) rc = hbldr_send(3, args, sizeof args, 1);
    svcCloseHandle(hbldr);
    if (R_FAILED(rc)) {
        problem("Luma3DS didn't take the game.", "  Open acww.3dsx from the Homebrew\n  Launcher instead.");
        return 0;
    }
    if (own) {  // the game in this title's place
        LUMA_SHARED_CONFIG->selectedHbldr3dsxTid = self;
        aptSetChainloader(self, MEDIATYPE_SD);
        return 0;
    }
    // (on closing: the jump, which the HOME Menu does)
    aptSetChainloader((u64)title, ((u64)title >> 32) == 0x00040000u ? MEDIATYPE_SD : MEDIATYPE_NAND);
    return 0;
}

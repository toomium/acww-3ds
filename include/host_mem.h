#ifndef HOST_MEM_H
#define HOST_MEM_H

// The DS memory the game reaches by address, as host objects (docs/portability.md 3.2): the system work area at the
// end of main RAM, DTCM, ITCM, the palettes, OAM, VRAM and the main RAM arena. Nothing is mapped at their DS
// addresses; a DS address written in the source goes through these:
// - HW_SYSWORK(T, m): a T * to member m of the system work area (0x027ff000-0x027fffff), HW_SYSWORK_AT(T, m, o)
//   o bytes into it. On 32-bit hosts HostSysWork has the DS's layout (checked in modern/host/mem.c); on 64-bit hosts
//   its pointers (the arena bounds, the thread info, the exception vector) are pointer-sized.
// - HW_DTCM_BLOCK, HW_ITCM_BLOCK: DTCM (16 KB: the arenas, the IRQ stack checks, HW_INTR_CHECK_BUF) and ITCM (32 KB:
//   only an arena on the host). The DTCM module's variables are ordinary variables.
// - HW_PLTT, HW_OAM: the palettes (0x05000000, 2 KB) and OAM (0x07000000, 2 KB).
// - HostVram_CpuPtr(a): the CPU's view of VRAM address a (0x06000000-0x068a3fff), resolved through the CPU windows
//   of modern/host/vram.c, which follow VRAMCNT as the DS's do (a pointer kept across a VRAMCNT change sees what the
//   DS would show there).
// - HostMainRam_Ptr(a): main RAM address a in 0x02200000-0x023fffff (the arena past the program's data: the DS's
//   main arena, which the 32-bit host's root heap uses).
// Self-contained (no includes): the game's units with their own basic types include it too.

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HostSysWork {
    unsigned char sharedArena[0x680];       // 0x027ff000 HW_SHARED_ARENA (OS arena 5)
    unsigned char reserved_680[0x400];      // 0x027ff680 (0x027ff814: a boot flag, OS_IsRunOnEmulator-style check)
    unsigned char cardRomHeader[0x160];     // 0x027ffa80 HW_CARD_ROM_HEADER
    unsigned char reserved_be0[0x20];       // 0x027ffbe0
    unsigned char bootCheckInfo[0x20];      // 0x027ffc00 HW_BOOT_CHECK_INFO_BUF (+0: card id, +0x10, +0x14)
    unsigned int resetParameter;            // 0x027ffc20 HW_RESET_PARAMETER_BUF
    unsigned char reserved_c24[8];          // 0x027ffc24
    unsigned int romBaseOffset;             // 0x027ffc2c HW_ROM_BASE_OFFSET_BUF
    unsigned char ctrdgModuleInfo[0xc];     // 0x027ffc30 HW_CTRDG_MODULE_INFO_BUF
    unsigned int vblankCount;               // 0x027ffc3c HW_VBLANK_COUNT_BUF
    unsigned char wmBootBuf[0x40];          // 0x027ffc40 HW_WM_BOOT_BUF (+0: the boot type)
    unsigned char nvramUserInfo[0x100];     // 0x027ffc80 HW_NVRAM_USER_INFO (+0x74: the MAC address)
    unsigned char biosExcpStack[0x10];      // 0x027ffd80 HW_BIOS_EXCP_STACK_MAIN
    unsigned char reserved_d90[0xc];        // 0x027ffd90
    void *excpVector;                       // 0x027ffd9c HW_EXCP_VECTOR_MAIN
    void *arenaLo[9];                       // 0x027ffda0 HW_ARENA_INFO_BUF (OSArenaInfo.lo)
    void *arenaHi[9];                       // 0x027ffdc4 (OSArenaInfo.hi)
    unsigned int realTimeClock[2];          // 0x027ffde8 HW_REAL_TIME_CLOCK_BUF (date, time; +2: the status)
    unsigned char dmaClearData[0x10];       // 0x027ffdf0 HW_DMA_CLEAR_DATA_BUF
    unsigned char romHeader[0x160];         // 0x027ffe00 HW_ROM_HEADER_BUF
    unsigned char isdReserved[0x20];        // 0x027fff60 HW_ISD_RESERVED
    unsigned int pxiSignalParam[2];         // 0x027fff80 HW_PXI_SIGNAL_PARAM_ARM9 / ARM7
    unsigned int pxiHandleChecker[2];       // 0x027fff88 HW_PXI_HANDLE_CHECKER_ARM9 / ARM7
    unsigned int micLastAddress;            // 0x027fff90 HW_MIC_LAST_ADDRESS
    unsigned short micSamplingData;         // 0x027fff94 HW_MIC_SAMPLING_DATA
    unsigned short wmCallbackControl;       // 0x027fff96 HW_WM_CALLBACK_CONTROL
    unsigned short wmRssiPool;              // 0x027fff98 HW_WM_RSSI_POOL
    unsigned char reserved_f9a[2];          // 0x027fff9a
    unsigned int componentParam;            // 0x027fff9c HW_COMPONENT_PARAM
    void *threadInfoMain;                   // 0x027fffa0 HW_THREADINFO_MAIN (OSThreadInfo *)
    unsigned int threadInfoSub;             // 0x027fffa4 HW_THREADINFO_SUB (the ARM7's)
    unsigned short buttonXY;                // 0x027fffa8 HW_BUTTON_XY_BUF
    unsigned short touchPanel[2];           // 0x027fffaa HW_TOUCHPANEL_BUF
    unsigned short autoloadSync;            // 0x027fffae HW_AUTOLOAD_SYNC_BUF
    unsigned int lockIdFlagMain[2];         // 0x027fffb0 HW_LOCK_ID_FLAG_MAIN
    unsigned int lockIdFlagSub[2];          // 0x027fffb8 HW_LOCK_ID_FLAG_SUB
    unsigned int vramCLock[2];              // 0x027fffc0 HW_VRAM_C_LOCK_BUF
    unsigned int vramDLock[2];              // 0x027fffc8 HW_VRAM_D_LOCK_BUF
    unsigned int wram0Lock[2];              // 0x027fffd0 HW_WRAM_BLOCK0_LOCK_BUF
    unsigned int wram1Lock[2];              // 0x027fffd8 HW_WRAM_BLOCK1_LOCK_BUF
    unsigned int cardLock[2];               // 0x027fffe0 HW_CARD_LOCK_BUF
    unsigned int ctrdgLock[2];              // 0x027fffe8 HW_CTRDG_LOCK_BUF
    unsigned int initLock[2];               // 0x027ffff0 HW_INIT_LOCK_BUF
    unsigned short mmemCheckerMain;         // 0x027ffff8 HW_MMEMCHECKER_MAIN
    unsigned short mmemCheckerSub;          // 0x027ffffa HW_MMEMCHECKER_SUB
    unsigned char reserved_ffc[4];          // 0x027ffffc
} HostSysWork;

extern HostSysWork gHostSysWork;
#define HW_SYSWORK(T, m) ((T *)&gHostSysWork.m)
#define HW_SYSWORK_AT(T, m, o) ((T *)((unsigned char *)&gHostSysWork.m + (o)))

// HOST_MEM_RUNTIME_ALIGN (Windows, macOS): COFF objects can't ask for more than 8 KB of alignment, Mach-O ones for
// more than 32 KB, so the blocks aligned more (DTCM, ITCM, the main RAM block; vram.c's banks and windows) are
// allocated at start-up (HostMem_Init, from main.c's early_init) and their names stand for the allocations (sizeof
// still gives the block's size)
#if defined(_WIN32) || defined(__APPLE__)
#define HOST_MEM_RUNTIME_ALIGN 1
#else
#define HOST_MEM_RUNTIME_ALIGN 0
#endif

#if HOST_MEM_RUNTIME_ALIGN
extern unsigned char (*gHostDtcmBlock)[0x4000], (*gHostItcmBlock)[0x8000];
#define gHostDtcm (*gHostDtcmBlock)
#define gHostItcm (*gHostItcmBlock)
#else
extern unsigned char gHostDtcm[0x4000];
extern unsigned char gHostItcm[0x8000];
#endif
#define HW_DTCM_BLOCK (gHostDtcm)
#define HW_ITCM_BLOCK (gHostItcm)
#define HW_INTR_CHECK_BUF ((volatile unsigned int *)(gHostDtcm + 0x3ff8))

extern unsigned char gHostPltt[0x800];
extern unsigned char gHostOam[0x800];
#define HW_PLTT (gHostPltt)  // 0x05000000: BG palette A, +0x200 OBJ A, +0x400 BG B, +0x600 OBJ B
#define HW_OAM (gHostOam)    // 0x07000000: OAM A, +0x400 OAM B

void *HostVram_CpuPtr(unsigned int dsaddr);
// the enhanced 3DS port (N3DS_DIRECT): an LCDC address of banks A-I in the bank's own memory,
// for the SDK's texture uploads, which then leave the banks where they are (vram.c)
void *host_vram_lcdc_direct(unsigned int dsaddr);

#define HOST_MAIN_RAM_LO 0x02200000u
#define HOST_MAIN_RAM_HI 0x02400000u
#if HOST_MEM_RUNTIME_ALIGN
extern unsigned char (*gHostMainRamBlock)[HOST_MAIN_RAM_HI - HOST_MAIN_RAM_LO];
#define gHostMainRam (*gHostMainRamBlock)
#else
extern unsigned char gHostMainRam[HOST_MAIN_RAM_HI - HOST_MAIN_RAM_LO];
#endif
void *HostMainRam_Ptr(unsigned int dsaddr);

// the empty private-WRAM arena (OS arena 6: lo == hi on the DS)
extern unsigned char gHostWramArena[32];

// The game's variables: ordinary data and bss of the program (docs/portability.md 3.3), between the variables of
// modern/link/game_begin.c and game_end.c, the first and last of the game's objects in the link (linkers keep the
// input order within .data and .bss). Its read-only data (and, in PIE builds, .data.rel.ro) lies elsewhere.
extern char gHostGameDataBegin[1], gHostGameDataEnd[1], gHostGameBssBegin[1], gHostGameBssEnd[1];

// is [p, p + n) inside one of the blocks above, the game's data and bss, or (64-bit hosts) the root heap's block?
int HostMem_IsGameRam(const void *p, unsigned int n);

#ifdef __cplusplus
}
#endif

#endif

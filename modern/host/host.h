// Host build runtime (modern/host/): shared declarations. See docs/host.md.
#ifndef HOST_HOST_H
#define HOST_HOST_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef unsigned int u32;
typedef signed int s32;  // (the game's types, include/types.h: uint32_t is unsigned long on arm-none-eabi)
typedef unsigned long long u64;  // (the game's type: uint64_t is unsigned long on 64-bit Linux)

// main.c
void host_fatal(const char *msg) __attribute__((noreturn));
void host_unimplemented(const char *what);
extern int gHostVerbose;
extern const char *gHostRomPath;

// HOST_DEBUG_POSIX: the debugging aids built on POSIX signals (debug_posix.c: crash report, SIGINT dump,
// ACWW_PROFILE, ACWW_WATCH). On by default on POSIX systems; nothing else depends on them.
#ifndef HOST_DEBUG_POSIX
#if defined(_WIN32)
#define HOST_DEBUG_POSIX 0
#else
#define HOST_DEBUG_POSIX 1
#endif
#endif

// hw.c: memory map, I/O registers, time, DMA
void host_map_memory(void);
int host_highmem(void);                // ACWW_HIGHMEM: the low-4 GB poisoning harness (x86-64 Linux)
void host_highmem_init(void);          // reserve the low 4 GB
void HostMem_Init(void);               // mem.c: the host memory blocks allocated at start-up (HOST_MEM_RUNTIME_ALIGN)
void HostMem_Check(void);              // mem.c: the game's variables and the host memory blocks as expected
void *HostMem_AlignedBlock(size_t size, size_t align);  // zeroed, aligned, never freed
u8 *host_io(u32 addr);                 // an I/O register (the register model's storage)
// the game's register accesses (include/nitro/io_access.h): the register's address as a pointer-sized number
unsigned long long HostIo_Read(uintptr_t addr, unsigned int size);
unsigned long long HostIo_Write(uintptr_t addr, unsigned int size, unsigned long long value);
void host_io_ring_dump(void);          // the last register accesses (stderr)
void host_quit(void) __attribute__((noreturn));  // the end of a run: reports, then _Exit (hw.c)
void host_set_paused(int paused);  // the frontend holds the game (the watchdog keeps quiet)
void host_exit_dumps(void);            // the reports written at exit (ACWW_IOHIST, ACWW_PROFILE)
#include "prof.h"  // (ACWW_FPS's profile)

// debug_posix.c (empty without HOST_DEBUG_POSIX)
void host_debug_init(void);
void host_debug_crash_handlers(void);  // (re)install the crash report's handlers
void host_debug_exit_dumps(void);
u32 host_irq_pending(void);            // IE & IF when IME is set
void host_irq_ack(u32 mask);
void host_irq_raise(u32 mask);
void host_irq_call(int bit);  // os.c: one interrupt handler at once (the H-blank, hw.c)
void host_advance_to_next_event(void);
u64 host_frame(void);
u64 host_cycles(void);
// a background thread (hw.c; another core: the 3DS's core 2 or 1) that ends when host_threads_quitting says so,
// calling host_thread_stopped: the program's end waits for it (the 3DS frees the threads' stacks)
// (otherCore: 0 the game's core, 1 another (the New 3DS's core 2 first), 2 another, the 3DS's system core 1 first)
int host_thread_start(void (*fn)(void *), void *arg, int otherCore);
int host_threads_quitting(void);
void host_thread_stopped(void);
void host_spend_cycles(u64 n);  // a busy wait of n cycles (hw.c; nothing with ACWW_TIMING=legacy)

// os.c
void *host_stack_alloc(size_t n);  // a stack for game code
void host_deliver_irqs(void);

// pxi7.c: the ARM7 side of the PXI FIFO
void pxi7_init(void);
void pxi7_receive(u32 word);           // a word the ARM9 sent
void pxi7_send(u32 word);              // a word for the ARM9 (receive FIFO)
void pxi7_sync_written(u32 value);     // IPCSYNC write

// card.c: ROM reads through the ARM9 card registers
void card_init(const char *rom);
void card_control_written(u32 romctrl);
u32 card_read_data(void);

#endif

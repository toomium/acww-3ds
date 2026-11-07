// Host build: the frontend backend of the i386 build (frontend.h): the AcwwShm block is POSIX shared memory, shown
// by the viewer (modern/host/viewer/viewer.c, a 64-bit SDL program: the 32-bit game process can't load the system's
// 64-bit SDL), which writes keys and the touch position back. Started here unless there is no display or it can't be
// found. The 64-bit builds use frontend_sdl.c instead (HOST_FRONTEND_SDL), or frontend_none.c without SDL2.
#if !HOST_FRONTEND_SDL && !HOST_FRONTEND_NONE && !HOST_FRONTEND_3DS

#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "frontend.h"

static char sName[64];

AcwwShm *frontend_backend_open(const char *argv0) {
    if (!getenv("DISPLAY") && !getenv("WAYLAND_DISPLAY")) return 0;
    snprintf(sName, sizeof sName, "%s-%d", ACWW_SHM_NAME_DEFAULT, (int)getpid());
    int fd = shm_open(sName, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd < 0 || ftruncate(fd, sizeof(AcwwShm)) < 0) { perror("frontend: shm"); return 0; }
    AcwwShm *shm = mmap(0, sizeof(AcwwShm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (shm == MAP_FAILED) return 0;
    memset(shm, 0, sizeof *shm);
    shm->magic = ACWW_SHM_MAGIC;
    // the viewer: ACWW_VIEWER, or acww-viewer next to this program
    char viewer[1024];
    const char *v = getenv("ACWW_VIEWER");
    if (v) snprintf(viewer, sizeof viewer, "%s", v);
    else {
        snprintf(viewer, sizeof viewer, "%s", argv0);
        char *slash = strrchr(viewer, '/');
        snprintf(slash ? slash + 1 : viewer, sizeof viewer - (slash ? slash + 1 - viewer : 0), "acww-viewer");
    }
    pid_t pid = fork();
    if (pid == 0) {
        // the viewer is a 64-bit program: drop the 32-bit sysroot's library path
        unsetenv("LD_LIBRARY_PATH");
        execl(viewer, viewer, sName, (char *)0);
        perror(viewer);
        _exit(1);
    }
    return shm;
}

void frontend_backend_frame(AcwwShm *shm) { (void)shm; }  // (the viewer polls the block)

void frontend_backend_close(void) {
    if (sName[0]) shm_unlink(sName);
}

#endif

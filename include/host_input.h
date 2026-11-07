#ifndef HOST_INPUT_H
#define HOST_INPUT_H

// The 3DS's circle pad for the game (modern/host/frontend.c sets it each frame; ACWW_ANALOG=0 turns it off): the
// direction as the game's pad angle (gPad.dir: 0 down, 0x4000 right, -0x8000 up, -0x4000 left) and the tilt
// (0x1000 full; 0: not tilted, the D-pad's angles as on the DS). Self-contained: the game's units include it.
#ifdef __cplusplus
extern "C" {
#endif
extern volatile short gHostStickAngle;
extern volatile unsigned short gHostStickTilt;
#ifdef __cplusplus
}
#endif

#endif

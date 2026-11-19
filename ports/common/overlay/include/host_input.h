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
// The D-pad's own directions held (KEYINPUT's bits 4-7: right left up down), without the circle pad's or the stick's
// (the backends set it; the game's keys have both): "New Leaf controls" swap tools with its left and right
// (modern/port/tool_swap.cpp). host_dpad_tap: a left (0x20) or right (0x10) newly pressed in the last 20 frames, taken
// (0 none); a tap during the equip's animation thus waits for the player to stand again.
extern volatile unsigned char gHostDpad;
unsigned host_dpad_tap(void);
#ifdef __cplusplus
}
#endif

#endif

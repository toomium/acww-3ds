#include "ds_div.h"
#include "host_mem.h"
#include "host_shared_calls.h"
#include "gfx/HBlankTask.h"
#include "uptr.h"
#include "types.h"
#include "sys/ClockDateTime.h"
#include "gfx/VecFx32.h"
#include "Unk_020d8c7c.h"
#include "net/CommManager.h"
#include "gfx/Mtx43.h"
#include "game/EventDayEntry.h"
#include "item/PickedItem.h"
#include "gfx/StarTwinkle.h"
#include "item/ItemPickSpec.h"
#include "snd/SndEnvChannel.h"
#include "gfx/SpriteAnim.h"
#include "game/WeatherRecord.h"
#include "item/Letter.h"
#include "game/FxVec3.h"
#include "snd/RainSndChannel.h"
#include "snd/SkySndChannel.h"
#include "game/SkyProc.h"
#include "actor/Actor.h"
#include "gfx/Rgb555.h"
#include "actor/SpNpcActor.h"
#include "talk/SpNpcTalkRequest.h"
#include "talk/SpNpcKatieTalk.h"
#include "npc/SpNpcKatie.h"
#include "nitro/io_access.h"
#include "actor/NpcActor.h"
#include "gfx/Camera.h"
#include "item/LetterView.h"
#include "player/PlayerData.h"
#include "save/SaveData.h"
#include "nitro/fs.h"
#include "town/PeteFallState.h"
#include "town/VisitorSpawnFlags.h"
#include "player/PlayerDailyTalkFlags.h"
#include "proto/autoload_2/unk_02119434.h"
#include "gfx/ScreenTransition.h"
#include "proto/main/unk_020411dc.h"
#include "proto/main/unk_020c00c0.h"
#include "proto/main/unk_02065330.h"
#include "proto/main/unk_0206269c.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_02133d04.h"
#include "proto/itcm/unk_01ffcc60.h"
#include "proto/main/unk_02000c2c.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020027b4.h"
#include "proto/main/unk_02002b1c.h"
#include "proto/main/unk_0203ecec.h"
#include "proto/main/unk_020407fc.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02063f3c.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_020815f8.h"
#include "proto/main/unk_020850e0.h"
#include "proto/main/unk_02085940.h"
#include "proto/main/unk_02087e70.h"
#include "proto/main/unk_02093ff0.h"
#include "proto/main/unk_020943dc.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02095cdc.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_0209eb94.h"
#include "proto/main/unk_020afeb4.h"
#include "proto/main/unk_020b0774.h"
#include "proto/main/unk_020b0e60.h"
#include "proto/main/unk_020b7190.h"
#include "proto/main/unk_020b8d9c.h"
#include "proto/main/unk_020c00c0.h"
#include "proto/ov003/unk_ov003_02204ce8.h"
#include "proto/ov003/unk_ov003_022192b4.h"

// ======== class types (global scope) ========
class SkyProc;
struct MinuteHour;
struct WeatherManager;
struct SkyLightingLocals;
struct SkySpawnRate;
struct Letter;
struct ItemPickSpec;
struct PickedItem;
struct StarWishLetterVars;
struct Unk_020bcb04_Ent;
struct SpriteAnim;
struct SkySprite;
struct SkyObjGfxSlot;
struct SkyObjGfxLoader;
struct SkyShotRequest;
struct SndEnvChannel;
struct SkySndChannel;
struct RainSndChannel;
struct FxVec3;
struct SkySePlayer;
struct SkyShotSequence;
struct SkyObjPalette;
struct SkySprites;
struct SkyObjGfxDef;
struct SkyObjCharLayout;
struct SkyLookAtVec;
struct Unk_020bee28_Vec2;
struct FireworkColorPair;
struct Unk_020bee28_V;
struct SkyParticleStackPad;
struct SkyShootingStarStackPad;
struct Mtx43;
struct WeatherRecord;
struct EventDayEntry;
class SpNpcTalkRequest;
class SpNpcKatieTalk;
class SpNpcActor;
class SpNpcKatie;
// {minute, hour} as Clock_GetMinuteHour writes them
struct MinuteHour {
    /* 0x0 */ u8 minute;
    /* 0x1 */ u8 hour;
};
// Per-line affine parameters of the cloud BG layer (BG3 reference point x/y and pa scale), written by
// Sky_UpdateLineTables, copied to the BG registers by the H-blank handler and the V-blank functions.
struct SkyLineAffine {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s16 pa;
    /* 0xa */ u8 pad_0a[2];
};
// Sky backdrop gradient (sSkyGradient): SkyGradient_Build interpolates the 192 line colours of one buffer between the
// two key colours/lines set by Sky_BlendPalettes (palette colours 4/5); bufferIndex is the buffer last built.
struct SkyGradient {
    /* 0x000 */ s32 bufferIndex;
    /* 0x004 */ u16 lineColors[2][0xc0];
    /* 0x304 */ u16 keyColors[2];
    /* 0x308 */ s32 keyLines[2];

    void setKey(s32 idx, u16 color, s32 line);
};
// Weather and sky state (gWeatherManager): cloud level 0..4 and its transitions, the line tables of the cloud BG,
// the star BG scroll and the sky palettes.
struct WeatherManager {
    /* 0x0000 */ s32 engine;
    /* 0x0004 */ s32 lineBufferIndex;
    /* 0x0008 */ SkyLineAffine lineAffine[2][0xc0];
    /* 0x1208 */ s32 cloudScroll;
    /* 0x120c */ s32 transitionProgress;
    /* 0x1210 */ u16 lineBlend[2][0xc0];
    /* 0x1510 */ u8 pad_1510[4];
    /* 0x1514 */ s32 screenOffsetY[2];
    /* 0x151c */ s32 bufferIndex;
    /* 0x1520 */ s32 transitionBusy;
    /* 0x1524 */ s32 level;
    /* 0x1528 */ s32 targetLevel;
    /* 0x152c */ s32 requestedLevel;
    /* 0x1530 */ s32 direction;
    /* 0x1534 */ s32 precipKind;
    /* 0x1538 */ s16 rainSlant;
    /* 0x153a */ u8 pad_153a[2];
    /* 0x153c */ u16 starScrollX;
    /* 0x153e */ u16 starScrollY;
    /* 0x1540 */ u8 pad_1540[4];
    /* 0x1544 */ u8 *paletteFiles[2][2];
    /* 0x1554 */ u32 paletteFileSizes[2][2];
    /* 0x1564 */ u16 *palettes[3]; // blended sky BG colours: [0] current level, [1] target level, [2] mix of both
    /* 0x1570 */ s32 cloudVariant;
    /* 0x1574 */ s32 nextCloudVariant;
    /* 0x1578 */ s32 streamBlock;
    /* 0x157c */ u8 *curCloudScreen;
    /* 0x1580 */ u32 curCloudScreenSize;
    /* 0x1584 */ u8 *cloudScreen;
    /* 0x1588 */ u32 cloudScreenSize;
    /* 0x158c */ s32 transitionStep;
    /* 0x1590 */ s32 transitionTimer;
    /* 0x1594 */ s32 prevPlayerZ;

    WeatherManager();
    ~WeatherManager();
    void streamCloudScreen();
    void loadNextCloudGraphics();
    BOOL loadCloudScreen(u8 **p1, u32 *p2, s32 mode, s32 idx);
    BOOL loadCloudChars(s32 idx, s32 unused);
    s32 getPatternWeather(s32 a, s32 b);
    s32 getWeatherAtOffset(s32 v);
    void updateHourly();
    BOOL requestLevel(s32 v);
    void startTransition(s32 v);
    void setLevels(s32 a, s32 b);
    s32 getPrecipKind(s32 v);
    void init();
    void func_020ba49c();
    void getSkyBlend(s32 *a, s32 *b);
    void rollRainSlant();
    void updateTransitionTimed();
    void transitionTimedWait();
    void transitionTimedFinish();
    void transitionTimedBlend();
    void transitionTimedBegin();
    void updateTransitionStreamed();
    void streamCurrentCloudScreen();
    void reloadCurrentCloudGraphics();
};
// Lighting of the scene for the current hour and weather (Sky_UpdateLighting): light colours, light parameters
struct SkyLight {
    /* 0x0 */ u16 colors[4];
    /* 0x8 */ u8 params[2];
    /* 0xa */ u8 pad_0a[2];
};
// Locals of Sky_UpdateLighting kept together in their stack order: room light colour, current time, 3D clear colour
// (gradient key 1) and the colour argument of NNS_G3dGlbMaterialColorSpecEmi
struct SkyLightingLocals {
    /* 0x0 */ u16 roomColor;
    /* 0x2 */ MinuteHour time;
    /* 0x4 */ u16 clearColor;
    /* 0x6 */ u16 roomColorArg;
};
// Slingshot shot request of one player (SkySprites::shotRequests): set by SkySprites::onSlingshotFired from the player's
// position, turned into kind 11 sprites (3 with the golden slingshot) by the shot spawner.
struct SkyShotRequest {
    /* 0x00 */ s32 playerSlot;
    /* 0x04 */ VecFx32 pos;
    /* 0x10 */ u8 golden;
    /* 0x11 */ u8 pending;

    SkyShotRequest();
    ~SkyShotRequest();
    void clear();
    void set(s32 a, s32 *p, u8 b);
};
enum Unk_020bb8a4_E { Unk_020bb8a4_E_0 = 0 };
// Row of sRainSpawnRates / sSnowSpawnRates (per precipitation strength): spawn interval = base + Random(random)
struct SkySpawnRate {
    /* 0x0 */ s32 baseInterval;
    /* 0x4 */ s32 randomInterval;
};
// Graphics slot of the sky sprites (SkySprites::gfxSlots, 5): which sky object's characters are loaded into the slot's
// OBJ VRAM area, by how many sprites it is used, and whether the characters still have to be loaded / uploaded.
struct SkyObjGfxSlot {
    /* 0x00 */ s32 gfxId;
    /* 0x04 */ s32 refCount;
    /* 0x08 */ u8 loaded;
    /* 0x09 */ u8 dirty;
    /* 0x0a */ u8 unk_0a[2];

    SkyObjGfxSlot();
};
// Row of sSkySpriteAnimSeqs: the animation sequence of a sky sprite and its play-once flag (SkySprite_StartAnim).
struct SkySpriteAnimDef {
    /* 0x00 */ SpriteAnimSeq seq;
    /* 0x08 */ s32 playOnce;
};
// Locals of SkySprites::sendWishLetters: variant of the "ev_star" letter and its present
struct StarWishLetterVars {
    /* 0x0 */ u8 msgVariant;
    /* 0x1 */ u8 pad_01;
    /* 0x2 */ u16 present;
};
struct Unk_020bcb04_Ent {
    u16 id;
    u8 pad[10];
};
// One 16-colour row of the moon palette file (a_sky_moon_obj_ncl.bin, SkyObjGfxLoader::moonPalette), one per night
// hour: colours 3..7 for a cloudy sky, 11..15 for a clear one (SkySprite_CalcMoonColors)
struct SkyMoonPaletteRow {
    /* 0x00 */ u16 unk_00[3];
    /* 0x06 */ u16 cloudyColors[5];
    /* 0x10 */ u16 unk_10[3];
    /* 0x16 */ u16 clearColors[5];
};
// Loader of the sky objects' graphics (SkySprites::loader): the file, a read buffer, the characters of the 5 graphics
// slots (8 tile rows of 0x180 bytes, the slots' parts side by side), the source palettes (a_sky_obj_ncl.bin) and the
// moon palettes (a_sky_moon_obj_ncl.bin). init(): SkyObjGfxLoader_Init.
struct SkyObjGfxLoader {
    /* 0x0000 */ FSFile file;
    /* 0x0048 */ u8 readBuf[0x400];
    /* 0x0448 */ u8 chars[0xc00];
    /* 0x1048 */ u8 palettes[0x1c0];
    /* 0x1208 */ SkyMoonPaletteRow moonPalette[10];
    /* 0x1348 */ u32 magic;
    void init();
};
#ifndef HOST64
static_assert(sizeof(SkyObjGfxLoader) == 0x134c, "SkyObjGfxLoader");
#endif
// Sky sprite (0x74; SkySprites::sprites holds 60): rain drop, snow flake, particle, shooting star, moon, balloon, UFO,
// Pete, lightning bolt, firework, rainbow, slingshot shot or bird (kind 0..12; 13 = free). initByKind/updateByKind/
// endByKind dispatch to the kind's functions (methods here, SkySprite_* free functions for kinds 1..5).
struct SkySprite {
    /* 0x00 */ s32 kind;
    /* 0x04 */ s32 state; // 0 free, 1 starting, 2 running, 3 ending
    /* 0x08 */ s32 gfxId;
    /* 0x0c */ s32 animSeq;
    /* 0x10 */ SpriteAnim anim;
    /* 0x24 */ s32 gfxSlot;
    /* 0x28 */ s32 trackX;
    /* 0x2c */ s16 wavePhase;
    /* 0x2e */ u8 dir;
    /* 0x2f */ u8 localShot;
    /* 0x30 */ u8 hit;
    /* 0x31 */ u8 hidden;
    /* 0x32 */ u8 unk_32[2];
    /* 0x34 */ VecFx32 screenPos;
    /* 0x40 */ VecFx32 screenVel;
    /* 0x4c */ s32 scaleX;
    /* 0x50 */ s32 scaleY;
    /* 0x54 */ s16 angle;
    /* 0x56 */ s8 hFlip;
    /* 0x57 */ s8 vFlip;
    /* 0x58 */ s32 priority;
    /* 0x5c */ u8 cullRadius;
    /* 0x5d */ s8 palette;
    /* 0x5e */ u8 unk_5e[2];
    /* 0x60 */ u32 work0;
    /* 0x64 */ s32 work1;
    /* 0x68 */ u32 work2;
    /* 0x6c */ s32 work3;
    /* 0x70 */ s32 work4;

    SkySprite();
    ~SkySprite();
    SkySprite *construct();
    void clear();
    void resetFree();
    void initByKind(u32 a);
    void updateByKind();
    void endByKind();
    void release();
    void startAnim();
    void requestSe(s32 a);
    void requestSeSustained(s32 a);
    void requestSeSustainedOn(u32 a, u32 b);
    s32 checkShotHit(s32 a, s32 b, s32 c);
    void beginCrossing(s32 a, s32 b);
    s32 advanceCrossing(s32 a, s32 b, s32 c);
    // kind 0 rain drop
    void initRainDrop(u32 flag);
    void updateRainDrop();
    void endRainDrop();
    // kinds 1..5: snow flake, particle, shooting star, moon, balloon
    void initSnowFlake(u32 arg);
    void updateSnowFlake();
    void endSnowFlake();
    void initParticle(u32 arg);
    void updateParticle();
    void endParticle();
    void initShootingStar(u32 unused);
    void updateShootingStar();
    void endShootingStar();
    void initMoon(u32 unused);
    void updateMoon();
    void endMoon();
    void initBalloon(u32 arg);
    void updateBalloon();
    void endBalloon();
    // kind 6 UFO
    void initUfo(u32 unused);
    void updateUfo();
    void endUfo();
    void spawnUfoDebris();
    // kind 7 Pete
    void initPete(u32 unused);
    void updatePete();
    void endPete();
    // kind 8 lightning
    void initLightning(u32 unused);
    void updateLightning();
    void endLightning();
    // kind 9 firework
    void initFirework(u32 a);
    void initFireworkShell(u32 a);
    void initFireworkBurst(u32 a);
    void updateFirework();
    void updateFireworkShell();
    void updateFireworkBurst();
    void updateFireworkScale(s32 a);
    s32 riseDistance(s32 a);
    void updateFireworkPalette();
    void releaseFireworkPalette();
    void endFirework();
    // kind 10 rainbow
    void initRainbow(u32 unused);
    void updateRainbow();
    void endRainbow();
    // kind 11 slingshot shot
    void initShot(u32 a);
    void updateShot();
    void endShot();
    // kind 12 bird
    void initBird(u32 a);
    void updateBird();
    void endBird();
};
struct SkySePlayer {
    SkySndChannel channels[8];
    FxVec3 positions[8];
    u8 active;

    SkySePlayer();
    void update();
    void request(s32 idx, s32 a, VecFx32* p);
    void requestSustained(s32 idx, s32 a, VecFx32* p);
    void releaseAll();
    void resetAll();
};
struct SkyShotSequence {
    s32 state;
    s32 playerSlot;
    s32 timer;
    s32 targetX;
    s32 targetY;
    s32 targetScale;
    u8 fallDir;
    s16 headPitch;
    s16 headYaw;
    u8 balloonDropEnded;
    u8 landSePending;
    u8 splashSePending;
    u8 goldenBalloon;

    SkyShotSequence();
    void relaxHead(s32 a, s32 b);
    void lookAtTarget();
    void reset();
    void actPeteFallen();
    void actPeteFall();
    void actPeteHit();
    void actUfoCrashed();
    void actUfoFall();
    void actUfoHit();
    void actBalloonDrop();
    void actBalloonHit();
    void actShotFlight();
    void update();
    void setTarget(s32 a, s32 b, s32 c, u8 d);
    void onPeteFell();
    void onPeteHit();
    void onUfoFell();
    void onUfoHit();
    void onBalloonFell();
    void onBalloonHit(u8 a);
    void endWatch(s32 a);
    void startShot(s32 a);
};
struct SkyObjPalette {
    s32 balloonColor;
    s8 loadedSrcPalettes[4];
    u16 overrideMask;
    u16 overrideColors[15];
    u16 colors[16];

    SkyObjPalette();
    void setOverride(s32 idx, u16* p);
    BOOL hasOverride(s32 idx);
    void clearOverride(s32 idx);
    void setLoadedRow(s32 a, s32 b);
    BOOL isLoaded(s32 idx);
    void invalidateFor(s32 idx);
    void invalidate(s32 v);
    void pickBalloonColor();
    BOOL applyOverrides(s32 v);
};
// The sky sprite system (gSkySprites): 60 sprites, their 5 graphics slots, the graphics loader and OBJ palette, the
// spawner state (rain, snow, lightning, moon, rainbow, birds, shooting stars, fireworks, balloon / UFO / Pete
// visitors), the slingshot shot requests and shot sequence, and the positional SE player.
struct SkySprites {
    /* 0x0000 */ SkySprite sprites[60];
    /* 0x1b30 */ SkyObjGfxSlot gfxSlots[5];
    /* 0x1b6c */ SkyObjGfxLoader loader;
    /* 0x2eb8 */ SkyObjPalette palette;
    /* 0x2f00 */ s32 spawnInterval;
    /* 0x2f04 */ s32 spawnAccum;
    /* 0x2f08 */ s32 precipIntensity;
    /* 0x2f0c */ s32 snowCounter;
    /* 0x2f10 */ s32 thunderTimer;
    /* 0x2f14 */ s32 spriteCount;
    union {
        /* 0x2f18 */ u16 minuteHour;     // {minute, hour} of this frame (Clock_GetMinuteHour)
        MinuteHour time;
    };
    union {
        /* 0x2f1a */ u16 prevMinuteHour; // ... and of the previous frame
        MinuteHour prevTime;
    };
    /* 0x2f1c */ s32 second;
    /* 0x2f20 */ s32 prevSecond;
    /* 0x2f24 */ u8 balloonChance;
    /* 0x2f25 */ u8 ufoChance;
    /* 0x2f26 */ u8 ufoEventToday;
    /* 0x2f27 */ u8 peteEventToday;
    /* 0x2f28 */ u8 fireworksToday;
    /* 0x2f29 */ u8 birdsRequested;
    /* 0x2f2a */ u8 unk_2f2a[2];
    /* 0x2f2c */ s32 fireworksPattern;
    /* 0x2f30 */ s32 fireworksPatternTime;
    /* 0x2f34 */ s32 fireworksBigTimer;
    /* 0x2f38 */ s32 fireworksSlotTimers[4];
    /* 0x2f48 */ s32 fireworksInterval;
    /* 0x2f4c */ s32 shootingStarDelay;
    /* 0x2f50 */ u8 shootingStarVisible;
    /* 0x2f51 */ u8 rainbowStrength;
    /* 0x2f52 */ u8 unk_2f52[2];
    /* 0x2f54 */ s32 rainStrength;
    /* 0x2f58 */ SkyShotRequest shotRequests[4];
    /* 0x2fa8 */ SkyShotSequence shotSeq;
    /* 0x2fcc */ SkySePlayer se;

    SkySprites();
    ~SkySprites();
    void start();
    void stop();
    void initClock();
    void tickClock();
    void checkTodayEvents();
    void onDayChange(BOOL a);
    void sendWishLetters();
    void onSlingshotFired(s32 i);
    SkyShotRequest *getShotRequest(s32 i);
    SkySprite *spawn(s32 kind, s32 idx, VecFx32 *vec, s32 arg);
    s32 getGfxIdForKind(s32 kind, s32 arg);
    s32 findFreeIndex(s32 kind, s32 idx);
    s32 findKind(s32 id);
    void killKind(s32 id);
    void killAll();
    s32 pickGfxSlot(s32 t);
    void releaseConflictingSlots(s32 k);
    void reloadAllGfx();
    void loadDirtyGfx();
    void uploadDirtyGfx();
    void spawnInitialPrecip();
    void updateLightning();
    void updateThunderFlash();
    void updateMoon();
    void updateRainbow();
    // spawners (event checks run by SkySprites_UpdateEvents)
    void updateRain();
    void updateSnow();
    void updateShootingStar();
    void updateFireworksShow();
    void updateBalloon();
    void updateUfo();
    void updatePete();
    void updateBirds();
    void spawnShots();
    void applyRainbowBlend();
    // fireworks show: launch slots 0..3 and the pattern acts (sFireworksPattern* tables)
    BOOL canLaunchBig(s32 idx);
    BOOL canLaunchSmall(s32 idx);
    void launchFirework(s32 idx, s32 a, s32 b);
    void rollLaunchInterval(s32 *out);
    void tickBigLaunch();
    void tickLowBigLaunch();
    void tickSmallLaunches();
    void tickPairLaunches();
    void tickTripleLaunches();
    void setFireworksTiming(s32 a, s32 b, s32 c);
    void selectFireworksPattern(s32 mode);
    void fireworksPatternAct01();
    void fireworksPatternAct02();
    void fireworksPatternAct03();
    void fireworksPatternAct04();
    void fireworksPatternAct05();
    void fireworksPatternAct06();
    void fireworksPatternAct07();
    void fireworksPatternAct08();
    void fireworksPatternAct09();
    void fireworksPatternAct0A();
    void fireworksPatternAct0B();
    void fireworksPatternAct0C();
    void fireworksPatternAct0D();
    void fireworksPatternAct0E();
    void fireworksPatternAct0F();
    void fireworksPatternAct10();
    void fireworksPatternAct11();
    void fireworksPatternAct12();
    void fireworksPatternAct13();
};
#ifndef HOST64
static_assert(__builtin_offsetof(SkySprites, gfxSlots) == 0x1b30, "SkySprites.gfxSlots");
static_assert(__builtin_offsetof(SkySprites, loader) == 0x1b6c, "SkySprites.loader");
static_assert(__builtin_offsetof(SkySprites, palette) == 0x2eb8, "SkySprites.palette");
static_assert(__builtin_offsetof(SkySprites, shotSeq) == 0x2fa8, "SkySprites.shotSeq");
static_assert(__builtin_offsetof(SkySprites, se) == 0x2fcc, "SkySprites.se");
static_assert(__builtin_offsetof(WeatherManager, rainSlant) == 0x1538, "WeatherManager.rainSlant");
static_assert(__builtin_offsetof(WeatherManager, paletteFiles) == 0x1544, "WeatherManager.paletteFiles");
#endif
// Row of sSkyObjGfxTable (one per sky object graphic): char file (sSkyObjCharFiles), top/bottom tile numbers, char
// layout (sSkyObjCharLayouts), source palette row and OBJ palette; unk_0e/unk_0f are 0 in every row
struct SkyObjGfxDef { u32 charFile; u16 charTileA; u16 charTileB; u32 charLayout; s8 srcPalette; s8 objPalette; s8 unk_0e; s8 unk_0f; };
// Row of sSkyObjCharLayouts: where the top and bottom tile runs of a graphic go in its slot
struct SkyObjCharLayout { u32 topTileX; u32 topTileCount; u32 bottomTileX; u32 bottomTileCount; };
// gCameraLookAt as SkySprite_ProjectWorldPos reads it, and its local copy (vector object, trivial ctor/dtor)
struct SkyLookAtVec {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s32 z;
    SkyLookAtVec() {}
    ~SkyLookAtVec() {}
};
enum Unk_020be0bc_E { Unk_020be0bc_E_0 = 0 };
enum Unk_020be820_E { Unk_020be820_E_0 = 0 };
struct Unk_020bee28_Vec2 {
    s32 x, y;
    Unk_020bee28_Vec2(s32 a, s32 b) { x = a; y = b; }
};
// sFireworkColors row: a burst fades from the first colour to the second (SkySprite::updateFireworkPalette)
struct FireworkColorPair {
    /* 0x0 */ Rgb555 start;
    /* 0x2 */ Rgb555 end;
};
struct Unk_020bee28_V {
    s32 x, y, z;
    Unk_020bee28_V(const VecFx32Copy &o) { x = o.x; y = o.y; z = o.z; }
    ~Unk_020bee28_V() {}
};
struct SkyParticleStackPad {
    s32 v[3];
    SkyParticleStackPad() {}
    ~SkyParticleStackPad() {}
};
struct SkyShootingStarStackPad {
    s32 v[4];
    SkyShootingStarStackPad() {}
    ~SkyShootingStarStackPad() {}
};
// ---------------------------------------------------------------------------------------------------------------------

struct U234_Record {
    void *fn;
    u16 executePriority;
    u16 drawPriority;
};

// ======== what this unit uses from other units (declared as the definitions write them) ========
extern "C" {
// SDK / runtime (assembly)
void MI_CpuCopy8(const void *src, void *dst, u32 size);
void MI_CpuFill8(void *dst, u8 data, u32 size);
void DC_FlushRange(const void *start, u32 size);
s32 _s32_div_f(s32 a, s32 b);
u32 _u32_div_f(u32 a, u32 b);
extern "C" void Sky_HBlankMain(void);
extern "C" void Sky_HBlankSub(void);
fx32 func_01ffcb0c(fx32 v1, fx32 v2);
fx32 FX_Div(fx32 numer, fx32 denom);
fx32 FX_Inv(fx32 x);
fx32 FX_Sqrt(fx32 x);
void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab);
void MTX_MultVec43(const VecFx32 *pSrc, const Mtx43 *pMtx, VecFx32 *pDst);  // (MtxFx43 there: the same layout)
void GX_LoadOBJ(const void *src, u32 offset, u32 size);
void GXS_LoadOBJ(const void *src, u32 offset, u32 size);
void GX_LoadOBJPltt(const void *src, u32 offset, u32 size);
void GXS_LoadOBJPltt(const void *src, u32 offset, u32 size);
void GX_LoadBGPltt(const void *src, u32 offset, u32 size);
void GXS_LoadBGPltt(const void *src, u32 offset, u32 size);
void NNS_G3dGlbMaterialColorSpecEmi(u32 a, u32 b, BOOL c);
BOOL FS_OpenFile(FSFile *file, const char *path);
s32 FS_ReadFile(FSFile *file, void *dst, s32 len);
BOOL FS_SeekFile(FSFile *file, s32 pos, u32 origin);
BOOL FS_CloseFile(FSFile *file);
extern const s16 data_02135f44[4096 * 2];  // sine / cosine pairs
// math, random (autoload_2)
s16 Math_Atan2(s32 x, s32 y);
BOOL Math_StepAngle(s16 *p, s16 target, s16 step);
BOOL Math_StepS32Alt(s32 *p, s32 target, s32 step);
u32 Random_NextBelow(u32 *seed, u32 n);
void Random_SetSeed(u32 *seed, u32 v);
void Vec_Scale(VecFx32 *v, s32 s);
extern FxVec3 gVec3Zero;
extern s32 gFrameCounter;
extern Mtx43 data_021f47e0;
// camera
Mtx43 *Camera_GetViewMatrix(void);  // (MtxFx43 there: the same layout)
s32 Camera_GetCloseUpFactor(void);
s32 Camera_RestorePrevMode(void);
void Camera_StartShake(s32 a);
extern Camera *gCamera;
extern FxVec3 gCameraLookAt;
extern const s32 data_020c8cb8;
extern const s32 data_020c8cbc;
extern const u32 kFireworkBurstFlag29;
// clock
void Clock_GetDateTime(ClockDateTime *p);
void Clock_GetDate(u8 *out);
void Clock_GetMinuteHour(u8 *out);
u32 Clock_GetSecond();
s32 Date_DaysBetween(u8 *a, u8 *b);
void DateTime_AddDays(ClockDateTime *p, s32 n);
void DateTime_AddMinutes(ClockDateTime *p, s32 n);
s32 DateTime_Compare(ClockDateTime *a, ClockDateTime *b, u32 mask);
s32 DateTime_DiffDays(ClockDateTime *a, ClockDateTime *b);
s32 DateTime_GetWeatherPeriod(u8 *p);
void DateTime_SubHours(ClockDateTime *p, s32 n);
// events
s32 Event_GetState(s32 a, void *b, s32 c);
// (defined as (EventEntryView *, ClockDateTimeCopy, s32), types of src/main/unk_0203f104.cpp: ClockDateTimeCopy has a
// copy constructor, so the date is passed as a pointer to a copy)
s32 EventSchedule_CollectDay(void *out, void *date, s32 x);
void EventWeekSlots_MarkTodaySeen(u32 id);
// scene, lights, fog
u32 Scene_GetCurrent();
u8 Scene_GetSkyKind(BOOL a);
BOOL Scene_InTownUnk31();
u8 SceneLights_GetLightParam(u32 x);
s16 SceneLights_GetRoomColor(void);
void SceneLights_StartFlash(u32 v);
void Fog_SetAlpha(u32 v);
void Fog_SetOffset(u16 v);
extern u8 gFieldSceneKind;
// H-blank tasks
BOOL HBlank_Add(HBlankTask *t, void (*a)(), void (*b)(), void (*c)());
BOOL HBlank_Remove(HBlankTask *t);
BOOL HBlank_Replace(HBlankTask *t, void (*a)(), void (*b)());
// save data
extern SaveData gSaveData;
}
extern CommManager *const gCommManager;

// ======== the unit's own functions and objects (defined further down) ========
extern "C" {
SkySprites *SkySprites_Destruct(SkySprites *p);
SkyProc *SkyProc_Create();
void Sky_ProjectToScreenX(s32 *out, VecFx32 *in);
BOOL SkySprite_FollowShootingStar(SkySprite *this_);
BOOL SkySprite_CalcMoonPos(VecFx32 *out);
void SkySprite_CalcMoonColors(SkySprite *this_);
void SkySprite_SetMoonClarity(SkySprite *this_);
void SkySprite_FadeMoonClarity(SkySprite *this_);
void SkySprite_InitMoonPalette(SkySprite *this_);
void SkySprite_UpdateMoonPalette(SkySprite *this_);
void SkySprite_ResetMoonPalette(void);
void SkySprite_SpawnHitParticle(VecFx32 *pos, s32 scale, s32 speed);
s32 SkySprite_GetFireworkPalIndex(s32 a, s32 flag);
Unk_020bee28_Vec2 SkySprite_ProjectFirework(s32 a, s32 b);
void SkySprite_StartAnim(SkySprite* p);
void SkySprite_Release(SkySprite* p);
void SkySprite_BeginCrossing(SkySprite* p, u8 a, s32 b);
BOOL SkySprite_AdvanceCrossing(SkySprite* p, s32 a, s32 b, s32 c);
void SkySprite_ProjectWorldPos(SkySprite* p, VecFx32* q, s32 a);
void SkySprite_ProjectCrossing(SkySprite* p, s32 a);
BOOL SkySprite_CheckShotHit(SkySprite* p, s32 a, s32 b, s32 c);
s32 SkySprite_MakeSoundPos(VecFx32* out, s32 a, s32 b, s32 c);
void SkySprite_GetSoundPos(SkySprite* p, VecFx32* out);
void SkySprite_GetPos(SkySprite* p, VecFx32* out);
void SkySprite_RequestSeSustained(SkySprite* p, s32 a);
void SkySprite_RequestSe(SkySprite* p, s32 a);
void SkySprite_RequestSeSustainedOn(SkySprite* p, s32 a, s32 b);
void SkyObjGfxLoader_Init(SkyObjGfxLoader *p);
BOOL SkyObjGfxLoader_LoadChars(SkyObjGfxLoader *p, s32 idx);
BOOL SkyObjGfxLoader_LoadPalettes(SkyObjGfxLoader *p);
void SkyObjGfxLoader_FlushChars(SkyObjGfxLoader *p);
void SkyObjGfxSlot_Init(SkyObjGfxSlot* p);
void SkyObjGfxSlot_Upload(SkyObjGfxSlot* p, SkyObjGfxLoader *base);
void SkyObjGfxSlot_Acquire(SkyObjGfxSlot* p, s32 v, SkyObjPalette *x);
void SkyObjGfxSlot_SetGfx(SkyObjGfxSlot* p, s32 v);
void SkyObjGfxSlot_RefreshPalette(SkyObjGfxSlot* p, s32 a);
void SkyObjGfxSlot_Release(SkyObjGfxSlot* p);
SkyObjPalette*SkyObjPalette_Init(SkyObjPalette* p);
void SkyObjPalette_Load(SkyObjPalette* p, SkyObjGfxLoader *base, u32 idx);
void SkyShot_AddHit();
BOOL SkyShot_HasMaxHits();
s32 SkySprites_GetMoonPhaseGfx();
void SkySprites_ApplyRainbowBlend(SkySprites *obj);
void SkySprites_OnStarWish(SkySprites *obj);
void SkySprites_Start(SkySprites *obj);
void SkySprites_Stop(SkySprites *obj);
void SkySprites_UpdateEvents(SkySprites *obj);
void SkySprites_UpdateEventsIndoor(SkySprites *obj);
void SkySprites_UpdateStub(void *);
void SkySprites_Update(SkySprites *obj);
void SkySprites_UpdateIndoor(SkySprites *obj);
void SkySprites_Draw(SkySprites *obj);
void Sky_CalcLightColors(s32 a, s32 b, u32 c, u32 d);
void Sky_CalcFogOffset(s32 a, s32 b, u32 c, u32 d);
void Sky_CalcLightParams(s32 a, s32 b, u32 c, u32 d);
void Sky_UpdateLighting(s32 a, s32 b);
s16 Sky_GetLightColor(s32 i);
u8 Sky_GetLightParam(s32 i);
void RainSe_Init(RainSndChannel *p);
void RainSe_Update(RainSndChannel *p);
void RainSe_Release(RainSndChannel *p);
void RainSe_InitVolume(RainSndChannel *p);
void RainSe_FadeVolume(RainSndChannel *o);
BOOL Sky_LoadPaletteFiles();
BOOL Sky_AllocPaletteBufs(u16 **p);
void Color_Lerp(u16 *d, const u16 *s1, const u16 *s2, s32 t);
void Color_Lerp6(u16 *d, u16 *s1, u16 *s2, s32 t);
s32 Sky_ApplyPalettes(s32 x);
void Sky_BlendPalettes();
s32 Sky_LoadPalettes(s32 x);
u16 Sky_CalcStarScrollX(WeatherManager *self, ClockDateTime *date);
u32 Sky_CalcStarScrollY(WeatherManager *self, ClockDateTime *d);
u32 Sky_GetStarScrollYNow(WeatherManager *self);
void SkyGradient_Build();
void Sky_UpdateLineTables();
void Sky_VBlankSub();
void Sky_VBlankMain();
void Sky_VBlankDisabled(void);
void Sky_FrameOutdoor(void);
void Sky_FrameIndoor(void);
BOOL Sky_LoadCloudBg(u32 idx);
BOOL Sky_LoadNextCloudBg(u32 idx);
BOOL Sky_LoadStarBg(u32 idx);
void Sky_ResetObjGfx(void);
BOOL Sky_SetEngine(s32 arg);
void Sky_Disable(void);
s32 Weather_GetFallingPrecip(void);
s32 Weather_GetCurrent(void);
void Weather_GetLevels(s32 *a, s32 *b);
u32 Weather_GetPrecipKind(void);
u16 *Sky_GetCurrentPalette(void);
u32 Sky_GetStarScrollX(void);
u32 Sky_GetStarScrollY(void);
void Sky_GetStarViewingTime(ClockDateTime *out, s32 a, s32 b);
void Weather_RerollRainSlantAlt3(void);
void Weather_RerollRainSlantAlt2(void);
void Weather_RerollRainSlantAlt(void);
void Weather_RerollRainSlant(void);
void Sky_OnDayChange(u32 x);
void Sky_OnSlingshotFired(u32 x);
void Sky_SwapBuffers(void);
void Sky_EndBalloonDrop(void);
void Sky_PlayBalloonDropSe(BOOL x);
u32 Sky_IsShootingStarVisible(void);
void Sky_WishOnShootingStar(void);
void Sky_RequestBirds(void);
extern HBlankTask sSkyHBlankTask;
extern SkyGradient sSkyGradient;
extern SkyLight sSkyLight;
extern SkySpriteAnimDef sSkySpriteAnimSeqs[46];
extern SkySprites gSkySprites;
extern SpriteAnimFrame data_020e4a64[1];
extern SpriteAnimFrame data_020e4a70[1];
extern SpriteAnimFrame data_020e4a7c[1];
extern SpriteAnimFrame data_020e4a88[1];
extern SpriteAnimFrame data_020e4a94[1];
extern SpriteAnimFrame data_020e4aa0[1];
extern SpriteAnimFrame data_020e4aac[1];
extern SpriteAnimFrame data_020e4ab8[1];
extern SpriteAnimFrame data_020e4ac4[1];
extern SpriteAnimFrame data_020e4adc[1];
extern SpriteAnimFrame data_020e4ae8[1];
extern SpriteAnimFrame data_020e4af4[1];
extern SpriteAnimFrame data_020e4b00[1];
extern SpriteAnimFrame data_020e4b0c[1];
extern SpriteAnimFrame data_020e4b18[1];
extern SpriteAnimFrame data_020e4b24[1];
extern SpriteAnimFrame data_020e4b30[1];
extern SpriteAnimFrame data_020e4b3c[1];
extern SpriteAnimFrame data_020e4b48[1];
extern SpriteAnimFrame data_020e4ddc[2];
extern SpriteAnimFrame data_020e50e8[3];
extern SpriteAnimFrame data_020e510c[3];
extern SpriteAnimFrame data_020e5130[3];
extern SpriteAnimFrame data_020e5154[3];
extern SpriteAnimFrame data_020e5178[3];
extern SpriteAnimFrame data_020e519c[3];
extern SpriteAnimFrame data_020e51c0[3];
extern SpriteAnimFrame data_020e51e4[3];
extern SpriteAnimFrame data_020e5258[4];
extern SpriteAnimFrame data_020e5288[4];
extern SpriteAnimFrame data_020e5318[4];
extern SpriteAnimFrame data_020e5348[4];
extern SpriteAnimFrame data_020e53b0[6];
extern SpriteAnimFrame data_020e56b0[7];
extern SpriteAnimFrame data_020e5704[8];
extern SpriteAnimFrame data_020e5764[9];
extern SpriteAnimFrame data_020e57d0[9];
extern SpriteAnimFrame data_020e583c[9];
extern SpriteAnimFrame data_020e58a8[9];
extern SpriteAnimFrame data_020e5914[9];
extern SpriteAnimFrame data_020e5980[15];
extern SpriteAnimFrame data_020e5af4[44];
extern SpriteAnimFrame data_020e5d04[44];
extern SpriteAnimFrame data_020e5f14[44];
extern SpriteAnimFrame data_020e6124[44];
extern SpriteAnimFrame data_020e6334[44];
extern StarTwinkle data_021efc18;
extern U234_Record sSkyProcProfile;
extern WeatherManager gWeatherManager;
extern char **sCloudScreenFiles[3];
extern char *data_020e4d70[5];
extern char *data_020e4d98[5];
extern char *data_020e5230[10];
extern char *sCloudCharFiles[5];
extern char *sSkyPaletteFiles[2][2];
extern char data_020e4dac[23];
extern char data_020e4df4[27];
extern char data_020e4e10[27];
extern char data_020e4e48[30];
extern char data_020e4e68[30];
extern char data_020e4e88[30];
extern char data_020e4ea8[30];
extern char data_020e4ec8[30];
extern char data_020e4ee8[30];
extern char data_020e4f08[30];
extern char data_020e4f28[30];
extern char data_020e4f48[30];
extern char data_020e4f68[30];
extern char data_020e4f88[30];
extern char data_020e4fa8[30];
extern char data_020e4fc8[30];
extern char data_020e4fe8[31];
extern char data_020e5008[31];
extern char data_020e5028[31];
extern char data_020e5048[31];
extern char data_020e5068[31];
extern char data_020e5088[31];
extern char data_020e50a8[31];
extern char data_020e50c8[31];
extern const FireworkColorPair sFireworkColors[4];
extern const SkyObjCharLayout sSkyObjCharLayouts[5];
extern const SkyObjGfxDef sSkyObjGfxTable[52];
extern const SkySpawnRate sRainSpawnRates[4];
extern const SkySpawnRate sSnowSpawnRates[4];
extern const char *const sSkyObjCharFiles[3];
extern const s32 sFireworkFlicker[2];
extern const s32 sFireworkScaleBigLong[44];
extern const s32 sFireworkScaleBig[9];
extern const s32 sFireworkScaleSmallLong[44];
extern const s32 sFireworkScaleSmall[9];
extern const s32 sParticleAnimIds[6];
extern const s32 sParticleTrailScales[23];
extern const s32 sRainParallax[3];
extern const s32 sRainSeIds[4];
extern const s32 sSkyPaletteSetByLevel[5];
extern const s32 sSkySpriteDefaultGfx[13];
extern const s32 sSnowAnimIds[3];
extern const s32 sSnowFallSpeeds[3];
extern const s32 sSnowParallax[3];
extern const s8 sBirdDelays[4];
extern const s8 sWeatherHourTable[768];
extern const u16 *const *const sSkyLightColorTables[5];
extern const u16 *const data_020e4b74[4];
extern const u16 *const data_020e4b84[4];
extern const u16 *const sSkyFogOffsetTables[5];
extern const u16 data_020d0fc8[24];
extern const u16 data_020d0ff8[24];
extern const u16 data_020d1028[24];
extern const u16 data_020d1058[24];
extern const u16 data_020d1088[24];
extern const u16 data_020d10b8[24];
extern const u16 data_020d10e8[24];
extern const u16 data_020d1118[24];
extern const u16 data_020d1148[24];
extern const u16 data_020d1178[24];
extern const u16 sSkyObjPalDefaultColor;
extern const u32 sFireworkShellTypes[4];
extern const u8 *const *const sSkyLightParamTables[5];
extern const u8 *const data_020e478c[2];
extern const u8 *const data_020e4824[2];
extern const u8 data_020d0e04[5];
extern const u8 data_020d0ec8[24];
extern const u8 data_020d0ee0[24];
extern const u8 data_020d0ef8[24];
extern const u8 data_020d0f10[24];
extern const u8 sFireworksPatternDelays[20];
extern const u8 sFireworksPatternWeights[14];
extern const u8 sSkyCloudBgLayer[4];
extern const u8 sSkyNextCloudBgLayer[4];
extern const u8 sSkyObjPalOverrideSlots[15];
extern const u8 sSkyPaletteLayer[4];
extern const u8 sSkyStarBgLayer[4];
extern s32 sRainSideToggle;
extern s32 sSkyCrossingWidth;
extern s32 sSkyLineScrollX[2];
extern s32 sSkyLineScrollY[2];
extern s32 sSnowSideToggle;
extern u16 sMoonColors[6];
extern u16 sSkyBlendLineCache[2];
extern GXOamAttr data_020e4644[1];
extern GXOamAttr data_020e465c[1];
extern GXOamAttr data_020e466c[1];
extern GXOamAttr data_020e4674[1];
extern GXOamAttr data_020e4684[1];
extern GXOamAttr data_020e469c[1];
extern GXOamAttr data_020e46dc[1];
extern GXOamAttr data_020e46ec[1];
extern GXOamAttr data_020e46fc[1];
extern GXOamAttr data_020e470c[1];
extern GXOamAttr data_020e4734[1];
extern GXOamAttr data_020e475c[1];
extern GXOamAttr data_020e4764[1];
extern GXOamAttr data_020e476c[1];
extern GXOamAttr data_020e4774[1];
extern GXOamAttr data_020e477c[1];
extern GXOamAttr data_020e47b4[1];
extern GXOamAttr data_020e47c4[1];
extern GXOamAttr data_020e47ec[1];
extern GXOamAttr data_020e47f4[1];
extern GXOamAttr data_020e47fc[1];
extern GXOamAttr data_020e4814[1];
extern GXOamAttr data_020e482c[1];
extern GXOamAttr data_020e483c[1];
extern GXOamAttr data_020e4844[1];
extern GXOamAttr data_020e484c[1];
extern GXOamAttr data_020e4854[1];
extern GXOamAttr data_020e485c[1];
extern GXOamAttr data_020e4864[1];
extern GXOamAttr data_020e486c[1];
extern GXOamAttr data_020e4874[1];
extern GXOamAttr data_020e487c[1];
extern GXOamAttr data_020e4884[1];
extern GXOamAttr data_020e488c[1];
extern GXOamAttr data_020e489c[1];
extern GXOamAttr data_020e48bc[1];
extern GXOamAttr data_020e48c4[1];
extern GXOamAttr data_020e48cc[1];
extern GXOamAttr data_020e48d4[1];
extern GXOamAttr data_020e48dc[1];
extern GXOamAttr data_020e48ec[1];
extern GXOamAttr data_020e48f4[1];
extern GXOamAttr data_020e491c[1];
extern GXOamAttr data_020e492c[1];
extern GXOamAttr data_020e495c[1];
extern GXOamAttr data_020e497c[1];
extern GXOamAttr data_020e4984[1];
extern GXOamAttr data_020e498c[1];
extern GXOamAttr data_020e499c[1];
extern GXOamAttr data_020e49a4[1];
extern GXOamAttr data_020e49cc[1];
extern GXOamAttr data_020e49d4[1];
extern GXOamAttr data_020e49ec[1];
extern GXOamAttr data_020e49f4[1];
extern GXOamAttr data_020e49fc[1];
extern GXOamAttr data_020e4a0c[1];
extern GXOamAttr data_020e4a14[1];
extern GXOamAttr data_020e4a1c[1];
extern GXOamAttr data_020e4a24[1];
extern GXOamAttr data_020e4a3c[1];
extern GXOamAttr data_020e4a54[1];
extern GXOamAttr data_020e4a5c[1];
extern GXOamAttr data_020e4b54[2];
extern GXOamAttr data_020e4b64[2];
extern GXOamAttr data_020e4b94[2];
extern GXOamAttr data_020e4ba4[2];
extern GXOamAttr data_020e4bb4[2];
extern GXOamAttr data_020e4bc4[2];
extern GXOamAttr data_020e4bd4[2];
extern GXOamAttr data_020e4be4[2];
extern GXOamAttr data_020e4bf4[2];
extern GXOamAttr data_020e4c04[2];
extern GXOamAttr data_020e4c14[2];
extern GXOamAttr data_020e4c24[2];
extern GXOamAttr data_020e4c34[2];
extern GXOamAttr data_020e4c44[2];
extern GXOamAttr data_020e4c54[2];
extern GXOamAttr data_020e4c64[2];
extern GXOamAttr data_020e4c74[2];
extern GXOamAttr data_020e4c84[2];
extern GXOamAttr data_020e4c94[2];
extern GXOamAttr data_020e4cb4[2];
extern GXOamAttr data_020e4cc4[2];
extern GXOamAttr data_020e4cd4[2];
extern GXOamAttr data_020e4ce4[2];
extern GXOamAttr data_020e4cf4[2];
extern GXOamAttr data_020e4d04[2];
extern GXOamAttr data_020e4d14[2];
extern GXOamAttr data_020e4d24[2];
extern GXOamAttr data_020e5208[5];
extern GXOamAttr data_020e52b8[6];
extern GXOamAttr data_020e52e8[6];
extern GXOamAttr data_020e5378[7];
extern GXOamAttr data_020e53f8[9];
extern GXOamAttr data_020e5440[9];
extern GXOamAttr data_020e5488[9];
extern GXOamAttr data_020e54d0[10];
extern GXOamAttr data_020e5520[10];
extern GXOamAttr data_020e5570[10];
extern GXOamAttr data_020e55c0[10];
extern GXOamAttr data_020e5610[10];
extern GXOamAttr data_020e5a34[24];
extern u32 sSkyBlackBackdrop[1];
extern u32 sSkyOutdoors;
extern u8 data_020e4630[4];
extern u8 data_020e4634[4];
extern u8 sSkyLineTablesReady;
}

// ======== unk_020bfe30.cpp ========

// ~SkySprites (_ZN10SkySpritesD1Ev is this function on the DS; ~SkySprites does the same): the arrays of objects with destructors,
// each from its last element, as __cxa_vec_cleanup destroyed them
// Members that symbols.txt names as labels of other functions (on the DS the same code): defined here by those
// functions.
SkySprite::SkySprite() {  // construct()'s code: anim is constructed as a member
    clear();
    resetFree();
}
void SkySprite::release() { SkySprite_Release(this); }
void SkySprite::startAnim() { SkySprite_StartAnim(this); }
void SkySprite::requestSe(s32 a) { SkySprite_RequestSe(this, a); }
void SkySprite::requestSeSustained(s32 a) { SkySprite_RequestSeSustained(this, a); }
void SkySprite::requestSeSustainedOn(u32 a, u32 b) { SkySprite_RequestSeSustainedOn(this, a, b); }
s32 SkySprite::checkShotHit(s32 a, s32 b, s32 c) { return SkySprite_CheckShotHit(this, a, b, c); }
void SkySprite::beginCrossing(s32 a, s32 b) { SkySprite_BeginCrossing(this, a, b); }
s32 SkySprite::advanceCrossing(s32 a, s32 b, s32 c) { return SkySprite_AdvanceCrossing(this, a, b, c); }
SkyObjGfxSlot::SkyObjGfxSlot() { SkyObjGfxSlot_Init(this); }
void SkyObjGfxLoader::init() { SkyObjGfxLoader_Init(this); }
SkyObjPalette::SkyObjPalette() { SkyObjPalette_Init(this); }
SkySprites::~SkySprites() {}  // the members' destructors: SkySprites_Destruct's code
void SkySprites::applyRainbowBlend() { SkySprites_ApplyRainbowBlend(this); }
WeatherManager::~WeatherManager() { func_020ba49c(); }

extern "C" SkySprites *SkySprites_Destruct(SkySprites *p) {
    for (s32 i = 7; i >= 0; i--) {
        p->se.positions[i].~FxVec3();
    }
    for (s32 i = 3; i >= 0; i--) {
        p->shotRequests[i].~SkyShotRequest();
    }
    for (s32 i = 0x3b; i >= 0; i--) {
        p->sprites[i].~SkySprite();
    }
    return p;
}

// ---------------------------------------------------------------------------------------------------------------------
// Vtable classes of the library (autoload_2)

extern "C" SkyProc *SkyProc_Create() {
    return new SkyProc();
}

extern "C" void Sky_ProjectToScreenX(s32 *out, VecFx32 *in) {
    Mtx43 *m = Camera_GetViewMatrix();
    data_021f47e0 = *m;
    VecFx32 v;
    v.x = in->x;
    v.y = in->y;
    v.z = in->z;
    VecFx32 a;
    VecFx32 b;
    WorldCurve_Apply(&a, &v);
    MTX_MultVec43(&a, &data_021f47e0, &b);
    s32 c = gCamera->getFovTan();
    s32 q = -FX_Div(0x60000, c);
    Vec_Scale(&b, FX_Div(q, b.z));
    *out = b.x + 0x80000;
}

void SkySprite::initRainDrop(u32 flag) {
    s32 idx;
    s32 x = (sRainSideToggle * Random_GlobalBelow(0x8a) + 0x80) << 12;
    sRainSideToggle *= -1;
    s32 y = -0x14000 - (Random_GlobalBelow(0x10) << 12);
    VecFx32 *p34 = &screenPos;
    p34->x = x;
    p34->y = y;
    p34->z = 0;
    s32 mul = gSkySprites.rainStrength;
    s32 rr = Random_GlobalBelow(0xaac) - 0x556;
    s32 sh = ((mul * (rr + gWeatherManager.rainSlant)) << 4) >> 16;
    idx = ((u16)sh >> 4) * 2;
    VecFx32 *p40 = &screenVel;
    const s16 *tab = data_02135f44;
    p40->x = tab[idx] * -0x24;
    p40->y = tab[idx + 1] * 0x24;
    p40->z = 0;
    scaleY = 0x800;
    angle = (u16)sh;
    s32 r = Random_GlobalBelow(3);
    animSeq = r + 2;
    s32 v;
    switch (r) {
    case 0:
        v = Random_GlobalBelow(0x40) + 0x80;
        break;
    case 1:
        v = Random_GlobalBelow(0x40) + 0x40;
        break;
    default:
        v = Random_GlobalBelow(0x40);
        break;
    }
    work0 = v;
    if (flag == 1) {
        screenPos.y += Random_GlobalBelow(v + 0x101) << 12;
    }
    priority = 2;
}

void SkySprite::updateRainDrop() {
    VecFx32 *p = &screenPos;
    VEC_Add(p, &screenVel, p);
    Actor *e = (Actor *)PlayerActor_GetActor(4);
    if (e) {
        s32 d = -(e->position.x - e->prevPosition.x);
        s32 m = sRainParallax[work2];
        d = (d * m) >> 12;
        p->x += d;
    }
    if (p->y > ((s32)work0 << 12) + 0x100000) {
        state = 3;
    } else if (p->x < -0x14000) {
        p->x += 0x11e000;
    } else if (p->x > 0x114000) {
        p->x -= 0x11e000;
    }
}

void SkySprite::endRainDrop() {
    SkySprite_Release(this);
}


// ======== unk_020bf4ac.cpp ========
// Locals of SkySprite_CalcMoonColors: current time, the clear/cloudy colours of this hour and the next, the result
struct MoonColorBlend {
    /* 0x0 */ u8 minute;
    /* 0x1 */ u8 hour;
    /* 0x2 */ Rgb555 clearColor;
    /* 0x4 */ Rgb555 clearColorNext;
    /* 0x6 */ Rgb555 cloudyColor;
    /* 0x8 */ Rgb555 cloudyColorNext;
    /* 0xa */ u16 blended;
};


void SkySprite::initSnowFlake(u32 arg) {
    s32 a = Random_GlobalBelow(3);
    s32 b = Random_GlobalBelow(0x80);
    s32 g = sSnowSideToggle;
    s32 x = (g * b + 0x80) << 12;
    VecFx32 *pos, *vel;
    sSnowSideToggle = g * 0xffffffff;
    pos = &screenPos;
    screenPos.x = x;
    pos->y = -0x14000;
    pos->z = 0;
    vel = &screenVel;
    screenVel.x = 0;
    vel->y = sSnowFallSpeeds[a];
    vel->z = 0;
    work0 = Random_GlobalBelow(0x300) + 0x180;
    work1 = Random_GlobalBelow(0x10000);
    work2 = a;
    work3 = x;
    work4 = Random_GlobalBelow(2) + 0x5000;
    animSeq = sSnowAnimIds[a];
    priority = 2;
    if (arg == 1) {
        screenPos.y = screenPos.y + (Random_GlobalBelow(0x1c1) << 12);
    }
}

void SkySprite::updateSnowFlake() {
    VecFx32 *pos = &screenPos;
    Actor *o;
    s32 base, ang;
    VEC_Add(pos, &screenVel, pos);
    o = (Actor *)PlayerActor_GetActor(4);
    if (o != NULL) {
        s32 dv = -(o->position.x - o->prevPosition.x);
        dv = (dv * sSnowParallax[work2]) >> 12;
        dv += work3;
        work3 = dv;
    }
    base = work3;
    ang = (s16)((s16)work1 + work0);
    work1 = ang;
    { s32 tv = data_02135f44[((u16)ang >> 4) * 2]; s32 sc = work4; pos->x = base + ((tv * sc) >> 12); }
    if (pos->y > 0x1d4000) {
        state = 3;
    } else if (pos->x < -0x14000) {
        pos->x = pos->x + 0x11e000;
    } else if (pos->x > 0x114000) {
        pos->x = pos->x - 0x11e000;
    }
}

void SkySprite::endSnowFlake() { SkySprite_Release(this); }

extern "C" BOOL SkySprite_FollowShootingStar(SkySprite *this_) {
    SkyShootingStarStackPad pad;
    SkySprite *p = &gSkySprites.sprites[gSkySprites.findKind(3)];
    BOOL result = FALSE;
    if (p != NULL && p->state == 2) {
        VecFx32 *pos = &p->screenPos;
        s32 k = ((s32)((u32)(p->angle << 16) >> 16) >> 4) * 2;
        s32 d = FX_Div(0x10000, p->scaleY);
        s32 y = pos->y + func_01ffcb0c(data_02135f44[k + 1], d);
        s32 x = pos->x - func_01ffcb0c(data_02135f44[k], d);
        VecFx32 *out = &this_->screenPos;
        out->x = x;
        out->y = y;
        out->z = 0;
        result = TRUE;
    }
    return result;
}

void SkySprite::initParticle(u32 arg) {
    s32 m = arg & 0xf;
    animSeq = sParticleAnimIds[m];
    if (m == 1) {
        priority = 3;
    } else {
        priority = 2;
    }
    work0 = arg;
    work1 = 0;
    if (m == 1) {
        s32 v = sParticleTrailScales[0];
        scaleX = v;
        scaleY = v;
        if (!SkySprite_FollowShootingStar(this)) {
            state = 3;
        }
    }
}

void SkySprite::updateParticle() {
    s32 m = work0 & 0xf;
    work1 = work1 + 1;
    if (m == 0) {
        Vec_Scale(&screenVel, 0xe66);
        VEC_Add(&screenPos, &screenVel, &screenPos);
        if (anim.isFinished()) {
            state = 3;
        }
    } else if (m == 1) {
        s32 n = work1;
        s32 v = sParticleTrailScales[n];
        if (n >= 0x17) {
            state = 3;
        } else {
            scaleX = v;
            scaleY = v;
            *(u16 *)&angle += 0xe39;
            if (!SkySprite_FollowShootingStar(this)) {
                state = 3;
            }
        }
    } else if (work1 >= 6) {
        state = 3;
    }
}

void SkySprite::endParticle() { SkySprite_Release(this); }

void SkySprite::initShootingStar(u32 unused) {
    s32 ang, x, y, sn, cs, k, nx, ny;
    scaleX = 0x1000;
    scaleY = 0x800;
    if (Random_GlobalBelow(2) == 0) {
        x = (Random_GlobalBelow(0x3c) + 0xc4) << 12;
        y = 0;
    } else {
        x = 0x100000;
        y = Random_GlobalBelow(0x30) << 12;
    }
    ang = Math_Atan2((Random_GlobalBelow(0x17) + 0x80 << 12) - y, -x);
    k = ((u16)ang >> 4) * 2;
    sn = data_02135f44[k];
    cs = data_02135f44[k + 1];
    x -= cs * 0x23;
    y -= sn * 0x23;
    screenPos.x = x;
    screenPos.y = y;
    screenPos.z = 0;
    screenVel.x = func_01ffcb0c(cs, 0x7800);
    screenVel.y = func_01ffcb0c(sn, 0x7800);
    screenVel.z = 0;
    angle = ang - 0x4000;
    animSeq = 0x11;
    priority = 3;
    gSkySprites.shootingStarVisible = 0;
    SkySprite_RequestSeSustainedOn(this, 6, 0x801);
}

void SkySprite::updateShootingStar() {
    VEC_Add(&screenPos, &screenVel, &screenPos);
    s32 x = screenPos.x >> 12;
    BOOL out;
    s32 y = screenPos.y >> 12;
    if (y < -0x23 || y > 0xca || x < -0x23 || x > 0x123) {
        out = TRUE;
    } else {
        out = FALSE;
    }
    s32 in;
    if (work0 == 0 && !out && y > 0 && y < 0xc0 && x > 0 && x < 0x100) {
        in = 1;
    } else {
        in = 0;
    }
    gSkySprites.shootingStarVisible = in;
    if (out) {
        state = 3;
    }
}

void SkySprite::endShootingStar() {
    gSkySprites.shootingStarVisible = 0;
    SkySprite_Release(this);
}

extern "C" BOOL SkySprite_CalcMoonPos(VecFx32 *out) {
    BOOL ok = TRUE;
    MinuteHour d;
    u32 b, a, base, off, x;
    s32 r, y;
    Clock_GetMinuteHour(&d.minute);
    b = d.hour;
    a = d.minute;
    base = Clock_GetSecond();
    off = 0;
    if (b >= 0x13) {
        off = base + (a * 0x3c + (b - 0x13) * 0xe10);
    } else if (b < 4) {
        off = base + (b * 0xe10 + 0x4650 + a * 0x3c);
    } else {
        ok = FALSE;
    }
    if (ok) {
        x = _u32_div_f(off << 12, 0x7e90);
        r = func_01ffcb0c(0xffee0000, x) + 0x110000;
        y = func_01ffcb0c(x - 0x800, x - 0x800);
        y = FX_Sqrt(0x1000 - y);
        y = FX_Div(y - 0xddb, 0x225);
        out->x = r;
        y <<= 4;
        y = -y;
        out->y = y + 0x3c000;
        out->z = 0;
    }
    return ok;
}

extern "C" void SkySprite_CalcMoonColors(SkySprite *this_) {
    MoonColorBlend rec;
    SkyMoonPaletteRow *t0;
    SkyMoonPaletteRow *t1;
    s32 inv, v0, v1, v2, v3, v4, v5, w0, w1, t;
    u32 i;
    s32 r7, r5;
    Clock_GetMinuteHour(&rec.minute);
    u32 idx = rec.hour;
    u32 mon = rec.minute;
    if (idx >= 0x13) idx -= 0x13; else idx += 5;
    t0 = &gSkySprites.loader.moonPalette[idx];
    t1 = &gSkySprites.loader.moonPalette[idx + 1];
    t = this_->work3;
    inv = 0x1000 - t;
    r7 = _s32_div_f(mon << 12, 60);
    r5 = 0x1000 - r7;
    for (i = 0; i < 5; i++) {
        s32 w2;
        *(u16 *)&rec.clearColor = t0->clearColors[i];
        *(u16 *)&rec.clearColorNext = t1->clearColors[i];
        *(u16 *)&rec.cloudyColor = t0->cloudyColors[i];
        *(u16 *)&rec.cloudyColorNext = t1->cloudyColors[i];
        v0 = func_01ffcb0c(rec.clearColor.r << 12, r5) + func_01ffcb0c(rec.clearColorNext.r << 12, r7);
        v1 = func_01ffcb0c(rec.clearColor.g << 12, r5) + func_01ffcb0c(rec.clearColorNext.g << 12, r7);
        v2 = func_01ffcb0c(rec.clearColor.b << 12, r5) + func_01ffcb0c(rec.clearColorNext.b << 12, r7);
        v3 = func_01ffcb0c(rec.cloudyColor.r << 12, r5) + func_01ffcb0c(rec.cloudyColorNext.r << 12, r7);
        v4 = func_01ffcb0c(rec.cloudyColor.g << 12, r5) + func_01ffcb0c(rec.cloudyColorNext.g << 12, r7);
        v5 = func_01ffcb0c(rec.cloudyColor.b << 12, r5) + func_01ffcb0c(rec.cloudyColorNext.b << 12, r7);
        w0 = func_01ffcb0c(v0, t) + func_01ffcb0c(v3, inv);
        w1 = func_01ffcb0c(v1, t) + func_01ffcb0c(v4, inv);
        w2 = func_01ffcb0c(v2, t) + func_01ffcb0c(v5, inv);
        rec.blended = (((w2 + 0x800) >> 12) << 10) | (((w0 + 0x800) >> 12) | (((w1 + 0x800) >> 12) << 5));
        sMoonColors[i] = rec.blended;
    }
}

extern "C" void SkySprite_SetMoonClarity(SkySprite *this_) {
    s32 s = gWeatherManager.level;
    s32 f;
    if (s != 3 && s != 4) {
        f = 0x1000;
    } else {
        f = 0;
    }
    this_->work3 = f;
}

extern "C" void SkySprite_FadeMoonClarity(SkySprite *this_) {
    s32 v;
    s32 s = gWeatherManager.targetLevel;
    s32 f;
    if (s != 3 && s != 4) {
        f = 0x1000;
    } else {
        f = 0;
    }
    v = this_->work3;
    Math_StepS32Alt(&v, f, 0x100);
    this_->work3 = v;
}

extern "C" void SkySprite_InitMoonPalette(SkySprite *this_) {
    SkyObjPalette *p = &gSkySprites.palette;
    SkySprite_SetMoonClarity(this_);
    SkySprite_CalcMoonColors(this_);
    for (u32 i = 0; i < 5; i++) {
        p->setOverride(i + 10, &sMoonColors[i]);
    }
}

extern "C" void SkySprite_UpdateMoonPalette(SkySprite *this_) {
    SkyObjPalette *p = &gSkySprites.palette;
    SkySprite_FadeMoonClarity(this_);
    SkySprite_CalcMoonColors(this_);
    for (u32 i = 0; i < 5; i++) {
        p->setOverride(i + 10, &sMoonColors[i]);
    }
}

extern "C" void SkySprite_ResetMoonPalette(void) {
    SkyObjPalette *p = &gSkySprites.palette;
    for (u32 i = 0; i < 5; i++) {
        p->clearOverride(i + 10);
    }
}

void SkySprite::initMoon(u32 unused) {
    if (SkySprite_CalcMoonPos(&screenPos)) {
        animSeq = 0x12;
        priority = 3;
        SkySprite_InitMoonPalette(this);
    } else {
        state = 3;
    }
}

void SkySprite::updateMoon() {
    if ((gFrameCounter & 0x7f) == 0) {
        if (!SkySprite_CalcMoonPos(&screenPos)) {
            state = 3;
        }
        SkySprite_UpdateMoonPalette(this);
    }
}

void SkySprite::endMoon() {
    SkySprite_ResetMoonPalette();
    SkySprite_Release(this);
}

void SkySprite::initBalloon(u32 arg) {
    SkySprite_BeginCrossing(this, Random_GlobalBelow(2) == 0 ? 1 : 0, 0x3e8);
    animSeq = 1;
    priority = 2;
    work0 = 0;
    arg &= 1;
    work1 = arg != 0 ? 1 : 0;
    work2 = 0;
}

void SkySprite::updateBalloon() {
    s32 t;
    if (work0 == 0) {
        if (SkySprite_AdvanceCrossing(this, 0x1ec, 0x5000, 0x3e8)) {
            state = 3;
        } else if (SkySprite_CheckShotHit(this, 0xe000, 0x1a000, -0x3000)) {
            t = gfxId + 1;
            SkyObjGfxSlot_SetGfx(&gSkySprites.gfxSlots[gfxSlot], t);
            gfxId = t;
            gSkySprites.shotSeq.onBalloonHit(work1 != 0 ? 1 : 0);
            SkySprite_RequestSeSustained(this, 0x7f8);
            work0 = 1;
        } else {
            SkySprite_RequestSe(this, 0x7f7);
        }
    } else if (work0 == 1) {
        animSeq = 0;
        SkySprite_StartAnim(this);
        screenVel.x = 0;
        screenVel.y = -0x800;
        screenVel.z = 0;
        work0 = 2;
        work2 = 0;
    } else {
        screenVel.y += 0x666;
        screenVel.y = func_01ffcb0c(screenVel.y, 0xfd7);
        VEC_Add(&screenPos, &screenVel, &screenPos);
        if (screenPos.y > 0xd0000) {
            gSkySprites.shotSeq.onBalloonFell();
            state = 3;
        }
    }
    if ((s32)work0 >= 1) {
        gSkySprites.shotSeq.setTarget(screenPos.x, screenPos.y, scaleX, dir == 0 ? 1 : 0);
    }
}

void SkySprite::endBalloon() { SkySprite_Release(this); }


// ======== unk_020beb40.cpp ========


void SkySprite::spawnUfoDebris()
{
    if (work1 > 0) {
        if (work1 == 1) {
            s32 r4, scale, y, q, idx;
            work1 = Random_GlobalBelow(2) + 1;
            r4 = Random_GlobalBelow(2) + 2;
            scale = scaleX;
            q = FX_Div(Random_GlobalBelow(0x10) << 12, scale);
            idx = ((u16)(s16)Random_GlobalBelow(0x10000) >> 4) << 1;
            y = screenPos.y + func_01ffcb0c(q, data_02135f44[idx]);
            VecFx32 pos;
            pos.x = screenPos.x + func_01ffcb0c(q, data_02135f44[idx + 1]);
            pos.y = y;
            pos.z = 0;
            SkySprite *o = gSkySprites.spawn(2, 0x3c, &pos, r4 & 0xf);
            if (o) {
                o->scaleX = scale;
                o->scaleY = scale;
            }
        } else {
            work1 = work1 - 1;
        }
    }
}

void SkySprite::initUfo(u32 unused)
{
    beginCrossing(Random_GlobalBelow(2) == 0 ? 1 : 0, 0x1000);
    animSeq = 0x16;
    priority = 2;
    gSaveData.setFlag(9);
    work0 = 0;
    work1 = 0;
    work2 = 0;
}

void SkySprite::updateUfo()
{
    if (work0 == 0) {
        if (advanceCrossing(0x385, 0x1000, 0xfa0)) {
            state = 3;
        } else if (checkShotHit(0x1a000, 0xc000, 0x5000)) {
            SkyObjGfxSlot_SetGfx(&gSkySprites.gfxSlots[gfxSlot], 0x24);
            gfxId = 0x24;
            requestSeSustained(0x7fc);
            gSkySprites.shotSeq.onUfoHit();
            work0 = 1;
        } else {
            requestSe(0x7fb);
        }
    } else if (work0 == 1) {
        animSeq = 0x17;
        startAnim();
        screenVel.x = dir ? 0x2666 : -0x2666;
        screenVel.y = -0x399a;
        screenVel.z = 0;
        work0 = 2;
    } else if (work0 == 2) {
        if (anim.isFinished()) {
            SkyObjGfxSlot_SetGfx(&gSkySprites.gfxSlots[gfxSlot], 0x25);
            gfxId = 0x25;
            work0 = 3;
            work1 = 1;
        }
    } else if (work0 == 3) {
        animSeq = 0x18;
        startAnim();
        work0 = 4;
        work2 = 0;
    } else if (work0 == 4) {
        if (screenPos.y > 0xd0000) {
            EventWeekSlots_MarkTodaySeen(0x44);
            s32 *obj = (s32 *)PlayerActor_GetBodyPos(PlayerActor_GetLocalSessionSlot());
            Town_PlaceGulliverShip(obj, dir == 0 ? 1 : 0);
            gSkySprites.shotSeq.onUfoFell();
            state = 3;
        }
    }
    if ((s32)work0 >= 2) {
        VecFx32Copy v = gVec3Zero;
        s32 sc;
        if ((s32)work0 >= 4) {
            v.x = dir ? 0x19a : -0x19a;
            v.y = 0x19a;
            sc = 0xfd7;
        } else {
            v.y = 0x800;
            sc = 0x1000;
        }
        VEC_Add(&screenVel, &v, &screenVel);
        Vec_Scale(&screenVel, sc);
        VEC_Add(&screenPos, &screenVel, &screenPos);
    }
    if ((s32)work0 >= 1) {
        gSkySprites.shotSeq.setTarget(screenPos.x, screenPos.y, scaleX, dir == 0 ? 1 : 0);
    }
    spawnUfoDebris();
}

void SkySprite::endUfo()
{
    release();
}

extern "C" void SkySprite_SpawnHitParticle(VecFx32 *pos, s32 scale, s32 speed)
{
    SkyParticleStackPad tmp;
    SkySprite *o = gSkySprites.spawn(2, 0x3c, pos, 0);
    if (o) {
        o->scaleX = scale;
        o->scaleY = scale;
        o->screenVel.x = func_01ffcb0c(speed, 0x800);
        o->screenVel.y = 0;
        o->screenVel.z = 0;
    }
}

void SkySprite::initPete(u32 unused)
{
    beginCrossing(gfxId == 0x28 ? 1 : 0, 0x8000);
    animSeq = 0x13;
    priority = 2;
    work0 = 0;
    work1 = 0;
}

void SkySprite::updatePete()
{
    if (work0 == 0) {
        if (advanceCrossing(0x30a, 0x8000, -100)) {
            state = 3;
        } else if (checkShotHit(0x1b000, 0x15000, 0)) {
            s32 t = gfxId + 1;
            SkyObjGfxSlot_SetGfx(&gSkySprites.gfxSlots[gfxSlot], t);
            gfxId = t;
            gSkySprites.shotSeq.onPeteHit();
            requestSeSustained(0x7ff);
            work0 = 1;
            work1 = 0;
        } else if (anim.getFrameIndex() == 0) {
            if (work1++ == 0) requestSeSustained(0x7fe);
        } else {
            work1 = 0;
        }
    } else if (work0 == 1) {
        animSeq = 0x14;
        startAnim();
        anim.pause();
        screenVel.x = dir ? 0x3c00 : -0x3c00;
        screenVel.y = -0x4000;
        screenVel.z = 0;
        SkySprite_SpawnHitParticle(&screenPos, scaleX, screenVel.x);
        scaleX = func_01ffcb0c(scaleX, 0xe8c);
        scaleY = func_01ffcb0c(scaleY, 0xe8c);
        work0 = 2;
        work1 = 0;
    } else if (work0 == 2) {
        work1 = work1 + 1;
        if (work1 >= 1) {
            scaleX = func_01ffcb0c(scaleX, 0x119a);
            scaleY = func_01ffcb0c(scaleY, 0x119a);
            work0 = 6;
            work1 = 0;
            anim.restart();
        }
    }
    if ((s32)work0 >= 6) {
        screenVel.y += 0x59a;
        Vec_Scale(&screenVel, 0x1000);
        VEC_Add(&screenPos, &screenVel, &screenPos);
    }
    if ((s32)work0 >= 6 && screenPos.y > 0xd0000) {
        gSkySprites.shotSeq.onPeteFell();
        EventWeekSlots_MarkTodaySeen(0x45);
        state = 3;
    }
    if ((s32)work0 >= 1) {
        gSkySprites.shotSeq.setTarget(screenPos.x, screenPos.y, scaleX, dir == 0 ? 1 : 0);
    }
}

void SkySprite::endPete()
{
    release();
}

void SkySprite::initLightning(u32 unused)
{
    s32 a = (Random_GlobalBelow(0x80) * (Random_GlobalBelow(2) * 2 - 1) + 0x80) << 12;
    s32 c = (Random_GlobalBelow(0x50) + 0x50) << 12;
    VecFx32 *p = &screenPos;
    screenPos.x = a;
    p->y = c;
    p->z = 0;
    animSeq = Random_GlobalBelow(8) + 8;
    priority = 2;
}

void SkySprite::updateLightning()
{
    if (anim.isFinished()) state = 3;
}

void SkySprite::endLightning()
{
    release();
}

extern "C" s32 SkySprite_GetFireworkPalIndex(s32 a, s32 flag)
{
    s32 r = 0xf;
    if (flag != 0) {
        r = 0;
    } else if (a == 0) {
        r = 2;
    } else if (a == 1) {
        r = 4;
    } else if (a == 2) {
        r = 6;
    } else if (a == 3) {
        r = 8;
    }
    return r;
}

extern "C" Unk_020bee28_Vec2 SkySprite_ProjectFirework(s32 a, s32 b)
{
    Unk_020bee28_V v = gCameraLookAt;
    s32 base = data_020c8cb8;
    s32 d = v.z - base;
    s32 cnt = data_020c8cbc;
    s32 h = _s32_div_f(cnt << 2, 2);
    s32 p = FX_Div((v.x - cnt * 3) << 4, h);
    s32 q = FX_Div(d, base << 2);
    if (p < -0x10000) p = -0x10000;
    else if (p > 0x10000) p = 0x10000;
    if (q < 0) q = 0;
    else if (q > 0x1000) q = 0x1000;
    q = func_01ffcb0c(q - 0x1000, q - 0x1000);
    s32 e = 0x1000;
    e -= q;
    p = -p;
    s32 w = (e - 0x800) << 5;
    s32 c;
    if (b < 0) c = 0;
    else if (b > 0xbf000) c = 0xbf000;
    else c = b;
    s32 m = func_01ffcb0c(w, FX_Div(0xbf000 - c, 0xbf000));
    b = b + m;
    return Unk_020bee28_Vec2(a + p, b);
}

void SkySprite::updateFireworkPalette()
{
    s32 n = work1;
    s32 t, lo, hi, r, g, inv;
    if (n < 0x10) {
        t = 0;
    } else if (n < 0x1b) {
        t = _s32_div_f((n - 0x10) << 12, 11);
    } else {
        t = 0x1000;
    }
    u32 v = work0;
    s32 k = (v >> 8) & 3;
    BOOL f = ((v >> 31) & 1) != 0;
    lo = SkySprite_GetFireworkPalIndex(k, f);
    hi = lo + 1;
    s32 i1, i0;
    i0 = 0;
    i1 = i0;
    if (n >= 0xb) {
        s32 x = n & 2;
        if (x == 0) i0 = 1;
        if (x != 0) i1 = 1;
        else i1 = 0;
    }
    const s32 *p0 = sFireworkFlicker + i0;
    const s32 *p1 = sFireworkFlicker + i1;
    if (k >= 4 || lo >= 15 || hi >= 15) return;
    const FireworkColorPair *e = &sFireworkColors[k];
    r = 0; g = 0; inv = 0x1000 - t;
    r = func_01ffcb0c(sFireworkColors[k].start.r << 12, inv) + func_01ffcb0c(e->end.r << 12, t);
    g = func_01ffcb0c(sFireworkColors[k].start.g << 12, inv) + func_01ffcb0c(e->end.g << 12, t);
    s32 b = func_01ffcb0c(sFireworkColors[k].start.b << 12, inv) + func_01ffcb0c(e->end.b << 12, t);
    u32 B1, G1, R0, R1, B0, G0;
    s32 r0 = func_01ffcb0c(r, *p0);
    s32 g0 = func_01ffcb0c(g, *p0);
    s32 b0 = func_01ffcb0c(b, *p0);
    s32 r1 = func_01ffcb0c(r, *p1);
    s32 g1 = func_01ffcb0c(g, *p1);
    s32 b1 = func_01ffcb0c(b, *p1);
    G0 = (g0 + 0x800) >> 12;
    B0 = (b0 + 0x800) >> 12;
    R1 = (r1 + 0x800) >> 12;
    G1 = (g1 + 0x800) >> 12;
    B1 = (b1 + 0x800) >> 12;
    R0 = (r0 + 0x800) >> 12;
    if (R0 > 0x1f) R0 = 0x1f;
    if (G0 > 0x1f) G0 = 0x1f;
    if (B0 > 0x1f) B0 = 0x1f;
    if (R1 > 0x1f) R1 = 0x1f;
    if (G1 > 0x1f) G1 = 0x1f;
    if (B1 > 0x1f) B1 = 0x1f;
    u16 col[2];
    col[0] = R0 | (G0 << 5) | (B0 << 10);
    col[1] = R1 | (G1 << 5) | (B1 << 10);
    s32 idx = gfxSlot;
    s32 u8v = gfxId;
    SkyObjPalette *const pal = &gSkySprites.palette;
    pal->setOverride(lo, &col[0]);
    pal->setOverride(hi, &col[1]);
    SkyObjGfxSlot_RefreshPalette(&gSkySprites.gfxSlots[idx], u8v);
}

void SkySprite::releaseFireworkPalette()
{
    u32 v = work0;
    BOOL f = ((v >> 31) & 1) != 0;
    s32 lo = SkySprite_GetFireworkPalIndex((v >> 8) & 3, f);
    s32 hi = lo + 1;
    if (lo < 15) gSkySprites.palette.clearOverride(lo);
    if (hi < 15) gSkySprites.palette.clearOverride(hi);
}

s32 SkySprite::riseDistance(s32 a)
{
    return a << 14;
}

void SkySprite::updateFireworkScale(s32 a)
{
    s32 max;
    const s32 *tab;
    u32 v = work0;
    BOOL f = ((v >> 30) & 1) != 0;
    if ((v >> 31) & 1) {
        if (f) { tab = sFireworkScaleBigLong; max = 0xb0; }
        else { tab = sFireworkScaleBig; max = 0x24; }
    } else {
        if (f) { tab = sFireworkScaleSmallLong; max = 0xb0; }
        else { tab = sFireworkScaleSmall; max = 0x24; }
    }
    if (a < 0) a = anim.getFrameIndex();
    if (a < 0) max = 0;
    else if (a <= max) max = a;
    s32 x = tab[max];
    scaleX = x;
    scaleY = x;
}

void SkySprite::initFirework(u32 a)
{
    s32 id = (a & 0xf) + 0x1d;
    animSeq = id;
    priority = 2;
    work0 = a;
    work1 = 0;
    work2 = 0;
    BOOL r = FALSE;
    s32 t = id - 0x21;
    if ((u32)t <= 9 && ((1 << t) & 0x249)) r = TRUE;
    if (r) {
        initFireworkShell(a);
    } else {
        initFireworkBurst(a);
    }
}


// ======== unk_020be204.cpp ========
static inline void SkyVec_Set(VecFx32 *v, s32 x, s32 y, s32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}
static inline BOOL SkySprite_IsOffScreen(VecFx32 *v) {
    BOOL r = v->y < -0xa000 || v->y > 0xca000 || v->x < -0xa000 || v->x > 0x10a000;
    return r;
}
static inline BOOL Sky_TestBit(u32 v, u32 n) {
    BOOL r = ((v >> n) & 1) ? TRUE : FALSE;
    return r;
}
static inline u32 Firework_VariantOf(Unk_020be820_E v) { return (v - 0x1d) & 0xf; }

void SkySprite::initFireworkBurst(u32 a) {
    BOOL b = Sky_TestBit(a, 31);
    updateFireworkScale(0);
    work3 = screenPos.x;
    work4 = screenPos.y;
    {
        Unk_020bee28_Vec2 sp = SkySprite_ProjectFirework(work3, work4);
        screenPos.x = sp.x;
        screenPos.y = sp.y;
    }
    u32 r2 = b ? 0x804 : 0x803;
    u32 m = (a >> 8) & 3;
    requestSeSustainedOn(b ? 1 : m + 2, r2);
    if (b) {
        cullRadius = 4;
        SceneLights_StartFlash(m + 5);
    } else {
        SceneLights_StartFlash(m + 1);
    }
}

void SkySprite::initFireworkShell(u32 a) {
    BOOL b = Sky_TestBit(a, 31);
    s32 x, y;
    if (b) {
        x = Random_GlobalBelow(0x60) + 0x50;
        if ((a >> 28) & 1) {
            y = 0;
        } else {
            y = Random_GlobalBelow(0x20);
        }
        y += 0x52;
    } else {
        x = Random_GlobalBelow(0xa0) + 0x30;
        y = Random_GlobalBelow(0x60) + 0x32;
    }
    work3 = x << 12;
    work4 = 0xbf000;
    screenPos.z = y << 12;
    {
        Unk_020bee28_Vec2 sp = SkySprite_ProjectFirework(work3, work4);
        screenPos.x = sp.x;
        screenPos.y = sp.y;
    }
    screenVel.x = 0;
    screenVel.y = -0x3800;
    screenVel.z = 0;
    scaleY = 0xa000;
    u32 m = (a >> 8) & 3;
    if (b) {
        m = 1;
    } else {
        m += 2;
    }
    requestSeSustainedOn(m, 0x802);
}

void SkySprite::updateFirework() {
    work1++;
    u32 t = animSeq;
    BOOL m = FALSE;
    t -= 0x21;
    if (t > 9) {
    } else if ((1 << t) & 0x249) {
        m = TRUE;
    }
    if (m) {
        updateFireworkShell();
    } else {
        updateFireworkBurst();
    }
}

void SkySprite::updateFireworkBurst() {
    u32 flags = work0;
    BOOL r4 = ((flags >> 30) & 1) ? FALSE : TRUE;
    BOOL r6 = Sky_TestBit(flags, 31);
    if (anim.isFinished()) {
        if (r4) {
            releaseFireworkPalette();
        }
        state = 3;
    } else {
        updateFireworkScale(-1);
        {
            Unk_020bee28_Vec2 sp = SkySprite_ProjectFirework(work3, work4);
            screenPos.x = sp.x;
            screenPos.y = sp.y;
        }
        if (r4) {
            updateFireworkPalette();
        }
    }
    if (work1 == 0x1d) {
        work0 |= kFireworkBurstFlag29;
    }
}

void SkySprite::updateFireworkShell() {
    s32 lim;
    u32 flags = work0;
    BOOL b31 = Sky_TestBit(flags, 31);
    BOOL done = FALSE;
    if (hidden) {
        work2++;
        if ((s32)work2 > 6) {
            done = TRUE;
        }
    } else {
        lim = 0xc0000 - screenPos.z;
        s32 r = riseDistance(work1);
        if (r <= 0x20000) {
            scaleY = FX_Div(0x20000, r);
            work4 = 0xc0000 - _s32_div_f(r, 2);
        } else {
            if (r < lim) {
                scaleY = 0x1000;
            } else {
                scaleY = func_01ffcb0c(scaleY, 0x14cd);
                if (scaleY >= 0xa000) {
                    scaleY = 0xa000;
                    hidden = 1;
                }
                r = lim;
            }
            work4 = 0xc0000 - (r - FX_Div(0x10000, scaleY));
        }
        {
            Unk_020bee28_Vec2 sp = SkySprite_ProjectFirework(work3, work4);
            screenPos.x = sp.x;
            screenPos.y = sp.y;
        }
    }
    if (done) {
        u32 m, a;
        state = 3;
        m = (flags >> 8) & 3;
        if ((work0 >> 31) & 1) {
            m = (m & 3) << 8;
            a = m | 0x80000000;
            m |= 0xc0000001;
        } else {
            s32 c = animSeq;
            m = (m & 3) << 8;
            a = m | Firework_VariantOf((Unk_020be820_E)(c - 2));
            m |= Firework_VariantOf((Unk_020be820_E)(c - 1)) | 0x40000000;
        }
        VecFx32 v;
        SkyVec_Set(&v, work3, screenPos.z - 0x2000, 0);
        gSkySprites.spawn(9, 0x3c, &v, a);
        gSkySprites.spawn(9, 0x3c, &v, m);
    }
}

void SkySprite::endFirework() {
    u32 t = animSeq;
    BOOL m = FALSE;
    t -= 0x21;
    if (t > 9) {
    } else if ((1 << t) & 0x249) {
        m = TRUE;
    }
    if (!m && !((work0 >> 30) & 1)) {
        releaseFireworkPalette();
    }
    release();
}

void SkySprite::initRainbow(u32 unused) {
    screenPos.x = 0xb0000;
    screenPos.y = 0x67000;
    animSeq = 0x15;
    priority = 3;
}

void SkySprite::updateRainbow() {
}

void SkySprite::endRainbow() {
    release();
}

void SkySprite::initShot(u32 a) {
    SkyShotRequest *rec = gSkySprites.getShotRequest(a & 0xf);
    s32 id;
    animSeq = 0x2d;
    priority = 2;
    work0 = 0x18;
    id = rec->playerSlot;
    work1 = id;
    VecFx32 *pos = &rec->pos;
    s32 vtx;
    Sky_ProjectToScreenX(&vtx, pos);
    VecFx32 *p34 = &screenPos;
    p34->x = vtx;
    p34->y = 0xc5000;
    p34->z = 0;
    VecFx32 *p40 = &screenVel;
    p40->x = 0;
    p40->y = -0xc000;
    p40->z = 0;
    s32 lim = data_020c8cb8 * 2;
    if (id == PlayerActor_GetLocalSessionSlot() && pos->z < lim) {
        localShot = 1;
    }
    s32 d = pos->z - gCameraLookAt.z;
    BOOL far = (d < -0x14000 || d > 0xf000) ? TRUE : FALSE;
    a |= far << 31;
    if (far || screenPos.y >= 0xc0000) {
        hidden = 1;
    }
    work2 = a;
}

void SkySprite::updateShot() {
    u32 flags = work2;
    work0--;
    if ((s32)work0 < 0) {
        VecFx32 *pos = &screenPos;
        VecFx32 *vel = &screenVel;
        if ((s32)work0 == -2) {
            u32 t = (flags >> 4) & 0xf;
            if (t == 1) {
                vel->x = 0x2800;
            } else if (t == 2) {
                vel->x = -0x2800;
            }
        }
        VEC_Add(pos, vel, pos);
        if (SkySprite_IsOffScreen(pos) || hit) {
            state = 3;
        }
    }
    if (hidden && !((flags >> 31) & 1) && screenPos.y < 0xc0000) {
        hidden = 0;
    }
}

void SkySprite::endShot() {
    release();
}

void SkySprite::initBird(u32 a) {
    work0 = a;
    work1 = sBirdDelays[a & 3] + 0x14;
    animSeq = 0x2c;
    priority = 2;
    s32 y = (Random_GlobalBelow(0x38) + 0x90) << 12;
    screenPos.x = -0x8000;
    screenPos.y = y;
    screenPos.z = 0;
    screenVel.x = 0x1000;
    screenVel.y = 0x2000;
    screenVel.z = 0;
    screenVel.x += Random_GlobalBelow2(0x8000);
    screenVel.y -= Random_GlobalBelow2(0x2000);
    work3 = -0x666;
    work4 = 0xcd;
    work3 += 0x8cd;
    work4 -= 0x666;
}

void SkySprite::updateBird() {
    if (work1 > 0) {
        work1--;
        if (work1 <= 0 && (work0 & 3) == 0) {
            gSkySprites.se.requestSustained(7, 0x805, &gVec3Zero);
        }
    } else {
        VecFx32 *pos = &screenPos;
        VecFx32 *vel = &screenVel;
        vel->x += work3;
        vel->y += work4;
        Vec_Scale(vel, 0xfc3);
        VEC_Add(pos, vel, pos);
        if (SkySprite_IsOffScreen(pos)) {
            state = 3;
        }
    }
}

void SkySprite::endBird() {
    release();
}

SkySprite *SkySprite::construct() {
    new (&anim) SpriteAnim();
    clear();
    resetFree();
    return this;
}

void SkySprite::clear() {
    u32 z = 0;
    u32 m = ~z;
    scaleX = 0x1000;
    scaleY = 0x1000;
    screenPos.x = 0;
    screenPos.y = 0;
    screenPos.z = 0;
    screenVel.x = 0;
    screenVel.y = 0;
    screenVel.z = 0;
    angle = 0;
    trackX = 0;
    wavePhase = 0;
    dir = 1;
    localShot = 0;
    hit = 0;
    hidden = 0;
    hFlip = 0;
    vFlip = 0;
    priority = m;
    cullRadius = 0;
    *(s8 *)&palette = m;
    work0 = 0;
    work1 = 0;
    work2 = 0;
    work3 = 0;
    work4 = 0;
}

void SkySprite::resetFree() {
    kind = 0xd;
    state = 0;
    gfxId = 0x34;
    animSeq = 0x2e;
    gfxSlot = 5;
    scaleX = 0x1000;
    scaleY = 0x1000;
    angle = 0;
}

extern "C" {
const SkyObjGfxDef sSkyObjGfxTable[52] = {
    {0, 0x0, 0xffff, 0, 0, 12, 0, 0},
    {0, 0xc, 0xffff, 0, 0, 12, 0, 0},
    {0, 0x8c, 0xffff, 0, 0, 12, 0, 0},
    {0, 0xffff, 0x200, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x204, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x208, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x20c, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x210, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x214, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x218, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x21c, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x280, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x284, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x288, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x28c, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x290, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x294, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x298, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x29c, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x300, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x304, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x308, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x30c, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x310, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x314, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x318, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x31c, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x380, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x384, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x388, 1, 0, 12, 0, 0},
    {0, 0xffff, 0x38c, 1, 0, 12, 0, 0},
    {1, 0x200, 0x280, 2, 4, 14, 0, 0},
    {1, 0x208, 0x288, 2, 4, 14, 0, 0},
    {1, 0x300, 0x380, 2, 13, 14, 0, 0},
    {1, 0x308, 0x388, 2, 13, 14, 0, 0},
    {1, 0x0, 0x80, 2, 2, 14, 0, 0},
    {1, 0x8, 0x88, 2, 2, 14, 0, 0},
    {1, 0x10, 0x90, 2, 2, 14, 0, 0},
    {1, 0x100, 0x180, 2, 3, 14, 0, 0},
    {1, 0x108, 0x188, 2, 3, 14, 0, 0},
    {1, 0x110, 0x190, 2, 3, 14, 0, 0},
    {1, 0x118, 0x198, 2, 3, 14, 0, 0},
    {0, 0x100, 0x180, 2, 0, 12, 0, 0},
    {0, 0x10c, 0xffff, 0, 9, 13, 0, 0},
    {0, 0x110, 0x190, 2, 9, 13, 0, 0},
    {0, 0x118, 0x198, 2, 9, 13, 0, 0},
    {0, 0x4, 0x80, 4, 1, 13, 0, 0},
    {1, 0x210, 0xffff, 0, 3, 14, 0, 0},
    {2, 0xffff, 0xffff, 6, -1, -1, 0, 0},
    {2, 0xffff, 0xffff, 6, -1, -1, 0, 0},
    {2, 0xffff, 0xffff, 6, -1, -1, 0, 0},
    {0, 0x10, 0xffff, 0, 0, 12, 0, 0},
};
const u8 sSkyNextCloudBgLayer[4] = {
    0x6, 0x2, 0x0, 0x0,
};
SpriteAnimFrame data_020e6124[44] = {
    {NULL, 2, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 1},
    {data_020e49f4, 1, 0, 2},
    {data_020e49f4, 1, 0, 2},
    {data_020e49f4, 1, 0, 3},
    {data_020e49f4, 1, 0, 3},
    {data_020e49f4, 1, 0, 4},
    {data_020e49f4, 1, 0, 4},
    {data_020e49f4, 1, 0, 5},
    {data_020e49f4, 1, 0, 5},
    {data_020e49f4, 1, 0, 6},
    {data_020e49f4, 1, 0, 6},
    {data_020e49f4, 1, 0, 6},
    {data_020e49f4, 1, 0, 7},
    {data_020e49f4, 1, 0, 7},
    {data_020e49f4, 1, 0, 8},
    {data_020e49f4, 1, 0, 8},
    {data_020e49f4, 1, 0, 9},
    {data_020e49f4, 1, 0, 9},
    {data_020e49f4, 1, 0, 10},
    {data_020e49f4, 1, 0, 10},
    {data_020e49f4, 1, 0, 11},
    {data_020e49f4, 1, 0, 11},
    {data_020e49f4, 1, 0, 11},
    {data_020e49f4, 1, 0, 12},
    {data_020e49f4, 1, 0, 12},
    {data_020e49f4, 1, 0, 12},
    {data_020e49f4, 1, 0, 13},
    {data_020e49f4, 1, 0, 13},
    {data_020e49f4, 1, 0, 13},
    {data_020e49f4, 1, 0, 14},
    {data_020e49f4, 1, 0, 14},
    {data_020e49f4, 1, 0, 14},
    {data_020e49f4, 1, 0, 14},
    {data_020e49f4, 1, 0, 15},
    {data_020e49f4, 1, 0, 15},
    {data_020e49f4, 1, 0, 15},
    {data_020e49f4, 1, 0, 15},
};
GXOamAttr data_020e4c24[2] = {{0x91f880e0, 0x9e, 0, 0x0, 0x0}, {0x91f88000, 0x11e, 0, 0x0, 0xffff}};
}
void SkySprite::initByKind(u32 a) {
    static void (SkySprite::*tbl[13])(u32) = {
        &SkySprite::initRainDrop, &SkySprite::initSnowFlake, &SkySprite::initParticle,
        &SkySprite::initShootingStar, &SkySprite::initMoon, &SkySprite::initBalloon,
        &SkySprite::initUfo, &SkySprite::initPete, &SkySprite::initLightning,
        &SkySprite::initFirework, &SkySprite::initRainbow,
        &SkySprite::initShot, &SkySprite::initBird,
    };
    void (SkySprite::*fn)(u32) = tbl[kind];
    if (fn) {
        (this->*fn)(a);
    }
}

extern "C" {
const u16 data_020d1148[24] = {
    0xd2, 0xcb, 0xc3, 0xbb, 0xb4, 0xb4, 0xb4, 0xd2, 0xf0, 0x104, 0x118, 0x12c,
    0x140, 0x140, 0x140, 0x140, 0x140, 0x140, 0x140, 0x140, 0x140, 0x11b, 0xff, 0xee,
};
GXOamAttr data_020e4644[1] = {{0x81f000f0, 0x9c, 0, 0x3, 0xffff}};
const s32 sSnowAnimIds[3] = {0x5, 0x6, 0x7};
GXOamAttr data_020e4c34[2] = {{0x51fc80e0, 0x9d, 0, 0x0, 0x0}, {0x51fc8000, 0x11d, 0, 0x0, 0xffff}};
const u8 sSkyObjPalOverrideSlots[15] = {0xde, 0xdf, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xcb, 0xcc, 0xcd, 0xce,
    0xcf};
char *data_020e4d70[5] = {data_020e4fe8, data_020e5008, data_020e5028,
    data_020e5048, data_020e5048};
const u16 data_020d1178[24] = {
    0x96, 0x8f, 0x87, 0x80, 0x78, 0x78, 0x78, 0x96, 0xb4, 0xbc, 0xc3, 0xc1,
    0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xd2, 0xc3, 0xb4, 0xa5,
};
SpriteAnimFrame data_020e4ae8[1] = {
    {data_020e4864, 4, 0, 0},
};
GXOamAttr data_020e4c54[2] = {{0x71fc8000, 0x9d, 0, 0x0, 0x0}, {0x71fc80e0, 0x11d, 0, 0x0, 0xffff}};
GXOamAttr data_020e497c[1] = {{0x81f000f0, 0x11c, 0, 0x2, 0xffff}};
SpriteAnimFrame data_020e5258[4] = {
    {data_020e49a4, 1, 0, 0},
    {data_020e4a1c, 1, 0, 0},
    {data_020e486c, 1, 0, 0},
    {data_020e477c, 1, 0, 0},
};
const u32 sFireworkShellTypes[4] = {0x21, 0x24, 0x27, 0x2a};
s32 sSnowSideToggle = 1;
const s8 sWeatherHourTable[768] = {
    0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2,
    0x2, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x2, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, 0x2, 0x1, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, 0x2, 0x2, 0x2, 0x1, 0x1, 0x2, 0x2,
    0x2, 0x1, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x2, 0x2, 0x2,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x1,
    0x1, 0x1, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2,
    0x3, 0x3, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x3, 0x2, 0x2, 0x1, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2, 0x3, 0x3,
    0x3, 0x2, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x2, 0x3, 0x3, 0x2,
    0x2, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x3, 0x3, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x2, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x4, 0x3, 0x3, 0x4,
    0x4, 0x4, 0x4, 0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x3, 0x3, 0x4, 0x4, 0x4, 0x4, 0x4,
    0x4, 0x3, 0x3, 0x3, 0x4, 0x4, 0x4, 0x3, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
    0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x3, 0x3, 0x3, 0x2, 0x2, 0x3, 0x3, 0x3, 0x4, 0x4,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, 0x2,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x3, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0x2, 0x2, 0x3, 0x3, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x0, 0x0, 0x1, 0x1, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x2, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x2,
    0x3, 0x2, 0x2, 0x2, 0x2, 0x2, 0x3, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, 0x2, 0x3, 0x2, 0x2, 0x2, 0x1, 0x1,
    0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0,
    0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x3, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x3, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x4, 0x2, 0x1, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1,
    0x4, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x2, 0x2, 0x1, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x4, 0x4, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x2,
    0x2, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x2,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2, 0x3, 0x3, 0x2,
    0x2, 0x2, 0x1, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x1, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x4, 0x4, 0x3, 0x3, 0x3, 0x2, 0x2, 0x3,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x1, 0x1, 0x2, 0x2, 0x2, 0x2,
};
GXOamAttr data_020e5610[10] = {{0x1ff0009, 0xd4, 0, 0x3, 0x0}, {0x300300e6, 0x94, 0, 0x3, 0x0}, {0x201100ff, 0xb6, 0, 0x3, 0x0}, {0x1e900fa, 0xb6, 0, 0x3, 0x0}, {0xd00f4, 0xb6, 0, 0x3, 0x0}, {0x1f700e9, 0x95, 0, 0x3, 0x0}, {0x300f000c, 0xb4, 0, 0x3, 0x0}, {0x1000ea, 0xd7, 0, 0x3, 0x0}, {0x1f000ef, 0xb4, 0, 0x3, 0x0}, {0x1ec0005, 0x97, 0, 0x3, 0xffff}};
GXOamAttr data_020e5440[9] = {{0x1f500e5, 0xb7, 0, 0x3, 0x0}, {0x30130001, 0xb7, 0, 0x3, 0x0}, {0x100d0009, 0xb6, 0, 0x3, 0x0}, {0x200600e3, 0xb6, 0, 0x3, 0x0}, {0x1e800f9, 0xb6, 0, 0x3, 0x0}, {0x301000f7, 0xb6, 0, 0x3, 0x0}, {0x201100ea, 0x97, 0, 0x3, 0x0}, {0x31ed00ed, 0x94, 0, 0x3, 0x0}, {0x11ed0006, 0x97, 0, 0x3, 0xffff}};
u8 data_020e4634[4] = {0x9};
GXOamAttr data_020e48d4[1] = {{0x81f000f0, 0x98, 0, 0x2, 0xffff}};
const SkySpawnRate sSnowSpawnRates[4] = {{0x3e8, 0x7d0}, {0x3e8, 0x7d0}, {0x320, 0x3e8}, {0x258, 0x320}};
SpriteAnimFrame data_020e58a8[9] = {
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 0},
    {data_020e49f4, 1, 0, 2},
    {NULL, 37, 0, 0},
};
const s32 sParticleAnimIds[6] = {0x2b, 0x10, 0x19, 0x1a, 0x1b, 0x1c};
GXOamAttr data_020e470c[1] = {{0x1fc00fc, 0x78, 0, 0x0, 0xffff}};
const s32 sFireworkScaleSmallLong[44] = {0x1000, 0x2800, 0x11c7, 0xc4f, 0xa00, 0x8e4, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800};
char data_020e5088[31] = "/sky/d_2d_b_cld_f_a_bg_nsc.bin";
GXOamAttr data_020e52b8[6] = {{0x1700f7, 0xb7, 0, 0x3, 0x0}, {0x11f0000c, 0x95, 0, 0x3, 0x0}, {0x100f000c, 0xd4, 0, 0x3, 0x0}, {0x11e200fc, 0x95, 0, 0x3, 0x0}, {0x101400e8, 0xd6, 0, 0x3, 0x0}, {0x11ea00e9, 0xb6, 0, 0x3, 0xffff}};
WeatherManager gWeatherManager;
GXOamAttr data_020e5488[9] = {{0x130002, 0xf7, 0, 0x3, 0x0}, {0x31f500e5, 0xf6, 0, 0x3, 0x0}, {0xc0007, 0xf6, 0, 0x3, 0x0}, {0x200600e4, 0xf6, 0, 0x3, 0x0}, {0x1e600fa, 0xb6, 0, 0x3, 0x0}, {0x101300f5, 0xb6, 0, 0x3, 0x0}, {0x201300ea, 0x97, 0, 0x3, 0x0}, {0x31eb00eb, 0xd5, 0, 0x3, 0x0}, {0x11f20008, 0x97, 0, 0x3, 0xffff}};
GXOamAttr data_020e4a5c[1] = {{0x400080f0, 0x94, 0, 0x0, 0xffff}};
GXOamAttr data_020e4cc4[2] = {{0x91f880e0, 0x9a, 0, 0x0, 0x0}, {0x91f88000, 0x11a, 0, 0x0, 0xffff}};
GXOamAttr data_020e4cd4[2] = {{0x51fc80e0, 0x99, 0, 0x0, 0x0}, {0x51fc8000, 0x119, 0, 0x0, 0xffff}};
SpriteAnimFrame data_020e4ac4[1] = {
    {data_020e46dc, 1, 0, 0},
};
SpriteAnimFrame data_020e4a94[1] = {
    {data_020e47c4, 1, 0, 0},
};
char data_020e4f88[30] = "/sky/d_2d_b_cld_f1_bg_ncl.bin";
GXOamAttr data_020e4cf4[2] = {{0x61fc8000, 0x99, 0, 0x0, 0x0}, {0x61fc80e0, 0x119, 0, 0x0, 0xffff}};
U234_Record sSkyProcProfile = {(void *)SkyProc_Create, 0x89, 0x8f};
char data_020e4e10[27] = "/sky/d_2d_b_cld_bg_ncg.bin";
}
void SkySprite::updateByKind() {
    static void (SkySprite::*tbl[13])() = {
        &SkySprite::updateRainDrop, &SkySprite::updateSnowFlake, &SkySprite::updateParticle,
        &SkySprite::updateShootingStar, &SkySprite::updateMoon, &SkySprite::updateBalloon,
        &SkySprite::updateUfo, &SkySprite::updatePete, &SkySprite::updateLightning,
        &SkySprite::updateFirework, &SkySprite::updateRainbow, &SkySprite::updateShot,
        &SkySprite::updateBird,
    };
    void (SkySprite::*fn)() = tbl[kind];
    if (fn) {
        (this->*fn)();
    }
}


// ======== unk_020bd868.cpp ========
extern "C" {
static inline void SkyObjGfx_LoadTileRow(const u8 *d, u32 a, u32 n, u32 k, BOOL flag) {
    u32 sz = n << 5;
    u32 dst = (k + a) << 5;
    GX_LoadOBJ(d, dst, sz);
    if (flag) GXS_LoadOBJ(d, dst, sz);
}
}


extern "C" {
GXOamAttr data_020e47f4[1] = {{0x81f000f0, 0x118, 0, 0x4, 0xffff}};
GXOamAttr data_020e4d14[2] = {{0x71fc8000, 0x99, 0, 0x0, 0x0}, {0x71fc80e0, 0x119, 0, 0x0, 0xffff}};
GXOamAttr data_020e4d24[2] = {{0x71fc8000, 0x98, 0, 0x0, 0x0}, {0x71fc80e0, 0x118, 0, 0x0, 0xffff}};
const SkyObjCharLayout sSkyObjCharLayouts[5] = {{0x0, 0x4, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x4}, {0x4, 0x8, 0x4, 0x8}, {0x0, 0xc, 0x4, 0x8}, {0x4, 0x8, 0x0, 0xc}};
u32 sSkyOutdoors;
GXOamAttr data_020e487c[1] = {{0x0, 0x94, 0, 0x0, 0xffff}};
char *data_020e4d98[5] = {data_020e5068, data_020e5068, data_020e5088,
    data_020e50a8, data_020e50c8};
char data_020e4fa8[30] = "/sky/d_2d_b_cld_r0_bg_ncl.bin";
SkySpriteAnimDef sSkySpriteAnimSeqs[46] = {
    {{data_020e5318, 4}, 1},
    {{data_020e5704, 8}, 0},
    {{data_020e4adc, 1}, 1},
    {{data_020e4ae8, 1}, 1},
    {{data_020e4af4, 1}, 1},
    {{data_020e4b00, 1}, 1},
    {{data_020e4b0c, 1}, 1},
    {{data_020e4b18, 1}, 1},
    {{data_020e50e8, 3}, 1},
    {{data_020e510c, 3}, 1},
    {{data_020e5130, 3}, 1},
    {{data_020e5154, 3}, 1},
    {{data_020e5178, 3}, 1},
    {{data_020e519c, 3}, 1},
    {{data_020e51c0, 3}, 1},
    {{data_020e51e4, 3}, 1},
    {{data_020e4b30, 1}, 1},
    {{data_020e4b3c, 1}, 1},
    {{data_020e4b48, 1}, 1},
    {{data_020e5348, 4}, 0},
    {{data_020e5258, 4}, 0},
    {{data_020e4a64, 1}, 1},
    {{data_020e56b0, 7}, 0},
    {{data_020e5288, 4}, 1},
    {{data_020e53b0, 6}, 0},
    {{data_020e4a70, 1}, 1},
    {{data_020e4b24, 1}, 1},
    {{data_020e4a7c, 1}, 1},
    {{data_020e4a88, 1}, 1},
    {{data_020e5764, 9}, 1},
    {{data_020e5af4, 44}, 1},
    {{data_020e57d0, 9}, 1},
    {{data_020e5d04, 44}, 1},
    {{data_020e4a94, 1}, 1},
    {{data_020e583c, 9}, 1},
    {{data_020e5f14, 44}, 1},
    {{data_020e4aa0, 1}, 1},
    {{data_020e58a8, 9}, 1},
    {{data_020e6124, 44}, 1},
    {{data_020e4aac, 1}, 1},
    {{data_020e5914, 9}, 1},
    {{data_020e6334, 44}, 1},
    {{data_020e4ab8, 1}, 1},
    {{data_020e5980, 15}, 1},
    {{data_020e4ddc, 2}, 0},
    {{data_020e4ac4, 1}, 1},
};
char data_020e50c8[31] = "/sky/d_2d_b_cld_r_a_bg_nsc.bin";
SpriteAnimFrame data_020e4b30[1] = {
    {data_020e498c, 1, 0, 0},
};
GXOamAttr data_020e4a54[1] = {{0x81f880f0, 0x118, 0, 0x4, 0xffff}};
GXOamAttr data_020e4a3c[1] = {{0x1fc80f8, 0x94, 0, 0x0, 0xffff}};
GXOamAttr data_020e4984[1] = {{0x81f880f0, 0x98, 0, 0x4, 0xffff}};
const u8 sSkyStarBgLayer[4] = {
    0x4, 0x0, 0x0, 0x0,
};
GXOamAttr data_020e49ec[1] = {{0x81e003e0, 0x98, 0, 0x9, 0xffff}};
GXOamAttr data_020e476c[1] = {{0x81f880f0, 0x98, 0, 0x4, 0xffff}};
SpriteAnimFrame data_020e4b3c[1] = {
    {data_020e482c, 1, 0, 0},
};
SpriteAnimFrame data_020e53b0[6] = {
    {data_020e466c, 2, 0, 0},
    {data_020e495c, 2, 0, 0},
    {data_020e4764, 2, 0, 0},
    {data_020e48dc, 2, 0, 0},
    {data_020e499c, 2, 0, 0},
    {data_020e469c, 2, 0, 0},
};
GXOamAttr data_020e49d4[1] = {{0x1fc00fc, 0x77, 0, 0x0, 0xffff}};
SpriteAnimFrame data_020e5980[15] = {
    {data_020e5208, 1, 0, 0},
    {data_020e54d0, 1, 0, 0},
    {data_020e5520, 1, 0, 0},
    {data_020e5570, 1, 0, 0},
    {data_020e55c0, 1, 0, 0},
    {data_020e5610, 1, 0, 0},
    {data_020e53f8, 3, 0, 0},
    {data_020e5440, 3, 0, 0},
    {data_020e5488, 3, 0, 0},
    {data_020e5378, 3, 0, 0},
    {data_020e52b8, 3, 0, 0},
    {data_020e52e8, 3, 0, 0},
    {data_020e4bc4, 3, 0, 0},
    {data_020e4bd4, 3, 0, 0},
    {data_020e4be4, 3, 0, 0},
};
SkyLight sSkyLight;
const u8 *const data_020e478c[2] = {data_020d0ec8, data_020d0ee0};
GXOamAttr data_020e469c[1] = {{0x91f000f0, 0x98, 0, 0x2, 0xffff}};
char data_020e4e48[30] = "/sky/d_2d_b_cld_b0_bg_nsc.bin";
GXOamAttr data_020e495c[1] = {{0x81f000f0, 0x118, 0, 0x2, 0xffff}};
GXOamAttr data_020e4764[1] = {{0x81f000f0, 0x11c, 0, 0x2, 0xffff}};
GXOamAttr data_020e4874[1] = {{0x81f880f0, 0x9e, 0, 0x4, 0xffff}};
u16 sMoonColors[6];
char data_020e4e68[30] = "/sky/d_2d_b_cld_f0_bg_nsc.bin";
GXOamAttr data_020e5378[7] = {{0x301500f7, 0xf6, 0, 0x3, 0x0}, {0x11f1000a, 0x95, 0, 0x3, 0x0}, {0xe000b, 0xd4, 0, 0x3, 0x0}, {0x11e300fb, 0x95, 0, 0x3, 0x0}, {0x200600e4, 0xd4, 0, 0x3, 0x0}, {0x101300e8, 0xd5, 0, 0x3, 0x0}, {0x31ea00ea, 0xf5, 0, 0x3, 0xffff}};
const s32 sFireworkScaleSmall[9] = {0x2800, 0x11c7, 0xc4f, 0xa00, 0x8e4, 0x86c, 0x86c, 0x86c, 0x86c};
char data_020e4e88[30] = "/sky/d_2d_b_cld_f1_bg_nsc.bin";
}
void SkySprite::endByKind() {
    static void (SkySprite::*tbl[13])() = {
        &SkySprite::endRainDrop,
        &SkySprite::endSnowFlake,
        &SkySprite::endParticle,
        &SkySprite::endShootingStar,
        &SkySprite::endMoon,
        &SkySprite::endBalloon,
        &SkySprite::endUfo,
        &SkySprite::endPete,
        &SkySprite::endLightning,
        &SkySprite::endFirework,
        &SkySprite::endRainbow,
        &SkySprite::endShot,
        &SkySprite::endBird,
    };
    void (SkySprite::*fn)() = tbl[kind];
    if (fn) (this->*fn)();
}

extern "C" void SkySprite_StartAnim(SkySprite* p) {
    Unk_020be0bc_E i = (Unk_020be0bc_E)p->animSeq;
    SkySpriteAnimDef* row = &sSkySpriteAnimSeqs[i];
    p->anim.setSeq((SpriteAnimSeq *)(row));
    p->anim.setPlayOnce(row->playOnce);
    p->anim.restart();
}

extern "C" void SkySprite_Release(SkySprite* p) {
    s32 i = p->gfxSlot;
    if (i != 6) {
        SkyObjGfxSlot_Release(&gSkySprites.gfxSlots[i]);
    }
    (p)->resetFree();
}

extern "C" void SkySprite_BeginCrossing(SkySprite* p, u8 a, s32 b) {
    p->dir = a;
    s32 v;
    if (p->dir != 0) {
        v = 0;
    } else {
        v = sSkyCrossingWidth;
    }
    p->trackX = v;
    return SkySprite_ProjectCrossing(p, b);
}

extern "C" BOOL SkySprite_AdvanceCrossing(SkySprite* p, s32 a, s32 b, s32 c) {
    BOOL r = FALSE;
    if (p->dir != 0) {
        p->trackX += a;
        if (p->trackX > sSkyCrossingWidth) r = TRUE;
    } else {
        p->trackX -= a;
        if (p->trackX < 0) r = TRUE;
    }
    if (!r) {
        p->wavePhase += c;
        SkySprite_ProjectCrossing(p, b);
    }
    return r;
}

extern "C" void SkySprite_ProjectWorldPos(SkySprite* p, VecFx32* q, s32 a) {
    SkyLookAtVec loc;
    loc.x = gCameraLookAt.x;
    loc.y = gCameraLookAt.y;
    loc.z = gCameraLookAt.z;
    s32 inv, idx, z, y, t;
    s32 sx, sy, sc, fx, k, dz;
    dz = loc.z - q->z;
    fx = FX_Div(q->x - loc.x, data_020c8cbc);
    dz = FX_Div(dz, data_020c8cb8 << 2);
    t = func_01ffcb0c(-0x1000, dz - 0x1000);
    k = func_01ffcb0c(dz, t + 0x1000);
    s32 w = func_01ffcb0c(-0x666, k) + 0xe66;
    y = func_01ffcb0c(fx, w);
    z = func_01ffcb0c(y + 0x800, 0x1000);
    sx = z << 8;
    sy = func_01ffcb0c(0x50000, k) + 0x50000;
    inv = 0x1000 - k;
    if (gCamera != 0) {
        idx = func_01ffcb0c(Camera_GetCloseUpFactor(), inv);
    } else {
        idx = 0;
    }
    idx = func_01ffcb0c(idx, 0x10000);
    p->screenPos.x = sx;
    p->screenPos.y = sy - idx;
    p->screenPos.z = 0;
    sc = func_01ffcb0c(-0xc00, k) + 0x1000;
    if (sc < 0x400) sc = 0x400;
    else if (sc > 0x1000) sc = 0x1000;
    z = FX_Inv(sc);
    p->scaleX = z;
    p->scaleY = z;
    idx = (u16)p->wavePhase >> 4;
    z = func_01ffcb0c(data_02135f44[idx * 2], a);
    z = func_01ffcb0c(z, sc);
    p->screenPos.y += z;
}

extern "C" void SkySprite_ProjectCrossing(SkySprite* p, s32 a) {
    VecFx32 t;
    t.x = p->trackX;
    t.y = 0;
    t.z = data_020c8cb8;
    SkySprite_ProjectWorldPos(p, &t, a);
}

extern "C" BOOL SkySprite_CheckShotHit(SkySprite* p, s32 a, s32 b, s32 c) {
    s32 d;
    s32 w = func_01ffcb0c(a, p->scaleX);
    s32 h = func_01ffcb0c(b, p->scaleY);
    s32 dy = func_01ffcb0c(c, p->scaleY);
    s32 hw = _s32_div_f(w, 2);
    s32 hh = _s32_div_f(h, 2);
    s32 y = p->screenPos.y + dy;
    s32 x = p->screenPos.x;
    s32 left = x - hw;
    s32 right = x + hw;
    s32 top = y - hh;
    s32 bottom = y + hh;
    SkySprite* o = &gSkySprites.sprites[33];
    BOOL found = FALSE;
    for (; o < &gSkySprites.sprites[45]; o++) {
        if (o->state == 2 && o->localShot != 0) {
            d = o->screenVel.y;
            if (d < 0) d = -d;
            s32 xl = o->screenPos.x - 0x1000;
            s32 yt = o->screenPos.y - 0x1000;
            s32 yb = d + (o->screenPos.y + 0x1000);
            s32 xr = o->screenPos.x + 0x1000;
            if (left <= xr && right >= xl && top <= yb && bottom >= yt) {
                found = TRUE;
                o->hit = 1;
            }
        }
    }
    return found;
}

extern "C" s32 SkySprite_MakeSoundPos(VecFx32* out, s32 a, s32 b, s32 c) {
    s32 t = func_01ffcb0c(0x400, c);
    s32 u = func_01ffcb0c(0x1000, c);
    s32 v = FX_Div(0x1000 - t, u - t);
    if (v < 0) v = 0;
    else if (v > 0x1000) v = 0x1000;
    out->x = a;
    out->y = b;
    out->z = v;
    return v;
}

extern "C" void SkySprite_GetSoundPos(SkySprite* p, VecFx32* out) {
    SkySprite_MakeSoundPos(out, p->screenPos.x, p->screenPos.y, p->scaleX);
}

extern "C" void SkySprite_GetPos(SkySprite* p, VecFx32* out) {
    out->x = p->screenPos.x;
    out->y = p->screenPos.y;
    out->z = 0;
}

extern "C" void SkySprite_RequestSeSustained(SkySprite* p, s32 a) {
    VecFx32 out;
    SkySprite_GetSoundPos(p, &out);
    gSkySprites.se.requestSustained(0, a, &out);
}

extern "C" void SkySprite_RequestSe(SkySprite* p, s32 a) {
    VecFx32 out;
    SkySprite_GetSoundPos(p, &out);
    gSkySprites.se.request(0, a, &out);
}

extern "C" void SkySprite_RequestSeSustainedOn(SkySprite* p, s32 a, s32 b) {
    VecFx32 out;
    SkySprite_GetPos(p, &out);
    gSkySprites.se.requestSustained(a, b, &out);
}

// SkyObjGfxLoader::init (calls it)
extern "C" void SkyObjGfxLoader_Init(SkyObjGfxLoader *p) {
    FS_InitFile(&p->file);
    MI_CpuFill8(p->readBuf, 0, sizeof(p->readBuf));
    MI_CpuFill8(p->chars, 0, sizeof(p->chars));
    MI_CpuFill8(p->palettes, 0, sizeof(p->palettes));
    MI_CpuFill8(p->moonPalette, 0, sizeof(p->moonPalette));
    p->magic = 0x12345678;
}

extern "C" BOOL SkyObjGfxLoader_LoadChars(SkyObjGfxLoader *p, s32 idx) {
    const SkyObjGfxDef* row = &sSkyObjGfxTable[idx];
    const SkyObjCharLayout* t;
    s32 a = FS_OpenFile(&p->file, sSkyObjCharFiles[row->charFile]);
    BOOL ok1 = TRUE;
    BOOL ok2 = TRUE;
    u8* src;
    s32 i;
    s32 f;
    t = &sSkyObjCharLayouts[row->charLayout];
    if (row->charTileA != 0xffff) {
        src = p->chars;
        ok1 &= FS_SeekFile(&p->file, (row->charTileA & ~0x1f) << 5, 0);
        for (i = 0; i < 4; i++) {
            if (t->topTileCount != 0) {
                ok2 &= FS_ReadFile(&p->file, p->readBuf, 0x400) > 0;
                MI_CpuCopy8(p->readBuf + ((row->charTileA & 0x1f) << 5), src + (t->topTileX << 5), t->topTileCount << 5);
            }
            src += 0x180;
        }
    }
    if (row->charTileB != 0xffff) {
        src = p->chars + 4 * 0x180;
        ok1 &= FS_SeekFile(&p->file, (row->charTileB & ~0x1f) << 5, 0);
        for (i = 4; i < 8; i++) {
            if (t->bottomTileCount != 0) {
                ok2 &= FS_ReadFile(&p->file, p->readBuf, 0x400) > 0;
                MI_CpuCopy8(p->readBuf + ((row->charTileB & 0x1f) << 5), src + (t->bottomTileX << 5), t->bottomTileCount << 5);
            }
            src += 0x180;
        }
    }
    f = FS_CloseFile(&p->file);
    if (a && ok1 && ok2 && f) return TRUE;
    return FALSE;
}

extern "C" {
const s32 sSkyPaletteSetByLevel[5] = {0x0, 0x0, 0x0, 0x1, 0x1};
}
extern "C" BOOL SkyObjGfxLoader_LoadPalettes(SkyObjGfxLoader *p) {
    char name1[0x17] = "/sky/a_sky_obj_ncl.bin";
    s32 a = FS_OpenFile(&p->file, name1);
    BOOL b = FS_ReadFile(&p->file, p->palettes, 0x1c0) > 0;
    s32 c = FS_CloseFile(&p->file);
    char name2[0x1c] = "/sky/a_sky_moon_obj_ncl.bin";
    s32 d = FS_OpenFile(&p->file, name2);
    BOOL e = FS_ReadFile(&p->file, p->moonPalette, 0x140) > 0;
    s32 f = FS_CloseFile(&p->file);
    if (a && b && c && d && e && f) return TRUE;
    return FALSE;
}

extern "C" void SkyObjGfxLoader_FlushChars(SkyObjGfxLoader *p) {
    DC_FlushRange(p->chars, 0xc00);
}

extern "C" void SkyObjGfxSlot_Init(SkyObjGfxSlot* p) {
    p->gfxId = 0x34;
    p->refCount = 0;
    p->loaded = 0;
    p->dirty = 0;
}

extern "C" void SkyObjGfxSlot_Upload(SkyObjGfxSlot* p, SkyObjGfxLoader *base) {
    const SkyObjCharLayout* row;
    const u8 *dst;
    u32 o, k;
    BOOL flag;
    if (gWeatherManager.engine == 0) flag = TRUE; else flag = FALSE;
    SkyObjGfxLoader_FlushChars(base);
    const SkyObjGfxDef* ent = &sSkyObjGfxTable[p->gfxId];
    row = &sSkyObjCharLayouts[ent->charLayout];
    dst = base->chars;
    o = 0;
    k = 0x94;
    for (s32 i = 0; i < 4; i++) {
        const u8 *d;
        u32 n = row->topTileCount;
        if (n != 0) {
            u32 a = row->topTileX;
            d = dst + (o << 5);
            d += a << 5;
            SkyObjGfx_LoadTileRow(d, a, n, k, flag);
        }
        o += 0xc;
        k += 0x20;
    }
    for (s32 i = 4; i < 8; i++) {
        const u8 *d;
        u32 n = row->bottomTileCount;
        if (n != 0) {
            u32 a = row->bottomTileX;
            d = dst + (o << 5);
            d += a << 5;
            SkyObjGfx_LoadTileRow(d, a, n, k, flag);
        }
        o += 0xc;
        k += 0x20;
    }
}

extern "C" void SkyObjGfxSlot_Acquire(SkyObjGfxSlot* p, s32 v, SkyObjPalette *x) {
    p->refCount++;
    if (v != p->gfxId) {
        p->loaded = 0;
        x->invalidateFor(v);
        p->gfxId = v;
    }
}

extern "C" void SkyObjGfxSlot_SetGfx(SkyObjGfxSlot* p, s32 v) {
    if (v != p->gfxId) {
        p->loaded = 0;
        p->dirty = 1;
        p->gfxId = v;
    }
}

extern "C" void SkyObjGfxSlot_RefreshPalette(SkyObjGfxSlot* p, s32 a) {
    p->dirty = 1;
    gSkySprites.palette.invalidateFor(a);
}

extern "C" void SkyObjGfxSlot_Release(SkyObjGfxSlot* p) {
    s32 t = p->refCount - 1;
    if (t <= 0) t = 0;
    p->refCount = t;
}

extern "C" SkyObjPalette* SkyObjPalette_Init(SkyObjPalette* p) {
    volatile u16 tmp;
    u16* end;
    u16* q;
    p->balloonColor = 0;
    p->overrideMask = 0;
    (p)->invalidate(-1);
    end = (u16*)p->colors;
    q = p->overrideColors;
    tmp = sSkyObjPalDefaultColor;
    for (; q < end; q++) {
        *q = tmp;
    }
    MI_CpuFill8(p->colors, 0, 0x20);
    return p;
}

extern "C" void SkyObjPalette_Load(SkyObjPalette* p, SkyObjGfxLoader *base, u32 idx) {
    const SkyObjGfxDef* row = &sSkyObjGfxTable[idx];
    s32 a = row->srcPalette;
    u32 b;
    s32 res;
    if (idx - 0x1f <= 1) {
        a += p->balloonColor;
    }
    b = row->objPalette << 5;
    MI_CpuCopy8(base->palettes + a * 32, p->colors, 0x20);
    res = (p)->applyOverrides(row->objPalette);
    DC_FlushRange(p->colors, 0x20);
    GX_LoadOBJPltt(p->colors, b, 0x20);
    if (gWeatherManager.engine == 0) {
        GXS_LoadOBJPltt(p->colors, b, 0x20);
    }
    if (res == 0) {
        (p)->setLoadedRow(row->objPalette, a);
    }
}


// ======== unk_020bcf04.cpp ========
typedef void (SkyShotSequence::*SkyShotActFn)();


BOOL SkyObjPalette::applyOverrides(s32 v) {
    BOOL result = FALSE;
    if (overrideMask != 0) {
        for (s32 i = 0; i < 15; i++) {
            BOOL has = hasOverride(i);
            s32 s = sSkyObjPalOverrideSlots[i];
            BOOL match = ((s >> 4) & 0xf) == v;
            if (has && match) {
                colors[s & 0xf] = overrideColors[i];
                result = TRUE;
            }
        }
    }
    return result;
}

void SkyObjPalette::pickBalloonColor() {
    balloonColor = Random_GlobalBelow(5);
    invalidate(sSkyObjGfxTable[0x1f].objPalette);
}

void SkyObjPalette::invalidate(s32 v) {
    if (v < 0) {
        for (s32 i = 0; (u32)i < 4; i++) {
            loadedSrcPalettes[i] = -1;
        }
    } else {
        loadedSrcPalettes[v - 12] = -1;
    }
}

void SkyObjPalette::invalidateFor(s32 idx) {
    const SkyObjGfxDef* e = &sSkyObjGfxTable[idx];
    invalidate(e->objPalette);
}

BOOL SkyObjPalette::isLoaded(s32 idx) {
    const SkyObjGfxDef* e = &sSkyObjGfxTable[idx];
    s32 y = e->objPalette;
    s32 x = e->srcPalette;
    BOOL result = TRUE;
    if (y >= 0 && x >= 0) {
        if (x != loadedSrcPalettes[y - 12]) {
            result = FALSE;
        }
    }
    return result;
}

void SkyObjPalette::setLoadedRow(s32 a, s32 b) {
    if (a >= 0 && b >= 0) {
        loadedSrcPalettes[a - 12] = b;
    }
}

void SkyObjPalette::clearOverride(s32 idx) {
    overrideMask &= ~(1 << idx);
}

BOOL SkyObjPalette::hasOverride(s32 idx) {
    if (overrideMask & (1 << idx)) {
        return TRUE;
    }
    return FALSE;
}

void SkyObjPalette::setOverride(s32 idx, u16* p) {
    overrideMask |= 1 << idx;
    overrideColors[idx] = *p;
    invalidate((sSkyObjPalOverrideSlots[idx] >> 4) & 0xf);
}

SkyShotRequest::SkyShotRequest() {
    clear();
}

void SkyShotRequest::set(s32 a, s32* p, u8 b) {
    playerSlot = a;
    pos.x = p[0];
    pos.y = p[1];
    pos.z = p[2];
    golden = b;
    pending = 1;
}

void SkyShotRequest::clear() {
    playerSlot = 4;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    golden = 0;
    pending = 0;
}

SkyShotSequence::SkyShotSequence() {
    reset();
}

void SkyShotSequence::startShot(s32 a) {
    if (a == PlayerActor_GetLocalSessionSlot()) {
        state = 1;
        playerSlot = a;
        timer = 0x2b;
    }
}

void SkyShotSequence::endWatch(s32 a) {
    PlayerActor_SetWatchMode(0, a);
}

void SkyShotSequence::onBalloonHit(u8 a) {
    goldenBalloon = a;
    state = 2;
    PlayerActor_SetWatchMode(1, playerSlot);
    SkyShot_AddHit();
}

void SkyShotSequence::onBalloonFell() {
    state = 3;
    timer = 2;
    balloonDropEnded = 0;
    landSePending = 0;
    splashSePending = 0;
}

void SkyShotSequence::onUfoHit() {
    state = 4;
    PlayerActor_SetWatchMode(1, playerSlot);
    SkyShot_AddHit();
}

void SkyShotSequence::onUfoFell() {
    state = 5;
    timer = 0x1e;
}

void SkyShotSequence::onPeteHit() {
    state = 7;
    PlayerActor_SetWatchMode(1, playerSlot);
    SkyShot_AddHit();
}

void SkyShotSequence::onPeteFell() {
    state = 8;
    timer = 0x14;
}

void SkyShotSequence::setTarget(s32 a, s32 b, s32 c, u8 d) {
    targetX = a;
    targetY = b;
    targetScale = c;
    fallDir = d;
}

extern "C" {
const u16 data_020d0ff8[24] = {
    0x53f9, 0x53f9, 0x53f9, 0x53f9, 0x53f9, 0x428f, 0x3105, 0x1882, 0x820, 0x420, 0x400, 0x0,
    0x0, 0x400, 0x800, 0xc01, 0x1001, 0x2444, 0x38a8, 0x468f, 0x53f9, 0x53f9, 0x53f9, 0x53f9,
};
char data_020e4ea8[30] = "/sky/d_2d_b_cld_c0_bg_nsc.bin";
GXOamAttr data_020e485c[1] = {{0x400080f0, 0x96, 0, 0x0, 0xffff}};
const s32 sSnowParallax[3] = {0x7fff, 0x6667, 0x4cce};
SpriteAnimFrame data_020e5704[8] = {
    {data_020e476c, 2, 0, 0},
    {data_020e4a14, 3, 0, 0},
    {data_020e489c, 4, 0, 0},
    {data_020e4874, 10, 0, 0},
    {data_020e48f4, 2, 0, 0},
    {data_020e4a54, 3, 0, 0},
    {data_020e4984, 4, 0, 0},
    {data_020e48ec, 10, 0, 0},
};
GXOamAttr data_020e491c[1] = {{0x91f000f0, 0x9c, 0, 0x2, 0xffff}};
GXOamAttr data_020e4b64[2] = {{0x41fc80e0, 0x99, 0, 0x0, 0x0}, {0x41fc8000, 0x119, 0, 0x0, 0xffff}};
SpriteAnimFrame data_020e4a70[1] = {
    {data_020e46ec, 1, 0, 0},
};
u8 sSkyLineTablesReady;
const u8 data_020d0e04[5] = {0xf, 0xe, 0xd, 0xe, 0xd};
StarTwinkle data_021efc18;
GXOamAttr data_020e47b4[1] = {{0x41fb80f0, 0x97, 0, 0x9, 0xffff}};
const u16 data_020d1028[24] = {
    0x50a5, 0x50a5, 0x50a5, 0x50a5, 0x50a5, 0x4cc6, 0x4ce7, 0x4d4a, 0x4d8c, 0x4d8c, 0x4d6b, 0x4d6b,
    0x4d4a, 0x494a, 0x494a, 0x494a, 0x494a, 0x4929, 0x4ce7, 0x4ca5, 0x50a5, 0x50a5, 0x50a5, 0x50a5,
};
const u16 *const data_020e4b74[4] = {data_020d0fc8, data_020d0ff8, data_020d1028, data_020d1058};
GXOamAttr data_020e47ec[1] = {{0x81f000f0, 0x11c, 0, 0x4, 0xffff}};
const u16 *const data_020e4b84[4] = {data_020d1088, data_020d10b8, data_020d10e8, data_020d1118};
}
void SkyShotSequence::update() {
    static SkyShotActFn tbl[10] = {
        0,
        &SkyShotSequence::actShotFlight,
        &SkyShotSequence::actBalloonHit,
        &SkyShotSequence::actBalloonDrop,
        &SkyShotSequence::actUfoHit,
        &SkyShotSequence::actUfoFall,
        &SkyShotSequence::actUfoCrashed,
        &SkyShotSequence::actPeteHit,
        &SkyShotSequence::actPeteFall,
        &SkyShotSequence::actPeteFallen,
    };
    SkyShotActFn f = tbl[state];
    if (f) {
        (this->*f)();
    }
}

extern "C" void SkyShot_AddHit() {
    PlayerData *p = PlayerData_GetCurrent();
    u32 n = p->getSkyShotHits();
    if (n < 0x10) {
        p->setSkyShotHits(n + 1);
    }
}

extern "C" BOOL SkyShot_HasMaxHits() {
    return PlayerData_GetCurrent()->getSkyShotHits() >= 0x10;
}

void SkyShotSequence::actShotFlight() {
    timer--;
    if (timer <= 0) {
        endWatch(playerSlot);
        reset();
    }
}

void SkyShotSequence::actBalloonHit() {
    lookAtTarget();
}

void SkyShotSequence::actBalloonDrop() {
    relaxHead(0x190, 0x190);
    if (timer > 0) {
        timer--;
        if (timer == 0) {
            if (FieldItemFx_StartBalloonDrop(goldenBalloon != 0 ? 1 : 0, ((targetX + 0x800) >> 12) - 0x80) == 0) {
                balloonDropEnded = 1;
            }
        }
    }
    if (landSePending != 0 || splashSePending != 0) {
        s32 id = landSePending != 0 ? 0x7f9 : 0x7fa;
        VecFx32 v;
        SkySprite_MakeSoundPos(&v, targetX, targetY, targetScale);
        gSkySprites.se.requestSustained(0, id, &v);
        landSePending = 0;
        splashSePending = 0;
    }
    if (balloonDropEnded != 0) {
        Shared_PlayerActor_LocalEndWatch();
        reset();
    }
}

void SkyShotSequence::actUfoHit() {
    lookAtTarget();
}

void SkyShotSequence::actUfoFall() {
    relaxHead(0x50, 0);
    timer--;
    if (timer <= 0) {
        VecFx32 v;
        Camera_StartShake(0xa3d);
        SkySprite_MakeSoundPos(&v, targetX, targetY, targetScale);
        gSkySprites.se.requestSustained(0, 0x7fd, &v);
        timer = 0x32;
        state = 6;
    }
}

void SkyShotSequence::actUfoCrashed() {
    relaxHead(0x190, 0x190);
    timer--;
    if (timer <= 0) {
        Shared_PlayerActor_LocalEndWatch();
        Camera_RestorePrevMode();
        for (s32 i = 0; i < 4; i++) {
            PlayerData *p = (PlayerData *)PlayerData_GetResident(gSaveData.players, i);
            if (p && p->isUsed()) {
                ((PlayerDailyTalkFlags *)p->getDailyTalkFlags())->clear(0x14);
            }
        }
        GulliverQuest_Clear(gSaveData.gulliverQuest);
        EventAnnounce_Request(0x44, 0x63, 0);
        reset();
    }
}

void SkyShotSequence::actPeteHit() {
    lookAtTarget();
}

void SkyShotSequence::actPeteFall() {
    relaxHead(0x50, 0);
    if (timer > 0) {
        timer--;
    } else {
        VecFx32 v;
        Camera_StartShake(0x5c3);
        SkySprite_MakeSoundPos(&v, targetX, targetY, targetScale);
        gSkySprites.se.requestSustained(0, 0x800, &v);
        timer = 0x1e;
        state = 9;
    }
}

void SkyShotSequence::actPeteFallen() {
    relaxHead(0x190, 0x190);
    timer--;
    if (timer <= 0) {
        void* p = PlayerActor_GetBodyPos(playerSlot);
        if (p) {
            ((PeteFallState *)(TownSessionState_GetPeteFall(TownSessionState_Get())))->pickFallPos((VecFx32 *)p, fallDir);
        }
        Shared_PlayerActor_LocalEndWatch();
        Camera_RestorePrevMode();
        reset();
    }
}

void SkyShotSequence::reset() {
    state = 0;
    playerSlot = 4;
    timer = 0;
    targetX = 0;
    targetY = 0;
    targetScale = 0x1000;
    fallDir = 0;
    headPitch = 0;
    headYaw = 0;
    balloonDropEnded = 0;
    landSePending = 0;
    splashSePending = 0;
    goldenBalloon = 0;
}

void SkyShotSequence::lookAtTarget() {
    VecFx32 *p = (VecFx32 *)PlayerActor_GetBodyPos(4);
    s32 x = 0x80000;
    if (p) {
        Sky_ProjectToScreenX(&x, p);
    }
    s32 t = (FX_Div(targetX - x, 0x100000) * -10000) >> 12;
    if (t < -5000) {
        t = -5000;
    } else if (t > 5000) {
        t = 5000;
    }
    s16 v = t;
    PlayerActor_SetHeadTilt(4000, v, 4);
    headPitch = 4000;
    headYaw = v;
}

void SkyShotSequence::relaxHead(s32 a, s32 b) {
    Math_StepAngle(&headPitch, 0, a);
    Math_StepAngle(&headYaw, 0, b);
    PlayerActor_SetHeadTilt(headPitch, headYaw, 4);
}

SkySePlayer::SkySePlayer() {
    active = 0;
    for (FxVec3* p = positions; p < (FxVec3*)&active; p++) {
        p->x = 0;
        p->y = 0;
        p->z = 0;
    }
}

void SkySePlayer::resetAll() {
    for (SkySndChannel* e = channels; e < (SkySndChannel*)positions; e++) {
        e->callReset();
    }
    active = 1;
}

void SkySePlayer::releaseAll() {
    active = 0;
    for (SkySndChannel* e = &channels[7]; e >= channels; e--) {
        e->callRelease();
    }
}

void SkySePlayer::requestSustained(s32 idx, s32 a, VecFx32* p) {
    if (active) {
        channels[idx].callRequestSustained(a);
        positions[idx].x = p->x;
        positions[idx].y = p->y;
        positions[idx].z = p->z;
    }
}

void SkySePlayer::request(s32 idx, s32 a, VecFx32* p) {
    if (active) {
        channels[idx].callRequest(a);
        positions[idx].x = p->x;
        positions[idx].y = p->y;
        positions[idx].z = p->z;
    }
}

void SkySePlayer::update() {
    if (active) {
        SkySndChannel* a = channels;
        FxVec3* b = positions;
        for (s32 i = 0; i < 8; i++, a++, b++) {
            a->callUpdate(i == 7 ? NULL : b);
        }
    }
}

SkySprite::~SkySprite() {
    }

SkyShotRequest::~SkyShotRequest() {
    }

SkySprites::SkySprites() {
    spawnInterval = 0;
    spawnAccum = 0;
    precipIntensity = 0;
    snowCounter = 0;
    thunderTimer = 0;
    spriteCount = 0;
    balloonChance = 0;
    ufoChance = 0;
    ufoEventToday = 0;
    peteEventToday = 0;
    fireworksToday = 0;
    birdsRequested = 0;
    fireworksPattern = 0;
    fireworksPatternTime = 0;
    fireworksBigTimer = 0;
    fireworksSlotTimers[0] = 0;
    fireworksSlotTimers[1] = 0;
    fireworksSlotTimers[2] = 0;
    fireworksSlotTimers[3] = 0;
    fireworksInterval = 0;
    shootingStarDelay = 0;
    shootingStarVisible = 0;
    rainbowStrength = 0;
    rainStrength = 0;
    loader.init();
}


// ======== unk_020bc58c.cpp ========
void SkySprites::reloadAllGfx() {
    SkyObjGfxSlot *p, *end = &gfxSlots[5];
    for (p = &gfxSlots[0]; p < end; p++) {
        s32 id = p->gfxId;
        if (id != 0x34) {
            s32 r;
            s32 flag = p->loaded;
            r = palette.isLoaded(id);
            if (flag == 0) {
                SkyObjGfxLoader_LoadChars(&loader, id);
                SkyObjGfxSlot_Upload(p, &loader);
                p->loaded = 1;
                p->dirty = 0;
            }
            if (r == 0) {
                SkyObjPalette_Load(&palette, &loader, id);
            }
        }
    }
}

void SkySprites::loadDirtyGfx() {
    SkyObjGfxSlot *end = &gfxSlots[5], *p;
    for (p = &gfxSlots[0]; p < end; p++) {
        s32 id = p->gfxId;
        if (p->dirty != 0) {
            if (id == 0x34) {
                p->dirty = 0;
            } else if (p->loaded == 0) {
                SkyObjGfxLoader_LoadChars(&loader, id);
            }
        }
    }
}

void SkySprites::uploadDirtyGfx() {
    SkyObjGfxSlot *p, *end = &gfxSlots[5];
    for (p = &gfxSlots[0]; p < end; p++) {
        s32 id = p->gfxId;
        if (p->dirty != 0) {
            if (id != 0x34) {
                s32 r;
                s32 flag = p->loaded;
                r = palette.isLoaded(id);
                if (flag == 0) {
                    SkyObjGfxSlot_Upload(p, &loader);
                    p->loaded = 1;
                }
                if (r == 0) {
                    SkyObjPalette_Load(&palette, &loader, id);
                }
            }
            p->dirty = 0;
        }
    }
}

s32 SkySprites::pickGfxSlot(s32 t) {
    s32 grp;
    BOOL ok;
    grp = sSkyObjGfxTable[t].charLayout;
    ok = FALSE;
    if (grp == 6) {
        ok = TRUE;
    } else if (t == gfxSlots[grp].gfxId) {
        ok = TRUE;
    } else {
        BOOL c0 = gfxSlots[0].refCount > 0 ? 1 : 0;
        BOOL c1 = gfxSlots[1].refCount > 0 ? 1 : 0;
        BOOL c2 = gfxSlots[2].refCount > 0 ? 1 : 0;
        BOOL c3 = gfxSlots[3].refCount > 0 ? 1 : 0;
        BOOL c4 = gfxSlots[4].refCount > 0 ? 1 : 0;
        if (grp == 0) {
            ok = (c0 == 0 && c3 == 0) ? TRUE : FALSE;
        } else if (grp == 1) {
            ok = (c1 == 0 && c4 == 0) ? TRUE : FALSE;
        } else if (grp == 2) {
            ok = (c2 == 0 && c3 == 0 && c4 == 0) ? TRUE : FALSE;
        } else if (grp == 3) {
            ok = (c3 == 0 && c0 == 0 && c2 == 0 && c4 == 0) ? TRUE : FALSE;
        } else if (grp == 4) {
            ok = (c4 == 0 && c1 == 0 && c2 == 0 && c3 == 0) ? TRUE : FALSE;
        }
    }
    if (!ok) return 5;
    return grp;
}

void SkySprites::releaseConflictingSlots(s32 k) {
    if (k == 0) {
        gfxSlots[3].gfxId = 0x34;
        return;
    }
    if (k == 1) {
        gfxSlots[4].gfxId = 0x34;
        return;
    }
    if (k == 2) {
        gfxSlots[3].gfxId = 0x34;
        gfxSlots[4].gfxId = 0x34;
        return;
    }
    if (k == 3) {
        gfxSlots[0].gfxId = 0x34;
        gfxSlots[2].gfxId = 0x34;
        gfxSlots[4].gfxId = 0x34;
        return;
    }
    if (k == 4) {
        gfxSlots[1].gfxId = 0x34;
        gfxSlots[2].gfxId = 0x34;
        gfxSlots[3].gfxId = 0x34;
    }
}

s32 SkySprites::findFreeIndex(s32 kind, s32 idx) {
    s32 r = 0x3c;
    if (idx != 0x3c) {
        if (sprites[idx].kind == 0xd) r = idx;
    } else if (kind == 9) {
        SkySprite *p = &sprites[0x2e];
        s32 i;
        for (i = 0x2e; i <= 0x3a; i++, p++) {
            if (p->kind == 0xd) {
                r = i;
                break;
            }
        }
    } else if (spriteCount < 0x1e) {
        s32 i;
        SkySprite *p = &sprites[0];
        for (i = 0; i < 0x1e; p++, i++) {
            if (p->kind == 0xd) {
                r = i;
                break;
            }
        }
    }
    return r;
}

s32 SkySprites::findKind(s32 id) {
    s32 r = 0x3c;
    s32 i;
    SkySprite *p = sprites;
    for (i = 0; i < 0x3c; i++, p++) {
        if (id == p->kind) {
            r = i;
            break;
        }
    }
    return r;
}

void SkySprites::start() {
    checkTodayEvents();
    spawnInitialPrecip();
    updateMoon();
    updateRainbow();
    balloonChance = 0;
}

void SkySprites::stop() {
    killAll();
}

void SkySprites::checkTodayEvents() {
    ClockDateTimeWords a;
    ClockDateTimeWords b;
    ClockDateTimeWords c;
    Unk_020bcb04_Ent arr[7];
    Unk_020bcb04_Ent *p, *end;
    ufoEventToday = 0;
    peteEventToday = 0;
    fireworksToday = 0;
    a.words[0] = 0;
    a.words[1] = 0;
    Clock_GetDateTime(&a.dt);
    MI_CpuCopy8(&a, &b, 8);
    end = arr + EventSchedule_CollectDay(arr, &b, 0);
    for (p = arr; p < end; p++) {
        u32 id = p->id;
        BOOL ok;
        MI_CpuCopy8(&a, &c, 8);
        if (Event_GetState(id, &c, 0)) ok = TRUE; else ok = FALSE;
        if (ok) {
            if (id == 0x44) {
                ufoEventToday = 1;
            } else if (id == 0x45) {
                peteEventToday = 1;
            } else if (id == 0x13 || id == 0xf) {
                fireworksToday = 1;
            }
        }
    }
}

void SkySprites::onDayChange(BOOL a) {
    if (a) {
        gSaveData.clearFlag(9);
        checkTodayEvents();
        sendWishLetters();
    }
}

void SkySprites::onSlingshotFired(s32 i) {
    s32 j = 0;
    if (i < 4) j = i;
    VecFx32 *v = PlayerActor_GetBodyPos(i);
    if (v != NULL) {
        u16 buf[4];
        PlayerActor_GetSlotHeldItem(buf, i);
        s32 flag = 0;
        if (buf[0] >= 0x137b && buf[0] <= 0x137b) flag = 1;
        (getShotRequest(j))->set(i, (s32 *)(v), flag);
        shotSeq.startShot(i);
    } else {
        shotSeq.endWatch(i);
    }
}

SkyShotRequest *SkySprites::getShotRequest(s32 i) {
    return &shotRequests[i];
}

void SkySprites::sendWishLetters() {
    s32 i;
    for (i = 0; i < 4; i++) {
        PlayerData *o = (PlayerData *)PlayerData_GetResident(gSaveData.players, i);
        if (o != NULL && o->testFlag(0x32) != 0) {
            Letter ctx;
            StarWishLetterVars l;
            l.msgVariant = Random_GlobalBelow(3);
            Letter_ComposeFromMail((LetterView *)&ctx, &l.msgVariant, "ev_star", data_020e4634, data_020e4630, o->getPlayerId());
            ItemPickSpec q(0, 4);
            ItemPick_One(&l.present, &q, NULL, NULL, 1, 1, NULL);
            ((LetterView *)(&ctx))->setPresent(l.present, 1);
            if (LetterDelivery_PutInAddresseeMailbox(&ctx) != 0) {
                o->clearFlag(0x32);
            }
        }
    }
}

void SkySprites::initClock() {
    Clock_GetMinuteHour(&time.minute);
    second = Clock_GetSecond();
    prevMinuteHour = minuteHour;
    prevSecond = second;
}

void SkySprites::tickClock() {
    prevMinuteHour = minuteHour;
    prevSecond = second;
    Clock_GetMinuteHour(&time.minute);
    second = Clock_GetSecond();
}

extern "C" s32 SkySprites_GetMoonPhaseGfx() {
    u8 buf[12];
    buf[2] = 1;
    buf[3] = 1;
    buf[4] = 0;
    buf[5] = 0;
    Clock_GetDate(buf + 6);
    Clock_GetMinuteHour(buf);
    s32 t = Date_DaysBetween(buf + 6, buf + 2);
    if (buf[1] < 12) t--;
    if (t < 0) t = 0;
    t = t * 100 + 0x974;
    t = ((t % 0xb89) << 12) / 100;
    t = FX_Div(t, 0x1d87b);
    t = (t * 0x1c - 0x800) >> 12;
    if (t < 0) t = 0x1b;
    if (t > 0x1b) t = 0x1b;
    return t + 3;
}

s32 SkySprites::getGfxIdForKind(s32 kind, s32 arg) {
    s32 r = sSkySpriteDefaultGfx[kind];
    if (kind == 4) {
        r = SkySprites_GetMoonPhaseGfx();
    } else if (kind == 7) {
        if (Random_GlobalBelow(2) != 0) r = 0x28;
    } else if (kind == 5) {
        if ((arg & 1) != 0) r = 0x21;
    } else if (kind == 9) {
        s32 v = (arg & 0xf) + 0x1d;
        if ((u32)(v - 0x1d) <= 1) {
            r = 0x2d;
        } else if (v == 0x21 || v == 0x24 || v == 0x27 || v == 0x2a) {
            r = 0x2b;
        } else {
            r = 0x2c;
        }
    } else if (kind == 2) {
        s32 v = arg & 0xf;
        if (v == 0) r = 0x2f;
        else if (v == 1) r = 0x31;
        else r = 0x30;
    }
    return r;
}

SkySprite *SkySprites::spawn(s32 kind, s32 idx, VecFx32 *vec, s32 arg) {
    s32 t = getGfxIdForKind(kind, arg);
    SkySprite *slot = NULL;
    s32 grp = pickGfxSlot(t);
    if (grp != 5) {
        s32 n = findFreeIndex(kind, idx);
        if (n != 0x3c) {
            if (grp != 6) {
                SkyObjGfxSlot_Acquire(&gfxSlots[grp], t, &palette);
                releaseConflictingSlots(grp);
            }
            slot = &sprites[n];
            (slot)->clear();
            sprites[n].kind = kind;
            slot->state = 1;
            slot->gfxSlot = grp;
            slot->gfxId = t;
            slot->palette = sSkyObjGfxTable[t].objPalette;
            if (vec != NULL) {
                slot->screenPos.x = vec->x;
                slot->screenPos.y = vec->y;
                slot->screenPos.z = vec->z;
            }
            (slot)->initByKind(arg);
            spriteCount++;
        }
    }
    return slot;
}

void SkySprites::killKind(s32 id) {
    SkySprite *p, *end = &sprites[0x3c];
    for (p = &sprites[0]; p < end; p++) {
        if (id == p->kind) {
            (p)->endByKind();
            spriteCount--;
        }
    }
}

void SkySprites::killAll() {
    SkySprite *p, *end = &sprites[0x3c];
    for (p = &sprites[0]; p < end; p++) {
        if (p->kind != 0xd) {
            (p)->endByKind();
            spriteCount--;
        }
    }
}

void SkySprites::spawnInitialPrecip() {
    BOOL a, b;
    s32 v = gWeatherManager.level;
    a = (v == 3);
    b = (v == 4);
    if (a || b) {
        s32 w = gWeatherManager.precipKind;
        s32 idx = 0xd;
        if (w == 1) idx = 0;
        else if (w == 2) idx = 1;
        precipIntensity = 0x6400;
        snowCounter = 0x32;
        if (idx != 0xd) {
            s32 n = a ? 15 : 20;
            s32 i;
            for (i = 0; i < n; i++) {
                spawn(idx, 0x3c, NULL, 1);
            }
        }
        if (a) rainStrength = 0x800;
        else rainStrength = 0x1000;
    } else {
        precipIntensity = 0;
        snowCounter = 0;
        rainStrength = 0;
    }
}

void SkySprites::updateLightning() {
    if (thunderTimer <= 0) thunderTimer = 0x32;
    thunderTimer--;
    if (thunderTimer <= 0) {
        if (Random_GlobalBelow(8) == 0) {
            spawn(8, 0x3b, NULL, 0);
        }
        SceneLights_StartFlash(0);
        thunderTimer = Random_GlobalBelow(200) + 10;
    }
}

void SkySprites::updateThunderFlash() {
    if (thunderTimer <= 0) thunderTimer = 0x32;
    thunderTimer--;
    if (thunderTimer <= 0) {
        SceneLights_StartFlash(0);
        thunderTimer = Random_GlobalBelow(200) + 10;
    }
}


// ======== unk_020bbc28.cpp ========
typedef void (SkySprites::*Unk_020bbeb8_Fn)();

void SkySprites::updateRain() {
    s32 kind = 0;
    s32 a = gWeatherManager.level;
    s32 b = gWeatherManager.targetLevel;
    if (a == b) {
        if (a == 3) {
            kind = 2;
        } else if (a == 4) {
            kind = 4;
        }
    } else if (a > b) {
        if (b == 3) {
            kind = 3;
        } else if (b <= 3) {
            kind = 1;
        }
    } else {
        if (b == 3) {
            kind = 1;
        } else if (b > 3) {
            kind = 3;
        }
    }
    if (kind == 0) {
        precipIntensity = 0;
    } else {
        const SkySpawnRate *t = &sRainSpawnRates[kind - 1];
        if (spawnInterval == 0) {
            spawnInterval = t->baseInterval + Random_GlobalBelow(t->randomInterval);
        }
        if (b >= 3) {
            precipIntensity += 2;
            if (precipIntensity > 0x6400) {
                precipIntensity = 0x6400;
            }
        } else {
            precipIntensity -= 2;
            if (precipIntensity < 0) {
                precipIntensity = 0;
            }
        }
        spawnAccum += precipIntensity >> 8;
        while (spawnAccum >= spawnInterval) {
            spawn(0, 0x3c, 0, 0);
            spawnAccum -= spawnInterval;
            if (spawnAccum <= spawnInterval) {
                spawnInterval = t->baseInterval + Random_GlobalBelow(t->randomInterval);
                if (spawnInterval < 0x23) {
                    spawnInterval = 0x23;
                }
                spawnAccum = 0;
                break;
            }
        }
    }
    if (kind == 4) {
        updateLightning();
    }
    Math_StepS32Alt(&rainStrength, b == 4 ? 0x1000 : 0x800, 2);
}

void SkySprites::updateSnow() {
    s32 kind = 0;
    s32 a = gWeatherManager.level;
    s32 b = gWeatherManager.targetLevel;
    if (a == b) {
        if (a == 3) {
            kind = 2;
        } else if (a == 4) {
            kind = 4;
        }
    } else if (a > b) {
        if (b == 3) {
            kind = 3;
        } else if (b <= 3) {
            kind = 1;
        }
    } else {
        if (b == 3) {
            kind = 1;
        } else if (b > 3) {
            kind = 3;
        }
    }
    if (kind == 0) {
        spawnInterval = 0;
        snowCounter = 0;
    } else {
        const SkySpawnRate *t = &sSnowSpawnRates[kind - 1];
        if (spawnInterval == 0) {
            spawnInterval = 0x3e8;
        }
        if (snowCounter > 2) {
            if (b >= 3) {
                precipIntensity += 2;
                if (precipIntensity > 0x6400) {
                    precipIntensity = 0x6400;
                }
            } else {
                precipIntensity -= 2;
                if (precipIntensity < 0) {
                    precipIntensity = 0;
                }
            }
        } else {
            if (b >= 3) {
                precipIntensity = 0x6400;
            } else {
                precipIntensity -= 2;
                if (precipIntensity < 0) {
                    precipIntensity = 0;
                }
            }
        }
        spawnAccum += precipIntensity >> 8;
        while (spawnAccum >= spawnInterval) {
            spawn(1, 0x3c, 0, 0);
            spawnAccum -= spawnInterval;
            if (spawnAccum <= spawnInterval) {
                switch (snowCounter) {
                case 0:
                    spawnInterval = 0x2af8;
                    spawnAccum = 0;
                    break;
                case 1:
                    spawnInterval = 0x2328;
                    spawnAccum = 0;
                    break;
                default:
                    spawnInterval = t->baseInterval + Random_GlobalBelow(t->randomInterval);
                    if (snowCounter < 0x32) {
                        spawnAccum = 0x4b;
                    } else {
                        spawnAccum = 0;
                    }
                    break;
                }
                snowCounter++;
                if (snowCounter > 0x32) {
                    snowCounter = 0x32;
                }
                break;
            }
        }
    }
}

void SkySprites::updateShootingStar() {
    u8 v = time.hour;
    BOOL a = TRUE;
    if (v < 0x13 && v >= 4) {
        a = FALSE;
    }
    s32 x = gWeatherManager.level;
    BOOL c = FALSE;
    if (x != 3 && x != 4) {
        c = TRUE;
    }
    BOOL d = FALSE;
    if (fireworksPattern == 0 && findKind(9) == 0x3c) {
        d = TRUE;
    }
    if (a && c && d) {
        if (shootingStarDelay > 0) {
            shootingStarDelay--;
            if (shootingStarDelay == 0) {
                spawn(3, 0x1e, 0, 0);
            }
        } else if (second == 0x1e && second != prevSecond) {
            if (!Random_GlobalBelow((&gSaveData.weather)->todayPattern == 0 ? 4 : 0x100)) {
                shootingStarDelay = Random_GlobalBelow(0x258);
            }
        }
    } else {
        shootingStarDelay = 0;
    }
}

void SkySprites::updateMoon() {
    u8 v = time.hour;
    BOOL b;
    if (sprites[31].kind == 4) {
        b = TRUE;
    } else {
        b = FALSE;
    }
    if (v >= 0x13 || v < 4) {
        if (!b) {
            spawn(4, 0x1f, 0, 0);
        }
    } else if (b) {
        killKind(4);
    }
}

extern "C" {
SpriteAnimFrame data_020e510c[3] = {
    {data_020e4c24, 2, 0, 0},
    {data_020e4c34, 1, 0, 0},
    {data_020e4bf4, 1, 0, 0},
};
SpriteAnimFrame data_020e4a88[1] = {
    {data_020e470c, 1, 0, 0},
};
GXOamAttr data_020e46dc[1] = {{0x1fc00fc, 0x40, 0, 0x0, 0xffff}};
GXOamAttr data_020e46fc[1] = {{0x81f000f0, 0x9c, 0, 0x2, 0xffff}};
GXOamAttr data_020e4ba4[2] = {{0xa1f88000, 0x9e, 0, 0x0, 0x0}, {0xa1f880e0, 0x11e, 0, 0x0, 0xffff}};
SpriteAnimFrame data_020e4aa0[1] = {
    {data_020e48c4, 1, 0, 0},
};
SpriteAnimFrame data_020e4aac[1] = {
    {data_020e4a0c, 1, 0, 0},
};
SpriteAnimFrame data_020e5130[3] = {
    {data_020e4c44, 2, 0, 0},
    {data_020e4c54, 1, 0, 0},
    {data_020e4c64, 1, 0, 0},
};
char data_020e4ec8[30] = "/sky/d_2d_b_cld_c1_bg_nsc.bin";
const u16 data_020d10e8[24] = {
    0x48a5, 0x48a5, 0x48a5, 0x48a5, 0x48a5, 0x48c6, 0x48e7, 0x4d29, 0x4d4a, 0x4d4a, 0x4d4a, 0x4d4a,
    0x4d4a, 0x4d4a, 0x4d4a, 0x4d4a, 0x4d4a, 0x4d29, 0x4ce7, 0x4ca5, 0x48a5, 0x48a5, 0x48a5, 0x48a5,
};
GXOamAttr data_020e4bb4[2] = {{0x41fc80e0, 0x9d, 0, 0x0, 0x0}, {0x41fc8000, 0x11d, 0, 0x0, 0xffff}};
const u16 data_020d1118[24] = {
    0x6eb1, 0x72d1, 0x72d2, 0x76f4, 0x7af5, 0x7ef5, 0x7f14, 0x7f56, 0x7fb9, 0x7fb9, 0x7fb9, 0x7fb9,
    0x7fb9, 0x7fb9, 0x7fb9, 0x7fb9, 0x7fb9, 0x7b39, 0x76b6, 0x76f6, 0x7716, 0x76f5, 0x72f4, 0x6eb2,
};
GXOamAttr data_020e4bc4[2] = {{0x1400e7, 0xf6, 0, 0x3, 0x0}, {0x1e70000, 0xb7, 0, 0x3, 0xffff}};
const u8 data_020d0ef8[24] = {
    0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0x0, 0x13, 0x13, 0x14, 0x14, 0x15,
    0x15, 0x16, 0x16, 0x16, 0x15, 0x13, 0x0, 0xb, 0xb, 0xb, 0xb, 0xb,
};
SpriteAnimFrame data_020e5154[3] = {
    {data_020e4ba4, 2, 0, 0},
    {data_020e4c74, 1, 0, 0},
    {data_020e4c84, 1, 0, 0},
};
GXOamAttr data_020e4bd4[2] = {{0x1600e7, 0xd4, 0, 0x3, 0x0}, {0x1e90002, 0xf6, 0, 0x3, 0xffff}};
char *data_020e5230[10] = {data_020e4e48, data_020e4e48, data_020e4e68,
    data_020e4e88, data_020e4ea8, data_020e4ec8, data_020e4ee8,
    data_020e4f08, data_020e4f28, data_020e4f48};
GXOamAttr data_020e4a24[1] = {{0x81f000f0, 0x118, 0, 0x2, 0xffff}};
GXOamAttr data_020e4be4[2] = {{0x1800e7, 0xf7, 0, 0x3, 0x0}, {0x1eb0004, 0xd4, 0, 0x3, 0xffff}};
char **sCloudScreenFiles[3] = {data_020e5230, data_020e4d98, data_020e4d70};
const u16 *const *const sSkyLightColorTables[5] = {data_020e4b74, data_020e4b74, data_020e4b84, data_020e4b84, data_020e4b84};
GXOamAttr data_020e488c[1] = {{0x0, 0xb4, 0, 0x0, 0xffff}};
GXOamAttr data_020e55c0[10] = {{0x300200e8, 0x94, 0, 0x3, 0x0}, {0x201000fe, 0xb6, 0, 0x3, 0x0}, {0x10000009, 0xb6, 0, 0x3, 0x0}, {0x1eb00fa, 0xb6, 0, 0x3, 0x0}, {0xc00f4, 0xb6, 0, 0x3, 0x0}, {0x1f800eb, 0x95, 0, 0x3, 0x0}, {0x300f000c, 0xb4, 0, 0x3, 0x0}, {0x200f00ea, 0xd7, 0, 0x3, 0x0}, {0x1f200f1, 0xd7, 0, 0x3, 0x0}, {0x1ed0004, 0x97, 0, 0x3, 0xffff}};
const u8 data_020d0f10[24] = {
    0xc, 0xc, 0xc, 0xc, 0xa, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0xa, 0xc, 0xc, 0xc,
};
char data_020e5048[31] = "/sky/d_2d_b_cld_r_b_bg_nsc.bin";
s32 sSkyLineScrollY[2];
const s32 sSkySpriteDefaultGfx[13] = {0x0, 0x1, 0x2f, 0x2, 0x3, 0x1f, 0x23, 0x26, 0x2a, 0x2b, 0x2e, 0x32, 0x33};
char data_020e4df4[27] = "/sky/a_sky_fly_obj_ncg.bin";
GXOamAttr data_020e4bf4[2] = {{0x51fc80e0, 0x9c, 0, 0x0, 0x0}, {0x51fc8000, 0x11c, 0, 0x0, 0xffff}};
GXOamAttr data_020e482c[1] = {{0x81f004f0, 0x94, 0, 0x0, 0xffff}};
GXOamAttr data_020e4684[1] = {{0x81f000f0, 0x9c, 0, 0x4, 0xffff}};
GXOamAttr data_020e4c04[2] = {{0x81f880e0, 0x9e, 0, 0x0, 0x0}, {0x81f88000, 0x11e, 0, 0x0, 0xffff}};
GXOamAttr data_020e4674[1] = {{0x81f000f0, 0x118, 0, 0x3, 0xffff}};
char data_020e4ee8[30] = "/sky/d_2d_b_cld_r0_bg_nsc.bin";
GXOamAttr data_020e49a4[1] = {{0x81f000f0, 0x98, 0, 0x3, 0xffff}};
GXOamAttr data_020e484c[1] = {{0x81e003e0, 0x118, 0, 0x9, 0xffff}};
char data_020e4f28[30] = "/sky/d_2d_b_cld_h0_bg_nsc.bin";
SpriteAnimFrame data_020e519c[3] = {
    {data_020e4cc4, 2, 0, 0},
    {data_020e4cd4, 1, 0, 0},
    {data_020e4b54, 1, 0, 0},
};
GXOamAttr data_020e4c44[2] = {{0xb1f88000, 0x9e, 0, 0x0, 0x0}, {0xb1f880e0, 0x11e, 0, 0x0, 0xffff}};
const SkySpawnRate sRainSpawnRates[4] = {{0x2d, 0x69}, {0x1e, 0x5a}, {0xf, 0x4b}, {0x1, 0x3c}};
GXOamAttr data_020e47fc[1] = {{0x91f000f0, 0x11c, 0, 0x2, 0xffff}};
SpriteAnimFrame data_020e51c0[3] = {
    {data_020e4ce4, 2, 0, 0},
    {data_020e4cf4, 1, 0, 0},
    {data_020e4d04, 1, 0, 0},
};
u16 sSkyBlendLineCache[2] = {0xffff, 0xffff};  // (the last light parameter of each line buffer)
GXOamAttr data_020e4c74[2] = {{0x61fc8000, 0x9d, 0, 0x0, 0x0}, {0x61fc80e0, 0x11d, 0, 0x0, 0xffff}};
GXOamAttr data_020e4c84[2] = {{0x61fc8000, 0x9c, 0, 0x0, 0x0}, {0x61fc80e0, 0x11c, 0, 0x0, 0xffff}};
SpriteAnimFrame data_020e4af4[1] = {
    {data_020e485c, 4, 0, 0},
};
SpriteAnimFrame data_020e4b00[1] = {
    {data_020e487c, 4, 0, 0},
};
SpriteAnimFrame data_020e4ddc[2] = {
    {data_020e4a3c, 2, 0, 0},
    {data_020e483c, 1, 0, 0},
};
GXOamAttr data_020e5a34[24] = {{0x800004d0, 0x114, 0, 0x1, 0x0}, {0x802084d0, 0x118, 0, 0x1, 0x0}, {0x3044d8, 0x13a, 0, 0x1, 0x0}, {0x403004e0, 0x15a, 0, 0x1, 0x0}, {0x802004f0, 0x98, 0, 0x1, 0x0}, {0x1804f0, 0x9f, 0, 0x1, 0x0}, {0x80400410, 0x11c, 0, 0x1, 0x0}, {0x380410, 0x11b, 0, 0x1, 0x0}, {0x80404400, 0xdc, 0, 0x1, 0x0}, {0x404004f0, 0x9c, 0, 0x1, 0x0}, {0x4004e8, 0xbf, 0, 0x1, 0x0}, {0x91e004d0, 0x114, 0, 0x1, 0x0}, {0x91d084d0, 0x118, 0, 0x1, 0x0}, {0x11c044d8, 0x13a, 0, 0x1, 0x0}, {0x51c004e0, 0x15a, 0, 0x1, 0x0}, {0x91c004f0, 0x98, 0, 0x1, 0x0}, {0x11e004f0, 0x9f, 0, 0x1, 0x0}, {0x91a00410, 0x11c, 0, 0x1, 0x0}, {0x11c00410, 0x11b, 0, 0x1, 0x0}, {0x91a04400, 0xdc, 0, 0x1, 0x0}, {0x51b004f0, 0x9c, 0, 0x1, 0x0}, {0x11b804e8, 0xbf, 0, 0x1, 0x0}, {0x5004f8, 0xbe, 0, 0x1, 0x0}, {0x11a804f8, 0xbe, 0, 0x1, 0xffff}};
SkyGradient sSkyGradient;
GXOamAttr data_020e4a1c[1] = {{0x81f000f0, 0x9c, 0, 0x3, 0xffff}};
s32 sSkyLineScrollX[2];
GXOamAttr data_020e4814[1] = {{0x81f000f0, 0x98, 0, 0x4, 0xffff}};
GXOamAttr data_020e4ce4[2] = {{0xa1f88000, 0x9a, 0, 0x0, 0x0}, {0xa1f880e0, 0x11a, 0, 0x0, 0xffff}};
SpriteAnimFrame data_020e4b18[1] = {
    {data_020e4854, 4, 0, 0},
};
GXOamAttr data_020e4d04[2] = {{0x61fc8000, 0x98, 0, 0x0, 0x0}, {0x61fc80e0, 0x118, 0, 0x0, 0xffff}};
u8 data_020e4630[4] = {0x3d};
const u8 *const *const sSkyLightParamTables[5] = {data_020e478c, data_020e478c, data_020e4824, data_020e4824, data_020e4824};
const u8 sSkyCloudBgLayer[4] = {
    0x6, 0x2, 0x0, 0x0,
};
SpriteAnimFrame data_020e51e4[3] = {
    {data_020e4b94, 2, 0, 0},
    {data_020e4d14, 1, 0, 0},
    {data_020e4d24, 1, 0, 0},
};
char data_020e50a8[31] = "/sky/d_2d_b_cld_c_a_bg_nsc.bin";
SkySprites gSkySprites;
GXOamAttr data_020e499c[1] = {{0x91f000f0, 0x118, 0, 0x2, 0xffff}};
SpriteAnimFrame data_020e4b48[1] = {
    {data_020e4734, 4, 0, 0},
};
const char *const sSkyObjCharFiles[3] = {data_020e4dac, data_020e4df4, 0};
GXOamAttr data_020e477c[1] = {{0x81f000f0, 0x11c, 0, 0x3, 0xffff}};
GXOamAttr data_020e4774[1] = {{0x1fc00fc, 0x57, 0, 0x0, 0xffff}};
GXOamAttr data_020e492c[1] = {{0x91f000f0, 0x118, 0, 0x2, 0xffff}};
const u8 data_020d0ec8[24] = {
    0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x0, 0x18, 0x18, 0x19, 0x19, 0x1a,
    0x1a, 0x1b, 0x1b, 0x1b, 0x1a, 0x18, 0x0, 0xe, 0xe, 0xe, 0xe, 0xe,
};
const FireworkColorPair sFireworkColors[4] = {
    {{0, 26, 0, 0}, {27, 16, 0, 0}},
    {{27, 16, 0, 0}, {0, 26, 0, 0}},
    {{4, 17, 31, 0}, {27, 3, 31, 0}},
    {{27, 3, 31, 0}, {31, 8, 8, 0}},
};
GXOamAttr data_020e486c[1] = {{0x81f000f0, 0x118, 0, 0x3, 0xffff}};
GXOamAttr data_020e4a14[1] = {{0x81f880f0, 0x9a, 0, 0x4, 0xffff}};
const u16 data_020d0fc8[24] = {
    0x0, 0x400, 0x820, 0xc20, 0x1440, 0x3e0a, 0x67f4, 0x67fb, 0x67ff, 0x67ff, 0x67ff, 0x67ff,
    0x67ff, 0x5bff, 0x4bdf, 0x3fdf, 0x33df, 0x1abf, 0x21f, 0x16d, 0xa5, 0x63, 0x42, 0x21,
};
char data_020e4dac[23] = "/sky/a_sky_obj_ncg.bin";
const s32 sSnowFallSpeeds[3] = {0x2402, 0x123a, 0x17e2};
GXOamAttr data_020e5520[10] = {{0x300000ed, 0x94, 0, 0x3, 0x0}, {0x200a00fc, 0xb6, 0, 0x3, 0x0}, {0x10010007, 0xb6, 0, 0x3, 0x0}, {0x1ee00fa, 0xb6, 0, 0x3, 0x0}, {0x900f3, 0xb6, 0, 0x3, 0x0}, {0x1f900ee, 0x95, 0, 0x3, 0x0}, {0x300b0008, 0xb4, 0, 0x3, 0x0}, {0x100c00ed, 0x97, 0, 0x3, 0x0}, {0x1f500f3, 0xb4, 0, 0x3, 0x0}, {0x1ef0002, 0x97, 0, 0x3, 0xffff}};
GXOamAttr data_020e49f4[1] = {{0x81e003e0, 0x11c, 0, 0x9, 0xffff}};
const s32 sParticleTrailScales[23] = {0x5000, 0x16db, 0x1000, 0xe8c, 0xd55, 0xc4f, 0xc4f, 0xc4f, 0xc4f, 0xc4f, 0xc4f, 0xd55, 0xe8c, 0x1000, 0x11c7, 0x1400, 0x16db, 0x1aab, 0x2000, 0x2800, 0x3555, 0x5000, 0xa000};
GXOamAttr data_020e48f4[1] = {{0x81f880f0, 0x9c, 0, 0x4, 0xffff}};
char data_020e5028[31] = "/sky/d_2d_b_cld_c_b_bg_nsc.bin";
GXOamAttr data_020e4734[1] = {{0x81f000f0, 0x114, 0, 0x0, 0xffff}};
SpriteAnimFrame data_020e56b0[7] = {
    {data_020e48d4, 2, 0, 0},
    {data_020e46fc, 2, 0, 0},
    {data_020e475c, 2, 0, 0},
    {data_020e4844, 2, 0, 0},
    {data_020e47fc, 2, 0, 0},
    {data_020e492c, 2, 0, 0},
    {data_020e491c, 2, 0, 0},
};
u32 sSkyBlackBackdrop[1];
GXOamAttr data_020e5570[10] = {{0x300100ea, 0x94, 0, 0x3, 0x0}, {0x200d00fd, 0xb6, 0, 0x3, 0x0}, {0x10000008, 0xb6, 0, 0x3, 0x0}, {0x1ec00fa, 0xb6, 0, 0x3, 0x0}, {0xb00f3, 0xb6, 0, 0x3, 0x0}, {0x1f800ec, 0x95, 0, 0x3, 0x0}, {0x300e000b, 0xb4, 0, 0x3, 0x0}, {0x100e00eb, 0x97, 0, 0x3, 0x0}, {0x1f300f2, 0xd7, 0, 0x3, 0x0}, {0x1ed0003, 0x97, 0, 0x3, 0xffff}};
SpriteAnimFrame data_020e5d04[44] = {
    {NULL, 2, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 1},
    {data_020e49ec, 1, 0, 2},
    {data_020e49ec, 1, 0, 2},
    {data_020e49ec, 1, 0, 3},
    {data_020e49ec, 1, 0, 3},
    {data_020e49ec, 1, 0, 4},
    {data_020e49ec, 1, 0, 4},
    {data_020e49ec, 1, 0, 5},
    {data_020e49ec, 1, 0, 5},
    {data_020e49ec, 1, 0, 6},
    {data_020e49ec, 1, 0, 6},
    {data_020e49ec, 1, 0, 6},
    {data_020e49ec, 1, 0, 7},
    {data_020e49ec, 1, 0, 7},
    {data_020e49ec, 1, 0, 8},
    {data_020e49ec, 1, 0, 8},
    {data_020e49ec, 1, 0, 9},
    {data_020e49ec, 1, 0, 9},
    {data_020e49ec, 1, 0, 10},
    {data_020e49ec, 1, 0, 10},
    {data_020e49ec, 1, 0, 11},
    {data_020e49ec, 1, 0, 11},
    {data_020e49ec, 1, 0, 11},
    {data_020e49ec, 1, 0, 12},
    {data_020e49ec, 1, 0, 12},
    {data_020e49ec, 1, 0, 12},
    {data_020e49ec, 1, 0, 13},
    {data_020e49ec, 1, 0, 13},
    {data_020e49ec, 1, 0, 13},
    {data_020e49ec, 1, 0, 14},
    {data_020e49ec, 1, 0, 14},
    {data_020e49ec, 1, 0, 14},
    {data_020e49ec, 1, 0, 14},
    {data_020e49ec, 1, 0, 15},
    {data_020e49ec, 1, 0, 15},
    {data_020e49ec, 1, 0, 15},
    {data_020e49ec, 1, 0, 15},
};
const u8 data_020d0ee0[24] = {
    0x10, 0x10, 0x10, 0x10, 0xd, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0xd, 0x10, 0x10, 0x10,
};
s32 sRainSideToggle = 1;
const u16 data_020d10b8[24] = {
    0x3ed0, 0x3ed0, 0x3ed0, 0x3ed0, 0x3ed0, 0x31e9, 0x24e2, 0x1060, 0x820, 0x420, 0x400, 0x0,
    0x0, 0x400, 0x800, 0xc01, 0x1001, 0x1801, 0x2443, 0x31ea, 0x3ed0, 0x3ed0, 0x3ed0, 0x3ed0,
};
GXOamAttr data_020e465c[1] = {{0x81f000f0, 0x11c, 0, 0x3, 0xffff}};
SpriteAnimFrame data_020e57d0[9] = {
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 0},
    {data_020e49ec, 1, 0, 2},
    {NULL, 37, 0, 0},
};
GXOamAttr data_020e48bc[1] = {{0x81f000f0, 0x9c, 0, 0x2, 0xffff}};
SpriteAnimFrame data_020e4ab8[1] = {
    {data_020e47b4, 1, 0, 0},
};
char data_020e4f08[30] = "/sky/d_2d_b_cld_r1_bg_nsc.bin";
GXOamAttr data_020e4884[1] = {{0x81f000f0, 0x98, 0, 0x2, 0xffff}};
HBlankTask sSkyHBlankTask;
GXOamAttr data_020e4c14[2] = {{0x41fc80e0, 0x9c, 0, 0x0, 0x0}, {0x41fc8000, 0x11c, 0, 0x0, 0xffff}};
GXOamAttr data_020e466c[1] = {{0x81f000f0, 0x9c, 0, 0x2, 0xffff}};
SpriteAnimFrame data_020e583c[9] = {
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 2},
    {NULL, 37, 0, 0},
};
char data_020e4f48[30] = "/sky/d_2d_b_cld_h1_bg_nsc.bin";
GXOamAttr data_020e49cc[1] = {{0x81f000f0, 0x98, 0, 0x3, 0xffff}};
GXOamAttr data_020e4c64[2] = {{0x71fc8000, 0x9c, 0, 0x0, 0x0}, {0x71fc80e0, 0x11c, 0, 0x0, 0xffff}};
SpriteAnimFrame data_020e5288[4] = {
    {data_020e4884, 2, 0, 0},
    {data_020e48bc, 2, 0, 0},
    {data_020e4a24, 2, 0, 0},
    {data_020e497c, 2, 0, 0},
};
GXOamAttr data_020e46ec[1] = {{0x1fc00fc, 0x58, 0, 0x0, 0xffff}};
char *sSkyPaletteFiles[2][2] = {{data_020e4f68, data_020e4f88}, {data_020e4fa8, data_020e4fc8}};
GXOamAttr data_020e49fc[1] = {{0x81e003e0, 0x9c, 0, 0x9, 0xffff}};
GXOamAttr data_020e4a0c[1] = {{0x41fb80f0, 0x96, 0, 0x9, 0xffff}};
SpriteAnimFrame data_020e6334[44] = {
    {NULL, 2, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 1},
    {data_020e49fc, 1, 0, 2},
    {data_020e49fc, 1, 0, 2},
    {data_020e49fc, 1, 0, 3},
    {data_020e49fc, 1, 0, 3},
    {data_020e49fc, 1, 0, 4},
    {data_020e49fc, 1, 0, 4},
    {data_020e49fc, 1, 0, 5},
    {data_020e49fc, 1, 0, 5},
    {data_020e49fc, 1, 0, 6},
    {data_020e49fc, 1, 0, 6},
    {data_020e49fc, 1, 0, 6},
    {data_020e49fc, 1, 0, 7},
    {data_020e49fc, 1, 0, 7},
    {data_020e49fc, 1, 0, 8},
    {data_020e49fc, 1, 0, 8},
    {data_020e49fc, 1, 0, 9},
    {data_020e49fc, 1, 0, 9},
    {data_020e49fc, 1, 0, 10},
    {data_020e49fc, 1, 0, 10},
    {data_020e49fc, 1, 0, 11},
    {data_020e49fc, 1, 0, 11},
    {data_020e49fc, 1, 0, 11},
    {data_020e49fc, 1, 0, 12},
    {data_020e49fc, 1, 0, 12},
    {data_020e49fc, 1, 0, 12},
    {data_020e49fc, 1, 0, 13},
    {data_020e49fc, 1, 0, 13},
    {data_020e49fc, 1, 0, 13},
    {data_020e49fc, 1, 0, 14},
    {data_020e49fc, 1, 0, 14},
    {data_020e49fc, 1, 0, 14},
    {data_020e49fc, 1, 0, 14},
    {data_020e49fc, 1, 0, 15},
    {data_020e49fc, 1, 0, 15},
    {data_020e49fc, 1, 0, 15},
    {data_020e49fc, 1, 0, 15},
};
GXOamAttr data_020e48cc[1] = {{0xc1c003c0, 0x98, 0, 0x9, 0xffff}};
GXOamAttr data_020e475c[1] = {{0x81f000f0, 0x118, 0, 0x2, 0xffff}};
SpriteAnimFrame data_020e5318[4] = {
    {data_020e4814, 1, 0, 0},
    {data_020e4684, 1, 0, 0},
    {data_020e47f4, 1, 0, 0},
    {data_020e47ec, 1, 0, 0},
};
SpriteAnimFrame data_020e5348[4] = {
    {data_020e49cc, 2, 0, 0},
    {data_020e4644, 2, 0, 0},
    {data_020e4674, 2, 0, 0},
    {data_020e465c, 2, 0, 0},
};
SpriteAnimFrame data_020e5914[9] = {
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 0},
    {data_020e49fc, 1, 0, 2},
    {NULL, 37, 0, 0},
};
const u8 *const data_020e4824[2] = {data_020d0ef8, data_020d0f10};
SpriteAnimFrame data_020e5764[9] = {
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 2},
    {NULL, 37, 0, 0},
};
char data_020e4fe8[31] = "/sky/d_2d_b_cld_b_b_bg_nsc.bin";
SpriteAnimFrame data_020e5af4[44] = {
    {NULL, 2, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 0},
    {data_020e48cc, 1, 0, 1},
    {data_020e48cc, 1, 0, 2},
    {data_020e48cc, 1, 0, 2},
    {data_020e48cc, 1, 0, 3},
    {data_020e48cc, 1, 0, 3},
    {data_020e48cc, 1, 0, 4},
    {data_020e48cc, 1, 0, 4},
    {data_020e48cc, 1, 0, 5},
    {data_020e48cc, 1, 0, 5},
    {data_020e48cc, 1, 0, 6},
    {data_020e48cc, 1, 0, 6},
    {data_020e48cc, 1, 0, 6},
    {data_020e48cc, 1, 0, 7},
    {data_020e48cc, 1, 0, 7},
    {data_020e48cc, 1, 0, 8},
    {data_020e48cc, 1, 0, 8},
    {data_020e48cc, 1, 0, 9},
    {data_020e48cc, 1, 0, 9},
    {data_020e48cc, 1, 0, 10},
    {data_020e48cc, 1, 0, 10},
    {data_020e48cc, 1, 0, 11},
    {data_020e48cc, 1, 0, 11},
    {data_020e48cc, 1, 0, 11},
    {data_020e48cc, 1, 0, 12},
    {data_020e48cc, 1, 0, 12},
    {data_020e48cc, 1, 0, 12},
    {data_020e48cc, 1, 0, 13},
    {data_020e48cc, 1, 0, 13},
    {data_020e48cc, 1, 0, 13},
    {data_020e48cc, 1, 0, 14},
    {data_020e48cc, 1, 0, 14},
    {data_020e48cc, 1, 0, 14},
    {data_020e48cc, 1, 0, 14},
    {data_020e48cc, 1, 0, 15},
    {data_020e48cc, 1, 0, 15},
    {data_020e48cc, 1, 0, 15},
    {data_020e48cc, 1, 0, 15},
};
GXOamAttr data_020e4b54[2] = {{0x51fc80e0, 0x98, 0, 0x0, 0x0}, {0x51fc8000, 0x118, 0, 0x0, 0xffff}};
const u8 sFireworksPatternDelays[20] = {
    0x0, 0x0, 0x78, 0xfa, 0x28, 0x78, 0xfa, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x28, 0x0,
    0x0, 0x28, 0x0, 0x0,
};
const u16 sSkyObjPalDefaultColor = 0;
GXOamAttr data_020e47c4[1] = {{0x41fb80f0, 0x94, 0, 0x9, 0xffff}};
const u16 data_020d1058[24] = {
    0x6731, 0x6b51, 0x6f51, 0x7351, 0x7792, 0x7fb4, 0x7ff5, 0x7ffc, 0x7fff, 0x7fff, 0x7fff, 0x7fff,
    0x7fff, 0x7fff, 0x77df, 0x6fdf, 0x6bdf, 0x6f7f, 0x66ff, 0x5ef5, 0x6f97, 0x6f75, 0x6f74, 0x6b52,
};
SpriteAnimFrame data_020e4a7c[1] = {
    {data_020e49d4, 1, 0, 0},
};
const u16 data_020d1088[24] = {
    0x0, 0x1001, 0x2022, 0x3023, 0x3c44, 0x48a8, 0x554c, 0x5dee, 0x668f, 0x668f, 0x668f, 0x668f,
    0x668f, 0x668f, 0x668f, 0x668f, 0x668f, 0x5a2f, 0x4d8c, 0x4508, 0x3cc6, 0x2c63, 0x1c42, 0xc21,
};
GXOamAttr data_020e498c[1] = {{0x41f003f0, 0x10, 2, 0x0, 0xffff}};
GXOamAttr data_020e489c[1] = {{0x81f880f0, 0x9c, 0, 0x4, 0xffff}};
char *sCloudCharFiles[5] = {data_020e4e10, data_020e4e10, data_020e4e10,
    data_020e4e10, data_020e4e10};
SpriteAnimFrame data_020e5f14[44] = {
    {NULL, 2, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 0},
    {data_020e484c, 1, 0, 1},
    {data_020e484c, 1, 0, 2},
    {data_020e484c, 1, 0, 2},
    {data_020e484c, 1, 0, 3},
    {data_020e484c, 1, 0, 3},
    {data_020e484c, 1, 0, 4},
    {data_020e484c, 1, 0, 4},
    {data_020e484c, 1, 0, 5},
    {data_020e484c, 1, 0, 5},
    {data_020e484c, 1, 0, 6},
    {data_020e484c, 1, 0, 6},
    {data_020e484c, 1, 0, 6},
    {data_020e484c, 1, 0, 7},
    {data_020e484c, 1, 0, 7},
    {data_020e484c, 1, 0, 8},
    {data_020e484c, 1, 0, 8},
    {data_020e484c, 1, 0, 9},
    {data_020e484c, 1, 0, 9},
    {data_020e484c, 1, 0, 10},
    {data_020e484c, 1, 0, 10},
    {data_020e484c, 1, 0, 11},
    {data_020e484c, 1, 0, 11},
    {data_020e484c, 1, 0, 11},
    {data_020e484c, 1, 0, 12},
    {data_020e484c, 1, 0, 12},
    {data_020e484c, 1, 0, 12},
    {data_020e484c, 1, 0, 13},
    {data_020e484c, 1, 0, 13},
    {data_020e484c, 1, 0, 13},
    {data_020e484c, 1, 0, 14},
    {data_020e484c, 1, 0, 14},
    {data_020e484c, 1, 0, 14},
    {data_020e484c, 1, 0, 14},
    {data_020e484c, 1, 0, 15},
    {data_020e484c, 1, 0, 15},
    {data_020e484c, 1, 0, 15},
    {data_020e484c, 1, 0, 15},
};
GXOamAttr data_020e4854[1] = {{0x0, 0xd4, 0, 0x0, 0xffff}};
const u16 *const sSkyFogOffsetTables[5] = {data_020d1148, data_020d1148, data_020d1178, data_020d1178, data_020d1178};
SpriteAnimFrame data_020e4adc[1] = {
    {data_020e4a5c, 4, 0, 0},
};
const s32 sFireworkScaleBigLong[44] = {0x1000, 0x2800, 0x11c7, 0xc4f, 0xa00, 0x8e4, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x86c, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x835, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800};
SpriteAnimFrame data_020e4a64[1] = {
    {data_020e5a34, 4, 0, 0},
};
SpriteAnimFrame data_020e4b24[1] = {
    {data_020e4774, 1, 0, 0},
};
GXOamAttr data_020e4c94[2] = {{0x81f880e0, 0x9a, 0, 0x0, 0x0}, {0x81f88000, 0x11a, 0, 0x0, 0xffff}};
GXOamAttr data_020e4cb4[2] = {{0x41fc80e0, 0x98, 0, 0x0, 0x0}, {0x41fc8000, 0x118, 0, 0x0, 0xffff}};
s32 sSkyCrossingWidth = data_020c8cbc * 6;
GXOamAttr data_020e52e8[6] = {{0x101400e8, 0xb6, 0, 0x3, 0x0}, {0x1ef000d, 0xb7, 0, 0x3, 0x0}, {0x11e300fe, 0x95, 0, 0x3, 0x0}, {0x3012000a, 0xd4, 0, 0x3, 0x0}, {0x101800f8, 0xd4, 0, 0x3, 0x0}, {0x21e800e8, 0xb7, 0, 0x3, 0xffff}};
char data_020e4fc8[30] = "/sky/d_2d_b_cld_r1_bg_ncl.bin";
const s32 sRainSeIds[4] = {-1, 0x80a, 0x4ce, -1};
const s32 sFireworkScaleBig[9] = {0x2800, 0x11c7, 0xc4f, 0xa00, 0x8e4, 0x86c, 0x86c, 0x86c, 0x86c};
char data_020e5008[31] = "/sky/d_2d_b_cld_f_b_bg_nsc.bin";
GXOamAttr data_020e48ec[1] = {{0x81f880f0, 0x11a, 0, 0x4, 0xffff}};
GXOamAttr data_020e4b94[2] = {{0xb1f88000, 0x9a, 0, 0x0, 0x0}, {0xb1f880e0, 0x11a, 0, 0x0, 0xffff}};
GXOamAttr data_020e48c4[1] = {{0x41fb80f0, 0x95, 0, 0x9, 0xffff}};
GXOamAttr data_020e4844[1] = {{0x81f000f0, 0x11c, 0, 0x2, 0xffff}};
GXOamAttr data_020e483c[1] = {{0x1fc80f8, 0x95, 0, 0x0, 0xffff}};
SpriteAnimFrame data_020e5178[3] = {
    {data_020e4c94, 2, 0, 0},
    {data_020e4b64, 1, 0, 0},
    {data_020e4cb4, 1, 0, 0},
};
char data_020e5068[31] = "/sky/d_2d_b_cld_b_a_bg_nsc.bin";
const s8 sBirdDelays[4] = {
    0x1, 0x3, 0x5, 0x0,
};
SpriteAnimFrame data_020e4b0c[1] = {
    {data_020e488c, 4, 0, 0},
};
const s32 sRainParallax[3] = {0x7fff, 0x6667, 0x4cce};
GXOamAttr data_020e54d0[10] = {{0x31ff00f0, 0x94, 0, 0x3, 0x0}, {0x200700fb, 0xb6, 0, 0x3, 0x0}, {0x10020005, 0xb6, 0, 0x3, 0x0}, {0x1f200fa, 0xb6, 0, 0x3, 0x0}, {0x400f5, 0xb6, 0, 0x3, 0x0}, {0x1f900f0, 0x95, 0, 0x3, 0x0}, {0x30090006, 0xb4, 0, 0x3, 0x0}, {0x100800ef, 0x97, 0, 0x3, 0x0}, {0x1f600f4, 0xb4, 0, 0x3, 0x0}, {0x1f00002, 0x97, 0, 0x3, 0xffff}};
SpriteAnimFrame data_020e50e8[3] = {
    {data_020e4c04, 2, 0, 0},
    {data_020e4bb4, 1, 0, 0},
    {data_020e4c14, 1, 0, 0},
};
GXOamAttr data_020e5208[5] = {{0x1fb00f1, 0x95, 0, 0x3, 0x0}, {0x30060004, 0xb4, 0, 0x3, 0x0}, {0x100500f1, 0x97, 0, 0x3, 0x0}, {0x1fb00f8, 0xb4, 0, 0x3, 0x0}, {0x1f20002, 0x97, 0, 0x3, 0xffff}};
GXOamAttr data_020e4864[1] = {{0x400080f0, 0x95, 0, 0x0, 0xffff}};
GXOamAttr data_020e53f8[9] = {{0x1f700e7, 0xb7, 0, 0x3, 0x0}, {0x120000, 0xf6, 0, 0x3, 0x0}, {0x1010000c, 0x97, 0, 0x3, 0x0}, {0x300400e4, 0x95, 0, 0x3, 0x0}, {0x1e900f9, 0xb6, 0, 0x3, 0x0}, {0xe00f5, 0xb6, 0, 0x3, 0x0}, {0x1100ea, 0xd7, 0, 0x3, 0x0}, {0x11ef00ed, 0xd7, 0, 0x3, 0x0}, {0x11ec0006, 0x97, 0, 0x3, 0xffff}};
}
void SkySprites::updateFireworksShow() {
    BOOL r4 = FALSE;
    BOOL r6 = FALSE;
    ClockDateTimeWords s;
    Unk_020bbeb8_Fn fn;
    ClockDateTimeWords t1, t2, t3, t4;
    s.words[0] = 0;
    s.words[1] = 0;
    s32 kind = EventAnnounce_GetActiveEvent();
    Clock_GetDateTime(&s.dt);
    MI_CpuCopy8(&s, &t1, 8);
    if (Event_GetState(0x13, &t1, 1)) {
        u8 a = time.hour;
        u8 b = time.minute;
        s32 c = second;
        if (a < 2) {
            r4 = TRUE;
            if (a == 0 && b == 0 && c == 0) {
                r6 = TRUE;
            }
        }
    } else if (kind == 0xf) {
        if (Scene_InTownUnk31()) {
            MI_CpuCopy8(&s, &t2, 8);
            if (Event_GetState(0xf, &t2, r4)) {
                r4 = TRUE;
            }
        } else if ((gCommManager)->isOnline()) {
            MI_CpuCopy8(&s, &t3, 8);
            if (Event_GetState(0xf, &t3, r4)) {
                r4 = TRUE;
            }
        } else {
            r4 = TRUE;
        }
    } else if (Scene_GetCurrent() == 0x2c) {
        BOOL x;
        MI_CpuCopy8(&s, &t4, 8);
        if (Event_GetState(0xf, &t4, 1) == 2) {
            x = TRUE;
        } else {
            x = r4;
        }
        if (x) {
            r4 = TRUE;
        }
    }
    if (r4) {
        static Unk_020bbeb8_Fn tbl[20] = {
            0, &SkySprites::fireworksPatternAct01, &SkySprites::fireworksPatternAct02, &SkySprites::fireworksPatternAct03,
            &SkySprites::fireworksPatternAct04, &SkySprites::fireworksPatternAct05, &SkySprites::fireworksPatternAct06,
            &SkySprites::fireworksPatternAct07, &SkySprites::fireworksPatternAct08, &SkySprites::fireworksPatternAct09,
            &SkySprites::fireworksPatternAct0A, &SkySprites::fireworksPatternAct0B, &SkySprites::fireworksPatternAct0C,
            &SkySprites::fireworksPatternAct0D, &SkySprites::fireworksPatternAct0E, &SkySprites::fireworksPatternAct0F,
            &SkySprites::fireworksPatternAct10, &SkySprites::fireworksPatternAct11, &SkySprites::fireworksPatternAct12,
            &SkySprites::fireworksPatternAct13,
        };
        if (fireworksPattern == 0) {
            s32 v;
            if (r6) {
                v = 0x12;
            } else if (Random_GlobalBelow(2) == 0) {
                v = 0xe;
            } else {
                v = 0x10;
            }
            selectFireworksPattern(v);
            fireworksPatternTime = 0;
        }
        fireworksPatternTime--;
        fn = tbl[fireworksPattern];
        (this->*fn)();
    } else {
        fireworksPattern = 0;
        fireworksPatternTime = 0;
        fireworksBigTimer = 0;
        fireworksSlotTimers[0] = 0;
        fireworksSlotTimers[1] = 0;
        fireworksSlotTimers[2] = 0;
        fireworksSlotTimers[3] = 0;
        fireworksInterval = 0;
    }
}

void SkySprites::updateBalloon() {
    if (!(gCommManager)->isSlotActive(gCommManager->myAid)) {
        u8 v = time.hour;
        if (v >= 0xa && v < 0x10) {
            if (prevTime.minute != time.minute) {
                if ((u32)time.minute % 10 == 4) {
                    if (sprites[45].kind != 5) {
                        if (balloonChance == 0) {
                            balloonChance = Random_GlobalBelow(8) + 1;
                        }
                        u8 c = balloonChance;
                        if (Random_GlobalBelow(8) < c) {
                            if (!PlayerData_GetCurrent()->testFlag(0x30) && SkyShot_HasMaxHits() && !Random_GlobalBelow(4)) {
                                spawn(5, 0x2d, 0, 1);
                            } else {
                                spawn(5, 0x2d, 0, 0);
                                palette.pickBalloonColor();
                            }
                            balloonChance = 1;
                        } else if (c < 8) {
                            balloonChance++;
                        }
                    }
                }
            }
        }
    }
}

void SkySprites::updateUfo() {
    if (ufoEventToday != 0) {
        if (gSaveData.testFlag(9)) {
            ufoEventToday = 0;
        } else {
            ClockDateTimeWords s;
            ClockDateTimeWords t;
            s.words[0] = 0;
            s.words[1] = 0;
            BOOL b;
            Clock_GetDateTime(&s.dt);
            MI_CpuCopy8(&s, &t, 8);
            if (Event_GetState(0x44, &t, 0) == 0) {
                b = TRUE;
            } else {
                b = FALSE;
            }
            if (b) {
                ufoEventToday = 0;
            }
        }
    }
    if (ufoEventToday != 0) {
        if (!(gCommManager)->isSlotActive(gCommManager->myAid)) {
            u8 v = time.minute;
            u32 m = (u32)v % 10;
            if (prevTime.minute != v) {
                if (m == 2 || m == 7) {
                    if (!((VisitorSpawnFlags *)(TownSessionState_GetVisitorFlags(TownSessionState_Get())))->isVisitorSpawned()) {
                        if (sprites[45].kind != 6) {
                            if (ufoChance == 0) {
                                ufoChance = Random_GlobalBelow(8) + 1;
                            }
                            u8 c = ufoChance;
                            if (Random_GlobalBelow(8) < c) {
                                spawn(6, 0x2d, 0, 0);
                                ufoChance = 1;
                            } else if (c < 8) {
                                ufoChance++;
                            }
                        }
                    }
                }
            }
        }
    }
}

void SkySprites::updatePete() {
    if (peteEventToday != 0) {
        NpcActor *npc = NpcRegistry_FindSpNpc(8);
        if (npc) {
            if (npc->isUpdating()) {
                peteEventToday = 0;
            }
        }
    }
    if (peteEventToday != 0) {
        if (!(gCommManager)->isSlotActive(gCommManager->myAid)) {
            s32 r = Weather_GetFallingPrecip();
            if (r != 1 && r != 2) {
                if (time.hour != prevTime.hour) {
                    if (time.hour == 9 || time.hour == 0x11) {
                        if (!((PeteFallState *)(TownSessionState_GetPeteFall(TownSessionState_Get())))->isVisitorActive()) {
                            if (sprites[45].kind != 7) {
                                spawn(7, 0x2d, 0, 0);
                            }
                        }
                    }
                }
            }
        }
    }
}


// ======== unk_020bb25c.cpp ========

void SkySprites::updateRainbow() {
    rainbowStrength = 0;
    u8 k = (&gSaveData.weather)->todayPattern;
    if (k >= 0x13 && k <= 0x17) {
        if (*(data_020d0e04 + k - 0x13) == time.hour) {
            s32 n = time.minute;
            if (n < 0x2d) {
                s32 v;
                if (n <= 0xf) {
                    v = n * 0xccc;
                } else {
                    v = (n - 0xf) * -0x666 + 0xc000;
                }
                if (v < 0) {
                    v = 0;
                } else if (v > 0xc000) {
                    v = 0xc000;
                }
                rainbowStrength = (v + 0x800) >> 12;
            }
        }
    }
    BOOL f = sprites[32].kind == 10 ? TRUE : FALSE;
    if (rainbowStrength != 0) {
        applyRainbowBlend();
        if (!f) {
            if (!spawn(10, 0x20, 0, 0)) {
                rainbowStrength = 0;
            }
        }
    } else if (f) {
        killKind(10);
    }
}

void SkySprites::spawnShots() {
    SkyShotRequest *p = &shotRequests[0];
    SkyShotRequest *end = p + 4;
    s32 slot = 0x21;
    s32 i = 0;
    for (; p < end; p++, slot += 3, i++) {
        if (p->pending != 0) {
            u8 flag = p->golden;
            if (spawn(0xb, slot, NULL, i)) {
                if (flag != 0) {
                    spawn(0xb, slot + 1, NULL, i | 0x10);
                    spawn(0xb, slot + 2, NULL, i | 0x20);
                }
            } else {
                shotSeq.endWatch(p->playerSlot);
            }
            (p)->clear();
        }
    }
}

void SkySprites::updateBirds() {
    if (birdsRequested != 0) {
        birdsRequested = 0;
        BOOL a = Weather_GetFallingPrecip() == 0 ? TRUE : FALSE;
        ClockDateTimeWords x[3];  // from, to, now
        x[0].words[0] = 0;
        x[0].words[1] = 0;
        x[1].words[0] = 0;
        x[1].words[1] = 0;
        x[2].words[0] = 0;
        x[2].words[1] = 0;
        Clock_GetDateTime(&x[2].dt);
        MI_CpuCopy8(&x[2], &x[0], 8);
        MI_CpuCopy8(&x[2], &x[1], 8);
        x[0].dt.month = 3;
        x[0].dt.day = 0x10;
        x[1].dt.month = 9;
        x[1].dt.day = 0xf;
        u32 r5 = DateTime_Compare(&x[0].dt, &x[2].dt, 0x18);
        u32 r0 = DateTime_Compare(&x[2].dt, &x[1].dt, 0x18);
        BOOL b = FALSE;
        if (r5 == (u32)-1 || r5 == 0) {
            if (r0 == (u32)-1 || r0 == 0) {
                b = TRUE;
            }
        }
        x[0].dt.hour = 6;
        x[1].dt.hour = 9;
        r5 = DateTime_Compare(&x[0].dt, &x[2].dt, 4);
        r0 = DateTime_Compare(&x[2].dt, &x[1].dt, 4);
        BOOL c = FALSE;
        if (r5 == (u32)-1 || r5 == 0) {
            if (r0 == (u32)-1) {
                c = TRUE;
            }
        }
        if (a && b && c) {
            spawn(0xc, 0x3c, 0, 0);
            spawn(0xc, 0x3c, 0, 1);
            spawn(0xc, 0x3c, 0, 2);
        }
    }
}

BOOL SkySprites::canLaunchBig(s32 idx) {
    BOOL result = TRUE;
    SkySprite *p = &sprites[46];
    SkySprite *end = &sprites[59];
    for (; p < end; p++) {
        if (p->kind == 9) {
            u32 f = p->work0;
            s32 slot = (f >> 8) & 3;
            BOOL b1 = ((f >> 29) & 1) ? TRUE : FALSE;
            if (slot == idx || !b1) {
                result = FALSE;
                break;
            }
        }
    }
    return result;
}

BOOL SkySprites::canLaunchSmall(s32 idx) {
    BOOL result = TRUE;
    SkySprite *p = &sprites[46];
    SkySprite *end = &sprites[59];
    for (; p < end; p++) {
        if (p->kind == 9) {
            s32 slot;
            BOOL b1, b2;
            u32 f = p->work0;
            slot = (f >> 8) & 3;
            b1 = ((f >> 29) & 1) ? TRUE : FALSE;
            b2 = ((f >> 31) & 1) ? TRUE : FALSE;
            if (slot == idx) {
                result = FALSE;
                break;
            }
            if (b2 && !b1) {
                result = FALSE;
                break;
            }
        }
    }
    return result;
}

void SkySprites::launchFirework(s32 idx, s32 a, s32 b) {
    if (sSkyOutdoors != 0) {
        Unk_020bb8a4_E fa = (Unk_020bb8a4_E)(a << 31); Unk_020bb8a4_E lo = (Unk_020bb8a4_E)((sFireworkShellTypes[idx] - 0x1d) & 0xf); Unk_020bb8a4_E i8 = (Unk_020bb8a4_E)((idx & 3) << 8); Unk_020bb8a4_E fb = (Unk_020bb8a4_E)(b << 28); spawn(9, 0x3c, 0, fb | (i8 | (lo | fa)));
    } else if (a != 0) {
        SceneLights_StartFlash(idx + 5);
    } else {
        SceneLights_StartFlash(idx + 1);
    }
}

void SkySprites::rollLaunchInterval(s32 *out) {
    s32 r;
    if (fireworksInterval > 0) {
        r = Random_GlobalBelow(fireworksInterval);
    } else {
        r = 0;
    }
    *out = r + 0x41;
}

void SkySprites::tickBigLaunch() {
    fireworksBigTimer = fireworksBigTimer - 1;
    if (fireworksBigTimer <= 0) {
        s32 r = Random_GlobalBelow(4);
        if (canLaunchBig(r)) {
            launchFirework(r, 1, 0);
            rollLaunchInterval(&fireworksBigTimer);
        }
    }
}

void SkySprites::tickLowBigLaunch() {
    fireworksBigTimer = fireworksBigTimer - 1;
    if (fireworksBigTimer <= 0) {
        s32 r = Random_GlobalBelow(4);
        if (canLaunchBig(r)) {
            launchFirework(r, 1, 1);
            rollLaunchInterval(&fireworksBigTimer);
        }
    }
}

void SkySprites::tickSmallLaunches() {
    s32 *ptrs[4] = {&fireworksSlotTimers[0], &fireworksSlotTimers[1], &fireworksSlotTimers[2], &fireworksSlotTimers[3]};
    s32 **pp = ptrs;
    s32 i;
    for (i = 0; i < 4; i++, pp++) {
        s32 *c = *pp;
        *c = *c - 1;
        if (*c <= 0) {
            if (canLaunchSmall(i)) {
                launchFirework(i, 0, 0);
                rollLaunchInterval(c);
            }
        }
    }
}

void SkySprites::tickPairLaunches() {
    s32 *ptrs[4] = {&fireworksSlotTimers[0], &fireworksSlotTimers[1], &fireworksSlotTimers[2], &fireworksSlotTimers[3]};
    s32 **pp = ptrs;
    s32 all = 1;
    s32 i;
    for (i = 0; i < 4; i++, pp++) {
        s32 *c = *pp;
        if (*c > 0) {
            *c = *c - 1;
            if (*c <= 0) {
                if (canLaunchSmall(i)) {
                    launchFirework(i, 0, 0);
                } else {
                    *c = 1;
                }
            }
        }
        s32 t = (*c <= 0) ? 1 : 0;
        if (all & t) {
            all = 1;
        } else {
            all = 0;
        }
    }
    if (all) {
        s32 a = Random_GlobalBelow(4);
        s32 b = (a + 1 + Random_GlobalBelow(3)) & 3;
        s32 **p2 = ptrs;
        for (i = 0; i < 4; i++, p2++) {
            if (a == i || b == i) {
                *(*p2) = fireworksInterval + Random_GlobalBelow(30);
            }
        }
    }
}

void SkySprites::tickTripleLaunches() {
    s32 *ptrs[4] = {&fireworksSlotTimers[0], &fireworksSlotTimers[1], &fireworksSlotTimers[2], &fireworksSlotTimers[3]};
    s32 **pp = ptrs;
    s32 all = 1;
    s32 i;
    for (i = 0; i < 4; i++, pp++) {
        s32 *c = *pp;
        if (*c > 0) {
            *c = *c - 1;
            if (*c <= 0) {
                if (canLaunchSmall(i)) {
                    launchFirework(i, 0, 0);
                } else {
                    *c = 1;
                }
            }
        }
        s32 t = (*c <= 0) ? 1 : 0;
        if (all & t) {
            all = 1;
        } else {
            all = 0;
        }
    }
    if (all) {
        s32 skip = Random_GlobalBelow(4);
        s32 **p2 = ptrs;
        for (i = 0; i < 4; i++, p2++) {
            if (i != skip) {
                *(*p2) = fireworksInterval + Random_GlobalBelow(30);
            }
        }
    }
}

void SkySprites::setFireworksTiming(s32 a, s32 b, s32 c) {
    fireworksInterval = a;
    s32 r;
    if (c > 0) {
        r = Random_GlobalBelow(c);
    } else {
        r = 0;
    }
    fireworksPatternTime = b + r;
}

void SkySprites::selectFireworksPattern(s32 mode) {
    if (mode == 0x14) {
        u32 r = Random_GlobalBelow(100);
        const u8 *p = &sFireworksPatternWeights[1];
        s32 m = 5;
        s32 i;
        for (i = 1; i < 14; i++, p++) {
            if (r < *p) {
                m = i;
                break;
            }
        }
        fireworksPattern = m;
    } else {
        fireworksPattern = mode;
    }
    s32 idx = fireworksPattern;
    s32 cnt = sFireworksPatternDelays[idx];
    if (cnt > 0) {
        BOOL flag = FALSE;
        u32 sh = idx - 1;
        if (sh <= 18 && ((1 << sh) & 0x4c007) != 0) {
            flag = TRUE;
        }
        if (flag) {
            fireworksBigTimer = Random_GlobalBelow(cnt);
        } else {
            fireworksSlotTimers[0] = Random_GlobalBelow(cnt);
            fireworksSlotTimers[1] = Random_GlobalBelow(cnt);
            fireworksSlotTimers[2] = Random_GlobalBelow(cnt);
            fireworksSlotTimers[3] = Random_GlobalBelow(cnt);
        }
    }
}

void SkySprites::fireworksPatternAct01() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0x14, 0x64, 400); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickBigLaunch(); 
}

void SkySprites::fireworksPatternAct02() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0x64, 0x64, 500); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickBigLaunch(); 
}

void SkySprites::fireworksPatternAct03() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0xc8, 0x64, 600); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickBigLaunch(); 
}

void SkySprites::fireworksPatternAct04() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0x32, 0x64, 400); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickSmallLaunches(); 
}

void SkySprites::fireworksPatternAct05() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0xa0, 0x64, 500); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickSmallLaunches(); 
}

void SkySprites::fireworksPatternAct06() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0x12c, 0x64, 600); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickSmallLaunches(); 
}

void SkySprites::fireworksPatternAct07() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0x1e, 0x96, 400); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickPairLaunches(); 
}

void SkySprites::fireworksPatternAct08() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0x3c, 0x96, 500); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickPairLaunches(); 
}

void SkySprites::fireworksPatternAct09() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0x64, 0x96, 600); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickPairLaunches(); 
}

void SkySprites::fireworksPatternAct0A() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0x28, 0x96, 400); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickTripleLaunches(); 
}

void SkySprites::fireworksPatternAct0B() {
    s32 v = fireworksPatternTime; 
    if (v < 0) { 
        setFireworksTiming(0x50, 0x96, 500); 
    } else if (v == 0) { 
        selectFireworksPattern(0x14); 
    } 
    tickTripleLaunches(); 
}


// ======== unk_020ba93c.cpp ========


void SkySprites::fireworksPatternAct0C() {
    if (fireworksPatternTime < 0) {
        setFireworksTiming(0x8c, 0x96, 0x258);
    } else if (fireworksPatternTime == 0) {
        selectFireworksPattern(0x14);
    }
    tickTripleLaunches();
}

void SkySprites::fireworksPatternAct0D() {
    if (fireworksPatternTime < 0) {
        setFireworksTiming(0, 0x64, 0x96);
    } else if (fireworksPatternTime == 0) {
        selectFireworksPattern(0x14);
    }
}

void SkySprites::fireworksPatternAct0E() {
    if (fireworksPatternTime < 0) {
        setFireworksTiming(0, 0x64, 0);
    } else if (fireworksPatternTime == 0) {
        selectFireworksPattern(0xf);
    }
    tickSmallLaunches();
}

void SkySprites::fireworksPatternAct0F() {
    if (fireworksPatternTime < 0) {
        setFireworksTiming(0, 0xc8, 0);
    } else if (fireworksPatternTime == 0) {
        selectFireworksPattern(4);
    }
    tickBigLaunch();
}

void SkySprites::fireworksPatternAct10() {
    if (fireworksPatternTime < 0) {
        setFireworksTiming(0, 0x64, 0);
    } else if (fireworksPatternTime == 0) {
        selectFireworksPattern(0x11);
    }
    tickBigLaunch();
}

void SkySprites::fireworksPatternAct11() {
    if (fireworksPatternTime < 0) {
        setFireworksTiming(0, 0x64, 0);
    } else if (fireworksPatternTime == 0) {
        selectFireworksPattern(1);
    }
    tickSmallLaunches();
}

void SkySprites::fireworksPatternAct12() {
    if (fireworksPatternTime < 0) {
        setFireworksTiming(0, 0xe1, 0);
    } else if (fireworksPatternTime == 0) {
        selectFireworksPattern(0x13);
    }
}

void SkySprites::fireworksPatternAct13() {
    if (fireworksPatternTime < 0) {
        setFireworksTiming(0, 0x64, 0);
    } else if (fireworksPatternTime == 0) {
        selectFireworksPattern(0x10);
    }
    tickLowBigLaunch();
}

extern "C" void SkySprites_ApplyRainbowBlend(SkySprites *obj) {
    u16 *p = gWeatherManager.lineBlend[gWeatherManager.lineBufferIndex];
    u16 *end1 = p + 0x77;
    u16 *end2 = p + 0x97;
    u32 v = obj->rainbowStrength;
    u16 h = v | 0x1000;
    s32 step, i;
    for (; p < end1; p++) {
        *p = h;
    }
    step = _s32_div_f(-(v << 12), 0x20);
    i = 0;
    for (; p < end2; p++, i++) {
        *p = (v + ((step * i + 0x800) >> 12)) | 0x1000;
    }
}

extern "C" void SkySprites_OnStarWish(SkySprites *obj) {
    SkySprite *e = &obj->sprites[obj->findKind(3)];
    if (e != NULL) {
        obj->spawn(2, 0x3c, NULL, 1);
        e->work0 = 1;
    }
}

extern "C" void SkySprites_Start(SkySprites *obj) {
    obj->start();
    obj->se.resetAll();
}

extern "C" void SkySprites_Stop(SkySprites *obj) {
    obj->se.releaseAll();
    obj->stop();
}

extern "C" void SkySprites_UpdateEvents(SkySprites *obj) {
    switch (gWeatherManager.precipKind) {
    case 1:
        obj->updateRain();
        break;
    case 2:
        obj->updateSnow();
        break;
    default:
        obj->precipIntensity = 0;
        obj->snowCounter = 0;
        break;
    }
    obj->updateBirds();
    obj->updateMoon();
    obj->updateRainbow();
    obj->updateShootingStar();
    obj->updateFireworksShow();
    if (Scene_GetCurrent() != 0x2c) {
        obj->updateBalloon();
        obj->updateUfo();
        obj->updatePete();
        obj->spawnShots();
    }
}

extern "C" void SkySprites_UpdateEventsIndoor(SkySprites *obj) {
    s32 r = Scene_GetSkyKind(0);
    if (r != 0 && r != 3) {
        if (gWeatherManager.precipKind == 1 && gWeatherManager.level == 4) {
            obj->updateThunderFlash();
        }
        obj->updateFireworksShow();
    }
}

extern "C" void SkySprites_UpdateStub(void *) {
}

extern "C" void SkySprites_Update(SkySprites *obj) {
    SkySprite *e = obj->sprites;
    SkySprite *end = obj->sprites + 0x3c;
    s32 *cnt;
    obj->tickClock();
    SkySprites_UpdateStub(obj);
    SkySprites_UpdateEvents(obj);
    cnt = &obj->spriteCount;
    for (; e < end; e++) {
        if (e->gfxId != 0x34) {
            SpriteAnim *sub = &e->anim;
            switch (e->state) {
            case 1:
                if (e->gfxSlot != 6) {
                    obj->reloadAllGfx();
                }
                SkySprite_StartAnim(e);
                e->state = 2;
                break;
            case 2:
                sub->update();
                e->updateByKind();
                break;
            case 3:
                e->endByKind();
                *cnt -= 1;
                break;
            }
        }
    }
    obj->loadDirtyGfx();
    obj->shotSeq.update();
    obj->se.update();
}

extern "C" void SkySprites_UpdateIndoor(SkySprites *obj) {
    SkySprites_UpdateEventsIndoor(obj);
}

extern "C" void SkySprites_Draw(SkySprites *obj) {
    SkySprite *e;
    SkySprite *end = obj->sprites + 0x3c;
    s32 yoff = gWeatherManager.screenOffsetY[gWeatherManager.bufferIndex ^ 1];
    s32 id = -1;
    for (e = obj->sprites; e < end; e++) {
        SpriteAnim *sub;
        GXOamAttr *r;
        VecFx32 *pos;
        s32 a, b, x, y, py;
        if (e->kind == 0xd) continue;
        if (e->state != 2) continue;
        if (e->hidden != 0) continue;
        sub = &e->anim;
        r = sub->getCell();
        if (r == NULL) continue;
        pos = &e->screenPos;
        a = sub->getFrameX(id);
        b = sub->getFrameY(id);
        x = a + ((pos->x + 0x800) >> 12);
        py = b + ((pos->y + 0x800) >> 12);
        y = py - yoff;
        if (gWeatherManager.engine == 1) {
            u8 k = e->cullRadius;
            BOOL hidden = FALSE;
            if (k != 0) {
                s32 lo = y - k;
                s32 hi = y + k;
                if (hi < 0 || lo > 0xbf) hidden = TRUE;
            }
            if (hidden) continue;
            Oam_DrawCell(0, r, x, y, e->palette, e->priority, e->scaleX, e->scaleY, (u16)e->angle, id, (u32)(e->hFlip << 24) >> 24, (u32)(e->vFlip << 24) >> 24);
        } else if (py < 0xc0) {
            u8 k = e->cullRadius;
            BOOL hidden = FALSE;
            if (k != 0) {
                s32 lo = y - k;
                s32 hi = y + k;
                if (hi < 0 || lo > 0xbf) hidden = TRUE;
            }
            if (hidden) continue;
            Oam_DrawCell(1, r, x, y, e->palette, e->priority, e->scaleX, e->scaleY, (u16)e->angle, id, (u8)e->hFlip, (u8)e->vFlip);
        } else if (py > 0x100) {
            u8 k;
            BOOL hidden;
            s32 y2 = y - 0x100;
            k = e->cullRadius;
            hidden = FALSE;
            if (k != 0) {
                s32 lo = y2 - k;
                s32 hi = y2 + k;
                if (hi < 0 || lo > 0xbf) hidden = TRUE;
            }
            if (hidden) continue;
            Oam_DrawCell(0, r, x, y2, e->palette, e->priority, e->scaleX, e->scaleY, (u16)e->angle, id, (u32)(e->hFlip << 24) >> 24, (u32)(e->vFlip << 24) >> 24);
        }
    }
}

extern "C" void Sky_CalcLightColors(s32 a, s32 b, u32 c, u32 d) {
    u16 *out = sSkyLight.colors;
    u16 tmp[2];
    const u16 *const *row;
    const u16 *t;
    s32 i;
    if (gWeatherManager.targetLevel != gWeatherManager.level) {
        for (i = 0; i < 4; i++) {
            row = sSkyLightColorTables[gWeatherManager.level];
            t = row[i];
            Color_Lerp(&tmp[0], t + c, t + d, a);
            row = sSkyLightColorTables[gWeatherManager.targetLevel];
            t = row[i];
            Color_Lerp(&tmp[1], t + c, t + d, a);
            Color_Lerp(out, &tmp[0], &tmp[1], b);
            out++;
        }
    } else {
        row = sSkyLightColorTables[gWeatherManager.level];
        for (i = 0; i < 4; i++) {
            t = row[i];
            Color_Lerp(out, t + c, t + d, a);
            out++;
        }
    }
}

extern "C" void Sky_CalcFogOffset(s32 a, s32 b, u32 c, u32 d) {
    s32 cur = gWeatherManager.level;
    s32 next = gWeatherManager.targetLevel;
    if (next != cur) {
        const u16 *pc = sSkyFogOffsetTables[cur];
        u32 t1 = (u16)(((0x1000 - a) * pc[c] + a * pc[d]) >> 12);
        const u16 *pn = sSkyFogOffsetTables[next];
        t1 = t1 * (0x1000 - b);
        u32 t2 = (u16)(((0x1000 - a) * pn[c] + a * pn[d]) >> 12);
        Fog_SetOffset((u16)((t1 + t2 * b) >> 12));
    } else {
        Fog_SetOffset((u16)(((0x1000 - a) * sSkyFogOffsetTables[cur][c] + a * sSkyFogOffsetTables[cur][d]) >> 12));
    }
}

extern "C" void Sky_CalcLightParams(s32 a, s32 b, u32 c, u32 d) {
    u8 *out = sSkyLight.params;
    s32 i;
    s32 ia2, ia, ib;
    if (gWeatherManager.targetLevel != gWeatherManager.level) {
        i = 0;
        ia = 0x1000 - a;
        ib = 0x1000 - b;
        for (; i < 2; i++) {
            const u8 *p1 = sSkyLightParamTables[gWeatherManager.level][i];
            s32 t1 = (u16)((ia * p1[c] + a * p1[d]) >> 12);
            const u8 *p2 = sSkyLightParamTables[gWeatherManager.targetLevel][i];
            t1 = t1 * ib;
            s32 t2 = (u16)((ia * p2[c] + a * p2[d]) >> 12);
            *out = (t1 + t2 * b) >> 12;
            out++;
        }
    } else {
        i = 0;
        ia2 = 0x1000 - a;
        for (; i < 2; i++) {
            const u8 *p = sSkyLightParamTables[gWeatherManager.level][i];
            *out = (ia2 * p[c] + a * p[d]) >> 12;
            out++;
        }
    }
}

extern "C" void Sky_UpdateLighting(s32 a, s32 b) {
    SkyLightingLocals b0;
    u32 hour, rem;
    Clock_GetMinuteHour(&b0.time.minute);
    hour = b0.time.hour;
    rem = (hour + 1) % 24;
    Sky_CalcLightColors(a, b, hour, rem);
    Sky_CalcLightParams(a, b, hour, rem);
    Sky_CalcFogOffset(a, b, hour, rem);
    if (sSkyOutdoors != 0) {
        Fog_SetAlpha(0);
        Gfx3d_SetClearDepth(0x1c2);
        b0.clearColor = sSkyGradient.keyColors[1];
        Gfx3d_SetClearColor(b0.clearColor);
    }
    b0.roomColor = SceneLights_GetRoomColor();
    b0.roomColorArg = b0.roomColor;
    NNS_G3dGlbMaterialColorSpecEmi(0, b0.roomColorArg, 0);
}

extern "C" s16 Sky_GetLightColor(s32 i) {
    return sSkyLight.colors[i];
}

extern "C" u8 Sky_GetLightParam(s32 i) {
    return sSkyLight.params[i];
}

extern "C" void RainSe_Init(RainSndChannel *p) {
    RainSe_InitVolume(p);
    p->callReset();
}

extern "C" void RainSe_Update(RainSndChannel *p) {
    s32 v[3];
    if (p->volume != 0) {
        s32 k = sRainSeIds[Scene_GetSkyKind(0)];
        if (k >= 0) {
            p->callRequest(k);
        }
    }
    func_02133ef8(v, 12);
    v[2] = p->volume >> 8;
    p->callUpdate(v);
    RainSe_FadeVolume(p);
}

extern "C" void RainSe_Release(RainSndChannel *p) {
    p->callRelease();
}

extern "C" void RainSe_InitVolume(RainSndChannel *p) {
    BOOL a = gWeatherManager.level == 3;
    BOOL b = gWeatherManager.level == 4;
    BOOL c = gWeatherManager.precipKind == 1;
    p->volume = 0;
    if (c) {
        if (a) {
            p->volume = 0x99a00;
        } else if (b) {
            p->volume = 0x100000;
        }
    }
}


// ======== unk_020b9fe4.cpp ========


extern "C" void RainSe_FadeVolume(RainSndChannel *o) {
    s32 a = gWeatherManager.targetLevel == 3;
    s32 b = gWeatherManager.targetLevel == 4;
    s32 on = gWeatherManager.precipKind == 1;
    s32 v = 0;
    s32 w;
    if (on) {
        if (a) {
            v = 0x99a00;
        } else if (b) {
            v = 0x100000;
        }
    }
    w = 0xdf;
    if (on) {
        if (v < o->volume) {
            w = 0x1eb;
        }
    } else {
        w = 0x5200;
    }
    Math_StepS32Alt(&o->volume, v, w);
}

extern "C" BOOL Sky_LoadPaletteFiles() {
    s32 i, j;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (gWeatherManager.paletteFiles[i][j] == 0) {
                u8 **dst = &gWeatherManager.paletteFiles[i][j];
                *dst = (u8 *)File_LoadAlloc(sSkyPaletteFiles[i][j], gCurrentHeap, -4, &gWeatherManager.paletteFileSizes[i][j]);
                if (!*dst) {
                    return FALSE;
                }
            } else {
                File_LoadToBuffer(sSkyPaletteFiles[i][j], gWeatherManager.paletteFiles[i][j], gWeatherManager.paletteFileSizes[i][j]);
            }
        }
    }
    return TRUE;
}

extern "C" BOOL Sky_AllocPaletteBufs(u16 **p) {
    s32 i;
    for (i = 0; i < 3; i++) {
        if (*p == 0) {
            *p = (u16 *)Mem_AllocTail(32);
            if (*p == 0) {
                return FALSE;
            }
        }
        p++;
    }
    return TRUE;
}

void WeatherManager::rollRainSlant() {
    ClockDateTimeWords t;
    u32 st;
    u16 buf[8];
    u32 seed;
    t.words[0] = 0;
    t.words[1] = 0;
    TownId_CopyTo((u16 *)gSaveData.townId, buf);
    Clock_GetDateTime(&t.dt);
    seed = t.dt.day | ((t.dt.month << 5) | ((buf[0] << 16) | ((t.dt.year & 0x1f) << 9)));
    Random_SetSeed(&st, 1);
    Random_SetSeed(&st, seed);
    rainSlant = Random_NextBelow(&st, 0x2aaa) - 0x1555;
}

extern "C" void Color_Lerp(u16 *d, const u16 *s1, const u16 *s2, s32 t) {
    Rgb555 *dc = (Rgb555 *)d;
    const Rgb555 *c1 = (const Rgb555 *)s1;
    const Rgb555 *c2 = (const Rgb555 *)s2;
    s32 inv = 0x1000 - t;
    dc->r = (c1->r * inv + c2->r * t) >> 12;
    dc->g = (c1->g * inv + c2->g * t) >> 12;
    dc->b = (c1->b * inv + c2->b * t) >> 12;
    dc->x = 0;
}

extern "C" void Color_Lerp6(u16 *d, u16 *s1, u16 *s2, s32 t) {
    s32 i;
    for (i = 0; i < 6; i++) {
        Color_Lerp(d, s1, s2, t);
        d++;
        s1++;
        s2++;
    }
}

void WeatherManager::getSkyBlend(s32 *a, s32 *b) {
    MinuteHour t;
    Clock_GetMinuteHour(&t.minute);
    *a = (u32)(t.minute * 0x44445) >> 12;
    if (targetLevel != level) {
        *b = _s32_div_f(transitionProgress << 12, 32);
    } else {
        *b = 0;
    }
}

extern "C" s32 Sky_ApplyPalettes(s32 x) {
    u16 **pal = gWeatherManager.palettes;
    s32 r;
    Sky_BlendPalettes();
    if (gWeatherManager.targetLevel != gWeatherManager.level) {
        r = Gfx2d_LoadPaletteRange((u8 *)pal[2], sSkyPaletteLayer[x], 1, 1, 1);
    } else {
        r = Gfx2d_LoadPaletteRange((u8 *)pal[0], sSkyPaletteLayer[x], 1, 1, 1);
    }
    return r;
}

extern "C" void Sky_BlendPalettes() {
    u16 **pal = gWeatherManager.palettes;
    MinuteHour t;
    s32 a = 0;
    s32 b = 0;
    s32 h, pm;
    s32 h2, pm2;
    gWeatherManager.getSkyBlend(&a, &b);
    Clock_GetMinuteHour(&t.minute);
    h = t.hour;
    if ((s32)t.hour < 12) {
        pm = 0;
    } else {
        h -= 12;
        pm = 1;
    }
    h2 = (u32)(t.hour + 1) % 24;
    if (h2 < 12) {
        pm2 = 0;
    } else {
        h2 -= 12;
        pm2 = 1;
    }
    s32 set = sSkyPaletteSetByLevel[gWeatherManager.level];
    u16 *s1 = &((u16 *)gWeatherManager.paletteFiles[set][pm])[h * 16];
    u16 *s2 = &((u16 *)gWeatherManager.paletteFiles[set][pm2])[h2 * 16];
    Color_Lerp6(pal[0], s1, s2, a);
    if (gWeatherManager.targetLevel != gWeatherManager.level) {
        set = sSkyPaletteSetByLevel[gWeatherManager.targetLevel];
        s1 = &((u16 *)gWeatherManager.paletteFiles[set][pm])[h * 16];
        s2 = &((u16 *)gWeatherManager.paletteFiles[set][pm2])[h2 * 16];
        Color_Lerp6(pal[1], s1, s2, a);
        Color_Lerp6(pal[2], pal[0], pal[1], b);
        sSkyGradient.setKey(0, pal[2][4], 0);
        sSkyGradient.setKey(1, pal[2][5], 0xc0);
    } else {
        sSkyGradient.setKey(0, pal[0][4], 0);
        sSkyGradient.setKey(1, pal[0][5], 0xc0);
    }
    Sky_UpdateLighting(a, b);
}

WeatherManager::WeatherManager() {
    engine = 0;
    transitionBusy = 0;
    curCloudScreen = 0;
    cloudScreen = 0;
    transitionStep = 0;
    streamBlock = -1;
    screenOffsetY[0] = 0;
    screenOffsetY[1] = 0;
    bufferIndex = 0;
    palettes[0] = 0;
    palettes[1] = 0;
    palettes[2] = 0;
    precipKind = 0;
    rollRainSlant();
}

void WeatherManager::func_020ba49c() {
}

void WeatherManager::init() {
    ClockDateTimeWords t;
    s32 r = getWeatherAtOffset(0);
    switch (r) {
    case 3:
    case 4:
        (&gSaveData.weather)->rained = 1;
        break;
    }
    setLevels(r, r);
    engine = 1;
    transitionStep = 0;
    requestedLevel = -1;
    direction = 2;
    nextCloudVariant = Random_GlobalBelow(2);
    cloudVariant = nextCloudVariant;
    transitionBusy = 0;
    streamBlock = 0;
    transitionProgress = 0;
    t.words[0] = 0;
    t.words[1] = 0;
    Clock_GetDateTime(&t.dt);
    starScrollY = Sky_CalcStarScrollY(this, &t.dt);
    DateTime_SubHours(&t.dt, 6);
    starScrollX = Sky_CalcStarScrollX(this, &t.dt);
}

extern "C" s32 Sky_LoadPalettes(s32 x) {
    s32 r = 0;
    if (Sky_LoadPaletteFiles()) {
        if (Sky_AllocPaletteBufs(gWeatherManager.palettes)) {
            if (sSkyOutdoors != 0) {
                r = Sky_ApplyPalettes(x);
            } else {
                Sky_BlendPalettes();
                r = TRUE;
            }
        }
    }
    return r;
}

s32 WeatherManager::getPrecipKind(s32 v) {
    s32 r = 0;
    ClockDateTimeWords t;
    t.words[0] = 0;
    t.words[1] = 0;
    Clock_GetDateTime(&t.dt);
    switch (v) {
    case 3:
    case 4:
        switch (DateTime_GetWeatherPeriod((u8 *)&t)) {
        case 0:
        case 1:
        case 2:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
            r = 2;
            break;
        default:
            r = 1;
            break;
        }
        break;
    }
    return r;
}

void WeatherManager::setLevels(s32 a, s32 b) {
    s32 x = getPrecipKind(a);
    s32 y = getPrecipKind(b);
    if (x != 0) {
        precipKind = x;
    } else if (y != 0) {
        precipKind = y;
    } else {
        precipKind = 0;
    }
    level = a;
    targetLevel = b;
}

void WeatherManager::startTransition(s32 v) {
    requestedLevel = v;
    if (v > level) {
        direction = 0;
    } else {
        direction = 1;
    }
    nextCloudVariant = Random_GlobalBelow(2);
}

BOOL WeatherManager::requestLevel(s32 v) {
    BOOL r = FALSE;
    if (v == -1) {
    } else if (v == level) {
        r = TRUE;
    } else if (transitionBusy == 0) {
        startTransition(v);
        r = TRUE;
        transitionStep = r;
        transitionBusy = r;
    }
    return r;
}

void WeatherManager::updateHourly() {
    ClockDateTimeWords t;
    WeatherRecord *w = &gSaveData.weather;
    t.words[0] = 0;
    t.words[1] = 0;
    Clock_GetDateTime(&t.dt);
    if ((w->hourBase + 6) % 24 != t.dt.hour) {
        s32 r;
        Weather_UpdateDaily(w, &t);
        r = getWeatherAtOffset(1);
        switch (r) {
        case 3:
        case 4:
            w->rained = 1;
            break;
        }
        if (requestLevel(r)) {
            s32 h = t.dt.hour - 6;
            if (h < 0) {
                h += 24;
            }
            w->hourBase = h;
        }
    }
}

s32 WeatherManager::getWeatherAtOffset(s32 v) {
    s32 t = v + (&gSaveData.weather)->hourBase;
    return getPatternWeather((&gSaveData.weather)->todayPattern, t % 24);
}

s32 WeatherManager::getPatternWeather(s32 a, s32 b) {
    const s8 *p = sWeatherHourTable + a * 24;
    return p[b];
}

BOOL WeatherManager::loadCloudChars(s32 idx, s32 unused) {
    BOOL ok = FALSE;
    if (Gfx2d_LoadCharFile8bpp(sCloudCharFiles[idx], gCurrentHeap, unused, 16, 16, 47)) {
        ok = TRUE;
    }
    return ok;
}

BOOL WeatherManager::loadCloudScreen(u8 **p1, u32 *p2, s32 mode, s32 idx) {
    BOOL ok = FALSE;
    s32 i;
    const char *res;
    if (mode == 0) {
        s32 t = idx * 2;
        i = t + cloudVariant;
    } else {
        i = idx;
    }
    res = sCloudScreenFiles[mode][i];
    if (*p1 == NULL) {
        *p1 = (u8 *)File_LoadAlloc(res, gCurrentHeap, -4, p2);
        if (*p1 != NULL) {
            ok = TRUE;
        }
    } else if (File_LoadToBuffer(res, *p1, *p2)) {
        ok = TRUE;
    }
    return ok;
}

void WeatherManager::loadNextCloudGraphics() {
    s32 mode, next;
    if (direction == 0) {
        mode = 1;
        next = level + 1;
    } else {
        mode = 2;
        next = level - 1;
    }
    if (loadCloudChars(next, 6)) {
        if (loadCloudScreen(&cloudScreen, &cloudScreenSize, mode, next)) {
            setLevels(level, next);
            if (level < 3 && targetLevel >= 3) {
                rollRainSlant();
            }
            streamBlock = 0;
            transitionProgress = 0;
            transitionStep = 2;
        }
    }
}

void WeatherManager::streamCloudScreen() {
    s32 idx = streamBlock;
    s32 blk = (((cloudScroll >> 8) - 8) & 0xff) >> 3;
    if (blk == idx) {
        if (Gfx2d_LoadScreen(cloudScreen + (blk << 5), 6, 32, blk << 5)) {
            s32 cnt = transitionProgress;
            idx = idx + 1;
            cnt = cnt + 1;
            if (idx >= 32) {
                idx = -1;
                cnt = 32;
                setLevels(targetLevel, targetLevel);
                cloudVariant = nextCloudVariant;
                transitionStep = 3;
            }
            streamBlock = idx;
            transitionProgress = cnt;
        }
    }
}


// ======== unk_020b96b8.cpp ========
#define REG16(a) (*(volatile u16 *)(a))
#define REG32(a) (*(volatile u32 *)(a))


void WeatherManager::reloadCurrentCloudGraphics()
{
    s32 cur = level;
    if (loadCloudChars(cur, 6) != 0) {
        if (loadCloudScreen(&curCloudScreen, &curCloudScreenSize, 0, cur) != 0) {
            streamBlock = 0;
            transitionProgress = 0x20;
            transitionStep = 4;
        }
    }
}

void WeatherManager::streamCurrentCloudScreen()
{
    s32 cur = streamBlock;
    s32 k = (((cloudScroll >> 8) - 8) & 0xff) >> 3;
    if (k == cur) {
        if (Gfx2d_LoadScreen(curCloudScreen + (k << 5), 6, 0x20, k << 5) != 0) {
            cur++;
            if (cur >= 0x20) {
                cur = -1;
                if (requestedLevel == level) {
                    transitionStep = 0;
                    requestedLevel = cur;
                    direction = 2;
                    transitionBusy = 0;
                } else {
                    transitionStep = 1;
                }
            }
            streamBlock = cur;
        }
    }
}

void WeatherManager::updateTransitionStreamed()
{
    if (transitionBusy != 0) {
        switch (transitionStep) {
        case 1:
            loadNextCloudGraphics();
            break;
        case 2:
            streamCloudScreen();
            break;
        case 3:
            reloadCurrentCloudGraphics();
            break;
        case 4:
            streamCurrentCloudScreen();
            break;
        }
    }
}

void WeatherManager::transitionTimedBegin()
{
    s32 cur = level;
    s32 next;
    if (requestedLevel > cur) {
        next = cur + 1;
    } else {
        next = cur - 1;
    }
    setLevels(cur, next);
    transitionProgress = 0;
    transitionStep = 2;
    transitionTimer = 0;
}

void WeatherManager::transitionTimedBlend()
{
    transitionTimer++;
    if (transitionTimer >= 0x2a8) {
        setLevels(targetLevel, targetLevel);
        transitionStep = 3;
    }
    transitionProgress = (transitionTimer << 5) / 0x2a8;
}

void WeatherManager::transitionTimedFinish()
{
    transitionProgress = 0x20;
    transitionStep = 4;
    transitionTimer = 0;
}

void WeatherManager::transitionTimedWait()
{
    transitionTimer++;
    if (transitionTimer >= 0x2a8) {
        if (requestedLevel == level) {
            transitionStep = 0;
            requestedLevel = -1;
            direction = 2;
            transitionBusy = 0;
        } else {
            transitionStep = 1;
        }
    }
}

void WeatherManager::updateTransitionTimed()
{
    if (transitionBusy != 0) {
        switch (transitionStep) {
        case 1:
            transitionTimedBegin();
            break;
        case 2:
            transitionTimedBlend();
            break;
        case 3:
            transitionTimedFinish();
            break;
        case 4:
            transitionTimedWait();
            break;
        }
    }
}

// (self: unused)
extern "C" u16 Sky_CalcStarScrollX(WeatherManager *self, ClockDateTime *date)
{
    ClockDateTimeWords epoch;  // 1 January 2000
    epoch.words[0] = 0x1000000;
    epoch.words[1] = 1;
    return (u16)((DateTime_DiffDays(&epoch.dt, date) % 0x40) << 3);
}

// (self: unused)
extern "C" u32 Sky_CalcStarScrollY(WeatherManager *self, ClockDateTime *d)
{
    s32 m = (d->hour + 6) % 0x18;
    if (m >= 0xc) return 0x100;
    s32 t = m * 0x3c;
    t += d->minute;
    return (u32)((t * 0x68000) / 0x2d0 << 4) >> 16;
}

extern "C" u32 Sky_GetStarScrollYNow(WeatherManager *self)
{
    ClockDateTimeWords d;
    d.words[0] = 0;
    d.words[1] = 0;
    Clock_GetDateTime(&d.dt);
    return Sky_CalcStarScrollY(self, &d.dt);
}

void SkyGradient::setKey(s32 idx, u16 color, s32 line)
{
    keyColors[idx] = color;
    keyLines[idx] = line;
}

extern "C" void SkyGradient_Build()
{
    s32 idx = (sSkyGradient.bufferIndex + 1) % 2;
    volatile u16 out;
    Rgb555 c1, c2;
    volatile u16 in1, in2;
    in1 = sSkyGradient.keyColors[0];
    c1 = *(Rgb555 *)&in1;
    in2 = sSkyGradient.keyColors[1];
    c2 = *(Rgb555 *)&in2;
    u16 *dst = sSkyGradient.lineColors[idx];
    s32 s0 = sSkyGradient.keyLines[0];
    s32 s1 = sSkyGradient.keyLines[1];
    s32 i = 0;
    s32 span = s1 - s0;
    s32 c2g = c2.g;
    s32 c1g = c1.g;
    s32 c2r = c2.r;
    s32 c1r = c1.r;
    s32 c2b = c2.b;
    s32 c1b = c1.b;
    do {
        if (i <= s0) {
            out = in1;
        } else if (i >= s1) {
            out = in2;
        } else {
            s32 t = DS_DIV(((i - s0) << 8), span);
            s32 u = 0x100 - t;
            u32 col = (u16)((c1b * u + c2b * t) >> 8) << 10;
            col |= (u16)((c1r * u + c2r * t) >> 8) | (u16)((c1g * u + c2g * t) >> 8) << 5;
            out = col;
        }
        *dst++ = out;
        i++;
    } while (i < 0xc0);
    sSkyGradient.bufferIndex = idx;
}

extern "C" void Sky_UpdateLineTables()
{
    u16 *pal;
    s32 a = 0, b = 0;
    s32 idx = (gWeatherManager.lineBufferIndex + 1) % 2;
    s32 j, i;
    pal = gWeatherManager.lineBlend[idx];
    Actor *o = (Actor *)PlayerActor_GetActor(4);
    if (o) {
        VecFx32 *p = &o->position;
        if (p) {
            a = p->x >> 3;
            s32 *g = &gWeatherManager.prevPlayerZ;
            s32 pos = p->z;
            s32 d = (pos - *g) >> 4;
            if (d < 0) {
                b = (d * -176) >> 8;
            } else {
                b = -(d << 7) >> 8;
            }
            *g = pos;
        }
    }
    s32 *gp = &gWeatherManager.cloudScroll;
    s32 c = b; b = *gp; b += 0x60; b += c;
    if (b >= 0x10000) b -= 0x10000;
    *gp = b;
    if (sSkyLineTablesReady == 0) {
        sSkyLineTablesReady = 1;
        for (i = 0; i < 2; i++) {
            sSkyLineScrollX[i] = a;
            sSkyLineScrollY[i] = b;
            SkyLineAffine *q = gWeatherManager.lineAffine[i];
            for (j = 0; j < 0xc0; j++) {
                s32 t = -(j << 8) / 0xc0;
                q->pa = DS_DIV(0x10000, (t + 0x200));
                s32 n = (q->pa - 0x100) << 7;
                n = -n;
                q->x = a + n;
                s32 u = (j * 0xc000 / 0xc0 + 0x3000) / 0xc0;
                q->y = b + (u * u >> 8) * 0xc0;
                q++;
            }
        }
    }
    s32 da = a - sSkyLineScrollX[idx];
    s32 db = b - sSkyLineScrollY[idx];
    if (da != 0 || db != 0) {
        sSkyLineScrollX[idx] = a;
        sSkyLineScrollY[idx] = b;
        SkyLineAffine *end, *q;
        q = gWeatherManager.lineAffine[idx];
        end = q + 0xc0;
        if (db == 0) {
            for (; q < end; q++) q->x += da;
        } else if (da == 0) {
            for (; q < end; q++) q->y += db;
        } else {
            for (; q < end; q++) {
                q->x += da;
                q->y += db;
            }
        }
    }
    i = 0;
    u32 w = SceneLights_GetLightParam(1);
    if (w != sSkyBlendLineCache[idx]) {
        sSkyBlendLineCache[idx] = w;
        u16 c = w | 0x1000;
        for (; i < 0x77; i++) *pal++ = c;
        for (; i < 0x97; i++) *pal++ = (w - (w * (i - 0x77) >> 5)) | 0x1000;
        for (; i < 0x98; i++) *pal++ = 0x10;
        for (; i < 0xa8; i++) {
            u32 t = (u32)((i - 0x98) << 4) >> 4;
            u32 v = 0x10 - t;
            *pal++ = v | ((0x10 - v) << 8);
        }
        for (; i < 0xc0; i++) *pal++ = 0x1000;
    }
    gWeatherManager.lineBufferIndex = idx;
}

#ifdef N3DS_DIRECT
extern "C" void gpu2d_margin_affine(int engine, int bg, int line, int pa, s32 x, s32 y);  // (modern/host/gpu2d)
// The enhanced port's wide view: the cloud BG on the 24 lines above and below the DS's
// screen, as the line tables would have them if they went on (Sky_UpdateLineTables's formulas; the screen's line L
// shows table line L - 1 + the screen offset, as in Sky_HBlankMain / Sub; line 0 that of line 1 (Sky_VBlankMain /
// Sub), and the lines above go on from it), so that the clouds' perspective goes on beyond the DS's edges
static void Sky_GiveMarginLines(s32 engine)
{
    s32 idx = gWeatherManager.lineBufferIndex;
    s32 off = gWeatherManager.screenOffsetY[gWeatherManager.bufferIndex];
    for (s32 line = -24; line < 0xc0 + 24; line++) {
        if (line == 0) line = 0xc0;
        s32 j = (line < 0 ? line : line - 1) + off;
        s32 t = -(j * 0x100) / 0xc0;
        s32 pa = DS_DIV(0x10000, (t + 0x200));
        s32 x = sSkyLineScrollX[idx] - (pa - 0x100) * 0x80;
        s32 u = (j * 0xc000 / 0xc0 + 0x3000) / 0xc0;
        s32 y = sSkyLineScrollY[idx] + (u * u >> 8) * 0xc0;
        if (engine) x = (u16)x, y = (u16)y;  // (as Sky_HBlankSub writes them)
        gpu2d_margin_affine(engine, 3, line, pa, x, y);
    }
}
#endif

extern "C" void Sky_VBlankSub()
{
    u8 t[2];
    volatile u16 v1, v2;
    s32 y = gWeatherManager.screenOffsetY[gWeatherManager.bufferIndex];
    v1 = sSkyGradient.keyColors[0];
    REG16(HW_PLTT + 0x400) = v1;
    v2 = sSkyGradient.keyColors[1];
    REG16(HW_PLTT) = v2;
    SkyLineAffine *e = gWeatherManager.lineAffine[gWeatherManager.lineBufferIndex];
#ifdef N3DS_DIRECT  // (line 0 from the screen offset's table line, as line 1: the wide view's margin above goes on from it)
    e += y < 0xc0 ? y : 0xbf;
#endif
    IO_W(&REG16(0x4001030), e->pa);
    IO_W(&REG32(0x4001038), e->x);
    IO_W(&REG32(0x400103c), e->y);
#ifdef N3DS_DIRECT
    Sky_GiveMarginLines(1);
#endif
    Clock_GetMinuteHour(t);
    if (t[1] >= 6 && t[1] < 0x12) {
        IO_W(&REG32(0x4001000), IO_R(&REG32(0x4001000)) & 0xfffffdff);
        if (MenuCtrl_IsMenuOnTop() == 0) Gfx2d_HideSubPlanes(2);
    } else {
        IO_OR(&REG32(0x4001000), 0x200);
        if (MenuCtrl_IsMenuOnTop() == 0) Gfx2d_ShowSubPlanes(2);
    }
    if (gSkySprites.rainbowStrength != 0) {
        IO_W(&REG16(0x4001050), 0x2040);
    } else {
        IO_W(&REG16(0x4001050), 0x2042);
    }
    u16 s = y + gWeatherManager.starScrollY;
    if (s > 0x100) s = 0x100;
    IO_W(&REG16(0x4001016), s);
    IO_W(&REG16(0x4001014), gWeatherManager.starScrollX);
}

extern "C" void Sky_VBlankMain()
{
    u8 t[2];
    volatile u16 v1, v2;
    s32 y = gWeatherManager.screenOffsetY[gWeatherManager.bufferIndex];
    v1 = sSkyGradient.keyColors[0];
    REG16(HW_PLTT + 0x400) = v1;
    v2 = sSkyGradient.keyColors[0];
    REG16(HW_PLTT) = v2;
    SkyLineAffine *e = gWeatherManager.lineAffine[gWeatherManager.lineBufferIndex];
#ifdef N3DS_DIRECT  // (line 0 from the screen offset's table line, as line 1: the wide view's margin above goes on from it)
    e += y < 0xc0 ? y : 0xbf;
#endif
    IO_W(&REG16(0x4000030), e->pa);
    IO_W(&REG32(0x4000038), e->x);
    IO_W(&REG32(0x400003c), e->y);
#ifdef N3DS_DIRECT
    Sky_GiveMarginLines(0);
#endif
    Clock_GetMinuteHour(t);
    if (t[1] >= 6 && t[1] < 0x12) {
        IO_W(&REG32(0x4000000), IO_R(&REG32(0x4000000)) & 0xfffffdff);
        Gfx2d_HideMainPlanes(2);
    } else {
        IO_OR(&REG32(0x4000000), 0x200);
        Gfx2d_ShowMainPlanes(2);
    }
    s32 r;
    if (y > 0xa8) {
        IO_W(&REG32(0x4000000), IO_R(&REG32(0x4000000)) & 0xfffff5ff);
        Gfx2d_HideMainPlanes(0xa);
        IO_W(&REG16(0x4000050), 0x2040);
    } else {
        IO_OR(&REG32(0x4000000), 0x800);
        Gfx2d_ShowMainPlanes(8);
        if (y > 0x97) {
            IO_W(&REG32(0x4000000), IO_R(&REG32(0x4000000)) & 0xfffffdff);
            Gfx2d_HideMainPlanes(2);
            IO_W(&REG16(0x4000050), 0x2048);
        } else if (gSkySprites.rainbowStrength != 0) {
            IO_W(&REG16(0x4000050), 0x2040);
        } else {
            IO_W(&REG16(0x4000050), 0x2042);
        }
    }
    r = 0xc0 - y;
    if (r < 0) r = 0;
    else if (r > 0xc0) r = 0xc0;
    u16 s = y + gWeatherManager.starScrollY;
    if (s > 0x100) s = 0x100;
    IO_W(&REG16(0x4000016), s);
    IO_W(&REG16(0x4000014), gWeatherManager.starScrollX);
    Gfx2d_EnableMainWindows(1);
    Gfx2d_SetMainWin0Planes(0x15);
    Gfx2d_SetMainWinOutPlanes(0x1f);
    Gfx2d_SetMainWin0Rect(0, r, 0xff, 0xc0);
}

// The H-blank handlers (ITCM 0x01ffcd50 / 0x01ffceb8, code in a gap of the DS build; HBlank_Handler calls them after
// each visible line): the next line of the sky from the line tables, y being VCOUNT plus the screen offset (at most
// 191) - the backdrop gradient, the cloud BG's affine line, the blend, and below the horizon (151, 168) the layers
// and blending the sea shows with
extern "C" void Sky_HBlankMain()
{
    s32 y = IO_R(&REG16(0x4000006)) + gWeatherManager.screenOffsetY[gWeatherManager.bufferIndex];
    if (y >= 0xc0) y = 0xbf;
    REG16(HW_PLTT + 0x400) = sSkyGradient.keyColors[0];
    REG16(HW_PLTT) = sSkyGradient.lineColors[sSkyGradient.bufferIndex][y];
    SkyLineAffine *e = &gWeatherManager.lineAffine[gWeatherManager.lineBufferIndex][y];
    IO_W(&REG16(0x4000030), e->pa);
    IO_W(&REG32(0x4000038), e->x);
    IO_W(&REG32(0x400003c), e->y);
    if (y >= 0xa8) {
        IO_W(&REG32(0x4000000), IO_R(&REG32(0x4000000)) & 0xfffff5ff);
        IO_W(&REG16(0x4000050), 0x2040);
    } else if (y >= 0x97) {
        IO_W(&REG32(0x4000000), IO_R(&REG32(0x4000000)) & 0xfffffdff);
        IO_W(&REG16(0x4000050), 0x2048);
    }
    IO_W(&REG16(0x4000052), gWeatherManager.lineBlend[gWeatherManager.lineBufferIndex][y]);
}

// (engine B's: the reference point's low 16 bits only, as the DS code writes them)
extern "C" void Sky_HBlankSub()
{
    s32 y = IO_R(&REG16(0x4000006)) + gWeatherManager.screenOffsetY[gWeatherManager.bufferIndex];
    if (y >= 0xc0) y = 0xbf;
    REG16(HW_PLTT + 0x400) = sSkyGradient.lineColors[sSkyGradient.bufferIndex][y];
    REG16(HW_PLTT) = sSkyGradient.keyColors[1];
    SkyLineAffine *e = &gWeatherManager.lineAffine[gWeatherManager.lineBufferIndex][y];
    IO_W(&REG16(0x4001030), e->pa);
    IO_W(&REG32(0x4001038), (u16)e->x);
    IO_W(&REG32(0x400103c), (u16)e->y);
    if (y >= 0x97) {
        IO_W(&REG32(0x4001000), IO_R(&REG32(0x4001000)) & 0xfffffdff);
        IO_W(&REG16(0x4001050), 0x2048);
    }
    IO_W(&REG16(0x4001052), gWeatherManager.lineBlend[gWeatherManager.lineBufferIndex][y]);
}

#undef REG16
#undef REG32

// ======== unk_020b8d98.cpp ========
typedef volatile u16 vu16;
typedef volatile u32 vu32;
#define data_020e676c "menu/star/a_bg.bch"
#define data_020e6780 "menu/star/bg.bsc"

void Sky_FrameIndoor(void);
void Sky_FrameOutdoor(void);
void Sky_VBlankDisabled(void);

void Sky_VBlankDisabled(void) {
    vu16 t;
    t = sSkyGradient.keyColors[0];
    *(vu16 *)(HW_PLTT + 0x400) = t;
    *(vu16 *)HW_PLTT = sSkyGradient.lineColors[0][0xbf];
    IO_W((vu32 *)0x4000000, IO_R((vu32 *)0x4000000) & 0xfffff5ff);
}

void Sky_FrameOutdoor(void) {
    s32 a, b;
    SkyGradient_Build();
    if (!MenuCtrl_IsMenuOnTop()) {
        Sky_UpdateLineTables();
        gWeatherManager.updateTransitionStreamed();
    }
    Sky_ApplyPalettes(gWeatherManager.engine);
    if (gCamera != 0) {
        a = MenuCtrl_GetTransitionProgress() * 0x123 >> 12;
        b = Camera_GetCloseUpFactor() * 30 >> 12;
        gWeatherManager.screenOffsetY[gWeatherManager.bufferIndex ^ 1] = a + b;
    }
    gWeatherManager.starScrollY = Sky_GetStarScrollYNow(&gWeatherManager);
}

void Sky_FrameIndoor(void) {
    s32 a, b;
    if (!MenuCtrl_IsMenuOnTop()) {
        gWeatherManager.updateTransitionTimed();
    }
    gWeatherManager.getSkyBlend(&a, &b);
    Sky_BlendPalettes();
}

BOOL SkyProc::onCreate() {
    u32 r4 = 0;
    u32 v = 0;
    BOOL t = (gFieldSceneKind == 0);
    if (t) {
        v = 1;
    }
    sSkyOutdoors = v;
    gWeatherManager.init();
    if (sSkyOutdoors != 0) {
        if (SkyObjGfxLoader_LoadPalettes(&gSkySprites.loader)) {
            if (Sky_SetEngine(0)) {
                SkySprites_Start(&gSkySprites);
                r4 = HBlank_Add(&sSkyHBlankTask, Sky_HBlankSub, Sky_VBlankSub, Sky_FrameOutdoor);
            }
        }
    } else {
        if (Sky_LoadPalettes(0)) {
            r4 = HBlank_Add(&sSkyHBlankTask, 0, NULL, Sky_FrameIndoor);
        }
    }
    if (r4 != 0) {
        RainSe_Init(&envSndChannel);
    }
    return r4;
}

BOOL SkyProc::onExecute() {
    if (sSkyOutdoors != 0) {
        if (!MenuCtrl_IsMenuOnTop()) {
            StarTwinkle_Update(&data_021efc18);
        }
        SkySprites_Update(&gSkySprites);
    } else {
        SkySprites_UpdateIndoor(&gSkySprites);
    }
    gWeatherManager.updateHourly();
    RainSe_Update(&envSndChannel);
    return TRUE;
}

BOOL SkyProc::onDraw() {
    if (sSkyOutdoors != 0) {
        SkySprites_Draw(&gSkySprites);
    }
    return TRUE;
}

BOOL SkyProc::onDelete() {
    BOOL saved = HBlank_Remove(&sSkyHBlankTask);
    s32 i, j, k;
    GX_LoadBGPltt(sSkyBlackBackdrop, 0, 2);
    GXS_LoadBGPltt(sSkyBlackBackdrop, 0, 2);
    if (gWeatherManager.curCloudScreen != NULL) {
        Mem_Free(gWeatherManager.curCloudScreen);
    }
    gWeatherManager.curCloudScreen = NULL;
    if (gWeatherManager.cloudScreen != NULL) {
        Mem_Free(gWeatherManager.cloudScreen);
    }
    gWeatherManager.cloudScreen = NULL;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (gWeatherManager.paletteFiles[i][j] != NULL) {
                Mem_Free(gWeatherManager.paletteFiles[i][j]);
            }
            gWeatherManager.paletteFiles[i][j] = NULL;
        }
    }
    for (k = 0; k < 3; k++) {
        if (gWeatherManager.palettes[k] != NULL) {
            Mem_Free(gWeatherManager.palettes[k]);
            gWeatherManager.palettes[k] = NULL;
        }
    }
    if (sSkyOutdoors != 0) {
        SkySprites_Stop(&gSkySprites);
    }
    StarTwinkle_Stop(&data_021efc18);
    RainSe_Release(&envSndChannel);
    return saved;
}

extern "C" BOOL Sky_LoadCloudBg(u32 idx) {
    u8 r5;
    s32 r4;
    BOOL result;
    r4 = gWeatherManager.level;
    result = FALSE;
    r5 = sSkyCloudBgLayer[idx];
    if (Sky_LoadPalettes(idx)) {
        if (gWeatherManager.loadCloudChars(r4, r5)) {
            u32 *r7 = &gWeatherManager.curCloudScreenSize;
            if (gWeatherManager.loadCloudScreen((u8 **)(&gWeatherManager.curCloudScreen), (u32 *)(r7), 0, r4)) {
                if (Gfx2d_LoadScreen(gWeatherManager.curCloudScreen, r5, *r7, 0)) {
                    result = TRUE;
                }
            }
        }
    }
    return result;
}

extern "C" BOOL Sky_LoadNextCloudBg(u32 idx) {
    BOOL result = FALSE;
    if (gWeatherManager.transitionBusy != 0 && gWeatherManager.direction != 2) {
        s32 r3 = gWeatherManager.direction;
        s32 r5 = gWeatherManager.streamBlock;
        s32 r2 = gWeatherManager.level;
        s32 r1 = gWeatherManager.targetLevel;
        s32 r4;
        u32 r7;
        u32 stk4;
        if (r1 == r2) {
            if (r5 >= 0) {
                r5 = 0x20 - r5;
            } else if (r5 == 0) {
                switch (gWeatherManager.transitionStep) {
                case 3:
                case 4:
                    r5 = 0x20 - r5;
                    break;
                }
            }
        }
        if (r5 <= 0) {
            result = TRUE;
            goto end;
        }
        r7 = sSkyNextCloudBgLayer[idx];
        if (r3 == 0) {
            r4 = r2 + 1;
            stk4 = 1;
        } else {
            r4 = r2 - 1;
            stk4 = 2;
        }
        switch (gWeatherManager.transitionStep) {
        case 3:
        case 4:
            if (r1 == r2) {
                r4 = r2;
            }
            break;
        }
        if (gWeatherManager.loadCloudChars(r4, r7)) {
            if (gWeatherManager.loadCloudScreen((u8 **)(&gWeatherManager.cloudScreen), (u32 *)(&gWeatherManager.cloudScreenSize), stk4, r4)) {
                u8 *buf = gWeatherManager.cloudScreen;
                u32 off, n;
                if (r4 == gWeatherManager.level) {
                    n = r5 << 5;
                    off = (0x20 - r5) << 5;
                    buf += off;
                } else {
                    n = r5 << 5;
                    off = 0;
                }
                if (Gfx2d_LoadScreen(buf, r7, n, off)) {
                    result = TRUE;
                }
            }
        }
    } else {
        result = TRUE;
    }
end:
    return result;
}

extern "C" BOOL Sky_LoadStarBg(u32 idx) {
    Heap *heap = gCurrentHeap;
    u8 v6 = sSkyStarBgLayer[idx];
    u8 *buf;
    if (!Gfx2d_LoadCharFile(data_020e676c, heap, v6, 0x10, 0x10, 0x1f)) {
        return FALSE;
    }
    buf = (u8 *)Heap_AllocTail(heap, 0x1000);
    if (buf == NULL) {
        return FALSE;
    }
    File_LoadToBuffer(data_020e6780, buf, 0x1000);
    Constellation_MaskSkyScreen((u16 *)buf);
    if (!Gfx2d_LoadScreen(buf, v6, 0x1000, 0)) {
        Heap_Free(heap, buf);
        return FALSE;
    }
    Heap_Free(heap, buf);
    StarTwinkle_Init(&data_021efc18, v6);
    return TRUE;
}

extern "C" void Sky_ResetObjGfx(void) {
    SkyObjGfxSlot *p = gSkySprites.gfxSlots;
    s32 i;
    for (i = 0; i < 5; p++, i++) {
        p->loaded = 0;
    }
    gSkySprites.palette.invalidate(-1);
    gSkySprites.reloadAllGfx();
    gSkySprites.initClock();
}

extern "C" BOOL Sky_SetEngine(s32 arg) {
    BOOL result = FALSE;
    MinuteHour t;
    if (arg == 0) {
        Gfx2d_SetSubBgModeState(1);
        Clock_GetMinuteHour(&t.minute);
        if (t.hour >= 6 && t.hour < 0x12) {
            Gfx2d_HideSubPlanes(2);
        } else {
            Gfx2d_ShowSubPlanes(2);
        }
        Gfx2d_ShowSubPlanes(0x10);
        Gfx2d_ShowSubPlanes(8);
        vu16 *ra = (vu16 *)0x400100a;
        vu16 *rb = (vu16 *)0x400100e;
        IO_W(ra, (IO_R(ra) & ~3) | 3);
        IO_W(rb, (IO_R(rb) & ~3) | 2);
        IO_W(ra, (IO_R(ra) & 0x43) | 0x4c00);
        IO_W(rb, (IO_R(rb) & 0x43) | 0x6f00);
        HBlank_Replace(&sSkyHBlankTask, Sky_HBlankSub, Sky_VBlankSub);
    } else {
        Gfx2d_SetMainBgModeState(1);
        Clock_GetMinuteHour(&t.minute);
        if (t.hour >= 6 && t.hour < 0x12) {
            Gfx2d_HideMainPlanes(2);
        } else {
            Gfx2d_ShowMainPlanes(2);
        }
        Gfx2d_ShowMainPlanes(0x10);
        Gfx2d_ShowMainPlanes(8);
        vu16 *ra = (vu16 *)0x400000a;
        vu16 *rb = (vu16 *)0x400000e;
        IO_W(ra, (IO_R(ra) & ~3) | 3);
        IO_W(rb, (IO_R(rb) & ~3) | 2);
        IO_W(ra, (IO_R(ra) & 0x43) | 0x4400);
        IO_W(rb, (IO_R(rb) & 0x43) | 0x6700);
        HBlank_Replace(&sSkyHBlankTask, Sky_HBlankMain, Sky_VBlankMain);
    }
    if (arg != gWeatherManager.engine || arg == 1) {
        if (Sky_LoadCloudBg(arg) && Sky_LoadNextCloudBg(arg) && Sky_LoadStarBg(arg)) {
            gWeatherManager.engine = arg;
            Sky_ResetObjGfx();
            result = TRUE;
        }
    }
    Actor *p = (Actor *)PlayerActor_GetActor(4);
    gWeatherManager.prevPlayerZ = 0;
    if (p != NULL) {
        VecFx32 *pos = &p->position;
        if (pos != NULL) {
            gWeatherManager.prevPlayerZ = pos->z;
        }
    }
    return result;
}

extern "C" void Sky_Disable(void) {
    HBlank_Replace(&sSkyHBlankTask, Sky_HBlankNone, Sky_VBlankDisabled);
    Gfx2d_HideMainPlanes(0xa);
}

extern "C" s32 Weather_GetFallingPrecip(void) {
    switch (gWeatherManager.level) {
    case 2:
    case 3:
    case 4:
        switch (gWeatherManager.precipKind) {
        case 1:
            return 1;
        case 2:
            return 2;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

extern "C" s32 Weather_GetCurrent(void) { return gWeatherManager.getWeatherAtOffset(0); }

extern "C" void Weather_GetLevels(s32 *a, s32 *b) {
    *a = gWeatherManager.level;
    *b = gWeatherManager.targetLevel;
}

extern "C" u32 Weather_GetPrecipKind(void) { return gWeatherManager.precipKind; }

extern "C" u16 *Sky_GetCurrentPalette(void) {
    if (gWeatherManager.targetLevel != gWeatherManager.level) {
        return gWeatherManager.palettes[2];
    }
    return gWeatherManager.palettes[0];
}

extern "C" u32 Sky_GetStarScrollX(void) { return gWeatherManager.starScrollX; }

extern "C" u32 Sky_GetStarScrollY(void) { return gWeatherManager.starScrollY; }

extern "C" void Sky_GetStarViewingTime(ClockDateTime *out, s32 a, s32 b) {
    ClockDateTimeWords tmp;
    s32 r7;
    tmp.words[0] = 0;
    tmp.words[1] = 0;
    Clock_GetDateTime(&tmp.dt);
    Sky_CalcStarScrollY(&gWeatherManager, &tmp.dt);
    DateTime_SubHours(&tmp.dt, 6);
    r7 = Sky_CalcStarScrollX(&gWeatherManager, &tmp.dt);
    MI_CpuCopy8(&tmp, out, 8);
    if (a < 0) {
        a += 0x200;
    }
    if (a < r7) {
        a += 0x200;
    }
    DateTime_AddDays(out, (a - r7) / 8);
    if (b < 0) {
        b = 0;
    } else if (b > 0x68) {
        b = 0x68;
    }
    out->hour = 0x12;
    out->minute = 0;
    out->second = 0;
    DateTime_AddMinutes(out, b * 0x2d0 / 0x68);
    if (out->hour == 6) {
        DateTime_SubHours(out, 1);
    }
    Clock_GetDateTime(&tmp.dt);
    DateTime_AddMinutes(&tmp.dt, 0x1e);
    if (DateTime_Compare(&tmp.dt, out, 0x3c) == 1) {
        DateTime_AddDays(out, 0x40);
    }
}

extern "C" void Weather_RerollRainSlantAlt3(void) { gWeatherManager.rollRainSlant(); }

extern "C" void Weather_RerollRainSlantAlt2(void) { gWeatherManager.rollRainSlant(); }

extern "C" void Weather_RerollRainSlantAlt(void) { gWeatherManager.rollRainSlant(); }

extern "C" void Weather_RerollRainSlant(void) { gWeatherManager.rollRainSlant(); }

extern "C" void Sky_OnDayChange(u32 x) { gSkySprites.onDayChange(x); }

extern "C" void Sky_OnSlingshotFired(u32 x) { gSkySprites.onSlingshotFired(x); }

extern "C" void Sky_SwapBuffers(void) {
    gWeatherManager.bufferIndex ^= 1;
    gSkySprites.uploadDirtyGfx();
}

extern "C" void Sky_EndBalloonDrop(void) { gSkySprites.shotSeq.balloonDropEnded = 1; }

extern "C" void Sky_PlayBalloonDropSe(BOOL x) {
    if (x) {
        gSkySprites.shotSeq.splashSePending = 1;
    } else {
        gSkySprites.shotSeq.landSePending = 1;
    }
}

extern "C" u32 Sky_IsShootingStarVisible(void) { return gSkySprites.shootingStarVisible; }

extern "C" void Sky_WishOnShootingStar(void) {
    PlayerData *p = PlayerData_GetCurrent();
    if (p != NULL) {
        p->setFlag(0x32);
        SkySprites_OnStarWish(&gSkySprites);
    }
}

extern "C" void Sky_RequestBirds(void) { gSkySprites.birdsRequested = 1; }


extern "C" {
GXOamAttr data_020e48dc[1] = {{0x91f000f0, 0x11c, 0, 0x2, 0xffff}};
const s32 sFireworkFlicker[2] = {0x1000, 0x800};
const u8 sSkyPaletteLayer[4] = {
    0x6, 0x2, 0x0, 0x0,
};
const u8 sFireworksPatternWeights[14] = {0x0, 0x5, 0xa, 0xf, 0x19, 0x23, 0x2d, 0x37, 0x41, 0x4b, 0x51, 0x59, 0x5f, 0x64};
char data_020e4f68[30] = "/sky/d_2d_b_cld_f0_bg_ncl.bin";
}

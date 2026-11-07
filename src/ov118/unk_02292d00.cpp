#include "ds_div.h"
#include "host_labels.h"
#include "host_shared_calls.h"
#include "talk/MsgString9C.h"
#include "types.h"
#include "gfx/VecFx32.h"
#include "Unk_020d8c7c.h"
#include "town/TownMapMarkers.h"
#include "ui/UiWidget.h"
#include "menu/MenuProc.h"
#include "ui/ScrollKnob.h"
#include "ui/LabelString.h"
#include "gfx/BgVramTask.h"
#include "menu/MenuCursor.h"
#include "menu/MenuScrollKnob.h"
#include "menu/MenuTabBar.h"
#include "sys/ProcProfile.h"
#include "talk/MsgString.h"
#include "ui/HandCursor.h"
#include "proto/main/unk_020b4828.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/autoload_2/unk_020ec848.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_02063f3c.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_0206f834.h"
#include "proto/main/unk_020943dc.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_020b7d84.h"
#include "proto/ov002/unk_ov002_02200840.h"
#include "proto/ov002/unk_ov002_022013ac.h"
#include "proto/ov090/unk_ov090_022918e0.h"
#include "proto/ov094/unk_ov094_02292360.h"
#include "proto/ov117/unk_ov117_02292360.h"
#include "proto/ov118/unk_02292d00.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/main/unk_02087e70.h"
#include "proto/main/unk_020b2c50.h"
#include "proto/main/unk_020637e4.h"
#include "proto/ov002/unk_ov002_022025cc.h"
#include "player/PlayerData.h"
#ifdef N3DS_DIRECT
#include "n3ds_ui.h"
// the options button as a hand target in the tab row, left of the pockets' tab (0x10 is the scroll knob): a tab target
// whose tab is N3DS_OPT_TAB
#define MAP_OPT_TARGET 0x11
#endif

extern "C" {
extern u16 gPad[];
extern u8 gFieldSceneKind;
#define gSaveTownId HOST_LABEL(u8 (*)[], gSaveTownId)
#define gSavePlayers HOST_LABEL(u8 (*)[], gSavePlayers)
#define gSaveVillagers HOST_LABEL(u8 (*)[], gSaveVillagers)

s32 _s32_div_f(s32 a, s32 b);
BOOL SaveVillagers_IsOccupied(void *a, s32 b);
void MIi_CpuCopy16(void *dst, void *src, u32 n);
void MIi_CpuClear16(u32 v, void *dst, u32 n);
void Math_StepS32(void *p, s32 a, s32 b);
}













// 3-byte element with empty out-of-line ctor and dtor
class MapTabMarker {
public:
    MapTabMarker();
    ~MapTabMarker();
    u8 x;
    u8 y;
    u8 cell;
};

#define A8V (*(volatile u8 *)&blinkTimer)
#define ACV (*(volatile u8 *)&cursorTarget)

class MapTab;
typedef void (MapTab::*Unk_ov118_022955c8_Fn)();

// Vtable 0x022955c8, size 0x459c
class MapTab : public MenuProc {
public:
    MapTab() : screenTasks(), textLabels(), scrollKnob(), cursor(), townMarkers(), terrainMarkers(), buildingMarkers() {}

    virtual BOOL onCreate();
    virtual BOOL onDelete();
    virtual BOOL onDraw();
    virtual BOOL execTransition();
    virtual BOOL execMain();
    virtual BOOL execPhase3();
    virtual BOOL execPhase4();
    virtual BOOL execClosed();

    void clearFlags(u32 m);
    void setFlags(u32 m);
    BOOL testFlags(u32 m);
    void playSeAtMarker(s32 a);
    s32 onCursorDecide();
    s32 moveCursorByPad(u32 pad);
    void refreshCursor();
    void releaseCursor();
    void pressCursor();
    void moveCursorToTarget();
    void hideCursor();
    s32 getCursorTargetY();
    s32 getCursorTargetX();
    void showCursor();
    void stopBlink();
    void startBlink();
    void updateBlink();
    s32 touchListTabs();
    void scrollListByPad();
    void scrollListToKnob(s32 v);
    BOOL touchScrollBar();
    void updateListScroll();
    BOOL isListScrolling();
    void setListScrollTarget(u32 v);
    void setListScroll(u32 v);
    void ensureSelectionVisible();
    void jumpToSelection();
    u8 selectionToMarker(u32 v);
    u8 selectionToListRow(u32 v);
    void highlightListRow(u32 v);
    void selectEntry(u8 v);
    u8 findMarkerAt(s32 x, s32 y);
    BOOL touchMarker();
    void setupMarkers(TownMapMarkers *p);
    void drawMapIcon(s32 x, s32 y, s32 n, s32 flag, s32 pal);
    BOOL touchListRow();
    s32 getListScrollMax();
    u8 getListCount();
    u8 *getListEntries();
    void rebuildList();
    void showPlacesList();
    void showResidentsList();
    void setEntryName(LabelString *p, u32 idx);
    void buildEntryLabels(u8 *tbl);
    void layoutListLabels();
    void buildEntryLists();
    LabelString *allocTextLabel();
    void resetTextLabels();
    s32 getEntryIconRow(u32 x, s32 idx);
    void buildListScreen();
    void clipListScreen();
    void exitMapCursor();
    void startButtonInput();
    void startTouchInput();
    void updateMapCursor();
    void updateKnobRelease();
    void updateKnobHold();
    void updateKnobGrab();
    void updateCursorRelease();
    void updateCursorPress();
    void updateCursorMove();
    void updateButtons();
    void updateBarTap();
    void updateKnobDrag();
    void updateTouch();
    void loadObjGfx();
    void loadMapImage();
    void loadMapStep2();
    void loadMapStep1();
    void loadBgGfx();
    void setupBgLayers();
    void postInputUpdate();
    void preInputUpdate();
    void postStateUpdate();
    void preStateUpdate();
    void releaseResources();
    void initMapTab();
    void stateClosing();
    void stateClose();
    void stateOpening();
    void stateOpen();
    void stateBuildList();
    void stateLoadMap3();
    void stateLoadMap2();
    void stateLoadMap1();
    void stateLoadBg();
    BOOL requestTab(s32 x);
    BOOL handleTabSwitch();
    void runMainState();

    /* 0x0091 */ u8 unk_91[3];
    /* 0x0094 */ s32 slideY;
    /* 0x0098 */ u16 listScroll;
    /* 0x009a */ u16 listScrollTarget;
    /* 0x009c */ u16 flags;
    /* 0x009e */ u8 selfMarkerX;
    /* 0x009f */ u8 selfMarkerY;
    /* 0x00a0 */ volatile u8 labelCount;
    /* 0x00a1 */ u8 residentCount;
    /* 0x00a2 */ u8 placeCount;
    /* 0x00a3 */ u8 listTopRow;
    /* 0x00a4 */ u8 knobPos;
    /* 0x00a5 */ u8 dragStartTouchY;
    /* 0x00a6 */ u8 knobLastTickPos;
    /* 0x00a7 */ u8 dragStartKnobPos;
    /* 0x00a8 */ u8 blinkTimer;
    /* 0x00a9 */ u8 selectedMarker;
    /* 0x00aa */ u8 selectedRow;
    /* 0x00ab */ u8 returnState;
    /* 0x00ac */ u8 cursorTarget;
    /* 0x00ad */ u8 cursorMarker;
    /* 0x00ae */ u8 mapCursorX;
    /* 0x00af */ u8 mapCursorY;
    /* 0x00b0 */ u8 playerEntryCount;
    /* 0x00b1 */ u8 selfBlinkCounter;
    /* 0x00b2 */ u8 unk_b2[2];
    /* 0x00b4 */ BgVramTask screenTasks[2];
    /* 0x00fc */ LabelString textLabels[13];
    /* 0x043c */ MenuScrollKnob scrollKnob;
    /* 0x0484 */ MenuCursorBuf0 cursor;
    /* 0x04e8 */ u16 frameScreen[0x400];
    /* 0x0ce8 */ u16 listScreen[0x400];
    /* 0x14e8 */ u16 listScreenWork[0x400];
    /* 0x1ce8 */ u16 listScreenBase[0x400];
    /* 0x24e8 */ u8 mapChars[0x2000];
    /* 0x44e8 */ TownMapMarkers townMarkers;
    /* 0x454e */ u8 residentEntries[13];
    /* 0x455b */ u8 placeEntries[13];
    /* 0x4568 */ MapTabMarker terrainMarkers[3];
    /* 0x4571 */ MapTabMarker buildingMarkers[14];
};





extern "C" ProcProfile sMapTabProfile;
extern "C" GXOamAttr sMapTabIconCell;
extern "C" const u8 sMapTabPlaceMarkers[5];
extern "C" const u8 sMapTabMarkerPlaceRows[5];
extern "C" const u8 sMapTabFacilityCells[5];
extern "C" GXOamAttr sMapTabPlacesButtonCells[4];
extern "C" GXOamAttr sMapTabResidentsButtonCells[4];
extern "C" GXOamAttr sMapTabFrameCells[10];





static inline BOOL Unk_ov118_022946d4_Both() {
    if (gTouchHeld != 0 && gTouchChanged != 0) return TRUE;
    return FALSE;
}

static inline BOOL Unk_ov118_02294a58_IsZero(u8 v) {
    return v == 0 ? TRUE : FALSE;
}

extern "C" MapTab *MapTab_Create() { return new MapTab(); }

BOOL MapTab::onCreate() {
    initMapTab();
    setTransitionState(0);
    setPhase(1);
    return TRUE;
}

BOOL MapTab::onDelete() {
    ((MenuTabBar *)ProcBase_GetParent(this))->onTabMenuClosed();
    releaseResources();
    return TRUE;
}

BOOL MapTab::onDraw() {
    u32 base = slideY + 0x60;
    s32 i;
    if (testFlags(4)) {
        if (testFlags(8)) {
            scrollKnob.moveTo(0x67, (slideY - 0x12) + knobPos);
        }
        if (MenuCtrl_IsButtons()) {
            if (testFlags(0x1000)) {
                s32 a = scrollKnob.getGripX();
                s32 b = scrollKnob.getGripY();
                ((MenuCursorBase *)&cursor)->warpTo(a, b);
            }
            ((MenuCursorBase *)&cursor)->drawWrapped();
        }
        Oam_DrawCell(1, &sMapTabFrameCells[0], 0x80, base, -1, 2, 0x1000, 0x1000, 0, -1, 0, 0);
        s32 x0, x1;
        if (testFlags(1)) {
            x1 = 0xc;
            x0 = 0xd;
        } else {
            x1 = 0xb;
            x0 = 0xe;
        }
        Oam_DrawCell(1, &sMapTabPlacesButtonCells[0], 0x80, base, x0, 2, 0x1000, 0x1000, 0, -1, 0, 0);
        Oam_DrawCell(1, &sMapTabResidentsButtonCells[0], 0x80, base, x1, 2, 0x1000, 0x1000, 0, -1, 0, 0);
        if (MenuCtrl_IsButtons() && testFlags(0x800)) {
            u32 t = cursorMarker;
            if (t != 0xe) {
                MapTabMarker *e = &buildingMarkers[t];
                if (e->cell != 0xc) {
                    drawMapIcon(e->x, slideY + e->y, 0xb, 0, -1);
                }
            }
        }
        selfBlinkCounter = (selfBlinkCounter + 1) & 0xf;
        if ((selfBlinkCounter & 0xc) != 0) {
            drawMapIcon(selfMarkerX, selfMarkerY + slideY, 0xa, 0, -1);
        }
        for (i = 0; i < 3; i++) {
            MapTabMarker *e = &terrainMarkers[i];
            u32 c = e->cell;
            if (c != 0xc) {
                drawMapIcon(e->x, slideY + e->y, (u8)(c & 0x7f), (c & 0x80) != 0 ? 1 : 0, -1);
            }
        }
        for (i = 0; i < 14; i++) {
            MapTabMarker *e = &buildingMarkers[i];
            u8 *q = &e->cell;
            if (*q != 0xc) {
                s32 f;
                if (i == selectedMarker && !testFlags(0x100)) {
                    f = 8;
                } else {
                    f = -1;
                }
                drawMapIcon(e->x, slideY + e->y, *q, 0, f);
            }
        }
        if (testFlags(8)) {
            scrollKnob.draw();
        }
    }
    return TRUE;
}

extern "C" GXOamAttr sMapTabResidentsButtonCells[4] = {{0x404d00d3, 0x1a5, 0, 0xc, 0x0}, {0x5d80d3, 0x1a7, 0, 0xc, 0x0}, {0x4d40e3, 0x1e5, 0, 0xc, 0x0}, {0x5d00e3, 0x1e7, 0, 0xc, 0xffff}};

extern "C" const u8 sMapTabMarkerPlaceRows[5] = {0x02, 0x03, 0x00, 0x01, 0x04};

extern "C" GXOamAttr sMapTabIconCell = {0x41f800f8, 0xc0, 0, 0x7, 0xffff};

extern "C" const u8 sMapTabPlaceMarkers[5] = {0x0b, 0x0c, 0x09, 0x0a, 0x0d};

BOOL MapTab::execTransition() {
    static Unk_ov118_022955c8_Fn tbl[9] = {&MapTab::stateLoadBg, &MapTab::stateLoadMap1, &MapTab::stateLoadMap2, &MapTab::stateLoadMap3, &MapTab::stateBuildList, &MapTab::stateOpen, &MapTab::stateOpening, &MapTab::stateClose, &MapTab::stateClosing};
    preStateUpdate();
    (this->*tbl[transitionState])();
    postStateUpdate();
    return TRUE;
}

extern "C" GXOamAttr sMapTabPlacesButtonCells[4] = {{0x402a00d3, 0x1a2, 0, 0xe, 0x0}, {0x3a80d3, 0x1a4, 0, 0xe, 0x0}, {0x2a40e3, 0x1e2, 0, 0xe, 0x0}, {0x3a00e3, 0x1e4, 0, 0xe, 0xffff}};

void MapTab::runMainState() {
    static Unk_ov118_022955c8_Fn tbl[11] = {&MapTab::updateTouch, &MapTab::updateKnobDrag, &MapTab::updateBarTap, &MapTab::updateButtons, &MapTab::updateCursorMove, &MapTab::updateCursorPress, &MapTab::updateCursorRelease, &MapTab::updateKnobGrab, &MapTab::updateKnobHold, &MapTab::updateKnobRelease, &MapTab::updateMapCursor};
    (this->*tbl[mainState])();
}

extern "C" ProcProfile sMapTabProfile = {(void *(*)())MapTab_Create, 0xa1, 0xa5};

extern "C" const u8 sMapTabFacilityCells[5] = {0x02, 0x03, 0x04, 0x05, 0x06};

extern "C" GXOamAttr sMapTabFrameCells[10] = {{0x819d40b8, 0x1ab, 0, 0x6, 0x0}, {0x81bd40b8, 0x1af, 0, 0x6, 0x0}, {0x81dd40b8, 0x1b3, 0, 0x6, 0x0}, {0x81fd40b8, 0x1b7, 0, 0x6, 0x0}, {0x401d00b8, 0x1bb, 0, 0x6, 0x0}, {0x81f540b8, 0xd9, 0, 0x6, 0x0}, {0x81d540b8, 0xd9, 0, 0x6, 0x0}, {0x81b540b8, 0xd9, 0, 0x6, 0x0}, {0x901540b8, 0xd8, 0, 0x6, 0x0}, {0x819540b8, 0xd8, 0, 0x6, 0xffff}};

BOOL MapTab::execMain() {
    if (handleTabSwitch()) {
        return TRUE;
    }
    preInputUpdate();
    runMainState();
    postInputUpdate();
    return TRUE;
}

BOOL MapTab::execPhase3() { return TRUE; }

BOOL MapTab::execPhase4() { return TRUE; }

BOOL MapTab::execClosed() {
    ProcBase_RequestDelete(this);
    return TRUE;
}

BOOL MapTab::handleTabSwitch() {
    MenuCtrl_TickForceClose();
    if (MenuCtrl_IsForceCloseDue()) {
        return requestTab(7);
    }
    if (mainState != 0 && mainState != 3 && mainState != 10) {
        return FALSE;
    }
    s32 r = -1;
    if (MenuCtrl_IsTouch()) {
        r = MenuTabBar_HitTestTouch();
    } else {
        u16 k = gPad[1];
        if ((k & 0x400) != 0 || (k & 2) != 0) {
            r = 7;
        } else if ((k & 0x800) != 0) {
            r = 0;
        } else if ((k & 4) != 0) {
            r = 4;
        }
    }
    return requestTab(r);
}

BOOL MapTab::requestTab(s32 x) {
    void *r = ProcBase_GetParent(this);
    if (x != -1 && x != 5) {
        ((MenuTabBar *)r)->selectTab((u8)x);
        transitionState = 7;
        setPhase(1);
        return TRUE;
    }
    return FALSE;
}

void MapTab::stateLoadBg() {
    setupBgLayers();
    loadBgGfx();
    setTransitionState(1);
    if (!testFlags(0x2000)) {
        stateLoadMap1();
    }
}

void MapTab::stateLoadMap1() {
    loadMapStep1();
    setTransitionState(2);
    if (!testFlags(0x2000)) {
        stateLoadMap2();
    }
}

void MapTab::stateLoadMap2() {
    loadMapStep2();
    setTransitionState(3);
    if (!testFlags(0x2000)) {
        stateLoadMap3();
    }
}

void MapTab::stateLoadMap3() {
    loadMapImage();
    setTransitionState(4);
    if (!testFlags(0x2000)) {
        stateBuildList();
        clearFlags(0x2000);
    }
}

void MapTab::stateBuildList() {
    showResidentsList();
    clipListScreen();
    setTransitionState(5);
}

void MapTab::stateOpen() {
    loadObjGfx();
    {
        MsgString9C name;
        TownId_GetNameString(gSaveTownId, &name);
        String_SetSlot(0, &name);
        LabelString *o = allocTextLabel();
        o->createLabel(8, 0x1ab, 0x12, 0xf, 0, 0);
        String_Load2dMenu(o, 0xa9);
        o->redrawAligned(1, 0);
    }
    beginSubSlideIn(0xa, 3, 0, 0x30);
    Gfx2d_ShowLayer(4);
    applySlideOffset(4, 0, 0);
    Gfx2d_ShowLayer(6);
    applySlideOffset(6, 0, 0x50 - listScroll);
    setFlags(4);
    slideY = getSlideOffsetY();
    setTransitionState(6);
}

void MapTab::stateOpening() {
    if (stepSlideIn(0)) {
        setPhase(2);
        if (MenuCtrl_IsTouch()) {
            startTouchInput();
        } else {
            startButtonInput();
        }
    }
    applySlideOffset(4, 0, 0);
    applySlideOffset(6, 0, 0x50 - listScroll);
    slideY = getSlideOffsetY();
}

void MapTab::stateClose() {
    hideCursor();
    beginSubSlideOut(0xa, 0, 0, 0x30);
    applySlideOffset(4, 0, 0);
    applySlideOffset(6, 0, 0x50 - listScroll);
    setTransitionState(8);
    slideY = getSlideOffsetY();
}

void MapTab::stateClosing() {
    if (stepSlideOut(0)) {
        Gfx2d_ResetLayer(4);
        Gfx2d_ResetLayer(6);
        clearFlags(4);
        setPhase(5);
    } else {
        applySlideOffset(4, 0, 0);
        applySlideOffset(6, 0, 0x50 - listScroll);
        slideY = getSlideOffsetY();
    }
}

void MapTab::initMapTab() {
    volatile VecFx32 v;
    VecFx32 *p;
    labelCount = 0;
    flags = 0;
    listScroll = 0;
    listScrollTarget = 0;
    knobPos = 0;
    slideY = 0;
    blinkTimer = 0;
    cursorTarget = 8;
    mapCursorX = 0x58;
    mapCursorY = 0x70;
    cursorMarker = 0xe;
    selectEntry(0);
    buildEntryLists();
    if (Unk_ov118_02294a58_IsZero(gFieldSceneKind)) {
        p = (VecFx32 *)PlayerActor_GetBodyPos(4);
        v.x = p->x;
        v.y = p->y;
        v.z = p->z;
    } else {
        Scene_GetWarpRequest();
        p = ScenePos_GetPos(&gTownReturnPos);
        v.x = p->x;
        v.y = p->y;
        v.z = p->z;
    }
    s32 z = (v.z + 0x800) >> 12;
    selfMarkerX = ((v.x + 0x800) >> 12) - 8;
    selfMarkerY = z + 13;
    scrollKnob.setState(1);
    if (((MenuTabBar *)ProcBase_GetParent(this))->isJustOpened()) {
        setFlags(0x2000);
    }
}

void MapTab::releaseResources() {
    resetTextLabels();
    screenTasks[0].cancel();
    screenTasks[1].cancel();
}

void MapTab::preStateUpdate() {
    resetTextLabels();
}

void MapTab::postStateUpdate() {
    if (isListScrolling()) {
        updateListScroll();
        Gfx2d_SetLayerOffset(6, 0, listScroll - 0x50);
        if (listTopRow != (listScroll >> 4)) {
            setFlags(0x80);
        }
    }
    updateBlink();
    if (testFlags(0x80)) {
        clipListScreen();
        clearFlags(0x80);
    }
    if (testFlags(0x20)) {
        if (screenTasks[1].requestScreen(frameScreen, 4, 0x800, 0)) {
            clearFlags(0x20);
        }
    }
    if (testFlags(2)) {
        if (screenTasks[0].requestScreen(listScreen, 6, 0x800, 0)) {
            clearFlags(2);
        }
    }
}

void MapTab::preInputUpdate() {
    preStateUpdate();
    scrollKnob.update();
    cursor.update();
}

void MapTab::postInputUpdate() {
    postStateUpdate();
}

void MapTab::setupBgLayers() {
    Gfx2d_SetLayerPriority(4, 2);
    Gfx2d_SetLayerPriority(6, 2);
    Gfx2d_SetLayerControl(4, 0, 0, 0);
    Gfx2d_SetLayerControl(6, 0, 0, 0);
}

void MapTab::loadBgGfx() {
    void *p = gCurrentHeap;
    Gfx2d_LoadPaletteFile("menu/map/b_map_bg.bpl", p, 4, 1, 1, 0xf);
    Gfx2d_LoadCharFile("menu/map/b_map_bg_0.bch", p, 4, 0x11, 0x11, 0x5f);
    Gfx2d_LoadCharFile("menu/map/b_map_bg_1.bch", p, 4, 0x230, 0x230, 0x25f);
    File_LoadToBuffer("menu/map/b_map_a_bg.bsc", frameScreen, 0x800);
    Gfx2d_LoadScreen((u8 *)frameScreen, 4, 0x800, 0);
    File_LoadToBuffer("menu/map/b_map_b_bg.bsc", listScreenBase, 0x800);
    BgScreen_SetRectPalette(listScreenBase, 0x13, 0, 0x1c, 1, 4);
}

void MapTab::loadMapStep1() {
    Shared_TownMapImage_AllocGlobal();
    TownMapImage_DrawRowGlobal(0);
    Shared_TownMapImage_BuildMarkersGlobal(0);
    Shared_TownMapImage_BuildMarkersGlobal(1);
}

void MapTab::loadMapStep2() {
    TownMapImage_DrawRowGlobal(1);
    TownMapImage_DrawRowGlobal(2);
}

void MapTab::loadMapImage() {
    u8 *a;
    TownMapMarkers *b;
    TownMapImage_DrawRowGlobal(3);
    a = mapChars;
    Shared_TownMapMarkers_Clear(&townMarkers);
    b = &townMarkers;
    Shared_TownMapImage_FinishGlobal(a, b); // ov117 (the decompiler named ov094's function at the same address)
    Gfx2d_LoadCharRange(a, 4, 0x60, 0x60, 0x15f);
    setupMarkers(b);
}

void MapTab::loadObjGfx() {
    void *p = gCurrentHeap;
    Gfx2d_LoadPaletteFile("menu/map/b_map_obj.bpl", p, 8, 6, 6, 0xe);
    Gfx2d_LoadCharFile("menu/map/b_map_obj_0.bch", p, 8, 0xc0, 0xc0, 0xff);
    Gfx2d_LoadCharFile("menu/map/b_map_obj_1.bch", p, 8, 0x180, 0x180, 0x1ff);
}

void MapTab::updateTouch() {
    if (checkSwitchToButtons(1)) {
        startButtonInput();
        return;
    }
    if (Unk_ov118_022946d4_Both()) {
        if (touchListTabs()) {
            Snd_PlaySe(0x36);
        } else if (touchScrollBar()) {
            scrollKnob.grab();
        } else if (touchMarker()) {
            playSeAtMarker(0x38);
        } else if (touchListRow()) {
            playSeAtMarker(0x37);
        }
    }
}

void MapTab::updateKnobDrag() {
    if (gTouchHeld == 0) {
        scrollKnob.setState(3);
        startTouchInput();
    }
    scrollListToKnob(dragStartKnobPos + (gTouchCurY - dragStartTouchY));
}

void MapTab::updateBarTap() {
    u32 v;
    if (gTouchHeld == 0) {
        scrollKnob.setState(3);
        startTouchInput();
    }
    v = knobPos;
    Math_StepS32(&v, gTouchCurY - 0x54, 8);
    scrollListToKnob(v);
}

void MapTab::updateButtons() {
    if (checkSwitchToTouch()) {
        startTouchInput();
        return;
    }
    switch (moveCursorByPad(takeRepeatedKeys())) {
    case 1:
        if (testFlags(8)) {
            u8 t = cursorTarget;
            if (t >= 0xa && t <= 0xf) {
                setListScrollTarget(listScroll & ~0xf);
            }
        }
        moveCursorToTarget();
        break;
    case 2:
        selectEntry(0);
        highlightListRow(0xe);
        cursorMarker = findMarkerAt(mapCursorX, mapCursorY);
        setFlags(0x800);
        setMainState(0xa);
        moveCursorToTarget();
        break;
    case 3:
        moveCursorToTarget();
        break;
    default: {
        u32 trg = gPad[1];
        if (trg & 1) {
            pressCursor();
        } else if (trg & 0x100) {
            requestTab(MenuTabBar_NextTab(5));
        } else if (trg & 0x200) {
            requestTab(MenuTabBar_PrevTab(5));
        }
        break;
    }
    }
}

void MapTab::updateCursorMove() {
    if (((MenuCursorBase *)&cursor)->isMoving() == 0) {
        setMainState(returnState);
        runMainState();
    }
}

void MapTab::updateCursorPress() {
    if (((HandCursor *)(&cursor))->isAnimDone()) {
        s32 r = onCursorDecide();
        if (r != 1) {
            if (r == 2) {
                setListScroll(0);
                listTopRow = 0xff;
                releaseCursor();
            } else {
                releaseCursor();
            }
        }
    }
}

void MapTab::updateCursorRelease() {
    if (((HandCursor *)(&cursor))->isAnimDone()) {
        refreshCursor();
        if (testFlags(0x800)) {
            setMainState(0xa);
        } else {
            setMainState(3);
        }
    }
}

void MapTab::updateKnobGrab() {
    if (scrollKnob.areAnimsDone()) {
        setMainState(8);
    }
}

void MapTab::updateKnobHold() {
    if ((gPad[0] & 1) == 0) {
        scrollKnob.setState(3);
        setMainState(9);
    } else {
        scrollListByPad();
    }
}

void MapTab::updateKnobRelease() {
    if (scrollKnob.areAnimsDone()) {
        releaseCursor();
        clearFlags(0x1000);
    }
}

void MapTab::updateMapCursor() {
    s32 trg, x, y, cur, ox, oy;
    if (checkSwitchToTouch()) {
        startTouchInput();
        return;
    }
    trg = gPad[1];
    if ((trg & 1) && cursorMarker != 0xe) {
        pressCursor();
        return;
    }
    if (trg & 0x100) {
        requestTab(MenuTabBar_NextTab(5));
        return;
    }
    if (trg & 0x200) {
        requestTab(MenuTabBar_PrevTab(5));
        return;
    }
    ox = mapCursorX;
    x = ox;
    oy = mapCursorY;
    y = oy;
    cur = gPad[0];
    if (cur & 0x40) {
        y = oy - 4;
    } else if (cur & 0x80) {
        y = oy + 4;
    }
    if (y < 0x30) {
        cursorTarget = MenuTabBar_TabFromX(ox);
        ((MenuCursor *)&cursor)->switchToAnim0D();
        exitMapCursor();
        return;
    }
    if (y > 0xb0) y = 0xb0;
    if (cur & 0x20) {
        x = x - 4;
    } else if (cur & 0x10) {
        x = x + 4;
    }
    if (x < 0x18) {
        x = 0x18;
    } else if (x > 0x98) {
        if (y < 0x50) {
            cursorTarget = 9;
        } else {
            y = (y - 0x50) >> 4;
            if (y >= getListCount()) {
                y = getListCount() - 1;
            }
            y += 0xa;
            cursorTarget = y;
        }
        exitMapCursor();
        return;
    }
    if (x == ox && y == oy) return;
    mapCursorX = x;
    mapCursorY = y;
    cursorMarker = findMarkerAt(mapCursorX, mapCursorY);
    ((MenuCursorBase *)&cursor)->warpTo(mapCursorX, mapCursorY);
}

void MapTab::startTouchInput() {
    hideCursor();
    setMainState(0);
}

void MapTab::startButtonInput() {
    scrollKnob.setState(1);
    showCursor();
    restartKeyRepeat();
    if (testFlags(0x800)) {
        setMainState(0xa);
    } else {
        setMainState(3);
    }
}

void MapTab::exitMapCursor() {
    selectEntry(0);
    highlightListRow(0xe);
    clearFlags(0x800);
    setMainState(3);
    moveCursorToTarget();
}

void MapTab::clipListScreen() {
    volatile u16 v0, v1, v2, v3;
    s32 off, j, i, n;
    MIi_CpuCopy16(listScreenWork, listScreen, 0x800);
    n = listScroll >> 4;
    listTopRow = n;
    off = 0x13;
    for (i = 0; i < n; i++) {
        v0 = 0x10;
        MIi_CpuClear16(v0, listScreen + off, 0x14);
        v1 = 0x10;
        MIi_CpuClear16(v1, listScreen + (off + 0x20), 0x14);
        off += 0x40;
    }
    j = n + 7;
    off = j * 0x40 + 0x13;
    for (; j < 13; j++) {
        v2 = 0x10;
        MIi_CpuClear16(v2, listScreen + off, 0x14);
        v3 = 0x10;
        MIi_CpuClear16(v3, listScreen + (off + 0x20), 0x14);
        off += 0x40;
    }
    setFlags(2);
}

void MapTab::buildListScreen() {
    s32 i;
    u8 *tbl;
    MIi_CpuCopy16(listScreenBase, listScreenWork, 0x800);
    tbl = getListEntries();
    for (i = 0; i < 13; i++) {
        s32 a = getEntryIconRow(tbl[i], i) * 0x40 + 0x13;
        s32 b = i * 0x40 + 0x13;
        listScreenWork[b] = listScreenBase[a];
        listScreenWork[b + 1] = listScreenBase[a + 1];
        listScreenWork[b + 0x20] = listScreenBase[a + 0x20];
        listScreenWork[b + 0x21] = listScreenBase[a + 0x21];
    }
    setFlags(2);
}

s32 MapTab::getEntryIconRow(u32 x, s32 idx) {
    if (x == 0) return 7;
    if (x < 6) return 0;
    if (x >= 6 && x < 14) return 1;
    switch (x - 14) {
    case 2: return 2;
    case 0: return 3;
    case 3: return 4;
    case 1: return 5;
    case 4: return 6;
    }
    return 7;
}

void MapTab::resetTextLabels() {
    s32 i = 0;
    labelCount = 0;
    LabelString *p = textLabels;
    do {
        (p + i)->destroyLabel();
        i++;
    } while (i < 13);
}

LabelString *MapTab::allocTextLabel() {
    if (labelCount >= 13) {
        return &textLabels[12];
    }
    labelCount = labelCount + 1;
    return &textLabels[labelCount - 1];
}

void MapTab::buildEntryLists() {
    s32 n = 0;
    s32 m, i;
    m = PlayerDataArray_FindById(gSavePlayers, (u16 *)PlayerData_GetCurrent()->getPlayerId());
    if (m != -1) {
        residentEntries[0] = 1;
        n++;
    }
    for (i = 0; i < 4; i++) {
        if (i == m) {
            continue;
        }
        if (!PlayerDataArray_IsUsed(gSavePlayers, i)) {
            continue;
        }
        residentEntries[n] = i + 2;
        n++;
    }
    playerEntryCount = n;
    for (i = 0; i < 8; i++) {
        if (SaveVillagers_IsOccupied(gSaveVillagers, i)) {
            residentEntries[n] = i + 6;
            n++;
        }
    }
    residentCount = n;
    for (; n < 0xd; n++) {
        placeEntries[n] = 0;
    }
    n = 0;
    for (i = 0; i < 5; i++) {
        placeEntries[n] = i + 0xe;
        n++;
    }
    placeCount = n;
    for (; n < 0xd; n++) {
        placeEntries[n] = 0;
    }
}

void MapTab::layoutListLabels() {
    if (testFlags(1)) {
        buildEntryLabels(placeEntries);
    } else {
        buildEntryLabels(residentEntries);
    }
}

void MapTab::buildEntryLabels(u8 *tbl) {
    s32 i;
    for (i = 0; i < 0xd; i++) {
        LabelString *w = allocTextLabel();
        w->createLabel(6, (i << 4) + 0x160, 8, 1, 0xf, 0);
        setEntryName(w, tbl[i]);
        w->redrawAligned(0, 0);
    }
}

void MapTab::setEntryName(LabelString *p, u32 idx) {
    if (idx == 0) {
        ((MsgString *)(p))->clear();
    } else if (idx == 1) {
        PopupChoice_CopyPlayerIdName(p, PlayerData_GetCurrent()->getPlayerId());
    } else if (idx >= 2 && idx < 6) {
        PopupChoice_CopyResidentName(p, idx - 2);
    } else if (idx >= 6 && idx < 0xe) {
        PopupChoice_CopyVillagerName(p, idx - 6);
    } else if (idx >= 0xe && idx < 0x13) {
        String_Load2dMenu(p, idx + 0x88);
    } else {
        ((MsgString *)(p))->clear();
    }
}

void MapTab::showResidentsList() {
    clearFlags(1);
    rebuildList();
}

void MapTab::showPlacesList() {
    setFlags(1);
    rebuildList();
}

void MapTab::rebuildList() {
    s32 k;
    buildListScreen();
    layoutListLabels();
    if (getListScrollMax() == 0) {
        clearFlags(8);
        k = 4;
    } else {
        setFlags(8);
        k = 3;
    }
    BgScreen_SetRectPalette(frameScreen, 0x1d, 0xa, 0x1d, 0x15, k);
    setFlags(0x20);
    highlightListRow(selectedRow);
}

u8 *MapTab::getListEntries() {
    if (testFlags(1)) {
        return placeEntries;
    }
    return residentEntries;
}

u8 MapTab::getListCount() {
    if (testFlags(1)) {
        return placeCount;
    }
    return residentCount;
}

s32 MapTab::getListScrollMax() {
    u32 r = getListCount();
    if (r <= 6) {
        return 0;
    }
    return (r - 6) << 4;
}

BOOL MapTab::touchListRow() {
    s32 x = gTouchCurX;
    s32 y = gTouchCurY;
    s32 idx;
    if (x < 0x98 || x > 0xe8) {
        return FALSE;
    }
    if (y < 0x50 || y >= 0xb0) {
        return FALSE;
    }
    idx = (y + (listScroll - 0x50)) >> 4;
    if (testFlags(1) && idx == 5) {
        return FALSE;
    }
    selectEntry(idx + 0xf);
    return TRUE;
}

void MapTab::drawMapIcon(s32 x, s32 y, s32 n, s32 flag, s32 pal) {
    u16 t;
    sMapTabIconCell.charNo = n * 2 + 0xc0;
    if (flag != 0) {
        t = sMapTabIconCell.rsParam | 8;
        sMapTabIconCell.rsParam = t;
    }
    Oam_DrawObj(1, &sMapTabIconCell, x, y, pal, 1, NULL);
    if (flag != 0) {
        sMapTabIconCell.rsParam = t & 0x17;
    }
}

void MapTab::setupMarkers(TownMapMarkers *p) {
    s32 k, i;
    u32 j;
    TownMapMarker *rec;
    i = 0;
    k = i;
    for (; k < 3; i++, k++) {
        rec = (TownMapMarker *)Shared_TownMapMarkers_Get(p, i);
        if (rec != 0) {
            switch (TownMapMarkers_GetKind(p, i)) {
            case 0:
                terrainMarkers[k].cell = 8;
                break;
            case 1:
                terrainMarkers[k].cell = 7;
                break;
            case 2:
                terrainMarkers[k].cell = 9;
                break;
            case 3:
                terrainMarkers[k].cell = 0x89;
                break;
            default:
                terrainMarkers[k].cell = 0xc;
                break;
            }
            terrainMarkers[k].x = rec->x - 8;
            terrainMarkers[k].y = rec->y + 0x10;
        } else {
            terrainMarkers[k].cell = 0xc;
        }
    }
    j = 3;
    k = 0;
    for (; k < 0xe; j++, k++) {
        rec = (TownMapMarker *)Shared_TownMapMarkers_Get(p, j);
        if (rec != 0) {
            if (j >= 3 && j <= 0xa) {
                buildingMarkers[k].cell = 0;
            } else if (j == 0xb) {
                buildingMarkers[k].cell = 1;
            } else {
                buildingMarkers[k].cell = *(sMapTabFacilityCells + j - 0xc);
            }
            buildingMarkers[k].x = rec->x - 8;
            buildingMarkers[k].y = rec->y + 0x10;
        } else {
            buildingMarkers[k].cell = 0xc;
        }
    }
}

BOOL MapTab::touchMarker() {
    u8 r = findMarkerAt(gTouchCurX, gTouchCurY);
    if (r == 0xe) {
        return FALSE;
    }
    selectEntry(r + 1);
    return TRUE;
}

u8 MapTab::findMarkerAt(s32 x, s32 y) {
    s32 i;
    for (i = 0; i < 0xe; i++) {
        MapTabMarker *e = &buildingMarkers[i];
        if (e->cell != 0xc) {
            s32 px = e->x;
            if (px - 8 < x && px + 8 > x) {
                s32 py = e->y;
                if (py - 8 < y && py + 8 > y) {
                    return (u8)i;
                }
            }
        }
    }
    return 0xe;
}

void MapTab::selectEntry(u8 v) {
    if (v == 0) {
        selectedMarker = 0xe;
        selectedRow = 0xe;
        stopBlink();
        return;
    }
    startBlink();
    selectedRow = selectionToListRow(v);
    selectedMarker = selectionToMarker(v);
    if (v >= 1 && v < 0xf) {
        clearFlags(0x200);
        if (testFlags(1)) {
            if (v >= 1 && v <= 9) {
                showResidentsList();
                jumpToSelection();
                return;
            }
        } else {
            if (v < 1 || v > 9) {
                showPlacesList();
                jumpToSelection();
                return;
            }
        }
    } else {
        setFlags(0x200);
    }
    ensureSelectionVisible();
    highlightListRow(0xe);
    highlightListRow(selectedRow);
}

void MapTab::highlightListRow(u32 v) {
    setFlags(0x80);
    if (v == 0xe) {
        BgScreen_SetRectPalette(listScreenWork, 0x13, 0, 0x1c, 0x19, 4);
    } else if (v == 0xd) {
        BgScreen_SetRectPalette(listScreenWork, 0x13, 0, 0x1c, playerEntryCount * 2 - 1, 3);
    } else {
        BgScreen_SetRectPalette(listScreenWork, 0x13, v * 2, 0x1c, v * 2 + 1, 3);
    }
}

u8 MapTab::selectionToListRow(u32 v) {
    if (v == 9) {
        return 0xd;
    }
    if (v >= 1 && v < 9) {
        s32 t = v + 5;
        s32 i = 0;
        s32 n = residentCount;
        for (; i < n; i++) {
            if (t == residentEntries[i]) {
                return (u8)i;
            }
        }
        return 0xe;
    }
    if (v < 0xf && v >= 0xa) {
        return sMapTabMarkerPlaceRows[v - 0xa];
    }
    if (v < 0x1c && v >= 0xf) {
        return (u8)(v - 0xf);
    }
    return 0xe;
}

u8 MapTab::selectionToMarker(u32 v) {
    if (v >= 1 && v < 0xf) {
        return (u8)(v - 1);
    }
    if (v >= 0xf && v < 0x1c) {
        u32 b = getListEntries()[v - 0xf];
        if (b == 0) {
            return 0xe;
        }
        if (b == 1 || (b >= 2 && b < 6)) {
            return 8;
        }
        if (b >= 6 && b < 0xe) {
            return (u8)(b - 6);
        }
        if (b >= 0xe && b < 0x13) {
            return sMapTabPlaceMarkers[b - 0xe];
        }
    }
    return 0xe;
}

void MapTab::jumpToSelection() {
    u32 a = selectedRow;
    u32 v;
    if (a <= 5 || a == 0xd) {
        v = 0;
    } else {
        v = (a - 5) << 4;
    }
    setListScroll(v);
    listTopRow = 0xff;
}

void MapTab::ensureSelectionVisible() {
    u32 c = selectedRow;
    if (c != 0xe) {
        if (c == 0xd) {
            setListScrollTarget(0);
        } else {
            s32 t = (listScroll + 0xf) >> 4;
            if ((s32)c < t) {
                setListScrollTarget(c << 4);
            }
            s32 h = listScroll >> 4;
            u32 a = selectedRow;
            s32 m;
            if (a <= 5) {
                m = 0;
            } else {
                m = a - 5;
            }
            if (h < m) {
                setListScrollTarget(m << 4);
            }
        }
    }
}

void MapTab::setListScroll(u32 v) {
    listScroll = v;
    listScrollTarget = listScroll;
    setFlags(0x10);
}

void MapTab::setListScrollTarget(u32 v) {
    listScrollTarget = v;
    setFlags(0x10);
}

BOOL MapTab::isListScrolling() {
    return testFlags(0x10);
}

void MapTab::updateListScroll() {
    s32 n = getListScrollMax();
    if (n == 0) {
        clearFlags(0x10);
        knobPos = 0;
    } else {
        u32 a = ((volatile MapTab *)this)->listScrollTarget;
        u32 b = ((volatile MapTab *)this)->listScroll;
        if (b == a) {
            clearFlags(0x10);
        } else if (b < a) {
            listScroll = listScroll + 8;
            if (listScroll > listScrollTarget) {
                listScroll = listScrollTarget;
            }
        } else if (b < 8) {
            listScroll = a;
        } else {
            listScroll = listScroll - 8;
            if (listScroll < listScrollTarget) {
                listScroll = listScrollTarget;
            }
        }
        knobPos = DS_DIV((s32)(listScroll * 0x58), n);
    }
}

BOOL MapTab::touchScrollBar() {
    s32 x, y;
    if (!testFlags(8)) {
        return FALSE;
    }
    x = gTouchCurX;
    y = gTouchCurY;
    if (scrollKnob.hitTest(x, y)) {
        dragStartTouchY = y;
        dragStartKnobPos = knobPos;
        knobLastTickPos = dragStartKnobPos;
        setMainState(1);
        return TRUE;
    } else if (x > 0xe8 && x < 0xf5 && y > 0x56 && y < 0xae) {
        setMainState(2);
        return TRUE;
    }
    return FALSE;
}

void MapTab::scrollListToKnob(s32 v) {
    s32 t = v;
    if (t < 0) {
        t = 0;
    } else if (t > 0x58) {
        t = 0x58;
    }
    s32 d = t - knobLastTickPos;
    if (d >= 4 || d <= -4) {
        Menu_PlayScrollTickSe();
        knobLastTickPos = t;
    }
    s32 m = getListScrollMax();
    setListScroll(_s32_div_f(t * m, 0x58));
}

void MapTab::scrollListByPad() {
    s32 pos = listScrollTarget;
    u16 pad = gPad[0];
    if (pad & 0x40) {
        pos -= 4;
    } else if (pad & 0x80) {
        pos += 4;
    }
    s32 m = getListScrollMax();
    if (pos < 0) {
        pos = 0;
    } else if (pos > m) {
        pos = m;
    }
    if (pos != listScrollTarget) {
        Menu_PlayScrollTickSe();
    }
    setListScroll(pos);
}

s32 MapTab::touchListTabs() {
    s32 a = gTouchCurX;
    s32 b = gTouchCurY;
    if (b < 0x33 || b > 0x4b) {
        return 0;
    }
    if (testFlags(1)) {
        if (a < 0xcd || a > 0xe5) {
            return 0;
        }
        selectEntry(0);
        showResidentsList();
    } else {
        if (a < 0xaa || a > 0xc2) {
            return 0;
        }
        selectEntry(0);
        showPlacesList();
    }
    setListScroll(0);
    listTopRow = 0xff;
    return 1;
}

void MapTab::updateBlink() {
    if (testFlags(0x40)) {
        if (A8V != 0) {
            A8V = A8V - 1;
        }
        u32 v = blinkTimer;
        if (v == 0) {
            if (testFlags(0x200)) {
                clearFlags(0x100);
            } else {
                highlightListRow(selectedRow);
            }
            blinkTimer = 0xf;
        } else if (blinkTimer == 5) {
            if (testFlags(0x200)) {
                setFlags(0x100);
            } else {
                highlightListRow(0xe);
            }
        }
    }
}

void MapTab::startBlink() {
    setFlags(0x40);
    clearFlags(0x100);
    blinkTimer = 0xf;
}

void MapTab::stopBlink() {
    clearFlags(0x40);
    clearFlags(0x100);
}

void MapTab::showCursor() {
    if (!testFlags(8) && cursorTarget == 0x10) {
        cursorTarget = 8;
    }
    s32 a = getCursorTargetX();
    s32 b = getCursorTargetY();
    ((MenuCursorBase *)&cursor)->warpTo(a, b);
    if (cursorTarget >= 0xa && cursorTarget <= 0xf) {
        setListScrollTarget(listScroll & ~0xf);
    }
#ifdef N3DS_DIRECT
    if (cursorTarget <= 7 || cursorTarget == MAP_OPT_TARGET) {
#else
    if (cursorTarget <= 7) {
#endif
        ((MenuCursor *)&cursor)->setAnimIfChanged(0xd);
    } else {
        ((MenuCursor *)&cursor)->setAnimIfChanged(1);
    }
    refreshCursor();
}

s32 MapTab::getCursorTargetX() {
    if (testFlags(0x800)) {
        return mapCursorX;
    }
    u32 c = cursorTarget;
    if (c >= 0xa && c <= 0xf) {
        return 0xa0;
    }
    if (c <= 7) {
        return MenuTabBar_GetTabX(c);
    }
#ifdef N3DS_DIRECT
    if (c == MAP_OPT_TARGET) {
        return MenuTabBar_GetTabX(N3DS_OPT_TAB);
    }
#endif
    if (c == 8) {
        return 0xd8;
    }
    if (c == 9) {
        return 0xb4;
    }
    if (c == 0x10) {
        return scrollKnob.getGripX();
    }
    return 0x80;
}

s32 MapTab::getCursorTargetY() {
    if (testFlags(0x800)) {
        return mapCursorY;
    }
    u32 c = cursorTarget;
    if (c >= 0xa && c <= 0xf) {
        return (c - 0xa) * 16 + 0x58;
    }
    if (c >= 8 && c <= 9) {
        return 0x40;
    }
    if (c <= 7) {
        return 8;
    }
#ifdef N3DS_DIRECT
    if (c == MAP_OPT_TARGET) {
        return 8;
    }
#endif
    if (c == 0x10) {
        return scrollKnob.getGripY();
    }
    return 0x60;
}

void MapTab::hideCursor() {
    ((MenuCursor *)&cursor)->setAnimIfChanged(0);
    cursor.update();
}

void MapTab::moveCursorToTarget() {
    if (testFlags(0x4000)) {
        s32 a = getCursorTargetX();
        s32 b = getCursorTargetY();
        ((MenuCursorBase *)&cursor)->warpTo(a, b);
        clearFlags(0x4000);
    } else {
        s32 a = getCursorTargetX();
        s32 b = getCursorTargetY();
        ((MenuCursorBase *)&cursor)->moveToEase(a, b, 3, 1);
        returnState = mainState;
        setMainState(4);
    }
}

void MapTab::pressCursor() {
    ((MenuCursor *)&cursor)->setPosePress();
    setMainState(5);
}

void MapTab::releaseCursor() {
    ((MenuCursorBase *)&cursor)->setPoseRelease();
    setMainState(6);
}

void MapTab::refreshCursor() {
    ((MenuCursorBase *)&cursor)->setPoseIdle();
    cursor.update();
}

s32 MapTab::moveCursorByPad(u32 pad) {
    u32 old = cursorTarget;
    if (old <= 7) {
        if (MenuKeys_HasDown(pad)) {
            s32 cv = cursorTarget;
            if (cv <= 3) {
                mapCursorX = ((MenuCursorBase *)&cursor)->getScreenX();
                mapCursorY = 0x30;
                ((MenuCursor *)&cursor)->switchToAnim01();
                return 2;
            }
            if (cv >= 5) {
                cursorTarget = 8;
            } else {
                cursorTarget = 9;
            }
            ((MenuCursor *)&cursor)->switchToAnim01();
        } else if (MenuKeys_HasLeft(pad)) {
            if (ACV != 0) {
                ACV = ACV - 1;
            }
#ifdef N3DS_DIRECT
            else {
                cursorTarget = MAP_OPT_TARGET;  // (from the pockets' tab onto the options button)
            }
#endif
        } else if (MenuKeys_HasRight(pad)) {
            if (ACV < 7) {
                ACV = ACV + 1;
            }
        }
    } else if (old >= 0xa && old <= 0xf) {
        if (testFlags(8) && MenuKeys_HasRight(pad)) {
            cursorTarget = 0x10;
        } else if (MenuKeys_HasUp(pad)) {
            if (ACV > 0xa) {
                ACV = ACV - 1;
            } else {
                u32 h = listScrollTarget;
                if (((s32)h >> 4) > 0) {
                    setListScrollTarget(h - 0x10);
                    return 3;
                }
                cursorTarget = 9;
            }
        } else if (MenuKeys_HasDown(pad)) {
            s32 n = getListCount() - 1;
            u8 c = cursorTarget;
            if (c < 0xf) {
                if (c < n + 0xa) {
                    ACV = ACV + 1;
                }
            } else {
                u32 h = listScrollTarget;
                if (((s32)h >> 4) + 5 < n) {
                    setListScrollTarget(h + 0x10);
                    return 3;
                }
            }
        } else if (MenuKeys_HasLeft(pad)) {
            mapCursorX = 0x98;
            mapCursorY = ((MenuCursorBase *)&cursor)->getScreenY();
            return 2;
        }
#ifdef N3DS_DIRECT
    } else if (old == MAP_OPT_TARGET) {
        // the options button: Right back onto the pockets' tab, Down into the map as from the pockets' tab
        if (MenuKeys_HasDown(pad)) {
            cursorTarget = 0;
            mapCursorX = ((MenuCursorBase *)&cursor)->getScreenX();
            mapCursorY = 0x30;
            ((MenuCursor *)&cursor)->switchToAnim01();
            return 2;
        } else if (MenuKeys_HasRight(pad)) {
            cursorTarget = 0;
        }
#endif
    } else if (old == 0x10) {
        if (MenuKeys_HasUp(pad)) {
            cursorTarget = 8;
        } else if (MenuKeys_HasLeft(pad)) {
            s32 v = scrollKnob.getGripY();
            if (v < 0x50) {
                v = 0x50;
            }
            if (v >= 0xb0) {
                v = 0xaf;
            }
            cursorTarget = ((v - 0x50) >> 4) + 0xa;
        }
    } else if ((u8)(old + 0xf8) <= 1) {
        if (MenuKeys_HasUp(pad)) {
            cursorTarget = 5;
            ((MenuCursor *)&cursor)->switchToAnim0D();
        } else if (MenuKeys_HasDown(pad)) {
            cursorTarget = 0xa;
        } else if (MenuKeys_HasLeft(pad)) {
            if (cursorTarget == 9) {
                mapCursorX = 0x98;
                mapCursorY = ((MenuCursorBase *)&cursor)->getScreenY();
                return 2;
            }
            cursorTarget = 9;
        } else if (MenuKeys_HasRight(pad)) {
            if (cursorTarget == 8 && testFlags(8)) {
                cursorTarget = 0x10;
            } else {
                cursorTarget = 8;
            }
        }
    }
    if (old != cursorTarget) {
        return 1;
    }
    return 0;
}

s32 MapTab::onCursorDecide() {
    if (testFlags(0x800)) {
        selectEntry(cursorMarker + 1);
        playSeAtMarker(0x38);
        return 0;
    }
    u32 s = cursorTarget;
    if (s == 0x10) {
        setMainState(7);
        scrollKnob.grab();
        setFlags(0x1000);
        Menu_PlayScrollGrabSe();
        return 1;
    } else if (s == 8) {
        if (testFlags(1)) {
            selectEntry(0);
            showResidentsList();
            Snd_PlaySe(0x36);
            return 2;
        }
        return 0;
    } else if (s == 9) {
        if (!testFlags(1)) {
            selectEntry(0);
            showPlacesList();
            Snd_PlaySe(0x36);
            return 2;
        }
        return 0;
#ifdef N3DS_DIRECT
    } else if (s == MAP_OPT_TARGET) {
        return requestTab(N3DS_OPT_TAB) ? 1 : 0;
#endif
    } else if (s <= 7) {
        if (requestTab(s)) {
            return 1;
        }
        return 0;
    } else if (s >= 0xa && s <= 0xf) {
        { u32 t = listTopRow; selectEntry(t + s + 5); };
        playSeAtMarker(0x37);
        return 0;
    }
    return 0;
}

void MapTab::playSeAtMarker(s32 a) {
    u32 i = selectedMarker;
    if (i != 0xe) {
        s32 v = (buildingMarkers[i].x - 0x18) * 2 - 0x7f;
        if (v < -0x7f) {
            v = -0x7f;
        } else if (v > 0x80) {
            v = 0x80;
        }
        Snd_PlaySePanned(a, v);
    }
}

BOOL MapTab::testFlags(u32 m) {
    if (flags & m) {
        return TRUE;
    }
    return FALSE;
}

void MapTab::setFlags(u32 m) { flags = flags | m; }

void MapTab::clearFlags(u32 m) { flags = flags & ~m; }

MapTabMarker::MapTabMarker() {}

MapTabMarker::~MapTabMarker() {}

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_02292d00) {
    HOST_OVERLAY_VAR(sMapTabResidentsButtonCells),
    HOST_OVERLAY_VAR(sMapTabIconCell),
    HOST_OVERLAY_VAR(sMapTabPlacesButtonCells),
    HOST_OVERLAY_VAR(sMapTabProfile),
    HOST_OVERLAY_VAR(sMapTabFrameCells),
    HOST_OVERLAY_VARS_END
};

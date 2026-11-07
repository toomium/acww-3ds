#include "uptr.h"
#include "types.h"
#include "Unk_020d8c7c.h"
#include "text/Unk_02050288.h"
#include "talk/MsgStringBase.h"
#include "talk/MsgStringAttr.h"
#include "ui/NameLabelBalloon.h"
#include "gfx/SpriteAnim.h"
#include "ui/UiWidget.h"
#include "talk/MsgTextLabel.h"
#include "talk/MsgString.h"
#include "talk/MsgString25.h"
#include "talk/MsgString3.h"
#include "ui/HudWallet.h"
#include "ui/HudController.h"

// ---------------------------------------------------------------------------------------------------------------------
// Classes of the unit (declarations of the classes whose vtable another unit owns come first)

struct SpriteAnimSeq;








// ---------------------------------------------------------------------------------------------------------------------
extern "C" {
s32 func_01ffcb0c(s32 a, s32 b);
void Math_ApproachS32(void *p, s32 a, s32 b, s32 c, s32 d);
void Math_StepS32Alt(void *p, s32 a, s32 b);
MsgTextLabel *MsgTextLabel_CreateVram(u32 a, s32 b, s32 c);
void MsgTextLabel_Destroy(TextLabel *obj);
BOOL TalkRequest_FinishCameraView();
BOOL TalkRequest_IsCameraViewRunning();
void TalkRequest_AddCameraView();
BOOL Input_IsTouchMode();
BOOL Input_IsButtonMode();
BOOL Input_IsAnyKeyTrig();
BOOL Input_IsTouchTrig();
void Input_SetButtonMode();
void Input_SetTouchMode();
s32 Input_StoreMode();
BOOL TalkRequest_IsActive();
void Camera_SetPresetCell(u32 a, u32 b);
BOOL Input_IsTouchTrigInRect(s32 x0, s32 x1, s32 y0, s32 y1);
void MI_CpuCopy8(void *src, void *dst, u32 n);
s32 Scene_GetCurrent();
s32 Clock_GetWeekday();

// functions of the unit that other classes of the unit call (symbols.txt names)
}

extern u8 gFieldSceneKind;
extern u8 data_020d479c[];
extern u8 data_020d4794[];
extern u8 data_020d477c[];
extern u8 data_020d4784[];
extern u8 data_020d478c[];
// The sequence table at data_020d467c spans several arrays (src/main/unk_020d467c.cpp)
extern void *const gSpriteSeqTable[45];
extern u16 gPad[];

// Data of this unit
extern char *const kHudColonString;
extern char *const kHudAmPmStrings[2];
extern const s16 kHudCountdownSeconds[6];
extern const u8 kHudWeekdayGlyphs[8][2];
extern const u8 kHudCameraGridTouchPos[18];
extern const s8 kHudCameraGridDirs[18];
extern const u8 kHudCameraGridCells[18];

// ---------------------------------------------------------------------------------------------------------------------













#include "ui/HudProc.h"
#include "sys/ProcProfile.h"
#include "player/PlayerData.h"
#include "player/PlayerInventory.h"
#include "proto/main/unk_0209cb74.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/itcm/unk_01ffa4a0.h"
#include "proto/main/unk_020030d8.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_020116e0.h"
#include "proto/main/unk_02038474.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_02087e70.h"
#include "proto/main/unk_02089fbc.h"
#include "proto/main/unk_0208de68.h"
#include "proto/main/unk_0208eeac.h"
#include "proto/main/unk_020501d4.h"
#include "proto/main/unk_020b2c50.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_020b7190.h"
#include "proto/main/unk_020b7d84.h"
#include "n3ds_ui.h"



static inline BOOL Unk_0208a150_IsOne(u8 v) { return v == 1 ? TRUE : FALSE; }
static inline BOOL Unk_0208b9f0_Or(BOOL a, BOOL v) { if ((a | v) != 0) return TRUE; return FALSE; }


typedef void (HudCameraButton::*Unk_020e0f2c_Fn)();

typedef void (HudCameraGrid::*Unk_0208b728_Fn)();

typedef void (HudCountdown::*Unk_020e0f10_Fn)();

typedef void (HudClock::*Unk_020e0f64_Fn)();

void NameLabelBalloon::createLabel() {
    if (textLabel == NULL) {
        BOOL c = kind == 4 ? TRUE : FALSE;
        s32 width = c ? 0x1c8 : (kind << 6) + 0xc0;
        u8 t = c ? 0xe : 0xf;
        textLabel = MsgTextLabel_CreateVram(width, 0x14, 2);
        TextLabel *o = textLabel;
        if (o != NULL) {
            o->vramLoader = 4;
            TextLabel *p = textLabel;
            p->textStart = text.data();
            textLabel->copyMode = 2;
            textLabel->rowStride1K = 1;
            textLabel->bgColor = t;
            textLabel->fgColor = 0xd;
            textLabel->requestRedraw();
        }
    }
}

void NameLabelBalloon::freeLabel() {
    if (textLabel != NULL) {
        MsgTextLabel_Destroy(textLabel);
        textLabel = NULL;
    }
}

MsgString3::MsgString3() {
    clear();
}

MsgString3::~MsgString3() {}

u32 MsgString3::capacity() {
    return 3;
}

u8 *MsgString3::data() {
    return text;
}

HudClock::HudClock()
    : state(0), showDelay(0), monthLabel(NULL), dayLabel(NULL), weekdayLabel(NULL), amPmLabel(NULL), hourLabel(NULL), minuteLabel(NULL),
      colonLabel(NULL), colonTimer(0), monthDirty(0), dayDirty(0), weekdayDirty(0), amPmDirty(0), hourDirty(0), minuteDirty(0), showRequested(0),
      altLayout(0), slideY(0), slideTarget(0), slideSpeed(0), slideDelay(-1), weekday(0) {
    day = 0;
    month = 0;
    minute = 0;
    hour = 0;
}

HudClock::~HudClock() {
    anim.restart();
    release();
}

void HudClock::draw() {
    if (state != 0) {
        GXOamAttr *a = anim.getCell();  // (SpriteAnimFrame::cell: an OAM cell)
        if (a != 0) {
            s32 x = anim.getFrameX(-1);
            s32 y = anim.getFrameY(-1);
            s32 bx = getOriginX();
            s32 t = (slideY + 0x800) >> 12;
            s32 g = getOriginY();
            s32 by = g + t;
            s32 f = weekday == 0 ? 0xf : 9;
            Oam_DrawCell(0, a, bx + x, by + y, f, -1, 0x1000, 0x1000, 0, -1, 0, 0);
        }
    }
}

void HudClock::update() {
    static Unk_020e0f64_Fn tbl[4] = {(Unk_020e0f64_Fn)&HudClock::updateHidden, (Unk_020e0f64_Fn)&HudClock::updateAppearing,
                                     (Unk_020e0f64_Fn)&HudClock::updateShown, (Unk_020e0f64_Fn)&HudClock::updateHiding};
    (this->*tbl[state])();
    pollDateTime();
    refreshLabels();
    if (state != 0) {
        updateSlide();
    }
}

void HudClock::reset() {
    showRequested = 0;
    enterHidden();
}

void HudClock::release() {
    freeLabels();
}

void HudClock::callUpdate() {
    update();
}

void HudClock::callDraw() {
    draw();
}

void HudClock::show() {
    showRequested = 1;
    BOOL v = TRUE;
    if (gFieldSceneKind != 1) {
        v = FALSE;
    }
    altLayout = v ? 1 : 0;
}

void HudClock::hide() {
    showRequested = 0;
}

BOOL HudClock::isHidden() {
    if (state == 0) {
        return TRUE;
    }
    return FALSE;
}

void HudClock::enterHidden() {
    state = 0;
}

void HudClock::updateHidden() {
    if (canShow() != 0) {
        showDelay = showDelay - 1;
        if (showDelay <= 0) {
            enterAppearing();
        }
    }
}

void HudClock::enterAppearing() {
    state = 1;
    resetSlide();
    s32 i = altLayout != 0 ? 0x29 : 4;
    anim.setSeq((SpriteAnimSeq *)gSpriteSeqTable[i]);
    anim.setPlayOnce(1);
    anim.restart();
    createLabels();
}

void HudClock::updateAppearing() {
    anim.update();
    if (anim.isFinished()) {
        enterShown();
    }
}

void HudClock::enterShown() {
    state = 2;
}

void HudClock::updateShown() {
    if (canShow() == 0) {
        enterHiding();
    }
}

void HudClock::enterHiding() {
    state = 3;
    s32 i = altLayout != 0 ? 0x2a : 5;
    anim.setSeq((SpriteAnimSeq *)gSpriteSeqTable[i]);
    anim.setPlayOnce(1);
    anim.restart();
}

void HudClock::updateHiding() {
    anim.update();
    if (anim.isFinished()) {
        freeLabels();
        enterHidden();
    }
}

u32 HudClock::canShow() {
    u32 r = showRequested;
    if (r != 0) {
        BOOL t = TalkRequest_IsActive();
        BOOL a = PlayerActor_IsInAction(2, 4);
        BOOL b = MenuCtrl_IsMenuOpen();
        BOOL f5 = FALSE;
        if (t != 0 && b == 0) {
            f5 = TRUE;
        }
        BOOL f4 = FALSE;
        if (b == 0 && a == 0) {
            f4 = TRUE;
        }
        BOOL f7 = FALSE;
        if (t != 0 && b != 0) {
            BOOL x = MenuCtrl_IsTransitionActive();
            s32 y = MenuCtrl_GetTransitionProgressOrFull();
            if (x != 0) {
                if (y < 0x1000) {
                    f7 = TRUE;
                }
            } else {
                f7 = TRUE;
            }
        }
        if (f5 || f4 || f7) {
            r = 0;
            if (f5 || f4) {
                showDelay = 0x1e;
            } else {
                showDelay = 1;
            }
        }
    } else {
        showDelay = 10;
    }
    return r;
}

void HudClock::createLabels() {
    createMonthLabel();
    createDayLabel();
    createWeekdayLabel();
    createAmPmLabel();
    createHourLabel();
    createMinuteLabel();
    createColonLabel();
}

void HudClock::createMonthLabel() {
    if (monthLabel == NULL) {
        monthLabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0x94 : 0x80, 3, 2);
        MsgTextLabel *o = monthLabel;
        if (o != NULL) {
            o->vramLoader = 4;
            monthLabel->copyMode = 2;
            monthLabel->rowStride1K = 1;
            monthLabel->bgColor = 0;
            monthLabel->fgColor = 0xc;
            monthLabel->font = (GameFontDesc *)&gFontC;  // (a GameFont: a GameFontDesc)
            monthLabel->requestRedraw();
            monthDirty = 1;
        }
    }
}

void HudClock::createDayLabel() {
    if (dayLabel == NULL) {
        dayLabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0x97 : 0x83, 3, 2);
        MsgTextLabel *o = dayLabel;
        if (o != NULL) {
            o->vramLoader = 4;
            dayLabel->copyMode = 2;
            dayLabel->rowStride1K = 1;
            dayLabel->bgColor = 0;
            dayLabel->fgColor = 0xc;
            dayLabel->font = (GameFontDesc *)&gFontC;  // (a GameFont: a GameFontDesc)
            dayLabel->requestRedraw();
            dayDirty = 1;
        }
    }
}

void HudClock::createWeekdayLabel() {
    if (weekdayLabel == NULL) {
        weekdayLabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0x9a : 0x86, 2, 2);
        MsgTextLabel *o = weekdayLabel;
        if (o != NULL) {
            o->vramLoader = 4;
            weekdayLabel->copyMode = 2;
            weekdayLabel->rowStride1K = 1;
            weekdayLabel->bgColor = 0;
            weekdayLabel->fgColor = 0xa;
            weekdayLabel->font = (GameFontDesc *)&gFontC;  // (a GameFont: a GameFontDesc)
            weekdayLabel->requestRedraw();
            weekdayDirty = 1;
        }
    }
}

void HudClock::createAmPmLabel() {
    if (amPmLabel == NULL) {
        amPmLabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0xd4 : 0x88, 2, 1);
        MsgTextLabel *o = amPmLabel;
        if (o != NULL) {
            o->vramLoader = 4;
            amPmLabel->copyMode = 2;
            amPmLabel->rowStride1K = 1;
            amPmLabel->bgColor = 0;
            amPmLabel->fgColor = 9;
            amPmLabel->font = (GameFontDesc *)&gFontD;  // (a GameFont: a GameFontDesc)
            amPmLabel->requestRedraw();
            amPmDirty = 1;
        }
    }
}

void HudClock::createHourLabel() {
    if (hourLabel == NULL) {
        hourLabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0xf4 : 0xa8, 2, 1);
        MsgTextLabel *o = hourLabel;
        if (o != NULL) {
            o->vramLoader = 4;
            hourLabel->copyMode = 2;
            hourLabel->rowStride1K = 1;
            hourLabel->bgColor = 0;
            hourLabel->fgColor = 9;
            hourLabel->font = (GameFontDesc *)&gFontD;  // (a GameFont: a GameFontDesc)
            hourLabel->requestRedraw();
            hourDirty = 1;
        }
    }
}

void HudClock::createMinuteLabel() {
    if (minuteLabel == NULL) {
        minuteLabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0xf6 : 0xaa, 2, 1);
        MsgTextLabel *o = minuteLabel;
        if (o != NULL) {
            o->vramLoader = 4;
            minuteLabel->copyMode = 2;
            minuteLabel->rowStride1K = 1;
            minuteLabel->bgColor = 0;
            minuteLabel->fgColor = 9;
            minuteLabel->font = (GameFontDesc *)&gFontD;  // (a GameFont: a GameFontDesc)
            minuteLabel->requestRedraw();
            minuteDirty = 1;
        }
    }
}

void HudClock::createColonLabel() {
    if (colonLabel == NULL) {
        colonLabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0xd8 : 0x8c, 1, 2);
        MsgTextLabel *o = colonLabel;
        if (o != NULL) {
            o->vramLoader = 4;
            colonLabel->textStart = (u8 *)kHudColonString;
            colonLabel->copyMode = 2;
            colonLabel->rowStride1K = 1;
            colonLabel->font = (GameFontDesc *)&gFontC;  // (a GameFont: a GameFontDesc)
            colonLabel->alignCenter();
            colonLabel->bgColor = 0;
            colonLabel->fgColor = 9;
            colonLabel->requestClear(0);
        }
    }
}

void HudClock::freeLabels() {
    if (monthLabel != NULL) {
        MsgTextLabel_Destroy(monthLabel);
        monthLabel = NULL;
    }
    if (dayLabel != NULL) {
        MsgTextLabel_Destroy(dayLabel);
        dayLabel = NULL;
    }
    if (weekdayLabel != NULL) {
        MsgTextLabel_Destroy(weekdayLabel);
        weekdayLabel = NULL;
    }
    if (amPmLabel != NULL) {
        MsgTextLabel_Destroy(amPmLabel);
        amPmLabel = NULL;
    }
    if (hourLabel != NULL) {
        MsgTextLabel_Destroy(hourLabel);
        hourLabel = NULL;
    }
    if (minuteLabel != NULL) {
        MsgTextLabel_Destroy(minuteLabel);
        minuteLabel = NULL;
    }
    if (colonLabel != NULL) {
        MsgTextLabel_Destroy(colonLabel);
        colonLabel = NULL;
    }
}

void HudClock::refreshLabels() {
    refreshMonth();
    refreshDay();
    refreshWeekday();
    refreshAmPm();
    refreshHour();
    refreshMinute();
    blinkColon();
}

void HudClock::refreshMonth() {
    if (monthLabel != 0 && monthDirty != 0) {
        monthDirty = 0;
        String_FormatNumber(&monthText, month, 2, 0, 0, 1);
        TextLabel *t = monthLabel;
        t->textStart = monthText.data();
        monthLabel->alignRight();
        monthLabel->requestRedraw();
    }
}

void HudClock::refreshDay() {
    if (dayLabel != 0 && dayDirty != 0) {
        dayDirty = 0;
        String_FormatNumber(&dayText, day, 2, 0, 0, 1);
        TextLabel *t = dayLabel;
        t->textStart = dayText.data();
        dayLabel->requestRedraw();
    }
}

void HudClock::refreshWeekday() {
    if (weekdayLabel != 0 && weekdayDirty != 0) {
        weekdayDirty = 0;
        s32 i = *(volatile s32 *)&weekday;
        weekdayLabel->textStart = const_cast<u8 *>(kHudWeekdayGlyphs[i]);
        weekdayLabel->alignCenter();
        weekdayLabel->requestRedraw();
    }
}

void HudClock::refreshAmPm() {
    if (amPmLabel != 0 && amPmDirty != 0) {
        amPmDirty = 0;
        s32 i = 0;
        if (hour >= 12) i = 1;
        amPmLabel->textStart = (u8 *)kHudAmPmStrings[i];
        amPmLabel->alignCenter();
        amPmLabel->requestRedraw();
    }
}

void HudClock::refreshHour() {
    if (hourLabel != 0 && hourDirty != 0) {
        hourDirty = 0;
        u8 c = hour;
        if (c >= 12) c = (u8)(c - 12);
        if (c == 0) c = 12;
        String_FormatNumber(&hourText, c, 2, 0, 0, 1);
        TextLabel *t = hourLabel;
        t->textStart = hourText.data();
        hourLabel->alignRight();
        hourLabel->requestRedraw();
    }
}

void HudClock::refreshMinute() {
    if (minuteLabel != 0 && minuteDirty != 0) {
        minuteDirty = 0;
        String_FormatNumber(&minuteText, minute, 2, 6, 0, 1);
        TextLabel *t = minuteLabel;
        t->textStart = minuteText.data();
        minuteLabel->alignCenter();
        minuteLabel->requestRedraw();
    }
}

void HudClock::blinkColon() {
    if (colonLabel != 0) {
        colonTimer = colonTimer - 1;
        s32 t = colonTimer;
        if (t <= 0) {
            colonTimer = 0x14;
            colonLabel->requestRedraw();
        } else if (t == 8) {
            colonLabel->requestClear(0);
        }
    }
}

void HudClock::pollDateTime() {
    u8 dayMonth[2];    // {day, month}
    u8 minuteHour[2];  // {minute, hour}
    Clock_GetDayMonth(dayMonth);
    Clock_GetMinuteHour(minuteHour);
    s32 t = Clock_GetWeekday();
    if (dayMonth[0] != day || dayMonth[1] != month) {
        if (dayMonth[1] != month) monthDirty = 1;
        if (dayMonth[0] != day) dayDirty = 1;
        day = dayMonth[0];
        month = dayMonth[1];
    }
    if (minuteHour[0] != minute || minuteHour[1] != hour) {
        if (minuteHour[1] != hour) {
            hourDirty = 1;
            amPmDirty = 1;
        }
        if (minuteHour[0] != minute) minuteDirty = 1;
        minute = minuteHour[0];
        hour = minuteHour[1];
    }
    if (t != weekday) {
        weekdayDirty = 1;
        weekday = t;
    }
}

void HudClock::updateSlide() {
    s32 a = ChatBalloon_IsOwnBusy();
    s32 b = canShow();
    s32 t;
    if (a != 0 && b != 0) t = -0x14000; else t = 0;
    slideSpeed = slideSpeed + 0xa00;
    s32 v = slideSpeed;
    if (v < 0x2300) v = 0x2300; else if (v > 0x5000) v = 0x5000;
    slideSpeed = v;
    if (t != slideTarget) {
        s32 c = slideDelay;
        if (c < 0 || b == 0 || (slideDelay = c + 1, slideDelay > 10)) {
            slideTarget = t;
            slideDelay = 0;
        }
    } else {
        slideDelay = 0;
    }
    Math_ApproachS32(&slideY, slideTarget, 0x600, slideSpeed, 0x2300);
}

void HudClock::resetSlide() {
    slideY = 0;
    slideTarget = 0;
    slideDelay = -1;
    slideSpeed = 0;
}

HudCountdown::HudCountdown() : state(0), minutesLabel(0), secondsLabel(0), countALabel(0), countBLabel(0) {
    minutesDirty = 0;
    secondsDirty = 0;
    countADirty = 0;
    countBDirty = 0;
    showRequested = 0;
    altLayout = 0;
    countA = 0;
    countB = 0;
    durationKind = 0;
    endTime = ClockDateTime();
    remaining = ClockDateTime();
    finishTimer = 0;
    slideY = 0;
    slideTarget = 0;
    slideSpeed = 0;
    slideDelay = -1;
    lastTickTime = 0;
    lastTickTimeHi = 0;
}

HudCountdown::~HudCountdown() {
    anim.restart();
    release();
}

void HudCountdown::draw() {
    if (state != 0) {
        GXOamAttr *p = anim.getCell();  // (SpriteAnimFrame::cell: an OAM cell)
        if (p != 0) {
            s32 a = anim.getFrameX(-1);
            s32 b = anim.getFrameY(-1);
            s32 c = getOriginX();
            s32 d = (slideY + 0x800) >> 12;
            s32 e = getOriginY();
            e += d;
            Oam_DrawCell(0, p, c + a, e + b, -1, -1, 0x1000, 0x1000, 0, -1, 0, 0);
        }
    }
}

void HudCountdown::update() {
    updateRemaining();
    static Unk_020e0f10_Fn tbl[4] = {(Unk_020e0f10_Fn)&HudCountdown::updateHidden, (Unk_020e0f10_Fn)&HudCountdown::updateAppearing,
                                     (Unk_020e0f10_Fn)&HudCountdown::updateShown, (Unk_020e0f10_Fn)&HudCountdown::updateHiding};
    (this->*tbl[state])();
    refreshLabels();
    if (state != 0) updateSlide();
}

void HudCountdown::reset() {
    showRequested = 0;
    enterHidden();
}

void HudCountdown::release() {
    freeLabels();
}

void HudCountdown::callUpdate() {
    update();
}

void HudCountdown::callDraw() {
    draw();
}

BOOL HudCountdown::isStopped() {
    if (durationKind == 0) return TRUE;
    return FALSE;
}

void HudCountdown::start(s32 a, s32 b) {
    durationKind = a;
    finishTimer = 0;
    if (a != 0) {
        Clock_GetRtcDateTime(&endTime);
        DateTime_AddSeconds(&endTime, kHudCountdownSeconds[a]);
        countA = 0;
        countB = 0;
        minutesDirty = 1;
        secondsDirty = 1;
        countADirty = 1;
        countBDirty = 1;
    }
    if (b == 0) {
        FieldInfoBalloon_ShowTimerMsg(a == 0 ? 2 : 1);
    }
}

void HudCountdown::incCountA() {
    if (durationKind != 0) {
        countA = countA + 1;
        countADirty = 1;
    }
}

void HudCountdown::incCountB() {
    if (durationKind != 0) {
        countB = countB + 1;
        countBDirty = 1;
    }
}

void HudCountdown::show() {
    showRequested = 1;
    BOOL t = TRUE;
    if (gFieldSceneKind != 1) t = FALSE;
    altLayout = (t != 0) ? 1 : 0;
}

void HudCountdown::hide() {
    showRequested = 0;
}

BOOL HudCountdown::isHidden() {
    if (state == 0) return TRUE;
    return FALSE;
}

BOOL HudCountdown::isFinished() {
    if (isStopped() != 0 && finishTimer <= 0) return TRUE;
    return FALSE;
}

void HudCountdown::enterHidden() {
    state = 0;
}

void HudCountdown::updateHidden() {
    if (canShow() != 0) enterAppearing();
}

void HudCountdown::enterAppearing() {
    state = 1;
    resetSlide();
    s32 i;
    if (altLayout != 0) i = 0x2b; else i = 0x26;
    anim.setSeq((SpriteAnimSeq *)gSpriteSeqTable[i]);
    anim.setPlayOnce(1);
    anim.restart();
    createLabels();
}

void HudCountdown::updateAppearing() {
    anim.update();
    if (anim.isFinished() != 0) enterShown();
}

void HudCountdown::enterShown() {
    state = 2;
}

void HudCountdown::updateShown() {
    if (canShow() == 0) enterHiding();
}

void HudCountdown::enterHiding() {
    state = 3;
    s32 i;
    if (altLayout != 0) i = 0x2c; else i = 0x27;
    anim.setSeq((SpriteAnimSeq *)gSpriteSeqTable[i]);
    anim.setPlayOnce(1);
    anim.restart();
}

void HudCountdown::updateHiding() {
    anim.update();
    if (anim.isFinished() != 0) {
        freeLabels();
        enterHidden();
    }
}

u8 HudCountdown::canShow() {
    u8 r = showRequested;
    if (r != 0) {
        s32 a = TalkRequest_IsActive();
        s32 b = MenuCtrl_IsMenuOpen();
        BOOL c = FALSE;
        if (a != 0 && b == 0) c = TRUE;
        BOOL d = FALSE;
        if (a != 0 && b != 0) {
            s32 p = MenuCtrl_IsTransitionActive();
            s32 q = MenuCtrl_GetTransitionProgressOrFull();
            if (p != 0) {
                if (q < 0x1000) d = TRUE;
            } else {
                d = TRUE;
            }
        }
        if (c != 0 || d != 0) {
            r = 0;
            if (c != 0) finishTimer = r;
        }
    } else {
        finishTimer = 0;
    }
    return r;
}

void HudCountdown::createLabels() {
    createMinutesLabel();
    createSecondsLabel();
    createCountALabel();
    createCountBLabel();
}

void HudCountdown::createMinutesLabel() {
    minutesLabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0x114 : 0x80, 3, 2);
    if (minutesLabel != NULL) {
        minutesLabel->vramLoader = 4;
        minutesLabel->copyMode = 2;
        minutesLabel->rowStride1K = 1;
        minutesLabel->bgColor = 0;
        minutesLabel->fgColor = 0xc;
        minutesLabel->font = (GameFontDesc *)&gFontC;  // (a GameFont: a GameFontDesc)
        TextLabel *t = minutesLabel;
        t->textStart = minutesText.data();
        minutesDirty = 1;
    }
}

void HudCountdown::createSecondsLabel() {
    secondsLabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0x117 : 0x83, 3, 2);
    if (secondsLabel != NULL) {
        secondsLabel->vramLoader = 4;
        secondsLabel->copyMode = 2;
        secondsLabel->rowStride1K = 1;
        secondsLabel->bgColor = 0;
        secondsLabel->fgColor = 0xc;
        secondsLabel->font = (GameFontDesc *)&gFontC;  // (a GameFont: a GameFontDesc)
        TextLabel *t = secondsLabel;
        t->textStart = secondsText.data();
        secondsDirty = 1;
    }
}

void HudCountdown::createCountALabel() {
    countALabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0x174 : 0xa8, 2, 1);
    if (countALabel != NULL) {
        countALabel->vramLoader = 4;
        countALabel->copyMode = 2;
        countALabel->rowStride1K = 1;
        countALabel->bgColor = 0;
        countALabel->fgColor = 5;
        countALabel->font = (GameFontDesc *)&gFontD;  // (a GameFont: a GameFontDesc)
        TextLabel *t = countALabel;
        t->textStart = countAText.data();
        countADirty = 1;
    }
}

void HudCountdown::createCountBLabel() {
    countBLabel = MsgTextLabel_CreateVram(altLayout != 0 ? 0x176 : 0xaa, 2, 1);
    if (countBLabel != NULL) {
        countBLabel->vramLoader = 4;
        countBLabel->copyMode = 2;
        countBLabel->rowStride1K = 1;
        countBLabel->bgColor = 0;
        countBLabel->fgColor = 5;
        countBLabel->font = (GameFontDesc *)&gFontD;  // (a GameFont: a GameFontDesc)
        TextLabel *t = countBLabel;
        t->textStart = countBText.data();
        countBDirty = 1;
    }
}

void HudCountdown::freeLabels() {
    if (minutesLabel != NULL) {
        MsgTextLabel_Destroy(minutesLabel);
        minutesLabel = NULL;
    }
    if (secondsLabel != NULL) {
        MsgTextLabel_Destroy(secondsLabel);
        secondsLabel = NULL;
    }
    if (countALabel != NULL) {
        MsgTextLabel_Destroy(countALabel);
        countALabel = NULL;
    }
    if (countBLabel != NULL) {
        MsgTextLabel_Destroy(countBLabel);
        countBLabel = NULL;
    }
}

void HudCountdown::refreshLabels() {
    refreshMinutes();
    refreshSeconds();
    refreshCountA();
    refreshCountB();
}

void HudCountdown::refreshMinutes() {
    if (minutesDirty != 0 && minutesLabel != NULL) {
        String_FormatNumber(&minutesText, remaining.minute, 2, 6, 0, 1);
        minutesLabel->alignRight();
        minutesLabel->requestRedraw();
        minutesDirty = 0;
    }
}

void HudCountdown::refreshSeconds() {
    if (secondsDirty != 0 && secondsLabel != NULL) {
        String_FormatNumber(&secondsText, remaining.second, 2, 6, 0, 1);
        secondsLabel->requestRedraw();
        secondsDirty = 0;
    }
}

void HudCountdown::refreshCountA() {
    if (countADirty != 0 && countALabel != NULL) {
        String_FormatNumber(&countAText, countA, 2, 6, 0, 1);
        countALabel->alignCenter();
        countALabel->requestRedraw();
        countADirty = 0;
    }
}

void HudCountdown::refreshCountB() {
    if (countBDirty != 0 && countBLabel != NULL) {
        String_FormatNumber(&countBText, countB, 2, 6, 0, 1);
        countBLabel->alignCenter();
        countBLabel->requestRedraw();
        countBDirty = 0;
    }
}

void HudCountdown::updateRemaining() {
    ClockDateTime a;  // the end time, then the time left
    ClockDateTime b;  // now
    ClockDateTime c;
    BOOL x;
    BOOL y;
    BOOL t;
    s32 r;
    if (finishTimer > 0) {
        finishTimer = finishTimer - 1;
    }
    if (durationKind != 0) {
        MI_CpuCopy8(&endTime, &a, sizeof(a));
        b = ClockDateTime();
        Clock_GetRtcDateTime(&b);
        r = DateTime_Compare(&b, &a, 0x3f);
        if (r == 0) {
            goto yes;
        }
        if (r == 1) {
        yes:
            x = TRUE;
        } else {
            x = FALSE;
        }
        y = x;
        if (x) {
            a = ClockDateTime();
        } else {
            u32 k;
            MI_CpuCopy8(&b, &c, sizeof(c));
            DateTime_Sub(&a, &c);
            k = x;
            t = DateTime_Compare(&remaining, &a, 1) ? TRUE : FALSE;
            x = (k | t) ? TRUE : FALSE;
            r = DateTime_Compare(&remaining, &a, 2) ? TRUE : FALSE;
            k |= r;
            y = k ? TRUE : FALSE;
        }
        u8 *p95 = &secondsDirty;
        *p95 = Unk_0208b9f0_Or(secondsDirty, x);
        u8 *p94 = &minutesDirty;
        *p94 = Unk_0208b9f0_Or(minutesDirty, y);
        if (x != 0 || y != 0) {
            u32 b0, b1;
            MI_CpuCopy8(&a, &remaining, sizeof(remaining));
            b0 = a.second;
            b1 = a.minute;
            if (b1 == 0) {
                if (b0 == 0) {
                    finishTimer = 0x258;
                    durationKind = 0;
                    if (Scene_GetCurrent() != 0x2e) {
                        Snd_PlaySe(0x65);
                    }
                    FieldInfoBalloon_ShowTimerMsg(3);
                } else if (b0 <= 10) {
                    u64 now = OS_GetTick();
                    u64 d = now - ((u64)lastTickTimeHi << 32 | lastTickTime);
                    u64 q = (d << 6) / 0x82ea;
                    if (q <= 0x44c) {
                        if (Scene_GetCurrent() != 0x2e) {
                            Snd_PlaySe(0x64);
                        }
                    }
                    lastTickTime = (u32)now;
                    lastTickTimeHi = (u32)(now >> 32);
                } else if (b0 >= 0xb) {
                    u64 now = OS_GetTick();
                    lastTickTime = (u32)now;
                    lastTickTimeHi = (u32)(now >> 32);
                }
            }
        }
    }
}

void HudCountdown::updateSlide() {
    BOOL a = ChatBalloon_IsOwnBusy();
    BOOL b = canShow();
    s32 t;
    s32 c4;
    if (a && b) {
        t = 0xfffec000;
    } else {
        t = 0;
    }
    slideSpeed = slideSpeed + 0xa00;
    c4 = slideSpeed;
    if (c4 < 0x2300) {
        c4 = 0x2300;
    } else if (c4 > 0x5000) {
        c4 = 0x5000;
    }
    slideSpeed = c4;
    if (t != slideTarget) {
        if (slideDelay >= 0 && b) {
            slideDelay = slideDelay + 1;
            if (slideDelay <= 10) {
                goto end;
            }
        }
        slideTarget = t;
        slideDelay = 0;
    } else {
        slideDelay = 0;
    }
end:
    Math_ApproachS32(&slideY, slideTarget, 0x600, slideSpeed, 0x2300);
}

void HudCountdown::resetSlide() {
    slideY = 0;
    slideTarget = 0;
    slideDelay = -1;
    slideSpeed = 0;
}

HudCameraGrid::HudCameraGrid() {
    state = 0;
    cell = 0;
    cellX = 0;
    cellY = 0;
    activeCell = 0;
    visible = 0;
}

HudCameraGrid::~HudCameraGrid() { resetCellAnims(); }

void HudCameraGrid::draw() {
    if (state != 0) {
        s32 bx = getOriginX();
        s32 by = getOriginY();
        s32 i = 0;
        s32 nb = -1;
        s32 z = 0;
        for (; i < 9; i++) {
            SpriteAnim *e = &cellAnims[i];
            GXOamAttr *p = e->getCell();  // (SpriteAnimFrame::cell: an OAM cell)
            if (p != NULL) {
                s32 sx = e->getFrameX(nb);
                s32 sy = e->getFrameY(nb);
                s32 pal = (i == activeCell) ? 6 : 5;
                Oam_DrawCell(z, p, bx + sx, by + sy, pal, nb, 0x1000, 0x1000, z, nb, z, z);
            }
        }
    }
}

void HudCameraGrid::update() {
    static Unk_0208b728_Fn tbl[4] = {(Unk_0208b728_Fn)&HudCameraGrid::updateClosed, (Unk_0208b728_Fn)&HudCameraGrid::updateOpening,
                                     (Unk_0208b728_Fn)&HudCameraGrid::updateOpen, (Unk_0208b728_Fn)&HudCameraGrid::updateClosing};
    (this->*tbl[state])();
}

void HudCameraGrid::reset() {
    visible = 0;
    cell = 0;
    cellX = 0;
    cellY = 0;
    activeCell = 0;
    enterClosed();
}

void HudCameraGrid::resetCellAnims() {
    s32 i;
    for (i = 0; i < 9; i++) {
        cellAnims[i].restart();
    }
}

void HudCameraGrid::callUpdate() { update(); }

void HudCameraGrid::callDraw() {
    if (state == 2) {
        Input_IsButtonMode();
    }
    draw();
}

void HudCameraGrid::setVisible(u32 v) { visible = v; }

BOOL HudCameraGrid::isClosed() {
    if (state == 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL HudCameraGrid::isOpen() {
    if (state == 2) {
        return TRUE;
    }
    return FALSE;
}

BOOL HudCameraGrid::isSettled() {
    s32 s = state;
    BOOL two = (s == 2) ? TRUE : FALSE;
    BOOL r = FALSE;
    if (s == 0) {
        return TRUE;
    }
    if (two) {
        r = TRUE;
    }
    return r;
}

BOOL HudCameraGrid::isOtherDeviceTriggered() {
    BOOL r = FALSE;
    if (Input_IsTouchMode()) {
        if (Input_IsAnyKeyTrig()) {
            r = TRUE;
        }
    } else if (Input_IsButtonMode()) {
        if (Input_IsTouchTrig()) {
            r = TRUE;
        }
    }
    return r;
}

BOOL HudCameraGrid::pickCellByTouch() {
    BOOL r = FALSE;
    s32 i = 0;
    for (; i < 9; i++) {
        const u8 *q = &kHudCameraGridTouchPos[i * 2];
        s32 a = q[0] + 0x80;
        s32 b = q[1] + 0x60;
        if (Input_IsTouchTrigInRect(a, a + 0x10, b, b + 0x10)) {
            r = TRUE;
            cell = i;
            setPosFromCell();
            break;
        }
    }
    return r;
}

BOOL HudCameraGrid::moveByDpad() {
    s32 ox = cellX;
    s32 oy = cellY;
    u32 k = gPad[1];
    if ((k & 0x10) != 0) {
        cellX = cellX + 1;
    } else if ((k & 0x20) != 0) {
        cellX = cellX - 1;
    } else if ((k & 0x40) != 0) {
        cellY = cellY - 1;
    } else if ((k & 0x80) != 0) {
        cellY = cellY + 1;
    }
    s32 t = cellX;
    if (t < -1) t = -1;
    else if (t > 1) t = 1;
    cellX = t;
    t = cellY;
    if (t < -1) t = -1;
    else if (t > 1) t = 1;
    cellY = t;
    setCellFromPos();
    if (ox != cellX || oy != cellY) {
        return TRUE;
    }
    return FALSE;
}

void HudCameraGrid::applySelection() {
    Camera_SetPresetCell((kHudCameraGridCells + 1)[cell * 2], kHudCameraGridCells[cell * 2]);
    s32 p = activeCell;
    if (cell != p) {
        s32 a = (kHudCameraGridDirs + 1)[cell * 2];
        s32 b = (kHudCameraGridDirs + 1)[p * 2];
        s32 snd;
        if (a < b) {
            snd = 0x4d;
        } else if (a > b) {
            snd = 0x4c;
        } else {
            snd = 0x4e;
        }
        Snd_PlaySe(snd);
        activeCell = cell;
    }
}

void HudCameraGrid::setCellFromPos() {
    s32 i = 0;
    while (i < 9) {
        s32 n = i * 2;
        const s8 *e = kHudCameraGridDirs + n;
        if (cellX == kHudCameraGridDirs[n] && cellY == e[1]) break;
        i++;
    }
    cell = i;
}

void HudCameraGrid::setPosFromCell() {
    cellX = kHudCameraGridDirs[cell * 2];
    cellY = (kHudCameraGridDirs + 1)[cell * 2];
}

void HudCameraGrid::enterClosed() {
    state = 0;
}

void HudCameraGrid::updateClosed() {
    if (visible != 0) {
        enterOpening();
    }
}

void HudCameraGrid::enterOpening() {
    s32 one = 1;
    state = one;
    s32 i;
    for (i = 0; i < 9; i++) {
        SpriteAnim *o = &cellAnims[i];
        o->setSeq((SpriteAnimSeq *)gSpriteSeqTable[i + 0xe]);
        o->setPlayOnce(one);
        o->restart();
    }
}

void HudCameraGrid::updateOpening() {
    BOOL z = FALSE;
    BOOL r = TRUE;
    s32 i;
    for (i = 0; i < 9; i++) {
        SpriteAnim *o = &cellAnims[i];
        o->update();
        if (!o->isFinished()) {
            r = z;
        }
    }
    if (r) {
        enterOpen();
    }
}

void HudCameraGrid::enterOpen() {
    state = 2;
}

void HudCameraGrid::updateOpen() {
    if (visible == 0) {
        enterClosing();
    } else if (!isOtherDeviceTriggered()) {
        BOOL f = FALSE;
        if (Input_IsTouchMode()) {
            if (pickCellByTouch()) {
                f = TRUE;
            }
        } else if (Input_IsButtonMode()) {
            if (moveByDpad()) {
                f = TRUE;
            }
        }
        if (f) {
            applySelection();
        }
    }
}

void HudCameraGrid::enterClosing() {
    state = 3;
    s32 i;
    for (i = 0; i < 9; i++) {
        SpriteAnim *o = &cellAnims[i];
        o->setSeq((SpriteAnimSeq *)gSpriteSeqTable[i + 0x17]);
        o->setPlayOnce(1);
        o->restart();
    }
}

void HudCameraGrid::updateClosing() {
    BOOL z = FALSE;
    BOOL r = TRUE;
    s32 i;
    for (i = 0; i < 9; i++) {
        SpriteAnim *o = &cellAnims[i];
        o->update();
        if (!o->isFinished()) {
            r = z;
        }
    }
    if (r) {
        enterClosed();
    }
}

HudCameraButton::HudCameraButton() : state(0), gridActive(0), openedByButton(0), enabled(0) {
}

HudCameraButton::~HudCameraButton() {
    release();
}

void HudCameraButton::draw() {
    if (state != 0) {
        GXOamAttr *v = anim.getCell();  // (SpriteAnimFrame::cell: an OAM cell)
        if (v != 0) {
            s32 x = anim.getFrameX(-1);
            s32 y = anim.getFrameY(-1);
            s32 bx = getOriginX();
            s32 by = getOriginY();
            s32 t = ((u32)(state - 3) <= 2) ? 6 : 5;
            Oam_DrawCell(0, v, bx + x, by + y, t, -1, 0x1000, 0x1000, 0, -1, 0, 0);
        }
    }
}

void HudCameraButton::update() {
    static Unk_020e0f2c_Fn tbl[7] = {
        (Unk_020e0f2c_Fn)&HudCameraButton::updateHidden, (Unk_020e0f2c_Fn)&HudCameraButton::updateAppearing,
        (Unk_020e0f2c_Fn)&HudCameraButton::updateShown, (Unk_020e0f2c_Fn)&HudCameraButton::updateGridOpening,
        (Unk_020e0f2c_Fn)&HudCameraButton::updateGridOpen, (Unk_020e0f2c_Fn)&HudCameraButton::updateGridClosing,
        (Unk_020e0f2c_Fn)&HudCameraButton::updateHiding,
    };
    (this->*tbl[state])();
}

void HudCameraButton::reset() {
    enabled = 0;
    gridActive = 0;
    openedByButton = 0;
    enterHidden();
    grid.reset();
}

void HudCameraButton::release() {
    grid.resetCellAnims();
    anim.restart();
}

void HudCameraButton::callUpdate() {
    update();
    grid.callUpdate();
    updateInputMode();
}

void HudCameraButton::callDraw() {
    draw();
    grid.callDraw();
}

void HudCameraButton::enable() {
    enabled = 1;
}

void HudCameraButton::disable() {
    enabled = 0;
}

BOOL HudCameraButton::isHidden() {
    if (state == 0 && grid.isClosed()) {
        return TRUE;
    }
    return FALSE;
}

BOOL HudCameraButton::canShow() {
    BOOL a;
    if (TalkRequest_IsActive()) {
        a = TRUE;
    } else {
        a = FALSE;
    }
    u32 v = PlayerActor_GetAction(4);
    BOOL b, c;
    if (v - 8 <= 7) b = TRUE; else b = FALSE;
    if (v - 0x24 <= 8) c = TRUE; else c = FALSE;
    BOOL r;
    if (enabled != 0 && !a && !b && !c) {
        r = TRUE;
    } else {
        r = FALSE;
    }
    return r;
}

BOOL HudCameraButton::isTogglePressed(s32 flag) {
    BOOL r = FALSE;
    BOOL f = r;
    if (gridActive != 0) {
        if (Input_IsTouchMode()) {
            if (Input_IsAnyKeyTrig()) {
                f = TRUE;
            }
        } else if (Input_IsButtonMode()) {
            if (Input_IsTouchTrig()) {
                f = TRUE;
            }
        }
    }
    if (!f) {
        if (grid.isSettled()) {
            u32 k = gPad[1];
            if ((k & 0x400) != 0 || (flag != 0 && (k & 2) != 0)) {
                r = TRUE;
                openedByButton = 1;
            } else {
                BOOL c;
                if (gTouchHeld != 0 && gTouchChanged != 0) {
                    c = TRUE;
                } else {
                    c = FALSE;
                }
                if (c) {
                    s32 a = gTouchPressX;
                    if ((s32)gTouchPressY < 16 && a >= 0xc8 && a < 0xe8) {
                        r = TRUE;
                        openedByButton = 0;
                    }
                }
            }
        }
    }
    return r;
}

void HudCameraButton::updateInputMode() {
    if (gridActive != 0) {
        if (Input_IsTouchMode()) {
            if (Input_IsAnyKeyTrig()) {
                Input_SetButtonMode();
            }
        } else if (Input_IsButtonMode()) {
            if (Input_IsTouchTrig()) {
                Input_SetTouchMode();
            }
        }
        Input_StoreMode();
    }
}

void HudCameraButton::enterHidden() {
    state = 0;
}

void HudCameraButton::updateHidden() {
    if (canShow()) {
        enterAppearing();
    }
}

void HudCameraButton::enterAppearing() {
    state = 1;
    anim.setSeq((SpriteAnimSeq *)(data_020d477c));
    anim.setPlayOnce(1);
    anim.restart();
}

void HudCameraButton::updateAppearing() {
    anim.update();
    if (anim.isFinished()) {
        enterShown();
    }
}

void HudCameraButton::enterShown() {
    state = 2;
    openedByButton = 0;
    anim.setSeq((SpriteAnimSeq *)(data_020d4784));
    anim.setPlayOnce(1);
    anim.restart();
    anim.pause();
}

void HudCameraButton::updateShown() {
    if (TalkRequest_IsCameraViewRunning()) {
        MenuCtrl_ResetForceClose();
        enterGridOpening();
    } else if (!canShow()) {
        enterHiding();
    } else if (isTogglePressed(0)) {
        TalkRequest_AddCameraView();
    }
}

void HudCameraButton::enterGridOpening() {
    state = 3;
    grid.setVisible(1);
    gridActive = 1;
    if (openedByButton != 0) {
        Input_SetButtonMode();
    } else {
        Input_SetTouchMode();
    }
    Snd_PlaySe(0x4a);
}

void HudCameraButton::updateGridOpening() {
    if (grid.isOpen()) {
        enterGridOpen();
    }
}

void HudCameraButton::enterGridOpen() {
    state = 4;
    anim.setSeq((SpriteAnimSeq *)(data_020d478c));
    anim.setPlayOnce(1);
    anim.restart();
}

void HudCameraButton::updateGridOpen() {
    BOOL a = isTogglePressed(1);
    BOOL b = MenuCtrl_IsForceCloseDue();
    if (a || b) {
        enterGridClosing();
    }
}

void HudCameraButton::enterGridClosing() {
    state = 5;
    grid.setVisible(0);
    Snd_PlaySe(0x4b);
}

void HudCameraButton::updateGridClosing() {
    if (grid.isClosed()) {
        TalkRequest_FinishCameraView();
        gridActive = 0;
        enterShown();
    }
}

void HudCameraButton::enterHiding() {
    state = 6;
    anim.setSeq((SpriteAnimSeq *)(data_020d4784));
    anim.setPlayOnce(1);
    anim.restart();
}

void HudCameraButton::updateHiding() {
    anim.update();
    if (anim.isFinished()) {
        enterHidden();
    }
}

HudWallet::HudWallet()
    : state(0), baseY(0), slideY(0), slideTarget(0), slideSpeed(0), slideDelay(-1), label(NULL) {
    shownBells = 0;
    showRequested = 0;
    rolling = 0;
    valueFrozen = 0;
}

HudWallet::~HudWallet() {
    release();
}

void HudWallet::draw() {
    if (state != 0) {
        GXOamAttr *h = anim.getCell();  // (SpriteAnimFrame::cell: an OAM cell)
        if (h != 0) {
            s32 a = getOriginX();
            s32 x = a + anim.getFrameX(-1);
            s32 b = (slideY + 0x800) >> 12;
            s32 c = getOriginY();
            s32 e = anim.getFrameY(-1);
            s32 y = b;
            y += baseY + (c + e);
            Oam_DrawCell(0, h, x, y, -1, -1, 0x1000, 0x1000, 0, -1, 0, 0);
        }
    }
}

void HudWallet::update() {
    static Fn tbl[4] = {
        (Fn)&HudWallet::updateHidden, (Fn)&HudWallet::updateAppearing,
        (Fn)&HudWallet::updateShown, (Fn)&HudWallet::updateHiding,
    };
    (this->*tbl[state])();
}

void HudWallet::reset() {
    showRequested = 0;
    syncValue();
    resetSlide();
    enterHidden();
}

void HudWallet::release() {
    setRolling(0);
    anim.restart();
    freeLabel();
}

void HudWallet::callUpdate() {
    update();
    updateSlide();
}

void HudWallet::callDraw() {
    updateBaseY();
    draw();
}

void HudWallet::show() {
    showRequested = 1;
}

void HudWallet::hide() {
    showRequested = 0;
}

BOOL HudWallet::isHidden() {
    if (state == 0) {
        return TRUE;
    }
    return FALSE;
}

void HudWallet::freezeValue() {
    valueFrozen = 1;
}

void HudWallet::unfreezeValue() {
    valueFrozen = 0;
}

void HudWallet::enterHidden() {
    state = 0;
}

void HudWallet::updateHidden() {
    if (showRequested != 0) {
        enterAppearing();
    }
}

void HudWallet::enterAppearing() {
    state = 1;
    anim.setSeq((SpriteAnimSeq *)(data_020d4794));
    anim.setPlayOnce(1);
    anim.restart();
    syncValue();
    createLabel();
}

void HudWallet::updateAppearing() {
    anim.update();
    if (anim.isFinished() != 0) {
        enterShown();
    }
}

void HudWallet::enterShown() {
    state = 2;
}

void HudWallet::updateShown() {
    BOOL r = rollTowardTarget();
    if (showRequested == 0) {
        enterHiding();
        r = FALSE;
    }
    setRolling(r);
}

void HudWallet::enterHiding() {
    state = 3;
    anim.setSeq((SpriteAnimSeq *)(data_020d479c));
    anim.setPlayOnce(1);
    anim.restart();
}

void HudWallet::updateHiding() {
    anim.update();
    if (anim.isFinished() != 0) {
        freeLabel();
        enterHidden();
    }
}

void HudWallet::createLabel() {
    if (label == NULL) {
        label = MsgTextLabel_CreateVram(0x80, 8, 1);
        if (label != NULL) {
            label->vramLoader = 4;
            label->copyMode = 2;
            label->rowStride1K = 1;
            label->bgColor = 0;
            label->fgColor = 0xc;
            TextLabel *o = label;
            o->textStart = text.data();
            label->font = (GameFontDesc *)&gFontD;  // (a GameFont: a GameFontDesc)
            label->alignRight();
            label->requestRedraw();
        }
    }
}

void HudWallet::freeLabel() {
    if (label != NULL) {
        MsgTextLabel_Destroy(label);
        label = NULL;
    }
}

void HudWallet::syncValue() {
    shownBells = Hud_GetBells();
    formatValue();
    rolling = 0;
    valueFrozen = 0;
}

BOOL HudWallet::rollTowardTarget() {
    BOOL r = FALSE;
    if (valueFrozen == 0) {
        s32 v = Hud_GetBells();
        if (shownBells != v) {
            s32 d = v - shownBells;
            if (d < 0) {
                d = -d;
            }
            s32 t = (d / 6 + 0x32) / 10;
            Math_StepS32Alt(&shownBells, v, t * 10 + 7);
            formatValue();
            if (label != NULL) {
                label->alignRight();
                label->requestRedraw();
            }
            r = TRUE;
        }
    }
    return r;
}

void HudWallet::formatValue() {
    String_FormatNumber(&text, shownBells, 7, 1, 0, 1);
}

void HudWallet::setRolling(BOOL v) {
    if (rolling != 0) {
        if (v == 0) {
            Snd_StopBellRollSe();
            Snd_PlaySe(0x2e);
        }
    } else if (v != 0) {
        Snd_StartBellRollSe();
    }
    rolling = v;
}

extern "C" s32 Hud_GetBells() {
    PlayerData *c = PlayerData_GetCurrent();
    s32 r = 0;
    if (c != 0) {
        s32 a = ((PlayerInventory *)(c->getInventory()))->getTotalBells(1);
        r = a + MenuCtrl_GetHandBells();
    }
    return r;
}

void HudWallet::updateBaseY() {
    s32 v = MenuCtrl_GetTransitionProgressOrFull();
    s32 a = func_01ffcb0c(0, v);
    s32 b = func_01ffcb0c(0xc0000, 0x1000 - v);
    baseY = (a + b) >> 12;
}

void HudWallet::updateSlide() {
    if (MenuCtrl_IsTransitionActive() != 0) {
        s32 r;
        if (ChatBalloon_IsRemoteBusy() != 0) {
            r = 0x28000;
        } else {
            r = 0;
        }
        slideSpeed = slideSpeed + 0xa00;
        s32 t = slideSpeed;
        if (t < 0x2300) {
            t = 0x2300;
        } else if (t > 0x5000) {
            t = 0x5000;
        }
        slideSpeed = t;
        if (r != slideTarget) {
            if (slideDelay < 0 || (slideDelay = slideDelay + 1, slideDelay > 5)) {
                slideTarget = r;
                slideDelay = 0;
            }
        } else {
            slideDelay = 0;
        }
        Math_ApproachS32(&slideY, slideTarget, 0x600, slideSpeed, 0x2300);
    } else {
        resetSlide();
    }
}

void HudWallet::resetSlide() {
    slideY = 0;
    slideTarget = 0;
    slideDelay = -1;
    slideSpeed = 0;
}

HudController::HudController() : state(0) {
    hideRequest = 0;
    hideRequestB = 0;
    buttonLayout = 0;
}

HudController::~HudController() {}

extern "C" void Hud_Init() { gHud.init(); }

extern "C" void Hud_Exit() { gHud.exit(); }

extern "C" void Hud_Update() { gHud.update(); }

extern "C" void Hud_Draw() { gHud.draw(); }

extern "C" void Hud_Hide() { gHud.hideRequest = 1; }

extern "C" void Hud_Show() { gHud.hideRequest = 0; }

extern "C" void Hud_ClearHideB() { gHud.hideRequestB = 0; }

extern "C" void *Hud_GetCountdown() { return &gHud.countdown; }

extern "C" void *Hud_GetWallet() { return &gHud.wallet; }

void HudController::init() {
    Hud_Show();
    Hud_ClearHideB();
    ((HudControllerStates *)(this))->enterModeForRoom();
    clock.reset();
    countdown.reset();
    ((HudCameraButton *)(&cameraButton))->reset();
    wallet.reset();
    buttonLayout = 0;
}

void HudController::exit() {
    wallet.release();
    ((HudCameraButton *)(&cameraButton))->release();
    countdown.release();
    clock.release();
}

void HudController::update() {
    static Fn tbl[6] = {
        (Fn)&HudController::updateOff, (Fn)&HudController::updateClock, (Fn)&HudControllerStates::updateCountdown,
        (Fn)&HudControllerStates::updateCameraButton, (Fn)&HudControllerStates::updateWallet, (Fn)&HudControllerStates::updateHidden,
    };
    (this->*tbl[state])();
    clock.callUpdate();
    countdown.callUpdate();
    ((HudCameraButton *)(&cameraButton))->callUpdate();
    wallet.callUpdate();
    ((HudControllerStates *)(this))->updateInputLayout();
}

void HudController::draw() {
#ifdef N3DS_DIRECT  // (n3ds_ui.h: each panel to the wide screen's edges it is near; they draw into buffer A)
    int n = Oam_Count(0);
    clock.callDraw();
    Oam_Anchor(0, n, N3DS_UI_NEAR(0)), n = Oam_Count(0);
    countdown.callDraw();
    Oam_Anchor(0, n, N3DS_UI_NEAR(1)), n = Oam_Count(0);
    ((HudCameraButton *)(&cameraButton))->callDraw();
    Oam_Anchor(0, n, N3DS_UI_NEAR(2)), n = Oam_Count(0);
    wallet.callDraw();
    Oam_Anchor(0, n, N3DS_UI_NEAR(3));
#else
    clock.callDraw();
    countdown.callDraw();
    ((HudCameraButton *)(&cameraButton))->callDraw();
    wallet.callDraw();
#endif
}

void HudController::enterOff() {
    state = 0;
}

void HudController::updateOff() {
    if (hideRequest != 0 || hideRequestB != 0) {
        ((HudControllerStates *)(this))->enterHidden();
    }
}

void HudController::enterClock() {
    state = 1;
}

void HudController::updateClock() {
    BOOL a;
    BOOL b;
    if (hideRequest != 0 || hideRequestB != 0) {
        a = TRUE;
    } else {
        a = FALSE;
    }
    b = countdown.isFinished() == 0 ? TRUE : FALSE;
    if (a || b) {
        clock.hide();
        if (clock.isHidden() != 0) {
            if (a) {
                ((HudControllerStates *)(this))->enterHidden();
            } else {
                enterCountdown();
            }
        }
    } else {
        clock.show();
    }
}

void HudController::enterCountdown() {
    state = 2;
    HudObjGfx_SetCountdownVariant(1);
    HudObjGfx_LoadKind(1, 1);
}

void HudControllerStates::updateCountdown() {
    BOOL r5;
    if (hideRequest != 0 || hideRequestB != 0) {
        r5 = TRUE;
    } else {
        r5 = FALSE;
    }
    BOOL r0 = countdown.isFinished();
    if (r5 || r0) {
        countdown.hide();
        if (countdown.isHidden()) {
            if (r5) {
                enterHidden();
            } else {
                HudObjGfx_SetCountdownVariant(0);
                HudObjGfx_LoadKind(4, 1);
                ((HudController *)(this))->enterClock();
            }
        }
    } else {
        countdown.show();
    }
}

void HudControllerStates::enterCameraButton() { state = 3; }

void HudControllerStates::updateCameraButton() {
    if (hideRequest != 0 || hideRequestB != 0) {
        ((HudCameraButton *)(&cameraButton))->disable();
        if (((HudCameraButton *)(&cameraButton))->isHidden()) {
            enterHidden();
        }
    } else {
        ((HudCameraButton *)(&cameraButton))->enable();
    }
    updateSharedPanels();
}

void HudControllerStates::enterWallet() { state = 4; }

void HudControllerStates::updateWallet() {
    ((HudWallet *)(&wallet))->show();
    updateSharedPanels();
}

void HudControllerStates::enterHidden() {
    HudObjGfx_LoadKind(3, 1);
    state = 5;
}

void HudControllerStates::updateHidden() {
    if (hideRequest != 0 || hideRequestB != 0) {
        ((HudWallet *)(&wallet))->show();
    } else {
        ((HudWallet *)(&wallet))->hide();
        if (((HudWallet *)(&wallet))->isHidden()) {
            HudObjGfx_LoadKind(4, 1);
            enterModeForRoom();
        }
    }
    updateSharedPanels();
}

void HudControllerStates::updateSharedPanels() {
    if (Hud_GetSceneHudKind() != 0) {
        if (Unk_0208a150_IsOne(gFieldSceneKind)) {
            if (countdown.isFinished()) {
                countdown.hide();
                if (countdown.isHidden()) {
                    clock.show();
                }
            } else {
                clock.hide();
                if (clock.isHidden()) {
                    countdown.show();
                }
            }
        }
    }
}

void HudControllerStates::enterModeForRoom() {
    s32 r = Hud_GetSceneHudKind();
    if (r == 1) {
        if (HudObjGfx_GetCountdownVariant()) {
            ((HudController *)(this))->enterCountdown();
        } else {
            ((HudController *)(this))->enterClock();
        }
    } else if (r == 2) {
        enterCameraButton();
    } else if (r == 3) {
        enterWallet();
    } else {
        ((HudController *)(this))->enterOff();
    }
}

void HudControllerStates::updateInputLayout() {
    BOOL r = FALSE;
    if (buttonLayout != 0) {
        if (InputMode_IsButtons()) {
            r = TRUE;
        }
    } else {
        if (InputMode_IsTouch()) {
            r = TRUE;
        }
    }
    if (r) {
        buttonLayout = buttonLayout == 0 ? 1 : 0;
        if (state == 3) {
            HudObjGfx_LoadCameraButton(buttonLayout, 1, 1);
        }
    }
}

extern "C" HudProc *HudProc_Create() { return new HudProc(); }

HudProc::HudProc() {}

HudProc::~HudProc() {}

BOOL HudProc::onCreate() {
    HudLinkIcon_Reset();
    HudUnkSlideIcon_Reset();
    HudUnkIcon_Reset();
    Hud_Init();
    return TRUE;
}

BOOL HudProc::onDelete() {
    Hud_Exit();
    HudUnkIcon_Exit();
    HudUnkSlideIcon_Exit();
    HudLinkIcon_Exit();
    return TRUE;
}

BOOL HudProc::onExecute() {
    HudLinkIcon_Update();
    HudUnkSlideIcon_Update();
    HudUnkIcon_Update();
    Hud_Update();
    return TRUE;
}

BOOL HudProc::onDraw() {
#ifdef N3DS_DIRECT  // (n3ds_ui.h: each icon to the wide screen's edges it is near; Hud_Draw anchors its panels)
    HudUnkSlideIcon_Draw();  // (not anchored: it slides sideways round the middle - not seen yet)
    int n = Oam_Count(0);
    HudLinkIcon_Draw();
    Oam_Anchor(0, n, N3DS_UI_NEAR(5));
    Hud_Draw();
    n = Oam_Count(0);
    HudUnkIcon_Draw();
    Oam_Anchor(0, n, N3DS_UI_NEAR(6));
#else
    HudUnkSlideIcon_Draw();
    HudLinkIcon_Draw();
    Hud_Draw();
    HudUnkIcon_Draw();
#endif
    return TRUE;
}

// ---------------------------------------------------------------------------------------------------------------------
// Data

char sHudColonStr[] = ":";
char sHudAmStr[] = "AM";
char sHudPmStr[] = "PM";

char *const kHudColonString = sHudColonStr;
char *const kHudAmPmStrings[2] = {sHudAmStr, sHudPmStr};
const s16 kHudCountdownSeconds[6] = {0, 180, 300, 600, 900, 0};
// one-glyph strings: the weekday symbols of the clock font
const u8 kHudWeekdayGlyphs[8][2] = {{'g', 0}, {'a', 0}, {'b', 0}, {'c', 0}, {'d', 0}, {'e', 0}, {'f', 0}, {0, 0}};
const u8 kHudCameraGridTouchPos[18] = {0x5b, 0x3c, 0x49, 0x2b, 0x5b, 0x2b, 0x6c, 0x2b, 0x49,
                              0x3c, 0x6c, 0x3c, 0x49, 0x4c, 0x5b, 0x4c, 0x6c, 0x4c};
const s8 kHudCameraGridDirs[18] = {0, 0, -1, -1, 0, -1, 1, -1, -1, 0, 1, 0, -1, 1, 0, 1, 1, 1};
const u8 kHudCameraGridCells[18] = {1, 1, 0, 0, 1, 0, 2, 0, 0, 1, 2, 1, 0, 2, 1, 2, 2, 2};
// 0x020cf63c: last .rodata object of this file (0x14 bytes, continues the ascending size run); read by the unit at
// 0x0208d154 (0x0208d324)
extern const s32 kNameLabelBalloonKindAnims[5];
const s32 kNameLabelBalloonKindAnims[5] = {10, 11, 12, 13, 0x28};

ProcProfile sHudProcProfile = {(void *(*)())HudProc_Create, 0xca, 0x8e};

HudController gHud;


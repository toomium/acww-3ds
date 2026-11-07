#include "uptr.h"
#include "host_labels.h"
#include "types.h"
#include "sys/new.h"
#include "text/Unk_02050288.h"
#include "talk/MsgStringBase.h"
#include "talk/MsgTag.h"
#include "talk/MsgRunner.h"
#include "talk/TalkWindowState.h"
#include "talk/BmgReader.h"
#include "talk/BmgMsgAttr.h"
#include "talk/MsgParser.h"
#include "talk/MsgTextLabel.h"
#include "talk/MsgRequest.h"
#include "talk/MsgProcessor.h"
#include "talk/TalkBmgReader.h"
#include "talk/MsgWalker.h"
#include "room/HouseData.h"
#include "talk/MsgString9.h"
#include "talk/MsgString.h"
#include "talk/MsgString33.h"
#include "talk/TalkMsgRequest.h"
#include "talk/MsgString17.h"
#include "talk/MsgString17B.h"
#include "talk/MsgString9B.h"
#include "talk/MsgString25.h"
#include "talk/MsgString11.h"
#include "talk/MsgString9C.h"
#include "talk/ChoiceList.h"
#include "talk/ChoiceMenu.h"
#include "talk/ChoiceString.h"
#include "ui/TalkArrow.h"
#include "nitro/fs.h"
#include "net/CommManager.h"
#include "nitro/io_access.h"
#include "player/PlayerData.h"
#include "player/PlayerId.h"
#include "snd/BgmVolumeMixer.h"
#include "snd/BgmManager.h"
#include "save/SaveData.h"
#include "npc/VillagerDataItemView.h"
#include "save/VillagerData.h"
#include "save/VillagerData.h"
#include "talk/TalkFrame.h"
#include "talk/TalkTextBox.h"
#include "talk/TalkParser.h"
#include "talk/TalkVoice.h"
#include "talk/TalkTagScanner.h"
#include "talk/ArticleCacheEntry.h"
#include "proto/autoload_2/unk_02119434.h"
#include "proto/autoload_2/unk_020e7500.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_021104ac.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_0203c23c.h"
#include "proto/main/unk_0203c92c.h"
#include "proto/main/unk_02038474.h"
#include "proto/main/unk_020501d4.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_020637e4.h"
#include "proto/main/unk_02065e88.h"
#include "proto/main/unk_020b2c50.h"
#include "proto/main/unk_020742f4.h"
#include "proto/main/unk_02097444.h"
#include "n3ds_ui.h"

struct VillagerState;

// Declared here: defined in units whose prototypes are not shared yet (include/proto/.excluded), in assembly or in
// data, as they are defined.
extern "C" {
// src/main/unk_020a6974.cpp
MsgTextLabel *MsgTextLabel_CreateVram(u32 a, s32 b, s32 c);
void MsgTextLabel_Destroy(MsgTextLabel *obj);
u8 *Msg_GetColorTag(s32 i);
u8 *Msg_FindTag(u8 *text, s32 group, s32 id);  // (the tag's address or NULL)
void MsgTag_DtorStub();
u8 *Bmg_GetMsgAttr(u8 *p);
void BmgMsgAttr_Copy(BmgMsgAttr *d, BmgMsgAttr *s);
u8 BmgMsgAttr_GetByte05(BmgMsgAttr *s);
u8 BmgMsgAttr_GetByte06(BmgMsgAttr *s);
u8 BmgMsgAttr_GetByte07(BmgMsgAttr *s);
u8 BmgMsgAttr_GetByte09(BmgMsgAttr *s);
void BmgMsgAttr_GetByte08(u8 *out, BmgMsgAttr *s);
u32 BmgMsgAttr_LookupUnkA(BmgMsgAttr *s);
u32 BmgMsgAttr_LookupUnkB(BmgMsgAttr *s);
u32 BmgMsgAttr_LookupUnkC(BmgMsgAttr *s);
// src/main/unk_02077ac4.cpp (the villager data: VillagerData, VillagerMemory are declared there only)
u32 Villager_GetAnimalKind(VillagerData *a);
VillagerMemory *Villager_FindMemory(void *a, void *b);
s32 Villager_FindMemoryIndex(VillagerMemory *t, void *id);
VillagerMemory *Villager_GetMemory(VillagerMemory *t, u32 i);
void Villager_GetNicknameFor(void *a, void *b, void *c);
s32 Villager_GetTrend(VillagerData *a, MsgString *out);
void Villager_GetFriendName(void *a, MsgString *b);
void Villager_GetEnemyName(void *a, MsgString *b);
void Villager_GetRandomOtherName(void *a, MsgString *b);
void Villager_GetImpressionText(void *self, MsgString *out, PlayerData *p);
VillagerState *Villager_GetState(VillagerData *a);
u32 VillagerState_GetMood(VillagerState *e);
s32 SaveVillagers_Count(u8 *base);
// the clock, input, weather
u32 Clock_GetYear();
void Clock_GetDayMonth(u8 *out);
void Clock_GetMinuteHour(u8 *out);
s32 Clock_GetWeekday();
u32 Clock_GetSecond();
s32 Clock_GetTimeOfDay();
BOOL Input_IsTouchTrigInRect(s32 x0, s32 x1, s32 y0, s32 y1);
BOOL Input_IsBHeld(void);
BOOL Input_IsATrig(void);
BOOL Input_IsBTrig(void);
BOOL Input_IsTouchMode(void);
BOOL Input_IsAnyKeyTrig(void);
void Input_SetButtonMode(void);
u8 Input_IsButtonMode(void);
BOOL Input_IsTouchTrig();
void Input_SetTouchMode(void);
void Input_StoreMode(void);
void Input_LoadMode(void);
s32 Weather_GetFallingPrecip(void);
// the SDK, the C library
s32 func_01ffcb0c(s32 v1, s32 v2);  // (fx32)
void MI_CpuFill8(void *dst, u32 value, u32 size);
void DC_FlushRange(void *p, u32 size);
void GX_LoadBGPltt(const void *src, u32 offset, u32 size);
void GX_LoadBG2Char(const void *src, u32 offset, u32 size);
void GX_LoadBG2Scr(const void *src, u32 offset, u32 size);
BOOL FS_OpenFile(FSFile *file, const char *path);
s32 FS_ReadFile(FSFile *file, void *dst, s32 len);
BOOL FS_CloseFile(FSFile *file);
char *strncpy(char *dst, const char *src, u32 n);
s32 strncmp(const char *a, const char *b, u32 n);
u32 strlen(const char *s);
// data
extern const s16 data_02135f44[];
extern u8 gTalkMsgIndexNone;
extern u8 gTalkMsgIndexEnd;
extern u16 sCharSortKeyZero;
extern u16 data_020cbadc;
extern u16 data_020cbae0;
extern u16 data_020cbae4;
extern s32 data_020cbf90;
extern BgmManager *data_021c1b3c;
extern CommManager *gCommManager;
// this unit's data (defined below)
extern const u8 data_020cba1c[0x6];
extern const u8 data_020cba14[0x5];
extern const u8 data_020cba0c[0x3];
extern const u8 data_020cba24[0x7];
extern const s8 sTalkFriendshipDeltas[0x21];
extern const char *const sTalkMessageDirs[9];
extern const char *const sTalkMessageSubdirs[26];
extern const u16 data_020cba10[2];
}
extern SaveData gSaveData;
#define gSaveHouse HOST_LABEL(HouseData *, gSaveHouse)  // (gSaveData.house)

// Msg_FindTag with the text's address as a pointer
static inline u8 *Msg_FindTagPtr(u8 *p, s32 group, s32 id) { return Msg_FindTag(p, group, id); }

// ======== unk_0206c714.cpp ========
namespace n0 {
extern "C" { extern const u8 data_020cba1c[0x6]; const u8 data_020cba1c[0x6] = {26, 5, 1, 1, 0, 0}; }
extern "C" { TalkWindowState *gTalkWindows; }
}
TalkBmgReader::TalkBmgReader() : BmgReader(1) {}
// ---- TalkBmgReader
TalkBmgReader::~TalkBmgReader() {}

// ======== unk_0206be04.cpp ========
typedef void (TalkRenderProcessor::*Unk_020ddc64_Fn)();

void TalkBmgReader::clear() {
    MI_CpuFill8(buffer, 0, 0x800);
}
u8 *TalkBmgReader::getBuffer() {
    return buffer;
}
u32 TalkBmgReader::getBufferSize() {
    return 0x800;
}
TalkRenderProcessor::TalkRenderProcessor(TalkWindowState *owner) : MsgProcessor(1), window(owner), measureLabel(0) {
    altTextEnd = 0;
    selectEnd = 0;
    articleMode = 0;
    measureLabel = MsgTextLabel_CreateVram(0, 0x14, 2);
    if (measureLabel != 0) measureLabel->setProcessor(this);
}
TalkRenderProcessor::~TalkRenderProcessor() {
    if (measureLabel != 0) MsgTextLabel_Destroy(measureLabel);
}
s32 TalkRenderProcessor::measureLine(u8 *text, BOOL centered) {
    s32 r = 0;
    if (centered != 0) {
        if (measureLabel != 0) {
            measureLabel->textStart = text;
            measureLabel->alignCenter();
            r = measureLabel->xOffset;
            measureLabel->textStart = 0;
        }
    }
    return r;
}
void TalkRenderProcessor::onBegin() {
    if (measureLabel != 0) {
        articleMode = 0;
        altTextEnd = 0;
        selectEnd = 0;
        measureLabel->onBegin();
    }
}
void TalkRenderProcessor::onEnd() {
    if (measureLabel != 0) measureLabel->onEnd();
}
void TalkRenderProcessor::onChar(u32 c) {
    if (measureLabel != 0) measureLabel->onChar(c);
    if (cursor == altTextEnd) {
        altTextEnd = 0;
        popText();
    }
    if (cursor == selectEnd) {
        selectEnd = 0;
        popText();
    }
}
void TalkRenderProcessor::onTag(u8 *p) {
    tag.parse(p);
    dispatchTag();
}
void TalkRenderProcessor::pushArticle(MsgString *e) {
    MsgString *o = 0;
    if (articleMode == 0) {
        o = String_GetArticle(&e->attr.attrA);
    } else if (articleMode == 1) {
        o = String_GetArticle(&e->attr.attrB);
    }
    if (o != 0) pushText(o->data());
    articleMode = 0;
}
void TalkRenderProcessor::expandSlot(s32 idx) {
    MsgString33 *e = &window->slots[idx];
    pushText(e->data());
    pushArticle(e);
}
void TalkRenderProcessor::expandNamedSlot(s32 idx) {
    MsgString33 *e = &window->namedSlots[idx];
    pushText(e->data());
    pushArticle(e);
}
void TalkRenderProcessor::selectByPlayerGender() {
    char *p0, *p1;
    tag.getStrings2(&p0, &p1);
    if (PlayerData_GetCurrent()->getPlayerId()->getGender() == 0) {
        if (p0 != 0) pushText((u8 *)p0);
    } else {
        if (p1 != 0) {
            selectEnd = cursor;
            pushText((u8 *)p1);
        }
    }
}
void TalkRenderProcessor::selectBySlotForm(MsgString *a) {
    char *p0, *p1, *p2;
    tag.getStrings3(&p0, &p1, &p2);
    if (a->attr.form == 0) {
        if (p0 != 0) pushText((u8 *)p0);
    } else if (a->attr.form == 1) {
        if (p1 != 0) pushText((u8 *)p1);
    } else if (a->attr.form == 2) {
        if (p2 != 0) {
            selectEnd = cursor;
            pushText((u8 *)p2);
        }
    }
}
void TalkRenderProcessor::dispatchTag() {
    static Unk_020ddc64_Fn tbl[12] = { &TalkRenderProcessor::tagGroup00, &TalkRenderProcessor::tagGroup01, &TalkRenderProcessor::tagGroup02, 0, &TalkRenderProcessor::tagGroup04, 0, 0, 0, 0, 0, 0, &TalkRenderProcessor::tagGroup0b };
    s32 i = tag.group;
    Unk_020ddc64_Fn f = 0;
    if (i == 0xff) f = &TalkRenderProcessor::tagGroupFf;
    else if (i < 12) f = tbl[i];
    if (f != 0) (this->*f)();
}
void TalkRenderProcessor::tagGroup00() {
    static Unk_020ddc64_Fn tbl[11] = { &TalkRenderProcessor::tagGlyph0, &TalkRenderProcessor::tagGlyph1, 0, 0, 0, 0, &TalkRenderProcessor::tagGlyph6, &TalkRenderProcessor::tagGlyph7, &TalkRenderProcessor::tagGlyph8, 0, 0 };
    Unk_020ddc64_Fn f = tbl[tag.id];
    (this->*f)();
}
void TalkRenderProcessor::tagGroup01() {
    static Unk_020ddc64_Fn tbl[17] = { 0, 0, 0, 0, 0, 0, &TalkRenderProcessor::tagGreeting, &TalkRenderProcessor::tagGreetingEnd, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    Unk_020ddc64_Fn f = tbl[tag.id];
    if (f != 0) (this->*f)();
}
void TalkRenderProcessor::tagGroup02() {
    skip(tag.getTrailingStringsSize());
}
void TalkRenderProcessor::tagGroup04() {
    static Unk_020ddc64_Fn tbl[35] = { &TalkRenderProcessor::tagPlayerName, &TalkRenderProcessor::tagSpeakerName, &TalkRenderProcessor::tagCatchphrase, &TalkRenderProcessor::tagTownName, &TalkRenderProcessor::tagYear, &TalkRenderProcessor::tagMonth, &TalkRenderProcessor::tagDay, &TalkRenderProcessor::tagWeekday, &TalkRenderProcessor::tagHour, &TalkRenderProcessor::tagMinute, &TalkRenderProcessor::tagNumber1714, &TalkRenderProcessor::tagFriendName, &TalkRenderProcessor::tagEnemyName, &TalkRenderProcessor::tagRandomVillagerName, &TalkRenderProcessor::tagOtherResidentName, &TalkRenderProcessor::tagSlot0, &TalkRenderProcessor::tagSlot1, &TalkRenderProcessor::tagSlot2, &TalkRenderProcessor::tagSlot3, &TalkRenderProcessor::tagSlot4, &TalkRenderProcessor::tagSlot5, &TalkRenderProcessor::tagSlot6, &TalkRenderProcessor::tagSlot7, &TalkRenderProcessor::tagSlot8, &TalkRenderProcessor::tagSlot9, &TalkRenderProcessor::tagSlot10, &TalkRenderProcessor::tagNamedSlot0, &TalkRenderProcessor::tagNamedSlot1, &TalkRenderProcessor::tagNamedSlot2, &TalkRenderProcessor::tagNamedSlot3, &TalkRenderProcessor::tagImpression, &TalkRenderProcessor::tagNickname, 0, &TalkRenderProcessor::tagCompliment, &TalkRenderProcessor::tagTrend };
    Unk_020ddc64_Fn f = tbl[tag.id];
    if (f != 0) (this->*f)();
}
void TalkRenderProcessor::tagGroup0b() {
    static Unk_020ddc64_Fn tbl[20] = { 0, &TalkRenderProcessor::tagArticleMode1, &TalkRenderProcessor::tagArticleMode2, 0, &TalkRenderProcessor::tagSelectByPlayerGender, &TalkRenderProcessor::tagSlotForm0, &TalkRenderProcessor::tagSlotForm1, &TalkRenderProcessor::tagSlotForm2, &TalkRenderProcessor::tagSlotForm3, &TalkRenderProcessor::tagSlotForm4, &TalkRenderProcessor::tagSlotForm5, &TalkRenderProcessor::tagSlotForm6, &TalkRenderProcessor::tagSlotForm7, &TalkRenderProcessor::tagSlotForm8, &TalkRenderProcessor::tagSlotForm9, &TalkRenderProcessor::tagSlotForm10, &TalkRenderProcessor::tagNamedSlotForm0, &TalkRenderProcessor::tagNamedSlotForm1, &TalkRenderProcessor::tagNamedSlotForm2, &TalkRenderProcessor::tagNamedSlotForm3 };
    Unk_020ddc64_Fn f = tbl[tag.id];
    if (f != 0) (this->*f)();
}

// ======== unk_0206b4e4.cpp ========
typedef void (TalkRenderProcessor::*Unk_0206bd88_Fn)();

void TalkRenderProcessor::tagGroupFf() {
    static Unk_0206bd88_Fn tbl[3] = { 0, 0, &TalkRenderProcessor::tagAltText };
    Unk_0206bd88_Fn f = tbl[tag.id];
    if (f != 0) (this->*f)();
}
void TalkRenderProcessor::tagGlyph0() { pushText((u8 *)Text_GetSpecialCharStr6()); }
void TalkRenderProcessor::tagGlyph1() { pushText((u8 *)Text_GetSpecialCharStr7()); }
void TalkRenderProcessor::tagGlyph6() { pushText((u8 *)Text_GetSpecialCharStr5()); }
void TalkRenderProcessor::tagGlyph7() { pushText((u8 *)Text_GetSpecialCharStr4()); }
void TalkRenderProcessor::tagGlyph8() { pushText((u8 *)Text_GetSpecialCharStr1()); }
void TalkRenderProcessor::tagGreeting() {
    u8 *base = cursor;
    u8 *r = Msg_FindTagPtr(base, 1, 7);
    if (r) {
        if (window->buildGreeting()) {
            skip(r - base);
            pushText(window->greeting.data());
        }
    }
}
void TalkRenderProcessor::tagGreetingEnd() {}
void TalkRenderProcessor::tagPlayerName() {
    window->buildPlayerName();
    pushText(window->playerName.data());
}
void TalkRenderProcessor::tagSpeakerName() {
    pushText(window->request->getSpeakerName()->data());
}
void TalkRenderProcessor::tagCatchphrase() {
    window->buildCatchphrase();
    pushText(window->catchphrase.data());
}
void TalkRenderProcessor::tagTownName() {
    window->buildTownName();
    pushText(window->townName.data());
}
void TalkRenderProcessor::tagYear() { pushText(window->getYearText()->data()); }
void TalkRenderProcessor::tagMonth() { pushText(window->getMonthText()->data()); }
void TalkRenderProcessor::tagDay() { pushText(window->getDayText()->data()); }
void TalkRenderProcessor::tagWeekday() { pushText(window->getWeekdayText()->data()); }
void TalkRenderProcessor::tagHour() { pushText(window->getHourText()->data()); }
void TalkRenderProcessor::tagMinute() { pushText(window->getMinuteText()->data()); }
void TalkRenderProcessor::tagNumber1714() { pushText(window->getNumber1714Text()->data()); }
void TalkRenderProcessor::tagFriendName() {
    window->buildFriendName();
    pushText(window->friendName.data());
}
void TalkRenderProcessor::tagEnemyName() {
    window->buildEnemyName();
    pushText(window->enemyName.data());
}
void TalkRenderProcessor::tagRandomVillagerName() {
    window->buildRandomVillagerName();
    pushText(window->randomVillagerName.data());
}
void TalkRenderProcessor::tagOtherResidentName() {
    window->buildOtherResidentName();
    pushText(window->otherResidentName.data());
}
void TalkRenderProcessor::tagSlot0() { expandSlot(0); }
void TalkRenderProcessor::tagSlot1() { expandSlot(1); }
void TalkRenderProcessor::tagSlot2() { expandSlot(2); }
void TalkRenderProcessor::tagSlot3() { expandSlot(3); }
void TalkRenderProcessor::tagSlot4() { expandSlot(4); }
void TalkRenderProcessor::tagSlot5() { expandSlot(5); }
void TalkRenderProcessor::tagSlot6() { expandSlot(6); }
void TalkRenderProcessor::tagSlot7() { expandSlot(7); }
void TalkRenderProcessor::tagSlot8() { expandSlot(8); }
void TalkRenderProcessor::tagSlot9() { expandSlot(9); }
void TalkRenderProcessor::tagSlot10() { expandSlot(10); }
void TalkRenderProcessor::tagNamedSlot0() { expandNamedSlot(0); }
void TalkRenderProcessor::tagNamedSlot1() { expandNamedSlot(1); }
void TalkRenderProcessor::tagNamedSlot2() { expandNamedSlot(2); }
void TalkRenderProcessor::tagNamedSlot3() { expandNamedSlot(3); }
void TalkRenderProcessor::tagImpression() {
    window->buildImpression();
    pushText(window->impression.data());
}
void TalkRenderProcessor::tagNickname() {
    window->buildNickname();
    pushText(window->nickname.data());
}
void TalkRenderProcessor::tagCompliment() {
    window->buildCompliment();
    pushText(window->compliment.data());
}
void TalkRenderProcessor::tagTrend() {
    window->buildTrend();
    pushText(window->trend.data());
}
void TalkRenderProcessor::tagArticleMode1() { articleMode = 1; }
void TalkRenderProcessor::tagArticleMode2() { articleMode = 2; }
void TalkRenderProcessor::tagSelectByPlayerGender() { selectByPlayerGender(); }
void TalkRenderProcessor::tagSlotForm0() { selectBySlotForm(&window->slots[0]); }
void TalkRenderProcessor::tagSlotForm1() { selectBySlotForm(&window->slots[1]); }
void TalkRenderProcessor::tagSlotForm2() { selectBySlotForm(&window->slots[2]); }
void TalkRenderProcessor::tagSlotForm3() { selectBySlotForm(&window->slots[3]); }
void TalkRenderProcessor::tagSlotForm4() { selectBySlotForm(&window->slots[4]); }
void TalkRenderProcessor::tagSlotForm5() { selectBySlotForm(&window->slots[5]); }
void TalkRenderProcessor::tagSlotForm6() { selectBySlotForm(&window->slots[6]); }
void TalkRenderProcessor::tagSlotForm7() { selectBySlotForm(&window->slots[7]); }
void TalkRenderProcessor::tagSlotForm8() { selectBySlotForm(&window->slots[8]); }
void TalkRenderProcessor::tagSlotForm9() { selectBySlotForm(&window->slots[9]); }
void TalkRenderProcessor::tagSlotForm10() { selectBySlotForm(&window->slots[10]); }
void TalkRenderProcessor::tagNamedSlotForm0() { selectBySlotForm(&window->namedSlots[0]); }
void TalkRenderProcessor::tagNamedSlotForm1() { selectBySlotForm(&window->namedSlots[1]); }
void TalkRenderProcessor::tagNamedSlotForm2() { selectBySlotForm(&window->namedSlots[2]); }
void TalkRenderProcessor::tagNamedSlotForm3() { selectBySlotForm(&window->namedSlots[3]); }
// ---- TalkRenderProcessor state handlers
void TalkRenderProcessor::tagAltText() {
    u32 x;
    char *y;
    char *z;
    tag.getAltTextArgs(&x, &y, &z);
    if (!Talk_IsAltTextEnabled()) {
        skip(x * 2);
        pushText((u8 *)z);
        altTextEnd = (u8 *)y;
    }
}
TalkTextBox::TalkTextBox(TalkWindowState *o) : length(0), numLines(0), centered(0), renderer(o) {
    clear();
    s32 i;
    for (i = 0; (u32)i < 3; i++) {
        labels[i] = 0;
    }
}
TalkTextBox::~TalkTextBox() {
    destroyLabels();
}
void TalkTextBox::clear() { clearWithColor(0); }
void TalkTextBox::clearWithColor(u32 v) {
    s32 i;
    length = 0;
    MI_CpuFill8(text, 0, 0x400);
    for (i = 0; (u32)i < 3; i++) {
        lineStarts[i] = 0;
        lineDirty[i] = 0;
        lineOffsets[i] = 0;
    }
    numLines = 0;
    setAllLineColors(v);
}
void TalkTextBox::setAllLineColors(u32 v) {
    s32 i;
    for (i = 0; (u32)i < 3; i++) {
        lineColors[i] = v;
    }
}
BOOL TalkTextBox::appendBytes(s8 *src, u32 n) {
    u32 pos = length;
    BOOL ok;
    if (0x400 - pos > n) {
        ok = TRUE;
    } else {
        ok = FALSE;
    }
    if (ok) {
        u32 i;
        for (i = 0; i < n; i++) {
            (&text[length])[i] = src[i];
        }
        length += n;
        markLineDirty();
    }
    return ok;
}
BOOL TalkTextBox::appendChar(s32 c0) {
    s8 c = (s8)c0;
    u32 pos = length;
    BOOL ok;
    if (0x400 - pos > 1) {
        ok = TRUE;
    } else {
        ok = FALSE;
    }
    if (ok) {
        length++;
        text[pos] = c;
        markLineDirty();
    }
    return ok;
}
void TalkTextBox::appendTag(MsgTag *q) {
    appendBytes((s8 *)q->raw, q->argLen + 5);
}
void TalkTextBox::beginLine(u8 *src) {
    if (numLines < 3) {
        lineOffsets[numLines] = renderer.measureLine(src, centered);
        u32 n = numLines;
        numLines = n + 1;
        lineStarts[n] = &text[length];
    }
}
void TalkTextBox::setRemainingLineColors(u32 v) {
    u32 i;
    for (i = numLines; i < 3; i++) {
        lineColors[i] = v;
    }
}
void TalkTextBox::setCentered(u8 v) { centered = v; }
void TalkTextBox::createLabels() {
    u32 i, a;
    for (i = 0, a = 0x11; i < 3; i++, a += 0x28) {
        MsgTextLabel *p = MsgTextLabel_CreateVram(a, 0x14, 2);
        if (p) {
            labels[i] = p;
            p->copyMode = 2;
            p->bgColor = 0xe;
            p->requestClear(0);
        }
    }
}
void TalkTextBox::destroyLabels() {
    u32 i;
    for (i = 0; i < 3; i++) {
        if (labels[i]) {
            MsgTextLabel_Destroy(labels[i]);
            labels[i] = 0;
        }
    }
}
// ---- TalkTextBox
void TalkTextBox::clearLabels() {
    u32 i;
    for (i = 0; i < 3; i++) {
        MsgTextLabel *p = labels[i];
        if (p) {
            p->requestClear(0);
            p->textStart = 0;
        }
    }
}

// ======== unk_0206ab74.cpp ========
void TalkTextBox::redrawLines() {
    u32 i;
    for (i = 0; i < numLines; i++) {
        MsgTextLabel *e = labels[i];
        if (e != 0 && lineDirty[i] != 0) {
            e->textStart = lineStarts[i];
            e->textEnd = 0;
            e->xOffset = lineOffsets[i];
            e->fgColor = Talk_ColorTagToTextColor(lineColors[i]);
            e->requestRedraw();
        }
    }
    if (numLines < 3) {
        MsgTextLabel *e = labels[numLines];
        if (e != 0) e->textEnd = text + length;
    }
}
void TalkTextBox::endText() {}
// ---------------------------------------------------------------------------
void TalkTextBox::markLineDirty() {
    s32 i = numLines;
    if (i != 0 && (u32)i <= 3) lineDirty[i - 1] = 1;
}
TalkCharStepper::TalkCharStepper(TalkWindowState *owner, TalkTextBox *buf)
    : window(owner), textBox(buf), mode(0), endPos(0), remaining(0) {
    stepAllowed = 0;
    printChars = 0;
}
TalkCharStepper::~TalkCharStepper() {}
void TalkCharStepper::startCount(u8 *p, s32 n, u8 flag) {
    if (n > 0) {
        reset();
        begin(p);
        endPos = 0;
        remaining = n;
        printChars = flag;
        stepAllowed = 0;
        mode = 1;
    }
}
void TalkCharStepper::startUntil(u8 *p, u8 *end, u8 flag) {
    if (end > p) {
        reset();
        begin(p);
        endPos = end;
        remaining = 0;
        printChars = flag;
        stepAllowed = 0;
        mode = 2;
    }
}
void TalkCharStepper::stepOnce() {
    s32 s = mode;
    if (s == 1) {
        stepAllowed = 1;
        run(FALSE);
    } else if (s == 2) {
        stepAllowed = 1;
        run(endPos);
    }
}
BOOL TalkCharStepper::isIdle() {
    if (mode == 0) return TRUE;
    return FALSE;
}
void TalkCharStepper::onBegin() {}
void TalkCharStepper::onEnd() {
    if (mode == 2) stop();
}
void TalkCharStepper::onChar(u32 c) {
    if (printChars != 0) textBox->appendChar(c);
    stepAllowed = 0;
    if (mode == 1) {
        remaining = remaining - 1;
        if (remaining <= 0) stop();
    } else if (mode == 2) {
        window->onCharPrinted(c);
    }
}
void TalkCharStepper::onTag(u8 *) {}
BOOL TalkCharStepper::canContinue() {
    return stepAllowed;
}
// ---------------------------------------------------------------------------
void TalkCharStepper::stop() {
    mode = 0;
    endPos = 0;
    remaining = 0;
    stepAllowed = 0;
    printChars = 0;
    reset();
}
TalkParser::TalkParser(TalkWindowState *owner, TalkTextBox *buf)
    : window(owner), textBox(buf), lineCount(0), printStatus(0), printMode(0), altTextStepper(owner, buf), mainTextStepper(owner, buf) {
    choiceDeferred = 0;
    capitalizeNext = 0;
    selectEnd = 0;
    articleMode = 0;
}
TalkParser::~TalkParser() {}
void TalkParser::stop() {
    lineCount = 0;
    textBox->endText();
    textBox->clearWithColor(printVars.textColor);
    textBox->beginLine(cursor);
    capitalizeNext = 0;
    articleMode = 0;
}
void TalkParser::setPrintMode(s32 v) {
    printMode = v;
}
void TalkParser::setFastForward() {
    if (printVars.fastForwardLock == 0) printVars.fastForward = 1;
}
void TalkParser::clearFastForward() {
    printVars.fastForward = 0;
}
void TalkParser::resetColor() {
    printVars.textColor = 0;
    window->voice.textColor = 0;
}
void TalkParser::onBegin() {
    printStatus = 1;
    lineCount = 0;
    printVars.fastForward = 0;
    printVars.holdStep = 0;
    printVars.instant = 0;
    printVars.fastForwardLock = 0;
    printVars.charCount = 0;
    printVars.stepPhase = 0;
    printVars.waitTimer = 0;
    textBox->clear();
    textBox->beginLine(cursor);
    window->resetVoice();
    capitalizeNext = 0;
    selectEnd = 0;
    articleMode = 0;
    window->voice.instantByTag = 0;
    window->voice.inWait = 0;
}
void TalkParser::onEnd() {
    printStatus = 3;
    textBox->endText();
}
void TalkParser::onChar(u32 c) {
    if (capitalizeNext != 0) {
        c = Text_ToUpper(c);
        capitalizeNext = 0;
    }
    BOOL nl = (c == 10) ? TRUE : FALSE;
    BOOL sp = (c == 0x20) ? TRUE : FALSE;
    textBox->appendChar(c);
    if (nl) {
        textBox->beginLine(cursor);
        lineCount++;
        if (lineCount >= 3) printStatus = 2;
    }
    window->onCharPrinted(c);
    window->voice.inWait = 0;
    if (!sp && !nl) printVars.charCount++;
    if (cursor == selectEnd) {
        selectEnd = 0;
        popText();
    }
}
void TalkParser::onTag(u8 *p) {
    tag.parse(p);
    dispatchTag();
}
BOOL TalkParser::canContinue() {
    BOOL r = TRUE;
    s32 s = printStatus;
    if (s == 2 || s == 5) {
        r = FALSE;
        printVars.charCount = r;
        printVars.stepPhase = r;
        printVars.waitTimer = r;
    } else if (s == 4) {
        r = FALSE;
        printVars.charCount = r;
        printVars.stepPhase = r;
    } else if (s == 1) {
        if (!stepSpeed()) r = FALSE;
    }
    return r;
}
// (an empty debug hook: the line and text of a message variable that could not be built)
void TalkParser::debugPrintMissing(s32 line, const char *what) {}
BOOL TalkParser::steppersIdle() {
    if (altTextStepper.isIdle() && mainTextStepper.isIdle()) return TRUE;
    return FALSE;
}
void TalkParser::stepSteppers() {
    altTextStepper.stepOnce();
    mainTextStepper.stepOnce();
}
BOOL TalkParser::stepSpeed() {
    BOOL r = TRUE;
    BOOL f = r;
    if (printMode != 1 && printVars.instant == 0) f = FALSE;
    if (printVars.holdStep != 0) {
        r = FALSE;
        printVars.holdStep = 0;
        goto end;
    }
    if (f) {
        printVars.charCount = 0;
        printVars.stepPhase = 0;
        printVars.waitTimer = 0;
        while (!steppersIdle()) {
            stepSteppers();
        }
        goto end;
    }
    BOOL t;
    if (printMode != 2 && printVars.fastForwardLock == 0 && printVars.fastForward != 0) {
        t = TRUE;
    } else {
        t = FALSE;
    }
    if (t) printVars.waitTimer = 0;
    s32 v;
    v = printVars.waitTimer;
    if (v > 0) {
        printVars.waitTimer = v - 0x1800;
        r = FALSE;
        goto end;
    }
    s32 n;
    if (printVars.stepPhase == 0) n = 1;
    else n = 2;
    if (t) n = 3;
    if (printVars.charCount >= n) {
        printVars.charCount = 0;
        printVars.stepPhase = printVars.stepPhase + 1;
        if (printVars.stepPhase >= 2) printVars.stepPhase = 0;
        r = FALSE;
        goto end;
    }
    while (!steppersIdle()) {
        stepSteppers();
        printVars.charCount = printVars.charCount + 1;
        if (printVars.charCount >= n) {
            printVars.charCount = 0;
            printVars.stepPhase = printVars.stepPhase + 1;
            if (printVars.stepPhase >= 2) printVars.stepPhase = 0;
            r = FALSE;
            goto end;
        }
    }
end:
    return r;
}
void TalkParser::expandArticle(MsgString *ent, BOOL flag) {
    MsgString *r = 0;
    if (articleMode == 0) {
        r = String_GetArticle(&ent->attr.attrA);
    } else if (articleMode == 1) {
        r = String_GetArticle(&ent->attr.attrB);
    }
    if (r != 0) {
        if (flag) {
            pushText(Msg_GetColorTag(printVars.textColor));
            pushText(r->data());
            pushText(Msg_GetColorTag(0));
        } else {
            pushText(r->data());
        }
    }
    articleMode = 0;
}
void TalkParser::expandSlot(s32 idx) {
    MsgString *ent = &window->slots[idx];
    pushText(ent->data());
    expandArticle(ent, 1);
}
void TalkParser::expandNamedSlot(s32 idx) {
    MsgString *ent = &window->namedSlots[idx];
    u32 color = window->namedSlotColors[idx];
    pushText(Msg_GetColorTag(printVars.textColor));
    pushText(ent->data());
    pushText(Msg_GetColorTag(color));
    expandArticle(ent, 0);
}
void TalkParser::selectByPlayerGender() {
    char *a, *b;
    tag.getStrings2(&a, &b);
    if (PlayerData_GetCurrent()->getPlayerId()->getGender() == 0) {
        if (a != 0) pushText((u8 *)a);
    } else {
        if (b != 0) {
            selectEnd = cursor;
            pushText((u8 *)b);
        }
    }
}
void TalkParser::selectBySlotForm(MsgString *p) {
    char *a, *b, *c;
    tag.getStrings3(&a, &b, &c);
    s32 k = p->attr.form;
    if (k == 0) {
        if (a != 0) pushText((u8 *)a);
    } else if (k == 1) {
        if (b != 0) pushText((u8 *)b);
    } else if (k == 2) {
        if (c != 0) {
            selectEnd = cursor;
            pushText((u8 *)c);
        }
    }
}
void TalkParser::jumpToMessage(u8 *p) {
    window->setNextMessage(p, (void *)(0));
    window->setAdvancePending();
}
// ---------------------------------------------------------------------------
void TalkParser::dispatchTag() {
    typedef void (TalkParser::*Fn)();
    static Fn tbl[12] = {
        &TalkParser::tagGroup00, &TalkParser::tagGroup01, &TalkParser::tagGroup02,
        &TalkParser::tagGroup03, &TalkParser::tagGroup04, &TalkParser::tagGroup05,
        &TalkParser::tagGroup06, &TalkParser::tagGroup07, &TalkParser::tagGroup08,
        &TalkParser::tagGroup09, &TalkParser::tagGroup0a, &TalkParser::tagGroup0b,
    };
    s32 id = tag.group;
    Fn fn = 0;
    if (id == 0xff) {
        fn = &TalkParser::tagGroupFf;
    } else if (id < 12) {
        fn = tbl[id];
    }
    (this->*fn)();
}

// ======== unk_0206a198.cpp ========
void TalkParser::tagGroup00()
{
    static void (TalkParser::*const tbl[11])() = {
        &TalkParser::tagGlyph0,
        &TalkParser::tagGlyph1,
        &TalkParser::tagNop,
        &TalkParser::tagNop,
        &TalkParser::tagNop,
        &TalkParser::tagNop,
        &TalkParser::tagGlyph6,
        &TalkParser::tagGlyph7,
        &TalkParser::tagGlyph8,
        &TalkParser::tagNop,
        &TalkParser::tagNop,
    };
    void (TalkParser::*f)() = tbl[tag.id];
    (this->*f)();
}
void TalkParser::tagGroup01()
{
    static void (TalkParser::*const tbl[17])() = {
        &TalkParser::tagWait,
        &TalkParser::tagPageBreak,
        &TalkParser::tagFastOn,
        &TalkParser::tagFastOff,
        &TalkParser::tagFastLockOn,
        &TalkParser::tagFastLockOff,
        &TalkParser::tagGreeting,
        &TalkParser::tagGreetingEnd,
        &TalkParser::tagShakeSmall,
        &TalkParser::tagShakeMedium,
        &TalkParser::tagShakeLarge,
        &TalkParser::tagSignal0,
        &TalkParser::tagSignal1,
        &TalkParser::tagSignal2,
        &TalkParser::tagSignal3,
        &TalkParser::tagSignal4,
        &TalkParser::tagAutoAdvance,
    };
    void (TalkParser::*f)() = tbl[tag.id];
    (this->*f)();
}
void TalkParser::tagGroup02()
{
    static void (TalkParser::*const tbl[8])() = {
        &TalkParser::tagChoice2,
        &TalkParser::tagChoice3,
        &TalkParser::tagChoice4,
        &TalkParser::tagChoice5,
        &TalkParser::tagChoice2B,
        &TalkParser::tagChoice3B,
        &TalkParser::tagChoice4B,
        &TalkParser::tagChoice5B,
    };
    void (TalkParser::*f)() = tbl[tag.id];
    if (choiceDeferred) {
        choiceDeferred = 0;
        (this->*f)();
    } else {
        unreadTag(tag.raw);
        choiceDeferred = 1;
        pushText((u8 *)data_020cba1c);
    }
}
void TalkParser::tagGroup03()
{
    static void (TalkParser::*const tbl[5])() = {
        &TalkParser::tagAction0,
        &TalkParser::tagAction1,
        &TalkParser::tagAction2,
        &TalkParser::tagAction3,
        &TalkParser::tagAction4,
    };
    void (TalkParser::*f)() = tbl[tag.id];
    (this->*f)();
}
void TalkParser::tagGroup04()
{
    static void (TalkParser::*const tbl[35])() = {
        &TalkParser::tagPlayerName,
        &TalkParser::tagSpeakerName,
        &TalkParser::tagCatchphrase,
        &TalkParser::tagTownName,
        &TalkParser::tagYear,
        &TalkParser::tagMonth,
        &TalkParser::tagDay,
        &TalkParser::tagWeekday,
        &TalkParser::tagHour,
        &TalkParser::tagMinute,
        &TalkParser::tagNumber1714,
        &TalkParser::tagFriendName,
        &TalkParser::tagEnemyName,
        &TalkParser::tagRandomVillagerName,
        &TalkParser::tagOtherResidentName,
        &TalkParser::tagSlot0,
        &TalkParser::tagSlot1,
        &TalkParser::tagSlot2,
        &TalkParser::tagSlot3,
        &TalkParser::tagSlot4,
        &TalkParser::tagSlot5,
        &TalkParser::tagSlot6,
        &TalkParser::tagSlot7,
        &TalkParser::tagSlot8,
        &TalkParser::tagSlot9,
        &TalkParser::tagSlot10,
        &TalkParser::tagNamedSlot0,
        &TalkParser::tagNamedSlot1,
        &TalkParser::tagNamedSlot2,
        &TalkParser::tagNamedSlot3,
        &TalkParser::tagImpression,
        &TalkParser::tagNickname,
        &TalkParser::tagNop0420,
        &TalkParser::tagCompliment,
        &TalkParser::tagTrend,
    };
    void (TalkParser::*f)() = tbl[tag.id];
    (this->*f)();
}
void TalkParser::tagGroup05()
{
    static void (TalkParser::*const tbl[8])() = {
        &TalkParser::tagNop,
        &TalkParser::tagNop,
        &TalkParser::tagNop,
        &TalkParser::tagNop,
        &TalkParser::tagNop,
        &TalkParser::tagChoiceSlots,
        &TalkParser::tagNop,
        &TalkParser::tagNop,
    };
    void (TalkParser::*f)() = tbl[tag.id];
    (this->*f)();
}
void TalkParser::tagGroup06()
{
    static void (TalkParser::*const tbl[14])() = {
        &TalkParser::tagBranchRandom2,
        &TalkParser::tagBranchRandom3,
        &TalkParser::tagBranchFriendship,
        &TalkParser::tagBranchPlayerGender,
        &TalkParser::tagBranchHouseUnk,
        &TalkParser::tagBranchInsectCount,
        &TalkParser::tagBranchFishCount,
        &TalkParser::tagBranchVillagerCount,
        &TalkParser::tagBranchUnk8,
        &TalkParser::tagBranchWeather,
        &TalkParser::tagBranchResidentCount,
        &TalkParser::tagBranchWeekday,
        &TalkParser::tagBranchVisitor,
        &TalkParser::tagBranchSpecies,
    };
    void (TalkParser::*f)() = tbl[tag.id];
    (this->*f)();
}
void TalkParser::tagGroup07()
{
    u32 v = tag.id;
    TalkMsgRequest *p = window->request;
    MsgTag_DtorStub();
    p->onEventTag(v);
}
void TalkParser::tagGroup08()
{
    static void (TalkParser::*const tbl[3])() = {
        &TalkParser::tagNop0800,
        &TalkParser::tagNop0801,
        &TalkParser::tagNop0802,
    };
    void (TalkParser::*f)() = tbl[tag.id];
    (this->*f)();
}
void TalkParser::tagGroup09()
{
    static void (TalkParser::*const tbl[10])() = {
        &TalkParser::tagSignal09_0,
        &TalkParser::tagSignal09_1,
        &TalkParser::tagSignal09_2,
        &TalkParser::tagSignal09_3,
        &TalkParser::tagSignal09_4,
        &TalkParser::tagSignal09_5,
        &TalkParser::tagSignal09_6,
        &TalkParser::tagSignal09_7,
        &TalkParser::tagSignal09_8,
        &TalkParser::tagSignal09_9,
    };
    void (TalkParser::*f)() = tbl[tag.id];
    (this->*f)();
}
void TalkParser::tagGroup0a()
{
    static void (TalkParser::*const tbl[1])() = {
        &TalkParser::tagNop0a00,
    };
    void (TalkParser::*f)() = tbl[tag.id];
    (this->*f)();
}

// ======== unk_02069834.cpp ========
typedef void (TalkParser::*Unk_02069fa4_Fn)();
void TalkParser::tagGroup0b() {
    static Unk_02069fa4_Fn tbl[20] = {
        &TalkParser::tagNop,
        &TalkParser::tagArticleMode1,
        &TalkParser::tagArticleMode2,
        &TalkParser::tagCapitalizeNext,
        &TalkParser::tagSelectByPlayerGender,
        &TalkParser::tagSlotForm0,
        &TalkParser::tagSlotForm1,
        &TalkParser::tagSlotForm2,
        &TalkParser::tagSlotForm3,
        &TalkParser::tagSlotForm4,
        &TalkParser::tagSlotForm5,
        &TalkParser::tagSlotForm6,
        &TalkParser::tagSlotForm7,
        &TalkParser::tagSlotForm8,
        &TalkParser::tagSlotForm9,
        &TalkParser::tagSlotForm10,
        &TalkParser::tagNamedSlotForm0,
        &TalkParser::tagNamedSlotForm1,
        &TalkParser::tagNamedSlotForm2,
        &TalkParser::tagNamedSlotForm3};
    Unk_02069fa4_Fn f = tbl[tag.id];
    (this->*f)();
}
void TalkParser::tagGroupFf() {
    static Unk_02069fa4_Fn tbl[3] = {&TalkParser::tagColor, 0, &TalkParser::tagAltText};
    Unk_02069fa4_Fn f = tbl[tag.id];
    (this->*f)();
}
void TalkParser::tagNop() {}
void TalkParser::tagGlyph0() {
    pushText((u8 *)Text_GetSpecialCharStr6()); }
void TalkParser::tagGlyph1() {
    pushText((u8 *)Text_GetSpecialCharStr7()); }
void TalkParser::tagGlyph6() {
    pushText((u8 *)Text_GetSpecialCharStr5()); }
void TalkParser::tagGlyph7() {
    pushText((u8 *)Text_GetSpecialCharStr4()); }
void TalkParser::tagGlyph8() {
    pushText((u8 *)Text_GetSpecialCharStr1()); }
void TalkParser::tagWait() {
    u16 v[2];
    tag.getArgU16(v);
    printVars.waitTimer = v[0] << 12;
    window->voice.inWait = 1;
}
void TalkParser::tagPageBreak() {
    MsgTag_DtorStub();
    printStatus = 4;
}
void TalkParser::tagFastOn() {
    MsgTag_DtorStub();
    printVars.instant = 1;
    window->voice.instantByTag = 1;
}
void TalkParser::tagFastOff() {
    MsgTag_DtorStub();
    printVars.instant = 0;
    window->voice.instantByTag = 0;
}
void TalkParser::tagFastLockOn() {
    MsgTag_DtorStub();
    printVars.fastForwardLock = 1;
}
void TalkParser::tagFastLockOff() {
    MsgTag_DtorStub();
    printVars.fastForwardLock = 0;
}
void TalkParser::tagGreeting() {
    u8 *base = cursor;
    u8 *p = Msg_FindTagPtr(base, 1, 7);
    if (p != 0) {
        if (window->buildGreeting() != 0) {
            skip(p - base);
            pushText(window->greeting.data());
        }
    }
}
void TalkParser::tagGreetingEnd() {}
void TalkParser::tagShakeSmall() {
    Snd_PlaySe(6);
    window->startShake(0x6000, 0x666, 0x4cc, 0x1199);
}
void TalkParser::tagShakeMedium() {
    Snd_PlaySe(7);
    window->startShake(0xa000, 0x800, 0x800, 0x1333);
}
void TalkParser::tagShakeLarge() {
    Snd_PlaySe(8);
    window->startShake(0x11000, 0xa66, 0xccc, 0x1000);
}
void TalkParser::tagSignal0() {
    TalkMsgRequest *o = window->request;
    MsgTag_DtorStub();
    o->onSignalTag(0);
}
void TalkParser::tagSignal1() {
    TalkMsgRequest *o = window->request;
    MsgTag_DtorStub();
    o->onSignalTag(1);
}
void TalkParser::tagSignal2() {
    TalkMsgRequest *o = window->request;
    MsgTag_DtorStub();
    o->onSignalTag(2);
}
void TalkParser::tagSignal3() {
    TalkMsgRequest *o = window->request;
    MsgTag_DtorStub();
    o->onSignalTag(3);
}
void TalkParser::tagSignal4() {
    TalkMsgRequest *o = window->request;
    MsgTag_DtorStub();
    o->onSignalTag(4);
}
void TalkParser::tagAutoAdvance() {
    u8 v[4];
    tag.getArgs1(v);
    window->startAutoAdvance(v[0]);
}
void TalkParser::tagChoice2() {
    ChoiceList *l = window->getChoiceList();
    ChoiceEntry *a = l->getEntry(0);
    ChoiceEntry *b = l->getEntry(1);
    l->clear();
    u8 *a1 = a->getValuePtr();
    MsgString *a2 = a->getText();
    u8 *b1 = b->getValuePtr();
    MsgString *b2 = b->getText();
    skip(tag.readArgStrings2(a1, a2, b1, b2));
    l->setCount(2);
    window->choiceMenu.setListChoices(l);
    window->choiceMenu.open();
    printStatus = 5;
}
void TalkParser::tagChoice3() {
    ChoiceList *l = window->getChoiceList();
    ChoiceEntry *a = l->getEntry(0);
    ChoiceEntry *b = l->getEntry(1);
    ChoiceEntry *c = l->getEntry(2);
    l->clear();
    u8 *a1 = a->getValuePtr();
    MsgString *a2 = a->getText();
    u8 *b1 = b->getValuePtr();
    MsgString *b2 = b->getText();
    u8 *c1 = c->getValuePtr();
    MsgString *c2 = c->getText();
    skip(tag.readArgStrings3(a1, a2, b1, b2, c1, c2));
    l->setCount(3);
    window->choiceMenu.setListChoices(l);
    window->choiceMenu.open();
    printStatus = 5;
}
void TalkParser::tagChoice4() {
    ChoiceList *l = window->getChoiceList();
    ChoiceEntry *a = l->getEntry(0);
    ChoiceEntry *b = l->getEntry(1);
    ChoiceEntry *c = l->getEntry(2);
    ChoiceEntry *d = l->getEntry(3);
    l->clear();
    u8 *a1 = a->getValuePtr();
    MsgString *a2 = a->getText();
    u8 *b1 = b->getValuePtr();
    MsgString *b2 = b->getText();
    u8 *c1 = c->getValuePtr();
    MsgString *c2 = c->getText();
    u8 *d1 = d->getValuePtr();
    MsgString *d2 = d->getText();
    skip(tag.readArgStrings4(a1, a2, b1, b2, c1, c2, d1, d2));
    l->setCount(4);
    window->choiceMenu.setListChoices(l);
    window->choiceMenu.open();
    printStatus = 5;
}
void TalkParser::tagChoice5() {
    ChoiceList *l = window->getChoiceList();
    ChoiceEntry *a = l->getEntry(0);
    ChoiceEntry *b = l->getEntry(1);
    ChoiceEntry *c = l->getEntry(2);
    ChoiceEntry *d = l->getEntry(3);
    ChoiceEntry *e = l->getEntry(4);
    l->clear();
    u8 *a1 = a->getValuePtr();
    MsgString *a2 = a->getText();
    u8 *b1 = b->getValuePtr();
    MsgString *b2 = b->getText();
    u8 *c1 = c->getValuePtr();
    MsgString *c2 = c->getText();
    u8 *d1 = d->getValuePtr();
    MsgString *d2 = d->getText();
    u8 *e1 = e->getValuePtr();
    MsgString *e2 = e->getText();
    skip(tag.readArgStrings5(a1, a2, b1, b2, c1, c2, d1, d2, e1, e2));
    l->setCount(5);
    window->choiceMenu.setListChoices(l);
    window->choiceMenu.open();
    printStatus = 5;
}
void TalkParser::tagChoice2B() {
    tagChoice2();
    window->getChoiceList()->setCancelToLast();
}
void TalkParser::tagChoice3B() {
    tagChoice3();
    window->getChoiceList()->setCancelToLast();
}
void TalkParser::tagChoice4B() {
    tagChoice4();
    window->getChoiceList()->setCancelToLast();
}
void TalkParser::tagChoice5B() {
    tagChoice5();
    window->getChoiceList()->setCancelToLast();
}
void TalkParser::tagAction0() {
    TalkMsgRequest *o = window->request;
    MsgTag_DtorStub();
    o->onActionTag0();
}
void TalkParser::tagAction1() {
    TalkMsgRequest *o = window->request;
    u8 v[4];
    tag.getArgs1(v);
    o->onActionTag1(v[0]);
}
void TalkParser::tagAction2() {
    TalkMsgRequest *o = window->request;
    u8 v[4];
    tag.getArgs1(v);
    o->onActionTag2(v[0]);
}
void TalkParser::tagAction3() {
    TalkMsgRequest *o = window->request;
    u8 v[4];
    tag.getArgs1(v);
    o->onActionTag3(v[0]);
}
void TalkParser::tagAction4() {
    TalkMsgRequest *o = window->request;
    u8 v[4];
    tag.getArgs1(v);
    o->onActionTag4(v[0]);
}
void TalkParser::tagPlayerName() {
    window->buildPlayerName();
    pushText(Msg_GetColorTag(printVars.textColor));
    pushText(window->playerName.data());
    pushText(Msg_GetColorTag(5));
}
void TalkParser::tagSpeakerName() {
    pushText(Msg_GetColorTag(printVars.textColor));
    pushText(window->request->getSpeakerName()->data());
    pushText(Msg_GetColorTag(6));
}

// ======== unk_02068f10.cpp ========
// (the texts of TalkParser::debugPrintMissing)
#define data_020dde2c ((char *)"\202i\220e\226\247\223x")
#define data_020dde38 ((char *)"\203u\201[\203\200")
#define data_020dde40 ((char *)"\203z\203\201\203R\203g\203o")
#define data_020dde4c ((char *)"\203j\203b\203N\203l\201[\203\200")
#define data_020dde5c ((char *)"\210\363\217\333")
#define data_020dde64 ((char *)"\221\274\202\314\217Z\220l\202o\226\274")
#define data_020dde74 ((char *)"\203\211\203\223\203_\203\200\202m\202o\202b")
#define data_020dde84 ((char *)"\222\207\210\253\202\265\202m\202o\202b")
#define data_020dde94 ((char *)"\222\207\227\307\202\265\202m\202o\202b")
#define data_020ddea4 ((char *)"\214\373\202\256\202\271\202\360\216g\227p\201H")
void TalkParser::tagCatchphrase() {
    if (!window->buildCatchphrase()) debugPrintMissing(0xb58, data_020ddea4);
    pushText(window->catchphrase.data());
}
void TalkParser::tagTownName() {
    window->buildTownName();
    pushText(Msg_GetColorTag(printVars.textColor));
    pushText(window->townName.data());
    pushText(Msg_GetColorTag(8));
}
void TalkParser::tagYear() { pushText(window->getYearText()->data()); }
void TalkParser::tagMonth() { pushText(window->getMonthText()->data()); }
void TalkParser::tagDay() { pushText(window->getDayText()->data()); }
void TalkParser::tagWeekday() { pushText(window->getWeekdayText()->data()); }
void TalkParser::tagHour() { pushText(window->getHourText()->data()); }
void TalkParser::tagMinute() { pushText(window->getMinuteText()->data()); }
void TalkParser::tagNumber1714() { pushText(window->getNumber1714Text()->data()); }
void TalkParser::tagFriendName() {
    if (!window->buildFriendName()) debugPrintMissing(0xbbd, data_020dde94);
    pushText(Msg_GetColorTag(printVars.textColor));
    pushText(window->friendName.data());
    pushText(Msg_GetColorTag(6));
}
void TalkParser::tagEnemyName() {
    if (!window->buildEnemyName()) debugPrintMissing(0xbcd, data_020dde84);
    pushText(Msg_GetColorTag(printVars.textColor));
    pushText(window->enemyName.data());
    pushText(Msg_GetColorTag(6));
}
void TalkParser::tagRandomVillagerName() {
    if (!window->buildRandomVillagerName()) debugPrintMissing(0xbdd, data_020dde74);
    pushText(Msg_GetColorTag(printVars.textColor));
    pushText(window->randomVillagerName.data());
    pushText(Msg_GetColorTag(6));
}
void TalkParser::tagOtherResidentName() {
    if (!window->buildOtherResidentName()) debugPrintMissing(0xbee, data_020dde64);
    pushText(Msg_GetColorTag(printVars.textColor));
    pushText(window->otherResidentName.data());
    s32 k = 5;
    if (window->otherResidentIsVillager != 0) k = 6;
    pushText(Msg_GetColorTag(k));
}
void TalkParser::tagSlot0() { expandSlot(0); }
void TalkParser::tagSlot1() { expandSlot(1); }
void TalkParser::tagSlot2() { expandSlot(2); }
void TalkParser::tagSlot3() { expandSlot(3); }
void TalkParser::tagSlot4() { expandSlot(4); }
void TalkParser::tagSlot5() { expandSlot(5); }
void TalkParser::tagSlot6() { expandSlot(6); }
void TalkParser::tagSlot7() { expandSlot(7); }
void TalkParser::tagSlot8() { expandSlot(8); }
void TalkParser::tagSlot9() { expandSlot(9); }
void TalkParser::tagSlot10() { expandSlot(10); }
void TalkParser::tagNamedSlot0() { expandNamedSlot(0); }
void TalkParser::tagNamedSlot1() { expandNamedSlot(1); }
void TalkParser::tagNamedSlot2() { expandNamedSlot(2); }
void TalkParser::tagNamedSlot3() { expandNamedSlot(3); }
void TalkParser::tagImpression() {
    if (!window->buildImpression()) debugPrintMissing(0xc7d, data_020dde5c);
    pushText(window->impression.data());
}
void TalkParser::tagNickname() {
    if (!window->buildNickname()) debugPrintMissing(0xc8b, data_020dde4c);
    pushText(Msg_GetColorTag(printVars.textColor));
    pushText(window->nickname.data());
    pushText(Msg_GetColorTag(5));
}
void TalkParser::tagNop0420() {}
void TalkParser::tagCompliment() {
    if (!window->buildCompliment()) debugPrintMissing(0xca4, data_020dde40);
    pushText(Msg_GetColorTag(printVars.textColor));
    pushText(window->compliment.data());
    pushText(Msg_GetColorTag(2));
}
void TalkParser::tagTrend() {
    if (!window->buildTrend()) debugPrintMissing(0xcb5, data_020dde38);
    pushText(window->trend.data());
}
void TalkParser::tagChoiceSlots() {
    if (choiceDeferred != 0) {
        choiceDeferred = 0;
        ChoiceList *o = window->getChoiceList();
        ChoiceEntry *a = o->getEntry(0);
        ChoiceEntry *b = o->getEntry(1);
        ChoiceEntry *c = o->getEntry(2);
        ChoiceEntry *d = o->getEntry(3);
        o->clear();
        u8 *a0 = a->getWeightPtr();
        u8 *a1 = a->getValuePtr();
        u8 *b0 = b->getWeightPtr();
        u8 *b1 = b->getValuePtr();
        u8 *c0 = c->getWeightPtr();
        u8 *c1 = c->getValuePtr();
        u8 *d0 = d->getWeightPtr();
        u8 *d1 = d->getValuePtr();
        ChoiceString *e0 = o->getFirstText();
        ChoiceString *e1 = o->getLastText();
        skip(tag.readArgBytes8Strings2(a0, a1, b0, b1, c0, c1, d0, d1, e0, e1));
        o->setCount(4);
        window->choiceMenu.setSliderChoices(o);
        window->choiceMenu.open();
        printStatus = 5;
    } else {
        unreadTag(tag.raw);
        choiceDeferred = 1;
        pushText((u8 *)data_020cba1c);
    }
}
void TalkParser::tagBranchRandom2() {
    u8 r[4];
    window->request->onConditionTag(0, 2);
    tag.getArgs2(&r[1], &r[2]);
    u8 *q = &r[1];
    r[0] = q[Random_GlobalBelow(2)];
    jumpToMessage(r);
}
void TalkParser::tagBranchRandom3() {
    u8 r[4];
    window->request->onConditionTag(1, 3);
    tag.getArgs3(&r[1], &r[2], &r[3]);
    u8 *q = &r[1];
    r[0] = q[Random_GlobalBelow(3)];
    jumpToMessage(r);
}
void TalkParser::tagBranchFriendship() {
    u8 r[4];
    TalkMsgRequest *o = window->request;
    o->onConditionTag(2, 2);
    tag.getArgs3(&r[0], &r[2], &r[3]);
    s32 k = 0;
    void *p = o->getSpeakerData();
    PlayerData *pd = PlayerData_GetCurrent();
    if (p) {
        PlayerId *v = pd->getPlayerId();
        VillagerMemory *m;
        if ((m = Villager_FindMemory(p, v)) != 0) {
            if (m->getFriendship() < r[0]) k = 1;
        }
    } else {
        debugPrintMissing(0xd64, data_020dde2c);
    }
    u8 *q = &r[2];
    r[1] = q[k];
    jumpToMessage(&r[1]);
}
void TalkParser::tagBranchPlayerGender() {
    u8 r[4];
    window->request->onConditionTag(3, 2);
    tag.getArgs2(&r[1], &r[2]);
    s32 i;
    if (PlayerData_GetCurrent()->getPlayerId()->getGender() != 0) i = 1;
    else i = 0;
    u8 *q = &r[1];
    r[0] = q[i];
    jumpToMessage(r);
}
void TalkParser::tagBranchHouseUnk() {
    u8 r[5];
    window->request->onConditionTag(4, 4);
    tag.getArgs4(&r[1], &r[2], &r[3], &r[4]);
    s32 t = gSaveHouse.getLevel();
    s32 i;
    if (t == 0) i = 0;
    else if (t >= 1 && t <= 2) i = 1;
    else if (t == 3) i = 2;
    else i = 3;
    u8 *q = &r[1];
    r[0] = q[i];
    jumpToMessage(r);
}
void TalkParser::tagBranchInsectCount() {
    u8 r[4];
    window->request->onConditionTag(5, 3);
    tag.getArgs3(&r[1], &r[2], &r[3]);
    u32 t = Catalog_CountInsects();
    s32 i;
    if (t == 0x38) i = 2;
    else if (t >= 0x21) i = 1;
    else i = 0;
    u8 *q = &r[1];
    r[0] = q[i];
    jumpToMessage(r);
}
void TalkParser::tagBranchFishCount() {
    u8 r[4];
    window->request->onConditionTag(6, 3);
    tag.getArgs3(&r[1], &r[2], &r[3]);
    u32 t = Catalog_CountFish();
    s32 i;
    if (t == 0x38) i = 2;
    else if (t >= 0x21) i = 1;
    else i = 0;
    u8 *q = &r[1];
    r[0] = q[i];
    jumpToMessage(r);
}
void TalkParser::tagBranchVillagerCount() {
    u8 r[5];
    window->request->onConditionTag(7, 4);
    tag.getArgs4(&r[1], &r[2], &r[3], &r[4]);
    s32 n = SaveVillagers_Count(gSaveData.villagers);
    s32 i;
    if (n <= data_020cbf90) i = 0;
    else if (n == data_020cbf90 + 1) i = 1;
    else if (n >= 8) i = 3;
    else i = 2;
    u8 *q = &r[1];
    r[0] = q[i];
    jumpToMessage(r);
}

// ======== unk_02068e9c.cpp ========
void TalkParser::tagBranchUnk8() {
    u8 r[5];
    window->request->onConditionTag(8, 4);
    tag.getArgs4(&r[1], &r[2], &r[3], &r[4]);
    s32 v = window->timeOfDay;
    s32 i = 0;
    if (v == 0) {
    } else if (v == 1) i = 1;
    else if (v == 2) i = 2;
    else if (v == 3) i = 3;
    u8 *q = &r[1];
    r[0] = q[i];
    jumpToMessage(r);
}

// ======== unk_020685c4.cpp ========
#define data_020dddbc "/a_mes/a_mes_bg_ncg.bin"
#define data_020dddd4 "/a_mes/a_mes_bg_ncl.bin"
#define data_020dddec "/a_mes/a_mes0a_bg_nsc.bin"
#define data_020dde08 "/a_mes/a_mes0b_bg_nsc.bin"
#define data_020dde24 ((char *)"\202i\216\355\221\260")
void TalkParser::tagBranchWeather() {
    u8 b[4];
    s32 i, r;
    window->request->onConditionTag(9, 3);
    tag.getArgs3(&b[1], &b[2], &b[3]);
    r = Weather_GetFallingPrecip();
    i = 0;
    if (r == 1) i = 1;
    else if (r == 2) i = 2;
    b[0] = (&b[1])[i];
    jumpToMessage(b);
}
void TalkParser::tagBranchResidentCount() {
    u8 b[8];
    s32 i, t;
    window->request->onConditionTag(0xa, 4);
    tag.getArgs4(&b[1], &b[2], &b[3], &b[4]);
    t = PlayerDataArray_CountUsed(gSaveData.players);
    i = t - 1;
    if (i < 0) i = 0;
    else if (i > 3) i = 3;
    b[0] = (&b[1])[i];
    jumpToMessage(b);
}
void TalkParser::tagBranchWeekday() {
    u8 b[8];
    window->request->onConditionTag(0xb, 7);
    tag.getArgBytes(&b[1], 7);
    u32 v = window->weekday;
    s32 i;
    if (v == 0) i = 6;
    else i = v - 1;
    b[0] = (&b[1])[i];
    jumpToMessage(b);
}
void TalkParser::tagBranchVisitor() {
    u8 b[8];
    window->request->onConditionTag(0xc, 2);
    tag.getArgs2(&b[1], &b[2]);
    s32 i;
    s32 r = PlayerData_IsResidentIndex(PlayerData_GetCurrentIndex());
    if (r == 1) i = 0;
    else i = 1;
    b[0] = (&b[1])[i];
    jumpToMessage(b);
}
void TalkParser::tagBranchSpecies() {
    u8 b[4];
    TalkMsgRequest *m = window->request;
    BOOL i;
    void *res;
    m->onConditionTag(0xd, 2);
    tag.getArgs3(&b[0], &b[2], &b[3]);
    b[0]--;
    res = m->getSpeakerData();
    i = 0;
    if (res != 0) {
        s32 c = Villager_GetAnimalKind((VillagerData *)res);
        if (c != b[0]) i = 1;
    } else {
        debugPrintMissing(0xeeb, data_020dde24);
    }
    u8 *q = &b[2];
    b[1] = q[i];
    jumpToMessage(&b[1]);
}
void TalkParser::tagNop0800() {}
void TalkParser::tagNop0801() {}
void TalkParser::tagNop0802() {}
void TalkParser::tagSignal09_0() {
    TalkMsgRequest *m = window->request;
    MsgTag_DtorStub();
    m->onTag09_0();
}
void TalkParser::tagSignal09_1() {
    TalkMsgRequest *m = window->request;
    MsgTag_DtorStub();
    m->onTag09_1();
}
void TalkParser::tagSignal09_2() {
    TalkMsgRequest *m = window->request;
    MsgTag_DtorStub();
    m->onTag09_2();
}
void TalkParser::tagSignal09_3() {
    TalkMsgRequest *m = window->request;
    MsgTag_DtorStub();
    m->onTag09_3();
}
void TalkParser::tagSignal09_4() {
    TalkMsgRequest *m = window->request;
    MsgTag_DtorStub();
    m->onTag09_4();
}
void TalkParser::tagSignal09_5() {
    TalkMsgRequest *m = window->request;
    MsgTag_DtorStub();
    m->onTag09_5();
}
void TalkParser::tagSignal09_6() {
    TalkMsgRequest *m = window->request;
    MsgTag_DtorStub();
    m->onTag09_6();
}
void TalkParser::tagSignal09_7() {
    TalkMsgRequest *m = window->request;
    MsgTag_DtorStub();
    m->onTag09_7();
}
void TalkParser::tagSignal09_8() {
    TalkMsgRequest *m = window->request;
    MsgTag_DtorStub();
    m->onTag09_8();
}
void TalkParser::tagSignal09_9() {
    TalkMsgRequest *m = window->request;
    MsgTag_DtorStub();
    m->onTag09_9();
}
void TalkParser::tagNop0a00() {}
void TalkParser::tagArticleMode1() {
    articleMode = 1;
}
void TalkParser::tagArticleMode2() {
    articleMode = 2;
}
void TalkParser::tagCapitalizeNext() {
    capitalizeNext = 1;
}
void TalkParser::tagSelectByPlayerGender() {
    selectByPlayerGender();
}
void TalkParser::tagSlotForm0() { selectBySlotForm(&window->slots[0]); }
void TalkParser::tagSlotForm1() { selectBySlotForm(&window->slots[1]); }
void TalkParser::tagSlotForm2() { selectBySlotForm(&window->slots[2]); }
void TalkParser::tagSlotForm3() { selectBySlotForm(&window->slots[3]); }
void TalkParser::tagSlotForm4() { selectBySlotForm(&window->slots[4]); }
void TalkParser::tagSlotForm5() { selectBySlotForm(&window->slots[5]); }
void TalkParser::tagSlotForm6() { selectBySlotForm(&window->slots[6]); }
void TalkParser::tagSlotForm7() { selectBySlotForm(&window->slots[7]); }
void TalkParser::tagSlotForm8() { selectBySlotForm(&window->slots[8]); }
void TalkParser::tagSlotForm9() { selectBySlotForm(&window->slots[9]); }
void TalkParser::tagSlotForm10() { selectBySlotForm(&window->slots[10]); }
void TalkParser::tagNamedSlotForm0() { selectBySlotForm(&window->namedSlots[0]); }
void TalkParser::tagNamedSlotForm1() { selectBySlotForm(&window->namedSlots[1]); }
void TalkParser::tagNamedSlotForm2() { selectBySlotForm(&window->namedSlots[2]); }
void TalkParser::tagNamedSlotForm3() { selectBySlotForm(&window->namedSlots[3]); }
void TalkParser::tagColor() {
    printVars.textColor = tag.getArgU8();
    textBox->appendTag(&tag);
    textBox->setRemainingLineColors(printVars.textColor);
    window->voice.textColor = printVars.textColor;
}
void TalkParser::tagAltText() {
    u32 a;
    char *b;
    char *c;
    tag.getAltTextArgs(&a, &b, &c);
    BOOL r = Talk_IsAltTextEnabled();
    altTextStepper.startCount((u8 *)b, a, r);
    mainTextStepper.startUntil((u8 *)c, (u8 *)b, r == 0);
    skip(a * 2);
}
TalkTagScanner::TalkTagScanner(TalkWindowState *owner) {
    window = owner;
    unk_28 = 0;
}
TalkTagScanner::~TalkTagScanner() {}
void TalkTagScanner::scan(u8 *p) {
    resetScan();
    unk_28 = 0;
    begin(p);
    run(FALSE);
}
void TalkTagScanner::onTag(u8 *p) {
    tag.parse(p);
    if (unk_28 == 0) {
        if (tag.group == 10 && tag.id == 0) {
            reportTag();
        }
    }
}
void TalkTagScanner::resetScan() {
    unk_28 = 0;
    new (&tag) MsgTag();
}
// ---- TalkTagScanner
void TalkTagScanner::reportTag() {
    u8 b;
    tag.getArgs1(&b);
    window->request->onScannedTag(b);
}
TalkFrame::TalkFrame()
    : screenData(NULL), paletteData(NULL), charData(NULL), arrow(1), scrollX(0), scrollY(0), busyIconTimer(0) {
    arrow.setAltStyle();
    busyIcon.init();
}
TalkFrame::~TalkFrame() {
    busyIcon.exit();
    freeBuffers();
}
void TalkFrame::update() {
    updateArrow();
    updateBusyIcon();
}
void TalkFrame::draw() {
    drawArrow();
    drawBusyIcon();
}
BOOL TalkFrame::loadScreen(FSFile *file, BOOL alt) {
    BOOL a = FS_OpenFile(file, alt ? data_020dddec : data_020dde08);
    BOOL ok;
    screenData = (u16 *)Mem_AllocTail(0x800);
    if (screenData != NULL) {
        s32 n = FS_ReadFile(file, screenData, 0x800);
        ok = FALSE;
        if (n != ~ok) ok = TRUE;
    } else {
        ok = FALSE;
    }
    BOOL r = FS_CloseFile(file);
    if (a && ok && r && screenData) return TRUE;
    return FALSE;
}
BOOL TalkFrame::loadPalette(FSFile *file) {
    BOOL a = FS_OpenFile(file, data_020dddd4);
    BOOL ok;
    paletteData = Mem_AllocTail(0x180);
    if (paletteData != NULL) {
        s32 n = FS_ReadFile(file, paletteData, 0x180);
        ok = FALSE;
        if (n != ~ok) ok = TRUE;
    } else {
        ok = FALSE;
    }
    BOOL r = FS_CloseFile(file);
    if (a && ok && r && paletteData) return TRUE;
    return FALSE;
}
BOOL TalkFrame::loadCharacters(FSFile *file) {
    BOOL a = FS_OpenFile(file, data_020dddbc);
    BOOL ok;
    charData = Mem_AllocTail(0x2800);
    if (charData != NULL) {
        s32 n = FS_ReadFile(file, charData, 0x2800);
        ok = FALSE;
        if (n != ~ok) ok = TRUE;
    } else {
        ok = FALSE;
    }
    BOOL r = FS_CloseFile(file);
    if (a && ok && r && charData) return TRUE;
    return FALSE;
}
void TalkFrame::setNamePalette(s32 a, BOOL b) {
    u32 pal;
    s32 y, x;
    if (b != 0) {
        pal = 0xb000;
    } else if (a == 0) {
        pal = 0x1000;
    } else {
        pal = 0x2000;
    }
    if (pal != 0x1000) {
        for (y = 13; y <= 16; y++) {
            for (x = 3; x <= 14; x++) {
                u16 *p = screenData + y * 32;
                if ((p[x] & 0xf000) == 0x1000) {
                    u16 v = (u16)(p[x] & 0xffff0fff);
                    p[x] = v | pal;
                }
            }
        }
    }
}
// ---- TalkFrame
void TalkFrame::setTextPalette(s32 idx) {
    s32 y, x;
    for (y = 15; y <= 24; y++) {
        for (x = 1; x <= 30; x++) {
            u16 *p = screenData + y * 32;
            u16 v = (u16)(p[x] & 0xffff0fff);
            p[x] = v | (((u32)data_020cba24[idx] << 28) >> 16);
        }
    }
}
#undef data_020dddbc
#undef data_020dddd4
#undef data_020dddec
#undef data_020dde08
#undef data_020dde24

// ======== unk_02068558.cpp ========
BOOL TalkFrame::load(s32 a, u32 b, s32 c, u8 d) {
    BOOL result = FALSE;
    BOOL alt = (b == 0 && a == 0) ? TRUE : FALSE;
    FSFile file;
    FS_InitFile(&file);
    if (loadScreen(&file, alt)) {
        if (loadPalette(&file)) {
            if (loadCharacters(&file)) {
                if (alt) {
                    setNamePalette(c, d);
                } else {
                    setTextPalette(a);
                }
                result = TRUE;
            }
        }
    }
    return result;
}

// ======== unk_02067c70.cpp ========
void TalkFrame::freeBuffers()
{
    if (screenData) { Mem_Free(screenData); screenData = 0; }
    if (paletteData) { Mem_Free(paletteData); paletteData = 0; }
    if (charData) { Mem_Free(charData); charData = 0; }
}
extern "C" void TalkFrame_SetupBgControl()
{
    volatile u16 *r = (volatile u16 *)0x400000c;
    IO_W(r, (IO_R(r) & ~3) | 1);
    IO_W(r, (IO_R(r) & 0x43) | 0x600);
    IO_W(r, IO_R(r) & ~0x40);
    Gfx2d_SetMainWin0Planes(0x1f);
    Gfx2d_SetMainWinOutPlanes(0x1b);
}
void TalkFrame::uploadBg()
{
    u16 *q = (u16 *)paletteData;
    DC_FlushRange(q + 1, 0x17e);
    GX_LoadBGPltt(q + 1, 2, 0x17e);
    DC_FlushRange(charData, 0x2800);
    GX_LoadBG2Char(charData, 0, 0x2800);
    DC_FlushRange(screenData, 0x800);
    GX_LoadBG2Scr(screenData, 0, 0x800);
}
#ifdef N3DS_DIRECT
int gN3dsTalkBgShown = 0;  // (n3ds_ui.h)
#endif
extern "C" void TalkFrame_ShowBg()
{
    Gfx2d_ShowMainPlanes(4);
    Gfx2d_EnableMainWindows(1);
#ifdef N3DS_DIRECT  // (the text box, BG2 in window 0, at the wide screen's bottom edge)
    gpu2d_bg_anchor(0, 4, 1, N3DS_UI_BOTTOM);
    gN3dsTalkBgShown = 1;
#endif
}
extern "C" void TalkFrame_HideBg()
{
    Gfx2d_HideMainPlanes(4);
    Gfx2d_DisableMainWindows(1);
#ifdef N3DS_DIRECT
    gpu2d_bg_anchor(0, 0, 0, 0);
    gN3dsTalkBgShown = 0;
#endif
}
void TalkFrame::setScrollY(s32 y) { setScroll(0, y); }
void TalkFrame::setScroll(s32 x, s32 y)
{
    scrollX = x;
    scrollY = y;
    Gfx2d_SetMainBg2Offset(x, y);
    s32 hi = 0xff - x;
    s32 lo = -x;
    if (lo < 0) lo = 0;
    if (hi > 0xff) hi = 0xff;
    Gfx2d_SetMainWin0Rect(lo, 0x3c, hi, 0xc0);
}
void TalkFrame::hideArrow() { arrow.setState(0); }
void TalkFrame::showArrowWait() { arrow.setState(1); }
void TalkFrame::showArrowNext() { arrow.setState(2); }
void TalkFrame::showArrowEnd() { arrow.setState(3); }
void TalkFrame::updateArrow()
{
    arrow.setOffset(scrollX + 0x55, scrollY + 0x47);
    arrow.update();
}
void TalkFrame::drawArrow() { arrow.draw(); }
BOOL TalkFrame::isArrowHidden()
{
    if (busyIconTimer == 0 && arrow.getState() == 0) return TRUE;
    return FALSE;
}
BOOL TalkFrame::isArrowWaiting()
{
    if (busyIconTimer == 0 && arrow.getState() == 1) return TRUE;
    return FALSE;
}
BOOL TalkFrame::isArrowNextDone()
{
    if (busyIconTimer == 0 && arrow.getState() == 2 && arrow.isAnimDone()) return TRUE;
    return FALSE;
}
BOOL TalkFrame::isArrowEndDone()
{
    if (busyIconTimer == 0 && arrow.getState() == 3 && arrow.isAnimDone()) return TRUE;
    return FALSE;
}
void TalkFrame::showBusyIcon(u32 se)
{
    busyIcon.requestShow(se);
    busyIconTimer = -1;
}
void TalkFrame::hideBusyIcon()
{
    busyIcon.requestHide();
    busyIconTimer = 4;
}
void TalkFrame::updateBusyIcon()
{
    busyIcon.setPos(scrollX + 0x59, scrollY + 0x4b);
    busyIcon.callUpdate();
    if (busyIconTimer > 0) busyIconTimer--;
}
void TalkFrame::drawBusyIcon() { busyIcon.callDraw(); }
extern "C" u8 Talk_ColorTagToTextColor(u32 x)
{
    if (x == 9) x = 2;
    return x + 1;
}
void TalkVoice::setVoiceOverride(s32 v) { voiceOverride = v; }
void TalkVoice::clearVoiceOverride() { voiceOverride = 5; }
void TalkVoice::setMsgModeOverride(s32 v) { msgModeOverride = v; }
void TalkVoice::clearMsgModeOverride() { msgModeOverride = 7; }
void TalkVoice::setVoiceType(s32 r)
{
    if (voiceOverride == 5 && voiceType != 5 && voiceType != r) {
        Snd_SetVoiceType(r);
        voiceType = r;
    }
}
void TalkVoice::refreshVoiceType()
{
    setVoiceType(window->request->getVoiceType());
}
TalkVoice::TalkVoice(TalkWindowState *o)
    : window(o), msgModeOverride(7), msgMode(0), instantByMsg(0), instantByTag(0), inWait(0), voiceOverride(5), voiceType(5), textColor(0)
{
    reset(1);
}
TalkVoice::~TalkVoice() {}
void TalkVoice::reset(s32 flag)
{
    s32 i = 0;
    u16 d = sCharSortKeyZero;
    for (; i < 2; i++) charKeys[i] = d;
    if (flag != 0) carryCharKey = d;
    numCharKeys = 0;
}
void TalkVoice::pushChar(u32 c)
{
    u32 cur = sCharSortKeyZero;
    u32 nw = cur;
    s32 low;
    if (numCharKeys < 2) low = 1; else low = 0;
    BOOL a, b;
    if (charKeys[0] == 0x2a) a = 1; else a = 0;
    if (charKeys[1] == 0x2a) b = 1; else b = 0;
    if (low != 0 || a != 0 || b != 0) {
        u8 buf;
        if (Text_AsciiToGameChar(&buf, c)) {
            s32 v = Text_GetCharSortKey(buf);
            if (v != cur) {
                if (v == 0x26 && inWait != 0) v = 0x27;
                nw = v;
            }
        }
    }
    if (nw != cur) {
        if (low == 0) {
            if (a != 0) {
                charKeys[0] = charKeys[1];
                charKeys[1] = cur;
                numCharKeys = numCharKeys - 1;
            } else if (b != 0) {
                charKeys[1] = cur;
                numCharKeys = numCharKeys - 1;
            }
        }
        s32 n = numCharKeys;
        if (n < 2) {
            numCharKeys = n + 1;
            charKeys[n] = nw;
        }
    }
}
void TalkVoice::update()
{
    updateVoiceType();
    shiftHistory();
    if (instantByMsg == 0 && instantByTag == 0) {
        s32 a = getVoiceStyle();
        s32 b = getSpeakerMoodIndex();
        if (numCharKeys >= 1) {
            if (voiceType == 5) {
                if (a != 2) Snd_PlaySe(0x2c);
            } else {
                Snd_PlayTalkVoice(a, b, charKeys[0], charKeys[1]);
            }
        }
        s32 flag = 0;
        if (textColor != 4) {
            s32 i = 0;
            u16 d = sCharSortKeyZero;
            for (; i < numCharKeys; i++) {
                u16 h = charKeys[i];
                if (h != d && h != 0x2a && h != 0x2b && h != 0x29 && h != 0x26 && h != 0x28 && h != 0x27 && h != 0x25)
                    flag = 1;
            }
        }
        window->setVoicePlaying(flag);
    }
    if (inWait != 0) window->resetVoice();
}
void TalkVoice::setMsgMode(s32 v) { msgMode = v; }
void TalkVoice::begin()
{
    s32 r = 5;
    if (voiceOverride != 5) {
        r = voiceOverride;
    } else if (getVoiceStyle() == 0) {
        if (msgMode == 3) r = 1;
        else r = window->request->getVoiceType();
    }
    voiceType = r;
    if (r != 5) Snd_BeginTalk(r);
}
void TalkVoice::end()
{
    if (voiceType != 5) Snd_EndTalk();
    voiceType = 5;
}
u32 TalkVoice::getVoiceStyle()
{
    u32 r = PlayerOptions_GetTalkVoice();
    s32 a = msgMode;
    BOOL b = r == 0 ? TRUE : FALSE;
    if (msgModeOverride != 7) a = msgModeOverride;
    if (a != 0 && a != 3 && b) r = 1;
    if (textColor == 4 && b) r = 1;
    return data_020cba0c[r];
}
u8 TalkVoice::getSpeakerMoodIndex()
{
    u32 i = 0;
    void *p = window->request->getSpeakerData();
    if (p) {
        VillagerState *st = Villager_GetState((VillagerData *)p);
        if (st) i = VillagerState_GetMood(st);
    }
    return data_020cba14[i];
}
void TalkVoice::updateVoiceType()
{
    s32 r = 5;
    if (voiceType == 0) {
        if (textColor == 9) r = 4;
    } else if (voiceType == 4) {
        if (textColor != 9) r = window->request->getVoiceType();
    }
    if (r != 5) setVoiceType(r);
}
void TalkVoice::shiftHistory()
{
    if (numCharKeys == 0) {
        u16 c = sCharSortKeyZero;
        if (carryCharKey != c) {
            charKeys[0] = carryCharKey;
            carryCharKey = c;
            numCharKeys++;
        }
    } else if (numCharKeys == 1) {
        if (carryCharKey != sCharSortKeyZero) {
            charKeys[1] = charKeys[0];
            charKeys[0] = carryCharKey;
            carryCharKey = charKeys[1];
            numCharKeys++;
        }
    } else {
        carryCharKey = charKeys[1];
    }
}
TalkWindowState::TalkWindowState()
    : index(0), state(0), nextState(6), stateStep(0), autoAdvanceTimer(0), openMode(4), choicePending(0),
      scrollY(0), shakeAngle(0), shakeAmplitude(0), shakeDecay(0), shakeScaleX(0), shakeScaleY(0),
      nameLabel(0), textBox(this), parser(this, &textBox), runner(&parser),
      request(0), voice(this), year(0), weekday(0), second(0), timeOfDay(0),
      randomVillagerNameBuilt(0), otherResidentNameBuilt(0), otherResidentIsVillager(0), nextMsgIndex(gTalkMsgIndexNone),
      advancePending(0), advanceLocked(0), startNotifyPending(0), endNotifyPending(0), choiceNotifyPending(0), voicePlaying(0), inputDisabled(0), noOpenSe(0), silent(0)
{
    MI_CpuFill8(nextFileName, 0, 0x1a);
}

// ======== unk_020671ec.cpp ========
#define data_020dddb8 ((u8 *)"20")
typedef void (TalkWindowState::*Unk_020660f8_Fn)();

// (the members are destroyed after the body: greeting ... frame, the reverse of their order)
TalkWindowState::~TalkWindowState() {
    destroyNameLabel();
    detachRequest();
}
BOOL TalkWindowState::setNextMessageIfUnset(u8 *src, void *s) {
    BOOL r = FALSE;
    if (nextMsgIndex == gTalkMsgIndexNone) {
        setNextMessage(src, s);
        r = TRUE;
    }
    return r;
}
void TalkWindowState::setNextMessage(u8 *src, void *s) {
    nextMsgIndex = *src;
    if (s) {
        strncpy((char *)nextFileName, (const char *)s, 0x19);
    } else {
        MI_CpuFill8(nextFileName, 0, 0x1a);
    }
}
void TalkWindowState::lockAdvance() { advanceLocked = 1; }
void TalkWindowState::unlockAdvance() { advanceLocked = 0; }
void TalkWindowState::setAdvancePending() { advancePending = 1; }
void TalkWindowState::clearAdvancePending() { advancePending = 0; }
s32 TalkWindowState::setSlot(s32 idx, void *p) {
    return slots[idx].copy((MsgString *)(p));
}
void TalkWindowState::setSlotFromString(s32 idx, u8 *a, const char *b) {
    String_Load(&slots[idx], a, b);
}
void TalkWindowState::setNamedSlot(s32 idx, void *p, u32 color) {
    namedSlots[idx].copy((MsgString *)(p));
    namedSlotColors[idx] = color;
}
void TalkWindowState::openChoices(s32 v) {
    if (v) {
        choicePending = 1;
    } else {
        choiceMenu.setListChoices(&choiceList);
        choiceMenu.open();
    }
}
ChoiceList *TalkWindowState::getChoiceList() { return &choiceList; }
u8 TalkWindowState::isVoicePlaying() { return voicePlaying; }
void TalkWindowState::showBusyIcon(u32 se) { frame.showBusyIcon(se); }
void TalkWindowState::hideBusyIcon() { frame.hideBusyIcon(); }
void TalkWindowState::attachRequest(TalkMsgRequest *p) {
    request = p;
    p->attachWindow(this);
}
void TalkWindowState::detachRequest() {
    if (request) {
        request->detachWindow();
        request = 0;
    }
}
void TalkWindowState::disableInput() { inputDisabled = 1; }
void TalkWindowState::setKeepSe() { noOpenSe = 1; }
void TalkWindowState::setSilent() { silent = 1; }
extern "C" TalkWindowState *TalkWindow_Get(s32 i) {
    TalkWindowState *r = 0;
    if (gTalkWindows) r = gTalkWindows + i;
    return r;
}
extern "C" void TalkWindow_CreateAll() {
    gTalkWindows = new (kGameHeap) TalkWindowState[2];
    for (s32 i = 0; i < 2; i++) gTalkWindows[i].index = i;
}
extern "C" void TalkWindow_DestroyAll() {
    if (gTalkWindows) {
        delete[] gTalkWindows;
        gTalkWindows = 0;
    }
}
extern "C" void TalkWindow_UpdateAll() {
    if (gTalkWindows) {
        for (s32 i = 0; i < 2; i++) gTalkWindows[i].update();
    }
}
extern "C" void TalkWindow_DrawAll() {
    if (gTalkWindows) {
        for (s32 i = 0; i < 2; i++) gTalkWindows[i].draw();
    }
}
namespace n0 {
extern "C" { extern const u8 data_020cba14[0x5]; const u8 data_020cba14[0x5] = {0, 4, 1, 2, 3}; }
extern "C" { extern const u8 data_020cba0c[0x3]; const u8 data_020cba0c[0x3] = {0, 1, 2}; }
extern "C" { extern const u8 data_020cba24[0x7]; const u8 data_020cba24[0x7] = {3, 6, 7, 8, 9, 5, 10}; }
extern "C" { s16 sTalkWindowCloseOffsets[6] = {-101, -48, -20, -10, 0, 0}; }
}
void TalkWindowState::update() {
    static Unk_020660f8_Fn tbl[6] = {
        &TalkWindowState::execIdle, &TalkWindowState::execOpening, &TalkWindowState::execWaitInput,
        &TalkWindowState::execPrinting, &TalkWindowState::execClosing, &TalkWindowState::execClosed,
    };
    (this->*tbl[state])();
    applyStateChange();
    updateShake();
    frame.update();
    choiceMenu.update();
    if (state != 0 && state != 5) storeInputMode();
}
void TalkWindowState::draw() {
#ifdef N3DS_DIRECT  // (n3ds_ui.h: the text box at the wide screen's bottom edge, its OBJs with its BG: TalkFrame_ShowBg)
    int n = Oam_Count(0);
    frame.draw();
    choiceMenu.draw();
    Oam_Anchor(0, n, N3DS_UI_BOTTOM);
#else
    frame.draw();
    choiceMenu.draw();
#endif
}
extern "C" BOOL Talk_IsAltTextEnabled() {
    if (PlayerOptions_IsHiragana() == 0) return TRUE;
    return FALSE;
}
void TalkWindowState::onCharPrinted(s32 c) {
    voice.pushChar(c);
}
void TalkWindowState::setVoicePlaying(BOOL v) {
    if (v) {
        voicePlaying = 1;
    } else {
        voicePlaying = 0;
    }
}
void TalkWindowState::resetVoice() {
    voicePlaying = 0;
    voice.reset(1);
}
MsgString *TalkWindowState::getYearText() {
    MsgString33 t;
    String_FormatNumber(&t, year, 2, 6, 0, 0);
    yearText.set(data_020dddb8);
    yearText.appendString(&t);
    return &yearText;
}
MsgString *TalkWindowState::getMonthText() {
    String_GetMonthName(&monthText, month);
    return &monthText;
}
MsgString *TalkWindowState::getDayText() {
    String_GetDayOrdinal(&dayText, day);
    return &dayText;
}
MsgString *TalkWindowState::getWeekdayText() {
    String_GetWeekdayName(&weekdayText, weekday, 0);
    return &weekdayText;
}
MsgString *TalkWindowState::getHourText() {
    u32 t = hour;
    if (t >= 12) t -= 12;
    if (t == 0) t = 12;
    String_FormatNumber(&hourText, t, 2, 0, 0, 0);
    return &hourText;
}
MsgString *TalkWindowState::getMinuteText() {
    String_FormatNumber(&minuteText, minute, 2, 6, 9, 0);
    return &minuteText;
}
MsgString *TalkWindowState::getNumber1714Text() {
    String_FormatNumber(&secondText, second, 2, 0, 0, 0);
    return &secondText;
}
void TalkWindowState::startShake(s32 a, s32 b, s32 c, s32 d) {
    shakeAngle = 0;
    shakeAmplitude = a;
    shakeDecay = b;
    shakeScaleX = c;
    shakeScaleY = d;
    shakeDecay = shakeDecay * 3;
    shakeDecay = shakeDecay >> 1;
}
void TalkWindowState::refreshNameColor() {
    s32 y;
    s32 x;
    u16 v;
    u16 c0;
    u16 c1;
    u16 * row;
    u16 c2;
    s32 mode;
    u16 * base;
    u16 * p;
    BOOL flag;
    if (request->getSpeakerData() == 0) {
        flag = TRUE;
    } else {
        flag = FALSE;
    }
    mode = request->getNameKind();
    base = (u16 *)G2_GetBG2ScrPtr();
    y = 13;
    c0 = data_020cbadc;
    c1 = data_020cbae0;
    c2 = data_020cbae4;
    for (; y <= 16; y++) {
        x = 3;
        row = base + y * 32;
        do {
            p = row + x;
            v = *p & 0xffff0fff;
            if (flag) {
                v |= c0;
            } else if (mode == 1) {
                v |= c1;
            } else {
                v |= c2;
            }
            *p = v;
        x++; } while (x <= 14);
    }
    destroyNameLabel();
    createNameLabel();
}
void TalkWindowState::loadInputMode() {
    if (inputDisabled == 0) Input_LoadMode();
}
void TalkWindowState::storeInputMode() {
    if (inputDisabled == 0) Input_StoreMode();
}
BOOL TalkWindowState::checkDeviceSwitch() {
    BOOL r = FALSE;
    if (inputDisabled == 0) {
        if (Input_IsTouchMode()) {
            if (Input_IsAnyKeyTrig()) {
                Input_SetButtonMode();
                r = TRUE;
            }
        } else if (Input_IsButtonMode()) {
            if (Input_IsTouchTrig()) {
                Input_SetTouchMode();
                r = TRUE;
            }
        }
    }
    return r;
}
BOOL TalkWindowState::isAdvanceTriggered() {
    BOOL r = FALSE;
    if (inputDisabled == 0) {
        if (checkDeviceSwitch() == 0) {
            if (Input_IsTouchTrigInRect(8, 0xf8, 0x70, 0xc0)) {
                r = TRUE;
            } else if (Input_IsATrig() || Input_IsBTrig()) {
                r = TRUE;
            }
        }
    }
    return r;
}
BOOL TalkWindowState::isSkipHeld() {
    BOOL r = FALSE;
    if (inputDisabled == 0) {
        if (checkDeviceSwitch() == 0) {
            if (Input_IsTouchTrigInRect(8, 0xf8, 0x70, 0xc0)) {
                r = TRUE;
            } else if (Input_IsBHeld()) {
                r = TRUE;
            }
        }
    }
    return r;
}
void TalkWindowState::updateShake() {
    if (shakeAmplitude != 0) {
        BOOL c = TRUE;
        if (state != 3 && state != 2) c = FALSE;
        BOOL z = Math_StepS32Alt(&shakeAmplitude, 0, shakeDecay) != 0 ? TRUE : FALSE;
        if (c != 0 && z == 0) {
            shakeAngle = shakeAngle + 0x4000;
            s32 a = func_01ffcb0c(shakeAmplitude, shakeScaleX);
            s32 b = func_01ffcb0c(shakeAmplitude, shakeScaleY);
            s32 ang = shakeAngle;
            a = func_01ffcb0c(data_02135f44[(((u16)(s16)(ang + 0x2000)) >> 4) * 2 + 1], a);
            b = func_01ffcb0c(data_02135f44[(((u16)(s16)(ang * 2 - 0x4000)) >> 4) * 2], b);
            frame.setScroll(-(a >> 12), scrollY + (b >> 12));
        } else {
            s32 zero = 0;
            shakeAngle = zero;
            shakeAmplitude = zero;
            shakeDecay = zero;
            shakeScaleX = zero;
            shakeScaleY = zero;
            frame.setScroll(zero, scrollY);
        }
    }
}
void TalkWindowState::captureClock() {
    year = Clock_GetYear();
    Clock_GetDayMonth(&day);
    Clock_GetMinuteHour(&minute);
    weekday = Clock_GetWeekday();
    second = Clock_GetSecond();
    timeOfDay = Clock_GetTimeOfDay();
}
void TalkWindowState::resetPrint() {
    parser.clearFastForward();
    stopAutoAdvance();
}
void TalkWindowState::finish() {
    clearAdvancePending();
    resetPrint();
    request->onTalkEnd();
}
void TalkWindowState::stopText() {
    parser.stop();
    textBox.clearLabels();
    finish();
}
#undef data_020dddb8

// ======== unk_02067188.cpp ========
void TalkWindowState::resetTextVars() {
    parser.resetColor();
    clearRandomVillagerName();
    clearOtherResidentName();
    clearCatchphrase();
    clearTownName();
    clearPlayerName();
    clearFriendName();
    clearEnemyName();
    clearTrend();
    clearImpression();
    clearNickname();
    clearCompliment();
    clearGreeting();
    stopText();
}

// ======== unk_020668a0.cpp ========
#define data_020ddd7c ((char *)"%s/%s/%s/%s_.bmg")
#define data_020ddd90 ((char *)"%s/%s/Other/%s_.bmg")
#define data_020ddda4 ((char *)"%s/Other/%s_.bmg")
void TalkWindowState::resetVars() {
    resetTextVars();
    noOpenSe = 0;
}
void TalkWindowState::reset() {
    resetVars();
    clearSlots();
    clearNamedSlots();
    unlockAdvance();
}
void TalkWindowState::clearSlots() {
    s32 i;
    for (i = 0; i < 11; i++) {
        slots[i].initEmpty();
    }
}
void TalkWindowState::clearNamedSlots() {
    s32 i;
    for (i = 0; i < 4; i++) {
        namedSlots[i].initEmpty();
        namedSlotColors[i] = 7;
    }
}
void TalkWindowState::clearCatchphrase() { catchphrase.clear(); }
BOOL TalkWindowState::buildCatchphrase() {
    void *r6 = request->getSpeakerData();
    BOOL r4 = FALSE;
    catchphrase.clear();
    if (r6 != 0) {
        static_cast<VillagerData *>(r6)->getCatchphrase(&catchphrase, (void *)1);
        r4 = TRUE;
    }
    return r4;
}
void TalkWindowState::clearTownName() { townName.clear(); }
BOOL TalkWindowState::buildTownName() {
    townName.clear();
    TownId_GetNameString(gSaveData.townId, &townName);
    return TRUE;
}
void TalkWindowState::clearPlayerName() { playerName.clear(); }
BOOL TalkWindowState::buildPlayerName() {
    PlayerData *r4 = PlayerData_GetCurrent();
    playerName.clear();
    r4->getPlayerId()->getNameString(&playerName);
    return TRUE;
}
void TalkWindowState::clearFriendName() { friendName.clear(); }
BOOL TalkWindowState::buildFriendName() {
    void *r6 = request->getSpeakerData();
    BOOL r4 = FALSE;
    friendName.clear();
    if (r6 != 0) {
        Villager_GetFriendName(r6, &friendName);
        r4 = TRUE;
    }
    return r4;
}
void TalkWindowState::clearEnemyName() { enemyName.clear(); }
BOOL TalkWindowState::buildEnemyName() {
    void *r6 = request->getSpeakerData();
    BOOL r4 = FALSE;
    enemyName.clear();
    if (r6 != 0) {
        Villager_GetEnemyName(r6, &enemyName);
        r4 = TRUE;
    }
    return r4;
}
void TalkWindowState::clearRandomVillagerName() {
    randomVillagerNameBuilt = 0;
    randomVillagerName.clear();
}
BOOL TalkWindowState::buildRandomVillagerName() {
    BOOL r4 = TRUE;
    if (randomVillagerNameBuilt == 0) {
        void *r6 = request->getSpeakerData();
        randomVillagerName.clear();
        if (r6 != 0) {
            Villager_GetRandomOtherName(r6, &randomVillagerName);
        } else {
            r4 = FALSE;
        }
        randomVillagerNameBuilt = 1;
    }
    return r4;
}
void TalkWindowState::clearOtherResidentName() {
    otherResidentNameBuilt = 0;
    otherResidentIsVillager = 0;
    otherResidentName.clear();
}
BOOL TalkWindowState::buildOtherResidentName() {
    BOOL r4 = TRUE;
    if (otherResidentNameBuilt == 0) {
        PlayerData *g = PlayerData_GetCurrent();
        otherResidentName.clear();
        if (g->getOtherResidentName(&otherResidentName) == 0) {
            void *r0 = request->getSpeakerData();
            if (r0 != 0) {
                Villager_GetRandomOtherName(r0, &otherResidentName);
                otherResidentIsVillager = r4;
            } else {
                r4 = FALSE;
            }
        }
        otherResidentNameBuilt = 1;
    }
    return r4;
}
void TalkWindowState::clearTrend() { trend.clear(); }
BOOL TalkWindowState::buildTrend() {
    void *r6 = request->getSpeakerData();
    BOOL r4 = FALSE;
    trend.clear();
    if (r6 != 0) {
        if (Villager_GetTrend((VillagerData *)r6, &trend) != 0) {
            r4 = TRUE;
        }
    }
    return r4;
}
void TalkWindowState::clearImpression() { impression.clear(); }
BOOL TalkWindowState::buildImpression() {
    void *r6 = request->getSpeakerData();
    BOOL r4 = FALSE;
    impression.clear();
    if (r6 != 0) {
        Villager_GetImpressionText(r6, &impression, NULL);
        r4 = TRUE;
    }
    return r4;
}
void TalkWindowState::clearNickname() { nickname.clear(); }
BOOL TalkWindowState::buildNickname() {
    void *r6 = request->getSpeakerData();
    BOOL r4 = FALSE;
    nickname.clear();
    if (r6 != 0) {
        PlayerData *g = PlayerData_GetCurrent();
        Villager_GetNicknameFor(r6, &nickname, g->getPlayerId());
        r4 = TRUE;
    }
    return r4;
}
void TalkWindowState::clearCompliment() { compliment.clear(); }
BOOL TalkWindowState::buildCompliment() {
    void *r6 = request->getSpeakerData();
    BOOL r4 = FALSE;
    compliment.clear();
    if (r6 != 0) {
        PlayerData *g = PlayerData_GetCurrent();
        ((VillagerDataItemView *)r6)->getComplimentFor(&compliment, g->getPlayerId());
        r4 = TRUE;
    }
    return r4;
}
void TalkWindowState::clearGreeting() { greeting.clear(); }
BOOL TalkWindowState::buildGreeting() {
    void *r6 = request->getSpeakerData();
    BOOL r4 = FALSE;
    greeting.clear();
    if (r6 != 0) {
        PlayerData *g = PlayerData_GetCurrent();
        if (((VillagerDataItemView *)r6)->getGreetingFor(&greeting, g->getPlayerId()) != 0) {
            r4 = TRUE;
        }
    }
    return r4;
}
void TalkWindowState::stopAutoAdvance() {
    autoAdvanceTimer = 0;
}
void TalkWindowState::startAutoAdvance(s32 frames) {
    autoAdvanceTimer = frames;
}
BOOL TalkWindowState::tickAutoAdvance() {
    BOOL r = FALSE;
    if (autoAdvanceTimer > 0) {
        autoAdvanceTimer--;
        if (autoAdvanceTimer == 0) {
            r = TRUE;
        }
    }
    return r;
}
void TalkWindowState::applyFriendshipDelta(s32 idx) {
    if (idx != 0) {
        void *r4 = request->getSpeakerData();
        PlayerData *g = PlayerData_GetCurrent();
        if (r4 != 0) {
            s32 r7 = Villager_FindMemoryIndex((VillagerMemory *)r4, g->getPlayerId());
            VillagerMemory *r6 = Villager_GetMemory((VillagerMemory *)r4, r7);
            if (r6 != 0) {
                r6->addFriendship(sTalkFriendshipDeltas[idx]);
                VillagerSync_Friendship(r4, r7, r6->getFriendship());
            }
        }
    }
}
void TalkWindowState::createNameLabel() {
    nameLabel = MsgTextLabel_CreateVram(0x89, 8, 2);
    if (nameLabel != 0) {
        nameLabel->copyMode = 2;
        nameLabel->bgColor = 0xe;
        MsgString9 *o = request->getSpeakerName();
        MsgTextLabel *t = nameLabel;
        t->textStart = o->data();
        nameLabel->alignCenter();
        nameLabel->fgColor = 2;
        nameLabel->requestRedraw();
    }
}
void TalkWindowState::destroyNameLabel() {
    if (nameLabel != 0) {
        MsgTextLabel_Destroy(nameLabel);
        nameLabel = 0;
    }
}
void TalkWindowState::notifyMessageStart() {
    if (startNotifyPending != 0) {
        request->onMessageStart(BmgMsgAttr_GetByte06(&msgAttr));
        startNotifyPending = 0;
    }
}
void TalkWindowState::notifyMessageEnd() {
    if (endNotifyPending != 0) {
        applyFriendshipDelta(BmgMsgAttr_GetByte05(&msgAttr));
        request->onMessageEnd(BmgMsgAttr_GetByte07(&msgAttr));
        endNotifyPending = 0;
    }
}
void TalkWindowState::notifyChoice() {
    if (choiceNotifyPending != 0) {
        request->onChoice(BmgMsgAttr_GetByte07(choiceList.getResultAttr()));
        choiceNotifyPending = 0;
    }
}
void TalkWindowState::playMessageSe() {
    s32 r0 = BmgMsgAttr_GetByte09(&msgAttr);
    if (r0 < 2) {
        if (data_020cba10[r0] != 0) {
            Snd_PlaySe(data_020cba10[r0]);
        }
    }
}
void TalkWindowState::startMessage() {
    u8 loc;
    endNotifyPending = 1;
    startNotifyPending = 1;
    choiceNotifyPending = 0;
    textBox.setCentered(BmgMsgAttr_LookupUnkC(&msgAttr) == 1 ? 1 : 0);
    s32 r4 = BmgMsgAttr_LookupUnkB(&msgAttr);
    parser.setPrintMode(r4);
    voice.instantByMsg = r4 == 1 ? 1 : 0;
    BmgMsgAttr_GetByte08(&loc, &msgAttr);
    setNextMessage(&loc, (void *)(0));
    notifyMessageStart();
    playMessageSe();
    runner.reset();
    runner.start(msgBuffer.buffer);  // (msgBuffer.getBuffer())
}
void TalkWindowState::scanMessageTags() {
    ChoiceString *o = choiceList.getResultText();
    TalkTagScanner loc(this);
    loc.scan(o->data());
}
char *TalkWindowState::buildMessagePath(const char *a, const char *b) {
    const char *dir = b;
    if (dir == 0) {
        dir = request->getMsgDir();
    }
    const char *r5 = a ? a : (const char *)request->fileName;
    const char *r7 = matchDirPrefix(r5);
    if (r7 != 0) {
        r5 += strlen(r7) + 1;
        const char *r4 = matchSubdirPrefix(r5);
        if (r4 != 0) {
            const char *e = r5 + (strlen(r4) + 1);
            Str_SPrintf(sTalkMessagePath, data_020ddd7c, dir, r7, r4, e);
        } else {
            Str_SPrintf(sTalkMessagePath, data_020ddd90, dir, r7, r5);
        }
    } else {
        Str_SPrintf(sTalkMessagePath, data_020ddda4, dir, r5);
    }
    return sTalkMessagePath;
}
const char *TalkWindowState::matchDirPrefix(const char *s) {
    const char *const *p;
    const char *e;
    for (p = sTalkMessageDirs; (e = *p) != 0; p++) {
        u32 n = strlen(e);
        if (strncmp(s, e, n) == 0 && s[n] == '_') {
            break;
        }
    }
    return e;
}
const char *TalkWindowState::matchSubdirPrefix(const char *s) {
    const char *const *p;
    const char *e;
    for (p = sTalkMessageSubdirs; (e = *p) != 0; p++) {
        u32 n = strlen(e);
        if (strncmp(s, e, n) == 0 && s[n] == '_') {
            break;
        }
    }
    return e;
}
void TalkWindowState::loadMessageAttr() {
    msgBuffer.clear();
    if (msgBuffer.open(buildMessagePath(0, 0))) {
        msgBuffer.loadMessage(&request->msgIndex);
    }
    BmgMsgAttr_Copy(&msgAttr, (BmgMsgAttr *)Bmg_GetMsgAttr((u8 *)&msgBuffer));
    msgBuffer.close();
}
#undef data_020ddd7c
#undef data_020ddd90
#undef data_020ddda4

// ======== unk_0206684c.cpp ========
void TalkWindowState::loadNextMessage() {
    request->msgIndex = nextMsgIndex;
    if (nextFileName[0] != 0) {
        request->setFileName((const char *)nextFileName);
    }
    setNextMessage(&gTalkMsgIndexNone, 0);
    loadMessageAttr();
    startMessage();
    captureClock();
}

// ======== unk_02065f50.cpp ========
void TalkWindowState::reloadMessage() {
    loadMessageAttr();
    startMessage();
    captureClock();
}
void TalkWindowState::onChoiceDone() {
    u8 *a = choiceList.getResultValue();
    char *b = choiceList.getResultName();
    setNextMessage(a, b);
    setAdvancePending();
    choiceNotifyPending = 1;
    scanMessageTags();
    notifyChoice();
}
namespace n0 {
extern "C" { extern const s8 sTalkFriendshipDeltas[0x21]; const s8 sTalkFriendshipDeltas[0x21] = {0, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -15, -20, -25, -30, -35, -40}; }
extern "C" { char sTalkMessagePath[0x40]; }
extern "C" { extern const char *const sTalkMessageDirs[9]; const char *const sTalkMessageDirs[9] = {"fu", "bo", "ha", "ta", "ko", "ge", "sp", "obj", 0}; }
extern "C" { extern const char *const sTalkMessageSubdirs[26]; const char *const sTalkMessageSubdirs[26] = {"ai", "tsu", "ap", "3p", "etc", "ev", "npc", "q", "q01", "q02", "q03", "q04", "q05", "q06", "q07", "q08", "q09", "q10", "q11", "q12", "q13", "q14", "q15", "q16", "q17", 0}; }
}
void TalkWindowState::applyStateChange() {
    static Unk_020660f8_Fn tbl[6] = {
        &TalkWindowState::enterIdle, &TalkWindowState::enterOpening, &TalkWindowState::enterWaitInput,
        &TalkWindowState::enterPrinting, &TalkWindowState::enterClosing, &TalkWindowState::enterClosed,
    };
    if (nextState != 6) {
        state = nextState;
        (this->*tbl[nextState])();
        nextState = 6;
    }
}
namespace n0 {
extern "C" { s16 sTalkWindowOpenOffsets[6] = {0, -7, -14, -25, -50, -101}; }
extern "C" { extern const u16 data_020cba10[2]; const u16 data_020cba10[2] = {0x0, 0x27}; }
}
void TalkWindowState::enterIdle() { TalkFrame_HideBg(); }
void TalkWindowState::execIdle() {}
void TalkWindowState::enterOpening() {
    loadInputMode();
    BOOL c = openMode != 4 ? TRUE : FALSE;
    if (c) loadNextMessage();
    else reloadMessage();
    s32 r6 = BmgMsgAttr_LookupUnkA(&msgAttr);
    TalkMsgRequest *o = request;
    u8 *p = o->getSpeakerName()->data();
    u32 v8 = request->getNameKind();
    BOOL r7 = request->getSpeakerData() == 0 ? TRUE : FALSE;
    u32 r2 = request->isNoSpeakerName();
    if (r2 == 0 && r6 == 0 && *(s8 *)p == 0) r6 = 5;
    frame.load(r6, r2, v8, r7);
    voice.setMsgMode(r6);
    TalkFrame_SetupBgControl();
    frame.uploadBg();
    TalkFrame_ShowBg();
    frame.freeBuffers();
    stateStep = 5;
    scrollY = sTalkWindowOpenOffsets[stateStep];
    frame.setScrollY(scrollY);
    frame.hideArrow();
    createNameLabel();
    textBox.createLabels();
    if (gCommManager) ChatBalloon_Dismiss(gCommManager->myAid);
    if (noOpenSe == 0) Snd_PlaySe(9);
    if (inputDisabled == 0) {
        if (openMode != 1 && openMode != 2 && openMode != 3) data_021c1b3c->mixer.startTalkDuck();
    }
    if (c) openMode = 4;
}
void TalkWindowState::execOpening() {
    BOOL done = FALSE;
    if (stateStep > 0) stateStep = stateStep - 1;
    else done = TRUE;
    scrollY = sTalkWindowOpenOffsets[stateStep];
    frame.setScrollY(scrollY);
    if (done) {
        if (inputDisabled == 0) voice.begin();
        nextState = 3;
    }
}
void TalkWindowState::enterWaitInput() { frame.hideArrow(); }
void TalkWindowState::execWaitInput() {
    runner.advance();
    s32 st = parser.printStatus;
    BOOL a = st == 2 ? TRUE : FALSE;
    BOOL b = st == 3 ? TRUE : FALSE;
    BOOL c = st == 4 ? TRUE : FALSE;
    BOOL d = st == 5 ? TRUE : FALSE;
    if (choiceMenu.justDecided() != 0) onChoiceDone();
    if (a || b || c) {
        if (advanceLocked == 0 && choiceMenu.isIdle() != 0) {
            if (advancePending != 0) {
                advance();
            } else if (frame.isArrowHidden() != 0) {
                frame.showArrowWait();
            } else if (frame.isArrowWaiting() != 0) {
                if (isAdvanceTriggered() != 0 || tickAutoAdvance() != 0) {
                    frame.showArrowNext();
                    if (silent == 0) Snd_PlaySe(0x10);
                    if (choicePending != 0 || parser.choiceDeferred != 0) Snd_PlaySe(0x13);
                }
            } else if (frame.isArrowNextDone() != 0) {
                frame.showArrowEnd();
            } else if (frame.isArrowEndDone() != 0) {
                advance();
            }
        } else {
            if (frame.isArrowHidden() == 0) frame.hideArrow();
        }
    } else if (d) {
        if (choiceMenu.isIdle() != 0) advance();
    }
}
void TalkWindowState::enterPrinting() {
    frame.hideArrow();
    stateStep = 1;
}
void TalkWindowState::execPrinting() {
    if (isSkipHeld() != 0) parser.setFastForward();
    voice.reset(0);
    runner.advance();
    textBox.redrawLines();
    if (inputDisabled == 0) voice.update();
    s32 st = parser.printStatus;
    BOOL b3 = st == 3 ? TRUE : FALSE;
    BOOL b2 = st == 2 ? TRUE : FALSE;
    BOOL b4 = st == 4 ? TRUE : FALSE;
    BOOL b5 = st == 5 ? TRUE : FALSE;
    BOOL b0 = st == 0 ? TRUE : FALSE;
    BOOL z = stateStep == 0 ? TRUE : FALSE;
    BOOL go = TRUE;
    if (!z && !b2 && !b4 && !b5 && !b0) go = FALSE;
    if (b3) stateStep = 0;
    if (go) {
        if (z) notifyMessageEnd();
        nextState = 2;
        resetVoice();
    }
}
void TalkWindowState::enterClosing() {
    frame.hideArrow();
    stateStep = 4;
    scrollY = sTalkWindowCloseOffsets[stateStep];
    if (inputDisabled == 0) {
        voice.end();
        if (openMode != 1 && openMode != 2 && openMode != 3) data_021c1b3c->mixer.endTalkDuck();
    }
    if (silent == 0) Snd_PlaySe(10);
    request->onWindowClose();
}
void TalkWindowState::execClosing() {
    BOOL done = FALSE;
    if (stateStep > 0) stateStep = stateStep - 1;
    else done = TRUE;
    scrollY = sTalkWindowCloseOffsets[stateStep];
    frame.setScrollY(scrollY);
    if (done) {
        destroyNameLabel();
        textBox.destroyLabels();
        if (openMode != 4) {
            nextState = 5;
        } else {
            inputDisabled = 0;
            silent = 0;
            nextState = 0;
        }
    }
}
void TalkWindowState::enterClosed() { TalkFrame_HideBg(); }
void TalkWindowState::execClosed() {}
// ---- state class ----
void TalkWindowState::advance() {
    s32 st = parser.printStatus;
    BOOL a = st == 2 ? TRUE : FALSE;
    BOOL b = st == 4 ? TRUE : FALSE;
    BOOL c = st == 5 ? TRUE : FALSE;
    s32 r7 = 6;
    s32 r6 = r7;
    s32 flag = 0;
    s32 act = 0;
    if (choicePending != 0) {
        choicePending = 0;
        openChoices(0);
    } else if (a || b || c) {
        r7 = 1;
        r6 = 3;
        if (c) act = r7;
        else if (b) act = 2;
        else act = r6;
    } else {
        u32 x = gTalkMsgIndexNone;
        u32 y = nextMsgIndex;
        if (y == gTalkMsgIndexEnd) {
            r7 = 0;
            r6 = 4;
            act = 6;
        } else if (openMode != 4) {
            r7 = 0;
            r6 = 4;
            act = 5;
        } else if (y != x) {
            flag = 1;
            act = 4;
        }
    }
    if (flag != 0) {
        loadNextMessage();
        r7 = 1;
        r6 = 3;
    }
    if (r7 != 6) parser.printStatus = r7;
    if (r6 != 6) {
        nextState = r6;
        if (act == 1) resetPrint();
        else if (act == 2) finish();
        else if (act == 3) stopText();
        else if (act == 4) resetTextVars();
        else if (act == 5) resetVars();
        else if (act == 6) reset();
    }
}
MsgString9::MsgString9() { clear(); }
MsgString9::~MsgString9() {}
u32 MsgString9::capacity() { return 9; }
// ---- MsgString9 ----
u8 *MsgString9::data() { return text; }
TalkMsgRequest::TalkMsgRequest() {
    window = 0;
    noSpeakerName = 0;
}
TalkMsgRequest::~TalkMsgRequest() {}
void TalkMsgRequest::resetMsg() {
    MsgRequest::resetMsg();
    speakerName.clear();
    window = 0;
    noSpeakerName = 0;
}
void TalkMsgRequest::setSpeakerName(u8 *p, u32 v) {
    speakerName.set(p);
    speakerName.attr.form = v;
    noSpeakerName = 0;
}
void TalkMsgRequest::setSpeakerNameStr(MsgString *p, u32 v) {
    speakerName.copy(p);
    speakerName.attr.form = v;
    noSpeakerName = 0;
}
// ---- TalkMsgRequest ----
void TalkMsgRequest::setNoSpeakerName(u32 v) {
    speakerName.clear();
    speakerName.attr.form = v;
    noSpeakerName = 1;
}

// ======== unk_02065f14.cpp ========
void TalkMsgRequest::changeSpeakerName(MsgString *p, u32 v) {
    TalkWindowState *o = window;
    s32 t = o->state;
    BOOL five = (t == 5);
    setSpeakerNameStr(p, v);
    if (t != 0 && !five) {
        window->refreshNameColor();
        window->voice.refreshVoiceType();
    }
}

// ======== unk_020655e4.cpp ========
#define data_020ddd68 ((char *)"/script/ENG/message")
MsgString9 *TalkMsgRequest::getSpeakerName() { return &speakerName; }
u32 TalkMsgRequest::isNoSpeakerName() { return noSpeakerName; }
u32 TalkMsgRequest::getNameKind() { return speakerName.attr.form; }
const char *TalkMsgRequest::getMsgDir() { return data_020ddd68; }
void TalkMsgRequest::onMessageStart(u32 attr) {}
void TalkMsgRequest::onMessageEnd(u32 attr) {}
void TalkMsgRequest::onChoice(u32 attr) {}
void TalkMsgRequest::onSignalTag(s32 id) {}
void TalkMsgRequest::onActionTag0() {}
void TalkMsgRequest::onActionTag1(u32 arg) {}
void TalkMsgRequest::onActionTag2(u32 arg) {}
void TalkMsgRequest::onActionTag3(u32 arg) {}
void TalkMsgRequest::onActionTag4(u32 arg) {}
void TalkMsgRequest::onConditionTag(u32 condition, u32 branchCount) {}
void TalkMsgRequest::onEventTag(u32 a) {}
void TalkMsgRequest::onTag09_0() {}
void TalkMsgRequest::onTag09_1() {}
void TalkMsgRequest::onTag09_2() {}
void TalkMsgRequest::onTag09_3() {}
void TalkMsgRequest::onTag09_4() {}
void TalkMsgRequest::onTag09_5() {}
void TalkMsgRequest::onTag09_6() {}
void TalkMsgRequest::onTag09_7() {}
void TalkMsgRequest::onTag09_8() {}
void TalkMsgRequest::onTag09_9() {}
void TalkMsgRequest::onScannedTag(u32 tag) {}
void *TalkMsgRequest::getSpeakerData() { return NULL; }
s32 TalkMsgRequest::getVoiceType() { return 5; }
void TalkMsgRequest::onWindowClose() {}
void TalkMsgRequest::onTalkEnd() {}
void TalkMsgRequest::attachWindow(TalkWindowState *w) { window = w; }
void TalkMsgRequest::detachWindow() { window = 0; }
#undef data_020ddd68

#ifndef HOST64
// the DS layout (the 32-bit build keeps it)
static_assert(sizeof(TalkFrame) == 0x80, "TalkFrame");
static_assert(sizeof(TalkTextBox) == 0x490, "TalkTextBox");
static_assert(sizeof(TalkBmgReader) == 0x8a4, "TalkBmgReader");
static_assert(sizeof(TalkParser) == 0xe4, "TalkParser");
static_assert(sizeof(TalkCharStepper) == 0x3c, "TalkCharStepper");
static_assert(sizeof(TalkVoice) == 0x2c, "TalkVoice");
static_assert(sizeof(TalkWindowState) == 0x1a1c, "TalkWindowState");
static_assert(__builtin_offsetof(TalkWindowState, frame) == 0x1c, "TalkWindowState.frame");
static_assert(__builtin_offsetof(TalkWindowState, scrollY) == 0x9c, "TalkWindowState.scrollY");
static_assert(__builtin_offsetof(TalkWindowState, choiceMenu) == 0xb4, "TalkWindowState.choiceMenu");
static_assert(__builtin_offsetof(TalkWindowState, choiceList) == 0x314, "TalkWindowState.choiceList");
static_assert(__builtin_offsetof(TalkWindowState, nameLabel) == 0x588, "TalkWindowState.nameLabel");
static_assert(__builtin_offsetof(TalkWindowState, textBox) == 0x58c, "TalkWindowState.textBox");
static_assert(__builtin_offsetof(TalkWindowState, msgBuffer) == 0xa1c, "TalkWindowState.msgBuffer");
static_assert(__builtin_offsetof(TalkWindowState, parser) == 0x12c0, "TalkWindowState.parser");
static_assert(__builtin_offsetof(TalkWindowState, runner) == 0x13a4, "TalkWindowState.runner");
static_assert(__builtin_offsetof(TalkWindowState, request) == 0x13b0, "TalkWindowState.request");
static_assert(__builtin_offsetof(TalkWindowState, slots) == 0x13c0, "TalkWindowState.slots");
static_assert(__builtin_offsetof(TalkWindowState, namedSlots) == 0x15fc, "TalkWindowState.namedSlots");
static_assert(__builtin_offsetof(TalkWindowState, voice) == 0x16dc, "TalkWindowState.voice");
static_assert(__builtin_offsetof(TalkWindowState, year) == 0x1708, "TalkWindowState.year");
static_assert(__builtin_offsetof(TalkWindowState, catchphrase) == 0x1860, "TalkWindowState.catchphrase");
static_assert(__builtin_offsetof(TalkWindowState, greeting) == 0x19d0, "TalkWindowState.greeting");
static_assert(__builtin_offsetof(TalkWindowState, nextMsgIndex) == 0x19f7, "TalkWindowState.nextMsgIndex");
static_assert(__builtin_offsetof(TalkWindowState, silent) == 0x1a1a, "TalkWindowState.silent");
#endif

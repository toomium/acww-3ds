#include "types.h"
#include "n3ds_options.h"
#include "talk/MsgStringAttr.h"
#include "item/ItemId.h"
#include "sys/RecordFile.h"
#include "talk/EncodedStringBase.h"
#include "talk/EncodedString.h"
#include "talk/EncodedString16Buf.h"
#include "game/InfoTableSet.h"
#include "item/ItemInfoTables.h"
#include "proto/main/unk_020501d4.h"
#include "proto/main/unk_02052b50.h"
#include "proto/main/unk_02061478.h"




extern "C" {
u32 ItemInfo_GetPrice(u16 *p);
s32 ItemInfo_GetNameForm(u16 *);
u16 *ItemInfo_GetName(u16 *p);
void ItemInfo_FreeIndoor();
void ItemInfo_LoadIndoor(s32 a);
void ItemInfo_Exit();
s32 ItemInfo_Init();
BOOL ItemInfo_IsReady();
s32 Item_GetFurnitureIndex(u16 *p);
BOOL Item_IsFurniture(u16 *p);
BOOL Item_IsNormalItem(u16 *p);
BOOL Item_IsFlowerAltItem(u16 *p);
BOOL Item_IsFlowerItem(u16 *p);
void Item_FromPlacedForm(u16 *out, u16 *in);
u8 *Series_GetName(s32);
BOOL ItemInfo_IsHoldable(u16 *p);
s32 ItemInfo_GetSeason(u16 *p);
s32 ItemInfo_GetUnk07(u16 *p);
s32 ItemInfo_GetClass(u16 *p);
s32 ItemInfo_GetKind(u16 *p);
s32 ItemInfo_GetSeries(u16 *p);
s32 ItemInfo_GetNameAttrA(u16 *p);
s32 ItemInfo_GetNameAttrB(u16 *p);
s32 ItemInfo_GetUnk02(u16 *p);
void ItemInfo_CountClass1dHeadwear();
void ItemInfo_CountFlowerItems();
void ItemInfo_CountHoldable();
void ItemInfoTables_FreeIndoor(ItemInfoTables *);
void ItemInfoTables_LoadIndoor(ItemInfoTables *, s32);
void ItemInfoTables_Close(ItemInfoTables *);
s32 ItemInfoTables_Open(ItemInfoTables *);
void MI_CpuCopy8(const void *src, void *dst, u32 n);
}

extern const u16 sFtrClassBasePoints[46];
extern char sIconModelStrNet[];
extern char sIconModelStrCap[];
extern char sIconModelStrWig[];
extern char sIconModelStrBox[];
extern char sIconModelStrAxe[];
extern char sIconModelStrSeed[];
extern char sIconModelStrGnet[];
extern char sIconModelStrCage[];
extern char sIconModelStrCoin[];
extern char sIconModelStrDust[];
extern char sIconModelStrGaxe[];
extern char sIconModelStrKabu[];
extern char sIconModelStrTane[];
extern char sIconModelStrWall[];
extern char sIconModelStrPear[];
extern char sIconModelStrGear[];
extern char sIconModelStrLeaf[];
extern char sIconModelStrSango[];
extern char sIconModelStrPaper[];
extern char sIconModelStrScoop[];
extern char sIconModelStrTimer[];
extern char sIconModelStrAcorn[];
extern char sIconModelStrMusic[];
extern char sIconModelStrCloth[];
extern char sIconModelStrPeach[];
extern char sIconModelStrApple[];
extern char sIconModelStrCherry[];
extern char sIconModelStrRKabu[];
extern char sIconModelStrGscoop[];
extern char sIconModelStrFossil[];
extern char sIconModelStrHanabi[];
extern char sIconModelStrClover[];
extern char sIconModelStrHaniwa[];
extern char sIconModelStrCarpet[];
extern char sIconModelStrOrange[];
extern char sIconModelStrPresent[];
extern char sIconModelStrPaint01[];
extern char sIconModelStrPaint02[];
extern char sIconModelStrPaint03[];
extern char sIconModelStrPaint05[];
extern char sIconModelStrPaint06[];
extern char sIconModelStrPaint07[];
extern char sIconModelStrPaint08[];
extern char sIconModelStrPaint09[];
extern char sIconModelStrPaint10[];
extern char sIconModelStrPaint11[];
extern char sIconModelStrPaint12[];
extern char sIconModelStrPaint13[];
extern char sIconModelStrPaint14[];
extern char sIconModelStrPaint15[];
extern char sIconModelStrSoldout[];
extern char sIconModelStrCoconut[];
extern char sIconModelStrCracker[];
extern char sIconModelStrMakigai[];
extern char sIconModelStrPaint04[];
extern char sIconModelStrGlasses[];
extern char sIconModelStrToolbox[];
extern char sIconModelStrPaint00[];
extern char sIconModelStrMakigai2[];
extern char sIconModelStrSeedling[];
extern char sIconModelStrMedicine[];
extern char sIconModelStrPaperbag[];
extern char sIconModelStrUmbrella[];
extern char sIconModelStrGtoolbox[];
extern char sIconModelStrWatering[];
extern char sIconModelStrPachinko[];
extern char sIconModelStrMoneybag[];
extern char sIconModelStrDeliverL[];
extern char sIconModelStrHoneycomb[];
extern char sIconModelStrGwatering[];
extern char sIconModelStrGpachinko[];
extern char sIconModelStrSeedlingC[];
extern char sIconModelStrFishingrod[];
extern char sIconModelStrSeedpitfall[];
extern char sIconModelStrBottleMail[];
extern char sIconModelStrGfishingrod[];

char sIconModelStrPresent[] = "present";
char sIconModelStrPaint11[] = "paint11";
char sIconModelStrPaint14[] = "paint14";
char sIconModelStrPaint06[] = "paint06";
char sIconModelStrWall[] = "wall";
char sIconModelStrFishingrod[] = "fishingrod";
char sIconModelStrSeedpitfall[] = "seedpitfall";
char sIconModelStrMoneybag[] = "moneybag";
char sIconModelStrPaint09[] = "paint09";
char sIconModelStrHanabi[] = "hanabi";
char sIconModelStrBottleMail[] = "bottle_mail";
char sIconModelStrTane[] = "tane";
char sIconModelStrWig[] = "wig";
char sIconModelStrDust[] = "dust";
const u16 sFtrClassBasePoints[46] = {0x33, 0x33, 0x33, 0x33, 0x97, 0x19c, 0x3e8, 0x33c, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 0x3, 0x12c, 0x3e8, 0x457, 0x378, 0x19c, 0x53, 0x607, 0xc80, 0x3, 0x0, 0x0, 0x19c, 0x500, 0x145, 0x3, 0x390, 0x320, 0x320, 0x320, 0x3, 0x3, 0x3, 0x0, 0x97, 0x0, 0x457, 0x457, 0x0, 0x97, 0x97, 0xd2};
char sIconModelStrPaint12[] = "paint12";
char sIconModelStrPaint08[] = "paint08";
char sIconModelStrPaint15[] = "paint15";
s32 sFlowerAltCount;
char sIconModelStrBox[] = "box";
char sIconModelStrPaper[] = "paper";
char sIconModelStrAxe[] = "axe";
char sIconModelStrCracker[] = "cracker";
char sIconModelStrMakigai[] = "makigai";
ItemInfoTables gItemInfo;
char sIconModelStrPeach[] = "peach";
char sIconModelStrOrange[] = "orange";
char sIconModelStrCap[] = "cap";
char sIconModelStrMakigai2[] = "makigai2";
char sIconModelStrScoop[] = "scoop";
char sIconModelStrPaint03[] = "paint03";
char sIconModelStrSeed[] = "seed";
s32 sHoldableItemCount;
char sIconModelStrGwatering[] = "gwatering";
char sIconModelStrMedicine[] = "medicine";
char sIconModelStrGpachinko[] = "gpachinko";
char sIconModelStrCage[] = "cage";
char sIconModelStrGscoop[] = "gscoop";
char sIconModelStrPaperbag[] = "paperbag";
char sIconModelStrApple[] = "apple";
char sIconModelStrUmbrella[] = "umbrella";
char sIconModelStrKabu[] = "kabu";
char sIconModelStrGtoolbox[] = "gtoolbox";
char sIconModelStrPaint05[] = "paint05";
char sIconModelStrPachinko[] = "pachinko";
char sIconModelStrCloth[] = "cloth";
char sIconModelStrMusic[] = "music";
char sIconModelStrGnet[] = "gnet";
char sIconModelStrCherry[] = "cherry";
char sIconModelStrCoin[] = "coin";
char sIconModelStrAcorn[] = "acorn";
char sIconModelStrLeaf[] = "leaf";
char sIconModelStrGear[] = "gear";
const char *sItemIconModelNames[146] = {
    sIconModelStrLeaf, sIconModelStrLeaf, sIconModelStrHaniwa, sIconModelStrHaniwa,
    sIconModelStrBox, sIconModelStrWall, sIconModelStrBox, sIconModelStrCarpet,
    sIconModelStrBox, sIconModelStrMusic, sIconModelStrPaperbag, sIconModelStrPaper,
    sIconModelStrCloth, sIconModelStrCloth, sIconModelStrBox, sIconModelStrUmbrella,
    sIconModelStrBox, sIconModelStrCap, sIconModelStrBox, sIconModelStrGlasses,
    sIconModelStrToolbox, sIconModelStrScoop, sIconModelStrGtoolbox, sIconModelStrGscoop,
    sIconModelStrToolbox, sIconModelStrAxe, sIconModelStrGtoolbox, sIconModelStrGaxe,
    sIconModelStrToolbox, sIconModelStrFishingrod, sIconModelStrGtoolbox, sIconModelStrGfishingrod,
    sIconModelStrToolbox, sIconModelStrNet, sIconModelStrGtoolbox, sIconModelStrGnet,
    sIconModelStrToolbox, sIconModelStrWatering, sIconModelStrGtoolbox, sIconModelStrGwatering,
    sIconModelStrToolbox, sIconModelStrPachinko, sIconModelStrGtoolbox, sIconModelStrGpachinko,
    sIconModelStrMoneybag, sIconModelStrCoin, sIconModelStrMoneybag, sIconModelStrMoneybag,
    sIconModelStrBox, sIconModelStrWig, sIconModelStrApple, sIconModelStrApple,
    sIconModelStrOrange, sIconModelStrOrange, sIconModelStrPear, sIconModelStrPear,
    sIconModelStrPeach, sIconModelStrPeach, sIconModelStrCherry, sIconModelStrCherry,
    sIconModelStrAcorn, sIconModelStrAcorn, sIconModelStrKabu, sIconModelStrKabu,
    sIconModelStrRKabu, sIconModelStrRKabu, sIconModelStrDust, sIconModelStrDust,
    sIconModelStrFossil, sIconModelStrFossil, sIconModelStrLeaf, sIconModelStrLeaf,
    sIconModelStrMakigai, sIconModelStrMakigai, sIconModelStrMakigai2, sIconModelStrMakigai2,
    sIconModelStrSango, sIconModelStrSango, sIconModelStrPresent, sIconModelStrPresent,
    sIconModelStrCage, sIconModelStrCage, sIconModelStrGear, sIconModelStrGear,
    sIconModelStrPaperbag, sIconModelStrSeed, sIconModelStrPaperbag, sIconModelStrSeedling,
    sIconModelStrSeedpitfall, sIconModelStrSeedpitfall, sIconModelStrPaperbag, sIconModelStrSeedlingC,
    sIconModelStrBottleMail, sIconModelStrBottleMail, sIconModelStrLeaf, sIconModelStrPaint00,
    sIconModelStrLeaf, sIconModelStrPaint01, sIconModelStrLeaf, sIconModelStrPaint02,
    sIconModelStrLeaf, sIconModelStrPaint03, sIconModelStrLeaf, sIconModelStrPaint04,
    sIconModelStrLeaf, sIconModelStrPaint05, sIconModelStrLeaf, sIconModelStrPaint06,
    sIconModelStrLeaf, sIconModelStrPaint07, sIconModelStrLeaf, sIconModelStrPaint08,
    sIconModelStrLeaf, sIconModelStrPaint09, sIconModelStrLeaf, sIconModelStrPaint10,
    sIconModelStrLeaf, sIconModelStrPaint11, sIconModelStrLeaf, sIconModelStrPaint12,
    sIconModelStrLeaf, sIconModelStrPaint13, sIconModelStrLeaf, sIconModelStrPaint14,
    sIconModelStrLeaf, sIconModelStrPaint15, sIconModelStrLeaf, sIconModelStrSoldout,
    sIconModelStrCoconut, sIconModelStrCoconut, sIconModelStrPaperbag, sIconModelStrMedicine,
    sIconModelStrDeliverL, sIconModelStrDeliverL, sIconModelStrPaperbag, sIconModelStrCracker,
    sIconModelStrPaperbag, sIconModelStrHanabi, sIconModelStrClover, sIconModelStrClover,
    sIconModelStrTane, sIconModelStrLeaf, sIconModelStrHoneycomb, sIconModelStrLeaf,
    sIconModelStrToolbox, sIconModelStrTimer,
};
char sIconModelStrToolbox[] = "toolbox";
char sIconModelStrHoneycomb[] = "honeycomb";
char sIconModelStrPaint01[] = "paint01";
char sIconModelStrPaint02[] = "paint02";
char sIconModelStrSeedling[] = "seedling";
char sIconModelStrNet[] = "net";
char sIconModelStrSeedlingC[] = "seedling_c";
char sIconModelStrHaniwa[] = "haniwa";
char sIconModelStrFossil[] = "fossil";
char sIconModelStrSango[] = "sango";
s32 sFlowerItemCount;
s32 sFullHeadwearClass1dCount;
char sIconModelStrCarpet[] = "carpet";
char sIconModelStrPaint10[] = "paint10";
char sIconModelStrPaint13[] = "paint13";
char sIconModelStrGfishingrod[] = "gfishingrod";
char sIconModelStrCoconut[] = "coconut";
char sIconModelStrDeliverL[] = "deliver_l";
s32 sHatClass1dCount;
char sIconModelStrPear[] = "pear";
char sIconModelStrRKabu[] = "r_kabu";
char sIconModelStrWatering[] = "watering";
char sIconModelStrGaxe[] = "gaxe";
ItemId sFirstHoldableItem(0xfff1);
char sIconModelStrPaint04[] = "paint04";
char sIconModelStrTimer[] = "timer";
char sIconModelStrPaint07[] = "paint07";
char sIconModelStrSoldout[] = "soldout";
char sIconModelStrGlasses[] = "glasses";
char sIconModelStrClover[] = "clover";
char sIconModelStrPaint00[] = "paint00";

static inline BOOL Unk_02061478_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u16 v = *p;
    if (v >= lo && v <= hi) r = TRUE;
    return r;
}
static inline BOOL Unk_02061478_V(u16 v, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (v >= lo && v <= hi) r = TRUE;
    return r;
}
static inline s32 Unk_02061478_I(u16 v, u32 lo, u32 hi) {
    if (v >= lo && v <= hi) return (s32)(v - lo) >> 2;
    return -1;
}
static inline u16 Unk_02061478_M(s32 i, u32 n, u32 base) {
    if ((u32)i < n) return i + base;
    return base;
}

static inline BOOL Unk_02061794_Eq(s32 a, s32 b) {
    BOOL r = FALSE;
    if (a == b) r = TRUE;
    return r;
}

static inline u16 Unk_020621d8_Idx(u32 i, u32 n, u32 base) {
    if (i < n) return base + i;
    return base;
}

static inline BOOL Unk_020622cc_IsFree(u16 *g, u16 *t) {
    if (Item_IsFurniture(g)) {
        *t = 0xfff1;
        s32 x = Item_GetFurnitureIndex(g);
        if (x == Item_GetFurnitureIndex(t)) return TRUE;
        return FALSE;
    }
    if (*g == 0xfff1) return TRUE;
    return FALSE;
}

static inline u8 *Unk_02061e0c_Lookup(u32 v) {
    u32 i = v & 0xfff;
#ifdef N3DS_DIRECT
    i = n3ds_item_record(i);  // (n3ds_options.h: a fruit stack's record is its fruit's)
#endif
    if (i >= 0x56e) i = 0x56d;
    return gItemInfo.getAlways()->getRecord(i);
}

static inline BOOL Unk_02061e0c_InRange(u16 *p) {
    BOOL r = FALSE;
    u16 v = *p;
    if (v >= 0x1000 && v <= 0x10ff) r = TRUE;
    return r;
}

static inline u16 Unk_02061e0c_Conv(u16 *p) {
    u16 v = *p;
    s32 t;
    if (v >= 0x1000 && v <= 0x10ff) t = v - 0x1000;
    else t = -1;
    t |= 3;
    if ((u32)t < 0x100) return t + 0x1000;
    return 0x1000;
}

static inline u8 *Unk_02062024_Lookup(u32 v) {
    u32 i = v & 0xfff;
#ifdef N3DS_DIRECT
    i = n3ds_item_record(i);  // (n3ds_options.h: a fruit stack's record is its fruit's)
#endif
    if (i >= 0x56e) i = 0x56d;
    return gItemInfo.getDma()->getRecord(i);
}

extern "C" u16 *ItemInfo_GetName(u16 *p);

static inline BOOL Unk_0206198c_Tail(u32 v, u32 sh) {
    u32 i = v & 0xfff;
#ifdef N3DS_DIRECT
    i = n3ds_item_record(i);  // (n3ds_options.h: a fruit stack's record is its fruit's)
#endif
    if (i >= 0x56e) i = 0x56d;
    u8 *r = gItemInfo.getAlways()->getRecord(i);
    if (r) return (r[9] >> sh) & 1 ? TRUE : FALSE;
    return FALSE;
}

BOOL EncodedString16Buf::copyTo(u8 *out, s32 n) {
    if (n >= (s32)capacity()) {
        MI_CpuCopy8(text, out, capacity());
        return TRUE;
    }
    return FALSE;
}

ItemInfoTables::ItemInfoTables() {}

// (~InfoTableSet closes the three tables once more: RecordFile::close is idempotent)
ItemInfoTables::~ItemInfoTables() {
    close();
    series.close();
}

s32 ItemInfoTables_Open(ItemInfoTables *o) {
    BOOL ok;
    sHoldableItemCount = 0;
    ok = TRUE;
    ok = (ok | o->open((void *)"/item_info/always.bin", 0xc, (void *)"/item_info/indoor.bin", 4, (void *)"/item_info/dma.bin", 0x14, 0x600)) ? TRUE : FALSE;
    if (ok) {
        ItemInfo_CountHoldable();
        ItemInfo_CountFlowerItems();
        ItemInfo_CountClass1dHeadwear();
    }
    ok = (ok | o->series.open((void *)"/item_info/series.bin", 0x14, 0x80)) ? TRUE : FALSE;
    if (ok) {
        o->series.loadAll();
    }
    return ok;
}

void ItemInfoTables_Close(ItemInfoTables *o) {
    o->close();
    o->series.close();
}

void ItemInfoTables_LoadIndoor(ItemInfoTables *p, s32 x) { p->loadIndoor(x); }

void ItemInfoTables_FreeIndoor(ItemInfoTables *p) { p->freeIndoor(); }

void ItemInfo_CountHoldable() {
    u16 i;
    struct { u16 a; u16 b; } l;
    for (i = 0x1000; i < 0x156e; i++) {
        l.a = i;
        if (ItemInfo_IsHoldable(&l.a)) {
            if (Unk_020622cc_IsFree(&sFirstHoldableItem.id, &l.b)) {
                sFirstHoldableItem.id = l.a;
            }
            sHoldableItemCount++;
        }
    }
}

void ItemInfo_CountFlowerItems() {
    u32 i;
    struct { u16 a; u16 b; } l;
    for (i = 0; i < 0x21; i++) {
        l.a = Unk_020621d8_Idx(i, 0x21, 0x1408);
        l.b = Unk_020621d8_Idx(i, 0x21, 0x1471);
        if (Item_IsFlowerItem(&l.a)) {
            sFlowerItemCount++;
        } else if (Item_IsFlowerAltItem(&l.b)) {
            sFlowerAltCount++;
        }
    }
}

void ItemInfo_CountClass1dHeadwear() {
    u32 i;
    struct { u16 a; u16 b; } l;
    sFullHeadwearClass1dCount = 0;
    sHatClass1dCount = 0;
    for (i = 0; i < 0x40; i++) {
        l.a = Unk_020621d8_Idx(i, 0x40, 0x13c8);
        if (ItemInfo_GetClass(&l.a) == 0x1d) sHatClass1dCount++;
    }
    for (i = 0; i < 0x20; i++) {
        l.b = Unk_020621d8_Idx(i, 0x20, 0x13a8);
        if (ItemInfo_GetClass(&l.b) == 0x1d) sFullHeadwearClass1dCount++;
    }
}

BOOL ItemInfo_IsReady() { return TRUE; }

s32 ItemInfo_Init() { return ItemInfoTables_Open(&gItemInfo); }

void ItemInfo_Exit() { ItemInfoTables_Close(&gItemInfo); }

void ItemInfo_LoadIndoor(s32 a) { ItemInfoTables_LoadIndoor(&gItemInfo, a); }

// ---------------------------------------------------------------------------------------------------------------------
void ItemInfo_FreeIndoor() { ItemInfoTables_FreeIndoor(&gItemInfo); }

u16 *ItemInfo_GetName(u16 *p) {
    if (Unk_02061e0c_InRange(p)) {
        volatile u16 tmp = Unk_02061e0c_Conv(p);
        u8 *r = Unk_02062024_Lookup(tmp);
#ifdef N3DS_DIRECT
        if (r && N3ds_LangEur()) r++;  // (the EUR record's form byte, n3ds_options.h)
#endif
        if (r) return (u16 *)(r + 2);
        return 0;
    }
    u8 *r = Unk_02062024_Lookup(*p);
#ifdef N3DS_DIRECT
    if (r && N3ds_LangEur()) r++;  // (the EUR record's form byte, n3ds_options.h)
#endif
    if (r) return (u16 *)(r + 2);
    return 0;
}

s32 ItemInfo_GetUnk02(u16 *p) {
    u8 *r = Unk_02061e0c_Lookup(*p);
    if (r) return *(u16 *)(r + 2);
    return 0;
}

#ifdef N3DS_DIRECT
s32 ItemInfo_GetNameForm(u16 *p) {  // (the EUR record's form byte, n3ds_options.h)
    u8 *r = N3ds_LangEur() ? Unk_02062024_Lookup(*p) : 0;
    return r ? (s8)r[2] : -1;
}
#else
s32 ItemInfo_GetNameForm(u16 *) { return -1; }
#endif

s32 ItemInfo_GetNameAttrB(u16 *p) {
    u8 *r = Unk_02062024_Lookup(*p);
    if (r) return r[0];
    return 0;
}

s32 ItemInfo_GetNameAttrA(u16 *p) {
    u8 *r = Unk_02062024_Lookup(*p);
    if (r) return r[1];
    return 0;
}

s32 ItemInfo_GetSeries(u16 *p) {
    u8 *r = Unk_02061e0c_Lookup(*p);
    if (r) return r[5];
    return 0;
}

u32 ItemInfo_GetPrice(u16 *p) {
    u32 rec;
    if (Unk_02061e0c_InRange(p)) {
        volatile u16 tmp = Unk_02061e0c_Conv(p);
        u8 *r = Unk_02061e0c_Lookup(tmp);
        if (r) rec = *(u16 *)r;
        else rec = 0;
    } else {
        u8 *r = Unk_02061e0c_Lookup(*p);
        if (r) rec = *(u16 *)r;
        else rec = 0;
    }
    if (Unk_02061e0c_InRange(p)) {
        u16 v = *p;
        s32 t;
        if (v >= 0x1000 && v <= 0x10ff) t = v - 0x1000;
        else t = -1;
        t &= 3;
        rec = (rec * (t + 1)) << 14 >> 16;
    }
    return rec;
}

s32 ItemInfo_GetKind(u16 *p) {
    u8 *r = Unk_02061e0c_Lookup(*p);
    if (r) return r[4];
    return 0;
}

s32 ItemInfo_GetClass(u16 *p) {
    if (Unk_02061e0c_InRange(p)) {
        volatile u16 tmp = Unk_02061e0c_Conv(p);
        u8 *r = Unk_02061e0c_Lookup(tmp);
        if (r) return r[6];
        return 0;
    }
    u8 *r = Unk_02061e0c_Lookup(*p);
    if (r) return r[6];
    return 0;
}

s32 ItemInfo_GetUnk07(u16 *p) {
    u32 i = *p & 0xfff;
#ifdef N3DS_DIRECT
    i = n3ds_item_record(i);  // (n3ds_options.h: a fruit stack's record is its fruit's)
#endif
    if (i >= 0x56e) i = 0x56d;
    u8 *r = gItemInfo.getAlways()->getRecord(i);
    if (r) return r[7];
    return 0;
}

s32 ItemInfo_GetSeason(u16 *p) {
    if (ItemInfo_GetKind(p) == 5) {
        u32 i = *p & 0xfff;
    #ifdef N3DS_DIRECT
    i = n3ds_item_record(i);  // (n3ds_options.h: a fruit stack's record is its fruit's)
#endif
    if (i >= 0x56e) i = 0x56d;
        u8 *r = gItemInfo.getAlways()->getRecord(i);
        if (r) return r[8];
        return 0;
    }
    return -1;
}

extern "C" BOOL ItemInfo_IsHoldable(u16 *p) {
    BOOL in = FALSE;
    u16 v = *p;
    if (v >= 0x1000 && v <= 0x10ff) in = TRUE;
    if (in) {
        s32 d;
        volatile u16 t;
        if (v >= 0x1000 && v <= 0x10ff) d = v - 0x1000;
        else d = -1;
        d |= 3;
        t = Unk_02061478_M(d, 0x100, 0x1000);
        return Unk_0206198c_Tail(t, 0);
    }
    return Unk_0206198c_Tail(v, 0);
}

extern "C" BOOL ItemInfo_TestFlag1(u16 *p) {
    BOOL in = FALSE;
    u16 v = *p;
    if (v >= 0x1000 && v <= 0x10ff) in = TRUE;
    if (in) {
        s32 d;
        volatile u16 t;
        if (v >= 0x1000 && v <= 0x10ff) d = v - 0x1000;
        else d = -1;
        d |= 3;
        t = Unk_02061478_M(d, 0x100, 0x1000);
        return Unk_0206198c_Tail(t, 1);
    }
    return Unk_0206198c_Tail(v, 1);
}

extern "C" BOOL ItemInfo_TestFlag2(u16 *p) {
    BOOL in = FALSE;
    u16 v = *p;
    if (v >= 0x1000 && v <= 0x10ff) in = TRUE;
    if (in) {
        s32 d;
        volatile u16 t;
        if (v >= 0x1000 && v <= 0x10ff) d = v - 0x1000;
        else d = -1;
        d |= 3;
        t = Unk_02061478_M(d, 0x100, 0x1000);
        return Unk_0206198c_Tail(t, 2);
    }
    return Unk_0206198c_Tail(v, 2);
}

extern "C" BOOL ItemInfo_TestFlag3(u16 *p) {
    BOOL in = FALSE;
    u16 v = *p;
    if (v >= 0x1000 && v <= 0x10ff) in = TRUE;
    if (in) {
        s32 d;
        volatile u16 t;
        if (v >= 0x1000 && v <= 0x10ff) d = v - 0x1000;
        else d = -1;
        d |= 3;
        t = Unk_02061478_M(d, 0x100, 0x1000);
        return Unk_0206198c_Tail(t, 3);
    }
    return Unk_0206198c_Tail(v, 3);
}

extern "C" BOOL ItemInfo_TestFlag4(u16 *p) {
    BOOL in = FALSE;
    u16 v = *p;
    if (v >= 0x1000 && v <= 0x10ff) in = TRUE;
    if (in) {
        s32 d;
        volatile u16 t;
        if (v >= 0x1000 && v <= 0x10ff) d = v - 0x1000;
        else d = -1;
        d |= 3;
        t = Unk_02061478_M(d, 0x100, 0x1000);
        return Unk_0206198c_Tail(t, 4);
    }
    return Unk_0206198c_Tail(v, 4);
}

extern "C" u32 ItemInfo_GetIndoorUnk1(u16 *p) {
    u32 i = *p & 0xfff;
#ifdef N3DS_DIRECT
    i = n3ds_item_record(i);  // (n3ds_options.h: a fruit stack's record is its fruit's)
#endif
    if (i >= 0x56e) i = 0x56d;
    u8 *r = gItemInfo.getIndoor()->getRecord(i);
    if (r) return r[1];
    return 0;
}

extern "C" u32 ItemInfo_GetIndoorUnk0(u16 *p) {
    u32 i = *p & 0xfff;
#ifdef N3DS_DIRECT
    i = n3ds_item_record(i);  // (n3ds_options.h: a fruit stack's record is its fruit's)
#endif
    if (i >= 0x56e) i = 0x56d;
    u8 *r = gItemInfo.getIndoor()->getRecord(i);
    if (r) return r[0];
    return 0;
}

extern "C" u32 Series_GetType(s32 i) {
    if (i < 0x4a) {
        u8 *r = gItemInfo.series.getRecord(i);
        if (r) return *r;
    }
    return 3;
}

extern "C" u8 *Series_GetName(s32 i) {
    if (i < 0x4a) {
        u8 *r = gItemInfo.series.getRecord(i);
        if (r) return r + 1;
    }
    return NULL;
}

extern "C" u32 FtrClass_GetBasePoints(s32 i) {
    if (i < 0x2e) return sFtrClassBasePoints[i];
    return 0;
}

extern "C" const char *Item_GetIconModelName(s32 a, s32 b) {
    if (a < 0x49) {
        u32 c = b == 0 ? 1 : 0;
        return sItemIconModelNames[a * 2 + (u8)c];
    }
    return Item_GetIconModelName(0, b);
}

extern "C" u32 ItemInfo_GetHoldableCount() { return sHoldableItemCount; }

extern "C" void ItemInfo_GetNthHoldable(u16 *out, s32 idx) {
    if (sHoldableItemCount) {
        s32 cnt = 0;
        u16 i;
        for (i = sFirstHoldableItem.id; i < 0x156e; i++) {
            u16 buf;
            buf = i;
            if (ItemInfo_IsHoldable(&buf)) {
                if (cnt == idx) {
                    *out = buf;
                    return;
                }
                cnt++;
            }
        }
    }
    *out = sFirstHoldableItem.id;
}

extern "C" s32 ItemInfo_GetHoldableIndex(u16 *p) {
    if (sHoldableItemCount) {
        s32 cnt = 0;
        u16 i;
        BOOL a = FALSE, b = FALSE;
        for (i = sFirstHoldableItem.id; i < 0x156e; i++) {
            u16 buf;
            buf = i;
            if (ItemInfo_IsHoldable(&buf)) {
                BOOL f;
                if (Item_IsFurniture(&buf)) {
                    s32 x = Item_GetFurnitureIndex(&buf);
                    f = (x == Item_GetFurnitureIndex(p)) ? TRUE : a;
                } else {
                    f = (buf == *p) ? TRUE : b;
                }
                if (f) return cnt;
                cnt++;
            }
        }
    }
    return -1;
}

extern "C" u32 Item_GetFlowerItemCount() { return sFlowerItemCount; }

extern "C" u32 Item_GetFlowerAltCount() { return sFlowerAltCount; }

extern "C" u32 ItemInfo_GetHatClass1dCount() { return sHatClass1dCount; }

extern "C" u32 ItemInfo_GetFullHeadwearClass1dCount() { return sFullHeadwearClass1dCount; }

// ---------------------------------------------------------------------------------------------------------------------
extern "C" void Item_FromPlacedForm(u16 *out, u16 *in) {
    u32 o; s32 k; u16 t[2];
    if (Unk_02061478_R(in, 0x3984, 0x3d83)) {
        *out = Unk_02061478_M(Unk_02061478_I(*in, 0x3984, 0x3d83), 0x100, 0x11a8);
    } else if (*in >= 0x42a4 && *in <= 0x4383) {
        *out = Unk_02061478_M(Unk_02061478_I(*in, 0x42a4, 0x4383), 0x38, 0x12b0);
    } else if (*in >= 0x4384 && *in <= 0x4463) {
        *out = Unk_02061478_M(Unk_02061478_I(*in, 0x4384, 0x4463), 0x38, 0x12e8);
    } else if (*in >= 0x3e24 && *in <= 0x3ea3) {
        *out = Unk_02061478_M(Unk_02061478_I(*in, 0x3e24, 0x3ea3), 0x20, 0x1380);
    } else if (*in >= 0x3fa4 && *in <= 0x40a3) {
        *out = Unk_02061478_M(Unk_02061478_I(*in, 0x3fa4, 0x40a3), 0x40, 0x13c8);
    } else if (*in >= 0x40a4 && *in <= 0x4123) {
        *out = Unk_02061478_M(Unk_02061478_I(*in, 0x40a4, 0x4123), 0x20, 0x13a8);
    } else if (*in >= 0x4124 && *in <= 0x4223) {
        *out = Unk_02061478_M(Unk_02061478_I(*in, 0x4124, 0x4223), 0x40, 0x1431);
    } else if (*in >= 0x44e8 && *in <= 0x450b) {
        *out = Unk_02061478_M(Unk_02061478_I(*in, 0x44e8, 0x450b), 9, 0x1554);
    } else {
        k = -1;
        if (*in >= 0x4464 && *in <= 0x44e7) k = Unk_02061478_I(*in, 0x4464, 0x44e7);
        if (Unk_02061478_V(*in, 0, 0x20)) k = *in;
        if (k != -1) {
            t[0] = Unk_02061478_M(k, 0x21, 0x1408);
            if (Item_IsFlowerItem(&t[0])) {
                *out = t[0];
            } else {
                t[1] = Unk_02061478_M(k, 0x21, 0x1471);
                if (Item_IsFlowerAltItem(&t[1])) *out = t[1];
                else *out = 0x137c;
            }
        } else if ((*in >= 0xd4 && *in <= 0xda) || (*in >= 0xdb && *in <= 0xe1)) {
            if (*in >= 0xd4 && *in <= 0xda) o = *in - 0xd4;
            else o = *in - 0xdb;
            *out = Unk_02061478_M(o, 7, 0x153b);
        } else {
            *out = *in;
        }
    }
}


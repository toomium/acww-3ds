#include "types.h"
#include "n3ds_options.h"
#include "item/ItemIconCache.h"
#include "item/ItemName.h"
#include "menu/InventoryGridTypes.h"
#include "menu/InventoryItemGrid.h"
#include "gfx/BgVramTask.h"
#include "player/PlayerData.h"
#include "player/PlayerInventory.h"
#include "talk/LabelBalloonText.h"
#include "talk/MsgString9B.h"
#include "ui/LabelBalloon.h"
#include "ui/LabelString.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_020501d4.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02063f3c.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_0206f834.h"
#include "proto/main/unk_02087e0c.h"
#include "proto/main/unk_02087e70.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_02098e90.h"
#include "proto/main/unk_020b2c50.h"
#include "proto/ov094/unk_ov094_02292360.h"
#include "proto/ov094/unk_ov094_02292da8.h"

extern "C" {
s32 Item_GetInfoUnk02(u16 *p);  // src/main/unk_0204a780.cpp (not in the headers)
void MI_CpuCopy8(const void *src, void *dst, u32 n);  // SDK assembly
}

#define data_ov094_02294b80 "menu/icon/pre%d.bch"
#define data_ov094_02294b94 "menu/icon/icon%02d.bch"

extern "C" {

OamObjTemplate sHeldItemSprite = {{0x00, 0x00, 0x00, 0x40, 0x1c, 0x61, 0xff, 0xff}};
extern "C" const u8 sPresentAnimScales[8] = {0x08, 0x04, 0x00, 0x03, 0x07, 0x0a, 0x00, 0x00};
extern "C" const u8 sPresentAnimFrames[8] = {0x00, 0xff, 0x01, 0x02, 0x03, 0xff, 0x00, 0x00};
extern "C" const u32 sItemCursorLift[3] = {0xfffffffe, 0xfffffffd, 0xfffffffb};
extern "C" const s32 sItemScaleTable[10] = {0x0, 0xa000, 0x5000, 0x3555, 0x2800, 0x2000, 0x1aaa, 0x16db, 0x1400, 0x11c7};
// [0]: the underlay of an item, [1]: the focus frame (two objects), [2]: the mark
OamObjTemplate sItemGridMarkSprites[3] = {{{0x00, 0x00, 0x00, 0x80, 0x44, 0x51, 0x00, 0x00}}, {{0x02, 0x00, 0x02, 0x80, 0x44, 0x11, 0xff, 0xff}}, {{0x00, 0x00, 0x00, 0x80, 0x4c, 0x61, 0xff, 0xff}}};
extern "C" const u8 sItemIconPalettes[336] = {0x07, 0x0c, 0x07, 0x0c, 0x08, 0x07, 0x07, 0x07, 0x07, 0x09, 0x0a, 0x0a, 0x08, 0x07, 0x08, 0x07, 0x07, 0x07, 0x08, 0x09, 0x08, 0x07, 0x08, 0x07, 0x08, 0x0c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0a, 0x08, 0x08, 0x0d, 0x0b, 0x07, 0x0c, 0x07, 0x0c, 0x0c, 0x0c, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x09, 0x0c, 0x0c, 0x09, 0x09, 0x0c, 0x0c, 0x0c, 0x09, 0x0c, 0x0c, 0x0b, 0x0c, 0x0c, 0x08, 0x0c, 0x08, 0x08, 0x08, 0x0c, 0x08, 0x07, 0x09, 0x07, 0x0a, 0x0c, 0x0d, 0x0c, 0x0c, 0x0a, 0x0c, 0x08, 0x0a, 0x0a, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x07, 0x07, 0x07, 0x08, 0x0c, 0x0d, 0x0d, 0x0c, 0x0c, 0x0a, 0x07, 0x0c, 0x0c, 0x0b, 0x0c, 0x0c, 0x0c, 0x07, 0x0c, 0x0d, 0x0b, 0x07, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0b, 0x0c, 0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x09, 0x0c, 0x09, 0x0d, 0x0c, 0x0c, 0x0c, 0x0a, 0x0d, 0x0d, 0x0c, 0x0a, 0x07, 0x0c, 0x0c, 0x0c, 0x0c, 0x0d, 0x0a, 0x0d, 0x0d, 0x0c, 0x0d, 0x0d, 0x0c, 0x0d, 0x0c, 0x09, 0x0b, 0x08, 0x0c, 0x0d, 0x0a, 0x0a, 0x0c, 0x0d, 0x0d, 0x0b, 0x0a, 0x08, 0x08, 0x0c, 0x09, 0x0c, 0x0d, 0x0a, 0x0c, 0x07, 0x09, 0x0a, 0x0c, 0x09, 0x09, 0x0c, 0x0c, 0x0c, 0x0d, 0x0a, 0x0c, 0x08, 0x09, 0x08, 0x09, 0x08, 0x08, 0x0a, 0x08, 0x08, 0x0a, 0x0a, 0x08, 0x0a, 0x09, 0x08, 0x09, 0x0a, 0x0c, 0x08, 0x09, 0x0a, 0x08, 0x09, 0x0a, 0x08, 0x07, 0x08, 0x07, 0x07, 0x09, 0x0a, 0x08};
OamObjTemplate sItemGridSlotSprites[34] = {{{0x14, 0x00, 0x8c, 0x41, 0xc0, 0x70, 0x00, 0x00}}, {{0x14, 0x00, 0xac, 0x41, 0xc2, 0x70, 0x00, 0x00}}, {{0x14, 0x00, 0xcc, 0x41, 0xc4, 0x70, 0x00, 0x00}}, {{0x14, 0x00, 0xec, 0x41, 0xc6, 0x70, 0x00, 0x00}}, {{0x14, 0x00, 0x0c, 0x40, 0xc8, 0x70, 0x00, 0x00}}, {{0x2c, 0x00, 0x9c, 0x41, 0xca, 0x70, 0x00, 0x00}}, {{0x2c, 0x00, 0xbc, 0x41, 0xcc, 0x70, 0x00, 0x00}}, {{0x2c, 0x00, 0xdc, 0x41, 0xce, 0x70, 0x00, 0x00}}, {{0x2c, 0x00, 0xfc, 0x41, 0xd0, 0x70, 0x00, 0x00}}, {{0x2c, 0x00, 0x1c, 0x40, 0xd2, 0x70, 0x00, 0x00}}, {{0x44, 0x00, 0xac, 0x41, 0xd4, 0x70, 0x00, 0x00}}, {{0x44, 0x00, 0xcc, 0x41, 0xd6, 0x70, 0x00, 0x00}}, {{0x44, 0x00, 0xec, 0x41, 0xd8, 0x70, 0x00, 0x00}}, {{0x44, 0x00, 0x0c, 0x40, 0xda, 0x70, 0x00, 0x00}}, {{0x44, 0x00, 0x2c, 0x40, 0xdc, 0x70, 0x00, 0x00}}, {{0xb4, 0x00, 0x8c, 0x41, 0xde, 0x70, 0x00, 0x00}}, {{0xb4, 0x00, 0xac, 0x41, 0x00, 0x71, 0x00, 0x00}}, {{0xb4, 0x00, 0xcc, 0x41, 0x02, 0x71, 0x00, 0x00}}, {{0xb4, 0x00, 0xec, 0x41, 0x04, 0x71, 0x00, 0x00}}, {{0xb4, 0x00, 0x0c, 0x40, 0x06, 0x71, 0x00, 0x00}}, {{0xcc, 0x00, 0x9c, 0x41, 0x08, 0x71, 0x00, 0x00}}, {{0xcc, 0x00, 0xbc, 0x41, 0x0a, 0x71, 0x00, 0x00}}, {{0xcc, 0x00, 0xdc, 0x41, 0x0c, 0x71, 0x00, 0x00}}, {{0xcc, 0x00, 0xfc, 0x41, 0x0e, 0x71, 0x00, 0x00}}, {{0xcc, 0x00, 0x1c, 0x40, 0x10, 0x71, 0x00, 0x00}}, {{0xe4, 0x00, 0xac, 0x41, 0x12, 0x71, 0x00, 0x00}}, {{0xe4, 0x00, 0xcc, 0x41, 0x14, 0x71, 0x00, 0x00}}, {{0xe4, 0x00, 0xec, 0x41, 0x16, 0x71, 0x00, 0x00}}, {{0xe4, 0x00, 0x0c, 0x40, 0x18, 0x71, 0x00, 0x00}}, {{0xe4, 0x00, 0x2c, 0x40, 0x1a, 0x71, 0x00, 0x00}}, {{0xef, 0x00, 0xf7, 0x41, 0x1e, 0x71, 0x00, 0x00}}, {{0xef, 0x00, 0x2a, 0x40, 0x1e, 0x71, 0x00, 0x00}}, {{0xd8, 0x00, 0x10, 0x40, 0x1e, 0x71, 0x00, 0x00}}, {{0xf0, 0x00, 0xa0, 0x41, 0x1e, 0x61, 0xff, 0xff}}};

ItemIconCache::ItemIconCache() {}

ItemIconCache::~ItemIconCache() {}

void ItemIconCache::invalidate()
{
    loadedPage = 0xff;
}

u8 *ItemIconCache::getIconChars(s32 idx)
{
    char buf[0x28];
    s32 page = idx >> 4;
    if (page != loadedPage) {
        loadedPage = page;
        Str_SPrintf(buf, data_ov094_02294b94, page);
        File_LoadToBuffer(buf, iconChars, 0x800);
    }
    u8 *r = iconChars;
    r += Menu_GetIconCharIndex(idx & 0xf) << 5;
    return r;
}

u8 *ItemIconCache::getPresentChars(s32 idx, s32 variant)
{
    char buf[0x28];
    Str_SPrintf(buf, data_ov094_02294b80, variant);
    File_LoadToBuffer(buf, iconChars, 0x800);
    u8 *r = iconChars;
    r += Menu_GetIconCharIndex(idx) << 5;
    loadedPage = 0xff;
    return r;
}

u8 InventoryItemGrid_GetIconPalette(void *o, s32 i)
{
    return sItemIconPalettes[i];
}

InventoryItemGrid::InventoryItemGrid()
{
}

InventoryItemGrid::~InventoryItemGrid() {}

void InventoryItemGrid_Init(InventoryItemGrid *o, u32 a)
{
    o->iconCache.invalidate();
    InventoryItemGrid_ClearBits(o->occupiedBits);
    InventoryItemGrid_ClearBits(o->markedBits);
    InventoryItemGrid_ClearBits(o->disabledBits);
    o->cursorSlot = 0x23;
    o->cursorLiftTimer = 0;
    o->objPriority = a;
    o->boxItems = 0;
    o->heldScale = 0xa;
    o->showHeldFocus = 1;
}

void InventoryItemGrid_PreUpdate(InventoryItemGrid *o)
{
    InventoryItemGrid_CancelUploads(o);
    u8 v = o->cursorLiftTimer;
    if (v != 0) {
        o->cursorLiftTimer = v - 1;
    }
}

void InventoryItemGrid_Exit(InventoryItemGrid *o)
{
    InventoryItemGrid_CancelUploads(o);
}

u32 InventoryItemGrid_FindPocketSlotAt(InventoryItemGrid *o, s32 a, s32 b)
{
    if (b < 0x68) {
        return 0x23;
    }
    return InventoryItemGrid_FindSlotInRange(o, a - 0x94, b - 0x74, a - 0x7c, b - 0x5c, 0, 0xe);
}

u32 InventoryItemGrid_FindBoxSlotAt(InventoryItemGrid *o, s32 a, s32 b)
{
    if (b > 0x68) {
        return 0x23;
    }
    return InventoryItemGrid_FindSlotInRange(o, a - 0x94, b - 0x74, a - 0x7c, b - 0x5c, 0xf, 0x1d);
}

s32 InventoryItemGrid_HitTestSlot21(InventoryItemGrid *o, s32 a, s32 b)
{
    return InventoryItemGrid_HitTestSlot(o, a, b, 0x21);
}

u32 InventoryItemGrid_FindSlotInRange(InventoryItemGrid *o, s32 a, s32 b, s32 c, s32 d, u8 s, u8 e)
{
    s32 i = s;
    for (; i <= e; i++) {
        OamObjTemplate *p = InventoryItemGrid_GetSlotSprite(o, i);
        s32 x = Oam_GetObjX(p);
        if (a < x && x < c) {
            s32 y = Oam_GetObjY(p);
            if (b < y && y < d) {
                return (u8)i;
            }
        }
    }
    return 0x23;
}

s32 InventoryItemGrid_HitTestSlot(InventoryItemGrid *o, s32 a, s32 b, s32 c)
{
    s32 a1 = a - 0x7c;
    s32 b1 = b - 0x74;
    s32 b2 = b - 0x5c;
    OamObjTemplate *e = InventoryItemGrid_GetSlotSprite(o, c);
    s32 x = Oam_GetObjX(e);
    if (a - 0x94 < x && x < a1) {
        s32 y = Oam_GetObjY(e);
        if (b1 < y && y < b2) {
            return TRUE;
        }
    }
    return FALSE;
}

#ifdef N3DS_DIRECT
// Stacked fruit (n3ds_options.h): a stack's icon with its count in the bottom right corner, a light 3x5 digit in a dark
// frame. The icons' palettes (menu/icon/b_obj_itm.bpl, OBJ palettes 7-14) all have their dark outline at 0xf and a
// light grey at 0xd (palette 7: white at 9). src as the icon pages hold it: the top tiles at src, the bottom at +0x400.
static u8 sStackIcon[0x440];
static u8 *InventoryItemGrid_StackIcon(u8 *src, u32 item, u32 pal)
{
    // the digits' rows (3 bits each, the left pixel the top bit), from the top
    static const u8 digits[10][5] = {{7, 5, 5, 5, 7}, {2, 6, 2, 2, 7}, {7, 1, 7, 4, 7}, {7, 1, 7, 1, 7}, {5, 5, 7, 1, 1},
                                     {7, 4, 7, 1, 7}, {7, 4, 7, 5, 7}, {7, 1, 1, 2, 2}, {7, 5, 7, 5, 7}, {7, 5, 7, 1, 7}};
    s32 n = n3ds_fruit_count(item);
    if (n < 2) return src;
    MI_CpuCopy8(src, sStackIcon, 0x40);
    MI_CpuCopy8(src + 0x400, sStackIcon + 0x400, 0x40);
    const u8 *d = digits[n % 10];
    u32 light = pal == 7 ? 9 : 0xd;
    for (s32 y = 9; y < 16; y++) {      // the digit at x 12-14, y 10-14; its frame one pixel around it
        for (s32 x = 11; x < 16; x++) {
            BOOL on = x >= 12 && x <= 14 && y >= 10 && y <= 14 && (d[y - 10] >> (14 - x) & 1);
            u32 c = on ? light : 0xf;
            u8 *b = sStackIcon + (y >> 3) * 0x400 + (x >> 3) * 0x20 + (y & 7) * 4 + (x & 7) / 2;
            *b = (x & 1) ? (u8)((*b & 0x0f) | c << 4) : (u8)((*b & 0xf0) | c);
        }
    }
    return sStackIcon;
}
#endif

void InventoryItemGrid_LoadSlotIcon(InventoryItemGrid *o, s32 k, u16 *p, s32 a)
{
    if (*p == 0xfff1) {
        InventoryItemGrid_ClearBit(o->occupiedBits, k);
    } else {
        InventoryItemGrid_SetBit(o->occupiedBits, k);
        s32 r = InventoryItemGrid_GetIconIndex(o, p, a);
        OamObjTemplate *e = InventoryItemGrid_GetSlotSprite(o, k);
        s32 c = e->charName();
        u8 *q = o->iconCache.getIconChars(r);
#ifdef N3DS_DIRECT
        q = InventoryItemGrid_StackIcon(q, *p, InventoryItemGrid_GetIconPalette(&o->iconCache, r));
#endif
        Gfx2d_LoadCharRange(q, 8, c, c, c + 1);
        Gfx2d_LoadCharRange(q + 0x400, 8, c + 0x20, c + 0x20, c + 0x21);
        u32 n = InventoryItemGrid_GetIconPalette(&o->iconCache, r);
        e->setPalette(n);
    }
}

void InventoryItemGrid_LoadPockets(InventoryItemGrid *o)
{
    PlayerInventory *h = (PlayerInventory *)PlayerData_GetCurrent()->getInventory();
    u16 *base = h->getPocket(0);
    s32 i;
    s32 k;
    k = 0;
    i = 0;
    do {
        InventoryItemGrid_LoadSlotIcon(o, k, base + i, h->getPocketFlags(i));
        k++;
        i++;
    } while (i < 0xf);
}

void InventoryItemGrid_LoadBox(InventoryItemGrid *o, u16 *arr)
{
    s32 i;
    s32 k;
    s32 z = 0;
    k = 0xf;
    i = 0;
    do {
        u16 v = arr[i];
        InventoryItemGrid_LoadSlotIcon(o, k, &v, z);
        k++;
        i++;
    } while (i < 0xf);
    o->boxItems = arr;
}

s32 InventoryItemGrid_GetIconIndex(InventoryItemGrid *o, u16 *p, s32 mode)
{
    switch (mode) {
    case 1:
        return 0xb3;
    case 2:
        if (InvItem_IsDeliveryItem(*p)) {
            return Item_GetInfoUnk02(p);
        }
        return 0xb4;
    default:
        return Item_GetInfoUnk02(p);
    }
}

void InventoryItemGrid_SetBalloonItemName(InventoryItemGrid *o, LabelBalloon *dst, u16 *p, s32 mode)
{
    LabelString text;
    LabelBalloonText balloonText;
    ItemName itemName;
    MsgString9B recipient;
    switch (mode) {
    case 1:
        String_Load2dMenu(&text, 0x18);
        StrBuf_Copy((StrBuf *)&balloonText, (StrBuf *)&text);
        break;
    case 2:
        if (InvItem_IsDeliveryParcel(*p)) {
            itemName.setFromItem(p);
            StrBuf_Copy((StrBuf *)&balloonText, (StrBuf *)&itemName);
        } else {
            Player_GetDeliveryRecipientName(&recipient, p);
            String_SetSlot(0, &recipient);
            String_Load2dMenu(&text, 0x40);
            StrBuf_Copy((StrBuf *)&balloonText, (StrBuf *)&text);
        }
        break;
    case 0:
    default:
        itemName.setFromItem(p);
#ifdef N3DS_DIRECT
        if (n3ds_fruit_count(*p) > 1) {  // a fruit stack (n3ds_options.h): "apple x3"
            u8 n[4] = {' ', 'x', (u8)('0' + n3ds_fruit_count(*p)), 0};
            itemName.append(n);
        }
#endif
        StrBuf_Copy((StrBuf *)&balloonText, (StrBuf *)&itemName);
        break;
    }
    dst->setText((StrBuf *)&balloonText);  // StrBuf: MsgStringBase's interface under its other name
}

void InventoryItemGrid_ShowSlotName(InventoryItemGrid *o, LabelBalloon *dst, s32 b)
{
    u16 item = InventoryItemGrid_GetSlotItem(o, b);
    if (item != 0xfff1) {
        InventoryItemGrid_SetBalloonItemName(o, dst, &item, InventoryItemGrid_GetSlotFlags(o, b));
    }
}

s32 InventoryItemGrid_GetSlotX(InventoryItemGrid *o, s32 i)
{
    return Oam_GetObjX(InventoryItemGrid_GetSlotSprite(o, i)) + 0x80;
}

s32 InventoryItemGrid_GetSlotY(InventoryItemGrid *o, s32 i)
{
    return Oam_GetObjY(InventoryItemGrid_GetSlotSprite(o, i)) + 0x60;
}

BOOL InventoryItemGrid_IsCursorSlot(InventoryItemGrid *o, s32 v)
{
    if (v == o->cursorSlot) {
        return TRUE;
    }
    return FALSE;
}

s32 InventoryItemGrid_GetCursorLift(InventoryItemGrid *o)
{
    return sItemCursorLift[o->cursorLiftTimer];
}

void InventoryItemGrid_ClearCursorSlot(InventoryItemGrid *o)
{
    o->cursorSlot = 0x23;
}

void InventoryItemGrid_SetCursorSlot(InventoryItemGrid *o, u32 v)
{
    if (InventoryItemGrid_IsSlotDisabled(o, (u8)v)) {
        InventoryItemGrid_ClearCursorSlot(o);
    } else if (o->cursorSlot != v) {
        o->cursorSlot = v;
        o->cursorLiftTimer = 2;
    }
}

void InventoryItemGrid_ClearMarks(InventoryItemGrid *s) {
    InventoryItemGrid_ClearBits(s->markedBits);
}

void InventoryItemGrid_MarkSlot(InventoryItemGrid *s, s32 i) {
    InventoryItemGrid_SetBit(s->markedBits, i);
}

u16 InventoryItemGrid_GetSlotItem(InventoryItemGrid *s, s32 i) {
    PlayerInventory *t = (PlayerInventory *)PlayerData_GetCurrent()->getInventory();
    if (i >= 0 && i <= 0xe) {
        return t->getPocket(0)[i];
    }
    if (i >= 0xf && i <= 0x1d) {
        u16 *p = s->boxItems;
        if (p) {
            return p[i - 0xf];
        }
    }
    return 0xfff1;
}

u32 InventoryItemGrid_GetSlotFlags(InventoryItemGrid *s, s32 i) {
    if (i >= 0 && i <= 0xe) {
        return (u8)((PlayerInventory *)PlayerData_GetCurrent()->getInventory())->getPocketFlags(i);
    }
    return 0;
}

void InventoryItemGrid_ClearSlot(InventoryItemGrid *s, s32 i) {
    InventoryItemGrid_SetSlotItem(s, i, 0xfff1, 0);
    InventoryItemGrid_ClearBit(s->occupiedBits, i);
}

void InventoryItemGrid_SetSlotItem(InventoryItemGrid *s, s32 i, u32 v, s32 x) {
    u16 w = v;
    if (i >= 0 && i <= 0xe) {
        Pocket_SetItem(&w, x, i);
    } else if (i >= 0xf && i <= 0x1d) {
        s->boxItems[i - 0xf] = v;
    }
}

void InventoryItemGrid_RefreshSlot(InventoryItemGrid *s, s32 i) {
    OamObjTemplate *e = InventoryItemGrid_GetSlotSprite(s, i);
    u16 v = InventoryItemGrid_GetSlotItem(s, i);
    u32 c = InventoryItemGrid_GetSlotFlags(s, i);
    if (v == 0xfff1) {
        InventoryItemGrid_ClearBit(s->occupiedBits, i);
    } else {
        InventoryItemGrid_SetBit(s->occupiedBits, i);
        InventoryItemGrid_SetSpriteItem(s, e, v, c);
    }
}

void InventoryItemGrid_SetHeldItem(InventoryItemGrid *s, u32 item, s32 m) {
    InventoryItemGrid_SetSpriteItem(s, &sHeldItemSprite, item, m);
}

void InventoryItemGrid_SetSpriteItem(InventoryItemGrid *s, OamObjTemplate *r, u32 v, s32 m) {
    u16 w = v;
    s32 idx = InventoryItemGrid_GetIconIndex(s, &w, m);
    u8 *d = s->iconCache.getIconChars(idx);
    u32 c = InventoryItemGrid_GetIconPalette(&s->iconCache, idx);
#ifdef N3DS_DIRECT
    d = InventoryItemGrid_StackIcon(d, w, c);
#endif
    InventoryItemGrid_UploadIcon(s, r, d, c);
}

void InventoryItemGrid_UploadIcon(InventoryItemGrid *s, OamObjTemplate *r, u8 *dst, s32 c) {
    u32 t = r->charName();
    u32 k = InventoryItemGrid_AllocUpload(s);
    ItemIconUploadChars *e = &s->iconUploadChars[k];
    u8 *p = e->top;
    u8 *q = e->bottom;
    MI_CpuCopy8(dst, p, 0x40);
    MI_CpuCopy8(dst + 0x400, q, 0x40);
    s->iconUploadTasks[k].requestCharPair(p, q, 8, t, t + 1, t + 0x20, t + 0x21);
    r->setPalette(c);
}

BOOL InventoryItemGrid_IsSlotDisabled(InventoryItemGrid *s, s32 i) {
    return InventoryItemGrid_TestBit(s->disabledBits, i);
}

void InventoryItemGrid_DisableSlotRange(InventoryItemGrid *s, u8 i, u8 e) {
    while (i <= e) {
        InventoryItemGrid_DisableSlot(s, i);
        i++;
    }
}

void InventoryItemGrid_DisableSlot(InventoryItemGrid *s, s32 i) {
    InventoryItemGrid_SetBit(s->disabledBits, i);
}

void InventoryItemGrid_DrawPockets(InventoryItemGrid *s, s32 x, s32 y) {
    OamObjTemplate *e = InventoryItemGrid_GetSlotSprite(s, 0);
    s32 i;
    for (i = 0; i <= 0xe; e++, i++) {
        InventoryItemGrid_DrawSlot(s, e, i, x + 0x80, y + 0x60);
    }
}

void InventoryItemGrid_DrawPocketsClipped(InventoryItemGrid *s, s32 x, s32 y, s32 w) {
    OamObjTemplate *e = InventoryItemGrid_GetSlotSprite(s, 0);
    s32 i;
    for (i = 0; i <= 0xe; e++, i++) {
        if (InventoryItemGrid_GetSlotX(s, (u8)i) + 0x18 > w) {
            InventoryItemGrid_DrawSlot(s, e, i, x + 0x80, y + 0x60);
        }
    }
}

void InventoryItemGrid_DrawBox(InventoryItemGrid *s, s32 x, s32 y) {
    OamObjTemplate *e = InventoryItemGrid_GetSlotSprite(s, 0xf);
    s32 i;
    for (i = 0xf; i <= 0x1d; e++, i++) {
        InventoryItemGrid_DrawSlot(s, e, i, x + 0x80, y + 0x60);
    }
}

void InventoryItemGrid_DrawExtraMarks(InventoryItemGrid *s, s32 x, s32 y) {
    if (InventoryItemGrid_TestBit(s->markedBits, 0x21)) {
        OamObjTemplate *e = InventoryItemGrid_GetSlotSprite(s, 0x21);
        s32 a = x + Oam_GetObjX(e) + 0x80;
        s32 b = y + Oam_GetObjY(e) + 0x60;
        InventoryItemGrid_DrawMark(s, a, b);
    }
    if (InventoryItemGrid_TestBit(s->markedBits, 0x22)) {
        InventoryItemGrid_DrawMark(s, x + 4, y + 0xac);
    }
}

void InventoryItemGrid_DrawHeldItem(InventoryItemGrid *s, s32 x, s32 y) {
    s32 a = x + Oam_GetObjX(&sHeldItemSprite);
    s32 b = y + Oam_GetObjY(&sHeldItemSprite);
    u32 v = InventoryItemGrid_GetScale(s->heldScale);
    if (v != 0) {
        if (v == 0x1000) {
            Oam_DrawObj(1, &sHeldItemSprite, x, y, -1, s->objPriority, NULL);
        } else {
            OamAffineMtx l;
            l.v[0] = v;
            l.v[1] = 0;
            l.v[2] = 0;
            l.v[3] = v;
            Oam_DrawObj(1, &sHeldItemSprite, x, y, -1, s->objPriority, l.v);
        }
    }
    InventoryItemGrid_DrawUnderlay(s, a, b, -1);
    if (s->showHeldFocus) {
        InventoryItemGrid_DrawFocus(s, a, b);
    }
}

BOOL InventoryItemGrid_IsSlotEmpty(InventoryItemGrid *s, s32 i) {
    BOOL r;
    if (InventoryItemGrid_TestBit(s->occupiedBits, i)) {
        r = FALSE;
    } else {
        r = TRUE;
    }
    return r;
}

void InventoryItemGrid_DrawMark(InventoryItemGrid *s, s32 a, s32 b) {
    Oam_DrawObj(1, &sItemGridMarkSprites[2], a - 8, b - 8, -1, s->objPriority, NULL);
}

void InventoryItemGrid_DrawUnderlay(InventoryItemGrid *s, s32 a, s32 b, s32 c) {
    Oam_DrawObj(1, &sItemGridMarkSprites[0], a - 8, b - 8, c, s->objPriority, NULL);
}

void InventoryItemGrid_DrawFocus(InventoryItemGrid *s, s32 a, s32 b) {
    Oam_DrawObj(1, &sItemGridMarkSprites[1], a - 8, b - 8, -1, s->objPriority, NULL);
}

void InventoryItemGrid_DrawSlot(InventoryItemGrid *s, OamObjTemplate *e, s32 idx, s32 x, s32 y) {
    s32 a = x + Oam_GetObjX(e);
    s32 b = y + Oam_GetObjY(e);
    s32 off = 0;
    if (InventoryItemGrid_IsCursorSlot(s, idx)) {
        off = InventoryItemGrid_GetCursorLift(s);
        a += off;
        b += off;
    }
    if (InventoryItemGrid_TestBit(s->markedBits, idx)) {
        InventoryItemGrid_DrawMark(s, a, b);
    }
    if (InventoryItemGrid_TestBit(s->occupiedBits, idx)) {
        s32 c = -1;
        if (InventoryItemGrid_IsSlotDisabled(s, (u8)idx)) {
            c = 0xe;
        }
        Oam_DrawObj(1, e, x + off, y + off, c, s->objPriority, NULL);
        InventoryItemGrid_DrawUnderlay(s, a, b, c);
        if (InventoryItemGrid_IsCursorSlot(s, idx)) {
            InventoryItemGrid_DrawFocus(s, a, b);
        }
    }
}

OamObjTemplate *InventoryItemGrid_GetSlotSprite(InventoryItemGrid *s, s32 i) {
    return &sItemGridSlotSprites[i];
}

u32 InventoryItemGrid_GetScale(u32 i) {
    if (i >= 10) {
        return 0x1000;
    }
    return sItemScaleTable[i];
}

void InventoryItemGrid_CancelUploads(InventoryItemGrid *s) {
    s32 i;
    for (i = 0; i < 3; i++) {
        s->iconUploadTasks[i].cancel();
    }
    s->numIconUploads = 0;
}

u32 InventoryItemGrid_AllocUpload(InventoryItemGrid *s) {
    u32 v = s->numIconUploads;
    if (v >= 3) {
        return 2;
    }
    s->numIconUploads = v + 1;
    return v;
}

void InventoryItemGrid_StartPresentAnim(InventoryItemGrid *s, u32 v, s32 m) {
    s->presentItem = v;
    s->presentAnimStep = 0;
    if (m == 1) {
        s->presentVariant = 0;
    } else {
        s->presentVariant = 1;
    }
    if (MenuCtrl_IsTouch()) {
        s->showHeldFocus = 0;
    }
}

void InventoryItemGrid_EndPresentAnim(InventoryItemGrid *s) {
    s->heldScale = 10;
    s->showHeldFocus = 1;
}

BOOL InventoryItemGrid_UpdatePresentAnim(InventoryItemGrid *s) {
    u32 st = s->presentAnimStep;
    if (st < 6) {
        u32 v = sPresentAnimFrames[st];
        if (v != 0xff) {
            u8 *r = s->iconCache.getPresentChars(v, s->presentVariant);
            InventoryItemGrid_UploadIcon(s, &sHeldItemSprite, r, 8);
        }
        s->presentAnimStep++;
    } else if (st < 8) {
        s->heldScale = InventoryItemGrid_GetPresentAnimScale(s);
        s->presentAnimStep++;
    } else if (st == 8) {
        InventoryItemGrid_SetHeldItem(s, s->presentItem, 0);
        s->heldScale = 0;
        s->presentAnimStep++;
    } else if (st < 0xc) {
        s->heldScale = InventoryItemGrid_GetPresentAnimScale(s);
        s->presentAnimStep++;
    } else {
        InventoryItemGrid_EndPresentAnim(s);
        return TRUE;
    }
    return FALSE;
}

s32 InventoryItemGrid_GetPresentAnimScale(InventoryItemGrid *s) {
    return sPresentAnimScales[s->presentAnimStep - 6];
}

void InventoryItemGrid_ClearBits(u32 *bits) {
    s32 i;
    u32 z;
    i = 0;
    z = i;
    for (; i < 2; i++) {
        bits[i] = z;
    }
}

void InventoryItemGrid_SetBit(u32 *bits, s32 i) {
    bits[i >> 5] |= 1 << (i & 0x1f);
}

void InventoryItemGrid_ClearBit(u32 *bits, s32 i) {
    bits[i >> 5] &= ~(1 << (i & 0x1f));
}

BOOL InventoryItemGrid_TestBit(u32 *bits, s32 i) {
    BOOL r = TRUE;
    if (((1 << (i & 0x1f)) & bits[i >> 5]) == 0) {
        r = FALSE;
    }
    return r;
}

}  // extern "C"

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_ov094_02292da8) {
    HOST_OVERLAY_VAR(sHeldItemSprite),
    HOST_OVERLAY_VAR(sItemGridMarkSprites),
    HOST_OVERLAY_VAR(sItemGridSlotSprites),
    HOST_OVERLAY_VARS_END
};

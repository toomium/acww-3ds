#include "types.h"
#include "proto/main/unk_020b4828.h"
#include "gfx/VecFx32.h"
#include "game/TouchPicker.h"
#include "game/TouchPickSphere.h"
#include "game/BugNetTarget.h"
#include "nitro/gxoam.h"
#include "proto/main/unk_0203ecec.h"
#include "proto/main/unk_02085940.h"
#include "proto/main/unk_02087e0c.h"
#include "proto/itcm/unk_01ffcfc0.h"
#include "proto/main/unk_02087e70.h"
#include "proto/main/unk_02088b98.h"
#include "n3ds_ui.h"




extern "C" {
extern s32 sOamObjHeights[];
extern s32 sOamObjWidths[];
extern s16 data_02135f44[];

s32 FX_Div(s32 v, s32 s);
s32 func_01ffcb0c(s32 a, s32 b);
void Vec_Sub(VecFx32 *out, VecFx32 *a, VecFx32 *b);
void MIi_CpuCopy32(void *a, void *b, u32 c);
void MIi_CpuCopyFast(void *a, void *b, u32 c);
void DC_FlushRange(void *a, u32 b);
void GX_LoadOAM(void *a, u32 b, u32 c);
void GXS_LoadOAM(void *a, u32 b, u32 c);
}


enum Unk_02087e70_Mode_ { Unk_02087e70_Mode_0 = 0, Unk_02087e70_Mode_1 = 1, Unk_02087e70_Mode_2 = 2, Unk_02087e70_Mode_3 = 3 };

static inline void Unk_02087e70_SetAttr(GXOamAttr *oam, s32 x, s32 y, s32 priority, Unk_02087e70_Mode_ mode, u32 mosaic, s32 effect, u32 shape, u32 color, u32 charName, s32 cParam, s32 rsParam)
{
    if (effect == 0x100 || effect == 0x300) {
        if (mode == 3) {
            oam->attr01 = effect | (((x & 0x1ff) << 16) | (shape | ((mosaic << 12) | ((mode << 10) | ((rsParam << 25) | (y & 0xff))))));
        } else {
            oam->attr01 = effect | (((x & 0x1ff) << 16) | (shape | ((mosaic << 12) | ((mode << 10) | ((y & 0xff) | ((rsParam << 25) | (color << 13)))))));
        }
    } else {
        if (mode == 3) {
            oam->attr01 = effect | (((x & 0x1ff) << 16) | (shape | ((mosaic << 12) | ((y & 0xff) | (mode << 10)))));
        } else {
            oam->attr01 = effect | (((x & 0x1ff) << 16) | (shape | ((mosaic << 12) | ((mode << 10) | ((color << 13) | (y & 0xff))))));
        }
    }
    oam->attr2 = (cParam << 12) | (charName | (priority << 10));
}

BugNetTarget::BugNetTarget() {
    nextTarget = 0;
    isHit = 0;
}

BugNetTarget::~BugNetTarget() {
}

void BugNetTarget::submit(VecFx32 *a, s32 b, s32 radius, u8 d) {
    nextTarget = 0;
    BugNetTarget *h = data_021ce63c;
    if (h == 0) {
        data_021ce63c = this;
    } else {
        nextTarget = h;
        data_021ce63c = this;
    }
    isHit = 0;
    hitRadius = b;
    Scene_GetTouchPicker()->addSphere(this, a, radius, 4, d);
}

extern "C" BOOL BugNet_HitTest(void *a, void *b, s32 rad, u8 *out) {
    BOOL result = FALSE;
    BugNetTarget *p = data_021ce63c;
    VecFx32 v1, v2, v3;
    VecFx32 pts[6];
    u32 i;
    WorldCurve_Apply(&v1, (const VecFx32 *)a);  // (a, b: the net's two points)
    WorldCurve_Apply(&v2, (const VecFx32 *)b);
    Vec_Sub(&v3, &v2, &v1);
    v3.x = FX_Div(v3.x, 0x6000);
    v3.y = FX_Div(v3.y, 0x6000);
    v3.z = FX_Div(v3.z, 0x6000);
    for (i = 0; i < 6; i++) {
        pts[i].x = v1.x + i * v3.x;
        pts[i].y = v1.y + i * v3.y;
        pts[i].z = v1.z + i * v3.z;
    }
    while (p) {
        p->isHit = 0;
        s32 len = func_01ffcb0c(rad + p->hitRadius, rad + p->hitRadius);
        for (i = 0; i < 6; i++) {
            if ((s64)len >= Vec_DistSq(&pts[i], &p->center)) {
                p->isHit = 1;
                result = TRUE;
                if (out) {
                    *out = p->index;
                }
                break;
            }
        }
        p = p->nextTarget;
    }
    return result;
}

#ifdef N3DS_DIRECT
// (n3ds_ui.h: each entry's UI anchor, 0 none; given to the host with the buffers)
static u8 sOamAnchorA[0x80], sOamAnchorB[0x80];
extern "C" void gpu2d_obj_anchors(int engine, const void *oam, const u8 *anchors);  // (modern/host/gpu2d)
#endif

extern "C" void Oam_LoadBuffers() {
    GX_LoadOAM(sOamBufferA, 0, 0x400);
    GXS_LoadOAM(sOamBufferB, 0, 0x400);
#ifdef N3DS_DIRECT
    gpu2d_obj_anchors(0, sOamBufferA, sOamAnchorA);
    gpu2d_obj_anchors(1, sOamBufferB, sOamAnchorB);
#endif
}

extern "C" void Oam_FlushBuffers() {
    DC_FlushRange(sOamBufferA, 0x400);
    DC_FlushRange(sOamBufferB, 0x400);
}

extern "C" void Oam_ResetBuffers() {
    sOamBufferA[0].attr01 = 0xc0;
    sOamBufferA[0].attr2 = 0;
    MIi_CpuCopy32(sOamBufferA, &sOamBufferA[1], 0x18);
    MIi_CpuCopyFast(sOamBufferA, &sOamBufferA[4], 0x3e0);
    MIi_CpuCopyFast(sOamBufferA, sOamBufferB, 0x400);
    sOamCountA = 0;
    sOamAffineCountA = 0;
    sOamCountB = 0;
    sOamAffineCountB = 0;
#ifdef N3DS_DIRECT
    for (int i = 0; i < 0x80; i++) sOamAnchorA[i] = sOamAnchorB[i] = 0;
#endif
}

// obj: the object's OAM attributes (a GXOamAttr: attr0 / attr1, then attr2), drawn at x / y
extern "C" s32 Oam_DrawObj(s32 mode, const void *obj, s32 x, s32 y, s32 pal, s32 pri, s32 *rect) {
    u32 *info = (u32 *)obj;
    GXOamAttr *ent;
    s32 *cntp;
    s32 *othp;
    s32 idx, c;
    u32 bit13, base;
    s32 x0, y0, w, h;
    s32 mode2;
    if (Oam_UseBufferA(mode)) {
        cntp = &sOamCountA;
        c = *cntp;
        if (c >= 0x80) {
            return -1;
        }
        ent = sOamBufferA + c;
        othp = &sOamAffineCountA;
    } else {
        cntp = &sOamCountB;
        c = *cntp;
        if (c >= 0x80) {
            return -1;
        }
        ent = sOamBufferB + c;
        othp = &sOamAffineCountB;
    }
    x0 = (info[0] << 7) >> 23;
    if (x0 >= 0x100) {
        x0 -= 0x200;
    }
    x0 += x;
    y0 = *(s8 *)info + y;
    w = Oam_GetObjWidth(info);
    h = Oam_GetObjHeight(info);
    if (rect && ((info[0] << 22) >> 30) != 1) {
        x0 -= w >> 1;
        y0 -= h >> 1;
        w <<= 1;
        h <<= 1;
    }
    if (x0 + w < 0 || x0 > 0x100) {
        return -2;
    }
    if (y0 + h < 0 || y0 > 0xc0) {
        return -3;
    }
    if (pal == -1) {
        pal = (info[1] << 16) >> 28;
    }
    if (pri == -1) {
        pri = (info[1] << 20) >> 30;
    }
    if (rect) {
        idx = Oam_AllocAffine((u8 *)(ent - *cntp), othp, rect);  // (the buffer start)
        if (idx == -1) {
            return -4;
        }
        if (((info[0] << 22) >> 30) == 1) {
            mode2 = 0x100;
        } else {
            mode2 = 0x300;
        }
    } else {
        idx = 0;
        mode2 = *(volatile u32 *)&info[0] & 0x30000000;
    }
    base = (u32)(info[1] << 22) >> 22;
    bit13 = (info[0] << 18) >> 31;
    Unk_02087e70_SetAttr(ent, x0, y0, pri, (Unk_02087e70_Mode_)((info[0] << 20) >> 30), (info[0] << 19) >> 31, mode2, info[0] & 0xc000c000, bit13, base, pal, idx);
    *cntp = *cntp + 1;
    return 1;
}

extern "C" GXOamAttr *Oam_DrawObjRotated(u32 mode, GXOamAttr *e, s32 dx, s32 dy, s32 pal, s32 pri, s32 scale, s32 rot, s32 sz)
{
    GXOamAttr *oam;
    s32 *cnt;
    s32 *aux;
    s32 x, y, w, h;
    s32 idx;
    s32 flags;
    s32 n;
    u32 b13;
    u32 w1lo;
    if (Oam_UseBufferA(mode)) {
        cnt = &sOamCountA;
        n = *cnt;
        if (n >= 0x80) {
            return 0;
        }
        oam = &sOamBufferA[n];
        aux = &sOamAffineCountA;
    } else {
        cnt = &sOamCountB;
        n = *cnt;
        if (n >= 0x80) {
            return 0;
        }
        oam = &sOamBufferB[n];
        aux = &sOamAffineCountB;
    }
    x = (e->attr01 << 7) >> 23;
    if (x >= 0x100) {
        x -= 0x200;
    }
    y = *(s8 *)e;
    w = Oam_GetObjWidth(&e->attr01);
    h = Oam_GetObjHeight(&e->attr01);
    if (rot != 0) {
        if (sz != 0) {
            s32 s, ty, c, hw, hh, t;
            s32 i2 = ((s32)(u16)(s16)rot >> 4) * 2;
            s32 nx, ny;
            hw = w >> 1;
            hh = h >> 1;
            ty = y + hh;
            s = data_02135f44[i2];
            x += hw;
            c = data_02135f44[i2 + 1];
            nx = func_01ffcb0c(c, x);
            nx -= func_01ffcb0c(s, ty);
            t = func_01ffcb0c(c, ty);
            ny = t + func_01ffcb0c(s, x);
            x = ((nx + (nx > 0 ? 0x800 : -0x800)) >> 12) - hw;
            y = ((ny + (ny > 0 ? 0x800 : -0x800)) >> 12) - hh;
        } else {
            s32 hw = w >> 1;
            s32 hh = h >> 1;
            s32 i2 = ((s32)(u16)(s16)rot >> 4) * 2;
            s32 s = data_02135f44[i2];
            s32 c = data_02135f44[i2 + 1];
            s32 ty = y + hh;
            s32 tx = x + hw;
            s32 a = tx * c;
            s32 b = ty * s;
            x = ((a - b) >> 12) - hw;
            y = ((ty * c + tx * s) >> 12) - hh;
        }
    }
    if ((scale != 0x1000 || rot != 0) && ((e->attr01 << 22) >> 30) != 1) {
        x -= w >> 1;
        y -= h >> 1;
        w <<= 1;
        h <<= 1;
    }
    x += dx;
    y += dy;
    if (rot != 0) {
        if (w < h) {
            w = h;
        }
        if (x + w < 0 || x > 0x100) {
            return 0;
        }
        if (y + w < 0 || y > 0xc0) {
            return 0;
        }
    } else {
        if (x + w < 0 || x > 0x100) {
            return 0;
        }
        if (y + h < 0 || y > 0xc0) {
            return 0;
        }
    }
    if (pal == -1) {
        pal = e->cParam;
    }
    if (pri == -1) {
        pri = e->priority;
    }
    if (scale != 0x1000 || rot != 0) {
        s32 m[4];
        s32 i = (rot >> 4) * 2;
        s32 cc = data_02135f44[i + 1];
        s32 c = (cc * scale + 0x800) >> 12;
        m[0] = c;
        s32 ss = data_02135f44[i];
        s32 s = (ss * scale + 0x800) >> 12;
        m[1] = s;
        m[2] = -s;
        m[3] = c;
        if (e->attr01 & 0x10000000) {
            m[0] = -c;
            m[1] = -s;
        }
        if (e->attr01 & 0x20000000) {
            m[2] = -m[2];
            m[3] = -m[3];
        }
        idx = Oam_AllocAffine((u8 *)(oam - *cnt), aux, m);
        if (idx == -1) {
            return 0;
        }
        if (((e->attr01 << 22) >> 30) == 1) {
            flags = 0x100;
        } else {
            flags = 0x300;
        }
    } else {
        idx = 0;
        flags = ((volatile GXOamAttr *)e)->attr01 & 0x30000000;
    }
    w1lo = e->charNo;
    b13 = (e->attr01 << 18) >> 31;
    Unk_02087e70_SetAttr(oam, x, y, pri, (Unk_02087e70_Mode_)((e->attr01 << 20) >> 30), (e->attr01 << 19) >> 31, flags, e->attr01 & 0xc000c000, b13, w1lo, pal, idx);
    *cnt = *cnt + 1;
    return oam;
}

extern "C" void Oam_DrawCell(u32 mode, GXOamAttr *e, s32 dx, s32 dy, s32 pal, s32 pri, s32 sx, s32 sy, s32 rot, s32 sz, s32 fx, s32 fy)
{
    GXOamAttr *oam;
    s32 priv;
    s32 palv;
    s32 w;
    BOOL use;
    s32 *cnt;
    s32 y;
    s32 *aux;
    s32 h;
    s32 hw;
    s32 x;
    s32 hh;
    s32 d;
    Unk_02087e70_Mode_ size;
    s32 idx;
    s32 flags;
    u32 b13;
    u32 w1lo;
    BOOL k3;
    BOOL k0;
    BOOL k1;
    if (Oam_UseBufferA(mode)) {
        oam = sOamBufferA;
        cnt = &sOamCountA;
        oam += *cnt;
        aux = &sOamAffineCountA;
    } else {
        oam = sOamBufferB;
        cnt = &sOamCountB;
        oam += *cnt;
        aux = &sOamAffineCountB;
    }
    if (sx != 0x1000 || sy != 0x1000 || rot != 0) {
        use = TRUE;
    } else {
        use = FALSE;
    }
top:
    {
        if (*cnt >= 0x80) {
            goto end;
        }
        k3 = FALSE;
        if (use && ((e->attr01 << 22) >> 30) == 3) {
            k3 = TRUE;
        }
        k0 = FALSE;
        if (use && ((e->attr01 << 22) >> 30) == 0) {
            k0 = TRUE;
        }
        k1 = FALSE;
        if (use && ((e->attr01 << 22) >> 30) == 1) {
            k1 = TRUE;
        }
        x = (e->attr01 << 7) >> 23;
        if (x >= 0x100) {
            x -= 0x200;
        }
        y = *(s8 *)e;
        w = Oam_GetObjWidth(&e->attr01);
        h = Oam_GetObjHeight(&e->attr01);
        if (use) {
            if (k3) {
                w <<= 1;
                h <<= 1;
            }
            hw = w >> 1;
            hh = h >> 1;
            x = (x + hw) << 12;
            y = (y + hh) << 12;
            if (sx != 0x1000) {
                x = FX_Div(x, sx);
            }
            if (sy != 0x1000) {
                y = FX_Div(y, sy);
            }
            if (rot != 0) {
                s16 *sinp = &data_02135f44[((s32)(u16)(s16)rot >> 4) * 2];
                s16 *cosp = &data_02135f44[((s32)(u16)(s16)rot >> 4) * 2 + 1];
                s16 a = *sinp;
                s16 b = *cosp;
                s32 f;
                d = func_01ffcb0c(b, x) - func_01ffcb0c(a, y);
                f = func_01ffcb0c(b, y) + func_01ffcb0c(a, x);
                x = ((d + (d > 0 ? 0x800 : -0x800)) >> 12) - hw;
                y = ((f + (f > 0 ? 0x800 : -0x800)) >> 12) - hh;
            } else {
                x = ((x + 0x800) >> 12) - hw;
                y = ((y + 0x800) >> 12) - hh;
            }
            if (k0) {
                x -= hw;
                y -= hh;
                w <<= 1;
                h <<= 1;
            }
        }
        x += dx;
        y += dy;
        if (rot != 0) {
            if (w < h) {
                w = h;
            }
            if (x + w < 0 || x > 0x100) {
                if (e->_3 == 0xffff) {
                    goto end;
                }
                e++;
                goto top;
            }
            if (y + w < 0 || y > 0xc0) {
                if (e->_3 == 0xffff) {
                    goto end;
                }
                e++;
                goto top;
            }
        } else {
            if (x + w < 0 || x > 0x100) {
                if (e->_3 == 0xffff) {
                    goto end;
                }
                e++;
                goto top;
            }
            if (y + h < 0 || y > 0xc0) {
                if (e->_3 == 0xffff) {
                    goto end;
                }
                e++;
                goto top;
            }
        }
        if (pal == -1) {
            palv = e->cParam;
        } else {
            palv = pal;
        }
        if (pri == -1) {
            priv = e->priority;
        } else {
            priv = pri;
        }
        if (sz > 0) {
            size = (Unk_02087e70_Mode_)sz;
        } else {
            size = (Unk_02087e70_Mode_)((e->attr01 << 20) >> 30);
        }
        if (use) {
            s32 P, Q;
            s32 m[4];
            s32 cc, ss;
            s32 ri = (rot >> 4) * 2;
            cc = data_02135f44[ri + 1];
            P = (cc * sx + 0x800) >> 12;
            m[0] = P;
            ss = data_02135f44[ri];
            Q = (ss * sx + 0x800) >> 12;
            m[1] = Q;
            m[2] = -((ss * sy + 0x800) >> 12);
            m[3] = (cc * sy + 0x800) >> 12;
            if (e->attr01 & 0x10000000) {
                m[0] = -P;
                m[1] = -Q;
            }
            if (e->attr01 & 0x20000000) {
                m[2] = -m[2];
                m[3] = -m[3];
            }
            idx = Oam_AllocAffine((u8 *)(oam - *cnt), aux, m);
            if (idx == -1) {
                if (e->_3 == 0xffff) {
                    goto end;
                }
                e++;
                goto top;
            }
            if (k1) {
                flags = 0x100;
            } else {
                flags = 0x300;
            }
        } else {
            idx = 0;
            flags = e->attr01 & 0x30000000;
        }
        if (fx) {
            flags |= 0x10000000;
        }
        if (fy) {
            flags |= 0x20000000;
        }
        w1lo = e->charNo;
        b13 = (e->attr01 << 18) >> 31;
        Unk_02087e70_SetAttr(oam, x, y, priv, size, (e->attr01 << 19) >> 31, flags, e->attr01 & 0xc000c000, b13, w1lo, palv, idx);
        oam++;
        *cnt = *cnt + 1;
        if (e->_3 == 0xffff) {
            goto end;
        }
        e++;
        goto top;
    }
end:;
}

// Declarations for data defined further down (definition order sets the data layout)

GXOamAttr sOamBufferA[0x80];

s32 sOamCountB;

GXOamAttr sOamBufferB[0x80];

s32 sOamCountA;

s32 sOamAffineCountB;

s32 sOamAffineCountA;

#ifdef N3DS_DIRECT
extern "C" int Oam_Count(int mode) { return Oam_UseBufferA(mode) ? sOamCountA : sOamCountB; }
extern "C" void Oam_Anchor(int mode, int from, unsigned anchor) {
    u8 *t = Oam_UseBufferA(mode) ? sOamAnchorA : sOamAnchorB;
    for (int i = from < 0 ? 0 : from, n = Oam_Count(mode); i < n; i++) t[i] = (u8)anchor;
}
#endif

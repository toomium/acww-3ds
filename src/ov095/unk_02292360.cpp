// mwcc-flags: -str reuse
#include "types.h"
#include "nitro/gxoam.h"
#include "menu/Keyboard.h"
#include "player/PlayerData.h"
#include "player/PlayerId.h"
#include "ui/LabelString.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_0203ecec.h"
#include "proto/main/unk_020501d4.h"
#include "proto/main/unk_02051218.h"
#include "proto/main/unk_02063f3c.h"
#include "proto/main/unk_0206cbdc.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_0206f834.h"
#include "proto/main/unk_02085940.h"
#include "proto/main/unk_02087e0c.h"
#include "proto/main/unk_02087e70.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_020b7d84.h"
#include "proto/ov002/unk_ov002_02200840.h"
#include "proto/ov090/unk_ov090_022918e0.h"
#include "proto/ov095/unk_02292360.h"
#ifdef N3DS_DIRECT
#include "n3ds_ui.h"
#endif
#include "gfx/BgVramTask.h"


extern "C" {
extern volatile u16 sCharSortKeyZero;
s32 _s32_div_f(s32 a, s32 b);
void MI_CpuCopy8(const void *src, void *dst, u32 n);  // SDK assembly

}

extern "C" s16 sKeyCodesPage3[];
extern "C" s16 sKeyCodesPage1Lower[];
extern "C" s16 sKeyCodesPage1Upper[];
extern "C" s16 sKeyCodesPage1Alt[];
extern "C" s16 sKeyCodesPage2[];
extern "C" const s32 data_ov095_02295494[];
extern "C" const s32 data_ov095_022954cc[];
extern "C" const s32 data_ov095_02295504[];
extern "C" const s32 data_ov095_0229553c[];
extern "C" char data_ov095_02295704[];
extern "C" char data_ov095_0229574c[];
extern "C" char data_ov095_022956d4[];
extern "C" char data_ov095_0229565c[];
extern "C" char data_ov095_022956ec[];
extern "C" char data_ov095_0229577c[];
extern "C" char data_ov095_0229568c[];
extern "C" char data_ov095_022956a4[];
extern "C" char data_ov095_022956bc[];
extern "C" char data_ov095_02295674[];
extern "C" char data_ov095_02295734[];
extern "C" char data_ov095_0229571c[];
extern "C" char data_ov095_02295794[];
extern "C" char data_ov095_02295764[];
extern "C" char data_ov095_02295620[];
extern "C" char data_ov095_02295648[];
extern "C" char data_ov095_022955f8[];
extern "C" char data_ov095_022955bc[];
extern "C" char data_ov095_022955a8[];
extern "C" char data_ov095_022955d0[];
extern "C" char data_ov095_022955e4[];
extern "C" s32 data_ov095_0229560c[];
extern "C" s32 data_ov095_02295634[];
extern "C" const s32 data_ov095_02295474[];
extern "C" const s32 data_ov095_02295454[];
extern "C" const s32 data_ov095_02295484[];
extern "C" const s32 data_ov095_02295464[];

extern "C" GXOamAttr data_ov095_022965bc[60] = {{0x81884000, 0x18b, 1, 0xc, 0x0}, {0x819c4000, 0x18b, 1, 0xc, 0x0}, {0x81b04000, 0x18b, 1, 0xc, 0x0}, {0x81c44000, 0x18b, 1, 0xc, 0x0}, {0x81d84000, 0x18b, 1, 0xc, 0x0}, {0x81ec4000, 0x18b, 1, 0xc, 0x0}, {0x80004000, 0x18b, 1, 0xc, 0x0}, {0x80144000, 0x18b, 1, 0xc, 0x0}, {0x80284000, 0x18b, 1, 0xc, 0x0}, {0x803c4000, 0x18b, 1, 0xc, 0x0}, {0x80504000, 0x18b, 1, 0xc, 0x0}, {0x80644000, 0x18f, 1, 0xc, 0x0}, {0x80644008, 0x1cf, 1, 0xc, 0x0}, {0x81884010, 0x18b, 1, 0xc, 0x0}, {0x819c4010, 0x18b, 1, 0xc, 0x0}, {0x81b04010, 0x18b, 1, 0xc, 0x0}, {0x81c44010, 0x18b, 1, 0xc, 0x0}, {0x81d84010, 0x18b, 1, 0xc, 0x0}, {0x81ec4010, 0x18b, 1, 0xc, 0x0}, {0x80004010, 0x18b, 1, 0xc, 0x0}, {0x80144010, 0x18b, 1, 0xc, 0x0}, {0x80284010, 0x18b, 1, 0xc, 0x0}, {0x803c4010, 0x18b, 1, 0xc, 0x0}, {0x80504010, 0x18b, 1, 0xc, 0x0}, {0x80644018, 0x18f, 1, 0xc, 0x0}, {0x80640020, 0x18f, 1, 0xc, 0x0}, {0x81884020, 0x18b, 1, 0xc, 0x0}, {0x819c4020, 0x18b, 1, 0xc, 0x0}, {0x81b04020, 0x18b, 1, 0xc, 0x0}, {0x81c44020, 0x18b, 1, 0xc, 0x0}, {0x81d84020, 0x18b, 1, 0xc, 0x0}, {0x81ec4020, 0x18b, 1, 0xc, 0x0}, {0x80004020, 0x18b, 1, 0xc, 0x0}, {0x80144020, 0x18b, 1, 0xc, 0x0}, {0x80284020, 0x18b, 1, 0xc, 0x0}, {0x803c4020, 0x18b, 1, 0xc, 0x0}, {0x80504020, 0x18b, 1, 0xc, 0x0}, {0x81884030, 0x18b, 1, 0xc, 0x0}, {0x819c4030, 0x18b, 1, 0xc, 0x0}, {0x81b04030, 0x18b, 1, 0xc, 0x0}, {0x81c44030, 0x18b, 1, 0xc, 0x0}, {0x81d84030, 0x18b, 1, 0xc, 0x0}, {0x81ec4030, 0x18b, 1, 0xc, 0x0}, {0x80004030, 0x18b, 1, 0xc, 0x0}, {0x80144030, 0x18b, 1, 0xc, 0x0}, {0x80284030, 0x18b, 1, 0xc, 0x0}, {0x803c4030, 0x18b, 1, 0xc, 0x0}, {0x80504030, 0x18b, 1, 0xc, 0x0}, {0x81884040, 0x18b, 1, 0xc, 0x0}, {0x819c4040, 0x18b, 1, 0xc, 0x0}, {0x81b04040, 0x18b, 1, 0xc, 0x0}, {0x81c44040, 0x18b, 1, 0xc, 0x0}, {0x81d84040, 0x18b, 1, 0xc, 0x0}, {0x81ec4040, 0x18b, 1, 0xc, 0x0}, {0x80004040, 0x18b, 1, 0xc, 0x0}, {0x80144040, 0x18b, 1, 0xc, 0x0}, {0x40280040, 0x1cb, 1, 0xc, 0x0}, {0x80384040, 0xc5, 1, 0xc, 0x0}, {0x80504040, 0xc5, 1, 0xc, 0x0}, {0x406b0040, 0x1cd, 1, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_022963e4[59] = {{0x81884000, 0x18b, 1, 0xc, 0x0}, {0x81884010, 0x18b, 1, 0xc, 0x0}, {0x81884020, 0x18b, 1, 0xc, 0x0}, {0x81884030, 0x18b, 1, 0xc, 0x0}, {0x81884040, 0x18b, 1, 0xc, 0x0}, {0x819c4000, 0x18b, 1, 0xc, 0x0}, {0x819c4010, 0x18b, 1, 0xc, 0x0}, {0x819c4020, 0x18b, 1, 0xc, 0x0}, {0x819c4030, 0x18b, 1, 0xc, 0x0}, {0x819c4040, 0x18b, 1, 0xc, 0x0}, {0x81b04000, 0x18b, 1, 0xc, 0x0}, {0x81b04010, 0x18b, 1, 0xc, 0x0}, {0x81b04020, 0x18b, 1, 0xc, 0x0}, {0x81b04030, 0x18b, 1, 0xc, 0x0}, {0x81b04040, 0x18b, 1, 0xc, 0x0}, {0x81c44000, 0x18b, 1, 0xc, 0x0}, {0x81c44010, 0x18b, 1, 0xc, 0x0}, {0x81c44020, 0x18b, 1, 0xc, 0x0}, {0x81c44030, 0x18b, 1, 0xc, 0x0}, {0x81c44040, 0x18b, 1, 0xc, 0x0}, {0x81d84000, 0x18b, 1, 0xc, 0x0}, {0x81d84010, 0x18b, 1, 0xc, 0x0}, {0x81d84020, 0x18b, 1, 0xc, 0x0}, {0x81d84030, 0x18b, 1, 0xc, 0x0}, {0x81d84040, 0x18b, 1, 0xc, 0x0}, {0x81ec4000, 0x18b, 1, 0xc, 0x0}, {0x81ec4010, 0x18b, 1, 0xc, 0x0}, {0x81ec4020, 0x18b, 1, 0xc, 0x0}, {0x81ec4030, 0x18b, 1, 0xc, 0x0}, {0x81ec4040, 0x18b, 1, 0xc, 0x0}, {0x80004000, 0x18b, 1, 0xc, 0x0}, {0x80004010, 0x18b, 1, 0xc, 0x0}, {0x80004020, 0x18b, 1, 0xc, 0x0}, {0x80004030, 0x18b, 1, 0xc, 0x0}, {0x80004040, 0x18b, 1, 0xc, 0x0}, {0x80144000, 0x18b, 1, 0xc, 0x0}, {0x80144010, 0x18b, 1, 0xc, 0x0}, {0x80144020, 0x18b, 1, 0xc, 0x0}, {0x80144030, 0x18b, 1, 0xc, 0x0}, {0x80144040, 0x18b, 1, 0xc, 0x0}, {0x80284000, 0x18b, 1, 0xc, 0x0}, {0x80284010, 0x18b, 1, 0xc, 0x0}, {0x80284020, 0x18b, 1, 0xc, 0x0}, {0x80284030, 0x18b, 1, 0xc, 0x0}, {0x80284040, 0x18b, 1, 0xc, 0x0}, {0x803c4000, 0x18b, 1, 0xc, 0x0}, {0x803c4010, 0x18b, 1, 0xc, 0x0}, {0x803c4020, 0x18b, 1, 0xc, 0x0}, {0x803c4030, 0x18b, 1, 0xc, 0x0}, {0x803c4040, 0x18b, 1, 0xc, 0x0}, {0x80504000, 0x18b, 1, 0xc, 0x0}, {0x80504010, 0x18b, 1, 0xc, 0x0}, {0x80504020, 0x18b, 1, 0xc, 0x0}, {0x80504030, 0x18b, 1, 0xc, 0x0}, {0x80504040, 0x18b, 1, 0xc, 0x0}, {0x80644000, 0x18b, 1, 0xc, 0x0}, {0x80644010, 0x18b, 1, 0xc, 0x0}, {0x80640020, 0x18f, 1, 0xc, 0x0}, {0x80644040, 0x18b, 1, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02296214[58] = {{0x81844000, 0x18b, 1, 0xc, 0x0}, {0x81984000, 0x18b, 1, 0xc, 0x0}, {0x81ac4000, 0x18b, 1, 0xc, 0x0}, {0x81c04000, 0x18b, 1, 0xc, 0x0}, {0x81d44000, 0x18b, 1, 0xc, 0x0}, {0x81e84000, 0x18b, 1, 0xc, 0x0}, {0x81fc4000, 0x18b, 1, 0xc, 0x0}, {0x80104000, 0x18b, 1, 0xc, 0x0}, {0x80244000, 0x18b, 1, 0xc, 0x0}, {0x80384000, 0x18b, 1, 0xc, 0x0}, {0x804c4000, 0x18b, 1, 0xc, 0x0}, {0x80604000, 0x18b, 1, 0xc, 0x0}, {0x818c4010, 0x18b, 1, 0xc, 0x0}, {0x81a04010, 0x18b, 1, 0xc, 0x0}, {0x81b44010, 0x18b, 1, 0xc, 0x0}, {0x81c84010, 0x18b, 1, 0xc, 0x0}, {0x81dc4010, 0x18b, 1, 0xc, 0x0}, {0x81f04010, 0x18b, 1, 0xc, 0x0}, {0x80044010, 0x18b, 1, 0xc, 0x0}, {0x80184010, 0x18b, 1, 0xc, 0x0}, {0x802c4010, 0x18b, 1, 0xc, 0x0}, {0x80404010, 0x18b, 1, 0xc, 0x0}, {0x40540010, 0x1cb, 1, 0xc, 0x0}, {0x805f4010, 0x1cb, 1, 0xc, 0x0}, {0x81834020, 0x18b, 1, 0xc, 0x0}, {0x81974020, 0x18b, 1, 0xc, 0x0}, {0x81ab4020, 0x18b, 1, 0xc, 0x0}, {0x81bf4020, 0x18b, 1, 0xc, 0x0}, {0x81d34020, 0x18b, 1, 0xc, 0x0}, {0x81e74020, 0x18b, 1, 0xc, 0x0}, {0x81fb4020, 0x18b, 1, 0xc, 0x0}, {0x800f4020, 0x18b, 1, 0xc, 0x0}, {0x80234020, 0x18b, 1, 0xc, 0x0}, {0x80374020, 0x18b, 1, 0xc, 0x0}, {0x404b0020, 0x1cb, 1, 0xc, 0x0}, {0x804e4020, 0xc5, 1, 0xc, 0x0}, {0x805f4020, 0x1cb, 1, 0xc, 0x0}, {0x81834030, 0x1cb, 1, 0xc, 0x0}, {0x81a04030, 0x18b, 1, 0xc, 0x0}, {0x81b44030, 0x18b, 1, 0xc, 0x0}, {0x81c84030, 0x18b, 1, 0xc, 0x0}, {0x81dc4030, 0x18b, 1, 0xc, 0x0}, {0x81f04030, 0x18b, 1, 0xc, 0x0}, {0x80044030, 0x18b, 1, 0xc, 0x0}, {0x80184030, 0x18b, 1, 0xc, 0x0}, {0x802c4030, 0x18b, 1, 0xc, 0x0}, {0x80404030, 0x18b, 1, 0xc, 0x0}, {0x80544030, 0x18b, 1, 0xc, 0x0}, {0x81984040, 0x18b, 1, 0xc, 0x0}, {0x81ac4040, 0x18b, 1, 0xc, 0x0}, {0x41c00040, 0x1cb, 1, 0xc, 0x0}, {0x81d04040, 0xc5, 0, 0xc, 0x0}, {0x81f04040, 0xc5, 0, 0xc, 0x0}, {0x800d4040, 0xc5, 0, 0xc, 0x0}, {0x801f4040, 0xc5, 0, 0xc, 0x0}, {0x403f0040, 0x1cd, 1, 0xc, 0x0}, {0x804c4040, 0x18b, 1, 0xc, 0x0}, {0x80604040, 0x18b, 1, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_022960dc[39] = {{0x81884008, 0x18b, 1, 0xc, 0x0}, {0x819c4008, 0x18b, 1, 0xc, 0x0}, {0x81b04008, 0x18b, 1, 0xc, 0x0}, {0x81c44008, 0x18b, 1, 0xc, 0x0}, {0x81d84008, 0x18b, 1, 0xc, 0x0}, {0x81ec4008, 0x18b, 1, 0xc, 0x0}, {0x80004008, 0x18b, 1, 0xc, 0x0}, {0x80144008, 0x18b, 1, 0xc, 0x0}, {0x80284008, 0x18b, 1, 0xc, 0x0}, {0x803c4008, 0x18b, 1, 0xc, 0x0}, {0x80504008, 0x18b, 1, 0xc, 0x0}, {0x80644008, 0x18b, 1, 0xc, 0x0}, {0x81884018, 0x18b, 1, 0xc, 0x0}, {0x819c4018, 0x18b, 1, 0xc, 0x0}, {0x81b04018, 0x18b, 1, 0xc, 0x0}, {0x81c44018, 0x18b, 1, 0xc, 0x0}, {0x81d84018, 0x18b, 1, 0xc, 0x0}, {0x81ec4018, 0x18b, 1, 0xc, 0x0}, {0x80004018, 0x18b, 1, 0xc, 0x0}, {0x80144018, 0x18b, 1, 0xc, 0x0}, {0x80284018, 0x18b, 1, 0xc, 0x0}, {0x803c4018, 0x18b, 1, 0xc, 0x0}, {0x80688018, 0x195, 1, 0xc, 0x0}, {0x80500018, 0x193, 1, 0xc, 0x0}, {0x81884028, 0x18b, 1, 0xc, 0x0}, {0x819c4028, 0x18b, 1, 0xc, 0x0}, {0x81b04028, 0x18b, 1, 0xc, 0x0}, {0x81c44028, 0x18b, 1, 0xc, 0x0}, {0x81d84028, 0x18b, 1, 0xc, 0x0}, {0x81ec4028, 0x18b, 1, 0xc, 0x0}, {0x80004028, 0x18b, 1, 0xc, 0x0}, {0x80144028, 0x18b, 1, 0xc, 0x0}, {0x80284028, 0x18b, 1, 0xc, 0x0}, {0x803c4028, 0x18b, 1, 0xc, 0x0}, {0x41d00038, 0x1cb, 1, 0xc, 0x0}, {0x81e04038, 0xc5, 1, 0xc, 0x0}, {0x80004038, 0xc5, 1, 0xc, 0x0}, {0x80084038, 0xc5, 1, 0xc, 0x0}, {0x40230038, 0x1cd, 1, 0xc, 0xffff}};
extern "C" GXOamAttr *sLengthGaugeCells[32] = {data_ov095_022957ac, data_ov095_022958e4, data_ov095_02295cf4, data_ov095_0229590c, data_ov095_02295934, data_ov095_0229595c, data_ov095_0229586c, data_ov095_02295984, data_ov095_022959ac, data_ov095_022959d4, data_ov095_02295d1c, data_ov095_022959fc, data_ov095_02295a24, data_ov095_02295a4c, data_ov095_02295a74, data_ov095_0229580c, data_ov095_02295a9c, data_ov095_02295ac4, data_ov095_02295aec, data_ov095_02295b14, data_ov095_02295b3c, data_ov095_02295b64, data_ov095_02295b8c, data_ov095_02295bb4, data_ov095_02295bdc, data_ov095_02295c04, data_ov095_02295c2c, data_ov095_02295c54, data_ov095_02295c7c, data_ov095_02295ca4, data_ov095_02295ccc, data_ov095_0229582c};
extern "C" s16 sKeyCodesPage3[56] = {102, 103, 104, 106, 110, 111, 112, 113, 114, 115, 116, 117, 120, 121, 122, 124, 68, 126, 127, 128, 129, 109, 119, 101, 71, 72, 73, 75, 79, 80, 81, 82, 83, 84, 85, 86, 89, 90, 91, 93, 65, 95, 96, 97, 98, 78, 88, 187, 217, 167, 188, 189, 256, 134, 133, 0};
extern "C" s16 sKeyCodesPage1Lower[54] = {54, 55, 56, 57, 58, 59, 60, 61, 62, 53, 179, 177, 43, 49, 31, 44, 46, 51, 47, 35, 41, 42, 256, 288, 27, 45, 30, 32, 33, 34, 36, 37, 38, 134, 287, 52, 50, 29, 48, 28, 40, 39, 146, 148, 209, 142, 143, 291, 133, 289, 290, 135, 155, 0};
extern "C" s16 sKeyCodesPage1Upper[54] = {220, 221, 222, 223, 147, 145, 140, 156, 166, 153, 218, 219, 17, 23, 5, 18, 20, 25, 21, 9, 15, 16, 256, 288, 1, 19, 4, 6, 7, 8, 10, 11, 12, 134, 287, 26, 24, 3, 22, 2, 14, 13, 152, 154, 161, 157, 159, 291, 133, 289, 290, 150, 151, 0};
extern "C" s16 sKeyCodesPage1Alt[54] = {54, 55, 56, 57, 58, 59, 60, 61, 62, 53, 201, 156, 43, 49, 31, 44, 46, 51, 47, 35, 41, 42, 256, 288, 27, 45, 30, 32, 33, 34, 36, 37, 38, 134, 287, 52, 50, 29, 48, 28, 40, 39, 156, 156, 156, 157, 159, 291, 133, 289, 290, 135, 155, 0};
extern "C" GXOamAttr data_ov095_02295e48[12] = {{0x41f700eb, 0x109, 0, 8, 0x0}, {0x400d00eb, 0x10b, 0, 8, 0x0}, {0x402300eb, 0x10d, 0, 8, 0x0}, {0x403900eb, 0x10f, 0, 8, 0x0}, {0x81ef40eb, 0xcb, 0, 10, 0x0}, {0x800540eb, 0xcb, 0, 10, 0x0}, {0x801b40eb, 0xcb, 0, 10, 0x0}, {0x803140eb, 0xcb, 0, 10, 0x0}, {0x81ef40ed, 0xcb, 0, 1, 0x0}, {0x800540ed, 0xcb, 0, 1, 0x0}, {0x801b40ed, 0xcb, 0, 1, 0x0}, {0x803140ed, 0xcb, 0, 1, 0xffff}};
extern "C" s16 sKeyCodesPage2[34] = {220, 221, 222, 223, 135, 155, 201, 166, 161, 149, 156, 179, 177, 146, 148, 209, 150, 151, 145, 147, 140, 157, 159, 142, 143, 152, 154, 218, 219, 153, 183, 256, 134, 133};
extern "C" const s32 data_ov095_02295494[14] = {92, 226, 226, 226, 226, 226, 81, 93, 106, 108, 109, 219, 220, 82};
extern "C" const s32 data_ov095_022954cc[14] = {144, 226, 226, 226, 226, 226, 143, 226, 226, 226, 226, 219, 220, 226};
extern "C" const s32 data_ov095_02295504[14] = {199, 226, 226, 226, 226, 226, 198, 226, 226, 226, 226, 219, 220, 226};
extern "C" const s32 data_ov095_0229553c[14] = {57, 50, 51, 52, 53, 54, 56, 226, 226, 226, 226, 219, 220, 226};
extern "C" GXOamAttr data_ov095_02295d44[6] = {{0x41b700eb, 0x101, 0, 0xa, 0x0}, {0x41c900eb, 0x103, 0, 0xa, 0x0}, {0x41db00eb, 0x105, 0, 0xa, 0x0}, {0x41b700ed, 0x101, 0, 0x1, 0x0}, {0x41c900ed, 0x103, 0, 0x1, 0x0}, {0x41db00ed, 0x105, 0, 0x1, 0xffff}};
extern "C" GXOamAttr data_ov095_02295d74[6] = {{0x41b700e3, 0x101, 0, 0xa, 0x0}, {0x41c900e3, 0x103, 0, 0xa, 0x0}, {0x41db00e3, 0x105, 0, 0xa, 0x0}, {0x41b700e5, 0x101, 0, 0x1, 0x0}, {0x41c900e5, 0x103, 0, 0x1, 0x0}, {0x41db00e5, 0x105, 0, 0x1, 0xffff}};
extern "C" GXOamAttr data_ov095_02295da4[6] = {{0x018500ea, 0xfc, 0, 0xc, 0x0}, {0x418d40ea, 0xd8, 0, 0xc, 0x0}, {0x418100e8, 0x118, 0, 0xc, 0x0}, {0x419d00e8, 0x11c, 0, 0xc, 0x0}, {0x01ad80e8, 0x11e, 0, 0xc, 0x0}, {0x419100e8, 0x11c, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295dd4[6] = {{0x007300ea, 0xfd, 0, 0xe, 0x0}, {0x405340ea, 0xf8, 0, 0xe, 0x0}, {0x506f00e8, 0x118, 0, 0xe, 0x0}, {0x505300e8, 0x11c, 0, 0xe, 0x0}, {0x104b80e8, 0x11e, 0, 0xe, 0x0}, {0x506100e8, 0x11c, 0, 0xe, 0xffff}};
extern "C" GXOamAttr data_ov095_02295c7c[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002340b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295ca4[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002240b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295ccc[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002140b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295cf4[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002d40b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295d1c[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002540b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295c2c[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002540b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_0229586c[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002940b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295894[5] = {{0x418d40f2, 0xd8, 0, 0xc, 0x0}, {0x418100f0, 0x118, 0, 0xc, 0x0}, {0x419d00f0, 0x11c, 0, 0xc, 0x0}, {0x01ad80f0, 0x11e, 0, 0xc, 0x0}, {0x419100f0, 0x11c, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_022958bc[5] = {{0x405340f2, 0xf8, 0, 0xe, 0x0}, {0x506f00f0, 0x118, 0, 0xc, 0x0}, {0x505300f0, 0x11c, 0, 0xc, 0x0}, {0x104b80f0, 0x11e, 0, 0xc, 0x0}, {0x506100f0, 0x11c, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295b8c[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002940b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_022958e4[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002e40b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_0229590c[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002c40b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295934[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002b40b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295aec[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002d40b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_0229595c[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002a40b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295984[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002840b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_022959ac[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002740b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_022959d4[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002640b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_022959fc[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002440b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295a24[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002340b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295a4c[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002240b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295a74[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002140b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295a9c[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x002140b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295ac4[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0x0}, {0x001e40b8, 0x1a0, 0, 0xe, 0x0}, {0x002140b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295b14[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002c40b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295b3c[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002b40b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295b64[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002a40b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295bb4[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002840b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295bdc[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002740b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295c04[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002640b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295c54[5] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002440b8, 0x1a2, 0, 0xc, 0x0}, {0x001f40b8, 0x1a0, 0, 0xe, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_022957ac[4] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002f40b8, 0x1a0, 0, 0xe, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0xffff}};
extern "C" char *sKeyboardModeCharFiles[8] = {data_ov095_02295648, data_ov095_022955e4, data_ov095_022955f8, data_ov095_022955d0, data_ov095_02295620, data_ov095_02295620, data_ov095_022955a8, data_ov095_022955bc};
extern "C" char *sKeyboardModeScreensChat[8] = {data_ov095_0229565c, data_ov095_0229571c, data_ov095_0229577c, data_ov095_02295734, data_ov095_02295764, data_ov095_02295764, data_ov095_0229574c, data_ov095_02295794};
extern "C" GXOamAttr data_ov095_0229580c[4] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002040b8, 0x1a0, 0, 0xe, 0x0}, {0x002f40b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_0229582c[4] = {{0x800f40b5, 0x184, 0, 0xc, 0x0}, {0x802540b5, 0x187, 0, 0xc, 0x0}, {0x002040b8, 0x1a2, 0, 0xc, 0x0}, {0x003040b8, 0x1a2, 0, 0xc, 0xffff}};
extern "C" char *sKeyboardModeScreens[8] = {data_ov095_0229568c, data_ov095_022956a4, data_ov095_02295674, data_ov095_022956bc, data_ov095_022956d4, data_ov095_022956d4, data_ov095_022956ec, data_ov095_02295704};
extern "C" char data_ov095_02295704[24] = "menu/chat2/b_key8.bsc";
extern "C" char data_ov095_0229574c[24] = "menu/chat2/b_chtk9.bsc";
extern "C" char data_ov095_022956d4[24] = "menu/chat2/b_key4.bsc";
extern "C" char data_ov095_0229565c[24] = "menu/chat2/b_cht.bsc";
extern "C" char data_ov095_022956ec[24] = "menu/chat2/b_key9.bsc";
extern "C" char data_ov095_0229577c[24] = "menu/chat2/b_chtk6.bsc";
extern "C" char data_ov095_0229568c[24] = "menu/chat2/b_key0.bsc";
extern "C" char data_ov095_022956a4[24] = "menu/chat2/b_key1.bsc";
extern "C" char data_ov095_022956bc[24] = "menu/chat2/b_key7.bsc";
extern "C" char data_ov095_02295674[24] = "menu/chat2/b_key6.bsc";
extern "C" char data_ov095_02295734[24] = "menu/chat2/b_chtk7.bsc";
extern "C" char data_ov095_0229571c[24] = "menu/chat2/b_chtk1.bsc";
extern "C" char data_ov095_02295794[24] = "menu/chat2/b_chtk8.bsc";
extern "C" char data_ov095_02295764[24] = "menu/chat2/b_chtk4.bsc";
extern "C" char data_ov095_02295620[20] = "menu/chat2/key4.bch";
extern "C" char data_ov095_02295648[20] = "menu/chat2/key0.bch";
extern "C" char data_ov095_022955f8[20] = "menu/chat2/key6.bch";
extern "C" char data_ov095_022955bc[20] = "menu/chat2/key8.bch";
extern "C" char data_ov095_022955a8[20] = "menu/chat2/key9.bch";
extern "C" char data_ov095_022955d0[20] = "menu/chat2/key7.bch";
extern "C" char data_ov095_022955e4[20] = "menu/chat2/key1.bch";
extern "C" s32 data_ov095_0229560c[5] = {59, 71, 82, 93, 104};
extern "C" s32 data_ov095_02295634[5] = {70, 81, 92, 103, 111};
extern "C" GXOamAttr *sKeyboardPageCells[4] = {data_ov095_022963e4, data_ov095_02296214, data_ov095_022960dc, data_ov095_022965bc};
extern "C" const s32 data_ov095_02295474[4] = {0, 59, 112, 146};
extern "C" const s32 data_ov095_02295454[4] = {45, 70, 121, 156};
extern "C" const s32 data_ov095_02295484[4] = {28, 4, 8, 8};
extern "C" const s32 data_ov095_02295464[4] = {55, 81, 121, 198};
extern "C" GXOamAttr data_ov095_02295590[1] = {{0x01ff8000, 0x100, 0, 0xc, 0xffff}};
extern "C" GXOamAttr data_ov095_02295588[1] = {{0x805400cb, 0xd4, 0, 0x6, 0xffff}};
extern "C" GXOamAttr data_ov095_02295580[1] = {{0x805400ce, 0xd4, 0, 0x1, 0xffff}};

BOOL Keyboard_IsSlotDisabled(Keyboard *s, s32 i) {
    u32 v = s->disabledSlots;
    if ((v & (1 << i)) != 0) return TRUE;
    return FALSE;
}

void Keyboard_DisableKey(Keyboard *s, s32 i) {
    s->disabledSlots |= (1 << i);
    switch (s->page) {
    case 0: Keyboard_SetPage0KeyPalette(s, data_ov095_0229553c[i], 0xe); break;
    case 1: Keyboard_SetPage1KeyPalette(s, data_ov095_02295494[i], 0xe); break;
    case 2: Keyboard_SetPage2KeyPalette(s, data_ov095_022954cc[i], 0xe); break;
    case 3: Keyboard_SetPage3KeyPalette(s, data_ov095_02295504[i], 0xe); break;
    }
    Keyboard_SetFlags(s, 1);
}

void Keyboard_EnableKey(Keyboard *s, s32 i) {
    s->disabledSlots &= ~(1 << i);
    switch (s->page) {
    case 0: Keyboard_SetPage0KeyPalette(s, data_ov095_0229553c[i], 0xc); break;
    case 1: Keyboard_SetPage1KeyPalette(s, data_ov095_02295494[i], 0xc); break;
    case 2: Keyboard_SetPage2KeyPalette(s, data_ov095_022954cc[i], 0xc); break;
    case 3: Keyboard_SetPage3KeyPalette(s, data_ov095_02295504[i], 0xc); break;
    }
    Keyboard_SetFlags(s, 1);
}

BOOL Keyboard_IsKeyDisabled(Keyboard *s, s32 a)
{
    s32 i;
    const s32 *tbl;
    if (a == -1) {
        return FALSE;
    }
    switch (s->page) {
    case 0:
        tbl = data_ov095_0229553c;
        break;
    case 1:
        tbl = data_ov095_02295494;
        break;
    case 2:
        tbl = data_ov095_022954cc;
        break;
    case 3:
        tbl = data_ov095_02295504;
        break;
    default:
        return FALSE;
    }
    for (i = 0; i < 0xe; i++) {
        if (a == tbl[i]) {
            if (Keyboard_IsSlotDisabled(s, i)) {
                return TRUE;
            }
            return FALSE;
        }
    }
    return FALSE;
}

BOOL Keyboard_IsMenuTabKey(Keyboard *s, s32 a)
{
    if (a >= 0xcd && a <= 0xd4) {
        return TRUE;
    }
#ifdef N3DS_DIRECT
    if (a == N3DS_KB_OPT_KEY) {
        return TRUE;
    }
#endif
    return FALSE;
}

BOOL Keyboard_IsOnMenuTabKey(Keyboard *s, s32 a)
{
    if (a == -1) {
        a = s->cursorKey;
    }
    return Keyboard_IsMenuTabKey(s, a);
}

BOOL Keyboard_IsOnButtonKey(Keyboard *s, s32 a)
{
    if (a == -1) {
        a = s->cursorKey;
    }
    switch (a) {
    case 0xd6:
    case 0xd7:
    case 0xd8:
    case 0xd9:
    case 0xda:
        return TRUE;
    }
    return FALSE;
}

BOOL Keyboard_IsFull(Keyboard *s)
{
    return Keyboard_TestFlags(s, 0x40);
}

BOOL Keyboard_IsTooWide(Keyboard *s)
{
    return Keyboard_TestFlags(s, 0x80);
}

void Keyboard_ResetKeyPalettes(Keyboard *s)
{
    GXOamAttr *p;
    s32 i;
    p = Keyboard_GetPageCells(s);
    if (p != NULL) {
        for (i = 0; i < 0x64; i++) {
            p[i].cParam = 0xc;
            if (p[i]._3 == 0xffff) {
                i = 0x64;
            }
        }
    }
    for (i = 0; i < 0xe; i++) {
        if (Keyboard_IsSlotDisabled(s, i)) {
            Keyboard_DisableKey(s, i);
        }
    }
    Keyboard_UpdateShiftKey(s);
    Keyboard_SetFlags(s, 1);
}

void Keyboard_ClearHighlight(Keyboard *s)
{
    s32 i;
    if (s->pressedKey != -1) {
        Keyboard_SetKeyPalette(s, s->pressedKey, 0xc);
        s->pressedKey = -1;
    }
    for (i = 0; i < 0xe; i++) {
        if (Keyboard_IsSlotDisabled(s, i)) {
            Keyboard_DisableKey(s, i);
        }
    }
    Keyboard_UpdateShiftKey(s);
    Keyboard_SetFlags(s, 1);
}

void Keyboard_UpdateShiftKey(Keyboard *s)
{
    if (s->mode != 3) {
        Keyboard_ClearFlags(s, 0x20);
    }
    if (s->mode != 2 && s->mode == 3) {
        if (Keyboard_TestFlags(s, 0x20)) {
            Keyboard_SetPage1KeyPalette(s, 0x5d, 0xd);
        } else {
            Keyboard_SetPage1KeyPalette(s, 0x52, 0xd);
        }
    }
}

void Keyboard_SetPage0KeyPalette(Keyboard *s, s32 a, s32 b)
{
    GXOamAttr *p;
    if (Keyboard_SetCopyPastePalette(s, a, b) != 0) {
        return;
    }
    if (a < 0 || a > 0x3a) {
        return;
    }
    p = Keyboard_GetPageCells(s);
    if (a >= 0 && a <= 0x31) {
        a += 5;
    } else if (a >= 0x32 && a <= 0x36) {
        a -= 0x32;
    } else if (a >= 0x37 && a <= 0x3a) {
    } else {
        return;
    }
    p[a].cParam = b;
}

void Keyboard_SetPage1KeyPalette(Keyboard *s, s32 a, s32 b)
{
    GXOamAttr *p;
    s32 i;
    if (Keyboard_SetCopyPastePalette(s, a, b) != 0) {
        return;
    }
    if (a < 0x3b || a > 0x6f) {
        return;
    }
    p = Keyboard_GetPageCells(s);
    if (a >= 0x3b && a <= 0x46) {
        i = a - 0x3b;
    } else if (a >= 0x47 && a <= 0x50) {
        i = a - 0x3b;
    } else if (a == 0x51) {
        i = 0x16;
        p[23].cParam = b;
    } else if (a >= 0x52 && a <= 0x5b) {
        i = a - 0x3a;
    } else if (a == 0x5c) {
        i = 0x22;
        p[35].cParam = b;
        p[36].cParam = b;
    } else if (a >= 0x5d && a <= 0x67) {
        i = a - 0x38;
    } else if (a >= 0x68 && a <= 0x69) {
        i = a - 0x38;
    } else if (a == 0x6b) {
        i = 0x32;
        p[51].cParam = b;
        p[52].cParam = b;
        p[53].cParam = b;
        p[54].cParam = b;
        p[55].cParam = b;
    } else if (a >= 0x6e && a <= 0x6f) {
        i = a - 0x36;
    } else {
        return;
    }
    p[i].cParam = b;
}

void Keyboard_SetPage2KeyPalette(Keyboard *s, s32 a, s32 b)
{
    GXOamAttr *p;
    s32 i;
    if (Keyboard_SetCopyPastePalette(s, a, b) != 0) {
        return;
    }
    if (a < 0x70 || a > 0x91) {
        return;
    }
    p = Keyboard_GetPageCells(s);
    if (a >= 0x70 && a <= 0x79) {
        i = a - 0x70;
    } else if (a >= 0x7a && a <= 0x83) {
        i = a - 0x6e;
    } else if (a >= 0x84 && a <= 0x8d) {
        i = a - 0x6c;
    } else {
        a -= 0x8e;
        switch (a) {
        case 0:
            i = 0xa;
            break;
        case 1:
            i = 0xb;
            break;
        case 2:
            i = 0x16;
            p[23].cParam = b;
            break;
        case 3:
            i = 0x22;
            p[35].cParam = b;
            p[36].cParam = b;
            p[37].cParam = b;
            p[38].cParam = b;
            break;
        default:
            return;
        }
    }
    p[i].cParam = b;
}

void Keyboard_SetPage3KeyPalette(Keyboard *s, s32 a, s32 b)
{
    GXOamAttr *p;
    s32 i;
    if (Keyboard_SetCopyPastePalette(s, a, b) != 0) {
        return;
    }
    if (a < 0x92 || a > 0xc8) {
        return;
    }
    p = Keyboard_GetPageCells(s);
    if (a >= 0x92 && a <= 0x9c) {
        i = a - 0x92;
    } else if (a >= 0x9d && a <= 0xa7) {
        i = a - 0x90;
    } else if (a >= 0xa8 && a <= 0xb2) {
        i = a - 0x8e;
    } else if (a >= 0xb3 && a <= 0xbd) {
        i = a - 0x8e;
    } else if (a >= 0xbe && a <= 0xc5) {
        i = a - 0x8e;
    } else {
        switch (a) {
        case 0xc6:
            i = 0xb;
            p[12].cParam = b;
            break;
        case 0xc7:
            i = 0x18;
            p[25].cParam = b;
            break;
        case 0xc8:
            i = 0x38;
            p[57].cParam = b;
            p[58].cParam = b;
            p[59].cParam = b;
            break;
        default:
            return;
        }
    }
    p[i].cParam = b;
}

s32 Keyboard_SetCopyPastePalette(Keyboard *s, s32 a, s32 b)
{
    if (a == 0xdb) {
        goto body;
    }
    if (a == 0xdc) {
body:
        switch (b) {
        case 0xc:
            b = 0xc;
            break;
        case 0xe:
            b = 0xe;
            break;
        case 0xd:
            b = 0xd;
            break;
        }
        if (a == 0xdb) {
            s->copyKeyPalette = b;
        } else {
            s->pasteKeyPalette = b;
        }
    }
    return 0;
}

void Keyboard_SetKeyPalette(Keyboard *s, s32 a, s32 b)
{
    switch (s->page) {
    case 0:
        Keyboard_SetPage0KeyPalette(s, a, b);
        break;
    case 1:
        Keyboard_SetPage1KeyPalette(s, a, b);
        break;
    case 2:
        Keyboard_SetPage2KeyPalette(s, a, b);
        break;
    case 3:
        Keyboard_SetPage3KeyPalette(s, a, b);
        break;
    }
    Keyboard_SetFlags(s, 1);
}

void Keyboard_HighlightKey(Keyboard *s, s32 a)
{
    Keyboard_SetKeyPalette(s, a, 0xd);
}

s32 Keyboard_TouchPage1Key(Keyboard *s, u32 a, u32 b)
{
    if (b < 0x60) {
        return -1;
    }
    if (b >= 0xb0) {
        return -1;
    }
    if (b < 0x70) {
        if (a < 4) {
            return -1;
        }
        if (a >= 0xf4) {
            return -1;
        }
        return _s32_div_f(a - 4, 0x14) + 0x3b;
    }
    if (b < 0x80) {
        if (a < 0xc) {
            return -1;
        }
        if (a >= 0xfc) {
            return -1;
        }
        if (a >= 0xd4) {
            return 0x51;
        }
        return _s32_div_f(a - 0xc, 0x14) + 0x47;
    }
    if (b < 0x90) {
        if (a < 4) {
            return -1;
        }
        if (a < 0x18) {
            return 0x52;
        }
        if (a >= 0xfc) {
            return -1;
        }
        if (a >= 0xcc) {
            return 0x5c;
        }
        return _s32_div_f(a - 0x18, 0x14) + 0x53;
    }
    if (b < 0xa0) {
        if (a < 4) {
            return -1;
        }
        if (a < 0x20) {
            return 0x5d;
        }
        if (a >= 0xe8) {
            return -1;
        }
        return _s32_div_f(a - 0x20, 0x14) + 0x5e;
    }
    if (b < 0xb0) {
        if (a < 0x18) {
            return -1;
        }
        if (a >= 0x40 && a < 0xcc) {
            return 0x6b;
        }
        if (a < 0x54) {
            return _s32_div_f(a - 0x18, 0x14) + 0x68;
        }
        if (a >= 0xf4) {
            return -1;
        }
        return _s32_div_f(a - 0xa4, 0x14) + 0x6c;
    }
    return -1;
}

s32 Keyboard_TouchPage2Key(Keyboard *s, u32 a, u32 b)
{
    if (b < 0x68) {
        return -1;
    }
    if (b >= 0xa8) {
        return -1;
    }
    if (b >= 0x98) {
        if (a >= 0x50 && a < 0xb0) {
            return 0x91;
        }
        return -1;
    }
    if (a < 0x8) {
        return -1;
    }
    if (a >= 0xf8) {
        return -1;
    }
    if (a < 0xd0) {
        {
            s32 c = _s32_div_f(a - 8, 0x14);
            s32 rw = ((s32)(b - 0x68) >> 4) * 10;
            return c + rw + 0x70;
        }
    }
    if (b >= 0x78) {
        return 0x90;
    }
    if (a < 0xe4) {
        return 0x8e;
    }
    return 0x8f;
}

s32 Keyboard_TouchPage3Key(Keyboard *s, u32 a, u32 b)
{
    s32 row;
    s32 col;
    if (b < 0x60 || b >= 0xb0) {
        return -1;
    }
    if (a < 0x8 || a >= 0xf8) {
        return -1;
    }
    row = (s32)(b - 0x60) >> 4;
    col = _s32_div_f(a - 8, 0x14);
    if (row == 4) {
        if (col >= 8) {
            return 0xc8;
        }
        return col + 0x92 + row * 0xb;
    }
    if (col >= 0xb) {
        if (b < 0x78) {
            return 0xc6;
        }
        return 0xc7;
    }
    return col + 0x92 + row * 0xb;
}

s32 Keyboard_TouchCopyPasteKey(Keyboard *s, u32 a, u32 b)
{
    if (Keyboard_TestFlags(s, 4)) {
        if (b >= 0x50 && b <= 0x58) {
            if (a < 0x30) {
                return 0xdb;
            }
            if (a > 0xd0) {
                return 0xdc;
            }
        }
    } else {
        if (b >= 0x48 && b <= 0x50) {
            if (a < 0x30) {
                return 0xdb;
            }
            if (a > 0xd0) {
                return 0xdc;
            }
        }
    }
    return -1;
}

s32 Keyboard_TouchKey(Keyboard *s, s32 a, u32 b)
{
    s32 r;
    s->pressedKey = -1;
    r = Keyboard_TouchCopyPasteKey(s, a, b);
    if (r == -1) {
        if (Keyboard_TestFlags(s, 4) == 0) {
            if (b < 0xf7) {
                b = (u8)(b + 8);
            } else {
                b = 0xff;
            }
        }
        switch (s->page) {
        case 0:
            break;
        case 1:
            r = Keyboard_TouchPage1Key(s, a, b);
            if (r == 0x6a || (u32)(r - 0x6c) <= 1) {
                r = 0x6b;
            }
            break;
        case 2:
            r = Keyboard_TouchPage2Key(s, a, b);
            break;
        case 3:
            r = Keyboard_TouchPage3Key(s, a, b);
            break;
        default:
            return -1;
        }
    }
    if (Keyboard_IsKeyDisabled(s, r)) {
        return -1;
    }
    s->pressedKey = r;
    return r;
}

s32 Keyboard_GetPressedKey(Keyboard *s)
{
    return s->pressedKey;
}

s32 Keyboard_GetSpecialKeyCode(Keyboard *s, s32 a)
{
    if (a >= 0xc9 && a <= 0xe1) {
        return a + 0x3d;
    }
#ifdef N3DS_DIRECT
    if (a == N3DS_KB_OPT_KEY) {
        return N3DS_KB_OPT_CODE;
    }
#endif
    return -1;
}

s32 Keyboard_GetKeyCodePage0Special(Keyboard *s, s32 a)
{
    if (a == 0x26) {
        return 0x87;
    }
    if (a == 0x27) {
        return 0x9b;
    }
    if (a == 0x37) {
        return 0xc9;
    }
    if (a == 0x3a) {
        return 0x85;
    }
    if (a == 0x30) {
        return 0x9c;
    }
    if (a == 0x31) {
        return 0x9c;
    }
    if (a == 0x39) {
        return 0x86;
    }
    if (a == 0x38) {
        return 0x100;
    }
    if (a >= 0x32 && a <= 0x36) {
        return a + 0xcf;
    }
    return 0;
}

s32 Keyboard_GetKeyCodePage0A(Keyboard *s, s32 a)
{
    if (a >= 0 && a <= 0x25) {
        return a + 0x9c;
    }
    if (a >= 0x28 && a <= 0x2f) {
        return a + 0x74;
    }
    return Keyboard_GetKeyCodePage0Special(s, a);
}

s32 Keyboard_GetKeyCodePage0B(Keyboard *s, s32 a)
{
    if (a >= 0 && a <= 0x25) {
        return a + 0x9c;
    }
    if (a >= 0x28 && a <= 0x2f) {
        return a + 0x74;
    }
    return Keyboard_GetKeyCodePage0Special(s, a);
}

s32 Keyboard_GetKeyCodePage1Lower(Keyboard *s, s32 a)
{
    if (a >= 0x3b && a <= 0x6f) {
        return sKeyCodesPage1Lower[a - 0x3b];
    }
    return 0;
}

s32 Keyboard_GetKeyCodePage1Upper(Keyboard *s, s32 a)
{
    if (a >= 0x3b && a <= 0x6f) {
        return sKeyCodesPage1Upper[a - 0x3b];
    }
    return 0;
}

s32 Keyboard_GetKeyCodePage1Alt(Keyboard *s, s32 a)
{
    if (a >= 0x3b && a <= 0x6f) {
        return sKeyCodesPage1Alt[a - 0x3b];
    }
    return 0;
}

s32 Keyboard_GetKeyCodePage2(Keyboard *s, s32 a)
{
    if (a >= 0x70 && a <= 0x91) {
        return sKeyCodesPage2[a - 0x70];
    }
    return 0;
}

s32 Keyboard_GetKeyCodePage3(Keyboard *s, s32 a)
{
    if (a >= 0x92 && a <= 0xc8) {
        return sKeyCodesPage3[a - 0x92];
    }
    return 0;
}

s32 Keyboard_GetKeyCode(Keyboard *s, s32 a, u32 b)
{
    s32 r;
    if (b >= 8) {
        b = s->mode;
    }
    r = Keyboard_GetSpecialKeyCode(s, a);
    if (r >= 0) {
        return r;
    }
    switch (b) {
    case 0:
        return Keyboard_GetKeyCodePage0A(s, a);
    case 1:
        return Keyboard_GetKeyCodePage0B(s, a);
    case 2:
        return Keyboard_GetKeyCodePage1Lower(s, a);
    case 3:
        return Keyboard_GetKeyCodePage1Upper(s, a);
    case 4:
    case 5:
        return Keyboard_GetKeyCodePage1Alt(s, a);
    case 6:
        return Keyboard_GetKeyCodePage2(s, a);
    case 7:
        return Keyboard_GetKeyCodePage3(s, a);
    }
    return 0x85;
}

void Keyboard_RestoreLastPage(Keyboard *s, s32 a)
{
    s->page = MenuCtrl_GetKeyboardPage();
    Keyboard_SetMode(s, MenuCtrl_GetKeyboardPageMode(s->page), a, 0);
}

void Keyboard_SetMode(Keyboard *s, s32 mode, s32 a, s32 c)
{
    Heap *g;
    u8 *h;
    char *path;
    if (mode == 8) {
        mode = MenuCtrl_GetKeyboardPageMode(s->page);
    }
    if (mode == 3 && Keyboard_TestFlags(s, 0x20) != 0) {
        MenuCtrl_SetKeyboardPageMode(s->page, 2);
    } else {
        MenuCtrl_SetKeyboardPageMode(s->page, mode);
    }
    g = gCurrentHeap;
    if (c == 0) {
        Gfx2d_LoadPaletteFile("menu/chat2/b_cht_bg.bpl", g, a, 1, 1, 4);
        Gfx2d_LoadPaletteFile("menu/chat2/b_cht_bg.bpl", g, a, 1, 7, 7);
        Gfx2d_LoadPaletteFile("menu/chat2/b_cht_bg.bpl", g, a, 1, 9, 9);
    }
    if (s->mode == mode && c != 0) {
        goto end;
    }
    File_LoadToBuffer(sKeyboardModeCharFiles[mode], s->keyChars, 0x22c0);
    if (Keyboard_TestFlags(s, 4) != 0) {
        path = sKeyboardModeScreensChat[mode];
    } else {
        path = sKeyboardModeScreens[mode];
    }
    h = (u8 *)File_LoadAlloc(path, g, -4, 0);
    MI_CpuCopy8(h + 0x240, &s->screenBuf[576], 0x380);
    Keyboard_MarkScreenDirty(s);
    Heap_Free(g, h);
    s->bgTasks[0].requestChars(s->keyChars, a, 0x1ea, 0x1ea, 0x2ff);
    s->mode = mode;
    switch (s->mode) {
    case 0:
        Keyboard_DisableKey(s, 1);
        Keyboard_EnableKey(s, 2);
        break;
    case 1:
        Keyboard_EnableKey(s, 1);
        Keyboard_DisableKey(s, 2);
        break;
    case 2:
    case 3:
        Keyboard_DisableKey(s, 8);
        Keyboard_EnableKey(s, 9);
        Keyboard_EnableKey(s, 10);
        Keyboard_EnableKey(s, 7);
        Keyboard_EnableKey(s, 13);
        break;
    case 4:
        Keyboard_EnableKey(s, 8);
        Keyboard_DisableKey(s, 9);
        Keyboard_EnableKey(s, 10);
        Keyboard_DisableKey(s, 7);
        Keyboard_DisableKey(s, 13);
        break;
    case 5:
        Keyboard_EnableKey(s, 8);
        Keyboard_EnableKey(s, 9);
        Keyboard_DisableKey(s, 10);
        Keyboard_DisableKey(s, 7);
        Keyboard_DisableKey(s, 13);
        break;
    case 6:
        break;
    }
    Keyboard_ResetTypedRun(s);
end:
    Keyboard_ResetKeyPalettes(s);
}

void Keyboard_Reload(Keyboard *s, s32 a)
{
    Keyboard_SetMode(s, s->mode, a, 0);
    Keyboard_EndFrame(s, a);
}

void Keyboard_SelectPageByCode(Keyboard *s, s32 a, s32 b)
{
    switch (a) {
    case 0x107:
        s->page = 1;
        break;
    case 0x108:
        s->page = 3;
        break;
    case 0x109:
        s->page = 2;
        break;
    }
    Keyboard_SetMode(s, 8, b, 1);
}

void Keyboard_ToggleCapsLock(Keyboard *s, s32 a)
{
    if (Keyboard_TestFlags(s, 0x20) != 0) {
        s->mode = 2;
        Keyboard_ClearFlags(s, 0x20);
    }
    switch (s->mode) {
    case 2:
        Keyboard_SetMode(s, 3, a, 1);
        break;
    case 3:
        Keyboard_SetMode(s, 2, a, 1);
        break;
    }
}

void Keyboard_ToggleOneShotShift(Keyboard *s, s32 a)
{
    if (Keyboard_TestFlags(s, 0x20) != 0) {
        Keyboard_ClearFlags(s, 0x20);
        Keyboard_SetMode(s, 2, a, 1);
    } else {
        Keyboard_SetFlags(s, 0x20);
        Keyboard_SetMode(s, 3, a, 1);
    }
}

void Keyboard_EndOneShotShift(Keyboard *s, s32 a)
{
    if (Keyboard_TestFlags(s, 0x20) != 0) {
        Keyboard_ClearFlags(s, 0x20);
        Keyboard_SetMode(s, 2, a, 1);
    }
}

void Keyboard_Init(Keyboard *s, s32 a)
{
    s->disabledSlots = 0;
    s->mode = 8;
    s->pressedKey = -1;
    s->flags = 0;
    s->selectedEmotion = 0xff;
    s->emotionCount = 0;
    s->cursorKey = -1;
    s->cursorX = 100;
    s->cursorY = 100;
    Keyboard_ResetTypedRun(s);
    if (a == 0) {
        Keyboard_SetFlags(s, 4);
        s->tabCells = data_ov095_02295d44;
    } else {
        s->tabCells = data_ov095_02295d74;
    }
    s->layout = a;
    s->copyKeyPalette = 0xc;
    s->pasteKeyPalette = 0xc;
    s->page = 1;
    s->tabCells[0].attr2 = (s->tabCells[0].attr2 & ~0x3ff) | 0x103;
    s->tabCells[1].attr2 = (s->tabCells[1].attr2 & ~0x3ff) | 0x107;
    PlayerData *pd;
    if ((pd = PlayerData_GetCurrent()) != NULL) {
        if (pd->getPlayerId()->getGender() == 0) {
            Snd_SetKeySeMode(0);
            return;
        }
    }
    Snd_SetKeySeMode(1);
}

void Keyboard_Shutdown(Keyboard *s)
{
    s->bgTasks[0].cancel();
    s->bgTasks[1].cancel();
    s->labels[0].destroyLabel();
    s->labels[1].destroyLabel();
    MenuCtrl_SetKeyboardPage(s->page);
}

void Keyboard_SetAltWriteLayout(Keyboard *s)
{
    Keyboard_SetFlags(s, 8);
}

void Keyboard_LoadLetterChars(Keyboard *s, s32 a)
{
    Gfx2d_LoadCharFile("menu/letter/b_ltr.bch", gCurrentHeap, a, 0x13d, 0x1eb, 0x1f0);
}

void Keyboard_LoadScreenFile(Keyboard *s, const char *a)
{
    File_LoadToBuffer(a, s->screenBuf, 0x800);
}

void Keyboard_LoadScreenNow(Keyboard *s, s32 a)
{
    Gfx2d_LoadScreen(s->screenBuf, a, 0x800, 0);
    Keyboard_ClearFlags(s, 1);
}

void Keyboard_EndFrame(Keyboard *s, s32 a)
{
    if (Keyboard_TestFlags(s, 1) != 0) {
        if (s->bgTasks[1].requestScreen(s->screenBuf, a, 0x800, 0) != 0) {
            Keyboard_ClearFlags(s, 1);
        }
    }
    Keyboard_ClearFlags(s, 2);
    Keyboard_ClearFlags(s, 0x200);
}

void Keyboard_MarkScreenDirty(Keyboard *s)
{
    Keyboard_SetFlags(s, 1);
}

s32 Keyboard_UpdatePressedKey(Keyboard *s)
{
    if (s->pressTimer != 0) {
        s->pressTimer--;
        if (s->pressTimer >= 4) {
            return 1;
        }
        if (s->pressTimer == 0) {
            Keyboard_ClearHighlight(s);
        }
    }
    return 0;
}

void Keyboard_StartKeyRepeat(Keyboard *s)
{
    s->pressTimer = 5;
    s->keyRepeatTimer = 0xd;
}

BOOL Keyboard_TickKeyRepeat(Keyboard *s)
{
    if (s->pressTimer > 1) {
        s->pressTimer--;
    }
    if (s->keyRepeatTimer != 0) {
        s->keyRepeatTimer--;
    } else {
        s->keyRepeatTimer = 1;
        return TRUE;
    }
    return FALSE;
}

void Keyboard_DisableModifierKeys(Keyboard *s)
{
    Keyboard_DisableKey(s, 6);
    Keyboard_DisableKey(s, 3);
    Keyboard_DisableKey(s, 4);
    Keyboard_DisableKey(s, 5);
}

void Keyboard_UpdateModifierKeys(Keyboard *s, s32 a)
{
    Keyboard_EnableKey(s, 6);
    if (Keyboard_ModifyCharKey103(s, a) == 0) {
        Keyboard_DisableKey(s, 3);
    } else {
        Keyboard_EnableKey(s, 3);
    }
    if (Keyboard_ModifyCharKey104(s, a) == 0) {
        Keyboard_DisableKey(s, 4);
    } else {
        Keyboard_EnableKey(s, 4);
    }
    if (Keyboard_ModifyCharKey105(s, a) == 0) {
        Keyboard_DisableKey(s, 5);
    } else {
        Keyboard_EnableKey(s, 5);
    }
}

BOOL Keyboard_IsControlCode(Keyboard *s, s32 a)
{
    if (a >= 0x100) {
        return TRUE;
    }
    return FALSE;
}

BOOL Keyboard_CanInsert(Keyboard *s, u8 *a, s32 b, s32 c, s32 d, s32 e)
{
    s32 n;
    if (d == c) {
        if (Keyboard_TestFlags(s, 0x100) == 0) {
            Keyboard_SetFlags(s, 0x40);
        }
        return FALSE;
    }
    n = Text_GetLength(a, d);
    if (n == d && a[n - 1] != 0x85) {
        if (Keyboard_TestFlags(s, 0x100) == 0) {
            Keyboard_SetFlags(s, 0x40);
        }
        return FALSE;
    }
    if (e < 0) {
        return TRUE;
    }
    d = Text_MeasureWidth(a, d);
    if (d + Text_GetCharWidth((u32)b) > e) {
        if (Keyboard_TestFlags(s, 0x100) == 0) {
            Keyboard_SetFlags(s, 0x80);
        }
        return FALSE;
    }
    return TRUE;
}

BOOL Keyboard_InsertAt(Keyboard *s, u8 *a, s32 b, s32 c, s32 d, s32 e)
{
    s32 i;
    if (Keyboard_CanInsert(s, a, b, c, d, e) == 0) {
        return FALSE;
    }
    for (i = d - 1; i > c; i--) {
        a[i] = a[i - 1];
    }
    a[c] = b;
    return TRUE;
}

BOOL Keyboard_InsertChar(Keyboard *s, u8 *a, s32 b, u8 *p, s32 c, s32 d, s32 e, s32 f)
{
    if (e != 0) {
        Keyboard_SetFlags(s, 0x100);
    } else {
        Keyboard_ClearFlags(s, 0x40);
        Keyboard_ClearFlags(s, 0x80);
        Keyboard_ClearFlags(s, 0x100);
    }
    if (Keyboard_InsertAt(s, a, b, *p, c, d) != 0) {
        if (f != 0 && e == 0) {
            Keyboard_PlayCharSe(s, b);
        }
        *p = *p + 1;
        return TRUE;
    }
    return FALSE;
}

BOOL Keyboard_InsertCharMultiline(Keyboard *s, void *p1, s32 p2, u8 *p3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9)
{
    u8 loc;
    s32 o24;
    s32 o28;
    struct Pad { s32 v[6]; Pad() {} ~Pad() {} } pad;
    u8 saved = s->typedRunLength;
    Heap *heap = gCurrentHeap;
    void *buf = Heap_AllocTail(heap, 0xc0);
    if (buf == NULL) {
        return FALSE;
    }
    Mem_Copy(p1, buf, a4);
    loc = *p3;
    if (Keyboard_InsertChar(s, (u8 *)buf, p2, &loc, a4, 0x2710, a8, 0) == 0) {
        Heap_Free(heap, buf);
        return FALSE;
    }
    if (Text_SplitLines((u8 *)buf, &o28, &o24, a4, a5, a7, a6) == 0) {
        if (s->typedRunLength != saved) {
            s->typedRunLength = saved;
        }
        Heap_Free(heap, buf);
        if (Keyboard_TestFlags(s, 0x100) == 0) {
            Keyboard_SetFlags(s, 0x80);
        }
        return FALSE;
    }
    if (a8 == 0) {
        Mem_Copy(buf, p1, a4);
        *p3 = loc;
        if (a9 == 1 && loc != 0) {
            Keyboard_PlayCharSe(s, ((u8 *)p1)[loc - 1]);
        }
    }
    Heap_Free(heap, buf);
    return TRUE;
}

s32 Keyboard_DeleteRange(void *s, u8 *buf, s32 a, s32 b, s32 n)
{
    s32 lo, hi, j, t;
    if (a > b) {
        lo = b;
        hi = a;
    } else {
        lo = a;
        hi = b;
    }
    for (j = 0; j + hi < n; j++) {
        t = buf[j + hi];
        buf[j + lo] = t;
    }
    hi -= lo;
    t = 0;
    for (j = 1; j <= hi; j++) {
        buf[n - j] = t;
    }
    return lo;
}

BOOL Keyboard_ReplaceCharBeforeCursor(Keyboard *s, u8 *buf, s32 c, s32 n)
{
    if (n == 0) {
        return FALSE;
    }
    n--;
    buf[n] = c;
    Keyboard_PlayCharSe(s, c);
    return TRUE;
}

s32 Keyboard_ModifyCharKey103(Keyboard *s, s32 a)
{
    return 0;
}

s32 Keyboard_ModifyCharKey104(Keyboard *s, s32 a)
{
    return 0;
}

s32 Keyboard_ModifyCharKey105(Keyboard *s, s32 a)
{
    return 0;
}

u8 Keyboard_HitTestText(void *s, u8 *str, s32 a, s32 b, u8 limit0, u8 *out)
{
    s32 count;
    Text_FitToWidth(str, a, b, &count, 0);
    s32 i, w;
    w = 0;
    i = w;
    s32 limit = limit0;
    for (; i < count; i++) {
        s32 c = Text_GetCharWidth(str[i]);
        if (w + (c >> 1) > limit) {
            if (out) {
                *out = i;
            }
            return w;
        }
        w += c;
    }
    if (out) {
        *out = count;
    }
    return w;
}

s32 Keyboard_HandleModeKey(Keyboard *s, s32 a, s32 b)
{
    if (a == 0x86) {
        Keyboard_ResetTypedRun(s);
    }
    if (Keyboard_IsControlCode(s, a) == 0) {
        Keyboard_EndOneShotShift(s, b);
        return 0;
    }
    switch (a) {
    case 0x101:
        Keyboard_SetMode(s, 0, b, 1);
        Snd_PlaySe(0x18);
        return 2;
    case 0x102:
        Keyboard_SetMode(s, 1, b, 1);
        Snd_PlaySe(0x18);
        return 2;
    case 0x103:
    case 0x104:
    case 0x105:
    case 0x106:
        break;
    case 0x11f:
        Keyboard_ToggleOneShotShift(s, b);
        Snd_PlaySe(0x18);
        return 2;
    case 0x120:
        Keyboard_ToggleCapsLock(s, b);
        Snd_PlaySe(0x18);
        return 2;
    case 0x107:
    case 0x108:
    case 0x109:
        Keyboard_SelectPageByCode(s, a, b);
        Snd_PlaySe(0x18);
        return 2;
    case 0x121:
        Keyboard_SetMode(s, 4, b, 1);
        Snd_PlaySe(0x18);
        return 2;
    case 0x122:
        Keyboard_SetMode(s, 5, b, 1);
        Snd_PlaySe(0x18);
        return 2;
    case 0x123:
        Keyboard_SetMode(s, 2, b, 1);
        Snd_PlaySe(0x18);
        return 2;
    }
    Keyboard_EndOneShotShift(s, b);
    if (a == 0x100) {
        Keyboard_ShrinkTypedRun(s);
    }
    return 0;
}

void Keyboard_ResetTypedRun(Keyboard *s)
{
    s->typedRunLength = 0;
}

void Keyboard_ShrinkTypedRun(Keyboard *s)
{
    if (s->typedRunLength != 0) {
        s->typedRunLength--;
    }
}

u8 Keyboard_GetTypedRunLength(Keyboard *s)
{
    return s->typedRunLength;
}

void Keyboard_BeginPaste(Keyboard *s)
{
    Keyboard_SetFlags(s, 2);
}

void Keyboard_EndPaste(Keyboard *s)
{
    Keyboard_ClearFlags(s, 2);
}

void Keyboard_LoadObjGfx(Keyboard *s)
{
    Heap *h = gCurrentHeap;
    Gfx2d_LoadPaletteFile("menu/chat2/b_cht_obj.bpl", h, 8, 4, 4, 0xe);
    Gfx2d_LoadCharFile("menu/chat2/b_cht_obj_0.bch", h, 8, 0xc0, 0xc0, 0x13f);
    Gfx2d_LoadCharFile("menu/chat2/b_cht_obj_1.bch", h, 8, 0x180, 0x180, 0x1ff);
    String_Load2dMenu(&s->labels[0], 0x9c);
    s->labels[0].createSmallLabel(8, 0xd8, 4, 0xa, 0, 0);
    s->labels[0].redrawAligned(1, 0);
    String_Load2dMenu(&s->labels[1], 0x9d);
    s->labels[1].createSmallLabel(8, 0xf8, 4, 0xa, 0, 0);
    s->labels[1].redrawAligned(1, 0);
}

void Keyboard_LoadEmotionIcons(Keyboard *s)
{
    s32 i;
    s32 v;
    s32 off;
    u32 p;
    s->emotionCount = Emotion_CountLearned();
    for (i = 0; i < s->emotionCount; i++) {
        s->emotionSlots[i] = Emotion_GetSlot(i);
    }
    File_LoadToBuffer("menu/chat2/ten0.bch", s->emotionIconBuf, 0x1000);
    for (i = 0; i < s->emotionCount; i++) {
        v = s->emotionSlots[i] - 1;
        off = ((v & 0xf) << 6) + ((v >> 4) << 11);
        p = i * 2 + 0x109;
        Gfx2d_LoadCharRange(s->emotionIconBuf + off, 8, p, p, p + 1);
        Gfx2d_LoadCharRange(s->emotionIconBuf + (off + 0x400), 8, p + 0x20, p + 0x20, p + 0x21);
    }
}

void Keyboard_Draw(Keyboard *s, s32 x, s32 y, s32 z)
{
    s32 sel = 0;
    s32 i;
    switch (s->page) {
    case 1:
        break;
    case 2:
        sel = 2;
        break;
    case 3:
        sel = 1;
        break;
    }
    for (i = 0; i < 3; i++) {
        if (sel == i) {
            Oam_DrawObj(1, &s->tabCells[i], x, y + 2, 0xb, z, NULL);
        } else {
            Oam_DrawObj(1, &s->tabCells[i], x, y, 0xa, z, NULL);
        }
    }
    Oam_DrawCell(1, &s->tabCells[3], x, y, -1, z, 0x1000, 0x1000, 0, -1, 0, 0);
    if (Keyboard_TestFlags(s, 4) == 0) {
        y -= 8;
    }
    Keyboard_DrawPageKeys(s, x, y);
}

void Keyboard_DrawPageKeys(Keyboard *s, s32 x, s32 y)
{
    Oam_DrawCell(1, Keyboard_GetPageCells(s), x, y, -1, 3, 0x1000, 0x1000, 0, -1, 0, 0);
}

GXOamAttr *Keyboard_GetPageCells(Keyboard *s)
{
    u32 i = s->page;
    if (i >= 4) {
        return NULL;
    }
    return sKeyboardPageCells[i];
}

void Keyboard_DrawEmotionKeys(Keyboard *s, s32 x, s32 y)
{
    s32 i;
    s32 t10, t14;
    for (i = 0; i < s->emotionCount; i++) {
        t10 = -1;
        t14 = y;
        if (s->selectedEmotion == i) {
            t10 = 0xb;
            t14 = y + 2;
        }
        Oam_DrawObj(1, &data_ov095_02295e48[i], x, t14, -1, 2, NULL);
        Oam_DrawObj(1, &data_ov095_02295e48[i + 4], x, t14, t10, 2, NULL);
        Oam_DrawObj(1, &data_ov095_02295e48[i + 8], x, y, -1, 2, NULL);
    }
}

void Keyboard_DrawSendKey(void *s, s32 x, s32 y, s32 z, s32 w)
{
    Oam_DrawObj(1, data_ov095_02295588, x, y + w, z, 2, NULL);
    Oam_DrawObj(1, data_ov095_02295580, x, y, -1, 2, NULL);
}

BOOL Keyboard_TouchSendKey()
{
    if (Cell_HitTest(data_ov095_02295588, gTouchCurX - 0x80, gTouchCurY - 0x60, 0, 0) != 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL Keyboard_TouchPageTab(Keyboard *s)
{
    s32 r = Cell_HitTestList(s->tabCells, 3, gTouchCurX - 0x80, gTouchCurY - 0x60, 2, 2);
    if (r == -1) {
        return FALSE;
    }
    switch (r) {
    case 0:
        r = 1;
        break;
    case 2:
        r = 2;
        break;
    case 1:
        r = 3;
        break;
    }
    if (r == s->page) {
        return FALSE;
    }
    s->page = r;
    Snd_PlaySe(0x18);
    return TRUE;
}

BOOL Keyboard_TouchEmotionKey(Keyboard *s)
{
    s32 r = Cell_HitTestList((&data_ov095_02295e48[4]), s->emotionCount, gTouchCurX - 0x80, gTouchCurY - 0x60, 0, 0);
    if (r == -1) {
        return FALSE;
    }
    s->selectedEmotion = r;
    return TRUE;
}

void Keyboard_ClearSelectedEmotion(Keyboard *s)
{
    s->selectedEmotion = 0xff;
}

void Keyboard_SelectEmotionByCode(Keyboard *s, s32 v)
{
    s->selectedEmotion = v - 0x11b;
}

s32 Keyboard_GetSelectedEmotion(Keyboard *s)
{
    u32 i = s->selectedEmotion;
    if (i >= 4) {
        return 0xff;
    }
    return s->emotionSlots[i];
}

void Keyboard_DrawCaret(void *s, s32 x, s32 y, s32 z)
{
    Oam_DrawObj(1, data_ov095_02295590, x, y, -1, z, NULL);
}

void Keyboard_DrawCopyPasteKeysChat(Keyboard *s, s32 x, s32 y)
{
    Oam_DrawCell(1, data_ov095_02295894, x, y, s->copyKeyPalette, 2, 0x1000, 0x1000, 0, -1, 0, 0);
    Oam_DrawCell(1, data_ov095_022958bc, x, y, s->pasteKeyPalette, 2, 0x1000, 0x1000, 0, -1, 0, 0);
}

void Keyboard_DrawCopyPasteKeys(Keyboard *s, s32 x, s32 y)
{
    Oam_DrawCell(1, data_ov095_02295da4, x, y, s->copyKeyPalette, 1, 0x1000, 0x1000, 0, -1, 0, 0);
    Oam_DrawCell(1, data_ov095_02295dd4, x, y, s->pasteKeyPalette, 1, 0x1000, 0x1000, 0, -1, 0, 0);
}

void Keyboard_DrawLengthGauge(void *s, s32 x, s32 y, s32 idx)
{
    Oam_DrawCell(1, sLengthGaugeCells[idx], x, y, -1, 1, 0x1000, 0x1000, 0, -1, 0, 0);
}

void Keyboard_DrawLengthGaugeAt(void *s, s32 x, s32 y, s32 idx)
{
    Keyboard_DrawLengthGauge(s, x - 0x20, y + 0x32, idx);
}

s32 Keyboard_GetTabKeyX(Keyboard *s, s32 a)
{
    s32 r;
    switch (a) {
    case 0xca:
    case 0xcb:
    case 0xcc:
        r = Oam_GetObjX(&s->tabCells[a - 0xca].attr01) + 0x88;
        break;
    default:
        r = 0x80;
    }
    return r;
}

s32 Keyboard_CalcTabRowKeyPos(Keyboard *s)
{
    s32 v = s->cursorKey;
    switch (v) {
    case 0xca:
    case 0xcb:
    case 0xcc:
        s->cursorX = Keyboard_GetTabKeyX(s, v);
        s->cursorY = Oam_GetObjY((s8 *)&s->tabCells[v - 0xca]) + 0x68;
        return 1;
    case 0xdb:
        s->cursorX = 0x18;
        s->cursorY = 0x54;
        if (Keyboard_TestFlags(s, 4) == 0) {
            s->cursorY -= 8;
        }
        return 1;
    case 0xdc:
        s->cursorX = 0xe8;
        s->cursorY = 0x54;
        if (Keyboard_TestFlags(s, 4) == 0) {
            s->cursorY -= 8;
        }
        return 1;
    }
    return 0;
}

s32 Keyboard_GetEmotionKeyX(Keyboard *s, s32 a)
{
    if (a >= 0xde && a <= 0xe1) {
        return Oam_GetObjX(&data_ov095_02295e48[a - 0xde].attr01) + 0x88;
    }
    return 0x80;
}

s32 Keyboard_CalcChatKeyPos(Keyboard *s)
{
    s32 v = s->cursorKey;
    if (v == 0xc9) {
        s->cursorX = Oam_GetObjX(&data_ov095_02295588[0].attr01) + 0x98;
        s->cursorY = Oam_GetObjY((s8 *)&data_ov095_02295588[0]) + 0x70;
        return 1;
    }
    if (v >= 0xde && v <= 0xe1) {
        s->cursorX = Keyboard_GetEmotionKeyX(s, v);
        s->cursorY = Oam_GetObjY((s8 *)&data_ov095_02295e48[0]) + 0x68;
        return 1;
    }
    if (Keyboard_IsMenuTabKey(s, v) != 0) {
#ifdef N3DS_DIRECT
        s->cursorX = MenuTabBar_GetTabX(s->cursorKey == N3DS_KB_OPT_KEY ? N3DS_OPT_TAB : s->cursorKey - 0xcd);
#else
        s->cursorX = MenuTabBar_GetTabX(s->cursorKey - 0xcd);
#endif
        s->cursorY = 8;
        return 1;
    }
    return 0;
}

s32 Keyboard_CalcWriteButtonPos(Keyboard *s)
{
    s32 v = s->cursorKey;
    if (v == 0xd5) {
        return 1;
    }
    switch (v) {
    case 0xd6:
        s->cursorX = 0xca;
        s->cursorY = 0xb6;
        return 1;
    case 0xd7:
        s->cursorX = 0x58;
        s->cursorY = 0xb6;
        return 1;
    case 0xd8:
        s->cursorX = 0x74;
        s->cursorY = 0xb6;
        return 1;
    }
    return 0;
}

s32 Keyboard_CalcNameEntryButtonPos(Keyboard *s)
{
    switch (s->cursorKey) {
    case 0xd9:
        s->cursorX = 0xc4;
        s->cursorY = 0xb6;
        return 1;
    case 0xda:
        s->cursorX = 0x7c;
        s->cursorY = 0xb6;
        return 1;
    }
    return 0;
}

s32 Keyboard_CalcExtraKeyPos(Keyboard *s)
{
    s32 r = 0;
    if (Keyboard_CalcTabRowKeyPos(s) != 0) {
        return 1;
    }
    switch (s->layout) {
    case 0:
        r = Keyboard_CalcChatKeyPos(s);
        break;
    case 1:
    case 2:
        r = Keyboard_CalcWriteButtonPos(s);
        break;
    case 3:
    case 4:
    case 5:
        r = Keyboard_CalcNameEntryButtonPos(s);
        break;
    }
    return r;
}

void Keyboard_CalcCursorPosPage0(Keyboard *s)
{
}

void Keyboard_CalcCursorPosPage1(Keyboard *s)
{
    s32 v;
    if (Keyboard_CalcExtraKeyPos(s) == 0) {
        v = s->cursorKey;
        if (v >= 0x3b) {
            if (v <= 0x46) {
                s->cursorY = 0x68;
                s->cursorX = (s->cursorKey - 0x3b) * 0x14 + 0xe;
            } else if (v <= 0x51) {
                s->cursorY = 0x78;
                s->cursorX = (s->cursorKey - 0x47) * 0x14 + 0x16;
                if (s->cursorKey == 0x51) {
                    s->cursorX += 0xa;
                }
            } else if (v <= 0x5c) {
                s->cursorY = 0x88;
                s->cursorX = (s->cursorKey - 0x52) * 0x14 + 0xe;
                if (s->cursorKey == 0x5c) {
                    s->cursorX += 0xe;
                }
            } else if (v <= 0x67) {
                s->cursorY = 0x98;
                s->cursorX = (s->cursorKey - 0x5d) * 0x14 + 0x16;
                if (s->cursorKey == 0x5d) {
                    s->cursorX -= 3;
                }
            } else if (v <= 0x6f) {
                s->cursorY = 0xa8;
                v = s->cursorKey;
                if (v < 0x6b) {
                    s->cursorX = (v - 0x68) * 0x14 + 0x22;
                } else if (v == 0x6b) {
                    s->cursorX = 0x86;
                } else {
                    s->cursorX = (v - 0x6c) * 0x14 + 0xae;
                }
            }
        }
    }
}

void Keyboard_CalcCursorPosPage2(Keyboard *s)
{
    s32 c;
    if (Keyboard_CalcExtraKeyPos(s) == 0) {
        c = s->cursorKey;
        if (c >= 0x70) {
            if (c <= 0x8d) {
                s32 d = c - 0x70;
                c = d;
                s->cursorX = (c % 10) * 0x14 + 0x12;
                s->cursorY = ((c / 10) * 2 + 0xe) * 8;
            } else {
                switch (c - 0x8e) {
                case 0:
                    s->cursorX = 0xda;
                    s->cursorY = 0x70;
                    break;
                case 1:
                    s->cursorX = 0xee;
                    s->cursorY = 0x70;
                    break;
                case 2:
                    s->cursorX = 0xe4;
                    s->cursorY = 0x88;
                    break;
                case 3:
                    s->cursorX = 0x80;
                    s->cursorY = 0xa0;
                    break;
                }
            }
        }
    }
}

void Keyboard_CalcCursorPosPage3(Keyboard *s)
{
    s32 c, x, y;
    if (Keyboard_CalcExtraKeyPos(s) == 0) {
        c = s->cursorKey;
        if (c <= 0xc5) {
            x = ((c - 0x92) % 11) * 0x14 + 0x12;
            c -= 0x92;
            y = c / 11;
            y = y * 2 + 0xd;
            y *= 8;
        } else {
            x = 0xee;
            switch (c) {
            case 0xc6:
                y = 0x6c;
                break;
            case 0xc7:
                y = 0x8c;
                break;
            case 0xc8:
                x = 0xd0;
                y = 0xa8;
                break;
            }
        }
        s->cursorX = x;
        s->cursorY = y;
    }
}

void Keyboard_UpdateCursorPos(Keyboard *s)
{
    switch (s->page) {
    case 0:
        Keyboard_CalcCursorPosPage0(s);
        break;
    case 1:
        Keyboard_CalcCursorPosPage1(s);
        break;
    case 2:
        Keyboard_CalcCursorPosPage2(s);
        break;
    case 3:
        Keyboard_CalcCursorPosPage3(s);
        break;
    }
}

void Keyboard_MoveDownToMainButton(Keyboard *s)
{
    switch (s->layout) {
    case 1:
    case 2:
        s->cursorKey = 0xd6;
        break;
    case 4:
    case 5:
        s->cursorKey = 0xd9;
        break;
    }
}

void Keyboard_MoveDownToButtons(Keyboard *s)
{
    switch (s->layout) {
    case 1:
        if (Keyboard_TestFlags(s, 8) != 0) {
            s->cursorKey = 0xd6;
        } else {
            s->cursorKey = 0xd7;
        }
        break;
    case 2:
        s->cursorKey = 0xd8;
        break;
    case 5:
        s->cursorKey = 0xda;
        break;
    case 4:
        s->cursorKey = 0xd9;
        break;
    }
}

void Keyboard_MoveOnPage1(Keyboard *s, u32 p)
{
    s32 c, st, lo, hi, lo2, hi2, hc, v;
    if (Keyboard_MoveOnExtraKeys(s, p) != 0) {
        return;
    }
    st = s->cursorKey;
    if (st < 0x3b) {
        goto common;
    }
    if (st <= 0x46) {
        c = 0;
        goto common;
    }
    if (st <= 0x51) {
        c = 1;
        if (st != 0x51) {
            goto common;
        }
        if (MenuKeys_HasUp(p) != 0) {
            if (MenuKeys_HasLeft(p) != 0) {
                s->cursorKey = 0x45;
            } else {
                s->cursorKey = 0x46;
            }
            return;
        }
        if (MenuKeys_HasDown(p) == 0) {
            goto common;
        }
        s->cursorKey = 0x5c;
        return;
    }
    if (st <= 0x5c) {
        c = 2;
        if (st != 0x5c) {
            goto common;
        }
        if (MenuKeys_HasUp(p) == 0) {
            goto common;
        }
        if (MenuKeys_HasLeft(p) != 0) {
            s->cursorKey = 0x50;
        } else {
            s->cursorKey = 0x51;
        }
        return;
    }
    if (st <= 0x67) {
        c = 3;
        if (MenuKeys_HasDown(p) == 0) {
            goto common;
        }
        if (MenuKeys_HasLeft(p) != 0) {
            s->cursorKey--;
        }
        if (s->cursorKey < 0x5d) {
            s->cursorKey = 0x5d;
        }
        v = s->cursorKey;
        if (v <= 0x5e) {
            s->cursorKey = v + 0xb;
        } else if (v <= 0x65) {
            s->cursorKey = 0x6b;
        } else {
            s->cursorKey = v + 8;
        }
        return;
    }
    if (st > 0x6f) {
        return;
    }
    c = 4;
    if (MenuKeys_HasUp(p) != 0) {
        v = s->cursorKey;
        if (v == 0x6b) {
            s->cursorKey = 0x63;
            if (MenuKeys_HasLeft(p) != 0) {
                s->cursorKey--;
            } else if (MenuKeys_HasRight(p) != 0) {
                s->cursorKey++;
            }
            return;
        }
        if (v > 0x6b) {
            s->cursorKey = v - 8;
            if (MenuKeys_HasRight(p) == 0) {
                return;
            }
            v = s->cursorKey;
            if (v >= 0x67) {
                return;
            }
            s->cursorKey = v + 1;
            return;
        }
        goto common;
    }
    if (MenuKeys_HasDown(p) != 0) {
        if (s->cursorKey < 0x6c) {
            Keyboard_MoveDownToButtons(s);
        } else {
            Keyboard_MoveDownToMainButton(s);
        }
        return;
    }
common:
    if (MenuKeys_HasUp(p) != 0) {
        if (c == 0) {
            Keyboard_GoToTabRow(s);
            return;
        }
        lo = data_ov095_0229560c[c - 1];
        hi = data_ov095_02295634[c - 1];
        s->cursorKey = s->cursorKey - (hi - lo + 1);
        if (c == 2) {
            s->cursorKey--;
        }
        if (MenuKeys_HasRight(p) != 0) {
            s->cursorKey++;
        }
        v = s->cursorKey;
        if (v < lo) {
            s->cursorKey = lo;
        } else if (v > hi) {
            s->cursorKey = hi;
        }
    } else if (MenuKeys_HasDown(p) != 0 && c < 3) {
        lo2 = data_ov095_0229560c[c + 1];
        hi2 = data_ov095_02295634[c + 1];
        hc = data_ov095_02295634[c];
        s->cursorKey = s->cursorKey + (hc - data_ov095_0229560c[c] + 1);
        if (c == 1) {
            s->cursorKey++;
        }
        if (MenuKeys_HasLeft(p) != 0) {
            s->cursorKey--;
        }
        v = s->cursorKey;
        if (v > hi2) {
            s->cursorKey = hi2;
        } else if (v < lo2) {
            s->cursorKey = lo2;
        }
    } else {
        if (s->cursorKey == 0x6b) {
            if (MenuKeys_HasLeft(p) != 0) {
                s->cursorKey = 0x69;
            } else if (MenuKeys_HasRight(p) != 0) {
                s->cursorKey = 0x6e;
            }
            return;
        }
        if (MenuKeys_HasLeft(p) != 0) {
            if (s->cursorKey > data_ov095_0229560c[c]) {
                s->cursorKey = s->cursorKey - 1;
            } else {
                s->cursorWrap = 1;
                s->cursorKey = data_ov095_02295634[c];
            }
        } else if (MenuKeys_HasRight(p) != 0) {
            if (s->cursorKey < data_ov095_02295634[c]) {
                s->cursorKey = s->cursorKey + 1;
            } else {
                s->cursorWrap = 2;
                s->cursorKey = data_ov095_0229560c[c];
            }
        }
        v = s->cursorKey;
        if (v == 0x6a || (u32)(v - 0x6c) <= 1) {
            s->cursorKey = 0x6b;
        }
    }
}

void Keyboard_MoveOnPage2(Keyboard *s, u32 p)
{
    s32 r7, v;
    if (Keyboard_MoveOnExtraKeys(s, p) != 0) {
        return;
    }
    v = s->cursorKey;
    if (v < 0x70) {
        return;
    }
    if (v <= 0x8d) {
        s32 d = v - 0x70;
        r7 = d % 10;
        v = d / 10;
        if (MenuKeys_HasUp(p) != 0) {
            if (v > 0) {
                v--;
                s->cursorKey -= 10;
            } else {
                Keyboard_GoToTabRow(s);
                return;
            }
        } else if (MenuKeys_HasDown(p) != 0) {
            if (v < 2) {
                v++;
                s->cursorKey += 10;
            } else {
                s->cursorKey = 0x91;
                return;
            }
        }
        if (MenuKeys_HasLeft(p) != 0) {
            if (r7 > 0) {
                s->cursorKey--;
                return;
            }
            s->cursorWrap = 1;
            if (v == 0) {
                s->cursorKey = 0x8f;
            } else {
                s->cursorKey = 0x90;
            }
            return;
        }
        if (MenuKeys_HasRight(p) == 0) {
            return;
        }
        if (r7 < 9) {
            s->cursorKey++;
            return;
        }
        if (v == 0) {
            s->cursorKey = 0x8e;
        } else {
            s->cursorKey = 0x90;
        }
        return;
    }
    v -= 0x8e;
    switch (v) {
    case 0:
        if (MenuKeys_HasUp(p) != 0) {
            Keyboard_GoToTabRow(s);
        } else if (MenuKeys_HasDown(p) != 0) {
            if (MenuKeys_HasLeft(p) != 0) {
                s->cursorKey = 0x83;
            } else {
                s->cursorKey = 0x90;
            }
        } else if (MenuKeys_HasLeft(p) != 0) {
            s->cursorKey = 0x79;
        } else if (MenuKeys_HasRight(p) != 0) {
            s->cursorKey = 0x8f;
        }
        break;
    case 1:
        if (MenuKeys_HasUp(p) != 0) {
            Keyboard_GoToTabRow(s);
        } else if (MenuKeys_HasDown(p) != 0) {
            s->cursorKey = 0x90;
        } else if (MenuKeys_HasLeft(p) != 0) {
            s->cursorKey = 0x8e;
        } else if (MenuKeys_HasRight(p) != 0) {
            s->cursorWrap = 2;
            s->cursorKey = 0x70;
        }
        break;
    case 2:
        if (MenuKeys_HasUp(p) != 0) {
            s->cursorKey = 0x8f;
            if (MenuKeys_HasLeft(p) != 0) {
                s->cursorKey--;
            }
            break;
        }
        if (MenuKeys_HasDown(p) != 0) {
            Keyboard_MoveDownToMainButton(s);
            if (s->cursorKey != 0x90) {
                break;
            }
        }
        if (MenuKeys_HasLeft(p) != 0) {
            if (MenuKeys_HasDown(p) != 0) {
                s->cursorKey = 0x8d;
            } else {
                s->cursorKey = 0x83;
            }
        } else if (MenuKeys_HasRight(p) != 0) {
            s->cursorWrap = 2;
            if (MenuKeys_HasDown(p) != 0) {
                s->cursorKey = 0x84;
            } else {
                s->cursorKey = 0x7a;
            }
        }
        break;
    case 3:
        if (MenuKeys_HasUp(p) != 0) {
            s->cursorKey = 0x8a;
            if (MenuKeys_HasLeft(p) != 0) {
                s->cursorKey--;
            }
        } else if (MenuKeys_HasRight(p) != 0) {
            Keyboard_MoveDownToMainButton(s);
        } else if (MenuKeys_HasDown(p) != 0) {
            Keyboard_MoveDownToButtons(s);
        }
        break;
    }
}

void Keyboard_MoveOnPage3(Keyboard *s, u32 p)
{
    s32 st, lo;
    if (p == NULL) return;
    if (Keyboard_MoveOnExtraKeys(s, p) != 0) return;
    st = s->cursorKey;
    if (st < 0x92) return;
    if (st > 0xc8) return;
    if (st <= 0xc5) {
        st -= 0x92;
        lo = st % 0xb;
        st = st / 0xb;
        if (MenuKeys_HasUp(p) != 0) {
            if (st > 0) {
                st--;
            } else {
                Keyboard_GoToTabRow(s);
                return;
            }
        } else if (MenuKeys_HasDown(p) != 0) {
            if (st < 4) {
                st++;
            } else {
                Keyboard_MoveDownToButtons(s);
                return;
            }
        }
        if (MenuKeys_HasLeft(p) != 0) {
            if (lo > 0) {
                lo--;
            } else {
                s->cursorWrap = 1;
                if (st < 2) {
                    s->cursorKey = 0xc6;
                } else if (st < 4) {
                    s->cursorKey = 0xc7;
                } else {
                    s->cursorKey = 0xc8;
                }
                return;
            }
        } else if (MenuKeys_HasRight(p) != 0) {
            if (lo < 10) {
                lo++;
            } else {
                if (st < 2) {
                    s->cursorKey = 0xc6;
                } else if (st < 4) {
                    s->cursorKey = 0xc7;
                } else {
                    s->cursorKey = 0xc8;
                }
                return;
            }
        }
        st = lo + st * 0xb + 0x92;
        if (st > 0xc5) st = 0xc8;
        s->cursorKey = st;
        return;
    }
    if (MenuKeys_HasRight(p) != 0) {
        s->cursorWrap = 2;
        switch (s->cursorKey) {
        case 0xc6:
            if (MenuKeys_HasDown(p) != 0) {
                s->cursorKey = 0x9d;
            } else {
                s->cursorKey = 0x92;
            }
            break;
        case 0xc7:
            if (MenuKeys_HasUp(p) != 0) {
                s->cursorKey = 0x9d;
            } else if (MenuKeys_HasDown(p) != 0) {
                s->cursorKey = 0xb3;
            } else {
                s->cursorKey = 0xa8;
            }
            break;
        case 0xc8:
            s->cursorKey = 0xbe;
            break;
        }
    } else if (MenuKeys_HasLeft(p) != 0) {
        switch (s->cursorKey) {
        case 0xc6:
            if (MenuKeys_HasDown(p) != 0) {
                s->cursorKey = 0xa7;
            } else {
                s->cursorKey = 0x9c;
            }
            break;
        case 0xc7:
            if (MenuKeys_HasUp(p) != 0) {
                s->cursorKey = 0xa7;
            } else if (MenuKeys_HasDown(p) != 0) {
                s->cursorKey = 0xbd;
            } else {
                s->cursorKey = 0xb2;
            }
            break;
        case 0xc8:
            s->cursorKey = 0xc5;
            break;
        }
    } else if (MenuKeys_HasUp(p) != 0) {
        st = s->cursorKey;
        if (st == 0xc6) {
            Keyboard_GoToTabRow(s);
        } else if (st == 0xc7) {
            s->cursorKey = 0xc6;
        } else {
            s->cursorKey = 0xbd;
        }
    } else if (MenuKeys_HasDown(p) != 0) {
        if (s->cursorKey < 0xc8) {
            s->cursorKey++;
        } else {
            Keyboard_MoveDownToMainButton(s);
        }
    }
}

void Keyboard_MoveAboveTabRow(Keyboard *s)
{
    switch (s->layout) {
    case 0:
        if (s->cursorKey == 0xdc) {
            s->cursorKey = 0xc9;
        } else {
            Keyboard_SetFlags(s, 0x10);
        }
        break;
    case 1:
    case 2:
        s->cursorKey = 0xd5;
        break;
    case 3:
    case 4:
    case 5:
        Keyboard_SetFlags(s, 0x10);
        break;
    }
}

void Keyboard_SelectTabRowNearX(Keyboard *s, s32 a)
{
    s32 best, d, i;
    if (a < 0x80) {
        s->cursorKey = 0xdb;
        best = a - 0x18;
    } else {
        s->cursorKey = 0xdc;
        best = a - 0xe8;
    }
    for (i = 0; i < 3; i++) {
        d = a - Keyboard_GetTabKeyX(s, i + 0xca);
        if (d < 0) d = -d;
        if (d < best) {
            best = d;
            s->cursorKey = i + 0xca;
        }
    }
    if (Keyboard_TestFlags(s, 4) != 0) {
        if (best < 0) best = -best;
        for (i = 0; i < s->emotionCount; i++) {
            d = a - Keyboard_GetEmotionKeyX(s, i + 0xde);
            if (d < 0) d = -d;
            if (d < best) {
                best = d;
                s->cursorKey = i + 0xde;
            }
        }
    }
}

void Keyboard_GoToTabRow(Keyboard *s)
{
    Keyboard_SelectTabRowNearX(s, Keyboard_GetCursorX(s));
}

void Keyboard_EnterKeysAtX(Keyboard *s, s32 a)
{
    u32 mode = s->page;
    u32 off = mode << 2;
    s32 lo = *(s32 *)((u8 *)data_ov095_02295484 + off);
    s32 v;
    if (a < lo) {
        if (mode == 0) {
            s->cursorKey = 0x32;
            Keyboard_UpdateCursorPos(s);
            return;
        }
        a = lo;
    }
    v = (a - lo) / 0x14;
    if (mode == 0) {
        v *= 5;
    }
    a = v + data_ov095_02295474[mode];
    if (a > data_ov095_02295454[mode]) {
        a = *(s32 *)((u8 *)data_ov095_02295464 + off);
    }
    s->cursorKey = a;
    Keyboard_UpdateCursorPos(s);
}

void Keyboard_EnterTabRowAtX2(Keyboard *s, s32 a)
{
    Keyboard_SelectTabRowNearX(s, a);
    Keyboard_UpdateCursorPos(s);
}

void Keyboard_SelectMenuTabKey(Keyboard *s, s32 a)
{
    s->cursorKey = MenuTabBar_TabFromX(a) + 0xcd;
    Keyboard_UpdateCursorPos(s);
}

void Keyboard_SelectSendKey(Keyboard *s)
{
    s->cursorKey = 0xc9;
    Keyboard_UpdateCursorPos(s);
}

void Keyboard_EnterTabRowAtX(Keyboard *s, s32 a)
{
    Keyboard_SelectTabRowNearX(s, a);
    Keyboard_UpdateCursorPos(s);
}

BOOL Keyboard_MoveOnTabRow(Keyboard *s, u32 p)
{
    switch (s->cursorKey) {
    case 0xca:
    case 0xcb:
    case 0xcc:
        if (MenuKeys_HasRight(p) != 0) {
            if (s->cursorKey < 0xcc) {
                s->cursorKey++;
            } else if (s->emotionCount != 0) {
                s->cursorKey = 0xde;
            } else {
                s->cursorKey = 0xdc;
            }
            return TRUE;
        } else if (MenuKeys_HasLeft(p) != 0) {
            if (s->cursorKey > 0xca) {
                s->cursorKey--;
            } else {
                s->cursorKey = 0xdb;
            }
            return TRUE;
        } else if (MenuKeys_HasDown(p) != 0) {
            Keyboard_EnterKeysAtX(s, s->cursorX);
        } else if (MenuKeys_HasUp(p) != 0) {
            Keyboard_MoveAboveTabRow(s);
        }
        return TRUE;
    case 0xdb:
        if (MenuKeys_HasUp(p) != 0) {
            Keyboard_MoveAboveTabRow(s);
        } else if (MenuKeys_HasDown(p) != 0) {
            Keyboard_EnterKeysAtX(s, s->cursorX);
        } else if (MenuKeys_HasRight(p) != 0) {
            s->cursorKey = 0xca;
        } else if (MenuKeys_HasLeft(p) != 0) {
            s->cursorKey = 0xdc;
            s->cursorWrap = 1;
        }
        return TRUE;
    case 0xdc:
        if (MenuKeys_HasUp(p) != 0) {
            Keyboard_MoveAboveTabRow(s);
        } else if (MenuKeys_HasDown(p) != 0) {
            Keyboard_EnterKeysAtX(s, s->cursorX);
        } else if (MenuKeys_HasLeft(p) != 0) {
            if (s->emotionCount != 0) {
                s->cursorKey = s->emotionCount + 0xdd;
            } else {
                s->cursorKey = 0xcc;
            }
        } else if (MenuKeys_HasRight(p) != 0) {
            s->cursorKey = 0xdb;
            s->cursorWrap = 2;
        }
        return TRUE;
    case 0xde:
    case 0xdf:
    case 0xe0:
    case 0xe1:
        if (MenuKeys_HasUp(p) != 0) {
            Keyboard_MoveAboveTabRow(s);
        } else if (MenuKeys_HasDown(p) != 0) {
            Keyboard_EnterKeysAtX(s, s->cursorX);
        } else if (MenuKeys_HasLeft(p) != 0) {
            if (s->cursorKey > 0xde) {
                s->cursorKey--;
            } else {
                s->cursorKey = 0xcc;
            }
        } else if (MenuKeys_HasRight(p) != 0) {
            if (s->cursorKey < s->emotionCount + 0xdd) {
                s->cursorKey++;
            } else {
                s->cursorKey = 0xdc;
            }
        }
        return TRUE;
    }
    return FALSE;
}

BOOL Keyboard_MoveOnChatKeys(Keyboard *s, u32 p)
{
    if (s->cursorKey == 0xc9) {
        if (MenuKeys_HasDown(p) != 0) {
            s->cursorKey = 0xdc;
        } else if (MenuKeys_HasUp(p) != 0) {
            s->cursorKey = MenuTabBar_TabFromX(s->cursorX) + 0xcd;
        } else if (MenuKeys_HasLeft(p) != 0) {
            Keyboard_SetFlags(s, 0x10);
        }
        return TRUE;
    }
#ifdef N3DS_DIRECT
    // the options button: Left from the pockets' tab key, Right back to it, Down as from it
    if (s->cursorKey == N3DS_KB_OPT_KEY) {
        if (MenuKeys_HasDown(p) != 0) {
            Keyboard_SetFlags(s, 0x10);
        } else if (MenuKeys_HasRight(p) != 0) {
            s->cursorKey = 0xcd;
        }
        return TRUE;
    }
    if (s->cursorKey == 0xcd && MenuKeys_HasLeft(p) != 0 && MenuKeys_HasDown(p) == 0) {
        s->cursorKey = N3DS_KB_OPT_KEY;
        return TRUE;
    }
#endif
    if (Keyboard_IsMenuTabKey(s, s->cursorKey) != 0) {
        if (MenuKeys_HasDown(p) != 0) {
            if (s->cursorKey >= 0xd3) {
                s->cursorKey = 0xc9;
            } else {
                Keyboard_SetFlags(s, 0x10);
            }
            return TRUE;
        } else if (MenuKeys_HasRight(p) != 0) {
            if (s->cursorKey < 0xd4) {
                s->cursorKey++;
            }
        } else if (MenuKeys_HasLeft(p) != 0) {
            if (s->cursorKey > 0xcd) {
                s->cursorKey--;
            }
        }
        return TRUE;
    }
    return FALSE;
}

BOOL Keyboard_MoveOnWriteButtons(Keyboard *s, u32 p)
{
    s32 st = s->cursorKey;
    if (st == 0xd5) {
        if (MenuKeys_HasDown(p) != 0) {
            s->cursorKey = 0xdc;
        }
        return TRUE;
    }
    switch (st) {
    case 0xd6:
        if (MenuKeys_HasUp(p) != 0) {
            switch (s->page) {
            case 0:
                s->cursorKey = 0x2c;
                break;
            case 1:
                s->cursorKey = 0x6e;
                break;
            case 2:
                s->cursorKey = 0x90;
                break;
            case 3:
                s->cursorKey = 0xc8;
                break;
            }
        } else if (MenuKeys_HasLeft(p) != 0) {
            Keyboard_MoveDownToButtons(s);
        }
        return TRUE;
    case 0xd7:
        if (MenuKeys_HasUp(p) != 0) {
            switch (s->page) {
            case 0:
                s->cursorKey = 0xe;
                break;
            case 1:
                s->cursorKey = 0x69;
                break;
            case 2:
                s->cursorKey = 0x91;
                break;
            case 3:
                s->cursorKey = 0xc0;
                break;
            }
        } else if (MenuKeys_HasRight(p) != 0) {
            s->cursorKey = 0xd6;
        }
        return TRUE;
    case 0xd8:
        if (MenuKeys_HasUp(p) != 0) {
            switch (s->page) {
            case 0:
                s->cursorKey = 0x13;
                break;
            case 1:
                s->cursorKey = 0x6b;
                break;
            case 2:
                s->cursorKey = 0x91;
                break;
            case 3:
                s->cursorKey = 0xc2;
                break;
            }
        } else if (MenuKeys_HasRight(p) != 0) {
            s->cursorKey = 0xd6;
        }
        return TRUE;
    default:
        if (st == 0xd7 || st == 0xd8) {
            if (MenuKeys_HasDown(p) != 0) {
                s->cursorKey = 0xd5;
            } else if (MenuKeys_HasRight(p) != 0) {
                s->cursorKey = 0xd6;
            }
            return TRUE;
        }
        return FALSE;
    }
}

BOOL Keyboard_MoveOnNameEntryButtons(Keyboard *s, u32 p)
{
    if (s->cursorKey == 0xd9) {
        if (MenuKeys_HasUp(p) != 0) {
            switch (s->page) {
            case 0:
                s->cursorKey = 0x2c;
                break;
            case 1:
                s->cursorKey = 0x6e;
                break;
            case 2:
                s->cursorKey = 0x90;
                break;
            case 3:
                s->cursorKey = 0xc8;
                break;
            }
        } else if (s->layout == 5) {
            if (MenuKeys_HasLeft(p) != 0) {
                s->cursorKey = 0xda;
            }
        }
        return TRUE;
    } else if (s->cursorKey == 0xda) {
        if (MenuKeys_HasUp(p) != 0) {
            switch (s->page) {
            case 0:
                s->cursorKey = 0x18;
                break;
            case 1:
                s->cursorKey = 0x6b;
                break;
            case 2:
                s->cursorKey = 0x91;
                break;
            case 3:
                s->cursorKey = 0xc3;
                break;
            }
        } else if (MenuKeys_HasRight(p) != 0) {
            s->cursorKey = 0xd9;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL Keyboard_MoveOnExtraKeys(Keyboard *s, u32 p)
{
    BOOL r;
    if (Keyboard_MoveOnTabRow(s, p) != 0) {
        return TRUE;
    }
    r = FALSE;
    switch (s->layout) {
    case 0:
        r = Keyboard_MoveOnChatKeys(s, p);
        break;
    case 1:
    case 2:
        r = Keyboard_MoveOnWriteButtons(s, p);
        break;
    case 3:
        break;
    case 4:
    case 5:
        r = Keyboard_MoveOnNameEntryButtons(s, p);
        break;
    }
    return r;
}

s32 Keyboard_GetCursorX(Keyboard *s)
{
    if (s->cursorKey == 0xd5) {
        s->cursorX = s->knobGripX;
    }
    switch (s->cursorWrap) {
    case 1:
        return s->cursorX - 0x100;
    case 2:
        return s->cursorX + 0x100;
    }
    return s->cursorX;
}

s32 Keyboard_GetCursorY(Keyboard *s)
{
    if (s->cursorKey == 0xd5) {
        s->cursorY = s->knobGripY;
        return s->cursorY;
    }
    if (Keyboard_GetSpecialKeyCode(s, s->cursorKey) != -1) {
        return s->cursorY;
    }
    if (Keyboard_TestFlags(s, 4) == 0) {
        return s->cursorY - 8;
    }
    return s->cursorY;
}

void Keyboard_SetTextFieldPos(Keyboard *s, s32 a, s32 b)
{
    s->knobGripX = a;
    s->knobGripY = b;
}

void Keyboard_ResetCursor(Keyboard *s)
{
    switch (s->page) {
    case 0:
        s->cursorKey = 0;
        Keyboard_CalcCursorPosPage0(s);
        break;
    case 1:
        s->cursorKey = 0x3b;
        Keyboard_CalcCursorPosPage1(s);
        break;
    case 2:
        s->cursorKey = 0x70;
        Keyboard_CalcCursorPosPage2(s);
        break;
    case 3:
        s->cursorKey = 0x92;
        Keyboard_CalcCursorPosPage3(s);
        break;
    }
}

s32 Keyboard_MoveCursor(Keyboard *s, u32 p)
{
    s32 old = s->cursorKey;
    s->cursorWrap = 0;
    Keyboard_ClearFlags(s, 0x10);
    switch (s->page) {
    case 0:
        break;
    case 1:
        Keyboard_MoveOnPage1(s, p);
        break;
    case 2:
        Keyboard_MoveOnPage2(s, p);
        break;
    case 3:
        Keyboard_MoveOnPage3(s, p);
        break;
    }
    if (Keyboard_TestFlags(s, 0x10) != 0) {
        return 4;
    }
    if (old != s->cursorKey) {
        Keyboard_UpdateCursorPos(s);
        if (Keyboard_IsOnMenuTabKey(s, s->cursorKey) != 0) {
            return 2;
        }
        if (Keyboard_IsOnButtonKey(s, s->cursorKey) != 0) {
            return 3;
        }
        return 1;
    }
    return 0;
}

s32 Keyboard_PressCursorKey(Keyboard *s)
{
    s32 st;
    if (Keyboard_IsKeyDisabled(s, s->cursorKey) != 0) {
        return -1;
    }
    st = s->cursorKey;
    switch (st) {
    case 0xca:
        if (s->page == 1) return -1;
        break;
    case 0xcc:
        if (s->page == 2) return -1;
        break;
    case 0xcb:
        if (s->page == 3) return -1;
        break;
    }
    s->pressedKey = st;
    return s->cursorKey;
}

void Keyboard_PlayCopySe()
{
    Snd_PlaySe(0x16);
}

void Keyboard_PlayPasteSe()
{
    Snd_PlaySe(0x17);
}

void Keyboard_PlayCharSe(Keyboard *s, s32 a)
{
    if (Keyboard_TestFlags(s, 0x200) == 0) {
        Keyboard_SetFlags(s, 0x200);
        s32 r = Text_GetCharSortKey(a);
        switch (r) {
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
            r = sCharSortKeyZero;
        }
        if (r != sCharSortKeyZero) {
            Snd_PlayKeySe(r);
        } else {
            Snd_PlayKeySe(0x2c);
        }
    }
}

BOOL Keyboard_TestFlags(Keyboard *s, u32 m)
{
    if (s->flags & m) return TRUE;
    return FALSE;
}

void Keyboard_SetFlags(Keyboard *s, u32 m)
{
    s->flags |= m;
}

void Keyboard_ClearFlags(Keyboard *s, u32 m)
{
    s->flags &= ~m;
}

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_02292360) {
    HOST_OVERLAY_VAR(data_ov095_022965bc),
    HOST_OVERLAY_VAR(data_ov095_022963e4),
    HOST_OVERLAY_VAR(data_ov095_02296214),
    HOST_OVERLAY_VAR(data_ov095_022960dc),
    HOST_OVERLAY_VAR(sLengthGaugeCells),
    HOST_OVERLAY_VAR(sKeyCodesPage3),
    HOST_OVERLAY_VAR(sKeyCodesPage1Lower),
    HOST_OVERLAY_VAR(sKeyCodesPage1Upper),
    HOST_OVERLAY_VAR(sKeyCodesPage1Alt),
    HOST_OVERLAY_VAR(data_ov095_02295e48),
    HOST_OVERLAY_VAR(sKeyCodesPage2),
    HOST_OVERLAY_VAR(data_ov095_02295d44),
    HOST_OVERLAY_VAR(data_ov095_02295d74),
    HOST_OVERLAY_VAR(data_ov095_02295da4),
    HOST_OVERLAY_VAR(data_ov095_02295dd4),
    HOST_OVERLAY_VAR(data_ov095_02295c7c),
    HOST_OVERLAY_VAR(data_ov095_02295ca4),
    HOST_OVERLAY_VAR(data_ov095_02295ccc),
    HOST_OVERLAY_VAR(data_ov095_02295cf4),
    HOST_OVERLAY_VAR(data_ov095_02295d1c),
    HOST_OVERLAY_VAR(data_ov095_02295c2c),
    HOST_OVERLAY_VAR(data_ov095_0229586c),
    HOST_OVERLAY_VAR(data_ov095_02295894),
    HOST_OVERLAY_VAR(data_ov095_022958bc),
    HOST_OVERLAY_VAR(data_ov095_02295b8c),
    HOST_OVERLAY_VAR(data_ov095_022958e4),
    HOST_OVERLAY_VAR(data_ov095_0229590c),
    HOST_OVERLAY_VAR(data_ov095_02295934),
    HOST_OVERLAY_VAR(data_ov095_02295aec),
    HOST_OVERLAY_VAR(data_ov095_0229595c),
    HOST_OVERLAY_VAR(data_ov095_02295984),
    HOST_OVERLAY_VAR(data_ov095_022959ac),
    HOST_OVERLAY_VAR(data_ov095_022959d4),
    HOST_OVERLAY_VAR(data_ov095_022959fc),
    HOST_OVERLAY_VAR(data_ov095_02295a24),
    HOST_OVERLAY_VAR(data_ov095_02295a4c),
    HOST_OVERLAY_VAR(data_ov095_02295a74),
    HOST_OVERLAY_VAR(data_ov095_02295a9c),
    HOST_OVERLAY_VAR(data_ov095_02295ac4),
    HOST_OVERLAY_VAR(data_ov095_02295b14),
    HOST_OVERLAY_VAR(data_ov095_02295b3c),
    HOST_OVERLAY_VAR(data_ov095_02295b64),
    HOST_OVERLAY_VAR(data_ov095_02295bb4),
    HOST_OVERLAY_VAR(data_ov095_02295bdc),
    HOST_OVERLAY_VAR(data_ov095_02295c04),
    HOST_OVERLAY_VAR(data_ov095_02295c54),
    HOST_OVERLAY_VAR(data_ov095_022957ac),
    HOST_OVERLAY_VAR(sKeyboardModeCharFiles),
    HOST_OVERLAY_VAR(sKeyboardModeScreensChat),
    HOST_OVERLAY_VAR(data_ov095_0229580c),
    HOST_OVERLAY_VAR(data_ov095_0229582c),
    HOST_OVERLAY_VAR(sKeyboardModeScreens),
    HOST_OVERLAY_VAR(data_ov095_02295704),
    HOST_OVERLAY_VAR(data_ov095_0229574c),
    HOST_OVERLAY_VAR(data_ov095_022956d4),
    HOST_OVERLAY_VAR(data_ov095_0229565c),
    HOST_OVERLAY_VAR(data_ov095_022956ec),
    HOST_OVERLAY_VAR(data_ov095_0229577c),
    HOST_OVERLAY_VAR(data_ov095_0229568c),
    HOST_OVERLAY_VAR(data_ov095_022956a4),
    HOST_OVERLAY_VAR(data_ov095_022956bc),
    HOST_OVERLAY_VAR(data_ov095_02295674),
    HOST_OVERLAY_VAR(data_ov095_02295734),
    HOST_OVERLAY_VAR(data_ov095_0229571c),
    HOST_OVERLAY_VAR(data_ov095_02295794),
    HOST_OVERLAY_VAR(data_ov095_02295764),
    HOST_OVERLAY_VAR(data_ov095_02295620),
    HOST_OVERLAY_VAR(data_ov095_02295648),
    HOST_OVERLAY_VAR(data_ov095_022955f8),
    HOST_OVERLAY_VAR(data_ov095_022955bc),
    HOST_OVERLAY_VAR(data_ov095_022955a8),
    HOST_OVERLAY_VAR(data_ov095_022955d0),
    HOST_OVERLAY_VAR(data_ov095_022955e4),
    HOST_OVERLAY_VAR(data_ov095_0229560c),
    HOST_OVERLAY_VAR(data_ov095_02295634),
    HOST_OVERLAY_VAR(sKeyboardPageCells),
    HOST_OVERLAY_VAR(data_ov095_02295590),
    HOST_OVERLAY_VAR(data_ov095_02295588),
    HOST_OVERLAY_VAR(data_ov095_02295580),
    HOST_OVERLAY_VARS_END
};

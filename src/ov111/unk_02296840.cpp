// ov111: scene overlay (class ChatMenu, vtable 0x02298a48, 0x3e58 bytes): a character/name entry screen.
#include "types.h"
#include "host_shared_calls.h"
#include "net/CommManager.h"
#include "Unk_020d8c7c.h"
#include "text/Unk_02050288.h"
#include "ui/UiWidget.h"
#include "player/PlayerId.h"
#include "talk/MsgString.h"
#include "menu/MenuProc.h"
#include "ui/HandCursor.h"
#include "talk/EncodedString.h"
#include "talk/ChatBalloonText.h"
#include "talk/MsgString9B.h"
#include "ui/LabelString.h"
#include "gfx/BgVramTask.h"
#include "menu/MenuCursor.h"
#include "player/PlayerData.h"
#include "menu/Keyboard.h"
#include "menu/MenuErrorMessage.h"
#include "menu/MenuTabBar.h"
#include "sys/ProcProfile.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_020ec848.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_02038474.h"
#include "proto/main/unk_02051218.h"
#include "proto/main/unk_02063f3c.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_020943dc.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_020b2c50.h"
#include "proto/main/unk_020b7d84.h"
#include "proto/ov002/unk_ov002_02200840.h"
#include "proto/ov090/unk_ov090_022918e0.h"
#include "proto/ov095/unk_02292360.h"
#include "proto/ov111/unk_02296840.h"
#ifdef N3DS_DIRECT
#include "n3ds_ui.h"
#endif

enum Unk_ov111_022970cc_Status { UNK_OV111_ST_0 = 0, UNK_OV111_ST_1 = 1, UNK_OV111_ST_2 = 2, UNK_OV111_ST_3 = 3 };

class ChatMenu;
typedef ChatMenu S;






extern "C" {
extern u8 gU8None;
extern u16 gPad[];
extern CommManager *gCommManager;

void MsgTextLabel_Destroy(void *p);
void *MsgTextLabel_CreateVram(u32 a, u32 b, u32 c);
s32 MI_CpuCopy8(void *src, void *dst, s32 n);
}













// vtable 0x02298a30, data at +0xe, 0x20 bytes
class EncodedString32 : public EncodedString {
public:
    EncodedString32() {}
    virtual ~EncodedString32() {}
    virtual u32 capacity();
    virtual u8 *data();

    u8 text[0x20];
};

typedef void (ChatMenu::*Unk_ov111_02298a48_Fn)();

// Vtable 0x02298a48, size 0x3e58
class ChatMenu : public MenuProc {
public:
    ChatMenu()
        : keyboard(), balloonText(), text(), cursor(), errorMessage() {}
    // destructor left implicit

    virtual BOOL onCreate();
    virtual BOOL onDelete();
    virtual BOOL onExecute();
    virtual BOOL onDraw();
    virtual BOOL execTransition();
    virtual BOOL execMain();
    virtual BOOL execPhase3();
    virtual BOOL execPhase4();
    virtual BOOL execClosed();

    void clearFlags(u32 mask);
    void setFlags(u32 mask);
    BOOL testFlags(u32 mask);
    void refreshKeys();
    void endKeyboardFrame();
    void setKeyboardMode(u32 a);
    void returnToKeyNav();
    void releaseCursor();
    void pressCursor();
    void snapCursor();
    void moveCursorToTarget();
    void hideCursor();
    void showCursor();
    BOOL tryStartSend();
    BOOL tryCopyButton();
    BOOL tryPasteButton();
    BOOL tryBackspaceButton();
    BOOL tryPressKey();
    s32 navigateText(u32 pad);
    void clearSelection();
    BOOL hasSelection();
    u32 getCharBeforeCursor();
    void updateCaretX();
    void snapCaretToText();
    void setCaretFromTouchX(u32 v);
    void dragSelection();
    BOOL touchTextField();
    BOOL startSend();
    BOOL touchSendKey();
    s32 touchKey();
    s32 pressKeyCode(u32 x);

    // state-table targets (0x8c table)
    void transitionAct00();
    void transitionAct01();
    void transitionAct02();
    void transitionAct03();
    void transitionAct04();
    void transitionAct05();
    // state-table targets (0x8d table)
    void mainAct00();
    void mainAct01();
    void mainAct02();
    void mainAct03();
    void mainAct04();
    void mainAct05();
    void mainAct06();
    void mainAct07();
    void mainAct08();
    void mainAct09();
    void mainAct0A();
    void mainAct0B();
    void mainAct0C();
    void mainAct0D();
    void mainAct0E();
    void mainAct0F();

    void postInputUpdate();
    void preInputUpdate();
    void runMainState();

    /* 0x91 */ u8 unk_91[3];
    /* 0x94 */ s32 lengthGauge;
    /* 0x98 */ u8 returnState;
    /* 0x99 */ u8 unk_99;
    /* 0x9a */ u8 sendAnimFrame;
    /* 0x9b */ volatile u8 sendKeyAnim;
    /* 0x9c */ u8 caretBlinkTimer;
    /* 0x9d */ u8 sendAnimDelay;
    /* 0x9e */ u8 caretIndex;
    /* 0x9f */ u8 selectionStart;
    /* 0xa0 */ u8 selectionEnd;
    /* 0xa1 */ u8 caretX;
    /* 0xa2 */ u8 emotionTimer;
    /* 0xa3 */ u8 pad_a3;
    /* 0xa4 */ u16 flags;
    /* 0xa6 */ u8 unk_a6[2];
    /* 0xa8 */ TextLabel *textLabel;
    /* 0xac */ Keyboard keyboard;
    /* 0x3c68 */ ChatBalloonText balloonText;
    /* 0x3c9c */ EncodedString32 text;
    /* 0x3ccc */ MenuCursorBuf0 cursor;
    /* 0x3d30 */ MenuErrorMessage errorMessage;
    /* 0x3e38 */ u8 clipboard[0x20];
};

extern "C" {
void ChatMenu_PlayErrorSe(S *s);
void ChatMenu_Paste(S *s);
void ChatMenu_Copy(S *s);
void ChatMenu_ClearText(S *s);
void ChatMenu_ApplyModifier105(S *s);
BOOL ChatMenu_TryModifier105(S *s);
void ChatMenu_ApplyModifier104(S *s);
BOOL ChatMenu_TryModifier104(S *s);
void ChatMenu_ApplyModifier103(S *s);
BOOL ChatMenu_TryModifier103(S *s);
void ChatMenu_DeleteSelection(S *s);
BOOL ChatMenu_Backspace(S *s, Unk_ov111_022970cc_Status a);
BOOL ChatMenu_InsertChar(S *s, u32 a);
void ChatMenu_BeginSend(S *s);
void ChatMenu_SendText(S *s);
void ChatMenu_RedrawText(S *s);
void ChatMenu_CreateTextLabel(S *s);
void ChatMenu_DestroyTextLabel(S *s);
void ChatMenu_DrawKeyboard(S *s);
void ChatMenu_LoadSendAnimFrame(S *s, u32 a);
void ChatMenu_Exit(S *s);
void ChatMenu_Init(S *s);
void ChatMenu_ShowMessage(S *s, u32 v, Unk_ov111_022970cc_Status w);
void ChatMenu_PlayEmotion(S *s);
void ChatMenu_ResumeInput(S *s);
void ChatMenu_StartButtonInput(S *s);
void ChatMenu_StartTouchInput(S *s);
void ChatMenu_LoadBg(S *s);
BOOL ChatMenu_RequestTab(S *s, s32 a, u32 b);
BOOL ChatMenu_HandleTabSwitch(S *s);
}

static inline BOOL Both() {
    if (gTouchHeld != 0 && gTouchChanged != 0) {
        return TRUE;
    }
    return FALSE;
}



extern "C" ChatMenu *ChatMenu_Create() { return new ChatMenu(); }

BOOL ChatMenu::onCreate() {
    ChatMenu_Init(this);
    setTransitionState(0);
    setPhase(1);
    return TRUE;
}

BOOL ChatMenu::onDelete() {
    ((MenuTabBar *)ProcBase_GetParent(this))->onTabMenuClosed();
    ChatMenu_Exit(this);
    return TRUE;
}

BOOL ChatMenu::onDraw() {
    if (testFlags(4)) {
        if (MenuCtrl_IsButtons()) {
            ((MenuCursorBase *)&cursor)->drawWrapped();
        }
        ChatMenu_DrawKeyboard(this);
    }
    return TRUE;
}// Declarations for data defined further down (definition order sets the data layout)
extern "C" ProcProfile data_ov111_02298920;
extern "C" char data_ov111_022989c8[];
extern "C" char data_ov111_022989e0[];
extern "C" char data_ov111_022989f8[];
extern "C" char data_ov111_02298a10[];
extern "C" const char *sChatSendAnimScreens[4];

// Scene registration entry read by main: factory, then two ids
extern "C" ProcProfile data_ov111_02298920 = {(void *(*)())ChatMenu_Create, 0x9e, 0xa2};

// Data order: this unit is placed object by object (see object_order.txt).

BOOL ChatMenu::execTransition() {
    static Unk_ov111_02298a48_Fn tbl[6] = {
        &ChatMenu::transitionAct00,
        &ChatMenu::transitionAct01,
        &ChatMenu::transitionAct02,
        &ChatMenu::transitionAct03,
        &ChatMenu::transitionAct04,
        &ChatMenu::transitionAct05};
    (this->*tbl[transitionState])();
    return TRUE;
}

void ChatMenu::runMainState() {
    static Unk_ov111_02298a48_Fn tbl[16] = {
        &ChatMenu::mainAct00,
        &ChatMenu::mainAct01,
        &ChatMenu::mainAct02,
        &ChatMenu::mainAct03,
        &ChatMenu::mainAct04,
        &ChatMenu::mainAct05,
        &ChatMenu::mainAct06,
        &ChatMenu::mainAct07,
        &ChatMenu::mainAct08,
        &ChatMenu::mainAct09,
        &ChatMenu::mainAct0A,
        &ChatMenu::mainAct0B,
        &ChatMenu::mainAct0C,
        &ChatMenu::mainAct0D,
        &ChatMenu::mainAct0E,
        &ChatMenu::mainAct0F};
    (this->*tbl[mainState])();
}

BOOL ChatMenu::execMain() {
    if (ChatMenu_HandleTabSwitch(this)) {
        return TRUE;
    }
    preInputUpdate();
    runMainState();
    postInputUpdate();
    return TRUE;
}

BOOL ChatMenu::execPhase3() { return TRUE; }

BOOL ChatMenu::execPhase4() { return TRUE; }

BOOL ChatMenu::execClosed() {
    ProcBase_RequestDelete(this);
    return TRUE;
}

BOOL ChatMenu::onExecute() {
    ChatMenu_DestroyTextLabel(this);
    MenuProc::onExecute();
    return TRUE;
}

void ChatMenu::preInputUpdate() {
    cursor.update();
}

void ChatMenu::postInputUpdate() {
    if (emotionTimer != 0) {
        emotionTimer = *(volatile u8 *)&emotionTimer - 1;
        if (emotionTimer == 0) {
            Keyboard_ClearSelectedEmotion(&keyboard);
        }
    }
    endKeyboardFrame();
}

BOOL ChatMenu_HandleTabSwitch(S *s) {
    MenuCtrl_TickForceClose();
    if (MenuCtrl_IsForceCloseDue() != 0) {
        switch (s->mainState) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            return ChatMenu_RequestTab(s, 7, 1);
        case 4:
        default:
            break;
        }
    }
    if (s->mainState != 0 && s->mainState != 3 && s->mainState != 9) {
        return FALSE;
    }
    s32 r5 = -1;
    if (MenuCtrl_IsTouch() != 0) {
        r5 = MenuTabBar_HitTestTouch();
    } else {
        u32 t = gPad[1];
        if ((t & 4) != 0) {
            r5 = 7;
        } else if ((t & 0x800) != 0) {
            r5 = 0;
        } else if ((t & 0x400) != 0) {
            r5 = 5;
        }
    }
    return ChatMenu_RequestTab(s, r5, 0);
}

BOOL ChatMenu_RequestTab(S *s, s32 a, u32 b) {
    void *p = ProcBase_GetParent(s);
    if (a != -1 && a != 4) {
        ((MenuTabBar *)p)->selectTab((u8)a);
        s->hideCursor();
        s->transitionState = 4;
        s->setPhase(1);
        if (a == 7) {
            s->setFlags(0x10);
        }
        if (b != 0) {
            MenuCtrl_SetChatDraft(s->text.text);
        } else {
            MenuCtrl_ClearChatDraft();
        }
        return TRUE;
    }
    return FALSE;
}

void ChatMenu_SetupBgLayer() {
    Gfx2d_SetLayerPriority(6, 2);
    Gfx2d_SetLayerControl(6, 0, 0, 0);
}

void ChatMenu_LoadBg(S *s) {
    void *r4 = gCurrentHeap;
    Gfx2d_LoadPaletteFile("menu/chat2/b_cht_bg.bpl", r4, 6, 1, 1, 9);
    Keyboard_LoadScreenFile(&s->keyboard, data_ov111_022989c8);
    s->refreshKeys();
    Keyboard_LoadScreenNow(&s->keyboard, 6);
    Gfx2d_LoadCharFile("menu/chat2/b_cht.bch", r4, 6, 0x13d, 0x13d, 0x1e9);
}

void ChatMenu::transitionAct00() {
    ChatMenu_SetupBgLayer();
    Keyboard_LoadObjGfx(&this->keyboard);
    this->setTransitionState(1);
    if (this->testFlags(0x20) == 0) {
        this->transitionAct01();
    }
}

void ChatMenu::transitionAct01() {
    Shared_Keyboard_LoadEmotionIcons(&this->keyboard);
    ChatMenu_LoadBg(this);
    this->setTransitionState(2);
    if (this->testFlags(0x20) == 0) {
        this->transitionAct02();
    }
}

void ChatMenu::transitionAct02() {
    Shared_Keyboard_RestoreLastPage(&this->keyboard, 6);
    this->endKeyboardFrame();
    this->setFlags(4);
    this->beginSubSlideIn(8, 3, 0, 0x30);
    Gfx2d_ShowLayer(6);
    this->applySlideOffset(6, 0, 0);
    this->setTransitionState(3);
    ChatMenu_RedrawText(this);
}

void ChatMenu::transitionAct03() {
    if (this->stepSlideIn(0) != 0) {
        this->setPhase(2);
        ChatMenu_ResumeInput(this);
    }
    this->applySlideOffset(6, 0, 0);
}

void ChatMenu::transitionAct04() {
    this->beginSubSlideOut(8, 0, 0, 0x30);
    this->applySlideOffset(6, 0, 0);
    this->transitionState = 5;
}

void ChatMenu::transitionAct05() {
    if (this->stepSlideOut(0) != 0) {
        Gfx2d_HideLayer(6);
        this->setPhase(5);
    } else {
        this->applySlideOffset(6, 0, 0);
    }
}

void ChatMenu::mainAct00() {
    if (this->checkSwitchToButtons(1) != 0) {
        ChatMenu_StartButtonInput(this);
    } else if (Keyboard_UpdatePressedKey(&this->keyboard) == 0) {
        if (Both()) {
            s32 r = this->touchKey();
            if (r != 0) {
                if (r == 1) {
                    this->setMainState(1);
                }
            } else {
                if (Keyboard_TouchPageTab(&this->keyboard) != 0) {
                    this->setKeyboardMode(8);
                    ChatMenu_RedrawText(this);
                } else if (this->touchSendKey() != 0) {
                    this->setMainState(0xd);
                } else if (this->emotionTimer == 0 && Keyboard_TouchEmotionKey(&this->keyboard) != 0) {
                    ChatMenu_PlayEmotion(this);
                    this->setMainState(0);
                } else if (this->touchTextField() != 0) {
                    this->refreshKeys();
                    this->setMainState(2);
                }
            }
        }
    }
}

void ChatMenu::mainAct01() {
    if (gTouchHeld == 0) {
        this->setMainState(0);
    } else if (Shared_Keyboard_TickKeyRepeat(&this->keyboard) != 0) {
        s32 r5 = Keyboard_GetPressedKey(&this->keyboard);
        this->pressKeyCode(Keyboard_GetKeyCode(&this->keyboard, r5, 8));
        Shared_Keyboard_HighlightKey(&this->keyboard, r5);
    }
}

void ChatMenu::mainAct02() {
    if (gTouchHeld == 0) {
        this->setMainState(0);
        if (this->selectionStart == this->selectionEnd) {
            this->clearFlags(1);
        }
    } else {
        this->dragSelection();
    }
    ChatMenu_RedrawText(this);
}

void ChatMenu::mainAct03() {
    if (this->checkSwitchToTouch() != 0) {
        ChatMenu_StartTouchInput(this);
        return;
    }
    switch (Keyboard_MoveCursor(&this->keyboard, this->takeRepeatedKeys())) {
    case 1:
        ((MenuCursor *)&this->cursor)->switchToAnim01();
        this->moveCursorToTarget();
        break;
    case 2:
        ((MenuCursor *)&this->cursor)->switchToAnim0D();
        this->moveCursorToTarget();
        break;
    case 4:
        ((MenuCursor *)&this->cursor)->switchToAnim01();
        this->setFlags(2);
        this->setMainState(9);
        this->moveCursorToTarget();
        break;
    case 0:
    case 3:
    default:
        if (this->tryPressKey() != 0) {
            return;
        }
        if (this->tryBackspaceButton() != 0) {
            return;
        }
        if (this->tryStartSend() != 0) {
            return;
        }
        {
            u32 t = gPad[1];
            if ((t & 0x100) != 0) {
                ChatMenu_RequestTab(this, MenuTabBar_NextTab(4), 0);
            } else if ((t & 0x200) != 0) {
                ChatMenu_RequestTab(this, MenuTabBar_PrevTab(4), 0);
            }
        }
        break;
    }
}

void ChatMenu::mainAct04() {
    if (((MenuCursorBase *)&this->cursor)->isMoving() == 0) {
        this->setMainState(this->returnState);
        this->runMainState();
    }
}

void ChatMenu::mainAct05() {
    if (this->cursor.isAnimDone() != 0) {
        s32 r6 = Shared_Keyboard_PressCursorKey(&this->keyboard);
        s32 r4 = this->pressKeyCode(Keyboard_GetKeyCode(&this->keyboard, r6, 8));
        if (r4 != 3) {
            if (r4 == 1 && (gPad[0] & 1) != 0) {
                Keyboard_StartKeyRepeat(&this->keyboard);
                this->setMainState(6);
                Shared_Keyboard_HighlightKey(&this->keyboard, r6);
            } else {
                Shared_Keyboard_ClearHighlight(&this->keyboard);
                if (r4 != 4) {
                    this->releaseCursor();
                }
            }
        }
    }
}

void ChatMenu::mainAct06() {
    if ((gPad[0] & 1) == 0) {
        Shared_Keyboard_ClearHighlight(&this->keyboard);
        this->releaseCursor();
    } else if (Shared_Keyboard_TickKeyRepeat(&this->keyboard) != 0) {
        s32 r5 = Keyboard_GetPressedKey(&this->keyboard);
        this->pressKeyCode(Keyboard_GetKeyCode(&this->keyboard, r5, 8));
        Shared_Keyboard_HighlightKey(&this->keyboard, r5);
    }
}

void ChatMenu::mainAct07() {
    if (this->cursor.isAnimDone() != 0) {
        this->returnToKeyNav();
    }
}

void ChatMenu::mainAct08() {
    if ((gPad[0] & 2) == 0) {
        this->setMainState(this->returnState);
    } else if (Shared_Keyboard_TickKeyRepeat(&this->keyboard) != 0) {
        this->pressKeyCode(0x100);
        if (this->testFlags(2) != 0) {
            this->snapCursor();
        }
    }
}

void ChatMenu::mainAct09() {
    if (this->checkSwitchToTouch() != 0) {
        ChatMenu_StartTouchInput(this);
        return;
    }
    switch (this->navigateText(this->takeRepeatedKeys())) {
    case 1:
        Shared_Keyboard_ResetTypedRun(&this->keyboard);
        this->clearSelection();
        this->updateCaretX();
        ChatMenu_RedrawText(this);
        this->snapCursor();
        Snd_PlaySe(0xb);
        break;
    case 4:
        Shared_Keyboard_SelectSendKey(&this->keyboard);
        this->clearFlags(2);
        this->setMainState(3);
        this->moveCursorToTarget();
        break;
    case 2:
        Keyboard_SelectMenuTabKey(&this->keyboard, ((MenuCursorBase *)&this->cursor)->getScreenX());
        this->clearFlags(2);
        this->setMainState(3);
        ((MenuCursor *)&this->cursor)->switchToAnim0D();
        this->moveCursorToTarget();
        break;
    case 3:
        Shared_Keyboard_EnterTabRowAtX2(&this->keyboard, ((MenuCursorBase *)&this->cursor)->getScreenX());
        this->clearFlags(2);
        this->setMainState(3);
        this->moveCursorToTarget();
        break;
    case 0:
    default:
        if (this->tryCopyButton() != 0) {
            return;
        }
        if (this->tryPasteButton() != 0) {
            return;
        }
        {
            u8 old = this->caretIndex;
            if (this->tryBackspaceButton() != 0) {
                if (old != this->caretIndex) {
                    this->snapCursor();
                }
            } else if ((gPad[1] & 1) != 0) {
                this->setMainState(0xa);
                this->setFlags(1);
                this->selectionStart = this->caretIndex;
                this->selectionEnd = this->caretIndex;
            } else if (this->tryStartSend() != 0) {
                return;
            }
        }
        break;
    }
}

void ChatMenu::mainAct0A() {
    if ((gPad[0] & 1) == 0) {
        this->setMainState(9);
        if (this->selectionStart == this->selectionEnd) {
            this->clearFlags(1);
        }
    } else if (this->navigateText(this->takeRepeatedKeys()) == 1) {
        Shared_Keyboard_ResetTypedRun(&this->keyboard);
        this->updateCaretX();
        this->selectionEnd = this->caretIndex;
        this->snapCursor();
        Snd_PlaySe(0x15);
    }
    ChatMenu_RedrawText(this);
    this->refreshKeys();
}

void ChatMenu::mainAct0B() {
    if ((gPad[0] & 0x200) == 0) {
        this->setMainState(this->returnState);
        this->refreshKeys();
    }
}

void ChatMenu::mainAct0C() {
    if ((gPad[0] & 0x100) == 0) {
        this->setMainState(this->returnState);
        this->refreshKeys();
    }
}

void ChatMenu_StartTouchInput(S *s) {
    s->hideCursor();
    s->setMainState(0);
}

void ChatMenu_StartButtonInput(S *s) {
    s->restartKeyRepeat();
    s->showCursor();
    s->setMainState(3);
    ChatMenu_RedrawText(s);
    s->refreshKeys();
}

void ChatMenu_ResumeInput(S *s) {
    if (MenuCtrl_IsTouch() != 0) {
        ChatMenu_StartTouchInput(s);
    } else {
        ChatMenu_StartButtonInput(s);
    }
}

void ChatMenu_PlayEmotion(S *s) {
    u8 b = Shared_Keyboard_GetSelectedEmotion(&s->keyboard);
    PlayerActor_RequestEmotion(&b);
    s->emotionTimer = 0x14;
}

void ChatMenu_ShowMessage(S *s, u32 v, Unk_ov111_022970cc_Status w) {
    volatile u8 b = gU8None;
    b = v;
    s->errorMessage.open((u8 *)&b, w, 0);
    s->setMainState(0xf);
    s->hideCursor();
}

void ChatMenu::mainAct0D() {
    if (MenuCtrl_IsTouch()) {
        if (Both()) {
            if (this->emotionTimer == 0) {
                if (Keyboard_TouchEmotionKey(&this->keyboard)) {
                    ChatMenu_PlayEmotion(this);
                }
            }
        }
    }
    if (this->sendAnimFrame < 4) {
        this->sendAnimFrame = *(volatile u8 *)&this->sendAnimFrame + 1;
        ChatMenu_LoadSendAnimFrame(this, *(volatile u8 *)&this->sendAnimFrame);
        this->sendAnimDelay = 3;
    } else if (this->sendAnimDelay != 0) {
        this->sendAnimDelay = *(volatile u8 *)&this->sendAnimDelay - 1;
    } else {
        Shared_Keyboard_ResetTypedRun(&this->keyboard);
        this->setMainState(0xe);
    }
}

void ChatMenu::mainAct0E() {
    if (MenuCtrl_IsTouch()) {
        if (Both()) {
            if (this->emotionTimer == 0) {
                if (Keyboard_TouchEmotionKey(&this->keyboard)) {
                    ChatMenu_PlayEmotion(this);
                }
            }
        }
    }
    if (this->sendAnimFrame != 0) {
        if (this->sendAnimFrame == 3) {
            ChatMenu_SendText(this);
        }
        this->sendAnimFrame = this->sendAnimFrame - 1;
        ChatMenu_LoadSendAnimFrame(this, this->sendAnimFrame);
        if (this->sendAnimFrame == 1) {
            if (MenuCtrl_IsButtons()) {
                ((MenuCursorBase *)&this->cursor)->setPoseRelease();
            }
        }
    } else {
        ChatMenu_ClearText(this);
        ChatMenu_RedrawText(this);
        if (MenuCtrl_IsTouch()) {
            this->setMainState(0);
        } else if (this->cursor.getAnim()) {
            this->setMainState(7);
        } else {
            ChatMenu_StartButtonInput(this);
        }
    }
}

void ChatMenu::mainAct0F() {
    Keyboard_UpdatePressedKey(&this->keyboard);
    if (this->errorMessage.update(1)) {
        ChatMenu_ResumeInput(this);
    }
}

void ChatMenu_Init(S *s) {
    s->flags = 0;
    s->unk_99 = 0;
    s->sendAnimFrame = 0;
    s->sendKeyAnim = 0;
    s->emotionTimer = 0;
    ChatMenu_ClearText(s);
    s->textLabel = NULL;
    Keyboard_Init(&s->keyboard, 0);
    Mem_Copy((void *)MenuCtrl_GetChatDraft(), s->text.text, 0x20);
    PlayerActor_OnChatOpenNop();
    if (((MenuTabBar *)ProcBase_GetParent(s))->isJustOpened()) {
        s->setFlags(0x20);
    }
}

void ChatMenu_Exit(S *s) {
    ChatMenu_DestroyTextLabel(s);
    Keyboard_Shutdown(&s->keyboard);
    if (!s->testFlags(0x10)) {
        PlayerActor_OnChatCloseNop();
    }
}

void ChatMenu_LoadSendAnimFrame(S *s, u32 a) {
    u8 *r;
    const char *name;
    Heap *g = gCurrentHeap;
    if (a == 0) {
        name = sChatSendAnimScreens[0];
    } else {
        name = sChatSendAnimScreens[a - 1];
    }
    r = (u8 *)File_LoadAlloc(name, g, -4, 0);
    {
        u8 *src = s->keyboard.screenBuf;
        MI_CpuCopy8(r + 0x100, src + 0x100, 0x140);
        if (a == 1) {
            BgScreen_ReplaceRectPalette((u16 *)src, 0, 4, 0x1f, 9, 5, 6);
        }
    }
    Heap_Free(g, r);
    Shared_Keyboard_MarkScreenDirty(&s->keyboard);
}

void ChatMenu_DrawKeyboard(S *s) {
    u32 r4 = s->getSlideOffsetY() + 0x60;
    u32 t;
    Keyboard_Draw(&s->keyboard, 0x80, r4, 2);
    Keyboard_DrawEmotionKeys(&s->keyboard, 0x80, r4);
    if (s->sendAnimFrame != 0) {
        if (s->sendKeyAnim < 2) {
            s->sendKeyAnim = s->sendKeyAnim + 1;
        }
        t = 7;
    } else {
        if (s->sendKeyAnim != 0) {
            s->sendKeyAnim = s->sendKeyAnim - 1;
        }
        t = 6;
    }
    Keyboard_DrawSendKey(&s->keyboard, 0x80, r4, t, s->sendKeyAnim);
    s->caretBlinkTimer = s->caretBlinkTimer + 1;
    if (s->sendAnimFrame == 0 && (s->caretBlinkTimer & 0x10) != 0) {
        Keyboard_DrawCaret(&s->keyboard, s->caretX + 0x18, r4 - 0x38, 2);
    }
    Shared_Keyboard_DrawCopyPasteKeysChat(&s->keyboard, 0x80, r4);
    Keyboard_DrawLengthGauge(&s->keyboard, 0x80, r4, s->lengthGauge);
}

void ChatMenu_DestroyTextLabel(S *s) {
    if (s->textLabel != NULL) {
        MsgTextLabel_Destroy(s->textLabel);
        s->textLabel = NULL;
    }
}

void ChatMenu_CreateTextLabel(S *s) {
    if (s->textLabel == NULL) {
        s->textLabel = (TextLabel *)MsgTextLabel_CreateVram(0x13d, 0x15, 2);
        if (s->textLabel != NULL) {
            s->textLabel->vramLoader = 3;
            s->textLabel->copyMode = 1;
            s->textLabel->rowStride1K = 0;
            s->textLabel->bgColor = 2;
            s->textLabel->fgColor = 1;
            if (s->hasSelection()) {
                u32 a = s->selectionEnd;
                u32 b = s->selectionStart;
                u32 lo, cnt;
                if (b > a) {
                    lo = a;
                    cnt = b - a;
                } else {
                    lo = b;
                    cnt = a - b;
                }
                s->textLabel->setHighlight(2, 1, lo, cnt);
            } else {
                u32 r = Shared_Keyboard_GetTypedRunLength(&s->keyboard);
                if (r != 0) {
                    s->textLabel->setHighlight(0xe, 2, s->caretIndex - r, r);
                }
            }
        }
    }
}

void ChatMenu_RedrawText(S *s) {
    ChatMenu_CreateTextLabel(s);
    if (s->textLabel != NULL) {
        ((MsgString *)&s->balloonText)->fromEncoded(&s->text, 0, 0);
        TextLabel *t = s->textLabel;
        t->textStart = s->balloonText.data();
        s->textLabel->requestRedraw();
        s->lengthGauge = Text_GetLength(s->text.text, 0x20) * 0x1f / 0x20;
        if (s->lengthGauge > 0x1f) {
            s->lengthGauge = 0x1f;
        }
    }
}

void ChatMenu_SendText(S *s) {
    PlayerData *r = PlayerData_GetCurrent();
    MsgString9B buf;
    ((PlayerId *)r->getPlayerId())->getNameString((MsgString *)&buf);
    String_CensorTaboo(&s->balloonText);
    ChatBalloon_Post(gCommManager->myAid, (StrBuf *)&buf, &s->balloonText);
    ChatMenu_ClearText(s);
    ChatMenu_RedrawText(s);
}

void ChatMenu_BeginSend(S *s) {
    ChatBalloon_Dismiss(gCommManager->myAid);
}

BOOL ChatMenu_InsertChar(S *s, u32 a) {
    if (s->hasSelection()) {
        ChatMenu_DeleteSelection(s);
        Shared_Keyboard_ResetTypedRun(&s->keyboard);
    }
    if (Keyboard_InsertChar(&s->keyboard, s->text.text, a, &s->caretIndex, 0x20, 0xa0, 0, 1)) {
        s->updateCaretX();
        ChatMenu_RedrawText(s);
    } else {
        return FALSE;
    }
    return TRUE;
}

BOOL ChatMenu_Backspace(S *s, Unk_ov111_022970cc_Status a) {
    if (s->hasSelection()) {
        Shared_Keyboard_ResetTypedRun(&s->keyboard);
        Snd_PlaySe(0x35);
    } else if (s->caretIndex != 0) {
        s->selectionStart = s->caretIndex;
        s->selectionEnd = s->caretIndex - 1;
        Snd_PlaySe(0x35);
    } else if (s->text.text[0] != 0) {
        s->selectionStart = 0;
        s->selectionEnd = 1;
        Snd_PlaySe(0x35);
    } else {
        if (a != 0) {
            Snd_PlaySe(0x34);
        }
        return FALSE;
    }
    ChatMenu_DeleteSelection(s);
    ChatMenu_RedrawText(s);
    s->updateCaretX();
    return TRUE;
}

void ChatMenu_DeleteSelection(S *s) {
    u32 a = s->selectionEnd;
    u32 b = s->selectionStart;
    u32 lo, hi;
    if (b > a) {
        lo = a;
        hi = b;
    } else {
        lo = b;
        hi = a;
    }
    s->caretIndex = Shared_Keyboard_DeleteRange(&s->keyboard, s->text.text, lo, hi, 0x20);
    s->clearSelection();
}

BOOL ChatMenu_TryModifier103(S *s) {
    u32 a = s->getCharBeforeCursor();
    if (a == 0) return FALSE;
    u32 b = Keyboard_ModifyCharKey103(&s->keyboard, a);
    if (b == 0) return FALSE;
    if (Shared_Keyboard_ReplaceCharBeforeCursor(&s->keyboard, s->text.text, b, s->caretIndex)) return TRUE;
    return FALSE;
}

void ChatMenu_ApplyModifier103(S *s) {
    if (ChatMenu_TryModifier103(s)) {
        s->updateCaretX();
        ChatMenu_RedrawText(s);
    } else {
        ChatMenu_PlayErrorSe(s);
    }
}

BOOL ChatMenu_TryModifier104(S *s) {
    u32 a = s->getCharBeforeCursor();
    if (a == 0) return FALSE;
    u32 b = Shared_Keyboard_ModifyCharKey104(&s->keyboard, a);
    if (b == 0) return FALSE;
    if (Shared_Keyboard_ReplaceCharBeforeCursor(&s->keyboard, s->text.text, b, s->caretIndex)) return TRUE;
    return FALSE;
}

void ChatMenu_ApplyModifier104(S *s) {
    if (ChatMenu_TryModifier104(s)) {
        s->updateCaretX();
        ChatMenu_RedrawText(s);
    } else {
        ChatMenu_PlayErrorSe(s);
    }
}

BOOL ChatMenu_TryModifier105(S *s) {
    u32 a = s->getCharBeforeCursor();
    if (a == 0) return FALSE;
    u32 b = Keyboard_ModifyCharKey105(&s->keyboard, a);
    if (b == 0) return FALSE;
    if (Shared_Keyboard_ReplaceCharBeforeCursor(&s->keyboard, s->text.text, b, s->caretIndex)) return TRUE;
    return FALSE;
}

void ChatMenu_ApplyModifier105(S *s) {
    if (ChatMenu_TryModifier105(s)) {
        s->updateCaretX();
        ChatMenu_RedrawText(s);
    } else {
        ChatMenu_PlayErrorSe(s);
    }
}

void ChatMenu_ClearText(S *s) {
    s->caretBlinkTimer = 0x10;
    s->caretX = 0;
    s->caretIndex = 0;
    s->clearSelection();
    s->refreshKeys();
    Keyboard_ResetKeyPalettes(&s->keyboard);
    Mem_Clear(s->text.text, 0x20);
}

void ChatMenu_Copy(S *s) {
    if (s->hasSelection()) {
        u32 a = s->selectionEnd;
        u32 b = s->selectionStart;
        u32 lo, cnt;
        if (b > a) {
            lo = a;
            cnt = b - a;
        } else {
            lo = b;
            cnt = a - b;
        }
        Mem_Clear(s->clipboard, 0x20);
        Mem_Copy(s->text.text + lo, s->clipboard, cnt);
        s->setFlags(8);
        Shared_Keyboard_PlayCopySe();
        s->refreshKeys();
    }
}

void ChatMenu_Paste(S *s) {
    if (s->testFlags(8)) {
        s32 n;
        s32 i;
        s32 z;
        Keyboard_BeginPaste(&s->keyboard);
        if (s->hasSelection()) {
            s->caretIndex = Shared_Keyboard_DeleteRange(&s->keyboard, s->text.text, s->selectionStart, s->selectionEnd, 0x20);
            s->clearFlags(1);
        }
        Shared_Keyboard_ResetTypedRun(&s->keyboard);
        n = Text_GetLength(s->clipboard, 0x20);
        i = 0;
        z = i;
        for (; i < n; i++) {
            if (!Keyboard_InsertChar(&s->keyboard, s->text.text, s->clipboard[i], &s->caretIndex, 0x20, 0xa0, z, z)) {
                if (i == 0) {
                    ChatMenu_PlayErrorSe(s);
                }
                i = n;
            }
        }
        s->updateCaretX();
        ChatMenu_RedrawText(s);
        Keyboard_PlayPasteSe();
        Keyboard_EndPaste(&s->keyboard);
    }
}

void ChatMenu_PlayErrorSe(S *s) {
    Snd_PlaySe(0x34);
}

s32 ChatMenu::pressKeyCode(u32 x) {
    Unk_ov111_022970cc_Status r = UNK_OV111_ST_1;
    s32 t = Shared_Keyboard_HandleModeKey(&keyboard, x, 6);
    if (t != 0) {
        ChatMenu_RedrawText(this);
        return t;
    }
    if (Keyboard_IsControlCode(&keyboard, x)) {
        switch (x - 0x100) {
        case 0:
            ChatMenu_Backspace(this, r);
            break;
        case 3:
            ChatMenu_ApplyModifier103(this);
            r = UNK_OV111_ST_2;
            break;
        case 4:
            ChatMenu_ApplyModifier104(this);
            r = UNK_OV111_ST_2;
            break;
        case 5:
            ChatMenu_ApplyModifier105(this);
            r = UNK_OV111_ST_2;
            break;
        case 6:
            break;
        case 24:
            ChatMenu_Copy(this);
            r = UNK_OV111_ST_2;
            break;
        case 25:
            ChatMenu_Paste(this);
            r = UNK_OV111_ST_2;
            break;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
            ChatMenu_RequestTab(this, x - 0x10a, 0);
            r = UNK_OV111_ST_2;
            break;
#ifdef N3DS_DIRECT
        case N3DS_KB_OPT_CODE - 0x100:  // (the options button: N3DS_KB_OPT_KEY)
            ChatMenu_RequestTab(this, N3DS_OPT_TAB, 0);
            r = UNK_OV111_ST_2;
            break;
#endif
        case 27:
        case 28:
        case 29:
        case 30:
            if (emotionTimer != 0) {
                return 0;
            }
            Shared_Keyboard_SelectEmotionByCode(&keyboard, x);
            ChatMenu_PlayEmotion(this);
            releaseCursor();
            r = UNK_OV111_ST_3;
            break;
        default:
            return 0;
        }
    } else {
        s32 r6 = ChatMenu_InsertChar(this, (u8)x);
        if (Keyboard_IsFull(&keyboard)) {
            ChatMenu_ShowMessage(this, 0x1c, r);
            return 4;
        }
        if (Keyboard_IsTooWide(&keyboard)) {
            ChatMenu_ShowMessage(this, 0x1c, r);
            return 4;
        }
        if (r6 == 0) {
            ChatMenu_PlayErrorSe(this);
        }
    }
    return r;
}

s32 ChatMenu::touchKey() {
    Shared_Keyboard_ClearHighlight(&keyboard);
    s32 r4 = Shared_Keyboard_TouchKey(&keyboard, gTouchCurX, gTouchCurY);
    if (r4 == -1) {
        return 0;
    }
    s32 r6 = pressKeyCode(Keyboard_GetKeyCode(&keyboard, r4, 8));
    Shared_Keyboard_HighlightKey(&keyboard, r4);
    Keyboard_StartKeyRepeat(&keyboard);
    return r6;
}

BOOL ChatMenu::touchSendKey() {
    if (sendKeyAnim != 0) {
        return FALSE;
    }
    if (Shared_Keyboard_TouchSendKey()) {
        return startSend();
    }
    return FALSE;
}

BOOL ChatMenu::startSend() {
    if (sendKeyAnim != 0) {
        return FALSE;
    }
    if (Text_GetLength(text.text, 0x20) == 0) {
        ChatMenu_PlayErrorSe(this);
    } else {
        PlayerActor_RequestAct13();
        Snd_PlaySe(0x32);
        ChatMenu_BeginSend(this);
        return TRUE;
    }
    return FALSE;
}

BOOL ChatMenu::touchTextField() {
    s32 a = gTouchCurX;
    s32 b = gTouchCurY;
    if (b < 0x24 || b >= 0x3c) {
        return FALSE;
    }
    if (a < 0x10 || a >= 0xc8) {
        return FALSE;
    }
    if (a < 0x18) {
        a = 0x18;
    }
    setCaretFromTouchX(a);
    setFlags(1);
    selectionStart = caretIndex;
    selectionEnd = caretIndex;
    return TRUE;
}

void ChatMenu::dragSelection() {
    s32 v = gTouchCurX;
    if (v < 0x18) {
        v = 0x18;
    }
    if (v >= 0xc8) {
        v = 0xc7;
    }
    setCaretFromTouchX(v);
    if (selectionEnd != caretIndex) {
        Snd_PlaySe(0x15);
    }
    selectionEnd = caretIndex;
}

void ChatMenu::setCaretFromTouchX(u32 v) {
    caretX = v - 0x18;
    snapCaretToText();
    Shared_Keyboard_ResetTypedRun(&keyboard);
    ChatMenu_RedrawText(this);
}

void ChatMenu::snapCaretToText() {
    caretX = Keyboard_HitTestText(&keyboard, text.text, 0x20, 0xa0, caretX, &caretIndex);
    caretBlinkTimer = 0x10;
    refreshKeys();
}

void ChatMenu::updateCaretX() {
    caretX = Text_MeasureWidth(text.text, caretIndex);
    caretBlinkTimer = 0x10;
    refreshKeys();
}

u32 ChatMenu::getCharBeforeCursor() {
    if (caretIndex == 0) {
        return 0;
    }
    return text.text[caretIndex - 1];
}

BOOL ChatMenu::hasSelection() {
    if (!testFlags(1) || selectionStart == selectionEnd) {
        return FALSE;
    }
    return TRUE;
}

void ChatMenu::clearSelection() {
    selectionStart = 0;
    selectionEnd = 0;
    clearFlags(1);
}

s32 ChatMenu::navigateText(u32 pad) {
    if (pad == 0) {
        return 0;
    }
    if (MenuKeys_HasLeft(pad)) {
        if (*(volatile u8 *)&caretIndex != 0) {
            caretIndex = *(volatile u8 *)&caretIndex - 1;
            return 1;
        }
    } else if (MenuKeys_HasRight(pad)) {
        if (*(volatile u8 *)&caretIndex + 1 <= Text_GetLength(text.text, 0x20)) {
            caretIndex = *(volatile u8 *)&caretIndex + 1;
            return 1;
        }
        return 4;
    }
    if (MenuKeys_HasUp(pad)) {
        return 2;
    }
    if (MenuKeys_HasDown(pad)) {
        return 3;
    }
    return 0;
}

BOOL ChatMenu::tryPressKey() {
    if ((gPad[1] & 1) == 0) {
        return FALSE;
    }
    s32 r4 = Shared_Keyboard_PressCursorKey(&keyboard);
    if (r4 == -1) {
        return FALSE;
    }
    if (Keyboard_GetKeyCode(&keyboard, r4, 8) == 0x106) {
        if (startSend()) {
            ((MenuCursor *)&cursor)->setPosePress();
            setMainState(0xd);
            return TRUE;
        }
        return FALSE;
    }
    Shared_Keyboard_HighlightKey(&keyboard, r4);
    pressCursor();
    return TRUE;
}

BOOL ChatMenu::tryBackspaceButton() {
    if ((gPad[1] & 2) == 0) {
        return FALSE;
    }
    Keyboard_ShrinkTypedRun(&keyboard);
    if (ChatMenu_Backspace(this, UNK_OV111_ST_0)) {
        Keyboard_StartKeyRepeat(&keyboard);
        returnState = mainState;
        setMainState(8);
    } else {
        ChatMenu_RequestTab(this, 7, 0);
    }
    return TRUE;
}

BOOL ChatMenu::tryPasteButton() {
    if ((gPad[1] & 0x100) == 0) {
        return FALSE;
    }
    if (Keyboard_IsSlotDisabled(&keyboard, 0xc)) {
        return FALSE;
    }
    pressKeyCode(0x119);
    Shared_Keyboard_HighlightKey(&keyboard, 0xdc);
    snapCursor();
    returnState = mainState;
    setMainState(0xc);
    return TRUE;
}

BOOL ChatMenu::tryCopyButton() {
    if ((gPad[1] & 0x200) == 0) {
        return FALSE;
    }
    if (Keyboard_IsSlotDisabled(&keyboard, 0xb)) {
        return FALSE;
    }
    pressKeyCode(0x118);
    Shared_Keyboard_HighlightKey(&keyboard, 0xdb);
    returnState = mainState;
    setMainState(0xb);
    return TRUE;
}

BOOL ChatMenu::tryStartSend() {
    if ((gPad[1] & 8) == 0) {
        return FALSE;
    }
    if (startSend()) {
        hideCursor();
        setMainState(0xd);
        return TRUE;
    }
    return FALSE;
}

void ChatMenu::showCursor() {
    clearFlags(2);
    Keyboard_ResetCursor(&keyboard);
    s32 a = Keyboard_GetCursorX(&keyboard);
    s32 b = Keyboard_GetCursorY(&keyboard);
    ((MenuCursorBase *)&cursor)->warpTo(a, b);
    ((MenuCursor *)&cursor)->setAnimIfChanged(1);
    returnToKeyNav();
}

void ChatMenu::hideCursor() {
    ((MenuCursor *)&cursor)->setAnimIfChanged(0);
    cursor.update();
}

void ChatMenu::moveCursorToTarget() {
    if (testFlags(2)) {
        ((MenuCursorBase *)&cursor)->moveToEase(caretX + 0x18, 0x28, 3, 1);
        returnState = 9;
    } else {
        s32 a = Keyboard_GetCursorX(&keyboard);
        s32 b = Keyboard_GetCursorY(&keyboard);
        ((MenuCursorBase *)&cursor)->moveToEase(a, b, 2, 1);
        returnState = 3;
    }
    setMainState(4);
}

void ChatMenu::snapCursor() {
    if (testFlags(2)) {
        ((MenuCursorBase *)&cursor)->warpTo(caretX + 0x18, 0x28);
    } else {
        s32 a = Keyboard_GetCursorX(&keyboard);
        s32 b = Keyboard_GetCursorY(&keyboard);
        ((MenuCursorBase *)&cursor)->warpTo(a, b);
    }
    cursor.update();
}

void ChatMenu::pressCursor() {
    ((MenuCursor *)&cursor)->setPosePress();
    setMainState(5);
}

void ChatMenu::releaseCursor() {
    ((MenuCursorBase *)&cursor)->setPoseRelease();
    setMainState(7);
}

void ChatMenu::returnToKeyNav() {
    ((MenuCursorBase *)&cursor)->setPoseIdle();
    cursor.update();
    setMainState(3);
}

void ChatMenu::setKeyboardMode(u32 a) { Keyboard_SetMode(&keyboard, a, 6, 1); }

void ChatMenu::endKeyboardFrame() { Keyboard_EndFrame(&keyboard, 6); }

void ChatMenu::refreshKeys() {
    Keyboard_DisableKey(&keyboard, 0);
    if (hasSelection()) {
        Shared_Keyboard_EnableKey(&keyboard, 0xb);
    } else {
        Keyboard_DisableKey(&keyboard, 0xb);
    }
    if (testFlags(8)) {
        Shared_Keyboard_EnableKey(&keyboard, 0xc);
    } else {
        Keyboard_DisableKey(&keyboard, 0xc);
    }
    if (hasSelection()) {
        Shared_Keyboard_DisableModifierKeys(&keyboard);
        Shared_Keyboard_EnableKey(&keyboard, 6);
    } else if (caretIndex == 0) {
        Shared_Keyboard_DisableModifierKeys(&keyboard);
    } else {
        Keyboard_UpdateModifierKeys(&keyboard, getCharBeforeCursor());
    }
}

BOOL ChatMenu::testFlags(u32 mask) {
    if (flags & mask) {
        return TRUE;
    }
    return FALSE;
}

void ChatMenu::setFlags(u32 mask) { flags = flags | mask; }

void ChatMenu::clearFlags(u32 mask) { flags = flags & ~mask; }

u32 EncodedString32::capacity() { return 0x20; }

u8 *EncodedString32::data() { return text; }

// Data definition order is chosen so mwcc emits the objects in the original order
extern "C" char data_ov111_022989c8[] = "menu/chat2/b_cht.bsc";

extern "C" char data_ov111_022989e0[] = "menu/chat2/b_cht_0.bsc";

extern "C" char data_ov111_022989f8[] = "menu/chat2/b_cht_1.bsc";

extern "C" char data_ov111_02298a10[] = "menu/chat2/b_cht_2.bsc";

extern "C" const char *sChatSendAnimScreens[4] = {data_ov111_022989c8, data_ov111_022989e0, data_ov111_022989f8, data_ov111_02298a10};

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_02296840) {
    HOST_OVERLAY_VAR(data_ov111_02298920),
    HOST_OVERLAY_VAR(data_ov111_022989c8),
    HOST_OVERLAY_VAR(data_ov111_022989e0),
    HOST_OVERLAY_VAR(data_ov111_022989f8),
    HOST_OVERLAY_VAR(data_ov111_02298a10),
    HOST_OVERLAY_VAR(sChatSendAnimScreens),
    HOST_OVERLAY_VARS_END
};

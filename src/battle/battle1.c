//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"
#include <libetc.h>
#include <libgpu.h>

static void func_800B37A0(void);
static void func_800B37EC(void);
static void func_800B38E0(void);
static void func_800B3D38(void);
static void func_800B3D88(void);
static void func_800B3DBC(void);
static s32 func_800B3FAC(s32 arg0);
static void func_800B7FDC(void);
static void func_800B8360(s32);
static void func_800B85E0();
static void func_800B88CC(s32 battler_idx);
static void func_800BA4C8(void);
void func_800BA598(s16);
static void func_800BB030(s16);
static void func_800BB75C(Unk800BB75C* arg0, MATRIX* m, s16* arg2, s16* arg3);
static void func_800BB804(void);
static void func_800BB864(void);
static void func_800C4D10(void);
DR_MODE* func_800C4DC8(s16 x, s16 y, s16 w, s16 h, s32*);
static void func_800C627C(void);
void func_800C62F4(s32);
void func_800BC81C(s16 arg0, s16 arg1);

void func_800B30E4(void) {
    s32 i;

    D_801517C0 = &D_800FAFF4;
    D_801031E4 = 0;
    D_800F8368 = 0;
    D_80162084 = 0x200;
    func_800B383C();
    func_800B430C();
    VSync(0);
    SetDispMask(0);
    D_800F9F34 = 0;
    *(s8*)&D_800FA63C.u.sub.unk34 = 0;
    D_800FA6A0 = 0;
    func_800B37A0();
    func_800B3E2C();
    func_800BB684();
    func_800BC04C(func_800C4D10);
    func_800B7FDC();
    func_800B7FDC();
    do {
    } while (D_80095DD4);
    func_800B37EC();
    SetDispMask(1);
    while (1) {
        switch (D_80163C7C) {
        case 0:
            D_801635FC = 0x3D;
            func_800B38E0();
            func_800B7FDC();
            D_80163C7C = 1;
            break;
        case 1:
            func_800B7FDC();
            if (D_800F7DF4 == (u8)D_80166F64 && D_801518DC == 0) {
                func_800B3D38();
                func_800B5138();
                D_80163C7C = 6;
            }
            break;
        case 6:
            func_800B7FDC();
            func_800B3D88();
            for (i = 4; i < D_800F7E04[0] + 4; i++) {
                D_801518E4[i].D_80151922 |= 4;
            }
            D_80163C7C = 2;
            break;
        case 2:
            func_800B7FDC();
            if ((u8)D_80166F64 == 3 && D_801518DC == 0) {
                func_800B3DBC();
                D_80163C7C = 3;
                D_801518E4[0].D_80151922 |= 4;
                D_801518E4[1].D_80151922 |= 4;
                D_801518E4[2].D_80151922 |= 4;
            }
            break;
        case 3:
            func_800B7FDC();
            if (D_801635FC == 0) {
                D_80163C7C = 4;
                func_800C61C0();
            }
            break;
        default:
            return;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B33A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B36B4);

// one-shot setup call centered on the 320x240 screen
static void func_800B37A0(void) {
    func_800D91DC(0x140, 0xF0, D_80162084, D_800FA6A0, D_800FA63C.u.sub.unk34,
                  D_800F9F34);
}

static void func_800B37EC(void) {
    D_80162094 = 4;
    func_800D8A78(4);
    func_800E15D8();
    func_800D9E0C(-1, -1, 0);
    D_80095DD4 = 2;
}

// Load stage files
INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B383C);

// load stage entry i (D_800F7DF8[0]) into VRAM staging via DS_read;
// D_800E8050[i] holds a {loc,len} disk-sector pair, same shape as the Yamada
// record at D_800E8068
static void func_800B38E0(void) {
    s32 i = D_800F7DF8[0];

    DS_read(((u32*)D_800E8050)[i * 2], ((u32*)D_800E8050)[i * 2 + 1],
            0x801B0000, &func_800B3A04);
    func_800B7FB4();
}

static void func_800B3934(void) {
    func_800B5D38(2);
    func_800B5CD4(2);
    D_80166F64 = 3;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3968);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3A04);

void func_800B3AB8(void);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3AB8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3B84);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3C50);

static void func_800B3CD0(void) {
    Yamada* y;
    u_long* dst;

    dst = (u_long*)0x801B0000;
    func_800D2980(dst, 0, 0, 0);
    y = &D_800E8068[D_800FA9C4];
    DS_read(y->loc, *&D_800E8068[D_800FA9C4].len, dst, func_800B3AB8);
    func_800B7FB4();
}

static void func_800B3D38(void) {
    func_800C5E94();
    D_800F839C = D_800EA50C;
    DS_read(LBA_ENEMY6_SEFFECT, 0xA800, (u_long*)0x801B0000, func_800B3CD0);
    func_800B7FB4();
}

static void func_800B3D88(void) {
    func_800B588C();
    func_800B6B98(4, 10);
    func_800B36B4();
}

static void func_800B3DBC(void) {
    s32 i;

    func_800B4794();
    func_800B6B98(0, 3);
    func_800B6B98(3, 3);
    if (D_8016360C.setup.stageID == 57) {
        for (i = 0; i < 10; i++) {
            D_801518E4[i].D_80151909 |= 0x10;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3E2C);

// search the formation's 6 enemy slots for one whose enemyID matches arg0;
// if found, bump a counter and return 0, else return -1
static s32 func_800B3FAC(s32 arg0) {
    s32 i;
    u8* p = &D_800F7DF4;

    for (i = 0; i < (s32)sizeof(D_8016360C.formation);
         i += sizeof(FormationEntry)) {
        if (((FormationEntry*)((u8*)D_8016360C.formation + i))->enemyID ==
            arg0) {
            *p += 1;
            return 0;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B3FFC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B430C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B45F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B46B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B4794);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B4E30);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5138);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B54B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B588C);

// "Restore normal appearance" -- resets a battler's model position/scale
// and a handful of Unk80151200 timers/flags back to defaults, and zeroes
// each BattleModelSub's sv2.vy (count from BattleModel.unk10). Called by
// func_800C33F0 once doDeathSequence's fade finishes. arg1/arg2 are read
// by callers (func_800C33F0 passes targetIdx and a slot byte-offset) but
// never referenced in this function body -- genuinely dead arguments, not
// a mistake in this trace (same situation as func_800A3534's arg0).
// Initial decomp (m2c + manual cleanup) -- NOT byte-matched yet.
void func_800B5AAC(s32 battlerIdx, s16 arg1, s32 arg2) {
    s32 idx = battlerIdx & 0xFF;
    s32 i;

    D_801518E4[idx].D_80151A4C.vx = D_80163C80[idx].vx;
    D_801518E4[idx].D_80151A4C.vy = D_80163C80[idx].vy;
    D_801518E4[idx].D_801518EA = 0x1000;
    D_801518E4[idx].D_80151A4C.vz = D_80163C80[idx].vz;

    D_80151200[idx].D_8015123C = 0x1000;
    D_80151200[idx].D_8015123A = 0x1000;
    D_80151200[idx].D_80151238 = 0x1000;
    D_80151200[idx].D_8015120C = 0;
    D_80151200[idx].D_80151200 = 0;

    for (i = 0; i < D_801518E4[idx].unk10; i++) {
        D_801518E4[idx].D_80151A58[i].sv2.vy = 0;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5C1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5CD4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5D38);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B5E64);

void func_800B60E0(s16);
void func_800B5FC4(s16 arg0) { func_800B60E0(arg0); }

// Sets a "dirty/flash" bit (0x8) across a variable-length run of per-part
// flag bytes for a battler's main model (count from BattleModel.unk10,
// bytes starting right after the named field D_80151923) and, separately,
// for its D_800FA6D8 sub-model (count from .unk3C, bytes in .unk3E[]).
// Initial decomp (m2c + manual cleanup) -- NOT byte-matched yet.
void func_800B5FE8(s16 battlerIdx) {
    s32 i;

    for (i = 0; i < D_801518E4[battlerIdx].unk10; i++) {
        (&D_801518E4[battlerIdx].D_80151923)[i] |= 8;
    }
    for (i = 0; i < D_800FA6D8[battlerIdx].unk3C; i++) {
        D_800FA6D8[battlerIdx].unk3E[i] |= 8;
    }
}

void func_800B60E0(s16);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B60E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B64CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B677C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B6B98);

// drains D_80163798 (12-byte entries, -1-terminated, index D_801590E0), one
// entry per call, dispatched by a type byte (0-5, jtbl_800A05FC) via m2c
// structural read (not yet decompiled):
//   0 callback-driven step (func_800BC04C(&func_800C494C)), immediate
//   1 gated on D_800F7DE4: walks a linked status list (D_800FA9D0/1/2),
//     looks like "hide next status icon" (sets D_800FA6D4/D_80161EEC/
//     D_800F99E8 icon slots, or 0xF when the list is exhausted)
//   2 func_800C5C18(4 entry fields), immediate -- shape matches a sound cue
//   3 gated on D_800F7DE4: same linked-list shape as case 1, opposite flag
//     direction -- looks like "show next status icon"
//   4 gated on D_800F7DE4: constructs func_800C5694 (via func_800BBEAC) --
//     a D_80162978 job that tweens D_801518E4[target].D_801518EA (a
//     0x1000-scale fixed-point factor, confirmed elsewhere multiplied
//     against D_80151A4C.vy -- a scale/blend factor, NOT curHP) from its
//     current value toward this entry's target value over N ticks, one
//     step per tick (preset_idx repurposed as the target battler index,
//     tick_count repurposed as the per-tick increment)
//   5 immediate: sets a per-actor "step complete" flag, conditionally
//     copies animation-state fields
// D_800F7DE4 (the gate for cases 1/3/4) is set once per frame by
// func_800B7FDC below, once all actor slots are ready -- so this function
// is a generic "process the next queued visual/counter effect, one per
// frame" drainer, not itself the source of any particular command's
// damage/effect. See func_800A4AF4's comment in battle.c: opcode 0x14 just
// spins this to drain whatever's already queued
INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B6D6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B7764);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B798C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B79F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B7DB4);

static void func_800B7F6C(void) {
    volatile s32 padding;

    while (D_80062D99) {
        func_800B7FB4();
    }
    D_80062D98 = 0;
}

void func_800B7FB4(void) { D_801518DC = func_80034B44(); }

// per-frame tick: pumps the GPU ordering-table draw lists, runs render/vsync,
// drains the action-queue ring buffer (func_800A3ED0 -- see the queue-push
// writeup), and sets D_800F7DE4 = 1 exactly once per frame once every actor
// slot is ready and D_80162080 (a per-frame counter) reaches 0. func_800B6D6C
// gates several of its event-queue steps on this flag, effectively waiting
// for "the next frame is ready" before consuming a queued effect
static void func_800B7FDC(void) {
    s32 i;

    func_800B7FB4();
    ClearOTagR((u_long*)D_801517C0->unk40A4, LEN(D_801517C0->unk40A4));
    ClearOTag((u_long*)D_801517C0->unk4070, LEN(D_801517C0->unk4070));
    ClearOTag((u_long*)D_801517C0->unk4078, LEN(D_801517C0->unk4078));
    ClearOTagR((u_long*)D_801517C0->unk70, LEN(D_801517C0->unk70));
    ClearOTagR((u_long*)D_801517C0->unk4080, LEN(D_801517C0->unk4080));
    ClearOTag((u_long*)D_801517C0->unk40E4, LEN(D_801517C0->unk40E4));
    ClearOTag((u_long*)D_801517C0->unk40EC, LEN(D_801517C0->unk40EC));
    D_80163C74 = D_800F8368 == 0 ? (DR_MODE*)0x80168000 : (DR_MODE*)0x80184000;
    func_800B8360(1);
    func_800C5CC0();
    func_800B8438();
    for (i = 0; i < 10; i++) {
        if (D_801518E4[i].D_8015190A == 0) {
            D_800F7DE4 = 0;
            break;
        }
        if (D_80162080 == 0) {
            D_800F7DE4 = 1;
        } else {
            D_800F7DE4 = 0;
        }
    }
    func_800A3ED0();
    func_800B8360(2);
    func_800DCFD4((u_long*)D_801517C0->unk40E4);
    if (D_800F9D94 == 0) {
        ResetGraph(1);
        D_800F9D94 = 1;
    }
    if (D_8016376A & 2) {
        func_800E16B8(D_801517C0->unk40E4, 0x10, 0x10, D_8009D268[0]);
    }
    D_800FA9B8 = VSync(1);
    BATTLE_FlushImageQueue();
    func_800B7FB4();
    D_80158D08 = func_800D8A88();
    SetGeomScreen(D_80162084);
    D_801516F4++;
    func_800B7F6C();
    func_800B950C();
    D_801516A0 = D_800F198C;
}

void func_800B8234(s32 arg0) {
    if (arg0) {
        func_800D0C80(D_801590CC);
        D_801517BC = 0;
    }
}

static void func_800B8268(void) {
    s32 i;
    u8* var_a1;
    s32 var_t1;

    i = 0;
    var_t1 = 1;
    var_a1 = D_80163784;
    while (i < 10) {
        *var_a1 = D_801636B8[i].D_801636B9;
        if (!(D_80151200[i].D_8015120C & 8) &&
            D_801518E4[i].D_801518E6 != *var_a1 &&
            D_801518E4[i].D_8015190A == var_t1) {
            D_801518E4[i].D_80151922 |= 1;
            D_801518E4[i].D_801518E6 = *var_a1;
        }
        var_a1++;
        i += 1;
    }
    D_80163787 = 0;
}

// build a draw-mode prim (texture page selected by arg0) and add it to the OT
static void func_800B8360(s32 arg0) {
    SetDrawMode(D_80163C74, 1, 1, (arg0 & 3) << 5, 0);
    AddPrim(D_801517C0->unk4078, D_80163C74++);
}

static void func_800B83C4() {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (D_80151200[i].D_8015120C & 1) {
            func_800BA4C8();
            func_800BA40C();
            return;
        }
    }
    func_800BA40C();
    func_800BA4C8();
}

void func_800B8438(void) {
    func_800B9568();
    if (D_801635FC) {
        D_801635FC--;
    }
    switch (D_80163C7C) {
    case 2:
        func_800B905C();
        func_800BC440();
        func_800BA4C8();
        break;
    case 0:
    case 1:
    case 6:
        break;
    case 3:
    case 4:
    case 5:
    default:
        func_800B8EE4();
        func_800B905C();
        func_800B8234(D_801517BC);
        func_800BC440();
        func_800B7FB4();
        func_800B83C4();
        func_800B8B48();
        break;
    }
    func_800B7FB4();
    func_800B91CC();
    D_80151694 = D_80163758[1];
    func_800B85E0();
    func_800BC81C(D_800F8370, D_801518E4[D_801590CC].D_80151906);
    func_800BC8B0(D_800F8370);
    func_800B8268();
    SetFarColor(0, 0, 0);
    func_800BC538();
    func_800BC348();
    func_800BB75C(&D_800FA63C, &D_800FA958, &D_80158D00, &D_801031E8);
    func_800C627C();
}

static void func_800B85E0() {
    s32 i;

    if (D_800F7ED4 != 100 && D_800FA6B8) {
        func_800BB804();
        D_80163C7C = 5;
        func_800D8B2C();
        D_800F7ED4 = 100;
        D_80163798[D_801590E0].unk8 = -3;
        func_800BB684();
        for (i = 0; i < 3; i++) {
            D_801518E4[i].D_80151922 |= 0x20;
            D_80151200[i].D_80151200 = D_801636B8[i].D_801636C0;
        }
    }
    if (D_800F9D98 != 100 && (D_800707BE & 1)) {
        D_80163C7C = 5;
        func_800D8B2C();
        D_800F9D98 = 100;
        D_80163798[D_801590E0].unk8 = -1;
        func_800BB684();
    }
    if (!D_801590D8 && D_80163B80) {
        func_800BB864();
        D_801590D8 = 1;
    }
    if (D_800F9D9C != 100) {
        i = 0;
        if (D_800707BE & 8) {
            for (; i < 3; i++) {
                D_801518E4[i].D_80151922 |= 1;
                D_801518E4[i].D_801518E6 = D_801636B8[i].D_801636B9;
                D_801518E4[i].D_80151922 |= 0x20;
                D_80151200[i].D_80151200 = D_801636B8[i].D_801636C0;
            }
            D_800F9D9C = 100;
            D_80163C7C = 5;
            func_800D8B2C();
            D_80163798[D_801590E0].unk8 = -1;
            func_800BB684();
        }
    }
}

extern u8 D_801517F0[0x4E];

s16 func_800B888C(s32 arg0) {
    s32 i;

    for (i = 0; i < LEN(D_801517F0); i++) {
        if (arg0 == D_801517F0[i]) {
            return i;
        }
    }
}

// Registers func_800CE970 for battler_idx, fires immediately (delay=0).
static void func_800B88CC(s32 battler_idx) {
    s32 v = func_800BBEAC(&func_800CE970);

    D_80162978[v].delay = 0;
    D_80162978[v].target_battler_idx = battler_idx;
    func_800B8A34(func_800B888C(battler_idx), v);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8944);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8A34);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8B48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8E48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8EE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B8FCC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B905C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B91CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B950C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800B9568);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA11C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA24C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA2BC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA360);

static void func_800BA40C(void) {
    s32 i;
    u8 param;

    for (i = 0; i < 3; i++) {
        if (!(D_801518E4[i].D_80151909 & 2)) {
            param = i;
            func_800C1908(param);
            func_800BA598(i);
            if (D_801518E4[i].D_8015190B & 0x80) {
                func_800BB2A8(param);
                func_800BB030(i);
            }
        }
    }
}

static void func_800BA4C8(void) {
    s32 i;

    for (i = 4; i < D_800F7E04[0] + 4; i++) {
        if (!(D_801518E4[i].D_80151909 & 0x80)) {
            continue;
        }
        if (D_801518E4[i].D_80151909 & 2) {
            continue;
        }
        func_800C1908(i);
        func_800BA598(i);
        if (D_801518E4[i].D_8015190B & 0x80) {
            func_800BB030(i);
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BA598);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BACEC);

static void func_800BAF34(BattleModelSub* modelSub) {
    s32 flag;

    *(MATRIX**)0x1F800020 = modelSub->pm;
    *(MATRIX*)0x1F800024 = **(MATRIX**)0x1F800020;
    MulMatrix2((MATRIX*)0x1F800024, &modelSub->m);
    SetRotMatrix((MATRIX*)0x1F800024);
    SetTransMatrix((MATRIX*)0x1F800024);
    RotTrans(&modelSub->sv2, (VECTOR*)modelSub->m.t, &flag);
    SetRotMatrix(&modelSub->m);
    SetTransMatrix(&modelSub->m);
}

static void func_800BAFF8(MATRIX* m, VECTOR* v) {
    ScaleMatrix(m, v);
    SetRotMatrix(m);
    SetTransMatrix(m);
}

static void func_800BB030(s16 arg0) {
    s32 i;
    Unk801B0C98* unk;

    unk = (Unk801B0C98*)0x1F800020;
    SetFarColor(D_801518E4[arg0].D_8015190C, D_801518E4[arg0].D_8015190D,
                D_801518E4[arg0].D_8015190E);
    SetRotMatrix(&D_801518E4[arg0].m);
    SetTransMatrix(&D_801518E4[arg0].m);
    for (i = 0; i < D_800FA6D8[arg0].unk3C; i++) {
        RotMatrixYXZ(
            &D_800FA6D8[arg0].unk8[i].sv1, &D_800FA6D8[arg0].unk8[i].m);
    }

    for (i = 0; i < D_800FA6D8[arg0].unk3C; i++) {
        func_800BAF34(&D_800FA6D8[arg0].unk8[i]);
        if (!D_800FA6D8[arg0].unk4[i])
            continue;
        unk->unk0 = D_800FA6D8[arg0].unk4[i];
        unk->unk4 = D_800FA6D8[arg0].unk3E[i] | 0x180;
        unk->unk8 = 0;
        unk->unkA = D_801518E4[arg0].unk14[0];
        unk->unkC = 0x20;
        unk->unkE = D_801518E4[arg0].unk14[1];
        if (D_801518E4[arg0].D_80151909 & 4) {
            continue;
        }
        D_80163C74 = func_800D29D4(unk, D_801517C0->unk70, 12, D_80163C74);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB2A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB430);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB4F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB538);

void func_800BB67C(s32 arg0, Unk800BB67C* arg1) { arg1->unk30 = arg0; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BB684);

static void func_800BB75C(Unk800BB75C* arg0, MATRIX* m, s16* arg2, s16* arg3) {
    int flag;

    func_800D85B0(m, arg2, arg3, &D_800E7D10);
    RotMatrixYXZ(&arg0->sv, &arg0->m);
    TransMatrix(&arg0->m, &arg0->u.v);
    MulMatrix2(m, &arg0->m);
    SetRotMatrix(m);
    SetTransMatrix(m);
    RotTrans(&arg0->u.sub.sv2, (VECTOR*)&arg0->m.t, &flag);
    func_800BAFF8(&arg0->m, &D_800E7D20);
}

static void func_800BB89C(void);
static void func_800BB804(void) {
    if (!(D_8016376A & 0x20)) {
        func_80033E34(
            LBA_ENEMY6_FAN2, 0x1000, (u_long*)0x801D0000, func_800BB89C);
        func_800B7FB4();
        return;
    }
    D_80163B80 = 0;
    D_800FA6B8 = 0;
}

static void func_800BB864(void) {
    func_80033E34(LBA_ENEMY6_OVER2, 0x800, (u_long*)0x801D0000, func_800BB89C);
    func_800B7FB4();
}

static void func_800BB89C(void) {
    D_80163B80 = 0;
    D_800FA6B8 = 0;
    D_8009A000[0] = !(!(D_8016376A & 0x10) && !D_80083338) ? 0x10 : 0x14;
    D_8009A004[0] = 0x801D0000;
    func_8002DA7C();
}

void func_800BB90C(void) {
    D_8009A000[0] = 0xA0;
    D_8009A004[0] = 0x7F;
    func_8002DA7C();
}

static void func_800BB944(void) {
    func_800BB90C();
    D_8009A000[0] = 0xF1;
    func_8002DA7C();
}

// queue sound command 0xC1
void func_800BB978(void) {
    D_8009A000[0] = 0xC1;
    D_8009A004[0] = 0x12C;
    D_8009A008[0] = 0;
    func_8002DA7C();
}

// queue sound command 0x30, dispatched directly via func_8002DF88 (akao.c)
// rather than the D_8009A000 global queue used by the sibling functions below
void func_800BB9B8(s32 arg0) {
    s16* ptr;

    ptr = &D_800F4AD0;
    *ptr = 0x30;
    D_800F4AD4 = arg0 & 0xFFFF;
    D_800F4AD8 = arg0 & 0xFFFF;
    func_8002DF88(ptr);
}

// queue sound command 0x2B
void func_800BB9FC(s32 arg0) {
    s32 param;

    D_8009A000[0] = 0x2B;
    param = arg0 & 0xFFFF;
    D_8009A004[0] = 0x40;
    D_8009A008[0] = param;
    func_8002DA7C();
}

// queue sound command 0x20
static void func_800BBA40(s32 arg0) {
    s32 param;

    D_8009A000[0] = 0x20;
    param = arg0 & 0xFFFF;
    D_8009A004[0] = 0x40;
    D_8009A008[0] = param;
    func_8002DA7C();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBA84);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBB20);

static void func_800BBDF8(void) {
    if (D_800F8368 == 0) {
        D_800F4AF4 = D_80163C74;
        if ((u32)D_80163C74 > (u32)0x80184000) {
            PadStop();
            ResetGraph(1);
            StopCallback();
            SystemError('b', 0);
        }
    } else {
        D_800F4AF8 = D_80163C74;
        if ((u32)D_80163C74 > (u32)0x801A0000) {
            PadStop();
            ResetGraph(1);
            StopCallback();
            SystemError('b', 1);
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBEAC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BBF7C);

s32 func_800BC04C(void (*cb)(void));
INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC04C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC11C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC1E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC2F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC348);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC440);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC538);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC630);

void func_800BCA58(s32);
void func_800C1104();
static void func_800BC72C(void) {
    func_800C1104();
    func_800BCA58(3);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC754);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC81C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BC8B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BCA58);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BCB1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BE49C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BE69C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BE86C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BEA38);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFA98);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFB10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFB88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFDA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800BFF88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0088);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C018C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0254);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0314);

// magnitude of (arg0 - arg1) via GTE sqrt
static s16 func_800C03B8(s16 arg0, s16 arg1) {
    s32 delta;

    delta = arg0 - arg1;
    return SquareRoot0(delta * delta);
}

s32 func_800C03FC(s32 arg0, s32 arg1) { return arg0 < 0 ? -arg1 : arg1; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0410);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0480);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0630);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0900);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0970);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0B20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C0DD8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C1104);

// cosine-eased interpolation between arg0 and arg1
static s32 func_800C1304(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 val;
    s32 delta;

    delta = arg1 - arg0;
    val = (rcos((s16)(((arg3 << 0xB) / arg2) + 0x800)) + 0x1000) * delta;
    return arg0 + val / 0x2000;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C1394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C14C0);

static s32 func_800C169C(u8 arg0) {
    D_801518E4[arg0].D_80151909 |= 8;
    if (D_80151200[arg0].D_80151200 & 0x2000) {
        return 10;
    }
    if (D_80151200[arg0].D_80151200 & 0x4000) {
        return 5;
    }
    if (D_80151200[arg0].D_80151200 & 0x0008) {
        return 1;
    }
    if (D_80151200[arg0].D_80151200 & 0x800000) {
        return 3;
    }
    if (D_80151200[arg0].D_80151200 & 0x01000000) {
        return 6;
    }
    if (D_80151200[arg0].D_80151200 & 0x04000000) {
        return 8;
    }
    if (D_80151200[arg0].D_80151200 & 0x8000) {
        return 9;
    }
    if (D_80151200[arg0].D_80151200 & 0x400000) {
        return 7;
    }
    D_801518E4[arg0].D_80151909 &= ~8;
    return 0;
}

static void func_800C17A0(s32 arg0, s32 arg1) {
    switch (D_800EA19C[arg1][0]) {
    case 0:
        D_801518E4[arg0].unk14[0] = 0;
        break;
    case 1:
        D_801518E4[arg0].unk14[0] = 0x800;
        break;
    case 2:
        D_801518E4[arg0].unk14[0] = 0xC00;
        break;
    }
    D_801518E4[arg0].D_8015190C = D_800EA19C[arg1][1];
    D_801518E4[arg0].D_8015190D = D_800EA19C[arg1][2];
    D_801518E4[arg0].D_8015190E = D_800EA19C[arg1][3];
    D_801518E4[arg0].D_80151908 = 0;
}

static void func_800C5468(u8 arg0);
void func_800C5170(u8);

static void func_800C1908(u8 arg0) {
    s32 temp_a1;
    s16 var_a0;
    u8 temp_s0;

    temp_s0 = arg0;
    if (D_801518E4[temp_s0].D_80151922 & 0x20) {
        if (temp_s0 < 4) {
            D_800F9F28[temp_s0] = D_801636B8[temp_s0].D_801636C0;
        }
        func_800C5170(temp_s0);
        func_800C5468(temp_s0);
        func_800C17A0(temp_s0, func_800C169C(temp_s0));
        D_801518E4[temp_s0].D_80151922 &= 0xDF;
    }
    temp_a1 = arg0;
    if (D_80151200[temp_a1].D_80151235 == 0) {
        if (D_80151200[temp_a1].D_80151200 & 0x4000) {
            D_80151200[temp_a1].D_80151233 = 3;
            return;
        }
        D_80151200[temp_a1].D_80151233 = 0;
        if (D_80151200[temp_a1].D_80151200 & 0x100) {
            D_80151200[temp_a1].D_80151233 = 1;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x200) {
            D_80151200[temp_a1].D_80151233 = 2;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x400) {
            D_80151200[temp_a1].D_80151233 = 3;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x02000000) {
            D_80151200[temp_a1].D_80151233 = 3;
        }
        if (D_80151200[temp_a1].D_80151200 & 0x40) {
            if (D_801518E4[temp_a1].D_801518E6 == D_80163784[temp_a1]) {
                D_801518E4[temp_a1].unk160.vy += 0x100;
            }
        }
        var_a0 = arg0;
        if (D_80151200[var_a0].D_80151200 & 0x400000 &&
            D_801518E4[var_a0].D_801518E6 == D_80163784[var_a0]) {
            if (D_801518E4[var_a0].D_801518FC == 0) {
                D_801518E4[var_a0].unk160.vy = 0x800;
            } else {
                D_801518E4[var_a0].unk160.vy = 0;
            }
        }
        var_a0 = arg0;
        if (D_801518E4[var_a0].D_80151909 & 8) {
            if (D_801518E4[var_a0].D_80151908 < 0x10) {
                D_801518E4[var_a0].unk14[0] += 0x80;
            } else {
                D_801518E4[var_a0].unk14[0] -= 0x80;
            }
            D_801518E4[arg0].D_80151908--;
            D_801518E4[arg0].D_80151908 &= 0x1F;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C1D8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2000);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C20E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2150);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C223C);

// Initial (non-matching) decomp. Draws one sprite glyph and prepends
// {DR_MODE, SPRT} onto the ordering-table chain *tex points into (net
// effect: *tex -> mode -> sprt -> whatever *tex pointed to before this
// call), returning a pointer just past the SPRT primitive.
DR_MODE* func_800C2704(u_long** tex, s16 x, s16 y, s16 arg3, s32 arg4, s32 arg5,
                       s32 arg6, s32 arg7) {
    DR_MODE* mode = D_80163C74;
    SPRT* sprt = (SPRT*)((u8*)mode + 0xF0);

    SetDrawMode(mode, 1, 0, 0x1F, NULL);
    SetSprt(sprt);

    sprt->clut = GetClut(0x100, (arg7 & 1) ? 0x1E4 : 0x1E7);
    sprt->r0 = 0x80;
    sprt->g0 = 0x80;
    sprt->b0 = 0x80;
    sprt->u0 = (u8)arg3;
    sprt->v0 = (u8)arg4;
    sprt->w = (s16)arg5;
    sprt->h = (s16)arg6;
    sprt->x0 = x;
    sprt->y0 = y;

    sprt->tag = (sprt->tag & 0xFF000000) | ((u32)*tex & 0xFFFFFF);
    *tex = (u_long*)(((u32)*tex & 0xFF000000) | ((u32)sprt & 0xFFFFFF));
    mode->tag = (mode->tag & 0xFF000000) | ((u32)*tex & 0xFFFFFF);
    *tex = (u_long*)(((u32)*tex & 0xFF000000) | ((u32)mode & 0xFFFFFF));

    return (DR_MODE*)((u8*)sprt + 0x14);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2864);

// Forward declarations for helpers this file's still-INCLUDE_ASM functions
// don't otherwise expose a prototype for -- only needed for the initial
// (non-matching) decomps below, so exact arg types are a best guess.
extern u8 D_800EA230[];
extern s32 D_800F8158;
extern SVECTOR D_800E7D08;
extern s16 D_800F4B14[]; // digit-glyph texture-column lookup, see func_800C2C1C
void func_800C2C1C(s16 x, s16 y, s16 color, u8 digitCount, s32 value);
void func_800BAF34(BattleModelSub* model);
void func_800C2FD4(s32 battlerIdx, s32 soundId, u8 gate);
void func_800B5FE8(s16 battlerIdx);
void func_800C33F0(u8 slotIdx);
s32 func_800C2F20(s16 color, s16* table, s32 arg2);
void func_800B5AAC(s32 arg0, s16 arg1, s32 arg2);

// D_801621F0[60] pool job spawned by func_800CE970/CE7E0 (the damage-popup
// family): positions and draws the floating damage-number sprite each tick.
// unk10.ptr actually holds the target battler index (stored as a fake
// pointer -- see func_800CE970's construction). Initial decomp (m2c +
// manual cleanup of the array/field access) -- NOT byte-matched, offsets
// past D_80151200[].D_80151240/D_80151260/D_80151268 are a best guess.
//
// Initial decomp only, not byte-matched yet.
void func_800C2928(void) {
    Unk801621F0* slot = &D_801621F0[D_801590D4];

    switch (slot->D_801621F2) {
    case 0: {
        s32 targetIdx = (s32)slot->unk10.ptr;
        u8 farColorIntensity = D_801518E4[targetIdx].D_8015190F;

        slot->D_801621F2 = 1;
        slot->D_801621F4 = 0xB;
        slot->unkC = 0;

        *(s32*)0x1F800000 = farColorIntensity; // GTE far-color register
        func_800D3994(
            targetIdx, farColorIntensity, &D_80151200[targetIdx].D_80151268);

        // 3 raw halfword clears + a table-pointer store; exact sub-field
        // split within D_80151260/D_80151264/D_80151270 not resolved yet.
        *(s16*)((u8*)&D_80151200[targetIdx].D_80151264 + 0) = 0;
        *(s16*)((u8*)&D_80151200[targetIdx].D_80151260 + 2) = 0;
        D_80151200[targetIdx].D_80151260 = 0;
        D_80151200[targetIdx].D_80151270 = (s32)&D_800F8158;
        return;
    }
    case 1: {
        s32 targetIdx;
        s16 screenX, screenY;

        if (slot->D_801621F4 == 0) {
            slot->D_801621F0 = -1;
            return;
        }

        targetIdx = (s32)slot->unk10.ptr;
        RotMatrixYXZ((SVECTOR*)&D_80151200[targetIdx].D_80151260,
                     (MATRIX*)&D_80151200[targetIdx].D_80151240);
        func_800BAF34((BattleModelSub*)&D_80151200[targetIdx].D_80151240);
        *(s32*)0x1F800010 = RotTransPers(&D_800E7D08, (long*)0x1F800004,
                                         (long*)0x1F800008, (long*)0x1F80000C);

        screenX = *(s16*)0x1F800004;
        screenY = *(s16*)0x1F800006;
        slot->D_801621F6 = screenX;
        slot->unkA = screenY;
        slot->unk8 = screenY - D_800EA230[slot->unkC];

        if (slot->D_801621F6 < 0x18)
            slot->D_801621F6 = 0x18;
        if (slot->D_801621F6 >= 0x129)
            slot->D_801621F6 = 0x128;
        if (slot->unk8 < 0x10)
            slot->unk8 = 0x10;
        if (slot->unk8 >= 0x97)
            slot->unk8 = 0x96;

        func_800C2C1C(slot->D_801621F6, slot->unk8, slot->unkE,
                      D_800EA230[slot->unkC], slot->unk14);
        slot->unkC++;
        slot->D_801621F4--;
        return;
    }
    }
}

// D_801621F0[60] pool job's digit/icon drawing helper -- called by
// func_800C2928 (which computes screen position) to actually draw the
// floating damage-number sprite, or a special icon for a few sentinel
// "color" values instead of a real color (-1/-2/-3, meaning unresolved --
// candidates: miss/status icons). The color==-1/-2 branches genuinely only
// set up 4 of func_800C2704's 8 args in the retail asm (the other 4 are
// whatever's left in those registers) -- passed as 0 here instead, a
// deliberate simplification, not a faithful reproduction of that quirk.
// Initial decomp (m2c + manual cleanup) -- NOT byte-matched yet.
void func_800C2C1C(s16 x, s16 y, s16 color, u8 digitCount, s32 value) {
    Unk801621F0* slot = &D_801621F0[D_801590D4];
    s32 numGlyphs;
    s32 digitX;
    s32 i;

    // digitCount round-tripped through the GTE via func_800C2F20 -- ends up
    // as the actual glyph count used below; unclear why the GTE is
    // involved rather than using digitCount directly.
    *(s32*)0x1F800008 = func_800C2F20(color, D_800F4B14, color << 0x10) & 0xFF;
    numGlyphs = *(s32*)0x1F800008;

    if (color == -2) {
        D_80163C74 = func_800C2704(
            &D_801517C0->unk4080[1], x - 0x10, slot->unk8, 0x20, 0, 0, 0, 0);
        return;
    }
    if (color == -1) {
        D_80163C74 = func_800C2704(
            &D_801517C0->unk4080[1], x - 0xC, slot->unk8, 0x80, 0, 0, 0, 0);
        return;
    }
    if (color == -3) {
        D_80163C74 = func_800C2704(&D_801517C0->unk4080[1], x - 0x18,
                                   slot->unk8, 0x20, 0xEA, 0x20, 0xA, value);
        D_80163C74 = func_800C2704(&D_801517C0->unk4080[1], x + 8, slot->unk8,
                                   0x20, 0xF4, 0x12, 0xA, value);
        return;
    }
    if (color < -1) {
        return; // no other negative color does anything
    }

    // color >= 0: draw the numeric value, digit by digit, right to left
    if (value & 4) {
        // an extra glyph, meaning unresolved -- sign? critical-hit marker?
        D_80163C74 =
            func_800C2704(&D_801517C0->unk4080[1], x + numGlyphs * 4 + 1,
                          slot->unk8 + 6, 0x80, 0x93, 0xF, 5, value);
    }
    digitX = numGlyphs * 4 - 8;
    for (i = 0; i < numGlyphs; i++) {
        D_80163C74 =
            func_800C2704(&D_801517C0->unk4080[1], x + digitX, slot->unk8,
                          D_800F4B14[3 - i], 0x88, 8, 0xB, value);
        digitX -= 9;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C2F20);

// Queues an SFX (fixed id 0x2A) gated on `gate`, panned by the battler's
// position (D_801516FC). Matches the earlier live/static trace exactly.
// Initial decomp (m2c did almost all the work here) -- NOT byte-matched.
void func_800C2FD4(s32 battlerIdx, s32 soundId, u8 gate) {
    if (gate & 0xFF) {
        D_8009A000[0] = 0x2A;
        D_8009A008[0] = (s16)soundId;
        D_8009A004[0] = ((D_801516FC[battlerIdx & 0xFF][0] / 5) * 2) & 0x7E;
        func_8002DA7C();
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3068);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C328C);

// Shared per-tick "fade and eventually free" helper for the doDeathSequence
// family (and presumably its siblings) -- called every tick from the
// steady-state case. While delay counts down: ramps the blend-intensity
// field up (+0x80/tick) and fades the tint byte down (-0x10/tick, via
// signed-byte wraparound). Once delay hits 0: frees the D_80162978 slot,
// clears the "tint active" bit, sets the "settled" byte, and calls
// func_800B5AAC (a "restore normal appearance" refresh, unconfirmed).
// Initial decomp (m2c + manual cleanup) -- NOT byte-matched yet.
void func_800C33F0(u8 slotIdx) {
    BattleCallbackSlot* slot = &D_80162978[slotIdx];
    s16 targetIdx = slot->target_battler_idx;

    if (slot->delay == 0) {
        slot->state = -1;
        D_801518E4[targetIdx].D_80151909 &= 0x7F;
        D_80151200[targetIdx].D_8015120C &= 0xFFDF;
        D_801518E4[targetIdx].D_8015190A = 1;
        func_800B5AAC(targetIdx, targetIdx, (s32)slotIdx << 5);
        return;
    }

    D_801518E4[targetIdx].unk14[0] += 0x80;
    D_801518E4[targetIdx].D_8015190C +=
        0xF0; // signed-byte wraparound == -= 0x10
    slot->delay--;
}

// Effect type 0's dedicated per-frame job -- live-confirmed this session
// (via the Callback Breakpoint Inspector): fires when a battler's HP hits
// 0 via damage (func_800CE21C -> this, NOT a "Death" status spell/curse --
// that path never triggered this), and drives the ~14-tick fade-to-
// transparent/disappear sequence you see on a killed battler.
//
// Initial decomp (m2c + manual cleanup), renamed for readability while
// inspecting it -- NOT byte-matched yet (was func_800C3578).
void doDeathSequence(void) {
    BattleCallbackSlot* slot = &D_80162978[D_8015169C];

    switch (slot->tick_count) {
    case 0: {
        s16 targetIdx = slot->target_battler_idx;
        u8 presetByte = *(u8*)&slot->preset_idx; // low byte only

        slot->delay = 14;
        slot->tick_count++;
        func_800C2FD4(targetIdx & 0xFF, 0x16, presetByte);

        D_801518E4[targetIdx].D_8015190C = 0xF8; // SetFarColor R
        D_801518E4[targetIdx].D_8015190D = 0;    // SetFarColor G
        D_801518E4[targetIdx].D_8015190E = 0;    // SetFarColor B
        D_801518E4[targetIdx].unk14[0] = 0x800;

        func_800B5FE8(targetIdx);
    } // fallthrough
    case 1:
        func_800C33F0((u8)D_8015169C);
        return;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C36B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3950);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3AA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3CA8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3DE4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C3F44);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C40F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C428C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C44B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C45EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C4814);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C494C);

static void func_800C4B60(s16 arg0) {
    if (D_801621F0[arg0].D_801621F4 == 0) {
        D_801621F0[arg0].D_801621F0 = -1;
        return;
    }
    D_80163C74 = func_800C4DC8(0, D_801621F0[arg0].unkA, 320, 47, &D_800EA25C);
    D_80163C74 =
        func_800C4DC8(0, D_801621F0[arg0].unkA + 47, 320, 32, &D_800EA258);
    D_80163C74 = func_800C4DC8(0, D_801621F0[arg0].unk8, 320, 32, &D_800EA260);
    D_80163C74 =
        func_800C4DC8(0, D_801621F0[arg0].unk8 + 32, 320, 47, &D_800EA25C);
    D_801621F0[arg0].unk8 += 4;
    D_801621F0[arg0].unkA -= 4;
    D_801621F0[arg0].D_801621F4--;
}

static void func_800C4D10(void) {
    int arg0;

    arg0 = D_801590D4;
    switch (D_801621F0[arg0].D_801621F2) {
    case 0:
        D_801621F0[arg0].D_801621F4 = 21;
        D_801621F0[arg0].unk8 = 87;
        D_801621F0[arg0].unkA = 8;
        D_801621F0[arg0].D_801621F2++;
    case 1:
        func_800C4B60(arg0);
        break;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C4DC8);

static u_long* func_800C5040(u8 r, u8 g, u8 b, s32 tpage, u_long* ot);

u_long* func_800C4FC8(u8 r, u8 g, u8 b) {
    return func_800C5040(r, g, b, 1, (u_long*)&D_801517C0->unk4080[1]);
}

u_long* func_800C5004(u8 r, u8 g, u8 b) {
    return func_800C5040(r, g, b, 2, (u_long*)&D_801517C0->unk40EC);
}

static u_long* func_800C5040(u8 r, u8 g, u8 b, s32 tpage, u_long* ot) {
    DR_MODE* drMode;
    POLY_F4* poly;

    drMode = D_80163C74;
    SetDrawMode(drMode, 1, 0, (tpage & 3) << 5, NULL);
    poly = (POLY_F4*)(drMode + 24);
    SetPolyF4(poly);
    SetSemiTrans(poly, 1);
    poly->r0 = r;
    poly->g0 = g;
    poly->b0 = b;
    poly->x0 = 0;
    poly->y0 = 8;
    poly->x1 = 320;
    poly->y1 = 8;
    poly->x2 = 0;
    poly->y2 = 166;
    poly->x3 = 320;
    poly->y3 = 166;
    addPrim(ot, poly);
    addPrim(ot, drMode);
    return (u_long*)(poly + 1);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5170);

static void func_800C55B8(void);
static void func_800C5468(u8 arg0) {
    s32 var_v0_2;
    s32 var_v0;
    u16 temp_a1;

    var_v0 = arg0;
    if (D_80151200[var_v0].D_80151200 & 0x1000) {
        temp_a1 = D_80151200[var_v0].D_8015120C;
        if (!(temp_a1 & 0x80)) {
            D_80151200[var_v0].D_8015120C |= 0x80;
            var_v0_2 = func_800BC04C(func_800C55B8);
            D_801621F0[var_v0_2].D_801621F6 = arg0;
            D_801621F0[var_v0_2].D_801621F4 = 0x10;
            D_801621F0[var_v0_2].D_801621F2 = -0x80;
        }
    } else {
        temp_a1 = D_80151200[var_v0].D_8015120C;
        if (temp_a1 & 0x80) {
            D_80151200[var_v0].D_8015120C = temp_a1 & (~0x80);
            var_v0_2 = func_800BC04C(func_800C55B8);
            var_v0_2 = var_v0_2;
            D_801621F0[var_v0_2].D_801621F6 = arg0;
            D_801621F0[var_v0_2].D_801621F4 = 0x10;
            D_801621F0[var_v0_2].D_801621F2 = 0x80;
        }
    }
}

static void func_800C55B8(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801518E4[D_801621F0[D_801590D4].D_801621F6].D_801518EA +=
        D_801621F0[D_801590D4].D_801621F2;
    D_801621F0[D_801590D4].D_801621F4--;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5694);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C57B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5864);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C59B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5ADC);

// reset each slot's first field to -1 (empty)
static void func_800C5BEC(void) {
    s32 fill;
    s32 i;

    fill = -1;
    for (i = 0x17A; i >= 0; i -= 6) {
        *(s16*)&D_800F9DA8[i] = fill;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5C18);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5CC0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C5E94);

s32 func_800C60F4(void) { return Savemap.battle_msg_speed / 4 + 4; }

static void func_800C610C(void) {
    while (D_801518DC) {
        func_800B7FB4();
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C614C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C61C0);

// load an image into VRAM
static void func_800C627C(void) {
    s32 i;

    for (i = 0; i < 0xA; i++) {
        func_800C62F4(i & 0xFF);
    }
    D_800F4B24.x = 0;
    D_800F4B24.y = 0x1E0;
    D_800F4B24.w = 0x10;
    D_800F4B24.h = 0x1E;
    BATTLE_EnqueueLoadImage(&D_800F4B24, D_80158D0C);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C62F4);

void func_800C679C(void);

void func_800C64AC(void) { func_800BBEAC(func_800C679C); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C64D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C6628);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C679C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C6CB8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C70AC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C7220);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C7340);

static void func_800C74A4(void) {
    if (!(D_801518E4[3].D_80151909 & 2)) {
        func_800C7C4C(3, D_800F57D0->unk8, D_800F57D0 + 1, D_800F57D0);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C74E4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C76C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle1", func_800C7924);

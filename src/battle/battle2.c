//! PSYQ=3.3 CC1=2.6.3
#include "battle_private.h"

static void func_800D67BC(s32 arg0);
static void func_800D67E8(s32 arg0);
void func_800D6814(s32 arg0);

extern Yamada D_800EEBB8[]; // MAGIC/*.BIN overlay
extern s16 D_800EF5B0[];
extern s16 D_800EF63C[];
extern s16 D_800EF6A8[];
extern s16 D_800EF6D8[];
extern s16 D_800EF6FC[];
extern s16 D_800EF838[];
extern s16 D_800EF8D8[];

s32 func_800BBF7C(void (*f)(void));
s32 func_800BC04C(void (*f)());
void func_800C2928();
void func_800C328C();
void doDeathSequence(); // was func_800C3578 -- initial (non-matching) decomp,
                        // renamed for readability while inspecting it
void func_800C3950();
void func_800C3CA8();
void func_800C40F4();
void func_800C44B4();
void func_800C4814();
void func_800CFB14();
void func_800D1530();
s32 func_800D376C(BattleModelSub* arg0, s32 arg1, s16 nItems, u8* arg3);
void func_800D3AF0();
static void func_800D4D4C(s32 arg0, s32 arg1);

// MAGIC/ entrypoints
void func_801B037C(s16, u8);
void func_801B0000(s16, u8);
void func_801B0000_2(s16, u8);
void func_801B000C(s16, u8);
void func_801B0040(s16, u8);
void func_801B0054(s16, u8);
void func_801B0084(s16, u8);

static s32 func_800C7B60(s16 arg0, s16 nItems, u8* arg2) {
    BattleModelSub* var_a0;
    s32 var_a1;
    s32 temp_s0;
    s32 temp_v0;

    var_a0 = D_801518E4[arg0].D_80151A58;
    var_a1 = D_801518E4[arg0].D_80151958;
    D_801518E4[arg0].D_80151958 = func_800D376C(var_a0, var_a1, nItems, arg2);
    return D_801518E4[arg0].D_80151958 == 0;
}

static void func_800C7BE8(s16 arg0, s16 arg1, u8* arg2) {
    D_800FA6D8[arg0].unk0 =
        func_800D376C(D_800FA6D8[arg0].unk8, D_800FA6D8[arg0].unk0, arg1, arg2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800C7C4C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD400);

s16 func_800CD558(s16 arg0, u8* arg1) {
    u32 val;
    val = arg1[D_801518E4[arg0].D_80151920++];
    return (arg1[D_801518E4[arg0].D_80151920++] << 8) + val;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD5E4);

static void func_800CD798(u8 arg0) {
    D_801518E4[arg0].D_80151A4C.vx = D_80163C80[arg0].vx;
    D_801518E4[arg0].D_80151A4C.vy = D_80163C80[arg0].vy;
    D_801518E4[arg0].D_80151A4C.vz = D_80163C80[arg0].vz;
}

static void func_800CD82C(void) {
    D_801590DC = 1;
    D_801621F0[D_801590D4].D_801621F0 = -1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800CD860);

static void func_800CDD44(s16 arg0) {
    D_801518E4[arg0].D_8015191F = 1;
    D_801518E4[arg0].D_80151920 = 0;
    D_801518E4[arg0].D_80151921 = 0;
}

void func_800CDDA4(void) {
    if (D_80166F68) {
        D_800F9DA4 = D_800F9DA4 | 1;
    } else {
        D_800F9DA4 = D_800F9DA4 & ~1;
    }
}

static void func_800CDDE8(void) {
    func_800BBA84(D_801621F0[D_801590D4].D_801621F4, D_801590CC, 0);
    func_800D7178(D_801590CC, D_801621F0[D_801590D4].D_801621F2);
    D_801621F0[D_801590D4].D_801621F0 = -1;
}

static void func_800CDE78(void) {
    if (D_801516FC[D_801590CC][0] <
        D_801516FC[D_801621F0[D_801590D4].unk8][0]) {
        D_801621F0[D_801590D4].unk14 |= 0x100;
    }
    func_800D4D4C(D_801621F0[D_801590D4].unk8, D_801621F0[D_801590D4].unk14);
    D_801621F0[D_801590D4].D_801621F0 = -1;
}

void func_800CDF6C(s32 arg0, s16 arg1) {
    func_800BBA84(0x14, arg1, 0);
    func_800D58D0(arg0, 1, 1);
    func_800D4D4C(D_800FA6D4, 0x2C);
}

static void func_800CDFC4(void) {
    if (!D_801621F0[D_801590D4].D_801621F4) {
        D_80163C74 = (DR_MODE*)func_800C4FC8(0xFA, 0xFA, 0xFA);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

static void func_800CE058(s16 arg0) {
    Unk80151200* dst = &D_80151200[arg0];
    dst->D_8015123C = 0x1000;
    dst->D_8015123A = 0x1000;
    D_80151200[arg0].D_80151238 = 0x1000;
    D_80151200[arg0].D_8015120C |= 0x20;
}

static void func_800CE0C8(s16 arg0, u8 arg1, u8 arg2) {
    s32 ret;

    func_800CE058(arg0);
    switch (arg1) {
    case 0:
        ret = func_800BBEAC(doDeathSequence);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg2;
        break;
    case 8:
        ret = func_800BBEAC(func_800C4814);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg2;
        break;
    case 1:
    case 10:
        ret = func_800BBF7C(func_800C3950);
        D_801620AC[ret].D_801621B2 = arg2;
        D_801620AC[ret].D_801621B4 = arg0;
        D_801620AC[ret].D_801621B6 = 0xF8;
        return;
    case 2:
        ret = func_800BBEAC(func_800C3CA8);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg2;
        break;
    case 3:
        ret = func_800BBEAC(func_800C328C);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg2;
        break;
    case 17:
    case 18:
        ret = func_800BBEAC(func_800C40F4);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg2;
        break;
    case 7:
        ret = func_800BBEAC(func_800C44B4);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg2;
        break;
    }
}

static void func_800CE21C(s16 arg0, u8 arg1) {
    s32 ret;

    func_800CE058(arg0);
    switch (D_801636B8[arg0].D_801636BC) {
    case 0:
        ret = func_800BBEAC(doDeathSequence);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg1;
        break;
    case 8:
        ret = func_800BBEAC(func_800C4814);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg1;
        break;
    case 1:
    case 10:
        ret = func_800BBF7C(func_800C3950);
        D_801620AC[ret].D_801621B2 = arg1;
        D_801620AC[ret].D_801621B4 = arg0;
        D_801620AC[ret].D_801621B6 = 0xF8;
        break;
    case 2:
        ret = func_800BBEAC(func_800C3CA8);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg1;
        break;
    case 3:
        ret = func_800BBEAC(func_800C328C);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg1;
        break;
    case 17:
    case 18:
        ret = func_800BBEAC(func_800C40F4);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg1;
        break;
    case 7:
        ret = func_800BBEAC(func_800C44B4);
        D_80162978[ret].target_battler_idx = arg0;
        D_80162978[ret].preset_idx = arg1;
        break;
    }
}

static void func_800CE384(void) {
    u8* ptr;
    u8 do_work;
    u32 param;
    u32 param_hi;

    if (D_80151200[D_801621F0[D_801590D4].D_801621F6].D_80151234 !=
        D_801621F0[D_801590D4].D_801621F2) {
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    if (D_801621F0[D_801590D4].D_801621F4) {
        D_801621F0[D_801590D4].D_801621F4--;
        return;
    }
    do_work = 1;
    while (do_work) {
        ptr = D_801621F0[D_801590D4].unk10.ptr;
        switch (ptr[D_801621F0[D_801590D4].unk18++]) {
        case 0xFD:
            param = ptr[D_801621F0[D_801590D4].unk18++];
            param_hi = ptr[D_801621F0[D_801590D4].unk18++];
            param_hi <<= 8;
            param |= param_hi;
            D_801621F0[D_801590D4].D_801621F4 =
                ptr[D_801621F0[D_801590D4].unk18++];
            D_80151200[D_801621F0[D_801590D4].D_801621F6].D_8015122E = param;
            do_work = 0;
            break;
        case 0xFE:
            D_801621F0[D_801590D4].unk18 = 0;
            break;
        case 0xFF:
            D_801621F0[D_801590D4].D_801621F0 = -1;
            do_work = 0;
            break;
        default:
            do_work = 0;
            break;
        }
    }
}

static void func_800CE638(void) {
    switch (D_801621F0[D_801590D4].D_801621F2) {
    case 0:
        D_801621F0[D_801590D4].D_801621F4 = 3;
        D_801621F0[D_801590D4].D_801621F2++;
    case 1:
        if (D_801621F0[D_801590D4].D_801621F4 == 0) {
            D_801621F0[D_801590D4].D_801621F0 = -1;
            func_800A3534(
                D_801621F0[D_801590D4].unkA, D_801621F0[D_801590D4].unk8);
        }
        D_801621F0[D_801590D4].D_801621F4--;
        break;
    }
}

static void func_800CE75C(void) {
    if ((D_800FA69C >> D_80162978[D_8015169C].target_battler_idx) & 1) {
        func_800D67E8(D_80162978[D_8015169C].target_battler_idx);
    } else if ((D_80163608 >> D_80162978[D_8015169C].target_battler_idx) & 1) {
        func_800D67BC(D_80162978[D_8015169C].target_battler_idx);
    }
}

static void func_800CE7E0(void) {
    s32 dst;

    if (!D_80162978[D_8015169C].delay) {
        if (D_80162978[D_8015169C].preset_idx != -1) {
            func_800CE75C();
            dst = func_800BC04C(func_800C2928);
            D_801621F0[dst].unk14 = D_80162978[D_8015169C].unkA;
            D_801621F0[dst].unkE = D_80162978[D_8015169C].damage_dealt;
            D_801621F0[dst].unk10.ptr =
                (u8*)(u32)D_80162978[D_8015169C].target_battler_idx;
            dst = func_800BC04C(func_800CE638);
            D_801621F0[dst].unkA = D_80162978[D_8015169C].unk15;
            D_801621F0[dst].unk8 = D_80162978[D_8015169C].preset_idx;
        }
        D_80162978[D_8015169C].state = -1;
        return;
    } else {
        D_80162978[D_8015169C].delay--;
    }
}

void func_800CEB48(void);
void func_800CE970(void) {
    s32 dst;

    if (!D_80162978[D_8015169C].delay) {
        if (D_80162978[D_8015169C].unkA & 2) {
            D_80163C74 = (DR_MODE*)func_800C4FC8(0xFA, 0xFA, 0xFA);
        }
        if (D_80162978[D_8015169C].preset_idx != -1 &&
            D_80162978[D_8015169C].unk14 != 1) {
            func_800CE75C();
            dst = func_800BC04C(func_800C2928);
            D_801621F0[dst].unk14 = D_80162978[D_8015169C].unkA;
            D_801621F0[dst].unkE = D_80162978[D_8015169C].damage_dealt;
            D_801621F0[dst].unk10.ptr =
                (u8*)(u32)D_80162978[D_8015169C].target_battler_idx;
            dst = func_800BC04C(func_800CE638);
            D_801621F0[dst].unkA = D_80162978[D_8015169C].unk15;
            D_801621F0[dst].unk8 = D_80162978[D_8015169C].preset_idx;
        }
        func_800CEB48();
        return;
    } else {
        D_80162978[D_8015169C].delay--;
    }
}

// Forward declarations for globals this function touches that aren't
// declared elsewhere yet -- types are a best guess from usage, only needed
// for this initial (non-matching) decomp.
extern u8 D_800F8378;
extern s16 D_80161DE8; // per-battler flag bitmask, confirmed genuinely s16
                       // (was wrongly declared u8 -- caught while tracing
                       // the func_800C7C4C excerpt that shares this check)
extern u8 D_80162090;
extern u8 D_80163B38;
extern u8 D_800FA9C0;

// func_800CE970/CE7E0's (the damage-popup family's) finalize step -- called
// every tick once their `delay` reaches 0. Queues a sound/message cue via
// func_800BBA84, may spawn the on-screen popup job (func_800C2928, via a
// separate pool) the first time through, handles some scripted-event/
// model-animation special-casing (D_8016360C.setup.stageID etc., not
// resolved), and unconditionally frees the D_80162978 slot at the end --
// this is where the slot ACTUALLY gets freed, not inside func_800CE970
// itself.
//
// Initial decomp (m2c + manual cleanup of the array/field access) -- NOT
// byte-matched yet, and the scripted-event branch's exact meaning is
// unresolved.
void func_800CEB48(void) {
    BattleCallbackSlot* slot = &D_80162978[D_8015169C];
    s16 soundId, cueBattlerIdx;
    s16 attackerIdx = slot->unk8;
    s16 targetIdx = slot->target_battler_idx;

    if (slot->unkC != -1) {
        if (slot->damage_dealt == -1 && attackerIdx >= 4) {
            soundId = 5;
            cueBattlerIdx = targetIdx;
        } else {
            soundId = slot->unkC;
            cueBattlerIdx = targetIdx;
        }
        func_800BBA84(soundId, cueBattlerIdx, 0);
    }

    if (slot->unk10 != 0xFF) {
        s32 dst = func_800BC04C(func_800CDE78);
        D_801621F0[dst].unk8 = slot->target_battler_idx;
        *(s32*)0x1F800000 = dst;
        D_801621F0[dst].unk14 = slot->unk10;
    }

    if (attackerIdx != targetIdx) {
        if (D_801518E4[targetIdx].D_801518E8 == 0x33) {
            if (targetIdx >= 4) {
                s32 flag = (D_800F8378 == 0)
                               ? (((s16)D_80161DE8 >> targetIdx) & 1)
                               : (D_80151200[targetIdx].D_8015120C & 0x10);
                if (flag != 0) {
                    func_800CE21C(targetIdx, 1);
                }
            } else {
                D_801518E4[targetIdx].D_8015190A = 1;
            }
        } else {
            D_801518E4[targetIdx].D_8015190A = 0;
            if (D_8016360C.setup.stageID == 0x4E &&
                ((D_80163B38 == 0xF && D_801518E4[D_801590CC].D_80151906 == 6 &&
                  D_801518E4[D_801590CC].D_80151907 == 0x14) ||
                 D_800FA6D4 == 4)) {
                D_801518E4[slot->target_battler_idx].D_801518E6 = 1;
                D_801636B8[4].D_801636B9 = 5;
            } else {
                D_801518E4[targetIdx].D_801518E6 =
                    D_801518E4[targetIdx].D_801518E8;
            }
            D_80151200[targetIdx].D_8015120C |= 8;
            D_801518E4[targetIdx].D_80151922 |= 1;
            D_800FA9C0 = targetIdx;
            if (D_801518E4[D_801590CC].D_80151906 != 0x15 ||
                D_801518E4[D_801590CC].D_80151907 != 0xD) {
                if (!(D_801518E4[D_800FA6D4].D_8015190B & 0x40) &&
                    D_801031F0 == 0) {
                    s32 flags = D_80151200[D_800FA6D4].D_80151200;
                    if (!(flags & 0x4000) && !(flags & 0x400) &&
                        D_80162090 == 0) {
                        D_801518E4[D_800FA6D4].unk160.vy =
                            D_80151200[D_800FA6D4].D_8015122C;
                    }
                }
            }
        }
    } else if (((s16)D_80161DE8 >> attackerIdx) & 1) {
        if (attackerIdx >= 4) {
            D_801518E4[attackerIdx].D_801518E6 =
                D_801518E4[attackerIdx].D_801518E8;
            D_801518E4[targetIdx].D_80151922 |= 1;
            D_80151200[targetIdx].D_8015120C |= 8;
            D_800FA9C0 = targetIdx;
        }
    } else if (D_80163798[D_801590E0].unk1 == 3) {
        D_801518E4[attackerIdx].D_8015190A = 1;
    }

    D_80162978[D_8015169C].state = -1;
}

static void func_800CF2F0(void) {
    s16 index;

    if (D_801620AC[D_801590D0].D_801621B0 == 0) {
        D_801620AC[D_801590D0].D_801621AC = -1;
        return;
    }
    index = D_801620AC[D_801590D0].D_801621B4;
    D_801518E4[index].D_80151A4C.vy += D_801620AC[D_801590D0].D_801621B6;
    *(s32*)0x1F80000C = index;
    D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].D_801621B0 - 1;
}

static void func_800CF3CC(void) {
    s16 index;

    index = D_801620AC[D_801590D0].D_801621B4;
    *(s32*)0x1F80000C = index;
    if (D_801620AC[D_801590D0].D_801621B0 == 0) {
        D_801620AC[D_801590D0].D_801621AC = -1;
        return;
    }
    D_801518E4[index].unk160.vy += D_801620AC[D_801590D0].unkA;
    D_801620AC[D_801590D0].D_801621B0--;
}

static void func_800CF4A8(void) {
    s16 index;

    if (D_801620AC[D_801590D0].D_801621B0 == 0) {
        D_801620AC[D_801590D0].D_801621AC = -1;
        return;
    }
    index = D_801620AC[D_801590D0].D_801621B4;
    D_801518E4[index].D_80151A4C.vx += D_801620AC[D_801590D0].D_801621B6;
    D_801518E4[index].D_80151A4C.vz += D_801620AC[D_801590D0].unk8;
    *(s32*)0x1F80000C = index;
    D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].D_801621B0 - 1;
}

void func_800CF5BC(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX1 = D_801620AC[D_801590D0].D_801621B4;
    IDX2 = D_801620AC[D_801590D0].D_801621B6;
    if (D_801620AC[D_801590D0].D_801621AE == 0) {
        D_801620AC[D_801590D0].D_801621AE = 1;
        if (IDX1 >= 4) {
            if (D_801031F0 == 0) {
                D_801620AC[D_801590D0].D_801621B2 =
                    (MUL(D_801518E4[IDX2].D_80151A4C.vy,
                         D_801518E4[IDX2].D_801518EA) -
                     MUL(D_801518E4[IDX1].D_80151A4C.vy,
                         D_801518E4[IDX1].D_801518EA)) /
                    D_801620AC[D_801590D0].D_801621B0;
            } else {
                D_801620AC[D_801590D0].D_801621B2 = 0;
            }
        } else {
            D_801620AC[D_801590D0].D_801621B2 = 0;
        }
    }
    if (!D_801620AC[D_801590D0].unk14) {
        D_80166F58 = 0;
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621AC = -1;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vx += D_801620AC[D_801590D0].unk8;
        D_801518E4[IDX1].D_80151A4C.vz += D_801620AC[D_801590D0].unkA;
        D_801518E4[IDX1].D_80151A4C.vy += D_801620AC[D_801590D0].D_801621B2;
        D_801620AC[D_801590D0].D_801621B0--;
        return;
    }
    D_801620AC[D_801590D0].unk14--;
#undef IDX2
#undef IDX1
#undef MUL
}

void func_800CF8C0(s16 arg0, s16 arg1, u8 arg2) {
    u8 dst;

    dst = func_800BBF7C(func_800CFB14);
    D_801620AC[dst].D_801621B4 = arg0;
    D_801620AC[dst].D_801621B0 = arg1;
    D_801620AC[dst].D_801621B6 = D_800F99E8;
    D_801620AC[dst].unk14 = arg2;
    if (D_800F99E8 == arg0) {
        D_801620AC[dst].unk8 = 0;
        D_801620AC[dst].unkA = 0;
        D_801620AC[dst].D_801621B2 = 0;
        return;
    }
    D_801620AC[dst].unk8 =
        (D_80163C80[arg0].vx - D_801518E4[arg0].D_80151A4C.vx) / arg1;
    D_801620AC[dst].unkA =
        (D_80163C80[arg0].vz - D_801518E4[arg0].D_80151A4C.vz) / arg1;
    D_801620AC[dst].D_801621B2 =
        (D_80163C80[arg0].vy - D_801518E4[arg0].D_80151A4C.vy) / arg1;
}

void func_800CFB14(void) {
    s16 dst;

    dst = D_801620AC[D_801590D0].D_801621B4;
    *(s32*)0x1F80000C = dst;
    *(s32*)0x1F800008 = D_801620AC[D_801590D0].D_801621B6;
    if (D_801620AC[D_801590D0].unk14 == 0) {
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621AC = -1;
            return;
        }
        D_801518E4[dst].D_80151A4C.vx += D_801620AC[D_801590D0].unk8;
        D_801518E4[dst].D_80151A4C.vz += D_801620AC[D_801590D0].unkA;
        D_801518E4[dst].D_80151A4C.vy += D_801620AC[D_801590D0].D_801621B2;
        D_801620AC[D_801590D0].D_801621B0--;
        return;
    }
    D_801620AC[D_801590D0].unk14--;
}

void func_800CFCB0(void) {
    s32 temp_a3;
    s16 temp_a2;
    s32 temp_a1;

    if (D_801620AC[D_801590D0].D_801621B0 == 0) {
        D_801620AC[D_801590D0].D_801621AC = -1;
        return;
    }
    temp_a2 = D_801620AC[D_801590D0].D_801621B4;
    temp_a3 = D_801620AC[D_801590D0].D_801621B6;
    temp_a1 = D_801620AC[D_801590D0].unkC;
    *((s32*)0x1F80000C) = temp_a2;
    *((s32*)0x1F800008) = temp_a3;
    *((s32*)0x1F800010) = temp_a1;
    D_801518E4[temp_a2].D_80151A4C.vx += D_801620AC[D_801590D0].unk8;
    D_801518E4[temp_a2].D_80151A4C.vz += D_801620AC[D_801590D0].unkA;
    D_801518E4[temp_a2].D_80151A4C.vy +=
        D_800EEB28[temp_a1][D_801620AC[D_801590D0].unk14++];
    D_801620AC[D_801590D0].D_801621B0--;
}

void func_800CFE60(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX1 = D_801620AC[D_801590D0].D_801621B4;
    IDX2 = D_801620AC[D_801590D0].D_801621B6;
    switch (D_801620AC[D_801590D0].D_801621AE) {
    case 0:
        D_801620AC[D_801590D0].D_801621AE = 1;
        D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].unk15;
        break;
    case 1:
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621AE = 2;
            D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].unk16;
            D_801620AC[D_801590D0].D_801621B2 =
                MUL(D_801518E4[IDX1].D_80151A4C.vy -
                        D_801620AC[D_801590D0].unkC,
                    D_801518E4[IDX2].D_801518EA) /
                D_801620AC[D_801590D0].unk16;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vy += D_801620AC[D_801590D0].unk10;
        D_801620AC[D_801590D0].D_801621B0--;
        break;
    case 2:
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621AC = -1;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vx += D_801620AC[D_801590D0].unk8;
        D_801518E4[IDX1].D_80151A4C.vz += D_801620AC[D_801590D0].unkA;
        D_801518E4[IDX1].D_80151A4C.vy -= D_801620AC[D_801590D0].D_801621B2;
        D_801620AC[D_801590D0].D_801621B0--;
        break;
    }
#undef IDX2
#undef IDX1
#undef MUL
}

void func_800D01C0(void) {
#define MUL(a, b) (((a) * (b)) >> 12)
#define IDX1 *(s32*)0x1F80000C
#define IDX2 *(s32*)0x1F800008
    IDX2 = D_801620AC[D_801590D0].D_801621B6;
    IDX1 = D_801620AC[D_801590D0].D_801621B4;
    switch (D_801620AC[D_801590D0].D_801621AE) {
    case 0:
        D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].unk16;
        D_801620AC[D_801590D0].D_801621B2 =
            (D_801620AC[D_801590D0].unk10 - D_801518E4[IDX1].D_80151A4C.vy) /
            D_801620AC[D_801590D0].unk16;
        D_801620AC[D_801590D0].D_801621AE = 1;
        break;
    case 1:
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621B0 = D_801620AC[D_801590D0].unk15;
            D_801620AC[D_801590D0].D_801621B2 =
                MUL(D_801620AC[D_801590D0].unk10 - D_801620AC[D_801590D0].unkC,
                    D_801518E4[D_801620AC[D_801590D0].D_801621B6].D_801518EA) /
                D_801620AC[D_801590D0].unk15;
            D_801620AC[D_801590D0].D_801621AE = 2;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vx += D_801620AC[D_801590D0].unk8;
        D_801518E4[IDX1].D_80151A4C.vz += D_801620AC[D_801590D0].unkA;
        D_801518E4[IDX1].D_80151A4C.vy += D_801620AC[D_801590D0].D_801621B2;
        D_801620AC[D_801590D0].D_801621B0--;
        break;
    case 2:
        if (D_801620AC[D_801590D0].D_801621B0 == 0) {
            D_801620AC[D_801590D0].D_801621AC = -1;
            return;
        }
        D_801518E4[IDX1].D_80151A4C.vy -= D_801620AC[D_801590D0].D_801621B2;
        D_801620AC[D_801590D0].D_801621B0--;
        break;
    }
#undef IDX2
#undef IDX1
#undef MUL
}

void func_800D0578(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        func_800D4D4C(
            D_801621F0[D_801590D4].unk8, D_801621F0[D_801590D4].D_801621F6);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

void func_800D061C(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        func_800BBA84(D_801621F0[D_801590D4].D_801621F6, D_801590CC, 0);
        D_801621F0[D_801590D4].D_801621F0 = -1;
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

void func_800D06B8(void) {
    if (D_801621F0[D_801590D4].D_801621F4 == 0) {
        if (D_801518DC == 0) {
            func_800D0C80(D_801621F0[D_801590D4].D_801621F6);
            D_801621F0[D_801590D4].D_801621F0 = -1;
        }
    } else {
        D_801621F0[D_801590D4].D_801621F4--;
    }
}

void func_800D0760(void) {
    if (D_80162978[D_8015169C].preset_idx == 0) {
        if (D_80162978[D_8015169C].delay == 0) {
            D_80162978[D_8015169C].state = -1;
            return;
        }
        func_800DCF60(D_801518E4[D_801590CC].D_80151907,
                      D_80151200[D_801590CC].D_8015123E);
        D_80162978[D_8015169C].delay--;
        return;
    }
    D_80162978[D_8015169C].preset_idx--;
}

void func_800D088C(s32 loc, s32 len) {
    func_80033E34(loc, len, (u_long*)0x801B0000, NULL);
    func_800B7FB4();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D08B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D0958);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D09D0);

void func_800D0A44(void) {}

static void func_800D0AD4(void);
void func_800D0A4C(void) {
    s32 ret;
    s32 i;

    for (i = 0; i < 3; i++) {
        D_801518E4[i].D_80151909 |= 1;
    }
    func_801B0040(D_80151774, D_801590CC);
    ret = func_800BC04C(func_800D0AD4);
    *(s32*)0x1F800000 = ret;
    D_801621F0[ret].D_801621F4 = 2;
}

static void func_800D0AD4(void) {
    if (!D_801621F0[D_801590D4].D_801621F4) {
        D_801621F0[D_801590D4].D_801621F0 = -1;
        func_800BB978();
        return;
    }
    D_801621F0[D_801590D4].D_801621F4--;
}

void func_800D0B4C(u8 arg0) {
    D_800F8CF0 = 0;
    func_800D1530();
    switch (D_801518E4[arg0].D_80151907) {
    case 4:
        D_800EF9D8[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    case 7:
        func_801B037C(D_80151774, D_801590CC);
        break;
    case 8:
        D_800EFFE0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    }
}

void func_800D0C80(u8 arg0) {
    D_800F8CF0 = 0;
    func_800D1530();
    switch (D_801518E4[arg0].D_80151907) {
    case 2:
        if (D_801031F0 == 0) {
            if (D_801518E4[arg0].D_80151906 == 25) {
                D_801518E4[0].D_8015190A = 1;
                D_801518E4[1].D_8015190A = 1;
                D_801518E4[2].D_8015190A = 1;
            }
            D_800EFAF0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
            return;
        }
        switch (D_801518E4[arg0].D_80151906) {
        case 41:
            func_801B0000(D_80151774, D_801590CC);
            break;
        case 44:
            func_801B0000_2(D_80151774, D_801590CC);
            break;
        case 35:
            func_801B000C(D_80151774, D_801590CC);
            break;
        case 32:
            func_801B0054(D_80151774, D_801590CC);
            break;
        case 29:
            func_801B0084(D_80151774, D_801590CC);
            break;
        default:
            D_800EFAF0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
            break;
        }
        break;
    case 13:
        D_800EFBC8[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    case 20:
        if (D_801518E4[arg0].D_80151906 == 2) {
            if (D_801590CC == D_800FA9E8) {
                D_80163A98 = 0;
            } else {
                D_80163A98 = 1;
            }
        }
        *(s32*)0x1F800000 =
            D_800EFEA0[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        switch (D_801518E4[arg0].D_80151906) {
        case 0x2D:
        case 0x2E:
        case 0x2F:
        case 0x30:
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4F:
            *(s32*)0x1F800000 = 0;
            break;
        }
        func_800D08B8(arg0, *(s32*)0x1F800000);
        break;
    case 32:
        D_800EFC28[D_801518E4[arg0].D_80151906](D_80151774, D_801590CC);
        break;
    case 3:
        func_800C64AC();
        break;
    }
}

void func_800D1110(u8 arg0) {
    s32 lba;
    s32 var_a1;
    s32 id;

    switch (D_801518E4[arg0].D_80151907) {
    case 2:
        if (D_801031F0 == 0) {
            id = D_800EF63C[D_801518E4[arg0].D_80151906];
            func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        } else {
            switch (D_801518E4[arg0].D_80151906) {
            case 29:
                func_800D088C(D_800EEBB8[77].loc, D_800EEBB8[77].len);
                break;
            case 41:
                func_800D088C(D_800EEBB8[32].loc, D_800EEBB8[32].len);
                break;
            case 44:
                func_800D088C(D_800EEBB8[26].loc, D_800EEBB8[26].len);
                break;
            case 32:
                func_800D088C(D_800EEBB8[255].loc, D_800EEBB8[255].len);
                break;
            case 35:
                func_800D088C(D_800EEBB8[6].loc, D_800EEBB8[6].len);
                break;
            default:
                id = D_800EF63C[D_801518E4[arg0].D_80151906];
                lba = D_800EEBB8[id].loc;
                var_a1 = D_800EEBB8[id].len;
                func_800D088C(lba, var_a1);
                break;
            }
        }
        break;
    case 7:
        func_800D088C(D_800EEBB8[221].loc, D_800EEBB8[221].len);
        break;
    case 8:
        id = D_800EF8D8[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 13:
        id = D_800EF6A8[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 20:
        id = D_800EF838[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 4:
        id = D_800EF5B0[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 32:
        id = D_800EF6FC[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    case 3:
        id = D_800EF6D8[D_801518E4[arg0].D_80151906];
        func_800D088C(D_800EEBB8[id].loc, D_800EEBB8[id].len);
        break;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D1530);

void BATTLE_EnqueueLoadImage(RECT* rect, u_long* ptr) {
    D_800F01DC->method = QUEUE_LOAD_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->ptr = ptr;
    D_800F01DC++;
}

void BATTLE_EnqueueStoreImage(RECT* rect, u_long* ptr) {
    D_800F01DC->method = QUEUE_STORE_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->ptr = ptr;
    D_800F01DC++;
}

void BATTLE_EnqueueMoveImage(RECT* rect, s32 x, s32 y) {
    D_800F01DC->method = QUEUE_MOVE_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC->x = x;
    D_800F01DC->y = y;
    D_800F01DC++;
}

void BATTLE_EnqueueClearImage(RECT* rect) {
    D_800F01DC->method = QUEUE_CLEAR_IMAGE;
    D_800F01DC->rect = rect;
    D_800F01DC++;
}

void BATTLE_FlushImageQueue(void) {
    Unk800F01DC* item;

    for (item = D_800F4BAC; item < D_800F01DC; item++) {
        switch (item->method) {
        case QUEUE_LOAD_IMAGE:
            LoadImage(item->rect, item->ptr);
            break;
        case QUEUE_STORE_IMAGE:
            StoreImage(item->rect, item->ptr);
            break;
        case QUEUE_MOVE_IMAGE:
            MoveImage(item->rect, item->x, item->y);
            break;
        case QUEUE_CLEAR_IMAGE:
            ClearImage(item->rect, 0, 0, 0);
            break;
        }
    }
    D_800F01DC = D_800F4BAC;
}

void BATTLE_ResetImageQueue(void) { D_800F01DC = D_800F4BAC; }

void func_800D2710(u_long* addr, s16 x, s16 y) {
    TIM_IMAGE tim;

    OpenTIM(addr);
    ReadTIM(&tim);
    if (tim.crect && tim.caddr) {
        D_800F4B2C[D_800F01E0] = *tim.crect;
        D_800F4B2C[D_800F01E0].x += x & ~15;
        D_800F4B2C[D_800F01E0].y =
            y + D_800F4B2C[D_800F01E0].y; // requires GCC 2.6.3
        BATTLE_EnqueueLoadImage(&D_800F4B2C[D_800F01E0], tim.caddr);
        D_800F01E0 = (D_800F01E0 + 1) & 7;
    }
}

void func_800D2828(u_long* addr, s32 xy) {
    TIM_IMAGE tim;
    s32 temp_a1;
    s32 temp_a3;
    s32 temp_a2;

    OpenTIM(addr);
    ReadTIM(&tim);
    if (tim.prect && tim.paddr) {
        D_800F4B6C[D_800F01E4] = *tim.prect;
        temp_a1 = (tim.prect->y & 0x300) >> 4 | (tim.prect->x & 0x3FF) >> 6;
        temp_a2 = temp_a1 + xy;
        temp_a3 = (temp_a1 & 0x0F) * 0x40;
        D_800F4B6C[D_800F01E4].x =
            ((temp_a2 & 0x0F) * 0x40 + (D_800F4B6C[D_800F01E4].x - temp_a3)) &
            0x3FF;
        temp_a3 = (temp_a1 & 0x30) * 0x10;
        D_800F4B6C[D_800F01E4].y =
            ((temp_a2 & 0x30) * 0x10 + (D_800F4B6C[D_800F01E4].y - temp_a3)) &
            0x1FF;
        BATTLE_EnqueueLoadImage(&D_800F4B6C[D_800F01E4], tim.paddr);
        D_800F01E4 = (D_800F01E4 + 1) & 7;
    }
}

void func_800D2980(u_long* addr, s16 imgXY, s16 clutX, s16 clutY) {
    func_800D2710(addr, clutX, clutY);
    func_800D2828(addr, imgXY);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D29D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D32B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3354);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3418);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3474);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D34C8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3520);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3548);

s32 func_800D35D8(u8* arg0, s32* arg1, s32 arg2) {
    s32 bits;
    s32 i;

    bits = 0;
    for (i = 0; i < arg2; i++) {
        bits <<= 1;
        if ((arg0[*arg1 / 8] >> (7 - (*arg1 & 7))) & 1) {
            bits++;
        }
        *arg1 = *arg1 + 1;
    }
    bits <<= 32 - arg2;
    bits >>= 32 - arg2;
    return bits;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3658);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D376C);

void func_800D3994(s32 arg0, s32 arg1, void* arg2);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3994);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3A6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3AF0);

const s32 D_800A0D98[] = {0x00000000, 0x00000000, 0x00000000, 0x00000000,
                          0x00001000, 0x00000000, 0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3BF0);

void func_800D3D88(void) {
    Unk801621F0* temp_s0_2;
    Unk801621F0* temp_s1;
    s32 temp_s0;
    u16 temp_s2;

    temp_s1 = &D_801621F0[D_801590D4];
    temp_s0 = temp_s1->D_801621F0;
    temp_s2 = ((u8*)&D_801518E4[temp_s0].D_8015191A)[temp_s1->D_801621F2 & 1];
    temp_s0++; // !FAKE
    temp_s0--; // !FAKE
    if (temp_s2 != 0xFF) {
        temp_s0_2 = &D_801621F0[func_800BC04C(func_800D3BF0)];
        func_800D3994(temp_s0, temp_s2, &temp_s0_2->D_801621F4);
        temp_s0_2->D_801621F6 = 0;
        temp_s0_2->unkE = temp_s1->unkE;
        temp_s0_2->unk10.unk.unk0 = temp_s1->unk10.unk.unk0;
    }
    temp_s1->D_801621F2++;
    if (temp_s1->D_801621F2 == 4) {
        temp_s1->D_801621F0 = -1;
    }
}

void func_800D3E8C(s32 arg0) {
    Unk801621F0* temp_v0;

    temp_v0 = &D_801621F0[func_800BC04C(func_800D3D88)];
    temp_v0->D_801621F0 = arg0;
    temp_v0->unkE = *(s16*)& temp_v0->unk10 = D_801518E4[arg0].D_801518EA;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D3F0C);

void func_800D3F8C(void) {
    Unk801621F0* temp_s0;
    Unk801621F0* temp_s1;

    temp_s1 = &D_801621F0[D_801590D4];
    if (D_80062D98 == 0) {
        temp_s1->unkC--;
        if (temp_s1->unkC == -1) {
            temp_s0 = &D_801621F0[func_800BC04C(func_800D3AF0)];
            RotMatrixYXZ(&D_801518E4[temp_s1->unk10.unk.unk2].unk160,
                         (MATRIX*)0x1F800008);
            ApplyMatrixSV((MATRIX*)0x1F800008, (SVECTOR*)&temp_s1->D_801621F4,
                          (SVECTOR*)0x1F800000);
            temp_s0->D_801621F4 =
                D_801518E4[temp_s1->unk10.unk.unk2].D_80151A4C.vx +
                ((SVECTOR*)0x1F800000)->vx;
            temp_s0->D_801621F6 =
                D_801518E4[temp_s1->unk10.unk.unk2].D_80151A4C.vy +
                ((SVECTOR*)0x1F800000)->vy;
            temp_s0->unk8 = D_801518E4[temp_s1->unk10.unk.unk2].D_80151A4C.vz +
                            ((SVECTOR*)0x1F800000)->vz;
            temp_s0->unkE = temp_s1->unkE;
            temp_s0->unk10.unk.unk0 = temp_s1->unk10.unk.unk0;
            temp_s1->D_801621F0 = -1;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D415C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D41FC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4284);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4368);

static void func_800D4484(u_long* ot, u16 tpage) {
    DR_MODE* dr_mode;

    dr_mode = D_80163C74;
    SetDrawMode(dr_mode, 0, 1, tpage, NULL);
    AddPrim(ot, (void*)dr_mode);
    D_80163C74 = dr_mode + 1;
}

const s32 D_800A0DB8[] = {0x00000000, 0xFFFFF000, 0x00000000, 0x00000000};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D44E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D461C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4710);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D491C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4A64);

static void func_800D4D6C(s32 arg0, s32 arg1, s32 arg2);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4C08);

void func_800D4CBC(s32, s32, s32);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4CBC);

static void func_800D4D4C(s32 arg0, s32 arg1) {
    func_800D4CBC(arg0, arg1, 0x1000);
}

static void func_800D4D6C(s32 arg0, s32 arg1, s32 arg2) {
    func_800D4C08(arg0, arg1, 0x1000, arg2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4D90);

extern s32 D_800F10D8;
extern s32 D_800F4CEC[16];
extern s16 D_800F4D2C[16][10];

static s16* func_800D4FA8(s32 arg0) {
    s32 idx = D_800F10D8;
    s32 next = (idx + 1) & 0xF;

    D_800F4CEC[idx] = arg0;
    D_800F10D8 = next;
    return D_800F4D2C[idx];
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D4FF0);

void func_800D508C();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D508C);

// Alt view of a slot: bytes 4-7 as one s32 accumulator, bytes 12-15 as one
// s32 countdown (see BarrierData in magic/barrier.c for the same pattern).
typedef struct {
    s32 pad0;
    s32 accum;
    s32 pad8;
    s32 countdown;
} PoolAccum32;

extern PoolAccum32* D_800F10E0;
void func_800D508C(void);

// Zero the accumulator; step = 0x10000 (16.16 fixed-point 1.0) / arg0 per
// tick, so it reaches 1.0 after arg0 ticks.
void func_800D5138(s32 arg0) {
    if (D_800F10E0 == 0) {
        s32 tmp = func_800BBEAC(&func_800D508C);
        D_800F10E0 = (PoolAccum32*)&D_80162978[tmp];
    }
    D_800F10E0->accum = 0;
    D_800F10E0->countdown = 0x10000 / arg0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D51D4);

extern s32 D_800F10E4;
extern s16 D_800F5B74;

// Accumulates the step (delay:target_battler_idx as one s32) each tick;
// frees the slot once the countdown (unk8:unkA as one s32) hits 0.
// `*(s32*)&slot->field` (address-of the existing named half-field, widened
// in place) is deliberate, not `((PoolAccum32*)slot)->field` -- casting the
// whole pointer to a differently-shaped struct type changes how GCC 2.6.3
// re-derives the base address here and breaks byte-match; taking the
// address of an already-named field and just widening the access doesn't.
void func_800D5230(void) {
    BattleCallbackSlot* slot;
    s32 v0;
    s32 v1;

    slot = &D_80162978[D_8015169C];

    if (D_80062D98 == 0) {
        v0 = *(s32*)&slot->delay + *(s32*)&slot->target_battler_idx;
        *(s32*)&slot->delay = v0;
        D_800F5B74 = (s16)(v0 >> 0x10);
        v1 = *(s32*)&slot->unk8 - 1;
        *(s32*)&slot->unk8 = v1;
        if (v1 == 0) {
            D_800F10E4 = 0;
            slot->state = -1;
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D52A0);

// delay is a target bitmask and target_battler_idx a per-bit repeat count
// here (see BattleCallbackSlot's field comments) -- scans the bitmask from
// state, and for each set bit calls the func_800D5444-stored function pointer
// as fn(bitPos, preset_idx).
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5350);
void func_800D5350(void);

// unk8:unkA (0xC) is a single s32 here -- a real function pointer field,
// via an alt view of the slot, instead of an address-of-member cast.
typedef struct {
    u8 pad[0xC];
    void (*fn)(int, int);
} PoolFnPtr32;

void func_800D5444(s32 arg0, s32 arg1, s32 arg2, void (*arg3)(int, int)) {
    BattleCallbackSlot* temp_v0;

    temp_v0 = &D_80162978[func_800BBEAC(func_800D5350)];
    temp_v0->state = 0;
    temp_v0->delay = arg0;
    temp_v0->preset_idx = arg1;
    temp_v0->target_battler_idx = arg2;
    ((PoolFnPtr32*)temp_v0)->fn = arg3;
}

s32 func_800D54BC(s32 arg0) {
    s32 count;
    s32 i;

    count = 0;
    for (i = 0; i < 10; i++) {
        if ((arg0 >> i) & 1) {
            count++;
        }
    }
    return count;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D54EC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D55A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D55F4);

// Project a point through the current view matrix and convert its clamped
// on-screen X (0..319) into a 0..127 stereo pan value.
static s32 func_800D56A8(SVECTOR* sv) {
    s16 sxy[2];
    s32 p;
    s32 flag;

    SetRotMatrix(&D_800FA63C.m);
    SetTransMatrix(&D_800FA63C.m);
    RotTransPers(sv, (long*)sxy, (long*)&p, (long*)&flag);
    if (sxy[0] < 0) {
        sxy[0] = 0;
    } else if (sxy[0] >= 0x140) {
        sxy[0] = 0x13F;
    }
    return (sxy[0] * 128) / 320;
}

s32 func_800D574C(s32 arg0) {
    SVECTOR sv;

    func_800D54EC(arg0, &sv);
    return func_800D56A8(&sv);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5774);

// Initial (non-matching) decomp. The attack-windup timer: `tick_count`
// counts up (via the caller's own per-frame tick, not shown here) until it
// reaches `delay`; from then on, `preset_idx` is reused as a stagger
// interval -- only every `preset_idx`-th call past that point does
// anything else. When it does: `target_battler_idx` is reused as a
// bitmask of remaining targets, `state` as the scan cursor into it. Finds
// the lowest set bit at/after the cursor, writes that bit index into
// whatever func_800D4FA8(4) returns a pointer to, clears the bit, and
// frees the slot (`state = -1`) once the mask is empty.
extern s16* func_800D4FA8(s32 category);
void func_800D57C0(void) {
    BattleCallbackSlot* slot = &D_80162978[D_8015169C];
    s16 sinceDelay;
    s16 bit;

    if (D_80062D98 != 0) {
        return;
    }

    if (slot->tick_count >= slot->delay) {
        sinceDelay = slot->tick_count - slot->delay;
        if (sinceDelay % slot->preset_idx == 0) {
            bit = slot->state;
            while (!((slot->target_battler_idx >> bit) & 1)) {
                bit++;
            }
            slot->state = bit;
            *func_800D4FA8(4) = bit;
            slot->target_battler_idx &= ~(1 << bit);
            if (slot->target_battler_idx == 0) {
                slot->state = -1;
            }
        }
    }
    slot->tick_count++;
}

// Also called from func_800CDF6C(arg0, 1, 1).
void func_800D58D0(s16 arg0, s16 arg1, s16 arg2) {
    BattleCallbackSlot* temp_v0;

    temp_v0 = &D_80162978[func_800BBEAC(&func_800D57C0)];
    temp_v0->state = 0;
    temp_v0->target_battler_idx = arg0;
    temp_v0->preset_idx = arg2;
    temp_v0->delay = arg1;
}

void func_800D5938();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5938);

// delay is caller-supplied here, not always 0.
static void func_800D5A68(s16 arg0, s16 arg1) {
    BattleCallbackSlot* temp_v0;

    temp_v0 = &D_80162978[func_800BBEAC(func_800D5938)];
    temp_v0->state = 0;
    temp_v0->preset_idx = arg1;
    temp_v0->delay = arg0;
}

// Divide each byte lane of a packed color independently by a divisor,
// yielding a per-channel step (e.g. a color-fade increment).
static s32 func_800D5AC0(s32 arg0, s32 arg1) {
    return (((arg0 & 0xFF0000) / arg1) & 0xFF0000) |
           (((arg0 & 0xFF00) / arg1) & 0xFF00) |
           (((arg0 & 0xFF) / arg1) & 0xFF);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5B6C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D5D28);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D61AC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6260);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6394);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D650C);

extern u8 D_800F10EC[];
extern u8 D_800F11E8[];
extern u8 D_800F1304[];
u8* const D_800A0DC8[] = {D_800F10EC, D_800F11E8, D_800F1304};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6734);

void func_800D6734(s32, s32);
extern s32 D_800F14D4;

static void func_800D67BC(s32 arg0) {
    D_800F14D4 = 0x88;
    func_800D6734(arg0, 0);
}

static void func_800D67E8(s32 arg0) {
    D_800F14D4 = 0xA8;
    func_800D6734(arg0, 1);
}

void func_800D6814(s32 arg0) {
    D_800F14D4 = 0x88;
    func_800D6734(arg0, 2);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6840);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6998);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6ACC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6C20);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6D8C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D6F78);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D70C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7178);

void func_800D72B4();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D72B4);

static void func_800D7340(void) { func_800BBEAC(func_800D72B4); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7368);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D751C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D76B8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7724);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7888);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7A88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7B1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7BA4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7C2C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D7D3C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8304);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D83A4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D8468);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D84F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D85B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle2", func_800D87EC);

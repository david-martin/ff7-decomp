#include "battle_private.h"

const u8 D_800A0000 = 0;
const u8 D_800A0001 = 0;
const u16 D_800A0002 = 0;
const u8 D_800A0004[] = {
    0x14, 0x11, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
    0x21, 0x37, 0x00, 0x00, 0x22, 0x00, 0x06, 0x00, 0x26, 0x00, 0x12, 0x04,
    0x21, 0x0A, 0x00, 0x00, 0x21, 0x09, 0x00, 0x00, 0x28, 0xB1, 0x00, 0x20,
    0x2A, 0x11, 0x00, 0x00, 0x2C, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x24, 0x11, 0x04, 0x00, 0x00, 0x11, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00,
    0x1F, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x15, 0x11, 0x00, 0x00,
    0x16, 0x11, 0x00, 0x10, 0x1C, 0x11, 0x02, 0x00, 0x18, 0x11, 0x00, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x2E, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00};
// opcode-byte program for func_800A1798's dispatch loop: a 0x1F-delimited
// stream of per-command opcode sequences, sliced by D_800F38AC[cmdIndex]
// (see func_800A283C) into per-command runs; each byte indexes D_800E7B28
// (function-pointer table) for func_800A1798 to jalr through in order
const u8 D_800A0098[] = {
    0x1F, 0x0E, 0x09, 0x1F, 0x00, 0x0C, 0x09, 0x1F, 0x01, 0x0C, 0x09, 0x1F,
    0x02, 0x0D, 0x09, 0x1F, 0x1E, 0x09, 0x1F, 0x0A, 0x16, 0x09, 0x1F, 0x1D,
    0x09, 0x1F, 0x19, 0x09, 0x1F, 0x0E, 0x1C, 0x09, 0x1F, 0x0E, 0x1B, 0x09,
    0x1F, 0x1A, 0x09, 0x1F, 0x17, 0x1F, 0x03, 0x0C, 0x09, 0x1F, 0x1F, 0x1F,
    0x1F, 0x0E, 0x09, 0x1F, 0x04, 0x0B, 0x0F, 0x1F, 0x05, 0x1F, 0x06, 0x0C,
    0x09, 0x1F, 0x00, 0x0C, 0x09, 0x1F, 0x01, 0x0C, 0x09, 0x1F, 0x02, 0x0D,
    0x09, 0x1F, 0x0E, 0x09, 0x1F, 0x12, 0x0E, 0x09, 0x1F, 0x0E, 0x18, 0x09,
    0x1F, 0x10, 0x0E, 0x09, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x07, 0x0C, 0x09,
    0x1F, 0x08, 0x1F, 0x11, 0x1F, 0x13, 0x09, 0x1F, 0x14, 0x1F, 0x15, 0x0F,
    0x1F, 0x00, 0x00, 0x00};
const s32 D_800A0108 = 21;
const s32 D_800A010C[] = {2, 22, 3, 23, 4};

// entrypoint
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A1158);

// per-command opcode dispatcher: reads cmdIndex from the turn context
// (D_80063014->cmdIndex), looks up its opcode-sequence start via
// D_800F38AC[cmdIndex] into D_800A0098, then for each byte until the 0x1F
// delimiter, jalr's through D_800E7B28[opcode]. After each call, checks
// D_80062F14 -- if it goes >= 0 the whole sequence aborts immediately
// (handler requested a suspend, e.g. to wait on an animation), otherwise
// continues to the next opcode byte
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A1798);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A22C0);

void func_800A23BC(s32 arg0) {
    if (D_800E7A38 == arg0) {
        D_800E7A38 = -1;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A23E0);

void func_800A283C(void) {
    s32 next;
    s32* out;
    u32 i;
    u32 delim;

    i = 0;
    next = 0;
    delim = 0x1F;
    out = D_800F38AC;
    /* i is initialised separately and delim kept in a temp: both are
     * load-bearing for codegen */
    for (; i < 0x6D; i++) {
        if (i == next) {
            *out++ = i;
        }
        if (D_800A0098[i] == delim) {
            next = i + 1;
        }
    }
}

// Initial (non-matching) decomp, traced while hunting for func_800A1798's
// callees. "7777 HP" easter egg: if a party member's curHP is exactly 7777
// and this hasn't already been announced this battle, queues a message
// (func_800A3E98) and sets a per-party "already announced" flag bit
// (D_800F5BB8[i].unk29 bit 0x80).
extern u16 D_800F7DE2; // announcement counter this battle, capped at 0x40
void func_800A3E98(s32, s32, s32, s32, s32);
void func_800A2894(void) {
    s32 i;
    for (i = 0; i < 3; i++) {
        if (D_800F83E0[i].curHP == 0x1E61 && !(D_800F5BB8[i].unk29 & 0x80) &&
            D_800F7DE2 < 0x40) {
            D_800F7DE2++;
            D_800F5BB8[i].unk29 |= 0x80;
            func_800A3E98(i, 1, 1, 0, 0);
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2974);

void func_800A2B28(s32 arg0) {
    if (arg0 & 1) {
        D_80063014->cmdProperties |= 0x80;
    }
    if (arg0 & 2) {
        D_80063014->cmdProperties |= 0x40;
    }
    if (arg0 & 8) {
        D_80063014->cmdProperties |= 0x04;
    }
    if (arg0 & 0x10) {
        D_80063014->cmdProperties |= 0x800;
    }
    if (arg0 & 0xE0) {
        D_80063014->unkE8 = (arg0 >> 5) * 10;
    }
}

// Initial (non-matching) decomp. Looks up whether the current action's
// actionIndex exists as a byte in one of a few 8-byte-stride sub-ranges of
// actor arg0's per-effect table (D_8009D954[actor], 0x440-byte per-actor
// stride, see its declaration comment), selected by (D_80063014->cmdIndex
// - 2). If found, stores the matching row index into D_80063014->unk14.
// Which cmdIndex values map to which (count, tableOffset) sub-range is
// preserved structurally from the original jump table; this does NOT
// resolve what D_8009D954's rows or a match actually mean mechanically.
void func_800A2BF4(s32 arg0) {
    u8* actorTable = &D_8009D954[arg0 * 0x440];
    s32 count = 0;
    s32 tableOff = 0;
    s32 cmdSel = D_80063014->cmdIndex - 2;
    s32 i;

    if ((u32)cmdSel < 0x15) {
        switch (cmdSel) {
        case 0:
        case 18:
            count = 0x38;
            break;
        case 1:
        case 19:
            tableOff = 0x38;
            count = 0x10;
            break;
        case 10:
            tableOff = 0x48;
            count = 0x18;
            break;
        default:
            break; // no lookup for this cmdIndex
        }
    }
    if (count != 0) {
        s32 target = D_80063014->actionIndex;
        for (i = 0; i < count; i++) {
            if (actorTable[(tableOff + i) * 8] == (u8)target) {
                D_80063014->unk14 = tableOff + i;
                break;
            }
        }
    }
}

void func_800B10F0(s32, s32, s32, s32, s32, s32, s32);
void func_800A2CC4(s32 arg0) {
    func_800B10F0(D_80063014->actorId, arg0, D_80063014->cmdIndexCopy,
                  D_80063014->attackEffect, D_80063014->attackScenePos, 0, 0);
}

const u8 D_800A01A8[] = {0x05, 0x06, 0x07, 0x12, 0x0F, 0x00, 0x03, 0xA6};
u8 func_800A2D0C(void) {
    s32 temp_v1;

    if (D_80063014->targetIdx >= 3) {
        return D_800F83E0[D_80063014->targetIdx].unk11;
    }
    return D_800A01A8[D_80063014->unkCC];
}

void func_800A2D68(u8 arg0) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (D_80063014->followUpActions[i] == 0xFF) {
            D_80063014->followUpActions[i] = arg0;
            return;
        }
    }
}

// Enemy-only "scripted last action on death": if the dying target is an
// enemy (targetIdx>=4) and hasn't already run this once (guard bit 0x10 on
// D_800F5BB8[targetIdx].unk29), looks up that enemy-slot's raw enemy id
// (D_8016360C.formation[]) and, via that, a per-enemy "death action" id
// stashed at SceneEnemy.unk90[4] (0xFFFF sentinel = none configured -- most
// enemies leave this unset). If one is configured, queues it as a
// message/event (self-destruct, revenge attack, etc.) -- not Final Attack,
// not materia-driven at all.
extern void func_800A7254(s32, s32, s32, s32);
extern void func_800B0FFC(s32, s32, s32, s16*);
void func_800A2DB0(void) {
    s32 targetIdx = D_80063014->targetIdx;
    s32 enemyId;
    u16 deathActionId;

    if (targetIdx < 4) {
        return;
    }
    if (D_800F5BB8[targetIdx].unk29 & 0x10) {
        return;
    }

    enemyId = D_8016360C.formation[targetIdx - 4].enemyID;
    deathActionId = *(u16*)&D_800F5F44.enemy[enemyId].unk90[4];
    if (deathActionId == 0xFFFF) {
        return;
    }

    if (D_801636B8[targetIdx].D_801636BC < 0x11) {
        D_801636B8[targetIdx].D_801636BC = 7;
    }

    func_800A7254(0, D_80063014->actorId, 3, deathActionId);
    func_800B0FFC(D_80063014->actorId, 0x56, 1, (s16*)&deathActionId);

    D_800F5BB8[targetIdx].unk29 |= 0x10;
}

void func_800A2EFC(void) {
    D_800F3950 = D_800F3944;
    D_800F3954 = D_800F3948;
}

void func_800A2F24(void) {
    D_800F3944 = D_800F3950;
    D_800F3948 = D_800F3954;
}

Unk800A2F4C* func_800A2F4C(void) {
    Unk800A2F4C* unk = &D_80163798[D_800F3944];
    unk->unk3 = 0;
    unk->unk2 = 0;
    unk->unkA = D_800F3948;
    if (D_800F3944 < LEN(D_80163798)) {
        D_800F3944++;
    } else {
        func_800155A4(40);
    }
    return unk;
}

s8* func_800A2FD0();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A2FD0);

// Initial (non-matching) decomp. D_800FA9D0/D_800FA9D4 are two fields (byte
// "owner battler" + u16 "flags") of the same 0xC-stride record array,
// D_800F3948-many entries live. For each live record whose flags bit 0x4 is
// set, clears that bit on the PREVIOUS record with the same owner battler
// (tracked via a small per-battler "most recent record index" scratch
// array) before remembering this record as the new most-recent one. Net
// effect: only the newest flag-0x4 record per battler stays flagged. Why
// only 9 battler slots (not 10) is preserved as-is, not resolved.
typedef struct {
    s8 battler; // -1 = unused
    u8 pad[3];
    u16 flags;
    u8 pad2[6];
} Unk800FA9D0; // size 0xC
extern Unk800FA9D0 D_800FA9D0[];
void func_800A304C(void) {
    s32 mostRecent[9];
    s32 count = D_800F3948;
    s32 i;

    for (i = 0; i < 9; i++)
        mostRecent[i] = -1;
    if (count <= 0)
        return;

    for (i = 0; i < count; i++) {
        s32 battler = D_800FA9D0[i].battler;
        if (battler == -1)
            continue;
        if (!(D_800FA9D0[i].flags & 4))
            continue;
        if (mostRecent[battler] != -1) {
            D_800FA9D0[mostRecent[battler]].flags &= ~4;
        }
        mostRecent[battler] = i;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A311C);

void func_800A317C(void) {
    s8* ret = func_800A2FD0();
    *ret = -1;
}

void func_800A31A0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    Unk800A2F4C* unk = func_800A2F4C();
    unk->unk0 = arg0;
    unk->unk1 = arg1;
    unk->unk5 = arg2;
    unk->unk6 = arg3;
    unk->unk8 = -1;
    unk->unkA = -1;
}

static void func_800A3208(s8 arg0, s8 arg1) {
    if (D_800F3944 != 0) {
        Unk800A2F4C* ptr = &D_80163798[D_800F3944 - 1];
        ptr->unk3 = arg0;
        ptr->unk2 = arg1;
    }
}

static void func_800A3240(void) {
    if (D_800F3944 != 0) {
        D_80163798[D_800F3944 - 1].unk8 = -1;
    }
}

void func_800A3278(void) {
    D_800F3944 = 0;
    D_800F3948 = 0;
    D_80163798[0].unk0 = -1;
}

void func_800A329C(void) {
    if (D_800F3944) {
        D_800F3944--;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A32C0);

void func_800A3354();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3354);

void func_800A345C(void) {
    if (D_800F3944) {
        func_800A3354();
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3488);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A34CC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3534);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A35F8);

s32 func_800A37F8(s32 arg0) {
    if (arg0 >= 0) {
        D_800F39E0 = arg0;
        D_800F39E4 = 0;
        return 0;
    }
    return D_800F39E0;
}

s32 func_800A3828(void) {
    s32 ret = 0;
    if (D_800F39E0 == 3) {
        D_800F39E4 += D_800F5F44.D_800F7DA6;
    }
    if (D_800F5F44.D_800F7DAA == 2) {
        switch (D_800F3896) {
        case 0:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 24:
        case 26:
        case 27:
            ret = 1;
            break;
        }
    }
    ret |= func_800A4A80();
    ret |= (D_800F83D0 & 3) ? 1 : 0;
    if (D_800F39E4 > 0x4000) {
        ret = 1;
    }
    return ret;
}

// insert a QueuedAction directly into the 64-slot priority table -- the
// same insertion primitive func_800A3ED0 uses to drain the ring buffer, but
// also called directly by the enemy-AI VM's func_800B2B5C, bypassing the
// ring (func_800A4350) entirely
void func_800A3D4C(QueuedAction*);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3D4C);

void func_800A3E98(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    QueuedAction sp;

    sp.actorId = arg0;
    sp.priority = arg1;
    sp.cmdIndex = arg2;
    sp.attackIndex = arg3;
    sp.targetMask = arg4;
    func_800A3D4C(&sp);
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A3ED0);

void func_800A4350(s16 actorId, s16 cmdIndex, s16 attackIndex, u16 targetMask) {
    QueuedAction* entry;

    if (D_800F39D8 == ((D_800F39DC + 1) & 0xF)) {
        return;
    }

    entry = &D_800F3958[D_800F39DC];
    entry->priority = (cmdIndex == CMD_LIMIT) ? 5 : 6;
    entry->actorId = actorId;
    entry->cmdIndex = cmdIndex;
    entry->attackIndex = attackIndex;
    entry->targetMask = targetMask;

    // The three inventory-consuming commands all get this extra call --
    // compiles to retail's exact branch shape only as a switch (GCC's
    // binary-search lowering for these 3 sparse case values: pivot on
    // CMD_THROW, then a cmdIndex<9 range split between CMD_ITEM and
    // CMD_W_ITEM), not as a flat "||" chain.
    switch (cmdIndex) {
    case CMD_THROW:
    case CMD_ITEM:
    case CMD_W_ITEM:
        func_800A5660(actorId, attackIndex);
        break;
    }

    func_800A4D88(func_800A44D8(actorId));
    D_800F5F44.D_800F7DAC &= ~(1 << actorId);
    D_800F5F44.D_800F7DC2 |= 1 << actorId;
    D_800F39DC = (D_800F39DC + 1) & 0xF;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4480);

// Manipulate redirect: if arg0 (an enemy id) is currently manipulated
// (D_800F7DCA bit), return the party slot whose D_800F5E60[].unk6 is
// tracking it in place of arg0; otherwise arg0 passes through unchanged.
s32 func_800A44D8(s32 arg0) {
    s32 i;

    if (arg0 < 4) {
        goto end;
    }
    if (!((D_800F5F44.D_800F7DCA >> arg0) & 1)) {
        goto end;
    }
    for (i = 0; i < LEN(D_800F5E60); i++) {
        if (D_800F5E60[i].unk6 == arg0) {
            arg0 = i;
            goto end;
        }
    }
end:
    return arg0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4540);

void func_800A4844(s32 arg0) {
    s32 var_v0 = arg0 ? 3 : 1;
    D_800F39EC = var_v0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4860);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4954);

s32 func_800A4A80(void) {
    s32 ret;

    ret = 1;
    if (D_80163C7C > 3 && D_80163C7C < 6 && !(D_800F9DA4 & 1)) {
        if (D_800FAFDC) {
            ret = 1;
        } else {
            ret = 0;
        }
    }
    return ret;
}

void func_800A4ACC(s16 arg0, u16 arg1) { func_8001726C(arg0, arg1); }

// opcode 0x14 handler (D_800E7B28[0x14]): spins on func_800B6D6C() until
// status bit D_800F9DA4 & 2 clears. Not itself a damage dealer -- injecting
// cmdIndex 0x23 (single-opcode sequence: just this one) produced ~3.1%
// max-HP damage, but func_800B6D6C (still nonmatching, battle1 overlay) is
// just a generic drainer for the D_80163798 event queue (status-icon
// show/hide, sound cues, a scale-factor tween via func_800C5694 -- see its
// own comment in battle1.c), gated one-per-frame on D_800F7DE4 which
// func_800B7FDC sets. So this opcode is "wait for already-queued visual
// effects to finish", not the source of the damage -- whatever actually
// applies the HP change before this opcode runs is the real damage
// source, still untraced
void func_800B6D6C();
void func_800A4AF4(void) {
    while (D_800F9DA4 & 2) {
        func_800B6D6C();
    }
}

u8 func_800A4B3C(s32 index, s32 arg1) {
    if (arg1 != -1) {
        D_801518E4[index].D_8015190F = arg1;
    }
    return D_801518E4[index].D_8015190F;
}

void func_800A4B9C(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4BA4);

u8 func_800A4CA8(s32 arg0) { return D_800F39F0[arg0][0]; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4CC8);

void func_800A4D2C(s32 arg0) {
    u32 i;

    if (D_800F83C6) {
        return;
    }
    for (i = 0; i < LEN(D_800E7A48); i++) {
        if (D_800E7A48[i] == arg0) {
            return;
        }
        if (D_800E7A48[i] == 0xFF) {
            D_800E7A48[i] = arg0;
            return;
        }
    }
}

void func_800A4D88(s32 arg0) {
    u32 i;

    for (i = 0; i < LEN(D_800E7A48); i++) {
        if (D_800E7A48[i] == arg0) {
            for (; i < LEN(D_800E7A48) - 1; i++) {
                D_800E7A48[i] = D_800E7A48[i + 1];
                if (D_800E7A48[i] == 0xFF) {
                    break;
                }
            }
            return;
        }
    }
}

s16 func_800A4E00(void) {
    s32 arg;
    s32 result;

    result = -1;
    arg = D_800E7A48[0] & 0xFF;
    if (arg != 0xFF) {
        arg = -1;
        result = func_800A4CC8(D_800E7A48[0], arg);
    }
    return result;
}

void func_800A4E40(void) {
    u8 temp_s0;

    temp_s0 = D_800E7A48[0];
    if (temp_s0 != 0xFF) {
        func_800A4D88(temp_s0);
        func_800A4D2C(temp_s0);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4E80);

void func_800A4F14(s32 arg0) {
    s32 temp_v0;

    temp_v0 = arg0 * 0x44;
    *(u16*)((u8*)&D_800F5BB8[0].unk8 + temp_v0) &= 0xFFFE;
    *(u8*)((u8*)&D_800F5BB8[0].unkE + temp_v0) |= 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A4F60);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A50E0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5250);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A555C);

typedef struct {
    s16 a;
    s16 b;
} Unk800F3A40;

extern Unk800F3A40 D_800F3A40[];

void func_800A55BC(void) {
    s32 i;

    for (i = 0; i < 16; i++) {
        D_800F3A40[i].a = -1;
        D_800F3A40[i].b = -1;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A55F4);

void func_800A5660(s32, s16);
;
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5660);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A56B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5750);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5990);

const u8 D_800A0240[] = {
    0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80,
    0x54, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80,
    0xA8, 0x54, 0x0A, 0x80, 0x94, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80,
    0xA8, 0x54, 0x0A, 0x80, 0xA8, 0x54, 0x0A, 0x80, 0x14, 0x54, 0x0A, 0x80,
    0x34, 0x54, 0x0A, 0x80, 0x74, 0x54, 0x0A, 0x80};
const u8 D_800A0278[] = {
    0x05, 0x06, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x00, 0x5C, 0x5A,
    0x0A, 0x80, 0x88, 0x5A, 0x0A, 0x80, 0xA8, 0x5A, 0x0A, 0x80};
u8 func_800A5A5C(void) { return D_800A0278[func_80014BA8(7)]; }

s32 func_800A5A88(void) { return func_80014BA8(54); }

s32 func_800A5AA8(void) { return func_80014BA8(16) + 56; }

const u8 D_800A028C[] = {0x02, 0xFF, 0x01, 0x86};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5AC8);

// Initial (non-matching) decomp, best-effort. Counter-attack/reaction-
// ability scanner, called once per candidate battler from func_800A1798
// (arg0 = battlerIdx, arg1 = "triggerKind": 0 vs nonzero select different
// row-matching and message args below). Skips if arg1!=0 and the battler
// has a status in mask 0x82804C44 (a small status set, not individually
// confirmed); skips non-party battlers (idx >= 3); skips if
// D_800F83E0[idx].unk4 bit 0x10 is set (a per-battler flag, unconfirmed
// role). Then scans 8 reaction-config rows (3 bytes each: {triggerId,
// param, chance/counter}, D_8009D870[battlerIdx], same 0x440 per-battler
// stride as D_8009D954) for a row whose triggerId matches one of
// {arg1*3+1, +2, +3}; on a match, runs an RNG-vs-chance check
// (func_80014BA8) and, if it passes, calls func_800A5AC8 then queues a
// reaction message via func_800A3E98.
//
// NOT fully resolved, preserved as raw arithmetic rather than guessed:
// `savedStatus` is only meaningfully computed when arg1 != 0 (the asm reads
// whatever register state was left otherwise); its later use, added to
// battlerIdx and multiplied by 8 as a byte offset from
// D_800F83E0[0].unk1A, was not pinned down to a named field.
extern u8 D_8009D870[]; // per-battler reaction-config table, 0x440 stride,
                        // 8 rows of 3 bytes each: {triggerId, param, chance}
extern s32 func_800A5AC8(s32, s32, s32, void*);
void func_800A5BC8(s32 arg0, s32 arg1) {
    s32 savedStatus = 0;
    s32 fp = arg1 * 3 + 1;
    u8* row = &D_8009D870[arg0 * 0x440];
    s32 j;

    if (arg1 != 0) {
        savedStatus = D_800F83E0[arg0].status;
        if (savedStatus & 0x82804C44)
            return;
    }
    if (arg0 >= 3)
        return;
    if (D_800F83E0[arg0].unk4 & 0x10)
        return;

    for (j = 0; j < 8; j++, row += 3) {
        s32 s1;
        for (s1 = 0; s1 < 3; s1++) {
            s32 chance = row[2];
            s32 rng;

            if (row[0] != fp + s1 || chance == 0)
                continue;
            if (arg1 == 0) {
                row[2] = chance - 1;
                chance = 0x64;
            }
            rng = func_80014BA8(0x64);
            if (rng >= chance)
                continue;

            {
                s32 spBuf[2];
                s32 flags = func_800A5AC8(arg0, s1, row[1], spBuf);
                s32 arg1For3E98;
                if (flags & 2) {
                    arg1For3E98 = *(s16*)((u8*)D_800F83E0 + 0x1A +
                                          (savedStatus + arg0) * 8);
                } else {
                    arg1For3E98 = 1 << arg0;
                }
                // Tail also branches on the outer `arg1` (0/1/2) to pick a
                // slightly different set of func_800A3E98 args and whether
                // D_800F5BB8[arg0].unk29 gets a bit set -- not fully
                // resolved, this is the arg1==2 shape (the most common one
                // observed in func_800A1798's call sites).
                func_800A3E98(arg0, arg1For3E98, spBuf[0], spBuf[1], 0);
            }
        }
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5E0C);

s32 func_800A5EB0(s32, s16*);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5EB0);

s8* func_800A5F90(s32 arg0) { return &D_800F3A80[D_800F4280[arg0]]; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A5FB0);

// Initial (non-matching) decomp, best-effort. Called (up to 3x) from
// func_800A1798 per some triggering event. arg0 = eventKind (a party-slot
// battler index when < 4, else an enemy/formation-slot index offset by 4),
// arg1 = a bit position, arg2 = passed through to the message-id lookup.
//
// First: ORs bit arg1 into a shared per-battle flags halfword
// (D_800F5F44.D_800F7DBC); for eventKind < 4 also resolves a message id via
// func_800A5FB0, using a small indirection through
// D_801636B8[eventKind].D_801636B8 / D_800E7A58 when eventKind < 3; for
// eventKind >= 4, resolves it via D_800F5F44.script (the enemy scene's
// scripted-event byte program) instead, indexed through a
// D_8016360C-relative lookup whose exact role isn't pinned down.
//
// If a message id came back (nonzero): snapshots
// D_800F83E0[i].unk10/+0x13/unk11 for all 10 combatants, runs
// func_800B2A2C/func_800B1D48, then diffs the CURRENT values against the
// snapshot and queues an announcement (func_800A31A0 / func_800A34CC) for
// any combatant whose +0x13 byte or unk11 changed. Reads as a generic
// "detect and report a state change" utility, not an HP/damage function
// itself -- no D_800F83E0 curHP/status write anywhere in this function.
extern u8 D_80082884[];
extern s32 func_800A5FB0(u8*, s32, s32);
extern void func_800B2A2C(s32, s32);
extern void func_800B1D48(s32, s32, s32);
extern void func_800A34CC(s32, s32);
void func_800A6000(s32 arg0, s32 arg1, s32 arg2) {
    s32 msgId;
    s32 idOrLookup = -1;

    D_800F5F44.D_800F7DBC |= 1 << arg1;

    if (arg0 >= 4) {
        s32 idx = *(s16*)((u8*)&D_8016360C + 0x4C + (arg0 - 4) * 0x10);
        msgId = func_800A5FB0(D_800F5F44.script, idx, arg1);
    } else {
        if (arg0 < 3) {
            idOrLookup = D_801636B8[arg0].D_801636B8;
            if (idOrLookup != -1 && D_800E7A58[idOrLookup] != -1) {
                idOrLookup = D_800E7A58[idOrLookup];
            }
        }
        msgId = func_800A5FB0(D_80082884, idOrLookup, arg1);
    }

    if (msgId != 0) {
        u8 snap13[10], snap11[10];
        s32 i;

        for (i = 0; i < 10; i++) {
            snap13[i] = *((u8*)&D_800F83E0[i] + 0x13);
            snap11[i] = D_800F83E0[i].unk11;
        }
        func_800B2A2C(arg0, arg2);
        func_800B1D48(arg0, msgId, idOrLookup);
        for (i = 0; i < 10; i++) {
            if (snap13[i] != *((u8*)&D_800F83E0[i] + 0x13)) {
                func_800A31A0(i, 4, 0, 0x10);
            }
            if (snap11[i] != D_800F83E0[i].unk11) {
                func_800A34CC(i, 0);
            }
        }
    }
}

// Drains up to 8 pending queued battle messages/events: for each set bit
// in the shared D_800F5F44.D_800F7DBC bitmask, clears it, resolves a
// message via the same two-level func_800A5FB0 offset-table resolver used
// elsewhere, and runs it through the message/event VM (func_800B1D48) if a
// message id came back. Generic pump, not tied to which actor triggered it.
void func_800A61D4(void) {
    s32 i;

    func_800B2A2C(-1, 0);
    for (i = 0; i < 8; i++) {
        if (D_800F5F44.D_800F7DBC & (1 << i)) {
            s32 msgId;

            D_800F5F44.D_800F7DBC &= ~(1 << i);
            msgId = func_800A5FB0((u8*)&D_800F5F44._5, D_800F83CC & 3, i);
            if (msgId != 0) {
                func_800B1D48(3, msgId, -1);
            }
        }
    }
}

// Runs once per "battler subject was affected by an event from source"
// occurrence -- called from func_800A1798's hit-resolution tail
// (source, subject, 0) and from func_800AFECC's Death-bit block (i, i, 1).
// arg2 appears to mean "force through / this is a death event": it
// bypasses the D_800F5F44+0xC57 change-detection gate below and suppresses
// the trailing func_800A3488 call.
extern void func_800A3488(s32);
extern u16 D_800F83B2;
extern void func_800B2B5C(s16, s16, s16);
void func_800A6278(s32 arg0, s32 arg1, s32 arg2) {
    s32 queuedSomething = 0;

    if (arg1 >= 4) {
        if (!(D_800F5BB8[arg1].unk29 & 0x20)) {
            D_800F5BB8[arg1].unk29 |= 0x20;
            if (arg0 < 3) {
                D_800F5E60[arg0].unk7++;
            }
        }
    }

    if (!(D_800F83E0[arg1].unk4 & 0x2000)) {
        u8 beforeSnapshot = *((u8*)&D_800F5F44 + 0xC57);

        D_800F83E0[arg1].unk4 |= 0x2000;

        if (arg0 < 4) {
            func_800A5BC8(arg1, 0);
        }

        if (arg0 == arg1) {
            *((s16*)&D_800F83E0[arg1].unk18 + 1) = 0;
        } else {
            *((s16*)&D_800F83E0[arg1].unk18 + 1) = 1 << arg0;
        }
        func_800A6000(arg0, 3, 0);

        if (beforeSnapshot != *((u8*)&D_800F5F44 + 0xC57) || arg2 != 0) {
            if (!(D_800F83E0[arg1].unk4 & 0x1000)) {
                D_800F83B2 = 1 << arg1;
                func_800B2B5C(arg1, 0x25, 0);
            }
            queuedSomething = 1;
        }
    }

    if (D_800F83E0[arg1].unk4 & 0x1000) {
        queuedSomething = 1;
    }

    if (queuedSomething && arg2 == 0) {
        func_800A3488(arg1);
    }
}

void func_800A64A0(s32 arg0, s8 arg1) { D_800E7A58[arg0] = arg1; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A64B4);

void func_800A653C(s32 arg0) {
    s32 index = func_800A44D8(arg0);
    D_800F5BB8[index].unk4 = 0;
    D_800F5F44.D_800F7DC2 &= ~(1 << index);
}

void func_800A6590(s32 arg0) { func_800A4D88(arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A65B0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A661C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A66A4);

void func_800A6720(s32 arg0, s16 arg1) { func_800A5660(10, arg1); }

void func_800A6748(s32 arg0) {
    func_800A653C(arg0);
    func_800A4D88(arg0);
    D_800F5F44.D_800F7DAC &= ~(1 << arg0);
}

void func_800A6798(s32 arg0, s32 arg1) { func_800A37F8(arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A67B8);

void func_800A6834(s32 arg0) { D_800F5F44.D_800F7DC4 &= ~(1 << arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6858);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A68FC);

void func_800A6A3C(s32 arg0, s32 arg1) { D_800F5BB8[arg0].unkE |= arg1; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6A70);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6AC4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6B1C);

void func_800A6B88(s32 arg0, s32 arg1) {
    s32 i;

    for (i = 0; i < 0x140; i++) {
        if (D_801671B8[i].id == arg1) {
            if (!(D_801671B8[i].unk4 & 9)) {
                func_800A64B4(i);
            }
            return;
        }
    }
}

void func_800A6BFC(void) {}

extern s32 func_800151F4(s32);

void func_800A6C04(s32 arg0) {
    s16 sp10;

    sp10 = (s16)D_801636B8[arg0].D_801636B8;
    D_800F5F44.D_800F7DBE = func_800A5EB0(func_800151F4(0x26), &sp10) + 0x100;
    D_800F5F44.D_800F7DC0 = 0xF;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6C5C);

extern const u8 D_800A04D0[];

void func_800A6CC0(s32 arg0, s32 arg1) {
    u32 mask = ~(1 << D_800A04D0[arg1]);
    D_800F83E0[arg0].status &= mask;
}

void func_800A6D10(s32 arg0) { func_800A3E98(arg0, 3, 2, 54, 0); }

void func_800A6D3C(s32 arg0) {
    s32 temp_v1;

    temp_v1 = D_800F83E0[arg0].status;
    if (temp_v1 & 0x2000) {
        D_800F83E0[arg0].status = (temp_v1 & ~0x2000) | 0x4000;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6D88);

void func_800A6DFC(void) {}

void func_800A6E04(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6E0C);

void func_800A7254(s32, s32, s32, s32);
void func_800A6E6C(s32 arg0, s32 arg1) { func_800A7254(0, arg0, 13, arg1); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A6E9C);

void func_800B0FFC(s32, s32, s32, s16*);
void func_800A7034(s32 arg0, s16 arg1) {
    s16 out = arg1;
    func_800B0FFC(arg0, 0x53, 1, &out);
}

void func_800A7060(s32 arg0, s32 arg1) { func_800A7254(0, arg0, 12, arg1); }

void func_800A7090(s32 arg0) { D_800F5BB8[arg0].unk29 |= 0x40; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A70C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7130);

void func_800A71E0(void) {}

s32 func_800A71E8(s32 arg0) { return (arg0 + 1) & 0x7F; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A71F4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7254);

// Initial (non-matching) decomp. This function has 4 distinct pieces, only
// the 2nd of which (the ATB scan) was the reason for decompiling it;
// confidence noted per section.
extern u16 D_80062D78;
extern u16 D_800F7DE8;
extern u16 D_800F7DD0;
extern u16 D_8016375A;
extern u16 D_80163768;
extern u16
    D_8009D864[3][0x220]; // per-party ATB-gauge display snapshot, stride 0x440
extern void func_800A72C8(s32);
extern void func_800B0170(void);
extern void func_800B0234(void);
extern void func_800A4E80(s32);
extern void func_800A5750(void);
void func_800A38FC(void) {
    s32 i;

    func_800A72C8(0);

    // Section 1 (moderate confidence on the "why", high on structure): looks
    // like the ATB-pause-after-an-action timer the community wiki describes
    // ("time pauses briefly after another action in battle is queued or a
    // party member reaches full ATB, with the pause duration depending on
    // the battle speed setting"). D_800F5F44.D_800F7DC8 (values seen: 1, 3)
    // reads like a battle-speed/ATB-mode setting; D_800F7DD0 a speed
    // threshold. Not confirmed field-by-field.
    D_800F7DE8 |= 1;
    if (!func_800A3828()) {
        D_800F7DE8 &= ~1;
        if (D_800F5F44.D_800F7DB2 != 0 &&
            D_800F5F44.D_800F7DB2 == D_800F5F44.D_800F7DB6) {
            s16 sinceDelay, remainder;
            if ((D_80062D78 & 0xC) == 0xC) {
                D_800F5F44.D_800F7DAE++;
            }
            sinceDelay = D_800F5F44.D_800F7DB0 + 1;
            remainder = D_800F5F44.D_800F7DB4 + D_800F5F44.D_800F7DA6;
            D_800F5F44.D_800F7DB0 = sinceDelay;
            D_800F5F44.D_800F7DB4 = remainder;
            if ((u16)remainder >= 0x2001) {
                D_800F5F44.D_800F7DB4 = remainder - 0x2000;
                if ((u16)sinceDelay >= D_800F5F44.D_800F7DAE * 2) {
                    if (D_800F7DD0 < 0x4000 || D_800F5F44.D_800F7DC8 == 1 ||
                        D_800F5F44.D_800F7DC8 == 3) {
                        D_800F5F44.D_800F7DB2--;
                    }
                }
                D_800F5F44.D_800F7DB0 = 0;
                D_800F5F44.D_800F7DAE = 0;
            }
        }
    }
    func_800B0170();

    // Section 2 (high confidence, verified against the raw asm): the ATB
    // gauge scan. Loops all 10 combatants in D_800F5BB8[]; on a per-battler
    // "slot active" gate (D_8016375A/D_80163768-derived bitmask), fills the
    // ATB accumulator (unk4) by the fill rate (unk2, x4 if unk8 bit 0 is
    // set), frozen to 0 if a per-battler bit in the (out-of-struct-bounds,
    // separately-aliased) flags halfword at D_800F5BB8+0x2212 is set.
    // Clamps at 0xFFFF = ready.
    for (i = 0; i < 10; i++) {
        u16 slotActiveMask = (D_8016375A | 0x3F0) & (D_80163768 | 0x7);
        if ((slotActiveMask >> i) & 1) {
            s32 gauge = D_800F5BB8[i].unk4;
            if (gauge != 0xFFFF) {
                s32 rate = D_800F5BB8[i].unk2;
                u16 freezeMask = *(u16*)((u8*)D_800F5BB8 + 0x2212);
                if (*(s16*)&D_800F5BB8[i].unk8 & 1)
                    rate <<= 2;
                if ((freezeMask >> i) & 1)
                    rate = 0;
                gauge += rate;
                if (gauge > 0xFFFF)
                    gauge = 0xFFFF;
                D_800F5BB8[i].unk4 = gauge;
                if (gauge == 0xFFFF) {
                    s16 cmdIndex;
                    if (i >= 3 || (D_800F83E0[i].unk4 & 0x10)) {
                        cmdIndex = -1;
                    } else {
                        u16 chosenMask = *(u16*)((u8*)D_800F5BB8 + 0x220C);
                        if ((chosenMask >> i) & 1) {
                            cmdIndex = func_800A4CA8(i);
                        } else {
                            u16* aiQueuedMask =
                                (u16*)((u8*)D_800F5BB8 + 0x21F4);
                            func_800A4D2C(i);
                            *aiQueuedMask |= 1 << i;
                            goto skipQueue;
                        }
                    }
                    func_800A4350(i, cmdIndex, 0, 0);
                skipQueue:
                    *(s16*)&D_800F5BB8[i].unk8 &= ~1;
                    D_800F39E0 |= 2;
                }
            }
        }
    }

    // Section 3 (high confidence this is the Limit-gauge fill: it directly
    // reads/writes D_800F5E60[i].limitBar/limitBarUI, both already-named
    // fields; the exact tail branch that fires a message when the gauge
    // saturates is transcribed structurally, not fully narrated). Party
    // only (i<3), gated on D_800F83D0 bit 0x8.
    func_800B0234();
    for (i = 0; i < 3; i++) {
        if (D_800F83D0 & 8) {
            s32 gauge =
                *(s16*)((u8*)&D_8009D84C[i].unk18 + 2); // upper half of unk18
            s32 rate = D_800F5E60[i].limitBarUI;
            s32 divisor = *(u16*)((u8*)&D_800F83D0 + 4);
            s32 increment = (rate * 255) / divisor;
            if (increment >= 0x100)
                increment = 0xFF;
            if (gauge < increment) {
                s32 clampedTo = 0;
                gauge += 4;
                if (increment < gauge) {
                    gauge = increment;
                    clampedTo = 0xFF;
                }
                if (gauge == clampedTo) {
                    func_800A4E80(i);
                    func_800A7254(1, i, 1, 0);
                    gauge = increment << 8;
                } else {
                    gauge = increment << 8;
                }
                *(s16*)((u8*)&D_8009D84C[i].unk18 + 2) = gauge;
            }
        }
    }

    // Section 4 (moderate confidence): snapshots each party member's ATB
    // gauge (D_800F5BB8[i].unk4) into a display-facing mirror array.
    for (i = 0; i < 3; i++) {
        D_8009D864[i][0] = D_800F5BB8[i].unk4;
    }
    func_800A5750();
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A72C8);

void func_800A73C0(void) {
    D_80063014->actionIndexCopy = D_80063014->actionIndex;
}

void func_800A73D8(void) {
    D_80063014->actionIndexCopy = D_80063014->actionIndex + 56;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A73F8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7458);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A7560);

void func_800A76AC(void) {
    D_80063014->actionIndexCopy = D_80063014->actionIndex + 72;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A76CC);

void func_800A7784(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A778C);

const s16 D_800A0290[] = {0, 56, 72, 96, 256};
const s32 D_800A029C[] = {
    0x140D0302, 0x3D3CFFFF, 0x41403F3E, 0xFFFFFF42, 0xFFFFFFFF,
    0x43424140, 0x47464544, 0xFF444843, 0xFFFFFFFF};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A784C);

void func_800A7940(void) {
    D_80063014->inflictStatusMask = 0x400000;
    D_80063014->unkE4 = 0x59;
}

void func_800B1060(s32);
void func_800A795C(void) { func_800B1060(D_80063014->actionIndex); }

void func_800AF9C8();
void func_800A7988(void) { func_800AF9C8(); }

void func_800A79A8(void) {
    D_80063014->targetMask = 0;
    D_80063014->allowedTargetsMask = 1 << D_80063014->actorId;
}

// Kernel AttackData loader -- resolves whichever ability record applies to
// the current action (D_80063014->cmdIndexCopy/actionIndexCopy select
// between: the current scene's own attacks[] table for the internal
// by-raw-id command 0x20; the global kernel D_800708C4 table for
// actionIndexCopy<0x60; a small D_800A0240 animation-override lookup for
// actionIndexCopy in [0x60,0x80); or D_8009D8F8, a per-party-member
// Enemy-Skill/Blue-Magic table, for actionIndexCopy>=0x80 -- NOT fully
// resolved, preserved as raw arithmetic rather than guessed) and copies
// its fields onto the current action context, finishing with
// func_800A8D88(record.additionalEffects, record.effectsModifier) --
// the kernel spell/item "secondary status effect chance" system (already
// confirmed unrelated to any materia-driven mechanic).
extern AttackEntry D_800708C4[];
extern s32 func_80015AFC(s32, s32);
extern void func_800A8DCC(s32, s32);
extern void func_800A8D60(s32);
extern void func_800A8E54(s32);
static void func_800A8D88(s32, s32);
static s32 func_800B11B4(s32 arg0);
void func_800A79CC(void) {
    AttackEntry* record;
    s32 forceAnim36 = 0;

    if (D_80063014->cmdIndexCopy == 0x20) {
        s32 idx;

        D_80063014->mpCost = -1;
        idx = func_800B11B4(D_80063014->actionIndexCopy);
        record = &D_800F5F44.attacks[idx];
        D_80063014->attackScenePos = idx;
    } else if (D_80063014->actionIndexCopy < 0x80) {
        s32 idx = D_80063014->actionIndexCopy;

        record = &D_800708C4[idx];
        if (idx >= 0x60) {
            u8 anim = D_800A0240[idx];

            if (anim != 0xFF) {
                D_80063014->cmdAnimation = anim;
            }
            D_80063014->unk14 = -1;
        }
    } else {
        // Enemy-Skill/Blue-Magic resolution (actionIndexCopy>=0x80, party
        // only): NOT fully resolved, preserved as raw arithmetic rather
        // than guessed. Scans up to 3 rows of D_8009D8F8[actorId] (a
        // per-party-member table, same 0x440 stride as D_8009D84C) for a
        // row whose stored skill id matches (actionIndexCopy-0x80); on a
        // match, searches up to 12 entries via func_80015AFC to find which
        // "slot" holds that skill id and derives cmdAnimation from it.
        record = NULL;

        if (D_80063014->actorId < 3) {
            s8 learnedCount = D_801636B8[D_80063014->actorId].D_801636B8;
            u8* rows = &D_8009D8F8[D_80063014->actorId * 0x440];
            s32 rowStride = 8;
            s32 i;

            for (i = 0; i < 3; i++, rowStride += 0x1C) {
                u8 rowSkill = rows[i];

                if (rowSkill + 0x80 == D_80063014->actionIndexCopy) {
                    s32 slot = 0;
                    s32 j;

                    for (j = 0; j < 12; j++) {
                        s32 v = func_80015AFC(learnedCount, j);

                        if (v == 0x7F) {
                            break;
                        }
                        if (v == rowSkill) {
                            record = (AttackEntry*)(rows + rowStride);
                            D_80063014->unk14 = slot + 0x3C;
                            goto resolved;
                        }
                        slot++;
                    }
                }
            }
        }
    resolved:;
    }

    if (record == NULL) {
        func_800155A4(0x20);
        return;
    }

    if (D_80063014->cmdIndex == 2 && D_80063014->actionIndexCopy == 0x36) {
        // hardcoded special-case for one specific spell id (0x36) --
        // unconfirmed which spell, preserved as raw arithmetic
        forceAnim36 = 1;
    }

    if (D_80063014->unk14 != -1 && !(D_80063014->cmdProperties & 0x400000)) {
        // per-actor "linked ability charge" row (D_8009D954[actorId],
        // stride 8, indexed by unk14) -- NOT fully resolved, preserved as
        // raw arithmetic rather than guessed
        u8* row = (u8*)&D_8009D954[D_80063014->actorId * 0x440] +
                  D_80063014->unk14 * 8;

        D_80063014->mpCost = row[1];
        if (row[4] != 0 && row[3] != 0) {
            row[4]--;
            func_800A8E54(row[3]);
            func_800A7254(2, D_80063014->actorId, 0x15, 6);
        } else if (D_80063014->cmdIndexCopy == 3) {
            if (row[2] == 0xFF) {
                /* no charge tracking for this row; nothing to do */
            } else if (row[2] == 0) {
                D_80063014->unkDC = 0x79;
            } else {
                row[2]--;
                func_800A7254(2, D_80063014->actorId, 0x15, 4);
            }
        } else if (D_80063014->cmdProperties & 0x200) {
            if (row[2] == 0xFF) {
                /* nothing to do */
            } else if (row[2] == 0) {
                if (record->targetFlags & 8) {
                    D_80063014->cmdProperties |= 0x100000;
                }
            } else {
                row[2]--;
                func_800A7254(2, D_80063014->actorId, 0x15, 2);
            }
        }

        if (*(u8*)((u8*)&D_800F5F44 + 0x9E8) < 5 &&
            !(D_80063014->cmdProperties & 0x400000)) {
            D_80063014->unkF0 = row[7];
        }
    }

    if (forceAnim36) {
        D_80063014->cmdAnimation = 0x34;
    } else if (D_80063014->actorId < 4) {
        // Party-only "match this ability against a linked enemy's attack
        // list, for a camera-id override" search -- NOT fully resolved,
        // preserved as raw arithmetic rather than guessed (indexes
        // D_800F5F44.enemy[unk8], stride 0xB8, walking 16 halfword-sized
        // entries starting at local offset 0x48).
        u8* enemyBase = (u8*)&D_800F5F44 + D_80063014->unk8 * 0xB8;
        s32 i;
        s32 found = 0;

        // On a match retail resolves a camera-id override (`fp`) from the
        // matched enemy-table entry and feeds it into the unk60/unk64
        // default-fill below instead of the record's own camera ids --
        // NOT reproduced here (defaults below always take the record's
        // own values); only the "must find a match or trap" gate matters
        // for control flow, so that's what's preserved.
        for (i = 0; i < 16; i++) {
            u16 v = *(u16*)(enemyBase + 0x48 + i * 2);

            if (D_80063014->actionIndexCopy == v) {
                found = 1;
                break;
            }
        }
        if (!found) {
            func_800155A4(0x20);
            return;
        }
    }

    if (*(u8*)((u8*)&D_800F5F44 + 0x9E8) == 3 ||
        (D_80063014->cmdProperties & 0x400000)) {
        D_80063014->mpCost = 0;
    }

    D_80063014->unk3C = record->accuracyRate;
    D_80063014->unk40 = record->damageCalcID;
    D_80063014->actionElement =
        (record->elements == 0xFFFF) ? 0 : record->elements;
    D_80063014->actionPower = record->strength;
    D_80063014->unkCC = record->impactAnimID;
    func_800A8D60(record->targetFlags);

    // unk60/unk64 camera-id default-fill: for CMD_LIMIT (cmdIndexCopy==
    // 0x14) retail instead substitutes a Limit-Break-variant-selected
    // value (D_800F83D0 bit 0x8 picks record's own byte 8 or byte 0xA) --
    // not reproduced, the common (non-Limit) default is below.
    D_80063014->unk60 = record->cameraSingleID;
    D_80063014->unk64 = record->cameraMultiID;

    D_80063014->unk54 = record->impactSfxID;
    D_80063014->unk58 = record->impactSfxID;
    if (!(D_80063014->unk6C & 4)) {
        D_80063014->unk5C = record->impactSfxID;

        if (D_800F83E0[D_80063014->actorId].unk4 & 0x4000000) {
            D_80063014->unk3C >>= 1;
        }
    }

    func_800A8DCC(record->statusChange, record->statuses);

    if (forceAnim36) {
        D_800F83E0[D_80063014->actorId].unk4 &= ~0x200000;
    }

    func_800A8D88(record->additionalEffects, record->effectsModifier);
}

// D_800E7B28 opcode 0x0D (CMD_ITEM setup): item counterpart to
// func_800A79CC's kernel AttackData load, but much simpler -- items don't
// have the spell/enemy-skill/limit-variant dispatch, camera-id split
// (both unk60/unk64 default from the same field here), or D_8009D954
// linked-charge row. Ends the same way: func_800A8DCC/func_800A8D88 (the
// already-confirmed kernel "secondary status effect chance" system).
extern AttackEntry D_800722CC[];
void func_800A80DC(void) {
    AttackEntry* record = &D_800722CC[D_80063014->actionIndexCopy];

    D_80063014->mpCost = -1;
    D_80063014->unk40 = record->damageCalcID;
    D_80063014->actionPower = record->strength;
    if (record->elements != 0xFFFF) {
        D_80063014->actionElement = record->elements;
    }

    D_80063014->unk60 = record->cameraSingleID;
    D_80063014->unk64 = record->cameraSingleID;
    D_80063014->attackEffect = record->attackEffectID;
    D_80063014->unk6C = record->flags;
    func_800A8D60(record->targetFlags);
    func_800A8DCC(record->statusChange, record->statuses);
    func_800A8D88(record->additionalEffects, record->effectsModifier);
}

// D_800E7B28 opcode 0x0E: shared physical-attack weapon-config load, used
// by every command in the Attack family (Attack/Melee-Attack/Slash-All/
// 2x-Cut/4x-Cut/Flash/Morph/Deathblow). Party members (actorId<3) load
// their weapon's config row out of D_800F5EFC (0x18 B/battler); enemies
// skip straight to the shared tail. Tail: halves unk3C (damage?) under a
// D_800F83E0.unk4 status bit (0x4000000, unconfirmed which real status),
// forces a specific animation/message triple under another status bit
// (0x800), and the already-known "7777 curHP" easter egg reset.
extern void func_800A8DCC(s32, s32);
extern void func_800A8D60(s32);
void func_800A81B8(void) {
    s32 actorId = D_80063014->actorId;

    D_80063014->actionPower = 0x10;

    if (actorId < 3) {
        u8* cfg = &D_800F5EFC[actorId * 0x18];

        D_80063014->attackEffect = cfg[1];
        D_80063014->unk40 = cfg[2];
        D_80063014->unk3C = cfg[3];
        D_80063014->unk68 = cfg[4];
        D_80063014->actionElement = *(u16*)(cfg + 0xE);
        func_800A8DCC(5, *(s32*)(cfg + 0x14));

        if (D_80063014->unk60 == 0xFFFF) {
            D_80063014->unk60 = *(u16*)(cfg + 0x10);
        }
        if (D_80063014->unk64 == 0xFFFF) {
            D_80063014->unk64 = *(u16*)(cfg + 0x10);
        }
        D_80063014->unk6C = *(u16*)(cfg + 0x12);
        func_800A8D60(cfg[0]);

        D_80063014->unk54 = *(s16*)(cfg + 8);
        D_80063014->unk58 = *(s16*)(cfg + 0xA);
        D_80063014->unk5C = *(s16*)(cfg + 0xC);

        D_80063014->actionElement |= *(u16*)&D_8009D84C[actorId].unk24[0x18];
    }

    if (D_800F83E0[D_80063014->actorId].unk4 & 0x4000000) {
        D_80063014->unk3C >>= 1;
    }

    if (D_800F83E0[D_80063014->actorId].unk4 & 0x800) {
        D_80063014->unk54 = 0x16B;
        D_80063014->unk58 = 0x16C;
        D_80063014->unk5C = 5;
        D_80063014->unk68 = 0;
    }

    if (D_800F5BB8[D_80063014->actorId].curHP == 0x1E61) {
        D_80063014->actionElement = 0;
        D_80063014->inflictStatusMask = 0;
        D_80063014->unk84 = 0;
        D_80063014->unk88 = 0;
    }
}

// D_800E7B28 opcode 0x0F: pushes two display/animation-queue records for
// the current action -- one full Unk800A2F4C record (func_800A2F4C) and
// one abbreviated 2-byte-ish record via func_800A2FD0/func_800A317C (the
// same allocator func_800A317C itself uses to mark a slot unused).
void func_800A8424(void) {
    Unk800A2F4C* rec;
    s8* rec2;

    if (D_80063014->cmdAnimation < 0) {
        return;
    }

    rec = func_800A2F4C();
    rec->unk0 = D_80063014->actorId;
    rec->unk1 = D_80063014->unk1C;
    rec->unk5 = D_80063014->cmdAnimation;
    rec->unk3 = D_80063014->cmdIndexCopy;
    rec->unk2 = D_80063014->attackEffect;
    rec->unk4 = 0;
    rec->unk8 = D_80063014->unk60;

    rec2 = func_800A2FD0();
    *rec2 = D_80063014->actorId;
    *(s8*)(rec2 + 2) = 0;
    *(u16*)(rec2 + 4) = 0;
    func_800A317C();
    *(s8*)(rec2 + 1) = D_80063014->actorId;
}

void func_800A8528(void) { D_80063014->followUpCount = 4; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A853C);

void func_800A85A0(void) { D_80063014->followUpCount = 2; }

void func_800A85B4(void) {
    D_80063014->actionElement = 0x10;
    D_80063014->actionPower = 1;
    D_80063014->targetMask = 0;
    if (!((D_80163758[1] >> D_80063014->actorId) & 1)) {
        D_80063014->cmdAnimation = -1;
    }
}

// D_800E7B28 opcode 0x15: the enemy-AI VM's (func_800B1D48) fallback
// "basic attack" instruction -- attacks a uniformly random battler out of
// allowedTargetsMask. Party members no-op (their targets always come from
// the menu, never this random pick).
void func_800A85FC(void) {
    s32 targetBit;

    if (D_80063014->actorId < 3) {
        D_80063014->cmdAnimation = -1;
        return;
    }

    D_80063014->cmdIndex = CMD_ATTACK;
    D_80063014->targetMask = 0;
    targetBit = func_800B3030((u16)D_80063014->allowedTargetsMask);
    D_80063014->allowedTargetsMask = targetBit;
    D_80063014->actorId = func_80014A38(targetBit);
    D_80063014->cmdAnimation = 0x2F;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A866C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8A6C);

void func_800A8C94(void) {
    D_80063014->unk8C = 0xFF;
    D_80063014->unk40 = 0xB0;
    D_80063014->unk80 |= 1;
    D_80063014->unk3C = (s32)D_80063014->unk3C >> 1;
}

void func_800A8CC8(void) {
    D_80063014->unk6C &= ~0x2000;
    D_80063014->unk3C /= 3;
}

void func_800A8D04(void) { D_80063014->actionPower = 2; }

// seed this combatant's unk50 (a flag word later read by the damage formula
// in func_800AD804 -- bit 0x80 there appears to exempt a hit from the
// reduced per-target damage otherwise applied when an action strikes
// multiple targets) with a per-slot default, but only if nothing has set
// unk50 explicitly yet this turn (see func_800A8D60's sentinel check)
void func_800A8D60(s32 arg0);
void func_800A8D18(void) {
    func_800A8D60(D_800F5EFC[D_80063014->actorId * 0x18]);
}

void func_800A8D60(s32 arg0) {
    if (D_80063014->targetMask == 0xFF) {
        D_80063014->targetMask = arg0;
    }
}

static void func_800A8D88(s32 arg0, s32 arg1) {
    D_80063014->attackAddlEffect = -1;
    if (arg0 != 0xFF) {
        D_80063014->attackAddlEffect = arg0;
        D_80063014->addlEffectModifier = arg1;
        func_800A8E84(2);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8DCC);

void func_800A8E34(void) { func_800A79CC(); }

void func_800A8E54(s32 arg0) {
    D_80063014->unkF8 = arg0;
    D_80063014->unkAC = arg0 + 3;
    if (D_80063014->unkAC > 8) {
        D_80063014->unkAC = 8;
    }
}

const s16 D_800A02C0[] = {
    0x04, 0x3C, 0x04, 0x20, 0x01, 0x24, 0x10, 0x10, 0x04, 0x02, 0x02, 0x02,
    0x02, 0x01, 0x20, 0x04, 0x24, 0x10, 0x10, 0x04, 0x20, 0x10, 0x10, 0x10,
    0x30, 0x10, 0x20, 0x10, 0x10, 0x14, 0x01, 0x01, 0x01, 0x01, 0x01, 0x18};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A8E84);

static void func_800A9C24(void) {
    s32 temp_s0;
    s32 var_a0;
    s32 var_s0;
    s32 i;

    i = 0;
    var_a0 = 0;
    while (i < 7) {
        if (D_80163774[0] == D_800E7BA4[var_a0 + 0] &&
            D_80163774[1] == D_800E7BA4[var_a0 + 1] &&
            D_80163774[2] == D_800E7BA4[(var_s0 = var_a0) + 2]) {
            break;
        }
        i++;
        var_a0 += 3;
    }
    if (i) {
        D_80063014->actionIndexCopy = i + 0x68;
    } else {
        var_s0 = 4;
        for (i = 0; i < 4; i++) {
            var_s0 += func_80014BA8(10) & 0xFF;
            func_80014B54();
        }
        var_s0 += D_80063014->level / 21;
        var_s0 /= 2;
        var_s0 -= 4;
        if (var_s0 < 0) {
            var_s0 = 0;
        }
        if (var_s0 > 0xF) {
            var_s0 = 0xF;
        }
        D_80063014->actionIndexCopy = var_s0 + 0x38;
        D_80063014->cmdIndexCopy = 3;
    }
    D_80063014->targetMask = 0xFF;
    D_80063014->attackScenePos = D_80063014->actionIndexCopy;
    temp_s0 = D_80063014->cmdAnimation;
    func_800A8E34();
    D_80063014->cmdAnimation = temp_s0;
    D_80063014->mpCost = 0;
}

const u8 D_800A0398[] = {0x64, 0x14, 0x14, 0x14, 0xEC, 0xCE, 0xCE, 0x00};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800A9DA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA1C8);

static void func_800AA468(void) {
    s32 temp_s0;
    s32 var_s1;

    var_s1 = D_80063014->actorStatus;
    if (func_800B10B4(D_80063014->actorId)) {
        var_s1 |= 2;
    }
    temp_s0 = func_80014A58(var_s1 & 0x0400029A);
    temp_s0 += func_80014A58(var_s1 & 0x202000) * 2;
    D_80063014->targetDamage *= temp_s0 + 1;
}

static void func_800AA4FC(void) {
    s32 var_s0;

    var_s0 = 1;
    if (func_800B10B4(D_80063014->actorId) != 0) {
        var_s0 = 2;
    }
    if (D_80063014->actorStatus & 0x200000) {
        var_s0 *= 4;
    }
    D_80063014->targetDamage *= var_s0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA574);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA688);

s32 func_800AA6E8(s32 arg0, s32 arg1) {
    arg0 = arg0 < 4 ? 1 : 0;
    if (arg1 < 4) {
        arg0++;
    }
    return arg0 & 1;
}

static s32 func_800AA700(s32 arg0) {
    s32 var_v0;

    var_v0 = 0xF;
    if (arg0 < 4) {
        var_v0 = 0x3F0;
    }
    return func_800B3030(*D_80163758 & var_v0) & 0xFFFF;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA738);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AA950);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AABBC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB308);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB480);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB674);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB788);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB830);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AB9C4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ABA68);

const s32 D_800A03A0[] = {0x200, 0x100, 0x010, 0x020};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ABB0C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AC6B4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AC73C);

void func_800ACA24(void) {
    D_80063014->statusToAdd = 0;
    D_80063014->statusToCure = 0;
    D_80063014->statusToToggle = 0;
    D_80063014->statusAffectedMask = 0;
    D_80063014->damageLevel = 0;
    D_80063014->targetDamage = 0;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACA4C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACB98);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACD88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACE14);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ACE88);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD088);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD0FC);

void func_800AD324(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_t0;
    s32 var_a2;

    var_a2 = arg2;
    temp_t0 = D_80063014->attackPropertiesExtra & 1;
    if (arg3 & 1) {
        if (arg1 == D_80063014->targetIdx) {
            if (D_80063014->targetMP < var_a2) {
                var_a2 = D_80063014->targetMP;
            }
        }
        if (temp_t0) {
            var_a2 = -var_a2;
        }
        D_800F5BB8[arg0].unk30 -= var_a2;
    }
    if (arg3 & 2) {
        if (arg1 == D_80063014->targetIdx) {
            if (D_80063014->targetHP < var_a2) {
                var_a2 = D_80063014->targetHP;
            }
        }
        if (temp_t0) {
            var_a2 = -var_a2;
        }
        D_800F5BB8[arg0].unk2C -= var_a2;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD420);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD480);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD4EC);

const s8 D_800A04B0[] = {
    0x0A, 0x0B, 0x0C, 0x0D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x7F, 0x03, 0x34};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD5E8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD73C);

// multi-target damage-reduction formula, s32 func_800AD804(s32 damage, s32
// fullDamage): if fullDamage is false, it still gets forced true when
// unkB8 < 2 (single target) or unk50 & 0x80 is set (the exemption bit
// documented on unk50's seed at func_800A8D18/func_800A8D60 above); then
// if unkAC != 0 (hit-sequence position, see func_800A8E54) returns
// damage>>1, else returns damage unchanged when fullDamage else damage/3
// (magic-number signed divide) -- this is the classic "multi-target hits
// deal reduced per-target damage" mechanic
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD804);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD890);

// scale arg0 by a fixed-point random variance factor (~93.77%..100%), then
// clamp the result to a minimum of 1
static s32 func_800AD8DC(s32 arg0) {
    s32 temp_s0;
    s32 var_v0;

    var_v0 = arg0;
    temp_s0 = ((s32)(var_v0 * (func_80014B70() + 0xF01))) >> 0xC;
    var_v0 = temp_s0;
    if (temp_s0 == 0) {
        var_v0 = 1;
    }
    return var_v0;
}

void func_800AD924(void) { D_80063014->attackProperties |= 2; }

void func_800AD944();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AD944);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADBBC);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADC70);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800ADD2C);

s32 func_800AD804(s32, s32);
s32 func_800AD73C(s32);

void func_800ADDE8(void) {
    s32 base = D_80063014->unk4C + D_80063014->unk4;
    s32 term1 = base * 3;
    s32 term2 = D_80063014->unk48 * 0xB;
    s32 damage = (term2 + term1) * 2;
    D_80063014->unk214 = func_800AD8DC(func_800AD73C(func_800AD804(damage, 0)));
}

void func_800ADE5C(void) {
    D_80063014->targetDamage = D_80063014->actionPower * 20;
}

void func_800ADE84(void) {
    s32 value = D_80063014->unk48 * (0x200 - D_80063014->unk210);
    D_80063014->unk214 = func_800AD8DC(value / 32);
}

void func_800ADED8(void) {
    if (D_80063014->damageLevel & 0x40) {
        D_80063014->damageLevel = 1;
    } else {
        D_80063014->damageLevel = 0x80;
    }
}

void func_800ADF04(void) {
    D_80063014->attackPower = D_80063014->strength * 2;
    func_800AD944();
}

void func_800ADF38(void) {
    s32 divisor = func_80014A58(D_80063014->unk18);
    s32 result = 0;
    if (divisor != 0) {
        result = (D_80063014->unk48 + (divisor - 1)) / divisor;
    }
    D_80063014->unk214 = result;
}

void func_800ADFC0(void) {
    D_80063014->unk214 = *(u16*)(&D_800F5BB8[D_80063014->unk0].unk3C);
}

void func_800ADFF4(void) {
    s32 index = D_80063014->unk0;
    D_80063014->unk214 = D_800F83E0[index].maxHP - D_800F5BB8[index].unk3C;
}

void func_800AE050(void) {}

void func_800AE058(void) {}

void func_800AE060(void) {}

void func_800AE068(void) {}

void func_800AE070(void) {}

void func_800AE078(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE080);

void func_800AE234(void) {
    D_80063014->targetDamage =
        Savemap.memory_bank_1[26] + Savemap.memory_bank_1[27] * 256;
}

void func_800AE25C(void) {
    D_80063014->unk214 = D_800F83E0[D_80063014->unk208].curHP - 1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE2A0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE318);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE378);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE42C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE6C0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE764);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE82C);

void func_800AE954();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AE954);

const s8 D_800A04BC[] = {
    0x1E, 0x14, 0x3C, 0x1E, 0x7F, 0x7F, 0x0A, 0x64, 0x7F, 0x7F,
    0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x0D, 0x00, 0x00};
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEB20);

// this data belong to functions located above:
const u8 D_800A04D0[] = {0x0A, 0x19, 0x15, 0x0D, 0x10, 0x11, 0x03, 0x02,
                         0x0F, 0x1B, 0x14, 0x18, 0xFF, 0xFF, 0xFF, 0xFF};
int func_800B0378();
int func_800B062C();
int func_800B079C();
int func_800B0B8C();
int func_800B0910();
int func_800B089C();
int (* const D_800A04E0[])() = {
    func_800B0378, func_800B062C, func_800B079C, func_800B0B8C,
    func_800B0B8C, func_800B0B8C, func_800B0910, func_800B089C,
};
// ___end

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEB80);

void func_800AEBF0(void) { func_800AE954(); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEC10);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AEF68);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF0C4);

void func_800A7254(s32, s32, s32, s32);
void func_800AF1A8(s32 arg0) { func_800A7254(0, arg0, 8, 0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF1D4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF264);

void func_800AF320(s32 arg0, s32 arg1, s32 arg2) {
    func_800AEBF0();
    func_800AEB80(arg0, arg1, arg2);
    func_800AF0C4(arg0, arg1, arg2);
}

void func_800AF380(s32 arg0) { func_800A7254(2, arg0, 0x15, 0xF); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF3AC);

void func_800AF470(s32 arg0) { D_800F5BB8[arg0].unk28 = 3; }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF494);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF594);

void func_800B108C(s32 arg0);
void func_800AF63C(s32 arg0) { func_800B108C(arg0); }

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF65C);

static s32 func_800AF834(s32 arg0) {
    s32 result;
    s32 i;

    result = -1;
    for (i = 0; i < LEN(D_800A04D0); i++) {
        if (D_800A04D0[i] == arg0) {
            result = i;
        }
    }
    return result;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF874);

void func_800AF9C8();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800AF9C8);

extern s32 D_800F499C;
extern s32 D_800F49F8[][10];

s32 func_800AFE98(s32 arg0) { return D_800F49F8[D_800F499C][arg0] >> 0xC; }

// Initial (non-matching) decomp -- traced this session while hunting for
// "what actually detects HP<=0 and triggers death." CONFIRMED part: the
// per-combatant HP-delta-then-death-check block below (the `if (newHp < 0)`
// branch is the real Death-bit setter). The fixed-point accumulator setup
// above it is structurally faithful to the asm (3 parallel 32-bit
// accumulators per combatant, each holding a fractional remainder that
// carries across calls) but its exact game-meaning (which DoT/regen
// definition rateA/rateB select, why row selection alternates every call)
// is NOT resolved -- named generically rather than guessed. The trailing
// per-combatant loop over 16 counter bytes (reusing D_800F5BB8[i]'s
// unk10-unk1C words as a byte array) is likewise structurally preserved but
// its divisor-selection logic (the s1==0 / s1<2 / else branches) is
// unclear; left as-is rather than inventing a story for it.
extern s32 D_800F499C;   // toggles 0/1 each call -- selects a "row" below
extern s32 D_800F49A0[]; // stride 4, indexed directly by D_800F499C
extern s32
    D_800F49A8[]; // 2 rows (selected by D_800F499C) of 10 per-combatant words
extern s32
    D_800F49F8[]; // 2 rows (selected by D_800F499C) of 10 per-combatant words
extern s32 D_800F4A48[]; // per-combatant accumulator #1 (unclear semantics)
extern s32 D_800F4A70[]; // per-combatant accumulator #2 (unclear semantics)
extern s32 D_800F4A98[]; // per-combatant accumulator #3 -- its whole-part IS
                         // the HP delta applied below
extern void func_800A6278(s32, s32, s32);
extern void func_800A61D4(void);

void func_800AFECC(void) {
    s32 rowSel = D_800F499C;
    s32* rateAPtr = (s32*)((u8*)D_800F49F8 + rowSel * 0x28);
    s32* rateBPtr = (s32*)((u8*)D_800F49A8 + rowSel * 0x28);
    s32 accum0 = D_800F49A0[rowSel];
    s32 wholeA_slot0 =
        accum0 >>
        13; // one-time value, shared by every combatant's j==0 counter below
    s32 i;

    D_800F499C = rowSel ^ 1; // toggle row for next call
    D_800F49A0[rowSel] = accum0 & 0x1FFF;

    for (i = 0; i < 10; i++) {
        s32 rateA = *rateAPtr;
        s32 rateB = *rateBPtr;
        s32 accum1 = D_800F4A48[i] + rateB;
        s32 accum2;
        s32 accum3;
        s32 wholeA = accum1 >> 13;
        s32 wholeB;
        s32 wholeC;
        u8* counters =
            (u8*)&D_800F5BB8[i].unk10; // reinterprets unk10-unk1C as 16 bytes
        s32 j;

        D_800F4A48[i] = accum1 & 0x1FFF;
        accum2 = D_800F4A70[i] + rateA;
        wholeB = accum2 >> 12;
        D_800F4A70[i] = accum2 & 0xFFF;
        accum3 = D_800F4A98[i] + rateB * D_800F5BB8[i].unk6;
        D_800F4A98[i] = accum3 & 0x7FFF;
        *rateBPtr = 0; // one-shot consume this call's row value
        *rateAPtr = 0;

        if (!(D_800F83E0[i].status & 1)) { // skip if already dead
            wholeC = accum3 >> 15;
            if (D_800F83E0[i].curHP + wholeC < 0) {
                D_800F83E0[i].status |= 1; // Death bit -- the actual set
                func_800A6278(i, i, 1);
                func_800A61D4();
                D_800F83E0[i].curHP = 0;
            } else if (D_800F83E0[i].curHP + wholeC > D_800F83E0[i].maxHP) {
                D_800F83E0[i].curHP = D_800F83E0[i].maxHP;
            } else {
                D_800F83E0[i].curHP += wholeC;
            }
        }

        // Tail loop: 16 per-combatant counter bytes. Confirmed selection
        // rule (traced from the asm's branch chain, not guessed): counter 0
        // always uses the one-time shared `wholeA_slot0`; counters 2-3 use
        // `wholeB`; every other counter (1, 4-15) uses this combatant's own
        // `wholeA`. What these 16 counters actually represent per-combatant
        // is unclear (animation/hit-reaction timers is a guess, unconfirmed).
        for (j = 0; j < 16; j++) {
            s32 c = counters[j];
            if (c != 0) {
                s32 dec;
                if (j == 0) {
                    dec = wholeA_slot0;
                } else if (j == 2 || j == 3) {
                    dec = wholeB;
                } else {
                    dec = wholeA;
                }
                c -= dec;
                if (c < 0)
                    c = 0;
                if (c == 0)
                    func_800A7254(2, i, j, j);
                counters[j] = c;
            }
        }

        rateAPtr = (s32*)((u8*)rateAPtr + 4);
        rateBPtr = (s32*)((u8*)rateBPtr + 4);
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0170);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0234);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0378);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B062C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B079C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B089C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0910);

int func_800B0B8C(void) {}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0B94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0C14);

void func_800B0DF8(void) {
    if (D_80063014->targetConditionFlags & 2) {
        D_800F83E0[D_80063014->targetIdx].unk4 ^= 0x80;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0E5C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0EB4);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B0F04);

void func_800B0FFC(s32 arg0, s32 arg1, s32 arg2, s16* arg3) {
    func_800A31A0(
        arg0, 2, arg2, func_800A5EB0(func_8001521C(arg1), arg3) + 0x100);
}

void func_800B1060(s32 arg0) { func_800A31A0(10, 2, 1, arg0); }

void func_800B108C(s32 arg0) { func_800A31A0(arg0, 5, 0, 0); }

// true when the combatant's HP is at or below a quarter of max -- the "Near
// Death" threshold used by weapon-specific damage formulas (e.g. Powersoul's
// HP-based multiplier).
static s32 func_800B10B4(s32 arg0) {
    return D_800F83E0[arg0].curHP <= D_800F83E0[arg0].maxHP / 4;
}

void func_800B10F0(s32, s32, s32, s32, s32, s32, s32);
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B10F0);

// find arg0 in D_800F5F44.attackIDs[]; returns its index, or 0x20 (and signals
// func_800155A4) if it is not present
static s32 func_800B11B4(s32 arg0) {
    s32 i;
    u16* p;

    for (i = 0, p = D_800F5F44.attackIDs; i < LEN(D_800F5F44.attackIDs); i++) {
        if (*p == arg0) {
            break;
        }
        p++;
    }
    if (i == LEN(D_800F5F44.attackIDs)) {
        func_800155A4(0x20);
    }
    return i;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1218);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1268);

// nonzero if D_800F5F44.D_800F7DC8 is < 3
static u32 func_800B12DC(void) {
    u32 result = 0;
    s32 cmp = (s32)D_800F5F44.D_800F7DC8;

    if (cmp < 3) {
        result = (u32)~D_800F5F44.D_800F7DC8 >> 0x1F;
    }
    return result;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1304);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1368);

// Resolve a packed variable reference for the battle-script VM (func_800B1D48):
// map combatant arg0 + descriptor arg1 to a backing pointer (*arg2) and return
// a bit offset into it. arg1 < 0x2000 selects the per-combatant variable bank
// D_800F87F0[arg0] (0x80 bytes each); arg1 < 0x4000 selects the shared,
// battle-wide bank D_800F83A4; otherwise the per-combatant stat record
// D_800F83E0[arg0] (0x68 bytes each). func_800B153C / func_800B141C then read
// or write at that bit offset.
static s32 func_800B13B0(s32 arg0, s32 arg1, void** arg2) {
    s32 var_a1;

    var_a1 = arg1;
    if (var_a1 < 0x2000) {
        *arg2 = &D_800F87F0[arg0 * 0x80];
    } else if (var_a1 < 0x4000) {
        *arg2 = D_800F83A4;
        var_a1 -= 0x2000;
    } else {
        *arg2 = &D_800F83E0[arg0];
        var_a1 -= 0x4000;
    }
    return var_a1;
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B141C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B153C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1624);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B16D0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B17F0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B18A8);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1A5C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1AA0);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1B64);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1C1C);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1C94);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B1D48);

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2A2C);

extern u16 D_800F83B2; // shared battle-script "current target selection"
                       // variable -- an earlier opcode in the same AI
                       // script writes it before this one runs
extern s32 D_800F4AC8; // script-controlled queue priority for the action
                       // about to be queued

// enemy-AI VM (func_800B1D48) "queue an attack" instruction: un-biases
// attackIndex back to a raw list index for the two commands whose index
// gets a category offset added elsewhere (CMD_SUMMON: +0x38, matching
// func_800A73D8's own bias for actionIndexCopy; CMD_ENEMY_SKILL: +0x48),
// resolves an attack id through func_800B11B4 for the internal-only
// command 0x20 (unnamed in BattleCommand -- "attack using this raw attack
// id" rather than an index), then commits the script-chosen
// command/attack/target directly into the priority table via
// func_800A3D4C -- bypassing func_800A4350/the ring buffer entirely
void func_800B2B5C(s16 actorId, s16 cmdIndex, s16 attackIndex) {
    QueuedAction entry;

    if (cmdIndex == CMD_SUMMON) {
        attackIndex -= 0x38;
    }
    if (cmdIndex == CMD_ENEMY_SKILL) {
        attackIndex -= 0x48;
    }
    if (cmdIndex == 0x20) {
        attackIndex = func_800B11B4(attackIndex);
    }

    *(s16*)&D_800F83E0[actorId].unk18 = D_800F83B2;

    entry.priority = D_800F4AC8;
    entry.actorId = actorId;
    entry.cmdIndex = cmdIndex;
    entry.attackIndex = attackIndex;
    entry.targetMask = D_800F83B2;
    func_800A3D4C(&entry);
}

u16* func_800B2EBC();
s32 func_800B2C60(s32 arg0) {
    s32 var_s0;
    u16* ret;

    var_s0 = 0;
    if (arg0 <= 0xFFFE) {
        ret = func_800B2EBC();
        if (ret) {
            var_s0 = ret[2];
        }
    }
    return var_s0;
}

static void func_800B2CAC(s32 arg0, s32 arg1) {
    switch (arg0) {
    case 0:
        D_800F83A6 = Savemap.memory_bank_1[arg1];
        return;
    case 1:
        Savemap.memory_bank_1[arg1] = D_800F83A6;
        return;
    }
}

INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2CFC);

u16* func_800B2EBC();
INCLUDE_ASM("asm/us/battle/nonmatchings/battle", func_800B2EBC);

u8 func_800B2F30(void) { return func_80014B70(); }

u16 func_800B2F50(void) { return func_80014BE4(); }

// scale a 16-bit value into the range 1..100
static s32 func_800B2F70(void) {
    return (((func_800B2F50() & 0xFFFF) * 0x63) / 0xFFFF) + 1;
}

static s32 func_800B2FC4(s32 arg0) {
    return (arg0 * (func_800B2F30() + 0xF01)) >> 12;
}

static s32 func_800B3000(u16 arg0) {
    s32 count = 0;

    while (arg0 != 0) {
        if (arg0 & 1) {
            count++;
        }
        arg0 >>= 1;
    }
    return count;
}

// pick one set bit out of mask uniformly at random (returns 0 if mask is
// empty); shared by the enemy-AI VM's target-selection opcodes and the
// func_800A85FC "random attack" fallback
s32 func_800B3030(s32 mask) {
    s32 count = func_800B3000(mask & 0xFFFF);
    s32 pick;
    s32 bit;

    if (count == 0) {
        return 0;
    }

    pick = func_800B2F30() % count;
    for (bit = 1; bit & 0xFFFF; bit <<= 1) {
        if (mask & bit) {
            pick--;
            if (pick < 0) {
                return bit;
            }
        }
    }
    return 0;
}

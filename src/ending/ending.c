#include "common.h"

#include <libetc.h>
#include <libgpu.h>
#include <libgte.h>

extern s16* D_800A6528;
extern s32 D_800AF410;

typedef struct {
    u8 pad0[8];
    s32 unk8;
} D_80075D00_t;
extern D_80075D00_t* D_80075D00;

void func_80033B70();
void func_80036298();

typedef struct EndingObj {
    /* 0x00 */ void* unk0;
    /* 0x04 */ void* unk4;
    /* 0x08 */ u8 pad08[0x28 - 0x08];
    /* 0x28 */ s32 unk28;
    /* 0x2C */ s32 unk2C;
    /* 0x30 */ s32 unk30;
    /* 0x34 */ s32 unk34;
    /* 0x38 */ s16 unk38;
    /* 0x3A */ s16 unk3A;
    /* 0x3C */ s16 unk3C;
    /* 0x3E */ s16 unk3E;
    /* 0x40 */ s16 unk40;
    /* 0x42 */ s16 unk42;
    /* 0x44 */ s16 unk44;
    /* 0x46 */ s16 unk46;
} EndingObj;

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A0030);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A04C4);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A09DC);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A0AB8);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A0BA8);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A0CAC);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A0E68);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A0F90);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A11B4);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A12F0);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A139C);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A14BC);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A16E4);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A17C0);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A19A4);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A1E20);

static void func_800A1ED4(s32 arg0) { D_800A6528 = (s16*)arg0; }

s32 func_800A1EE4(void) { return 0; }

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A1EEC);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A1F48);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A1FA4);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A1FC8);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2014);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A208C);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A20D4);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A20F8);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2190);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A21CC);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2248);

s32 func_800A2274(void) {
    D_800A6528 = (s16*)((u8*)D_800A6528 - (*D_800A6528 + 1) * 2);
    return 1;
}

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A22A4);

s32 func_800A22D4(void) {
    D_800AF410 = 0;
    return 1;
}

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A22E4);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2328);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2380);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A23F8);

s32 func_800A2420(void) {
    s16* temp_v1;

    temp_v1 = D_800A6528;
    D_800A6528 = temp_v1 + 1;
    return (u32)D_80075D00->unk8 >= (u32)*temp_v1;
}

/* resets callbacks, graphics, pad, and geometry state */
static void func_800A2458(void) {
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    PadInit(0);
    InitGeom();
    func_80036298();
    func_80033B70();
}

void func_800A24A8(void) {
    s32 temp_v0;
    s32 new_var;

    new_var = -1;
loop_1:
    temp_v0 = func_800484A8();
    if (temp_v0 == new_var) {
        VSync(0);
        goto loop_1;
    }
    if (temp_v0 != 0) {
        do {
        } while (func_80048540(1) != 0);
    }
}

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2504);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A273C);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2888);

void func_800A2934(EndingObj* arg0, EndingObj* arg1) {
    arg0->unk4 = arg1;
    arg1->unk0 = arg0;
    arg1->unk34 = 0x1000;
    arg1->unk30 = 0x1000;
    arg1->unk2C = 0x1000;
    arg1->unk28 = 0x1000;
    arg1->unk46 = 0;
    arg1->unk44 = 0;
    arg1->unk42 = 0;
    arg1->unk40 = 0;
    arg1->unk3E = 0;
    arg1->unk3C = 0;
    arg1->unk3A = 0;
    arg1->unk38 = 0;
}

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2974);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2A2C);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2C68);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2E80);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2F1C);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A2FB8);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A310C);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A3178);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A3210);

/* splices a node out of a doubly-linked list */
static void func_800A32D8(void* arg0) {
    u32 v1, v0;
    v1 = *(u32*)arg0;
    v0 = *((u32*)arg0 + 1);
    *(u32*)((u8*)v1 + 4) = v0;
    *(u32*)v0 = v1;
}

static void func_800A32F0(void* arg0) { *((u8*)arg0 + 0xE) = 8; }

static void func_800A32FC(void* arg0) { *((u8*)arg0 + 0xE) = 4; }

static void func_800A3308(void* arg0) { *((u8*)arg0 + 0xE) = 0x10; }

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A3314);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A3368);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A343C);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A34C4);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A358C);

INCLUDE_ASM("asm/us/ending/nonmatchings/ending", func_800A379C);

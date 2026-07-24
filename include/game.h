#ifndef GAME_H
#define GAME_H

#include "psxsdk/libgpu.h"
#include <common.h>
#include <libgte.h>
#include <libgpu.h>

#ifndef FF7_STR
#define _S(x) x       // check the usage of 'bin/str' to see how this works
#define _SL(len, x) x // same as _S, but for fixed-length strings with padding
#endif

#define MAX_PARTY_COUNT 9
#define MAX_INVENTORY_COUNT 320
#define MAX_MATERIA_COUNT 200

typedef unsigned char ff7s[];

typedef enum {
    LABEL_TIME,
    LABEL_GIL,
} Labels;

typedef enum {
    INIT_YAMADA,
    INIT_WINDOW,
    INIT_KERNEL,
    BATTLE_BROM,
    BATTLE_TITLE,
    BATTLE_BATTLE,
    BATTLE_BATINI,
    BATTLE_SCENE,
    BATTLE_BATRES,
    BATTLE_CO,
    YAMADA_FILE_NUM,
} YamadaFile;

typedef struct {
    s32 loc; // disk sector where the file can be found
    s32 len; // file size in bytes
} Yamada;

typedef enum {
    LBA_INIT_YAMADA = 614,
    LBA_ENEMY6_SEFFECT = 30046,
    LBA_ENEMY6_OVER2 = 30694,
    LBA_ENEMY6_FAN2 = 30695,
} Lba;

typedef enum {
    SYNC_NONE = 0x0,
    SYNC_WAITING = 0x1,
    SYNC_DONE = 0x2,
} ScriptSyncState;

// Script controlled movement modes.
typedef enum {
    SMODE_NONE = 0x0,
    SMODE_WALK = 0x1,
    SMODE_JUMP = 0x3,
    SMODE_LADDER_V = 0x4,
    SMODE_LADDER_H = 0x5,
} ScriptedMoveMode;

typedef enum {
    FIELDOP_NONE,
    FIELDOP_FIELD_MAP_CHANGE,
    FIELDOP_ENTERING_BATTLE,
    FIELDOP_LOAD_MOVIE,
    FIELDOP_PLAY_MOVIE,
    FIELDOP_PLAY_ENDING_FMV,
    FIELDOP_CHAR_NAME_ENTRY,
    FIELDOP_PARTY_SELECT,
    FIELDOP_SHOP,
    FIELDOP_PARTY_MENU,
    FIELDOP_BEAT_GAME,
    FIELDOP_UNKB,
    FIELDOP_LOAD_MINIGAME,
    FIELDOP_CD_CHANGE,
    FIELDOP_SAVE_SCREEN,
    FIELDOP_YUFFIE_STEALS_MATERIA,
    FIELDOP_YUFFIE_RETURNS_MATERIA,
    FIELDOP_REMOVE_CHARS_MATERIA_ACCESSORY,
    FIELDOP_UNK12,
    FIELDOP_UNK13,
    FIELDOP_UNK14,
    FIELDOP_UNK15,
    FIELDOP_MASTER_MATERIA_CHECK,
    FIELDOP_ADD_MASTER_MATERIA,
    FIELDOP_JENOVA_SYNTH_COPY_LEVELS,
    FIELDOP_UNK19,
    FIELDOP_GAME_OVER,
} FieldOpcode;

typedef enum {
    SCRL_OFF,
    // Scroll to center camera on given entity's 3D model.
    // Immediately sets camera to target coordinates.
    SCRL_TO_ENTITY_INSTANT,
    // Constant movement speed determined by number of steps.
    SCRL_TO_ENTITY_LINEAR,
    // Uses a precalculated sine table for a smoother start and stop.
    SCRL_TO_ENTITY_SMOOTH,
    // Scroll to center camera on given coordinates.
    SCRL_TO_COORDS_INSTANT,
    SCRL_TO_COORDS_LINEAR,
    SCRL_TO_COORDS_SMOOTH,
} ScrollMode;

typedef enum {
    SCRLST_INIT,
    SCRLST_ACTIVE,
    SCRLST_DONE,
} ScrollState;

// https://wiki.ffrtt.ru/index.php/FF7/Field/Script/Opcodes/6B_FADE
typedef enum {
    FFT_INSTANT,
    FFT_INV4_TO_FIELD_SUB,
    FFT_FIELD_TO_INV4_SUB,
    // Fade to black when changing field map.
    FFT_SYS_FADE_TO_BLACK_FIELD_CHANGE,
    FFT_INSTANT_BLACK,
    FFT_STANDARD_TO_FIELD_ADD,
    FFT_FIELD_TO_STANDARD_ADD,
    FFT_INSTANT_INV1_SUB_HOLD_FIELD,
    FFT_INSTANT_INV1_SUB_HOLD_COLOR,
    FFT_INSTANT_STANDARD_ADD_HOLD_FIELD,
    FFT_INSTANT_STANDARD_ADD_HOLD_COLOR,
    FFT_FIELD_TO_STANDARD_ADD_HOLD_COLOR,
    FFT_FIELD_TO_STANDARD_SUB_HOLD_COLOR,
    // Fade to black when opening menu.
    FFT_SYS_FADE_TO_BLACK_MENU,
} FieldFadeType;

typedef enum {
    OMODE_INSTANT,
    OMODE_LINEAR,
    OMODE_SMOOTH,
    OMODE_DONE,
} OffsetMode;

typedef enum {
    MOVCMD_IDLE,
    MOVCMD_ACTIVE,
    MOVCMD_DONE,
} MovieCommandState;

typedef struct {
    s16 x1;
    s16 y1;
    s16 z1;
    s16 x2;
    s16 y2;
    s16 z2;
} LinePos;

typedef struct {
    s16 unk0;
    s16 unk2; // current page
    s16 unk4;
    s16 unk6; // total item count
    s16 unk8;
    s8 unkA;
    s8 unkB; // selected element
    s8 unkC;
    s8 unkD; // elements per page
    s8 unkE;
    s8 unkF;
    s8 unk10;
    s8 unk11;  // scroll type: 0=no wrap, 1/2:wrap, 3>:infinite
} Unk80026448; // size: 0x12

typedef union {
    void* poly;
    POLY_FT4* ft4;
    SPRT* sprt;
    TILE* tile;
    TILE_1* tile1;
    BLK_FILL* blk_fill;
    LINE_F2* linef2;
} Gpu;

typedef struct {
    u32 checksum;
    u8 leader_level;
    u8 leader_portrait;
    u8 portrait2;
    u8 portrait3;
    s8 leader_name[0x10];
    u16 leader_hp;
    u16 leader_hp_max;
    u16 leader_mp;
    u16 leader_mp_max;
    s32 gil;
    s32 time;
    s8 place_name[0x20];
    s32 menu_color[3];
} SaveHeader; // size: 0x54

// partially inspired by Q-Gears 'VI. The Save game format'
typedef struct {
    u8 char_id;
    u8 level;
    u8 strength;
    u8 vitality;
    u8 magic;
    u8 spirit;
    u8 dexterity;
    u8 luck;
    u8 strength_bonus;
    u8 vitality_bonus;
    u8 magic_bonus;
    u8 spirit_bonus;
    u8 dexterity_bonus;
    u8 luck_bonus;
    u8 limit_level;
    u8 limit_charge;
    u8 name[12];
    u8 weapon;
    u8 armor;
    u8 accessory;
    u8 status_flags; // Status effects that remain after battle. 0x10 = Sadness,
                     // 0x20 = Fury.
    u8 order;        // 0xFF = front row, 0xFE = back row
    u8 level_progress_bar; // ui related
    u16 limit_learn;
    u16 kill_count;
    u16 limit_lv1_count;
    u16 limit_lv2_count;
    u16 limit_lv3_count;
    u16 hp_cur;
    u16 hp_base;
    u16 mp_cur;
    u16 mp_base;
    u32 unk34;
    u16 hp_max;
    u16 mp_max;
    u32 exp;
    /* 0x40 */ s32 materia_weapon[8];
    /* 0x60 */ s32 materia_armor[8];
    /* 0x80 */ u32 exp_to_next_level;
} SavePartyMember; // size:0x84
typedef struct {
    SaveHeader header;
    /* 0x54 */ SavePartyMember party[MAX_PARTY_COUNT];
    /* 0x4F8 */ u8 partyID[4];
    /* 0x4FC */ u16 inventory[MAX_INVENTORY_COUNT];
    /* 0x77C */ s32 materia[MAX_MATERIA_COUNT];
    /* 0xA9C */ s32 yuffie_stolen_materia[48];
    /* 0xB5C */ u8 unk_b5c[32];
    /* 0xB7C */ s32 gil;
    /* 0xB80 */ s32 time;
    /* 0xB84 */ s32 countdown_timer_seconds;
    /* 0xB88 */ s32 game_timer_fraction;
    /* 0xB8C */ s32 countdown_timer_fraction;
    /* 0xB90 */ s32 worldmap_exit_action;
    /* 0xB94 */ u16 current_module;
    /* 0xB96 */ u16 current_location_id;
    /* 0xB98 */ u16 padding2;
    /* 0xB9A */ s16 field_x;
    /* 0xB9C */ s16 field_y;
    /* 0xB9E */ u16 field_triangle;
    /* 0xBA0 */ u8 field_direction;
    /* 0xBA1 */ u8 step_id;
    /* 0xBA2 */ u8 step_offset;
    /* 0xBA3 */ u8 padding3;
    /* 0xBA4 */ u8 memory_bank_1[256];
    /* 0xCA4 */ u8 memory_bank_2[256];
    /* 0xDA4 */ u8 memory_bank_3[256];
    /* 0xEA4 */ u8 memory_bank_4[256];
    /* 0xFA4 */ u8 memory_bank_5[256];
    /* 0x10A4 */ u16 phs_locking_mask;
    /* 0x10A6 */ u16 phs_visibility_mask;
    /* 0x10A8 */ u8 unk_10a8[48];
    /* 0x10D8 */ u8 battle_speed;
    /* 0x10D9 */ u8 battle_msg_speed;
    /* 0x10DA */ u16 config;
    /* 0x10DC */ u8 button_config[16];
    /* 0x10EC */ u8 field_msg_speed;
    /* 0x10ED */ u8 D_8009D7D1;  // ??
    /* 0x10EE */ u16 D_8009D7D2; // ??
    /* 0x10F0 */ u32 D_8009D7D4;
} SaveWork; // size: 0x10F4

// The "current action" context battle.c operates on via global D_80063014.
// Field names/roles cross-referenced against the community-documented
// "Actor Battle Data" struct at
// https://wiki.ffrtt.ru/index.php/FF7/Battle/Battle_Mechanics -- renamed
// only where this codebase's own (already-decompiled) usage independently
// corroborates the wiki's offset (a matching bit value, an existing
// formula, a confirmed array/sentinel pattern, etc), not from the wiki
// alone. Fields with no such local confirmation are left as unkXX even
// where the wiki names them.
typedef struct {
    s32 actorId; // used to self-check status/config (func_800B10B4,
                 // D_800F5EFC[actorId*0x18]) and to seed a self-target bit
                 // (1 << actorId)
    s32 level;   // used as `x += level / 21` in an unlifted scaling calc
    s32 unk8;
    s32 cmdIndex;    // per an existing comment on func_800A1798
    s32 actionIndex; // copied into cmdIndexCopy/actionIndexCopy below
    s32 unk14;
    s32 allowedTargetsMask; // set to `1 << actorId` (self-target default)
    s32 unk1C;
    s32 cmdAnimation; // saved/restored (temp = x; ...; x = temp) -- an
                      // animation-index idiom
    s32 attackEffect;
    s32 cmdIndexCopy;    // wiki: "Command Index (again)"
    s32 actionIndexCopy; // confirmed: set to actionIndex verbatim elsewhere
    s32 unk30;
    u8 unk34[4]; // character spacing array
    s32 mpCost;  // cleared before a cost calc
    s32 unk3C;
    s32 unk40;
    s32 actionElement;
    s32 actionPower;
    s32 attackPower; // confirmed: `attackPower = strength * 2`
    s32 targetMask;  // 0xFF sentinel pattern ("no target yet" / "all")
    s32 unk54;       // populated by both func_800A81B8 (weapon config) and
                     // func_800A79CC (kernel AttackData, = impactSfxID there)
    s32 unk58;       // same source as unk54; func_800A79CC sets both equal
    s32 unk5C;       // same shape; func_800A79CC skips this one if unk6C & 4
    s32 unk60;       // confirmed via func_800A79CC: defaults from the resolved
                     // AttackEntry's cameraSingleID (offset 8), overridable per
                     // Limit-Break-variant or per-enemy-attack lookup
    s32 unk64;       // ditto, from AttackEntry.cameraMultiID (offset 0xA)
    s32 unk68;
    s32 unk6C;
    s32 unk70;
    s32 unk74;
    s32 unk78;
    s32 unk7C;
    s32 inflictStatusMask; // confirmed: set to 0x400000, this session's
                           // confirmed Manipulate status bit
    s32 unk84;
    s32 unk88;
    s32 unk8C;
    s32 cmdProperties; // several unlifted callers OR in one flag each
                       // (0x80/0x40/0x04/0x800) from an incoming arg;
                       // 0x2000 confirmed via func_800AD0FC -- gates a call
                       // to the on-death handler func_800A2DB0; 0x200
                       // confirmed via func_800A1798 -- gates a per-
                       // equipped-materia command-linked charge-counter
                       // dispatch (see D_80162978_INVESTIGATION.md,
                       // "Steal As Well"); 0x100000 set by that same
                       // dispatch when a matching materia's charge count
                       // is already exhausted
    s32 unk94;
    s32 attackScenePos; // set to actionIndexCopy's value in one place
    s32 unk9C;
    s32 unkA0;
    s32 unkA4;
    s32 unkA8;
    s32 unkAC;
    s32 unkB0;
    s32 followUpCount; // small assigned constants (2, 4)
    s32 unkB8;
    s32 attackAddlEffect; // paired with addlEffectModifier below (both
                          // set together from two args, or -1 = none)
    s32 addlEffectModifier;
    s32 unkC4;
    s32 actorStatus;       // confirmed: `& 0x200000`, this session's confirmed
                           // Death Sentence status bit
    s32 unkCC;             // indexes D_800A01A8, otherwise unidentified
    u8 followUpActions[8]; // confirmed: 0xFF-sentinel-terminated array
    s32 strength;          // confirmed via the attackPower formula above
    s32 unkDC;
    s32 unkE0;
    s32 unkE4;
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    s32 unkF4;
    s32 unkF8;
    s32 unkFC;
    s32 unk100[0x40];
    s32 unk200;
    s32 unk204;
    s32 targetIdx; // confirmed: indexes D_800F83E0 directly
    s32 unk20C;
    s32 unk210;
    s32 targetDamage;     // accumulated (`*=`) and reset in the same function
    s32 attackProperties; // confirmed: `|= 2`, matching the wiki's own
                          // "bit 2 = Physical" note
    s32 unk21C;
    s32 attackPropertiesExtra; // wiki: heal/critical/damage-MP flags;
                               // confirmed via func_800AD0FC: bit 0 = heal
                               // (vs. damage), bit 2 (0x4) = apply to MP
                               // instead of HP
    s32 unk224;
    s32 unk228;
    s32 unk22C;
    s32 damageLevel;          // gated on `& 0x40`, reassigned to 1 or 0x80
    s32 targetConditionFlags; // confirmed: `& 2` checked
    s32 statusToAdd;          // confirmed: cleared together with the next 3
                              // fields in the same function
    s32 statusToCure;
    s32 statusToToggle;
    s32 statusAffectedMask;
    s32 unk248;
    s32 unk24C;
    s32 unk250;
    s32 unk254;
    s32 targetHP;  // confirmed: read and compared against a running value
    s32 targetMP;  // ditto
} ActorBattleData; // size: 0x260

// seems to be related to a party member during battle
typedef struct {
    s32 unk0;
    s16 unk4;
    s8 unk6;
    s8 unk7;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
    s16 unk14;
    s16 unk16;
    s32 unk18;
    s32 unk1C;
    s8 unk20;
    s8 unk21;
    s8 unk22;
    s8 unk23;
    u8 unk24[0x28];
    // 16 rows (confirmed via func_801B11BC, which walks all 16): one 6-byte
    // resolved-effect row per equipped materia slot (8 weapon + 8 armor,
    // Savemap.materia_weapon[8]/materia_armor[8]). Row: [0]=materia id
    // (0xFF=unused), [1]/[2] resolved kernel ability-effect byte (see
    // func_801B11BC), [3]=3 by default (func_801B137C), [4]=a 0xFF/status
    // sentinel toggled by func_801B11BC, [5] unconfirmed.
    u8 un4C[16][6];
    u8 unAC[4];
    u8 unB0[0x390];
} Unk8009D84C; // size: 0x440

// Per-materia kernel record, 0x14 B stride, indexed by materia ID
// (0x00-0x5A per the community materia-ID table; 0xFF = empty slot).
typedef struct {
    /* 00 */ u16 unk0;
    /* 02 */ u16 unk2;
    /* 04 */ u16 unk4[6]; // [0..3] confirmed: AP thresholds (x100) for
                          // star level 2/3/4/5; 0xFFFF = "no such level"
                          // (materia masters earlier). func_8001AC9C
                          // computes current star level (1-5) by checking
                          // a materia's live AP against these, highest
                          // first, and separately counts how many of the
                          // 4 are real (not 0xFFFF) into D_80062FBC.
                          // [4]/[5] unconfirmed (kept in-array, not split
                          // out, because other code indexes this array
                          // dynamically up to [4]).
    /* 10 */ u8 unk10;
    /* 11 */ u8 unk11;  // aliased elsewhere as D_800730DD[id][0]; &0xF =
                        // pairing category: 5 = Support (needs a partner,
                        // func_801B14E8/func_80017F38), 7 = "fire once per
                        // battle" support (func_801B11BC/func_801B1530),
                        // 6/8/9/10/11/12 = which of 6 writer functions a
                        // paired Support materia's bonus routes to when
                        // THIS materia is the partner (func_80018D4C).
    /* 12 */ u8 opcode; // aliased elsewhere as D_800730DE[id][0]; for a
                        // Support-category materia, its own "which
                        // effect" opcode (0x51-0x63, func_80018028's
                        // dispatch -- see func_80018028's own comment in
                        // src/main/18B8.c for the full opcode map).
    /* 13 */ u8 unk13;  // aliased elsewhere as D_800730DF[id][0]; start of
                        // a per-effect parameter block func_80018028
                        // copies 5 bytes from (into scratch D_80062E54) --
                        // NOT fully resolved: a naive 5-byte read from
                        // this offset runs 4 bytes past this struct's
                        // declared 0x14-byte end, into the next materia
                        // record. Either this struct needs more trailing
                        // fields than currently declared, or the real
                        // parameter block lives at a different base this
                        // investigation hasn't identified -- flagged, not
                        // guessed.
} Unk800730CC; // confirmed size >= 0x14; see unk13's comment for why the
               // true size may be larger

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u32 unk4;
} Unk80062F7C;

typedef struct {
    /* 0x00 */ LinePos pos;
    /* 0x0C */ u8 isActive;
    /* 0x0D */ u8 entityId;
    /* 0x0E */ u8 touch;
    /* 0x0F */ u8 across;
    /* 0x10 */ u8 requestPushScript;
    /* 0x11 */ u8 requestTalkScript;
    /* 0x12 */ u8 touchOn;
    /* 0x13 */ u8 touchOff;
    /* 0x14 */ u8 proximityAngle;
    /* 0x15 */ u8 isOnLine;
    /* 0x16 */ u8 slipDisabled;
    /* 0x17 */ u8 unk17;
} FieldLine; // size:0x18

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 unkC;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s8 unk1C[0x3B];
    /* 0x57 */ u8 entityId; // script entity that owns this model
    /* 0x58 */ u8 requestPushScript;
    /* 0x59 */ u8 unk59;
    /* 0x5A */ u8 requestTalkScript;
    /* 0x5B */ u8 unk5B;
    /* 0x5C */ u8 unk5C;
    /* 0x5D */ u8 scriptedMoveMode; // enum ScriptedMoveMode
    /* 0x5E */ u8 activeAnimId;
    /* 0x5F */ s8 unk5F;
    /* 0x60 */ s16 animSpeed;
    /* 0x62 */ s16 animCurrentFrame;
    /* 0x64 */ s16 animLastFrame;
    /* 0x66 */ u16 unk66; // model id
    /* 0x68 */ s8 unk68[0x1C];
} Unk80074EA4; // size:0x84

typedef struct {
    /* 0x00 */ u8 unk0[4];
    /* 0x04 */ u8 unk4; // model file index
    /* 0x05 */ u8 unk5[3];
} Unk8008357C; // size:0x8

typedef struct {
    /* 0x00 */ u8 unk0[0x1A];
    /* 0x1A */ u16 unk1A; // offset from unk1C to the animation headers
    /* 0x1C */ u8* unk1C;
    /* 0x20 */ u8 unk20[4];
} Unk8004A62CSub; // size:0x24

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ Unk8004A62CSub* unk4; // per-model-file records
} Unk8004A62C;

typedef struct {
    u8 enabled;
    u8 segmentActive;
    u8 rngId;
    s8 currentOffset;
    s16 amplitude;
    s16 start;
    s16 target;
    s16 numStepsPerSegment;
    s16 currentStep;
} FieldShakeData; // size:0xE

typedef struct {
    u8 unk0;
    // enum FieldOpcode.
    u8 opcode;
    // Used by some opcodes to carry extra info, ie. shop id
    // for shop menu, char id for name entry screen.
    s16 opcodeParam;
    // Stores player position when exiting field or jumping between field maps.
    s16 pcPosX;
    s16 pcPosY;
    s16 pcPosZ;
    // Used by field script opcodes SCR2D, SCRLC, and SCRLA to set target
    // coordinates when scrolling camera.
    s16 cameraScrollTargetX;
    s16 cameraScrollTargetY;
    s16 cameraScrollTargetZ; // Unused.
    // Scale of current field map. Affects 3D model sizes, movement speed, and
    // collision and interaction radius.
    s16 currentFieldScale;
    // viewOffset* are set by VWOFT opcode which applies viewOffset to
    // player's Z axis when camera is not scrolling.
    u8 viewOffsetNumSteps;
    u8 viewOffsetCurrentStep;
    u8 viewOffsetMode; // enum OffsetMode.
    u8 unk15;
    u16 viewOffset;
    s16 viewOffsetStart;
    s16 viewOffsetTarget;
    u8 unk1C;
    u8 cameraScrollMode; // enum ScrollMode.
    u8 cameraScrollTargetId;
    u8 cameraScrollState; // enum ScrollState.
    u16 cameraScrollNumSteps;
    // Following two variables are set when exiting from field to mini games,
    // world map, or another field map.
    u16 pcWalkMeshId;      // Walk mesh triangle id player is inside of.
    u16 pcDirection;       // Direction player is facing.
    s16 movieCommandState; // enum MovieCommandState.
    u16 modelCount;
    s16 pcModelId;
    u16 idleAnimId;
    u16 walkAnimId;
    u16 runAnimId;
    u8 characterLock;
    u8 suspendWalkAndAnim;
    u8 menuDisabled; // Set by MENU2.
    u8 unk35;
    u8 mapJumpDisabled; // Set by MPJPO. Disables gateways to other maps.
    u8 scrloSet;        // Set by SCRLO. Unused(?)
    // Set by MPDSP in field map junbin5. Also set to 1 if
    // fadeType == FFT_INSTANT_BLACK.
    u8 mpdspSet;
    // Set by MVCAM. Static field map camera is used instead of dynamic movie
    // camera.
    u8 movieCamDisabled;
    // Set by BGMOVIE. Enables movie camera if moviecamDisabled is not set.
    // Increases movement speed.
    u8 backgroundMovieEnabled;
    // Set by BTLON to disable or enable random encounters.
    u8 battlesDisabled;
    // Set by BTLTB.
    // Each field map has two sets of encounters BTLTB can switch between.
    u8 encounterTableId;
    // Set by BTLMD and BTMD2.
    u8 battleMode1;
    u16 battleMode2;
    u16 unk40;
    u8 unk42;
    u8 unk43;
    u32 nextBattleMusic;
    u32 nextFieldMusic;
    // Set by FADE or NFADE to start fades.
    u16 fadeType; // enum FieldFadeType.
    s16 fadeAdjust;
    s16 fadeSpeed;
    s16 fadeRed;
    s16 fadeGreen;
    s16 fadeBlue;
    u16 nFadeRedStart;
    u16 nFadeGreenStart;
    u16 nFadeBlueStart;
    s16 nFadeRedTarget;
    s16 nFadeGreenTarget;
    s16 nFadeBlueTarget;
    u16 prevFieldId;
    u8 unk66;
    u8 unk67;
    // Uses PADx macros in libetc.h
    u32 activeKeys;      // Currently active keys.
    u32 oldActiveKeys;   // activeKeys from last frame.
    u32 newActiveKeys;   // Was inactive last frame.
    u32 newInactiveKeys; // Was active last frame.
    u32 activeKeys2;
    u32 oldActiveKeys2;
    u32 newActiveKeys2;
    u32 newInactiveKeys2;
    s16 currentMovieFrame;
    // Set by SHAKE to enable a randomized camera shake effect.
    FieldShakeData shakeX;
    FieldShakeData shakeY;
    // Set by BGSCR. Affects parallax effect on camera movements.
    u16 layer2_bgScrollXSpeed;
    u16 layer2_bgScrollYSpeed;
    u16 layer3_bgScrollXSpeed;
    u16 layer3_bgScrollYSpeed;
    // Can be overridden by BGPDH.
    u16 layer3_depth; // Default: 1.
    u16 layer2_depth; // Default: 4095.
    // Bit fields that define which walk mesh triangles
    // the player can't travel between. IDLCK can override the accesses.
    u8 blockedAccesses[64];
    // Bit fields. Set by BGON, BGOFF, BGCLR, BGROL, and BGROL2.
    u8 backgroundLayerVisibility[64];
    u16 pad;  // Necessary with 4 byte alignment?
} FieldState; // size:0x134

typedef struct {
    u8 eventDataVersion;
    u8 eventVersion;
    u8 numEntities;
    u8 numModels;
    u16 stringOffset; // Offset to strings
    u16 numExtras;    // Akao and tutorials
    u16 scale;
    u16 pad[3];
    char author[8];
    char name[8];
    /*
    char entityNames[numEntities][8];
    u32 extras[numExtras]; // Offsets to akao/tutorial blocks
    u16 entityScripts[numEntities][32]; // Offsets to entity scripts
    */
} FieldScriptHeader; // size:Varies

extern u8 D_80049208[12];   // window colors maybe??
extern u8 D_800492F0[][12]; // see Labels enum
extern Unk8004A62C* D_8004A62C;
extern u16 D_80062D78; // pressed button?
extern u16 D_80062D7C; // pressed button?
extern u16 D_80062D7E; // pressed button?
extern u16 D_80062D80; // tapped button
extern u16 D_80062D82; // repeated button
extern u8 D_80062D98;
extern u8 D_80062D99;
extern s32 D_80062DCC;
extern u8 D_80062F19; // Enemy Lure/Away Modifier
extern u8 D_80062F1A;
extern u8 D_80062F1B;
extern Gpu D_80062F24;
extern u16 D_80062F3C;
extern s32 D_80062F58;
extern u_long* D_80062FC4;
extern ActorBattleData* D_80063014; // "current action" context, see the
                                    // struct's own comment
extern DRAWENV D_800706A4[2];
extern u8 g_FieldMusicLock; // MUSIC/FMUSC skip the sound engine while nonzero
                            // (set by the MULCK opcode)
extern u8 D_80070788;
extern u8 g_EntityToLine[48];
extern u16 D_800707BE;
extern u16 g_FieldWaitCounter[48];      // Used by WAIT opcode to pause script
extern u16 g_SavedFieldScriptPC[48][8]; // Program counters of paused scripts
extern s16 D_80071A5C;
extern u8 g_FieldScriptSyncWaitEntity[48][8];
extern s8 g_FieldDebugCurPage;
extern u8 D_80071E24;
extern u8 D_80071E2C;
extern u8 D_80071E30;
extern MATRIX* D_80071E40;
extern u8 D_80071E34;
extern u8 g_CurrentEntity; // entity owning the currently executing script
extern Unk800730CC D_800730CC[];
// D_800730DD/DE/DF are NOT independent arrays -- confirmed aliases of
// Unk800730CC's own unk11/opcode/unk13 fields (same 0x14 stride, byte
// offsets 0x11/0x12/0x13 respectively). Kept as separate declarations
// only because that's how they were first found (raw asm operands, no
// backing struct-field access yet); prefer `D_800730CC[id].unk11` etc.
// going forward.
extern u8 D_800730DD[][0x14]; // == D_800730CC[id].unk11
extern u8 D_800730DE[][0x14]; // == D_800730CC[id].opcode
extern u8 D_800730DF[][0x14]; // == D_800730CC[id].unk13 (+ overflow, see
                              // Unk800730CC.unk13's own comment)

// Elemental-affinity resolver pair (func_80019608 + these two tables):
// D_80069508[16] holds up to 16 known element IDs (fixed/kernel-wide,
// populated elsewhere -- not yet traced); func_80019608(elementId) linear-
// scans it and returns the matching slot index (0-15) or -1. D_8006950A
// is the parallel PER-CHARACTER output, 3 bytes/slot (one element's
// weak/resist/absorb-style flag triplet, unconfirmed which byte is which)
// -- OR'd with a caller-supplied flag by func_80018ECC/func_80018FC0 (the
// category-6/8 writers reached from func_80018D4C's paired-Support-materia
// dispatch, i.e. materia pairing with an Elemental-adjacent partner).
extern u8 D_80069508[16];
extern u8 D_8006950A[16][3];

// Status-affinity resolver pair, same shape as the elemental one above but
// for statuses: D_80069554[56] holds up to 56 known status IDs (kernel-
// wide, byte-per-status encoding rather than the runtime 32-bit bitmask);
// func_8001964C(statusId) resolves to a slot index (0-55) or -1.
// D_80069558 is the parallel per-character output, 5 bytes/slot -- OR'd
// with a caller-supplied flag by func_800190E8/func_80019064 (categories
// 9/10 from func_80018D4C's dispatch).
extern u8 D_80069554[56];
extern u8 D_80069558[56][5];

extern Unk80074EA4 D_80074EA4[2];
extern u8 D_800756E8[]; // per-model flags, indexed by field model id
extern s32 D_800756F8[];
extern int D_80075DEC;           // buffer index, either 0 or 1
extern u8 D_80075E24[256];       // map-local memory bank for field scripts
extern s8 D_80077F64[2][0x3400]; // polygon buffer
extern FieldLine g_FieldLines[32];
extern DRAWENV D_8007EAAC[2];
extern DISPENV D_8007EB68[2];
extern u8 g_EntityToModel[48]; // entity id -> model id (0xFF: none)
extern s8 D_8007EBCC;
extern s8 D_8007EBDC;
extern u8 D_8007EBE0;      // field debug mode
extern u8 g_CharacterLock; // mirror of the UC opcode's control-lock flag
extern u8 g_EntitySplitJoinState[48]; // states for SPLIT and JOIN opcodes
extern s16 D_80082248[]; // per-model current animation playback speed
extern u8 D_80083184[0x40];
extern u16 g_FieldScriptPC[48]; // program counters for active entity scripts
extern u8 D_8008325C[];         // per-model default animation id (DFANM)
extern u8 D_8008326C;
extern s32 D_80083274;
extern s16 D_8008327E[];
extern s16 D_800832A0;
extern s32 D_80083338;
extern u8 g_FieldScriptSyncState[48][8]; // sync states of entity scripts per
                                         // priority level
extern Unk8008357C* D_8008357C;
extern s16 g_FieldLineCount;
extern s8 D_80095DCC;
extern volatile u16 D_80095DD4;
extern s16 D_800965E0;
extern s16 D_800965E8;
extern volatile s16 D_800965EC;
extern u8 D_80099FFC;
extern s16 D_8009A000[1];
extern u32 D_8009A004[1];
extern s32 D_8009A008[1];
extern s32 D_8009A00C;
extern s32 D_8009A024[8];
extern u8 D_8009A058;                // currently executing field-script opcode
extern u8 g_FieldScriptPriority[48]; // active scripts execution priority
extern FieldState D_8009ABF4;
extern u8 D_8009AC2F;
extern u8 g_CharIdToEntity[9];
extern Unk80074EA4* g_FieldModels; // loaded field models
extern u8 g_FieldModelCount;       // number of allocated field models
extern FieldScriptHeader* g_FieldScripts;
extern FieldState* g_FieldState; // points to 0x8009abf4
extern SaveWork Savemap;         // 0x8009C6E4
extern u8 D_8009CBDC[];
extern u16 D_8009D288[];
extern u8 D_8009D2E7;
extern u8 D_8009D302;
extern u8 D_8009D391[1]; // part of a struct?
extern u8 D_8009D40D;
extern u8 D_8009D588; // disc number requested by the DSKCG opcode
extern u8 D_8009D684;
extern u8 D_8009D685;
extern u8 D_8009D686;
extern u8 D_8009D60E;
extern u8 DebugLevel; // field debug related
extern u8 D_8009D824;
extern s16 D_8009D828[]; // per-model base animation speed
extern u8 D_8009D8F8[];
extern u32 D_8009D260;
extern volatile s32 D_8009D268[];
extern Unk8009D84C D_8009D84C[3];
extern s8 D_8009FE8C;

// PSXSDK funcs
SVECTOR* ApplyMatrixSV(MATRIX* m, SVECTOR* v0, SVECTOR* v1);
MATRIX* RotMatrixYXZ(SVECTOR* r, MATRIX* m);
void SystemError(char c, long n);

s32 func_80014A38(u32 arg0); // bit index of the lowest set bit (log2 of a
                             // single-bit mask)
s32 func_80014B70(void);
s32 func_80014BA8(s32 arg0);
s32 func_8001521C(s32);
const char* func_80015248(s32 arg0, s32 arg1, s32 arg2);
void func_800155A4(s32);
void func_8001726C(s16, u16);
s32 func_8001C8D4();
void func_80021044(DRAWENV* draw_env, DISPENV* disp_env);
void func_800262D8();
void func_80026448(Unk80026448* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4,
                   s32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg9, s32 arg10,
                   s32 arg11, s32 arg12, u16 arg13);
void func_800269C0(void* poly);
s32 func_80026B70(unsigned char* str);
void func_80026F44(s32 x, s32 y, const char*, s32 color); // print FF7 string
int func_8002DA7C(void);

int func_80033DAC(int sector_no, void (*cb)());
int func_80033DE4(int sector_no);
int func_80033E34(int sector_no, size_t size, u_long* dst, void (*cb)());
int DS_read(int sector_no, size_t size, u_long* dst, void (*cb)());
int func_80033EDC(int sector_no, void (*cb)());
int func_80033F40(int sector_no, size_t size, u_long* dst, void (*cb)());
int func_80033FC4(int sector_no, size_t size, u_long* dst, void (*cb)());
u32 func_80034B44(void);

// from overlays
extern u8 SavedScriptIds[48][8]; // script ids of latest queued scripts
extern s32 D_8019DAA0;
extern u_long* D_8019D5E8; // otag array

#endif

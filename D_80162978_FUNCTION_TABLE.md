# Decompiled functions touched by the `D_80162978` investigation

Every function in `src/battle/{battle,battle1,battle2,battle3,batini}.c` that
has a real C body (not `INCLUDE_ASM`) and came up somewhere in
`D_80162978_INVESTIGATION.md`. Excludes functions still `INCLUDE_ASM`
(hand-traced from raw asm only, no C yet) and functions never touched by this
investigation (most of the battle overlay — this is not a full-file listing).

**Status** is ground truth, not a guess: `MATCHED` means the function's old
`asm/us/battle/nonmatchings/.../func_XXXXXXXX.s` file no longer exists (this
project's convention — a matched function's reference asm is deleted);
`NON-MATCHING` means that file still exists alongside the C body (kept for
permuter/reference use, meaning the C hasn't been confirmed byte-identical
yet). This is a mechanical check against the current worktree, current as of
this table's generation — re-run if functions have moved since.

**Name** is a descriptive label, not a formal rename — only `doDeathSequence`
has actually been renamed in source (`battle2.c`). Everything else is still
`func_XXXXXXXX`; the name column is this table's shorthand, not a claim about
the codebase.

## `D_80162978` pool — callbacks, constructors, tick functions

| Function | Status | Name | Description |
|---|---|---|---|
| `func_800BBEAC` | **NOT DECOMPILED** (`INCLUDE_ASM`; a best-effort C sketch exists only outside the repo, `/home/dmartin/bwork/`) | pool slot allocator | Scans `D_80161EF0[100]` for a free slot, claims it; hand off to permuter. |
| `func_800B88CC` | MATCHED | damage-popup job constructor | `func_800BC348`-scanned job constructor: allocates a `func_800CE7E0` slot seeded via `func_800B8A34(func_800B888C(battlerIdx), slot)`. |
| `func_800CE7E0` | MATCHED | damage-popup tick (twin A) | Counts down `delay`; on expiry (if `preset_idx != -1`) spawns a popup-display job + a `func_800CE638` checkpoint-restore job, frees itself. |
| `func_800CE970` | MATCHED | damage-popup tick (twin B) | Same as `func_800CE7E0` plus a screen-dim call when `unkA & 2`, and skips the popup spawn when `unk14 == 1`. |
| `func_800CEB48` | NON-MATCHING | popup finalize/free step | Runs at the end of every popup tick; reads `unk8`/`damage_dealt`/`unkC`, drives the death-sequence gate and animation-script re-select. |
| `func_800CE21C` | MATCHED | effect-slot dispatcher (by scene value) | Given a `battler_idx`, reads `D_801636B8[idx].D_801636BC` and constructs one of 6 effect-callback slots (`doDeathSequence`/`func_800C4814`/etc.) by value. |
| `func_800CE0C8` | MATCHED | effect-slot dispatcher (by explicit type) | Same 6-way dispatch as `func_800CE21C`, but the effect type is a caller-supplied `arg1` instead of scene data. |
| `func_800CE058` | MATCHED (not in doc's inventory, helper) | shared effect-slot init | Zeroes 3 render-blend fields + sets a flag bit on `D_80151200[idx]`; called by both dispatchers above before their switch. |
| `func_800D57C0` | NON-MATCHING | target-bitmask ramp/stagger dispatcher | `tick_count`/`delay` ramp gates a stagger interval (`preset_idx`); walks `target_battler_idx` as a bitmask of remaining targets via `state` as scan cursor, queues category-4 events (`func_800D4FA8`) per bit. |
| `func_800D5230` | NON-MATCHING | `D_800F10E4` ramp tick | Per-tick `accum += step`, writes `D_800F5B74 = accum>>16`, decrements `countdown`, frees at 0. Byte-identical idiomatic rewrite already done. |
| `func_800D5138` | NON-MATCHING | `D_800F10E0` ramp tick | Sibling ramp family: `accum += countdown` (a fixed per-tick step), clamps to `0xFFFF`, drives 3x `func_800C4FC8` screen-dim calls. |
| `func_800D5444` | NON-MATCHING | fn-ptr-family job constructor | Stores a combined 32-bit function pointer (`unk8:unkA`) consumed by `func_800D5350`'s bit-scanner (still untraced). |
| `func_800D58D0` | NON-MATCHING | windup-timer job constructor | Constructs the "queued action's delay from `D_800F99F4`" job; reached from 2 animation-VM opcodes (`0xF7` unconditional, `0xBE` gated). |
| `func_800D5A68` | MATCHED | frame-table-sequence job constructor | Constructs the `func_800D5938` job. |
| `func_800D6814` | MATCHED | reaction-flash helper (sound-id 0x88) | Sets `D_800F14D4 = 0x88`, calls `func_800D6734(battlerIdx, 2)` — category-6 handler target of the `func_800D4FA8` event queue. |
| `func_800D7340` | MATCHED | screen-flash-pulse job constructor | `func_800BBEAC(func_800D72B4)` — constructs the never-freeing `func_800D72B4` job; **no caller found anywhere in the tree** (dead code or unresolved indirect reference). |
| `func_800D3E8C` | MATCHED | `func_800D3D88` job constructor | Constructs a `D_801620AC` job seeded with a battler's current `D_801518EA` scale factor. |
| `func_800C64AC` | MATCHED | status-effect-tail job constructor | `func_800BBEAC(func_800C679C)` — one of 6 per-family "tail" helpers that decrement `delay` and free the slot for the 6 status-effect callbacks. |

## `D_801620AC` pool — movement/GTE tween jobs (10 slots)

All read fresh this pass (previously undocumented beyond "still raw asm,
none traced" in the doc — that line is now stale for these, they're matched
C). Same shape throughout: `D_801621B4`/`D_801621B6` = index/param, a
countdown (`D_801621B0` or similar), writing into
`D_801518E4[idx].D_80151A4C` (position) each tick, freeing (`D_801621AC =
-1`) at 0.

| Function | Status | Name | Description |
|---|---|---|---|
| `func_800CF2F0` | MATCHED | linear Y-tween tick | `D_80151A4C.vy += step` each tick, writes GTE scratchpad index; frees at count 0. |
| `func_800CF3CC` | MATCHED | linear Y-tween tick (alt field) | Same shape, writes `unk160.vy` instead of `D_80151A4C.vy`. |
| `func_800CF4A8` | MATCHED | linear X/Z-tween tick | Steps `vx`/`vz` each tick. |
| `func_800CF5BC` | MATCHED | camera/target-relative move tick | Computes a Y-velocity from two model indices' scaled heights on init, then steps `vx`/`vy`/`vz`; has an extra init-delay counter (`unk14`). |
| `func_800CF8C0` | MATCHED | "move over N frames" constructor | Computes per-frame x/y/z rates from current vs. target position, constructs a `func_800CFB14` job. |
| `func_800CFB14` | MATCHED | "move over N frames" tick | Steps position by the rates `func_800CF8C0` computed; init-delay counter. |
| `func_800CFCB0` | MATCHED | quantized-spawn-position tick | Steps position + pulls a Y-offset from a lookup table (`D_800EEB28`) indexed by a running counter. |
| `func_800CFE60` | MATCHED | 3-phase ramp-and-return tick | State machine (0/1/2): idle → ramp toward a target Y (computed from two models' heights) → return, then free. |
| `func_800D01C0` | MATCHED | 3-phase ramp-and-return tick (variant) | Same 3-phase shape as `func_800CFE60`, different field sourcing for the target Y. |
| `func_800D0578` | MATCHED | delay-then-message tick | Counts down `D_801621F4`; on 0, calls `func_800D4D4C` (message) and frees. |
| `func_800D061C` | MATCHED | delay-then-cue tick | Counts down; on 0, calls `func_800BBA84` (cue/sound queue) and frees. |
| `func_800D06B8` | MATCHED | delay-then-model-dispatch tick | Counts down, gated also on `D_801518DC`; on 0 calls `func_800D0C80` (magic-effect dispatch) and frees. |
| `func_800D0760` | MATCHED | disk-streaming-wait tick | Two-phase countdown (`preset_idx` then `delay`); mid-wait calls `func_800DCF60` each tick; frees at 0. |
| `func_800CDDE8` | MATCHED | fixed cue+message constructor | `func_800BBA84(delay, battler, 0)` + `func_800D7178(...)`, frees immediately — a "trigger fixed preset" one-shot, several animation-VM opcodes construct this with different fixed constants. |
| `func_800CDE78` | MATCHED (helper, not in doc's original inventory) | resync/comparison tick | Compares two `D_801516FC` rows, ORs a flag bit, calls `func_800D4D4C`, frees. |

## `D_801621F0` pool — popup/glyph/effect jobs (60 slots)

| Function | Status | Name | Description |
|---|---|---|---|
| `func_800CE638` | MATCHED | checkpoint-restore tick | 2-phase countdown; on completion calls `func_800A3534(unkA, unk8)` (HP/MP checkpoint restore) and frees. |
| `func_800CDFC4` | MATCHED | screen-dim countdown | Counts `D_801621F4` down; on 0 sets screen dim to max (`func_800C4FC8(0xFA,0xFA,0xFA)`) and frees. |
| `func_800CD82C` | MATCHED | one-shot flag setter | Sets `D_801590DC = 1`, frees immediately. |
| `func_800CDD44` | MATCHED | animation-script re-init | Resets a battler's script cursor (`D_8015191F`/`D_80151920`/`D_80151921`) — the model-swap/reload helper referenced by animation-VM opcodes `0xEE`/`0xFF`. |
| `func_800CE384` | MATCHED | byte-program mini-interpreter tick | Reads a small opcode stream (`0xFD`/`0xFE`/`0xFF` + default) from `unk10.ptr`, sets a render field, loops or frees. |
| `func_800D0B4C` | MATCHED | magic/summon effect dispatch (table family A) | Switches on `D_801518E4[idx].D_80151907` (4/7/8) to call one of `D_800EF9D8[]`/`func_801B037C`/`D_800EFFE0[]` — a magic-effect-ID → handler-function dispatcher. |
| `func_800D0C80` | MATCHED | magic/summon effect dispatch (table family B) | Larger sibling dispatch (cases 2/13/20/32/3) covering enemy-skill, PC-vs-PSX (`D_801031F0`) variant tables, and a `func_800C64AC` fallback. |
| `func_800D1110` | MATCHED | magic/summon effect dispatch (table family C, disk load) | Case-2 variant resolves a `D_800EEBB8` streaming-load record and issues the disk read via `func_800D088C`. |
| `func_800D088C` | MATCHED (helper) | streaming chunk load | `func_80033E34` + `func_800B7FB4` — loads a fixed-size chunk from disk into `0x801B0000`. |

## Command queue / ATB / battle loop architecture

| Function | Status | Name | Description |
|---|---|---|---|
| `func_800A38FC` | NON-MATCHING | per-frame battle tick | ATB-pause timer + the confirmed ATB gauge scan (fills gauges, queues `CMD_NONE`/chosen command/pending-menu push) + Limit-gauge fill + ATB display snapshot. |
| `func_800A4350` | MATCHED | push command into ring | Builds and pushes a `QueuedAction` into the 16-slot command ring. |
| `func_800A3E98` | MATCHED | direct priority-table insert | Builds a `QueuedAction` and inserts it straight into the 64-slot priority table (bypasses the ring). |
| `func_800A44D8` | MATCHED | Manipulate menu redirect | Party slot → real enemy battler id if that slot is a Manipulated stand-in. |
| `func_800A4CA8` | MATCHED | chosen-command lookup | `D_800F39F0[slot][0]` — the "already has a command chosen" ATB-scan branch. |
| `func_800A4D2C` | MATCHED | pending-menu FIFO push | Push (dedup) into `D_800E7A48`. |
| `func_800A4D88` | MATCHED | pending-menu FIFO remove | Remove-and-shift from `D_800E7A48`. |
| `func_800A4E00` | MATCHED | pending-menu FIFO peek | Peeks the FIFO head (Manipulate-resolved) or returns -1. |
| `func_800A4E40` | MATCHED | pending-menu FIFO rotate | Pop-then-repush the FIFO head. |
| `func_800A4AF4` | MATCHED | opcode `0x14`: wait-on-event-queue | Spins draining `D_80163798` (`func_800B6D6C`) until a status bit clears. |
| `func_800A653C` | MATCHED | ATB gauge reset on turn resolution | Resets `D_800F5BB8[realIdx].unk4 = 0` (through the Manipulate redirect) and clears the "chosen" bit. |
| `func_800A6590` | MATCHED (one-liner) | thin wrapper | `func_800A4D88(arg0)` — alias used elsewhere for FIFO removal. |
| `func_800B7FDC` | MATCHED | main per-frame battle-phase pump | Draws the GPU ordering-table, runs render/vsync, drains the queue ring; calls `func_800B8438` then `func_800A3ED0`. |
| `func_800B83C4` | MATCHED | battle-phase dispatcher entry | Calls into `func_800B8438`'s `switch (D_80163C7C)`. |
| `func_800B8438` | MATCHED | battle-phase `switch` | Central per-frame phase switch; the `default` case runs `func_800B8B48` (the new event-queue drain) among others. |
| `func_800BA40C` | MATCHED | per-frame party model tick | Loops 3 party slots, ticks animation (`func_800C1908`) + draw prep (`func_800BA598`) + far-color redraw (`func_800BB030`) when flagged. |
| `func_800BA4C8` | MATCHED | per-frame enemy model tick | Same as `func_800BA40C` for enemy slots (4..). |
| `func_800C1908` | MATCHED | per-battler animation-VM gate | Gated on `D_801518E4[idx].D_80151922 & 0x20`; calls `func_800C5170`/`func_800C5468`/`func_800C17A0` — the entry into the animation-script VM's per-battler tick. |
| `func_800C74A4` | MATCHED | secondary animation-VM entry (streaming) | `func_800C7C4C(3, ...)` for battler 3 specifically, gated the same way — the async disk-streaming-time invocation path. |
| `func_800DF900` | MATCHED | menu confirm-button handler | Finalizes the player's menu selection and calls `func_800A4350`. |

## Damage application / combat resolution

| Function | Status | Name | Description |
|---|---|---|---|
| `func_800A5BC8` | NON-MATCHING | counter/reaction-ability scanner | Scans 8 reaction-config rows (`D_8009D870`) for a `triggerId` match on `arg1*3+{1,2,3}`, RNG-gates, queues a reaction message. |
| `func_800A6000` | NON-MATCHING | generic state-change announcer | Snapshots 2 per-combatant byte fields, runs the message/AI VMs, diffs, announces changes. |
| `func_800A61D4` | NON-MATCHING | queued-message drain | Drains up to 8 pending `D_800F5F44.D_800F7DBC` bits into the message VM. |
| `func_800A6278` | NON-MATCHING | per-hit "affected by event" bookkeeping | Sets a couple of one-shot flags, calls `func_800A5BC8`/`func_800A6000`, conditionally drains messages — runs from both the command VM's hit tail and the Death-bit block. |
| `func_800A6A3C` | MATCHED | one-shot announcement latch | `D_800F5BB8[idx].unkE |= flag` — a pure "already shown" latch, no downstream reader found. |
| `func_800A6E6C` | MATCHED | thin wrapper | `func_800A7254(0, arg0, 13, arg1)`. |
| `func_800A79CC` | NON-MATCHING | kernel `AttackData` loader | The full spell/summon/enemy-skill/limit-variant setup: resolves camera IDs, `D_8009D954` linked-charge row, secondary status-effect chance. |
| `func_800A80DC` | NON-MATCHING | kernel item-data loader | Simpler sibling of `func_800A79CC` for `CMD_ITEM` (`D_800722CC` `AttackEntry` table, no spell-only dispatch branches). |
| `func_800A81B8` | NON-MATCHING | weapon-config loader (opcode `0x0E`) | Shared physical-attack setup for the whole Attack family; loads `D_800F5EFC[actorId*0x18]` for party members, seeds `targetMask` via `func_800A8D60`. |
| `func_800A8424` | NON-MATCHING | display/animation-queue push (opcode `0x0F`) | Pushes one full + one abbreviated animation-queue record for the current action. |
| `func_800A8528` / `func_800A85A0` | MATCHED | 4x-Cut / 2x-Cut setters | One-liners: `followUpCount = 4` / `= 2`. |
| `func_800A85FC` | NON-MATCHING | random-target Attack fallback (opcode `0x15`) | Hardcodes `CMD_ATTACK`, picks a uniformly random allowed target via `func_800B3030`. |
| `func_800A8D18` / `func_800A8D60` | MATCHED | `targetMask` weapon-config seed | `func_800A8D18` reads `D_800F5EFC[actorId*0x18]` byte 0; `func_800A8D60` seeds `targetMask` only if it's still the `0xFF` sentinel. |
| `func_800AD8DC` | MATCHED | damage-formula RNG variance | Scales a value by a ~93.77%-100% random factor, floors to 1. |
| `func_800ADE5C` | MATCHED | damage-formula variant | `targetDamage = actionPower * 20` — a fixed-multiplier formula case. |
| `func_800AE234` | MATCHED | damage-formula variant (Savemap-sourced) | `targetDamage` read directly from two `Savemap.memory_bank_1` bytes — scripted/fixed damage source, not yet tied to a specific mechanic. |
| `func_800AE954` | **NOT DECOMPILED** (`INCLUDE_ASM`; only a forward declaration in `battle.c`) | ATB fill-rate modifier | Touches only the ATB fill *rate*, gated on Haste/Slow/Sleep — never seeds the gauge itself. |
| `func_800AFECC` | NON-MATCHING | confirmed DoT/regen HP tick | Per-combatant fixed-point accumulator; sets the Death bit and applies `func_800A6278`/`func_800A61D4` on the zero-crossing. |
| `func_800B0FFC` | MATCHED | queue a battle text message | Thin wrapper resolving a message pointer (`func_800A5EB0`/`func_8001521C`) and forwarding to `func_800A31A0`. |
| `func_800B10B4` | MATCHED | Near-Death test | `curHP <= maxHP/4` — the Powersoul-style weapon-formula threshold. |
| `func_800B11B4` | MATCHED | attack-ID → slot lookup | Finds `arg0` in `D_800F5F44.attackIDs[]`; signals an error path if absent. |
| `func_800B2B5C` | NON-MATCHING | enemy-AI queued-attack builder | Un-biases `attackIndex` for `CMD_SUMMON`/`CMD_ENEMY_SKILL`, resolves the internal-only `0x20` command, builds and inserts a `QueuedAction` directly (bypasses the ring). |
| `func_800B2F30` | MATCHED | RNG byte | Wraps `func_80014B70()`. |
| `func_800B3000` | MATCHED | popcount | Bit-count of a `u16` mask. |
| `func_800B3030` | NON-MATCHING | pick random set bit | `count = popcount(mask)`, then walks to the `rng % count`-th set bit — feeds `func_800A85FC` and the enemy-AI VM's target selection. |
| `func_800A2D68` | MATCHED | push follow-up action | Appends to `followUpActions[8]` (0xFF-terminated). |
| `func_800A2DB0` | NON-MATCHING | enemy scripted death action | Looks up and queues a per-enemy "last action on death" (self-destruct/revenge) — ruled out as Final Attack. |
| `func_800A2BF4` | NON-MATCHING | linked-charge row lookup | Resolves the current action against `D_8009D954`, stores the row into `unk14`. |
| `func_800A304C` | NON-MATCHING | queued-message dedup | Keeps only the newest flagged entry per battler in a small message list. |
| `func_800A2894` | NON-MATCHING | "All Lucky 7s" announcer | curHP==7777 once-per-battler announcement + latch (confirmed against the community-documented status of the same name). |

## Support-materia / batini setup

| Function | Status | Name | Description |
|---|---|---|---|
| `func_801B0668` | NON-MATCHING | preemptive/back-attack ATB seed | Reads `BattleSetupType`, seeds `D_800F5BBC` (=`D_800F5BB8[].unk4` alias) per formation type or a speed formula. |
| `func_801B08C0` | **NOT DECOMPILED** (`INCLUDE_ASM`; traced from raw asm only, see investigation doc) | per-party-member battle setup | Zeroes ATB gauge; gates the Sneak-Attack-candidate flag (`unk29 & 2`) on the weapon-config byte; large 3×9 materia/formation setup loop, mostly untraced. |
| `func_801B11BC` | NON-MATCHING | materia-row + attack-slot cleanup | Cross-refs 16 equipped-materia rows against the kernel table (resolves effect byte, clears the Sneak-Attack-candidate category-7 flag); second pass over a still-unnamed `0x108`-`0x3FF` sub-region. |
| `func_801B14E8` | MATCHED | category-7 materia test | `(materia.unk11 & 0xF) == 7` test, encodes the slot index into the return value. |
| `func_801B1530` | MATCHED | category-7 scan (16 slots) | ORs `func_801B14E8` across all weapon+armor materia slots. |
| `func_801B18F8` | MATCHED | party stat init from persistent record | Copies maxHP/maxMP + several unk fields from `Unk8009D84C` into the live `D_800F83E0` combatant struct + menu widget, with a 999/9999 display-cap swap. |
| `func_801B23E0` | MATCHED | battle scene loader | Loads + gzip-decompresses a `BATTLE_SCENE` chunk by scene ID. |

## Sound / misc utility

| Function | Status | Name | Description |
|---|---|---|---|
| `func_800C2928` | NON-MATCHING | popup/glyph draw-mode call | `PadStop`-class draw call gated on `unkA & 2` in `func_800CE970`. |
| `func_800C2FD4` | NON-MATCHING | SFX-queue helper | Stages `D_8009A000/4/8` and calls `func_8002DA7C` — used by `doDeathSequence` and other status-effect callbacks. |
| `func_800B5FE8` | NON-MATCHING (explicit "initial decomp... NOT byte-matched" comment) | model "dirty/flash" flag propagator | ORs bit `0x8` into every sub-part flag byte of a battler's main model and its `D_800FA6D8` sub-model — the 6 status-effect callbacks' shared "flash the model" step. |
| `func_800C60F4` | MATCHED | message display speed | `Savemap.battle_msg_speed / 4 + 4` — feeds the status-effect-tail helpers' delay constant. |
| `func_800CD558` | MATCHED | script-stream u16 literal fetch | Reads 2 sequential bytes from the animation-script buffer (auto-advancing `D_801518E4[idx].D_80151920`), combines little-endian. |
| `func_800CD798` | MATCHED | model position resync | Copies `D_80163C80[idx]` (the position snapshot) back into `D_801518E4[idx].D_80151A4C` (live position) — companion to the opcode that snapshots it out. |
| `func_800CDDA4` | MATCHED | event-drain gate bit setter | Sets/clears bit `0x1` of `D_800F9DA4` based on `D_80166F68` — a sibling gate to the bit `0x2` `func_800A4AF4` spins on. |
| `func_800C33F0` | NON-MATCHING | status-effect tail (family A) | One of 6 per-status-callback "decrement delay, free at 0" helpers. |
| `func_800C4FC8` | MATCHED | screen dim/fade quad | Draws a full-screen semi-transparent `POLY_F4` at a given RGB (the `DR_MODE`-based dim/fade level setter used throughout the ramp families). |
| `func_800BB030` | MATCHED | battler sub-model draw prep | `SetFarColor` + per-sub-part `RotMatrixYXZ`/GPU-packet build; the "recompute far color" step `func_800BA40C`/`func_800BA4C8` call. |
| `func_800BB67C` | MATCHED | movement-state field setter | One-liner: `arg1->unk30 = arg0`. |
| `func_800BB90C` | MATCHED | fixed SFX trigger | Plays sound id `0xA0` at pan `0x7F` via `func_8002DA7C`. |

## Not actually decompiled — flagged, not included above

The `func_800D4FA8`/`func_800D4FF0` event queue (new this session, see
`D_80162978_INVESTIGATION.md`) and `func_800B888C`/`func_800D5938` were all
fully traced from **raw `INCLUDE_ASM` asm**, not from C — none of the five
have a C body in `battle1.c`/`battle2.c` yet, despite being completely
understood. A first draft of this table mistakenly listed them as
`NON-MATCHING` (which implies a C attempt exists); corrected here rather
than silently fixed, since it's an easy mistake to repeat — "traced" and
"decompiled" are not the same thing in this codebase, and
`func_800B888C` (still `INCLUDE_ASM`) is one character away from the
already-decompiled `func_800B88CC` in the table above.

| Function | Role (traced from asm only) |
|---|---|
| `func_800D4FA8` | event-queue producer — 16-slot ring, stores a `category` word + returns a 20-byte payload-record pointer. |
| `func_800D4FF0` | event-queue consumer — drains one `(category, record)` pair, or returns category `0` (empty) with the most-recent record. |
| `func_800B8B48` | event-queue per-frame dispatcher — 10-way jump table (disk-stream job registration, `func_800B88CC`/`func_800B8944` popup construction, `func_800D6814` reaction, a real `DS_read` disk load). |
| `func_800D5774` | event-queue producer (battler-gated) — queues category 4 or 6 depending on a per-battler bit in `D_800F836C`. |
| `func_800D5938` | frame-table sequencer — walks `D_801517F0` via a fixed-point ramp, queues category 10 per step. |
| `func_800B888C` | frame-table index lookup — linear-scans `D_801517F0` for `battlerIdx`, returns the found index or `0` if absent. |

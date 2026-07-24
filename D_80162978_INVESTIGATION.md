# D_80162978 process pool — investigation notes

Technical notes for the `BattleCallbackSlot` struct (`src/battle/battle_private.h`,
was `Unk80162978`) and its neighborhood. Code comments on this branch are kept
short; this file holds the fuller detail on field semantics, confidence
levels, and open questions.

## Callback family inventory

Every place that calls `func_800BBEAC(callback)` for `D_80162978`, found by
grepping the whole `mine/src` tree (not just battle2.c) — the array is
overlay-local to BATTLE (per the header's own "should be imported only by the
BATTLE overlay" guard comment), so no other file references it.

| Callback | Constructor | Status |
|---|---|---|
| `func_800CE970` / `func_800CE7E0` | `func_800B88CC` / `func_800B8A34` | lifted, understood |
| `func_800D57C0` | `func_800D58D0` | initial decomp, not matched |
| `func_800D5350` | `func_800D5444` | hand-traced, not lifted |
| `func_800D508C` | `func_800D5138` | hand-traced + lifted |
| (per-frame tick of the `D_800F10E4` ramp) `func_800D5230` | `func_800D52A0` | lifted; constructor hand-traced |
| `func_800D5938` | `func_800D5A68` | hand-traced |
| `func_800D72B4` | `func_800D7340` | hand-traced — doesn't touch its own slot at all |
| `func_800C679C` | `func_800C64AC` | hand-traced |
| `func_800C3578` | `func_800CE0C8`/`func_800CE21C` (effect type 0) | hand-traced |
| `func_800C4814` | ditto (effect type 8) | hand-traced |
| `func_800C3CA8` | ditto (effect type 2) | hand-traced |
| `func_800C328C` | ditto (effect type 3) | hand-traced |
| `func_800C40F4` | ditto (effect type 17/18) | hand-traced |
| `func_800C44B4` | ditto (effect type 7) | hand-traced |
| `func_801B0020`/`func_801B0220`/`func_801B04F4`/`func_801B0A90` | `magic/barrier.c` (`BarrierData`) | already named, baseline (not part of this pool's field naming) |
| `func_800C5694` | `func_800B6D6C` (dispatch case 4, not a top-level `func_800BBEAC` caller) | hand-traced — tweens `D_801518E4[target].D_801518EA`, a fixed-point scale factor, not HP |

Three more **consumers** (not `func_800BBEAC` callbacks themselves, but
functions that read fields of a slot), not traced — all too large for a
hand-traced pass:
- `func_800CEB48` (492 instructions) — called at the end of every
  `func_800CE970` tick; reads `unk8`/`damage_dealt`/`unkC` of that same
  slot. The popup family's finalize/free step (see below — this one is
  now understood in detail).
- `func_800C7C4C` (5768 instructions) — reads `target_battler_idx`/
  `damage_dealt`/`unkE`. A large fragment of the battle-script/model
  engine that happens to touch this struct, not a dedicated pool consumer
  (see "`func_800D58D0`'s caller" below — this function is now understood
  to be a separate bytecode interpreter).
- `func_800B6D6C` (647 instructions) — reads `tick_count`/`delay`/
  `preset_idx`/`target_battler_idx`. Likely a sibling producer to
  `func_800B8A34`. Not traced.

## Dispatcher: `func_800BC348`

The master per-frame tick for the pool. Called once per frame from the main
battle-update function (`battle1.c`, alongside the other per-frame
subsystem calls):

```c
void func_800BC348(void) {
    D_8015169C = 0;
    for (; D_8015169C < 100; D_8015169C++) {
        void (*callback)(void) = D_80161EF0[D_8015169C];
        if (callback == 0) continue;
        callback();
        if (D_80162978[D_8015169C].state == -1) {   // callback freed itself
            D_80161EF0[D_8015169C] = 0;              // slot fully released here
            D_80162978[D_8015169C].tick_count = 0;
            D_80162080--;                             // active-job counter
        }
    }
    D_8015169C = 0;
}
```

`D_8015169C` (`CUR_SLOT` in `battle_pool_watch.lua`) is the loop index,
visible to any callback while it's running — including one that constructs
a new job mid-scan. This is why `func_800BBEAC`'s allocator skips indices
below `D_8015169C` when picking a free slot: a job constructed by a
callback that's currently running gets a slot ahead of the scan cursor,
so it executes once in the same pass instead of waiting a full extra
frame.

Note the slot-release split: a callback sets its own `state = -1` to
signal "done," but `D_80161EF0[i]` (the field `func_800BBEAC`'s allocator
actually scans for "free") only gets zeroed here, one step later. A slot
can show a stale non-`-1` `state` for one poll right after it frees.

`func_800BC1E0` is the pool's one-time init/reset routine (called at
battle start). It zeroes `D_80162978`/`D_80161EF0` (100 entries) plus two
sibling pools using the identical generic-callback-slot pattern:
`D_801620AC[10]` (own callback array `D_80163B48`) and `D_801621F0[60]`
(own callback array `D_80163B84`, the popup/glyph-draw pool referenced
throughout this document). This pool design is reused at least 3 times in
the battle engine.

## Tooling

Two community-reference documents saved alongside this file (repo root of
this worktree, not yet committed): `FFVII_MATERIA_ID_TABLE.md` (materia ID
→ name → type, PC release) and `FFVII_BATTLE_SYSTEM_WIKI.md` (verbatim
save of finalfantasy.fandom.com's battle-system article — mechanics,
status list, damage formulas). Both are external documentation of
observed behavior, not derived from this codebase — per the "Naming
philosophy" section at the bottom of this file, treat any correlation
against real code findings as a hypothesis to check, not a given (a
materia ID or wiki fact matching a code offset by coincidence is exactly
the trap that section warns about — see the "Steal As Well" section above
for a caught example of this happening in practice).

`battle_pool_watch.lua`, at `/home/dmartin/ff7/battle_pool_watch.lua` (WSL
path; from Windows pcsx-redux use
`\\wsl.localhost\Ubuntu\home\dmartin\ff7\battle_pool_watch.lua`). Load via
the Lua console: `dofile('<path>')`. Draws several live ImGui windows:

- **"D_80162978 investigation"** — combatant HP/status (`D_800F83E0`),
  party HP/MP (`D_800F5BB8`) plus the `unk2A`/`maxMP` candidate field, live
  pool slots, and a persistent frame-tagged history (every line resolves
  the owning callback via `D_80161EF0`, Copy buttons throughout).
- **"Status ID correlator"** — auto-diffs `D_800F83E0[i].status` for
  newly-set bits and cross-references against status-effect-callback slot
  allocations, same battler, within a frame window; accumulates a
  permanent "confirmed bit ↔ callback" table so no manual eyeballing is
  needed. Also has one-click static dumps of the `D_800F9F3C` checkpoint
  ring and the `D_801517F0` frame table (no gameplay needed for either).
- **"Callback Breakpoint Inspector"** — arms real exec breakpoints on the
  19 known callbacks and ~10 constructor functions; hitting one pauses the
  emulator with a full slot dump (callbacks) or real register args
  (constructors). Resume via pcsx-redux's own Continue/Step.
- **"Sound Test"** — plays an arbitrary sound ID on demand by detouring
  the CPU directly into `func_8002DA7C`, no breakpoint involved
  (`PCSX.getRegisters()` gives a live, writable register pointer, and the
  script's per-frame draw callback is already a safe CPU-idle point). Must
  be used while the game is unpaused — the in-game pause banner mutes SFX
  downstream of the dispatch, independent of this tool.
- **"FF7 Battle Command Queue"** — the 16-slot ring buffer + 64-slot
  priority table from `ff7-re/reference/BATTLE_COMMAND_QUEUE.md`
  (`dc/battle-queue` branch) — unrelated to the pool itself.

Reload after any edit with the same `dofile(...)` call. Whenever a polled
struct's field shape changes, reset that struct's snapshot cache
unconditionally on load (`M.poolSnap = {}`, not `M.poolSnap or {}`) — a
stale snapshot from before a reload has old field names, and the first diff
against it passes `nil` to `string.format`'s `%d`, crashing.
`battlePoolWatch.clearHistory()` / `.clearStatusCorrelator()` reset their
respective logs; `.removeAll()` disables per-frame scanning and disarms
every breakpoint.

## Struct field semantics

- **`state`** (0x00): -1 = free, scanned by `func_800BBEAC`'s allocator
  scan of the *separate* `D_80161EF0[100]` callback-pointer table (not
  this field directly — a slot can show a stale non-free `state` for one
  poll after its callback already cleared).
- **`delay`** (0x04): countdown-to-fire in the `func_800B88CC`/
  `func_800CE970`/`func_800CE7E0` family; in `func_800D57C0` it's instead
  a fixed cap that `tick_count` counts up to. `func_800D5A68`/
  `func_800D5444`/`func_800D58D0` all take it as a plain caller-supplied
  arg (not always 0).
- **`tick_count`** (0x02, was `D_8016297A`): an up-counter 0..`delay`,
  ticking once every ~4 frames, in the `func_800D57C0` family. Once
  `tick_count >= delay`, `func_800D57C0` treats **`target_battler_idx` as
  a bitmask** of remaining targets, finds the lowest set bit at/after a
  cursor kept in `state` (reusing that field instead of the free/used
  convention while active), hands the bit to `func_800D4FA8(4)`, clears
  it, and only sets `state = -1` once the mask empties. `func_800D57C0`
  never writes `D_8016297E`, only reads it (as the stagger interval
  between bits via a modulus check) — any `D_8016297E` churn logged
  against this callback is leftover from a different job that reused the
  same slot number.
- **`damage_dealt`** (0x0A, was `D_80162982`): the actual damage number
  (or -1 for a miss/no-effect), confirmed across real attacks (Barret:
  231, a miss as -1, 233; a plain enemy hit: 10; a 2x-Cut hit: 1608) on
  both the `func_800CE7E0` and `func_800D57C0` families. `func_800D57C0`'s
  `delay` (ramp duration) is fixed per attacker/animation rather than
  scaled to the damage — three Barret attacks with wildly different
  outcomes all used `delay=6`; the enemy's plain attack used `delay=2` —
  it's animation timing, with `damage_dealt` just the payload dropped in
  when it completes.
- **`func_800CE7E0`** is a live-confirmed twin of `func_800CE970`
  (identical shape, already-decompiled source).
- **`D_800F83E0`** (`battle.h`, already-named upstream: `status`/`curHP`/
  `maxHP`) is genuinely indexed by *any* combatant (party 0-2, enemy 4-9,
  slot 3 unused) — confirmed via `batini.c`'s `D_800F83E0[4+i]` loop
  (i<6) and the overlapping `D_800F83AE[10]`. Array bound is `[10]`.
- **`D_800F5BB8`** (baseline symbol, type `Unk800AF470`, 0x44-byte stride
  per party member): `+0x3C` = current HP, `+0x3E` = current MP. Confirmed
  by a poison tick moving `+0x3C` by exactly the damage dealt while `+0x3E`
  stayed flat (MP untouched by a physical/status hit), and independently
  by `func_800A3534`'s asm, which does exactly two halfword stores at
  those two offsets.
- **`preset_idx`** (0x06, was `D_8016297E`) — see "`preset_idx`" below; an
  index into `D_800F9F3C`, a 128-entry checkpoint ring buffer (not a
  static preset table), consumed by `func_800A3534`.
- **Status bit table**: the full 32-bit `D_800F83E0.status` bitfield is
  documented in `battle.h`'s struct comment, confirmed bit by bit against
  the [FF7/Battle/Status_Effects wiki](https://wiki.ffrtt.ru/index.php/FF7/Battle/Status_Effects)
  (every locally live-tested bit matches the wiki exactly): `0x4` Sleep,
  `0x8` Poison, `0x10` Sadness, `0x40` Confuse, `0x100` Haste, `0x200`
  Slow, `0x400` Stop, `0x8000` Regen, `0x10000` Barrier, `0x20000`
  MBarrier, `0x40000` Reflect, `0x10000000` Death Force, `0x20000000`
  Resist, `0x1` Death, `0x80` Silence, `0x800` Frog, `0x1000` Small,
  `0x200000` Death Sentence, `0x400000` Manipulate, `0x800000` Berserk.
  Remaining bits (Near Death, Fury, Slow Numb, Petrify, Dual, Shield,
  Peerless, Paralysis, Darkness, Dual Drain, "Lucky Girl", Imprisoned) are
  wiki-only, not locally verified.
- **The 6 status-effect callbacks are NOT bound to any status bit above**
  — see "The 6 status-effect callbacks" below.
- **`unk8`/`unkA`** (0x0C/0x0E) — genuinely polymorphic: independent
  config values in one family, one joined function pointer in another;
  see "`unk8`/`unkA`" below. Left unnamed on purpose since no single name
  fits both consumers.
- **The struct itself: `Unk80162978` → `BattleCallbackSlot`**. Every field
  0x0C-0x1E has a comment even where no single name fits; see the struct
  definition in `battle_private.h` for the up-to-date per-field text (kept
  in sync with this file, not duplicated here).
- **Battle command queue priority bands**: `CMD_LIMIT` gets a special-cased
  `prio=5` (sorts ahead of waiting `prio=6` entries in the 64-slot table).
  A simultaneous 3-party-member "Attack" push at `prio=1` occurs when an
  enemy attacks — a dedicated counter-attack priority band. Poison ticks
  never appear in either queue — status damage-over-time bypasses
  `func_800A4350`'s queue system entirely, going through this pool
  instead.

## `func_800A3534` — checkpoint restore

Hand-traced from raw asm (49 instructions, not yet lifted to matched C — see
caveat below). A conditional HP/MP checkpoint restorer:

```c
void func_800A3534(s32 arg0 /* unused */, s16 ringIdx) {
    s16 slotIdx;
    u16 hpCheckpoint, mpCheckpoint;

    if (ringIdx < 0) return;
    slotIdx = D_800F9F3C[ringIdx];      // 14-byte-stride ring entry, field @0x0
    if ((u16)slotIdx >= 3) return;      // party slots 0-2 only

    hpCheckpoint = D_800F9F42[ringIdx]; // same entry, field @0x6
    if (D_800F83E0[slotIdx].maxHP >= hpCheckpoint) {
        D_800F5BB8[slotIdx].curHP = hpCheckpoint;
    }

    mpCheckpoint = D_800F9F44[ringIdx]; // same entry, field @0x8
    if (D_800F83E0[slotIdx].maxMP >= mpCheckpoint) {
        D_800F5BB8[slotIdx].curMP = mpCheckpoint;
    }
}
```

- `arg0` (first param, `$a0`) is read by the caller (`func_800CE638` in
  battle2.c passes `unkA`) but is never touched in the function body — a
  genuinely dead argument.
- Baseline (pre-existing) struct `Unk800AF470` (= `D_800F5BB8[]`'s element
  type, `battle_private.h`) has its 0x3C-0x3F range split into real
  `curHP`/`curMP` halfword fields (previously one `s32 unk3C`), matching
  the asm's two independent halfword stores at +0x3C and +0x3E.
- `Unk800F83E0.unk2A` → `maxMP`, and `unk28` → `curMP` (`battle.h`).
  Confirmed two ways: `func_800ABA68` (below) snapshots `curHP` and this
  same `unk28` field together from the same struct in the same pattern;
  and `func_801B18F8` sets `unk2A` (`maxMP`) right alongside `maxHP` from
  the same source struct (found via a pointer-param access,
  `arg2->unk2A`).
- **Not yet lifted to matched C** — the logic above is confirmed correct
  (traced instruction-by-instruction), but turning it into byte-identical
  C needs this repo's normal matching pipeline (permuter/decompme), same
  situation as `func_800BBEAC` below. Feed the C sketch above into that
  pipeline.

## `D_800F9F3C` — a checkpoint ring buffer, not a static preset table

Entries track a live party member's *current* HP/MP over the course of a
fight — dumping the table shows an entry holding a battler's exact current
HP at dump time, with new entries appearing (old ones unchanged) as the
fight progresses. That's a checkpoint log, not fixed ROM preset data. The
producer chain:

- **`func_800A311C(structPtr)`** (battle.c) allocates the next entry from a
  128-entry ring (`D_800F394C` is the wrapping write index, `& 0x7F`),
  writes `structPtr->byte0` (a battler index) into the ring entry's
  `partySlot` field, initializes `hpCheckpoint`/`mpCheckpoint` to the
  sentinel `0xFFFF` (unreachable by the `maxHP >= checkpoint` gate in
  `func_800A3534` until overwritten), and writes the allocated ring index
  back into `structPtr->byte3` — this is where `preset_idx`'s value
  originates.
- **`func_800ABA68(battlerIdx, val1, val2, val3)`** (battle.c) calls
  `func_800A311C`, then stores `val1`/`val2`/`val3` into the ring entry
  (offsets +0x2/+0x4/+0xA, not yet identified) and snapshots that
  battler's current HP and MP from `D_800F83E0` into the ring entry's
  `hpCheckpoint`/`mpCheckpoint` fields.
- Net effect: `func_800A3534` is a "restore HP/MP to a previously recorded
  checkpoint, clamped to current max" operation. What real mechanic this
  serves is still open — only 3 callers of `func_800ABA68` exist
  (`func_800AB830`, `func_800ABB0C`, `func_800AD088`), all still
  `INCLUDE_ASM`, not traced.

### Cross-reference: [FF7/Battle/Battle_Mechanics wiki](https://wiki.ffrtt.ru/index.php/FF7/Battle/Battle_Mechanics)

The community-documented "Actor Battle Data" structure (same wiki cited
elsewhere in this codebase for `QueuedAction`) correlates with this
codebase two ways:

1. **The wiki's "Actor Battle Data" struct is an exact match for an
   existing, already-in-this-codebase struct** — see "`ActorBattleData`"
   below. That one is renamed in code, with strong local corroboration for
   every renamed field (not just "the wiki says so").
2. **A separate, weaker correlation** against the still-open `D_800F9F3C`
   ring-entry mystery fields — cross-codebase inference only (FFRTT's
   offsets are their own reverse-engineering of what's likely a different,
   smaller record than `ActorBattleData`), with no local corroborating
   usage yet, so nothing here is renamed in code:

- **`258h Target's HP` / `25Ch Target's MP`**, in the wiki's "per-target
  action result" block — very likely just the wiki's name for exactly
  what's already independently confirmed as this ring entry's
  `hpCheckpoint`/`mpCheckpoint`.
- **`80h Inflicting Status(es)` / `84h Curing Status(es)` / `88h Toggling
  Status(es)`** on the action definition, mirrored at `238h`/`23Ch`/`240h`
  on the per-target result — three consecutive status-bitmask fields. A
  plausible (not yet confirmed) identity for `func_800ABA68`'s three
  still-unnamed caller-supplied values (stored at the ring entry's
  `+0x2`/`+0x4`/`+0xA`): candidate names `inflictStatusMask`/
  `cureStatusMask`/`toggleStatusMask`. To confirm: dump those 3 ring-entry
  offsets during a real status-inflicting hit and check whether the values
  look like bitmasks matching the confirmed status-bit table.
- **`C8h Actor's current status` / `228h Target's status`** — matches the
  already-named `D_800F83E0.status`.
- The wiki's "Inflicting Status(es)" living on the attack's own data is
  consistent with the 6 mystery status-effect callbacks only being
  reachable from `func_800CEB48` (which runs after physical-attack-shaped
  damage popups specifically, not spell/item application).

### `ActorBattleData` (was `Unk800A8D04`)

Not `D_80162978`-related directly, but too solid not to fold in.
`include/game.h` (a shared, cross-overlay header) declares
`extern Unk800A8D04* D_80063014;` — a 604-byte (`0x260`), all-`s32` struct
used only by `battle.c`. Field offsets and confirmed usages line up with
the wiki's "Actor Battle Data" almost exactly:

- `0xC` = `cmdIndex` — already had an existing code comment confirming
  this.
- `0x2C` = `actionIndexCopy` — code does `actionIndexCopy = actionIndex`
  (copying the `0x10` field verbatim).
- `0x4C`/`0xD8` = `attackPower`/`strength` — code computes
  `attackPower = strength * 2`.
- `0x80` = `inflictStatusMask` — code sets it to `0x400000`, the confirmed
  Manipulate status bit.
- `0xC8` = `actorStatus` — code checks `& 0x200000`, the confirmed Death
  Sentence bit.
- `0x208` = `targetIdx` — code uses it to index `D_800F83E0` directly.
- `0x218` = `attackProperties` — code does `|= 2`, matching the wiki's own
  note that bit `2` = Physical.
- `0x238`/`0x23C`/`0x240`/`0x244` (`statusToAdd`/`statusToCure`/
  `statusToToggle`/`statusAffectedMask`) — all four cleared together in
  one function, exactly the grouped quadruple the wiki describes.
- `0x258`/`0x25C` = `targetHP`/`targetMP` — both read and compared in the
  same function.

Renamed `Unk800A8D04` → `ActorBattleData` (matching the wiki's own section
title) and every field with a corroborating local usage (28 of the
struct's 88 fields); every field with *no* supporting usage in
already-decompiled code stays `unkXX`, even where the wiki names it.
`unkAC`/`unkCC`/`unkE4`/`unkE8`/`unkF8` have *some* local usage but too
weak or ambiguous to commit to a name.

## `preset_idx` (was `D_8016297E`)

The complete producer → consumer pipeline, traced statically through raw
asm:

**Producer** — `func_800B8A34(s16 action_id, s32 slot)` (raw asm, not
lifted; called from `func_800B88CC` as
`func_800B8A34(func_800B888C(battler_idx), v)`) copies a whole row out of a
family of parallel tables, all indexed by `action_id` with a 12-byte
stride, into the freshly-allocated `D_80162978[slot]`:
  - `D_800F99EC[action_id]` (u16) → `damage_dealt`
  - `D_801590CC` (plain global, not table-indexed) → `unk8`
  - `D_800F99EE[action_id]` (u16) → `unkA`
  - `D_800F99F0[action_id]` (s16, sign-extended) → `unkC`:`unkE` as one s32
  - `D_800F99F2[action_id]` (s16, sign-extended) → `unk10`:`unk12` as one s32
  - `D_800F8CF0` (plain global) → `unk14`
  - **`D_800F99F4[action_id]` (u16) → `preset_idx`**
  - `D_800F99F6[action_id]` (u8) → `unk15`

None of `D_800F99EC`/`EE`/`F0`/`F2`/`F4`/`F6` have repo symbols yet (no C
references them at all) — `func_800B8A34` is still `INCLUDE_ASM`. This is
the per-attack/animation "result definition" table: whatever `action_id`
identifies (returned by `func_800B888C(battler_idx)`), this table row is
what gets loaded into the popup/damage-number pool slot before its
`delay` countdown runs.

**`func_800B888C` resolved (24 raw-asm instructions, trivial to read in
full):** `action_id` is not an attack/animation ID at all — it's the
index at which `battler_idx` is found in `D_801517F0`, the already-known
78-entry (`0x4E`) "frame table" (per the Tooling section above, static-
dumpable without gameplay). Linear scan from index 0, first match wins;
if `battler_idx` isn't present anywhere in the table, the function falls
through and returns `0` (not a `-1`/error sentinel — a genuine quirk of
the retail asm, worth preserving exactly if this gets lifted). So the
whole `D_800F99xx` "result definition" row a popup loads is keyed by
*which slot of the frame table currently holds the acting battler*, not
by which attack/animation is playing — `D_801517F0` is the real
upstream ID space this table family is indexed by, still itself
unmodeled (what populates its 78 entries, and on what cadence, is not
traced in this pass).

**Consumer** — already-lifted `func_800CE7E0`/`func_800CE970`
(`battle2.c`), once `delay` hits 0 and `preset_idx != -1`: they forward it
verbatim (`D_801621F0[dst].unk8 = D_80162978[D_8015169C].preset_idx;`) into
a second pool slot bound to `func_800CE638`, whose countdown, once it
expires, calls `func_800A3534(unkA, unk8)` — i.e. `preset_idx` *is*
`func_800A3534`'s second argument, the ring index from the section above.
`-1` means "no checkpoint to restore" (matches the `!= -1` guard).

Full chain: **`func_800B888C` picks an action id → the `D_800F99xx` table
row seeds a damage-popup pool slot's `preset_idx` → once the popup
finishes, `preset_idx` is forwarded through a second pool slot →
`func_800A3534` uses it to conditionally snap a party member's current
HP/MP to a recorded checkpoint (clamped to their max).** Logged values (1,
4, 5, 6, 22, 23, 44, 45, 46...) are indices into this as-yet-unmodeled
table, not meaningful numbers by themselves.

The status-effect pool family (`func_800CE0C8`/`func_800CE21C` →
`func_800C3578` et al.) also writes `preset_idx` directly from a caller
arg, bypassing the `D_800F99xx` table — status effects (poison, etc.) need
their own way to pick a checkpoint-ring entry rather than going through
the per-attack-animation table.

Not yet identified: what `action_id` actually is (an attack/limit/spell
definition ID?), and the contents of the `D_800F9F3C` preset table itself
(what entry 1 vs 4 vs 22 etc. actually represents in-game).

## `unk8`/`unkA` (0x0C/0x0E) — polymorphic

Genuinely different per callback family, not a naming gap:

- **`func_800B8A34` producer / `func_800CE970`+`func_800CE7E0` consumer**:
  independent values. `unk8` = a snapshot of `D_801590CC` (a pervasive
  "current battler index" global used all over `battle2.c`); `unkA` =
  `D_800F99EE[action_id]` (same table family as `preset_idx`'s producer).
  Read back as a flags word here too — `func_800CE970` tests `unkA & 2`
  (unnamed bit, gates a `PadStop`-class draw-mode call).
- **`func_800D5444` producer / `func_800D5350` consumer**: one combined
  32-bit function pointer. `func_800D5350` (structurally a near-twin of
  `func_800D57C0`'s bit-scanner) reinterprets `delay` as a *target
  bitmask* (not a countdown) and `target_battler_idx` as a *per-bit repeat
  count* (not a target), while `state` is still the scan cursor. For each
  set bit found, it does `lw $v0, 0xC($s0)` (loads `unk8:unkA` as one
  word) then `jalr $v0` with `$a0` = the bit position and `$a1` =
  `preset_idx` — i.e. `((void(*)(int,int))slot)(bitPos, preset_idx)`. This
  is exactly the `arg3` callback `func_800D5444` stores via `PoolFnPtr32`
  (type updated from `void(*)(int)` to `void(*)(int, int)` to match — safe
  since a stored function pointer's declared type doesn't affect codegen
  at the storing call site, only at a call site, and the only call site is
  this one). `func_800D5444`'s actual callback argument (`arg3`) has no
  known caller in the retail binary (its only occurrence is its own
  definition), so the real callback function(s) it's used with are still
  open.

Not yet lifted: `func_800D5350` itself (logic is understood, exact GCC
register allocation is not — hand off to the permuter).

## The 6 status-effect callbacks — mechanics confirmed, not the classic ailments

These 6 callbacks are not bound to any of the classic status ailments. A
live correlator (auto-diffs `D_800F83E0[i].status` for newly-set bits,
cross-references against slot allocations bound to these 6 callbacks, same
battler, within a few frames) run across essentially the whole roster
(Poison, Regen, Resist, Sleep, Frog, Confuse, Silence, Death Force, Haste,
Slow, Stop, Barrier, MBarrier, Reflect, Mini, Berserk, Sadness, and enemy
Death) shows zero allocations for any of them via the classic
status-infliction paths (items, materia/spells).

The only two callers of the constructors (`func_800CE0C8`/`func_800CE21C`)
are `func_800CEB48` (492 instructions, runs at the end of every
`func_800CE970`/`func_800CE7E0` damage popup, i.e. only after
physical-attack-shaped events) and `func_800C7C4C` (5768 instructions, the
battle-script/model engine — see "`func_800D58D0`'s caller" below).
**Leading hypothesis, not yet tested:** this family may only trigger for
statuses inflicted via a physical attack (an enemy's innate attack effect,
or a weapon + Added-Effect-style materia combo), not via spells or items.

Mechanical findings (confirmed, independent of what real statuses these
are for):

`func_800C3578`/`func_800C4814`/`func_800C3CA8`/`func_800C328C`/`func_800C40F4`/
`func_800C44B4`, bound via `func_800CE0C8`/`func_800CE21C`'s switch on a small
"effect type" (0, 1/10, 2, 3, 7, 8, 17/18). All six share an almost identical
shape:

- Construction only sets `target_battler_idx` and `preset_idx`. Every one of
  the six self-initializes everything else via a `tick_count`-as-init-latch
  (0 → do setup, write `tick_count = 1`; any other value → no-op or steady
  state) — none of them use `tick_count` as a real counter, unlike
  `func_800D57C0`.
- `delay` is set to a small constant (14, 16, 22, 64...) on init and is a
  genuine countdown — but decremented by a *separate*, per-family shared
  "tail" helper (`func_800C33F0`, `func_800C3AA0`, `func_800C428C`,
  `func_800C45EC`, `func_800C3068`, `func_800C3F44` — one per sibling, not
  shared across all six), which frees the slot once it hits 0/1.
- All six read `preset_idx`'s low byte only, forwarding it as a gate flag to
  `func_800C2FD4` (an SFX-queuing helper — stages `D_8009A000/4/8` and calls
  `func_8002DA7C`), and all six read `target_battler_idx` to index the
  real per-battler working state: `BattleModel D_801518E4[]` (confirmed
  `sizeof == 0xB9C`), writing a fog/tint RGB triple (`SetFarColor` args,
  confirmed via `func_800BB030`) plus a blend-intensity field, then call
  `func_800B5FE8(battler)` to propagate a "dirty/flash" flag across the
  model's sub-parts.
- None of the six ever touch `damage_dealt` or anything past it (offsets
  0x0A-0x1E) — this whole family only uses the front 5 fields of the slot.

## `func_800D5230` idiomatic form

Rewritten to be fully idiomatic without breaking byte-match. Note on the
codebase's compiler sensitivity here, applicable elsewhere: byte-match
sensitivity in this GCC 2.6.3 target isn't "named-field access is unsafe"
as a blanket rule — it's specifically casting a *whole pointer* to a
*differently-shaped struct type* that can perturb address computation.
Taking the address of an *already-named field on the correct struct type*
and reinterpreting just that access's width (`*(s32*)&slot->field`) is a
different code shape to the compiler and is byte-safe. Worth trying on
other "kept as raw offset math" spots with this idiom.

Final form (byte-identical, no raw offset math):
```c
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
```

## The fixed-point ramp families (`D_800F10E0`/`D_800F10E4`)

Two cooperating single-instance 16.16-fixed-point ramps share the
`PoolAccum32` alt-view shape (`delay:preset_idx` as one s32 `accum`,
`unk8:unkA` as one s32 `countdown`; `target_battler_idx:damage_dealt` as
one s32 "step" only in the `D_800F10E4` family).

- **`D_800F10E0` family** (`func_800D5138` constructs, `func_800D508C`
  ticks, `func_800D51D4` re-derives): `accum += countdown` every tick
  (`countdown` here is really the fixed per-tick step, set once to
  `0x10000/N` and never decremented in this family); clamps `accum` to
  `0xFFFF` and stops. Every tick, reads the high byte of `accum`'s low
  16 bits (i.e. the fractional part scaled to 0-255) and passes it 3x to
  `func_800C4FC8` — a screen dim/fade-level (`DR_MODE`) setter. Frees the
  slot when `accum` drops to <=0. `func_800D51D4(arg0)` sets
  `countdown = -accum/arg0`, unwinding the ramp back toward 0 over `arg0`
  ticks — a "reverse" operation.
- **`D_800F10E4` family** (`func_800D52A0` constructs, `func_800D5230`
  ticks, already lifted): computes `step = ((target - D_800F5B74) << 16) /
  numSteps`, starts `accum` at the current `D_800F5B74` (cross-referencing
  the other family's output global), ticks `accum += step`, writes
  `D_800F5B74 = accum >> 16` every tick (the shared global other code
  reads as "the current fade/effect level"), decrements a literal
  `countdown` (ticks remaining), frees the slot at 0. The "smoothly
  retarget the shared level toward a new value" half of the pair.
- **`func_800D72B4`** (bound in the pool, never frees its own slot) doesn't
  touch its own `BattleCallbackSlot` fields at all — it's a permanent
  "tick me every frame" registration whose real job is driving a *different*
  pool (`D_801621F0[60]`, indexed by `D_801590D4`) through a 3-phase pulse:
  fire `func_800D5138(1)` (ramp to full in ~1 tick), idle one tick, fire
  `func_800D51D4(1)` (snap back to ~0 in ~1 tick) and free that
  `D_801621F0` item. Net effect: a very short, sharp dim/flash pulse (0→max
  →0 in ~3 frames) — a generic screen-flash utility (exact trigger/caller
  not found).

## `func_800BBEAC` — not byte-matched

Fully traced from raw asm:

- Scans `D_80161EF0[100]` (word/callback-pointer array) from index 0,
  skipping indices below `D_8015169C` (an unexplained floor — why does
  allocation never pick a low-numbered slot?), picks the first index `i`
  with `D_80161EF0[i] == 0`.
- Claims it: `D_80161EF0[i] = func` (the passed-in callback), `D_80162978[i].state
  = D_8015169C` (a placeholder callers usually immediately overwrite),
  increments a counter `D_80162080`, returns `i`.
- If all 100 are exhausted: fatal — `PadStop()`, `ResetGraph(1)`,
  `StopCallback()`, `SystemError(0x61, 1)`, and falls straight through to
  the epilogue with no loop-back.
- Best-effort hand-written C has correct control flow, argument/return
  type, and no spurious spills, but doesn't reproduce the exact register
  *letter* allocation (target uses `$t0`/`$t1`/`$a2`/`$a3`; the hand
  version lands on `$v0`/`$v1`/`$a1`) or one `lh` vs `lhu` instruction
  choice — fine-grained GCC 2.6.3 scheduling, hand off to the permuter
  rather than keep guessing. Current best-effort candidate sits in
  `/home/dmartin/bwork/func_800BBEAC/`.

## `func_800D57C0`

Initial (non-matching) decomp, in `src/battle/battle2.c`:

```c
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
```

`D_80062D98` is a global freeze/pause gate (already declared, `include/game.h`)
— the whole function no-ops while it's set. `preset_idx` is reused as a
stagger interval (only every `preset_idx`-th call past `delay` does
anything past the tick-count increment), `target_battler_idx` as a bitmask
of remaining targets, `state` as the scan cursor into that bitmask. Not
byte-matched — `func_800D4FA8` itself is still `INCLUDE_ASM` (only a
forward declaration, `s16* func_800D4FA8(s32 category)`, added here); what
it does with category `4`, and what the returned pointer's target actually
represents, is now fully resolved below.

## `func_800D4FA8`/`func_800D4FF0` — a 16-slot battle event queue, fully resolved

A generic single-producer-ish/single-consumer ring queue, distinct from
every other queue/ring already catalogued in this document (`D_80163798`,
`func_800A3E98`, `func_800A7254`'s ring, `func_800BBA84`'s cue queue,
`D_800F9F3C`'s checkpoint ring, `D_80081DC8`'s sound-message ring) — see
item 23 below, this is a new entry for that reconciliation question, not
one of the existing ones.

**Shape**, both halves short enough to read start to finish (24 and 46
raw instructions):
- `D_800F10D8` = write cursor, `D_800F10DC` = read cursor, both mod-16
  (`& 0xF`).
- `D_800F4CEC[16]` (`s32[16]`) — one word per slot, holds the `category`
  argument passed to the producer.
- `D_800F4D2C[16]` — one `0x14`-byte (20-byte) record per slot, payload
  shape defined by whichever category wrote it (categories confirmed to
  use halfwords at `+0x0`, `+0x4`, `+0x6`, `+0x8`, `+0x10`, and a word at
  `+0xC`, per the consumer below — no single struct fits all uses, same
  "polymorphic, leave unnamed" situation as `unk8`/`unkA` elsewhere in
  this doc).
- **Producer**: `s16* func_800D4FA8(s32 category)` — stores `category`
  at `D_800F4CEC[writeCursor]`, advances the write cursor, returns
  `&D_800F4D2C[writeCursor]` (the *pre-advance* slot) for the caller to
  fill in.
- **Consumer**: `s32 func_800D4FF0(void** out)` — if `readCursor !=
  writeCursor` (unread data exists): writes `&D_800F4D2C[readCursor]`
  into `*out`, advances the read cursor, returns
  `D_800F4CEC[readCursor]` (the category). If caught up: writes
  `&D_800F4D2C[(writeCursor-1)&0xF]` into `*out` anyway (the most
  recently written record, without consuming it) but returns `0` —
  i.e. `0` is the reserved "queue empty" sentinel category, not a
  valid producer value (matches: no producer below ever passes `0`).

**Confirmed producers** (all still `INCLUDE_ASM`):
- `func_800D57C0` — category `4`, payload = the target bit currently
  being processed (see the function body above: `*func_800D4FA8(4) =
  bit;`).
- `func_800D5774(battlerIdx)` (25 instr, new to this pass) — category
  `4` if bit `battlerIdx` of `D_800F836C` is set, else category `6`;
  payload = `battlerIdx` at record offset `0`.
- `func_800D5938` (the already-catalogued frame-table-sequencer job,
  "hand-traced" in the main inventory table) — category `0xA` (10);
  payload = the *value* read out of `D_801517F0[cursor]` at offset `0`
  and the pre-increment cursor index itself at offset `0x10`. Confirms
  `func_800D5938`'s real job: it walks `D_801517F0` (the same 78-entry
  "frame table" `func_800B888C` searches, see the `preset_idx` section
  above) sequentially via a fixed-point ramp identical in shape to the
  `D_800F10E0`/`D_800F10E4` ramp families (`delay`/`target_battler_idx`/
  `preset_idx` reused as ramp numerator/denominator), advancing its own
  cursor (`state`) one table entry at a time and queuing category `10`
  once a step's timing resolves — i.e. `D_801517F0` holds a **scripted
  sequence of per-step payloads** (most plausibly battler indices for a
  staggered multi-hit attack — 2x-Cut/4x-Cut/Slash-All are the obvious
  candidates given they're the multi-target-in-one-command family
  documented above) consumed one entry per timed step, not a static
  lookup table. `func_800B888C`'s own job (find which index currently
  holds a given battler) makes much more sense read this way: "which
  step of the in-progress scripted sequence corresponds to this
  battler."

**Consumer / per-frame dispatcher**: `func_800B8B48` (225 instr, called
once per frame from the main battle-phase switch in `battle1.c` — the
same place `func_800B83C4`/`func_800BC440` etc. run, see "Battle loop
architecture" above), fully read. Drains the queue in a tight loop
(`while ((category = func_800D4FF0(&rec)) != 0) { ... }`) and dispatches
`category - 1` through a 10-entry jump table (`jtbl_800A0634`, categories
1-10; only `1,2,4,5,6,7,9,10` do real work — `3` and `8` are reserved/
no-op, just loop again):
- **1** — reads 3 halfwords from the record (offsets `4`/`6`/`8`) into
  `D_80151778[0..2]`, stages the record's offset-`0x10` halfword into
  `D_800FAFE8`, then **constructs a `func_800BBEAC` pool job bound to
  `func_800C7340`** — this is the exact registration this document's
  "Animation-script VM caller chain" section flagged as "registered as
  an async `D_80162978`-style callback... during disk-streaming/load,
  not a per-frame `jal`" with no identified trigger; category 1 of this
  queue *is* that trigger. Also clears `D_800F8364`.
- **2** — sets `D_800F8364 = 0xFF` (pairs with category 1's clear —
  looks like a "streaming busy" flag toggle).
- **4, 5** (share one handler) — reads the record's offset-`0` halfword
  as `battlerIdx`, clears `D_800F8378`, calls **`func_800B88CC(battlerIdx)`**
  (the already-fully-understood damage-popup job constructor — see the
  callback family inventory at the top of this document). Also ORs bit
  `0x20` into `D_801518E4[battlerIdx].unk3E` (confirmed via the exact
  `BattleModel` stride, `0xB9C`/2972, computed the long way in the raw
  asm) and, using `func_800B888C(battlerIdx)`'s frame-table-index result,
  copies a word from `D_80163CC4[idx]` into `D_80151200[recordSlot]`
  (both arrays' element meaning not decoded this pass).
- **6** — reads `battlerIdx` the same way, calls `func_800D6814(battlerIdx)`
  (not yet examined), sets a *different* flag bit (`0x1` at
  `D_801518E4[battlerIdx].unk26`, not `.unk3E`).
- **7** — reads a word at record offset `0xC` and a halfword at `0x10`,
  indexes two parallel tables (`D_800E8C90`/`D_800E8C94`, stride 8) by
  that halfword, and calls **`DS_read`** directly — a real disk-streaming
  read, i.e. category 7 is a genuine "load this chunk" request riding the
  same queue as the popup-trigger categories.
- **9** — reads `battlerIdx`, clears `D_800F8378`, calls
  `func_800B88CC(battlerIdx)` with no extra bit-setting (the "plain"
  variant of 4/5).
- **10** — reads the record's offset-`0` halfword as `battlerIdx` and
  offset-`0x10` halfword as a second arg, calls **`func_800B8944(battlerIdx,
  arg)`** (already documented elsewhere in this file: constructs a
  `func_800BBEAC`+`func_800B8A34` job, sets/clears `D_8015120C` bit
  `0x10` based on `D_80163CC2[actionId] & 0x4`) — the consumer half of
  `func_800D5938`'s sequenced-payload production above. Also does the
  same `D_80163CC4`→`D_80151200` copy as categories 4/5, plus increments
  a small counter (`D_80163B38`).

**Net effect, item 13 from "Where to look next" closed**: `func_800D57C0`'s
per-bit dispatch (a bitmask of pending targets) doesn't call anything
directly — it stages a category-`4` queue entry per target bit, and this
whole system (drained once per frame, same cadence as the `D_80162978`
pool's own `func_800BC348` dispatcher) is what actually turns each staged
bit into a real damage-popup job via `func_800B88CC`. `D_800F836C` (a
per-battler bitmask, new to this pass) selects between the "popup"
(category 4→`func_800B88CC`) and "other reaction" (category 6→
`func_800D6814`) paths for `func_800D5774`'s callers — role of that
distinction not decoded.

## Death-sequence trigger

`func_800CE21C` (already matched) is a small dispatcher: given a
`battler_idx`, it reads `D_801636B8[idx].D_801636BC` and constructs one of
several effect-callback slots depending on its value — case `0` constructs
`doDeathSequence`. It's generic effect-triggering plumbing, not itself a
death-specific function.

`func_800CE21C` has exactly 2 callers in the whole `asm/us` tree:
`func_800CEB48` (the `func_800CE970` popup family's finalize/free step) and
one excerpt inside `func_800C7C4C` (around label `.L800CAECC`-`.L800CAF2C`).
Both gate the call with the same check:
```c
s32 flag = (D_800F8378 == 0) ? ((D_80161DE8 >> battlerIdx) & 1)
                              : (D_80151200[battlerIdx].D_8015120C & 0x10);
if (flag != 0) func_800CE21C(battlerIdx, 1);
```
In `func_800CEB48` this sits behind an extra precondition,
`D_801518E4[targetIdx].D_801518E8 == 0x33` (an animation/state sentinel,
not a raw HP compare), and `attackerIdx != targetIdx`. In the
`func_800C7C4C` excerpt it's its own dispatch case with no such guard.

None of this chain checks HP directly — every gate involved traces back to
static or per-action-type config data, not a live "just died" write:

- `D_80161DE8`'s only writer is `func_800B7DB4`, a one-time battle-setup
  function building static per-battler classification bitmasks from
  formation data (`D_80163CC0`, stride 8) — not a runtime HP check.
- `D_801518E8`'s only writer is `func_800B79F0`, which copies a byte from
  `D_800FA9D2[i]`, itself seeded once at formation load (`func_800BBB20`).
  Static, not live.
- `D_8015120C` bit `0x10`'s writer is `func_800B8944`: a small helper
  (calls `func_800BBEAC`+`func_800B8A34`, a sibling job constructor to
  `func_800B88CC`) that sets/clears the bit based on
  `D_80163CC2[actionId] & 0x4` — a static per-ability config table.
- The switch selector `D_801636B8[battler].D_801636BC` (u8, offset 0x4 of
  a 0x10-byte-stride 5-slot struct) has exactly 4 writers in the whole
  tree: `func_800A2DB0` (writes 7), `func_800ABB0C` (writes 8, gated on
  last-command == 0x1A/26), `func_800AEF68` and `func_800B54B8` (both copy
  a byte straight from an attack/effect-definition table,
  `D_800F5BB8[i]+0x2B` / `D_801518E4[i]+0x27 & 0x3F`). None of the 4 ever
  write a literal 0 — effect-type 0 (death) is the field's ambient/default
  value for a plain attack with no special inherent effect, not an active
  "you died" write. `func_800CEB48`'s whole gate really tests "is this an
  eligible target+action-type," and case-0 (death) falls out of it
  opportunistically when nothing else is going on.

The actual HP-reaches-zero detector, found by searching for whoever
subtracts damage from live HP and reacts to the zero-crossing:
**`func_800AFECC`** (now an initial, non-matching decomp in
`src/battle/battle.c`). Per-tick, loops over all 10 combatants, running 3
parallel fixed-point accumulators (`D_800F4A48`/`D_800F4A70`/`D_800F4A98`,
one word per combatant each) fed by 2 shared "rate" rows
(`D_800F49F8`/`D_800F49A8`, row selected by a `D_800F499C` toggle that
flips every call) — accumulator 3's fractional carry is the actual HP
delta:
```c
if (!(D_800F83E0[i].status & 1)) {           // skip if already dead
    wholeC = accum3 >> 15;                    // this combatant's HP delta
    if (D_800F83E0[i].curHP + wholeC < 0) {
        D_800F83E0[i].status |= 1;             // Death bit -- the actual set
        func_800A6278(i, i, 1);
        func_800A61D4();
        D_800F83E0[i].curHP = 0;
    } else if (D_800F83E0[i].curHP + wholeC > D_800F83E0[i].maxHP) {
        D_800F83E0[i].curHP = D_800F83E0[i].maxHP;
    } else {
        D_800F83E0[i].curHP += wholeC;
    }
}
```
The function also has a second, unrelated tail loop (16 per-combatant
counter bytes, reusing `D_800F5BB8[i]`'s `unk10`-`unk1C` words as a byte
array), decremented by one of 3 fixed-point "whole part" values depending
on counter index, calling `func_800A7254(2, i, j, j)` when a counter
reaches 0 — structurally preserved from the asm, exact game meaning
unclear.

**Open caveat**: the fixed-point accumulator shape is characteristic of a
periodic DoT/regen tick (Poison, Regen-style materia/status), not a
discrete weapon-hit's instant damage. A single 6707-damage physical attack
was observed live to trigger the death sequence, so a sibling mechanism
that applies instant attack/spell damage the same way
(`newHP<0 -> status|=1, func_800A6278(i,i,1), func_800A61D4()`) is expected
to exist but is not yet found.

`func_800A6278`/`func_800A61D4`'s complete caller lists have been checked
and do not lead to it:
- `func_800A6278`: only 2 callers total — `func_800AFECC` and
  `func_800A1798`, but that second call passes a different third arg
  (`func_800A6278(s2, s0, 0)`, inside an unrelated 32-status-bit notify
  loop).
- `func_800A61D4`: only 3 callers — `func_800AFECC`, plus `func_800A23E0`
  and `func_801B0050` (batini/magic-barrier). `func_800A23E0` is a
  "process one queued command against its targets" driver — loops
  targets, calls `func_800A1798` (a generic per-command opcode dispatcher:
  reads `D_80063014->cmdIndex`, looks up an opcode-byte sequence in
  `D_800A0098`, then `jalr`s through `D_800E7B28[opcode]` for each byte),
  then calls `func_800A61D4()` once per target as a generic finalize.
  Never touches `curHP`.

`func_800A1798`'s 5 direct helper calls (the 2 others, `func_800A3E98`/
`func_800B0FFC`, were already decompiled/matched) are now decompiled
(initial, non-matching, in `battle.c`): `func_800A2894`, `func_800A2BF4`,
`func_800A304C`, `func_800A5BC8`, `func_800A6000` — all message-queue/
reaction bookkeeping that brackets the opcode dispatch, not the dispatch
itself:

- `func_800A2894`: the "7777 curHP" easter-egg announcement (party only).
- `func_800A2BF4`: looks up whether the current action's `actionIndex`
  exists in a per-actor effect-config table (`D_8009D954`), stores the
  matching row into `D_80063014->unk14`.
- `func_800A304C`: dedupes a small queued-message list so only the newest
  flagged entry per battler survives.
- `func_800A5BC8`: counter-attack/reaction-ability scanner (8
  probability-gated reaction slots per battler, `D_8009D870`) — the
  counter-attack priority band mechanic noted above.
- `func_800A6000`: generic "snapshot two `D_800F83E0` byte fields across
  all 10 combatants, diff after some work, announce what changed" utility.

None of these touch `curHP` or the Death bit.

## `D_800E7B28` — the battle-command opcode table, ruled out as the damage source

`D_800E7B28` (31 entries, `main/data/47A38.data.s`) is `func_800A1798`'s
per-opcode function-pointer table, indexed directly by the raw opcode byte
(0-0x1E, no offset subtraction — confirmed from the dispatch code itself).
All 31 handlers checked for a `D_800F83E0` `curHP` (offset `0x2C`) write —
none write it; the only 2 that touch it at all (`func_800A866C`,
`func_800A7560`) only read it for a Near-Death-style comparison. This
opcode range is confirmed to be status/condition checks, message/reaction
queuing, and animation waits — the real damage-application code is not
here.

One entry resolved a real mislabel along the way: opcode `0x14`
(`func_800A4AF4`) spins on `func_800B6D6C()` (a generic drainer for the
`D_80163798` event queue) until a status bit clears. `func_800B6D6C`'s
dispatch case 4 constructs a `D_80162978` job bound to **`func_800C5694`**
(a callback not previously in the inventory) — this tweens
`D_801518E4[target].D_801518EA` from its current value toward a target
value stored in the triggering `D_80163798` entry, one step per tick
(`preset_idx` repurposed as the target battler index, `tick_count`
repurposed as the per-tick increment, computed via integer division of
the value delta by the tick count). `D_801518EA` had previously been
described (in an existing code comment) as an "HP-counter" — that's
wrong: it's a `0x1000`-scale fixed-point factor, confirmed elsewhere
(`battle2.c`) multiplied directly against `D_80151A4C.vy` (a
position/velocity component), i.e. a scale/blend factor for a physics or
render effect, not HP. Comments in `battle.c`/`battle1.c` corrected to
match.

Also add `func_800C5694` / `func_800B6D6C` case 4 to the callback family
inventory: a 15th `func_800BBEAC` construction site, distinct from the
14 already listed (constructed from inside `func_800B6D6C`'s dispatch
rather than being its own top-level caller).

**Sharper next leads**:

1. **`ActorBattleData->targetDamage`** (confirmed field, `include/game.h`)
   — where it's actually applied to real `curHP` is still unfound.
   `D_80063014` is a *pointer*, not an array, so field accesses compile to
   `lw` the pointer into a register first, then offset off that register
   — they never show up as `D_80063014 + <offset>` in the disassembly the
   way `D_800F83E0 + 0x2C` does, so a symbol-based grep doesn't find them.
   Locating the write requires either reading candidate functions in full
   (the damage-formula family starting around `func_800AD8DC` in
   `battle.c` — `func_800ADE5C`, `func_800AE234`, and neighbors compute
   `targetDamage` but don't touch `curHP` in the same function) or a
   smarter search for the specific register-relative offset pattern.
2. **`func_800A9DA0`** — a large (283-line) function that reads/writes a
   long list of `D_800F83E0` fields including `curHP`/`maxHP`, structured
   as "snapshot many fields, do work, write them all back." Not yet
   understood — could be an aggregate/average (near-death % display)
   rather than a real per-combatant update, but it's the only other
   `curHP`-writer found in a full grep of every `D_800F83E0 + 0x2C`
   reference (16 files total) that isn't already explained (full-heal,
   zero-out, checkpoint-restore, or the confirmed `func_800AFECC` DoT
   tick).

## `func_800D58D0`'s caller: a separate bytecode VM

`func_800D58D0` has exactly one caller in the whole tree: `func_800C7C4C`
(6021 lines).

`func_800C7C4C` is its own, completely separate bytecode interpreter from
`func_800A1798`'s battle-command dispatch — a per-battler *model/
animation-script* VM:

- Program counter: `D_801518E4[battler].unk3C` (a byte index, incremented
  each step).
- Script buffer: selected once per invocation via
  `scriptTable[D_801518E4[battler].unk2]` (`scriptTable` = the function's
  4th argument — an array of script pointers, `unk2` presumably a
  per-battler "current animation state" ID).
- Opcode dispatch: fetch the byte at `script[pc]`, subtract `0x8E`; if the
  result is `< 0x72` (114), `jr` through `jtbl_800A08C0` (114 entries,
  covering raw opcode bytes `0x8E`-`0xFF`). Bytes below `0x8E` are handled
  by an earlier, smaller table (`jtbl_800A0888`, ~14 entries, gated by a
  different field, `D_801518E4[battler].unk3E` bit 0) — not traced
  further.

`func_800D58D0` is reached by exactly 2 opcodes in that table:

- **Opcode `0xF7`** (`.L800CACA4`, table index `0x69`) — direct,
  unconditional. Reads the queued action's delay straight from the
  `D_800F99F4` config-table family and constructs the windup-timer job.
  Almost certainly what fires for a plain physical attack.
- **Opcode `0xBE`** (`.L800CADE0`, table index `0x30`) — same
  construction, but skipped if `D_801636B8[?].D_801636B8 == 2 && a0 == 8`
  and a per-formation flag bit (`D_800FA6D4`-indexed, bit `0x800`) is set
  — reads like an exemption for a specific scripted-event/boss case, not
  otherwise identified.

Index `0x13` (opcode `0xA1`) in the same table calls `func_800D5A68` (the
`func_800D5938` frame-table sequencer's constructor) — this opcode range
is generally "construct a D_80162978 pool job," several opcodes each
binding a different callback.

The script-select index is `D_801518E6` (already-named field, offset 0x2 of
`BattleModel`) — see "Battle loop architecture" below for its writers and
for `func_800C7C4C`'s own caller chain.

## Animation-script VM: the full opcode catalog

All 114 entries of `jtbl_800A08C0` (opcodes `0x8E`-`0xFF`) triaged — one-line
role each, confidence noted only where it's a guess rather than read
directly from the body. Boilerplate (script-PC fetch/advance,
per-battler struct-offset math) is omitted from every description below.

**What this reveals, structurally:**
- **Real control-flow constructs exist in the bytecode**, not just a flat
  opcode sequence: opcodes `0xB2`/`0xC9`/`0xCD` are block-delimiter marker
  bytes; opcodes `0xB3`/`0xC1`/`0xCA`/`0xCE` are conditional/unconditional
  "scan forward until marker byte X" skip-blocks (an if/goto primitive);
  opcodes `0x9E`/`0xEB`/`0xEC`/`0xF1`/`0xF3` rewind the script PC instead of
  advancing it, via alternate re-entry tails (`.L800CD3CC`/`.L800CD204`
  alongside the normal `.L800CD208`) — a repeat-N-times / wait-until-flag
  loop primitive.
- **The job-construction family is much bigger than previously catalogued.**
  Beyond the ~15 `D_80162978` callback families already in the main
  inventory table, this VM directly constructs jobs bound to at least a
  dozen more callbacks (`func_800CDFC4`, `func_800CF3CC`, `func_800CF4A8`,
  `func_800CF5BC`, `func_800CFCB0`, `func_800CF2F0`, `func_800CFE60`/
  `func_800D01C0`, `func_800D06B8`, `func_800D0578`, `func_800D061C`,
  `func_800CDDE8`, `func_800D0760`, `func_800CE7E0`, `func_800C3950` via
  `D_801620AC`) across all three generic pools (`D_80162978`/`D_801620AC`/
  `D_801621F0`) — none of these individual callbacks have been traced yet.
  Opcode `0xC2` in particular constructs `func_800CE7E0` (the damage-popup
  twin) seeded straight from the `D_800F99EC/EE/F4/F6` config tables — a
  **script-driven equivalent of the `func_800B88CC` construction path**
  already fully understood.
- **A model-swap / death-model family**: opcode `0xA2` loads alternate
  model-swap files; opcodes `0xEE`/`0xFF` (same handler, two opcode bytes)
  switch/reload the battler's model variant via `func_800CDD44`; opcode
  `0xFE` is the reset/teardown counterpart. Plausible candidate for how a
  battler's model actually becomes "corpse-shaped" independent of
  `doDeathSequence`'s own tint/fade — not confirmed, worth live-testing.
- **A large movement/facing family**: step forward/backward along current
  facing (`0xED`/`0xE4`), rotate/offset by a script angle (`0x9A`/`0xFB`,
  same handler), orbit an anchor with trig (`0xAB`), snap to a formation
  slot (`0xA6`), randomize a position offset (`0xFD`), plus several large,
  heavily-branching camera/target-relative positioning opcodes (`0x95`,
  `0x99`, `0xE5`, `0xFC`) that read camera-mode and formation globals
  (`D_800FA6D0`, `D_801031F0`) to decide how to aim.
- **GTE/scratchpad manipulation**: opcodes `0xAF`, `0xCB`, `0xCF`, `0xD5`
  write directly into PS1 scratchpad addresses (`0x1F800000` range),
  assembling what looks like GTE transform packets before calling helper
  functions (`func_800D61AC`, `func_800CD860`) — likely how this VM does
  its own 3D positioning math, distinct from the popup-projection GTE use
  in `func_800C2928`.

| Op | Role |
|---|---|
| `0x8E` | One-shot latch: sets `D_801620A4=1`/`D_801517C4=1` (guards a following effect to fire once) |
| `0x8F` | Clears the latch (pairs with `0x8E`) |
| `0x90` | Multi-branch pan/target-offset selector (mode byte 1/6/7/8/9), writes `D_800F8E74[battler]` |
| `0x91` | Constructs a `func_800CDFC4` job (`D_801621F0`, 1 param) |
| `0x92` | Sets `D_800FAFDC = 1` |
| `0x93` | Fire-and-forget `func_800C494C` job, no params |
| `0x94` | Movement job: computes start/end/rate from current+target coords over a duration, constructs a `func_800CF3CC` job (`D_801620AC`) — "move over N frames" |
| `0x95` | Large targeting/facing routine — branches on camera mode + formation checks, sets facing and state flags |
| `0x96` | Fetches a pose/animation id from `D_800EEA80`, calls `func_800D7A88` — "play/queue a pose" |
| `0x97` | Generic 3-arg scripted call `func_800CE0C8(p1,p2,p3)` |
| `0x98` | Constructs a `func_800D0760` job (`func_800BBEAC`), seeded by `func_800C60F4` |
| `0x99` | Large — sub-dispatches via its own table `jtbl_800A0AA0` on camera mode, builds `func_800CF5BC`/`func_800CF4A8` movement jobs relative to camera/target |
| `0x9A` / `0xFB` | Same handler: computes a facing angle (`ratan2` vs. the camera-focus battler) via `rsin`/`rcos`, derives a velocity vector, offsets position — "orbit/move toward camera-focus target" |
| `0x9B` | Sets `D_80162090 = 1` |
| `0x9C` | Calls `func_800BB90C()` |
| `0x9D` | Sub-dispatch via its own table `jtbl_800A0AC8` (7-way); one case triggers `func_801B00B0` (message/log/portrait) |
| `0x9E` | Conditional retry: rewinds the script PC by one byte until the camera is focused on the target, else clears a flag bit |
| `0x9F` | Clears `D_80162090 = 0` (pairs with `0x9B`) |
| `0xA0` | Resets/anchors movement state via `func_800BB67C`, zeroes velocity fields, sets a default rate |
| `0xA1` | **Known.** Constructs `func_800D5A68` (`func_800D5938` frame-table sequencer) |
| `0xA2` | Loads alternate model-swap files (`func_800B5E64`/`func_800B5C1C`), rewrites model-swap fields — candidate "swap to death/alternate model" |
| `0xA3` | Standard sound-play: stages `D_8009A000/4/8`, calls `func_8002DA7C` |
| `0xA4` | Constructs a `func_800CDDE8` job, fixed params `(2, 0x1F0)` — "trigger fixed preset" |
| `0xA5` | Same construction, fixed params `(3, 0x1E9)` — sibling preset |
| `0xA6` | Snaps position to the formation-slot table `D_80163C80` — "teleport to formation slot" |
| `0xA7` | Resets timing fields, loops `func_800CD400` once per active ally — "resync sub-effects" |
| `0xA8` | Generic 2-arg scripted call `func_800CF8C0(p1,p2)` |
| `0xA9` | Resets timing, increments global counter `D_800FAFD8` mod 16, loops `func_800CD400` per ally — "restart/resync all ally sub-scripts" |
| `0xAA` | Clears `D_801590DC = 0` |
| `0xAB` | Trig-based orbit/anchor reposition (`rsin`/`rcos` + `D_80163C80` lookup) |
| `0xAC` | Model-type/animation dispatch based on a per-battler byte range comparison |
| `0xAD` | 3-sequential-byte param setup feeding an out-of-range continuation — likely spline/particle setup |
| `0xAE` | Resets positional/velocity state, starts a job via `func_800BB67C` ("clear" variant of `0xAF`) |
| `0xAF` | Same as `0xAE` but the "set" variant; writes directly into PSX scratchpad `0x1F800000` |
| `0xB0` | Step perpendicular to current facing (+quarter turn), shared trig code with `0xB1` |
| `0xB1` | Step forward along current facing (shares trig code with `0xB0`) |
| `0xB2` | Marker byte / trivial — this label *is* the shared interpreter tail |
| `0xB3` | Conditional skip-to-marker: if a flag bit is set, scans forward for opcode byte `0xB2` |
| `0xB4` / `0xBA` | Combined: `0xBA` sets facing (+ a delta), falls through into `0xB4`'s game-mode dispatch (branches on `D_800FA6D0`) |
| `0xB5` | Multi-arg effect setup calling `func_800D415C` (7 args total) |
| `0xB6` | Registers a callback, `func_800CD82C`, via `func_800BC04C` |
| `0xB7` | Shares a tail with `0xF6`: unconditionally calls `func_800CE21C(1)` (forces the death-effect dispatch, no guard) |
| `0xB8` | Sets flag bit `0x04` on a per-battler status byte |
| `0xB9` | Starts a texture/palette-cache-style effect keyed by a script byte (`D_80163F30`, `func_800BB684`) |
| `0xBB` | Trivial — shared alternate tail `.L800CD188` |
| `0xBC` | Stashes a script byte into global `D_80163B3C` |
| `0xBD` | Sets `D_80162090 = 1`, calls `func_800CD5E4` — possible sound/effect variant |
| `0xBE` | **Known.** Gated construction of `func_800D58D0` (windup timer) |
| `0xBF` | Two-byte table lookup (`D_80166F68`), calls `func_800CDDA4` |
| `0xC0` | Trivial — shared alternate tail |
| `0xC1` | Unconditional skip-to-marker: scans forward for opcode byte `0xC9` |
| `0xC2` | Constructs a `func_800CE7E0` job (`func_800BBEAC`), populated from the `D_800F99EC/EE/F4/F6` config tables, calls `func_800B888C` — script-driven equivalent of the already-understood `func_800B88CC` popup-construction path |
| `0xC3` | Constructs a `func_800C3950` job via `func_800BBF7C` into `D_801620AC` |
| `0xC4` | One-shot gated call `func_800BBA84(0x185, -1, 0x40)`, latched by `D_800EEB24` |
| `0xC5` | Copies global `D_800F8374` into a per-battler field |
| `0xC6` | Writes a script arg into `D_800F8374` (get/set pair with `0xC5`) |
| `0xC7` | Sets model part-index fields, gated by per-part flags and global comparisons |
| `0xC8` | Constructs a `func_800CF4A8` job via `func_800BBF7C` (2 args) |
| `0xC9` | Marker byte / trivial |
| `0xCA` | Conditional skip-to-marker, gated on `D_801518DC`, scans for byte `0xC9` |
| `0xCB` | Builds a GTE-style vector/matrix packet directly in PSX scratchpad (`0x1F800000`-`0x18`), calls `func_800D61AC` |
| `0xCC` | Repositions toward target (`func_800D54EC`), spawns a `func_800CF5BC` job via `func_800CD860` |
| `0xCD` | Marker byte / trivial |
| `0xCE` | Conditional skip-to-marker, gated on battler id `< 4`, scans for byte `0xCD` |
| `0xCF` | Same scratchpad-vector pattern as `0xCB`, spawns via `func_800CD860` with position data |
| `0xD0` | Quantized-param spawn, aimed at self or facing-target depending on `D_801031F0` (`func_800CFCB0` job) |
| `0xD1` | Spawn positioned effect, self vs. target-facing mode (`func_800CF5BC` job) |
| `0xD2` | Trivial — shared alternate tail |
| `0xD3` | Trivial — shared alternate tail |
| `0xD4` | Division-based interpolation-step, constructs a `func_800CF2F0` job via `func_800BBF7C` |
| `0xD5` | Same scratchpad-vector pattern as `0xCB`/`0xCF`, selects callback `func_800CFE60` vs `func_800D01C0` |
| `0xD6` | Constructs a `func_800D06B8` job via `func_800BC04C` (1 arg, duplicated) |
| `0xD7` | Constructs a `func_800D0578` job via `func_800BC04C` (2 args, fixed-point scaled) |
| `0xD8` | Constructs a `func_800D061C` job via `func_800BC04C` (1 arg) |
| `0xD9` | Trivial — shared alternate tail |
| `0xDA` | Sets a state-type/value pair on per-battler fields |
| `0xDB` | Table-driven per-battler effect dispatch (`D_800FA6D8+8` lookup), calls `func_800D7A88` |
| `0xDC` | Scaled-parameter write (speed field × arg, shifted) |
| `0xDD` | Calls `func_800D7BA4(activeBattler, part, argA, argB)` |
| `0xDE` | Calls `func_800D7B1C` (sibling of `0xDD`) |
| `0xDF` | "Face target": `ratan2` toward `func_800D54EC`'s target result, stored as facing |
| `0xE0` | Constructs a `func_800CDDE8` job, fixed constants `(1, 0x24)` — sibling preset of `0xA4`/`0xA5`/`0xE6` |
| `0xE1` | Sets flag bit `0x4` on a per-part flags field |
| `0xE2` | Sets flag bit `0x2` on the same flags field (sibling of `0xE1`) |
| `0xE3` | Snapshots the active battler's position into global `D_80163C80[]` |
| `0xE4` | Move opposite current facing (`rsin`/`rcos`) |
| `0xE5` | Large target/formation-relative positioning dispatcher, own nested table `jtbl_800A0A88` |
| `0xE6` | Constructs a `func_800CDDE8` job, fixed constants `(0xD, 0)` — sibling preset |
| `0xE7` | Toggles a flag bit based on a script byte (nonzero = set, zero = clear) |
| `0xE8` | Calls `func_800D1110(battler)` |
| `0xE9` | Draws 2 pseudo-random values (`func_800CD558`), records last-spawned actor id, table-driven cue via `func_800CD860` |
| `0xEA` | Constructs a `func_800D0760` job (`func_800BBEAC`) seeded by `func_800C60F4` — same construction shape as `0x98` |
| `0xEB` | Gated: if `D_801518DC==0` calls `func_800D0B4C` directly, else loops (repeat-tail) until it clears |
| `0xEC` | Same gated pattern as `0xEB`, target `func_800D0C80` |
| `0xED` | Move forward in current facing (mirror of `0xE4`) |
| `0xEE` / `0xFF` | Same handler: clears status bits, sets a "moving" flag, switches/reloads the battler's model variant via `func_800CDD44` |
| `0xEF` | Cleanup op (clears timing fields), falls through to tail |
| `0xF0` | Calls `func_800D3E8C(battler)` |
| `0xF1` | Begins "move" state (clears flags, sets moving flag), then loops (repeat-tail) until the movement flag clears |
| `0xF2` | True no-op — this label *is* the main interpreter tail itself |
| `0xF3` | "Repeat N times": decrements a counter, loops (repeat-tail) until it hits zero |
| `0xF4` | Sets the repeat count consumed by `0xF3` (script byte → counter field) |
| `0xF5` | Arms a countdown/timer (writes a distance-table value to hardware register `0x1F80000C`), calls `func_800B54B8` |
| `0xF6` | **Known.** The `func_800CE21C` death-effect dispatch (gated) |
| `0xF7` | **Known.** Direct construction of `func_800D58D0` (windup timer) |
| `0xF8` | Loads a model via `func_800CE384`, distance-table lookups, stages camera/render struct fields |
| `0xF9` | Copies current-facing field into the "commanded facing" field |
| `0xFA` | Calls `func_800CD798(param)` |
| `0xFC` | Complex, multi-branch: computes facing via `ratan2` toward a camera-focus/paired battler, OR loops setting/clearing a "targetable" bit across a battler range, depending on mode flags |
| `0xFD` | Randomizes a target position offset (3× `func_800CD558`, x/y/z) |
| `0xFE` | Resets/tears down model-construction state (counterpart to `0xEE`/`0xFF`'s swap), falls to the same model-table-lookup tail |

**Priority follow-ups**, ranked by how much they'd likely unlock: the
model-swap family (`0xA2`/`0xEE`/`0xFF`/`0xFE`) for the death-sequence
connection; `0xC2` (script-driven `func_800CE7E0` construction, mirrors the
already-solved `func_800B88CC` path); the GTE-scratchpad opcodes
(`0xCB`/`0xCF`/`0xD5`); the large camera/targeting opcodes (`0x95`, `0x99`,
`0xE5`, `0xFC`); and the still-uncatalogued callback functions themselves
(`func_800CDFC4`, `func_800CF3CC`, `func_800CF5BC`, etc.) — all still raw
asm, none traced.

## Battle loop architecture

The battle overlay's per-frame entry point (invoked from the main
executable's overlay dispatcher, outside this disassembled tree — the same
slot `field`'s own per-frame function `func_800D8710` occupies for that
overlay) is `func_800D8D78` (`battle3.c`, still `INCLUDE_ASM`), which calls
`func_800A38FC` once per frame (guarded by the message-box/pause flag
`D_80062D99`).

**`func_800A38FC`** is now an initial (non-matching) decomp in
`src/battle/battle.c`, replacing its `INCLUDE_ASM`. It has 4 distinct
pieces:

1. **ATB-pause timer** (moderate confidence on intent, high on structure).
   Reads like the community-documented "time pauses briefly after another
   action is queued or a party member reaches full ATB, duration depending
   on battle speed" mechanic — `D_800F5F44.D_800F7DC8` (values seen: 1, 3)
   reads like a battle-speed/ATB-mode setting, `D_800F7DD0` a speed
   threshold, feeding an accumulate-and-wrap counter pair
   (`D_800F5F44.D_800F7DB0`/`D_800F7DB4`) that decrements a third counter
   (`D_800F5F44.D_800F7DAE`) when a ratio condition holds. Not confirmed
   field-by-field.
2. **The ATB gauge scan** (high confidence, this was the reason for
   decompiling it). Loops all 10 combatant slots of `D_800F5BB8[]` (stride
   `0x44`):
   ```c
   for (i = 0; i < 10; i++) {
       if (!slotActive(i)) continue;
       s32 gauge = D_800F5BB8[i].unk4;         // ATB accumulator
       if (gauge == 0xFFFF) continue;           // already ready, untouched
       s32 rate = D_800F5BB8[i].unk2;            // per-frame fill rate
       if (*(s16*)&D_800F5BB8[i].unk8 & 1) rate <<= 2;  // rate-doubling flag
       if (perBattlerFreezeBit(i)) rate = 0;              // freeze flag
       gauge += rate;
       if (gauge > 0xFFFF) gauge = 0xFFFF;                 // clamp = ready
       D_800F5BB8[i].unk4 = gauge;
       if (gauge == 0xFFFF) {
           if (i >= 3 || (D_800F83E0[i].unk4 & 0x10)) {
               func_800A4350(i, -1, 0, 0);          // queue CMD_NONE
           } else if (perBattlerChosenBit(i)) {
               func_800A4350(i, func_800A4CA8(i), 0, 0); // player's chosen command
           } else {
               func_800A4D2C(i);   // push into the pending-menu queue D_800E7A48 instead
           }
       }
   }
   ```
   The three per-battler bitmasks (freeze / "command already chosen" /
   "pushed to AI queue") live at fixed offsets past `D_800F5BB8[]`'s real
   `10*0x44` array bounds (`+0x2212`/`+0x220C`/`+0x21F4`) — separately
   aliased globals reusing the same base register, not real struct fields.
   `unk2`/`unk4`/`unk8` on `Unk800AF470` are not yet renamed to reflect
   this.
3. **Limit-gauge fill** (high confidence — the involved struct fields,
   `Unk800F5E60.limitBar`/`.limitBarUI`, are *already named* in this
   codebase, independently confirming the mechanic). Party only, gated on
   `D_800F83D0 & 8`: computes a per-tick increment from `limitBarUI` and a
   fixed rate divisor, adds it to a running gauge value (the upper 16 bits
   of `D_8009D84C[i].unk18` — a wider field split into two halves, only
   confirmed via this access), clamped to `0xFF`; fires a message
   (`func_800A4E80`/`func_800A7254(1, i, 1, 0)`) when the gauge lands
   exactly on its clamp target. The exact tail condition for firing that
   message is transcribed structurally, not fully narrated.
4. **ATB display snapshot** (moderate confidence): copies each party
   member's `unk4` gauge into a separate mirror array (`D_8009D864[]`),
   presumably what the UI actually reads to draw the ATB bars.

**`D_800E7A48` is NOT an AI queue** — corrected. It's a small 0xFF-terminated
FIFO (`func_800A4D2C` push, `func_800A4D88` pop, `func_800A4E00` peek,
`func_800A4E40` rotate — all already matched, `battle.c`) holding **party
members whose ATB just filled with no command chosen yet**, i.e. whose
command menu needs to open next. `func_800A4CC8` (raw asm, called from
`func_800A4E00`) is the reverse of the already-known Manipulate redirect
(`func_800A44D8`): given a party slot, if that slot is standing in for a
Manipulated enemy, it returns the enemy's real battler id instead, so the
right creature's menu opens. The only caller of `func_800A4E00` anywhere in
the tree is `func_800DE618` (battle3/menu overlay, raw asm), which stages
the resolved actor into the menu's own globals and opens its command menu;
`func_800DF900` (already matched) is the confirm-button handler that
finally calls the real `func_800A4350` once the player finishes picking a
command through the UI. **Enemies never enter this queue at all** — the ATB
scan (above) calls `func_800A4350(i, -1, 0, 0)` for them directly, same
frame, confirmed by re-reading that exact code.

**Enemy action/target selection: a third VM, `func_800B1D48`, running an
AI script out of the same `script[0x1000]` blob used for messages.**
`CMD_NONE` (`0xFF`) reaching `func_800A1798` is a dead end, not the AI
trigger: it gets masked (`& 0x3F` → `0x3F`/63) and indexes past
`D_800F38AC`'s real 38 entries into unrelated BSS — a latent, never-hit
corner case, not a hook. The real dispatch happens one level up, in
`func_800A23E0`'s drain loop: for any battler with `D_800F83E0[i].unk4 & 0x10`
set (true for every enemy, and for a manipulated/status-forced party member),
it calls `func_800A6000(actorId, 1, 2)` *before* handing the popped entry to
`func_800A1798`. `func_800A6000` resolves a per-formation-slot script index
through the same two-level `func_800A5FB0` offset-table lookup already known
for battle messages, but with a different `sub` argument (`1` = "AI decision"
vs. the message-lookup callers' own sub values) — the *same* resolver feeds
both text and executable AI bytecode depending on which sub-slot is asked
for. The resolved pointer is handed to `func_800B1D48` (still `INCLUDE_ASM`;
957 lines, comparisons across all 10 combatants' HP/status build 10-bit
result masks, 4 nested opcode jump tables — `jtbl_800A0538/0568/0580/05A0`).
The script's "queue an attack" opcode calls `func_800B2B5C` (`battle.c`,
decompiled), which un-biases `attackIndex` for the two commands whose index
is category-offset elsewhere (`CMD_SUMMON` −0x38, `CMD_ENEMY_SKILL` −0x48),
resolves an attack id through `func_800B11B4` for the internal-only command
`0x20` (unnamed in `BattleCommand`), then builds a `QueuedAction` and inserts
it **directly into the 64-slot priority table via `func_800A3D4C`** —
bypassing `func_800A4350`/the ring buffer entirely. Target selection itself
is ordinary script-variable I/O: earlier opcodes write the shared
`D_800F83B2` (u16, part of the `D_800F83A4` battle-wide variable bank) and
`func_800B2B5C` just reads it back, both into the queued entry's
`targetMask` and into `D_800F83E0[actorId].unk18` (cached copy — the store
is genuinely 16-bit, not a full 32-bit write, despite `unk18` currently being
declared `s32`).

There's also a hardcoded fallback with no script involved:
`func_800A85FC` (`battle.c`, decompiled) is opcode `0x15` of `D_800E7B28` —
"attack a uniformly random allowed target." It hardcodes `cmdIndex =
CMD_ATTACK`, picks one set bit out of `allowedTargetsMask` via `func_800B3030`
(decompiled — `func_800B3000` popcount + `func_800B2F30` RNG byte, walks the
mask to the `rng % popcount`-th set bit) and converts it to an index via
`func_80014A38`. Party members hitting this opcode just no-op
(`cmdAnimation = -1`) — their targets always come from the menu, never a
random pick.

**Command queue → command-opcode VM**: `func_800A3ED0` drains the 16-slot
ring into the 64-slot priority table; `func_800A23E0` drains that table in
priority order into `func_800A1798` (the per-command opcode VM,
`D_800E7B28`). `func_800A23E0`'s only caller is `func_800A1158`
(`// entrypoint` in `battle.c`, itself uncalled anywhere in the
disassembled tree — confirmed a genuine overlay entry point invoked
externally, same category as `func_800D8D78` above).

**Animation-script VM (`func_800C7C4C`) caller chain**: called from
`func_800C5170` ← `func_800C1908` (per-battler, gated on
`D_801518E4[battler].D_80151922 & 0x20`) ← `func_800BA40C`/`func_800BA4C8`
(loop party / loop enemies) ← `func_800B83C4` ← `func_800B8438` (the
battle-phase `switch (D_80163C7C)` dispatcher) ← `func_800B7FDC`. A
secondary path (`func_800C7220`/`func_800C7340` → `func_800C74A4` →
`func_800C7C4C`) is registered as an async `D_80162978`-style callback via
`func_800BBEAC` during disk-streaming/load, not a per-frame `jal` — a
setup-time invocation, separate from the per-frame path.

**Both VM chains hang off the same per-frame tick, `func_800B7FDC`**
(`battle1.c`, already commented as the function that "pumps the GPU
ordering-table draw lists, runs render/vsync, drains the action-queue ring
buffer, and sets `D_800F7DE4`... once per frame"): it calls
`func_800B8438()` (→ animation-script VM, ticks every active battler every
frame) then `func_800A3ED0()` (→ queue drain toward the command VM). The
last hop — whether `func_800A23E0`/`func_800A1798` fire within that same
per-frame call, or on a different cadence via their own entrypoint
(`func_800A1158`) — is not closed by a static call edge; best-supported
model is that the animation-script VM runs continuously per battler every
frame regardless of commands, while the command-opcode VM fires once per
dequeued/prioritized command.

`D_801518E6` (the animation-script select index) is written from 3
distinct places: `func_800C7C4C` rewrites its own script mid-execution (a
self-modifying "switch script" opcode, several sites, values from a lookup
table, a sibling field, or a raw opcode argument); `func_800CEB48` (the
damage-popup/hit-resolution finalize step) sets it to `1` or copies
`D_801518E4[targetIdx].D_801518E8` once a hit resolves against a battler —
i.e. picks a hit-reaction/default script right after combat resolution;
and setup code (`func_800B7764`, `func_800B6B98`) sets/clears it during
battler initialization.

## Real damage application: `func_800AD0FC`

The function that actually subtracts damage from a battler's real `curHP`
(everything upstream — command dispatch, formula computation on
`ActorBattleData->targetDamage` — was already understood; this was the
missing final step). Confirmed by reading the asm:

```c
void func_800AD0FC(void) {
    BattleCombatant* target = &D_800F83E0[D_80063014->targetIdx];
    s32 flags = D_80063014->attackPropertiesExtra;  // offset 0x220
    s32 isHeal = flags & 1;
    s32 damage = D_80063014->targetDamage;           // offset 0x214

    if (damage == 0) return;

    if (flags & 4) {                                  // apply to MP instead of HP
        target->curMP = func_80014B08(target->curMP, damage, target->maxMP, isHeal);
        return;
    }

    s32 oldHP = target->curHP;
    target->curHP = func_80014B08(oldHP, damage, target->maxHP, isHeal);
    if (target->curHP == 0 && oldHP != 0) {
        target->status |= 1;                          // Death bit
        if (D_80063014->cmdProperties & 0x2000) {
            func_800A2DB0();                            // on-death handler
        }
    }
    // trailing block (party-target, enemy-attacker, non-heal only): scales
    // damage as a fraction of maxHP and adds it into *(D_80063014->unk204 + 8),
    // clamped to 0xFF -- a Limit-gauge charge-on-damage-taken update
    // (independently corroborated: func_800A38FC's already-confirmed
    // Limit-gauge-fill logic reads a value it calls limitBarUI, an
    // already-named field, as its own "how much to add this tick" input --
    // consistent with this trailing block being what populates it, though
    // the exact memory location isn't proven to be the same one).
}
```

`func_80014B08(current, delta, max, addMode)` (`src/main/18B8.c`, still
`INCLUDE_ASM`) is a generic signed-delta-with-clamp helper: subtracts
`delta` from `current` and clamps to `[0, max]` with underflow protection
when `addMode==0`, or adds and clamps to `max` when healing.

Called from two sites, both still raw asm, both reading as "commit this
hit's effects to one target": `func_800AB830(targetIdx, isMP)` (pulls a
pending delta out of `D_800F5BB8[idx].unk2C`/`unk30`, sets
`targetIdx`/`targetDamage` on `D_80063014`, calls `func_800AD0FC`, then
queues a damage-message via `func_800A2FD0`/`func_800ABA68`) and
`func_800ABB0C` (the larger status-mask-resolution routine, calls
`func_800AD0FC` after resolving `statusToAdd`/`statusToCure`/etc., then
`func_800ABA68` for display/logging and conditionally `func_800AD088`).

`attackPropertiesExtra` bit `4` (apply-to-MP) is a new confirmed bit,
alongside the already-known bit `0`. `cmdProperties` bit `0x2000` (gates
the on-death handler call) is a new confirmed bit, alongside the
already-known `0x80`/`0x40`/`0x04`/`0x800`.

## Support-ability / reaction mechanisms

Cross-cutting survey of where several named FF7 mechanics (counters,
Cover, Steal As Well, Added Cut, Final Attack, Sneak Attack) actually live
in this codebase, or don't.

**Counter attacks — the reaction scanner, `func_800A5BC8`.** `D_8009D870`
(the 8-row-per-battler `{triggerId, param, chance}` table it reads) is not
an independent array: it's a byte range (`+0x24`) inside one large
per-battler struct based at `D_8009D84C` (`Unk8009D84C`, stride `0x440`,
10 entries) — the same struct also owns `D_8009D864` (`+0x18`, ATB display
snapshot), a 16-row×6-byte materia list (`+0x4C`), `D_8009D8F8` (`+0xAC`),
and `D_8009D954` (`+0x108`, per-actor effect-config table, already read by
`func_800A2BF4`). `D_8009D84C` itself is shared, persistent state — also
block-swapped whole by the party-reorder screen (`src/main/1255C.c`'s
`func_80023AD4`), not overlay-local to `battle`.

`func_800A5BC8` has exactly 3 call sites, each passing a different `arg1`
category:
- `func_801B0050` (batini's setup entrypoint) calls it with `arg1=1`,
  unconditionally, once per party member, at battle start (before any
  command dispatch — no attacker exists yet at this point).
- `func_800A6278` calls it with `arg1=0`, gated on the acting battler
  being an enemy (`arg0 >= 4`); `func_800A6278` itself runs from
  `func_800A1798`'s tail whenever `ActorBattleData->unk7C & ~D_800F7DE0`
  is set for a hit target.
- `func_800A1798`'s own tail also calls it directly with `arg1=2`, same
  enemy-attacker gating, but keyed off a *different* mask field
  (`ActorBattleData->unkA8`), bypassing `func_800A6278` entirely.

So `arg1` selects between 3 different "how was this battler affected"
categories (matched against `row.triggerId == arg1*3 + {1,2,3}`), but
**where the table's rows actually get populated (which materia produces
which `triggerId`/`chance`) was not found anywhere in `battle`/`batini`** —
the only writer candidate located is `func_801B11BC` (batini, decompiled),
which scans the per-battler materia list and cross-refs it against kernel
table `D_800707C5[materiaID*8]`, but doesn't touch `+0x24` in the code read
so far — it writes a resolved ability-effect byte back into each materia
slot's own row (`Unk8009D84C.un4C[16][6]`, corrected from a previously
undersized `[4][6]`+`un64[0x48]` split — the two were the same 16-row
array all along, one row per equipped materia, 8 weapon + 8 armor) and
into a couple of per-row status bytes, not into the reaction table.
The "Counter / Command Counter / Magic Counter" 3-way naming used to plan
this investigation is a **working hypothesis, not confirmed** — vanilla
FF7's Counter is one Support materia paired with whichever Command
materia grants its effect, so the 3 `arg1` categories may just track
"which of N pairing slots fired," not a fixed 3-ability taxonomy. No
Vigilance-style chance-booster was found anywhere either.

**The real materia-pairing resolution pipeline — found, in `src/main`, not
`src/battle` at all.** Confirmed entry point: `func_8001786C` (`src/main/18B8.c`),
called once per party slot from `func_801B0050` (batini's battle-setup
entrypoint) via `func_8001786C(slot)`. For each of the 4 weapon-slot-pairs
and 4 armor-slot-pairs (link codes `{2,3}`/`{6,7}` in the slot-type byte
from `GetPartySlotWeaponMateriaSlots`/`GetPartySlotArmorMateriaSlots`,
`src/main/1255C.c`), `func_80017E68` → `func_80017F38` identify which of
the pair (if either) is a "needs a partner" Support materia
(`D_800730DD[id]&0xF == 5`) and call `func_80018028(supportId,
partnerMateria, ...)`.

`func_80018028` is the actual per-materia effect dispatcher: it caches
`D_800730DF[supportId][0..4]` (a parameter row) into scratch
(`D_80062E54[5]`), resolves a level-scaled intensity via `func_8001AC9C`,
then switches on the Support materia's own opcode byte
(`D_800730DE[supportId][0]`, confirmed range `0x51`-`0x63` via
`jtbl_80010158`'s real 19 `.word` entries — read exactly, not inferred
from code order):
```
0x51, 0x63 -> func_800191A0(id, intensity, partnerId)
0x52/0x53/0x5B/0x60/0x61/0x62 -> unused/no-op
0x54 -> D_80062E84=8;  func_80018390(id, intensity, partnerId)
0x55 -> D_80062E84=7;  func_8001840C(id, intensity, partnerId)
0x56 -> D_80062E84=4;  func_800184C0(id, intensity, partnerId)
0x57 -> D_80062E84=1;  func_800184C0(id, intensity, partnerId)
0x58 -> func_80018C94(intensity, partnerId)              [no id arg]
0x59 -> D_80062E78=1;  func_80018D4C(partnerId)
0x5A -> D_80062E78=2;  func_80018D4C(partnerId)
0x5C -> D_80062E78=0x10; func_80018D4C(partnerId)
0x5D -> D_80062E78=8;  func_80018D4C(partnerId)
0x5E -> func_80018220(intensity, partnerId, ctx) -- Elemental atk (confirmed)
0x5F -> func_800182FC(intensity, partnerId, ctx) -- Elemental def (confirmed)
```
The 4 opcodes funneling into `func_80018D4C` (`0x59`/`0x5A`/`0x5C`/`0x5D`)
are exactly the "paired, command-materia-linked bonus" shape Counter
Attack / Added Cut / Steal As Well / Sneak Attack all have in real FF7 —
`func_80018D4C` re-dispatches by the *partner* (non-support) materia's own
category (`D_800730DD[partnerId]&0xF`, values 6-12, `jtbl_800101C8`
read exactly):
```
6  -> func_80018ECC(id)
7  -> no-op
8  -> func_80018FC0()        [no id arg]
9  -> func_800190E8(id)
10 -> func_80019064()        [no id arg]
11 -> func_80018E18(id)
12 -> func_80018E90()        [no args]
```
`func_80018E90` (category 12) is fully traced: it broadcasts the staged
flag byte (`D_80062E78` — one of `1`/`2`/`8`/`0x10`, i.e. exactly which of
the 4 opcodes fired) into byte offset `0x10F` of 16 consecutive 8-byte
rows starting at absolute struct offset `0x1C0` inside the party member's
big struct (`D_80062E60`, confirmed elsewhere as `&D_8009D84C[slot]`) —
i.e. it tags *every one* of that character's 16 command-materia-linked
"attack slot" entries with a bit identifying which bonus-support-materia
effect applies. This 8-byte-row array is the **same** one
`func_801B11BC`'s second pass (battle overlay) walks by absolute offset
(that pass's local `+0x108` = this array's attack-id byte; `+0x10F` here
is 7 bytes further into the same row).

**What this settles and what it doesn't:** the full pairing→opcode→
per-character-flag-broadcast pipeline is now mapped end to end, and the
4 real "bonus" opcodes are pinned to exact bit values (1/2/8/0x10) in an
exact byte (`+0x10F` of each attack-slot row). **Not settled:** which of
the 4 opcodes (`0x59`/`0x5A`/`0x5C`/`0x5D`) is Counter vs. Added Cut vs.
Steal As Well vs. something else — that requires either the retail
`D_800730DE` kernel data content (not present in source) or finding the
still-unlocated *reader* of the `+0x10F` flag byte during actual attack
execution (not found in `battle.c`/`battle1.c`/`battle2.c`/`battle3.c` by
grepping for the literal offset — the reader likely computes the address
via multiply, the same way `func_80018E90`'s own writer does, so a
literal-offset grep can't rule it out, only fail to confirm it). Whether
this pipeline connects back to the `D_8009D870` reaction-table scanner
above at all is **also unconfirmed** — they may be two separate systems.

None of `func_80017E68`/`func_80017F38`/`func_80018028`/`func_80018D4C`/
`func_80018E90` have been converted to C in source, despite being fully
traced: they live in `src/main/18B8.c`, and this project's `main` overlay
shares fixed-address layout with satellite executables (confirmed:
`brom.exe`, via `config/sym_extern.us.txt`) — a non-matching C decomp
there shifts subsequent code and silently breaks those executables' link
(hard `undefined reference` errors, not warnings). Unlike every
`battle`/`batini`/`battle1`/`battle2`/`battle3` file touched this session,
`src/main/*.c` functions can only safely be documented in comments above
their still-`INCLUDE_ASM` stub, not lifted to non-matching C.

**The per-materia kernel record — `Unk800730CC`, and what its bytes mean.**
`D_800730DD`/`D_800730DE`/`D_800730DF` are not independent arrays; they're
confirmed aliases into the ALREADY-named `Unk800730CC[id]` struct (0x14 B
stride, one record per materia ID) at byte offsets `0x11`/`0x12`/`0x13`
respectively (`unk11`, a new field `opcode`, and `unk13`). Full confirmed
layout:
- `unk4[0..3]` (u16, offsets 4-0xA): AP thresholds ×100 for star levels
  2/3/4/5 (`0xFFFF` = "no such level", materia masters earlier).
  **`func_8001AC9C`(id, currentAP) — the star-level calculator — is
  already decompiled in `src/main/18B8.c` (behind a `NON_MATCHINGS`
  reference block, matching GCC 2.6.3)**: checks `currentAP` against these
  four thresholds highest-first, returns the current level (1-5), and
  separately counts how many of the four are "real" (not `0xFFFF`) into
  `D_80062FBC`. This directly confirms the level-gated-charge-count shape
  your Final Attack hypothesis described exists somewhere in this
  pipeline — see below for where it's actually used.
- `unk11 & 0xF` — pairing category: `5` = Support (needs a partner,
  `func_801B14E8`/`func_80017F38`), `7` = "fire once per battle" support
  (`func_801B11BC`/`func_801B1530`), `6`/`8`/`9`/`10`/`11`/`12` = which of
  6 writer functions a paired Support materia's bonus routes to when THIS
  materia is the *partner* (`func_80018D4C`'s own dispatch).
- `opcode` (was `D_800730DE[id][0]`) — for a Support-category materia,
  its own "which effect" opcode, `0x51`-`0x63` (`func_80018028`'s
  dispatch, full map already documented above).
- `unk13` (was `D_800730DF[id][0]`) — start of a 5-byte parameter block
  `func_80018028` copies into scratch (`D_80062E54`). **Flagged, not
  resolved:** a naive 5-byte read from this offset runs 4 bytes past this
  struct's confirmed 0x14-byte span, into the next materia's record. Either
  the true record is larger than 0x14 bytes with fields not yet identified,
  or the parameter block genuinely lives elsewhere. Worth resolving before
  trusting anything read from bytes 2-4 of that block.

**The category-6/8/9/10 writers — elemental and status affinity tables,
not combat triggers.** Read in full (`func_80018ECC`/`func_80018FC0`
[category 6/8] and `func_800190E8`/`func_80019064` [category 9/10],
`src/main/nonmatchings/18B8/`):
- Categories 6/8 resolve through `func_80019608(elementId)` — a
  16-entry linear scan of `D_80069508[]` (fixed/kernel element-ID list) —
  returning a slot index 0-15, then OR the staged flag byte
  (`D_80062E78`) into the parallel per-character table `D_8006950A[slot]`
  (3 bytes/slot — a weak/resist/absorb-style triplet per element, which
  byte is which unconfirmed). 16 slots matches FF7's well-known 16-element
  encoding exactly.
- Categories 9/10 are the same shape for **statuses**: `func_8001964C`
  resolves a status ID against a 56-entry `D_80069554[]` list, OR-ing into
  `D_80069558[slot]` (5 bytes/slot). 56 fits a byte-per-status kernel
  encoding (as opposed to the runtime 32-bit status bitmask).
- Category 9 (`func_800190E8`) additionally loops **the partner materia's
  own star level** number of times (via `func_8001AC9C`), walking
  backward through that materia's own kernel record bytes — i.e. a
  higher-level partner grants more status-affinity slots at once.
None of these four functions touch HP, Death, or anything per-battle —
they're **passive, setup-time resistance/affinity flags**, most plausibly
Elemental (the non-`0x5E`/`0x5F` pairing case) and Added Effect/Absorb-
style materia. Ruled out as Counter/Added Cut/Steal As Well/Sneak
Attack/Final Attack.

**Categories 11/12 — the real candidates for the combat-triggered
materia, and where the level-gated-charge shape actually shows up.**
`func_80018E18` (category 11) and `func_80018E90` (category 12, already
decompiled) both write into the *same* per-character "attack slot" array
`func_801B11BC`'s second pass also walks (absolute struct offset range
~`0x108`-`0x400` inside `Unk8009D84C`, 8-byte rows) — specifically the
last byte of each row (local offset 7). The two differ in exactly the
way your Final Attack hypothesis predicts: **category 12 unconditionally
tags all 16 rows; category 11 tags only as many rows as the partner
materia's own star level** (`func_8001AC9C` again). This is the first
place in the whole pipeline where "how many times this can fire, scaled
by level" and "which command slot(s) it applies to" appear together in
one mechanism — the strongest structural match found for Final Attack
(or Counter/Sneak Attack/Quadra Magic) of anything traced so far. Still
unconfirmed: which specific opcode (of the 4 reaching `func_80018D4C`:
`0x59`/`0x5A`/`0x5C`/`0x5D`) lands on category 11 vs. 12 vs. one of the
elemental/status categories for a GIVEN pairing — that depends on what
the support materia is paired *with* at runtime, not on a static
materia-to-opcode table alone, so even the retail kernel data wouldn't
settle it without also knowing typical pairings.

**Still not found: the actual reader of the attack-slot flag byte during
battle**, and **still not found: any HP/Death check anywhere in this
entire pipeline** (all 13 opcode handlers plus their leaf functions read
in full this pass) — so Final Attack's real trigger, if it exists, is
either in a part of the per-hit/death code not yet decompiled
(`func_800A1798`'s 789-line opcode dispatch is the largest remaining
unexamined piece of that path), or hooks in a way this investigation
hasn't found the shape of yet.

**Sneak Attack — the "already fired" gate, setter now located too.**
`func_801B11BC`'s category-7 check (`row[1] == 7`) is gated on
`D_800F5BE1[actorId*0x44] & 2` (= `D_800F5BB8[actorId].unk29 & 2`) —
clearing the row's flag only if that bit is set. `func_801B1734` touches
the same byte but only bit `0x8`, unrelated.

The bit's setter: `func_801B08C0` (raw asm, called once from
`func_801B0050`, the batini setup entrypoint — the same function that
zeroes each party member's ATB gauge, `D_800F5BB8[i].unk4 = 0`, at
`0x4($s7)` right after this check). Per party member (`$s7` walks
`D_800F5BB8[i]`), gated on a byte read via a second base register (`$s5`)
that lands on the *already-named* `D_800F5EFC` (confirmed: `D_800F5BB8 +
0x344 == D_800F5EFC` exactly) — specifically `D_800F5EFC[actorId*0x18]`,
byte 0, the same per-battler weapon-config row `func_800A81B8` reads
(`D_800F5EFC[actorId*0x18]`, byte 0 not otherwise decoded yet, bytes
1-4/0xE/0x14 already named fields on `D_80063014`). Raw logic:
```c
if (!(D_800F5EFC[actorId * 0x18] & 0x20)) {
    D_800F5BB8[actorId].unk29 |= 2;
}
```
i.e. the flag gets set at battle start for any party member whose
equipped-weapon config byte 0 does *not* have bit `0x20` set — the
reverse of a "this weapon is exempt" flag, or equivalently "this weapon
type gets the bonus." Not yet resolved: what weapon property bit `0x20`
of that byte actually encodes (only bits covered by `func_800A81B8`'s
already-decoded byte range are named; byte 0 itself is still raw), and
whether "fires once per battle, gated by weapon type at setup" actually
matches Sneak Attack's real FF7 mechanic (which vanilla is a materia
effect, not a weapon property) — this could equally be an unrelated
weapon-category flag (e.g. long-range) that happens to share the same
per-battler byte offset. `func_801B08C0` itself is large (loops 3 party
members × a 9-row inner scan against `D_8009C738`/`D_8009CBDC`, mostly
materia/formation setup unrelated to this one bit) and not otherwise
traced in this pass.

**Cover — not found; conclusively ruled out of the engine's code.** No
target-redirect mechanism exists anywhere in
`battle.c`/`battle1.c`/`battle2.c`/`batini.c` (the only confirmed redirect
is the unrelated Manipulate one, `func_800A44D8`, which redirects whose
*menu* opens, not an in-flight attack's target). Every HP-ratio/percentage
check in the codebase was read and ruled out as a decoy (Near Death damage
multiplier in `func_800B10B4`, elemental-resist scaling, accuracy/evasion,
critical chance, the 7777-HP easter egg, Gil-reward math) — none reassign
`targetIdx`/`targetMask`. `battle3.c` (the menu overlay) has zero references
to any targeting field, ruling out a menu-cursor-level implementation.
`func_800B1D48` (the enemy-AI/message bytecode VM) — the last remaining
candidate — has now had every one of its ~32 opcodes (1 top-level
11-entry table plus 3 nested low-nibble sub-tables of 6/8/7 entries each,
`jtbl_800A0538`/`0568`/`0580`/`05A0`) read and catalogued in full. None of
them test the confirmed back-row bit (`D_800F83E0[i].unk4 & 0x40`), and
none divide/shift `curHP`/`maxHP` or compare either against a fraction —
the VM's "load variable" opcode (family 0) can only read *one* combatant's
real stat field at a time (whichever actor the VM's own `eventKind`
argument names), never scanning all 10 for a cross-combatant HP
comparison; the only opcodes that DO scan all 10 combatants (a generic
MIN/MAX-search pair, `jtbl_800A0580` idx 4/5) operate on a script-supplied
scratch value with no code path that ties it to HP specifically. The VM's
only confirmed write to `D_800F83B2` (the target-selection output) is
elemental-affinity-driven (`jtbl_800A05A0` idx 6, matching combatants
against the attack's element mask), not HP-driven. **Bottom line: Cover
is not implemented as identifiable engine code anywhere in this codebase.**
If it exists at all, it would have to be entirely a property of specific
per-enemy AI script *data* (bytes in `SCENE.BIN`, feeding the generic
MIN-search opcode with HP as the cached variable) rather than hardcoded
logic — which is outside what static decompilation of the engine can
confirm one way or the other.

**2x-Cut / 4x-Cut / Slash-All / Flash commands — confirmed, no RNG at the
command layer. This is NOT Added Cut** — corrected mislabeling: these are
their own dedicated Command materia (guaranteed multi-hit, replaces
Attack entirely). The real Added Cut is a *support* materia paired with
any command materia (Deathblow, Attack, whatever it's linked to), granting
a percentage chance of one bonus extra attack after that command executes
— a bolt-on modifier, the same shape as Steal As Well below, not a
distinct queued command. **Added Cut itself: investigated, not found.**
`func_800A2D68`/`ActorBattleData->followUpActions[8]` (its only reader,
`func_800AABBC`) and the `attackAddlEffect`/`addlEffectModifier` pair (all
4 producers of a `followUpActions[]` push, and all 36 cases of the
dispatcher that reaches them, `func_800A8E84`'s `jtbl_800A0308`) are
confirmed to be a different system entirely: the kernel spell/item's own
baked-in "chance of a secondary status effect" (loaded by
`func_800A79CC`'s AttackData read, from `D_800F5F44+0x228`/`D_800708C4`/
`D_8009D8F8[...]` depending on `cmdIndexCopy`/`actionIndexCopy`) — not a
materia-driven bolt-on, no equipped-materia check anywhere in that chain.
`un4C[16][6]` (the resolved per-slot materia-effect rows from
`func_801B11BC`) appears unread anywhere in currently-decompiled
battle-turn code. Not reached: `battle2.c` untouched; 7 `battle3.c`
functions referencing `D_8009D84C` untraced. What follows below is the
(still useful, but differently-named) 2x-Cut/4x-Cut/Slash-All/Flash
command mechanism, not Added Cut. `cmdIndex` is chosen at menu-confirm
time in `func_800DEC10`
(battle3 overlay): it reads a per-actor "available commands" row table,
`D_8009D898[actorId]` (3-byte rows: `cmdIndex`, a targeting-screen
selector, a disabled-flag byte), and stores `row.byte0` straight into
`D_800F389C`, which `func_800DF900` hands verbatim to `func_800A4350`.
Whichever row exists in the per-actor list is what gets queued — no
probability roll happens here. The opcode-VM side confirms these are all
thin variants of the same physical-attack path (opcode `0x0E` =
`func_800A81B8`, weapon-config load; opcode `0x09` = `func_800AABBC`, the
generic per-hit/multi-target loop driven by `ActorBattleData.followUpCount`):
`CMD_2X_CUT`/`CMD_4X_CUT` are literally `followUpCount = 2` /
`followUpCount = 4` (`func_800A85A0`/`func_800A8528`, both already
matched one-liners) prepended to the ordinary attack sequence;
`CMD_SLASH_ALL` uses the *identical* byte sequence as plain Attack — its
"hit everyone" behavior comes entirely from the `targetMask` it's queued
with, not from any opcode difference. Where `D_8009D898`/its sibling
disambiguator `D_8009CBDC` get *populated* (the actual "is this materia
equipped" scan) wasn't traced — `func_800A6E9C` (still `INCLUDE_ASM`)
writes `D_8009CBDC` and is the next lead.

**Steal As Well — still not found; a real `cmdIndex`-branching materia
dispatch turned up inside `func_800A1798`, but it's almost certainly a
different, unrelated mechanism (see caveat below) — don't mistake it for
Steal As Well in a future pass.** `CMD_STEAL`'s own opcode sequence
(`func_800A8D18` seed +
`func_800AABBC`) has no item/inventory access at all; `CMD_ATTACK`'s
sequence (`func_800A81B8` + `func_800AABBC`) has no steal-shaped logic
either; `attackAddlEffect`/`addlEffectModifier` are written only by
spell/item setup opcodes (`func_800A80DC`/`func_800A79CC`), never by
Attack or Steal's own chains; `func_800ABB0C` has no `cmdIndex` read and
no drop-table reference (confirmed: grepped in full for any `D_8009D84C`-
struct access, direct or via a computed `*0x440` stride — zero hits, so
`func_800ABB0C` never touches the per-character materia/attack-slot
struct at all, not just "no drop table").

**Correction to "no `cmdIndex`-branching machinery" above:** `func_800A1798`
(raw asm, 789 instructions, not previously read in full) does exactly
this, and was missed because it accesses the per-character struct through
`D_8009D898` (`= D_8009D84C + 0x4C`, the already-named `un4C[16][6]`
materia-row array) rather than through `D_8009D84C` itself, so it didn't
show up in a symbol-name grep for the struct's base. Early in command
resolution (`.L800A1B90`-`.L800A1C8C`), it loops the acting battler's 16
equipped-materia rows (`row = &D_8009D898[actorId*0x440 + i*6]`) and, for
each row whose `row[0]` equals `D_80063014->cmdIndex` (offset `0xC`),
indexes a 13-entry jump table `jtbl_800A0120` by `cmdIndex - 5` (valid
range 5-0x11: `CMD_STEAL`, `CMD_SENSE`, `CMD_COIN`, `CMD_THROW`,
`CMD_MORPH`, `CMD_DEATHBLOW`, `CMD_MANIPULATE`, `CMD_MIME`,
`CMD_ENEMY_SKILL`, two unnamed gap values, `CMD_MELEE_ATTACK`). 7 of the
13 entries (`CMD_COIN`/`CMD_THROW`/`CMD_MIME`/`CMD_ENEMY_SKILL`/both gap
values) are pure no-ops (jump straight past). The other 6
(`CMD_STEAL`/`CMD_SENSE`/`CMD_MORPH`/`CMD_DEATHBLOW`/`CMD_MANIPULATE`/
`CMD_MELEE_ATTACK`) share one real handler:
```c
if (D_80063014->cmdProperties & 0x200) {   // 2 new confirmed cmdProperties bits
    if (row[4] != 0) {
        row[4]--;                          // per-materia-row "charges remaining"
        func_800A7254(2, actorId, 0x15, 1);  // queue message 0x15
    } else {
        D_80063014->cmdProperties |= 0x100000;   // "out of charges" flag
    }
}
D_80063014->targetMask = row[2];   // row's resolved kernel effect-byte (func_801B11BC's first pass)
```
then continues to the next of the 16 materia rows (no early exit — every
matching row fires, not just the first). `cmdProperties` (offset `0x90`,
already named) gains 2 confirmed bits here: `0x200` (gates this whole
block) and `0x100000` (set when a matching materia's charge count is
already 0).

**Important caveat, checked against the materia ID table
(`FFVII_MATERIA_ID_TABLE.md`) — this is very likely NOT "Steal As Well" or
any command-linked support materia, despite the shape.** `row[0]` here is
the *same* `materiaId` byte `func_801B11BC` reads (confirmed: identical
row-array addressing, byte offset 0). The match condition is literally
`materiaId == cmdIndex` — i.e. this fires only when an equipped materia's
raw ID *number* happens to equal the active command's index (5, 6, 9, 10,
11, or 17), not when a materia is *linked to* that command by any pairing
table. Cross-referencing those 6 ID values against the materia table:
`0x05`=EXP Plus, `0x06`=Gil Plus, `0x09`=Chocobo Lure, `0x0A`=Pre-emptive,
`0x0B`=Long Range, `0x11`=Underwater — six unrelated **Independent**
materia, with zero plausible connection to
Steal/Sense/Morph/Deathblow/Manipulate/Melee Attack. A materiaId
coincidentally matching a live `cmdIndex` is exactly the kind of
"confident-sounding but wrong" trap this document's own naming philosophy
warns about (see bottom of file) — this is flagged here specifically so
it doesn't get treated as a solved lead. Two explanations seem most
likely, neither confirmed: (a) genuine dead/vestigial code that can't
fire meaningfully in retail play (would need `row[0]` to be something
other than `materiaId` to make sense, contradicting the confirmed shared
addressing — needs a second read to rule out a subtly different row
layout in this call path), or (b) `D_80063014->cmdIndex` at this specific
point in `func_800A1798` is not yet the real player-chosen command index
but some other small-integer value reusing the same struct offset. Until
one of these resolves, do not read this block as evidence for Steal As
Well, Added Cut, Counter, or any other support-materia mechanic — record
it only as "a structurally real per-materia dispatch exists here, keyed
oddly."

**The `targetMask` write is a real interaction with an already-documented
field, not a new one — and it runs before that field's other known
writer.** `targetMask` (offset `0x50`, confirmed, `0xFF` = "no target yet
/ all") is elsewhere seeded by `func_800A8D60` (`if (targetMask == 0xFF)
targetMask = arg0`, called from `func_800A81B8`/`func_800A8D18` as part
of a command's own opcode handler) — a sentinel-respecting default from
`D_800F5EFC[actorId*0x18]` byte 0, the *same* weapon-config byte that
gates the Sneak-Attack-candidate bit (see "Sneak Attack" above).
`func_800A1798`'s materia-row loop runs chronologically *before* the
opcode-byte VM that calls `func_800A81B8`/`func_800A8D18` (confirmed by
position in the function: the materia loop is part of early per-command
setup, well before the `D_800A0098` opcode-program fetch later in the
same function). So on the (per the caveat above, probably rare/coincidental)
occasions this dispatch fires at all, it sets `targetMask` to a small
non-`0xFF` value *before* `func_800A8D18` runs — which makes
`func_800A8D60`'s later sentinel check (`if (targetMask == 0xFF)`) fail,
silently skipping the weapon-config default. Given the caveat above about
what actually triggers this block, this is best read as "confirmed
mechanical fact about execution order," not evidence of a specific named
materia mechanic — it depends on what a `row[2]` kernel-ability-effect
byte value means when reinterpreted as a `targetMask` bitmask, which
isn't decoded.

**Not yet confirmed:** what message `0x15` says (would likely name the
mechanic immediately), and whether `row[4]` is the same field categories
11/12 (see "Categories 11/12" above) tag with a level-scoped count — if
so this closes the loop between "materia gets N charges at pairing time"
and "a charge gets spent here." Untried leads, still open: ~30 unexamined
cases of `func_800A8E84`'s `jtbl_800A0308` (the per-effect-id
dispatcher), and
`func_800ABB0C`'s still-unnamed callees (`func_800AD4EC`, `func_800AB308`,
`func_800AC73C`, `func_800AE82C`, `func_800ACD88`, `func_800AB480`).

**Also found in this pass, in the same function, unrelated to the above:**
`func_800A1798` reads/clears a *third* bit of `D_800F5BB8[actorId].unk29`
(`0x4`, alongside the already-confirmed `0x80` 7777-easter-egg and the
`0x2` Sneak-Attack-candidate bit, see "Sneak Attack" above): once per
command, for the acting battler, `if (unk29 & 4) { func_800B0FFC(actorId,
0x7A, 1, &{actorId}); unk29 &= ~4; }` — same "one-shot flag, fire a
message, clear it" idiom as the `0x2` bit, different message (`0x7A`) and
different gating point (unconditional per-command, not
`func_801B11BC`'s category-7 gate). Setter not found in this pass.

**Final Attack — not found; the obvious candidate is something else
entirely.** `func_800A2DB0` (the `cmdProperties & 0x2000`-gated on-death
handler called from `func_800AD0FC`, now decompiled) turned out to be
enemy-only and formation-data-driven, not materia-driven: it bails unless
`targetIdx >= 4`, looks up the dying slot's raw enemy id
(`D_8016360C.formation[targetIdx-4].enemyID`) and, via that, a per-enemy
"death action ID" stashed at `SceneEnemy.unk90[4]` (byte offset 0xA0 within
`D_800F5F44.enemy[enemyID]`), and if one is configured (not the `0xFFFF`
sentinel most enemies leave it at), queues it via
`func_800A7254`/`func_800B0FFC` — the classic "enemy performs one last
scripted action when it dies" mechanic (self-destruct, revenge attack). The
other on-death callers from `func_800AFECC`'s Death-bit block —
`func_800A6278`/`func_800A61D4` (both now decompiled) and the `cmdIndex
0x25` action they queue via `func_800B2B5C` (→ `func_800A85FC` random-attack
+ `func_800A8424` display/animation-queue push, both now decompiled) — are
generic bookkeeping with zero materia reads. Confirmed:
`Savemap.materia_weapon`/`materia_armor` are read **nowhere** in
`src/battle/` at all (only from `src/menu/itemmenu.c`, the equip UI) — so
support-materia effects generally are not resolved from live Savemap data
inside the battle overlay. **`func_8001C0EC` (main overlay) — the
previous best lead — has now been conclusively ruled out.** Fully traced
(207 instructions): it reads no `Savemap`/materia data whatsoever, direct
or computed-offset; its only caller anywhere in `main` is `func_8001786C`
(the materia-pairing entry point above), and *that* function's own two
call sites are both in `src/menu/title.c` and `src/world/world.c`,
both firing at save-load/party-slot-restore moments (paired with
`func_80020058(slot)`, a "reset this slot's stat block" call) — never
from `src/battle/`, never correlated with a materia-equip change.
`D_8009D954` (the table it touches) is confirmed to be a W-Magic/
W-Summon/Quadra-Magic-style "linked extra casts" charge tracker (its
sole consumer, `func_800A2BF4`→ the `unk14`-indexed row logic in
`func_800A79CC`, is gated on `cmdIndexCopy`/normal action setup, never on
death) — mechanically ruled out as Final Attack's storage on top of
being save-load-triggered rather than death-triggered. **Final Attack's
trigger point remains genuinely unlocated** after exhausting every lead
this investigation produced.

**Sneak Attack — not found as an ATB-gauge effect; the fire-once
candidate mechanism is now further weakened, not just unconfirmed.**
Every write to `D_800F5BB8[i].unk4` (the ATB gauge)
reachable from battle setup was traced (`func_801B08C0` zeroes it per
slot; `func_800A653C` resets it to 0 on turn resolution; `func_800AE954`
only ever touches the *fill rate*, gated on Haste/Slow/Sleep status, never
materia) — none pre-seed one individual character's gauge to ready. The
whole-side (party-vs-enemy, never per-character) preemptive/back-attack
seed is a separate, fully-traced mechanism: `func_801B0668` (batini, now
decompiled) reads
the collapsed `BattleSetupType` (`D_800F5F44.D_800F7DC8`) and seeds
`D_800F5BBC` — a previously-undocumented alias of `D_800F5BB8[].unk4`,
same aliasing pattern as `D_800F83AE`/`D_800F83A8`/`D_800F83CC` — to
`0xFFFE` (party) / `0` (enemies) for `SETUP_PREEMPTIVE`, the reverse for
`SETUP_BACK_ATTACK`/`SETUP_PINCER`, or a speed-based formula otherwise;
`D_800F83D0 & 8` overrides to the preemptive formula unconditionally. This
confirms the encounter-level mechanic but rules it out structurally as the
per-character Sneak Attack materia (there's no per-slot branch to hang a
materia check on here).

Separately, a real per-character "support-materia category, fire-once"
trigger system was found and fully traced, though its effect bottoms out
in an announcement, not a gameplay-state change: `func_801B1530`/
`func_801B14E8` (batini, already decompiled) scan a party member's
`materia_weapon[8]`/`materia_armor[8]` for any materia whose
`D_800730CC[id].unk11 & 0xF == 7` (category 7), packing a bitmask into
`D_800F5E60[i]` (confirmed identical to the "previously-undocumented
secondary struct at `D_800F5BB8+0x2A8+i*0x34`" — `0x800F5BB8+0x2A8 ==
0x800F5E60` exactly, and `Unk800F5E60` is already a named struct, home of
`limitBar`/`limitBarUI`). `func_800AC73C` wires this into
`D_80063014->unk204` per turn for party actors; `func_800ABB0C` checks it
(`turnContext->cmdIndexCopy == CMD_ENEMY_SKILL`, offset `0x28`) and calls
`func_800AB788`, whose full chain is now traced end to end:
`func_800AB788` (message `0x73`) → `func_800A7254(2, targetIdx, 0x12,
param)` → drained by `func_800A72C8(2)` (called every frame right after
`func_800AFECC`) → `func_800A6E6C` (kind-0/subtype-13 re-queue) →
`func_800A6A3C`: `D_800F5BB8[arg0].unkE |= arg1;`. **`unkE` is confirmed
to have zero readers anywhere in `battle.c`/`batini.c`/`battle1.c`/
`battle2.c`** — it's a shared "announcement already shown" one-shot latch,
also written by the unrelated Limit-gauge-full banner (`func_800A4E80`).
So this chain is conclusively a pure text/UI announcement with **no ATB or
turn-order effect** — not "unconfirmed," ruled out. The gating condition
is also a point *against* the "category 7 = Sneak Attack" identification,
not just neutral: it requires `cmdIndexCopy == CMD_ENEMY_SKILL`
specifically, i.e. this fires only when using the Enemy Skill command,
not "once at battle start regardless of command" the way Sneak Attack
should. **Genuinely unlocated** — category 7 is some other
CMD_ENEMY_SKILL-linked support materia's announcement, not Sneak Attack's
mechanism.

## Sound

`func_800C2FD4` (called by `doDeathSequence`'s init tick and other
status-effect callbacks) stages 3 globals (`D_8009A000` opcode,
`D_8009A008` sound id, `D_8009A004` pan) and calls `func_8002DA7C`. The
full path: `func_8002DA7C` enqueues the message (`D_80081DC8` ring,
`D_80063010` write index, guarded by a `D_80062F8C` busy flag) →
`func_8002E1A8` drains the queue once per frame → `D_80049548[opcode]`
jump table dispatches to the per-opcode handler (`func_8002B9AC` for
opcode `0x2A`, confirmed to only use the pan/id fields, not the other 3
message-record fields).

`soundId = 5` (used in `func_800CEB48` when `damage_dealt == -1`) is
documented on the FFRTT sound-effect-ID wiki as "Missed attack," matching
`damage_dealt == -1` meaning a miss. `soundId = 0x16` (22), the one
`doDeathSequence`'s init tick passes to `func_800C2FD4`, is confirmed live
(by ear, via the "Sound Test" tool) to be the death sound for a normal
(non-boss) enemy.

The in-game pause banner mutes SFX downstream of `func_8002B9AC` (SPU
channel keys on, but silent) — a gotcha for using the Sound Test tool, not
part of any battle mechanic.

## Where to look next, roughly in order of leverage

**Resolved this pass, kept for context (do not re-investigate):**
`func_800B1D48` — all ~32 opcodes across its 4 nested jump tables now
catalogued (see "Support-ability / reaction mechanisms" above); Cover
conclusively ruled out of its opcode set. Sneak Attack's fired-bit setter
— `func_801B08C0` (see "Sneak Attack" above) — located; the remaining
open sub-question (item 1 below) is now what weapon-config bit gates it,
not who sets it.

### Support-materia mechanics — the active thread, pick up here first

1. **`D_800F5EFC[actorId*0x18]` byte 0, bit `0x20`.** Gates
   `func_801B08C0`'s Sneak-Attack-flag write (see "Sneak Attack" above).
   This is the same per-battler weapon-config row `func_800A81B8` reads
   (bytes 1-4/0xE/0x14 already named); byte 0 itself is still raw. Cross-
   reference against the weapon-stat kernel data shape (equip screen /
   `src/menu`) to figure out what property lives there — a long-range
   flag and a "no back-row penalty" flag are both plausible, either of
   which would rule out the Sneak-Attack identification entirely.
2. **The attack-slot flag byte's reader — all 3 prior candidates now ruled
   out; the search needs a new angle.** `func_80018E90`/`func_80018E18`
   (categories 12/11, `src/main/18B8.c`) write a flag into the last byte
   of specific rows in a per-character 8-byte-row array (absolute
   `Unk8009D84C` offset range ~`0x108`-`0x400`, same array
   `func_801B11BC`'s second pass walks). `func_800A1798` (789 instr, now
   fully grepped for every `D_8009*`-range symbol it touches),
   `func_800AABBC` (517 instr, now read in full), and `func_800ABB0C` (809
   instr, checked for any `D_8009D84C`-struct access, direct or via a
   computed `*0x440` stride) — **none of the three ever touch this
   struct's `0x1C0`-`0x400` sub-range.** `func_800A1798` does touch the
   same struct, but only its `+0x4C` materia-row array (`D_8009D898`) via
   a *different*, real `cmdIndex`-keyed dispatch — see "Steal As Well"
   above, a solid new lead but for a different byte (`row[4]`/`row[2]` of
   the 6-byte materia row, not the `0x1C0`-range 8-byte "attack slot"
   rows). The `0x1C0`-`0x400` array's reader is still genuinely
   unlocated; candidates worth checking next: `func_800A2BF4` (already
   read once for an unrelated purpose — re-check for this specific
   offset range), or search by data flow from `func_800AC73C` (the other
   confirmed writer into this same big struct, wires `D_800F5E60`'s
   category-7 bitmask into `D_80063014->unk204`) rather than by candidate
   function.
3. **Which opcode (`0x59`/`0x5A`/`0x5C`/`0x5D`) is which materia.**
   Structurally as far as static reading can take it — needs either retail
   kernel data or live pairing tests (equip a known materia + a known
   partner, observe which category/writer fires). Not resolvable from
   source alone.
4. **`Unk800730CC.unk13`'s apparent struct-overrun** (the 5-byte parameter
   block `func_80018028` copies, which as currently understood reads 4
   bytes past this struct's confirmed `0x14`-byte span). Resolve before
   trusting any read of that block's bytes 2-4.
5. **`func_8001786C`'s own body** (`src/main/18B8.c`, ~411 lines) — the
   per-party-slot materia-pairing entry point itself; only its call
   pattern and its tail (`func_80017E68` loop over slot pairs) have been
   traced, not the whole function.

### Earlier-session findings, still open (generic callback-pool thread)

6. **Test a physical-attack-inflicted status** (enemy attack effect, or a
   weapon + Added-Effect-style materia combo) against the status-effect-
   callback correlator. If it doesn't fire, the mechanism is inside
   `func_800C7C4C` (see above).
7. **Trace `func_800AB830`/`func_800ABB0C`/`func_800AD088`** — the 3
   callers of `func_800ABA68` (the HP/MP-checkpoint snapshotter). Whichever
   of these actually triggers would explain what real mechanic
   `func_800A3534`'s "restore to checkpoint" serves. (Note: `func_800ABB0C`
   is also item 2 above — one trace serves both threads.)
8. **Feed `func_800A3534`'s traced C sketch into the permuter/decompme
   pipeline** to get it byte-matched. Same for `func_800B8A34` (the
   `preset_idx` producer), `func_800D5350` (the bitmask/fn-ptr
   dispatcher), `func_800A311C`/`func_800ABA68` (the checkpoint-ring
   producers) — all fully traced, all still `INCLUDE_ASM`.
9. ~~**What `func_800B888C(battler_idx)` returns**~~ — resolved: the
   index of `battler_idx` in `D_801517F0`'s 78-entry frame table (see
   `preset_idx`/`D_800F99xx` section above). New follow-up this opens:
   **what populates `D_801517F0`'s 78 entries, and on what cadence** —
   untraced, would pin down what `action_id`/the whole `D_800F99xx`
   table family is really indexed by in gameplay terms.
10. **Find `func_800D5444`'s real caller(s)** — currently unreferenced
    anywhere in the retail `asm/us` tree by direct call (indirect/function-
    pointer-table call, or an overlay this search missed). Would name the
    whole `func_800D5444`/`func_800D5350` family properly.
11. **`func_800B6D6C`** (647 instr, likely a sibling producer to
    `func_800B8A34`) — untraced.
12. **Find `func_800D72B4`'s caller** — re-checked this pass, still not
    found: neither it nor its sole constructor site, `func_800D7340`
    (`static`, `battle2.c`), has any caller or address-taken reference
    anywhere in `asm/us` (grepped for the literal hex address too, only a
    false-positive hit in an unrelated overlay's own address space).
    Genuinely stuck without a different search angle (binary/rodata scan
    for the raw pointer value in a not-yet-symbolized data table, most
    likely) — would pin down the exact trigger for its screen-flash pulse.
13. ~~**`func_800D4FA8`**~~ — resolved: a full 16-slot event queue with a
    10-category dispatcher, see "`func_800D4FA8`/`func_800D4FF0`" section
    above. Follow-ups it opens: `func_800D6814` (category 6 handler, not
    yet examined), `D_800F836C` (the bitmask picking category 4 vs. 6 in
    `func_800D5774`), and `D_80163CC4`/`D_80151200`/`D_800E8C90`/`D_800E8C94`
    (categories 4/5/7/10's still-undecoded side tables).
14. **`func_800BBEAC`** — hand off the current near-match to the permuter.
15. **The `D_8015169C` floor in `func_800BBEAC`'s allocator** — why does
    slot allocation skip everything below the current dispatch index?
16. **Counter-attack priority band** (`prio=1`) — only one observation so
    far; worth confirming it's consistent and finding what triggers it.
17. **Identify offsets +0x2/+0x4/+0xA of a `D_800F9F3C` checkpoint entry**
    — the 3 caller-supplied values `func_800ABA68` stores alongside the
    HP/MP snapshot. Candidate identity from the FFRTT wiki's "Actor Battle
    Data" (`Inflicting`/`Curing`/`Toggling Status(es)`, see cross-reference
    above) — dump those 3 offsets live during a real status-inflicting hit
    and check against the confirmed status-bit table.
18. **`func_800A9DA0`** — 283-line function that reads/writes `curHP`/
    `maxHP` across all combatants; not yet understood.
19. **What calls `func_800C7C4C`**, and what feeds
    `D_801518E4[battler].unk2` (the script-select index).

### Broader battle-architecture e2e gaps (not yet started)

20. **Targeting resolution** — how `allowedTargetsMask`/a menu pick becomes
    the final `targetIdx`/`targetMask` used by damage application; the
    exact resolver function was never pinned down (only its inputs/outputs).
21. **Battle start/setup and victory/end flow** — only fragments seen
    (`func_800BC1E0` pool init, `func_801B0050`/`func_801B23E0` scene load,
    `func_801B0668` ATB seed). No victory/reward/escape flow traced at all.
22. **Sibling pool dispatchers** — `D_801620AC[10]`/`D_801621F0[60]` share
    `func_800BBEAC`'s allocator pattern; their own per-frame dispatchers
    (if any) unconfirmed.
23. **Message/event queue count** — `D_80163798`, the `func_800A3E98` queue,
    `func_800A7254`'s ring, and `func_800BBA84`'s cue queue: genuinely
    separate systems, or facets of one? Not reconciled.

## Naming philosophy

Most renames were chosen so the *value itself* is recognizable while
live-debugging (`tick_count` counts visibly 0,1,2...; `damage_dealt` is
literally the on-screen damage number), or because a complete static
producer→consumer trace made the meaning as solid as a live observation
(`preset_idx`, the ramp families' `accum`/`countdown`/`step`). Fields with
no such story (`unk8`/`unkA`, `unkC`-`unk1A`) are deliberately left
generic rather than given a plausible-sounding but unverified name.

Pattern-matching an "effect type" number against a *community* status-bit
ordering that this repo's own code doesn't confirm, then reading a
hardcoded RGB triple as "the status's color," is the cautionary case to
avoid: it produced a confident-sounding but wrong answer for the 6
status-effect callbacks (backwards on poison/regen, and internally
inconsistent — two different guessed statuses shared one hardcoded color).
The fix isn't to stop naming things from static analysis — `preset_idx`
and the ramp-family fields are static-only and solid — it's that those
have a *complete, traced, self-consistent* producer-and-consumer chain,
while the status colors were an *external* assumption (a bit-order table
from outside this codebase) layered on top of an incomplete trace. Static
tracing this codebase's own data flow end-to-end is trustworthy; borrowing
outside domain knowledge to fill a gap the trace didn't actually close is
not — that's the line to hold, not "static vs. live" per se.

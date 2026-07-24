# D_80162978 solo-session playbook

Field manual for solo pcsx-redux sessions with `battle_pool_watch.lua` loaded.
Each entry is one open question you can resolve live, with exact steps and the
exact output to hand back to a future Claude session. Assumes the script is
loaded (`dofile('\\wsl.localhost\Ubuntu\home\dmartin\ff7\battle_pool_watch.lua')`)
and you can get into a battle. Start here, don't re-derive anything.

dofile([[\\wsl.localhost\Ubuntu\home\dmartin\ff7\battle_pool_watch.lua]])

---

## Pick your tool first (decision framework)

Match the question to the method before you touch anything:

- **Breakpoint** (Callback Breakpoint Inspector checkboxes) — you need to catch
  one specific, possibly-rare code path the instant it fires and read real
  register args/state. Freezes the whole emulator; resume with pcsx-redux's own
  **Continue / Step (F5 / F10/F11)**, not the script. Use for "does this fire,
  and with what args" — never for repeated/statistical watching (it stops the game
  every hit).
- **Dump button** (Status correlator has two) — the target is a static or
  slow-changing table. Safe to click anytime, often without even being in battle.
  No timing pressure.
- **Live correlator / accumulating window** (Status ID correlator, ActorBattleData
  action log) — you want a mapping built up across MANY actions. Just play
  normally; the window accumulates. Periodically Copy/screenshot its state.
- **Pause + read memory by hand** — no purpose-built readout exists for the exact
  address you need. In-game-pause (Start) or break the emulator, open pcsx-redux's
  **Memory** viewer, go to the address. This is the fallback, not the first move.

**Priority at a glance:** do HIGH first (cheap, high-leverage, mostly just play).
MEDIUM needs a bit of setup. LOW is nice-to-have or needs specific equipment.

| # | Thread | Method | Priority |
|---|---|---|---|
| 1 | damageCalcID → formula mapping | correlator (ActorBattleData) | HIGH |
| 2 | Which status-effect callback = which real status | correlator (Status ID) | HIGH |
| 3 | Materia-pairing opcode (0x59/0x5A/0x5C/0x5D) ID | breakpoint (materia section) | HIGH |
| 4 | Targeting resolution (menu pick → targetIdx) | correlator (ActorBattleData) | MEDIUM |
| 5 | Counter-attack priority band (prio=1) | correlator (Command Queue) | MEDIUM |
| 6 | D_800F9F3C ring offsets during a status hit | dump + timing (Status ID) | MEDIUM |
| 7 | Sneak-Attack weapon-config byte (bit 0x20) | pause + memory viewer | LOW |
| 8 | Which checkpoint-restore caller fires | native breakpoint | LOW |
| 9 | Static table dumps (preset / frame table) | dump (Status ID) | LOW |

---

## HIGH priority

### 1. damageCalcID → formula mapping

**Resolves:** which value of `unk40` (the flagged damageCalcID) each attack type
produces, so we learn which formula-selector goes with physical vs. magic vs.
limit vs. item vs. enemy attacks. No static read can give this.

**Window:** ActorBattleData (D_80063014) — the "Action-change history" log.

**Steps:**
1. Open the **ActorBattleData (D_80063014)** window. It logs a line every time
   `cmdIndex` OR `unk40` changes.
2. In battle, perform one of each attack type, one at a time, letting each fully
   resolve before the next: (a) a plain physical **Attack**; (b) a **Magic**
   spell (e.g. Bolt); (c) a **Limit**; (d) an **Item** that heals (Potion);
   (e) wait for an **enemy attack** to land.
3. After each, glance at the log — a new `ACTION:` line appears per action.
4. When you've got all five, click **Copy all** on the action-change history.

**Method:** correlator — you want the mapping across many actions, and freezing
per-action would be pointless here (the value is already logged).

**Expected output** (one line per action; hand these back verbatim):
```
[f1234] ACTION: cmdIndex=1 [Attack] unk40(damageCalcID?)=6 actionIndex=0 element=0 power=16 atkPow=32 -> targetIdx=4 targetDamage=231 attackProps=0x2 extra=0x0
```
Copy the whole block into a text file (or paste directly back to Claude). The
useful signal is `cmdIndex [name]` vs `unk40` vs `targetDamage`/`attackProps`
across the five action types — that's the dispatch table.

---

### 2. Which status-effect callback = which real status (physical-attack path)

**Resolves:** which of the 6 status-effect callbacks
(`func_800C3578`/`func_800C4814`/`func_800C3CA8`/`func_800C328C`/`func_800C40F4`/
`func_800C44B4`) drives which real FF7 status. The leading hypothesis is that
this family only fires for statuses inflicted via a **physical attack** (enemy
innate effect, or a weapon + Added-Effect combo), not spells/items — so test it
that way.

**Window:** Status ID correlator.

**Steps:**
1. Equip **Added Effect** support materia paired with a **status-inflicting
   materia** (e.g. Added Effect + Seal → Sleep/Silence, or Added Effect + Poison)
   on one character's weapon.
2. Open the **Status ID correlator** window. Optionally call
   `battlePoolWatch.confirmedMap = {}` in the console for a clean slate.
3. Physically **Attack** an enemy with that character. Repeat a few times (status
   infliction is chance-based; it won't land every hit).
4. Also test the enemy side: let an enemy whose normal attack inflicts a status
   (e.g. a Poison-biter) hit your party.
5. Watch the **"Confirmed bit <-> callback mappings"** table at the top fill in.
6. Click **Copy all** on that table when a pairing appears.



tifa attack with seal materia added affect
> battler 5 - sleep (bit=2), then silence (bit=7), then death (bit=0)

tifa attack with poison materia added effect
> [f197698] battler=4 bit=3 (0x8)

barret got sleep from enemy mgc
> [f196974] battler=1 bit=2 (0x4)
and tifa
> [f198134] battler=2 bit=2 (0x4)

all 3 got regen from cloud magic spell
> [f210358] battler=2 bit=15 (0x8000)
> [f210358] battler=1 bit=15 (0x8000)
> [f210358] battler=2 bit=15 (0x8000)


**Method:** correlator — it auto-diffs the status word against callback allocs,
battler-for-battler, frame-for-frame. No breakpoint needed; you'd only interrupt
the very timing it's measuring.

**Expected output** (the confirmed-mappings lines are the payload):
```
bit=3 (0x00000008) <-> func_800C328C (effect type 3)  [last seen battler=4, f2201]
```
Decode `bit=` via the status bit reference at the bottom of this file
(`bit=3` = Poison). Copy the confirmed-mappings block back to Claude — it's
self-contained (bit number + mask + callback + effect type). If nothing appears,
see troubleshooting: widen the match window or check the raw onset/alloc logs
below the table for a mistimed pairing.

---

### 3. Materia-pairing opcode identification (0x59 / 0x5A / 0x5C / 0x5D)

**Resolves:** which of the 4 paired-bonus support opcodes is which materia
(Counter Attack / Added Cut / Steal As Well / etc.). Static reading is exhausted;
this needs a live pairing test.

**Window:** Callback Breakpoint Inspector → the **"Support-materia pairing (8)"**
section (added this session).

**IMPORTANT timing:** these functions live in the always-resident main overlay,
so the breakpoints arm fine anytime — but they only **execute at materia-pairing
resolution**, i.e. when the party/materia config is (re)computed: changing the
pairing in the menu then returning to field/world, or loading a save. They do
**not** fire every frame and won't reliably fire just from entering a battle.
Arm them, then trigger a recompute.

**Steps:**
1. Equip a known **Support** materia linked to a known **partner** (Command)
   materia in one weapon/armor slot pair — one pairing at a time so only one
   opcode fires.
2. Open **Callback Breakpoint Inspector**. In the **Support-materia pairing**
   section, check **`func_80018D4C`** and all six category writers
   (`func_80018ECC`, `func_80018FC0`, `func_800190E8`, `func_80019064`,
   `func_80018E18`, `func_80018E90`). Optionally also `func_80018028`.
3. Trigger a recompute: exit the materia menu back to the **field/world map**
   (or load a save).
4. The emulator pauses when a materia fn fires. Read the **LAST BREAK** panel:
   which writer fired tells you the partner's category (6/8/9/10/11/12).
5. To also capture WHICH of the 4 opcodes: with the emulator paused on
   `func_80018D4C`, open the **Memory** viewer and read **`D_80062E78`**
   (`func_80018028` staged it: `0x59`→1, `0x5A`→2, `0x5D`→8, `0x5C`→0x10).
6. **Continue** (F5) to resume. Repeat with a different Support materia.
7. Copy the **LAST BREAK** panel (Copy all) for each pairing.

**Method:** breakpoint — you're catching a rare, one-shot setup-time call and
need its register args + the paired writer identity at that exact instant.

**Expected output** (break entry — copy verbatim, plus note the equipped pairing
and the `D_80062E78` value you read):
```
[f560] func_80018D4C  (constructs partner-category re-dispatch (0x59/0x5A/0x5C/0x5D funnel here))
  a0 (partnerId) = 20 (0x14)
--- (then, right after, a writer fires:)
[f560] func_80018E90  (constructs category 12 writer (attack-slot flag, all 16 rows))
```
Hand back, per pairing: the equipped Support+partner materia names, the writer
that fired, and the `D_80062E78` value. That triple is what pins opcode → materia.

---

## MEDIUM priority

### 4. Targeting resolution (menu pick → final target)

**Resolves:** how a menu target selection becomes the final `targetIdx`/
`targetMask` the engine uses — the resolver was never pinned down statically.

**Window:** ActorBattleData (D_80063014) — the live field dump (top of window).

**Steps:**
1. Open **ActorBattleData (D_80063014)**. Watch these live fields:
   `allowedTargetsMask` (+0x18), `targetMask` (+0x50), `targetIdx` (+0x208).
2. Open a command (Attack/Magic), move the **target cursor** across enemies,
   and confirm on one target.
3. Note how `allowedTargetsMask` (what's selectable) narrows to `targetMask`
   (chosen mask) and finally `targetIdx` (resolved single target).
4. Click **Copy all** on the field dump at the confirm moment.

**Method:** correlator/live watch — the fields update every frame; you're reading
a progression, not catching one instant. Freezing would just as easily be done
by in-game pause if you need to read mid-cursor.

**Expected output** (from the field dump; copy the three relevant lines):
```
+0x018 allowedTargetsMask     = 0x000003f0 (1008)
+0x050 targetMask             = 0x00000010 (16)
+0x208 targetIdx              = 4 (0x4)
```
Hand back the before (cursor open) and after (confirmed) values for all three,
plus which enemy you picked. The transition is the resolver's behavior.

---

### 5. Counter-attack priority band (prio=1)

**Resolves:** confirms the dedicated counter-attack priority band — a simultaneous
3-party-member "Attack" push at `prio=1` seen once when an enemy attacks. Needs a
second observation to confirm it's consistent and find its trigger.

**Window:** FF7 Battle Command Queue — the **Long queue** (sorted, top = runs next).

**Steps:**
1. Open **FF7 Battle Command Queue**.
2. Let an **enemy attack** the party (don't act first — just wait for the enemy
   turn to land).
3. The instant it hits, watch the **Long queue** for entries at **prio=1**.
4. Click **Copy all** on the long queue while those entries are present (they
   drain fast — in-game pause with Start right after the hit if you need to catch
   them).

**Method:** correlator/observe — you're confirming a queue pattern appears
consistently; the queue window already shows it. Pause-and-copy only to freeze a
fast-draining moment.

**Expected output** (long-queue lines at prio=1):
```
slot[ 3] actor=0 Attack attackIdx=0 targetMask=0x0010 prio=1 qPos=0
slot[ 5] actor=1 Attack attackIdx=0 targetMask=0x0010 prio=1 qPos=1
slot[ 8] actor=2 Attack attackIdx=0 targetMask=0x0010 prio=1 qPos=2
```
Hand back the prio=1 block plus what the enemy did to trigger it (which enemy,
which attack). Repeat across a couple of enemy attacks to confirm consistency.

---

### 6. D_800F9F3C ring offsets during a status-inflicting hit

**Resolves:** identity of ring-entry offsets `+0x2`/`+0x4`/`+0xA` (candidate
inflict/cure/toggle status masks per the FFRTT wiki). Confirm by checking them
against the status-bit table during a real status hit.

**Window:** Status ID correlator — **"Dump D_800F9F3C preset table"** button.

**Steps:**
1. Set up a status-inflicting hit as in entry #2 (Added Effect + status materia,
   or a status-inflicting enemy).
2. Land the status hit. Immediately **in-game pause (Start)** or break the
   emulator so the ring entry doesn't get overwritten.
3. In **Status ID correlator**, click **Dump D_800F9F3C preset table (static, no
   gameplay needed)**.
4. Find the row whose `partySlot` matches the affected battler and read
   `+0x2`/`+0x4`/`+0xA` (shown as hex).
5. Compare those hex values to the **status bit reference** (bottom of this file):
   do they look like status bitmasks matching the status you inflicted?
6. Click **Copy all** on the dump.

**Method:** dump + timing — the table is static memory, but you must dump it while
the just-written entry is fresh, so pause right after the hit.

**Expected output** (dump rows; the `+0x2`/`+0x4`/`+0xA` fields are the payload):
```
preset_idx= 3: partySlot=0 +0x2=0x8 +0x4=0x0 hpPreset=320 mpPreset=42 +0xA=0x0
```
Hand back the matching row(s) plus which status you inflicted and on whom, so a
`+0x2=0x8` can be checked against `0x8 = Poison`.

---

## LOW priority

### 7. Sneak-Attack weapon-config byte (bit 0x20)

**Resolves:** what property bit `0x20` of `D_800F5EFC[actorId*0x18]` byte 0
encodes — it gates the Sneak-Attack-candidate flag. If it turns out to be a
long-range / no-back-row-penalty flag, that rules out the Sneak-Attack reading.

**Window:** none — pcsx-redux **Memory** viewer.

**Steps:**
1. In battle with a party member (actor 0), **in-game pause (Start)**.
2. Open the **Memory** viewer, go to `0x800F5EFC` (actor 0; add `actorId*0x18`
   for actors 1/2 → `0x800F5F14`, `0x800F5F2C`).
3. Read **byte 0**; note whether bit `0x20` is set.
4. Change to a different weapon type (different character or reload with different
   equipment) and re-read.
5. Copy the byte value + which weapon was equipped, for each weapon type.

**Method:** pause + memory viewer — no readout exists for this byte; it's a
static per-battler config read.

**Expected output** (record per weapon):
```
D_800F5EFC[actor0] byte0 = 0x2A (bit 0x20 SET)  weapon = Buster Sword
D_800F5EFC[actor0] byte0 = 0x0A (bit 0x20 clear) weapon = <other>
```
Hand back the byte for several weapon types so the bit's meaning can be
cross-referenced against equip-screen weapon-stat kernel data.

---

### 8. Which checkpoint-restore caller fires

**Resolves:** which of `func_800AB830` / `func_800ABB0C` / `func_800AD088` (the 3
callers of the HP/MP-checkpoint snapshotter `func_800ABA68`) actually triggers,
which explains what real mechanic `func_800A3534`'s "restore to checkpoint"
serves. These aren't in the script's breakpoint list — use the native debugger.

**Window:** pcsx-redux native **Breakpoints** (Debug menu), not the script.

**Steps:**
1. In pcsx-redux, add three **Exec** breakpoints: `0x800AB830`, `0x800ABB0C`,
   `0x800AD088`.
2. Play a normal battle with a variety of actions (attacks, heals, statuses).
3. When one fires, note **which address** and **what you'd just done** in-game.
4. **Continue** (F5), keep observing which of the three fires and when.

**Method:** breakpoint — catching which rare caller fires, tied to an in-game
action. (Native, because the script's inspector only covers pool functions.)

**Expected output** (record per hit):
```
0x800ABB0C fired -- had just used Enemy Skill on party member 1
```
Hand back which addresses fired and the in-game action that preceded each. Note:
`func_800ABB0C` is also relevant to the attack-slot-flag-reader thread, so a hit
there is double useful.

---

### 9. Static table dumps (no gameplay needed)

**Resolves:** raw contents of the `D_800F9F3C` preset table and the `D_801517F0`
frame table — background data for the `preset_idx` and `func_800D5938` threads.
Pure reference; can be done outside battle.

**Window:** Status ID correlator — the two **Dump** buttons at the bottom.

**Steps:**
1. Open **Status ID correlator**.
2. Click **Dump D_800F9F3C preset table (static, no gameplay needed)** and
   **Dump D_801517F0 frame table (static, no gameplay needed)**.
3. Click **Copy all** under each.
4. For a richer preset table, re-dump partway through a fight (entries accumulate
   as the fight progresses) and hand back both snapshots.

**Method:** dump — static memory, no timing.

**Expected output:**
```
preset_idx= 1: partySlot=0 +0x2=0x0 +0x4=0x0 hpPreset=65535 mpPreset=65535 +0xA=0x0
D_801517F0 frame table: { 0x04, 0x05, 0x06, 0xff }
```
Hand back the full dumps. Rows flagged `<-- out of range` mark the real table end.

---

## Status bit reference (for decoding correlator/dump output)

Confirmed bits (locally live-tested this project). Bit number = position;
mask = `1 << bit`.

| bit | mask | status | bit | mask | status |
|---|---|---|---|---|---|
| 0 | 0x1 | Death | 11 | 0x800 | Frog |
| 2 | 0x4 | Sleep | 12 | 0x1000 | Small/Mini |
| 3 | 0x8 | Poison | 15 | 0x8000 | Regen |
| 4 | 0x10 | Sadness | 16 | 0x10000 | Barrier |
| 6 | 0x40 | Confuse | 17 | 0x20000 | MBarrier |
| 7 | 0x80 | Silence | 18 | 0x40000 | Reflect |
| 8 | 0x100 | Haste | 21 | 0x200000 | Death Sentence |
| 9 | 0x200 | Slow | 22 | 0x400000 | Manipulate |
| 10 | 0x400 | Stop | 23 | 0x800000 | Berserk |
| | | | 28 | 0x10000000 | Death Force |
| | | | 29 | 0x20000000 | Resist |

Status-effect callback → effect-type number (from the correlator's `effect type`
field): `func_800C3578`=0, `func_800C3CA8`=2, `func_800C328C`=3,
`func_800C44B4`=7, `func_800C4814`=8, `func_800C40F4`=17/18.

---

## If you get stuck / nothing's firing

- **Reload after any script edit:** re-run the `dofile(...)` call. Snapshot caches
  reset automatically on load.
- **Breakpoints seem stuck / the game keeps pausing:** `battlePoolWatch.removeAll()`
  in the console disarms every breakpoint AND disables per-frame scanning (all
  checkboxes uncheck). Re-`dofile` to bring scanning back.
- **Resume a paused emulator** with pcsx-redux's own **Continue (F5) / Step
  (F10/F11)** — the script has no resume button.
- **Correlator not matching:** inflict ONE status at a time. Widen `MATCH_WINDOW`
  (top of the correlator section in the script) if alloc and the status-word write
  land more than 6 frames apart. Check the raw "onsets" and "allocs" logs below the
  confirmed table for a mistimed pairing. `battlePoolWatch.clearStatusCorrelator()`
  resets the raw logs (keeps confirmed mappings); `battlePoolWatch.confirmedMap = {}`
  wipes confirmed mappings for a clean re-test.
- **History logs cluttered:** `battlePoolWatch.clearHistory()` (pool slots),
  `battlePoolWatch.clearAbdHistory()` (ActorBattleData action log).
- **Materia-pairing breakpoints never fire:** they execute at materia-pairing
  resolution (menu change → field/world, or save-load), NOT per-frame or on
  entering a battle. Trigger a recompute — see entry #3's timing note.
- **Sound Test plays but is silent:** the in-game pause banner mutes SFX downstream,
  independent of the tool. Use Sound Test **unpaused**.

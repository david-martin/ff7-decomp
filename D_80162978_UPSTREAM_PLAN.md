# Upstreaming plan for the `D_80162978` investigation branch

Current state (grounded in the actual working-tree diff, not the
investigation doc's prose, which drifts from source over time): one PR
already merged (`cdee657`, "name `D_80162978` pool fields, lift 4 matched
functions touching it"). On top of that, the working tree has ~1700
uncommitted lines across `battle.c`/`battle1.c`/`battle2.c`/`batini.c`/
`battle.h`/`battle_private.h`/`include/game.h`, plus 3 untracked reference
docs and 2 untracked scratch `.txt` dumps. This file is the plan for turning
that into a sequence of reviewable PRs.

## The two lanes, and why they must stay separate

**Lane A — comments only, zero build risk.** The large majority of the
diff (checked directly: `battle.h`, `battle_private.h`, and `game.h`'s
diffs are *entirely* new comments — no field renamed, no layout changed;
one exception, `func_800D5444`'s callback prototype widened from
`void(*)(int)` to `void(*)(int,int)`, safe because a declaration's type
doesn't affect codegen at the storing call site). Anything in this lane
can be reviewed on prose alone — a reviewer never has to ask "does this
still build/match." **Bundle a subsystem's comments with lifting its
already-`MATCHED` functions in the same PR** (the `cdee657` precedent) —
a matched lift carries the same zero risk as a comment, and pairing them
gives the PR a complete story ("here's what this struct means, and here's
proof in the form of real matched code using it").

**Lane B — genuinely new non-matching C** (29 functions moved out of
`INCLUDE_ASM` this branch, confirmed via `git diff | grep -c '^-INCLUDE_ASM'`
per file: 17 in `battle.c`, 8 in `battle1.c`, 2 in `battle2.c`, 2 in
`batini.c`). Each of these is a real correctness claim a reviewer has to
evaluate against the asm. Keep these PRs small (3-5 functions) and grouped
by what they mechanically do together, not by which file they happen to
live in.

**Hard constraint, applies to both lanes:** `src/main/18B8.c` (the
materia-pairing pipeline — `func_8001786C`/`func_80017E68`/`func_80018028`/
`func_80018D4C`/`func_80018E90` etc.) can **never** get a non-matching C
body. `main` shares fixed-address layout with `brom.exe`; a non-matching
decomp there shifts subsequent code and silently breaks that satellite
executable's link. Anything there is comment-on-the-`INCLUDE_ASM`-stub
only, forever (until it's byte-matched) — make sure no PR accidentally
converts one of these.

## Lane A order (do this first — cheap, safe, and it front-loads the
naming vocabulary later PRs' commit messages can lean on)

1. **`D_801620AC` movement/GTE-tween pool.** Comment the pool + lift its
   ~14 already-`MATCHED` tick functions (`func_800CF2F0`, `CF3CC`, `CF4A8`,
   `CF5BC`, `CF8C0`, `CFB14`, `CFCB0`, `CFE60`, `D01C0`, `D0578`, `D061C`,
   `D06B8`, `D0760`, `CDDE8`, `CDE78`). Split into 2 PRs if the maintainers
   want smaller diffs (a: position-tween family; b: delay-then-X family).
2. **`D_801621F0` popup/effect pool remainder.** `func_800CE638`,
   `CDFC4`, `CD82C`, `CDD44`, `CE384`, plus the magic-effect dispatch trio
   `D0B4C`/`0C80`/`1110`.
3. **Battle-loop per-frame architecture.** `func_800B7FDC`, `B8438`,
   `BA40C`, `BA4C8`, `C1908`, `C74A4`, `BB030`, `BB90C`, `BB67C`. High
   narrative value — this is the backbone everything else hangs off, worth
   landing early so later PR descriptions can link to it.
4. **Command queue / ATB plumbing.** `func_800A4350`, `A3E98`, `A44D8`,
   `A4CA8`, `A4D2C`, `A4D88`, `A4E00`, `A4E40`, `A4AF4`, `A653C`, `A6590`,
   `DF900`.
5. **Damage-formula & combat-resolution utility functions.**
   `func_800AD8DC`, `ADE5C`, `AE234`, `B10B4`, `B11B4`, `B2F30`, `B3000`,
   `A6A3C`, `A6E6C`, `A2D68`, `B0FFC`.
6. **Batini/materia baseline.** `func_801B14E8`, `1530`, `18F8`, `23E0`
   (own PR — different overlay/`progress_categories` entry than
   `battle`).

## Lane B order (real decompiles — smaller batches, each a clear claim)

7. **The real damage/death chain — the headline finding, but sequence it
   after Lane A #3 (loop) and #5 (formulas) so reviewers already have the
   vocabulary.** `func_800AFECC` (the DoT/regen tick that sets the Death
   bit), `func_800A2DB0` (enemy on-death scripted action), `func_800CEB48`
   (popup finalize/death-sequence gate). **Also include `func_800AD0FC`
   here even though it's not part of the current diff** — it's fully
   traced in the investigation doc already (complete C pseudocode, never
   written to source) and is the actual "subtract damage from real curHP"
   function; writing it up is close to zero extra effort and it's the
   single most-referenced missing piece in the damage-application story.
8. **Attack/item/weapon setup opcodes.** `func_800A79CC`, `A80DC`,
   `A81B8`, `A8424`.
9. **Reaction/counter-attack scanner.** `func_800A5BC8`, `A6000`, `A61D4`,
   `A6278`.
10. **ATB scan & announcements.** `func_800A2894`, `A304C`, `A38FC`.
11. **Batini setup (non-matching half).** `func_801B0668`, `1B11BC` — own
    PR, same overlay-scoping reason as Lane A #6.
12. **Everything else newly lifted, grouped as a "misc small utility"
    PR** (or two, if it's too big): `func_800A2BF4`, `A85FC`, `B2B5C`,
    `B3030`, `B5AAC`, `B5FE8`, `C2704`, `C2928`, `C2C1C`, `C2FD4`, `C33F0`,
    `D57C0`.

## Lane C — not decompiled at all yet (future phase, don't block on it)

Found and fully traced from raw asm this investigation, but no C exists
in source yet — writing an initial non-matching decomp is legitimate
(this project's own convention), just needs the C actually written first:
`func_800D4FA8`, `func_800D4FF0`, `func_800D5774`, `func_800B888C` (all
short, 24-46 instructions — quick wins), `func_800D5938` (bigger, the
frame-table sequencer), `func_801B08C0` (426 instructions, the biggest
ask), `func_800AE954` (small). `func_800BBEAC` already has a best-effort
C sketch outside the repo (`/home/dmartin/bwork/`) — feed that to the
permuter rather than hand-writing again.

## Not ready for any PR — open research, don't gate the above on it

Steal As Well / Added Cut / Final Attack / Cover / Sneak Attack's real
identity, the `D_800F9F3C` ring's mystery offsets, `D_80163CC4`/
`D_800E8C90` side tables from the new event queue. These are genuinely
inconclusive per the investigation doc's own hedging — landing code that
*names* something based on them would repeat exactly the mistake the
doc's "naming philosophy" section warns about. Keep chasing them in the
investigation doc; don't let them block any of the above, whose value is
independent of these open questions.

## Loose ends to resolve before PR #1 of this new batch

- `FFVII_MATERIA_ID_TABLE.md` / `FFVII_BATTLE_SYSTEM_WIKI.md`: decide
  whether these get committed as repo reference docs (matches the
  precedent of keeping `D_80162978_INVESTIGATION.md` itself in-tree) or
  stay local-only.
- `src/battle/barret attacks enemy.txt` / `enemy attacks barret.txt`: look
  like raw unprocessed notes/dumps, not finished reference material —
  either fold their content into the investigation doc or delete them,
  don't commit as-is.
- This file and `D_80162978_FUNCTION_TABLE.md` are planning/reference
  material for *this* upstreaming effort — same question applies to them.

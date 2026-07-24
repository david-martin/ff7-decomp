# FF7 Materia ID Table (PC release)

Community-documented reference (not derived from this codebase) — the
canonical materia ID → name → type mapping. Materia is stored as a single
byte ID followed by a 24-bit unsigned AP value. Enemy Skill materia does
not use the AP value; instead the 24 bits are switches for each learnable
skill. AP == 0xFFFFFF when mastered.

| ID | Name | Type |
|---|---|---|
| 0x00 | MP Plus | Independent |
| 0x01 | HP Plus | Independent |
| 0x02 | Speed Plus | Independent |
| 0x03 | Magic Plus | Independent |
| 0x04 | Luck Plus | Independent |
| 0x05 | EXP Plus | Independent |
| 0x06 | Gil Plus | Independent |
| 0x07 | Enemy Away | Independent |
| 0x08 | Enemy Lure | Independent |
| 0x09 | Chocobo Lure | Independent |
| 0x0A | Pre-emptive | Independent |
| 0x0B | Long Range | Independent |
| 0x0C | Mega All | Independent |
| 0x0D | Counter Attack | Independent |
| 0x0E | Slash-All | Command |
| 0x0F | Double Cut | Command |
| 0x10 | Cover | Independent |
| 0x11 | Underwater | Independent |
| 0x12 | HP <-> MP | Independent |
| 0x13 | W-Magic | Command |
| 0x14 | W-Summon | Command |
| 0x15 | W-Item | Command |
| 0x16 | Unknown | Placeholder? |
| 0x17 | All | Support |
| 0x18 | Counter | Support |
| 0x19 | Magic Counter | Support |
| 0x1A | MP Turbo | Support |
| 0x1B | MP Absorb | Support |
| 0x1C | HP Absorb | Support |
| 0x1D | Elemental | Support |
| 0x1E | Added Effect | Support |
| 0x1F | Sneak Attack | Support |
| 0x20 | Final Attack | Support |
| 0x21 | Added Cut | Support |
| 0x22 | Steal As Well | Support |
| 0x23 | Quadra Magic | Support |
| 0x24 | Steal | Command |
| 0x25 | Sense | Command |
| 0x26 | Unknown | Placeholder? |
| 0x27 | Throw | Command |
| 0x28 | Morph | Command |
| 0x29 | Deathblow | Command |
| 0x2A | Manipulate | Command |
| 0x2B | Mime | Command |
| 0x2C | Enemy Skill | Command |
| 0x2D | Unknown | Placeholder? |
| 0x2E | Unknown | Placeholder? |
| 0x2F | Unknown | Placeholder? |
| 0x30 | Master Command | Command |
| 0x31 | Fire | Magic |
| 0x32 | Ice | Magic |
| 0x33 | Earth | Magic |
| 0x34 | Lightning | Magic |
| 0x35 | Restore | Magic |
| 0x36 | Heal | Magic |
| 0x37 | Revive | Magic |
| 0x38 | Seal | Magic |
| 0x39 | Mystify | Magic |
| 0x3A | Transform | Magic |
| 0x3B | Exit | Magic |
| 0x3C | Poison | Magic |
| 0x3D | Demi | Magic |
| 0x3E | Barrier | Magic |
| 0x3F | Unknown | Placeholder? |
| 0x40 | Comet | Magic |
| 0x41 | Time | Magic |
| 0x42 | Unknown | Placeholder? |
| 0x43 | Unknown | Placeholder? |
| 0x44 | Destruct | Magic |
| 0x45 | Contain | Magic |
| 0x46 | FullCure | Magic |
| 0x47 | Shield | Magic |
| 0x48 | Ultima | Magic |
| 0x49 | Master Magic | Magic |
| 0x4A | Choco/Mog | Summon |
| 0x4B | Shiva | Summon |
| 0x4C | Ifrit | Summon |
| 0x4D | Ramuh | Summon |
| 0x4E | Titan | Summon |
| 0x4F | Odin | Summon |
| 0x50 | Leviathan | Summon |
| 0x51 | Bahamut | Summon |
| 0x52 | Kujata | Summon |
| 0x53 | Alexander | Summon |
| 0x54 | Phoenix | Summon |
| 0x55 | Neo Bahamut | Summon |
| 0x56 | Hades | Summon |
| 0x57 | Typhoon | Summon |
| 0x58 | Bahamut ZERO | Summon |
| 0x59 | Knights of Round | Summon |
| 0x5A | Master Summon | Summon |
| 0xFF | Empty Slot | NONE |

## Notes for cross-referencing against this decomp's D_800730DD/DE/DF findings

- **"Counter Attack" (0x0D, Independent) is a DIFFERENT materia from
  "Counter" (0x18, Support).** The Independent one likely doesn't need a
  pairing at all (self-contained chance-to-counter); the Support one is
  the pairing-required "counter using whatever command materia it's
  linked to" mechanic this investigation was chasing. Do not conflate
  them going forward.
- 13 Support-type materia total (0x17-0x23): All, Counter, Magic Counter,
  MP Turbo, MP Absorb, HP Absorb, Elemental, Added Effect, Sneak Attack,
  Final Attack, Added Cut, Steal As Well, Quadra Magic — matches the 13
  non-no-op opcode slots catalogued in `func_80018028`'s `D_800730DE`
  dispatch (0x51-0x63 minus the 6 confirmed-unused values), a clean 1:1
  structural match corroborating that catalog's completeness.
- Elemental (0x1D) is the only Support materia independently CONFIRMED
  against a specific opcode pair so far (0x5E attack-imbue / 0x5F
  defense), via already-decompiled-and-read handler bodies
  (`func_80018220`/`func_800182FC`).

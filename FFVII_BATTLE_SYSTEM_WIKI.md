# Final Fantasy VII battle system — community reference (finalfantasy.fandom.com)

Saved verbatim (source wikitext) from
https://finalfantasy.fandom.com/wiki/Final_Fantasy_VII_battle_system
for cross-referencing against this decomp's own findings in
`D_80162978_INVESTIGATION.md`. This is community documentation of observed
game *behavior*, not derived from this codebase — treat any correlation
with actual code findings as a hypothesis to verify, not a given (see the
"Naming philosophy" section of the investigation notes for why).

---

{{DISPLAYTITLE:''Final Fantasy VII'' battle system}}
[[File:FFVII Tifa Slash-All.png|thumb|250px|Battle in ''Final Fantasy VII''.]]
''[[Final Fantasy VII]]'' has a '''[[battle system]]''' based on [[Active Time Battle]] (ATB) where players control three [[party]] members who can act when their ATB gauge fills. At this point, a [[Menu (Final Fantasy VII)|menu]] opens for that party member to select from a variety of [[Final Fantasy VII abilities|abilities]], [[Magic (Final Fantasy VII)|magic]], and [[Summon (Final Fantasy VII)|summons]] provided by their equipped [[Materia (Final Fantasy VII equipment)|Materia]]. Party members also have a [[Limit (Final Fantasy VII)|Limit]] gauge that fills upon taking damage; once full, the Limit Break replaces the basic [[Attack (command)#Final Fantasy VII|Attack command]], and the character can unleash Limit Breaks that are unique to them.

The player can customize their characters' [[Final Fantasy VII weapons|weapon]], [[Final Fantasy VII armor|armor]], [[Final Fantasy VII accessories|accessory]], and Materia. The Materia they can equip is determined by how many Materia slots are available on their equipped weapon and armor. Since the Materia-provided abilities determine the party member's skills, and by extension their role and functionality, party members can be heavily customized to any playstyle, and their individuality derives mostly by their unique Limit Breaks, their [[Final Fantasy VII stats|stat]] differences, and later in the game, their [[Ultimate weapon (term)#Final Fantasy VII|ultimate weapon]].

The ATB system in ''Final Fantasy VII'' is based on the combat system introduced in ''[[Final Fantasy IV]]''. ''Final Fantasy VII'' is the first to utilize this system with only three party members, rather than four or five. The Limit Break system is also similar to [[Desperation Attack]]s from ''[[Final Fantasy VI]]'', with a different mechanic (where Desperation Attacks appear when a party member is on low [[HP (stat)|HP]], rather than being determined by a gauge). Most of the magic spells, summons, and special abilities were introduced in previous titles. Later, the [[Final Fantasy VII Remake battle system|battle system]] in ''[[Final Fantasy VII Remake]]'' adopted many of the battle mechanics from ''Final Fantasy VII'', while fusing it with real-time combat.

==Mechanics==
===Overview===
[[File:Cloud- Meteorain.jpg|thumb|right|Cloud's Meteorain ability.]]
''Final Fantasy VII'' makes use of the Active Time Battle system with three controllable party members at a time. Both party members and [[Final Fantasy VII enemies|enemies]] have an ATB gauge (only the player party member's gauges are visible), and are able to act once their gauge fills. By default the actions for party members are Attack (performing a basic physical attack), Item (using an [[Final Fantasy VII items|item]] in the player's inventory), Defend (guarding to reduce damage until their next action), and Change (to change to the front or back [[row]]). The player can also hold two buttons to try to Escape from the battle, receiving no [[Battle Results|rewards]].

There are three modes to ATB: Active, Wait, and Recommended; the third is the default. In Active mode, ATB continues to fill regardless of what is happening in-game, but will still pause for Summon animations. In Wait mode, time also stops when the player is targeting or navigating sub-menus to select an Item, Magic, or Summon for instance. Recommended is a blend of the two, where time stops only during battle animations.{{note|Each turn, time pauses briefly after another action in battle is queued or a party member reaches full ATB, with the pause duration depending on the battle speed setting. The pause cannot be triggered by a party member that was already at full ATB. Limit Breaks and counter moves also will not update the pause trigger due to their unique animation priority. Wait Mode also uses Recommended Mode's mechanics.}} These settings are effectively [[Difficulty level|difficulty settings]], where Wait is the easiest, Recommended is medium, and Active is the hardest.

The player arranges a party formation in the menu, and can change each member's row. Party members in the back row halve physical damage taken, but also deal 50% less physical damage than in the front row unless their attack is classed as [[Long Range|long range]], provided by certain weapons or abilities. For some party members, all or the vast majority of their weapons are long range, while for others, long range can only be achieved through Materia. Limits, Magic, Summons, and Items are unaffected by row.

Battles begin as [[random encounter]]s in certain fields, or when [[boss]] battles occur, usually at the end of a story beat. Depending on the randomly encountered [[Final Fantasy VII enemy formations|enemy formation]] or the boss battle, a battle may occur in one of five [[attack formations]]: a normal formation (three party members face down enemies of an opposing side), Preemptive Strike (same as before, except the player's party attacks first), Back Attack (where the party's row is reversed, and the enemies attack first), Ambush (where two groups of enemies surround the party either side), and Side Attack! (where the party is split in half, but surrounds the enemy). This can be advantageous or disadvantageous, depending on the player's strategy.

When a party member reaches 0 HP, they are [[Knocked Out|KO'd]] until revived by a spell, summon, item or actions taken on the field. If all party members are KO'd, it is [[Game Over (term)|Game Over]], and the player must return to their previous save file.

===Random encounters===
{{See|Random encounter}}
[[File:FFVII Aeris Attack.png|thumb|Random battle on the field, that is an Ambush.]]
Different field areas (such as in [[dungeon]]s) and the different areas on the [[world map]] (for which the name of the area appears in the [[Menu (Final Fantasy VII)|menu]]) have different encounter rates. Each area can support up to six normal battles and four special [[#Formation|formation]] battles (of three types: Back Attack, Side Attack!, and Attack from both sides!). World map areas can support a further four [[Chocobo (Final Fantasy VII enemy)|Chocobo]] battles.

When moving in an area on the field, a variable called the <code>Field Danger Counter</code> increments, and a battle check occurs. This happens roughly three times per second when moving, but the rate at which it increases depending on the area's <code>Encounter Value</code>, where higher values are safer and lower values have more frequent encounters. Walking increases the <code>Field Danger Counter</code> by <math>[1024 / Encounter Value]</math> per battle check, and running increases it by <math>[4096 / Encounter Value]</math> per check.{{note|As running is four times faster than walking, the distance covered will be about the same. However, the number of times the battle check is called is lower over the same distance, meaning the number of enemies encountered while running may be marginally smaller.}}<ref name="tfergusson">{{ref|id=tfergusson|year=2009|author=Terence Fergusson|title= Final Fantasy VII – Battle Mechanics FAQ|url=https://gamefaqs.gamespot.com/ps/197341-final-fantasy-vii/faqs/22395|platform=GameFAQs}}</ref>

The higher the current <code>Field Danger Counter</code>, the more likely a random battle will begin once the battle check occurs. The battle check performed is if <code>Rnd(0..255)</code> (a random number between 0 and 255) is less than <math>\left [ \frac{Danger Counter \times Enemy Lure Value}{256} \right ]</math>. The <code>Enemy Lure Value</code> is a fraction between 2/16 to 32/16, and is 16/16 (100%) by default, but can be increased or reduced by equipping the [[Enemy Lure (Final Fantasy VII)|Enemy Lure]] or [[Enemy Away (Final Fantasy VII)|Enemy Away]] Materia, respectively. When a random battle occurs, the <code>Field Danger Counter</code> is reset.

[[File:Chocobo encountered ff7.png|thumb|left|World map random battle with a Chocobo enemy.]]
The world map has its own <code>Danger Counter</code> variable, incremented by <math>\left [\frac {Enemy Lure Value \times 16384}{Encounter Value} \right ]</math> per check. The <code>Enemy Lure Value</code> doubles if it is greater than 16/16 (twice has much as it does on the field). Compared to normal location fields, battle checks occur less often on the world map, but the <code>Danger Counter</code> has a higher increase. A random battle begins if <code>Rnd(0..255)</code> is less than <math>[Danger~Counter / 256]</math>. The <code>Danger Counter</code> on the world map resets when leaving or entering it, or when submerging while in the [[Submarine (Final Fantasy VII transportation)|submarine]].

Random encounters occur throughout most of the world map, aside from the forest around [[North Crater]], the forest around [[Bone Village (field)|Bone Village]], the coral area between the [[Sleeping Forest]] side of the [[Corel Valley]] and the [[City of the Ancients]], or on the bridges in [[Wutai Area]]. Random encounters occur while riding the [[buggy]], but not on a [[Chocobo (Final Fantasy VII transportation)|Chocobo]], submarine, the ''[[Tiny Bronco]]'', or the ''[[Highwind (Final Fantasy VII transportation)|Highwind]]'' ([[Ultimate Weapon (Final Fantasy VII)|Ultimate Weapon]] battles in its resting spots are the only exception). Encounters are divided by the world map area one is in (displayed in the menu), and each area has four sets: for grass, dirt or snow, beach, and miscellaneous. Miscellaneous terrain is often used for forests or deserts, though it is also used for the canyon terrain in the [[Cosmo Area]] and the [[Great Glacier (Final Fantasy VII)|Great Glacier]] Snow Field terrain considered a part of the [[Icicle Area]].

When a random battle occurs, the following are considered in order:
[[File:Yuffie Encounter.jpg|thumb|Mystery Ninja is a unique encounter in a forest on the world map.]]
*Unique battle
**[[Mystery Ninja]] can be encountered in forests while [[Yuffie (Final Fantasy VII party member)|Yuffie]] is not recruited.
**While [[Chocobo Lure]] Materia is equipped, [[Chocobo (Final Fantasy VII enemy)|Chocobos]] can be encountered.
*Pre-emptive chance
**The base chance of getting a preemptive battle is 16/256.
**A mastered [[Pre-Emptive (Final Fantasy VII)|Pre-Emptive]] Materia boosts this up to 64/256. The maximum is 85/256, roughly a 1/3 chance.
**Some boss battles can be fought as pre-emptive battles if they have been flagged as "pre-emptive attack is possible, escape is impossible, battle finishes with a [[Final Fantasy VII victory poses|victory fanfare]]". As the only screens allowing a pre-emptive attack are those where random battles can occur, it is impossible to pre-empt boss battles such as [[Rufus (Final Fantasy VII boss)|Rufus]], since no random encounters can be fought in the screen where he is fought (even if the battle itself is flagged to enable pre-emptive).
**Likewise, if random battles are temporarily disabled on a screen, the player will never get a preemptive attack for any battles until random battles are enabled. As such, the only boss battles that can have a preemptive attack are [[Aps]] in the [[Sewer (Final Fantasy VII field)|sewers]], [[Jenova∙BIRTH]] in [[Cargo Ship (Final Fantasy VII field)|Cargo Ship]], [[Turks:Reno]] with [[Turks:Rude]] in [[Gongaga (Final Fantasy VII field)|Gongaga]], [[Snow (Final Fantasy VII)|Snow]] in the [[Great Glacier (Final Fantasy VII field)|Great Glacier]] cave, and [[Turks:Elena|Elena]], Turks:Reno, and Turks:Rude in Midgar winding tunnel.{{note|The three fixed battles on the first screen in the [[Cave of the Gi (field)|Cave of the Gi]] with the [[Gi Spector (Final Fantasy VII)|Gi Spector]]s should also have made use of the 'Pre-Emp OFF' flag, seeing as the other randomly encountered Gi Spector cannot be back-attacked. The same applies to the forced encounter with the [[Attack Squad]]s at [[Mt. Corel (Final Fantasy VII)|Mt. Corel]]'s reactor on Part 2 that can also be pre-empted.}}
*Special formations
**On the field: if the area has any special [[#Formation|formation]] battles (Back Attack, Side Attack!, or Attack from both sides!), the chance of getting them is calculated. The game rolls a <code>Rnd(0..63)</code> (random number between 0 and 63) to go through all possible formation battles for the area in the following order: Back Attack, Side Attack!, and Attack from both sides!. The number determines if a special formation occurs, and which one.
**On the world map, instead of a single random value being used for all types of special attack formations, they are checked individually. Therefore, the chance of not getting a special formation on the world map is not the sum of the chances, but instead the three chances of not getting a Back Attack, not getting a Side Attack!, and not getting an Attack from Both Sides multiplied together.
*Normal battles
**If neither a special formation battle nor Chocobo battle happens, normal battles occur. A single <code>Rnd(0..63)</code> is rolled to decide which battle is chosen.
**If the same battle formation is rolled as last time, the game will reroll once for a new normal battle. If it lands on the same battle again, no further rerolls are done. This means the chance of getting the same battle twice is reduced, but not eliminated.

Due to the [[inescapable battles glitch]], some random encounters are impossible to flee from. Battle encounters are skipped if the player opens the menu right when the battle was meant to initiate. This can lead to the [[repeating boss glitch]], where the skipped random battle results in the player fighting a boss twice. Although not a random encounter, if the player [[Diamond Weapon glitch|skips the Diamond Weapon boss]], they will become stranded, unable to finish the game, as the scene where the [[Sister Ray]] is fired never triggers.

[[Final Fantasy VII version differences|Encounter rates were higher]] in the [[Final Fantasy VII version differences|original Japanese version]], and in following releases all areas' encounter rates have been reduced by varying amounts. Also in all later releases, random encounters have been removed from the square area of [[Sector 8 (Final Fantasy VII field)|Sector 8]] at the start of the game, and outside of [[Holzoff]]'s cabin at the base of [[Gaea's Cliff (field)|Gaea's Cliff]].

===Formation===
The typical formation in a normal battle is the player's party stands in a line facing the opponents. In addition to this, areas have a number of possible special formation battles, and when a random encounter begins, the chance of getting these is applied first. The special formations mostly put the player at a disadvantage, though a mastered Pre-Emptive Materia can halve the chance for the disadvantageous formations; detecting an equipped mastered Pre-Emptive Materia will cause the game to apply a bonus when cycling through special formations.{{note|Two checks are used to detect a mastered Pre-Emptive Materia: a single character check and a global check. The single character check is performed whenever Materia or equipment are changed on a character, and none of the other characters are looked at. If the character whose Materia was changed does not have a Pre-Emptive Materia equipped when the menu is closed, the game decides the party does not have one and removes the Pre-Emptive bonus, irrespective of if another party member has it equipped. The global check is used when entering the menu from the world map or field screen, or when exiting [[PHS]]. This one checks the last character (bottommost) for the Pre-Emptive Materia. Therefore, it is best to place the Pre-Emptive Materia on the third character in the party.}}

After checking if the Pre-Emptive Materia's bonus is applied, when checking for formations, the game goes through all four possible special formation battles for that area in the following order (rolling a <code>Rnd(0..63)</code> to cycle if a special formation occurs, and which one):<ref name="tfergusson"/>
#'''Back Attack''' #1 and #2: The party begins facing away from the enemy (unless equipped with [[Sprint Shoes (Final Fantasy VII)|Sprint Shoes]]) and thus taking double damage from the first attack, with their normal formation reversed (party members in the back row start in the front row, and vice versa). The enemies always get the first attack, and there is no chance for it being a pre-emptive battle. A mastered Pre-Emptive Materia bonus halves the chance of this occurring, and by holding the Escape buttons and trying to escape, the party can immediately reverse.
#'''Side Attack!''': The party is split in half, surrounding the enemies in the middle. Pre-Emptive battle chance is normal for this formation; the player can get a pre-emptive attack and begin with all ATB bars filled while the enemies have theirs delayed.
#'''Attack from both sides!''': The party is surrounded by two enemy groups on either side. The party begins with their ATB bars empty, while enemies gain a slight boost to their ATB bars. A mastered Pre-Emptive Materia halves the chance of this battle formation occuring.

Some special encounters and boss battles have unique formation rules:
*Back Attack rules still apply in some special battles that are not Back Attacks, and some of the Back Attack rules still apply: the Change command to change [[row]] is grayed out, the enemies start with an empty ATB gauge, and one's first physical attack will deal double damage. This happens in the battles fought as [[Yuffie (Final Fantasy VII party member)|Yuffie]] in the [[Wutai (Final Fantasy VII field)|Wutai]] pagoda, and [[Cloud (Final Fantasy VII party member)|Cloud's]] final showdown against [[Sephiroth (Final Fantasy VII boss)|Sephiroth]].
*Though most enemies take double damage from back attacks, some bosses take more: [[Air Buster (Final Fantasy VII)|Air Buster]] takes five times normal damage, Aps quadruple damage, and [[Acrophies (Final Fantasy VII)|Acrophies]] eight times normal damage.
*The battle with [[Ruby Weapon (Final Fantasy VII)|Ruby Weapon]] will always start as if the party has been back-attacked. Although no back attack message will appear, the party starts with an empty ATB gauge and Ruby Weapon gets a double physical damage bonus until it is successfully attacked.

[[Cait Sith (Final Fantasy VII party member)|Cait Sith's]] [[Slots (Final Fantasy VII)|Slots]] [[Limit (Final Fantasy VII)|Limit Break]] has two outcomes that usually end the battle: Death Joker and Game Over. Death Joker usually kills all allies, and Game Over usually kills all enemies. If the battle is a Side Attack! or Attack from both sides!, however, the attacks can only target one enemy or ally party.

===Stats===
Party members and enemies each have a level, HP, MP, and a set of stats that determines their performance in combat (namely how much damage they take and receive). Party members level up by earning EXP to fill the requirement for the next level (which is slightly different per party member). Enemies' stats and levels are pre-determined. HP refers to how much damage can be taken before being KO'd, and MP refers to how many spells or summons can be cast.

Party members have a unique set of primary stats, most of which are used to determine their core derived stats that are used in battle. The primary stats, along with the HP and MP, increase on leveling up. Stats can also be permanently increased by using stat-upgrading items, or by certain equipment, including Materia. The primary stats are:
*'''Strength''': Determines the Attack stat, meaning characters with higher Strength have stronger physical attacks. Increased by Power Source.
*'''Dexterity''': Determines the Defense% stat, meaning characters with higher Dexterity are more likely to evade (less likely to get hit by) physical attacks. It also increases the speed ATB fills at, meaning the units are ready to execute commands faster. A quarter of the Dexterity stat, plus the equipped weapon, determines the Attack% stat. Increased by Speed Source.
*'''Vitality''': Determines the Defense stat, meaning characters with higher Vitality have stronger physical resistance. Increased by Guard Source.
*'''Magic''': Determines the Magic atk stat, meaning characters with higher Magic have stronger spells (both in terms of damage and healing). Increased by Magic Source.
*'''Spirit''': Determines the Magic def stat, meaning characters with higher Spirit have more magic resistance. Increased by Mind Source.
*'''Luck''': Used in calculations to determine if one of three things occur: a "Lucky Hit" (compare attacker's Luck with target's Luck; if reached, 100% chance the target will be hit with a normal physical attack), a "Lucky Evade" (compare attacker's Luck with target's Luck; if reached, 100% chance an attack will be evaded, if evadable) or a "critical hit" (if reached, deal approximately double damage). Increased by Luck Source. Enemies have pre-determined Luck stats.

Both enemies and party members have a core set of stats used in damage calculations. For party members, this is derived both from the equipment and from their primary stats; for enemies, this is pre-determined. These stats are:
*'''Attack''': Determines the strength of a physical attack, in the following formula: Base Damage = Attack + ((Attack + Level) / 32) * ((Attack * Level) / 32).
*'''Attack%''': Determines the accuracy of a physical attack, in the following formula: Hit% = (Attacker's Dexterity / 4 + Attack%) + Attacker's Defense% - Target's Defense%.
*'''Defense''': Determines the resistance to a physical attack and reduces base damage from a physical attack, in the following formula: Damage = (Power * (512 - Defense) * Base Damage) / (16 * 512).
*'''Defense%''': Determines the chance of evading a physical attack, in the following formula: Hit% = (Attacker's Dexterity / 4 + Attack%) + Attacker's Defense% - Target's Defense%.
*'''Magic atk''': Determines damage dealt by a magic damage, in the following formula: Base Damage = 6 * (Magic atk + Level). After this, the spell's own ability power is taken into account to determine final damage, in the formula: Final Damage = (Ability Power / 16) * Base Damage.
*'''Magic def''': Determines the resistance to a magic attack and reduces base damage from a magic attack, in the following formula: Damage = (Power * (512 - Magic def) * Base Damage) / (16 * 512).
*'''Magic def%''': Determines the chance to just avoid an attack entirely. Unlike other stats, Magic def% is a pure percentage-based stat, with no formula involved.

Other effects and statuses can increase or reduce damage outside of the damage formula determined by stats. For instance, Hero Drink can increase damage dealt beyond stat values, as can a variety of effects from weapons, while Barrier can reduce physical damage irrespective of status.

===Elements===
''Final Fantasy VII'' has a number of elements that apply to attacks. The element of an attack determines the damage it deals based on if the target is weak, resistant, immune to, or absorbs damage of an element.

There are two types of elements: magical and physical. Magical elements are usually associated with Magic and Summon Materia have elements innate to their attacks, or that can be applied to the attacks or resistance of the wearer when paired with Elemental Materia. Physical elements are applied to characters' weapons, which each have a physical element that relates to their weapon's damage type. Both magical and physical elements apply to the attacks dealt by enemies.

The magical elements are as follows:
*'''Fire''', associated with the Fire Materia, as well as Ifrit and Phoenix. Strong against Ice enemies.
*'''Ice''', associated with the Ice Materia, as well as Shiva. Strong against Fire enemies.
*'''Lightning''', associated with the Lightning Materia as well as Ramuh. Strong against aquatic enemies and machines.
*'''Earth''', associated with the Earth Materia as well as Titan. Though Earth-elemental spells deal stronger damage than spells of equivalent elements, they typically miss against flying enemies.
*'''Wind''', which has no associated Magic Materia, but is associated with Choco/Mog and can be used from a few other abilities. Effective against flying enemies.
*'''Water''', which has no associated Magic Materia, but is associated with Leviathan and can be used from a few other abilities.
*'''Poison''', associated with the Poison Materia as well as Hades. Poison is closely associated with the damage of the Poison status effect.
*'''Holy''', which has no associated Magic Materia and can only be used from the Alexander summon.
*'''Gravity''' associated with the Gravity Materia. Gravity deals damage based on the percentage of the target's HP, and is typically ineffective against bosses but strong against normal enemies.
*'''Restorative''', referring to any ability designed to cure HP, such as spells from the Restore Materia, rather than deal damage. Most targets simply recover HP when Restorative spells are used on them, but some undead enemies instead take damage from them.
*'''Hidden''', an element which is never officially named. A few enemy abilities deal damage based on the element, and pairing Elemental Materia with any Materia of no element grants a party protection against these attacks.

Physical elements are as follows:
*'''Cut''', associated with blades and slashing. It is the sole damage type for Cloud's weaponry, as well as a few of Barret's and one of Cid's weapons.
*'''Hit''', associated with blunts and trauma. It is the sole damage type for Aeris and Cait Sith's weapons, the damage type for all but one of Red XIII's weapons, as well as the primary damage type for Tifa's weapons, two of Barret's weapons, and one of Cid's.
*'''Punch''', associated with piercing weapons. Tifa and Barret each have three weapons that use the element, namely Tifa's claws.
*'''Shoot''', associated with ranged weapons. It is the sole damage type for Yuffie and Vincent's weaponry, the primary damage type of Barret's, and is possessed by one of Red XIII's.
*'''Shout''', associated with sonic attacks. No characters' weapons use this element, and only some enemy abilities deal damage based on the element.

Though most attacks have at least one element, some abilities, and a lot of Limit Breaks, deal entirely non-elemental damage.

===Statuses===
Party members and enemies can be afflicted with certain status effects, which can be both positive and negative depending on the status in question. Enemies can be immune to certain statuses, and certain equipment, including various accessories and the Added Effect Materia, can provide party members with the same immunity. Added Effect Materia can also be used by party members to inflict several statuses. A number of spells can remove statuses: Esuna removes most negative effects, DeBarrier removes a few positive effects, and DeSpell removes any positive effect.

The status effects are as follows:
*'''Death''', when a party member reaches 0. It is associated with the Destruct Materia and Odin.
*'''Near-death''', when a party member's HP is below 25% max health. This typically only has an aesthetic effect, where party members' HP digits turn yellow and they appear exhausted in their animations, though it counts as a status effect for Tifa's Master Fist and Powersoul weapons, doubling her damage.
*'''Sleep''', which renders a target unable to act until awoken (by a physical attack or by curing the status), but their ATB is not frozen. It is associated with the Seal Materia.
*'''Poison''', which causes a target to take 1/32 of their Max HP in Poison-elemental damage every 2.5 units of time in-game. It is associated with the Poison Materia.
*'''Sadness''', which causes a party member to take 30% less damage from physical and magical attacks, but halves the rate their Limit gauge fills. This is opposed to Fury, and indicated by a blue Limit bar.
*'''Fury''', which reduces the character's hit rate for both physical and magical attacks by 30%, but doubles the rate their Limit gauge fills. This is opposed to Sadness, and is indicated by a red Limit bar.
*'''Confusion''', causing a target to lose control of their actions and use random attacks against their allies. It is associated with the Mystify Materia.
*'''Silence''', which renders a target unable to use the Magic or Summon commands (as well as the associated W-Magic and W-Summon). It is associated with the Seal Materia.
*'''Haste''', which doubles the rate a target's ATB gauge fills, but also doubles the speed at which time-based effects run out. It can be applied by a number of abilities, including the Time Materia's Haste spell and the Big Guard ability from the Enemy Skill Materia. It is opposed to Slow.
*'''Slow''', which halves the rate a target's ATB gauge filled, and halves the speed time-base effects run out. It is associated with the Time Materia.
*'''Stop''', which freezes the the target's ATB gauge, as well as time-based effects. It is associated with Time Materia, Contain Materia, and Choco/Mog.
*'''Frog''', which prevents all commands except Attack, Item, and the Toad spell, and reduces damage to a quarter of their base damage. It is associated with the Transform Materia.
*'''Small''', which reduces a target's Attack stat to 1, causing them to only deal 1 damage with an attack. It is associated with the Transform Materia.
*'''Slow-numb''', which causes a target to enter the Petrify status after 60 seconds. It is only used by enemy attacks.
*'''Petrify''', which flags a target as defeated (similar to Death) unless it is cured before the battle ends. It is associated with the Contain Materia.
*'''Regen''', which heals a character by 1/32 of their max HP every 4 seconds. The status is irrelevant to the Restorative status, and can still heal undead enemies. It can be applied by a the Regen spell.
*'''Barrier''', which halves all physical damage taken. It can be applied by the Barrier and Wall spells from the Barrier Materia, along with a variety of other abilities including Big Guard.
*'''MBarrier''', which halves all magical damage taken. It can be applied by the MBarrier and Wall spells from the Barrier Materia, along with a variety of other abilities including Big Guard.
*'''Reflect''', which causes most spells cast against targets to bounce off onto the opposing party, up to four bounces. Reflect can be positive in preventing enemy spells from hitting the party, but will also prevent healing spells from working. It can be applied by a variety of sources, including the Reflect spell from the Barrier Materia.
*'''Dual''', a dummied status effect which does nothing.
*'''Shield''', which causes all physical attacks to deal no damage, and absorbs elemental damage. Targets can still be damaged by special non-elemental attacks and items. It can be applied by the Shield spell from the Shield Materia.
*'''Death-sentence''', which causes a target to be instantly killed after 60 seconds. It can be applied by the Death Sentence Enemy Skill, and is automatically applied by Curse Ring (which can be beneficial to quadruple Tifa's damage with Powersoul).
*'''Manipulate''', which allows a party member to control an enemy's actions. It can be used by the Manipulate Materia.
*'''Berserk''', which increases a target's physical damage by 50%, but causes them to only perform physical attacks against a random enemy, and be unable to control their actions. It is associated with the Mystify Materia.
*'''Peerless''', which makes a character immune to all physical and magical attacks for a short time, including ones that remove MP. It is only used by Aeris's Planet Protector and Great Gospel Limit Breaks.
*'''Paralyzed''', which causes a target to freeze, unable to act. It is shorter than Stop, and party members' Limit gauges do not fill when they are attacked while Paralyzed. A number of enemy attacks, a few items, and Cloud's Cross-slash can inflict the status.
*'''Darkness''', which halves the accuracy of target's physical attacks. This mostly applies to the party member's abilities, and has little impact on enemies. The status is mostly inflicted by enemy abilities.
*'''Seizure''', which causes a party member to lose HP over time. It is inflicted by the Waterball attack from Bottomswell.
*'''Death Force''', which prevents all forms of Instant Death for the rest of the battle. It is used by the Death Force Enemy Skill.
*'''Resist''', which locks in a target's current status effects (both positive and negative). It is granted by the Resist spell from the Heal Materia.
*'''Lucky Girl''', which causes all party member's attacks to be critical hits. It is granted as a possible combination from Cait Sith's Slots Limit Break.
*'''Imprisoned''', which immobilizes a target and flags them as defeated (though their current HP is retained). It can only be applied by certain boss abilities, and removed from destroying the entrapment.
*'''Back Row''', when a party member is placed in the back row, and thus physical damage dealt is halved, but so is physical damage taken.
*'''Defend''', when a party member uses Defend. It halves physical damage taken until their next turn.
*'''All Lucky 7s''', a rare effect when a party member's HP is 7777. This causes all their attacks to automatically deal 7777 damage.

===Battle rewards===
After the battle's end, a results screen displays the gained EXP, which fills up a gauge with the character's menu portrait next to it. When full, the character levels up. Gained Limit Breaks are also displayed on the screen. The Materia gain AP, which can boost their level to either grant them a new ability, or improve an existing one. If a Materia reaches its maximum level, it is mastered, and an identical unleveled Materia is split off from it. When a Materia levels up and gains a new ability, it is displayed in the rewards screen. The party also receives gil from battles that displays on the screen.

In addition, there is a chance that an enemy will drop an item, and players can choose to take specific ones, take all, or reject some. Typically, items dropped are recovery items (such as Potions and Ethers), offensive items (such as Grenades), or Chocobo breeding items (nuts and greens). Enemies never drop Materia in the results screen, but may drop one onto the field after the battle for the player to pick up. Enemies drop an item if they are killed (and not taken out with Remove) and the battle is properly finished, as long as the enemy has not been successfully stolen from, and the battle has a results screen (battles at the Battle Square have no screen).

An enemy that can drop items upon winning has a list of items with separate chances of being dropped. The numerical chance of each item, and the order in the enemy's list, are both relevant to the item dropped. For example, a Razor Weed has the following list:
: [8] Tranquilizer, [8] Hi-Potion, [8] Loco weed
When an item drops, each item in the list is looked at, and will be awarded if Rnd(0..63) <= Chance of Item (if a random number between 0 and 63 is greater than the chance of that item), meaning there is a (Chance + 1)/64 probability that the item being looked at drops. It will then be looked at in the following order:
* 9/64: Tranquilizer drops
* 55/64: Moves to next item
** 9/64: Hi-Potion drops
** 55/64: Move to next item
*** 9/64: Loco weed drops
*** 55/64: No item drops
As such, the overall chance of each item is:
* Tranquilizer: 9/64 chance, 14.06%
* Hi-Potion: 495/4096 chance, 12.08%
* Loco weed: 27225/262144 chance, 10.39%
* Nothing: 166375/262144 chance, 63.47%
This is because, even though each item has the same individual percentile chance, the earlier items in the list are more likely to drop. Some special enemies (including Mover, Magic Pot, as well as the storyline boss Proud Clod, and the optional bosses Ultimate Weapon, Emerald Weapon, and Ruby Weapon) have guaranteed item drops.

If the player defeats Emerald Weapon when there are eight seconds left on the timer, the Battle Results screen will overlap with the Game Over screen, resulting in the Emerald Weapon countdown glitch.

==Party members==
There are nine party members in ''Final Fantasy VII'', and one temporary party member: Cloud, Barret, Tifa, Aeris, Red XIII, Yuffie, Cait Sith, Vincent, Cid, and (temporary) Sephiroth. (Per-character playstyle/weapon/Limit-Break/ultimate-weapon notes omitted here for brevity — see the live wiki page for full per-character detail if needed.)

===Abilities===
Party members by default have the Attack, Item, Defend, and Change abilities. A large variety of abilities are then provided by equipped Materia. Equipping Magic Materia adds the Magic command, giving access to any spells provided by the equipped Magic Materia. Similarly, equipping Summon Materia adds the Summon command that allows them to summon monsters in battle. On top of this, a wide variety of abilities can be provided by Command Materia, each of which provides either an additional command to the menu, or replaces the default Attack command.

The only abilities unique to a character are their Limit Breaks. When a character's Limit gauge is filled, the Attack command is replaced by Limit, allowing them to unleash one of their unique Limit Breaks depending on their current Limit level. Higher Limit levels are more powerful, but at a higher level, the Limit gauge takes longer to build.

===Equipment===
Party members have three pieces of equipment: a weapon, an armor piece, and an accessory. These are acquired throughout the story when they are purchased from shops, dropped from enemies, or when completing quests.

Weapons are unique to each character, and provide stat boosts and Materia slots. Some weapons also have unique properties and mechanics to do additional damage, especially ultimate weapons, which can define a characters' playstyle and Materia build entirely. Characters can only equip weapons of their weapon type, though most characters have rough equivalents of a lot of weapons throughout the game in terms of similar stats and Materia slots.

Armor and accessories are not unique, and provide different roles. Armor provides Materia slots and increase in defensive stats, and sometimes provides unique properties (such as absorbing or reducing an element). Accessories provide unique effects and properties, which can come in the form of stat bonuses, immunities to statuses, or other improvements.

===Materia===
Materia is used to customize and tailor a party member's abilities, and thus their role and playstyle. Materia either provide them with command abilities, or other enhancements. This means the player can entirely alter the way a party member functions in battle, and determine how they are used. The number of Materia available to a party member depends on their equipped weapon and armor; when wanting to equip more Materia, party members must find a weapon or armor piece with more slots. Materia are either purchased from shops or earned from various quests.

There are five types of Materia:
* Magic Materia, used to grant spells for the Magic command. Spells cost MP to cast, and can come in the form of restorative spells such as Cure, offensive spells such as Fire, and spells applying statuses, such as Haste.
* Command Materia, used to grant abilities to a character. These can either be entirely additional commands on the menu, such as Steal, or ones that replace the Attack command, such as Double Cut. The one unique Command Materia is the Enemy Skill Materia, allowing skills learned from enemies to be used.
* Support Materia, which can be paired with other Materia in a linked slot to enhance the linked Materia or take advantage of its properties.
* Independent Materia, granting passive effects to the user which can come in the form of improved stats, such as HP Plus, or providing new passive abilities, such as Counter Attack.
* Summon Materia, granting the ability to summon a monster in battles, which has its own summon animation that lasts longer than a normal spell. Practically, they function very similarly to Magic Materia.

==Enemies==
When a battle begins, a group of enemies is fought from one of a set number of enemy formations. This determines if the attack is a special formation, and which enemies are fought in that encounter.

Enemies each have a set of stats that determine the damage they deal and receive. In addition, enemies have elemental properties including resistances, weaknesses, and immunities, as well as possible immunities to many status effects. Enemies also have items that can be dropped, stolen, or earned from morphing them.

==Source==
https://finalfantasy.fandom.com/wiki/Final_Fantasy_VII_battle_system
(Terence Fergusson's "Final Fantasy VII – Battle Mechanics FAQ" is cited
throughout as the primary source for the numeric formulas above.)

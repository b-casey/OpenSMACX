/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2020 Brendan Casey
 *
 * OpenSMACX is free software: you can redistribute it and / or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenSMACX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenSMACX. If not, see <http://www.gnu.org/licenses/>.
 */
#include "stdafx.h"
#include "temp.h"
#include "veh.h"
#include "alpha.h"
#include "base.h"
#include "faction.h"
#include "game.h"
#include "general.h" // range
#include "log.h"
#include "map.h"
#include "strings.h"
#include "technology.h"
#include "terraforming.h"

veh_prototype *VehPrototype = (veh_prototype *)0x009AB868; // [512]
veh *Veh = (veh *)0x00952828; // [2049]
rules_chassis *Chassis = (rules_chassis *)0x0094A330; // [9]
rules_weapon *Weapon = (rules_weapon *)0x0094AE60; // [26]
rules_armor *Armor = (rules_armor *)0x0094F278; // [14]
rules_reactor *Reactor = (rules_reactor *)0x009527F8; // [4]
rules_ability *Ability = (rules_ability *)0x009AB538; // [29]
rules_morale *Morale = (rules_morale *)0x00952328; // [7]
rules_combat_mode *DefenseModes = (rules_combat_mode *)0x00946A00; // [3]
rules_combat_mode *OffenseModes = (rules_combat_mode *)0x00946178; // [3]
rules_order *Order = (rules_order *)0x0096C878; // [9]
LPSTR *PlansShortName = (LPSTR *)0x00945FE0; // [15]
LPSTR *PlansFullName = (LPSTR *)0x00952360; // [15]
LPSTR *Triad = (LPSTR *)0x0094F1A8; // [3]

int *VehCurrentCount = (int *)0x009A64C8;
int *VehDropLiftVehID = (int *)0x009B2280;
int *VehLift_xCoord = (int *)0x009B2278;
int *VehLift_yCoord = (int *)0x009B2284;
BOOL *VehBitError = (BOOL *)0x009B228C;
uint32_t *VehBasicBattleMorale = (uint32_t *)0x00912420; // [2] ; [0] offense, [1] defense?
int VehMoraleModifierCount; // only used by say_morale(), optimize to local var?
// Battle related globals
uint32_t *VehBattleModCount = (uint32_t *)0x00915614; // [2]
BOOL *VehBattleTglUnk = (BOOL *)0x0091561C; // [2] ; planet_busting() + boom() + timers
int *VehBattleModifier = (int *)0x009155F0; // [2][4]
LPSTR *VehBattleDisplay = (LPSTR *)0x0090F554; // [2][4][80]
LPSTR VehBattleDisplayTerrain;

/*
Purpose: Craft an output string related to the specified Veh's morale.
Original Offset: 004B3FD0
Return Value: n/a
Status: Complete
*/
void __cdecl say_morale(LPSTR moraleOutput, uint32_t vehID, int factionIDvsNative) {
	uint32_t morale = morale_veh(vehID, false, factionIDvsNative), factionID = Veh[vehID].factionID;
	int protoID = Veh[vehID].protoID;
	std::string output = (protoID < MaxVehProtoFactionNum &&
		(Weapon[VehPrototype[protoID].weaponID].offenseRating < 0 || protoID == BSC_SPORE_LAUNCHER))
		? StringTable->get((int)Morale[morale].nameLifecycle)
		: StringTable->get((int)Morale[morale].name);
	if (VehPrototype[protoID].plan < PLAN_COLONIZATION) {
		uint32_t moralePenalty = 0;
		int homeBaseID = Veh[vehID].homeBaseID;
		if (homeBaseID >= 0 && Base[homeBaseID].state & BSTATE_DRONE_RIOTS_ACTIVE && morale > 0
			&& !(Players[factionID].ruleFlags & RFLAG_MORALE)) {
			output += " (-)";
			moralePenalty = 1;
		}
		VehMoraleModifierCount = 0;
		int baseID = base_at(Veh[vehID].xCoord, Veh[vehID].yCoord);
		if (baseID >= 0 && morale < 6) {
			if (has_fac_built(FAC_CHILDREN_CRECHE, baseID)) {
				VehMoraleModifierCount++;
				int moraleSEActive = range(PlayersData[factionID].socEffectActive.morale, -4, 4);
				if (moraleSEActive <= -2) {
					moraleSEActive++;
				}
				if (moraleSEActive < 0) {
					int moraleCap = morale + VehMoraleModifierCount;
					do {
						if (moraleCap >= 6) {
							break;
						}
						VehMoraleModifierCount++;
						moraleCap++;
						moraleSEActive++;
					} while (moraleSEActive < 0);
				}
				if(has_fac_built(FAC_BROOD_PIT, baseID) && protoID < MaxVehProtoFactionNum
					&& (Weapon[VehPrototype[protoID].weaponID].offenseRating < 0 
						|| protoID == BSC_SPORE_LAUNCHER)) {
					VehMoraleModifierCount++;
				}
			}
			else if (morale < 6 && has_fac_built(FAC_BROOD_PIT, baseID)
				&& protoID < MaxVehProtoFactionNum
				&& (Weapon[VehPrototype[protoID].weaponID].offenseRating < 0
					|| protoID == BSC_SPORE_LAUNCHER)) {
				VehMoraleModifierCount++;
				int moraleSEActive = range(PlayersData[factionID].socEffectActive.morale, -4, 4);
				if (moraleSEActive <= -2) {
					moraleSEActive++;
				}
				if (moraleSEActive < 0) {
					int moraleCap = morale + VehMoraleModifierCount;
					do {
						if (moraleCap >= 6) {
							break;
						}
						VehMoraleModifierCount++;
						moraleCap++;
						moraleSEActive++;
					} while (moraleSEActive < 0);
				}
			}
		}
		int moraleSEPending = PlayersData[factionID].socEffectPending.morale;
		if (moraleSEPending == 2 || moraleSEPending == 3) {
			VehMoraleModifierCount++;
		}
		if (!morale && !VehMoraleModifierCount) {
			VehMoraleModifierCount = 1;
		}
		if(VehMoraleModifierCount) {
			output += " (";
			for (int i = 0; i < VehMoraleModifierCount; i++) {
				output += "+";
			}
			output += ")";
		}
		VehMoraleModifierCount -= moralePenalty;
		if (Veh[vehID].state & VSTATE_DESIGNATE_DEFENDER) {
			output += "(d)";
		}
	}
	// TODO: assumes at least 1032 char nulled buffer (stringTemp), eventually remove
	strcat_s(moraleOutput, 1032, output.c_str());
}

/*
Purpose: Get the morale string for the specified Veh and store it into stringTemp buffer.
Original Offset: 004B43C0
Return Value: n/a
Status: Complete
*/
void __cdecl say_morale(uint32_t vehID, int factionIDvsNative) {
	say_morale(stringTemp->str, vehID, factionIDvsNative);
}

/*
Purpose: Calculate maximum range for Veh drops (air drops, Drop Pods).
Original Offset: 00500320
Return Value: Max range
Status: Complete
*/
uint32_t __cdecl drop_range(int factionID) {
	if (!has_tech(Rules->TechOrbInsertSansSpcElev, factionID) // default 'Graviton Theory'
		&& !has_project(SP_SPACE_ELEVATOR, factionID)) {
		return Rules->MaxAirdropSansOrbInsert;
	}
	return (*MapLongitudeBounds <= * MapLatitudeBounds) ? *MapLatitudeBounds : *MapLongitudeBounds;
}

/*
Purpose: Check whether protoID is a planet buster.
Original Offset: 005004F0
Return Value: reactorID if planet buster, otherwise 0
Status: Complete
*/
uint32_t __cdecl planet_buster2(int protoID) {
	return VehPrototype[protoID].plan == PLAN_PLANET_BUSTER ? VehPrototype[protoID].reactorID : 0;
}

/*
Purpose: Check whether vehID is a planet buster.
Original Offset: 00500520
Return Value: reactorID if planet buster, otherwise 0
Status: Complete
*/
uint32_t __cdecl planet_buster(int vehID) {
	return planet_buster2(Veh[vehID].protoID);
}

/*
Purpose: Calculate the specified tile's defensive value based on the terrain.
Original Offset: 005010C0
Return Value: Defense terrain value
Status: Complete
*/
uint32_t __cdecl defense_value(uint32_t factionID, uint32_t xCoord, uint32_t yCoord, 
	uint32_t vehIDDef, int vehIDAtk) {
	if (is_ocean(xCoord, yCoord) || base_who(xCoord, yCoord) >= 0) {
		return 2;
	}
	uint32_t bit = bit_at(xCoord, yCoord);
	if (bit & BIT_FUNGUS && altitude_at(xCoord, yCoord) >= ALT_BIT_OCEAN_SHELF && vehIDAtk >= 0
		&& (!Veh[vehIDAtk].factionID || has_project(SP_PHOLUS_MUTAGEN, Veh[vehIDAtk].factionID) 
			|| (Weapon[VehPrototype[Veh[vehIDAtk].protoID].weaponID].offenseRating < 0
				&& Veh[vehIDAtk].protoID < MaxVehProtoFactionNum))) {
				return 2;
	}
	uint32_t isRocky = rocky_at(xCoord, yCoord) > TERRAIN_BIT_ROLLING;
	VehBattleDisplayTerrain = label_get(91); // "Rocky"
	uint32_t defense = isRocky;
	if (bit & BIT_FUNGUS && altitude_at(xCoord, yCoord) >= ALT_BIT_OCEAN_SHELF && !defense
		&& get_triad(vehIDDef) != TRIAD_AIR) {
		if (vehIDAtk >= 0
			&& ((Weapon[VehPrototype[Veh[vehIDAtk].protoID].weaponID].offenseRating < 0
				&& Veh[vehIDAtk].protoID < MaxVehProtoFactionNum)
			|| has_project(SP_PHOLUS_MUTAGEN, Veh[vehIDAtk].factionID))) {
			defense = isRocky;
		}
		else {
			VehBattleDisplayTerrain = label_get(338); // "Fungus"
			defense = (has_project(SP_PHOLUS_MUTAGEN, factionID) 
				|| ((Veh[vehIDDef].protoID < MaxVehProtoFactionNum 
					&& Weapon[VehPrototype[Veh[vehIDDef].protoID].weaponID].offenseRating < 0)))
				? 2 : 1;
		}
	}
	if (bit & BIT_FOREST && !defense && (vehIDAtk < 0 || get_triad(vehIDAtk) == TRIAD_LAND)) {
		VehBattleDisplayTerrain = label_get(291); // "Forest"
		defense = 1;
	}
	return defense + 2;
}

/*
Purpose: Calculate the lifecycle (morale) of native life.
Original Offset: 00501350
Return Value: Lifecycle (morale) value
Status: Complete
*/
uint32_t __cdecl morale_alien(int vehID, int factionIDvsNative) {
	int morale;
	// Fungal Tower specific code, shifted to start and added vehID check to prevent crash
	if (vehID >= 0 && Veh[vehID].protoID == BSC_FUNGAL_TOWER) {
		morale = 0;
		int16_t xCoord = Veh[vehID].xCoord, yCoord = Veh[vehID].yCoord;
		// similar to is_coast() > except with fungus check + Ocean Shelf included
		for (uint32_t i = 1; i < 9; i++) {
			int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
			if (on_map(xRadius, yRadius) && bit_at(xRadius, yRadius) & BIT_FUNGUS
				&& altitude_at(xRadius, yRadius) >= ALT_BIT_OCEAN_SHELF) {
				morale++;
			}
		}
		morale -= 2;
	}
	else { // everything else
		if (*TurnCurrentNum < 45) {
			morale = 0;
		}
		else if (*TurnCurrentNum < 90) {
			morale = 1;
		}
		else if (*TurnCurrentNum < 170) {
			morale = 2;
		}
		else if (*TurnCurrentNum < 250) {
			morale = 3;
		}
		else if (*TurnCurrentNum < 330) {
			morale = 4;
		}
		else { // 330+
			morale = 6;
		}
		if (factionIDvsNative > 0) {
			morale += (PlayersData[factionIDvsNative].majorAtrocities != 0)
				+ (TectonicDetonationCount[factionIDvsNative] != 0);
		}
		if (vehID >= 0) {
			if (Veh[vehID].state & VSTATE_MONOLITH_UPGRADED) {
				morale++;
			}
			if (Veh[vehID].protoID == BSC_LOCUSTS_OF_CHIRON) {
				morale++;
			}
			morale += (Veh[vehID].flags >> 3) & 3; // 0x8|0x10 > +1, +2, or +3; TODO: unknown flags
		}
	}
	return range(morale, 0, 6);
}

/*
Purpose: Calculate psi combat factor for attacking or defending units.
Original Offset: 00501500
Return Value: Psi factor
Status: Complete
*/
int __cdecl psi_factor(int combatRatio, int factionID, BOOL isAttack, BOOL isFungalTower) {
	int rulePsi = Players[factionID].rulePsi;
	int factor = rulePsi ? ((rulePsi + 100) * combatRatio) / 100 : combatRatio;
	if (isAttack) {
		if (has_project(SP_DREAM_TWISTER, factionID)) {
			factor += factor / 2; // Psi Attack +50%
		}
	}
	else {
		if (has_project(SP_NEURAL_AMPLIFIER, factionID)) {
			factor += factor / 2; // Psi Defense +50%s
		}
		if (isFungalTower) {
			factor += factor / 2; // SMACX only: likely +50% Fungal Tower defense bonus
		}
	}
	return factor;
}

/*
Purpose: Get the basic offense value for an attacking Veh with an optional defender Veh parameter.
Original Offset: 005015B0
Return Value: Basic offense
Status: Complete
*/
int __cdecl get_basic_offense(uint32_t vehIDAtk, int vehIDDef, uint32_t psiCombatType, 
	BOOL isBombardment, BOOL isUnkTgl) { // artillery/duel related? Is offense of defender?
	uint32_t factionIDAtk = Veh[vehIDAtk].factionID, protoIDAtk = Veh[vehIDAtk].protoID;
	uint32_t morale = factionIDAtk ? morale_veh(vehIDAtk, true, 0) :
		morale_alien(vehIDAtk, vehIDDef >= 0 ? Veh[vehIDDef].factionID : -1);
	int baseIDAtk = base_at(Veh[vehIDAtk].xCoord, Veh[vehIDAtk].yCoord);
	if (baseIDAtk >= 0) {
		if (has_fac_built(FAC_CHILDREN_CRECHE, baseIDAtk)) {
			morale++;
			int moraleSEActive = range(PlayersData[factionIDAtk].socEffectActive.morale, -4, 4);
			if (moraleSEActive <= -2) {
				moraleSEActive++;
			}
			morale -= moraleSEActive;
		}
		else if (has_fac_built(FAC_BROOD_PIT, baseIDAtk) && protoIDAtk < MaxVehProtoFactionNum
			&& (Weapon[VehPrototype[protoIDAtk].weaponID].offenseRating < 0
				|| protoIDAtk == BSC_SPORE_LAUNCHER)) {
			morale++;
			int moraleSEActive = range(PlayersData[factionIDAtk].socEffectActive.morale, -4, 4);
			if (moraleSEActive <= -2) {
				moraleSEActive++;
			}
			morale -= moraleSEActive;
		}
		if (isUnkTgl) {
			int moraleSEPending = PlayersData[factionIDAtk].socEffectPending.morale;
			if (moraleSEPending >= 2 && moraleSEPending <= 3) {
				morale++;
			}
			if (vehIDDef >= 0) {
				if (Veh[vehIDDef].factionID) {
					if ((protoIDAtk >= MaxVehProtoFactionNum
						|| (Weapon[VehPrototype[protoIDAtk].weaponID].offenseRating >= 0
							&& protoIDAtk != BSC_SPORE_LAUNCHER))
						&& !has_abil(protoIDAtk, ABL_DISSOCIATIVE_WAVE)
						&& has_abil(Veh[vehIDDef].protoID, ABL_SOPORIFIC_GAS)) {
						morale -= 2;
					}
				}
				else {
					morale++;
				}
			}
		}
	}
	if (isUnkTgl) {
		morale = range(morale, 1, 6);
	}
	VehBasicBattleMorale[isUnkTgl != 0] = morale; // shifted up from original
	morale += 6;
	uint32_t offense = offense_proto(protoIDAtk, vehIDDef, isBombardment);
	if (psiCombatType) {
		offense = psi_factor(offense, factionIDAtk, true, false);
	}
	return offense * morale * 4;
}

/*
Purpose: Get the basic defense value for a defending Veh with an optional attacker Veh parameter.
Original Offset: 00501940
Return Value: Basic defense
Status: Complete
*/
int __cdecl get_basic_defense(uint32_t vehIDDef, int vehIDAtk, uint32_t psiCombatType,
	BOOL isBombardment) {
	uint32_t factionIDDef = Veh[vehIDDef].factionID, protoIDDef = Veh[vehIDDef].protoID;
	uint32_t morale = factionIDDef ? morale_veh(vehIDDef, true, 0)
		: morale_alien(vehIDDef, vehIDAtk >= 0 ? Veh[vehIDAtk].factionID : -1);
	int baseIDDef = base_at(Veh[vehIDDef].xCoord, Veh[vehIDDef].yCoord);
	if (baseIDDef >= 0) {
		if (has_fac_built(FAC_CHILDREN_CRECHE, baseIDDef)) {
			morale++;
			int moraleSEActive = range(PlayersData[factionIDDef].socEffectActive.morale, -4, 0);
			if (moraleSEActive <= -2) {
				moraleSEActive++;
			}
			morale -= moraleSEActive;
		}
		else if (has_fac_built(FAC_BROOD_PIT, baseIDDef) &&  protoIDDef < MaxVehProtoFactionNum
			&& (Weapon[VehPrototype[protoIDDef].weaponID].offenseRating < 0
				|| protoIDDef == BSC_SPORE_LAUNCHER)) {
			morale++;
			int moraleSEActive = range(PlayersData[factionIDDef].socEffectActive.morale, -4, 4);
			if (moraleSEActive <= -2) {
				moraleSEActive++;
			}
			morale -= moraleSEActive;
		}
		if (has_fac_built(FAC_HEADQUARTERS, baseIDDef)) { // bug fix: per manual
			morale++; // "Units in a headquarters base automatically gain +1 Morale when defending."
		}
		int moraleSEPending = PlayersData[factionIDDef].socEffectPending.morale;
		if (moraleSEPending >= 2 && moraleSEPending <= 3) {
			morale++;
		}
		if (vehIDAtk >= 0 && !Veh[vehIDAtk].factionID) {
			morale++;
		}
	}
	if (vehIDAtk >= 0 && Veh[vehIDAtk].factionID && (protoIDDef >= MaxVehProtoFactionNum 
		|| (Weapon[VehPrototype[protoIDDef].weaponID].offenseRating >= 0 
			&& protoIDDef != BSC_SPORE_LAUNCHER)) && !has_abil(protoIDDef, ABL_DISSOCIATIVE_WAVE)
		&& has_abil(Veh[vehIDAtk].protoID, ABL_SOPORIFIC_GAS)) {
		morale -= 2;
	}
	morale = range(morale, 1, 6);
	VehBasicBattleMorale[1] = morale;
	morale += 6;
	uint32_t planDef = VehPrototype[protoIDDef].plan;
	if (planDef == PLAN_ALIEN_ARTIFACT) {
		return 1;
	}
	if (planDef == PLAN_INFO_WARFARE
		&& Armor[VehPrototype[protoIDDef].armorID].defenseRating == 1
		// bug fix: added vehIDAtk bounds check to prevent potential arbitrary memory read
		&& (vehIDAtk < 0 || VehPrototype[Veh[vehIDAtk].protoID].plan != PLAN_INFO_WARFARE)) {
		return 1;
	}
	uint32_t defense = armor_proto(protoIDDef, vehIDAtk, isBombardment);
	if (psiCombatType) {
		defense = psi_factor(defense, factionIDDef, false, protoIDDef == BSC_FUNGAL_TOWER);
	}
	return defense * morale;
}

/*
Purpose: Initialize or reset battle related global variables.
Original Offset: 00501D30
Return Value: n/a
Status: Complete
*/
void __cdecl battle_init() {
	VehBattleModCount[0] = 0;
	VehBattleModCount[1] = 0;
	VehBattleTglUnk[0] = false;
	VehBattleTglUnk[1] = false;
}

/*
Purpose: Add combat battle modifier for type (offense, defense). TODO: Revise global offsets once 
         all references are decompiled.
Original Offset: 00501D50
Return Value: n/a
Status: Complete
*/
void __cdecl add_bat(uint32_t type, int modifier, LPCSTR displayStr) {
	uint32_t offset = VehBattleModCount[type];
	if (modifier && offset < 4) {
		VehBattleModifier[type * 4 + offset] = modifier;
		strcpy_s((LPSTR)&VehBattleDisplay[type * 80 + offset * 20], 80, displayStr);
		VehBattleModCount[type]++;
	}
}

/*
Purpose: Calculate battle outcome between two units.
Original Offset: 00501DA0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl battle_compute(int vehIDAtk, int vehIDDef, int *offenseOutput, int *defenseOutput,
	int combatType) {
	int offense = 8, defense = 8, protoIDAtk = -1, protoIDDef = -1;
	uint32_t factionIDAtk = 0, factionIDDef = 0;
	if (vehIDAtk >= 0) {
		protoIDAtk = Veh[vehIDAtk].protoID;
		factionIDAtk = Veh[vehIDAtk].factionID;
	}
	if (vehIDDef >= 0) {
		protoIDDef = Veh[vehIDDef].protoID;
		factionIDDef = Veh[vehIDDef].factionID;
	}
	battle_init();
	BOOL isArtillery = combatType & 1;
	BOOL isBombardment = (isArtillery // added vehIDAtk check
		|| (vehIDAtk >= 0 && Chassis[VehPrototype[protoIDAtk].chassisID].missile)) ? true : false;
	uint32_t psiCombatType = 0;
	if (!isBombardment || protoIDAtk == BSC_SPORE_LAUNCHER || protoIDDef == BSC_SPORE_LAUNCHER) {
		if (vehIDAtk >= 0
			&& Weapon[VehPrototype[protoIDAtk].weaponID].offenseRating < 0) {
			psiCombatType = 1; // PSI attacker
		}
		if (vehIDDef >= 0 && Armor[VehPrototype[protoIDDef].armorID].defenseRating < 0) {
			psiCombatType |= 2; // PSI defender
		}
		// NOTE: PSI bonuses below are only for display purposes, actual calculation is done in 
		//       combat morale functions.
		if (psiCombatType) {
			// Calculate PSI attack bonus
			int rulePSIAtk = Players[factionIDAtk].rulePsi;
			if (rulePSIAtk) {
				add_bat(0, rulePSIAtk, label_get(342)); // "Gaian Psi Bonus"
			}
			if (has_project(SP_DREAM_TWISTER, factionIDAtk)) {
				add_bat(0, 50, label_get(343)); // "Dream Twist"
			}
			// PSI Defense?
			if (psiCombatType) { // shouldn't this be & 2?
				int rulePSIDef = Players[factionIDDef].rulePsi;
				if (rulePSIDef) {
					add_bat(1, rulePSIDef, label_get(342)); // "Gaian Psi Bonus"
				}
				if (has_project(SP_NEURAL_AMPLIFIER, factionIDDef)) {
					add_bat(1, 50, label_get(344)); // "Neural Amp"
				}
				if (vehIDDef >= 0 && protoIDDef == BSC_FUNGAL_TOWER) {
					add_bat(1, 50, VehPrototype[BSC_FUNGAL_TOWER].vehName); // "Fungal Tower"
				}
			}
		}
		if (vehIDAtk >= 0) {
			offense = get_basic_offense(vehIDAtk, vehIDDef, psiCombatType, isBombardment, false);
			if (VehPrototype[protoIDAtk].plan != PLAN_INFO_WARFARE) {
				if (vehIDDef >= 0 && !combatType) { // checking if isn't wpn vs wpn ; air toggle
					int xCoordDef = Veh[vehIDDef].xCoord, yCoordDef = Veh[vehIDDef].yCoord;
					if (bit_at(xCoordDef, yCoordDef) & BIT_FUNGUS
						&& altitude_at(xCoordDef, yCoordDef) >= ALT_BIT_OCEAN_SHELF
						&& ((protoIDAtk < MaxVehProtoFactionNum
							&& Weapon[VehPrototype[protoIDAtk].weaponID].offenseRating < 0)
							|| has_project(SP_PHOLUS_MUTAGEN, factionIDAtk))
						&& Weapon[VehPrototype[protoIDDef].weaponID].offenseRating >= 0) {
						offense += offense / 2;
						add_bat(0, 50, label_get(338)); // "Fungus"
					}
					if (psiCombatType & 2
						&& (VehPrototype[protoIDAtk].weaponID == WPN_RESONANCE_LASER
							|| VehPrototype[protoIDAtk].weaponID == WPN_RESONANCE_BOLT)) {
						offense = (offense * 125) / 100;
						add_bat(0, 25, label_get(1110)); // "Resonance Attack"
					}
					if (!has_abil(protoIDDef, ABL_DISSOCIATIVE_WAVE) && psiCombatType &&
						has_abil(protoIDAtk, ABL_EMPATHIC) && Rules->CombatPctEmpSongAtkVsPsi) {
						offense = offense * (Rules->CombatPctEmpSongAtkVsPsi + 100) / 100;
						add_bat(0, Rules->CombatPctEmpSongAtkVsPsi,
							Ability[abil_index(ABL_EMPATHIC)].name);
					}
				}
				if (Veh[vehIDAtk].state & VSTATE_MADE_AIRDROP && has_abil(protoIDAtk, ABL_DROP_POD)
					&& Rules->CombatPenPctAtkAirdrop) {
					offense = (100 - Rules->CombatPctEmpSongAtkVsPsi) * offense / 100;
					/*
					uint32_t dropRange;
					if (has_tech(Rules->TechOrbInsertSansSpcElev, factionIDAtk)
						|| has_project(SP_SPACE_ELEVATOR, factionIDAtk)) {
						dropRange = (*MapHorizontalBounds <= *MapLatitudeBounds)
							? *MapLatitudeBounds : *MapHorizontalBounds;
					}
					else {
						dropRange = Rules->MaxAirdropSansOrbInsert;
					}
					*/
					add_bat(0, Rules->CombatPenPctAtkAirdrop,
						drop_range(factionIDAtk) <= Rules->MaxAirdropSansOrbInsert 
						? label_get(437) // "Air Drop"
						: label_get(438)); // "Orbital Insertion"
				}
				if (Players[factionIDAtk].ruleFlags & RFLAG_FANATIC
					&& Rules->CombatPctFanaticAtkBonus && !combatType && !psiCombatType) {
					offense = (Rules->CombatPctFanaticAtkBonus + 100) * offense / 100;
					add_bat(0, Rules->CombatPctFanaticAtkBonus, label_get(528));
				}
				int bonusCount = Players[factionIDAtk].factionBonusCount;
				for (int i = 0; i < bonusCount; i++) {
					if (Players[factionIDAtk].factionBonusID[i] == RULE_OFFENSE) {
						int ruleOffBonus = Players[factionIDAtk].factionBonusVal1[i];
						offense = offense * ruleOffBonus / 100;
						add_bat(0, ruleOffBonus, label_get(1108)); // "Alien Offense"
					}
				}
				if (psiCombatType && factionIDAtk) {
					int planetSEAtk = PlayersData[factionIDAtk].socEffectActive.planet;
					if (planetSEAtk && Rules->CombatPctPsiAtkBonusPLANET) {
						int planetModifier = planetSEAtk * Rules->CombatPctPsiAtkBonusPLANET;
						add_bat(0, planetModifier, label_get(625)); // "Planet"
						offense = (planetModifier + 100) * offense / 100;
					}
				}
			}
		}
		if (!(combatType & 2) && vehIDDef >= 0) {
			defense = get_basic_defense(vehIDDef, vehIDAtk, psiCombatType, isBombardment);
			int bonusCount = Players[factionIDDef].factionBonusCount;
			for (int i = 0; i < bonusCount; i++) {
				if (Players[factionIDDef].factionBonusID[i] == RULE_DEFENSE) {
					int ruleDefBonus = Players[factionIDDef].factionBonusVal1[i];
					defense = defense * ruleDefBonus / 100;
					add_bat(1, ruleDefBonus, label_get(1109)); // "Alien Defense"
				}
			}
			if (vehIDAtk >= 0 && VehPrototype[protoIDDef].plan == PLAN_INFO_WARFARE) {
				defense *= 4;
			}
			else {
				int xCoordDef = Veh[vehIDDef].xCoord, yCoordDef = Veh[vehIDDef].yCoord;
				int baseIDDef = base_at(xCoordDef, yCoordDef);
				if (psiCombatType) {
					if (baseIDDef >= 0 && Rules->CombatPctBaseDef) {
						add_bat(1, Rules->CombatPctBaseDef, label_get(332)); // "Base"
						defense = defense * (Rules->CombatPctBaseDef + 100) / 100;
					}
					defense *= 4;
				}
				else {
					uint32_t triadDef = get_proto_triad(protoIDDef);
					uint32_t terrainDef = (triadDef == TRIAD_AIR)
						? 2 : defense_value(factionIDDef, xCoordDef, yCoordDef, vehIDDef, vehIDAtk);
					uint32_t altAtk, altDef;
					if (vehIDAtk >= 0 && get_triad(vehIDAtk) == TRIAD_LAND) {
						if (combatType && Rules->CombatPctArtBonusLvlAlt && triadDef == TRIAD_LAND
							&& (altAtk = alt_at(Veh[vehIDAtk].xCoord, Veh[vehIDAtk].yCoord),
								altDef = alt_at(xCoordDef, yCoordDef), altAtk > altDef)) {
							offense = offense * (Rules->CombatPctArtBonusLvlAlt 
								* (altAtk - altDef) + 100) / 100;
							add_bat(0, Rules->CombatPctArtBonusLvlAlt, label_get(576)); //"Altitude"
						}
						if (Rules->CombatPctMobileOpenGround && !combatType && baseIDDef < 0
							&& terrainDef == 2 
							&& rocky_at(xCoordDef, yCoordDef) < TERRAIN_BIT_ROCKY) {
							uint32_t speedAtk = speed_proto(protoIDAtk);
							if (speedAtk > Rules->MoveRateRoads 
								&& speed_proto(protoIDDef) < speedAtk) {
								offense = offense * (Rules->CombatPctMobileOpenGround + 100) / 100;
								// "Mobile in open"
								add_bat(0, Rules->CombatPctMobileOpenGround, label_get(611));
							}
						}
						if (Rules->CombatPctDefVsMobileRough && !combatType && (terrainDef > 2
							|| baseIDDef >= 0) && rocky_at(xCoordDef, yCoordDef) < TERRAIN_BIT_ROCKY
							&& speed_proto(protoIDAtk) > Rules->MoveRateRoads) {
							defense = defense * (Rules->CombatPctDefVsMobileRough + 100) / 100;
							// "Rough vs. Mobile" : "Mobile vs. Base"
							add_bat(1, Rules->CombatPctDefVsMobileRough, baseIDDef < 0
								? label_get(548) : label_get(612));
						}
						if (Rules->CombatPctAtkRoad && !combatType) {
							// TODO: add check road/tube Combat % -> attacking along road
							offense = offense * (Rules->CombatPctAtkRoad + 100) / 100;
							add_bat(0, Rules->CombatPctAtkRoad, label_get(606)); // "Road Attack"
						}

						if (Rules->CombatPenPctAtkLwrElev && !combatType 
							&& altitude_at(xCoordDef, yCoordDef) 
							> altitude_at(Veh[vehIDAtk].xCoord, Veh[vehIDAtk].yCoord)
							&& !has_abil(protoIDAtk, ABL_ANTIGRAV_STRUTS)) {
							defense = defense * (Rules->CombatPenPctAtkLwrElev + 100) / 100;
							add_bat(1, Rules->CombatPenPctAtkLwrElev, label_get(441)); // "Uphill"
						}
						if (Rules->CombatPctAtkHigherElev && !combatType 
							&& altitude_at(Veh[vehIDAtk].xCoord, Veh[vehIDAtk].yCoord) 
							> altitude_at(xCoordDef, yCoordDef) 
							&& !has_abil(protoIDDef, ABL_ANTIGRAV_STRUTS)) {
							offense = offense * (Rules->CombatPctAtkHigherElev + 100) / 100;
							add_bat(0, Rules->CombatPctAtkRoad, label_get(330)); // "Downhill"
						}
					}
					defense *= terrainDef;
					if (terrainDef > 2) {
						std::string terrainModifier = label_get(331); // "Terrain"
						terrainModifier += " (";
						terrainModifier += StringTable->get(int(VehBattleDisplayTerrain));
						terrainModifier += ")";
						add_bat(1, 10 * (5 * terrainDef - 10), terrainModifier.c_str());
					}
					LPSTR displayDef; // only one is displayed
					uint32_t defMulti = 2;
					if (bit_at(xCoordDef, yCoordDef) & BIT_BUNKER && (vehIDAtk < 0
						|| get_triad(vehIDAtk) != TRIAD_AIR)) {
						defMulti = 3;
						displayDef = label_get(358); // "Bunker"
					}
					if (baseIDDef) {
						displayDef = label_get(332); // "Base"
						defMulti = 2;
						if (vehIDAtk >= 0) {
							uint32_t triadAtk = get_triad(vehIDAtk);
							uint32_t facModifier = 0;
							switch (triadAtk) {
							case TRIAD_LAND:
								if (has_fac_built(FAC_PERIMETER_DEFENSE, baseIDDef)
									|| has_project(SP_CITIZENS_DEFENSE_FORCE, factionIDDef)) {
									displayDef = label_get(354); // "Perimeter"
									facModifier = 4;
								}
								break;
							case TRIAD_SEA:
								if (has_fac_built(FAC_NAVAL_YARD, baseIDDef)
									|| has_project(SP_MARITIME_CONTROL_CENTER, factionIDDef)) {
									displayDef = Facility[FAC_NAVAL_YARD].name;
									facModifier = 4;
								}
								break;
							case TRIAD_AIR:
								if (has_fac_built(FAC_AEROSPACE_COMPLEX, baseIDDef)
									|| has_project(SP_CLOUDBASE_ACADEMY, factionIDDef)) {
									displayDef = Facility[FAC_AEROSPACE_COMPLEX].name;
									facModifier = 4;
								}
								break;
							default:
								facModifier = defMulti;
								break;
							}
							if (has_fac_built(FAC_TACHYON_FIELD, baseIDDef)) {
								facModifier += 2;
								displayDef = label_get(357); // "Tachyon"
							}
							if (facModifier < 2 || defMulti == 2
								&& (vehIDAtk < 0 || has_abil(protoIDAtk, ABL_BLINK_DISPLACER)
									&& Rules->CombatPctBaseDef)) {
								add_bat(1, Rules->CombatPctBaseDef, label_get(332)); // "Base"
								defense = defense * (Rules->CombatPctBaseDef + 100) / 100;
							}
							if (vehIDAtk >= 0 && has_abil(protoIDAtk, ABL_BLINK_DISPLACER)) {
								if (defMulti > 2) {
									defMulti = 2;
								}
								displayDef = label_get(428); // "Base vs. Blink"
							}
							if (Rules->CombatPctInfantryVsBase && !combatType && baseIDDef >= 0
								&& factionIDAtk && protoIDAtk > MaxVehProtoFactionNum
								&& Weapon[VehPrototype[protoIDAtk].weaponID].offenseRating >= 0 &&
								protoIDAtk != BSC_SPORE_LAUNCHER && !get_proto_triad(protoIDAtk)
								&& Chassis[VehPrototype[protoIDAtk].chassisID].speed == 1) {
								offense = offense * (Rules->CombatPctInfantryVsBase + 100) / 100;
								// "Infantry vs. Base"
								add_bat(0, Rules->CombatPctInfantryVsBase, label_get(547));
							}
							uint32_t bitDef;
							if (isArtillery && defMulti <= 2 && baseIDDef < 0
								&& rocky_at(xCoordDef, yCoordDef) < TERRAIN_BIT_ROCKY
								&& (bitDef = bit_at(xCoordDef, yCoordDef),
									!(bitDef & BIT_FOREST)) && (bitDef & BIT_FUNGUS
										|| altitude_at(xCoordDef, yCoordDef) 
										< ALT_BIT_OCEAN_SHELF)) {
								defMulti = 3;
								displayDef = label_get(525); // "Open Ground"
							}
							defense *= defMulti;
							if (defMulti > 2) {
								add_bat(1, 10 * (5 * defMulti - 10), 
									StringTable->get(int(displayDef)));
							}
						}
					}
				}
				if (factionIDDef && vehIDAtk >= 0 
					&& Chassis[VehPrototype[protoIDAtk].chassisID].missile) {
					for (int i = 0; i < 25; i++) {
						int xRadius = xrange(xCoordDef + xRadiusOffset[i]), 
							yRadius = yCoordDef + yRadiusOffset[i];
						if (on_map(xRadius, yRadius)) {
							int baseIDRadius = base_at(xRadius, yRadius);
							if (baseIDRadius >= 0
								&& Base[baseIDRadius].factionIDCurrent == factionIDDef
								&& has_fac_built(FAC_FLECHETTE_DEFENSE_SYS, baseIDRadius)) {
								defense = 3 * defense / 2;
								add_bat(1, 50, label_get(1113)); // "Flechette"
							}
						}
					}
				}
				if (!is_ocean(xCoordDef, yCoordDef)) {
					uint32_t sensorDef = 0;
					if (factionIDDef) {
						for (int i = 0; i < 25; i++) {
							int xRadius = xrange(xCoordDef + xRadiusOffset[i]),
								yRadius = yCoordDef + yRadiusOffset[i];
							uint32_t sensorStatus;
							if (on_map(xRadius, yRadius)
								&& (sensorStatus = is_sensor(xRadius, yRadius), sensorStatus)) {
								BOOL hasSensor = false;
								if (!is_ocean(xRadius, yRadius)) {
									int factionIDTerr = whose_territory(factionIDDef, xRadius, 
										yRadius, NULL, false);
									if (factionIDTerr < 0 
										|| (uint32_t)factionIDTerr == factionIDDef) {
										hasSensor = true;
									}
								}
								else {
									int baseIDFind = base_find(xRadius, yRadius);
									// assumes will find a base?
									if (Base[baseIDFind].factionIDCurrent == factionIDDef) {
										hasSensor = true;
									}
								}
								if (hasSensor) {
									sensorDef |= 1;
									if (sensorStatus > 1) {
										sensorDef |= 2;
									}
								}
							}
						}
					}
					if (sensorDef & 1) {
						defense = defense * (Rules->CombatPctDefRangeSensor + 100) / 100;
						add_bat(1, Rules->CombatPctDefRangeSensor, label_get(613)); // "Sensor"
					}
					if (sensorDef & 2) {
						defense = defense * (Rules->CombatPctDefRangeSensor + 100) / 100;
						add_bat(1, Rules->CombatPctDefRangeSensor, label_get(1123)); // "GSP"
					}
				}
				if (!has_abil(protoIDAtk, ABL_DISSOCIATIVE_WAVE) && Rules->CombatPctTranceDefVsPsi
					&& vehIDAtk >= 0 && psiCombatType & 1 && has_abil(protoIDDef, ABL_TRANCE)) {
					defense = defense * (Rules->CombatPctTranceDefVsPsi + 100) / 100;
					add_bat(1, Rules->CombatPctTranceDefVsPsi, label_get(329)); // "Trance"
				}
				uint32_t armorIDDef = VehPrototype[protoIDDef].armorID;
				if (psiCombatType & 1 && vehIDAtk >= 0 && (armorIDDef == ARM_RESONANCE_3_ARMOR 
					|| armorIDDef == ARM_RESONANCE_8_ARMOR)) {
					defense = 125 * defense / 100;
					add_bat(1, 25, label_get(1111)); // "Resonance Def."
				}
				// add check vehIDAtk >= 0 to skip this entire code section? original jumps to end
				if (vehIDAtk >= 0 && get_proto_triad(protoIDAtk) == TRIAD_AIR
					&& has_abil(protoIDAtk, ABL_AIR_SUPERIORITY) && !psiCombatType) {
					if (get_proto_triad(protoIDDef) == TRIAD_AIR) {
						int groundStrikePen = Rules->CombatPenPctAirSuprVsGrnd;
						if (groundStrikePen) {
							offense = offense * (100 - groundStrikePen) / 100;
							add_bat(0, -groundStrikePen, label_get(448)); // "Ground Strike"
						}
					}
					else {
						int airToAir = Rules->CombatPctAirSuprVsAir;
						if (airToAir && !has_abil(protoIDDef, ABL_AIR_SUPERIORITY)) {
							offense = offense * (airToAir + 100) / 100;
							add_bat(0, airToAir, label_get(449)); // "Air-to-Air"
						}
					}
				}
				if (vehIDAtk >= 0 && get_proto_triad(protoIDAtk) == TRIAD_AIR // added vehIDAtk chk
					&& get_proto_triad(protoIDDef) == TRIAD_AIR
					&& has_abil(protoIDDef, ABL_AIR_SUPERIORITY)
					&& has_abil(protoIDAtk, ABL_AIR_SUPERIORITY) && !psiCombatType
					&& !Chassis[VehPrototype[protoIDAtk].chassisID].missile
					&& !Chassis[VehPrototype[protoIDDef].chassisID].missile
					&& Rules->CombatPctAirSuprVsAir) {
					defense = defense * (Rules->CombatPctAirSuprVsAir + 100) / 100;
					add_bat(1, Rules->CombatPctAirSuprVsAir, label_get(449)); // "Air-to-Air"
				}
				if (!Weapon[VehPrototype[protoIDDef].weaponID].offenseRating
					&& Armor[VehPrototype[protoIDDef].armorID].defenseRating == 1 &&
					(factionIDAtk || baseIDDef < 0) && Rules->CombatPenPctNonCbtDefVsCbt) {
					defense = defense * (100 - Rules->CombatPenPctNonCbtDefVsCbt) / 100;
					add_bat(1, -Rules->CombatPenPctNonCbtDefVsCbt, label_get(439)); // "Non Combat"
				}
				if (vehIDAtk >= 0 && get_proto_triad(protoIDAtk) == TRIAD_SEA // added vehIDAtk chk
					&& get_proto_triad(protoIDDef) == TRIAD_SEA && baseIDDef >= 0 
					&& Rules->CombatPctBonusVsShipPort) {
					offense = offense * (Rules->CombatPctBonusVsShipPort + 100) / 100;
					add_bat(0, Rules->CombatPctBonusVsShipPort, label_get(335)); // "In Port"
				}
				if (armorIDDef == ARM_PULSE_3_ARMOR || armorIDDef == ARM_PULSE_8_ARMOR
					&& get_proto_triad(protoIDAtk) == TRIAD_LAND
					&& Chassis[VehPrototype[protoIDAtk].chassisID].speed > 1) {
					defense = 125 * defense / 100;
					add_bat(1, 25, label_get(1112)); // "Pulse Defense"
				}
				if (!has_abil(protoIDAtk, ABL_DISSOCIATIVE_WAVE)
					&& has_abil(protoIDDef, ABL_COMM_JAMMER)
					&& get_proto_triad(protoIDAtk) == TRIAD_LAND
					&& Chassis[VehPrototype[protoIDAtk].chassisID].speed > 1 
					&& Rules->CombatPctComJamDefVsMobl) {
					defense = defense * (Rules->CombatPctComJamDefVsMobl + 100) / 100;
					add_bat(1, Rules->CombatPctComJamDefVsMobl, label_get(336)); // "Jammer"
				}
				if (!has_abil(protoIDAtk, ABL_DISSOCIATIVE_WAVE) && has_abil(protoIDDef, ABL_AAA)
					&& get_proto_triad(protoIDAtk) == TRIAD_AIR && Rules->CombatPctAAABonusVsAir) {
					defense = defense * (Rules->CombatPctAAABonusVsAir + 100) / 100;
					add_bat(1, Rules->CombatPctAAABonusVsAir, label_get(337)); // "Tracking"
				}
			}
		}
		else if (vehIDDef >= 0) {
			defense = get_basic_offense(vehIDDef, vehIDAtk, psiCombatType, isBombardment, true);
			if (!(combatType & 0x18)) {
				uint32_t triadAtk = get_proto_triad(protoIDAtk),
					triadDef = get_proto_triad(protoIDDef);
				if (triadDef == TRIAD_SEA && triadAtk == TRIAD_LAND) {
					int gunVsShipArt = Rules->CombatPctLandGunVsShipArt;
					if (gunVsShipArt) {
						offense = (gunVsShipArt + 100) * offense / 100;
						add_bat(0, gunVsShipArt, label_get(435)); // "Land Based Guns"
					}
				}
				else if (triadAtk == TRIAD_SEA && triadDef == TRIAD_LAND) {
					int gunVsShipArt = Rules->CombatPctLandGunVsShipArt;
					if (gunVsShipArt) {
						defense = (gunVsShipArt + 100) * defense / 100;
						add_bat(1, gunVsShipArt, label_get(435)); // "Land Based Guns"
					}
				}
				else if (Rules->CombatPctArtBonusLvlAlt) {
					uint32_t altAtk = alt_at(Veh[vehIDAtk].xCoord, Veh[vehIDAtk].yCoord);
					uint32_t altDef = alt_at(Veh[vehIDDef].xCoord, Veh[vehIDDef].yCoord);
					if (altDef >= altAtk) {
						if (altDef > altAtk) { // ???
							int altModifierDef = (altDef - altAtk) * Rules->CombatPctArtBonusLvlAlt;
							defense = defense * (altModifierDef + 100) / 100;
							add_bat(1, altModifierDef, label_get(576)); // "Altitude"
						}
					}
					else {
						int altModifierAtk = (altAtk - altDef) * Rules->CombatPctArtBonusLvlAlt;
						offense = offense * (altModifierAtk + 100) / 100;
						add_bat(0, altModifierAtk, label_get(576)); // "Altitude"
					}
				}
			}
		}
	}
	if (offenseOutput) {
		*offenseOutput = offense;
	}
	if (defenseOutput) {
		*defenseOutput = defense;
	}
}

/*
Purpose: Determine the best defender.
Original Offset: 005044D0
Return Value: vehID of best defender
Status: Complete - testing
*/
uint32_t __cdecl best_defender(uint32_t vehIDDef, int vehIDAtk, BOOL checkArtillery) {
	int offense = vehIDAtk >= 0 ? get_basic_offense(vehIDAtk, vehIDDef, false, false, false) : 8;
	int xCoordDef = Veh[vehIDDef].xCoord, yCoordDef = Veh[vehIDDef].yCoord;
	BOOL isOceanDef = is_ocean(xCoordDef, yCoordDef);
	int baseIDDef = base_at(xCoordDef, yCoordDef);
	//
	int defenderSearch = -999;
	uint32_t bestDefVehID = vehIDDef;
	for (int i = veh_top(vehIDDef); i >= 0; i = Veh[i].nextVehIDStack) {
		uint32_t protoIDDef = Veh[i].protoID;
		if ((get_proto_triad(protoIDDef) != TRIAD_LAND || !isOceanDef && baseIDDef >= 0)
			// added vehIDAtk bounds check
			&& (vehIDAtk < 0 || VehPrototype[Veh[vehIDAtk].protoID].plan != PLAN_INFO_WARFARE
				|| VehPrototype[protoIDDef].plan == PLAN_INFO_WARFARE)) {
			uint32_t combatType = (can_arty(protoIDDef, true)
				&& (vehIDAtk || can_arty(Veh[vehIDAtk].protoID, true))) ? 2 : 0; // added bounds
			if (vehIDAtk >= 0 && get_triad(vehIDAtk) != TRIAD_AIR
				&& has_abil(protoIDDef, ABL_AIR_SUPERIORITY)
				&& !Chassis[VehPrototype[protoIDDef].chassisID].missile
				&& !Chassis[VehPrototype[Veh[vehIDAtk].protoID].chassisID].missile
				&& Weapon[VehPrototype[Veh[vehIDAtk].protoID].weaponID].offenseRating > 0
				&& Armor[VehPrototype[Veh[vehIDAtk].protoID].armorID].defenseRating > 0
				&& Weapon[VehPrototype[protoIDDef].weaponID].offenseRating > 0) {
				combatType |= 0xA; // air combat
				if (get_proto_triad(protoIDDef) == TRIAD_AIR) {
					combatType |= 0x10;
				}
			}
			int offenseOutput, defenseOutput;
			battle_compute(vehIDAtk, i, &offenseOutput, &defenseOutput, combatType);
			if (!offenseOutput) {
				break;
			}
			uint32_t protoPwrDef = proto_power(i);
			int defModifier = (((((range(protoPwrDef - Veh[i].dmgIncurred, 0, 9999) * defenseOutput)
				/ protoPwrDef) * offense) / offenseOutput) / 8)
				- Weapon[VehPrototype[protoIDDef].weaponID].offenseRating;
			uint32_t planDef = VehPrototype[protoIDDef].plan;
			if (planDef < PLAN_COLONIZATION || planDef == PLAN_TERRAFORMING) {
				defModifier *= 16;
			}
			if (vehIDAtk >= 0 && get_triad(vehIDAtk) == TRIAD_AIR 
				&& has_abil(protoIDDef, ABL_AIR_SUPERIORITY) 
				&& get_proto_triad(protoIDDef) == TRIAD_AIR 
				&& !Chassis[VehPrototype[protoIDDef].chassisID].missile
				&& !Chassis[VehPrototype[Veh[vehIDAtk].protoID].chassisID].missile) {
				defModifier += 0x80000;
			}
			else if (vehIDAtk >= 0 && get_triad(vehIDAtk) == TRIAD_AIR
				&& has_abil(Veh[vehIDAtk].protoID, ABL_AIR_SUPERIORITY)
				&& get_proto_triad(protoIDDef) == TRIAD_AIR && baseIDDef < 0
				&& !(bit_at(xCoordDef, yCoordDef) & BIT_AIRBASE)
				&& !stack_check(vehIDDef, 6, ABL_CARRIER, -1, -1)
				&& !Chassis[VehPrototype[protoIDDef].chassisID].missile
				&& !Chassis[VehPrototype[Veh[vehIDAtk].protoID].chassisID].missile) {
				defModifier += 0x80000;
			}
			else if (checkArtillery) {
				if (can_arty(protoIDDef, true)) {
					defModifier += 0x80000;
				}
			}
			else if ((!stack_check(vehIDDef, 3, TRIAD_AIR, -1, -1) || baseIDDef >= 0 
				|| bit_at(xCoordDef, yCoordDef) & BIT_AIRBASE 
				|| stack_check(vehIDDef, 6, ABL_CARRIER, -1, -1)) 
				&& Veh[i].state & VSTATE_DESIGNATE_DEFENDER) {
				defModifier += 0x80000;
			}
			int defense = i + (defModifier << 11);
			if (defense > defenderSearch) {
				defenderSearch = defense;
				bestDefVehID = i;
			}
		}
	}
	return bestDefVehID;
}

/*
Purpose: Determine whether any enemy naval transports are carrying land units within range to 
		 attack the specified base. If so, set the units to move towards the base.
Original Offset: 00506490
Return Value: n/a
Status: Complete
*/
void __cdecl invasions(uint32_t baseID) {
	uint32_t factionIDBase = Base[baseID].factionIDCurrent;
	int16_t xCoordBase = Base[baseID].xCoord, yCoordBase = Base[baseID].yCoord;
	for (int i = 0; i < *VehCurrentCount; i++) {
		uint32_t factionIDVeh = Veh[i].factionID;
		if (factionIDVeh && !is_human(factionIDVeh) && factionIDVeh != factionIDBase
			&& !has_treaty(factionIDVeh, factionIDBase, DTREATY_TREATY)) {
			int xCoordVeh = Veh[i].xCoord, yCoordVeh = Veh[i].yCoord;
			if (is_ocean(xCoordVeh, yCoordVeh) && veh_cargo(i)
				&& stack_check(i, 3, TRIAD_LAND, -1, -1)) {
				int proximity = vector_dist(xCoordBase, yCoordBase, xCoordVeh, yCoordVeh);
				if (proximity <= (int)speed(i, false)) {
					Veh[i].orders = ORDER_MOVE_TO;
					Veh[i].waypoint_xCoord[0] = xCoordBase;
					Veh[i].waypoint_yCoord[0] = yCoordBase;
				}
			}
		}
	}
}

/*
Purpose: Direct Veh to start moving automatically towards specified coordinates.
Original Offset: 00560AD0
Return Value: n/a
Status: Complete
*/
void __cdecl go_to(int vehID, char type, int xCoord, int yCoord) {
	Veh[vehID].orders = ORDER_MOVE_TO;
	Veh[vehID].moveToAIType = type;
	Veh[vehID].waypoint_xCoord[0] = (int16_t)xCoord;
	Veh[vehID].waypoint_yCoord[0] = (int16_t)yCoord;
	if (VehPrototype[Veh[vehID].protoID].plan == PLAN_COLONIZATION) {
		Veh[vehID].terraformingTurns = 0;
	}
}

/*
Purpose: Get Veh on top of stack.
Original Offset: 00579920
Return Value: vehID if found, otherwise -1
Status: Complete
*/
int __cdecl veh_top(int vehID) {
	if (vehID < 0) {
		return -1;
	}
	int topVehID = vehID;
	for (int i = Veh[topVehID].prevVehIDStack; i >= 0; i = Veh[i].prevVehIDStack) {
		topVehID = i;
	}
	return topVehID;
}

/*
Purpose: Get Veh current moves left.
Original Offset: 00579960
Return Value: Moves left
Status: Complete
*/
uint32_t __cdecl veh_moves(uint32_t vehID) {
	return range(speed(vehID, false) - Veh[vehID].movesExpended, 0, 999);
}

/*
Purpose: Get Veh power value from prototype.
Original Offset: 005799A0
Return Value: power
Status: Complete
*/
uint32_t __cdecl proto_power(uint32_t vehID) {
	int protoID = Veh[vehID].protoID;
	if (VehPrototype[protoID].plan == PLAN_ALIEN_ARTIFACT) {
		return 1;
	}
	return range(VehPrototype[protoID].reactorID, 1, 100) * 10;
}

/*
Purpose: Determine whether the specified Veh is eligible for a monolith morale upgrade.
Original Offset: 00579F80
Return Value: Is Veh eligible for monolith morale upgrade? true/false
Status: Complete
*/
BOOL __cdecl want_monolith(uint32_t vehID) {
	if (!(Veh[vehID].state & VSTATE_MONOLITH_UPGRADED)
		&& morale_veh(vehID, true, 0) < MORALE_ELITE && Veh[vehID].morale < MORALE_ELITE
		&& Weapon[VehPrototype[Veh[vehID].protoID].weaponID].offenseRating) {
		return true;
	}
	return false;
}

/*
Purpose: Calculate armor value from armorID
Original Offset: 0057D270
Return Value: Armor value / factor
Status: Complete
*/
int __cdecl arm_strat(int armorID, int factionID) {
	if (!ExpansionEnabled && armorID > ARM_PSI_DEFENSE) {
		return 1;
	}
	int defenseRating = Armor[armorID].defenseRating;
	if (defenseRating < 0) {
		return psi_factor((Rules->PsiCombatRatioDef[TRIAD_LAND]
			* PlayersData[factionID].enemyBestWeaponValue) / Rules->PsiCombatRatioAtk[TRIAD_LAND],
			factionID, false, false);
	}
	return defenseRating;
}

/*
Purpose: Calculate weapon value from weaponID
Original Offset: 0057D2E0
Return Value: Weapon value / factor
Status: Complete
*/
int __cdecl weap_strat(int weaponID, int factionID) {
	if (!ExpansionEnabled && (weaponID == WPN_RESONANCE_LASER || weaponID == WPN_RESONANCE_BOLT
		|| weaponID == WPN_STRING_DISRUPTOR))
		return 1;
	int offenseRating = Weapon[weaponID].offenseRating;
	if (offenseRating < 0) {
		return psi_factor((Rules->PsiCombatRatioAtk[TRIAD_LAND]
			* PlayersData[factionID].enemyBestArmorValue) / Rules->PsiCombatRatioDef[TRIAD_LAND],
			factionID, true, false);
	}
	return offenseRating;
}

/*
Purpose: Calculate weapon value from protoID
Original Offset: 0057D360
Return Value: Weapon value
Status: Complete
*/
int __cdecl weap_val(int protoID, int factionID) {
	return weap_strat(VehPrototype[protoID].weaponID, factionID);
}

/*
Purpose: Calculate armor value from armorID
Original Offset: 0057D3F0
Return Value: Armor value
Status: Complete
*/
int __cdecl arm_val(int armorID, int factionID) {
	if (factionID < 0) {
		return Armor[armorID].defenseRating * 2;
	}
	return arm_strat(armorID, factionID) * 2;
}

/*
Purpose: Calculate armor value from protoID
Original Offset: 0057D480
Return Value: Armor value
Status: Complete
*/
int __cdecl armor_val(int protoID, int factionID) {
	return arm_val(VehPrototype[protoID].armorID, factionID);
}

/*
Purpose: Calculate carrying/transport capacity for prototype
Original Offset: 0057D510
Return Value: Transport value
Status: Complete
*/
uint32_t __cdecl transport_val(uint32_t chassisID, int ability, uint32_t reactorID) {
	uint32_t transport = Chassis[chassisID].cargo;
	if (Chassis[chassisID].triad == TRIAD_SEA) {
		transport *= reactorID;
	}
	if (ability & ABL_SLOW) {
		transport /= 2; // -50%, rounded down
	}
	if (ability & ABL_HEAVY_TRANSPORT) {
		transport = (3 * transport + 1) / 2; // +50%, rounded up
	}
	return transport;
}

/*
Purpose: Craft Veh offensive related display string. Replaced existing non-safe strcat with String.
		 Original function took a 2nd parameter with char buffer and didn't return anything.
Original Offset: 0057D560
Return Value: string
Status: Complete
*/
std::string __cdecl say_offense(int protoID) {
	std::string output = std::to_string(offense_proto(protoID, -1, 0));
	if (has_abil(protoID, ABL_DROP_POD)) {
		output += "^";
	}
	if (has_abil(protoID, ABL_AMPHIBIOUS)) {
		output += "~";
	}
	if (has_abil(protoID, ABL_NERVE_GAS)) {
		output += "x";
	}
	if (has_abil(protoID, ABL_EMPATHIC)) {
		output += "e";
	}
	if (has_abil(protoID, ABL_BLINK_DISPLACER)) {
		output += "!";
	}
	if (can_arty(protoID, true)) {
		output.insert(output.begin(), '(');
		output += ")";
	}
	if (has_abil(protoID, ABL_AIR_SUPERIORITY)) {
		output.insert(output.begin(), '<');
		output += ">";
	}
	uint32_t weaponID = VehPrototype[protoID].weaponID;
	if (weaponID == WPN_RESONANCE_LASER || weaponID == WPN_RESONANCE_BOLT) {
		output += "r";
	}
	return output;
}

/*
Purpose: Craft Veh defensive related display string. Replaced existing non-safe strcat with String.
		 Original function took a 2nd parameter with char buffer and didn't return anything.
Original Offset: 0057D6D0
Return Value: string
Status: Complete
*/
std::string __cdecl say_defense(int protoID) {
	std::string output = std::to_string(armor_proto(protoID, -1, 0));
	if (has_abil(protoID, ABL_COMM_JAMMER)) {
		output += "+";
	}
	if (has_abil(protoID, ABL_TRANCE)) {
		output += "t";
	}
	if (has_abil(protoID, ABL_AAA)) {
		output.insert(output.begin(), '<');
		output += ">";
	}
	uint32_t armorID = VehPrototype[protoID].armorID;
	if (armorID == ARM_PULSE_3_ARMOR || armorID == ARM_PULSE_8_ARMOR) {
		output += "p";
	}
	else if (armorID == ARM_RESONANCE_3_ARMOR || armorID == ARM_RESONANCE_8_ARMOR) {
		output += "r";
	}
	return output;
}

/*
Purpose: Generate Veh stats from prototype id. Replaced existing non-safe strcat with String.
		 Reworked to integrate with existing C code.
Original Offset: 0057D7D0
Return Value: n/a
Status: Complete (TODO: Eventually remove LPSTR stat param and return std::string instead)
*/
void __cdecl say_stats_3(LPSTR stat, int protoID) {
	std::string output;
	int offRating = Weapon[VehPrototype[protoID].weaponID].offenseRating;
	if (offRating >= 0) {
		output = (offRating < 99) ? say_offense(protoID) : "*";
	}
	else {
		output = "?"; // PSI
	}
	output += "-";
	output += (Armor[VehPrototype[protoID].armorID].defenseRating) >= 0 ?
		say_defense(protoID) : "?";
	output += "-";
	output += std::to_string(speed_proto(protoID) / Rules->MoveRateRoads);
	uint8_t reactor = VehPrototype[protoID].reactorID;
	if (reactor > RECT_FISSION) {
		output += "*";
		output += std::to_string(reactor);
	}
	// assumes at least 256 char buffer, eventually remove
	// all calls but one use stringTemp (1032 buffer) > ProdPicker::calculate uses local 256 buffer
	strcat_s(stat, 256, output.c_str());
}

/*
Purpose: Generate Veh stats from prototype id using stringTemp buffer.
Original Offset: 0050B9A0
Return Value: n/a
Status: Complete
*/
void __cdecl say_stats_3(int protoID) {
	say_stats_3(stringTemp->str, protoID);
}

/*
Purpose: Generate Veh stats from prototype id. List whether unit is Psi, Sea or Air.
		 Replaced existing non-safe strcat with String. Reworked to integrate with existing C code.
Original Offset: 0057D8E0
Return Value: n/a
Status: Complete (TODO: Eventually remove LPSTR stat param and return std::string instead)
*/
void __cdecl say_stats_2(LPSTR stat, int protoID) {
	std::string output;
	int offRating = Weapon[VehPrototype[protoID].weaponID].offenseRating;
	if (offRating >= 0) {
		output = (offRating < 99) ? say_offense(protoID) : "*";
	}
	else {
		output = label_get(196); // 'Psi'
	}
	output += "-";
	output += (Armor[VehPrototype[protoID].armorID].defenseRating) >= 0 ?
		say_defense(protoID) : label_get(196); // 'Psi'
	output += "-";
	output += std::to_string(speed_proto(protoID) / Rules->MoveRateRoads);
	uint32_t triad = get_proto_triad(protoID);
	if (triad == TRIAD_SEA) {
		output += " ";
		output += label_get(163); // 'Sea'
	}
	else if (triad == TRIAD_AIR) {
		output += " ";
		output += label_get(162); // 'Air'
	}
	uint32_t reactor = VehPrototype[protoID].reactorID;
	if (reactor > 1) {
		output += "*";
		output += std::to_string(reactor);
	}
	// assumes at least 1032 char buffer (stringTemp), eventually remove
	strcat_s(stat, 1032, output.c_str());
}

/*
Generate Verbose Veh stats from prototype id. Used by Design Workshop and Military Command Nexus.
		 Replaced existing non-safe strcat with String. Reworked to integrate with existing C code.
Original Offset: 0057DAA0
Return Value: n/a
Status: Complete (TODO: Eventually remove LPSTR stat param and return std::string instead)
*/
void __cdecl say_stats(LPSTR stat, int protoID, LPSTR customSpacer) {
	std::string output;
	uint8_t plan = VehPrototype[protoID].plan;
	uint8_t chas = VehPrototype[protoID].chassisID;
	uint8_t triad = Chassis[chas].triad;
	uint8_t mode = Weapon[VehPrototype[protoID].weaponID].mode;
	int offRating = Weapon[VehPrototype[protoID].weaponID].offenseRating;
	int defRating = Armor[VehPrototype[protoID].armorID].defenseRating;
	if (plan == PLAN_RECONNAISANCE && triad == TRIAD_LAND && offRating == 1 && defRating == 1
		&& !VehPrototype[protoID].abilityFlags) {
		output = StringTable->get((int)PlansFullName[3]); // 'Explore/Defense'
		output += ", ";
	}
	else if (mode < 3) { // Projectile, energy, missile
		output = (plan != PLAN_DEFENSIVE || (offRating >= 0 && offRating <= defRating))
			? StringTable->get((int)PlansShortName[plan]) : label_get(312); // 'Combat'
		output += ", ";
	}
	if (offRating < 0 || mode < 3) {
		output += (offRating < 0) ? label_get(196) : say_offense(protoID); // 'Psi'
		output += customSpacer ? customSpacer : "/";
		output += (defRating < 0) ? label_get(196) : say_defense(protoID); // 'Psi'
		output += customSpacer ? customSpacer : "/";
		output += std::to_string(speed_proto(protoID) / Rules->MoveRateRoads);
	}
	else if (defRating != 1 || VehPrototype[protoID].abilityFlags || (Chassis[chas].speed != 1
		&& (mode != WPN_MODE_TRANSPORT || chas != CHSI_FOIL))) {
		output += StringTable->get(int(PlansShortName[mode]));
		if (plan == PLAN_NAVAL_TRANSPORT) {
			output += "(";
			output += std::to_string(VehPrototype[protoID].carryCapacity);
			output += ")";
		}
		output += ", ";
		output += (defRating < 0) ? label_get(196) : say_defense(protoID); // 'Psi'
		output += customSpacer ? customSpacer : "/";
		output += std::to_string(speed_proto(protoID) / Rules->MoveRateRoads);
	}
	else {
		output += StringTable->get((int)PlansFullName[mode]);
		if (plan == PLAN_NAVAL_TRANSPORT) {
			output += "(";
			output += std::to_string(VehPrototype[protoID].carryCapacity);
			output += ")";
		}
		if (triad) { // sea, air
			output += ","; // Bug fix: removed extra space
		}
	}
	if (triad == TRIAD_SEA) {
		output += " ";
		output += label_get(163); // 'Sea'
	}
	else if (triad == TRIAD_AIR) {
		output += " ";
		output += label_get(162); // 'Air'
	}
	uint32_t reactor = VehPrototype[protoID].reactorID;
	if (reactor > 1) {
		output += " (*";
		output += std::to_string(reactor);
		output += ")";
	}
	// assumes at least 1032 char buffer (stringTemp), eventually remove
	strcat_s(stat, 1032, output.c_str());
}

/*
Purpose: Check against faction's available tech for best available reactor.
Original Offset: 0057EFA0
Return Value: Best reactor available (1-4)
Status: Complete
*/
uint32_t __cdecl best_reactor(int factionID) {
	for (int i = MaxReactorNum - 1; i >= 0; i--) {
		if (has_tech(Reactor[i].preqTech, factionID)) {
			return i + 1;
		}
	}
	return 1; // default to Fission if there is an issue
}

/*
Purpose: Check against faction's available tech for best available chassis meeting requirements.
Original Offset: 0057EFF0
Return Value: Best chassis ID available (0-8)
Status: Complete
*/
int __cdecl pick_chassis(int factionID, int triadChk, int speedChk) {
	int chassisID = -1, bestSpeed = 0;
	for (int i = 0; i < MaxChassisNum; i++) {
		if (has_tech(Chassis[i].preqTech, factionID)
			&& (triadChk < 0 || triadChk == Chassis[i].triad)
			&& (!Chassis[i].missile || speedChk == -1)) {
			int speedCompare = 1;
			if (speedChk <= 0) { // 0, -1, -2
				if (!speedChk || triadChk != TRIAD_AIR || (speedChk == -1 && Chassis[i].missile)
					|| (speedChk == -2 && Chassis[i].range == 1)) {
					speedCompare = Chassis[i].speed + 1;
					if (speedCompare > bestSpeed) {
						bestSpeed = speedCompare;
						chassisID = i;
					}
				}
			}
			else if (Chassis[i].speed >= speedChk) { // 1, 2, 3
				if (Chassis[i].speed == speedChk) {
					speedCompare = 2;
				}
				if (speedCompare > bestSpeed) {
					bestSpeed = speedCompare;
					chassisID = i;
				}
			}
		}
	}
	return chassisID;
}

/*
Purpose: Check against faction's available tech for best available weapon meeting requirements.
		 The condition variable has a dual purpose of either maxCost or search for 1st instance of
		 a particular weapon mode.
Original Offset: 0057F0B0
Return Value: Best weapon ID available (0-25)
Status: Complete
*/
int __cdecl weapon_budget(int factionID, int condition, BOOL checkMode) {
	int weaponID = 0, bestOffense = -1;
	for (int i = 0; i < MaxWeaponNum; i++) {
		if (has_tech(Weapon[i].preqTech, factionID)) {
			if (condition >= 0) {
				if ((!checkMode || Weapon[i].mode < 2) && Weapon[i].cost <= condition
					&& Weapon[i].mode < 3 && Weapon[i].offenseRating < 99
					&& i != 23) { // Exclude Conventional Payload
					int offenseCompare = Weapon[i].offenseRating * 2;
					if (offenseCompare >= bestOffense) {
						bestOffense = offenseCompare;
						weaponID = i;
					}
				}
			}
			else if (Weapon[i].mode == -condition) {
				weaponID = i;
				break;
			}
		}
	}
	return weaponID;
}

/*
Purpose: Check against faction's available tech for best available armor meeting cost requirement.
Original Offset: 0057F150
Return Value: Best armor ID available (0-13)
Status: Complete
*/
int __cdecl armor_budget(int factionID, int maxCost) {
	int armorID = 0, bestDefense = -1;
	for (int i = 0; i < MaxArmorNum; i++) {
		if (has_tech(Armor[i].preqTech, factionID) && Armor[i].cost <= maxCost
			&& Armor[i].defenseRating >= 0) { // excludes PSI
			int defenseCompare = Armor[i].defenseRating;
			if (factionID >= 0 && !*ExpansionEnabled && i > 9) {
				defenseCompare = 1; // Pulse + Resonance in non-expansion mode
			}                       // This really only affects Pulse 8 Armor (Super Tensile Solids)
			defenseCompare *= 2;
			if (defenseCompare >= bestDefense) {
				armorID = i;
				bestDefense = defenseCompare;
			}
		}
	}
	return armorID;
}

/*
Purpose: Get index value of a particular ability's bitfield.
Original Offset: 00581170
Return Value: index
Status: Complete
*/
int __cdecl abil_index(int abilityID) {
	int index = 0;
	for (int check = abilityID; !(check & 1); index++) {
		check >>= 1;
	}
	return index;
}

/*
Purpose: Calculate movement penalty/cost.
Original Offset: 00593510
Return Value: Movement cost
Status: Complete
*/
int __cdecl hex_cost(int protoID, int factionID, int xCoordSrc, int yCoordSrc, int xCoordDst,
	int yCoordDst, BOOL toggle) {
	uint32_t bitDst = bit_at(xCoordDst, yCoordDst);
	if (is_ocean(xCoordDst, yCoordDst)) {
		if (bitDst & BIT_FUNGUS && altitude_at(xCoordDst, yCoordDst) == ALT_BIT_OCEAN_SHELF
			&& get_proto_triad(protoID) == TRIAD_SEA
			&& protoID != BSC_SEALURK // Bug fix
			&& protoID != BSC_ISLE_OF_THE_DEEP && !has_project(SP_XENOEMPATYH_DOME, factionID)) {
			return Rules->MoveRateRoads * 3;
		}
		return Rules->MoveRateRoads;
	}
	if (is_ocean(xCoordSrc, yCoordSrc)) {
		return Rules->MoveRateRoads;
	}
	if (protoID >= 0 && get_proto_triad(protoID) != TRIAD_LAND) {
		return Rules->MoveRateRoads;
	}
	// Land only conditions
	uint32_t bitSrc = bit_at(xCoordSrc, yCoordSrc);
	if (bitSrc & (BIT_MAGTUBE | BIT_BASE_IN_TILE) && bitDst & (BIT_MAGTUBE | BIT_BASE_IN_TILE)
		&& factionID) {
		return 0;
	}
	if ((bitSrc & (BIT_ROAD | BIT_BASE_IN_TILE) || bitSrc & BIT_FUNGUS && factionID > 0
		&& has_project(SP_XENOEMPATYH_DOME, factionID)) && bitDst & (BIT_ROAD | BIT_BASE_IN_TILE)
		&& factionID) {
		return 1;
	}
	if (factionID >= 0 && (has_project(SP_XENOEMPATYH_DOME, factionID) || !factionID
		|| protoID == BSC_MIND_WORMS || protoID == BSC_SPORE_LAUNCHER) && bitDst & BIT_FUNGUS) {
		return 1;
	}
	if (bitSrc & BIT_RIVER && bitDst & BIT_RIVER && x_dist(xCoordSrc, xCoordDst) == 1
		&& abs(yCoordSrc - yCoordDst) == 1 && factionID) {
		return 1;
	}
	if (VehPrototype[protoID].chassisID == CHSI_HOVERTANK
		|| has_abil(protoID, ABL_ANTIGRAV_STRUTS)) {
		return Rules->MoveRateRoads;
	}
	uint32_t cost = Rules->MoveRateRoads;
	if (rocky_at(xCoordDst, yCoordDst) > TERRAIN_BIT_ROLLING && !toggle) {
		cost += Rules->MoveRateRoads;
	}
	if (bitDst & BIT_FOREST && !toggle) {
		cost += Rules->MoveRateRoads;
	}
	if (factionID && bitDst & BIT_FUNGUS && (protoID >= MaxVehProtoFactionNum
		|| Weapon[VehPrototype[protoID].weaponID].offenseRating >= 0)) {
		uint8_t plan = VehPrototype[protoID].plan;
		if (plan != PLAN_TERRAFORMING && plan != PLAN_ALIEN_ARTIFACT
			&& PlayersData[factionID].socEffectActive.planet <= 0) {
			return cost + Rules->MoveRateRoads * 2;
		}
		uint32_t speed = speed_proto(protoID);
		if (cost <= speed) {
			return speed;
		}
	}
	return cost;
}

/*
Purpose: Move Veh to specified coordinates.
Original Offset: 005A59B0
Return Value: n/a
Status: Complete
*/
void __cdecl veh_put(int vehID, int xCoord, int yCoord) {
	veh_drop(veh_lift(vehID), xCoord, yCoord);
}

/*
Purpose: Get current health of vehID factoring in damage.
Original Offset: 005A59E0
Return Value: Health
Status: Complete
*/
uint32_t __cdecl veh_health(int vehID) {
	int protoID = Veh[vehID].protoID;
	int health = VehPrototype[protoID].plan != PLAN_ALIEN_ARTIFACT
		? range(VehPrototype[protoID].reactorID, 1, 100) * 10 : 1;
	return range(health - Veh[vehID].dmgIncurred, 0, 9999);
}

/*
Purpose: Calculates cost of prototype based on various factors. Optimized logic flow from original
		 without any difference to final calculation.
Original Offset: 005A5A60
Return Value: Cost of prototype
Status: Complete
*/
uint32_t __cdecl proto_cost(uint32_t chassisID, uint32_t weaponID, uint32_t armorID,
	uint32_t ability, uint32_t reactorID) {
	uint8_t weapCost = Weapon[weaponID].cost;
	// PB check: moved to start vs after 1st triad checks in original > no difference in logic
	if (Chassis[chassisID].missile && Weapon[weaponID].offenseRating >= 99) {
		return weapCost;
	}
	uint8_t triad = Chassis[chassisID].triad;
	uint32_t armorCost = Armor[armorID].cost;
	uint32_t speedCost = Chassis[chassisID].cost;
	int abilModifier = 0, flagsModifier = 0;
	if (ability) {
		for (int i = 0; i < MaxAbilityNum; i++) {
			if ((1 << i) & ability) {
				if (abilModifier) {
					abilModifier++; // Increased cost with more than one ability
				}
				int factor = Ability[i].costFactor;
				if (factor > 0) { // 1+ = Straight Cost; 25% increase per unit of cost
					abilModifier += factor;
				}
				else {
					switch (factor) {
					case 0: // None
					default:
						break;
						// Increases w/ ratio of weapon to armor: 0, 1, or 2. Rounded DOWN.
						// Never higher than 2.
					case -1:
						// fixed potential crash: this will never trigger in vanilla
						// but could with mods
						if (armorCost) {
							abilModifier += range(weapCost / armorCost, 0, 2);
						}
						break;
					case -2: // Increases w/ weapon value
						abilModifier += weapCost - 1;
						break;
					case -3: // Increases w/ armor value
						abilModifier += armorCost - 1;
						break;
					case -4: // Increases w/ speed value
						abilModifier += speedCost - 1;
						break;
					case -5: // Increases w/ weapon+armor value
						abilModifier += weapCost + armorCost - 2;
						break;
					case -6: // Increases w/ weapon+speed value
						abilModifier += weapCost + speedCost - 2;
						break;
					case -7: // Increases w/ armor+speed value
						abilModifier += armorCost + speedCost - 2;
						break;
					}
				}
				// 010000000000 - Cost increased for land units; Deep Radar
				// Shifted flag check into main ability loop rather than its
				// own loop at 1st triad checks
				if (Ability[i].flags & AFLAG_COST_INC_LAND_UNIT && triad == TRIAD_LAND) {
					// separate variable keeps logic same (two abilities, both with cost 0,
					// one with cost increase flag will trigger above "if (abilModifier)" if
					// this is directly abilModifier++)
					flagsModifier++;
				}
			}
		}
		abilModifier += flagsModifier; // adding here keeps logic consistent after optimization
	}
	if (triad == TRIAD_SEA) {
		armorCost /= 2;
		speedCost += reactorID;
	}
	else if (triad == TRIAD_AIR) {
		if (armorCost > 1) {
			armorCost *= reactorID * 2;
		}
		speedCost += reactorID * 2;
	} // TRIAD_LAND ability flag check moved into ability loop above
	uint32_t combatMod = armorCost / 2 + 1;
	if (combatMod < weapCost) { // which ever is greater
		combatMod = weapCost;
	}
	int protoMod;
	// shifted this check to top vs at end > no difference in logic
	if (combatMod == 1 && armorCost == 1 && speedCost == 1 && reactorID == RECT_FISSION) {
		protoMod = 1;
	}
	else {
		// (2 << n) == 2^(n + 1) ; (2 << n) / 2 == 2 ^ n;
		// will crash if reactorID is 0xFF > divide by zero; not putting in error checking
		// since this is unlikely even with modding however noting for future
		protoMod = ((speedCost + armorCost) * combatMod + ((2 << reactorID) / 2))
			/ (2 << reactorID);
		if (speedCost == 1) {
			protoMod = (protoMod / 2) + 1;
		}
		if (weapCost > 1 && Armor[armorID].cost > 1) {
			protoMod++;
			// moved inside nested check vs separate triad checks below > no difference in logic
			if (triad == TRIAD_LAND && speedCost > 1) {
				protoMod++;
			}
		}
		// excludes sea probes
		if (triad == TRIAD_SEA && Weapon[weaponID].mode != WPN_MODE_INFOWAR) {
			protoMod = (protoMod + 1) / 2;
		}
		else if (triad == TRIAD_AIR) {
			protoMod /= (Weapon[weaponID].mode > WPN_MODE_MISSILE) ? 2 : 4; // Non-combat : Combat
		}
		int reactorMod = (reactorID * 3 + 1) / 2;
		if (protoMod < reactorMod) { // which ever is greater
			protoMod = reactorMod;
		}
	}
	return (protoMod * (abilModifier + 4) + 2) / 4;
}

/*
Purpose: Calculates base cost of prototype.
Original Offset: 005A5D00
Return Value: Cost of prototype
Status: Complete
*/
uint32_t __cdecl base_cost(uint32_t protoID) {
	return proto_cost(VehPrototype[protoID].chassisID, VehPrototype[protoID].weaponID,
		VehPrototype[protoID].armorID, 0, VehPrototype[protoID].reactorID);
}

/*
Purpose: Create a new prototype. Sets initial values for everything except vehName and preqTech.
Original Offset: 005A5D40
Return Value: n/a
Status: Complete
*/
void __cdecl make_proto(int protoID, uint32_t chassisID, uint32_t weaponID, uint32_t armorID,
	uint32_t ability, uint32_t reactorID) {
	int unkLocal1 = 0; // TODO: Identify
	if (protoID >= MaxVehProtoFactionNum) {
		BOOL cond1 = false, cond2 = false, cond3 = false;
		int protoIDLoop;
		for (int i = 0; i < 128; i++) {
			protoIDLoop = i % MaxVehProtoFactionNum;
			if ((i % MaxVehProtoFactionNum) != i) {
				protoIDLoop += protoID - protoID % MaxVehProtoFactionNum;
			}
			int flagsCheck = VehPrototype[protoIDLoop].flags;
			if (flagsCheck & PROTO_ACTIVE) {
				if ((protoIDLoop <= MaxVehProtoFactionNum &&
					has_tech(VehPrototype[protoIDLoop].preqTech, protoID / MaxVehProtoFactionNum))
					|| (protoIDLoop > MaxVehProtoFactionNum
						&& (flagsCheck & PROTO_TYPED_COMPLETE))) {
					uint32_t loopWeaponID = VehPrototype[protoIDLoop].weaponID;
					if (loopWeaponID == weaponID) {
						cond1 = true;
					}
					uint32_t loopArmorID = VehPrototype[protoIDLoop].armorID;
					if (loopArmorID == armorID) {
						cond2 = true;
					}
					uint32_t loopChassisID = VehPrototype[protoIDLoop].chassisID;
					if (loopChassisID == chassisID) {
						cond3 = true;
					}
					int offRating = Weapon[weaponID].offenseRating;
					if (offRating > 0 && Weapon[loopWeaponID].offenseRating >= offRating) {
						cond1 = true;
					}
					int defRating = Armor[armorID].defenseRating;
					if (defRating > 0 && Armor[loopArmorID].defenseRating >= defRating) {
						cond2 = true;
					}
					if (Chassis[chassisID].triad == Chassis[loopChassisID].triad
						&& Chassis[loopChassisID].speed >= Chassis[chassisID].speed) {
						cond3 = true;
					}
				}
			}
		}
		if (cond1 && cond2 && cond3) {
			unkLocal1 = (protoIDLoop >= MaxVehProtoFactionNum
				&& VehPrototype[protoIDLoop].flags & 0x10) ? 3 : 1;
		}
	}
	VehPrototype[protoID].chassisID = (uint8_t)chassisID;
	VehPrototype[protoID].weaponID = (uint8_t)weaponID;
	VehPrototype[protoID].armorID = (uint8_t)armorID;
	VehPrototype[protoID].abilityFlags = ability;
	VehPrototype[protoID].reactorID = (uint8_t)reactorID;
	VehPrototype[protoID].cost = (uint8_t)proto_cost(chassisID, weaponID, armorID, ability,
		reactorID);
	VehPrototype[protoID].carryCapacity = (Weapon[weaponID].mode == WPN_MODE_TRANSPORT) ?
		(uint8_t)transport_val(chassisID, ability, reactorID) : 0;
	// set plan & unk1
	if (Chassis[chassisID].missile) {
		if (Weapon[weaponID].offenseRating < 99) { // non-PB missiles
			if (weaponID == WPN_TECTONIC_PAYLOAD) {
				VehPrototype[protoID].plan = PLAN_TECTONIC_MISSILE;
				VehPrototype[protoID].unk_1 = 22;
			}
			else if (weaponID == WPN_FUNGAL_PAYLOAD) {
				VehPrototype[protoID].plan = PLAN_FUNGAL_MISSILE;
				VehPrototype[protoID].unk_1 = 23;
			}
			else { // Conventional Payload
				VehPrototype[protoID].plan = PLAN_OFFENSIVE;
				VehPrototype[protoID].unk_1 = 13;
			}
		}
		else {
			VehPrototype[protoID].plan = PLAN_PLANET_BUSTER;
			VehPrototype[protoID].unk_1 = 14;
		}
	}
	else if (Weapon[weaponID].mode >= WPN_MODE_TRANSPORT) { // Non-combat
		VehPrototype[protoID].plan = Weapon[weaponID].mode;
		VehPrototype[protoID].unk_1 = Weapon[weaponID].mode + 0x20;
	}
	else if (Chassis[chassisID].triad == TRIAD_SEA) { // combat sea
		VehPrototype[protoID].plan = PLAN_NAVAL_SUPERIORITY;
		VehPrototype[protoID].unk_1 = 6; // same value as plan
	}
	else if (Chassis[chassisID].triad == TRIAD_AIR) { // combat air
		if (has_abil(protoID, ABL_AIR_SUPERIORITY)) {
			VehPrototype[protoID].plan = PLAN_AIR_SUPERIORITY;
			VehPrototype[protoID].unk_1 = 9;
		}
		else {
			VehPrototype[protoID].plan = PLAN_OFFENSIVE;
			VehPrototype[protoID].unk_1 = 8;
		}
	}
	else { // TRIAD_LAND combat unit
		VehPrototype[protoID].plan = PLAN_OFFENSIVE;
		if (Armor[armorID].defenseRating > 1) {
			VehPrototype[protoID].plan = PLAN_DEFENSIVE;
		}
		if (Weapon[weaponID].offenseRating >= Armor[armorID].defenseRating
			&& VehPrototype[protoID].plan == PLAN_DEFENSIVE) {
			VehPrototype[protoID].plan = PLAN_COMBAT;
		}
		if (Chassis[chassisID].speed > 1
			&& Weapon[weaponID].offenseRating > Armor[armorID].defenseRating) {
			VehPrototype[protoID].plan = PLAN_OFFENSIVE;
		}
		if (ability & (ABL_ARTILLERY | ABL_DROP_POD | ABL_AMPHIBIOUS)) {
			VehPrototype[protoID].plan = PLAN_OFFENSIVE;
		}
		VehPrototype[protoID].unk_1 = 3;
		if (Armor[armorID].defenseRating > Weapon[weaponID].offenseRating) {
			VehPrototype[protoID].unk_1 = 2;
		}
		if (Weapon[weaponID].offenseRating > Armor[armorID].defenseRating * 2) {
			VehPrototype[protoID].unk_1 = 1;
		}
		// changed from '>= 2' since '>= 3' will always overwrite it
		if (Chassis[chassisID].speed == 2) {
			VehPrototype[protoID].unk_1 = 4;
		}
		// added else, no point in checking if speed==2
		else if (Chassis[chassisID].speed >= 3) {
			VehPrototype[protoID].unk_1 = 5;
		}
		if (Weapon[weaponID].offenseRating == 1 && Armor[armorID].defenseRating == 1) {
			if (Chassis[chassisID].speed > 1) {
				VehPrototype[protoID].plan = PLAN_RECONNAISANCE;
			}
			if (has_abil(protoID, ABL_POLICE_2X)) {
				VehPrototype[protoID].plan = PLAN_DEFENSIVE;
			}
			VehPrototype[protoID].unk_1 = 10;
		}
	}
	// set final values
	VehPrototype[protoID].obsoleteFactions = 0;
	VehPrototype[protoID].combatFactions = (protoID >= MaxVehProtoFactionNum) ?
		1 << (protoID / MaxVehProtoFactionNum) : -1;
	VehPrototype[protoID].flags = unkLocal1 ? ((unkLocal1 & 2) ? 0x115 : 0x105) : PROTO_ACTIVE;
	VehPrototype[protoID].iconOffset = -1;
}

/*
Purpose: Search for the 1st prototype (basic + faction specific) that matches the specified plan.
Original Offset: 005AED50
Return Value: protoID or -1 if not found
Status: Complete
*/
int __cdecl get_plan(uint32_t factionID, uint32_t plan) {
	for (int i = 0; i < 128; i++) {
		uint32_t protoID = (i < MaxVehProtoFactionNum) ? i
			: (factionID * MaxVehProtoFactionNum) + i - MaxVehProtoFactionNum;
		if (VehPrototype[protoID].flags & PROTO_ACTIVE
			&& !(VehPrototype[protoID].obsoleteFactions & (1 << factionID))
			&& (protoID >= MaxVehProtoFactionNum
				|| has_tech(VehPrototype[protoID].preqTech, factionID))
			&& VehPrototype[protoID].plan == plan) {
			return protoID;
		}
	}
	return -1;
}

/*
Purpose: Determine if Veh wants to wake up based on certain conditions and preferences. Optional 
		 parameter for spotted vehID (-1 to skip).
Original Offset: 005B5EA0
Return Value: Does Veh want to wake? true/false
Status: Complete
*/
BOOL __cdecl want_to_wake(uint32_t factionID, uint32_t vehID, int spottedVehID) {
	int baseFactionID;
	uint32_t triad = get_triad(vehID), vehFactionID = Veh[vehID].factionID;
	if (triad == TRIAD_AIR && Veh[vehID].terraformingTurns && Veh[vehID].orders == ORDER_MOVE_TO
		&& (baseFactionID = base_who(Veh[vehID].waypoint_xCoord[0], Veh[vehID].waypoint_yCoord[0]),
			baseFactionID >= 0)) {
		if (((uint32_t)baseFactionID == vehFactionID) 
			|| has_treaty(vehFactionID, baseFactionID, DTREATY_PACT)) {
			return false;
		}
	}
	BOOL wantToWake;
	if (*IsMultiplayerNet) { // restructured to be more efficient with same logic
		if (has_treaty(vehFactionID, factionID, DTREATY_PACT)) {
			wantToWake = false;
		}
		else if (has_treaty(vehFactionID, factionID, DTREATY_TREATY)) {
			wantToWake = true;
		}
		else if (has_treaty(vehFactionID, factionID, DTREATY_TRUCE)) {
			wantToWake = true;
		}
		else {
			wantToWake = true;
		}
	}
	else {
		if (has_treaty(vehFactionID, factionID, DTREATY_PACT)) {
			wantToWake = *GamePreferences & PREF_AUTO_END_MOVE_SPOT_VEH_PACT;
		}
		else if (has_treaty(vehFactionID, factionID, DTREATY_TREATY)) {
			wantToWake = *GamePreferences & PREF_AUTO_END_MOVE_SPOT_VEH_TREATY;
		}
		else if (has_treaty(vehFactionID, factionID, DTREATY_TRUCE)) {
			wantToWake = *GamePreferences & PREF_AUTO_END_MOVE_SPOT_VEH_TRUCE;
		}
		else {
			wantToWake = *GamePreferences & PREF_AUTO_END_MOVE_SPOT_VEH_WAR;
		}
		if (!(*GamePreferences & PREF_AUTO_DONT_END_MOVE_DIFF_TRIAD)) {
			return wantToWake;
		}
	}
	if (spottedVehID >= 0) { // removed a check if vehID is also >= 0 (always unsigned)
		uint32_t triadSpotted = get_triad(spottedVehID);
		if (triad != triadSpotted && ((Veh[vehID].state & (VSTATE_UNK_1000000 | VSTATE_UNK_200))
			!= (VSTATE_UNK_1000000 | VSTATE_UNK_200) || triadSpotted == TRIAD_LAND)) {
			wantToWake = false;
		}
	}
	return wantToWake;
}

/*
Purpose: Wake up veh meeting conditions from the stack where vehID is located.
Original Offset: 005B6060
Return Value: n/a
Status: Complete
*/
void __cdecl wake_stack(int vehID) {
	for (int i = veh_top(vehID); i >= 0; i = Veh[i].nextVehIDStack) {
		int vehIDWayPoint, xCoord, yCoord;
		if (Veh[i].orders == ORDER_SENTRY_BOARD && (get_triad(i) || (xCoord = Veh[i].xCoord,
			yCoord = Veh[i].yCoord, !is_ocean(xCoord, yCoord)
			&& ((vehIDWayPoint = Veh[i].waypoint_xCoord[0], vehIDWayPoint < 0)
				|| get_triad(vehIDWayPoint) != TRIAD_AIR || base_who(xCoord, yCoord) >= 0
				|| bit_at(xCoord, yCoord) & BIT_AIRBASE)))) {
			Veh[i].orders = ORDER_NONE;
			Veh[i].state &= ~VSTATE_EXPLORE;
		}
	}
}

/*
Purpose: Move all Veh(s) in same stack as Veh to the destination coordinates.
Original Offset: 005B8AF0
Return Value: n/a
Status: Complete
*/
void __cdecl stack_put(int vehID, int xCoord, int yCoord) {
	int nextVehID = veh_top(vehID), vehIDLoop;
	if (nextVehID >= 0) {
		do {
			vehIDLoop = Veh[nextVehID].nextVehIDStack;
			veh_put(nextVehID, xCoord, yCoord);
			nextVehID = vehIDLoop;
		} while (vehIDLoop >= 0);
	}
}

/*
Purpose: Sort stack with transports moved to top.
Original Offset: 005B8B60
Return Value: n/a
Status: Complete
*/
void __cdecl stack_sort(int vehID) {
	int16_t xCoord = Veh[vehID].xCoord, yCoord = Veh[vehID].yCoord;
	int nextVehID = veh_top(vehID), vehIDPut = -1, vehIDLoop;
	if (nextVehID >= 0) {
		do {
			vehIDLoop = Veh[nextVehID].nextVehIDStack; // removed redundant < 0 check
			if (veh_cargo(nextVehID) || has_abil(Veh[nextVehID].protoID, ABL_CARRIER)) {
				vehIDPut = nextVehID;
				veh_put(nextVehID, -3, -3);
			}
			nextVehID = vehIDLoop;
		} while (vehIDLoop >= 0);
		stack_put(vehIDPut, xCoord, yCoord);
	}
}

/*
Purpose: Sort stack with colony pods at top followed by combat/offensive units.
Original Offset: 005B8C90
Return Value: n/a
Status: Complete
*/
void __cdecl stack_sort_2(int vehID) {
	int16_t xCoord = Veh[vehID].xCoord, yCoord = Veh[vehID].yCoord;
	int nextVehID = veh_top(vehID), vehIDPut = -1, vehIDLoop;
	if (nextVehID >= 0) {
		do {
			vehIDLoop = Veh[nextVehID].nextVehIDStack; // removed redundant < 0 check
			if (VehPrototype[Veh[nextVehID].protoID].plan == PLAN_COLONIZATION) { // Colony Pods
				vehIDPut = nextVehID;
				veh_put(nextVehID, -3, -3);
			}
			nextVehID = vehIDLoop;
		} while (vehIDLoop >= 0);
	}
	nextVehID = veh_at(xCoord, yCoord);
	if (nextVehID >= 0) {
		do {
			vehIDLoop = Veh[nextVehID].nextVehIDStack; // removed redundant < 0 check
			if (VehPrototype[Veh[nextVehID].protoID].plan <= PLAN_COMBAT) {
				vehIDPut = nextVehID;
				veh_put(nextVehID, -3, -3);
			}
			nextVehID = vehIDLoop;
		} while (vehIDLoop >= 0);
	}
	stack_put(vehIDPut, xCoord, yCoord);
}

/*
Purpose: Refresh/fix stack. Used by DirectPlay multiplayer only.
Original Offset: 005B8E10
Return Value: vehID or top vehID of stack; return is checked if >= 0
Status: Complete
*/
int __cdecl stack_fix(int vehID) {
	if (vehID < 0 || !*IsMultiplayerNet
		|| (Veh[vehID].nextVehIDStack < 0 && Veh[vehID].prevVehIDStack < 0)) {
		return vehID; // invalid vehID, not DirectPlay MP or no stack
	}
	for (int i = 0; i < *VehCurrentCount; i++) {
		// Bug fix: Original would compare against the exact same source coordinates (both vehID
		// instead of one being iterator) likely causing a performance hit.
		if (Veh[vehID].xCoord == Veh[i].xCoord && Veh[vehID].yCoord == Veh[i].yCoord) {
			veh_promote(i);
			stack_sort(vehID);
		}
	}
	return veh_top(vehID);
}

/*
Purpose: Various Veh stack related calculations based on type parameter (0-19) and conditions.
Original Offset: 005B9580
Return Value: Dependent on type parameter
Status: Complete
*/
int __cdecl stack_check(int vehID, uint32_t type, int cond1, int cond2, int cond3) {
	int retVal = 0;
	uint32_t plan, chas;
	for (int i = veh_top(vehID); i >= 0; i = Veh[i].nextVehIDStack) {
		switch (type) {
		case 0:
			if ((cond2 < 0 || Veh[i].factionID == cond2) && Veh[i].protoID == cond1) {
				retVal++;
			}
			break;
		case 1:
			if (cond1 < 0 || Veh[i].factionID == cond1) {
				retVal++;
			}
			break;
		case 2:
			if ((cond2 < 0 || Veh[i].factionID == cond2)
				&& VehPrototype[Veh[i].protoID].plan == cond1) {
				retVal++;
			}
			break;
		case 3:
			if ((cond2 < 0 || Veh[i].factionID == cond2) && get_triad(i) == cond1) {
				retVal++;
			}
			break;
		case 4:
			if (cond1 < 0 || Veh[i].factionID == cond1) {
				retVal += Weapon[VehPrototype[Veh[i].protoID].weaponID].offenseRating;
			}
			break;
		case 5:
			if (cond1 < 0 || Veh[i].factionID == cond1) {
				retVal += Armor[VehPrototype[Veh[i].protoID].armorID].defenseRating;
			}
			break;
		case 6:
			if ((cond2 < 0 || Veh[i].factionID == cond2) && has_abil(Veh[i].protoID, cond1)) {
				retVal++;
			}
			break;
		case 7:
			if (cond1 < 0 || Veh[i].factionID == cond1) {
				retVal += VehPrototype[Veh[i].protoID].cost;
			}
			break;
		case 8:
			if (cond1 < 0 || Veh[i].factionID == cond1) {
				uint32_t triad = get_triad(i);
				if (triad == TRIAD_LAND) {
					retVal--;
				}
				else if (triad == TRIAD_SEA) {
					retVal += veh_cargo(i);
				}
			}
			break;
		case 9:
			if ((cond2 < 0 || Veh[i].factionID == cond2) && Veh[i].orders == cond1) {
				retVal++;
			}
			break;
		case 10:
			if (Veh[i].factionID == cond1) {
				retVal++;
			}
			break;
		case 11:
			if ((cond3 < 0 || Veh[i].factionID == cond3)
				&& (Veh[i].state & cond1) == (uint32_t)cond2) {
				retVal++;
			}
			break;
		case 12:
			if (cond1 < 0 || Veh[i].factionID == cond1) {
				uint32_t factionID = Veh[i].factionID;
				int protoID = Veh[i].protoID;
				if (arm_strat(VehPrototype[protoID].armorID, factionID)
			> weap_strat(VehPrototype[protoID].weaponID, factionID)) {
					retVal++;
				}
			}
			break;
		case 13:
			if ((cond1 < 0 || Veh[i].factionID == cond1)
				&& Chassis[VehPrototype[Veh[i].protoID].chassisID].missile) {
				retVal++;
			}
			break;
		case 14:
			if ((cond1 < 0 || Veh[i].factionID == cond1)
				&& (plan = VehPrototype[Veh[i].protoID].plan,
					plan == PLAN_DEFENSIVE || plan == PLAN_COMBAT)) {
				retVal++;
			}
			break;
		case 15:
			if ((cond1 < 0 || Veh[i].factionID == cond1) && can_arty(Veh[i].protoID, TRUE)) {
				retVal++;
			}
			break;
		case 16:
			if ((cond1 < 0 || Veh[i].factionID == cond1)
				&& (plan = VehPrototype[Veh[i].protoID].plan, plan == PLAN_DEFENSIVE
					|| plan == PLAN_RECONNAISANCE || plan == PLAN_COMBAT)) {
				retVal++;
			}
			break;
		case 17:
			if ((cond2 < 0 || Veh[i].factionID == cond2)
				&& VehPrototype[Veh[i].protoID].unk_1 == cond1) {
				retVal++;
			}
			break;
		case 18:
			if ((cond1 < 0 || Veh[i].factionID == cond1)
				&& (chas = VehPrototype[Veh[i].protoID].chassisID,
					Chassis[chas].triad == TRIAD_AIR && Chassis[chas].range > 1)) {
				retVal++;
			}
			break;
		case 19:
			if ((cond1 < 0 || Veh[i].factionID == cond1)
				&& !Weapon[VehPrototype[Veh[i].protoID].weaponID].offenseRating) {
				retVal++;
			}
			break;
		default:
			break;
		}
	}
	return retVal;
}

/*
Purpose: Check to see whether provided faction and base can build a specific prototype. Checks are
		 included to prevent SMACX specific Veh from being built in SMAC mode.
Original Offset: 005BA910
Return Value: Is veh available to faction/base? true/false
Status: Complete
*/
BOOL __cdecl veh_avail(int protoID, int factionID, int baseID) {
	if (!(VehPrototype[protoID].flags & PROTO_ACTIVE)
		|| (VehPrototype[protoID].obsoleteFactions & (1 << factionID))) {
		return false;
	}
	if (protoID < MaxVehProtoFactionNum) {
		if (!has_tech(VehPrototype[protoID].preqTech, factionID)) {
			return false;
		}
	}
	if (VehPrototype[protoID].plan == PLAN_COLONIZATION && *GameRules & RULES_SCN_NO_COLONY_PODS) {
		return false;
	}
	if (baseID >= 0 && get_proto_triad(protoID) == TRIAD_SEA && !is_port(baseID, false)) {
		return false;
	}
	uint8_t weapID;
	uint32_t abilFlag;
	if (!*ExpansionEnabled && (VehPrototype[protoID].armorID > ARM_PSI_DEFENSE
		|| (weapID = VehPrototype[protoID].weaponID, weapID == WPN_RESONANCE_LASER
			|| weapID == WPN_RESONANCE_BOLT || weapID == WPN_STRING_DISRUPTOR
			|| weapID == WPN_TECTONIC_PAYLOAD || weapID == WPN_FUNGAL_PAYLOAD)
		|| (abilFlag = VehPrototype[protoID].abilityFlags, abilFlag == ABL_SOPORIFIC_GAS
			|| abilFlag == ABL_DISSOCIATIVE_WAVE || abilFlag == ABL_MARINE_DETACHMENT
			|| abilFlag == ABL_FUEL_NANOCELLS || abilFlag == ABL_ALGO_ENHANCEMENT)
		|| protoID == BSC_SEALURK || protoID == BSC_SPORE_LAUNCHER
		|| protoID == BSC_BATTLE_OGRE_MK1 || protoID == BSC_BATTLE_OGRE_MK2
		|| protoID == BSC_BATTLE_OGRE_MK3 || protoID == BSC_FUNGAL_TOWER
		|| protoID == BSC_UNITY_MINING_LASER)) {
		return false;
	}
	if (protoID < MaxVehProtoFactionNum) {
		return true;
	}
	return (protoID / MaxVehProtoFactionNum) == factionID;
}

/*
Purpose: Determine whether a faction wants the specified protoID based on the faction's current
		 prototype designs.
Original Offset: 005BE100
Return Value: Does faction want prototype? true/false
Status: Complete
*/
BOOL __cdecl wants_prototype(uint32_t protoID, uint32_t factionID) {
	uint32_t flags = VehPrototype[protoID].flags;
	if (!(flags & PROTO_ACTIVE) || !(flags & PROTO_TYPED_COMPLETE)) {
		return false;
	}
	uint32_t protoOffset = factionID * MaxVehProtoFactionNum;
	for (uint32_t i = 0; i < MaxVehProtoFactionNum; i++) {
		uint32_t protoIDCmp = protoOffset + i;
		uint32_t flagsCmp = VehPrototype[protoIDCmp].flags;
		if (flagsCmp & PROTO_ACTIVE && flagsCmp & PROTO_TYPED_COMPLETE
			&& VehPrototype[protoID].plan == VehPrototype[protoIDCmp].plan) {
			uint8_t chas = VehPrototype[protoID].chassisID,
				chasCmp = VehPrototype[protoIDCmp].chassisID;
			if (Chassis[chas].triad == Chassis[chasCmp].triad) {
				uint8_t weapIDCmp = VehPrototype[protoIDCmp].weaponID;
				uint8_t modeCmp = Weapon[weapIDCmp].mode;
				int8_t offRatingCmp;
				if ((modeCmp > WPN_MODE_MISSILE)
					? (modeCmp == Weapon[VehPrototype[protoID].weaponID].mode)
					: (offRatingCmp = Weapon[weapIDCmp].offenseRating, offRatingCmp < 0)
					? (offRatingCmp == Weapon[VehPrototype[protoID].weaponID].offenseRating)
					: (offRatingCmp >= Weapon[VehPrototype[protoID].weaponID].offenseRating)) {
					uint8_t armIDCmp = VehPrototype[protoIDCmp].armorID;
					if ((Armor[armIDCmp].defenseRating <= 0)
						? (armIDCmp == VehPrototype[protoID].armorID)
						: (armor_val(protoIDCmp, factionID) >= armor_val(protoID, factionID))) {
						if (Chassis[chasCmp].speed >= Chassis[chas].speed) {
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

/*
Purpose: Check coordinates for Veh and if found return topmost id.
Original Offset: 005BFE90
Return Value: vehID or -1 if nothing found or on error
Status: Complete
*/
int __cdecl veh_at(int xCoord, int yCoord) {
	if (on_map(xCoord, yCoord) && !(bit_at(xCoord, yCoord) & BIT_VEH_IN_TILE)) {
		return -1; // not found
	}
	for (int vehID = 0; vehID < *VehCurrentCount; vehID++) {
		if (Veh[vehID].xCoord == xCoord && Veh[vehID].yCoord == yCoord) {
			return veh_top(vehID);
		}
	}
	if (!on_map(xCoord, yCoord)) {
		return -1;
	}
	if (!*VehBitError) {
		log_say("Vehicle Bit Error  (x, y)", xCoord, yCoord, 0);
	}
	if (*GameState & STATE_SCENARIO_EDITOR || *GameState & STATE_DEBUG_MODE || *IsMultiplayerNet) {
		if (*VehBitError) {
			return -1;
		}
		*VehBitError = true;
	}
	rebuild_vehicle_bits();
	return -1;
}

/*
Purpose: Check whether prototype has a specific ability
Original Offset: 005BF1F0
Return Value: Does prototype have ability? true/false
Status: Complete
*/
BOOL __cdecl has_abil(int protoID, int abilityID) {
	if (VehPrototype[protoID].abilityFlags & abilityID) {
		return true;
	}
	uint32_t factionID = protoID / MaxVehProtoFactionNum;
	if (!factionID) {
		return false; // skip basic prototypes from #UNITS
	}
	if (is_alien_faction(factionID) && abilityID == ABL_DEEP_RADAR) {
		return true; // Caretakers + Usurpers > "Deep Radar" ability for all units
	}
	for (int i = 0; i < Players[factionID].factionBonusCount; i++) {
		if (Players[factionID].factionBonusID[i] == RULE_FREEABIL) {
			int abilBonusID = Players[factionID].factionBonusVal1[i];
			if (has_tech(Ability[abilBonusID].preqTech, factionID) &&
				(abilityID & (1 << abilBonusID))) {
				// Pirates > "Marine Detachment" ability for combat sea units with Adaptive Doctrine
				return true;
			}
		}
	}
	if (VehPrototype[protoID].weaponID == WPN_PROBE_TEAM && abilityID == ABL_ALGO_ENHANCEMENT
		&& has_project(SP_NETHACK_TERMINUS, factionID)) {
		return true; // All Probe Teams act as though they have the "Algorithmic Enhancement"
	}
	return false;
}

/*
Purpose: Temporarily remove Veh from current square and stack in preparation for another action such
		 as interacting with stack, moving or killing it.
Original Offset: 005BFFA0
Return Value: vehID
Status: Complete
*/
int __cdecl veh_lift(int vehID) {
	BOOL prevStackExists = false;
	int16_t prevVehID = Veh[vehID].prevVehIDStack, nextVehID = Veh[vehID].nextVehIDStack;
	if (prevVehID >= 0) {
		prevStackExists = true;
		Veh[prevVehID].nextVehIDStack = nextVehID;
	}
	int xCoord = Veh[vehID].xCoord, yCoord = Veh[vehID].yCoord;
	if (nextVehID >= 0) {
		Veh[nextVehID].prevVehIDStack = prevVehID;
	}
	else if (!prevStackExists && on_map(xCoord, yCoord)) {
		bit_set(xCoord, yCoord, BIT_VEH_IN_TILE, false);
	}
	*VehDropLiftVehID = vehID;
	*VehLift_xCoord = xCoord;
	*VehLift_yCoord = yCoord;
	Veh[vehID].xCoord = -1;
	Veh[vehID].yCoord = -1;
	Veh[vehID].nextVehIDStack = -1;
	Veh[vehID].prevVehIDStack = -1;
	return vehID;
}

/*
Purpose: Move/drop the specified Veh to coordinates.
Original Offset: 005C0080
Return Value: vehID (param1), doesn't seem to be used.
Status: Complete
*/
int __cdecl veh_drop(int vehID, int xCoord, int yCoord) {
	int vehIDDest = veh_at(xCoord, yCoord);
	Veh[vehID].nextVehIDStack = (int16_t)vehIDDest;
	Veh[vehID].prevVehIDStack = -1;
	Veh[vehID].xCoord = (int16_t)xCoord;
	Veh[vehID].yCoord = (int16_t)yCoord;
	*VehDropLiftVehID = -1;
	if (vehIDDest < 0) {
		if (yCoord < 0) {
			return vehID;
		}
		if (on_map(xCoord, yCoord) && !(bit_at(xCoord, yCoord) & BIT_BASE_IN_TILE)) {
			owner_set(xCoord, yCoord, Veh[vehID].factionID);
		}
	}
	else {
		Veh[vehIDDest].prevVehIDStack = (int16_t)vehID;
	}
	if (on_map(xCoord, yCoord)) {
		uint32_t flags = (Veh[vehID].factionID && get_triad(vehID) != TRIAD_AIR)
			? BIT_SUPPLY_REMOVE | BIT_VEH_IN_TILE : BIT_VEH_IN_TILE;
		bit_set(xCoord, yCoord, flags, true);
	}
	return vehID;
}

/*
Purpose: Set Veh status to sentry/board.
Original Offset: 005C01A0
Return Value: n/a
Status: Complete
*/
void __cdecl sleep(uint32_t vehID) {
	Veh[vehID].orders = ORDER_SENTRY_BOARD;
	Veh[vehID].waypoint_xCoord[0] = -1;
	Veh[vehID].waypoint_yCoord[0] = 0;
}

/*
Purpose: Move vehID to bottom of stack.
Original Offset: 005C01D0
Return Value: n/a
Status: Complete
*/
void __cdecl veh_demote(int vehID) {
	if (vehID >= 0) {
		int16_t nextVehID = Veh[vehID].nextVehIDStack;
		if (nextVehID >= 0) {
			int16_t lastVehID;
			do {
				lastVehID = nextVehID;
				nextVehID = Veh[lastVehID].nextVehIDStack;
			} while (nextVehID >= 0);

			if (lastVehID != vehID) {
				veh_lift(vehID);
				Veh[lastVehID].nextVehIDStack = (int16_t)vehID;
				Veh[vehID].prevVehIDStack = lastVehID;
				Veh[vehID].nextVehIDStack = -1;
				Veh[vehID].xCoord = Veh[lastVehID].xCoord;
				Veh[vehID].yCoord = Veh[lastVehID].yCoord;
			}
		}
	}
}

/*
Purpose: Move the specified vehID to the top of the stack.
Original Offset: 005C0260
Return Value: n/a
Status: Complete
*/
void __cdecl veh_promote(int vehID) {
	int vehIDTop = veh_top(vehID);
	if (vehIDTop >= 0 && vehIDTop != vehID) {
		veh_put(vehID, Veh[vehIDTop].xCoord, Veh[vehIDTop].yCoord);
	}
}

/*
Purpose: Clear specified Veh.
Original Offset: 005C02D0
Return Value: n/a
Status: Complete
*/
void __cdecl veh_clear(uint32_t vehID, int protoID, uint32_t factionID) {
	Veh[vehID].xCoord = -4;
	Veh[vehID].yCoord = -4;
	Veh[vehID].yearEndLurking = 0;
	Veh[vehID].unk_1 = 0;
	Veh[vehID].flags = 0;
	Veh[vehID].factionID = (uint8_t)factionID;
	Veh[vehID].protoID = (int16_t)protoID;
	Veh[vehID].nextVehIDStack = -1;
	Veh[vehID].prevVehIDStack = -1;
	Veh[vehID].waypointCount = 0;
	Veh[vehID].patrolCurrentPoint = 0;
	Veh[vehID].orders = 0;
	for (int i = 0; i < 4; i++) {
		Veh[vehID].waypoint_xCoord[i] = -1;
		Veh[vehID].waypoint_yCoord[i] = -1;
	}
	Veh[vehID].state = 0;
	Veh[vehID].movesExpended = 0;
	Veh[vehID].dmgIncurred = 0;
	Veh[vehID].orderAutoType = 0;
	Veh[vehID].terraformingTurns = 0;
	Veh[vehID].unk_6 = 0;
	Veh[vehID].moveToAIType = 0;
	Veh[vehID].visibility = 0;
	Veh[vehID].homeBaseID = -1;
	Veh[vehID].morale = (uint8_t)(Players[factionID].ruleMorale + 1);
	Veh[vehID].unk_5 = 2;
	Veh[vehID].probeAction = 0;
	Veh[vehID].probeSabotageID = 0;
}

/*
Purpose: Check if unit can perform artillery combat. The 2nd parameter determines how sea units
		 are treated.
Original Offset: 005C0DB0
Return Value: Has artillery ability? true/false
Status: Complete
*/
BOOL __cdecl can_arty(int protoID, BOOL seaTriadRetn) {
	if ((Weapon[VehPrototype[protoID].weaponID].offenseRating <= 0 // PSI + non-combat
		|| Armor[VehPrototype[protoID].armorID].defenseRating < 0) // PSI
		&& protoID != BSC_SPORE_LAUNCHER) { // Spore Launcher exception
		return false;
	}
	uint8_t triad = get_proto_triad(protoID);
	if (triad == TRIAD_SEA) {
		return seaTriadRetn; // cursory check shows this value always being set to true
	}
	if (triad == TRIAD_AIR) {
		return false;
	}
	return has_abil(protoID, ABL_ARTILLERY); // TRIAD_LAND
}

/*
Purpose: Calculate Veh morale. TODO: Determine if 2nd param is toggle for display vs actual morale.
Original Offset: 005C0E40
Return Value: Morale value
Status: Complete
*/
uint32_t __cdecl morale_veh(uint32_t vehID, BOOL checkDroneRiot, int factionIDvsNative) {
	uint32_t factionID = Veh[vehID].factionID;
	if (!factionID) {
		return morale_alien(vehID, factionIDvsNative);
	}
	int16_t protoID = Veh[vehID].protoID;
	if (VehPrototype[protoID].plan == PLAN_INFO_WARFARE) { // probe
		int probeMorale = range(PlayersData[factionID].socEffectActive.probe, 0, 3);
		probeMorale += has_project(SP_TELEPATHIC_MATRIX, factionID) ? 2 : 0;
		for (int i = 0; i < MaxTechnologyNum; i++) {
			if (Technology[i].flags & TFLAG_IMPROVED_PROBES && has_tech(i, factionID)) {
				probeMorale++;
			}
		}
		probeMorale += Veh[vehID].morale;
		return range(probeMorale, 2, 6);
	}
	char offenseRating = Weapon[VehPrototype[protoID].weaponID].offenseRating;
	if (protoID < MaxVehProtoFactionNum && offenseRating < 0) {
		return range(Veh[vehID].morale, 0, 6); // Basic Psi Veh
	}
	// everything else
	int moraleModifier = range(PlayersData[factionID].socEffectActive.morale, -4, 4);
	if (moraleModifier <= -2) {
		moraleModifier++;
	}
	else if (moraleModifier >= 2) {
		moraleModifier--;
	}
	int ruleMorale = Players[factionID].ruleMorale; // different from 'SOCIAL, MORALE'
	if (ruleMorale < 0) { // negative effects 1st
		moraleModifier += ruleMorale;
	}
	int homeBaseID = Veh[vehID].homeBaseID;
	if (homeBaseID >= 0) { // home base countering negative effects
		if (has_fac_built(FAC_CHILDREN_CRECHE, homeBaseID) && moraleModifier < 0) {
			moraleModifier /= 2;
		}
		if (has_fac_built(FAC_BROOD_PIT, homeBaseID) && protoID < MaxVehProtoFactionNum
			&& (offenseRating < 0 || protoID == BSC_SPORE_LAUNCHER) && moraleModifier < 0) {
			moraleModifier /= 2; // never reached due to above 'Basic Psi Veh' checks
		}
	}
	if (ruleMorale > 0) {
		moraleModifier += ruleMorale;
	}
	BOOL moraleFlag = Players[factionID].ruleFlags & RFLAG_MORALE;
	if (moraleFlag && moraleModifier < 0) {
		moraleModifier = 0;
	}
	if (checkDroneRiot && homeBaseID >= 0 && Base[homeBaseID].state & BSTATE_DRONE_RIOTS_ACTIVE
		&& !moraleFlag) {
		// bug fix: removed premature range bounding negating negative morale effects
		moraleModifier--;
	}
	return range(Veh[vehID].morale + moraleModifier, 0, 6);
}

/*
Purpose: Calculate offensive of prototype. Optional param of unit defending against (-1 to ignore)
		 as well as whether artillery or missile combat is being utilized.
Original Offset: 005C1150
Return Value: Prototype's armor
Status: Complete
*/
uint32_t __cdecl offense_proto(uint32_t protoID, int vehIDDef, BOOL isBombardment) {
	uint32_t weaponID = VehPrototype[protoID].weaponID;
	if (Weapon[weaponID].mode == WPN_MODE_INFOWAR && vehIDDef >= 0
		&& VehPrototype[Veh[vehIDDef].protoID].plan == PLAN_INFO_WARFARE) {
		return 16; // probe attacking another probe
	}
	// Bug fix: Veh[].protoID with vehIDDef -1 could cause arbitrary memory read (Reactor struct)
	// due to lack of bounds checking when comparing vehIDDef protoID to Spore Launcher
	if ((isBombardment || (Weapon[weaponID].offenseRating >= 0
		&& (vehIDDef < 0 || Armor[VehPrototype[Veh[vehIDDef].protoID].armorID].defenseRating >= 0)))
		&& (vehIDDef < 0 || Veh[vehIDDef].protoID != BSC_SPORE_LAUNCHER)
		&& protoID != BSC_SPORE_LAUNCHER) {
		int offRating = Weapon[VehPrototype[protoID].weaponID].offenseRating;
		if (offRating < 0) {
			offRating = -offRating;
		}
		if (Chassis[VehPrototype[protoID].chassisID].missile && offRating < 99) {
			offRating = (offRating * 3) / 2;
		}
		return (vehIDDef < 0) ? offRating : offRating * 8; // conventional
	}
	return (vehIDDef < 0) ? Rules->PsiCombatRatioAtk[TRIAD_LAND] :  // PSI
		Rules->PsiCombatRatioAtk[get_triad(vehIDDef)] * 8;
}

/*
Purpose: Calculate defense of prototype. Optional param if unit is being attacked (-1 to ignore)
		 as well as whether artillery or missile combat is being utilized.
Original Offset: 005C1290
Return Value: Prototype's armor
Status: Complete
*/
uint32_t __cdecl armor_proto(uint32_t protoID, int vehIDAtk, BOOL isBombardment) {
	if (Weapon[VehPrototype[protoID].weaponID].mode == WPN_MODE_INFOWAR && vehIDAtk >= 0
		&& VehPrototype[Veh[vehIDAtk].protoID].plan == PLAN_INFO_WARFARE) {
		return 16; // probe defending against another probe
	}
	// Bug fix: Veh[].protoID with vehIDAtk -1 could cause arbitrary memory read (Reactor struct)
	// due to lack of bounds checking when comparing vehIDAtk protoID to Spore Launcher
	if (isBombardment && (vehIDAtk < 0 || Veh[vehIDAtk].protoID != BSC_SPORE_LAUNCHER)
		&& protoID != BSC_SPORE_LAUNCHER
		|| (Armor[VehPrototype[protoID].armorID].defenseRating >= 0
			&& (vehIDAtk < 0
				|| Weapon[VehPrototype[Veh[vehIDAtk].protoID].weaponID].offenseRating >= 0))) {
		uint32_t defRating = range(Armor[VehPrototype[protoID].armorID].defenseRating, 1, 9999);
		return (vehIDAtk < 0) ? defRating : defRating * 8; // conventional
	}
	return (vehIDAtk < 0) ? Rules->PsiCombatRatioDef[TRIAD_LAND] : // PSI
		Rules->PsiCombatRatioDef[get_proto_triad(protoID)] * 8;
}

/*
Purpose: Calculate speed of prototype on roads.
Original Offset: 005C13B0
Return Value: Prototype's speed
Status: Complete
*/
uint32_t __cdecl speed_proto(uint32_t protoID) {
	if (protoID == BSC_FUNGAL_TOWER) {
		return 0; // cannot move
	}
	int chasID = VehPrototype[protoID].chassisID;
	int speedVal = Chassis[chasID].speed;
	uint8_t triad = Chassis[chasID].triad;
	uint32_t weapID = VehPrototype[protoID].weaponID;
	if (triad == TRIAD_AIR) {
		speedVal += VehPrototype[protoID].reactorID * 2;
	}
	if (Weapon[weapID].mode == WPN_MODE_TRANSPORT) {
		speedVal--;
	}
	if (has_abil(protoID, ABL_SLOW)) {
		speedVal--;
	}
	if (has_abil(protoID, ABL_ANTIGRAV_STRUTS)) {
		speedVal += (triad == TRIAD_AIR) ? VehPrototype[protoID].reactorID * 2 : 1;
	}
	if (triad == TRIAD_AIR) {
		if (has_abil(protoID, ABL_FUEL_NANOCELLS)) {
			speedVal += 2;
		}
		if (has_project(SP_CLOUDBASE_ACADEMY, protoID / MaxVehProtoFactionNum)) {
			speedVal += 2; // bonus from Aerospace Complex
		}
		if (has_abil(protoID, ABL_AIR_SUPERIORITY)) {
			// generally -20% to -25%, in some cases higher due to lossy division rounding
			speedVal = (speedVal * 4) / 5;
		}
		if (Weapon[weapID].mode == WPN_MODE_TRANSPORT) {
			speedVal /= 2; // 2nd penalty for air transports: -50%
		}
	}
	return range(speedVal, 1, 99) * Rules->MoveRateRoads;
}

/*
Purpose: Calculate speed of unit on roads taking into consideration prototype speed, elite morale,
		 if unit is damaged and other factors. The skipMorale parameter seems to only be set to true
		 for certain combat calculations in battle_fight().
Original Offset: 005C1540
Return Value: speed
Status: Complete
*/
uint32_t __cdecl speed(uint32_t vehID, BOOL skipMorale) {
	uint32_t protoID = Veh[vehID].protoID;
	if (protoID == BSC_FUNGAL_TOWER) { // moved this check to top vs bottom, same logic
		return 0; // cannot move
	}
	uint32_t speedVal = speed_proto(protoID);
	uint8_t triad = get_proto_triad(protoID);
	if (triad == TRIAD_SEA && has_project(SP_MARITIME_CONTROL_CENTER, Veh[vehID].factionID)) {
		speedVal += Rules->MoveRateRoads * 2;
	}
	if (!skipMorale && morale_veh(vehID, true, 0) == MORALE_ELITE
		&& (protoID >= MaxVehProtoFactionNum
			|| Weapon[VehPrototype[protoID].weaponID].offenseRating >= 0)) {
		speedVal += Rules->MoveRateRoads;
	}
	if (Veh[vehID].dmgIncurred && triad != TRIAD_AIR) {
		// optimized code without changes to logic
		uint32_t moves = speedVal / Rules->MoveRateRoads, reactorFac;
		if (VehPrototype[protoID].plan == PLAN_ALIEN_ARTIFACT) {
			speedVal = reactorFac = 1;
		}
		else {
			reactorFac = range(VehPrototype[protoID].reactorID, 1, 100) * 10;
			speedVal = range(reactorFac, 1, 99);
		}
		speedVal =
			(moves * range(reactorFac - Veh[vehID].dmgIncurred, 0, 9999) + speedVal - 1) / speedVal;
		speedVal = range(speedVal, (triad == TRIAD_SEA) ? 2 : 1, 999) * Rules->MoveRateRoads;
	}
	return speedVal;
}

/*
Purpose: Calculate cargo capacity of unit. Looks like Spore Launchers were considered to have cargo
		 capacity at one time.
Original Offset: 005C1760
Return Value: Cargo value
Status: Complete
*/
uint32_t __cdecl veh_cargo(uint32_t vehID) {
	uint32_t protoID = Veh[vehID].protoID;
	uint32_t cargo = VehPrototype[protoID].carryCapacity;
	return (cargo && protoID < MaxVehProtoFactionNum
		&& (Weapon[VehPrototype[protoID].weaponID].offenseRating < 0 // Isle of the Deep
			|| protoID == BSC_SPORE_LAUNCHER)) ? Veh[vehID].morale + 1 : cargo;
}

/*
Purpose: Determine extra percent cost for building prototype. Includes a check if the faction has
		 the free prototype flag set or if the player is using one of the easier difficulties.
Original Offset: 005C17D0
Return Value: % extra prototype cost
Status: Complete
*/
uint32_t __cdecl prototype_factor(uint32_t protoID) {
	uint32_t factionID = protoID / MaxVehProtoFactionNum;
	if (Players[factionID].ruleFlags & RFLAG_FREEPROTO
		|| PlayersData[factionID].diffLevel <= DLVL_SPECIALIST) {
		return 0;
	}
	uint8_t triad = get_proto_triad(protoID);
	switch (triad) {
	case TRIAD_SEA:
		return Rules->ExtraPctCostProtoSea;
	case TRIAD_AIR:
		return Rules->ExtraPctCostProtoAir;
	case TRIAD_LAND:
	default:
		return Rules->ExtraPctCostProtoLand;
	}
}

/*
Purpose: Calculate the specified prototype's overall cost to build. Optional output parameter
		 whether there is an associated 1st time prototype cost (true) or just the base (false).
Original Offset: 005C1850
Return Value: Mineral cost
Status: Complete
*/
uint32_t __cdecl veh_cost(uint32_t protoID, int baseID, BOOL *hasProtoCost) {
	uint32_t cost = VehPrototype[protoID].cost;
	if (baseID >= 0 && protoID < MaxVehProtoFactionNum // bug fix: added baseID bounds check
		&& (Weapon[VehPrototype[protoID].weaponID].offenseRating < 0
		|| protoID == BSC_SPORE_LAUNCHER) && has_fac_built(FAC_BROOD_PIT, baseID)) {
		cost = (cost * 3) / 4; // Decrease the cost of alien units by 25%
	}
	if (VehPrototype[protoID].plan == PLAN_COLONIZATION && baseID >= 0) {
		cost = range(cost, 1, 999);
	}
	uint32_t protoCost = 0;
	if (protoID >= MaxVehProtoFactionNum && !(VehPrototype[protoID].flags & PROTO_TYPED_COMPLETE)) {
		protoCost = (baseID >= 0 && has_fac_built(FAC_SKUNKWORKS, baseID)) ? 0 // moved checks up
			: (prototype_factor(protoID) * base_cost(protoID) + 50) / 100;
		cost += protoCost;
	}
	if (hasProtoCost) {
		*hasProtoCost = protoCost != 0;
	}
	return cost;
}

/*
Purpose: Check if land Veh inside Air transport can disembark. Transport must be in base or airbase.
Original Offset: 005C1C40
Return Value: True if Veh is in "jail" and cannot leave, false if Veh can disembark.
Status: Complete
*/
BOOL __cdecl veh_jail(int vehID) {
	if (get_triad(vehID) == TRIAD_LAND && Veh[vehID].orders == ORDER_SENTRY_BOARD
		&& Veh[vehID].waypoint_xCoord[0] >= 0
		&& get_triad(Veh[vehID].waypoint_xCoord[0]) == TRIAD_AIR
		&& (!(bit_at(Veh[vehID].xCoord, Veh[vehID].yCoord) & BIT_BASE_IN_TILE)
			|| owner_at(Veh[vehID].xCoord, Veh[vehID].yCoord) >= 8
			|| owner_at(Veh[vehID].xCoord, Veh[vehID].yCoord) <= 0)
		&& !(bit_at(Veh[vehID].xCoord, Veh[vehID].yCoord) & BIT_AIRBASE)) {
		return true;
	}
	return false;
}

/*
Purpose: Sets all moves as expended.
Original Offset: 005C1D20
Return Value: n/a
Status: Complete
*/
void __cdecl veh_skip(int vehID) {
	// TODO Bug: Due to size of movesExpended, speeds over 255 will be incorrect. The speed()
	//           function can return a value from 1-999. Eventually increase size to 16 bits.
	Veh[vehID].movesExpended = (uint8_t)speed(vehID, false);
}

/*
Purpose: Fake start of Veh init for fixed VehID 2048.
Original Offset: 005C1D50
Return Value: Fixed vehID (2048)
Status: Complete
*/
int __cdecl veh_fake(int protoID, int factionID) {
	veh_clear(2048, protoID, factionID);
	return 2048;
}

/*
Purpose: Activate unit and clear current action.
Original Offset: 005C1D70
Return Value: vehID (doesn't look to be used on return)
Status: Complete (TODO: Identify VSTATE flags)
*/
int __cdecl veh_wake(int vehID) {
	uint8_t orders = Veh[vehID].orders;
	uint32_t state = Veh[vehID].state;
	if (orders >= ORDER_FARM && orders < ORDER_MOVE_TO && !(state & VSTATE_CRAWLING)) {
		// TODO bug fix: Issue with movesExpended size / speed return, see veh_skip()
		Veh[vehID].movesExpended = (uint8_t)(speed(vehID, false) - Rules->MoveRateRoads);
		int terraTurns = Veh[vehID].terraformingTurns;
		if (terraTurns) {
			terraTurns -= contribution(vehID, orders - 4);
			if (terraTurns < 0) {
				terraTurns = 0;
			}
			Veh[vehID].terraformingTurns = (uint8_t)terraTurns;
		}
	}
	if (state & VSTATE_UNK_200 && Veh[vehID].orderAutoType == ORDERA_ON_ALERT
		&& !(state & VSTATE_UNK_4)) {
		Veh[vehID].movesExpended = 0;
	}
	Veh[vehID].orders = ORDER_NONE;
	Veh[vehID].state &= ~(VSTATE_UNK_200 | VSTATE_EXPLORE | VSTATE_UNK_1000000 | VSTATE_UNK_2000000
		| VSTATE_UNK_8000000);
	return vehID;
}

/*
Purpose: Get triad for specified prototype.
Original Offset: n/a
Return Value: triad (see veh_triad)
Status: Complete
*/
uint8_t __cdecl get_proto_triad(uint32_t protoID) {
	return Chassis[VehPrototype[protoID].chassisID].triad;
}

/*
Purpose: Get triad for specified veh.
Original Offset: n/a
Return Value: triad (see veh_triad)
Status: Complete
*/
uint8_t __cdecl get_triad(uint32_t vehID) {
	return get_proto_triad(Veh[vehID].protoID);
}

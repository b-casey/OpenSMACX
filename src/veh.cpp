 /*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2019 Brendan Casey
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
#include "veh.h"
#include "alpha.h"
#include "base.h"
#include "faction.h"
#include "general.h" // range
#include "log.h"
#include "map.h"
#include "strings.h"
#include "technology.h"
#include "temp.h"

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

/*
Purpose: Calculate Former rate to perform terrain enhancements.
Original Offset: 004C9A50
Return Value: Terraforming speed
Status: Complete
*/
DWORD __cdecl contribution(int vehID, DWORD terraformID) {
	DWORD rate = has_abil(Veh[vehID].protoID, ABL_SUPER_TERRAFORMER) ? 4 : 2;
	if (terraformID == (ORDER_REMOVE_FUNGUS - 4) || terraformID == (ORDER_PLANT_FUNGUS - 4)) {
		if (has_project(SP_XENOEMPATYH_DOME, Veh[vehID].factionID)) {
			rate *= 2; // Doubled
		}
	}
	else if (has_project(SP_WEATHER_PARADIGM, Veh[vehID].factionID)) {
		rate = (rate * 3) / 2; // +50%
	}
	return rate;
}

/*
Purpose: Calculate maximum range for Veh drops (air drops, Drop Pods).
Original Offset: 00500320
Return Value: Max range
Status: Complete
*/
DWORD __cdecl drop_range(int factionID) {
	if (!has_tech(Rules->TechOrbInsertSansSpcElev, factionID) // default 'Graviton Theory'
		&& !has_project(SP_SPACE_ELEVATOR, factionID)) {
		return Rules->MaxAirdropSansOrbInsert;
	}
	if (*MapHorizontalBounds > *MapVerticalBounds) {
		return *MapHorizontalBounds;
	}
	return *MapVerticalBounds;
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
			factor += factor / 2; // SMACX only, never used: likely +50% Fungal Tower defense bonus
		}
	}
	return factor;
}

/*
Purpose: 
Original Offset: 00532A90
Return Value: n/a
Status: WIP
*/
void __cdecl rebuild_vehicle_bits() {
	if (MapVerticalBounds <= 0) {
		return;
	}
	for (DWORD y = 0, z = 0; y < *MapVerticalBounds; y++) {
		DWORD val = (y & 1);
		if (val < *MapHorizontalBounds) {
			for (DWORD x = 0; x < *MapHorizontalBounds; x += 2, z++) {
				Map[z]->bit &= ~BIT_VEH_IN_TILE;
				for (int vehID = 0; vehID < *VehCurrentCount; vehID++) {
					if (Veh[vehID].xCoord == (int)x && Veh[vehID].yCoord == (int)y) {
						Map[z]->bit |= BIT_VEH_IN_TILE;
					}
					if (Map[z]->bit & BIT_BASE_IN_TILE) {
						//owner_set
						Map[z]->val2 &= 0xF0;
						Map[z]->val2 |= Veh[vehID].factionID & 0xF;
					}
				}
			}
		}
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
DWORD __cdecl veh_moves(int vehID) {
	return range(speed(vehID, FALSE) - Veh[vehID].movesExpended, 0, 999);
}

/*
Purpose: Get Veh power value from prototype.
Original Offset: 005799A0
Return Value: power
Status: Complete
*/
DWORD __cdecl proto_power(int vehID) {
	int protoID = Veh[vehID].protoID;
	if (VehPrototype[protoID].plan == PLAN_ALIEN_ARTIFACT) {
		return 1;
	}
	return range(VehPrototype[protoID].reactorID, 1, 100) * 10;
}

/*
Purpose: Calculate armor value from armorID
Original Offset: 0057D270
Return Value: Armor value / factor
Status: Complete
*/
int __cdecl arm_strat(int armorID, int factionID) {
	if (!SMACX_Enabled && armorID > ARM_PSI_DEFENSE) {
		return 1;
	}
	int defenseRating = Armor[armorID].defenseRating;
	if (defenseRating < 0) {
		return psi_factor((Rules->PsiCombatRatioDef[TRIAD_LAND] * PlayersData[factionID].unk_59)
			/ Rules->PsiCombatRatioAtk[TRIAD_LAND], factionID, FALSE, FALSE);
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
	if (!SMACX_Enabled && (weaponID == WPN_RESONANCE_LASER || weaponID == WPN_RESONANCE_BOLT
		|| weaponID == WPN_STRING_DISRUPTOR))
		return 1;
	int offenseRating = Weapon[weaponID].offenseRating;
	if (offenseRating < 0) {
		return psi_factor((Rules->PsiCombatRatioAtk[TRIAD_LAND] * PlayersData[factionID].unk_60)
			/ Rules->PsiCombatRatioDef[TRIAD_LAND], factionID, TRUE, FALSE);
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
DWORD __cdecl transport_val(DWORD chassisID, int ability, DWORD reactorID) {
	DWORD transport = Chassis[chassisID].cargo;
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
	if (can_arty(protoID, TRUE)) {
		output.insert(output.begin(), '(');
		output += ")";
	}
	if (has_abil(protoID, ABL_AIR_SUPERIORITY)) {
		output.insert(output.begin(), '<');
		output += ">";
	}
	DWORD weaponID = VehPrototype[protoID].weaponID;
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
	DWORD armorID = VehPrototype[protoID].armorID;
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
	BYTE reactor = VehPrototype[protoID].reactorID;
	if (reactor > 1) {
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
		output = StringTable->get((int)*((LPSTR *)Label->stringsPtr + 0xC4)); // 'Psi'
	}
	output += "-";
	output += (Armor[VehPrototype[protoID].armorID].defenseRating) >= 0 ?
		say_defense(protoID) : StringTable->get((int)*((LPSTR *)Label->stringsPtr + 0xC4)); // 'Psi'
	output += "-";
	output += std::to_string(speed_proto(protoID) / Rules->MoveRateRoads);
	DWORD triad = Chassis[VehPrototype[protoID].chassisID].triad;
	if (triad == TRIAD_SEA) {
		output += " ";
		output += StringTable->get((int)*((LPSTR *)Label->stringsPtr + 0xA3)); // 'Sea';
	}
	else if (triad == TRIAD_AIR) {
		output += " ";
		output += StringTable->get((int)*((LPSTR *)Label->stringsPtr + 0xA2)); // 'Air';
	}
	DWORD reactor = VehPrototype[protoID].reactorID;
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
	BYTE plan = VehPrototype[protoID].plan;
	BYTE chas = VehPrototype[protoID].chassisID;
	BYTE triad = Chassis[chas].triad;
	BYTE mode = Weapon[VehPrototype[protoID].weaponID].mode;
	int offRating = Weapon[VehPrototype[protoID].weaponID].offenseRating;
	int defRating = Armor[VehPrototype[protoID].armorID].defenseRating;
	if (plan == PLAN_RECONNAISANCE && triad == TRIAD_LAND && offRating == 1 && defRating == 1
		&& !VehPrototype[protoID].abilityFlags) {
		output = StringTable->get((int)PlansFullName[3]); // 'Explore/Defense'
		output += ", ";
	}
	else if (mode < 3) { // Projectile, energy, missile
		LPSTR temp = (plan != PLAN_DEFENSIVE || (offRating >= 0 && offRating <= defRating))
			? PlansShortName[plan] : *((LPSTR *)Label->stringsPtr + 0x138); // 'Combat'
		output = StringTable->get((int)temp);
		output += ", ";
	}
	if (offRating < 0 || mode < 3) {
		output += (offRating < 0) ? StringTable->get((int)*((LPSTR *)Label->stringsPtr + 0xC4)) :
			say_offense(protoID);
		output += customSpacer ? customSpacer : "/";
		output += (defRating < 0) ? StringTable->get((int)*((LPSTR *)Label->stringsPtr + 0xC4)) :
			say_defense(protoID);
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
		output += (defRating < 0) ? StringTable->get((int)*((LPSTR *)Label->stringsPtr + 0xC4)) :
			say_defense(protoID);
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
		output += StringTable->get((int)*((LPSTR *)Label->stringsPtr + 0xA3)); // 'Sea';
	}
	else if (triad == TRIAD_AIR) {
		output += " ";
		output += StringTable->get((int)*((LPSTR *)Label->stringsPtr + 0xA2)); // 'Air';
	}
	DWORD reactor = VehPrototype[protoID].reactorID;
	if (reactor > 1) {
		output += " (*";
		output += std::to_string(reactor);
		output += ")";
	}
	// assumes at least 1032 char buffer (stringTemp), eventually remove
	strcat_s(stat, 1032, output.c_str());
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
DWORD __cdecl veh_health(int vehID) {
	int protoID = Veh[vehID].protoID;
	int health = VehPrototype[protoID].plan != PLAN_ALIEN_ARTIFACT 
		? range(VehPrototype[protoID].reactorID, 1, 100) * 10: 1;
	return range(health - Veh[vehID].dmgIncurred, 0, 9999);
}

/*
Purpose: Calculates cost of prototype based on various factors. Optimized logic flow from original
		 without any difference to final calculation.
Original Offset: 005A5A60
Return Value: Cost of prototype
Status: Complete
*/
DWORD __cdecl proto_cost(DWORD chassisID, DWORD weaponID, DWORD armorID, DWORD ability,
	DWORD reactorID) {
	BYTE weapCost = Weapon[weaponID].cost;
	// PB check: moved to start vs after 1st triad checks in original > no difference in logic
	if (Chassis[chassisID].missile && Weapon[weaponID].offenseRating >= 99) {
		return weapCost;
	}
	BYTE triad = Chassis[chassisID].triad;
	DWORD armorCost = Armor[armorID].cost;
	DWORD speedCost = Chassis[chassisID].cost;
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
				if (Ability[i].flags & COST_INC_LAND_UNIT && triad == TRIAD_LAND) {
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
	DWORD combatMod = armorCost / 2 + 1;
	if (combatMod < weapCost) { // which ever is greater
		combatMod = weapCost;
	}
	int protoMod;
	// shifted this check to top vs at end > no difference in logic
	if (combatMod == 1 && armorCost == 1 && speedCost == 1 && reactorID == 1) {
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
DWORD __cdecl base_cost(int protoID) {
	return proto_cost(VehPrototype[protoID].chassisID, VehPrototype[protoID].weaponID,
		VehPrototype[protoID].armorID, 0, VehPrototype[protoID].reactorID);
}

/*
Purpose: Create a new prototype. Sets initial values for everything except vehName and preqTech.
Original Offset: 005A5D40
Return Value: n/a
Status: Complete
*/
void __cdecl make_proto(int protoID, DWORD chassisID, DWORD weaponID, DWORD armorID, DWORD ability,
	DWORD reactorID) {
	int unkLocal1 = 0;
	if (protoID >= MaxVehProtoFactionNum) {
		BOOL cond1 = FALSE, cond2 = FALSE, cond3 = FALSE;
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
					|| (protoIDLoop > MaxVehProtoFactionNum && (flagsCheck & 4))) {
					DWORD loopWeaponID = VehPrototype[protoIDLoop].weaponID;
					if (loopWeaponID == weaponID) {
						cond1 = TRUE;
					}
					DWORD loopArmorID = VehPrototype[protoIDLoop].armorID;
					if (loopArmorID == armorID) {
						cond2 = TRUE;
					}
					DWORD loopChassisID = VehPrototype[protoIDLoop].chassisID;
					if (loopChassisID == chassisID) {
						cond3 = TRUE;
					}
					int offRating = Weapon[weaponID].offenseRating;
					if (offRating > 0 && Weapon[loopWeaponID].offenseRating >= offRating) {
						cond1 = TRUE;
					}
					int defRating = Armor[armorID].defenseRating;
					if (defRating > 0 && Armor[loopArmorID].defenseRating >= defRating) {
						cond2 = TRUE;
					}
					if (Chassis[chassisID].triad == Chassis[loopChassisID].triad
						&& Chassis[loopChassisID].speed >= Chassis[chassisID].speed) {
						cond3 = TRUE;
					}
				}
			}
		}
		if (cond1 && cond2 && cond3) {
			unkLocal1 = (protoIDLoop >= MaxVehProtoFactionNum
				&& VehPrototype[protoIDLoop].flags & 0x10) ? 3 : 1;
		}
	}
	VehPrototype[protoID].chassisID = (BYTE)chassisID;
	VehPrototype[protoID].weaponID = (BYTE)weaponID;
	VehPrototype[protoID].armorID = (BYTE)armorID;
	VehPrototype[protoID].abilityFlags = ability;
	VehPrototype[protoID].reactorID = (BYTE)reactorID;
	VehPrototype[protoID].cost = (BYTE)proto_cost(chassisID, weaponID, armorID, ability,
		reactorID);
	VehPrototype[protoID].carryCapacity = (Weapon[weaponID].mode == WPN_MODE_TRANSPORT) ?
		(BYTE)transport_val(chassisID, ability, reactorID) : 0;
	// set plan & unk1
	if (Chassis[chassisID].missile) {
		if (Weapon[weaponID].offenseRating < 99) { // non-PB missiles
			if (weaponID == WPN_TECTONIC_PAYLOAD) {
				VehPrototype[protoID].plan = PLAN_TECTONIC_MISSILE;
				VehPrototype[protoID].unk1 = 22;
			}
			else if (weaponID == WPN_FUNGAL_PAYLOAD) {
				VehPrototype[protoID].plan = PLAN_FUNGAL_MISSILE;
				VehPrototype[protoID].unk1 = 23;
			}
			else { // Conventional Payload
				VehPrototype[protoID].plan = PLAN_OFFENSIVE;
				VehPrototype[protoID].unk1 = 13;
			}
		}
		else {
			VehPrototype[protoID].plan = PLAN_PLANET_BUSTER;
			VehPrototype[protoID].unk1 = 14;
		}
	}
	else if (Weapon[weaponID].mode >= WPN_MODE_TRANSPORT) { // Non-combat
		VehPrototype[protoID].plan = Weapon[weaponID].mode;
		VehPrototype[protoID].unk1 = Weapon[weaponID].mode + 0x20;
	}
	else if (Chassis[chassisID].triad == TRIAD_SEA) { // combat sea
		VehPrototype[protoID].plan = PLAN_NAVAL_SUPERIORITY;
		VehPrototype[protoID].unk1 = 6; // same value as plan
	}
	else if (Chassis[chassisID].triad == TRIAD_AIR) { // combat air
		if (has_abil(protoID, ABL_AIR_SUPERIORITY)) {
			VehPrototype[protoID].plan = PLAN_AIR_SUPERIORITY;
			VehPrototype[protoID].unk1 = 9;
		}
		else {
			VehPrototype[protoID].plan = PLAN_OFFENSIVE;
			VehPrototype[protoID].unk1 = 8;
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
		VehPrototype[protoID].unk1 = 3;
		if (Armor[armorID].defenseRating > Weapon[weaponID].offenseRating) {
			VehPrototype[protoID].unk1 = 2;
		}
		if (Weapon[weaponID].offenseRating > Armor[armorID].defenseRating * 2) {
			VehPrototype[protoID].unk1 = 1;
		}
		// changed from '>= 2' since '>= 3' will always overwrite it
		if (Chassis[chassisID].speed == 2) {
			VehPrototype[protoID].unk1 = 4;
		}
		// added else, no point in checking if speed==2
		else if (Chassis[chassisID].speed >= 3) {
			VehPrototype[protoID].unk1 = 5;
		}
		if (Weapon[weaponID].offenseRating == 1 && Armor[armorID].defenseRating == 1) {
			if (Chassis[chassisID].speed > 1) {
				VehPrototype[protoID].plan = PLAN_RECONNAISANCE;
			}
			if (has_abil(protoID, ABL_POLICE_2X)) {
				VehPrototype[protoID].plan = PLAN_DEFENSIVE;
			}
			VehPrototype[protoID].unk1 = 10;
		}
	}
	// set final values
	VehPrototype[protoID].unk2 = 0;
	VehPrototype[protoID].unk3 = (protoID >= MaxVehProtoFactionNum) ?
		1 << (protoID / MaxVehProtoFactionNum) : -1;
	VehPrototype[protoID].flags = unkLocal1 ? ((unkLocal1 & 2) ? 0x115 : 0x105) : PROTO_ACTIVE;
	VehPrototype[protoID].iconOffset = -1;
}

/*
Purpose:
Original Offset: 005BFE90
Return Value: vehID or -1 if nothing found
Status: WIP
*/
int __cdecl veh_at(int xCoord, int yCoord) {
	if (yCoord >= 0 && yCoord < (int)*MapVerticalBounds && xCoord >= 0 
		&& xCoord < (int)*MapHorizontalBounds && !(bit_at(xCoord, yCoord) & BIT_VEH_IN_TILE)) {
		return -1;
	}
	int vehID = 0;
	while (vehID < *VehCurrentCount) {
		if (Veh[vehID].xCoord == xCoord && Veh[vehID].yCoord == yCoord) {
			return veh_top(vehID);
		}
	}
	if (yCoord < 0 || yCoord >= (int)*MapVerticalBounds || xCoord < 0
		|| xCoord >= (int)*MapHorizontalBounds) {
		return -1;
	}
	if (!*VehBitError) {
		log_say("Vehicle Bit Error  (x, y)", xCoord, yCoord, 0);
	}
	if (*GameRules & TGL_SCENARIO_EDITOR || *GameRules & TGL_DEBUG_MODE || *MultiplayerToggle) {
		if (*VehBitError) {
			return -1;
		}
		*VehBitError = TRUE;
	}
	rebuild_vehicle_bits();
	return -1;
}

/*
Purpose: Check whether prototype has a specific ability
Original Offset: 005BF1F0
Return Value: Does prototype have ability? TRUE/FALSE
Status: Complete
*/
BOOL __cdecl has_abil(int protoID, int abilityID) {
	if (VehPrototype[protoID].abilityFlags & abilityID) {
		return TRUE;
	}
	DWORD factionID = protoID / MaxVehProtoFactionNum;
	if (!factionID) {
		return FALSE; // skip basic prototypes from #UNITS
	}
	if (Players[factionID].ruleFlags & FLAG_ALIEN && abilityID == ABL_DEEP_RADAR) {
		return TRUE; // Caretakers + Usurpers > "Deep Radar" ability for all units
	}
	for (int i = 0; i < Players[factionID].factionBonusCount; i++) {
		if (Players[factionID].factionBonusID[i] == RULE_FREEABIL) {
			int abilBonusID = Players[factionID].factionBonusVal1[i];
			if (has_tech(Ability[abilBonusID].preqTech, factionID) &&
				(abilityID & (1 << abilBonusID))) {
				// Pirates > "Marine Detachment" ability for combat sea units with Adaptive Doctrine
				return TRUE;
			}
		}
	}
	if (VehPrototype[protoID].weaponID == WPN_PROBE_TEAM && abilityID == ABL_ALGO_ENHANCEMENT
		&& has_project(SP_NETHACK_TERMINUS, factionID)) {
		return TRUE; // All Probe Teams act as though they have the "Algorithmic Enhancement"
	}
	return FALSE;
}

/*
Purpose: Temporarily remove Veh from current square and stack in preparation for another action such 
         as interacting with stack, moving or killing.
Original Offset: 005BFFA0
Return Value: vehID
Status: WIP - further tests
*/
int __cdecl veh_lift(int vehID) {
	BOOL prevStackExists = FALSE;
	short prevVehID = Veh[vehID].prevVehIDStack, nextVehID = Veh[vehID].nextVehIDStack;
	if (prevVehID >= 0) {
		prevStackExists = TRUE;
		Veh[prevVehID].nextVehIDStack = nextVehID;
	}
	int xCoord = Veh[vehID].xCoord, yCoord = Veh[vehID].yCoord;
	if (nextVehID >= 0) {
		Veh[nextVehID].prevVehIDStack = prevVehID;
	}
	else if(!prevStackExists && yCoord >= 0 && yCoord < (int)*MapVerticalBounds && xCoord >= 0
		&& xCoord < (int)*MapHorizontalBounds) {
		bit_set(xCoord, yCoord, 2, 0);
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
Purpose:
Original Offset: 005C0080
Return Value: vehID (param1), doesn't seem to be used
Status: WIP
*/
int __cdecl veh_drop(int vehID, int xCoord, int yCoord) {
	int vehIDAt = veh_at(xCoord, yCoord);
	Veh[vehID].nextVehIDStack = (short)vehIDAt;
	Veh[vehID].prevVehIDStack = -1;
	Veh[vehID].xCoord = (short)xCoord;
	Veh[vehID].yCoord = (short)yCoord;
	*VehDropLiftVehID = -1;
	if (vehIDAt < 0) {
		if (yCoord < 0) {
			return vehID;
		}
		if (yCoord < (int)*MapVerticalBounds && xCoord >= 0 && xCoord < (int)*MapHorizontalBounds
			&& !(bit_at(xCoord, yCoord) & BIT_BASE_IN_TILE)) {
			owner_set(xCoord, yCoord, Veh[vehID].factionID);
		}
	}
	else {
		Veh[vehIDAt].prevVehIDStack = (short)vehID;
	}
	if (yCoord >= 0 && yCoord < (int)*MapVerticalBounds && xCoord >= 0
		&& xCoord < (int)*MapHorizontalBounds) {
		DWORD flags = (Veh[vehID].factionID
			&& Chassis[VehPrototype[Veh[vehID].protoID].chassisID].triad == TRIAD_AIR)
			? BIT_SUPPLY_REMOVE | BIT_VEH_IN_TILE : BIT_VEH_IN_TILE;
		bit_set(xCoord, yCoord, flags, TRUE);
	}
	return vehID;
}


/*
Purpose: Set Veh status to sentry/board.
Original Offset: 005C01A0
Return Value: n/a
Status: Complete
*/
void __cdecl sleep(int vehID) {
	Veh[vehID].orders = ORDER_SENTRY_BOARD;
	Veh[vehID].waypoint_xCoord[0] = -1;
	Veh[vehID].waypoint_yCoord[0] = 0;
}

/*
Purpose: Move vehID down in stack. (?)
Original Offset: 005C01D0
Return Value: n/a
Status: WIP
*/
void __cdecl veh_demote(int vehID) {
	if (vehID >= 0) {
		short nextVehID = Veh[vehID].nextVehIDStack;
		while (nextVehID >= 0) {
			nextVehID = Veh[nextVehID].nextVehIDStack;
		}
		if (nextVehID != vehID) {
			veh_lift(vehID);
			Veh[nextVehID].nextVehIDStack = (short)vehID;
			Veh[vehID].prevVehIDStack = nextVehID;
			Veh[vehID].nextVehIDStack = -1;
			Veh[vehID].xCoord = Veh[nextVehID].xCoord;
			Veh[vehID].yCoord = Veh[nextVehID].yCoord;
		}
	}
}

/*
Purpose:
Original Offset: 005C0260
Return Value: n/a
Status: WIP
*/
void __cdecl veh_promote(int vehID) {
	if (vehID >= 0) {
		short prevVehID = Veh[vehID].prevVehIDStack;
		while (prevVehID >= 0) {
			prevVehID = Veh[prevVehID].prevVehIDStack;
		}
		if (prevVehID != vehID) {
			veh_lift(vehID);
			veh_drop(vehID, Veh[prevVehID].xCoord, Veh[prevVehID].yCoord);
		}
	}
}

/*
Purpose: Clear specified Veh.
Original Offset: 005C02D0
Return Value: n/a
Status: Complete
*/
void __cdecl veh_clear(int vehID, int protoID, int factionID) {
	Veh[vehID].xCoord = -4;
	Veh[vehID].yCoord = -4;
	Veh[vehID].yearEndLurking = 0;
	Veh[vehID].unknown1 = 0;
	Veh[vehID].flags = 0;
	Veh[vehID].factionID = (BYTE)factionID;
	Veh[vehID].protoID = (short)protoID;
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
	Veh[vehID].unknown6 = 0;
	Veh[vehID].unknown7 = 0;
	Veh[vehID].visibleToFaction = 0;
	Veh[vehID].homeBaseID = -1;
	Veh[vehID].morale = (BYTE)(Players[factionID].ruleMorale + 1);
	Veh[vehID].unknown5 = 2;
	Veh[vehID].unknown8 = 0;
	Veh[vehID].unknown9 = 0;
}

/*
Purpose: Check if unit can perform artillery combat. The 2nd parameter determines how sea units
         are treated.
Original Offset: 005C0DB0
Return Value: Has artillery ability? True/False
Status: Complete
*/
BOOL __cdecl can_arty(int protoID, BOOL seaTriadRetn) {
	if ((Weapon[VehPrototype[protoID].weaponID].offenseRating <= 0 // PSI + non-combat
		|| Armor[VehPrototype[protoID].armorID].defenseRating < 0) // PSI
		&& protoID != BSC_SPORE_LAUNCHER) { // Spore Launcher exception
		return FALSE;
	}
	BYTE triad = Chassis[VehPrototype[protoID].chassisID].triad;
	if (triad == TRIAD_SEA) {
		return seaTriadRetn; // cursory check shows this value always being set to TRUE
	}
	if (triad == TRIAD_AIR) {
		return FALSE;
	}
	return has_abil(protoID, ABL_ARTILLERY); // TRIAD_LAND
}

/*
Purpose: Calculate Veh morale.
Original Offset: 005C0E40
Return Value: Morale value
Status: WIP
*/
DWORD __cdecl morale_veh(int vehID, BOOL checkDroneRiot, int factionIDvsNative) {
	DWORD factionID = Veh[vehID].factionID;
	if (!factionID) {
		return morale_alien(vehID, factionIDvsNative); // Native life
	}
	SHORT protoID = Veh[vehID].protoID;
	if (VehPrototype[protoID].plan == PLAN_INFO_WARFARE) {
		int probeMorale = range(PlayersData[factionID].SE_Probe, 0, 3);
		probeMorale += has_project(SP_TELEPATHIC_MATRIX, factionID) ? 2 : 0;
		for (int i = 0; i < MaxTechnologyNum; i++) {
			if (Technology[i].flags & IMPROVED_PROBES && has_tech(i, factionID)) {
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
	int morale = 0;
	//
	int ruleMorale = Players[factionID].ruleMorale;
	if (ruleMorale < 0) {
		morale += ruleMorale;
	}
	int homeBaseID = Veh[vehID].homeBaseID;
	if (homeBaseID >= 0) {
		DWORD offset, mask;
		bitmask(FAC_CHILDREN_CRECHE, &offset, &mask);
		if (Base[homeBaseID].facilitiesPresentTable[offset] & mask) {
			if (morale < 0) {
				morale /= 2;
			}
		}
		bitmask(FAC_BROOD_PIT, &offset, &mask);
		if (Base[homeBaseID].facilitiesPresentTable[offset] & mask) {
			if (protoID <= MaxVehProtoFactionNum 
				&& (offenseRating < 0 || protoID == BSC_SPORE_LAUNCHER) && morale < 0) {
				morale /= 2;
			}
		}
	}
	if (ruleMorale > 0) {
		morale += ruleMorale;
	}
	BOOL moraleFlag = Players[factionID].ruleFlags & FLAG_MORALE;
	if (moraleFlag && morale < 0) {
		morale = 0;
	}
	if (checkDroneRiot) {
		if (homeBaseID >= 0) {
			if (Base[homeBaseID].status & BSTATUS_DRONE_RIOTS_ACTIVE && !moraleFlag) {
				morale = range(morale--, 0, 6);
			}
		}
	}
	morale += Veh[vehID].morale;
	return range(morale, 0, 6);
}

/*
Purpose: Calculate offensive of prototype. Optional param of unit defending against (-1 to ignore)
		 as well as whether artillery or missile combat is being utilized.
Original Offset: 005C1150
Return Value: Prototype's armor
Status: Complete
*/
DWORD __cdecl offense_proto(int protoID, int vehIDDef, BOOL isArtyMissile) {
	DWORD weaponID = VehPrototype[protoID].weaponID;
	if (Weapon[weaponID].mode == WPN_MODE_INFOWAR && vehIDDef >= 0 
		&& VehPrototype[Veh[vehIDDef].protoID].plan == PLAN_INFO_WARFARE) {
		return 16; // probe attacking another probe
	}
	// Bug fix: Veh[].protoID with vehIDDef -1 could cause arbitrary memory read (Reactor struct)
	// due to lack of bounds checking when comparing vehIDDef protoID to Spore Launcher
	if ((isArtyMissile || (Weapon[weaponID].offenseRating >= 0
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
	return (vehIDDef < 0) ? Rules->PsiCombatRatioAtk[TRIAD_LAND] : // PSI
	   Rules->PsiCombatRatioAtk[Chassis[VehPrototype[Veh[vehIDDef].protoID].chassisID].triad] * 8;
}

/*
Purpose: Calculate defense of prototype. Optional param if unit is being attacked (-1 to ignore) 
         as well as whether artillery or missile combat is being utilized.
Original Offset: 005C1290
Return Value: Prototype's armor
Status: Complete
*/
DWORD __cdecl armor_proto(int protoID, int vehIDAtk, BOOL isArtyMissile) {
	if (Weapon[VehPrototype[protoID].weaponID].mode == WPN_MODE_INFOWAR && vehIDAtk >= 0
		&& VehPrototype[Veh[vehIDAtk].protoID].plan == PLAN_INFO_WARFARE) {
		return 16; // probe defending against another probe
	}
	// Bug fix: Veh[].protoID with vehIDAtk -1 could cause arbitrary memory read (Reactor struct)
	// due to lack of bounds checking when comparing vehIDAtk protoID to Spore Launcher
	if (isArtyMissile && (vehIDAtk < 0 || Veh[vehIDAtk].protoID != BSC_SPORE_LAUNCHER)
		&& protoID != BSC_SPORE_LAUNCHER 
		|| (Armor[VehPrototype[protoID].armorID].defenseRating >= 0 
		&& (vehIDAtk < 0 
			|| Weapon[VehPrototype[Veh[vehIDAtk].protoID].weaponID].offenseRating >= 0))) {
		DWORD defRating = range(Armor[VehPrototype[protoID].armorID].defenseRating, 1, 9999);
		return (vehIDAtk < 0) ? defRating : defRating * 8; // conventional
	}
	return (vehIDAtk < 0) ? Rules->PsiCombatRatioDef[TRIAD_LAND] : // PSI
		Rules->PsiCombatRatioDef[Chassis[VehPrototype[protoID].chassisID].triad] * 8;
}

/*
Purpose: Calculate speed of prototype on roads.
Original Offset: 005C13B0
Return Value: Prototype's speed
Status: Complete
*/
DWORD __cdecl speed_proto(int protoID) {
	if (protoID == BSC_FUNGAL_TOWER) {
		return 0; // cannot move
	}
	int chasID = VehPrototype[protoID].chassisID;
	int speedVal = Chassis[chasID].speed;
	BYTE triad = Chassis[chasID].triad;
	DWORD weapID = VehPrototype[protoID].weaponID;
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
		 if unit is damaged and other factors. The skipMorale parameter seems to only be set to TRUE
		 for certain combat calculations in battle_fight().
Original Offset: 005C1540
Return Value: speed
Status: Complete
*/
DWORD __cdecl speed(int vehID, BOOL skipMorale) {
	DWORD protoID = Veh[vehID].protoID;
	if (protoID == BSC_FUNGAL_TOWER) { // moved this check to top vs bottom, same logic
		return 0; // cannot move
	}
	DWORD speedVal = speed_proto(protoID);
	BYTE triad = Chassis[VehPrototype[protoID].chassisID].triad;
	if (triad == TRIAD_SEA && has_project(SP_MARITIME_CONTROL_CENTER, Veh[vehID].factionID)) {
		speedVal += Rules->MoveRateRoads * 2;
	}
	if (!skipMorale && morale_veh(vehID, 1, 0) == MORALE_ELITE && (protoID >= MaxVehProtoFactionNum
		|| Weapon[VehPrototype[protoID].weaponID].offenseRating >= 0)) {
		speedVal += Rules->MoveRateRoads;
	}
	if (Veh[vehID].dmgIncurred && triad != TRIAD_AIR) {
		// optimized code without changes to logic
		DWORD moves = speedVal / Rules->MoveRateRoads, reactorFac;
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
DWORD __cdecl veh_cargo(int vehID) {
	DWORD protoID = Veh[vehID].protoID;
	DWORD cargo = VehPrototype[protoID].carryCapacity;
	return (cargo && protoID < MaxVehProtoFactionNum 
		&& (Weapon[VehPrototype[protoID].weaponID].offenseRating < 0 // Isle of the Deep
			|| protoID == BSC_SPORE_LAUNCHER)) ? Veh[vehID].morale + 1 : cargo;
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
	Veh[vehID].movesExpended = (BYTE)speed(vehID, FALSE);
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
Status: WIP - test, incomplete flags/enums
*/
int __cdecl veh_wake(int vehID) {
	char orders = Veh[vehID].orders;
	int state = Veh[vehID].state;
	if (orders >= ORDER_FARM && orders < ORDER_GO_TO && !(state & 0x4000000)) {
		// TODO Bug: Issue with movesExpended size / speed return, see veh_skip()
		Veh[vehID].movesExpended = (BYTE)(speed(vehID, FALSE) - Rules->MoveRateRoads);
		int terraTurns = Veh[vehID].terraformingTurns;
		if (terraTurns) {
			terraTurns -= contribution(vehID, orders - 4);
			if (terraTurns < 0) {
				terraTurns = 0;
			}
			Veh[vehID].terraformingTurns = (BYTE)terraTurns;
		}
	}
	if (state & 0x200 && Veh[vehID].orderAutoType == ORDERA_ON_ALERT && !(state & 4)) {
		Veh[vehID].movesExpended = 0;
	}
	Veh[vehID].orders = ORDER_NONE;
	Veh[vehID].state &= 0xF4FFBDFF;
	return vehID;
}
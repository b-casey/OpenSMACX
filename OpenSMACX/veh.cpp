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
#include "general.h" // range
#include "technology.h"
#include "temp.h"

veh_prototype *VehPrototype = (veh_prototype *)0x009AB868; // [512]
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

/*
Purpose: Calculate base speed of prototype on roads
Original Offset: 005C13B0
Return Value: Prototype's speed
Status: Complete
*/
DWORD __cdecl speed_proto(int protoID) {
	if (protoID == BSC_FUNGAL_TOWER) {
		return 0;
	}
	int chasID = VehPrototype[protoID].chassisType;
	DWORD speed = Chassis[chasID].speed;
	BYTE triad = Chassis[chasID].triad;
	char weapID = VehPrototype[protoID].weaponType;
	if (triad == TRIAD_AIR) {
		speed += VehPrototype[protoID].reactorType * 2;
	}
	if (Weapon[weapID].mode == WPN_MODE_Transport) {
		speed--;
	}
	if (has_abil(protoID, ABL_SLOW)) {
		speed--;
	}
	if (has_abil(protoID, ABL_ANTIGRAV_STRUTS)) {
		speed += (triad == TRIAD_AIR) ? VehPrototype[protoID].reactorType * 2 : 1;
	}
	if (triad == TRIAD_AIR) {
		if (has_abil(protoID, ABL_FUEL_NANOCELLS)) {
			speed += 2;
		}
		int baseID = SecretProject->CloudbaseAcademy;
		if (baseID >= 0 && Base[baseID].factionIDCurrent == (protoID / MaxVehProtoFactionNum)) {
			speed += 2;
		}
		if (has_abil(protoID, ABL_AIR_SUPERIORITY)) {
			speed = (speed * 4) / 5;
		}
		if (Weapon[weapID].mode == WPN_MODE_Transport) {
			speed /= 2;
		}
	}
	return range(speed, 1, 99) * Rules->MoveRateRoads;
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
		return FALSE; // skip Native life faction / base prototypes from #UNITS
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
	if (VehPrototype[protoID].weaponType == WPN_Probe_Team && abilityID == ABL_ALGO_ENHANCEMENT 
		&& SecretProject->NethackTerminus >= 0 
		&& Base[SecretProject->NethackTerminus].factionIDCurrent == factionID) {
		return TRUE; // All Probe Teams act as though they have the "Algorithmic Enhancement"
	}
	return FALSE;
}

/*
Purpose: Calculate carrying/transport capacity for prototype
Original Offset: 0057D510
Return Value: Transport value
Status: Complete
*/
DWORD __cdecl transport_val(int chassisID, int ability, int reactorID) {
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
Purpose: Calculates cost of prototype based on various factors. Optimized logic flow from original 
         without any difference to final calculation.
Original Offset: 005A5A60
Return Value: Cost of prototype
Status: Complete
*/
DWORD __cdecl proto_cost(int chassisType, int weapType, 
	int armorType, int ability, int reactorType) {
	BYTE weapCost = Weapon[weapType].cost;
	// PB check: moved to start vs after 1st triad checks in original > no difference in logic
	if (Chassis[chassisType].missile && Weapon[weapType].offenseRating >= 99) {
		return weapCost;
	}
	BYTE triad = Chassis[chassisType].triad;
	BYTE armorCost = Armor[armorType].cost;
	BYTE speedCost = Chassis[chassisType].cost;
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
		speedCost += reactorType;
	} 
	else if (triad == TRIAD_AIR) {
		if (armorCost > 1) {
			armorCost *= reactorType * 2;
		}
		speedCost += reactorType * 2;
	} // TRIAD_LAND ability flag check moved into ability loop above
	DWORD combatMod = armorCost / 2 + 1;
	if (combatMod < weapCost) { // which ever is greater
		combatMod = weapCost;
	}
	int protoMod;
	// shifted this check to top vs at end > no difference in logic
	if (combatMod == 1 && armorCost == 1 && speedCost == 1 && reactorType == 1) {
		protoMod = 1;
	}
	else {
		// (2 << n) == 2^(n + 1) ; (2 << n) / 2 == 2 ^ n;
		// will crash if reactorType is 0xFF > divide by zero; not putting in error checking 
		// since this is unlikely even with modding however noting for future
		protoMod = ((speedCost + armorCost) * combatMod + ((2 << reactorType) / 2)) 
			/ (2 << reactorType);
		if (speedCost == 1) {
			protoMod = (protoMod / 2) + 1;
		}
		if (weapCost > 1 && Armor[armorType].cost > 1) {
			protoMod++;
			// moved inside nested check vs separate triad checks below > no difference in logic
			if (triad == TRIAD_LAND && speedCost > 1) {
				protoMod++;
			}
		}
		// excludes sea probes
		if (triad == TRIAD_SEA && Weapon[weapType].mode != WPN_MODE_InfoWar) {
			protoMod = (protoMod + 1) / 2;
		}
		else if (triad == TRIAD_AIR) {
			protoMod /= (Weapon[weapType].mode > WPN_MODE_Missile) ? 2 : 4; // Non-combat : Combat
		}
		int reactorMod = (reactorType * 3 + 1) / 2;
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
	return proto_cost(VehPrototype[protoID].chassisType, VehPrototype[protoID].weaponType, 
		VehPrototype[protoID].armorType, 0, VehPrototype[protoID].reactorType);
}

/*
Purpose: Create a new prototype. Sets initial values for everything except vehName and preqTech.
Original Offset: 005A5D40
Return Value: n/a
Status: Complete
*/
void __cdecl make_proto(int protoID, int chassisType, int weapType, 
	int armorType, int ability, int reactorType) {
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
					int loopWeapType = VehPrototype[protoIDLoop].weaponType;
					if (loopWeapType == weapType) {
						cond1 = TRUE;
					}
					int loopArmorType = VehPrototype[protoIDLoop].armorType;
					if (loopArmorType == armorType) {
						cond2 = TRUE;
					}
					int loopChassisType = VehPrototype[protoIDLoop].chassisType;
					if (loopChassisType == chassisType) {
						cond3 = TRUE;
					}
					int offRating = Weapon[weapType].offenseRating;
					if (offRating > 0 && Weapon[loopWeapType].offenseRating >= offRating) {
						cond1 = TRUE;
					}
					int defRating = Armor[armorType].defenseRating;
					if (defRating > 0 && Armor[loopArmorType].defenseRating >= defRating) {
						cond2 = TRUE;
					}
					if (Chassis[chassisType].triad == Chassis[loopChassisType].triad 
						&& Chassis[loopChassisType].speed >= Chassis[chassisType].speed) {
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
	VehPrototype[protoID].chassisType = chassisType;
	VehPrototype[protoID].weaponType = weapType;
	VehPrototype[protoID].armorType = armorType;
	VehPrototype[protoID].abilityFlags = ability;
	VehPrototype[protoID].reactorType = reactorType;
	VehPrototype[protoID].cost = (BYTE)proto_cost(chassisType, weapType, 
		armorType, ability, reactorType);
	VehPrototype[protoID].carryCapacity = (Weapon[weapType].mode == WPN_MODE_Transport) ? 
		(BYTE)transport_val(chassisType, ability, reactorType) : 0;
	// set plan & unk1
	if (Chassis[chassisType].missile) {
		if (Weapon[weapType].offenseRating < 99) { // non-PB missiles
			if (weapType == WPN_Tectonic_Payload) {
				VehPrototype[protoID].plan = PLAN_TECTONIC_MISSILE;
				VehPrototype[protoID].unk1 = 22;
			}
			else if (weapType == WPN_Fungal_Payload) {
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
	else if (Weapon[weapType].mode >= WPN_MODE_Transport) { // Non-combat
		VehPrototype[protoID].plan = Weapon[weapType].mode;
		VehPrototype[protoID].unk1 = Weapon[weapType].mode + 0x20;
	}
	else if (Chassis[chassisType].triad == TRIAD_SEA) { // combat sea
		VehPrototype[protoID].plan = PLAN_NAVAL_SUPERIORITY;
		VehPrototype[protoID].unk1 = 6; // same value as plan
	}
	else if (Chassis[chassisType].triad == TRIAD_AIR) { // combat air
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
		if (Armor[armorType].defenseRating > 1) {
			VehPrototype[protoID].plan = PLAN_DEFENSIVE;
		}
		if (Weapon[weapType].offenseRating >= Armor[armorType].defenseRating 
			&& VehPrototype[protoID].plan == PLAN_DEFENSIVE) {
			VehPrototype[protoID].plan = PLAN_COMBAT;
		}
		if (Chassis[chassisType].speed > 1 
			&& Weapon[weapType].offenseRating > Armor[armorType].defenseRating) {
			VehPrototype[protoID].plan = PLAN_OFFENSIVE;
		}
		if (ability & (ABL_ARTILLERY | ABL_DROP_POD | ABL_AMPHIBIOUS)) {
			VehPrototype[protoID].plan = PLAN_OFFENSIVE;
		}
		VehPrototype[protoID].unk1 = 3;
		if (Armor[armorType].defenseRating > Weapon[weapType].offenseRating) {
			VehPrototype[protoID].unk1 = 2;
		}
		if (Weapon[weapType].offenseRating > Armor[armorType].defenseRating * 2) {
			VehPrototype[protoID].unk1 = 1;
		}
		// changed from '>= 2' since '>= 3' will always overwrite it
		if (Chassis[chassisType].speed == 2) {
			VehPrototype[protoID].unk1 = 4;
		}
		// added else, no point in checking if speed==2
		else if (Chassis[chassisType].speed >= 3) {
			VehPrototype[protoID].unk1 = 5;
		}
		if (Weapon[weapType].offenseRating == 1 && Armor[armorType].defenseRating == 1) {
			if (Chassis[chassisType].speed > 1) {
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
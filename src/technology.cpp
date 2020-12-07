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
#include "technology.h"
#include "alpha.h"
#include "base.h"
#include "council.h"
#include "game.h"
#include "general.h"
#include "map.h"
#include "random.h"
#include "strings.h"
#include "veh.h"

rules_technology *Technology = (rules_technology *)0x0094F358;
uint8_t *GameTechAchieved = (uint8_t *)0x009A6670;
rules_mandate *Mandate = (rules_mandate *)0x0094B4A0;
int *TechValidCount = (int *)0x00949730;
int *TechCommerceCount = (int *)0x00949734;
char TechName[80];

/*
Purpose: Get tech string for techID and store it into stringTemp buffer.
Original Offset: 005591A0
Return Value: n/a
Status: Complete
*/
void __cdecl say_tech(int techID, BOOL categoryLvl) {
	say_tech(stringTemp->str, techID, categoryLvl);
}

/*
Purpose: Determine whether a faction is able to jump up the tech tree for the specified tech.
Original Offset: 0057CE50
Return Value: Is a tech leap possible? true/false
Status: Complete
*/
BOOL __cdecl valid_tech_leap(uint32_t techID, int factionID) {
	int preq1 = Technology[techID].preqTech1;
	if (preq1 >= 0 && !(has_tech(Technology[preq1].preqTech1, factionID)
		&& has_tech(Technology[preq1].preqTech2, factionID))) {
		return false; // doesn't have 1st prerequisite prerequisites
	}
	int preq2 = Technology[techID].preqTech2;
	if (preq2 >= 0 && !(has_tech(Technology[preq2].preqTech1, factionID)
		&& has_tech(Technology[preq2].preqTech2, factionID))) {
		return false; // doesn't have 2nd prerequisite prerequisites
	}
	if (preq1 <= TechDisabled || preq2 <= TechDisabled) {
		return false; // disabled
	}
	for (uint32_t i = 0; i < MaxReactorNum; i++) {
		if ((int)techID == Reactor[i].preqTech) {
			return false; // leap not possible for reactor tech
		}
	}
	for (uint32_t i = 0; i < MaxWeaponNum; i++) {
		if (Weapon[i].preqTech == (int)techID) { // may end early if two weapons have the same preq
			return (PlayersData[factionID].ranking <= 2 // lowest two ranking factions
				// this line is an odd comparison (offensive rating <= best weapon id + 2)
				// however it might be to prevent leaps for later weapon tech
				&& Weapon[i].offenseRating <= (weapon_budget(factionID, 99, false) + 2));
		}
	}
	return true;
}

/*
Purpose: Craft an output string related to a specific technology. For techIDs outside the standard
		 range, craft a string related to world map, comm links or prototypes.
Original Offset: 005B9C40
Return Value: n/a
Status: Complete
*/
void __cdecl say_tech(LPSTR output, int techID, BOOL categoryLvl) {
	if (techID < -1) {
		strcat_s(output, 80, label_get(310)); // "Not Available"
	}
	else if (techID < 0) {
		strcat_s(output, 80, label_get(25)); // "NONE"
	}
	else if (techID == 9999) {
		strcat_s(output, 80, label_get(306)); // "World Map"
	}
	else if (techID < MaxTechnologyNum) {
		strcat_s(output, 80, StringTable->get((int)Technology[techID].name));
		if (categoryLvl) {
			sprintf_s(&output[strlen(output)], 80, " (%s%d)", // 'E#', 'D#', 'B#', 'C#'
				label_get(629 + tech_category(techID)), tech_recurse(techID, 0));
		}
	}
	else if (techID < 97) {
		if (*Language) {
			sprintf_s(&output[strlen(output)], 80, "%s (%s)", label_get(487), // 'Comm Frequency'
				get_noun(techID - 89));
		}
		else {
			sprintf_s(&output[strlen(output)], 80, "%s %s", Players[techID - 89].adjNameFaction,
				label_get(487)); // 'Comm Frequency'
		}
	}
	else {
		sprintf_s(&output[strlen(output)], 80, "%s %s", VehPrototype[techID - 97].vehName,
			label_get(185)); // 'Prototype'
	}
}

/*
Purpose: Get tech string for techID and store it into TechName buffer.
Original Offset: 005B9EF0
Return Value: Pointer to TechName
Status: Complete
*/
LPSTR __cdecl tech_name(int techID, BOOL categoryLvl) {
	TechName[0] = 0;
	say_tech(TechName, techID, categoryLvl);
	return TechName;
}

/*
Purpose: Check whether faction has a particular tech or not.
Original Offset: 005B9F20
Return Value: Faction has tech? true/false
Status: Complete
*/
BOOL __cdecl has_tech(int techID, int factionID) {
	if (factionID <= 0) {
		return false;
	}
	if (techID == TechNone) {
		return true;
	}
	if (techID < 0 || techID >= (MaxTechnologyNum - 1) // excludes 'Transcendent Thought'
		|| Technology[techID].preqTech1 == TechDisabled
		|| (Technology[techID].preqTech2 == TechDisabled
			&& Technology[techID].preqTech1 != TechNone)) {
		// "none, disable" ; valid #TECH preqTech entry
		return false;
	}
	return ((1 << factionID) & GameTechAchieved[techID]) != 0;
}

/*
Purpose: Determine technology level for techID.
Original Offset: 005B9F90
Return Value: Level
Status: Complete
*/
int __cdecl tech_recurse(int techID, int baseLevel) {
	if (techID < 0 || techID >= MaxTechnologyNum) {
		return baseLevel;
	}
	int val1 = tech_recurse(Technology[techID].preqTech1, baseLevel + 1);
	int val2 = tech_recurse(Technology[techID].preqTech2, baseLevel + 1);
	return (val1 > val2) ? val1 : val2;
}

/*
Purpose: Determine what category is dominate for techID. If there is a tie, the order of precedence
		 is as follows: growth > tech > wealth > power.
Original Offset: 005B9FE0
Return Value: Tech category id: growth (0), tech (1), wealth (2) or power (3).
Status: Complete
*/
int __cdecl tech_category(int techID) {
	uint32_t category = 0;
	int compare = Technology[techID].growthValue;
	if (Technology[techID].techValue > compare) {
		category = 1;
		compare = Technology[techID].techValue;
	}
	if (Technology[techID].wealthValue > compare) {
		category = 2;
		compare = Technology[techID].wealthValue;
	}
	return (Technology[techID].powerValue > compare) ? 3 : category;
}

/*
Purpose: Check to see whether provided faction can research a specific technology. Checks are
		 included to prevent SMACX specific Veh from being built in SMAC mode.
Original Offset: 005BAC20
Return Value: Is tech available to faction? true/false
Status: Complete
*/
BOOL __cdecl tech_avail(int techID, int factionID) {
	if (has_tech(techID, factionID) || techID >= MaxTechnologyNum || (!*ExpansionEnabled
		&& (techID == TECH_PRPSYCH || techID == TECH_FLDMOD || techID == TECH_ADAPDOC
			|| techID == TECH_ADAPECO || techID == TECH_BIOADAP || techID == TECH_SENTRES
			|| techID == TECH_SECMANI || techID == TECH_NEWMISS || techID == TECH_BFG9000))) {
		return false;
	}
	int preqTech1 = Technology[techID].preqTech1, preqTech2 = Technology[techID].preqTech2;
	if (preqTech1 < TechNone || preqTech2 < TechNone) {
		return false; // if either prerequisite tech is set to disabled (-2)
	}
	return (has_tech(preqTech1, factionID) && has_tech(preqTech2, factionID));
}

/*
Purpose: Calculate faction's tech related bonuses for commerce and resource production in fungus.
Original Offset: 005BAE60
Return Value: n/a
Status: Complete
*/
void __cdecl tech_effects(int factionID) {
	PlayersData[factionID].techCommerceBonus = Players[factionID].ruleCommerce;
	PlayersData[factionID].techFungusNutrient = 0;
	PlayersData[factionID].techFungusMineral = 0;
	PlayersData[factionID].techFungusEnergy = 0;
	PlayersData[factionID].techFungusUnk = 0;
	for (uint32_t techID = 0; techID < MaxTechnologyNum; techID++) {
		if (has_tech(techID, factionID)) {
			uint32_t flags = Technology[techID].flags;
			if (flags & TFLAG_INC_ENERGY_FUNGUS) {
				PlayersData[factionID].techFungusEnergy++;
			}
			if (flags & TFLAG_INC_MINERALS_FUNGUS) {
				PlayersData[factionID].techFungusMineral++;
			}
			if (flags & TFLAG_INC_NUTRIENT_FUNGUS) {
				PlayersData[factionID].techFungusNutrient++;
			}
			if (flags & TFLAG_INC_COMMERCE) {
				PlayersData[factionID].techCommerceBonus++;
			}
		}
	}
	for (int i = 0; i < Players[factionID].factionBonusCount; i++) {
		if (Players[factionID].factionBonusID[i] == RULE_FUNGNUTRIENT) {
			PlayersData[factionID].techFungusNutrient += Players[factionID].factionBonusVal1[i];
		}
		else if (Players[factionID].factionBonusID[i] == RULE_FUNGMINERALS) {
			PlayersData[factionID].techFungusMineral += Players[factionID].factionBonusVal1[i];
		}
		else if (Players[factionID].factionBonusID[i] == RULE_FUNGENERGY) {
			PlayersData[factionID].techFungusEnergy += Players[factionID].factionBonusVal1[i];
		}
	}
	// if values are below zero, cap at zero
	for (int *techFungus = &PlayersData[factionID].techFungusNutrient,
		*end = &PlayersData[factionID].techFungusUnk; techFungus <= end; techFungus++) {
		if (*techFungus < 0) {
			*techFungus = 0;
		}
	}
	if (PlayersData[factionID].socEffectPending.economy > 2) {
		PlayersData[factionID].techCommerceBonus++;
		if (PlayersData[factionID].socEffectPending.economy > 3) {
			PlayersData[factionID].techCommerceBonus++;
			if (PlayersData[factionID].socEffectPending.economy > 4) {
				PlayersData[factionID].techCommerceBonus++;
			}
		}
	}
}

/*
Purpose: Determine if preqTechID is a prerequisite of parentTechID within descending range.
Original Offset: 005BCB60
Return Value: Is preqTechID prerequisite of parentTechID? true/false
Status: Complete
*/
BOOL __cdecl tech_is_preq(int preqTechID, int parentTechID, uint32_t range) {
	if (preqTechID < 0 || parentTechID < 0) {
		return false;
	}
	if (preqTechID == parentTechID) {
		return true;
	}
	if (!range) {
		return false;
	}
	return tech_is_preq(preqTechID, Technology[parentTechID].preqTech1, range - 1)
		|| tech_is_preq(preqTechID, Technology[parentTechID].preqTech2, range - 1);
}

/*
Purpose: Determine how valuable the specified techID is to a faction. This id either corresponds to
		 a technology (0-88), another faction (89-96) or a prototype (97-608). The 3rd parameter
		 determines whether a simplistic or extended calculation is required for a technology id.
Original Offset: 005BCBE0
Return Value: Value of techID to the specified faction
Status: Complete
*/
int __cdecl tech_val(int techID, int factionID, BOOL simpleCalc) {
	if (techID == 9999) {
		return 2;
	}
	int valueRet;
	if (techID < MaxTechnologyNum) {
		uint32_t vendettaCount = 0;
		for (int i = 1; i < MaxPlayerNum; i++) {
			if (i != factionID && has_treaty(factionID, i, DTREATY_VENDETTA)) {
				vendettaCount++;
			}
		}
		uint32_t factorAI = 1;
		if (!simpleCalc) {
			factorAI = (*GameRules & RULES_BLIND_RESEARCH) ? 4 : 2;
		}
		BOOL aiGrowth = PlayersData[factionID].AI_Growth, aiPower = PlayersData[factionID].AI_Power,
			aiWealth = PlayersData[factionID].AI_Wealth, aiTech = PlayersData[factionID].AI_Tech;
		int growthVal = Technology[techID].growthValue, powerVal = Technology[techID].powerValue,
			wealthVal = Technology[techID].wealthValue, techVal = Technology[techID].techValue;
		valueRet = growthVal * (factorAI * aiGrowth + 1) + wealthVal * (factorAI * aiWealth + 1)
			+ techVal * (factorAI * aiTech + 1) + powerVal * (factorAI * aiPower + 1);
		uint32_t baseCount = PlayersData[factionID].currentNumBases;
		if ((!powerVal || !aiPower && !vendettaCount) && (!techVal || !aiTech)
			&& (!wealthVal || !aiWealth) && (!growthVal || !aiGrowth && baseCount >= 4)) {
			valueRet = (valueRet + 1) / 2;
		}
		BOOL isHuman = is_human(factionID);
		if (!isHuman && !has_tech(techID, factionID) && simpleCalc) {
			uint32_t achievedCount = bit_count(GameTechAchieved[techID]);
			if (achievedCount > 1) {
				valueRet += 2 - 2 * achievedCount; // if more than 1 faction has tech, inc priority
			}
			int searchLvl = 1;
			for (int i = 0; i < MaxTechnologyNum; i++) {
				if (has_tech(i, factionID)) {
					int currentLvl = tech_recurse(i, 0);
					if (searchLvl < currentLvl) {
						searchLvl = currentLvl;
					}
				}
			}
			int techIDLvl = tech_recurse(techID, 0);
			if (techIDLvl < searchLvl) {
				valueRet = valueRet * (techIDLvl + 1) / (searchLvl + 1);
			}
			if (valueRet < 1) {
				valueRet = 1;
			}
		}
		if (simpleCalc) {
			return valueRet;
		}
		if (baseCount) {
			for (int region = 1; region < MaxRegionLandNum; region++) {
				if (!bad_reg(region)) {
					uint32_t pwrBase = PlayersData[factionID].regionTotalBases[region] * powerVal;
					uint8_t plan = PlayersData[factionID].regionBasePlan[region];
					if (plan == PLAN_NAVAL_TRANSPORT && vendettaCount && !isHuman) {
						valueRet += (pwrBase / baseCount);
					}
					else if (plan == PLAN_DEFENSIVE) {
						valueRet += (pwrBase * 4) / (baseCount * (isHuman + 1));
					}
					else if (plan == PLAN_OFFENSIVE) {
						valueRet += (pwrBase * ((PlayersData[factionID].bestWeaponValue
							>= PlayersData[factionID].enemyBestWeaponValue) ? 2 : 4))
							/ (baseCount * (isHuman + 1));
					}
					else {
						for (int i = 1; i < MaxPlayerNum; i++) {
							if (i != factionID && PlayersData[i].regionTotalBases[region]
								&& PlayersData[factionID].regionTotalBases[region] 
								&& has_treaty(factionID, i, DTREATY_COMMLINK) 
								&& (!has_treaty(factionID, i, DTREATY_PACT | DTREATY_TREATY)
									|| has_treaty(factionID, i, DTREATY_WANT_REVENGE))) {
								valueRet += (pwrBase / (baseCount * (isHuman + 1)));
							}
						}
					}
				}
			}
		}
		if (has_tech(techID, factionID)) {
			return valueRet; // should this be moved further up?
		}
		if (climactic_battle()
			&& tech_is_preq(techID, Facility[FAC_ASCENT_TO_TRANSCENDENCE].preqTech, 2)) {
			valueRet *= 4;
		}
		if (PlayersData[factionID].socEffectBase.planet > 0 && aiGrowth) {
			if (tech_is_preq(techID, TECH_CENTMED, 9999)) {
				valueRet *= 3;
			}
			if (tech_is_preq(techID, TECH_PLAECON, 9999)) {
				valueRet *= 2;
			}
			if (tech_is_preq(techID, TECH_ALPHCEN, 3)) {
				valueRet *= 2;
			}
		}
		if (PlayersData[factionID].socEffectBase.probe <= 0) {
			if (tech_is_preq(techID, Facility[FAC_HUNTER_SEEKER_ALGO].preqTech, aiTech + 2)) {
				if (!aiPower) {
					valueRet *= 2;
				}
				if (aiTech) {
					valueRet *= 2;
				}
			}
		}
		if (aiGrowth && tech_is_preq(techID, TECH_DOCINIT, 2)) {
			valueRet *= 2;
		}
		if ((aiWealth || !*MapCloudCover) && tech_is_preq(techID, TECH_ENVECON, 9999)) {
			valueRet *= 2;
		}
		if (Technology[techID].flags & TFLAG_SECRETS && !GameTechAchieved[techID]
			&& !(*GameRules & RULES_BLIND_RESEARCH)) {
			valueRet *= (aiPower + 1) * 2;
		}
		if (Players[factionID].rulePsi > 0) {
			if (tech_is_preq(techID, Facility[FAC_DREAM_TWISTER].preqTech, 9999)) {
				valueRet *= 2;
			}
		}
		else {
			int preqTechFusion = Reactor[RECT_FUSION - 1].preqTech;
			if (techID == preqTechFusion) {
				valueRet *= 2;
			}
			if (techID == Reactor[RECT_QUANTUM - 1].preqTech) {
				valueRet *= 2;
			}
			if (tech_is_preq(techID, preqTechFusion, 9999)) {
				valueRet++;
			}
			if (tech_is_preq(techID, preqTechFusion, 1) && !(*GameRules & RULES_BLIND_RESEARCH)) {
				valueRet *= 2;
			}
		}
		int ecoDmgUnk = PlayersData[factionID].unk_49 / range(baseCount, 1, 9999);
		if (ecoDmgUnk > aiPower && (tech_is_preq(techID, Facility[FAC_HYBRID_FOREST].preqTech, 9999)
			|| tech_is_preq(techID, Facility[FAC_TREE_FARM].preqTech, 9999)
			|| tech_is_preq(techID, Facility[FAC_CENTAURI_PRESERVE].preqTech, 9999)
			|| tech_is_preq(techID, Facility[FAC_TEMPLE_OF_PLANET].preqTech, 9999))) {
			valueRet += ecoDmgUnk;
		}
		if (Players[factionID].rulePopulation > 0) {
			if (tech_is_preq(techID, Facility[FAC_HAB_COMPLEX].preqTech, 9999)) {
				valueRet *= 2;
			}
			else if (tech_is_preq(techID, Facility[FAC_HABITATION_DOME].preqTech, 9999)
				&& *TurnCurrentNum > 250) {
				valueRet = (valueRet * 3) / 2;
			}
		}
		if (aiPower) {
			for (int i = 0; i < MaxWeaponNum; i++) {
				if (Weapon[i].offenseRating) {
					int weapPreqTech = Weapon[i].preqTech;
					if (techID == weapPreqTech) {
						valueRet *= (isHuman + 3);
					}
					else if (tech_is_preq(techID, weapPreqTech, 1)) {
						valueRet *= (isHuman + 2);
					}
				}
			}
		}
		if (aiTech || !aiPower) {
			for (int i = 0; i < MaxTechnologyNum; i++) {
				if (!has_tech(i, factionID) && Technology[i].flags & TFLAG_SECRETS
					&& !GameTechAchieved[i] && tech_is_preq(techID, i, 1)) {
					valueRet *= 3;
				}
			}
		}
		if (tech_is_preq(techID, VehPrototype[BSC_FORMERS].preqTech, 9999)
			&& !has_tech(VehPrototype[BSC_FORMERS].preqTech, factionID)) {
			valueRet *= 2;
			if (isHuman) {
				valueRet *= 2;
			}
		}
		if (tech_is_preq(techID, Chassis[CHSI_FOIL].preqTech, 9999)
			&& !has_tech(Chassis[CHSI_FOIL].preqTech, factionID)) {
			BOOL search = false;
			for (int region = 1; region < MaxRegionLandNum; region++) {
				if (PlayersData[factionID].regionTotalBases[region]) {
					for (int i = 1; i < MaxPlayerNum; i++) {
						if (factionID != i && !PlayersData[i].regionTotalBases[region]) {
							search = true;
							break;
						}
					}
					if (search
						&& PlayersData[factionID].unk_79[region] >= Continents[region].tiles) {
						valueRet *= 3;
						if (isHuman) {
							valueRet *= 2;
						}
						break;
					}
				}
			}
			if (search) {
				valueRet = (valueRet * 2) + 4;
			}
		}
	}
	else if (techID < 97) { // factions
		uint32_t factor = 1, factionID2 = techID - MaxTechnologyNum;
		if (!wants_to_attack_(factionID, factionID2, 0)) {
			factor = 2;
		}
		if (!wants_to_attack_(factionID2, factionID, 0)) {
			factor++;
		}
		valueRet = factor * (factor / (PlayersData[factionID].AI_Fight + 2));
	}
	else {  // prototypes
		uint32_t protoID = techID - 97;
		valueRet = range(Weapon[VehPrototype[protoID].weaponID].offenseRating, 1, 2)
			+ range(Armor[VehPrototype[protoID].armorID].defenseRating, 1, 2)
			+ range(Chassis[VehPrototype[protoID].chassisID].speed, 1, 2)
			+ VehPrototype[protoID].reactorID - 2;
	}
	return valueRet;
}

/*
Purpose: Determine a tech the specified faction should research.
Original Offset: 005BDC10
Return Value: techID or -1
Status: Complete
*/
int __cdecl tech_ai(int factionID) {
	int techID = -1, search = -999;
	BOOL isHuman = is_human(factionID);
	for (int i = 0; i < MaxTechnologyNum; i++) {
		if (tech_avail(i, factionID)) {
			int techValue = tech_val(i, factionID, false), compare;
			if (*GameRules & RULES_BLIND_RESEARCH) {
				if (isHuman && (PlayersData[factionID].AI_Growth
					|| PlayersData[factionID].AI_Wealth)
					&& i == VehPrototype[BSC_FORMERS].preqTech) {
					return i; // Direct human player research toward gaining Formers
				}
				int preq = tech_recurse(i, 0);
				techValue = preq ? (techValue << 8) / preq : 0;
			}
			compare = isHuman ? random(0, techValue + 1) // not sure why humans get random()
				: rnd(techValue + 1, NULL); // vs AI rnd()
			if (compare > search) {
				search = compare;
				techID = i;
			}
		}
	}
	return techID;
}

/*
Purpose: Get powerValue from technology struct for techID
Original Offset: 005BDD70
Return Value: powerValue or 0 if techID is greater than max
Status: Complete
*/
int __cdecl tech_mil(int techID) {
	if (techID >= MaxTechnologyNum) {
		return 0; // why only this one returns 0 with other three returning 1? typo/bug?
	}
	return *(&Technology[techID].powerValue);
}

/*
Purpose: Get techValue from technology struct for techID
Original Offset: 005BDD90
Return Value: techValue or 1 if techID is greater than max
Status: Complete
*/
int __cdecl tech_tech(int techID) {
	if (techID >= MaxTechnologyNum) {
		return 1;
	}
	return *(&Technology[techID].techValue);
}

/*
Purpose: Get wealthValue from technology struct for techID
Original Offset: 005BDDC0
Return Value: wealthValue or 1 if techID is greater than max
Status: Complete
*/
int __cdecl tech_infra(int techID) {
	if (techID >= MaxTechnologyNum) {
		return 1;
	}
	return *(&Technology[techID].wealthValue);
}

/*
Purpose: Get growthValue from technology struct for techID
Original Offset: 005BDDF0
Return Value: growthValue or 1 if techID is greater than max
Status: Complete
*/
int __cdecl tech_colonize(int techID) {
	if (techID >= MaxTechnologyNum) {
		return 1;
	}
	return *(&Technology[techID].growthValue);
}

/*
Purpose: Calculate how much researching a tech will cost the specified faction.
Original Offset: 005BE6B0
Return Value: tech rate/cost
Status: Complete
*/
uint32_t __cdecl tech_rate(uint32_t factionID) {
	if (PlayersData[factionID].techCost >= 0) {
		return PlayersData[factionID].techCost; // already set
	}
	if (!Rules->TechDiscoveryRatePctStd) {
		return 999999999; // max cost
	}
	uint32_t playerFactor = range(PlayersData[factionID].earnedTechsSaved * 2
		- PlayersData[factionID].unk_26 + PlayersData[factionID].techRanking, 2, 9999);
	uint32_t topFactor = 0;
	for (uint32_t i = 1; i < MaxPlayerNum; i++) {
		uint32_t compare = PlayersData[i].earnedTechsSaved * 2 + PlayersData[i].techRanking;
		if (compare > topFactor) {
			topFactor = compare;
		}
	}
	playerFactor /= 2;
	topFactor /= 2;
	BOOL isHuman = is_human(factionID);
	uint32_t diffFactor = isHuman ? PlayersData[factionID].diffLevel : *DiffLevelCurrent;
	diffFactor += (diffFactor < DLVL_LIBRARIAN);
	uint32_t diffLvl = !isHuman ? *DiffLevelCurrent : DLVL_LIBRARIAN;
	diffFactor = isHuman ? diffFactor * 4 + 8 : 29 - diffFactor * 3;
	diffFactor = range(diffFactor, 12 - playerFactor, playerFactor + 12);
	uint32_t techStagnation = *GameRules & RULES_TECH_STAGNATION;
	uint32_t ruleFactor = techStagnation | 0x40; // 64 or 96
	uint32_t finFactor = range(playerFactor - (*TurnCurrentNum / (ruleFactor >> 3)),
		0, (diffFactor * (ruleFactor >> 5)) >> 1) + diffFactor;
	int reschBase = range(PlayersData[factionID].socEffectBase.research, -1, 1);
	uint32_t discovRate = (finFactor
		- range((topFactor - diffLvl - playerFactor + 7) / (8 - diffLvl),
			0, diffLvl * finFactor / 10 + 1))
		* range(playerFactor - reschBase, 1, 99999);
	if (Rules->TechDiscoveryRatePctStd != 100) {
		discovRate = 100 * discovRate / Rules->TechDiscoveryRatePctStd;
	}
	if (Players[factionID].ruleTechcost != 100) {
		discovRate = discovRate * Players[factionID].ruleTechcost / 100;
	}
	uint32_t cost = (discovRate * *MapAreaSqRoot) / 56;
	if (techStagnation) {
		cost += cost / 2; // Slower Rate of Research Discoveries
	}
	return range(cost, 1, 99999999);
}

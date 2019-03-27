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
#include "temp.h"
#include <string>
#include "alpha.h"
#include "base.h"
#include "faction.h"
#include "general.h"
#include "map.h"
#include "strings.h"
#include "technology.h"
#include "terraforming.h"
#include "text.h"
#include "textindex.h"
#include "veh.h"

Label *Labels = (Label *)0x009B90F8;
rules_resourceinfo *ResourceInfo = (rules_resourceinfo *)0x00945F50;
rules_time_control *TimeControl = (rules_time_control *)0x0094F1B8;
LPSTR *Compass = (LPSTR *)0x00945D48;
rules_resource *Resource = (rules_resource *)0x00946158;
rules_energy *Energy = (rules_energy *)0x0094A318;
LPSTR *Difficulty = (LPSTR *)0x0096C85C;

/*
Purpose: Parse #LABELS section inside labels.txt
Original Offset: 00616A00
Return Value: Was there an error? TRUE/FALSE
Status: Complete
*/
BOOL __cdecl labels_init() {
	labels_shutdown();
	if (text_open("labels", "labels")) {
		return TRUE;
	}
	text_get();
	Labels->count = text_item_number();
	Labels->stringsPtr = (LPSTR)mem_get(Labels->count * 4);
	if (!Labels->stringsPtr) {
		return TRUE;
	}
	for (int i = 0; i < Labels->count; i++) {
		*((LPSTR *)Labels->stringsPtr + i) = text_string();
	}
	text_close();
	return FALSE;
}

/*
Purpose: Clean up labels global variable
Original Offset: 006169D0
Return Value: n/a
Status: Complete
*/
void __cdecl labels_shutdown() {
	if (Labels->stringsPtr) {
		free(Labels->stringsPtr);
		Labels->stringsPtr = 0;
	}
	Labels->count = 0;
}

/*
Purpose: Get tech id value by parsing tech name from Txt item buffer. Optimized out of vanilla code,
         useful for readability.
Original Offset: 00585150
Return Value: Tech id
Status: Complete
*/
int __cdecl tech_item() { 
	text_get(); 
	return tech_name(text_item()); 
}

/*
Purpose: Parse and set noun item's gender and plurality from Txt buffer. Optimized out of vanilla 
         code, useful for readability.
Original Offset: 005871D0
Return Value: n/a
Status: Complete
*/
void __cdecl noun_item(int *gender, BOOL *plurality) {
	LPSTR noun = text_item();
	*gender = 0; // defaults to male ('m' || 'M')
	*plurality = FALSE; // defaults to singular ('1')
	if (noun[0] == 'f' || noun[0] == 'F') {
		*gender = 1;
	}
	else if (noun[0] == 'n' || noun[0] == 'N') {
		*gender = 2;
	}
	if (noun[1] == '2') {
		*plurality = TRUE;
	}
}

/*
Purpose: Convert tech name string to number offset id
Original Offset: 00584D60
Return Value: -1: 'None'; -2: 'Disabled' or error; Otherwise, tech id offset
Status: Complete. Crash bug if BADTECHKEY is triggered.
*/
int __cdecl tech_name(LPSTR techName) {
	purge_trailing(techName);
	if (!_stricmp(techName, "None")) {
		return NoneValue;
	}
	if (!_stricmp(techName, "Disable")) {
		return DisabledValue;
	}
	for (int techID = 0; techID < MaxTechnologyNum; techID++) {
		if (!_stricmp(techName, Technology[techID].id)) {
			return techID;
		}
	}
	parse_says(0, Txt->getFilePath(), -1, -1);
	parse_says(1, techName, -1, -1);
	parse_says(2, *ParseTempPtr1_1, -1, -1);
	X_pop("BADTECHKEY", 0);
	return DisabledValue;
}

/*
Purpose: Convert chassis name string to number offset id
Original Offset: 00584E40
Return Value: 0: error; -1: 'None'; -2: 'Disabled'; Otherwise, chassis id offset
Status: Complete. Crash bug if BADCHASKEY is triggered.
*/
int __cdecl chas_name(LPSTR chasName) {
	purge_trailing(chasName);
	if (!_stricmp(chasName, "None")) {
		return NoneValue;
	}
	if (!_stricmp(chasName, "Disable")) {
		return DisabledValue;
	}
	for (int chasID = 0; chasID < MaxChassisNum; chasID++) {
		if (!_stricmp(chasName, Chassis[chasID].offsv1Name)) {
			return chasID;
		}
	}
	parse_says(0, Txt->getFilePath(), -1, -1);
	parse_says(1, chasName, -1, -1);
	parse_says(2, *ParseTempPtr1_1, -1, -1);
	X_pop("BADCHASKEY", 0);
	return 0;
}

/*
Purpose: Convert weapon name string to number offset id
Original Offset: 00584F40
Return Value: 0: error; -1: 'None'; -2: 'Disabled'; Otherwise, weapon id offset
Status: Complete. Crash bug if BADWEAPKEY is triggered.
*/
int __cdecl weap_name(LPSTR weapName) {
	purge_trailing(weapName);
	if (!_stricmp(weapName, "None")) {
		return NoneValue;
	}
	if (!_stricmp(weapName, "Disable")) {
		return DisabledValue;
	}
	for (int weapID = 0; weapID < MaxWeaponNum; weapID++) {
		if (!_stricmp(weapName, Weapon[weapID].nameShort)) {
			return weapID;
		}
	}
	parse_says(0, Txt->getFilePath(), -1, -1);
	parse_says(1, weapName, -1, -1);
	parse_says(2, *ParseTempPtr1_1, -1, -1);
	X_pop("BADWEAPKEY", 0);
	return 0;
}

/*
Purpose: Convert armor name string to number offset id
Original Offset: 00585030
Return Value: 0: error; -1: 'None'; -2: 'Disabled'; Otherwise, armor id offset
Status: Complete. Crash bug if BADARMKEY is triggered.
*/
int __cdecl arm_name(LPSTR armName) {
	purge_trailing(armName);
	if (!_stricmp(armName, "None")) {
		return NoneValue;
	}
	if (!_stricmp(armName, "Disable")) {
		return DisabledValue;
	}
	for (int armID = 0; armID < MaxArmorNum; armID++) {
		if (!_stricmp(armName, Armor[armID].nameShort)) {
			return armID;
		}
	}
	parse_says(0, Txt->getFilePath(), -1, -1);
	parse_says(1, armName, -1, -1);
	parse_says(2, *ParseTempPtr1_1, -1, -1);
	X_pop("BADARMKEY", 0);
	return DisabledValue;
}

/*
Purpose: Parse #RULES & #WORLDBUILDER sections inside alpha(x).txt
Original Offset: 00585170
Return Value: Was there an error? TRUE/FALSE
Status: Complete
*/
BOOL __cdecl read_basic_rules() {
	// Rules
	if (text_open(AlphaxFileID, "RULES")) {
		return TRUE;
	}
	*RulesMoveRateRoads = text_get_number(1, 100);
	*RulesNutrientReqCitizen = text_get_number(0, 100);
	text_get();
	*RulesArtilleryDmgNum = range(text_item_number(), 1, 1000);
	*RulesArtilleryDmgDenom = range(text_item_number(), 1, 1000);
	*RulesArtilleryMaxRng = text_get_number(1, 8);
	*RulesMaxAirdropSansOrbInsert = text_get_number(1, 500);
	*RulesNutrientCostMulti = text_get_number(1, 100);
	*RulesMineralCostMulti = text_get_number(1, 100);
	*RulesTechDiscovRatePctStd = text_get_number(0, 1000);
	*RulesLimitMineralMineSansRoad = text_get_number(0, 100);
	*RulesTglNutrientEffectWithMine = text_get_number(-1, 0); // Some weird logic behind -1/0 value
	*RulesMinBaseSizeSpecialists = text_get_number(0, 100);
	*RulesDronesGenejackFactory = text_get_number(0, 100);
	*RulesPopLimitSansHabComplex = text_get_number(1, 100);
	*RulesPopLimitSansHabDome = text_get_number(1, 100);
	*RulesExtraPctCostProtoLand = text_get_number(0, 500);
	*RulesExtraPctCostProtoSea = text_get_number(0, 500);
	*RulesExtraPctCostProtoAir = text_get_number(0, 500);
	text_get();
	*RulesPsiCombatRatioLandAtk = range(text_item_number(), 1, 1000);
	*RulesPsiCombatRatioLandDef = range(text_item_number(), 1, 1000);
	text_get();
	*RulesPsiCombatRatioSeaAtk = range(text_item_number(), 1, 1000);
	*RulesPsiCombatRatioSeaDef = range(text_item_number(), 1, 1000);
	text_get();
	*RulesPsiCombatRatioAirAtk = range(text_item_number(), 1, 1000);
	*RulesPsiCombatRatioAirDef = range(text_item_number(), 1, 1000);
	*RulesPlayerStartEnergyReserve = text_get_number(0, 1000);
	*RulesCombatPctBaseDef = text_get_number(-100, 1000);
	*RulesCombatPctAtkRoad = text_get_number(-100, 1000);
	*RulesCombatPctAtkHigherElev = text_get_number(-100, 1000);
	*RulesCombatPenPctAtkLwrElev = text_get_number(-100, 1000);
	*RulesCombatPctMobileOpenGround = text_get_number(-100, 1000);
	*RulesCombatPctDefVsMobileRough = text_get_number(-100, 1000);
	*RulesCombatPctInfantryVsBase = text_get_number(-100, 1000);
	*RulesCombatPenPctAtkAirdrop = text_get_number(-100, 1000);
	*RulesCombatPctFanaticAtkBonus = text_get_number(-100, 1000);
	*RulesCombatPctLandGunVsShipArt = text_get_number(-100, 1000);
	*RulesCombatPctArtBonusLvlAlt = text_get_number(-100, 1000);
	*RulesCombatPctTranceDefVsPsi = text_get_number(-100, 1000);
	*RulesCombatPctEmpSongAtkVsPsi = text_get_number(-100, 1000);
	*RulesCombatPenPctAirSuprVsGrnd = text_get_number(-100, 1000);
	*RulesCombatPctAirSuprVsAir = text_get_number(-100, 1000);
	*RulesCombatPenPctNonCbtDefVsCbt = text_get_number(-100, 1000);
	*RulesCombatPctComJamDefVsMobl = text_get_number(-100, 1000);
	*RulesCombatPctBonusVsShipPort = text_get_number(-100, 1000);
	*RulesCombatPctAAABonusVsAir = text_get_number(-100, 1000);
	*RulesCombatPctDefRangeSensor = text_get_number(-100, 1000);
	*RulesCombatPctPsiAtkBonusPLANET = text_get_number(-100, 1000);
	*RulesRetoolPctPenProdChg = text_get_number(0, 100);
	*RulesRetoolStrictness = text_get_number(0, 3); // Bug fix: increased max param to 3
	*RulesRetoolExemption = text_get_number(0, 1000);
	*RulesMinTurnsCouncils = text_get_number(0, 1000);
	*RulesMineralsHarvestingForest = text_get_number(0, 100);
	*RulesTerritoryMaxDistBase = text_get_number(0, 100);
	*RulesTurnsCornerGblEnergyMrkt = text_get_number(1, 100);
	*RulesTechImproveFungusSqr = tech_item();
	*RulesTechEaseFungusMovement = tech_item();
	*RulesTechBuildRoadsFungus = tech_item();
	*RulesTechTwoSpecAbilities = tech_item();
	*RulesTechThreeNutrientsSqr = tech_item();
	*RulesTechThreeMineralsSqr = tech_item();
	*RulesTechThreeEnergySqr = tech_item();
	*RulesTechOrbInsertSansSpcElev = tech_item();
	*RulesTechMiningPlatformBonus = tech_item();
	*RulesTechEconomicVictory = tech_item();
	*RulesTglProbeCanStealTech = text_get_number(0, 1); // Fix: set min param to 0
	*RulesTglHumanAlwaysContactNet = text_get_number(0, 1); // Fix: set min param to 0
	*RulesTglHumansAlwaysContactPbem = text_get_number(0, 1); // Fix: set min param to 0
	*RulesMaxPctDmgArtVsUnitBaseBnkr = text_get_number(10, 100);
	*RulesMaxPctDmgArtVsUnitsOpen = text_get_number(10, 100);
	*RulesMaxPctDmgArtVsUnitsSea = text_get_number(10, 100);
	text_get();
	*RulesFreqGlobalWarmingNum = range(text_item_number(), 0, 1000);
	*RulesFreqGlobalWarmingDenom = range(text_item_number(), 1, 1000);
	*RulesNormalStartingYear = text_get_number(0, 999999);
	*RulesNormalEndYearLowThreeDiff = text_get_number(0, 999999);
	*RulesNormalEndYearHighThreeDiff = text_get_number(0, 999999);
	*RulesTglOblitBaseAtrocity = text_get_number(0, 1); // Fix: set min param to 0
	*RulesSizeBaseSubspaceGen = text_get_number(1, 999); // SMACX only
	*RulesSubspaceGeneratorsNeeded = text_get_number(1, 999); // SMACX only
	// World Builder
	if (text_open(AlphaxFileID, "WORLDBUILDER")) {
		return 1;
	}
	*WorldBuilderLandBase = text_get_number(50, 4000);
	*WorldBuilderLandMod = text_get_number(0, 2000);
	*WorldBuilderContinentBase = text_get_number(5, 1000);
	*WorldBuilderContinentMod = text_get_number(5, 1000);
	*WorldBuilderHillsBase = text_get_number(0, 100);
	*WorldBuilderHillsMod = text_get_number(0, 100);
	*WorldBuilderPlateauBase = text_get_number(0, 500);
	*WorldBuilderPlateauMod = text_get_number(0, 500);
	*WorldBuilderRiversBase = text_get_number(0, 100);
	*WorldBuilderRiversRainMod = text_get_number(0, 100);
	*WorldBuilderSolarEnergy = text_get_number(1, 64);
	*WorldBuilderThermalBand = text_get_number(1, 64);
	*WorldBuilderThermalDeviance = text_get_number(1, 64);
	*WorldBuilderGlobalWarming = text_get_number(1, 64);
	*WorldBuilderSeaLevelRises = text_get_number(1, 100);
	*WorldBuilderCloudmassPeaks = text_get_number(0, 20);
	*WorldBuilderCloudmassHills = text_get_number(0, 20);
	*WorldBuilderRainfallCoeff = text_get_number(0, 8);
	*WorldBuilderDeepWater = text_get_number(-100, 100);
	*WorldBuilderShelf = text_get_number(-100, 100);
	*WorldBuilderPlains = text_get_number(-100, 100);
	*WorldBuilderBeach = text_get_number(-100, 100);
	*WorldBuilderHills = text_get_number(0, 100);
	*WorldBuilderPeaks = text_get_number(-100, 100);
	*WorldBuilderFungus = text_get_number(0, 5);
	text_get();
	*WorldBuilderContSizeRatio1 = text_item_number();
	*WorldBuilderContSizeRatio2 = text_item_number();
	*WorldBuilderContSizeRatio3 = text_item_number();
	*WorldBuilderContSizeRatio4 = text_item_number();
	*WorldBuilderContSizeRatio5 = text_item_number();
	*WorldBuilderIslands = text_get_number(1, 500);
	return FALSE;
}

/*
Purpose: Parse #TECHNOLOGY section inside alpha(x).txt with dup check
Original Offset: 00585E30
Return Value: Was there an error? TRUE/FALSE
Status: Complete
*/
BOOL __cdecl read_tech() {
	if (text_open(AlphaxFileID, "TECHNOLOGY")) {
		return TRUE;
	}
	for (int i = 0; i < MaxTechnologyNum; i++) {
		text_get();
		text_item();
		text_item();
		strncpy_s(Technology[i].id, 8, *ParseTempPtr2_1, strlen(*ParseTempPtr2_1));
		Technology[i].id[7] = 0;
		for (int j = 0; j < i; j++) {
			if (!strcmp(Technology[i].id, Technology[j].id)) {
				parse_num(0, i);
				parse_num(1, j);
				parse_says(0, Technology[i].id, -1, -1);
				parse_says(1, FilefindPath->lastPath, -1, -1);
				parse_says(2, *ParseTempPtr1_1, -1, -1);
				X_pop("DUPLICATETECH", 0);
			}
		}
	}
	if (text_open(AlphaxFileID, "TECHNOLOGY")) {
		return TRUE;
	}
	*TechValidCount = 0;
	*TechCommerceCount = 0;
	for (int i = 0; i < MaxTechnologyNum; i++) {
		text_get();
		Technology[i].name = text_item_string();
		text_item();
		Technology[i].powerValue = text_item_number();
		Technology[i].techValue = text_item_number();
		Technology[i].wealthValue = text_item_number();
		Technology[i].growthValue = text_item_number();
		Technology[i].preqTech1 = tech_name(text_item());
		Technology[i].preqTech2 = tech_name(text_item());
		Technology[i].flags = text_item_binary();
		if (Technology[i].preqTech1 != DisabledValue
			&& Technology[i].preqTech2 != DisabledValue) {
			*TechValidCount += 1;
			if (Technology[i].flags & INC_COMMERCE) {
				*TechCommerceCount += 1;
			}
		}
	}
	return FALSE;
}

/*
Purpose: Parse #UNITS section inside alpha(x).txt
Original Offset: 00587240
Return Value: Was there an error? TRUE/FALSE
Status: Complete
*/
BOOL __cdecl read_units() {
	if (text_open(AlphaxFileID, "UNITS")) {
		return TRUE;
	}
	int totalUnits = text_get_number(0, MaxVehProtoFactionNum);
	for (int protoID = 0; protoID < totalUnits; protoID++) {
		text_get();
		LPSTR name = text_item();
		strncpy_s(VehPrototype[protoID].vehName, 32, name, strlen(name));
		int chasID = chas_name(text_item());
		int weapID = weap_name(text_item());
		int armorID = arm_name(text_item());
		int plan = text_item_number();
		int cost = text_item_number();
		int carry = text_item_number();
		VehPrototype[protoID].preqTech = tech_name(text_item());
		int icon = text_item_number();
		int ability = text_item_binary();
		int reactorID;
		switch (protoID) 
		{
			// There was a pointless explicit check for BSC_BATTLE_OGRE_MK1 to set reactor to 1
			// The parameters set by check are no different than default
			case BSC_BATTLE_OGRE_MK2:
				reactorID = 2;
				break;
			case BSC_BATTLE_OGRE_MK3:
				reactorID = 3;
				break;
			default:
				reactorID = 1;
				break;
		}
		make_proto(protoID, chasID, weapID, armorID, ability, reactorID);
		// If set, override auto calculated values from make_proto()
		if (plan != -1) { // plan auto calculate: -1
			VehPrototype[protoID].plan = plan;
		}
		if (cost) { // cost auto calculate: 0
			VehPrototype[protoID].cost = cost;
		}
		if (carry) { // carry auto calculate: 0
			VehPrototype[protoID].carryCapacity = carry;
		}
		VehPrototype[protoID].iconOffset = icon;
	}
	return FALSE;
}

/*
Purpose: Clear rule values for Player
Original Offset: 00585FE0
Return Value: n/a
Status: Complete
*/
void __cdecl clear_faction(Player *player) {
	player->ruleTechSelected = 0;
	player->ruleMorale = 0;
	player->ruleResearch = 0;
	player->ruleDrone = 0;
	player->ruleTalent = 0;
	player->ruleEnergy = 0;
	player->ruleInterest = 0;
	player->rulePopulation = 0;
	player->ruleHurry = 100;
	player->ruleTechcost = 100;
	player->rulePsi = 0;
	player->ruleSharetech = 0;
	player->ruleCommerce = 0;
	player->ruleFlags = 0;
	player->factionBonusCount = 0;
}

/*
Purpose: Overload of read faction function passing int value rather than Player structure.
Original Offset: 00586050
Return Value: n/a
Status: Complete
*/
void __cdecl read_faction(int playerID) {
	if (playerID) { // Skips Aliens
		read_faction(&Players[playerID], 0);
		load_faction_art(playerID);
	}
}

/*
Purpose: Parse 1st 8 lines of faction files into Player structure. Toggle parameter will end the 
         function early if set to 2. Original code never uses this.
Original Offset: 00586090
Return Value: n/a
Status: Complete
*/
void __cdecl read_faction(Player *player, int toggle) {
	clear_faction(player);
	if (text_open(player->filename, player->searchKey) 
		&& text_open(player->filename, player->filename)) {
		parse_says(0, player->searchKey, -1, -1);
		parse_says(1, player->filename, -1, -1);
		X_pop("PLAYERFILE", 0);
		return;
	}
	text_get();
	strcpy_s(player->nameFaction, 40, text_item());
	player->nameFaction[39];
	strcpy_s(player->descNameFaction, 24, text_item());
	player->descNameFaction[23] = 0;
	strcpy_s(player->nounFaction, 24, text_item());
	player->nounFaction[23] = 0;
	LPSTR gender = text_item();
	player->nounGender = MALE_GENDER;
	if (gender[0] == 'F' || gender[0] == 'f') {
		player->nounGender = FEMALE_GENDER;
	}
	else if (gender[0] == 'N' || gender[0] == 'n') {
		player->nounGender = NEUTRAL_GENDER;
	}
	player->isNounPlural = range(text_item_number() - 1, FALSE, TRUE); // original value: 1 or 2
	strcpy_s(player->nameLeader, 24, text_item());
	player->nameLeader[23] = 0;
	gender = text_item();
	player->isLeaderFemale = (gender[0] == 'F' || gender[0] == 'f') ? TRUE : FALSE;
	if (toggle == 2) {
		return;
	}
	player->AI_IntrestFight = text_item_number(); // -1, 0, 1
	player->AI_IntrestPower = text_item_number();
	player->AI_IntrestTech = text_item_number();
	player->AI_IntrestWealth = text_item_number();
	player->AI_IntrestGrowth = text_item_number();
	text_get();
	LPSTR parseRuleCheck = text_item();
	int len = strlen(parseRuleCheck);
	while (len) {
		LPSTR parseRule = new CHAR[len + 1];
		strcpy_s(parseRule, len + 1, parseRuleCheck);
		LPSTR parseParameter = text_item();
		if (!_strcmpi(parseRule, BonusName[0].key)) { // TECH
			// will have issues if custom tech abbreviations starting with numbers are used
			int playerSelected = atoi(parseParameter);
			if (!playerSelected && player->factionBonusCount < 8) {
				player->factionBonusID[player->factionBonusCount] = RULE_TECH;
				player->factionBonusVal1[player->factionBonusCount] = tech_name(parseParameter);
				player->factionBonusCount++;
			}
			else {
				player->ruleTechSelected = playerSelected;
			}
		}
		else if (!_strcmpi(parseRule, BonusName[1].key)) { // MORALE
			if (parseParameter[0] == '+') {
				parseParameter++;
			}
			player->ruleMorale = atoi(parseParameter);
			// 0 indicates an exemption from negative modifiers from other sources
			if (!player->ruleMorale) {
				player->ruleFlags |= FLAG_MORALE;
			}
		}
		// FACILITY
		else if (!_strcmpi(parseRule, BonusName[3].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_FACILITY;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		else if (!_strcmpi(parseRule, BonusName[4].key)) { // RESEARCH
			player->ruleResearch = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[5].key)) { // DRONE
			player->ruleDrone = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[6].key)) { // TALENT
			player->ruleTalent = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[7].key)) { // ENERGY
			player->ruleEnergy = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[8].key)) { // INTEREST
			player->ruleFlags |= FLAG_INTEREST;
			player->ruleInterest = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[9].key)) { // COMMERCE
			player->ruleCommerce = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[10].key)) { // POPULATION
			player->rulePopulation = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[11].key)) { // HURRY
			player->ruleHurry = range(atoi(parseParameter), 1, 1000);
		}
		else if (!_strcmpi(parseRule, BonusName[13].key)) { // TECHCOST
			player->ruleTechcost = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[12].key) && player->factionBonusCount < 8) { // UNIT
			player->factionBonusID[player->factionBonusCount] = RULE_UNIT;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		else if (!_strcmpi(parseRule, BonusName[2].key)) { // PSI
			player->rulePsi = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[14].key)) { // SHARETECH
			player->ruleSharetech = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[15].key)) { // TERRAFORM
			player->ruleFlags |= FLAG_TERRAFORM;
		}
		// SOCIAL, ROBUST, IMMUNITY; Moved factionBonusCount check to start rather than inner loop
		else if ((!_strcmpi(parseRule, BonusName[16].key) || !_strcmpi(parseRule, BonusName[17].key) 
			|| !_strcmpi(parseRule, BonusName[18].key)) && player->factionBonusCount < 8) {
			int value = 0;
			while (parseParameter[0] == '+' || parseParameter[0] == '-') {
				(parseParameter[0] == '+') ? value++ : value--;
				parseParameter++;
			}
			if (!value) { // cannot be zero
				value = 1;
			}
			for (int j = 0; j < MaxSocialEffectNum; j++) {
				if (!_strcmpi(SocialEffect[j].set1, parseParameter)) {
					if (!_strcmpi(parseRule, BonusName[17].key)) {
						player->factionBonusID[player->factionBonusCount] = RULE_ROBUST;
					}
					else {
						player->factionBonusID[player->factionBonusCount] =
							!_strcmpi(parseRule, BonusName[16].key) ? RULE_SOCIAL : RULE_IMMUNITY;
					}
					player->factionBonusVal1[player->factionBonusCount] = j; // Social Effect id
					player->factionBonusVal2[player->factionBonusCount] = value; // value modifier
					player->factionBonusCount++;
					break;
				}
			}
		}
		// IMPUNITY, PENALTY; Moved factionBonusCount check to start rather than inner loop
		else if ((!_strcmpi(parseRule, BonusName[19].key) 
			|| !_strcmpi(parseRule, BonusName[20].key)) && player->factionBonusCount < 8) {
			for (int j = 0; j < MaxSocialCatNum; j++) {
				for (int k = 0; k < MaxSocialModelNum; k++) {
					if (!_stricmp(parseParameter, 
						StringTable->get((int)SocialCategory[j].name[k]))) {
						player->factionBonusID[player->factionBonusCount] = 
							!_strcmpi(parseRule, BonusName[19].key) ? RULE_IMPUNITY : RULE_PENALTY;
						player->factionBonusVal1[player->factionBonusCount] = j; // category id
						player->factionBonusVal2[player->factionBonusCount] = k; // model id
						player->factionBonusCount++;
					}
				}
			}
		}
		// FUNGNUTRIENT
		else if (!_strcmpi(parseRule, BonusName[21].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_FUNGNUTRIENT;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		// FUNGMINERALS
		else if (!_strcmpi(parseRule, BonusName[22].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_FUNGMINERALS;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		// FUNGENERGY
		else if (!_strcmpi(parseRule, BonusName[23].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_FUNGENERGY;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		else if (!_strcmpi(parseRule, BonusName[24].key)) { // COMMFREQ
			player->ruleFlags |= FLAG_COMMFREQ;
		}
		else if (!_strcmpi(parseRule, BonusName[25].key)) { // MINDCONTROL
			player->ruleFlags |= FLAG_MINDCONTROL;
		}
		else if (!_strcmpi(parseRule, BonusName[26].key)) { // FANATIC
			player->ruleFlags |= FLAG_FANATIC;
		}
		// VOTES
		else if (!_strcmpi(parseRule, BonusName[27].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_VOTES;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		else if (!_strcmpi(parseRule, BonusName[28].key)) { // FREEPROTO
			player->ruleFlags |= FLAG_FREEPROTO;
		}
		else if (!_strcmpi(parseRule, BonusName[29].key)) { // AQUATIC
			player->ruleFlags |= FLAG_AQUATIC;
		}
		else if (!_strcmpi(parseRule, BonusName[30].key)) { // ALIEN
			player->ruleFlags |= FLAG_ALIEN;
		}
		// FREEFAC
		else if (!_strcmpi(parseRule, BonusName[31].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_FREEFAC;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		// REVOLT
		else if (!_strcmpi(parseRule, BonusName[32].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_REVOLT;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		// NODRONE
		else if (!_strcmpi(parseRule, BonusName[33].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_NODRONE;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		else if (!_strcmpi(parseRule, BonusName[34].key)) { // WORMPOLICE
			player->ruleFlags |= FLAG_WORMPOLICE;
		}
		// FREEABIL
		else if (!_strcmpi(parseRule, BonusName[35].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_FREEABIL;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		// PROBECOST
		else if (!_strcmpi(parseRule, BonusName[36].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_PROBECOST;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		// DEFENSE
		else if (!_strcmpi(parseRule, BonusName[37].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_DEFENSE;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		// OFFENSE
		else if (!_strcmpi(parseRule, BonusName[38].key) && player->factionBonusCount < 8) {
			player->factionBonusID[player->factionBonusCount] = RULE_OFFSENSE;
			player->factionBonusVal1[player->factionBonusCount] = atoi(parseParameter);
			player->factionBonusCount++;
		}
		else if (!_strcmpi(parseRule, BonusName[39].key)) { // TECHSHARE
			player->ruleFlags |= FLAG_TECHSHARE;
			player->ruleSharetech = atoi(parseParameter);
		}
		else if (!_strcmpi(parseRule, BonusName[40].key)) { // TECHSTEAL
			player->ruleFlags |= FLAG_TECHSTEAL;
		}
		parseRuleCheck = text_item();
		len = strlen(parseRuleCheck);
		delete parseRule;
	}
	// Societal Ideology + Anti-Ideology
	for (int i = 0; i < 2; i++) {
		*(&player->socIdeologyCategory + i) = -1;
		*(&player->socIdeologyModel + i) = 0;
		*(&player->socIdeologyEffect + i) = -1;
		text_get();
		LPSTR socCategory = text_item();
		for (int j = 0; j < MaxSocialCatNum; j++) {
			LPSTR checkCatType = StringTable->get((int)SocialCategory[j].type);
			if (*Language ? 
				!_strnicmp(socCategory, checkCatType, 4) : !_stricmp(socCategory, checkCatType)) {
				*(&player->socIdeologyCategory + i) = j;
				break;
			}
		}
		LPSTR socModel = text_item();
		int socCatNum = *(&player->socIdeologyCategory + i);
		if (socCatNum >= 0) {
			for (int j = 0; j < MaxSocialModelNum; j++) {
				LPSTR checkModel = StringTable->get((int)SocialCategory[socCatNum].name[j]);
				if (*Language ? 
					!_strnicmp(socModel, checkModel, 4) : !_stricmp(socModel, checkModel)) {
					*(&player->socIdeologyModel + i) = j;
					break;
				}
			}
		}
		LPSTR socEffect = text_item();
		for (int j = 0; j < MaxSocialEffectNum; j++) {
			if (!_strcmpi(SocialEffect[j].set1, socEffect)) {
				// Bug fix: Original code sets this value to -1, disabling AI faction "Emphasis" 
				// value. No indication this was intentional.
				*(&player->socIdeologyEffect + i) = j;
				break;
			}
		}
	}
	// Faction and Leader related strings
	text_get(); // skips 2nd value in this line, abbreviation unused?
	strcpy_s(player->nameAdjFaction, 128, text_item()); 
	player->nameAdjFaction[127] = 0;
	text_get();
	strcpy_s(player->assistantName, 24, text_item());
	player->assistantName[23] = 0;
	strcpy_s(player->scientistName, 24, text_item());
	player->scientistName[23] = 0;
	strcpy_s(player->assistantCity, 24, text_item());
	player->assistantCity[23] = 0;
	text_get();
	strcpy_s(player->titleLeader, 24, text_item());
	player->titleLeader[23] = 0;
	strcpy_s(player->adjLeader, 128, text_item());
	player->adjLeader[127] = 0;
	strcpy_s(player->adjInsultLeader, 128, text_item());
	player->adjInsultLeader[127] = 0;
	strcpy_s(player->adjFaction, 128, text_item());
	player->adjFaction[127] = 0;
	strcpy_s(player->adjInsultFaction, 128, text_item());
	player->adjInsultFaction[127] = 0;
	text_get();
	strcpy_s(player->insultLeader, 24, text_item());
	player->insultLeader[23] = 0;
}

/*
Purpose: Read faction filenames and search keys from ini file (SMACX only). Has added effect of 
         forcing Player searchKey to be set to filename value. Rewrote almost the entire function 
		 because of how terrible the original code logic was.
Original Offset: 0059DBD0
Return Value: n/a
Status: Complete
*/
void __cdecl prefs_fac_load() {
	if (SMACX_Enabled) {
		CHAR returnedString[256];
		GetPrivateProfileStringA("Alpha Centauri", "Prefs Format", "0", 
			returnedString, 256, ".\\Alpha Centauri.ini");
		if (atoi(returnedString) == 12) {
			for (int i = 1; i < MaxPlayerNum; i++) {
				std::string faction = "Faction ";
				faction += std::to_string(i);
				GetPrivateProfileStringA("Alpha Centauri", faction.c_str(), Players[i].filename, 
					returnedString, 256, ".\\Alpha Centauri.ini");
				strncpy_s(Players[i].filename, returnedString, 24);
				strncpy_s(Players[i].searchKey, returnedString, 24);
			}
		}
		else {
			// use separate loop rather than check "Prefs Format" value each time in single loop
			for (int i = 1; i < MaxPlayerNum; i++) {
				strncpy_s(Players[i].searchKey, Players[i].filename, 24);
			}
		}
	}
}

/*
Purpose: Parse #BONUSNAMES, #FACTIONS, #NEWFACTIONS sections inside alpha(x).txt
Original Offset: 00586F30
Return Value: Was there an error? TRUE/FALSE
Status: Complete
*/
BOOL __cdecl read_factions() {
	if (text_open(AlphaxFileID, "BONUSNAMES")) {
		return TRUE;
	}
	for (unsigned int i = 0; i < MaxBonusNameNum; i++) {
		if (!(i % 8)) { // 8 entries per line
			text_get();
		}
		strncpy_s(BonusName[i].key, text_item(), 24);
	}
	if (text_open(AlphaxFileID, SMACX_Enabled ? "NEWFACTIONS" : "FACTIONS")) {
		return TRUE;
	}
	for (int i = 1; i < MaxPlayerNum; i++) {
		text_get();
		strncpy_s(Players[i].filename, text_item(), 24);
		strncpy_s(Players[i].searchKey, text_item(), 24);
	}
	// SMACX only: Will override any values parsed from alphax.txt #NEWFACTIONS if set in ini; 
	// Removed SMACX_Enabled check since there is one inside function already.
	prefs_fac_load();
	// Removed debug code related to non-existent faction JENN282; Most of code never executed in 
	// vanilla. Nothing worthwhile to include, incomplete buggy code. Only present in SMACX.
	for (int i = 1; i < MaxPlayerNum; i++) {
		// skip Players[0], removed check (&Players[i] != &Players[0]) since i is already set to 1
		read_faction(&Players[i], 0);
		load_faction_art(i);
	}
	return FALSE;
}

/*
Purpose: Parse in all the game rules via alpha/x.txt. If param is set to TRUE, parse 
         #UNITS & #FACTIONS. Otherwise, skip both. New game vs reload?
Original Offset: 005873C0
Return Value: Was there an error? TRUE/FALSE
Status: Complete
*/
BOOL __cdecl read_rules(BOOL tglAllRules) {
	StringTable->init(49952);
	if (labels_init()) {
		return TRUE;
	}
	text_clear_index();
	text_make_index("SCRIPT");
	text_make_index(AlphaxFileID);
	// Technology; Rules; Terrain
	if (read_tech() || read_basic_rules() || text_open(AlphaxFileID, "TERRAIN")) {
		return TRUE;
	}
	for (int i = 0; i < MaxTerrainNum; i++) {
		text_get();
		// land + sea terraforming
		for (int j = 0; j < 2; j++) {
			*(&Terraforming[i].name + j) = text_item_string();
			*(&Terraforming[i].preqTech + j) = tech_name(text_item());
			// add in flags vs hard coded const struct
			*(&Terraforming[i].flag + j) = flagsTerraforming[i * 2 + j];
		}
		Terraforming[i].rate = text_item_number();
		// land + sea orders
		LPSTR orderStr = text_item();
		for (int j = 0; j < 2; j++) {
			parse_say(0, (int)*(&Terraforming[i].name + j), -1, -1);
			stringTemp->str[0] = 0;
			parse_string(orderStr, stringTemp->str);
			*(&Order[i + 4].order + j) = (LPSTR)StringTable->put(stringTemp->str);
		}
		Order[i + 4].letter = text_item_string();
		Terraforming[i].shortcuts = text_item_string();	
	}
	// Resource Info
	if (text_open(AlphaxFileID, "RESOURCEINFO")) {
		return TRUE;
	}
	for (int i = 0; i < MaxResourceInfoNum; i++) {
		text_get();
		text_item();
		ResourceInfo[i].nutrients = text_item_number();
		ResourceInfo[i].minerals = text_item_number();
		ResourceInfo[i].energy = text_item_number();
		ResourceInfo[i].unused = text_item_number();
	}
	// Time Controls
	if (text_open(AlphaxFileID, "TIMECONTROLS")) {
		return TRUE;
	}
	for (int i = 0; i < MaxTimeControlNum; i++) {
		text_get();
		TimeControl[i].name = text_item_string();
		TimeControl[i].turn = text_item_number();
		TimeControl[i].base = text_item_number();
		TimeControl[i].unit = text_item_number();
		TimeControl[i].event = text_item_number();
		TimeControl[i].extra = text_item_number();
		TimeControl[i].refresh = text_item_number();
		TimeControl[i].accumulated = text_item_number();
	}
	// Chassis
	if (text_open(AlphaxFileID, "CHASSIS")) {
		return TRUE;
	}
	for (int i = 0; i < MaxChassisNum; i++) {
		text_get();
		// offsv1
		Chassis[i].offsv1Name = text_item_string();
		noun_item(&Chassis[i].offsv1Gender, &Chassis[i].offsv1IsPlural);
		// offsv2
		Chassis[i].offsv2Name = text_item_string();
		noun_item(&Chassis[i].offsv2Gender, &Chassis[i].offsv2IsPlural);
		// defsv1
		Chassis[i].defsv1Name = text_item_string();
		noun_item(&Chassis[i].defsv1Gender, &Chassis[i].defsv1IsPlural);
		// defsv2
		Chassis[i].defsv2Name = text_item_string();
		noun_item(&Chassis[i].defsv2Gender, &Chassis[i].defsv2IsPlural);
		Chassis[i].speed = text_item_number();
		Chassis[i].triad = text_item_number();
		Chassis[i].range = text_item_number();
		Chassis[i].missile = text_item_number();
		Chassis[i].cargo = text_item_number();
		Chassis[i].cost = text_item_number();
		Chassis[i].preqTech = tech_name(text_item());
		// offsv_lrg
		Chassis[i].offsvNameLrg = text_item_string();
		noun_item(&Chassis[i].offsvGenderLrg, &Chassis[i].offsvIsPluralLrg);
		// defsv_lrg
		Chassis[i].defsvNameLrg = text_item_string();
		noun_item(&Chassis[i].defsvGenderLrg, &Chassis[i].defsvIsPluralLrg);
	}
	// Reactors
	if (text_open(AlphaxFileID, "REACTORS")) {
		return TRUE;
	}
	for (int i = 0; i < MaxReactorNum; i++) {
		text_get();
		Reactor[i].name = text_item_string();
		Reactor[i].nameShort = text_item_string();
		// Bug fix/Enhancement: original function skips this value and is left as zero, isn't 
		// referenced elsewhere in code. Likely because default power value is sequential. 
		// This will allow future modifications.
		Reactor[i].power = text_item_number();
		Reactor[i].preqTech = tech_name(text_item());
	}
	// Weapons
	if (text_open(AlphaxFileID, "WEAPONS")) {
		return TRUE;
	}
	for (int i = 0; i < MaxWeaponNum; i++) {
		text_get();
		Weapon[i].name = text_item_string();
		Weapon[i].nameShort = text_item_string();
		Weapon[i].offenseRating = text_item_number();
		Weapon[i].mode = text_item_number();
		Weapon[i].cost = text_item_number();
		Weapon[i].icon = text_item_number();
		Weapon[i].preqTech = tech_name(text_item());
	}
	// Defenses / Armor
	if (text_open(AlphaxFileID, "DEFENSES")) {
		return TRUE;
	}
	for (int i = 0; i < MaxArmorNum; i++) {
		text_get();
		Armor[i].name = text_item_string();
		Armor[i].nameShort = text_item_string();
		Armor[i].defenseRating = text_item_number();
		Armor[i].mode = text_item_number();
		Armor[i].cost = text_item_number();
		Armor[i].preqTech = tech_name(text_item());
	}
	// Abilities
	if (text_open(AlphaxFileID, "ABILITIES")) {
		return TRUE;
	}
	for (int i = 0; i < MaxAbilityNum; i++) {
		text_get();
		Ability[i].name = text_item_string();
		Ability[i].costFactor = text_item_number();
		Ability[i].preqTech = tech_name(text_item());
		Ability[i].abbreviation = text_item_string();
		Ability[i].flags = text_item_binary();
		Ability[i].description = text_item_string();
	}
	// Morale
	if (text_open(AlphaxFileID, "MORALE")) {
		return TRUE;
	}
	for (int i = 0; i < MaxMoraleNum; i++) {
		text_get();
		Morale[i].name = text_item_string();
		Morale[i].nameNative = text_item_string();
	}
	// Defense Modes
	if (text_open(AlphaxFileID, "DEFENSEMODES")) {
		return TRUE;
	}
	for (int i = 0; i < MaxDefenseModeNum; i++) {
		text_get();
		DefenseModes[i].name = text_item_string();
		DefenseModes[i].hyphened = text_item_string();
		DefenseModes[i].abbrev = text_item_string();
		DefenseModes[i].letter = text_item_string();
	}
	// Offense Modes
	if (text_open(AlphaxFileID, "OFFENSEMODES")) {
		return TRUE;
	}
	for (int i = 0; i < MaxOffenseModeNum; i++) {
		text_get();
		OffenseModes[i].name = text_item_string();
		OffenseModes[i].hyphened = text_item_string();
		OffenseModes[i].abbrev = text_item_string();
		OffenseModes[i].letter = text_item_string();
	}
	// Units basic prototypes (only if new game param1 boolean is set)
	// Potential bug: Look into issues with VEH data persisting between loaded saved games?
	if (tglAllRules) {
		for (int i = 0; i < MaxVehProtoNum; i++) {
			VehPrototype[i].vehName[0] = 0;
			VehPrototype[i].unk1 = 0;
			VehPrototype[i].unk4 = 0;
		}
		if (read_units()) {
			return TRUE;
		}
	}
	// Facilities
	if (text_open(AlphaxFileID, "FACILITIES")) {
		return TRUE;
	}
	for (int i = 1; i < MaxFacilityNum; i++) { // Facility[0] is null
		text_get();
		Facility[i].name = text_item_string();
		Facility[i].cost = text_item_number();
		Facility[i].maint = text_item_number();
		Facility[i].preqTech = tech_name(text_item());
		/*
		Enhancement: The original code explicitly sets this value to disabled (-2) overriding 
		alpha/x.txt.  It states in #FACILITIES alpha/x.txt: "Free  = No longer supported". The 
		original AC manual in Appendix 2 and official strategy guide both list the specific 
		facilities being free with certain tech.  However, this mechanic could have been removed 
		for balance reasons. Or maybe was dropped due to time constraints. There is code that 
		checks this value and sets the free facility only for new bases built after discovering the 
		tech. It looks like existing bases do not get it. Will have to review this more. For now, 
		this mechanic will be included as is. You can revert to vanilla behavior by modifying the 
		four entries below in alpha/x.txt #FACILITIES with free parameter set to Disabled.
		
		Recycling Tanks, 4, 0, Biogen, EcoEng2,   > free with "Adv.Ecological Engineering"
		Recreation Commons, 4, 1, Psych, SentEco, > free with "Sentient Econometrics"
		Energy Bank, 8, 1, IndEcon, QuanMac,      > free with "Quantum Machinery"
		Network Node, 8, 1, InfNet, HAL9000,      > free with "Self - Aware Machines"
		*/
		Facility[i].freeTech = tech_name(text_item());
		Facility[i].effect = text_item_string();
		if (i >= FacilitySPStart) {
			Facility[i].SP_AIFight = text_item_number();
			Facility[i].SP_AIPower = text_item_number();
			Facility[i].SP_AITech = text_item_number();
			Facility[i].SP_AIWealth = text_item_number();
			Facility[i].SP_AIGrowth = text_item_number();
		}
	}
	// Orders (basic)
	if (text_open(AlphaxFileID, "ORDERS")) {
		return TRUE;
	}
	for (int i = 0; i < MaxOrderNum; i++) {
		if (i < 4 || i > 23) { // Skipping over orders set by #TERRAIN
			text_get();
			Order[i].order = text_item_string();
			// Potential enhancement: Have separate string for sea
			Order[i].orderSea = Order[i].order;
			Order[i].letter = text_item_string();
		}
	}
	// Compass
	if (text_open(AlphaxFileID, "COMPASS")) {
		return TRUE;
	}
	for (int i = 0; i < MaxCompassNum; i++) {
		text_get();
		Compass[i] = text_item_string();
	}
	// Plans
	if (text_open(AlphaxFileID, "PLANS")) {
		return TRUE;
	}
	for (int i = 0; i < MaxPlanNum; i++) {
		text_get();
		// Future clean-up: Create structure with both short & full name vs split memory
		PlansShortName[i] = text_item_string();
		PlansFullName[i] = text_item_string();
	}
	// Triad
	if (text_open(AlphaxFileID, "TRIAD")) {
		return TRUE;
	}
	for (int i = 0; i < MaxTriadNum; i++) {
		text_get();
		Triad[i] = text_item_string();
	}
	// Resources
	if (text_open(AlphaxFileID, "RESOURCES")) {
		return TRUE;
	}
	for (int i = 0; i < MaxResourceNum; i++) {
		text_get();
		Resource[i].nameSingular = text_item_string();
		Resource[i].namePlural = text_item_string();
	}
	// Energy
	if (text_open(AlphaxFileID, "ENERGY")) {
		return TRUE;
	}
	for (int i = 0; i < MaxEnergyNum; i++) {
		text_get();
		Energy[i].abbrev = text_item_string();
		Energy[i].name = text_item_string();
	}
	// Citizens
	if (text_open(AlphaxFileID, "CITIZENS")) {
		return TRUE;
	}
	for (int i = 0; i < MaxCitizenNum; i++) {
		text_get();
		Citizen[i].singularName = text_item_string();
		Citizen[i].pluralName = text_item_string();
		if (i < 7) {
			Citizen[i].preqTech = tech_name(text_item());
			Citizen[i].obsolTech = tech_name(text_item());
			Citizen[i].opsBonus = text_item_number();
			Citizen[i].psychBonus = text_item_number();
			Citizen[i].researchBonus = text_item_number();
		}
	}
	// Socio
	if (text_open(AlphaxFileID, "SOCIO")) {
		return TRUE;
	}
	text_get();
	for (int i = 0; i < MaxSocialEffectNum; i++) {
		strcpy_s(SocialEffect[i].set1, 24, text_item());
	}
	text_get();
	for (int i = 0; i < MaxSocialEffectNum; i++) {
		strcpy_s(SocialEffect[i].set2, 24, text_item());
	}
	text_get();
	for (int i = 0; i < MaxSocialCatNum; i++) {
		SocialCategory[i].type = text_item_string();
	}
	for (int i = 0; i < MaxSocialCatNum; i++) {
		for (int j = 0; j < MaxSocialModelNum; j++) {
			text_get();
			SocialCategory[i].name[j] = text_item_string();
			SocialCategory[i].preqTech[j] = tech_name(text_item());
			memset(&SocialCategory[i].effect[j], 0, sizeof(rules_social_category_effect));
			LPSTR modValue = text_item();
			int modLen = strlen(modValue);
			while (modLen) {
				int value = 0;
				while (modValue[0] == '+' || modValue[0] == '-') {
					(modValue[0] == '+') ? value++ : value--;
					modValue++;
				}
				for (int k = 0; k < MaxSocialEffectNum; k++) {
					if (!_strcmpi(modValue, SocialEffect[k].set1)) {
						*(&SocialCategory[i].effect[j].economy + k) = value;
						break;
					}
				}
				modValue = text_item();
				modLen = strlen(modValue);
			}
		}
	}
	// Difficulty
	if (text_open(AlphaxFileID, "DIFF")) {
		return TRUE;
	}
	for (int i = 0; i < MaxDiffNum; i++) {
		text_get();
		Difficulty[i] = text_item_string();
	}
	// Factions
	if (tglAllRules && read_factions()) {
		return TRUE;
	}
	// Mandate
	if (text_open(AlphaxFileID, "MANDATE")) {
		return TRUE;
	}
	for (int i = 0; i < MaxMandateNum; i++) {
		text_get();
		Mandate[i].name = text_item_string();
		Mandate[i].nameCAPS = text_item_string();
	}
	// Mood
	if (text_open(AlphaxFileID, "MOOD")) {
		return TRUE;
	}
	for (int i = 0; i < MaxMoodNum; i++) {
		text_get();
		Mood[i] = text_item_string();
	}
	// Repute
	if (text_open(AlphaxFileID, "REPUTE")) {
		return TRUE;
	}
	for (int i = 0; i < MaxReputeNum; i++) {
		text_get();
		Repute[i] = text_item_string();
	}
	// Might
	if (text_open(AlphaxFileID, "MIGHT")) {
		return TRUE;
	}
	for (int i = 0; i < MaxMightNum; i++) {
		text_get();
		Might[i].adjStart = text_item_string();
		Might[i].adj = text_item_string();
	}
	// Proposals
	if (text_open(AlphaxFileID, "PROPOSALS")) {
		return TRUE;
	}
	for (int i = 0; i < MaxProposalNum; i++) {
		text_get();
		Proposal[i].name = text_item_string();
		Proposal[i].preqTech = tech_name(text_item());
		Proposal[i].description = text_item_string();
	}
	// Natural
	if (text_open(AlphaxFileID, "NATURAL")) {
		return TRUE;
	}
	for (int i = 0; i < MaxNaturalNum; i++) {
		text_get();
		Natural[i].name = text_item_string();
		Natural[i].nameShort = text_item_string();
	}
	// Revised original nested for loop code to be more efficient; Logic is still same. 
	// Buttons used by "Edit Map" menus.
	for (int i = 0, j = 0; i < MaxTerrainNum; i++) {
		// excludes: Fungus (removal), Aquifer, Raise Land, Lower Land, Level Terrain
		if (Terraforming[i].flag) {
			MainInterfaceVar->flatButton[j++ + 17].set_bubble_text(
				StringTable->get((int)Terraforming[i].name)); // 17-31
		}
	}
	// Jungle
	MainInterfaceVar->flatButton[32].set_bubble_text(StringTable->get((int)Natural[2].nameShort));
	// Dunes
	MainInterfaceVar->flatButton[33].set_bubble_text(StringTable->get((int)Natural[6].nameShort));
	// Uranium
	MainInterfaceVar->flatButton[34].set_bubble_text(StringTable->get((int)Natural[3].nameShort));
	// Geothermal
	MainInterfaceVar->flatButton[35].set_bubble_text(StringTable->get((int)Natural[10].nameShort));
	return FALSE;
}
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
#include "base.h"
#include "alpha.h"
#include "faction.h" // PlayerData
#include "game.h" // Difficulty level
#include "map.h" // x_dist(), region_at()
#include "strings.h"
#include "text.h"
#include "technology.h" // has_tech()
#include "veh.h" // best_reactor()

rules_facility *Facility = (rules_facility *)0x009A4B68;
rules_citizen *Citizen = (rules_citizen *)0x00946020;
base *Base = (base *)0x0097D040; // 512
base_secret_project *SecretProject = (base_secret_project *)0x009A6514; // 64
int *BaseIDCurrentSelected = (int *)0x00689370;
int *BaseCurrentCount = (int *)0x009A64CC;
int *BaseFindDist = (int *)0x0090EA04;
base **BaseCurrent = (base **)0x0090EA30;
uint32_t *ScnVictFacilityObj = (uint32_t *)0x009A6814;

/*
Purpose: Check if the base already has a particular facility built or if it's in the queue.
Original Offset: 00421670
Return Value: Does base already have or planning on building facility? true/false
Status: Complete
*/
BOOL __cdecl has_fac(int facilityID, int baseID, int queueCount) {
	if (facilityID >= FAC_SKY_HYDRO_LAB) {
		return false;
	}
	BOOL isBuilt = has_fac_built(facilityID, baseID);
	if (isBuilt || !queueCount) {
		return isBuilt;
	}
	if (queueCount <= 0 || queueCount > 10) { // added upper bounds check for queueCount
		return false;
	}
	for (int i = 0; i < queueCount; i++) {
		if (Base[baseID].queueProductionID[i] == -facilityID) {
			return true;
		}
	}
	return false;
}

/*
Purpose: Set current base globals.
Original Offset: 004E39D0
Return Value: n/a
Status: Complete
*/
void __cdecl set_base(int baseID) {
	*BaseIDCurrentSelected = baseID;
	*BaseCurrent = &Base[baseID];
}

/*
Purpose: Get base name string from baseID and store it in strBase. If baseID is -1, use 'NONE'.
Original Offset: 004E3A00
Return Value: n/a
Status: Complete
*/
void __cdecl say_base(LPSTR strBase, int baseID) {
	std::string output = (baseID >= 0) ? Base[baseID].nameString : label_get(25); // 'NONE'
	// assumes 1032 char buffer via stringTemp, eventually remove
	strcat_s(strBase, 1032, output.c_str());
}

/*
Purpose: Find baseID nearest to coordinates.
Original Offset: 004E3B80
Return Value: -1 if not found, otherwise baseID
Status: Complete
*/
int __cdecl base_find(int xCoord, int yCoord) {
	if (*BaseCurrentCount <= 0) {
		return -1;
	}
	int proximity = 9999, baseID = -1;
	for (int i = 0; i < *BaseCurrentCount; i++) {
		// removed redundant abs() around yCoord difference
		int dist = x_dist(cursor_dist(xCoord, Base[i].xCoord), yCoord - Base[i].yCoord);
		if (dist <= proximity) {
			proximity = dist;
			baseID = i;
		}
	}
	if (baseID >= 0) {
		*BaseFindDist = proximity;
	}
	return baseID;
}

/*
Purpose: Find baseID nearest to coordinates owned by faction.
Original Offset: 004E3C60
Return Value: -1 if not found, otherwise baseID
Status: Complete
*/
int __cdecl base_find(int xCoord, int yCoord, uint32_t factionID) {
	if (*BaseCurrentCount <= 0) {
		return -1;
	}
	int proximity = 9999, baseID = -1;
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (Base[i].factionIDCurrent == factionID) {
			// removed redundant abs() around yCoord difference
			int dist = x_dist(cursor_dist(xCoord, Base[i].xCoord), yCoord - Base[i].yCoord);
			if (dist <= proximity) {
				proximity = dist;
				baseID = i;
			}
		}
	}
	if (baseID >= 0) {
		*BaseFindDist = proximity;
	}
	return baseID;
}

/*
Purpose: Find baseID nearest to coordinates meeting various conditional checks.
Original Offset: 004E3D50
Return Value: -1 if not found, otherwise baseID
Status: Complete
*/
int __cdecl base_find(int xCoord, int yCoord, int factionID, int region, int factionID2, 
	int factionID3) {
	int proximity = 9999, baseID = -1;
	*BaseFindDist = 9999; // difference from other two functions where this is reset at start
	if (*BaseCurrentCount <= 0) {
		return -1;
	}
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (region < 0 || region_at(Base[i].xCoord, Base[i].yCoord) == (uint32_t)region) {
			if (factionID < 0 ? (factionID2 < 0 || Base[i].factionIDCurrent != factionID2) 
				: (factionID == Base[i].factionIDCurrent || (factionID2 == -2 
					? PlayersData[factionID].diploStatus[Base[i].factionIDCurrent] & DSTATUS_PACT 
					: (factionID2 >= 0 && factionID2 == Base[i].factionIDCurrent)))) {
				if (factionID3 < 0 || Base[i].factionIDCurrent == factionID3 
					|| ((1 << factionID3) & Base[i].unk2)) {
					// removed redundant abs() around yCoord difference
					int dist = x_dist(cursor_dist(xCoord, Base[i].xCoord), yCoord - Base[i].yCoord);
					if (dist <= proximity) {
						proximity = dist;
						baseID = i;
					}
				}
			}
		}
	}
	if (baseID >= 0) {
		*BaseFindDist = proximity;
	}
	return baseID;
}

/*
Purpose: Find the best specialist available to the current base with more weight placed on psych.
Original Offset: 004E4020
Return Value: Best citizen id (always going to be 1, 4, or 6 based on default weights)
Status: Complete
*/
uint32_t  __cdecl best_specialist() {
	int currentBonus = -999;
	uint32_t citizenID = 0;
	for (int i = 0; i < MaxSpecialistNum; i++) {
		if (has_tech(Citizen[i].preqTech, (*BaseCurrent)->factionIDCurrent)) {
			uint32_t bonus = Citizen[i].psychBonus * 3;
			if ((*BaseCurrent)->populationSize >= Rules->MinBaseSizeSpecialists) {
				bonus += Citizen[i].opsBonus + Citizen[i].researchBonus;
			}
			if ((int)bonus > currentBonus) {
				currentBonus = bonus;
				citizenID = i;
			}
		}
	}
	return citizenID;
}

/*
Purpose: Generate a base name. Added some minor randomization for sea bases and ensuring offset gets 
         increased for default base name. TODO: Eventually improve base name shuffling.
Original Offset: 004E4090
Return Value: n/a
Status: Complete
*/
void __cdecl name_base(int factionID, LPSTR nameOut, BOOL isFinal, BOOL isSeaBase) {
	if (isSeaBase && !text_open(Players[factionID].filename, "WATERBASES")) {
		uint32_t offsetSea = PlayersData[factionID].baseSeaNameOffset + 1;
		if (offsetSea > 1) {
			uint32_t total;
			for (total = 0; ; total++) {
				text_get();
				if (!strlen(*TextBufferGetPtr) || !_strnicmp(*TextBufferGetPtr, "#END", 4)) {
					break;
				}
			}
			if (offsetSea <= total) {
				int seed = ((*MapRandSeed + factionID) & 0xFE) | 1;
				uint32_t loop = 1;
				do {
					if (seed & 1) {
						seed ^= 0x170;
					}
					seed >>= 1;
				} while (seed >= (int)total || ++loop != offsetSea);
				offsetSea = seed + 1;
			}
		}
		if (!text_open(Players[factionID].filename, "WATERBASES")) {
			uint32_t count;
			for (count = 0; count < offsetSea; count++) {
				text_get();
				if (!strlen(*TextBufferGetPtr) || !_strnicmp(*TextBufferGetPtr, "#END", 4)) {
					break;
				}
			}
			if (count == offsetSea) {
				// water base name available
				strncpy_s(nameOut, 25, text_item(), 24);
				nameOut[23] = 0;
				if (isFinal) {
					PlayersData[factionID].baseSeaNameOffset++;
				}
				text_close();
				return;
			}
		}
		text_close();
	}
	// Land base names or generic
	uint32_t offset = PlayersData[factionID].baseNameOffset + 1;
	sprintf_s(nameOut, 25, "%s %d", get_noun(factionID), offset); // default if names exhausted
	if (isFinal) {
		PlayersData[factionID].baseNameOffset++;
	}
	if (!text_open(Players[factionID].filename, "BASES")) {
		if (offset > 1) {
			uint32_t total;
			for (total = 0; ; total++) {
				text_get();
				if (!strlen(*TextBufferGetPtr) || !_strnicmp(*TextBufferGetPtr, "#END", 4)) {
					break;
				}
			}
			if (offset <= total) {
				int seed = ((*MapRandSeed + factionID) & 0xFE) | 1;
				uint32_t loop = 1;
				do {
					if (seed & 1) {
						seed ^= 0x170;
					}
					seed >>= 1;
				} while (seed >= (int)total || ++loop != offset);
				offset = seed + 1;
			}
		}
		if (!text_open(Players[factionID].filename, "BASES")) {
			uint32_t count;
			for (count = 0; count < offset; count++) {
				text_get();
				if (!strlen(*TextBufferGetPtr) || !_strnicmp(*TextBufferGetPtr, "#END", 4)) {
					break;
				}
			}
			if (count != offset) {
				if (text_open("BASENAME", "GENERIC")) {
					return;
				}
				while (count < offset) {
					text_get();
					if (!strlen(*TextBufferGetPtr) || !_strnicmp(*TextBufferGetPtr, "#END", 4)) {
						return;
					}
					count++;
				}
			}
			strncpy_s(nameOut, 25, text_item(), 24);
			nameOut[23] = 0;
		}
		text_close();
	}
}

/*
Purpose: Used to determine if base has any restrictions around production item retooling.
Original Offset: 004E4700
Return Value: Fixed value (-1, 0, 1, 2, 3, -70) or productionID
Status: Complete
*/
int __cdecl base_making(int productionID, int baseID) {
	uint32_t retool = Rules->RetoolStrictness;
	int sknOff = facility_offset("Skunkworks");
	if ((has_fac_built(FAC_SKUNKWORKS, baseID) // has Skunkworks
		|| (Players[Base[baseID].factionIDCurrent].ruleFlags & FLAG_FREEPROTO // bug fix
			&& sknOff >= 0 && has_tech(Facility[sknOff].preqTech, Base[baseID].factionIDCurrent)))
		&& retool >= 1) { // don't override if retool strictness is already set to always free (0)
		retool = 1; // Skunkworks or FREEPROTO + prerequisite tech > 'Free in Category'
	}
	if (productionID < 0) { // facility or SP to build
		int queueID = Base[baseID].queueProductionID[0]; // current production item
		if (queueID < 0) { // non-Veh
			queueID = -queueID;
			if (queueID < FAC_SKY_HYDRO_LAB && has_fac_built(queueID, baseID)) {
				return -1; // facility completed outside normal process, no retool penalty to change
			}
		}
	}
	switch (retool) { // converted into switch to improve readability and performance
		case 0: // Always Free
			return 0;
		case 1: // Free in Category
			if (productionID >= 0) {
				return 0; // Veh
			}
			// SP (1), repeatable facility (2), non-repeatable facility (3)
			return (productionID > -70) ? (productionID > -65) + 2 : 1;
		case 2: // Free switching between SPs (default behavior)
			return (productionID <= -70) ? -70 : productionID;
		case 3: // Never Free
			return productionID;
		default:
			return 1; // should never be reached unless problem with retool value
	}
}

/*
Purpose: Calculate mineral loss if production is changed at base. The 2nd parameter is unused.
Original Offset: 004E4810
Return Value: Minerals that would be lost if production changed, or 0 if not applicable.
Status: Complete
*/
int __cdecl base_lose_minerals(int baseID, int productionID) {
	int minAccumal;
	if (Rules->RetoolPctPenProdChg
		&& FactionCurrentBitfield[0] & (1 << Base[baseID].factionIDCurrent)
		&& base_making(Base[baseID].productionIDLast, baseID)
		!= base_making(Base[baseID].queueProductionID[0], baseID)
		&& (minAccumal = Base[baseID].mineralsAccumulated2, minAccumal > Rules->RetoolExemption)) {
		return minAccumal - (100 - Rules->RetoolPctPenProdChg) 
			* (minAccumal - Rules->RetoolExemption) / 100 - Rules->RetoolExemption;
	}
	return 0;
}

/*
Purpose: Set or unset facility at base.
Original Offset: 004E48B0
Return Value: n/a
Status: Complete
*/
void __cdecl set_fac(int facilityID, int baseID, BOOL set) {
	uint32_t offset, mask;
	bitmask(facilityID, &offset, &mask);
	if (set) {
		Base[baseID].facilitiesPresentTable[offset] |= mask;
	}
	else {
		Base[baseID].facilitiesPresentTable[offset] &= ~mask;
	}
}

/*
Purpose: Check whether facility audio blurb announcement has played for faction.
Original Offset: 004E4900
Return Value: Has facility audio blurb played? true/false
Status: Complete
*/
BOOL __cdecl has_fac_announced(int factionID, int facilityID) {
	if (facilityID > FacilitySPStart) {
		return true;
	}
	uint32_t offset, mask;
	bitmask(facilityID, &offset, &mask);
	return (PlayersData[factionID].facilityAnnounced[offset] & mask) != 0;
}

/*
Purpose: Set or unset whether a particular facility audio blurb has played for specified faction.
Original Offset: 004E4960
Return Value: n/a
Status: Complete
*/
void __cdecl set_fac_announced(int factionID, int facilityID, BOOL set) {
	uint32_t offset, mask;
	bitmask(facilityID, &offset, &mask);
	if (set) {
		PlayersData[factionID].facilityAnnounced[offset] |= mask;
	}
	else {
		PlayersData[factionID].facilityAnnounced[offset] &= ~mask;
	}
}

/*
Purpose: Check what facility (if any) a base needs for additional population growth. Stand alone
         function unused in original game and likely optimized out.
Original Offset: 004EEF80
Return Value: Facility id needed for pop growth or zero if base already has Hab Complex and Dome.
Status: Complete
*/
uint32_t __cdecl pop_goal_fac(int baseID) {
	uint32_t factionID = Base[baseID].factionIDCurrent;
	uint32_t limitMod = has_project(SP_ASCETIC_VIRTUES, factionID) ? 2 : 0;
	int pop = Base[baseID].populationSize - limitMod + Players[factionID].rulePopulation;
	if (pop >= Rules->PopLimitSansHabComplex && !has_fac_built(FAC_HAB_COMPLEX, baseID)) {
		return FAC_HAB_COMPLEX;
	}
	if (pop >= Rules->PopLimitSansHabDome && !has_fac_built(FAC_HABITATION_DOME, baseID)) {
		return FAC_HABITATION_DOME;
	}
	return 0; // Pop hasn't reached capacity or Base already has Hab Complex and Dome
}

/*
Purpose: Calculate population goal for a base.
Original Offset: 004EF090
Return Value: Goal population
Status: Complete
*/
uint32_t __cdecl pop_goal(int baseID) {
	uint32_t factionID = Base[baseID].factionIDCurrent;
	uint32_t limitMod = has_project(SP_ASCETIC_VIRTUES, factionID) ? 2 : 0;
	int goal = (36 - Base[baseID].populationSize) / 6 + Base[baseID].populationSize;
	if (goal <= 6) {
		goal = 6;
	}
	if (!has_fac_built(FAC_HAB_COMPLEX, baseID)) {
		int compare = Rules->PopLimitSansHabComplex - Players[factionID].rulePopulation + limitMod;
		if (goal >= compare) {
			goal = compare;
		}
	}
	if (!has_fac_built(FAC_HABITATION_DOME, baseID)) {
		int compare = Rules->PopLimitSansHabDome - Players[factionID].rulePopulation + limitMod;
		if (goal >= compare) {
			goal = compare;
		}
	}
	return goal;
}

/*
Purpose: Calculate facility maintenance cost for specified faction.
Original Offset: 004F6510
Return Value: Facility maintenance cost
Status: Complete
*/
int __cdecl fac_maint(int facilityID, int factionID) {
	if (facilityID == FAC_COMMAND_CENTER) {
		int reactor = best_reactor(factionID);
		int diff = (PlayersData[factionID].diffLevel + 1) / 2;
		if (reactor < 0 || diff < 0) {
			return 0;
		}
		return (reactor > diff) ? diff : reactor;
	}
	int bonusCount = Players[factionID].factionBonusCount;
	for (int i = 0; i < bonusCount; i++) {
		int bonusID = Players[factionID].factionBonusID[i];
		if ((bonusID == RULE_FACILITY || (bonusID == RULE_FREEFAC 
			&& has_tech(Facility[Players[factionID].factionBonusVal1[i]].preqTech, factionID)))
			&& Players[factionID].factionBonusVal1[i] == facilityID) {
			return 0;
		}
	}
	return Facility[facilityID].maint;
}

/*
Purpose: Check if faction has secret project in a base they control.
Original Offset: 004F80D0
Return Value: Does faction have Secret Project? true/false
Status: Complete
*/
BOOL __cdecl has_project(uint32_t projectID, uint32_t factionID) {
	int baseID = base_project(projectID);
	if (baseID >= 0) {
		return (Base[baseID].factionIDCurrent == factionID);
	}
	return false; // Not built or destroyed
}

/*
Purpose: Checks whether facility (non-SP) has been build in currently selected base.
Original Offset: 00500290
Return Value: Does current base have facility? true/false
Status: Complete
*/
BOOL __cdecl has_fac_built(uint32_t facilityID) {
	if (facilityID >= FAC_SKY_HYDRO_LAB) {
		return false;
	}
	return has_fac_built(facilityID, *BaseIDCurrentSelected);
}

/*
Purpose: Check if the base already has a particular facility built.
Original Offset: n/a
Return Value: Does base have facility built? true/false
Status: Complete
*/
BOOL __cdecl has_fac_built(uint32_t facilityID, uint32_t baseID) {
	uint32_t offset, mask;
	bitmask(facilityID, &offset, &mask);
	return (Base[baseID].facilitiesPresentTable[offset] & mask) != 0;
}

/*
Purpose: Get current status of project.
Original Offset: 005002E0
Return Value: baseID or -1/-2 if not built or destroyed
Status: Complete
*/
int __cdecl base_project(uint32_t projectID) {
	return *(&SecretProject->HumanGenomeProject + projectID);
}

/*
Purpose: Calculate the amount of energy that can be stolen from a base based on its population.
Original Offset: 0050C4B0
Return Value: Energy
Status: Complete
*/
int __cdecl steal_energy(uint32_t baseID) {
	uint32_t factionID = Base[baseID].factionIDCurrent;
	int energy = PlayersData[factionID].energyCredits;
	return (energy <= 0) ? 0 :
		((energy * Base[baseID].populationSize) / (PlayersData[factionID].popTotal + 1));
}

/*
Purpose: Check if base is a port.
Original Offset: 00579A00
Return Value: Is base port? true/false
Status: Complete
*/
BOOL __cdecl is_port(int baseID, BOOL isBaseRadius) {
	return is_coast(Base[baseID].xCoord, Base[baseID].yCoord, isBaseRadius);
}

/*
Purpose: Check whether specified base is considered an objective.
Original Offset: 005AC060
Return Value: Is base an objective? true/false
Status: Complete
*/
BOOL __cdecl is_objective(int baseID) {
	if (*GameRules & SCN_VICT_ALL_BASE_COUNT_OBJ || Base[baseID].event & BEVENT_OBJECTIVE) {
		return true;
	}
	if (*GameRules & SCN_VICT_SP_COUNT_OBJ) {
		for (int i = 0; i < MaxSecretProjectNum; i++) {
			if (base_project(i) == baseID) {
				return true;
			}
		}
	}
	if (*GameState & SCN_VICT_BASE_FACIL_COUNT_OBJ && has_fac(*ScnVictFacilityObj, baseID, 0)) {
		return true;
	}
	return false;
}

/*
Purpose: Check if specified faction is currently building Ascent to Transcendence. This code isn't 
         used by original game. There was also a bug where it compares to a non-negative queue id.
Original Offset: 005AC630
Return Value: Is faction transcending? true/false
Status: Complete
*/
BOOL __cdecl transcending(int factionID) {
	if (!ascending(factionID)) {
		return false;
	}
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (Base[i].factionIDCurrent == factionID
			&& Base[i].queueProductionID[0] == -FAC_ASCENT_TO_TRANSCENDENCE) {
			return true;
		}
	}
	return false;
}

/*
Purpose: Check if Voice of Planet has been built that starts the Ascent to Transcendence sequence.
         The factionID parameter is unused and left in for compatibility.
Original Offset: 005AC680
Return Value: Has Voice of Planet been built? true/false
Status: Complete
*/
BOOL __cdecl ascending(int factionID) {
	return base_project(SP_VOICE_OF_PLANET) != SP_Unbuilt;
}

/*
Purpose: Check whether facility is redundant due to a Secret Project counting as that facility.
Original Offset: 005BA030
Return Value: Is facility redundant? true/false
Status: Complete
*/
BOOL __cdecl redundant(int facilityID, int factionID) {
	int projectID;
	switch (facilityID) {
		case FAC_NAVAL_YARD:
			projectID = SP_MARITIME_CONTROL_CENTER;
			break;
		case FAC_PERIMETER_DEFENSE:
			projectID = SP_CITIZENS_DEFENSE_FORCE;
			break;
		case FAC_COMMAND_CENTER:
			projectID = SP_COMMAND_NEXUS;
			break;
		case FAC_BIOENHANCEMENT_CENTER:
			projectID = SP_CYBORG_FACTORY;
			break;
		case FAC_QUANTUM_CONVERTER:
			projectID = SP_SINGULARITY_INDUCTOR;
			break;
		case FAC_AEROSPACE_COMPLEX:
			projectID = SP_CLOUDBASE_ACADEMY;
			break;
		case FAC_ENERGY_BANK:
			projectID = SP_PLANETARY_ENERGY_GRID;
			break;
		default:
			return false;
	}
	return has_project(projectID, factionID);
}

/*
Purpose: Check to see whether provided faction can build a specific facility or Secret Project in
         specified base. Checks are included to prevent SMACX specific facilities from being built 
		 in SMAC mode.
Original Offset: 005BA0E0
Return Value: Is facility or Secret Project available to faction and base? true/false
Status: Complete
*/
BOOL __cdecl facility_avail(int facilityID, int factionID, int baseID, int queueCount) {
	// initial checks
	if (!facilityID || (facilityID == FAC_SKUNKWORKS && *DiffLevelCurrent <= DLVL_SPECIALIST)
		|| (facilityID >= FAC_HUMAN_GENOME_PROJ && *GameRules & SCENRULE_NO_BUILDING_SP)) {
		return false; // Skunkworks removed if there are no prototype costs
	}
	if (facilityID == FAC_ASCENT_TO_TRANSCENDENCE) { // at top since anyone can build it
		return ascending(factionID) && *GameRules & VICTORY_HIGHER_GOAL
			&& _stricmp(Players[factionID].filename, "CARETAKE"); // bug fix for Caretakers
	}
	if (!has_tech(Facility[facilityID].preqTech, factionID)) { // Check tech for facility + SP
		return false;
	}
	// Secret Project availability
	if (!*SMACX_Enabled && (facilityID == FAC_MANIFOLD_HARMONICS
		|| facilityID == FAC_NETHACK_TERMINUS || facilityID == FAC_CLOUDBASE_ACADEMY
		|| facilityID == FAC_PLANETARY_ENERGY_GRID)) {
		return false;
	}
	if (facilityID == FAC_VOICE_OF_PLANET && !_stricmp(Players[factionID].filename, "CARETAKE")) {
		return false; // shifted Caretaker Ascent check to top (never reached here)    
	}
	if (facilityID >= FAC_HUMAN_GENOME_PROJ) {
		return base_project(facilityID - FAC_HUMAN_GENOME_PROJ) == SP_Unbuilt;
	}
	// Facility availability
	if (baseID < 0) {
		return true;
	}
	if (has_fac(facilityID, baseID, queueCount)) {
		return false; // already built or in queue
	}
	if (redundant(facilityID, factionID)) {
		return false; // has SP that counts as facility
	}
	switch (facilityID) { // consolidated into switch to improve readability and performance
		case FAC_RECYCLING_TANKS:
			return !has_fac(FAC_PRESSURE_DOME, baseID, queueCount); // count as Recycling Tank, skip
		case FAC_TACHYON_FIELD:
			return has_fac(FAC_PERIMETER_DEFENSE, baseID, queueCount)
				|| has_project(SP_CITIZENS_DEFENSE_FORCE, factionID); // Cumulative
		case FAC_SKUNKWORKS:
			return !(Players[factionID].ruleFlags & FLAG_FREEPROTO); // no prototype costs? skip
		case FAC_HOLOGRAM_THEATRE:
			return has_fac(FAC_RECREATION_COMMONS, baseID, queueCount) // not documented in manual
				&& !has_project(SP_VIRTUAL_WORLD, factionID); // Network Nodes replaces Theater
		case FAC_HYBRID_FOREST:
			return has_fac(FAC_TREE_FARM, baseID, queueCount); // Cumulative
		case FAC_QUANTUM_LAB:
			return has_fac(FAC_FUSION_LAB, baseID, queueCount); // Cumulative
		case FAC_NANOHOSPITAL:
			return has_fac(FAC_RESEARCH_HOSPITAL, baseID, queueCount); // Cumulative
		case FAC_PARADISE_GARDEN: // bug fix: added check
			return !has_fac(FAC_PUNISHMENT_SPHERE, baseID, queueCount); // antithetical
		case FAC_PUNISHMENT_SPHERE:
			return !has_fac(FAC_PARADISE_GARDEN, baseID, queueCount); // antithetical
		case FAC_NANOREPLICATOR:
			return has_fac(FAC_ROBOTIC_ASSEMBLY_PLANT, baseID, queueCount) // Cumulative
				|| has_fac(FAC_GENEJACK_FACTORY, baseID, queueCount);
		case FAC_HABITATION_DOME:
			return has_fac(FAC_HAB_COMPLEX, baseID, queueCount); // must have Complex
		case FAC_TEMPLE_OF_PLANET:
			return has_fac(FAC_CENTAURI_PRESERVE, baseID, queueCount); // must have Preserve
		case FAC_QUANTUM_CONVERTER:
			return has_fac(FAC_ROBOTIC_ASSEMBLY_PLANT, baseID, queueCount); // Cumulative
		case FAC_NAVAL_YARD:
			return is_coast(Base[baseID].xCoord, Base[baseID].yCoord, false); // needs ocean
		case FAC_AQUAFARM:
		case FAC_SUBSEA_TRUNKLINE:
		case FAC_THERMOCLINE_TRANSDUCER:
			return *SMACX_Enabled && is_coast(Base[baseID].xCoord, Base[baseID].yCoord, false);
		case FAC_COVERT_OPS_CENTER:
		case FAC_BROOD_PIT:
		case FAC_FLECHETTE_DEFENSE_SYS:
			return *SMACX_Enabled;
		case FAC_GEOSYNC_SURVEY_POD: // SMACX only & must have Aerospace Complex
			return *SMACX_Enabled && (has_fac(FAC_AEROSPACE_COMPLEX, baseID, queueCount)
				|| has_project(SP_CLOUDBASE_ACADEMY, factionID)
				|| has_project(SP_SPACE_ELEVATOR, factionID));
		case FAC_SKY_HYDRO_LAB:
		case FAC_NESSUS_MINING_STATION:
		case FAC_ORBITAL_POWER_TRANS:
		case FAC_ORBITAL_DEFENSE_POD:  // must have Aerospace Complex
			return has_fac(FAC_AEROSPACE_COMPLEX, baseID, queueCount)
				|| has_project(SP_CLOUDBASE_ACADEMY, factionID)
				|| has_project(SP_SPACE_ELEVATOR, factionID);
		case FAC_SUBSPACE_GENERATOR: // Progenitor factions only
			return *SMACX_Enabled && (Players[factionID].ruleFlags & FLAG_ALIEN);
		default:
			break;
	}
	return true;
}

/*
Purpose: To assist with locating a facility based on a name rather than using hardcoded offsets.
Original Offset: n/a
Return Value: -1 if not found, otherwise offset value
Status: Complete
*/
int facility_offset(LPCSTR facilSearch) {
	int offset = -1;
	for (uint32_t i = 0; i < MaxFacilityNum; i++) {
		LPSTR facName = Facility[i].name;
		if(facName && !_stricmp(facilSearch, facName)) {
			offset = i;
			break;
		}
	}
	return offset;
}
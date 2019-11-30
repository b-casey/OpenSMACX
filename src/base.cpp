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
#include "base.h"
#include "alpha.h"
#include "faction.h" // PlayerData
#include "game.h" // Difficulty level
#include "map.h" // x_dist(), region_at()
#include "strings.h"
#include "text.h"
#include "technology.h" // has_tech()

rules_facility *Facility = (rules_facility *)0x009A4B68;
rules_citizen *Citizen = (rules_citizen *)0x00946020;
base *Base = (base *)0x0097D040; // 512
base_secret_project *SecretProject = (base_secret_project *)0x009A6514; // 64
int *BaseIDCurrentSelected = (int *)0x00689370;
int *BaseCurrentCount = (int *)0x009A64CC;
int *BaseFindDist = (int *)0x0090EA04;
base **BaseCurrent = (base **)0x0090EA30;

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
	uint32_t offset, mask;
	bitmask(facilityID, &offset, &mask);
	BOOL isBuilt = (Base[baseID].facilitiesPresentTable[offset] & mask) != 0;
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
					? PlayersData[factionID].diploStatus[Base[i].factionIDCurrent] & DSTATE_PACT 
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
Purpose: Generate a base name.
Original Offset: 004E4090
Return Value: n/a
Status: WIP
*/
void __cdecl name_base(int factionID, LPSTR nameOut, BOOL isFinal, BOOL isWaterBase) {
	if (isWaterBase && !text_open(Players[factionID].filename, "WATERBASES")) {
		uint32_t offsetSea = PlayersData[factionID].baseSeaNameOffset + 1, search;
		for (search = 0; search < offsetSea; search++) {
			text_get();
			if (!strlen(*TextBufferGetPtr) || !_strnicmp(*TextBufferGetPtr, "#END", 4)) {
				break;
			}
		}
		if (search == offsetSea) {
			// water base name available
			strncpy_s(nameOut, 25, text_item(), 24);
			nameOut[23] = 0;
			if (isFinal) {
				PlayersData[factionID].baseSeaNameOffset = offsetSea;
			}
			text_close();
			return;
		}
	}
	uint32_t offset = PlayersData[factionID].baseNameOffset + 1;
	sprintf_s(nameOut, 25, "%s %d", get_noun(factionID), offset); // default if names exhausted
	if (!text_open(Players[factionID].filename, "BASES")) {
		uint32_t search;
		for (search = 0; ; search++) {
			text_get();
			if (!strlen(*TextBufferGetPtr) || !_strnicmp(*TextBufferGetPtr, "#END", 4)) {
				break;
			}
		}
		if (offset > 1 && offset <= search) {
			int seed = ((*MapRandSeed + factionID) & 0xFE) | 1;
			int loop = 1;
			do {
				if (seed & 1) {
					seed ^= 0x170;
				}
				seed >>= 1;
			} while (seed >= offset || ++loop != offset);
			offset = seed + 1;
		}
		if (!text_open(Players[factionID].filename, "BASES")) {
			for (search = 0; search < offset; search++) {
				text_get();
				if (!strlen(*TextBufferGetPtr) || !_strnicmp(*TextBufferGetPtr, "#END", 4)) {
					break;
				}
			}
			if (offset != search) {
				if (text_open("BASENAME", "GENERIC")) {
					return;
				}
				if (search < offset) {
					for (search = 0; search < offset; search++) {
						text_get();
						if (!strlen(*TextBufferGetPtr) || !_strnicmp(*TextBufferGetPtr, "#END", 4)) {
							return;
						}
					}
				}
			}
			strncpy_s(nameOut, 25, text_item(), 24);
			nameOut[23] = 0;
			if (isFinal) {
				PlayersData[factionID].baseSeaNameOffset++;
			}
			text_close();
			return;
		}
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
	uint32_t offset, mask;
	bitmask(FAC_SKUNKWORKS, &offset, &mask);
	if (Base[baseID].facilitiesPresentTable[offset] & mask && retool >= 1) {
		retool = 1; // Skunkworks overrides retooling strictness to 'Free in Category'
	}
	if (productionID < 0) { // facility or SP to build
		int queueID = Base[baseID].queueProductionID[0]; // current production item
		if (queueID < 0) { // non-Veh
			queueID = -queueID;
			if (queueID < FAC_SKY_HYDRO_LAB) {
				bitmask(queueID, &offset, &mask);
				if (Base[baseID].facilitiesPresentTable[offset] & mask) {
					return -1; // fac completed outside normal process, no retool penalty to change
				}
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
	if (pop >= Rules->PopLimitSansHabComplex) {
		uint32_t offset, mask;
		bitmask(FAC_HAB_COMPLEX, &offset, &mask);
		if (!(Base[baseID].facilitiesPresentTable[offset] & mask)) {
			return FAC_HAB_COMPLEX;
		}
	}
	if (pop >= Rules->PopLimitSansHabDome) {
		uint32_t offset, mask;
		bitmask(FAC_HABITATION_DOME, &offset, &mask);
		if (!(Base[baseID].facilitiesPresentTable[offset] & mask)) {
			return FAC_HABITATION_DOME;
		}
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
	uint32_t offset, mask;
	bitmask(FAC_HAB_COMPLEX, &offset, &mask);
	if (!(Base[baseID].facilitiesPresentTable[offset] & mask)) {
		int compare = Rules->PopLimitSansHabComplex - Players[factionID].rulePopulation + limitMod;
		if (goal >= compare) {
			goal = compare;
		}
	}
	bitmask(FAC_HABITATION_DOME, &offset, &mask);
	if (!(Base[baseID].facilitiesPresentTable[offset] & mask)) {
		int compare = Rules->PopLimitSansHabDome - Players[factionID].rulePopulation + limitMod;
		if (goal >= compare) {
			goal = compare;
		}
	}
	return goal;
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
	uint32_t offset, mask;
	bitmask(facilityID, &offset, &mask);
	return (Base[*BaseIDCurrentSelected].facilitiesPresentTable[offset] & mask) != 0;
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
Purpose: Calculate offset & bitmask from facilityID.
Original Offset: 0050BA00
Return Value: n/a
Status: Complete
*/
void __cdecl bitmask(uint32_t facilityID, uint32_t *offset, uint32_t *mask) {
	*offset = facilityID / 8;
	*mask = 1 << (facilityID & 7);
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
		|| (facilityID >= FAC_HUMAN_GENOME_PROJ && *GameRules2 & SCENRULE_NO_BUILDING_SP)) {
		return false; // Skunkworks removed if there are no prototype costs
	}
	if (facilityID == FAC_ASCENT_TO_TRANSCENDENCE) { // at top since anyone can build it
		return (base_project(SP_VOICE_OF_PLANET) != SP_Unbuilt) && *GameRules2 & VICTORY_HIGHER_GOAL
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
			return is_coast(Base[baseID].xCoord, Base[baseID].yCoord, FALSE); // needs ocean
		case FAC_AQUAFARM:
		case FAC_SUBSEA_TRUNKLINE:
		case FAC_THERMOCLINE_TRANSDUCER:
			return *SMACX_Enabled && is_coast(Base[baseID].xCoord, Base[baseID].yCoord, FALSE);
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
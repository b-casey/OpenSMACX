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
#include "base.h"
#include "alpha.h"
#include "faction.h"
#include "game.h"
#include "map.h"
#include "strings.h"
#include "text.h"
#include "technology.h"
#include "veh.h"

rules_facility *Facility = (rules_facility *)0x009A4B68;
rules_citizen *Citizen = (rules_citizen *)0x00946020;
base *Base = (base *)0x0097D040; // [512]
base_secret_project *SecretProject = (base_secret_project *)0x009A6514; // [64]
int *BaseIDCurrentSelected = (int *)0x00689370;
int *BaseCurrentCount = (int *)0x009A64CC;
int *BaseFindDist = (int *)0x0090EA04;
base **BaseCurrent = (base **)0x0090EA30;
uint32_t *ScnVictFacilityObj = (uint32_t *)0x009A6814;
uint32_t *BaseUpkeepStage = (uint32_t *)0x0090EA34;
uint32_t *BaseCurrentConvoyFrom = (uint32_t *)0x0090E904; // [4]
uint32_t *BaseCurrentConvoyTo = (uint32_t *)0x0090E93C; // [4]
int *BaseCurrentGrowthRate = (int *)0x0090E918;
uint32_t *BaseCurrentVehPacifismCount = (uint32_t *)0x0090E980;
uint32_t *BaseCurrentForcesSupported = (uint32_t *)0x0090E8FC;
uint32_t *BaseCurrentForcesMaintCost = (uint32_t *)0x0090E91C;

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
		int dist = vector_dist(xCoord, yCoord, Base[i].xCoord, Base[i].yCoord);
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
			int dist = vector_dist(xCoord, yCoord, Base[i].xCoord, Base[i].yCoord);
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
					? has_treaty(factionID, Base[i].factionIDCurrent, DTREATY_PACT)
					: (factionID2 >= 0 && factionID2 == Base[i].factionIDCurrent)))) {
				if (factionID3 < 0 || Base[i].factionIDCurrent == factionID3
					|| ((1 << factionID3) & Base[i].visibility)) {
					int dist = vector_dist(xCoord, yCoord, Base[i].xCoord, Base[i].yCoord);
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
			if ((*BaseCurrent)->populationSize >= (int)Rules->MinBaseSizeSpecialists) {
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
Purpose: TBD
Original Offset: 004E4350
Return Value: n/a
Status: Complete - testing
*/
void __cdecl base_mark(uint32_t baseID) {
	int xCoord = Base[baseID].xCoord, yCoord = Base[baseID].yCoord, xRadius, yRadius;
	uint32_t factionID = Base[baseID].factionIDCurrent;
	for (uint32_t i = 0; i < 49; i++) {
		xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			// optimize into one conditional check?
			(i >= 21) ? site_set(xRadius, yRadius, 0)
				: bit_set(xRadius, yRadius, BIT_BASE_RADIUS, true);
			if (i < 21) {
				using_set(xRadius, yRadius, factionID);
			}
		}
	}
	for (uint32_t f = 0; f < MaxPlayerNum; f++) {
		del_site(f, AI_GOAL_COLONIZE, xRadius, yRadius, 3); // odd it's using last radius point?
	}
}

/*
Purpose: Calculate the cost factor for the specified faction and resource type. Optional base param.
Original Offset: 004E4430
Return Value: Cost factor
Status: Complete
*/
int __cdecl cost_factor(uint32_t factionID, uint32_t rscType, int baseID) {
	const uint32_t diffCostBase[] = { 13, 12, 11, 10, 8, 7 };
	uint32_t factor;
	if (is_human(factionID)) {
		factor = 10;
	}
	else {
		factor = diffCostBase[*DiffLevelCurrent] - great_satan(FactionRankings[7], false)
			- (!*IsMultiplayerNet && is_human(FactionRankings[7]));
	}
	uint32_t costMultiplier = rscType ? Rules->MineralCostMulti : Rules->NutrientCostMulti;
	if (costMultiplier != 10) {
		factor = (factor * costMultiplier) / 10;
	}
	if (*MapSizePlanet == 1) {
		factor = (factor * 9) / 10;
	}
	else if (!*MapSizePlanet) {
		factor = (factor * 8) / 10;
	}
	if (rscType) {
		if (rscType == RSC_MINERALS) {
			switch (PlayersData[factionID].socEffectPending.industry) {
			case -7:
			case -6:
			case -5:
			case -4:
			case -3:
				return (factor * 13 + 9) / 10;
			case -2:
				return (factor * 6 + 4) / 5;
			case -1:
				return (factor * 11 + 9) / 10;
			case 0:
				return factor;
			case 1:
				return (factor * 9 + 9) / 10;
			case 2:
				return (factor * 4 + 4) / 5;
			case 3:
				return (factor * 7 + 9) / 10;
			case 4:
				return (factor * 3 + 4) / 5;
			default:
				return (factor + 1) / 2;
			}
		}
		else {
			return factor; // Energy/PSI? Legacy code logic in case cost used these?
		}
	}
	else { // nutrient
		int growthFactor = PlayersData[factionID].socEffectPending.growth;
		if (baseID >= 0) {
			if (has_fac_built(FAC_CHILDREN_CRECHE, baseID)) {
				growthFactor += 2; // +2 on growth scale
			}
			if (Base[baseID].state & BSTATE_GOLDEN_AGE_ACTIVE) {
				growthFactor += 2;
			}
		}
		growthFactor = range(growthFactor, -2, 5);
		return (factor * (10 - growthFactor) + 9) / 10;
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
		|| (Players[Base[baseID].factionIDCurrent].ruleFlags & RFLAG_FREEPROTO // bug fix
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
		return (productionID > -FacilitySPStart) ? (productionID > -FacilityRepStart) + 2 : 1;
	case 2: // Free switching between SPs (default behavior)
		return (productionID <= -FacilitySPStart) ? -FacilitySPStart : productionID;
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
int __cdecl base_lose_minerals(int baseID, int UNUSED(productionID)) {
	int minAccum;
	if (Rules->RetoolPctPenProdChg && is_human(Base[baseID].factionIDCurrent)
		&& base_making(Base[baseID].productionIDLast, baseID)
		!= base_making(Base[baseID].queueProductionID[0], baseID)
		&& (minAccum = Base[baseID].mineralsAccumulated2, minAccum > (int)Rules->RetoolExemption)) {
		return minAccum - (100 - Rules->RetoolPctPenProdChg)
			* (minAccum - Rules->RetoolExemption) / 100 - Rules->RetoolExemption;
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
		Base[baseID].facilitiesBuilt[offset] |= mask;
	}
	else {
		Base[baseID].facilitiesBuilt[offset] &= ~mask;
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
Purpose: Determine what Veh the specified base should start building first then add it to the queue.
Original Offset: 004E4AA0
Return Value: n/a
Status: Complete
*/
void __cdecl base_first(uint32_t baseID) {
	int priority = -1;
	uint32_t protoID = BSC_SCOUT_PATROL;
	uint32_t factionID = Base[baseID].factionIDCurrent;
	for (int i = 0; i < MaxVehProtoNum; i++) {
		if (veh_avail(i, factionID, baseID)) {
			if (i < MaxVehProtoFactionNum || VehPrototype[i].flags & PROTO_TYPED_COMPLETE) {
				int compare = Armor[VehPrototype[i].armorID].defenseRating * 32;
				uint32_t plan = VehPrototype[i].plan;
				if (plan == PLAN_DEFENSIVE) {
					compare *= 4;
				}
				else if (plan == PLAN_COMBAT) {
					compare *= 3;
				}
				else if (plan == PLAN_RECONNAISANCE) {
					compare *= 2;
				}
				if (get_proto_triad(i) != TRIAD_LAND) {
					compare /= 4;
				}
				compare -= VehPrototype[i].cost;
				if (compare >= priority) {
					priority = compare;
					protoID = i;
				}
			}
		}
	}
	Base[baseID].queueProductionID[0] = protoID;
}

/*
Purpose: Calculate the new unit morale bonus modifier provided by the base and faction for a triad.
Original Offset: 004E6400
Return Value: Morale bonus
Status: Complete
*/
uint32_t __cdecl morale_mod(uint32_t baseID, uint32_t factionID, uint32_t triad) {
	uint32_t moraleMod = 0;
	if (triad == TRIAD_LAND) {
		if (has_fac_built(FAC_COMMAND_CENTER, baseID) || has_project(SP_COMMAND_NEXUS, factionID)) {
			moraleMod = 2;
		}
	}
	else if (triad == TRIAD_SEA) {
		if (has_fac_built(FAC_NAVAL_YARD, baseID)
			|| has_project(SP_MARITIME_CONTROL_CENTER, factionID)) {
			moraleMod = 2;
		}
	}
	else if (triad == TRIAD_AIR) {
		if (has_fac_built(FAC_AEROSPACE_COMPLEX, baseID)
			|| has_project(SP_CLOUDBASE_ACADEMY, factionID)) {
			moraleMod = 2;
		}
	}
	if (has_fac_built(FAC_BIOENHANCEMENT_CENTER, baseID)
		|| has_project(SP_CYBORG_FACTORY, factionID)) {
		moraleMod += 2;
	}
	if (PlayersData[factionID].socEffectPending.morale < -1) {
		moraleMod /= 2;
	}
	return moraleMod;
}

/*
Purpose: Calculate the new native unit lifecycle bonus modifier provided by a base and faction.
Original Offset: 004E65C0
Return Value: Lifecycle bonus
Status: Complete
*/
uint32_t __cdecl breed_mod(uint32_t baseID, uint32_t factionID) {
	uint32_t breedMod = has_project(SP_XENOEMPATYH_DOME, factionID) ? 1 : 0;
	if (has_project(SP_PHOLUS_MUTAGEN, factionID)) {
		breedMod++;
	}
	if (has_project(SP_VOICE_OF_PLANET, factionID)) {
		breedMod++;
	}
	if (has_fac_built(FAC_CENTAURI_PRESERVE, baseID)) {
		breedMod++;
	}
	if (has_fac_built(FAC_TEMPLE_OF_PLANET, baseID)) {
		breedMod++;
	}
	if (has_fac_built(FAC_BIOLOGY_LAB, baseID)) {
		breedMod++;
	}
	if (has_fac_built(FAC_BIOENHANCEMENT_CENTER, baseID)
		|| has_project(SP_CYBORG_FACTORY, factionID)) {
		breedMod++;
	}
	return breedMod;
}

/*
Purpose: Calculate the number of native unit lifecycle/psi bonuses provided by a base and faction.
Original Offset: 004E6740
Return Value: Native life modifier count
Status: Complete
*/
uint32_t __cdecl worm_mod(uint32_t baseID, uint32_t factionID) {
	uint32_t wormMod = breed_mod(baseID, factionID);
	if (Players[factionID].rulePsi) {
		wormMod++;
	}
	if (has_project(SP_DREAM_TWISTER, factionID)) {
		wormMod++;
	}
	if (has_project(SP_NEURAL_AMPLIFIER, factionID)) {
		wormMod++;
	}
	return wormMod;
}

/*
Purpose: Calculate nutrients and growth for the current base.
Original Offset: 004E9B70
Return Value: n/a
Status: Complete
*/
void __cdecl base_nutrient() {
	uint32_t factionID = (*BaseCurrent)->factionIDCurrent;
	*BaseCurrentGrowthRate = PlayersData[factionID].socEffectPending.growth;
	if (has_fac_built(FAC_CHILDREN_CRECHE, *BaseIDCurrentSelected)) {
		*BaseCurrentGrowthRate += 2; // +2 on growth scale
	}
	if ((*BaseCurrent)->state & BSTATE_GOLDEN_AGE_ACTIVE) {
		*BaseCurrentGrowthRate += 2;
	}
	(*BaseCurrent)->nutrientIntake2 += BaseCurrentConvoyTo[RSC_NUTRIENTS];
	(*BaseCurrent)->nutrientConsumption = BaseCurrentConvoyFrom[RSC_NUTRIENTS]
		+ (*BaseCurrent)->populationSize * Rules->NutrientReqCitizen;
	(*BaseCurrent)->nutrientSurplus = (*BaseCurrent)->nutrientIntake2
		- (*BaseCurrent)->nutrientConsumption;
	if ((*BaseCurrent)->nutrientSurplus >= 0) {
		if ((*BaseCurrent)->nutrientsAccumulated < 0) {
			(*BaseCurrent)->nutrientsAccumulated = 0;
		}
	}
	else if (!((*BaseCurrent)->nutrientsAccumulated)) {
		(*BaseCurrent)->nutrientsAccumulated = -1;
	}
	if (*BaseUpkeepStage == 1) {
		PlayersData[factionID].nutrientSurplusTotal
			+= range((*BaseCurrent)->nutrientSurplus, 0, 99);
	}
}

/*
Purpose: Calculate minerals and ecological damage for the current base.
Original Offset: 004E9CB0
Return Value: n/a
Status: Complete
*/
void __cdecl base_minerals() {
	uint32_t factionID = (*BaseCurrent)->factionIDCurrent;
	(*BaseCurrent)->mineralIntake2 += BaseCurrentConvoyTo[RSC_MINERALS];
	uint32_t mineralBonus = (has_fac_built(FAC_QUANTUM_CONVERTER, *BaseIDCurrentSelected)
		|| has_project(SP_SINGULARITY_INDUCTOR, factionID)) ? 1 : 0;
	if (has_fac_built(FAC_ROBOTIC_ASSEMBLY_PLANT, *BaseIDCurrentSelected)) {
		mineralBonus++;
	}
	if (has_fac_built(FAC_GENEJACK_FACTORY, *BaseIDCurrentSelected)) {
		mineralBonus++;
	}
	if (has_fac_built(FAC_NANOREPLICATOR, *BaseIDCurrentSelected)) {
		mineralBonus++;
	}
	if (has_project(SP_BULK_MATTER_TRANSMITTER, factionID)) {
		mineralBonus++;
	}
	(*BaseCurrent)->mineralIntake2 = ((*BaseCurrent)->mineralIntake2 * (mineralBonus + 2)) / 2;
	(*BaseCurrent)->mineralConsumption = *BaseCurrentForcesMaintCost
		+ BaseCurrentConvoyFrom[RSC_MINERALS];
	(*BaseCurrent)->mineralSurplus = (*BaseCurrent)->mineralIntake2
		- (*BaseCurrent)->mineralConsumption;
	(*BaseCurrent)->mineralInefficiency = 0; // ?
	(*BaseCurrent)->mineralSurplus -= (*BaseCurrent)->mineralInefficiency; // ?
	(*BaseCurrent)->mineralSurplusFinal = (*BaseCurrent)->mineralSurplus;
	(*BaseCurrent)->ecoDamage /= 8;
	int planetEcoFactor = PlayersData[factionID].planetEcology + 16;
	if ((*BaseCurrent)->ecoDamage > 0) {
		int planetModifier = (*BaseCurrent)->ecoDamage;
		if ((*BaseCurrent)->ecoDamage >= planetEcoFactor) {
			planetModifier = planetEcoFactor;
		}
		planetEcoFactor -= planetModifier;
		(*BaseCurrent)->ecoDamage -= planetModifier;
	}
	int ecoDmgReduction = (has_fac_built(FAC_NANOREPLICATOR, *BaseIDCurrentSelected)
		|| has_project(SP_SINGULARITY_INDUCTOR, factionID)) ? 2 : 1;
	if (has_fac_built(FAC_CENTAURI_PRESERVE, *BaseIDCurrentSelected)) {
		ecoDmgReduction++;
	}
	if (has_fac_built(FAC_TEMPLE_OF_PLANET, *BaseIDCurrentSelected)) {
		ecoDmgReduction++;
	}
	if (has_project(SP_PHOLUS_MUTAGEN, factionID)) {
		ecoDmgReduction++;
	}
	(*BaseCurrent)->ecoDamage += ((*BaseCurrent)->mineralIntake2 - planetEcoFactor
		- range(PlayersData[factionID].satellitesMineral, 0, (*BaseCurrent)->populationSize))
		/ ecoDmgReduction;
	if (is_human(factionID)) {
		(*BaseCurrent)->ecoDamage += ((PlayersData[factionID].majorAtrocities
			+ TectonicDetonationCount[factionID]) * 5) / (range(*MapSeaLevel, 0, 100)
				/ range(WorldBuilder->SeaLevelRises, 1, 100) + ecoDmgReduction);
	}
	if ((*BaseCurrent)->ecoDamage < 0) {
		(*BaseCurrent)->ecoDamage = 0;
	}
	if (ascending(factionID) && *GameRules & RULES_VICTORY_TRANSCENDENCE) {
		(*BaseCurrent)->ecoDamage *= 2;
	}
	if (*GameState & STATE_PERIHELION_ACTIVE) {
		(*BaseCurrent)->ecoDamage *= 2;
	}
	uint32_t diffFactor;
	if (is_human(factionID)) {
		int diffLvl = PlayersData[factionID].diffLevel;
		diffFactor = !diffLvl ? DLVL_TALENT :
			((diffLvl <= DLVL_LIBRARIAN) ? DLVL_LIBRARIAN : DLVL_TRANSCEND);
	}
	else {
		diffFactor = range(6 - *DiffLevelCurrent, DLVL_SPECIALIST, DLVL_LIBRARIAN);
	}
	(*BaseCurrent)->ecoDamage = ((PlayersData[factionID].techRanking
		- PlayersData[factionID].theoryOfEverything)
		* (3 - range(PlayersData[factionID].socEffectPending.planet, -3, 2))
		* (*MapNativeLifeForms + 1) * (*BaseCurrent)->ecoDamage * diffFactor) / 6;
	(*BaseCurrent)->ecoDamage = ((*BaseCurrent)->ecoDamage + 50) / 100;
	int queueID;
	if (has_project(SP_SPACE_ELEVATOR, factionID) // orbital facilities double mineral prod rate
		&& (queueID = (*BaseCurrent)->queueProductionID[0], queueID == -FAC_SKY_HYDRO_LAB
			|| queueID == -FAC_NESSUS_MINING_STATION || queueID == -FAC_ORBITAL_POWER_TRANS
			|| queueID == -FAC_ORBITAL_DEFENSE_POD)) {
		(*BaseCurrent)->mineralIntake2 *= 2;
		(*BaseCurrent)->mineralSurplus = // doesn't update mineralSurplusFinal?
			(*BaseCurrent)->mineralIntake2 - (*BaseCurrent)->mineralConsumption;
	}
}

/*
Purpose: Calculate the current base's energy loss/inefficiency for an amount of energy.
Original Offset: 004EA1F0
Return Value: Amount of energy inefficiency
Status: Complete
*/
uint32_t __cdecl black_market(int energy) {
	if (energy <= 0) {
		return 0;
	}
	uint32_t factionID = (*BaseCurrent)->factionIDCurrent;
	int xCoord = (*BaseCurrent)->xCoord, yCoord = (*BaseCurrent)->yCoord;
	int distHQ = 999;
	for (int i = 0; i < *BaseCurrentCount; i++) { // modified version of vulnerable()
		if (Base[i].factionIDCurrent == factionID && has_fac_built(FAC_HEADQUARTERS, i)) {
			int dist = vector_dist(Base[i].xCoord, Base[i].yCoord, xCoord, yCoord);
			if (dist < distHQ) {
				distHQ = dist;
			}
		}
	}
	if (distHQ == 999) {
		distHQ = 16;
	}
	else if (distHQ == 0) {
		return 0;
	}
	BOOL hasCreche = has_fac_built(FAC_CHILDREN_CRECHE, *BaseIDCurrentSelected);
	if (*BaseUpkeepStage == 1) {
		for (int i = 0, j = 0; i >= -64; i -= 8, j++) {
			int ineff, factor;
			if (hasCreche) {
				ineff = j - 2; // +2 on efficiency scale
				factor = i + 16; // ?
			}
			else {
				ineff = j;
				factor = i;
			}
			if (ineff >= 8) {
				PlayersData[factionID].unk_46[j] += energy;
			}
			else {
				PlayersData[factionID].unk_46[j] += energy * distHQ / (factor + 64);
			}
		}
	}
	int ineffciency = 4 - PlayersData[factionID].socEffectPending.efficiency;
	if (hasCreche) {
		ineffciency -= 2; // +2 on efficiency scale
	}
	if (ineffciency >= 8) {
		return energy;
	}
	return range(energy * distHQ / ((8 - ineffciency) * 8), 0, energy);
}

/*
Purpose: Calculate two psych related check values for the specified faction.
Original Offset: 004EA4A0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl psych_check(uint32_t factionID, uint32_t *drones, uint32_t *talents) {
	*drones = 6 - (is_human(factionID) ? PlayersData[factionID].diffLevel : DLVL_LIBRARIAN);
	*talents = (((*drones + 2) * (PlayersData[factionID].socEffectPending.efficiency < 0 ? 4
		: PlayersData[factionID].socEffectPending.efficiency + 4) * *MapAreaSqRoot) / 56) / 2;
}

/*
Purpose: Calculate the current base's psych values.
Original Offset: 004EA540
Return Value: n/a
Status: WIP
*/
void __cdecl base_psych() {
	uint32_t factionID = (*BaseCurrent)->factionIDCurrent;
	int popSize = (*BaseCurrent)->populationSize;
	uint32_t dronesBase, talentsBase;
	int diffLvl = is_human(factionID) ? PlayersData[factionID].diffLevel : DLVL_LIBRARIAN;
	psych_check(factionID, &dronesBase, &talentsBase);
	if (talentsBase) {
		talentsBase = range((*BaseIDCurrentSelected % talentsBase
			+ PlayersData[factionID].currentNumBases - talentsBase) / talentsBase, 0, popSize);
	}
	int psychVal = range(popSize - dronesBase, 0, popSize);
	psychVal += range(((*BaseCurrent)->assimilationTurnsLeft + 9) / 10, 
		0, (popSize + diffLvl - 2) / 4);
	if (Players[factionID].ruleDrone) {
		psychVal += popSize / Players[factionID].ruleDrone;
	}
	int psychVal2 = 0;
	if (Players[factionID].ruleTalent) {
		psychVal2 += (Players[factionID].ruleTalent + popSize - 1) / Players[factionID].ruleTalent;
	}
	for (int i = 0; i < Players[factionID].factionBonusCount; i++) {
		if (Players[factionID].factionBonusID[i] == RULE_NODRONE) {
			psychVal -= Players[factionID].factionBonusVal1[i];
			break;
		}
	}
	int drones = has_fac_built(FAC_GENEJACK_FACTORY) ? Rules->DronesGenejackFactory : 0;
	if (has_fac_built(FAC_CHILDREN_CRECHE)) {
		drones -= 2;
	}
	if ((has_fac_built(FAC_NETWORK_NODE) && has_project(SP_VIRTUAL_WORLD, factionID)) 
		|| has_fac_built(FAC_HOLOGRAM_THEATRE)) {
		drones -= 2;
	}
	if (has_project(SP_PLANETARY_TRANS_SYS, factionID) && popSize <= 3) {
		drones--;
	}
	int talentsFac = has_fac_built(FAC_PARADISE_GARDEN) ? 2 : 0;
	if (has_fac_built(FAC_RESEARCH_HOSPITAL)) {
		drones--;
	}
	if (has_fac_built(FAC_NANOHOSPITAL)) {
		drones--;
	}
	int talentsSP = has_project(SP_HUMAN_GENOME_PROJ, factionID) ? 1 : 0;
	if (has_project(SP_CLINICAL_IMMORTALITY, factionID)) {
		talentsSP += 2; // TODO: eval - bug fix per manual: one extra talent at every base
	}
	BOOL hasPunishmentSphere = has_fac_built(FAC_PUNISHMENT_SPHERE);
	for (int i = 8; i >= 0; i--) {
		int val;
		if (!i) {
			val = PlayersData[factionID].socEffectPending.talent;
		}
		else if(*BaseUpkeepStage != 1) {
			break;
		}
		else {
			val = i - 4;
		}
		for (int j = 9; j >= 0; j--) {
			int val2;// , k;
			if (!j) {
				val2 = PlayersData[factionID].socEffectPending.police 
					+ has_fac_built(FAC_BROOD_PIT) * 2;
				if (i) {
					break;
				}
			}
			else if (*BaseUpkeepStage != 1 || !i) {
				break;
			}
			else {
				val2 = j - 6;
				//val2 = val;
			}
			val2 = range(val2 + 2, 0, 4);
			if (val2 > 1) {
				val2--;
			}
			(*BaseCurrent)->droneTotal = dronesBase;
			(*BaseCurrent)->talentTotal = talentsBase;
			if (val2 >= 0) {
				(*BaseCurrent)->talentTotal += val2;
			}
			else {
				(*BaseCurrent)->droneTotal -= val2;
			}
		}
	}
}

/*
Purpose: Determine the base with the most energy output.
Original Offset: 004EB490
Return Value: Base id with the most energy output or -1 for invalid requests
Status: Complete - testing
*/
int __cdecl base_rank(uint32_t faction_id, uint32_t rank_position) {
	int base_id[MaxBaseNum];
	int base_energy[MaxBaseNum];
	uint32_t bases_found = 0;
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (Base[i].factionIDCurrent == faction_id) {
			base_id[bases_found] = i;
			base_energy[bases_found++] = (Base[i].energyIntake1 * 512) + i;
		}
	}
	if (rank_position >= bases_found) {
		return -1;
	}
	sort(bases_found, base_id, base_energy);
	return base_id[rank_position];
}

/*
Purpose: Check what facility (if any) a base needs for additional population growth. Stand alone
		 function unused in original game and likely optimized out.
Original Offset: 004EEF80
Return Value: Facility id needed for pop growth or zero if base already has Hab Complex and Dome.
Status: Complete
*/
uint32_t __cdecl pop_goal_fac(uint32_t baseID) {
	uint32_t factionID = Base[baseID].factionIDCurrent;
	uint32_t limitMod = has_project(SP_ASCETIC_VIRTUES, factionID) ? 2 : 0;
	int pop = Base[baseID].populationSize - limitMod + Players[factionID].rulePopulation;
	if (pop >= (int)Rules->PopLimitSansHabComplex && !has_fac_built(FAC_HAB_COMPLEX, baseID)) {
		return FAC_HAB_COMPLEX;
	}
	if (pop >= (int)Rules->PopLimitSansHabDome && !has_fac_built(FAC_HABITATION_DOME, baseID)) {
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
uint32_t __cdecl pop_goal(uint32_t baseID) {
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
Purpose: Move the specified base's production queue forward if applicable.
Original Offset: 004F06E0
Return Value: Is there a valid item in queue to be built? true/false
Status: Complete - testing
*/
BOOL __cdecl base_queue(uint32_t baseID) {
	uint32_t factionID = Base[baseID].factionIDCurrent;
	while (Base[baseID].queueSize) {
		int queueProdID = Base[baseID].queueProductionID[0];
		if (queueProdID >= 0) {
			PlayersData[factionID].protoIDQueue[queueProdID]--;
		}
		for (uint32_t i = 0; i < Base[baseID].queueSize; i++) {
			Base[baseID].queueProductionID[i] = Base[baseID].queueProductionID[i + 1];
		}
		Base[baseID].queueSize--;
		queueProdID = Base[baseID].queueProductionID[0];
		if ((queueProdID >= 0 && veh_avail(queueProdID, factionID, baseID))
			|| facility_avail(-queueProdID, factionID, baseID, 0)) {
			if (queueProdID <= -FAC_HUMAN_GENOME_PROJ) {
				wave_it(36); // CPU project initiated
			}
			draw_radius(Base[baseID].xCoord, Base[baseID].yCoord, 2, 2);
			return true;
		}
	}
	return false;
}

/*
Purpose: Check if current base has had an energy shortfall. If so, reset all existing energy convoy
		 orders for the faction. TODO: Revisit and find a way to only reset specific base convoys.
Original Offset: 004F4DC0
Return Value: n/a
Status: Complete
*/
void __cdecl base_energy_costs() {
	if ((*BaseCurrent)->energySurplus >= 0 || *VehCurrentCount <= 0) {
		return;
	}
	uint32_t factionID = (*BaseCurrent)->factionIDCurrent;
	for (int i = 0; i < *VehCurrentCount; i++) {
		if (Veh[i].factionID == factionID && VehPrototype[Veh[i].protoID].plan == PLAN_SUPPLY_CONVOY
			&& Veh[i].orders == ORDER_CONVOY && Veh[i].orderAutoType == RSC_ENERGY
			&& base_who(Veh[i].xCoord, Veh[i].yCoord) >= 0) {
			Veh[i].orders = ORDER_NONE;
		}
	}
}

/*
Purpose: Calculate facility maintenance cost for specified faction.
Original Offset: 004F6510
Return Value: Facility maintenance cost
Status: Complete
*/
uint32_t __cdecl fac_maint(uint32_t facilityID, uint32_t factionID) {
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
			&& Players[factionID].factionBonusVal1[i] == (int)facilityID) {
			return 0;
		}
	}
	return Facility[facilityID].maint;
}

/*
Purpose: Calculate overall maintenance cost for the currently selected base.
Original Offset: 004F65F0
Return Value: Base maintenance cost
Status: Complete - testing
*/
void __cdecl base_maint() {
	uint32_t factionID = (*BaseCurrent)->factionIDCurrent;
	for (int fac = 1; fac < FacilitySPStart; fac++) {
		if (fac < FacilityRepStart && has_fac_built(fac)) {
			uint32_t maint = fac_maint(fac, factionID);
			if (has_project(SP_SELF_AWARE_COLONY, factionID)) {
				maint += (PlayersData[factionID].playerFlags & PFLAG_UNK_20) & 1;
				if (maint & 1) {
					PlayersData[factionID].playerFlags |= PFLAG_UNK_20;
				}
				else {
					PlayersData[factionID].playerFlags &= ~PFLAG_UNK_20;
				}
				maint /= 2;
			}
			PlayersData[factionID].energyReserves -= maint;
			PlayersData[factionID].maintCostTotal += maint;
			if (PlayersData[factionID].energyReserves < 0) {
				if (PlayersData[factionID].diffLevel <= DLVL_SPECIALIST 
					|| (*BaseCurrent)->queueProductionID[0] == -fac) {
					maint = 0;
				}
				else {
					set_fac(fac, *BaseIDCurrentSelected, false);
					PlayersData[factionID].energyReserves 
						+= cost_factor(factionID, RSC_MINERALS, -1) * Facility[fac].cost;
					parse_say(1, (int)*(Facility[fac].name), -1, -1);
					popb("POWERSHORT", 0x10000, 14, "genwarning_sm.pcx", NULL);
				}
			}
		}
	}
}

/*
Purpose: Check if the initial base name is unique. If not, append a sequential number after it.
         The function has been rewritten to work as intended.
Original Offset: 004F7FE0
Return Value: n/a
Status: Complete
*/
void __cdecl make_base_unique(uint32_t baseID) {
	purge_spaces(Base[baseID].nameString); // added to remove extraneous whitespace
	uint32_t found = 0;
	std::string coreUniqueName;
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (baseID != (uint32_t)i && !strcmp(Base[i].nameString, Base[baseID].nameString)) {
			if (!found) { // only create core part of base name on first collision
				Base[baseID].nameString[21] = 0;
				coreUniqueName = Base[baseID].nameString;
			}
			std::string uniqueName = coreUniqueName;
			uniqueName += "-" + std::to_string(++found + 1); // start with appending "-2"
			strcpy_s(Base[baseID].nameString, 25, uniqueName.c_str());
			i = 0; // reset loop to verify base name is still unique
		}
	}
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
	return (Base[baseID].facilitiesBuilt[offset] & mask) != 0;
}

/*
Purpose: Get the current status of the specified project.
Original Offset: 005002E0
Return Value: Base id, if not built (-1) or destroyed (-2)
Status: Complete
*/
int __cdecl base_project(uint32_t project_id) {
	return *(&SecretProject->HumanGenomeProject + project_id);
}

/*
Purpose: From the specified base and faction, determine a base to attack.
Original Offset: 0054AFA0
Return Value: baseID or 0
Status: Complete - testing
*/
uint32_t __cdecl attack_from(uint32_t base_id, uint32_t faction_id) {
	uint32_t region_src = region_at(Base[base_id].xCoord, Base[base_id].yCoord);
	uint32_t base_id_atk_from = 0;
	uint32_t search = 9999;
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (Base[i].factionIDCurrent == faction_id) {
			int x = Base[i].xCoord;
			int y = Base[i].yCoord;
			uint32_t compare = (x == PlayersData[faction_id].xCoordTarget
				&& y == PlayersData[faction_id].yCoordTarget) 
				? 0 : vector_dist(x, y, Base[base_id].xCoord, Base[base_id].yCoord);
			if (region_src != region_at(x, y)) {
				compare += 1000;
			}
			if (compare <= search) {
				search = compare;
				base_id_atk_from = i;
			}
		}
	}
	return base_id_atk_from;
}

/*
Purpose: Determine the value of the specified base between the requester and the respondent faction. 
         This valuation could be triggered either from a gift, a threat or a base swap.
Original Offset: 0054CB50
Return Value: Value of base or -1 for invalid requests
Status: Complete - testing
*/
int __cdecl value_of_base(int baseID, uint32_t factionIDReq, uint32_t factionIDRes, 
	uint32_t overmatchDegree, BOOL tgl) {
	if (baseID < 0) {
		return -1;
	}
	int xCoord = Base[baseID].xCoord, yCoord = Base[baseID].yCoord;
	int distFactor = vulnerable(factionIDReq, xCoord, yCoord);
	if (distFactor <= 0) {
		return -1;
	}
	uint32_t regionBase = region_at(xCoord, yCoord);
	for (uint32_t i = 1; i < RadiusRange[6]; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			int baseIDFound = base_at(xRadius, yRadius);
			if (baseIDFound >= 0) {
				uint32_t factionIDBase = Base[baseIDFound].factionIDCurrent;
				uint32_t regionFound = region_at(xRadius, yRadius);
				if (factionIDBase == factionIDReq) {
					if (regionBase == regionFound) {
						distFactor--;
					}
					break;
				}
				if (regionBase == regionFound) {
					if (factionIDBase != factionIDRes) {
						break;
					}
					distFactor++;
				}
			}
		}
	}
	int vehID = stack_fix(veh_at(xCoord, yCoord)); // reason to define here rather than below?
	int mostReserves = PlayersData[factionIDRes].energyReserves;
	if (PlayersData[factionIDReq].energyReserves > mostReserves) {
		mostReserves = PlayersData[factionIDReq].energyReserves;
	}
	uint32_t basePopFactor = Base[baseID].populationSize;
	if (basePopFactor < 3) {
		basePopFactor = 3;
	}
	// bug fix: treat value as unsigned, original uses signed which could cause incorrect valuation
	uint32_t value = ((mostReserves + 1000) / (distFactor + 4)) * basePopFactor;
	if (value < 100) {
		value = 100;
	}
	if (veh_who(xCoord, yCoord) < 0) {
		value /= 2;
	}
	uint32_t gifteeBaseCountRegion = PlayersData[factionIDRes].regionTotalBases[regionBase];
	if (!gifteeBaseCountRegion) {
		value *= 2;
	}
	uint32_t gifterBaseCountRegion = PlayersData[factionIDReq].regionTotalBases[regionBase];
	if (gifterBaseCountRegion == 1) {
		if (!gifteeBaseCountRegion) {
			value *= 2;
		}
		if (gifteeBaseCountRegion == 1) {
			value *= 2;
		}
	}
	if (tgl) {
		value *= 4;
	}
	if (gifteeBaseCountRegion && gifterBaseCountRegion) {
		if (gifteeBaseCountRegion >= gifterBaseCountRegion * 5) {
			value /= 2;
		}
		if (overmatchDegree) {
			value /= 2;
		}
	}
	uint32_t facilCount = 0;
	for (uint32_t fac = 1; fac < FacilitySPStart; fac++) {
		if (fac < FacilityRepStart && has_fac_built(fac, baseID)) {
			facilCount++;
			value += (Facility[fac].cost * facilCount);
		}
	}
	for (uint32_t proj = 0; proj < MaxSecretProjectNum; proj++) {
		if (base_project(proj) == (int)baseID) {
			value += (Facility[FacilitySPStart + proj].cost * 25);
		}
	}
	for (int i = veh_top(vehID); i >= 0; i = Veh[i].nextVehIDStack) {
		if (Veh[i].factionID == factionIDReq) {
			value += (VehPrototype[Veh[i].protoID].cost * 2);
		}
	}
	if (!_stricmp(Players[factionIDReq].filename, "BELIEVE")) {
		value *= 2;
	}
	if (is_objective(baseID)) {
		value *= 4;
	}
	return value;
}

/*
Purpose: Determine ideal non-offense (defense, combat, recon) unit count for the specified base.
Original Offset: 00560B30
Return Value: Amount of non-offensive units needed (1-10)
Status: Complete
*/
uint32_t __cdecl garrison_check(uint32_t baseID) {
	int xCoord = Base[baseID].xCoord, yCoord = Base[baseID].yCoord;
	uint32_t factionID = Base[baseID].factionIDCurrent;
	uint32_t spCount = 0;
	for (uint32_t i = 0; i < MaxSecretProjectNum; i++) {
		if (base_project(i) == (int)baseID) {
			spCount++;
		}
	}
	int garrison = (spCount + 2) / 3 + (Base[baseID].populationSize + 1) / 4 + 1;
	BOOL isObj = is_objective(baseID);
	if (has_fac_built(FAC_HEADQUARTERS, baseID) || bit_at(xCoord, yCoord) & BIT_UNK_40000000
		|| isObj) {
		garrison++;
	}
	if (isObj && PlayersData[factionID].playerFlags & PFLAG_STRAT_DEF_OBJECTIVES) {
		garrison++;
	}
	if (PlayersData[factionID].regionBasePlan[region_at(xCoord, yCoord)] == PLAN_COLONIZATION) {
		garrison--;
	}
	int seaFactionID = zoc_sea(xCoord, yCoord, factionID) - 1;
	if (seaFactionID > 0 
		&& (has_treaty(factionID, seaFactionID, DTREATY_VENDETTA | DTREATY_WANT_REVENGE)
		|| has_treaty(seaFactionID, factionID, DTREATY_WANT_REVENGE)
		|| PlayersData[seaFactionID].integrityBlemishes >= 4)) {
		garrison++;
	}
	int support = PlayersData[factionID].socEffectPending.support;
	if (support <= -4) {
		garrison--;
	}
	if (support <= -3 && garrison > 2) {
		garrison--;
	}
	return range(garrison, 1, 10);
}

/*
Purpose: Determine the number of defensive units needed to protect the specified base.
Original Offset: 00560D30
Return Value: Amount of defensive units needed (1-8)
Status: Complete
*/
uint32_t __cdecl defensive_check(uint32_t baseID) {
	uint32_t defenses = garrison_check(baseID);
	if (defenses > 5) {
		defenses--;
	}
	if (defenses > 2) {
		defenses--;
	}
	return defenses;
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
Purpose: Calculate how vulnerable the coordinates are for the specified faction based on how far
		 away this tile is from the faction's Headquarters.
Original Offset: 0059E980
Return Value: Radial distance between coordinates and faction's HQ or 12 if no HQ/bases
Status: Complete
*/
int __cdecl vulnerable(uint32_t factionID, int xCoord, int yCoord) {
	int dist = 12; // default value for no bases or no HQ
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (Base[i].factionIDCurrent == factionID && has_fac_built(FAC_HEADQUARTERS, i)) {
			dist = vector_dist(xCoord, yCoord, Base[i].xCoord, Base[i].yCoord);
			break;
		}
	}
	return dist;
}

/*
Purpose: Check whether specified base is considered an objective.
Original Offset: 005AC060
Return Value: Is base an objective? true/false
Status: Complete
*/
BOOL __cdecl is_objective(int baseID) {
	if (*GameRules & RULES_SCN_VICT_ALL_BASE_COUNT_OBJ || Base[baseID].event & BEVENT_OBJECTIVE) {
		return true;
	}
	if (*GameRules & RULES_SCN_VICT_SP_COUNT_OBJ) {
		for (int i = 0; i < MaxSecretProjectNum; i++) {
			if (base_project(i) == baseID) {
				return true;
			}
		}
	}
	if (*GameState & STATE_SCN_VICT_BASE_FACIL_COUNT_OBJ
		&& has_fac(*ScnVictFacilityObj, baseID, 0)) {
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
BOOL __cdecl ascending(int UNUSED(factionID)) {
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
		|| (facilityID >= FAC_HUMAN_GENOME_PROJ && *GameRules & RULES_SCN_NO_BUILDING_SP)) {
		return false; // Skunkworks removed if there are no prototype costs
	}
	if (facilityID == FAC_ASCENT_TO_TRANSCENDENCE) { // at top since anyone can build it
		return ascending(factionID) && *GameRules & RULES_VICTORY_TRANSCENDENCE
			&& _stricmp(Players[factionID].filename, "CARETAKE"); // bug fix for Caretakers
	}
	if (!has_tech(Facility[facilityID].preqTech, factionID)) { // Check tech for facility + SP
		return false;
	}
	// Secret Project availability
	if (!*ExpansionEnabled && (facilityID == FAC_MANIFOLD_HARMONICS
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
		return !(Players[factionID].ruleFlags & RFLAG_FREEPROTO); // no prototype costs? skip
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
		return *ExpansionEnabled && is_coast(Base[baseID].xCoord, Base[baseID].yCoord, false);
	case FAC_COVERT_OPS_CENTER:
	case FAC_BROOD_PIT:
	case FAC_FLECHETTE_DEFENSE_SYS:
		return *ExpansionEnabled;
	case FAC_GEOSYNC_SURVEY_POD: // SMACX only & must have Aerospace Complex
		return *ExpansionEnabled && (has_fac(FAC_AEROSPACE_COMPLEX, baseID, queueCount)
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
		return is_alien_faction(factionID);
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
int __cdecl facility_offset(LPCSTR facilSearch) {
	int offset = -1;
	for (uint32_t i = 0; i < MaxFacilityNum; i++) {
		LPSTR facName = Facility[i].name;
		if (facName && !_stricmp(facilSearch, facName)) {
			offset = i;
			break;
		}
	}
	return offset;
}

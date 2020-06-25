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
#include "terraforming.h"
#include "base.h"
#include "faction.h"
#include "game.h"
#include "general.h"
#include "map.h"
#include "technology.h"
#include "veh.h"

rules_terraforming *Terraforming = (rules_terraforming *)0x00691878;

/*
Purpose: Calculate the credit cost to lower or raise terrain at tile for the specified faction.
Original Offset: 004C9420
Return Value: Credit cost
Status: Complete - testing
*/
uint32_t __cdecl terraform_cost(int xCoord, int yCoord, uint32_t factionID) {
	uint32_t alt = alt_at(xCoord, yCoord);
	int cost = abs((int)alt - 3);
	cost += 2;
	cost *= cost;
	if (bit_at(xCoord, yCoord) & BIT_FUNGUS && altitude_at(xCoord, yCoord) >= ALT_BIT_OCEAN_SHELF) {
		cost *= 3;
	}
	cost *= 2;
	if (alt < ALT_SHORE_LINE) {
		cost *= 2;
		if (!GameTechDiscovered[TECH_DOCAIR]) {
			cost *= 2;
		}
	}
	int baseID = base_find(xCoord, yCoord, factionID);
	if (baseID >= 0) {
		int cursorDist = cursor_dist(xCoord, yCoord, Base[baseID].xCoord, Base[baseID].yCoord);
		cost *= range(cursorDist, 1, 100);
		int baseID2 = base_find(xCoord, yCoord, -1, -1, factionID, -1);
		if (baseID2 >= 0 && !(PlayersData[factionID].diploTreaties[Base[baseID2].factionIDCurrent] 
			& DTREATY_PACT)) {
			int cmp = (cursorDist * (Base[baseID2].populationSize + 2)) / 3;
			int cmp2 = (cursor_dist(xCoord, yCoord, Base[baseID2].xCoord, Base[baseID2].yCoord)
				* (Base[baseID].populationSize + 2)) / 3;
			if (cmp2 && cmp && cmp2 < cmp) {
				cost = (cost * cmp) / cmp2;
			}
		}
	}
	if (Players[factionID].ruleFlags & RFLAG_TERRAFORM) {
		cost /= 2;
	}
	cost /= 2;
	return range(cost, 1, 30000);
}

/*
Purpose: Calculate Former rate to perform terrain enhancements.
Original Offset: 004C9A50
Return Value: Terraforming speed
Status: Complete
*/
uint32_t __cdecl contribution(int vehID, uint32_t terraformID) {
	uint32_t rate = has_abil(Veh[vehID].protoID, ABL_SUPER_TERRAFORMER) ? 4 : 2;
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
Purpose: Check to see whether provided faction can construct specific terrain enhancement.
Original Offset: 005BAB40
Return Value: Is terrain enhancement available to faction? true/false
Status: Complete
*/
BOOL __cdecl terrain_avail(int terraformID, BOOL isSea, int factionID) {
	int preqTech = *(&Terraforming[terraformID].preqTech + isSea);
	if (preqTech < TechNone || ((terraformID == TERRA_RAISE_LAND || terraformID == TERRA_LOWER_LAND) 
		&& *GameRules & RULES_SCN_NO_TERRAFORMING)) {
		return false;
	}
	if (terraformID >= TERRA_CONDENSER && terraformID <= TERRA_LEVEL_TERRAIN
		&& has_project(SP_WEATHER_PARADIGM, factionID)) {
		return true;
	}
	return has_tech(preqTech, factionID);
}
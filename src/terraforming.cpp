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
#include "terraforming.h"
#include "base.h"
#include "game.h"
#include "map.h"
#include "technology.h"

rules_terraforming *Terraforming = (rules_terraforming *)0x00691878;

/*
Purpose: Calculate the amount of poor quality terrain attributes the tile has for farming (?).
Original Offset: 004ECB90
Return Value: Quality of terrain, lower is better (0-2)
Status: Complete
*/
uint32_t __cdecl crappy(int xCoord, int yCoord) {
	uint32_t poorQuality = 0;
	uint32_t climate = map_loc(xCoord, yCoord)->val1 & (CLIMATE_MOIST | CLIMATE_RAINY);
	if (!climate) {
		poorQuality = 1; // neither moist or rainy
	}
	uint32_t rocky = rocky_at(xCoord, yCoord);
	if (rocky == TERRAIN_ROCKY) {
		poorQuality++; // rocky
	}
	else if (rocky == TERRAIN_FLAT && climate < CLIMATE_RAINY) {
		poorQuality++; // flat, moist or arid
	}
	return poorQuality;
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
		&& *GameRules2 & SCENRULE_NO_TERRAFORMING)) {
		return false;
	}
	if (terraformID >= TERRA_CONDENSER && terraformID <= TERRA_LEVEL_TERRAIN
		&& has_project(SP_WEATHER_PARADIGM, factionID)) {
		return true;
	}
	return has_tech(preqTech, factionID);
}
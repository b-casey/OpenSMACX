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
#include "game.h"
#include "map.h"
#include "technology.h"
#include "veh.h"

rules_terraforming *Terraforming = (rules_terraforming *)0x00691878;

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
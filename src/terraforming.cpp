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

RulesTerraforming *Terraforming = (RulesTerraforming *)0x00691878;

/*
Purpose: Calculate the credit cost to lower or raise the tile's terrain for the specified faction.
Original Offset: 004C9420
Return Value: Credit cost
Status: Complete
*/
uint32_t __cdecl terraform_cost(int x, int y, uint32_t faction_id) {
	uint32_t alt = alt_at(x, y);
	int cost = abs((int)alt - 3);
	cost += 2;
	cost *= cost;
	if (bit_at(x, y) & BIT_FUNGUS && altitude_at(x, y) >= ALT_BIT_OCEAN_SHELF) {
		cost *= 3;
	}
	cost *= 2;
	if (alt < ALT_SHORE_LINE) {
		cost *= 2;
		if (!GameTechAchieved[TECH_DOCAIR]) {
			cost *= 2;
		}
	}
	int base_id = base_find(x, y, faction_id);
	if (base_id >= 0) {
		int cursor_distance = cursor_dist(x, y, Bases[base_id].x, Bases[base_id].y);
		cost *= range(cursor_distance, 1, 100);
		int base_id_prox = base_find(x, y, -1, -1, faction_id, -1);
		if (base_id_prox >= 0 
			&& !has_treaty(faction_id, Bases[base_id_prox].faction_id_current, DTREATY_PACT)) {
			int num_prox = (cursor_distance * (Bases[base_id_prox].population_size + 2)) / 3;
			int denom_prox = (cursor_dist(x, y, Bases[base_id_prox].x, Bases[base_id_prox].y)
				* (Bases[base_id].population_size + 2)) / 3;
			if (denom_prox && num_prox && denom_prox < num_prox) {
				cost = (cost * num_prox) / denom_prox; // increase cost based on proximity ratio
			}
		}
	}
	if (Players[faction_id].ruleFlags & RFLAG_TERRAFORM) {
		cost /= 2;
	}
	cost /= 2;
	return range(cost, 1, 30000);
}

/*
Purpose: Calculate the Former rate to perform terrain enhancements.
Original Offset: 004C9A50
Return Value: Terraforming speed
Status: Complete
*/
uint32_t __cdecl contribution(uint32_t veh_id, uint32_t terraform_id) {
	uint32_t rate = has_abil(Vehs[veh_id].proto_id, ABL_SUPER_TERRAFORMER) ? 4 : 2;
	if (terraform_id == (ORDER_REMOVE_FUNGUS - 4) || terraform_id == (ORDER_PLANT_FUNGUS - 4)) {
		if (has_project(SP_XENOEMPATYH_DOME, Vehs[veh_id].faction_id)) {
			rate *= 2; // Doubled
		}
	} else if (has_project(SP_WEATHER_PARADIGM, Vehs[veh_id].faction_id)) {
		rate = (rate * 3) / 2; // +50%
	}
	return rate;
}

/*
Purpose: Check to see whether the specified faction can construct a specific terrain enhancement.
Original Offset: 005BAB40
Return Value: Is terrain enhancement available to faction? true/false
Status: Complete
*/
BOOL __cdecl terrain_avail(uint32_t terraform_id, BOOL is_sea, int faction_id) {
	int preq_tech = *(&Terraforming[terraform_id].preq_tech + is_sea);
	if (preq_tech < TechNone || ((terraform_id == TERRA_RAISE_LAND
		|| terraform_id == TERRA_LOWER_LAND) && *GameRules & RULES_SCN_NO_TERRAFORMING)) {
		return false;
	}
	if (terraform_id >= TERRA_CONDENSER && terraform_id <= TERRA_LEVEL_TERRAIN
		&& has_project(SP_WEATHER_PARADIGM, faction_id)) {
		return true;
	}
	return has_tech(preq_tech, faction_id);
}

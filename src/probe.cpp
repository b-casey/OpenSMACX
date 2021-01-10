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
#include "probe.h"
#include "base.h"
#include "game.h"
#include "map.h"
#include "veh.h"

BOOL *ProbeHasAlgoEnhancement = (BOOL *)0x00945B30;
uint32_t *ProbeTargetFactionID = (uint32_t *)0x00945B34;
BOOL *ProbeTargetHasHSA = (BOOL *)0x00945B38;

/*
Purpose: Calculate the amount of energy that can be stolen from a base based on its population.
Original Offset: 0050C4B0
Return Value: Energy
Status: Complete
*/
int __cdecl steal_energy(uint32_t base_id) {
	uint32_t faction_id = Bases[base_id].faction_id_current;
	int energy = PlayersData[faction_id].energy_reserves;
	return (energy <= 0) ? 0 
		: ((energy * Bases[base_id].population_size) / (PlayersData[faction_id].pop_total + 1));
}

/*
Purpose: Calculate the cost for the faction to be able to mind control the specified base. The 3rd
		 parameter determines if this cost is for cornering the market (true) or via probe (false).
Original Offset: 0059EA80
Return Value: Mind control cost
Status: Complete
*/
int __cdecl mind_control(uint32_t base_id, uint32_t faction_id, BOOL is_corner_market) {
	uint32_t target_faction_id = Bases[base_id].faction_id_current;
	int target_x = Bases[base_id].x;
	int target_y = Bases[base_id].y;
	int cost = vulnerable(target_faction_id, target_x, target_y);
	if (cost <= 0) {
		if (!is_corner_market) {
			return -1;
		}
		cost = 1;
	}
	if (has_fac_built(FAC_GENEJACK_FACTORY, base_id)) {
		cost *= 2;
	}
	if (has_fac_built(FAC_CHILDREN_CRECHE, base_id)) {
		cost /= 2; // Reduces base's vulnerability to enemy mind control
	}
	if (has_fac_built(FAC_PUNISHMENT_SPHERE, base_id)) {
		cost /= 2;
	}
	if (Bases[base_id].nerve_staple_turns_left) {
		cost /= 2;
	}
	int veh_id = stack_fix(veh_at(target_x, target_y));
	cost = ((stack_check(veh_id, 2, PLAN_COMBAT, -1, -1)
		+ stack_check(veh_id, 2, PLAN_OFFENSIVE, -1, -1))
		* (stack_check(veh_id, 6, ABL_POLY_ENCRYPTION, -1, -1) + 1)
		+ PlayersData[faction_id].mind_control_total / 4 + Bases[base_id].population_size)
		* ((PlayersData[target_faction_id].corner_market_active
			+ PlayersData[target_faction_id].energy_reserves + 1200) / (cost + 4));
	if (!is_human(faction_id) && is_human(target_faction_id)) {
		int diff = PlayersData[target_faction_id].diff_level;
		if (diff > DLVL_LIBRARIAN) {
			cost = (cost * 3) / diff;
		}
	}
	BOOL has_pact = has_treaty(faction_id, target_faction_id, DTREATY_PACT);
	if (is_corner_market) {
		if (has_pact) {
			cost /= 2;
		}
		if (has_treaty(faction_id, target_faction_id, DTREATY_TREATY)) {
			cost /= 2;
		}
		int tech_comm_target = PlayersData[target_faction_id].tech_commerce_bonus;
		tech_comm_target *= tech_comm_target;
		int tech_comm_probe = PlayersData[faction_id].tech_commerce_bonus;
		tech_comm_probe *= tech_comm_probe;
		cost = (cost * (tech_comm_target + 1)) / (tech_comm_probe + 1);
	} else if (has_pact) {
		cost *= 2;
	}
	if (Bases[base_id].faction_id_former == faction_id) {
		cost /= 2;
	}
	uint32_t base_state = Bases[base_id].state;
	if (base_state & BSTATE_DRONE_RIOTS_ACTIVE) {
		cost /= 2;
	}
	if (base_state & BSTATE_GOLDEN_AGE_ACTIVE) {
		cost *= 2;
	}
	if (has_treaty(target_faction_id, faction_id, DTREATY_ATROCITY_VICTIM)) {
		cost *= 2;
	} else if (has_treaty(target_faction_id, faction_id, DTREATY_WANT_REVENGE)) {
		cost += cost / 2;
	}
	return cost;
}

/*
Purpose: Calculate the success and survival rates for a probe action based on the probe's morale and
		 the difficulty of the action. These are used to generate a chances probability string for
		 provided id. A baseID is an optional parameter to factor in its probe defenses.
Original Offset: 0059EEE0
Return Value: Success rate of probe
Status: Complete
*/
int __cdecl success_rates(uint32_t id, uint32_t morale, int diff_modifier, int base_id) {
	char probe_chances[25];
	int success_rate;
	if (diff_modifier < 0) {
		strcpy_s(probe_chances, 25, "100%");
		success_rate = diff_modifier;
	} else {
		if (morale < 1) {
			morale = 1;
		}
		int prb_defense = (base_id != -1 && has_fac_built(FAC_COVERT_OPS_CENTER, base_id)) ? 2 : 0;
		prb_defense = range(PlayersData[*ProbeTargetFactionID].soc_effect_active.probe + prb_defense, 
			-2, 0);
		uint32_t failure_rate = (diff_modifier * 100) / ((morale / 2) - prb_defense + 1);
		if (*ProbeHasAlgoEnhancement && !*ProbeTargetHasHSA) {
			failure_rate /= 2; // Algo Ench: failure cut in half when acting against normal targets
		}
		success_rate = 100 - failure_rate;
		if (*ProbeTargetHasHSA) {
			success_rate /= 2; // Chance of success is half what the chance would have been w/o HSA
		}
		uint32_t loss_rate = ((diff_modifier + 1) * 100) / (morale - prb_defense);
		if (*ProbeHasAlgoEnhancement && !*ProbeTargetHasHSA) {
			loss_rate /= 2;
		}
		int survival_rate = 100 - loss_rate;
		if (*ProbeTargetHasHSA) {
			survival_rate /= 2; // bug fix: original had an erroneous 2nd hit to successRate
		}
		// this check was removed in my unofficial patch, leaving it as is
		(survival_rate == success_rate) ? sprintf_s(probe_chances, 25, "%d%%", success_rate)
			: sprintf_s(probe_chances, 25, "%d%%, %d%%", success_rate, survival_rate);
	}
	parse_says(id, probe_chances, -1, -1);
	return success_rate;
}

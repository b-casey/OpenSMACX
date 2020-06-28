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
int __cdecl steal_energy(uint32_t baseID) {
	uint32_t factionID = Base[baseID].factionIDCurrent;
	int energy = PlayersData[factionID].energyReserves;
	return (energy <= 0) ? 0 :
		((energy * Base[baseID].populationSize) / (PlayersData[factionID].popTotal + 1));
}

/*
Purpose: Calculate the cost for the faction to be able to mind control the specified base. The 3rd
		 parameter determines if this cost is for cornering the market (true) or via probe (false).
Original Offset: 0059EA80
Return Value: Mind control cost
Status: Complete
*/
int __cdecl mind_control(uint32_t baseID, uint32_t factionID, BOOL isCornerMarket) {
	uint32_t targetFactionID = Base[baseID].factionIDCurrent;
	int target_xCoord = Base[baseID].xCoord, target_yCoord = Base[baseID].yCoord;
	int cost = vulnerable(targetFactionID, target_xCoord, target_yCoord);
	if (cost <= 0) {
		if (!isCornerMarket) {
			return -1;
		}
		cost = 1;
	}
	if (has_fac_built(FAC_GENEJACK_FACTORY, baseID)) {
		cost *= 2;
	}
	if (has_fac_built(FAC_CHILDREN_CRECHE, baseID)) {
		cost /= 2;
	}
	if (has_fac_built(FAC_PUNISHMENT_SPHERE, baseID)) {
		cost /= 2;
	}
	if (Base[baseID].nerveStapleTurnsLeft) {
		cost /= 2;
	}
	int vehID = stack_fix(veh_at(target_xCoord, target_yCoord));
	cost = ((stack_check(vehID, 2, PLAN_COMBAT, -1, -1)
		+ stack_check(vehID, 2, PLAN_OFFENSIVE, -1, -1))
		* (stack_check(vehID, 6, ABL_POLY_ENCRYPTION, -1, -1) + 1)
		+ PlayersData[factionID].mindControlTotal / 4 + Base[baseID].populationSize)
		* ((PlayersData[targetFactionID].cornerMarketActive
			+ PlayersData[targetFactionID].energyReserves + 1200) / (cost + 4));
	if (!is_human(factionID) && is_human(targetFactionID)) {
		int diff = PlayersData[targetFactionID].diffLevel;
		if (diff > DLVL_LIBRARIAN) {
			cost = (cost * 3) / diff;
		}
	}
	int treaties = PlayersData[factionID].diploTreaties[targetFactionID];
	if (isCornerMarket) {
		if (treaties & DTREATY_PACT) {
			cost /= 2;
		}
		if (treaties & DTREATY_TREATY) {
			cost /= 2;
		}
		int techCommTarget = PlayersData[targetFactionID].techCommerceBonus;
		int techCommProbe = PlayersData[factionID].techCommerceBonus;
		cost = (cost * (techCommTarget * techCommTarget + 1)) / (techCommProbe * techCommProbe + 1);
	}
	else {
		if (treaties & DTREATY_PACT) {
			cost *= 2;
		}
	}
	if (Base[baseID].factionIDFormer == factionID) {
		cost /= 2;
	}
	uint32_t baseStatus = Base[baseID].status;
	if (baseStatus & BSTATUS_DRONE_RIOTS_ACTIVE) {
		cost /= 2;
	}
	if (baseStatus & BSTATUS_GOLDEN_AGE_ACTIVE) {
		cost *= 2;
	}
	int targetTreaties = PlayersData[targetFactionID].diploTreaties[factionID];
	if (targetTreaties & DTREATY_ATROCITY_VICTIM) {
		cost *= 2;
	}
	else if (targetTreaties & DTREATY_WANT_REVENGE) {
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
int __cdecl success_rates(uint32_t id, uint32_t morale, int diffModifier, int baseID) {
	char probeChances[25];
	int successRate;
	if (diffModifier < 0) {
		strcpy_s(probeChances, 25, "100%");
		successRate = diffModifier;
	}
	else {
		if (morale < 1) {
			morale = 1;
		}
		int probeDefRating = (baseID != -1 && has_fac_built(FAC_COVERT_OPS_CENTER, baseID)) ? 2 : 0;
		probeDefRating = range(PlayersData[*ProbeTargetFactionID].SE_Probe + probeDefRating, -2, 0);
		uint32_t failureRate = (diffModifier * 100) / ((morale / 2) - probeDefRating + 1);
		if (*ProbeHasAlgoEnhancement && !*ProbeTargetHasHSA) {
			failureRate /= 2; // Algo Ench: failure cut in half when acting against normal targets
		}
		successRate = 100 - failureRate;
		if (*ProbeTargetHasHSA) {
			successRate /= 2; // Chance of success is half what the chance would have been w/o HSA
		}
		uint32_t lossRate = ((diffModifier + 1) * 100) / (morale - probeDefRating);
		if (*ProbeHasAlgoEnhancement && !*ProbeTargetHasHSA) {
			lossRate /= 2;
		}
		int survivalRate = 100 - lossRate;
		if (*ProbeTargetHasHSA) {
			survivalRate /= 2; // bug fix: original had an erroneous 2nd hit to successRate
		}
		// this check was removed in my unofficial patch, leaving it as is
		(survivalRate == successRate) ? sprintf_s(probeChances, 25, "%d%%", successRate) :
			sprintf_s(probeChances, 25, "%d%%, %d%%", successRate, survivalRate);
	}
	parse_says(id, probeChances, -1, -1);
	return successRate;
}
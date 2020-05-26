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
Status: Complete - testing
*/
int __cdecl mind_control(uint32_t baseID, uint32_t factionID, BOOL isCornerMarket) {
    // vulnerable()
    int xCoord = Base[baseID].xCoord, yCoord = Base[baseID].yCoord, cost = 12;
    uint32_t factionIDBase = Base[baseID].factionIDCurrent;
    for (int i = 0; i < *BaseCurrentCount; i++) {
        if (Base[i].factionIDCurrent == factionIDBase && has_fac(FAC_HEADQUARTERS, i, 0)) {
            cost = x_dist(cursor_dist(xCoord, Base[i].xCoord), yCoord - Base[i].yCoord);
			if (cost <= 0) {
				if (!isCornerMarket) {
					return -1;
				}
                cost = 1;
			}
			break;
        }
    }
    //
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
    int vehID = stack_fix(veh_at(xCoord, yCoord));
    cost = ((stack_check(vehID, 2, PLAN_COMBAT, -1, -1)
        + stack_check(vehID, 2, PLAN_OFFENSIVE, -1, -1))
        * (stack_check(vehID, 6, ABL_POLY_ENCRYPTION, -1, -1) + 1)
        + PlayersData[factionID].mindControlTotal / 4 + Base[baseID].populationSize)
        * ((PlayersData[factionIDBase].cornerMarketActive 
            + PlayersData[factionIDBase].energyReserves + 1200) / (cost + 4));
    if (!is_human(factionID) && is_human(factionIDBase)) {
        int diff = PlayersData[factionIDBase].diffLevel;
        if (diff > DLVL_LIBRARIAN) {
            cost = (cost * 3) / diff;
        }
    }
    int treaties = PlayersData[factionID].diploTreaties[factionIDBase];
    if (isCornerMarket) {
        if (treaties & DTREATY_PACT) {
            cost /= 2;
        }
		if (treaties & DTREATY_TREATY) {
            cost /= 2;
		}
        int techComm1 = PlayersData[factionIDBase].techCommerceBonus;
        int techComm2 = PlayersData[factionID].techCommerceBonus;
        cost = (cost * (techComm1 * techComm1 + 1)) / (techComm2 * techComm2 + 1);
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
    int treaties2 = PlayersData[factionIDBase].diploTreaties[factionID];
	if (treaties2 & DTREATY_ATROCITY_VICTIM) {
        cost *= 2;
	}
	else if (treaties2 & DTREATY_WANT_REVENGE) {
        cost += cost / 2;
	}
	return cost;
}
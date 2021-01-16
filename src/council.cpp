/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2021 Brendan Casey
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
#include "council.h"
#include "base.h"
#include "game.h"

RulesProposal *Proposal = (RulesProposal *)0x009A6828;

/*
Purpose: Calculate faction's vote count. Used for Planetary Governor and Supreme Leader.
Original Offset: 0052AD30
Return Value: Faction vote count
Status: Complete
*/
uint32_t __cdecl council_votes(uint32_t faction_id) {
    if (is_alien_faction(faction_id)) {
        return 0;
    }
    uint32_t votes = 0;
    for (int i = 0; i < *BaseCurrentCount; i++) {
        if (faction_id == Bases[i].faction_id_current) {
            votes += Bases[i].population_size;
        }
    }
    if (has_project(SP_EMPATH_GUILD, faction_id)) {
        votes += votes / 2; // +50% votes
    }
    if (has_project(SP_CLINICAL_IMMORTALITY, faction_id)) {
        votes *= 2; // Doubles votes
    }
    int bonus_count = Players[faction_id].faction_bonus_count;
    for (int i = 0; i < bonus_count; i++) {
        if (Players[faction_id].faction_bonus_id[i] == RULE_VOTES) {
            int votes_bonus = Players[faction_id].faction_bonus_val1[i];
            if (votes_bonus >= 0) {
                votes *= votes_bonus; // Peacekeeper bonus
            }
        }
    }
    return votes;
}

/*
Purpose: Check whether a faction's leader is eligible to be a Planetary Governor candidate.
Original Offset: 0052AE20
Return Value: Is the leader eligible (top two vote totals)? true/false
Status: Complete
*/
BOOL __cdecl eligible(uint32_t faction_id) {
    if (is_alien_faction(faction_id)) {
        return false;
    }
    uint32_t votes = council_votes(faction_id);
    uint32_t faction_count = 0;
    for (uint32_t i = 1; i < MaxPlayerNum; i++) {
        if (faction_id != i && is_alive(i) && council_votes(i) > votes) {
            faction_count++;
        }
    }
    return faction_count < 2;
}

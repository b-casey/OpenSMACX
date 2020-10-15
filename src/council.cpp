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
#include "council.h"
#include "base.h"
#include "game.h"

rules_proposal *Proposal = (rules_proposal *)0x009A6828;

/*
Purpose: Calculate faction's vote count. Used for Planetary Governor and Supreme Leader.
Original Offset: 0052AD30
Return Value: Faction vote count
Status: Complete
*/
uint32_t __cdecl council_votes(uint32_t factionID) {
	if (is_alien_faction(factionID)) {
		return 0;
	}
	uint32_t votes = 0;
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (factionID == Base[i].factionIDCurrent) {
			votes += Base[i].populationSize;
		}
	}
	if (has_project(SP_EMPATH_GUILD, factionID)) {
		votes += votes / 2; // +50% votes
	}
	if (has_project(SP_CLINICAL_IMMORTALITY, factionID)) {
		votes *= 2; // Doubles votes
	}
	int bonusCount = Players[factionID].factionBonusCount;
	for (int i = 0; i < bonusCount; i++) {
		if (Players[factionID].factionBonusID[i] == RULE_VOTES) {
			int votesBonus = Players[factionID].factionBonusVal1[i];
			if (votesBonus >= 0) {
				votes *= votesBonus; // Peacekeeper bonus
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
BOOL __cdecl eligible(uint32_t factionID) {
	if (is_alien_faction(factionID)) {
		return 0;
	}
	uint32_t votes = council_votes(factionID);
	uint32_t factionCount = 0;
	for (uint32_t i = 1; i < MaxPlayerNum; i++) {
		if (factionID != i && is_alive(i) && council_votes(i) > votes) {
			factionCount++;
		}
	}
	return factionCount < 2;
}

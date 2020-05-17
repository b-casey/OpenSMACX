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
#include "faction.h"
#include "base.h"
#include "council.h"
#include "game.h"
#include "map.h"
#include "technology.h"
#include "veh.h"

player *Players = (player *)0x00946A50;
player_data *PlayersData = (player_data *)0x0096C9E0;
faction_art *FactionArt = (faction_art*)0x0078E978;
rules_social_category *SocialCategory = (rules_social_category *)0x0094B000;
rules_social_effect *SocialEffect = (rules_social_effect *)0x00946580;
LPSTR *Mood = (LPSTR *)0x0094C9E4;
LPSTR *Repute = (LPSTR *)0x00946A30;
rules_might *Might = (rules_might *)0x0094C558;
rules_bonusname *BonusName = (rules_bonusname *)0x009461A8;
uint8_t *FactionCurrentBitfield = (uint8_t *)0x009A64E8; // Human controlled player bitfield?

/*
Purpose: Get Player's faction name adjective.
Original Offset: 0050B910
Return Value: Faction name adjective
Status: Complete
*/
LPSTR __cdecl get_adjective(int factionID) {
	return Players[factionID].nameAdjFaction;
}

/*
Purpose: Get Player's faction noun.
Original Offset: 0050B930
Return Value: Faction noun
Status: Complete
*/
LPSTR __cdecl get_noun(int factionID) {
	parse_set(Players[factionID].nounGender, Players[factionID].isNounPlural);
	return Players[factionID].nounFaction;
}

/*
Purpose: Check whether specified faction is nearing the diplomatic victory requirements to be able 
         to call a Supreme Leader vote. Optional 2nd parameter (0/-1 to disable) specifies a faction
		 to skip if they have pact with faction from 1st parameter.
Original Offset: 00539D40
Return Value: factionID nearing diplomatic victory or zero
Status: Complete
*/
uint32_t __cdecl aah_ooga(int factionID, int pactFactionID) {
	if (!(*GameRules & RULES_VICTORY_DIPLOMATIC)) {
		return 0; // Diplomatic Victory not allowed
	}
	uint32_t votesTotal = 0;
	for (uint32_t i = 1; i < MaxPlayerNum; i++) {
		votesTotal += council_votes(i);
	}
	uint32_t factionIDRet = 0;
	for (int i = 1; i < MaxPlayerNum; i++) {
		if (i != pactFactionID 
			&& (pactFactionID <= 0 || !(PlayersData[i].diploStatus[pactFactionID] & DSTATUS_PACT) 
				|| !(*GameRules & RULES_VICTORY_COOPERATIVE))) {
			uint32_t proposalPreq = Proposal[PROP_UNITE_SUPREME_LEADER].preqTech;
			if ((has_tech(proposalPreq, factionID)
				|| (proposalPreq >= 0 && (has_tech(Technology[proposalPreq].preqTech1, factionID)
					|| has_tech(Technology[proposalPreq].preqTech2, factionID))))
				&& council_votes(i) * 2 >= votesTotal && (!factionIDRet || i == factionID)) {
				factionIDRet = i;
			}
		}
	}
	return factionIDRet;
}

/*
Purpose: Human controlled player nearing endgame.
Original Offset: 00539E40
Return Value: Is human player nearing endgame? true/false
Status: Complete
*/
BOOL __cdecl climactic_battle() {
	uint32_t factionBits = FactionCurrentBitfield[0];
	for (uint32_t i = 1; i < MaxPlayerNum; i++) {
		if (factionBits & (1 << i) && PlayersData[i].cornerMarketTurn > *TurnCurrentNum) {
			return true; // Human controlled player initiated Corner Global Energy Market 
		}
	}
	if (aah_ooga(0, -1)) { // nearing Supreme Leader, these parameters will always return false
		return true; // TODO: Revisit in future once more end game code is complete. This may have 
		             //       been effectively disabled as a design decision rather than a bug.
	}
	if (ascending(0)) {
		for (uint32_t i = 1; i < MaxPlayerNum; i++) {
			if (factionBits & (1 << i) && (has_tech(Facility[FAC_PSI_GATE].preqTech, i)
				|| has_tech(Facility[FAC_VOICE_OF_PLANET].preqTech, i))) {
				return true; // Human controlled player has tech to build PSI Gates or VoP
			}
		}
	}
	return false;
}

/*
Purpose: Determine ideal unit count to protect faction's bases in the specified land region.
Original Offset: 00560D50
Return Value: Amount of defensive/offensive (?) units needed to guard region
Status: Complete
*/
uint32_t __cdecl guard_check(uint32_t factionID, uint32_t region) {
	if (region >= MaxRegionLandNum) {
		return 0;
	}
	int factor = 2 - PlayersData[factionID].AI_Fight;
	int planRegion = PlayersData[factionID].basePlanByRegion[region];
	if (planRegion == PLAN_COLONIZATION) {
		factor += 2;
	}
	else if (planRegion == PLAN_DEFENSIVE) {
		factor = 1; // 1-1 unit per base ratio
	}
	if (PlayersData[factionID].playerFlags & PFLAG_STRAT_DEF_OBJECTIVES) {
		factor = 1; // 1-1 unit per base ratio
	}
	return (PlayersData[factionID].baseCountByRegion[region] + factor - 1) / factor;
}

/*
Purpose: Validate whether each faction meets the requirements to have the Map revealed. Added some
         minor tweaks to improve performance.
Original Offset: 005A96D0
Return Value: n/a
Status: Complete
*/
void __cdecl see_map_check() {
	for (int factionID = 1; factionID < MaxPlayerNum; factionID++) {
		PlayersData[factionID].playerFlags &= ~PFLAG_MAP_REVEALED;
		uint32_t *satellites = &PlayersData[factionID].satSkyHydroLab;
		for (int i = 0; i < 4; i++, satellites++) {
			if (*satellites) {
				PlayersData[factionID].playerFlags |= PFLAG_MAP_REVEALED;
				break; // end satellite loop once set
			}
		}
		if (!(PlayersData[factionID].playerFlags & PFLAG_MAP_REVEALED)) { // skip Tech check if set
			for (int techID = 0; techID < MaxTechnologyNum; techID++) {
				if (Technology[techID].flags & TFLAG_REVEALS_MAP && has_tech(techID, factionID)) {
					PlayersData[factionID].playerFlags |= PFLAG_MAP_REVEALED;
				}
			}
		}
	}
}

/*
Purpose: Check to see whether provided faction can utilize a specific social category and model.
Original Offset: 005B4730
Return Value: Is social category/model available to faction? true/false
Status: Complete
*/
BOOL __cdecl society_avail(int socCategory, int socModel, int factionID) {
	if (Players[factionID].socAntiIdeologyCategory == socCategory 
		&& Players[factionID].socAntiIdeologyModel == socModel) {
		return false;
	}
	return has_tech(SocialCategory[socCategory].preqTech[socModel], factionID);
}
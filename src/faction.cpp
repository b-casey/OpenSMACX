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
#include "alpha.h"
#include "base.h"
#include "council.h"
#include "game.h"
#include "map.h"
#include "probe.h"
#include "technology.h"
#include "veh.h"

player *Players = (player *)0x00946A50;
player_data *PlayersData = (player_data *)0x0096C9E0;
faction_art *FactionArt = (faction_art *)0x0078E978;
rules_social_category *SocialCategory = (rules_social_category *)0x0094B000;
rules_social_effect *SocialEffect = (rules_social_effect *)0x00946580;
LPSTR *Mood = (LPSTR *)0x0094C9E4;
LPSTR *Repute = (LPSTR *)0x00946A30;
rules_might *Might = (rules_might *)0x0094C558;
rules_bonusname *BonusName = (rules_bonusname *)0x009461A8;
uint8_t *FactionsStatus = (uint8_t *)0x009A64E8;
uint32_t *FactionRankings = (uint32_t *)0x009A64EC; // [8]
uint32_t *RankingFactionIDUnk1 = (uint32_t *)0x009A650C;
uint32_t *RankingFactionIDUnk2 = (uint32_t *)0x009A6510;
uint32_t *FactionRankingsUnk = (uint32_t *)0x00945DD8; // [8]
int *DiploFriction = (int *)0x0093FA74; // not always bounded, should it be 0-20?
uint32_t *DiploFrictionFactionIDWith = (uint32_t *)0x0093FABC;
uint32_t *DiploFrictionFactionID = (uint32_t *)0x0093FAC0;

/*
Purpose: Check whether specified faction is a human player or computer controlled AI.
Original Offset: n/a
Return Value: Is faction a human? true/false
Status: Complete
*/
BOOL __cdecl is_human(uint32_t factionID) {
	return FactionsStatus[0] & (1 << factionID);
}

/*
Purpose: Check whether specified faction is alive or whether they've been eliminated.
Original Offset: n/a
Return Value: Is faction alive? true/false
Status: Complete
*/
BOOL __cdecl is_alive(uint32_t factionID) {
	return FactionsStatus[1] & (1 << factionID);
}

/*
Purpose: Check whether the primary faction has at least one of the specified treaties (bitfield) 
         with the secondary faction.
Original Offset: 005002F0
Return Value: Treaty status between the two factions, generally treated as a boolean
Status: Complete
*/
uint32_t __cdecl has_treaty(uint32_t factionID, uint32_t factionIDWith, uint32_t treaty) {
	return PlayersData[factionID].diploTreaties[factionIDWith] & treaty;
}

/*
Purpose: Get Player's faction name adjective.
Original Offset: 0050B910
Return Value: Faction name adjective
Status: Complete
*/
LPSTR __cdecl get_adjective(int factionID) {
	return Players[factionID].adjNameFaction;
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
Purpose: Determine whether automatic contact is enabled for net or PBEM games.
Original Offset: 00539160
Return Value: Is always contact enabled? true/false
Status: Complete
*/
BOOL __cdecl auto_contact() {
	if (*IsMultiplayerNet && Rules->TglHumanAlwaysContactNet) {
		return true;
	}
	return *IsMultiplayerPBEM && Rules->TglHumansAlwaysContactPBEM;
}

/*
Purpose: Determine if the overall dominant human faction is a minor threat based on base count.
Original Offset: 00539B70
Return Value: Is faction minor threat? true/false
Status: Complete
*/
BOOL __cdecl great_beelzebub(uint32_t factionID, BOOL isAggressive) {
	if (is_human(factionID) && FactionRankings[7] == factionID) {
		uint32_t basesThreat = (*TurnCurrentNum + 25) / 50;
		if (basesThreat < 4) {
			basesThreat = 4;
		}
		if (PlayersData[factionID].currentNumBases > basesThreat
			&& (PlayersData[factionID].diffLevel > DLVL_SPECIALIST
				|| *GameRules & RULES_INTENSE_RIVALRY || isAggressive)) {
			return true;
		}
	}
	return false;
}

/*
Purpose: Determine if specified faction is considered a threat based on game state and ranking.
Original Offset: 00539C00
Return Value: Is specified faction a threat? true/false
Status: Complete
*/
BOOL __cdecl great_satan(uint32_t factionID, BOOL isAggressive) {
	if (great_beelzebub(factionID, isAggressive)) {
		BOOL hasIntenseRiv = (*GameRules & RULES_INTENSE_RIVALRY);
		if (*TurnCurrentNum <= ((hasIntenseRiv ? 0 
			: (DLVL_TRANSCEND - PlayersData[factionID].diffLevel) * 50) + 100)) {
			return false;
		}
		if (climactic_battle() && aah_ooga(factionID, -1) == factionID) {
			return true;
		}
		uint32_t diffFactor, factor;
		if (hasIntenseRiv) {
			factor = 4;
			diffFactor = DLVL_TRANSCEND;
		}
		else if (PlayersData[factionID].diffLevel >= DLVL_LIBRARIAN 
			|| *GameRules & RULES_VICTORY_CONQUEST || *ObjectiveReqVictory <= 1000) {
			factor = 2;
			diffFactor = DLVL_LIBRARIAN;
		}
		else {
			factor = 1;
			diffFactor = DLVL_TALENT;
		}
		return (factor * FactionRankingsUnk[FactionRankings[7]] 
			>= diffFactor * FactionRankingsUnk[FactionRankings[6]]);
	}
	return false;
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
			&& (pactFactionID <= 0 || !has_treaty(i, pactFactionID, DTREATY_PACT)
				|| !(*GameRules & RULES_VICTORY_COOPERATIVE))) {
			int proposalPreq = Proposal[PROP_UNITE_SUPREME_LEADER].preqTech;
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
	for (uint32_t i = 1; i < MaxPlayerNum; i++) {
		if (is_human(i) && PlayersData[i].cornerMarketTurn > *TurnCurrentNum) {
			return true; // Human controlled player initiated Corner Global Energy Market
		}
	}
	if (aah_ooga(0, -1)) { // nearing Supreme Leader, these parameters will always return false
		return true; // TODO: Revisit in future once more end game code is complete. This may have
					 //       been effectively disabled as a design decision rather than a bug.
	}
	if (ascending(0)) {
		for (uint32_t i = 1; i < MaxPlayerNum; i++) {
			if (is_human(i) && (has_tech(Facility[FAC_PSI_GATE].preqTech, i)
				|| has_tech(Facility[FAC_VOICE_OF_PLANET].preqTech, i))) {
				return true; // Human controlled player has tech to build PSI Gates or VoP
			}
		}
	}
	return false;
}

/*
Purpose: Determine if the specified faction (AI only) is at game climax based on certain conditions.
Original Offset: 00539EF0
Return Value: Is at climax? true/false
Status: Complete - testing
*/
BOOL __cdecl at_climax(uint32_t factionID) {
	if (is_human(factionID) || *GameState & STATE_UNK_1 || *DiffLevelCurrent == DLVL_CITIZEN
		|| !climactic_battle()) {
		return false;
	}
	if (aah_ooga(factionID, factionID)) {
		return true;
	}
	for (uint32_t i = 1; i < MaxPlayerNum; i++) {
		if (i != factionID && PlayersData[factionID].cornerMarketTurn > *TurnCurrentNum) {
			if (has_treaty(factionID, i, DTREATY_PACT) || *GameRules & RULES_VICTORY_COOPERATIVE) {
				return true;
			}
		}
	}
	int transMostMinThem = 0, transMostMinUs = 0;
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (Base[i].queueProductionID[0] == -FAC_ASCENT_TO_TRANSCENDENCE) {
			int minAccum = Base[i].mineralsAccumulated;
			if (Base[i].factionIDCurrent != factionID) {
				if (transMostMinThem <= minAccum) {
					transMostMinThem = minAccum;
				}
			}
			else if (transMostMinUs <= minAccum) {
				transMostMinUs = minAccum;
			}
		}
	}
	for (uint32_t i = 0; i < MaxPlayerNum; i++) {
		if (i != factionID) {
			if (ascending(i) && !ascending(factionID)) { // both return same so irrelevant check
				return true;
			}
		}
	}
	return transMostMinUs && transMostMinUs > transMostMinThem;
}

/*
Purpose: Add friction between the two specified factions.
Original Offset: 0053A030
Return Value: n/a
Status: Complete
*/
void __cdecl cause_friction(uint32_t factionID, uint32_t factionIDWith, int friction) {
	uint32_t *diploFriction = &PlayersData[factionID].diploFriction[factionIDWith];
	*diploFriction = range(*diploFriction + friction, 0, 20);
	if (*DiploFrictionFactionID == factionID && *DiploFrictionFactionIDWith == factionIDWith) {
		*DiploFriction += friction; // not bounded?
	}
}

/*
Purpose: Normalize the diplomatic friction value into a mood offset.
Original Offset: 0053A090
Return Value: Mood (0-8)
Status: Complete
*/
uint32_t __cdecl get_mood(int friction) {
	if (friction <= 0) {
		return 0;
	}
	if (friction <= 2) {
		return 1;
	}
	if (friction <= 4) {
		return 2;
	}
	if (friction <= 8) {
		return 3;
	}
	if (friction <= 12) {
		return 4;
	}
	if (friction <= 15) {
		return 5;
	}
	if (friction <= 17) {
		return 6;
	}
	return (friction > 19) + 7;
}

/*
Purpose: Calculate the negative reputation the specified faction has with another.
Original Offset: 0053A100
Return Value: Bad reputation
Status: Complete
*/
uint32_t __cdecl reputation(uint32_t factionID, uint32_t factionIDWith) {
	return range(PlayersData[factionID].integrityBlemishes
		- PlayersData[factionID].diploUnk_1[factionIDWith], 0, 99);
}

/*
Purpose: Get the amount of patience the specified faction has with another faction.
Original Offset: 0053A150
Return Value: Patience
Status: Complete
*/
int __cdecl get_patience(uint32_t factionIDWith, uint32_t factionID) {
	if (has_treaty(factionID, factionIDWith, DTREATY_VENDETTA)) {
		return 1;
	}
	if (has_treaty(factionID, factionIDWith, DTREATY_PACT)) {
		return has_treaty(factionID, factionIDWith, DTREATY_HAVE_SURRENDERED) ? 500 : 6;
	}
	return (has_treaty(factionID, factionIDWith, DTREATY_TREATY) != 0) 
		- ((*DiploFriction + 3) / 8) + 3;
}

/*
Purpose: Calculate the amount of goodwill a loan will generate. This is used to reduce friction.
Original Offset: 0053A1C0
Return Value: Goodwill (friction reduction amount)
Status: Complete
*/
uint32_t __cdecl energy_value(uint32_t loanPrincipal) {
	uint32_t goodwill = 0, divisor = 2;
	for (int weight = 10, energy = loanPrincipal / 5; energy > 0; energy -= weight, weight = 20) {
		goodwill += ((weight >= 0) ? ((energy > weight) ? weight : energy) : 0) / divisor++;
	}
	return (goodwill + 4) / 5;
}

/*
Purpose: Set or unset the diplomatic treaty for the specified faction with another faction.
Original Offset: 0055BB30
Return Value: n/a
Status: Complete
*/
void __cdecl set_treaty(uint32_t factionID, uint32_t factionIDWith, uint32_t treaty, BOOL set) {
	if (set) {
		PlayersData[factionID].diploTreaties[factionIDWith] |= treaty;
		if (treaty & DTREATY_UNK_40) {
			PlayersData[factionID].diploMerc[factionIDWith] = 50;
		}
	}
	else {
		PlayersData[factionID].diploTreaties[factionIDWith] &= ~treaty;
	}
}

/*
Purpose: Set or unset the diplomatic agenda for the specified faction with another faction.
Original Offset: 0055BBA0
Return Value: n/a
Status: Complete
*/
void __cdecl set_agenda(uint32_t factionID, uint32_t factionIDWith, uint32_t agenda, BOOL set) {
	if (set) {
		PlayersData[factionID].diploAgenda[factionIDWith] |= agenda;
	}
	else {
		PlayersData[factionID].diploAgenda[factionIDWith] &= ~agenda;
	}
}

/*
Purpose: Determine ideal unit count to protect faction's bases in the specified land region.
Original Offset: 00560D50
Return Value: Amount of non-offensive units needed to guard region
Status: Complete
*/
uint32_t __cdecl guard_check(uint32_t factionID, uint32_t region) {
	if (region >= MaxRegionLandNum) {
		return 0;
	}
	int factor = 2 - PlayersData[factionID].AI_Fight;
	uint32_t planRegion = PlayersData[factionID].regionBasePlan[region];
	if (planRegion == PLAN_COLONIZATION) {
		factor += 2;
	}
	else if (planRegion == PLAN_DEFENSIVE) {
		factor = 1; // 1-1 unit per base ratio
	}
	if (PlayersData[factionID].playerFlags & PFLAG_STRAT_DEF_OBJECTIVES) {
		factor = 1; // 1-1 unit per base ratio
	}
	return (PlayersData[factionID].regionTotalBases[region] + factor - 1) / factor;
}

/*
Purpose: Add the specific goal to the faction's goals for the specified tile. Optional baseID param.
Original Offset: 00579A30
Return Value: n/a
Status:  Complete
*/
void __cdecl add_goal(uint32_t factionID, int type, int priority, int xCoord, int yCoord,
	int baseID) {
	if (!on_map(xCoord, yCoord)) {
		return;
	}
	for (int i = 0; i < MaxGoalsNum; i++) {
		goal &goals = PlayersData[factionID].goals[i];
		if (goals.xCoord == xCoord && goals.yCoord == yCoord && goals.type == type) {
			if (goals.priority <= priority) {
				goals.priority = (int16_t)priority;
			}
			return;
		}
	}
	int prioritySearch = 0, goalID = -1;
	for (int i = 0; i < MaxGoalsNum; i++) {
		goal &goals = PlayersData[factionID].goals[i];
		int typeCmp = goals.type, prirotyCmp = goals.priority;
		if (typeCmp < 0 || prirotyCmp < priority) {
			int cmp = typeCmp >= 0 ? 0 : 1000;
			if (!cmp) {
				cmp = prirotyCmp > 0 ? 20 - prirotyCmp : prirotyCmp + 100;
			}
			if (cmp > prioritySearch) {
				prioritySearch = cmp;
				goalID = i;
			}
		}
	}
	if (goalID >= 0) {
		goal &goals = PlayersData[factionID].goals[goalID];
		goals.type = (int16_t)type;
		goals.priority = (int16_t)priority;
		goals.xCoord = xCoord;
		goals.yCoord = yCoord;
		goals.baseID = baseID;
	}
}

/*
Purpose: Add the specific site to the faction's site goals for the specified tile.
Original Offset: 00579B70
Return Value: n/a
Status: Complete
*/
void __cdecl add_site(uint32_t factionID, int type, int priority, int xCoord, int yCoord) {
	if ((xCoord ^ yCoord) & 1 && *GameState & STATE_DEBUG_MODE) {
		danger("Bad SITE", "", xCoord, yCoord, type);
	}
	for (int i = 0; i < MaxSitesNum; i++) {
		goal &sites = PlayersData[factionID].sites[i];
		if (sites.xCoord == xCoord && sites.yCoord == yCoord && sites.type == type) {
			if (sites.priority <= priority) {
				sites.priority = (int16_t)priority;
			}
			return;
		}
	}
	int prioritySearch = 0, siteID = -1;
	for (int i = 0; i < MaxSitesNum; i++) {
		goal &sites = PlayersData[factionID].sites[i];
		int typeCmp = sites.type, prirotyCmp = sites.priority;
		if (typeCmp < 0 || prirotyCmp < priority) {
			int cmp = typeCmp >= 0 ? 0 : 1000;
			if (!cmp) {
				cmp = 20 - prirotyCmp;
			}
			if (cmp > prioritySearch) {
				prioritySearch = cmp;
				siteID = i;
			}
		}
	}
	if (siteID >= 0) {
		goal &sites = PlayersData[factionID].sites[siteID];
		sites.type = (int16_t)type;
		sites.priority = (int16_t)priority;
		sites.xCoord = xCoord;
		sites.yCoord = yCoord;
		add_goal(factionID, type, priority, xCoord, yCoord, -1);
	}
}

/*
Purpose: Check if a goal exists at the tile for the specified faction and type.
Original Offset: 00579CC0
Return Value: Does specific goal exist for faction at tile? true/false
Status: Complete
*/
BOOL __cdecl at_goal(uint32_t factionID, int type, int xCoord, int yCoord) {
	for (int i = 0; i < MaxGoalsNum; i++) {
		goal &goals = PlayersData[factionID].goals[i];
		if (goals.xCoord == xCoord && goals.yCoord == yCoord && goals.type == type) {
			return true;
		}
	}
	return false;
}

/*
Purpose: Check if a site exists at the tile for the specified faction and type.
Original Offset: 00579D20
Return Value: Does specific site exist for faction at tile? true/false
Status: Complete
*/
BOOL __cdecl at_site(uint32_t factionID, int type, int xCoord, int yCoord) {
	for (int i = 0; i < MaxSitesNum; i++) {
		goal &sites = PlayersData[factionID].sites[i];
		if (sites.xCoord == xCoord && sites.yCoord == yCoord && sites.type == type) {
			return true;
		}
	}
	return false;
}

/*
Purpose: Wipe all goals for the specified faction then recreate any active site related goals.
Original Offset: 00579D80
Return Value: n/a
Status: Complete
*/
void __cdecl wipe_goals(uint32_t factionID) {
	for (int i = 0; i < MaxGoalsNum; i++) {
		goal &goals = PlayersData[factionID].goals[i];
		int16_t priority = goals.priority;
		if (priority < 0) {
			goals.type = AI_GOAL_UNUSED;
		}
		else {
			goals.priority = -priority;
		}
	}
	for (int i = 0; i < MaxSitesNum; i++) {
		goal &sites = PlayersData[factionID].sites[i];
		int16_t type = sites.type;
		if (type >= 0) {
			add_goal(factionID, type, sites.priority, sites.xCoord, sites.yCoord, -1);
		}
	}
}

/*
Purpose: Initialize all goals for the specified faction.
Original Offset: 00579E00
Return Value: n/a
Status: Complete
*/
void __cdecl init_goals(uint32_t factionID) {
	for (int i = 0; i < MaxGoalsNum; i++) {
		goal &goals = PlayersData[factionID].goals[i];
		goals.type = -1;
		goals.priority = 0;
		goals.xCoord = 0;
		goals.yCoord = 0;
		goals.baseID = 0;
	}
	for (int i = 0; i < MaxSitesNum; i++) {
		goal &sites = PlayersData[factionID].sites[i];
		sites.type = -1;
		sites.priority = 0;
		sites.xCoord = 0;
		sites.yCoord = 0;
		sites.baseID = 0;
	}
}

/*
Purpose: Delete sites of the specified type within proximity of the tile along with related goals.
Original Offset: 00579E70
Return Value: n/a
Status: Complete
*/
void __cdecl del_site(uint32_t factionID, int type, int xCoord, int yCoord, int proximity) {
	for (int i = 0; i < MaxSitesNum; i++) {
		goal &sites = PlayersData[factionID].sites[i];
		if (sites.type == type) {
			int dist = vector_dist(xCoord, yCoord, sites.xCoord, sites.yCoord);
			if (dist <= proximity) {
				sites.type = AI_GOAL_UNUSED;
				sites.priority = 0;
				for (int j = 0; j < MaxGoalsNum; j++) {
					goal &goalCompare = PlayersData[factionID].goals[j];
					if (goalCompare.xCoord == sites.xCoord && goalCompare.yCoord == sites.yCoord &&
						goalCompare.type == type) {
						goalCompare.type = AI_GOAL_UNUSED;
					}
				}
			}
		}
	}
}

/*
Purpose: Calculate the cost for the faction to corner the Global Energy Market (Economic Victory).
Original Offset: 0059EE50
Return Value: Cost to corner the Global Energy Market
Status: Complete
*/
uint32_t __cdecl corner_market(uint32_t factionID) {
	int cost = 0;
	for (int i = 0; i < *BaseCurrentCount; i++) {
		uint32_t targetFactionID = Base[i].factionIDCurrent;
		if (targetFactionID != factionID) {
			if (!has_treaty(targetFactionID, factionID, DTREATY_PACT)
				|| !has_treaty(targetFactionID, factionID, DTREATY_HAVE_SURRENDERED)) {
				cost += mind_control(i, factionID, true);
			}
		}
	}
	return (cost < 1000) ? 1000 : cost;
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
		uint32_t *satellites = &PlayersData[factionID].satellitesNutrient;
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
Purpose: Calculate the base social engineering modifiers for the specified faction.
Original Offset: 005B0D70
Return Value: n/a
Status: Complete - testing
*/
void __cdecl compute_faction_modifiers(uint32_t factionID) {
	ZeroMemory(&PlayersData[factionID].socEffectBase, sizeof(social_effect));
	int count = Players[factionID].factionBonusCount;
	for (int i = 0; i < count; i++) {
		if (Players[factionID].factionBonusID[i] & RULE_SOCIAL) {
			*(&PlayersData[factionID].socEffectBase.economy
				+ Players[factionID].factionBonusVal1[i]) += Players[factionID].factionBonusVal2[i];
		}
	}
}

/*
Purpose: Calculate the social engineering effect modifiers for the specified faction.
Original Offset: 005B4210
Return Value: n/a
Status: Complete - testing
*/
void __cdecl social_calc(social_category *category, social_effect *effect, uint32_t factionID, 
	BOOL UNUSED(tgl1), BOOL isQuickCalc) {
	ZeroMemory(effect, sizeof(social_effect));
	BOOL hasCloningVatSP = has_project(SP_CLONING_VATS, factionID);
	BOOL hasNetBackboneSP = has_project(SP_NETWORK_BACKBONE, factionID);
	for (int i = 0; i < MaxSocialCatNum; i++) {
		uint32_t modelVal = *(&category->politics + i);
		for (int j = 0; j < MaxSocialEffectNum; j++) {
			int effectVal = *(&SocialCategory[i].modelEffect[modelVal].economy + j);
			if (effectVal < 0) {
				if ((i == 3 && j == 1 && hasNetBackboneSP) ||
					(((i == 3 && j == 3) || (i == 2 && j == 1)) && hasCloningVatSP)) {
					effectVal = 0;
				}
				else {
					for (int k = 0; k < Players[factionID].factionBonusCount; k++) {
						if (Players[factionID].factionBonusVal1[k] == i
							&& Players[factionID].factionBonusVal2[k] == j) {
							if (Players[factionID].factionBonusID[i] == RULE_IMPUNITY) {
								*(&effect->economy + j) -= effectVal;
							}
							else if (Players[factionID].factionBonusID[i] == RULE_PENALTY) {
								*(&effect->economy + j) += effectVal;
							}
						}
					}
				}
			}
			*(&effect->economy + j) += effectVal;
		}
	}
	if (!isQuickCalc) {
		if (has_project(SP_ASCETIC_VIRTUES, factionID)) {
			effect->police++;
		}
		if (has_project(SP_LIVING_REFINERY, factionID)) {
			effect->support += 2;
		}
		if (has_temple(factionID)) {
			effect->planet++;
			if (Players[factionID].ruleFlags & RFLAG_ALIEN) {
				effect->research++; // bonus documented in conceptsx.txt but not manual
			}
		}
		int count = 11;
		social_effect *effectCalc = effect, *effectBase = &PlayersData[factionID].socEffectBase;
		do {
			effectCalc->economy += effectBase->economy;
			effectCalc++;
			effectBase++;
			--count;
		} while (count);

		for (int i = 0; i < Players[factionID].factionBonusCount; i++) {
			if (Players[factionID].factionBonusID[i] == RULE_IMMUNITY) {
				*(&effect->economy + Players[factionID].factionBonusVal1[i]) 
					= range(*(&effect->economy + Players[factionID].factionBonusVal1[i]), 0, 999);
			}
			else if (Players[factionID].factionBonusID[i] == RULE_ROBUST) {
				int effectVal = *(&effect->economy + Players[factionID].factionBonusVal1[i]);
				if (effectVal < 0) {
					*(&effect->economy + Players[factionID].factionBonusVal1[i]) = effectVal / 2;
				}
			}
		}
	}
}

/*
Purpose: Handle the social engineering turn upkeep for the specified faction.
Original Offset: 005B44D0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl social_upkeep(uint32_t factionID) {
	for (int i = 0; i < MaxSocialCatNum; i++) {
		*(&PlayersData[factionID].socCategoryActive.politics + i) =
			*(&PlayersData[factionID].socCategoryPending.politics + i);
	}
	social_calc(&PlayersData[factionID].socCategoryPending, 
		&PlayersData[factionID].socEffectPending, factionID, false, false);
	social_calc(&PlayersData[factionID].socCategoryPending,
		&PlayersData[factionID].socEffectActive, factionID, false, false);
	social_calc(&PlayersData[factionID].socCategoryPending,
		&PlayersData[factionID].socEffectTemp, factionID, true, false);
	PlayersData[factionID].socUpheavalCostPaid = 0;
}

/*
Purpose: Calculate the cost of social upheaval for the specified faction.
Original Offset: 005B4550
Return Value: Social upheaval cost
Status: Complete
*/
uint32_t __cdecl social_upheaval(uint32_t factionID, social_category *categoryNew) {
	uint32_t changeCount = 0;
	for (int i = 0; i < MaxSocialCatNum; i++) {
		if (*(&categoryNew->politics + i) != 
			*(&PlayersData[factionID].socCategoryActive.politics + i)) {
			changeCount++;
		}
	}
	if (!changeCount) {
		return 0;
	}
	changeCount++;
	uint32_t diffLvl = is_human(factionID) ? PlayersData[factionID].diffLevel : DLVL_LIBRARIAN;
	uint32_t cost = changeCount * changeCount * changeCount * diffLvl;
	if (Players[factionID].ruleFlags & RFLAG_ALIEN) {
		cost += cost / 2;
	}
	return cost;
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

/*
Purpose: Calculate social engineering for AI.
Original Offset: 005B4790
Return Value: n/a
Status: Complete - testing (likely has multiple issues due to length + complexity)
*/
void __cdecl social_ai(uint32_t factionID, int growthVal, int techVal, int wealthVal, int powerVal, 
	social_category *output) {
	// setup
	int fightVal;
	int unkVal1 = -1;
	if (!output) {
		if (is_human(factionID)) {
			return;
		}
		else {
			growthVal = PlayersData[factionID].AI_Growth;
			techVal = PlayersData[factionID].AI_Tech;
			wealthVal = PlayersData[factionID].AI_Wealth;
			powerVal = PlayersData[factionID].AI_Power;
			fightVal = PlayersData[factionID].AI_Fight;
		}
	}
	else if (growthVal < 0) {
		return;
	}
	else if (growthVal > 100) {
		unkVal1 = growthVal - 100;
		growthVal = PlayersData[factionID].AI_Growth;
		techVal = PlayersData[factionID].AI_Tech;
		wealthVal = PlayersData[factionID].AI_Wealth;
		powerVal = PlayersData[factionID].AI_Power;
		fightVal = PlayersData[factionID].AI_Fight;
	}
	else {
		if (!powerVal) {
			fightVal = -(wealthVal != 0);
		}
		else {
			fightVal = 1;
		}
	}
	// purpose?? loop to set unkVal2 to 256
	uint32_t unkVal2 = 4;
	for (int i = 3; i >= 0; i--) {
		unkVal2 *= 4;
	}
	// future pop growth
	int popGoalGrowth = 0;
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (Base[i].factionIDCurrent == factionID) {
			popGoalGrowth += pop_goal(i) - Base[i].populationSize;
		}
	}
	// plan and enemy region metrics
	uint32_t unkSum1 = 0;
	uint32_t unkCount1 = 0;
	uint32_t unkCount2 = 0;
	uint32_t unkSum2 = 0;
	for (int region = 1; region < MaxRegionLandNum; region++) {
		if (!bad_reg(region)) {
			uint8_t baseCount = PlayersData[factionID].regionTotalBases[region];
			if (baseCount) {
				uint8_t plan = PlayersData[factionID].regionBasePlan[region];
				if (plan == PLAN_DEFENSIVE || PlayersData[factionID].unk_77[region] & 0x400) {
					unkSum1 += PlayersData[factionID].regionTotalCombatVehs[region] * 2;
				}
				else if (plan == PLAN_OFFENSIVE) {
					unkSum1 += PlayersData[factionID].regionTotalCombatVehs[region];
					unkCount2++;
				}
				else if (plan != PLAN_COLONIZATION) {
					unkCount1++;
				}
				if (plan <= PLAN_DEFENSIVE) {
					for (uint32_t f = 1; f < MaxPlayerNum; f++) {
						if (f != factionID) {
							if (has_treaty(factionID, f, DTREATY_VENDETTA | DTREATY_WANT_REVENGE)
								&& PlayersData[f].enemyBestPsiOffense 
								>= PlayersData[f].enemyBestWeaponValue 
								&& PlayersData[f].protoID_Active[BSC_MIND_WORMS] > 1) {
								unkSum2 += baseCount; // not used
							}
						}
					}
				}
			}
		}
	}
	if (output && powerVal && unkSum1 < 1) {
		unkSum1 = 1;
	}
	// tech ranking
	uint32_t unkVal3 = 1;
	if (*GameState & 0x200 // set in rankings(), related to intense riv + end game
		&& PlayersData[factionID].ranking < 6) {
		int techRankDiff = PlayersData[*RankingFactionIDUnk1].techRanking / 2
			- PlayersData[factionID].techRanking / 2;
		if (techRankDiff > 5) {
			unkVal3 = 2;
		}
		if (techRankDiff > 10) {
			unkVal3++;
		}
	}
	// search / calc
	int unkVal4 = -9999; // search val?
	int socCatBits = -1;
	// unkSum2 = 0 > used as iterator, optimization re-use, var unused
	for (uint32_t i = 0; i < unkVal2; i++) {
		social_category socCat;
		social_effect socEff;
		int k = i;
		for (int j = 0; j < MaxSocialCatNum; j++) {
			int model = k & 3;
			*(&socCat.politics + i) = model;
			if (model) {
				if (Players[factionID].socAntiIdeologyCategory == j
					|| Players[factionID].socAntiIdeologyModel == model
					|| !has_tech(SocialCategory[model].preqTech[j], factionID)) {
					break;
				}
			}
			k >>= 2;
		}
		do_all_non_input();
		social_calc(&socCat, &socEff, factionID, false, false);
		int unkVal6 = 0;
		if (unkVal1 >= 0) {
			unkVal6 = *(&socEff.economy + unkVal1) * 1000;
		}
		BOOL hasIdeology = false;
		int ideologyCat = Players[factionID].socIdeologyCategory;
		if (ideologyCat >= 0) {
			int ideologyMod = Players[factionID].socIdeologyModel;
			if (ideologyMod) {
				int unkVal7 = *(&socCat.politics + ideologyCat);
				if (!unkVal7 && unkVal7 == ideologyMod) {
					unkVal6 += PlayersData[factionID].currentNumBases;
				}
				else {
					hasIdeology = true;
				}
			}
		}
		int ideologyEff = Players[factionID].socAntiIdeologyEffect;
		if (ideologyEff >= 0) {
			uint32_t unkVal8 = *(&socEff.economy + ideologyEff);
			if (unkVal8 > 0) {
				unkVal6 += PlayersData[factionID].currentNumBases;
			}
			if (unkVal8 < 0) {
				hasIdeology = true;
			}
		}
		if (!hasIdeology) {
			// economy
			int efficiencyVal = PlayersData[factionID].unk_46[range(4 - socEff.efficiency, 0, 8)];
			int econVal = socEff.economy;
			int econWeight = 0;
			if (econVal < 2) {
				if (econVal <= 0) {
					if (econVal < -1) {
						for (int region = 0; region < MaxContinentNum; region++) {
							econWeight -= (socEff.economy + 1)
								* PlayersData[factionID].regionTotalBases[region]
								* ((PlayersData[factionID].regionBasePlan[region] != PLAN_DEFENSIVE)
									+ 1);
						}
						econWeight /= (unkSum1 + 1);
					}
					else {
						econWeight = -1;
					}
				}
				else {
					for (int region = 0; region < MaxContinentNum; region++) {
						econWeight += PlayersData[factionID].regionTotalBases[region]
							* ((PlayersData[factionID].regionBasePlan[region] != PLAN_DEFENSIVE) 
								+ 1);
					}
					econWeight /= (unkSum1 + 1);
				}
			}
			else {
				if (econVal > 4) {
					econVal = 4;
				}
				econWeight = PlayersData[factionID].unk_47 + econVal * 2 - 4 / (unkSum1 + 1);
			}
			for (; econWeight > 0; econWeight--, efficiencyVal--) {
				if (efficiencyVal <= 0) {
					break;
				}
			}
			if (!unkSum1) {
				econWeight *= 2;
			}
			econWeight *= ((PlayersData[factionID].AI_Fight < 0 && unkSum1 < 2) 
				+ techVal * 2 + wealthVal + 1);
			if (output) {
				if (wealthVal || techVal) {
					econWeight *= 2;
				}
				if (growthVal || powerVal) {
					econWeight /= 2;
				}
			}
			else if (wealthVal && !powerVal && !growthVal) {
				econWeight *= 2;
			}
			unkVal6 += econWeight / unkVal3;
			// support
			int supportVal = range(socEff.support + 4, 0, 7);
			int supportWeight = PlayersData[factionID].unk_38[supportVal];
			if (socEff.support <= -4) {
				supportWeight *= 3;
			}
			if (unkSum1) {
				supportWeight *= 2;
				if (socEff.support <= -3) {
					supportWeight *= 2;
				}
				if ((socEff.support == -1 || socEff.support == -2) && socEff.economy < 2) {
					supportWeight += supportWeight / 2;
				}
			}
			if (output) {
				if (powerVal) {
					supportWeight *= 3;
				}
				if (growthVal) {
					supportWeight *= 2;
				}
				if (techVal) {
					supportWeight /= 2;
				}
			}
			else {
				if (fightVal > 0) {
					supportWeight *= 2;
				}
				if (growthVal > 0) {
					supportWeight += supportWeight / 2;
				}
				if (wealthVal > 0) {
					supportWeight /= 2;
				}
				if (techVal > 0) {
					supportWeight += supportWeight / -4;
				}
				if (powerVal > 0) {
					supportWeight += supportWeight / 2;
				}
			}
			unkVal6 -= supportWeight;
			// morale
			int moraleVal = range(socEff.morale, -4, 4) * 2;
			if (moraleVal < -6) {
				moraleVal++;
			}
			if (moraleVal < -2) {
				moraleVal++;
			}
			if (moraleVal > 6) {
				moraleVal--;
			}
			if (moraleVal > 2) {
				moraleVal--;
			}
			int moraleWeight = (unkVal3 * moraleVal * (fightVal + 2) * (unkSum1 + 1)
				* (PlayersData[factionID].currentNumBases + PlayersData[factionID].unk_48)) / 8;
			if (output) {
				if (powerVal) {
					moraleWeight *= 3;
				}
				if (growthVal || wealthVal) {
					moraleWeight /= 2;
				}
			}
			else {
				if (fightVal > 1) {
					moraleWeight *= 2;
				}
				if (powerVal && !growthVal && !wealthVal) {
					moraleWeight *= 2;
				}
				if (fightVal < 0 && !powerVal && (wealthVal || techVal) && !unkSum1) {
					moraleWeight /= 2;
				}
			}
			unkVal6 += moraleWeight;
			// efficiency
			int efficWeight = efficiencyVal;
			if (socEff.efficiency == -3) {
				efficWeight = 2 * efficiencyVal;
			}
			else if (socEff.efficiency <= -4) {
				efficWeight = 4 * efficiencyVal;
			}
			if (!unkSum1) {
				efficWeight *= 2;
			}
			if (output) {
				if (techVal) {
					efficWeight *= 3;
				}
				if (growthVal || powerVal) {
					efficWeight /= 2;
				}
			}
			else {
				if (!wealthVal && !techVal) {
					if (powerVal) {
						efficWeight /= 2;
					}
				}
				else if (powerVal) {
					if (!wealthVal) {
						efficWeight /= 2;
					}
				}
				else if (!growthVal) {
					efficWeight *= 2;
				}
			}
			unkVal6 -= efficWeight;
			// growth
			int growthWeight = (socEff.growth * (unkCount1 + 1) * ((growthVal + 1) * 2 - wealthVal)
				* (popGoalGrowth + PlayersData[factionID].nutrientSurplusTotal)) / 5;
			if (aah_ooga(factionID, factionID)) {
				growthWeight *= 2;
			}
			if (Players[factionID].rulePopulation > 0) {
				growthWeight /= 2;
			}
			if (output) {
				if (growthVal || wealthVal) {
					growthWeight *= 2;
				}
				if (techVal) {
					growthWeight /= 2;
				}
			}
			else {
				if (growthVal && powerVal) {
					growthWeight *= 2;
				}
			}
			unkVal6 += growthWeight;
			// police
			int policeWeight = range(socEff.police, -2, 2) * PlayersData[factionID].currentNumBases 
				* (unkCount2 * 2 + 1);
			if (PlayersData[factionID].techRanking < PlayersData[*RankingFactionIDUnk2].techRanking
				&& PlayersData[factionID].ranking < PlayersData[*RankingFactionIDUnk2].ranking) {
				policeWeight *= 2;
			}
			if (PlayersData[factionID].techRanking < PlayersData[*RankingFactionIDUnk1].techRanking
				&& PlayersData[factionID].ranking < PlayersData[*RankingFactionIDUnk1].ranking) {
				policeWeight /= 2;
			}
			if (output && (powerVal || growthVal)) {
				policeWeight *= 2;
			}
			unkVal6 += policeWeight / (2 - fightVal);
			// talent
			int talentWeight = PlayersData[factionID].unk_39[range(socEff.talent + 4, 0, 7)]
				[range(socEff.police + 5, 0, 8)];
			if (unkSum1) {
				talentWeight += socEff.police * PlayersData[factionID].currentNumBases;
			}
			if (output) {
				if (powerVal || growthVal) {
					talentWeight *= 2;
				}
				if (wealthVal) {
					talentWeight /= 2;
				}
			}
			else {
				if (growthVal && techVal) {
					talentWeight *= 2;
				}
				if (powerVal && (techVal || wealthVal)) {
					talentWeight /= 2;
				}
			}
			unkVal6 += talentWeight;
			// planet
			int planetWeight = (4 - socEff.planet) * PlayersData[factionID].unk_49 * 4 / 4; // ?
			if (PlayersData[factionID].socEffectBase.planet > 0
				|| PlayersData[factionID].bestPsiOffense > PlayersData[factionID].bestWeaponValue) {
				planetWeight += PlayersData[factionID].totalMilVeh;
			}
			else if (unkVal3 > 1) {
				planetWeight /= 2;
			}
			if (output) {
				if (growthVal) {
					planetWeight *= 2;
				}
				if (wealthVal) {
					planetWeight /= 2;
				}
			}
			else {
				if (powerVal) {
					if (growthVal) {
						planetWeight /= 2;
					}
					if ((techVal || wealthVal) && !growthVal) {
						planetWeight /= 2;
					}
				}
				else {
					if (growthVal) {
						if (techVal) {
							planetWeight *= 2;
							if (!wealthVal && growthVal) {
								planetWeight /= 2;
							}
						}
						else {
							if (growthVal) {
								planetWeight /= 2;
							}
						}
					}
					else {
						if ((techVal || wealthVal) && growthVal) {
							planetWeight /= 2;
						}
					}
				}
				unkVal6 -= planetWeight;
				// research
				int researchWeight = range(socEff.research, -5, 5) 
					* PlayersData[factionID].labsTotal / 10;
				if (!unkSum1) {
					researchWeight *= 2;
				}
				if (output) {
					if (techVal) {
						researchWeight *= 2;
					}
					if (powerVal) {
						researchWeight /= 2;
					}
				}
				unkVal6 += researchWeight;
				// industry
				uint32_t unkVal3Temp = range(unkVal3, 1, 2);
				int industryWeight = socEff.industry * unkVal3Temp
					* PlayersData[factionID].popTotal * (2 * (wealthVal + unkSum1) + 1) / 10;
				if (powerVal) {
					industryWeight *= 2;
				}
				if (wealthVal) {
					industryWeight += industryWeight / 2;
				}
				if (techVal) {
					industryWeight /= 2;
				}
				if (fightVal < 0 && !wealthVal) {
					industryWeight /= 2;
				}
				unkVal6 += industryWeight;
				// why is this before probe? significance?
				unkVal6 += social_upheaval(factionID, &socCat) / -3;
				// probe
				if (socEff.probe) {
					int probeWeight;
					for (uint32_t f = 0; f < MaxPlayerNum; f++) {
						if (f != factionID) {
							probeWeight = PlayersData[i].mindControlTotal / 4 + 1;
							if (has_treaty(factionID, i, DTREATY_PACT)) {
								probeWeight /= 2;
							}
							if (!has_treaty(factionID, i, DTREATY_WANT_REVENGE)) {
								if (has_treaty(factionID, i, DTREATY_TREATY)) {
									probeWeight /= 2;
								}
								if (!has_treaty(factionID, i, DTREATY_COMMLINK)) {
									probeWeight /= 2;
								}
								if (!has_treaty(factionID, i, DTREATY_VENDETTA)) {
									probeWeight /= 2;
								}
							}
							probeWeight = ((PlayersData[i].currentNumBases
								+ PlayersData[factionID].currentNumBases)
								* range(socEff.probe, -2, 3) * probeWeight) / 2;
							if (techVal || wealthVal) {
								probeWeight *= 2;
							}
							unkVal6 += probeWeight;
						}
					}
				}
				if (unkVal6 >= unkVal4) {
					unkVal4 = unkVal6;
					socCatBits = unkSum2;
				}
			}
		}
	}
	if (socCatBits < 0) {
		if (output) {
			for (int i = 0; i < MaxSocialCatNum; i++) {
				*(&output->politics + i)
					= *(&PlayersData[factionID].socCategoryPending.politics + i);
			}
		}
	}
	else {
		if (!output) {
			BOOL noCatChange = true;
			for (int i = 0; i < MaxSocialCatNum; i++) {
				uint32_t catBit = socCatBits & 3;
				if (catBit != *(&PlayersData[factionID].socCategoryPending.politics + i)) {
					*(&PlayersData[factionID].socCategoryPending.politics + i) = catBit;
					noCatChange = false;
				}
				socCatBits >>= 2;
			}
			if (!noCatChange) {
				social_set(factionID);
				uint32_t cost 
					= social_upheaval(factionID, &PlayersData[factionID].socCategoryPending);
				PlayersData[factionID].energyReserves 
					= range(PlayersData[factionID].energyReserves - cost, 0, 999999999);
				PlayersData[factionID].socUpheavalCostPaid += cost;
				if (!is_human(factionID)) {
					consider_designs(factionID);
				}
			}
		}
		else {
			for (int i = 0; i < MaxSocialCatNum; i++) {
				*(&output->politics + i) = socCatBits & 3;
				socCatBits >>= 2;
			}
		}
	}
}

/*
Purpose: Calculate specified faction's best available weapon and armor ratings as well as the
		 fastest moving ground Veh chassis. Compare these capabilities to faction's best opponent
		 capabilities based on current diplomacy.
Original Offset: 00560DD0
Return Value: n/a
Status: Complete - testing / WIP
*/
void __cdecl enemy_capabilities(uint32_t factionID) {
	BOOL hasWorms = veh_avail(BSC_MIND_WORMS, factionID, -1);
	PlayersData[factionID].bestPsiOffense = hasWorms ? weap_strat(WPN_PSI_ATTACK, factionID) : 0;
	PlayersData[factionID].bestWeaponValue = 1;
	for (int i = 0; i < MaxWeaponNum; i++) {
		if (has_tech(Weapon[i].preqTech, factionID) && Weapon[i].offenseRating < 99) {
			int weapVal = weap_strat(i, factionID);
			if (Weapon[i].offenseRating < 0 && weapVal > PlayersData[factionID].bestPsiOffense) {
				PlayersData[factionID].bestPsiOffense = weapVal;
			}
			if (weapVal > PlayersData[factionID].bestWeaponValue) {
				PlayersData[factionID].bestWeaponValue = weapVal;
			}
		}
	}
	PlayersData[factionID].bestPsiDefense = hasWorms ? arm_strat(ARM_PSI_DEFENSE, factionID) : 0;
	PlayersData[factionID].bestArmorValue = 1;
	for (int i = 0; i < MaxArmorNum; i++) {
		if (has_tech(Armor[i].preqTech, factionID)) {
			int armVal = arm_strat(i, factionID);
			if (Armor[i].defenseRating < 0 && armVal > PlayersData[factionID].bestPsiDefense) {
				PlayersData[factionID].bestPsiDefense = armVal;
			}
			if (armVal > PlayersData[factionID].bestArmorValue) {
				PlayersData[factionID].bestArmorValue = armVal;
			}
		}
	}
	PlayersData[factionID].bestLandSpeed = 1;
	for (int i = 0; i < MaxChassisNum; i++) {
		if (has_tech(Chassis[i].preqTech, factionID) && Chassis[i].triad == TRIAD_LAND) {
			if (Chassis[i].speed > PlayersData[factionID].bestLandSpeed) {
				PlayersData[factionID].bestLandSpeed = Chassis[i].speed;
			}
		}
	}
	PlayersData[factionID].enemyBestWeaponValue = 0;
	PlayersData[factionID].enemyBestArmorValue = 0;
	PlayersData[factionID].enemyBestLandSpeed = 0;
	PlayersData[factionID].enemyBestPsiOffense = 0;
	PlayersData[factionID].enemyBestPsiDefense = 0;
	for (int i = 0; i < 4 && !PlayersData[factionID].enemyBestWeaponValue; i++) {
		// 1st pass: vendetta, no treaty, has commlink
		// 2nd pass: no treaty, has commlink
		// 3rd pass: has commlink
		// 4th pass: any non-pact faction
		for (uint32_t j = 1; j < MaxPlayerNum; j++) {
			if (j != factionID
				&& !has_treaty(i, j, DTREATY_PACT)
				&& ((!i && has_treaty(i, j, DTREATY_VENDETTA) && !has_treaty(i, j, DTREATY_TREATY)
					&& has_treaty(i, j, DTREATY_COMMLINK)) 
					|| (i == 1 && !has_treaty(i, j, DTREATY_TREATY) 
						&& has_treaty(i, j, DTREATY_COMMLINK))
					|| (i == 2 && has_treaty(i, j, DTREATY_COMMLINK)) || (i == 3))) {
				if (PlayersData[factionID].enemyBestWeaponValue < PlayersData[j].bestWeaponValue) {
					PlayersData[factionID].enemyBestWeaponValue = PlayersData[j].bestWeaponValue;
				}
				if (PlayersData[factionID].enemyBestArmorValue < PlayersData[j].bestArmorValue) {
					PlayersData[factionID].enemyBestArmorValue = PlayersData[j].bestArmorValue;
				}
				if (PlayersData[factionID].enemyBestLandSpeed < PlayersData[j].bestLandSpeed) {
					PlayersData[factionID].enemyBestLandSpeed = PlayersData[j].bestLandSpeed;
				}
				if (PlayersData[factionID].enemyBestPsiOffense < PlayersData[j].bestPsiOffense) {
					PlayersData[factionID].enemyBestPsiOffense = PlayersData[j].bestPsiOffense;
				}
				if (PlayersData[factionID].enemyBestPsiDefense < PlayersData[j].bestPsiDefense) {
					PlayersData[factionID].enemyBestPsiDefense = PlayersData[j].bestPsiDefense;
				}
			}
		}
	}
}

void __cdecl enemy_capabilities_t(uint32_t factionID) {
	// * PSI could potentially be best weapon?
	// * PSI should always be last Weapon
	// * faction order will affect initial run through
	// > potential fix would be to calc all factions at once before enemy best compares

	//BOOL hasWorms = veh_avail(BSC_MIND_WORMS, factionID, -1);
	//PlayersData[factionID].bestPsiAtkVal = hasWorms ? weap_strat(WPN_PSI_ATTACK, factionID) : 0;
	PlayersData[factionID].bestWeaponValue = 1;
	for (int i = 0; i < MaxWeaponNum; i++) {
		if (has_tech(Weapon[i].preqTech, factionID) && Weapon[i].offenseRating < 99) {
			int weapVal = weap_strat(i, factionID);
			//if (Weapon[i].offenseRating < 0 && weapVal > PlayersData[factionID].bestPsiAtkVal) {
			//	PlayersData[factionID].bestPsiAtkVal = weapVal;
			//}
			if (Weapon[i].offenseRating >= 0 && weapVal > PlayersData[factionID].bestWeaponValue) {
				PlayersData[factionID].bestWeaponValue = weapVal;
			}
		}
	}
	//PlayersData[factionID].bestPsiDefVal = hasWorms ? arm_strat(ARM_PSI_DEFENSE, factionID) : 0;
	PlayersData[factionID].bestArmorValue = 1;
	for (int i = 0; i < MaxArmorNum; i++) {
		if (has_tech(Armor[i].preqTech, factionID)) {
			int armVal = arm_strat(i, factionID);
			//if (Armor[i].defenseRating < 0 && armVal > PlayersData[factionID].bestPsiDefVal) {
			//	PlayersData[factionID].bestPsiDefVal = armVal;
			//}
			if (Armor[i].defenseRating >= 0 && armVal > PlayersData[factionID].bestArmorValue) {
				PlayersData[factionID].bestArmorValue = armVal;
			}
		}
	}
	PlayersData[factionID].bestLandSpeed = 1;
	for (int i = 0; i < MaxChassisNum; i++) {
		if (has_tech(Chassis[i].preqTech, factionID) && Chassis[i].triad == TRIAD_LAND) {
			if (Chassis[i].speed > PlayersData[factionID].bestLandSpeed) {
				PlayersData[factionID].bestLandSpeed = Chassis[i].speed;
			}
		}
	}
	PlayersData[factionID].enemyBestWeaponValue = 0;
	PlayersData[factionID].enemyBestArmorValue = 0;
	PlayersData[factionID].enemyBestLandSpeed = 0;
	//PlayersData[factionID].enemyBestPsiAtkVal = 0;
	//PlayersData[factionID].enemyBestPsiDefVal = 0;
	for (int i = 0; i < 4 && !PlayersData[factionID].enemyBestWeaponValue; i++) {
		// 1st pass: vendetta, no treaty, has commlink
		// 2nd pass: no treaty, has commlink
		// 3rd pass: has commlink
		// 4th pass: any non-pact faction
		for (uint32_t j = 1, treaties; j < MaxPlayerNum; j++) {
			if (j != factionID
				&& (treaties = PlayersData[i].diploTreaties[j], !(treaties & DTREATY_PACT))
				&& ((!i && treaties & DTREATY_VENDETTA && !(treaties & DTREATY_TREATY)
					&& treaties & DTREATY_COMMLINK)
					|| (i == 1 && !(treaties & DTREATY_TREATY) && treaties & DTREATY_COMMLINK)
					|| (i == 2 && treaties & DTREATY_COMMLINK) || (i == 3))) {
				if (PlayersData[factionID].enemyBestWeaponValue < PlayersData[j].bestWeaponValue) {
					PlayersData[factionID].enemyBestWeaponValue = PlayersData[j].bestWeaponValue;
				}
				if (PlayersData[factionID].enemyBestArmorValue < PlayersData[j].bestArmorValue) {
					PlayersData[factionID].enemyBestArmorValue = PlayersData[j].bestArmorValue;
				}
				if (PlayersData[factionID].enemyBestLandSpeed < PlayersData[j].bestLandSpeed) {
					PlayersData[factionID].enemyBestLandSpeed = PlayersData[j].bestLandSpeed;
				}
				/*
				if (PlayersData[factionID].enemyBestPsiAtkVal < PlayersData[j].bestPsiAtkVal) {
					PlayersData[factionID].enemyBestPsiAtkVal = PlayersData[j].bestPsiAtkVal;
				}
				if (PlayersData[factionID].enemyBestPsiDefVal < PlayersData[j].bestPsiDefVal) {
					PlayersData[factionID].enemyBestPsiDefVal = PlayersData[j].bestPsiDefVal;
				}
				*/
			}
		}
	}
	// PSI
	BOOL hasWorms = veh_avail(BSC_MIND_WORMS, factionID, -1);
	PlayersData[factionID].bestPsiOffense = hasWorms ? weap_strat(WPN_PSI_ATTACK, factionID) : 0;
	for (int i = 0; i < MaxWeaponNum; i++) {
		if (has_tech(Weapon[i].preqTech, factionID) && Weapon[i].offenseRating < 99) {
			int weapVal = weap_strat(i, factionID);
			if (Weapon[i].offenseRating < 0 && weapVal > PlayersData[factionID].bestPsiOffense) {
				PlayersData[factionID].bestPsiOffense = weapVal;
			}
		}
	}
	PlayersData[factionID].bestPsiDefense = hasWorms ? arm_strat(ARM_PSI_DEFENSE, factionID) : 0;
	for (int i = 0; i < MaxArmorNum; i++) {
		if (has_tech(Armor[i].preqTech, factionID)) {
			int armVal = arm_strat(i, factionID);
			if (Armor[i].defenseRating < 0 && armVal > PlayersData[factionID].bestPsiDefense) {
				PlayersData[factionID].bestPsiDefense = armVal;
			}
		}
	}
	PlayersData[factionID].enemyBestPsiOffense = 0;
	PlayersData[factionID].enemyBestPsiDefense = 0;
	for (int i = 0; i < 4 && !PlayersData[factionID].enemyBestPsiOffense; i++) {
		// 1st pass: vendetta, no treaty, has commlink
		// 2nd pass: no treaty, has commlink
		// 3rd pass: has commlink
		// 4th pass: any non-pact faction
		for (uint32_t j = 1, treaties; j < MaxPlayerNum; j++) {
			if (j != factionID
				&& (treaties = PlayersData[i].diploTreaties[j], !(treaties & DTREATY_PACT))
				&& ((!i && treaties & DTREATY_VENDETTA && !(treaties & DTREATY_TREATY)
					&& treaties & DTREATY_COMMLINK)
					|| (i == 1 && !(treaties & DTREATY_TREATY) && treaties & DTREATY_COMMLINK)
					|| (i == 2 && treaties & DTREATY_COMMLINK) || (i == 3))) {
				if (PlayersData[factionID].enemyBestPsiOffense < PlayersData[j].bestPsiOffense) {
					PlayersData[factionID].enemyBestPsiOffense = PlayersData[j].bestPsiOffense;
				}
				if (PlayersData[factionID].enemyBestPsiDefense < PlayersData[j].bestPsiDefense) {
					PlayersData[factionID].enemyBestPsiDefense = PlayersData[j].bestPsiDefense;
				}
			}
		}
	}
}

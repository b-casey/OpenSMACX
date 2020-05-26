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
uint8_t *FactionsStatus = (uint8_t *)0x009A64E8;

/*
Purpose: Check whether specified faction is a human player or computer controlled AI.
Original Offset: n/a
Return Value: Is faction a human? true/false
Status: Complete
*/
BOOL is_human(uint32_t factionID) {
	return FactionsStatus[0] & (1 << factionID);
}

/*
Purpose: Check whether specified faction is alive or whether they've been eliminated.
Original Offset: n/a
Return Value: Is faction alive? true/false
Status: Complete
*/
BOOL is_alive(uint32_t factionID) {
	return FactionsStatus[1] & (1 << factionID);
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
			&& (pactFactionID <= 0 || !(PlayersData[i].diploTreaties[pactFactionID] & DTREATY_PACT)
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
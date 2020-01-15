/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2019 Brendan Casey
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
#include "technology.h"

player *Players = (player *)0x00946A50;
player_data *PlayersData = (player_data *)0x0096C9E0;
faction_art *FactionArt = (faction_art*)0x0078E978;
rules_social_category *SocialCategory = (rules_social_category *)0x0094B000;
rules_social_effect *SocialEffect = (rules_social_effect *)0x00946580;
LPSTR *Mood = (LPSTR *)0x0094C9E4;
LPSTR *Repute = (LPSTR *)0x00946A30;
rules_might *Might = (rules_might *)0x0094C558;
rules_proposal *Proposal = (rules_proposal *)0x009A6828;
rules_bonusname *BonusName = (rules_bonusname *)0x009461A8;
uint8_t *FactionCurrentBitfield = (uint8_t *)0x009A64E8;

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
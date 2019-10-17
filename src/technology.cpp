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
#include "technology.h"
#include "game.h"

rules_technology *Technology = (rules_technology *)0x0094F358;
uint8_t *GameTechDiscovered = (uint8_t *)0x009A6670;
rules_mandate *Mandate = (rules_mandate *)0x0094B4A0;
int *TechValidCount = (int *)0x00949730;
int *TechCommerceCount = (int *)0x00949734;

/*
Purpose: Check whether faction has a particular tech or not.
Original Offset: 005B9F20
Return Value: Faction has tech? true/false
Status: Complete
*/
BOOL __cdecl has_tech(int techID, int factionID) {
	if (factionID <= 0) {
		return false;
	}
	if (techID == TechNone) {
		return true;
	}
	if (techID < 0 || techID >= (MaxTechnologyNum - 1) // excludes 'Transcendent Thought'
		|| Technology[techID].preqTech1 == TechDisabled
		|| (Technology[techID].preqTech2 == TechDisabled
			&& Technology[techID].preqTech1 != TechNone)) {
		// "none, disable" ; valid #TECH preqTech entry
		return false;
	}
	return ((1 << factionID) & GameTechDiscovered[techID]) != 0;
}

/*
Purpose: Check to see whether provided faction can research a specific technology. Checks are
		 included to prevent SMACX specific Veh from being built in SMAC mode.
Original Offset: 005BAC20
Return Value: Is tech available to faction? true/false
Status: Complete
*/
BOOL __cdecl tech_avail(int techID, int factionID) {
	if (has_tech(techID, factionID) || techID >= MaxTechnologyNum || (!*SMACX_Enabled 
		&& (techID == TECH_PRPSYCH || techID == TECH_FLDMOD || techID == TECH_ADAPDOC 
			|| techID == TECH_ADAPECO || techID == TECH_BIOADAP || techID == TECH_SENTRES 
			|| techID == TECH_SECMANI || techID == TECH_NEWMISS || techID == TECH_BFG9000))) {
		return false;
	}
	int preqTech1 = Technology[techID].preqTech1, preqTech2 = Technology[techID].preqTech2;
	if (preqTech1 < TechNone || preqTech2 < TechNone) {
		return false; // if either prerequisite tech is set to disabled (-2)
	}
	return (has_tech(preqTech1, factionID) && has_tech(preqTech2, factionID));
}

/*
Purpose: Get powerValue from technology struct for techID
Original Offset: 005BDD70
Return Value: powerValue or 0 if techID is greater than max
Status: Complete
*/
int __cdecl tech_mil(int techID) {
	if (techID >= MaxTechnologyNum) {
		return 0; // why only this one returns 0 with other three returning 1? typo/bug?
	}
	return *(&Technology[techID].powerValue);
}

/*
Purpose: Get techValue from technology struct for techID
Original Offset: 005BDD90
Return Value: techValue or 1 if techID is greater than max
Status: Complete
*/
int __cdecl tech_tech(int techID) {
	if (techID >= MaxTechnologyNum) {
		return 1;
	}
	return *(&Technology[techID].techValue);
}

/*
Purpose: Get wealthValue from technology struct for techID
Original Offset: 005BDDC0
Return Value: wealthValue or 1 if techID is greater than max
Status: Complete
*/
int __cdecl tech_infra(int techID) {
	if (techID >= MaxTechnologyNum) {
		return 1;
	}
	return *(&Technology[techID].wealthValue);
}

/*
Purpose: Get growthValue from technology struct for techID
Original Offset: 005BDDF0
Return Value: growthValue or 1 if techID is greater than max
Status: Complete
*/
int __cdecl tech_colonize(int techID) {
	if (techID >= MaxTechnologyNum) {
		return 1;
	}
	return *(&Technology[techID].growthValue);
}
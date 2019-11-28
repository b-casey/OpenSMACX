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
#include "technology.h"
#include "alpha.h"
#include "game.h"
#include "strings.h"
#include "veh.h"

rules_technology *Technology = (rules_technology *)0x0094F358;
uint8_t *GameTechDiscovered = (uint8_t *)0x009A6670;
rules_mandate *Mandate = (rules_mandate *)0x0094B4A0;
int *TechValidCount = (int *)0x00949730;
int *TechCommerceCount = (int *)0x00949734;
char TechName[80];

/*
Purpose: Get tech string for techID and store it into stringTemp buffer.
Original Offset: 005591A0
Return Value: n/a
Status: Complete
*/
void __cdecl say_tech(int techID, BOOL categoryLvl) {
	say_tech(stringTemp->str, techID, categoryLvl);
}

/*
Purpose: Craft an output string related to a specific technology. For techIDs outside the standard
         range, craft a string related to world map, comm links or prototypes.
Original Offset: 005B9C40
Return Value: n/a
Status: Complete
*/
void __cdecl say_tech(LPSTR output, int techID, BOOL categoryLvl) {
	if (techID < -1) {
		strcat_s(output, 80, label_get(310)); // "Not Available"
	}
	else if (techID < 0) {
		strcat_s(output, 80, label_get(25)); // "NONE"
	}
	else if (techID == 9999) {
		strcat_s(output, 80, label_get(306)); // "World Map"
	}
	else if (techID < MaxTechnologyNum) {
		strcat_s(output, 80, StringTable->get((int)Technology[techID].name));
		if (categoryLvl) {
			sprintf_s(&output[strlen(output)], 80, " (%s%d)", // 'E#', 'D#', 'B#', 'C#'
				label_get(629 + tech_category(techID)), tech_recurse(techID, 0));
		}
	}
	else if (techID < 97) {
		if (*Language) {
			sprintf_s(&output[strlen(output)], 80, "%s (%s)", label_get(487), // 'Comm Frequency'
				get_noun(techID - 89));
		}
		else {
			sprintf_s(&output[strlen(output)], 80, "%s %s", Players[techID - 89].nameAdjFaction,
				label_get(487)); // 'Comm Frequency'
		}
	}
	else {
		sprintf_s(&output[strlen(output)], 80, "%s %s", VehPrototype[techID - 97].vehName, 
			label_get(185)); // 'Prototype'
	}
}

/*
Purpose: Get tech string for techID and store it into TechName buffer.
Original Offset: 005B9EF0
Return Value: Pointer to TechName
Status: Complete
*/
LPSTR __cdecl tech_name(int techID, BOOL categoryLvl) {
	TechName[0] = 0;
	say_tech(TechName, techID, categoryLvl);
	return TechName;
}

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
Purpose: Determine technology level for techID.
Original Offset: 005B9F90
Return Value: Level
Status: Complete
*/
int __cdecl tech_recurse(int techID, int baseLevel) {
	if (techID < 0 || techID >= MaxTechnologyNum) {
		return baseLevel;
	}
	int val1 = tech_recurse(Technology[techID].preqTech1, baseLevel + 1);
	int val2 = tech_recurse(Technology[techID].preqTech2, baseLevel + 1);
	return (val1 > val2) ? val1 : val2;
}

/*
Purpose: Determine what category is dominate for techID. If there is a tie, the order of precedence
         is as follows: growth > tech > wealth > power.
Original Offset: 005B9FE0
Return Value: Tech category id: growth (0), tech (1), wealth (2) or power (3).
Status: Complete
*/
int __cdecl tech_category(int techID) {
	uint32_t category = 0;
	int compare = Technology[techID].growthValue;
	if (Technology[techID].techValue > compare) {
		category = 1;
		compare = Technology[techID].techValue;
	}
	if (Technology[techID].wealthValue > compare) {
		category = 2;
		compare = Technology[techID].wealthValue;
	}
	return (Technology[techID].powerValue > compare) ? 3 : category;
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
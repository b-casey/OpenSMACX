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

rules_technology *Technology = (rules_technology *)0x0094F358;
uint8_t *GameTechDiscovered = (uint8_t *)0x009A6670;
rules_mandate *Mandate = (rules_mandate *)0x0094B4A0;
int *TechValidCount = (int *)0x00949730;
int *TechCommerceCount = (int *)0x00949734;
char TechName[80];

// 005B9C40
void __cdecl say_tech(LPSTR output, int techID, int toggle) {
	//std::string output;
	if (techID < -1) {
		// "Not Available"
		strcat_s(output, 80, StringTable->get((int)*((LPSTR *)Label->stringsPtr + 310)));
	}
	else if (!techID) {
		// "NONE"
		strcat_s(output, 80, StringTable->get((int)*((LPSTR *)Label->stringsPtr + 25)));
	}
	else if (techID == 9999) {
		// "World Map"		
		strcat_s(output, 80, StringTable->get((int)*((LPSTR *)Label->stringsPtr + 306)));
	}
	else if (techID <= 89) {
		/*
		
		strcat_s(output, 80, StringTable->get((int)*Technology[techID].name));
		if (toggle) { // include category + preq
			strcat_s(output, 80, " (");
			strcat_s(output, 80, StringTable->get((int)*((LPSTR *)Label->stringsPtr + 
				tech_category(techID) + 629)));
			int val1 = tech_recurse(Technology[techID].preqTech1, 1);
			int val2 = tech_recurse(Technology[techID].preqTech2, 1);
			if (val1 > val2) {
				val2 = val1;
			}
			int len = strlen(output);
			wsprintfA(&output[len], "%d)", val2);
		}
		*/
	}
	else if (techID <= 97) {
		if (*Language) {
			strcat_s(output, 80, StringTable->get((int)*((LPSTR *)Label->stringsPtr + 487)));
			strcat_s(output, 80, " (");
			strcat_s(output, 80, Players[techID - 89].nameAdjFaction);
			*PluralityDefault = Players[techID - 89].isNounPlural;
			//*GenderDefault = Players[techID - 89].nounFaction
			
		}
		else {
			strcat_s(output, 80, Players[techID - 89].nameAdjFaction);
			strcat_s(output, 80, " ");
			strcat_s(output, 80, StringTable->get((int)*((LPSTR *)Label->stringsPtr + 487)));
		}
		
		/*
		v11 = techID - 89;
		if (Language)
		{
			GenderDefault = *(_DWORD *)&Players.nounFaction[1436 * v11 + 24];
			strcat(output, &Players.nounFaction[1436 * v11]);
			strcat(output, asc_682E98);
			return;
		}
		*/

	}
	//v17 = (const char *)Strings::get(&StringTable, v14);
	//strcat(v13, v17);
	else {
	/*
		v13 = output;
		strcat(output, &VehPrototype.vehName[4 * (techID + 4 * (3 * techID - 291)) - 388]);
		strcat(output, szSPACE);
		v14 = *((_DWORD *)Labels.stringsPtr + 185);
		strcat_s(output, 80, StringTable->get((int)*((LPSTR *)Label->stringsPtr + 306)));
		v17 = (const char *)Strings::get(&StringTable, v14);
		strcat(v13, v17);
		*/
	}
}

// 005B9EF0
LPSTR __cdecl tech_name(int techID, BOOL toggle) {
	TechName[0] = 0;
	say_tech(TechName, techID, toggle);
	return (LPSTR)&TechName;
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

// 005B9F90
int __cdecl tech_recurse(int techID, int ret) {
	if (techID < 0 || techID >= 89) {
		return ret;
	}
	int val1 = tech_recurse(Technology[techID].preqTech1, ret + 1);
	int val2 = tech_recurse(Technology[techID].preqTech2, ret + 1);
	return (val1 > val2) ? val1 : val2;
}

// 005B9FE0
int __cdecl tech_category(int techID) {
	int compare = Technology[techID].growthValue, tech = Technology[techID].techValue,
		wealth = Technology[techID].wealthValue, power = Technology[techID].powerValue;
	uint32_t category = 0;
	if (tech > compare) {
		category = 1;
		compare = tech;
	}
	if (wealth > compare) {
		category = 2;
		compare = wealth;
	}
	return (power > compare) ? 3 : category;
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
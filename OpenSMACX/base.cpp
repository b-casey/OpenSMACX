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
#include "base.h"

rules_facility *Facility = (rules_facility *)0x009A4B68;
rules_citizen *Citizen = (rules_citizen *)0x00946020;
base *Base = (base *)0x0097D040; // 512
base_secret_project *SecretProject = (base_secret_project *)0x009A6514; // 64

/*
Purpose: Check if faction has secret project in a base they control.
Original Offset: 004F80D0
Return Value: Does faction have Secret Project? TRUE/FALSE
Status: Complete
*/
BOOL __cdecl has_project(DWORD projectID, DWORD factionID) {
	int baseID = *(&SecretProject->HumanGenomeProject + projectID);
	if (baseID >= 0) {
		return (Base[baseID].factionIDCurrent == factionID);
	}
	return FALSE; // Not built or destroyed
}

/*
Purpose: Calculate offset & bitmask from facilityID.
Original Offset: 0050BA00
Return Value: n/a
Status: Complete
*/
void __cdecl bitmask(DWORD facilityID, DWORD *offset, DWORD *mask) {
	*offset = facilityID / 8;
	*mask = 1 << (facilityID & 7);
}
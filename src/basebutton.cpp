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
#include "basebutton.h"

/*
Purpose: Set bubble text string value
Original Offset: 00607550
Return Value: Non-zero error, otherwise zero if successful
Status: Complete with redirect for free to prevent hang/freeze. Incompatibility between older
		version of free with newer SDK version of free CRT. Revisit once more of code is redirected
		to dll.
*/
int BaseButton::set_bubble_text(LPCSTR input) {
	if (bubbleText) {
		_free(bubbleText);
		bubbleText = 0;
	}
	if (input) {
		int len = strlen(input) + 1;
		bubbleText = (LPSTR)mem_get_old(len);
		if (!bubbleText) {
			return 4;
		}
		strcpy_s(bubbleText, len, input);
	}
	return 0;
}

/*
Purpose: Set name string value
Original Offset: 006074E0
Return Value: Non-zero error, otherwise zero if successful
Status: Complete with redirect for free to prevent hang/freeze. Incompatibility between older
		version of free with newer SDK version of free CRT. Revisit once more of code is redirected
		to dll.
*/
int BaseButton::set_name(LPCSTR input) {
	// Fixed crash if input parameter was null. Original code had string copy outside last if
	// statement causing potential write to null name variable.
	if (name) {
		_free(name);
		name = 0;
	}
	if (input) {
		int len = strlen(input) + 1;
		name = (LPSTR)mem_get_old(len);
		if (!name) {
			return 4;
		}
		strcpy_s(name, len, input);
	}
	return 0;
}

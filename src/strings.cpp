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
#include "strings.h"

/*
Purpose: Initialize a new string table with specified size from parameter
Original Offset: 006168F0
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL Strings::init(size_t memSize) {
	if (isPopulated) {
		shutdown();
	}
	if (Heap::init(memSize)) {
		return true; // allocation failed
	}
	put("-Nil-");
	isPopulated = true;
	return false; // successful
}

/*
Purpose: Shutdown and free up string table memory
Original Offset: 00616950
Return Value: n/a
Status: Complete
*/
void Strings::shutdown() {
	Heap::shutdown();
	isPopulated = false;
}

/*
Purpose: Store input string to string table
Original Offset: 00616970
Return Value: Address of stored string
Status: Complete
*/
LPSTR Strings::put(LPCSTR input) {
	int len = strlen(input) + 1;
	LPSTR tableAddr = LPSTR(Heap::get(len));
	strcpy_s(tableAddr, len, input);
	return tableAddr;
}

/*
Purpose: Check if string is in table by looking at bounds within heap
Original Offset: 006169A0
Return Value: Address of stored string
Status: Complete
*/
LPSTR Strings::get(int address) {
	// checking if ptr is after base
	if ((LPVOID)address > getBasePtr()) {
		// checking ptr is not after end of table
		return ((LPSTR)address <= LPSTR(size_t(getBasePtr()) + getBaseSize())) ?
			(LPSTR)address : NULL;
	}
	return LPSTR(getBasePtr()); // return base
}

// global
Strings *StringTable = (Strings *)0x009B90D8;

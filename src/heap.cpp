/*
 * Copyright 2013-2014 Brendan Casey.
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
#include "heap.h"
#include "general.h" // mem_get()

/*
Purpose: Destroy current heap and zero out all class variables
Original Offset: 005D45B0
Return Value: n/a
Status: Complete
*/
void Heap::shutdown() {
	if (basePtr) {
		free(basePtr);
	}
	errFlags = 0;
	basePtr = currentPtr = 0;
	baseSize = freeSize = 0;
}

/*
Purpose: Deflate heap of any free memory. Toggle variable unused in original code.
Original Offset: 005D45E0
Return Value: n/a
Status: Complete
*/
void Heap::squeeze(int toggle) {
	size_t usedSize = baseSize - freeSize;
	LPVOID newAddr = realloc(basePtr, usedSize);
	if (newAddr) {
		basePtr = newAddr;
		baseSize = usedSize;
		freeSize = 0;
	}
}

/*
Purpose: Allocate memory based on requested size from parameter
Original Offset: 005D4620
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL Heap::init(size_t reqSize) {
	if (basePtr) {
		shutdown();
	}
	basePtr = mem_get(reqSize);
	if (basePtr) {
		currentPtr = basePtr;
		baseSize = freeSize = reqSize;
		return false;
	}
	return true; // error: failed to allocate memory
}

/*
Purpose: Check if there is currently enough free memory for the requested size parameter.
		 If not, get additional memory in blocks of 1024 bytes until there is enough.
Original Offset: 005D4680
Return Value: Memory pointer with address to requested size free.
Status: Complete
*/
LPVOID Heap::get(size_t reqSize) {
	while (freeSize < reqSize) {
		if (errFlags & 1) {
			return 0; // error
		}
		LPVOID newAddr = realloc(basePtr, baseSize + 1024);
		if (!newAddr) {
			char szError[150]; // max size of string + three int(s) + extra padding
			sprintf_s(szError, 150,
				"Aborting due to a heap shortage!\n"
				"Base size: %d\n"
				"Free size: %d\n"
				"Requested size: %d", 
				baseSize, freeSize, reqSize);
			MessageBoxA(NULL, szError, "Heap Notice!!", MB_OK);
			exit(3);
		}
		basePtr = newAddr;
		// bug fix: in case realloc shifts memory
		currentPtr = LPVOID(size_t(basePtr) + baseSize - freeSize);
		baseSize += 1024;
		freeSize += 1024;
	}
	LPVOID freeMemAddr = currentPtr;
	freeSize -= reqSize;
	currentPtr = LPVOID(size_t(currentPtr) + reqSize);
	return freeMemAddr;
}
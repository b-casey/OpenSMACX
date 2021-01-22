/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2021 Brendan Casey
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
Purpose: Initialize the class instance with a new string table of the specified size.
Original Offset: 006168F0
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL Strings::init(size_t mem_size) {
    if (is_populated_) {
        shutdown();
    }
    if (Heap::init(mem_size)) {
        return true; // allocation failed
    }
    put("-Nil-");
    is_populated_ = true;
    return false; // successful
}

/*
Purpose: Shutdown the class instance.
Original Offset: 00616950
Return Value: n/a
Status: Complete
*/
void Strings::shutdown() {
    Heap::shutdown();
    is_populated_ = false;
}

/*
Purpose: Put the input string into the string table.
Original Offset: 00616970
Return Value: Address of the stored string
Status: Complete
*/
LPSTR Strings::put(LPCSTR input) {
    size_t len = strlen(input) + 1;
    LPSTR table_addr = LPSTR(Heap::get(len));
    strcpy_s(table_addr, len, input);
    return table_addr;
}

/*
Purpose: Check if the string address is within the table.
Original Offset: 006169A0
Return Value: Address of the stored string
Status: Complete
*/
LPSTR Strings::get(int address) {
    if ((LPVOID)address > get_base()) { // checking if ptr is after base
        // checking ptr is not after end of table
        return ((LPSTR)address <= LPSTR(size_t(get_base()) + get_base_size())) 
            ? (LPSTR)address : NULL;
    }
    return LPSTR(get_base()); // return base
}

// global
Strings *StringTable = (Strings *)0x009B90D8;

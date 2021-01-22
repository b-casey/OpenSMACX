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
#include "temp.h"
#include "basebutton.h"

/*
Purpose: Set the button's bubble text.
Original Offset: 00607550
Return Value: Zero on success, non-zero on error
Status: Complete with redirect for free to prevent hang/freeze. Incompatibility between older
        version of free with newer SDK version of free CRT. Revisit once more of code is redirected
        to dll.
*/
int BaseButton::set_bubble_text(LPCSTR input) {
    if (bubble_text_) {
        _free(bubble_text_);
        bubble_text_ = 0;
    }
    if (input) {
        size_t len = strlen(input) + 1;
        bubble_text_ = (LPSTR)mem_get_old(len);
        if (!bubble_text_) {
            return 4;
        }
        strcpy_s(bubble_text_, len, input);
    }
    return 0;
}

/*
Purpose: Set the button's name string.
Original Offset: 006074E0
Return Value: Zero on success, non-zero on error
Status: Complete with redirect for free to prevent hang/freeze. Incompatibility between older
        version of free with newer SDK version of free CRT. Revisit once more of code is redirected
        to dll.
*/
int BaseButton::set_name(LPCSTR input) {
    // Bug fix: Fixed crash if input parameter was null. Original code had string copy outside last 
    // if statement causing potential write to null name variable.
    if (name_) {
        _free(name_);
        name_ = 0;
    }
    if (input) {
        size_t len = strlen(input) + 1;
        name_ = (LPSTR)mem_get_old(len);
        if (!name_) {
            return 4;
        }
        strcpy_s(name_, len, input);
    }
    return 0;
}

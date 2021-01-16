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
#include "spot.h"
#include "general.h" // mem_get()

/*
Purpose: Shutdown instance of Spot.
Original Offset: 005FA830
Return Value: n/a
Status: Complete
*/
void Spot::shutdown() {
    if (spots) {
        free(spots);
    }
    clear();
}

/*
Purpose: Initialize instance of Spot with count.
Original Offset: 005FA8A0
Return Value: n/a
Status: Complete
*/
void Spot::init(uint32_t count) {
    shutdown();
    spots = (SpotInternal *)mem_get(count * sizeof(SpotInternal));
    if (spots) {
        maxCount = count;
        addCount = 0;
    }
}

/*
Purpose: Search for specific Spot and replace the RECT value.
Original Offset: 005FA900
Return Value: n/a
Status: Complete
*/
void Spot::replace(int position, int type, int left, int top, int length, int width) {
    for (uint32_t i = 0; i < addCount; i++) {
        if (spots[i].position == position && spots[i].type == type) {
            spots[i].rect.left = left;
            spots[i].rect.right = left + length;
            spots[i].rect.top = top;
            spots[i].rect.bottom = top + width;
        }
    }
}

/*
Purpose: Add a Spot from individual values.
Original Offset: 005FA960
Return Value: -1 error, otherwise Spot position on success.
Status: Complete
*/
int Spot::add(int position, int type, int left, int top, int length, int width) {
    if (addCount >= maxCount) {
        return -1;
    }
    spots[addCount].rect.left = left;
    spots[addCount].rect.right = left + length;
    spots[addCount].rect.top = top;
    spots[addCount].rect.bottom = top + width;
    spots[addCount].position = position;
    spots[addCount].type = type;
    return addCount++;
}

/*
Purpose: Add a Spot from RECT. This was optimized out but available in macOS version.
Original Offset: n/a
Return Value: -1 error, otherwise Spot position on success.
Status: Complete
*/
int Spot::add(int position, int type, RECT *rect) {
    return add(position, type, rect->left, rect->top,
        rect->right - rect->left, rect->bottom - rect->top);
}

/*
Purpose: Remove all Spots at a specific position.
Original Offset: 005FA9C0
Return Value: n/a
Status: Complete
*/
void Spot::kill_pos(int position) {
    if (position < 0 || (uint32_t)position >= addCount) {
        return;
    }
    addCount--;
    if ((uint32_t)position < addCount) {
        size_t size = sizeof(SpotInternal) * (addCount - position);
        memcpy_s(&spots[position], size, &spots[position + 1], size);
    }
}

/*
Purpose: Remove a specific Spot.
Original Offset: 005FAA10
Return Value: n/a
Status: Complete
*/
void Spot::kill_specific(int position, int type) {
    for (int i = addCount - 1; i >= 0; i--) {
        if (spots[i].position == position && spots[i].type == type) {
            kill_pos(i);
        }
    }
}

/*
Purpose: Remove all Spots of specific type.
Original Offset: 005FAA90
Return Value: n/a
Status: Complete
*/
void Spot::kill_type(int type) {
    for (int i = addCount - 1; i >= 0; i--) {
        if (spots[i].type == type) {
            kill_pos(i);
        }
    }
}

/*
Purpose: Check if coordinates fall inside a Spot. If so, get information about Spot.
Original Offset: 005FAB00
Return Value: -1 error, otherwise Spot position on success.
Status: Complete
*/
int Spot::check(int xCoord, int yCoord, int *spotPos, int *spotType) {
    int offset = addCount - 1;
    if (offset < 0) {
        return -1;
    }
    for (int i = offset; i >= 0; i--) {
        if (xCoord >= spots[i].rect.left && xCoord < spots[i].rect.right
            && yCoord >= spots[i].rect.top && yCoord < spots[i].rect.bottom) {
            if (spotPos) {
                *spotPos = spots[i].position;
            }
            if (spotType) {
                *spotType = spots[i].type;
            }
            return i;
        }
    }
    return -1;
}

/*
Purpose: Check if coordinates fall inside a Spot. If so, get information about Spot including RECT.
Original Offset: 005FAB70
Return Value: -1 error, otherwise Spot position on success.
Status: Complete
*/
int Spot::check(int xCoord, int yCoord, int *spotPos, int *spotType, RECT *spotRect) {
    int offset = addCount - 1;
    if (offset < 0) {
        return -1;
    }
    for (int i = offset; i >= 0; i--) {
        if (xCoord >= spots[i].rect.left && xCoord < spots[i].rect.right
            && yCoord >= spots[i].rect.top && yCoord < spots[i].rect.bottom) {
            if (spotPos) {
                *spotPos = spots[i].position;
            }
            if (spotType) {
                *spotType = spots[i].type;
            }
            if (spotRect) {
                *spotRect = spots[i].rect;
            }
            return i;
        }
    }
    return -1;
}

/*
Purpose: Get a specific Spot's RECT.
Original Offset: 005FAC00
Return Value: -1 error, otherwise Spot position on success.
Status: Complete
*/
int Spot::get_rect(int position, int type, RECT *spotRect) {
    int offset = addCount - 1;
    if (offset < 0) {
        return -1;
    }
    for (int i = offset; i >= 0; i--) {
        if (spots[i].position == position && spots[i].type == type) {
            if (spotRect) {
                *spotRect = spots[i].rect;
            }
            return i;
        }
    }
    return -1;
}

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
#include "general.h"

/*
Purpose: Shutdown the class instance.
Original Offset: 005FA830
Return Value: n/a
Status: Complete
*/
void Spot::shutdown() {
    if (spots_) {
        free(spots_);
    }
    clear();
}

/*
Purpose: Initialize the class instance with count.
Original Offset: 005FA8A0
Return Value: n/a
Status: Complete
*/
void Spot::init(uint32_t count) {
    shutdown();
    spots_ = (SpotInternal *)mem_get(count * sizeof(SpotInternal));
    if (spots_) {
        max_count_ = count;
        add_count_ = 0;
    }
}

/*
Purpose: Search for a specific spot and replace the RECT value.
Original Offset: 005FA900
Return Value: n/a
Status: Complete
*/
void Spot::replace(int position, int type, int left, int top, int length, int width) {
    for (uint32_t i = 0; i < add_count_; i++) {
        if (spots_[i].position == position && spots_[i].type == type) {
            spots_[i].rect.left = left;
            spots_[i].rect.right = left + length;
            spots_[i].rect.top = top;
            spots_[i].rect.bottom = top + width;
        }
    }
}

/*
Purpose: Add a new spot from the specified values.
Original Offset: 005FA960
Return Value: Spot position on success otherwise -1 on error
Status: Complete
*/
int Spot::add(int position, int type, int left, int top, int length, int width) {
    if (add_count_ >= max_count_) {
        return -1;
    }
    spots_[add_count_].rect.left = left;
    spots_[add_count_].rect.right = left + length;
    spots_[add_count_].rect.top = top;
    spots_[add_count_].rect.bottom = top + width;
    spots_[add_count_].position = position;
    spots_[add_count_].type = type;
    return add_count_++;
}

/*
Purpose: Add a new spot from a RECT.
Original Offset: n/a (Optimized out but available in PPC version)
Return Value: Spot position on success otherwise -1 on error
Status: Complete
*/
int Spot::add(int position, int type, RECT *rect) {
    return add(position, type, rect->left, rect->top, rect->right - rect->left, 
        rect->bottom - rect->top);
}

/*
Purpose: Remove all spots at a specific position.
Original Offset: 005FA9C0
Return Value: n/a
Status: Complete
*/
void Spot::kill_pos(int position) {
    if (position < 0 || (uint32_t)position >= add_count_) {
        return;
    }
    add_count_--;
    if ((uint32_t)position < add_count_) {
        size_t size = sizeof(SpotInternal) * (add_count_ - position);
        memcpy_s(&spots_[position], size, &spots_[position + 1], size);
    }
}

/*
Purpose: Remove a specific spot.
Original Offset: 005FAA10
Return Value: n/a
Status: Complete
*/
void Spot::kill_specific(int position, int type) {
    for (int i = add_count_ - 1; i >= 0; i--) {
        if (spots_[i].position == position && spots_[i].type == type) {
            kill_pos(i);
        }
    }
}

/*
Purpose: Remove all spots of a specific type.
Original Offset: 005FAA90
Return Value: n/a
Status: Complete
*/
void Spot::kill_type(int type) {
    for (int i = add_count_ - 1; i >= 0; i--) {
        if (spots_[i].type == type) {
            kill_pos(i);
        }
    }
}

/*
Purpose: Check if the coordinates fall inside a spot. If so, return information about the spot.
Original Offset: 005FAB00
Return Value: Spot position on success otherwise -1 on error
Status: Complete
*/
int Spot::check(int x, int y, int *spot_pos, int *spot_type) {
    int offset = add_count_ - 1;
    if (offset < 0) {
        return -1;
    }
    for (int i = offset; i >= 0; i--) {
        if (x >= spots_[i].rect.left && x < spots_[i].rect.right && y >= spots_[i].rect.top 
            && y < spots_[i].rect.bottom) {
            if (spot_pos) {
                *spot_pos = spots_[i].position;
            }
            if (spot_type) {
                *spot_type = spots_[i].type;
            }
            return i;
        }
    }
    return -1;
}

/*
Purpose: Check if the coordinates fall within a spot. If so, return information about the spot.
Original Offset: 005FAB70
Return Value: Spot position on success otherwise -1 on error
Status: Complete
*/
int Spot::check(int x, int y, int *spot_pos, int *spot_type, RECT *spot_rect) {
    int offset = add_count_ - 1;
    if (offset < 0) {
        return -1;
    }
    for (int i = offset; i >= 0; i--) {
        if (x >= spots_[i].rect.left && x < spots_[i].rect.right && y >= spots_[i].rect.top 
            && y < spots_[i].rect.bottom) {
            if (spot_pos) {
                *spot_pos = spots_[i].position;
            }
            if (spot_type) {
                *spot_type = spots_[i].type;
            }
            if (spot_rect) {
                *spot_rect = spots_[i].rect;
            }
            return i;
        }
    }
    return -1;
}

/*
Purpose: Get the RECT for a specific spot.
Original Offset: 005FAC00
Return Value: Spot position on success otherwise -1 on error
Status: Complete
*/
int Spot::get_rect(int position, int type, RECT *spot_rect) {
    int offset = add_count_ - 1;
    if (offset < 0) {
        return -1;
    }
    for (int i = offset; i >= 0; i--) {
        if (spots_[i].position == position && spots_[i].type == type) {
            if (spot_rect) {
                *spot_rect = spots_[i].rect;
            }
            return i;
        }
    }
    return -1;
}

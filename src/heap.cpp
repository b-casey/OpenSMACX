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
#include "heap.h"
#include "general.h"

/*
Purpose: Shutdown the class instance.
Original Offset: 005D45B0
Return Value: n/a
Status: Complete
*/
void Heap::shutdown() {
    if (base_) {
        free(base_);
    }
    err_flags_ = 0;
    base_ = 0;
    current_ = 0;
    base_size_ = 0;
    free_size_ = 0;
}

/*
Purpose: Deflate the heap of any free memory.
Original Offset: 005D45E0
Return Value: n/a
Status: Complete
*/
void Heap::squeeze(int UNUSED(toggle)) {
    size_t used_size = base_size_ - free_size_;
    LPVOID new_addr = realloc(base_, used_size);
    if (new_addr) {
        base_ = new_addr;
        base_size_ = used_size;
        free_size_ = 0;
    }
}

/*
Purpose: Initialize the class instance and allocate the requested memory size.
Original Offset: 005D4620
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL Heap::init(size_t req_size) {
    if (base_) {
        shutdown();
    }
    base_ = mem_get(req_size);
    if (base_) {
        current_ = base_;
        base_size_ = req_size;
        free_size_ = req_size;
        return false;
    }
    return true; // error: failed to allocate memory
}

/*
Purpose: Get the requested memory size. If there currently isn't enough memory to meet the request, 
         allocate additional memory in blocks of 1024 bytes until request is met.
Original Offset: 005D4680
Return Value: Memory pointer with address to requested size
Status: Complete
*/
LPVOID Heap::get(size_t req_size) {
    while (free_size_ < req_size) {
        if (err_flags_ & 1) {
            return NULL; // error
        }
        LPVOID new_addr = realloc(base_, base_size_ + 1024);
        if (!new_addr) {
            char error[150]; // max size of string + three int(s) + extra padding
            sprintf_s(error, 150,
                "Aborting due to a heap shortage!\n"
                "Base size: %d\n"
                "Free size: %d\n"
                "Requested size: %d",
                base_size_, free_size_, req_size);
            MessageBoxA(NULL, error, "Heap Notice!!", MB_OK);
            exit(3);
        }
        base_ = new_addr;
        // bug fix: in case realloc shifts memory
        current_ = LPVOID(size_t(base_) + base_size_ - free_size_);
        base_size_ += 1024;
        free_size_ += 1024;
    }
    LPVOID free_mem_addr = current_;
    free_size_ -= req_size;
    current_ = LPVOID(size_t(current_) + req_size);
    return free_mem_addr;
}

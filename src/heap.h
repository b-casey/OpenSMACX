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
#pragma once

 /*
  * Heap class: Handles managing or allocating memory.
  */
class DLLEXPORT Heap {
    char errFlags;     // (+0) -> error toggle
    LPVOID basePtr;    // (+4) -> pointer to base memory address
    LPVOID currentPtr; // (+8) -> current memory address position
    size_t baseSize;   // (+12) -> size of total memory
    size_t freeSize;   // (+16) -> size of free available memory

public:
    Heap() : errFlags(0), basePtr(0), currentPtr(0), baseSize(0), freeSize(0) { } // 005D4560
    ~Heap() { shutdown(); }                                                       // 005D4580

    void shutdown();
    void squeeze(int toggle);
    BOOL init(size_t reqSize);
    LPVOID get(size_t reqSize);

    // additional functions to assist with encapsulation
    size_t getBaseSize() { return baseSize; } // Get base total memory size
    LPVOID getBasePtr() { return basePtr; }   // Get base memory address pointer
};

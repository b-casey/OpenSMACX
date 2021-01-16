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
#include "heap.h"

/*
* Designed to speed up the time it takes to find string resources
* in text files by creating an index of the file sections prefixed by '#'
*/
class OPENSMACX_API TextIndex {
    char fileName[256]; // name of file to be mapped
    uint32_t sectionCount; // number of section entries
    Heap heap;

public:
    TextIndex() : sectionCount(0) { fileName[0] = 0; } // 005FDF40
    ~TextIndex() { shutdown(); }                       // 005FDF60
    void shutdown() { sectionCount = 0; fileName[0] = 0; heap.shutdown(); }

    void make_index(LPCSTR sourceTxt);
    int search_index(LPCSTR sourceTxt, LPCSTR sectionTxt);

    // additional functions to assist with encapsulation
    uint32_t getCount() { return sectionCount; }
};

/*
* global
* Can skip setting TxtIndex constructor/destructor; Messy vector logic otherwise trying to assign to
* existing memory location. Existing code maps to class TextIndex() and ~TextIndex() that redirect to
* dll so no need replace them. Revisit in future.
* void __cdecl `dynamic initializer for 'TxtIndex''(): 005FD4E0
* void __cdecl `dynamic atexit destructor for 'TxtIndex''(): 005FD510
*/
constexpr int MaxTextIndexNum = 4;
extern TextIndex *TxtIndex;
OPENSMACX_API void __cdecl text_make_index(LPCSTR sourceTxt);
OPENSMACX_API int __cdecl text_search_index(LPCSTR sourceTxt, LPCSTR sectionTxt);
OPENSMACX_API void __cdecl text_clear_index();

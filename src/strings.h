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
#pragma once
#include "heap.h"

 /*
  * Strings class: Create and interact with a basic string table.
  */
class OPENSMACX_API Strings : Heap {
	BOOL isPopulated; // (+20) -> set to true when table is created

public:
	// Constructor, other variables are from subclass (Heap)
	Strings() : isPopulated(false) { } // 006168D0
	~Strings() { Heap::shutdown(); }  // 006169C0

	BOOL init(size_t memSize);
	void shutdown();
	LPSTR put(LPCSTR input);
	LPSTR get(int address);
};

// global
extern Strings *StringTable;

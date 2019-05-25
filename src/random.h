/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2019 Brendan Casey
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
  * Random class: Handles pseudo random number generator.
  */
class OPENSMACX_API Random {
	uint32_t seed;

public:
	Random() : seed(0) { } // 00625730
	~Random() { seed = 0; } // 00625740

	void reseed(uint32_t reseedValue);
	uint32_t get(uint32_t min, uint32_t max);
	double get();

	// additional functions to assist with encapsulation
	uint32_t getSeed() { return seed;  }
};

// global
extern Random *Rand;
OPENSMACX_API void __cdecl random_rand();
OPENSMACX_API void __cdecl random_rand_exit();
OPENSMACX_API void __cdecl random_reseed(uint32_t reseedValue);
OPENSMACX_API uint32_t __cdecl random_get();
OPENSMACX_API uint32_t __cdecl random(uint32_t min, uint32_t max);
OPENSMACX_API double __cdecl random();
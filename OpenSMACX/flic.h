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
#include "buffer.h"
#include "palette.h"

 /*
  * Flic class
  */
class OPENSMACX_API Flic {
	DWORD field_0;
	Buffer buffer;
	DWORD field_58C;
	DWORD field_590;
	DWORD field_594;
	DWORD field_598;
	DWORD field_59C;
	DWORD field_5A0;
	DWORD field_5A4;
	Buffer *ptrBuffer2;
	DWORD field_5AC;
	DWORD field_5B0;
	DWORD field_5B4;
	FILE *file;
	Palette *palette;
	DWORD field_5C0[329];

public:
	Flic() { ; }
	~Flic() { ; }
};
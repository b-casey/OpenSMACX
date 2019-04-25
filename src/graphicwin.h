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
#include "win.h"

 /*
  * GraphicWin class
  */
class OPENSMACX_API GraphicWin: Win {
	Buffer buffer;
	DWORD field_9CC;
	DWORD field_9D0;
	DWORD field_9D4;
	DWORD field_9D8;
	DWORD field_9DC;
	DWORD field_9E0;
	DWORD field_9E4;
	DWORD field_9E8;
	DWORD field_9EC;
	DWORD field_9F0;
	DWORD field_9F4;
	DWORD field_9F8;
	DWORD field_9FC;
	DWORD field_A00;
	DWORD field_A04;
	DWORD field_A08;
	DWORD field_A0C;
	DWORD field_A10;

public:
	GraphicWin() { ; }
	~GraphicWin() { ; }
};
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
#include "scroll.h"

 /*
  * StringBox class
  */
class OPENSMACX_API StringBox: GraphicWin {
	DWORD field_A14;
	DWORD field_A18;
	DWORD field_A1C;
	Scroll scroll;
	DWORD field_2B6C;
	DWORD field_2B70;
	DWORD field_2B74;
	DWORD field_2B78;
	DWORD field_2B7C;
	DWORD field_2B80;
	DWORD field_2B84;
	DWORD field_2B88;
	DWORD field_2B8C;
	DWORD field_2B90;
	DWORD field_2B94;
	DWORD field_2B98;
	DWORD field_2B9C;

public:
	StringBox() { ; }
	~StringBox() { ; }
};
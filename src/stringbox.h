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
#include "scroll.h"

 /*
  * StringBox class
  */
class OPENSMACX_API StringBox : GraphicWin {
	uint32_t field_A14;
	uint32_t field_A18;
	uint32_t field_A1C;
	Scroll scroll;
	uint32_t field_2B6C;
	uint32_t field_2B70;
	uint32_t field_2B74;
	uint32_t field_2B78;
	uint32_t field_2B7C;
	uint32_t field_2B80;
	uint32_t field_2B84;
	uint32_t field_2B88;
	uint32_t field_2B8C;
	uint32_t field_2B90;
	uint32_t field_2B94;
	uint32_t field_2B98;
	uint32_t field_2B9C;

public:
	StringBox() { ; }
	~StringBox() { ; }
};

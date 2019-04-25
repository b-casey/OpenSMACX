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
#include "flatbutton.h"
#include "graphicwin.h"
#include "sprite.h"

 /*
  * Scroll class
  */
class OPENSMACX_API Scroll: GraphicWin {
	DWORD field_A14;
	DWORD field_A18;
	DWORD borderColor;
	DWORD field_A20;
	DWORD field_A24;
	DWORD field_A28;
	int position;
	DWORD field_A30;
	DWORD field_A34;
	DWORD field_A38;
	DWORD field_A3C;
	DWORD paging;
	DWORD field_A44;
	DWORD field_A48;
	DWORD field_A4C;
	DWORD field_A50;
	DWORD barThickness1;
	DWORD barThickness2;
	DWORD color;
	DWORD barThickness3;
	Buffer *buffer;
	DWORD bevelThickness;
	DWORD bevelUpper;
	DWORD bevelLower;
	DWORD field_A74;
	Sprite *field_A78;
	Sprite *spriteLeft1;
	Sprite *spriteLeft2;
	Sprite *spriteLeft3;
	DWORD field_A88;
	DWORD field_A8C;
	DWORD field_A90;
	Sprite *spriteRight1;
	Sprite *spriteRight2;
	Sprite *spriteRight3;
	DWORD field_AA0;
	DWORD field_AA4;
	DWORD field_AA8;
	FlatButton flatButtonLeft;
	FlatButton flatButtonRight;
	DWORD field_2144;
	DWORD field_2148;

public:
	Scroll() { ; }
	~Scroll() { ; }
};
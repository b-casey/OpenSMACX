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
	uint32_t field_A14;
	uint32_t field_A18;
	uint32_t borderColor;
	uint32_t field_A20;
	uint32_t field_A24;
	uint32_t field_A28;
	int position;
	uint32_t field_A30;
	uint32_t field_A34;
	uint32_t field_A38;
	uint32_t field_A3C;
	uint32_t paging;
	uint32_t field_A44;
	uint32_t field_A48;
	uint32_t field_A4C;
	uint32_t field_A50;
	uint32_t barThickness1;
	uint32_t barThickness2;
	uint32_t color;
	uint32_t barThickness3;
	Buffer *buffer;
	uint32_t bevelThickness;
	uint32_t bevelUpper;
	uint32_t bevelLower;
	uint32_t field_A74;
	Sprite *field_A78;
	Sprite *spriteLeft1;
	Sprite *spriteLeft2;
	Sprite *spriteLeft3;
	uint32_t field_A88;
	uint32_t field_A8C;
	uint32_t field_A90;
	Sprite *spriteRight1;
	Sprite *spriteRight2;
	Sprite *spriteRight3;
	uint32_t field_AA0;
	uint32_t field_AA4;
	uint32_t field_AA8;
	FlatButton flatButtonLeft;
	FlatButton flatButtonRight;
	uint32_t field_2144;
	uint32_t field_2148;

public:
	Scroll() { ; }
	~Scroll() { ; }
};
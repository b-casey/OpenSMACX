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
#include "graphicwin.h"
#include "time.h"

class ButtonGroup; // forward declaration

 /*
  * BaseButton class
  */
class OPENSMACX_API BaseButton : GraphicWin {
	uint32_t field_A14;
	uint32_t field_A18;
	Time time1;
	uint32_t field_A44;
	uint32_t field_A48;
	Time time2;
	uint32_t field_A74;
	uint32_t field_A78;
	LPSTR name;
	LPSTR bubbleText;
	uint32_t color;
	uint32_t bevelUpper;
	uint32_t bevelLower;
	uint32_t bevelThickness;
	uint32_t field_A94;
	uint32_t field_A98;
	uint32_t field_A9C;
	uint32_t field_AA0;
	uint32_t field_AA4;
	ButtonGroup *btnGroup;
	uint32_t field_AAC;
	uint32_t field_AB0;
	uint32_t field_AB4;

public:
	BaseButton() { ; }
	~BaseButton() { ; }

	int set_bubble_text(LPCSTR input);
	int set_name(LPCSTR input);
};

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
#include "font.h"
#include "heap.h"

 /*
  * Dialog class
  */
class OPENSMACX_API Dialog {
	LPVOID vtable;
	Heap heap;
	Heap *pHeap;
	DWORD field_1C;
	DWORD field_20;
	DWORD field_24;
	DWORD field_28;
	DWORD field_2C;
	DWORD field_30;
	DWORD field_34;
	DWORD field_38;
	DWORD field_3C;
	DWORD field_40;
	DWORD field_44;
	DWORD field_48;
	DWORD field_4C;
	DWORD field_50;
	DWORD field_54;
	DWORD field_58;
	DWORD field_5C;
	DWORD field_60;
	DWORD field_64;
	DWORD field_68;
	DWORD field_6C;
	Font *font1;
	Font *font2;
	Font *font3;
	DWORD textColorA;
	DWORD textColor2A;
	DWORD textColor3A;
	DWORD textColorB;
	DWORD textColor2B;
	DWORD textColor3B;
	DWORD textColorC;
	DWORD textColor2C;
	DWORD textColor3C;
	DWORD textColorD;
	DWORD textColor2D;
	DWORD textColor3D;
	DWORD field_AC;
	DWORD field_B0;
	DWORD field_B4;
	DWORD field_B8;
	DWORD field_BC;
	DWORD field_C0;
	DWORD field_C4;
	DWORD field_C8;
	DWORD field_CC;
	DWORD field_D0;
	DWORD field_D4;
	DWORD field_D8;
	DWORD field_DC;
	DWORD field_E0;
	DWORD field_E4;
	DWORD field_E8;
	DWORD stateFlag;
	DWORD field_F0;

public:
	Dialog() { ; }
	~Dialog() { ; }
};
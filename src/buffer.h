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
#include "spot.h"

 /*
  * Buffer class
  */
class OPENSMACX_API Buffer {
	typedef int32_t Dib;
	LPVOID vtable;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	RECT rect1;
	RECT rect2;
	uint32_t field_40[4];
	uint32_t field_50;
	LPVOID *ppvBits;
	uint32_t field_58;
	uint32_t field_5C;
	HDC hdc2;
	HDC hdc;
	uint32_t field_68;
	uint32_t field_6C;
	HRGN field_70;
	uint32_t field_74;
	HBITMAP hBitmap;
	const BITMAPINFO *pbmi;
	uint32_t field_80;
	uint32_t field_84;
	uint16_t field_88;
	uint16_t field_8A;
	uint32_t field_8C;
	uint32_t field_90;
	uint32_t field_94;
	uint32_t field_98;
	uint32_t field_9C;
	uint32_t field_A0;
	Dib dib[256];
	uint32_t field_4A4;
	uint32_t field_4A8;
	uint32_t field_4AC;
	Spot spot;
	uint8_t field_4BC[80];
	uint32_t field_50C;
	uint32_t field_510;
	uint32_t field_514;
	uint32_t field_518;
	uint32_t field_51C;
	uint32_t field_520;
	uint32_t field_524;
	uint32_t field_528;
	Font *pFont1;
	Font *pFont2;
	Font *pFont3;
	Font *pFont4;
	uint32_t colorVal1;
	uint32_t color2Val1;
	uint32_t color3Val1;
	uint32_t colorhyperVal1;
	uint32_t colorVal2;
	uint32_t color2Val2;
	uint32_t color3Val2;
	uint32_t colorhyperVal2;
	uint32_t colorVal3;
	uint32_t color2Val3;
	uint32_t color3Val3;
	uint32_t colorhyperVal3;
	uint32_t colorVal4;
	uint32_t color2Val4;
	uint32_t color3Val4;
	uint32_t colorhyperVal4;
	uint32_t field_57C;
	int8_t field_580;
	uint32_t field_584;

public:
	Buffer() { ; }
	~Buffer() { ; }
};
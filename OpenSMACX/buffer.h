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
	typedef __int32 Dib;
	LPVOID vtable;
	DWORD field_4;
	DWORD field_8;
	DWORD field_C;
	DWORD field_10;
	DWORD field_14;
	DWORD field_18;
	DWORD field_1C;
	RECT rect1;
	RECT rect2;
	DWORD field_40[4];
	DWORD field_50;
	LPVOID *ppvBits;
	DWORD field_58;
	DWORD field_5C;
	HDC hdc2;
	HDC hdc;
	DWORD field_68;
	DWORD field_6C;
	HRGN field_70;
	DWORD field_74;
	HBITMAP hBitmap;
	const BITMAPINFO *pbmi;
	DWORD field_80;
	DWORD field_84;
	WORD field_88;
	WORD field_8A;
	DWORD field_8C;
	DWORD field_90;
	DWORD field_94;
	DWORD field_98;
	DWORD field_9C;
	DWORD field_A0;
	Dib dib[256];
	DWORD field_4A4;
	DWORD field_4A8;
	DWORD field_4AC;
	Spot spot;
	BYTE field_4BC[80];
	DWORD field_50C;
	DWORD field_510;
	DWORD field_514;
	DWORD field_518;
	DWORD field_51C;
	DWORD field_520;
	DWORD field_524;
	DWORD field_528;
	Font *pFont1;
	Font *pFont2;
	Font *pFont3;
	Font *pFont4;
	DWORD colorVal1;
	DWORD color2Val1;
	DWORD color3Val1;
	DWORD colorhyperVal1;
	DWORD colorVal2;
	DWORD color2Val2;
	DWORD color3Val2;
	DWORD colorhyperVal2;
	DWORD colorVal3;
	DWORD color2Val3;
	DWORD color3Val3;
	DWORD colorhyperVal3;
	DWORD colorVal4;
	DWORD color2Val4;
	DWORD color3Val4;
	DWORD colorhyperVal4;
	DWORD field_57C;
	CHAR field_580;
	DWORD field_584;

public:
	Buffer() { ; }
	~Buffer() { ; }
};
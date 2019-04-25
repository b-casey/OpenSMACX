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
#include "basebutton.h"
#include "sprite.h"

 /*
  * FlatButton class
  */
class OPENSMACX_API FlatButton: public BaseButton {
	DWORD field_AB8;
	DWORD field_ABC;
	DWORD field_AC0;
	DWORD field_AC4;
	DWORD field_AC8;
	DWORD field_ACC;
	DWORD field_AD0;
	DWORD field_AD4;
	DWORD field_AD8;
	DWORD field_ADC;
	Sprite *sprite0;
	DWORD field_AE4;
	DWORD field_AE8;
	DWORD field_AEC;
	DWORD field_AF0;
	DWORD field_AF4;
	DWORD field_AF8;
	DWORD field_AFC;
	DWORD field_B00;
	DWORD field_B04;
	DWORD field_B08;
	DWORD field_B0C;
	Sprite *sprite1;
	Sprite *sprite2;
	Sprite *sprite3;
	DWORD field_B1C;
	DWORD field_B20;
	DWORD field_B24;
	DWORD field_B28;
	DWORD field_B2C;
	DWORD field_B30;
	DWORD field_B34;
	DWORD field_B38;
	DWORD field_B3C;
	DWORD field_B40;
	DWORD field_B44;
	DWORD field_B48;

public:
	FlatButton() { ; }
	~FlatButton() { ; }
};
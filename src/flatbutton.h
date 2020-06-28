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
#include "basebutton.h"
#include "sprite.h"

 /*
  * FlatButton class
  */
class OPENSMACX_API FlatButton : public BaseButton {
	uint32_t field_AB8;
	uint32_t field_ABC;
	uint32_t field_AC0;
	uint32_t field_AC4;
	uint32_t field_AC8;
	uint32_t field_ACC;
	uint32_t field_AD0;
	uint32_t field_AD4;
	uint32_t field_AD8;
	uint32_t field_ADC;
	Sprite *sprite0;
	uint32_t field_AE4;
	uint32_t field_AE8;
	uint32_t field_AEC;
	uint32_t field_AF0;
	uint32_t field_AF4;
	uint32_t field_AF8;
	uint32_t field_AFC;
	uint32_t field_B00;
	uint32_t field_B04;
	uint32_t field_B08;
	uint32_t field_B0C;
	Sprite *sprite1;
	Sprite *sprite2;
	Sprite *sprite3;
	uint32_t field_B1C;
	uint32_t field_B20;
	uint32_t field_B24;
	uint32_t field_B28;
	uint32_t field_B2C;
	uint32_t field_B30;
	uint32_t field_B34;
	uint32_t field_B38;
	uint32_t field_B3C;
	uint32_t field_B40;
	uint32_t field_B44;
	uint32_t field_B48;

public:
	FlatButton() { ; }
	~FlatButton() { ; }
};

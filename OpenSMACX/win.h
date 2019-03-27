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
#include "autosound.h"
#include "buffer.h"
#include "heap.h"
#include "spot.h"
#include "sprite.h"

class Menu; // forward declaration
class Scroll; // forward declaration

 /*
  * Win class: Most basic window class.
  */
class OPENSMACX_API Win {
	AutoSound autoSound;
	DWORD field_98;
	DWORD field_9C;
	DWORD field_A0;
	DWORD field_A4;
	DWORD field_A8;
	DWORD field_AC;
	DWORD field_B0;
	Buffer *buffer1;
	Buffer *buffer2;
	Buffer *buffer3;
	Buffer *buffer4;
	Win *winParent;
	DWORD field_C8;
	DWORD field_CC;
	DWORD field_D0;
	DWORD field_D4;
	DWORD field_D8;
	Heap heap;
    Menu *menu;
	DWORD field_F4;
	DWORD field_F8;
	DWORD field_FC;
	DWORD field_100;
	DWORD field_104;
	DWORD field_108;
	DWORD field_10C;
	DWORD field_110;
	int captionHeight;
	int borderThickness;
	int bottomBorderThickness;
	DWORD field_120;
	DWORD field_124;
	DWORD field_128;
	DWORD field_12C;
	DWORD field_130;
	DWORD field_134;
	DWORD field_138;
	DWORD field_13C;
	DWORD field_140;
	DWORD field_144;
	DWORD field_148;
	DWORD field_14C;
	DWORD field_150;
	DWORD field_154;
	DWORD field_158;
	DWORD field_15C;
	DWORD field_160;
	DWORD field_164;
	DWORD field_168;
	DWORD field_16C;
	DWORD field_170;
	DWORD field_174;
	DWORD field_178;
	DWORD field_17C;
	DWORD field_180;
	DWORD field_184;
	Sprite *sprtCursor;
	DWORD field_18C;
	DWORD field_190;
	HCURSOR *hCursor;
	int cursorName;
	DWORD field_19C;
	DWORD field_1A0;
	DWORD field_1A4;
	DWORD field_1A8;
	DWORD field_1AC;
	DWORD field_1B0;
	DWORD field_1B4;
	DWORD field_1B8;
	DWORD field_1BC;
	DWORD field_1C0;
	DWORD field_1C4;
	DWORD field_1C8;
	DWORD field_1CC;
	DWORD field_1D0;
	DWORD field_1D4;
	DWORD field_1D8;
	DWORD field_1DC;
	DWORD field_1E0;
	DWORD field_1E4;
	DWORD field_1E8;
	DWORD field_1EC;
	DWORD field_1F0;
	DWORD field_1F4;
	DWORD field_1F8;
	DWORD field_1FC;
	DWORD field_200;
	DWORD field_204;
	DWORD field_208;
	DWORD field_20C;
	DWORD field_210;
	DWORD field_214;
	DWORD field_218;
	DWORD field_21C;
	DWORD field_220;
	DWORD field_224;
	DWORD field_228;
	DWORD field_22C;
	DWORD field_230;
	DWORD field_234;
	DWORD field_238;
	DWORD field_23C;
	DWORD field_240;
	DWORD field_244;
	DWORD field_248;
	DWORD field_24C;
	DWORD field_250;
	DWORD field_254;
	DWORD field_258;
	DWORD field_25C;
	DWORD field_260;
	DWORD field_264;
	DWORD field_268;
	DWORD field_26C;
	DWORD field_270;
	DWORD field_274;
	DWORD field_278;
	DWORD field_27C;
	DWORD field_280;
	DWORD field_284;
	DWORD field_288;
	DWORD field_28C;
	DWORD field_290;
	DWORD field_294;
	DWORD field_298;
	DWORD field_29C;
	DWORD field_2A0;
	DWORD field_2A4;
	DWORD field_2A8;
	DWORD field_2AC;
	DWORD field_2B0;
	DWORD field_2B4;
	DWORD field_2B8;
	DWORD field_2BC;
	DWORD field_2C0;
	DWORD field_2C4;
	DWORD field_2C8;
	DWORD field_2CC;
	DWORD field_2D0;
	DWORD field_2D4;
	DWORD field_2D8;
	DWORD field_2DC;
	DWORD field_2E0;
	DWORD field_2E4;
	DWORD field_2E8;
	DWORD field_2EC;
	DWORD field_2F0;
	DWORD field_2F4;
	DWORD field_2F8;
	DWORD field_2FC;
	DWORD field_300;
	DWORD field_304;
	DWORD field_308;
	DWORD field_30C;
	DWORD field_310;
	DWORD field_314;
	DWORD field_318;
	DWORD field_31C;
	DWORD field_320;
	DWORD field_324;
	DWORD field_328;
	DWORD field_32C;
	DWORD field_330;
	DWORD field_334;
	DWORD field_338;
	DWORD field_33C;
	DWORD field_340;
	DWORD field_344;
	DWORD field_348;
	DWORD field_34C;
	DWORD field_350;
	DWORD field_354;
	DWORD field_358;
	DWORD field_35C;
	DWORD field_360;
	DWORD field_364;
	DWORD field_368;
	DWORD field_36C;
	DWORD field_370;
	DWORD field_374;
	DWORD field_378;
	DWORD field_37C;
	DWORD field_380;
	DWORD field_384;
	DWORD field_388;
	DWORD field_38C;
	DWORD field_390;
	DWORD field_394;
	DWORD field_398;
	DWORD field_39C;
	DWORD field_3A0;
	DWORD field_3A4;
	DWORD field_3A8;
	DWORD field_3AC;
	DWORD field_3B0;
	Buffer *buffer5;
	DWORD field_3B8;
	DWORD field_3BC;
	Spot spot;
	DWORD field_3CC;
	DWORD field_3D0;
	DWORD field_3D4;
	DWORD field_3D8;
	DWORD field_3DC;
	DWORD field_3E0;
	DWORD field_3E4;
	DWORD field_3E8;
	DWORD field_3EC;
	DWORD field_3F0;
	DWORD field_3F4;
	DWORD field_3F8;
	Buffer *buffer6;
	DWORD field_400;
	DWORD field_404;
	DWORD field_408;
	DWORD field_40C;
	DWORD field_410;
	DWORD field_414;
	DWORD field_418;
	DWORD field_41C;
	DWORD field_420;
	DWORD field_424;
	DWORD field_428;
	DWORD field_42C;
	DWORD field_430;
	DWORD field_434;
	DWORD field_438;
	Scroll *scrollVert;
	Scroll *scrollHorz;

public:
	Win() { ; }
	~Win() { ; }
};
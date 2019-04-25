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
#include "time.h"

 /*
  * Palette class
  */
class OPENSMACX_API Palette {
	struct PaletteInternal {
		DWORD field_0;
		Time *ptrTime;
		BYTE field_8;
		BYTE field_9;
		BYTE field_A;
		BYTE field_B;
		void *field_C;
	};
	DWORD field_0;
	DWORD field_4;
	DWORD field_8;
	DWORD field_C;
	DWORD field_10;
	DWORD field_14;
	DWORD field_18;
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
	DWORD field_70;
	DWORD field_74;
	DWORD field_78;
	DWORD field_7C;
	DWORD field_80;
	DWORD field_84;
	DWORD field_88;
	DWORD field_8C;
	DWORD field_90;
	DWORD field_94;
	DWORD field_98;
	DWORD field_9C;
	DWORD field_A0;
	DWORD field_A4;
	DWORD field_A8;
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
	DWORD field_EC;
	DWORD field_F0;
	DWORD field_F4;
	DWORD field_F8;
	DWORD field_FC;
	DWORD field_100;
	DWORD field_104;
	DWORD field_108;
	DWORD field_10C;
	DWORD field_110;
	DWORD field_114;
	DWORD field_118;
	DWORD field_11C;
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
	DWORD field_188;
	DWORD field_18C;
	DWORD field_190;
	DWORD field_194;
	DWORD field_198;
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
	DWORD field_3B4;
	DWORD field_3B8;
	DWORD field_3BC;
	DWORD field_3C0;
	DWORD field_3C4;
	DWORD field_3C8;
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
	DWORD field_3FC;
	DWORD field_400;
	PaletteInternal paletteInternal[5];

public:
	Palette() { ; }
	~Palette() { ; }
};
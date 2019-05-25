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
		uint32_t field_0;
		Time *ptrTime;
		uint8_t field_8;
		uint8_t field_9;
		uint8_t field_A;
		uint8_t field_B;
		void *field_C;
	};
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t field_5C;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t field_7C;
	uint32_t field_80;
	uint32_t field_84;
	uint32_t field_88;
	uint32_t field_8C;
	uint32_t field_90;
	uint32_t field_94;
	uint32_t field_98;
	uint32_t field_9C;
	uint32_t field_A0;
	uint32_t field_A4;
	uint32_t field_A8;
	uint32_t field_AC;
	uint32_t field_B0;
	uint32_t field_B4;
	uint32_t field_B8;
	uint32_t field_BC;
	uint32_t field_C0;
	uint32_t field_C4;
	uint32_t field_C8;
	uint32_t field_CC;
	uint32_t field_D0;
	uint32_t field_D4;
	uint32_t field_D8;
	uint32_t field_DC;
	uint32_t field_E0;
	uint32_t field_E4;
	uint32_t field_E8;
	uint32_t field_EC;
	uint32_t field_F0;
	uint32_t field_F4;
	uint32_t field_F8;
	uint32_t field_FC;
	uint32_t field_100;
	uint32_t field_104;
	uint32_t field_108;
	uint32_t field_10C;
	uint32_t field_110;
	uint32_t field_114;
	uint32_t field_118;
	uint32_t field_11C;
	uint32_t field_120;
	uint32_t field_124;
	uint32_t field_128;
	uint32_t field_12C;
	uint32_t field_130;
	uint32_t field_134;
	uint32_t field_138;
	uint32_t field_13C;
	uint32_t field_140;
	uint32_t field_144;
	uint32_t field_148;
	uint32_t field_14C;
	uint32_t field_150;
	uint32_t field_154;
	uint32_t field_158;
	uint32_t field_15C;
	uint32_t field_160;
	uint32_t field_164;
	uint32_t field_168;
	uint32_t field_16C;
	uint32_t field_170;
	uint32_t field_174;
	uint32_t field_178;
	uint32_t field_17C;
	uint32_t field_180;
	uint32_t field_184;
	uint32_t field_188;
	uint32_t field_18C;
	uint32_t field_190;
	uint32_t field_194;
	uint32_t field_198;
	uint32_t field_19C;
	uint32_t field_1A0;
	uint32_t field_1A4;
	uint32_t field_1A8;
	uint32_t field_1AC;
	uint32_t field_1B0;
	uint32_t field_1B4;
	uint32_t field_1B8;
	uint32_t field_1BC;
	uint32_t field_1C0;
	uint32_t field_1C4;
	uint32_t field_1C8;
	uint32_t field_1CC;
	uint32_t field_1D0;
	uint32_t field_1D4;
	uint32_t field_1D8;
	uint32_t field_1DC;
	uint32_t field_1E0;
	uint32_t field_1E4;
	uint32_t field_1E8;
	uint32_t field_1EC;
	uint32_t field_1F0;
	uint32_t field_1F4;
	uint32_t field_1F8;
	uint32_t field_1FC;
	uint32_t field_200;
	uint32_t field_204;
	uint32_t field_208;
	uint32_t field_20C;
	uint32_t field_210;
	uint32_t field_214;
	uint32_t field_218;
	uint32_t field_21C;
	uint32_t field_220;
	uint32_t field_224;
	uint32_t field_228;
	uint32_t field_22C;
	uint32_t field_230;
	uint32_t field_234;
	uint32_t field_238;
	uint32_t field_23C;
	uint32_t field_240;
	uint32_t field_244;
	uint32_t field_248;
	uint32_t field_24C;
	uint32_t field_250;
	uint32_t field_254;
	uint32_t field_258;
	uint32_t field_25C;
	uint32_t field_260;
	uint32_t field_264;
	uint32_t field_268;
	uint32_t field_26C;
	uint32_t field_270;
	uint32_t field_274;
	uint32_t field_278;
	uint32_t field_27C;
	uint32_t field_280;
	uint32_t field_284;
	uint32_t field_288;
	uint32_t field_28C;
	uint32_t field_290;
	uint32_t field_294;
	uint32_t field_298;
	uint32_t field_29C;
	uint32_t field_2A0;
	uint32_t field_2A4;
	uint32_t field_2A8;
	uint32_t field_2AC;
	uint32_t field_2B0;
	uint32_t field_2B4;
	uint32_t field_2B8;
	uint32_t field_2BC;
	uint32_t field_2C0;
	uint32_t field_2C4;
	uint32_t field_2C8;
	uint32_t field_2CC;
	uint32_t field_2D0;
	uint32_t field_2D4;
	uint32_t field_2D8;
	uint32_t field_2DC;
	uint32_t field_2E0;
	uint32_t field_2E4;
	uint32_t field_2E8;
	uint32_t field_2EC;
	uint32_t field_2F0;
	uint32_t field_2F4;
	uint32_t field_2F8;
	uint32_t field_2FC;
	uint32_t field_300;
	uint32_t field_304;
	uint32_t field_308;
	uint32_t field_30C;
	uint32_t field_310;
	uint32_t field_314;
	uint32_t field_318;
	uint32_t field_31C;
	uint32_t field_320;
	uint32_t field_324;
	uint32_t field_328;
	uint32_t field_32C;
	uint32_t field_330;
	uint32_t field_334;
	uint32_t field_338;
	uint32_t field_33C;
	uint32_t field_340;
	uint32_t field_344;
	uint32_t field_348;
	uint32_t field_34C;
	uint32_t field_350;
	uint32_t field_354;
	uint32_t field_358;
	uint32_t field_35C;
	uint32_t field_360;
	uint32_t field_364;
	uint32_t field_368;
	uint32_t field_36C;
	uint32_t field_370;
	uint32_t field_374;
	uint32_t field_378;
	uint32_t field_37C;
	uint32_t field_380;
	uint32_t field_384;
	uint32_t field_388;
	uint32_t field_38C;
	uint32_t field_390;
	uint32_t field_394;
	uint32_t field_398;
	uint32_t field_39C;
	uint32_t field_3A0;
	uint32_t field_3A4;
	uint32_t field_3A8;
	uint32_t field_3AC;
	uint32_t field_3B0;
	uint32_t field_3B4;
	uint32_t field_3B8;
	uint32_t field_3BC;
	uint32_t field_3C0;
	uint32_t field_3C4;
	uint32_t field_3C8;
	uint32_t field_3CC;
	uint32_t field_3D0;
	uint32_t field_3D4;
	uint32_t field_3D8;
	uint32_t field_3DC;
	uint32_t field_3E0;
	uint32_t field_3E4;
	uint32_t field_3E8;
	uint32_t field_3EC;
	uint32_t field_3F0;
	uint32_t field_3F4;
	uint32_t field_3F8;
	uint32_t field_3FC;
	uint32_t field_400;
	PaletteInternal paletteInternal[5];

public:
	Palette() { ; }
	~Palette() { ; }
};
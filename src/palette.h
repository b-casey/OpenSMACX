/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2021 Brendan Casey
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
class DLLEXPORT Palette {
 public:
  Palette() { ; }
  ~Palette() { ; }

 private:
  struct PaletteInternal {
      uint32_t field_0;
      Time *time;
      uint8_t field_8;
      uint8_t field_9;
      uint8_t field_A;
      uint8_t field_B;
      void *field_C;
  };

  uint32_t field_0_;
  uint32_t field_4_;
  uint32_t field_8_;
  uint32_t field_C_;
  uint32_t field_10_;
  uint32_t field_14_;
  uint32_t field_18_;
  uint32_t field_1C_;
  uint32_t field_20_;
  uint32_t field_24_;
  uint32_t field_28_;
  uint32_t field_2C_;
  uint32_t field_30_;
  uint32_t field_34_;
  uint32_t field_38_;
  uint32_t field_3C_;
  uint32_t field_40_;
  uint32_t field_44_;
  uint32_t field_48_;
  uint32_t field_4C_;
  uint32_t field_50_;
  uint32_t field_54_;
  uint32_t field_58_;
  uint32_t field_5C_;
  uint32_t field_60_;
  uint32_t field_64_;
  uint32_t field_68_;
  uint32_t field_6C_;
  uint32_t field_70_;
  uint32_t field_74_;
  uint32_t field_78_;
  uint32_t field_7C_;
  uint32_t field_80_;
  uint32_t field_84_;
  uint32_t field_88_;
  uint32_t field_8C_;
  uint32_t field_90_;
  uint32_t field_94_;
  uint32_t field_98_;
  uint32_t field_9C_;
  uint32_t field_A0_;
  uint32_t field_A4_;
  uint32_t field_A8_;
  uint32_t field_AC_;
  uint32_t field_B0_;
  uint32_t field_B4_;
  uint32_t field_B8_;
  uint32_t field_BC_;
  uint32_t field_C0_;
  uint32_t field_C4_;
  uint32_t field_C8_;
  uint32_t field_CC_;
  uint32_t field_D0_;
  uint32_t field_D4_;
  uint32_t field_D8_;
  uint32_t field_DC_;
  uint32_t field_E0_;
  uint32_t field_E4_;
  uint32_t field_E8_;
  uint32_t field_EC_;
  uint32_t field_F0_;
  uint32_t field_F4_;
  uint32_t field_F8_;
  uint32_t field_FC_;
  uint32_t field_100_;
  uint32_t field_104_;
  uint32_t field_108_;
  uint32_t field_10C_;
  uint32_t field_110_;
  uint32_t field_114_;
  uint32_t field_118_;
  uint32_t field_11C_;
  uint32_t field_120_;
  uint32_t field_124_;
  uint32_t field_128_;
  uint32_t field_12C_;
  uint32_t field_130_;
  uint32_t field_134_;
  uint32_t field_138_;
  uint32_t field_13C_;
  uint32_t field_140_;
  uint32_t field_144_;
  uint32_t field_148_;
  uint32_t field_14C_;
  uint32_t field_150_;
  uint32_t field_154_;
  uint32_t field_158_;
  uint32_t field_15C_;
  uint32_t field_160_;
  uint32_t field_164_;
  uint32_t field_168_;
  uint32_t field_16C_;
  uint32_t field_170_;
  uint32_t field_174_;
  uint32_t field_178_;
  uint32_t field_17C_;
  uint32_t field_180_;
  uint32_t field_184_;
  uint32_t field_188_;
  uint32_t field_18C_;
  uint32_t field_190_;
  uint32_t field_194_;
  uint32_t field_198_;
  uint32_t field_19C_;
  uint32_t field_1A0_;
  uint32_t field_1A4_;
  uint32_t field_1A8_;
  uint32_t field_1AC_;
  uint32_t field_1B0_;
  uint32_t field_1B4_;
  uint32_t field_1B8_;
  uint32_t field_1BC_;
  uint32_t field_1C0_;
  uint32_t field_1C4_;
  uint32_t field_1C8_;
  uint32_t field_1CC_;
  uint32_t field_1D0_;
  uint32_t field_1D4_;
  uint32_t field_1D8_;
  uint32_t field_1DC_;
  uint32_t field_1E0_;
  uint32_t field_1E4_;
  uint32_t field_1E8_;
  uint32_t field_1EC_;
  uint32_t field_1F0_;
  uint32_t field_1F4_;
  uint32_t field_1F8_;
  uint32_t field_1FC_;
  uint32_t field_200_;
  uint32_t field_204_;
  uint32_t field_208_;
  uint32_t field_20C_;
  uint32_t field_210_;
  uint32_t field_214_;
  uint32_t field_218_;
  uint32_t field_21C_;
  uint32_t field_220_;
  uint32_t field_224_;
  uint32_t field_228_;
  uint32_t field_22C_;
  uint32_t field_230_;
  uint32_t field_234_;
  uint32_t field_238_;
  uint32_t field_23C_;
  uint32_t field_240_;
  uint32_t field_244_;
  uint32_t field_248_;
  uint32_t field_24C_;
  uint32_t field_250_;
  uint32_t field_254_;
  uint32_t field_258_;
  uint32_t field_25C_;
  uint32_t field_260_;
  uint32_t field_264_;
  uint32_t field_268_;
  uint32_t field_26C_;
  uint32_t field_270_;
  uint32_t field_274_;
  uint32_t field_278_;
  uint32_t field_27C_;
  uint32_t field_280_;
  uint32_t field_284_;
  uint32_t field_288_;
  uint32_t field_28C_;
  uint32_t field_290_;
  uint32_t field_294_;
  uint32_t field_298_;
  uint32_t field_29C_;
  uint32_t field_2A0_;
  uint32_t field_2A4_;
  uint32_t field_2A8_;
  uint32_t field_2AC_;
  uint32_t field_2B0_;
  uint32_t field_2B4_;
  uint32_t field_2B8_;
  uint32_t field_2BC_;
  uint32_t field_2C0_;
  uint32_t field_2C4_;
  uint32_t field_2C8_;
  uint32_t field_2CC_;
  uint32_t field_2D0_;
  uint32_t field_2D4_;
  uint32_t field_2D8_;
  uint32_t field_2DC_;
  uint32_t field_2E0_;
  uint32_t field_2E4_;
  uint32_t field_2E8_;
  uint32_t field_2EC_;
  uint32_t field_2F0_;
  uint32_t field_2F4_;
  uint32_t field_2F8_;
  uint32_t field_2FC_;
  uint32_t field_300_;
  uint32_t field_304_;
  uint32_t field_308_;
  uint32_t field_30C_;
  uint32_t field_310_;
  uint32_t field_314_;
  uint32_t field_318_;
  uint32_t field_31C_;
  uint32_t field_320_;
  uint32_t field_324_;
  uint32_t field_328_;
  uint32_t field_32C_;
  uint32_t field_330_;
  uint32_t field_334_;
  uint32_t field_338_;
  uint32_t field_33C_;
  uint32_t field_340_;
  uint32_t field_344_;
  uint32_t field_348_;
  uint32_t field_34C_;
  uint32_t field_350_;
  uint32_t field_354_;
  uint32_t field_358_;
  uint32_t field_35C_;
  uint32_t field_360_;
  uint32_t field_364_;
  uint32_t field_368_;
  uint32_t field_36C_;
  uint32_t field_370_;
  uint32_t field_374_;
  uint32_t field_378_;
  uint32_t field_37C_;
  uint32_t field_380_;
  uint32_t field_384_;
  uint32_t field_388_;
  uint32_t field_38C_;
  uint32_t field_390_;
  uint32_t field_394_;
  uint32_t field_398_;
  uint32_t field_39C_;
  uint32_t field_3A0_;
  uint32_t field_3A4_;
  uint32_t field_3A8_;
  uint32_t field_3AC_;
  uint32_t field_3B0_;
  uint32_t field_3B4_;
  uint32_t field_3B8_;
  uint32_t field_3BC_;
  uint32_t field_3C0_;
  uint32_t field_3C4_;
  uint32_t field_3C8_;
  uint32_t field_3CC_;
  uint32_t field_3D0_;
  uint32_t field_3D4_;
  uint32_t field_3D8_;
  uint32_t field_3DC_;
  uint32_t field_3E0_;
  uint32_t field_3E4_;
  uint32_t field_3E8_;
  uint32_t field_3EC_;
  uint32_t field_3F0_;
  uint32_t field_3F4_;
  uint32_t field_3F8_;
  uint32_t field_3FC_;
  uint32_t field_400_;
  PaletteInternal internal_[5];
};

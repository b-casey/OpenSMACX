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
#include "font.h"
#include "heap.h"

 /*
  * Dialog class
  */
class DLLEXPORT Dialog {
 public:
  Dialog() { ; }
  ~Dialog() { ; }

 private:
  LPVOID vtable_;
  Heap heap_;
  Heap *heap_ptr_;
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
  Font *font1_;
  Font *font2_;
  Font *font3_;
  uint32_t text_color_a_;
  uint32_t text_color_2a_;
  uint32_t text_color_3a_;
  uint32_t text_color_b_;
  uint32_t text_color_2b_;
  uint32_t text_color_3b_;
  uint32_t text_color_c_;
  uint32_t text_color_2c_;
  uint32_t text_color_3c_;
  uint32_t text_color_d_;
  uint32_t text_color_2d_;
  uint32_t text_color_3d_;
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
  uint32_t state_flag_;
  uint32_t field_F0_;
};

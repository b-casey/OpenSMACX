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
 * but WITHOUT ANY WARRANTY_; without even the implied warranty of
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
class DLLEXPORT Buffer {
 public:
  Buffer() { ; }
  ~Buffer() { ; }

 private:
  typedef int32_t Dib;
  
  LPVOID vtable_;
  uint32_t field_4_;
  uint32_t field_8_;
  uint32_t field_C_;
  uint32_t field_10_;
  uint32_t field_14_;
  uint32_t field_18_;
  uint32_t field_1C_;
  RECT rect1_;
  RECT rect2_;
  uint32_t field_40_[4];
  uint32_t field_50_;
  LPVOID *ppv_bits_;
  uint32_t field_58_;
  uint32_t field_5C_;
  HDC hdc2_;
  HDC hdc_;
  uint32_t field_68_;
  uint32_t field_6C_;
  HRGN field_70_;
  uint32_t field_74_;
  HBITMAP bitmap_handle_;
  const BITMAPINFO *bitmap_info_;
  uint32_t field_80_;
  uint32_t field_84_;
  uint16_t field_88_;
  uint16_t field_8A_;
  uint32_t field_8C_;
  uint32_t field_90_;
  uint32_t field_94_;
  uint32_t field_98_;
  uint32_t field_9C_;
  uint32_t field_A0_;
  Dib dib_[256];
  uint32_t field_4A4_;
  uint32_t field_4A8_;
  uint32_t field_4AC_;
  Spot spot_;
  uint8_t field_4BC_[80];
  uint32_t field_50C_;
  uint32_t field_510_;
  uint32_t field_514_;
  uint32_t field_518_;
  uint32_t field_51C_;
  uint32_t field_520_;
  uint32_t field_524_;
  uint32_t field_528_;
  Font *font1_;
  Font *font2_;
  Font *font3_;
  Font *font4_;
  uint32_t color_val_1_;
  uint32_t color_2_val_1_;
  uint32_t color_3_val_1_;
  uint32_t color_hyper_val_1_;
  uint32_t color_val_2_;
  uint32_t color_2_val_2_;
  uint32_t color_3_val_2_;
  uint32_t color_hyper_val_2_;
  uint32_t color_val_3_;
  uint32_t color_2_val_3_;
  uint32_t color_3_val_3_;
  uint32_t color_hyper_val_3_;
  uint32_t color_val_4_;
  uint32_t color_2_val_4_;
  uint32_t color_3_val_4_;
  uint32_t color_hyper_val_4_;
  uint32_t field_57C_;
  int8_t field_580_;
  uint32_t field_584_;
};

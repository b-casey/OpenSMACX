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
#include "stdafx.h"

 /*
  * Sprite class
  */
class DLLEXPORT Sprite {
 public:
  Sprite() { ; }
  ~Sprite() { ; }

 private:
  int field_0_;
  int field_4_;
  char field_8_;
  char pad1_;
  char pad2_;
  char pad3_;
  int field_C_;
  int field_10_;
  int field_14_;
  int field_18_;
  int field_1C_;
  int field_20_;
  int field_24_;
  int field_28_;
};

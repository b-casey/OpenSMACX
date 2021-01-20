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
#include "buffer.h"
#include "palette.h"

 /*
  * Flic class
  */
class DLLEXPORT Flic {
 public:
  Flic() { ; }
  ~Flic() { ; }

 private:
  uint32_t field_0_;
  Buffer buffer_;
  uint32_t field_58C_;
  uint32_t field_590_;
  uint32_t field_594_;
  uint32_t field_598_;
  uint32_t field_59C_;
  uint32_t field_5A0_;
  uint32_t field_5A4_;
  Buffer *buffer2_;
  uint32_t field_5AC_;
  uint32_t field_5B0_;
  uint32_t field_5B4_;
  FILE *file_;
  Palette *palette_;
  uint32_t field_5C0_[329];
};

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
#include "win.h"

 /*
  * GraphicWin class
  */
class DLLEXPORT GraphicWin : Win {
 public:
  GraphicWin() { ; }
  ~GraphicWin() { ; }
 private:
  Buffer buffer_;
  uint32_t field_9CC_;
  uint32_t field_9D0_;
  uint32_t field_9D4_;
  uint32_t field_9D8_;
  uint32_t field_9DC_;
  uint32_t field_9E0_;
  uint32_t field_9E4_;
  uint32_t field_9E8_;
  uint32_t field_9EC_;
  uint32_t field_9F0_;
  uint32_t field_9F4_;
  uint32_t field_9F8_;
  uint32_t field_9FC_;
  uint32_t field_A00_;
  uint32_t field_A04_;
  uint32_t field_A08_;
  uint32_t field_A0C_;
  uint32_t field_A10_;
};

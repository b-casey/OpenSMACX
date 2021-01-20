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
#include "basebutton.h"

 /*
  * ButtonGroup class
  */
class DLLEXPORT ButtonGroup {
 public:
  ButtonGroup() { ; }
  ~ButtonGroup() { ; }

 private:
  BaseButton *buttons_[32];
  uint32_t count_;
  uint32_t field_84_;
  uint32_t field_88_;
  uint32_t field_8C_;
  uint32_t field_90_;
};

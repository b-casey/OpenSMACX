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
#include "graphicwin.h"
#include "time.h"

class ButtonGroup; // forward declaration

 /*
  * BaseButton class
  */
class DLLEXPORT BaseButton : GraphicWin {
 public:
  BaseButton() { ; }
  ~BaseButton() { ; }

  int set_bubble_text(LPCSTR input);
  int set_name(LPCSTR input);

 private:
  uint32_t field_A14_;
  uint32_t field_A18_;
  Time time1_;
  uint32_t field_A44_;
  uint32_t field_A48_;
  Time time2_;
  uint32_t field_A74_;
  uint32_t field_A78_;
  LPSTR name_;
  LPSTR bubble_text_;
  uint32_t color_;
  uint32_t bevel_upper_;
  uint32_t bevel_lower_;
  uint32_t bevel_thickness_;
  uint32_t field_A94_;
  uint32_t field_A98_;
  uint32_t field_A9C_;
  uint32_t field_AA0_;
  uint32_t field_AA4_;
  ButtonGroup *group_;
  uint32_t field_AAC_;
  uint32_t field_AB0_;
  uint32_t field_AB4_;
};

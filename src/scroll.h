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
#include "flatbutton.h"
#include "graphicwin.h"
#include "sprite.h"

 /*
  * Scroll class
  */
class DLLEXPORT Scroll : GraphicWin {
 public:
  Scroll() { ; }
  ~Scroll() { ; }

 private:
  uint32_t field_A14_;
  uint32_t field_A18_;
  uint32_t border_color_;
  uint32_t field_A20_;
  uint32_t field_A24_;
  uint32_t field_A28_;
  int position_;
  uint32_t field_A30_;
  uint32_t field_A34_;
  uint32_t field_A38_;
  uint32_t field_A3C_;
  uint32_t paging_;
  uint32_t field_A44_;
  uint32_t field_A48_;
  uint32_t field_A4C_;
  uint32_t field_A50_;
  uint32_t bar_thickness1_;
  uint32_t bar_thickness2_;
  uint32_t color_;
  uint32_t bar_thickness3_;
  Buffer *buffer_;
  uint32_t bevel_thickness_;
  uint32_t bevel_upper_;
  uint32_t bevel_lower_;
  uint32_t field_A74_;
  Sprite *field_A78_;
  Sprite *sprite_left1_;
  Sprite *sprite_left2_;
  Sprite *sprite_left3_;
  uint32_t field_A88_;
  uint32_t field_A8C_;
  uint32_t field_A90_;
  Sprite *sprite_right1_;
  Sprite *sprite_right2_;
  Sprite *sprite_right3_;
  uint32_t field_AA0_;
  uint32_t field_AA4_;
  uint32_t field_AA8_;
  FlatButton flat_button_left_;
  FlatButton flat_button_right_;
  uint32_t field_2144_;
  uint32_t field_2148_;
};

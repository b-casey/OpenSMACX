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
    Buffer buffer;
    uint32_t field_9CC;
    uint32_t field_9D0;
    uint32_t field_9D4;
    uint32_t field_9D8;
    uint32_t field_9DC;
    uint32_t field_9E0;
    uint32_t field_9E4;
    uint32_t field_9E8;
    uint32_t field_9EC;
    uint32_t field_9F0;
    uint32_t field_9F4;
    uint32_t field_9F8;
    uint32_t field_9FC;
    uint32_t field_A00;
    uint32_t field_A04;
    uint32_t field_A08;
    uint32_t field_A0C;
    uint32_t field_A10;

public:
    GraphicWin() { ; }
    ~GraphicWin() { ; }
};

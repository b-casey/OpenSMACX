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
class OPENSMACX_API Dialog {
    LPVOID vtable;
    Heap heap;
    Heap *pHeap;
    uint32_t field_1C;
    uint32_t field_20;
    uint32_t field_24;
    uint32_t field_28;
    uint32_t field_2C;
    uint32_t field_30;
    uint32_t field_34;
    uint32_t field_38;
    uint32_t field_3C;
    uint32_t field_40;
    uint32_t field_44;
    uint32_t field_48;
    uint32_t field_4C;
    uint32_t field_50;
    uint32_t field_54;
    uint32_t field_58;
    uint32_t field_5C;
    uint32_t field_60;
    uint32_t field_64;
    uint32_t field_68;
    uint32_t field_6C;
    Font *font1;
    Font *font2;
    Font *font3;
    uint32_t textColorA;
    uint32_t textColor2A;
    uint32_t textColor3A;
    uint32_t textColorB;
    uint32_t textColor2B;
    uint32_t textColor3B;
    uint32_t textColorC;
    uint32_t textColor2C;
    uint32_t textColor3C;
    uint32_t textColorD;
    uint32_t textColor2D;
    uint32_t textColor3D;
    uint32_t field_AC;
    uint32_t field_B0;
    uint32_t field_B4;
    uint32_t field_B8;
    uint32_t field_BC;
    uint32_t field_C0;
    uint32_t field_C4;
    uint32_t field_C8;
    uint32_t field_CC;
    uint32_t field_D0;
    uint32_t field_D4;
    uint32_t field_D8;
    uint32_t field_DC;
    uint32_t field_E0;
    uint32_t field_E4;
    uint32_t field_E8;
    uint32_t stateFlag;
    uint32_t field_F0;

public:
    Dialog() { ; }
    ~Dialog() { ; }
};

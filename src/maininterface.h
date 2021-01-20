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
#include "buttongroup.h"
#include "flatbutton.h"
#include "flic.h"
#include "graphicwin.h"
#include "stringbox.h"
#include "time.h"

 /*
  * MainInterface class
  */
class DLLEXPORT MainInterface : GraphicWin {
 public:
  MainInterface() { ; }
  ~MainInterface() { ; }

  uint32_t set_bubble_text(uint32_t button_id, LPCSTR text);

 private:
  uint32_t field_A14_;
  uint32_t field_A18_;
  uint32_t field_A1C_;
  uint32_t field_A20_;
  uint32_t field_A24_;
  uint32_t field_A28_;
  uint32_t field_A2C_;
  uint32_t field_A30_;
  uint32_t field_A34_;
  uint32_t field_A38_;
  uint32_t field_A3C_;
  uint32_t field_A40_;
  uint32_t field_A44_;
  uint32_t field_A48_;
  uint32_t field_A4C_;
  uint32_t field_A50_;
  uint32_t field_A54_;
  uint32_t field_A58_;
  uint32_t field_A5C_;
  uint32_t field_A60_;
  uint32_t field_A64_;
  uint32_t field_A68_;
  uint32_t field_A6C_;
  uint32_t field_A70_;
  uint32_t field_A74_;
  uint32_t field_A78_;
  uint32_t field_A7C_;
  uint32_t field_A80_;
  uint32_t field_A84_;
  uint32_t field_A88_;
  RECT rect2_;
  RECT rect3_;
  uint32_t field_AAC_;
  uint32_t field_AB0_;
  uint32_t field_AB4_;
  uint32_t field_AB8_;
  uint32_t field_ABC_;
  uint32_t field_AC0_;
  uint32_t field_AC4_;
  uint32_t field_AC8_;
  uint32_t field_ACC_;
  uint32_t field_AD0_;
  uint32_t field_AD4_;
  uint32_t field_AD8_;
  uint32_t field_ADC_;
  uint32_t field_AE0_;
  uint32_t field_AE4_;
  uint32_t field_AE8_;
  uint32_t field_AEC_;
  uint32_t field_AF0_;
  uint32_t field_AF4_;
  uint32_t field_AF8_;
  uint32_t field_AFC_;
  uint32_t field_B00_;
  uint32_t field_B04_;
  uint32_t field_B08_;
  uint32_t field_B0C_;
  uint32_t field_B10_;
  uint32_t field_B14_;
  uint32_t field_B18_;
  uint32_t field_B1C_;
  uint32_t field_B20_;
  uint32_t field_B24_;
  uint32_t field_B28_;
  uint32_t field_B2C_;
  uint32_t field_B30_;
  uint32_t field_B34_;
  uint32_t field_B38_;
  uint32_t field_B3C_;
  uint32_t field_B40_;
  uint32_t field_B44_;
  uint32_t field_B48_;
  uint32_t field_B4C_;
  uint32_t field_B50_;
  uint32_t field_B54_;
  uint32_t field_B58_;
  uint32_t field_B5C_;
  uint32_t field_B60_;
  uint32_t field_B64_;
  uint32_t field_B68_;
  uint32_t field_B6C_;
  uint32_t field_B70_;
  uint32_t field_B74_;
  uint32_t field_B78_;
  uint32_t field_B7C_;
  uint32_t field_B80_;
  uint32_t field_B84_;
  uint32_t field_B88_;
  uint32_t field_B8C_;
  uint32_t field_B90_;
  uint32_t field_B94_;
  uint32_t field_B98_;
  uint32_t field_B9C_;
  uint32_t field_BA0_;
  uint32_t field_BA4_;
  uint32_t field_BA8_;
  uint32_t field_BAC_;
  uint32_t field_BB0_;
  uint32_t field_BB4_;
  uint32_t field_BB8_;
  uint32_t field_BBC_;
  uint32_t field_BC0_;
  uint32_t field_BC4_;
  uint32_t field_BC8_;
  uint32_t field_BCC_;
  uint32_t field_BD0_;
  uint32_t field_BD4_;
  uint32_t field_BD8_;
  uint32_t field_BDC_;
  uint32_t field_BE0_;
  uint32_t field_BE4_;
  uint32_t field_BE8_;
  uint32_t field_BEC_;
  uint32_t field_BF0_;
  uint32_t field_BF4_;
  uint32_t field_BF8_;
  uint32_t field_BFC_;
  uint32_t field_C00_;
  uint32_t field_C04_;
  uint32_t field_C08_;
  uint32_t field_C0C_;
  uint32_t field_C10_;
  uint32_t field_C14_;
  uint32_t field_C18_;
  uint32_t field_C1C_;
  uint32_t field_C20_;
  uint32_t field_C24_;
  uint32_t field_C28_;
  uint32_t field_C2C_;
  uint32_t field_C30_;
  uint32_t field_C34_;
  uint32_t field_C38_;
  uint32_t field_C3C_;
  uint32_t field_C40_;
  uint32_t field_C44_;
  uint32_t field_C48_;
  uint32_t field_C4C_;
  uint32_t field_C50_;
  uint32_t field_C54_;
  uint32_t field_C58_;
  uint32_t field_C5C_;
  uint32_t field_C60_;
  uint32_t field_C64_;
  uint32_t field_C68_;
  uint32_t field_C6C_;
  uint32_t field_C70_;
  uint32_t field_C74_;
  uint32_t field_C78_;
  uint32_t field_C7C_;
  uint32_t field_C80_;
  uint32_t field_C84_;
  uint32_t field_C88_;
  uint32_t field_C8C_;
  uint32_t field_C90_;
  uint32_t field_C94_;
  uint32_t field_C98_;
  uint32_t field_C9C_;
  uint32_t field_CA0_;
  uint32_t field_CA4_;
  uint32_t field_CA8_;
  uint32_t field_CAC_;
  uint32_t field_CB0_;
  uint32_t field_CB4_;
  uint32_t field_CB8_;
  uint32_t field_CBC_;
  uint32_t field_CC0_;
  uint32_t field_CC4_;
  uint32_t field_CC8_;
  uint32_t field_CCC_;
  uint32_t field_CD0_;
  uint32_t field_CD4_;
  uint32_t field_CD8_;
  uint32_t field_CDC_;
  uint32_t field_CE0_;
  uint32_t field_CE4_;
  uint32_t field_CE8_;
  uint32_t field_CEC_;
  uint32_t field_CF0_;
  uint32_t field_CF4_;
  uint32_t field_CF8_;
  uint32_t field_CFC_;
  uint32_t field_D00_;
  uint32_t field_D04_;
  uint32_t field_D08_;
  uint32_t field_D0C_;
  uint32_t field_D10_;
  uint32_t field_D14_;
  uint32_t field_D18_;
  uint32_t field_D1C_;
  uint32_t field_D20_;
  uint32_t field_D24_;
  uint32_t field_D28_;
  uint32_t field_D2C_;
  uint32_t field_D30_;
  uint32_t field_D34_;
  uint32_t field_D38_;
  uint32_t field_D3C_;
  uint32_t field_D40_;
  uint32_t field_D44_;
  uint32_t field_D48_;
  uint32_t field_D4C_;
  uint32_t field_D50_;
  uint32_t field_D54_;
  uint32_t field_D58_;
  uint32_t field_D5C_;
  uint32_t field_D60_;
  uint32_t field_D64_;
  uint32_t field_D68_;
  uint32_t field_D6C_;
  uint32_t field_D70_;
  uint32_t field_D74_;
  uint32_t field_D78_;
  uint32_t field_D7C_;
  uint32_t field_D80_;
  uint32_t field_D84_;
  uint32_t field_D88_;
  uint32_t field_D8C_;
  uint32_t field_D90_;
  uint32_t field_D94_;
  uint32_t field_D98_;
  uint32_t field_D9C_;
  uint32_t field_DA0_;
  uint32_t field_DA4_;
  uint32_t field_DA8_;
  uint32_t field_DAC_;
  uint32_t field_DB0_;
  uint32_t field_DB4_;
  uint32_t field_DB8_;
  uint32_t field_DBC_;
  RECT rect1_;
  uint32_t field_DD0_;
  uint32_t field_DD4_;
  uint32_t field_DD8_;
  uint32_t field_DDC_;
  uint32_t field_DE0_;
  uint32_t field_DE4_;
  uint32_t field_DE8_;
  uint32_t field_DEC_;
  uint32_t field_DF0_;
  uint32_t field_DF4_;
  uint32_t field_DF8_;
  uint32_t field_DFC_;
  FlatButton flat_button_[42];
  ButtonGroup button_group_[4];
  uint32_t field_1EAC8_;
  StringBox string_box_;
  Font font1_;
  Font font2_;
  Font font3_;
  uint32_t field_216E4_;
  uint32_t field_216E8_;
  uint32_t field_216EC_;
  uint32_t field_216F0_;
  Sprite sprites1_[4];
  Sprite sprites2_[4];
  Sprite sprites3_[2];
  Sprite sprites4_[2];
  Sprite sprites5_[10];
  Sprite sprites6_[10];
  Sprite sprites7_[6];
  Sprite sprites8_[3];
  Sprite sprites9_[75];
  Sprite sprites10_[18];
  Sprite sprite11_;
  Sprite sprites12_[6];
  Sprite sprites13_[2];
  Flic flics1_[2];
  Flic flic2_;
  Spot spot_;
  Time time1_;
  Time time2_;
};

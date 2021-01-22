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

 /*
  * Font class
  */
class DLLEXPORT Font {
 public:
  Font()
      : unk_1_(-1), 
        is_fot_set_(0), 
        font_obj_(0), 
        line_height_(0), 
        height_(0), 
        ascent_(0), 
        descent_(0),
        fot_file_name_(0) { } // 00618EA0
  Font(LPSTR font_name, int height, int style) { init(font_name, height, style); } // 00618EC0
  ~Font() { close(); } // 00618EE0

  // int UNK1(int, int, int, int) { return 1; } // no direct references
  int init(LPCSTR font_name, int height, uint32_t style);
  int init(LPCSTR file, LPCSTR font_name, int height, uint32_t style);
  void close();
  int width(LPSTR input);
  int width(LPSTR input, size_t max_len);
  LPSTR find_line_break_l(LPSTR input, int *break_len, size_t len);

  // eventually make atomic for thread safety
  static HDC FontHDC;
  static int FontInitCount;
  static int __cdecl init_font_class(Font *font);
  static void __cdecl close_font_class();

 private:
  int unk_1_; // height offset? set outside of class functions
  BOOL is_fot_set_; // used only by both init() functions
  HFONT font_obj_;
  int line_height_;
  int height_;
  int internal_leading_;
  int ascent_;
  int descent_;
  int pad_; // padding? no references
  LPSTR fot_file_name_;
};

// global
extern Font **FontDefaultPtr;

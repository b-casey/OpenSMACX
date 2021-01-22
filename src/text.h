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
#include "general.h"

 /*
  * Text class: Handles basic text operations.
  */
class DLLEXPORT Text {
 public:
  Text() : current_pos_(0), text_file_(0), buffer_get_(0), buffer_item_(0) { // 005FD860
      file_name_[0] = 0;
      file_path_[0] = 0;
  }
  Text(size_t size) : current_pos_(0), text_file_(0), buffer_get_(0), buffer_item_(0) { // 005FD880
      file_name_[0] = 0; 
      file_path_[0] = 0;
      buffer_get_ = (LPSTR)mem_get(size);
      if (buffer_get_) {
          buffer_item_ = (LPSTR)mem_get(size);
      }
  }
  ~Text() { shutdown(); } // 00608C00

  int init(size_t size);
  void shutdown();
  void close();
  BOOL open(LPCSTR src_id, LPCSTR section_id);
  LPSTR get();
  LPSTR string();
  LPSTR item();
  LPSTR item_string();
  int item_number();
  int item_binary();
  int item_hex();
  // additional functions to assist with encapsulation
  LPSTR update() { current_pos_ = buffer_get_; return buffer_get_; }
  LPSTR get_file_path() { return file_path_; }
  LPSTR get_buffer_item() { return buffer_item_; }
  LPSTR get_buffer_get() { return buffer_get_; }

 private:
  char file_name_[80];  // (+0)    : stores text filename string
  char file_path_[256]; // (+0x50) : stores path of last opened file
  LPSTR current_pos_;   // (+0x150)
  FILE *text_file_;     // (+0x154)
  LPSTR buffer_get_;    // (+0x158)
  LPSTR buffer_item_;   // (+0x15C)
};

// global
extern Text *Txt;
extern LPSTR *TextBufferGetPtr;
extern LPSTR *TextBufferItemPtr;
DLLEXPORT void __cdecl text_txt();
DLLEXPORT void __cdecl text_txt_exit();
DLLEXPORT void __cdecl text_set_get_ptr();
DLLEXPORT void __cdecl text_set_item_ptr();
DLLEXPORT void __cdecl text_close();
DLLEXPORT BOOL __cdecl text_open(LPCSTR src_id, LPCSTR section_id);
DLLEXPORT LPSTR __cdecl text_get();
DLLEXPORT LPSTR __cdecl text_string();
DLLEXPORT LPSTR __cdecl text_item();
DLLEXPORT LPSTR __cdecl text_item_string();
DLLEXPORT int __cdecl text_item_number();
DLLEXPORT int __cdecl text_item_binary();
DLLEXPORT int __cdecl text_item_hex();
DLLEXPORT int __cdecl text_get_number(int min, int max);

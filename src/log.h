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
  * Log class: Handles debug logging.
  */
class DLLEXPORT Log {
 public:
  Log() : log_file_(0), is_disabled_(false) { }           // 00625FB0
  Log(LPCSTR input) : log_file_(0), is_disabled_(false) { // 00625FC0
      if (input) {
          size_t len = strlen(input) + 1;
          log_file_ = (LPSTR)mem_get(len);
          if (log_file_) {
              strcpy_s(log_file_, len, input);
              reset();
          }
      }
  }
  ~Log() { if (log_file_) { free(log_file_); log_file_ = 0; } } // 00626020

  int init(LPCSTR input);
  void reset();
  void say(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3);
  void say_hex(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3);
  // additional functions to assist with encapsulation
  void set_state(BOOL state) { is_disabled_ = state ? false : true; }

 private:
  LPSTR log_file_;
  BOOL is_disabled_;
};

// global
extern Log *Logging;
extern BOOL *IsLoggingDisabled;

DLLEXPORT void __cdecl log_logging();
DLLEXPORT void __cdecl log_logging_exit();
DLLEXPORT void __cdecl log_reset();
DLLEXPORT void __cdecl log_say(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3);
DLLEXPORT void __cdecl log_say(LPCSTR str1, int num1, int num2, int num3);
DLLEXPORT void __cdecl log_say_hex(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3);
DLLEXPORT void __cdecl log_say_hex(LPCSTR str1, int num1, int num2, int num3);
DLLEXPORT void __cdecl log_set_state(BOOL state);

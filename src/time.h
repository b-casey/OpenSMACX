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
#pragma comment(lib, "Winmm.lib")
#include <mmsystem.h>

 /*
  * Time class
  */
class DLLEXPORT Time {
 public:
  Time() // 006161D0
      : unk_tgl_(0),
        id_event_(0), 
        callback1_(0), 
        callback2_(0), 
        cb_param2_(0), 
        cb_param1_(0),
        count_(0), 
        unk_1_(0), 
        resolution_(5), 
        unk_2_(0) { }
  ~Time() { close(); } // 00616200
  
  void init(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res);
  void init(void(__cdecl *callback)(int, int), int param, int param2, uint32_t cnt, uint32_t res);
  uint32_t start(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res);
  uint32_t start(void(__cdecl *callback)(int, int), int param, int param2, uint32_t cnt, 
                 uint32_t res);
  uint32_t pulse(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res);
  uint32_t pulse(void(__cdecl *callback)(int, int), int param, int param2, uint32_t cnt, 
                 uint32_t res);
  uint32_t start();
  uint32_t pulse();
  void stop();
  void close();
  void set_modal() { TimeModal = this; }  // 00616860
  void release_modal() { TimeModal = 0; } // 00616870

  // eventually make atomic for thread safety
  static Time *TimeModal;
  static int TimeInitCount;
  static void TimerProc(HWND hwnd, uint32_t msg, UINT_PTR id_timer, DWORD elapsed);
  static void MultimediaProc(uint32_t timer_id, uint32_t msg, DWORD_PTR dw_user, DWORD_PTR dw1,
                             DWORD_PTR dw2);
  static int __cdecl init_class() { ++TimeInitCount; return 0; } // 00616880
  static void __cdecl close_class() { --TimeInitCount; }         // 00616890

 private:
  int unk_tgl_; // some kind of toggle or type
  UINT_PTR id_event_;
  void(__cdecl *callback1_)(int);
  void(__cdecl *callback2_)(int, int);
  int cb_param2_; // callback 2nd parameter
  int cb_param1_; // callback 1st parameter
  uint32_t count_; // either delay (timeSetEvent) or elapsed (SetTimer) value
  int unk_1_; // BOOL? related to Timer/Multimedia Proc? one time execution?
  uint32_t resolution_;
  int unk_2_;
};

// global
extern Time *TurnTimer;
extern Time *LineTimer;
extern Time *BlinkTimer;
extern Time *Blink2Timer;
extern Time *GoTimer;
extern Time *ConsoleTimer;

DLLEXPORT void __cdecl start_timers();
DLLEXPORT void __cdecl stop_timers();
DLLEXPORT void __cdecl flush_timer();

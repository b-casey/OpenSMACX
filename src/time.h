/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2019 Brendan Casey
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
class OPENSMACX_API Time {

private:
	int unkToggle; // some kind of toggle or type
	UINT_PTR idEvent;
	void (__cdecl *callback1)(int);
	void (__cdecl *callback2)(int, int);
	int cbParam2; // callback 2nd parameter
	int cbParam1; // callback 1st parameter
	uint32_t count; // either delay (timeSetEvent) or elapsed (SetTimer) value
	int unk1;
	uint32_t resolution;
	int unk2;

public:
	Time(): unkToggle(0), idEvent(0), callback1(0), callback2(0), cbParam2(0), cbParam1(0), 
		count(0), unk1(0), resolution(5), unk2(0) { } // 006161D0
	~Time() { close(); } // 00616200

	void init(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res);
	void init(void(__cdecl *callback)(int, int), int param, int param2, uint32_t cnt, uint32_t res);
	int start(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res);
	int start(void(__cdecl *callback)(int, int), int param, int param2, uint32_t cnt, uint32_t res);
	int pulse(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res);
	int pulse(void(__cdecl *callback)(int, int), int param, int param2, uint32_t cnt, uint32_t res);
	int start();
	int pulse();
	void stop();
	void close();
	void set_modal() { TimeModal = this; } // 00616860
	void release_modal() { TimeModal = 0; } // 00616870

	// eventually make atomic for thread safety
	static Time *TimeModal;
	static int TimeInitCount;
	static void TimerProc(HWND a2, uint32_t a3, UINT_PTR wParam, uint32_t a5);
	static void MultimediaProc(uint32_t timerID, uint32_t msg, uint32_t user, 
		uint32_t dw1, uint32_t);
	static int __cdecl init_class() { ++TimeInitCount; return 0; } // 00616880
	static void __cdecl close_class() { --TimeInitCount; } // 00616890
};

// global
OPENSMACX_API void __cdecl flush_timer();
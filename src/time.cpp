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
#include "stdafx.h"
#include "time.h"
#include "temp.h" // MsgStatus, HandleMain

Time *Time::TimeModal;
int Time::TimeInitCount;

/*
Purpose: Initialize instance of Time with single parameter callback.
Original Offset: 00616260
Return Value: n/a
Status: Complete ; test pending
*/
void Time::init(void (__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res) {
	stop();
	unkToggle = 0;
	callback1 = callback;
	callback2 = 0;
	cbParam2 = 0;
	cbParam1 = param;
	count = cnt;
	unk1 = 0;
	resolution = res;
	unk2 = 0;
}

/*
Purpose: Initialize instance of Time with two parameter callback.
Original Offset: 006162D0
Return Value: n/a
Status: Complete ; test pending
*/
void Time::init(void (__cdecl *callback)(int, int), int param, int param2, 
	uint32_t cnt, uint32_t res) {
	stop();
	unkToggle = 0;
	callback1 = 0;
	callback2 = callback;
	cbParam2 = param2;
	cbParam1 = param;
	count = cnt;
	unk1 = 0;
	resolution = res;
	unk2 = 0;
}

/*
Purpose: Start instance of Time with single parameter callback.
Original Offset: 00616350
Return Value: Zero success, non-zero error
Status: Complete ; test pending
*/
int Time::start(void(__cdecl * callback)(int), int param, uint32_t cnt, uint32_t res) {
	init(callback, param, cnt, res);
	if (!callback) {
		return 7;
	}
	unkToggle = 0;
	if (count >= 50) {
		idEvent = SetTimer(*HandleMain, idEvent, count, (TIMERPROC)TimerProc);
	}
	else {
		idEvent = timeSetEvent(count, resolution, 
			(LPTIMECALLBACK)MultimediaProc, idEvent, TIME_PERIODIC);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Start instance of Time with two parameter callback.
Original Offset: 00616410
Return Value: Zero success, non-zero error
Status: Complete ; test pending
*/
int Time::start(void(__cdecl * callback)(int, int), int param, int param2,
	uint32_t cnt, uint32_t res) {
	init(callback, param, param2, cnt, res);
	if (!callback) {
		return 7;
	}
	unkToggle = 0;
	if (count >= 50) {
		idEvent = SetTimer(*HandleMain, idEvent, count, (TIMERPROC)TimerProc);
	}
	else {
		idEvent = timeSetEvent(count, resolution, 
			(LPTIMECALLBACK)MultimediaProc, idEvent, TIME_PERIODIC);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Start instance of Time with single parameter callback.
Original Offset: 006164D0
Return Value: Zero success, non-zero error
Status: Complete ; test pending
*/
int Time::pulse(void(__cdecl * callback)(int), int param, uint32_t cnt, uint32_t res) {
	init(callback, param, cnt, res);
	if (!callback) {
		return 7;
	}
	unkToggle = 0;
	if (count >= 50) {
		idEvent = SetTimer(*HandleMain, idEvent, count, (TIMERPROC)TimerProc);
	}
	else {
		// Bug fix: Original code had fuEvent param set to TIME_PERIODIC, the same as start().
		// Based on the rest of code of how pulse() should work, this should be TIME_ONESHOT.
		// This code branch is never used in original game.
		idEvent = timeSetEvent(count, resolution, 
			(LPTIMECALLBACK)MultimediaProc, idEvent, TIME_ONESHOT);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Start instance of Time with two parameter callback.
Original Offset: 00616590
Return Value: Zero success, non-zero error
Status: Complete ; test pending
*/
int Time::pulse(void(__cdecl * callback)(int, int), int param, int param2,
	uint32_t cnt, uint32_t res) {
	init(callback, param, param2, cnt, res);
	if (!callback) {
		return 7;
	}
	if (count >= 50) {
		idEvent = SetTimer(*HandleMain, idEvent, count, (TIMERPROC)TimerProc);
	}
	else {
		idEvent = timeSetEvent(count, resolution, 
			(LPTIMECALLBACK)MultimediaProc, idEvent, TIME_ONESHOT);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Start instance of Time.
Original Offset: 00616650
Return Value: Zero success, non-zero error
Status: Complete ; test pending
*/
int Time::start() {
	if (!callback1 && !callback2) {
		return 7;
	}
	unkToggle &= 0xFFFFFFFE;
	if (count >= 50) {
		idEvent = SetTimer(*HandleMain, idEvent, count, (TIMERPROC)TimerProc);
	}
	else {
		idEvent = timeSetEvent(count, resolution,
			(LPTIMECALLBACK)MultimediaProc, idEvent, TIME_PERIODIC);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Start instance of Time.
Original Offset: 006166C0
Return Value: Zero success, non-zero error
Status: Complete ; test pending
*/
int Time::pulse() {
	if (!callback1 && !callback2) {
		return 7;
	}
	if (count >= 50) {
		unkToggle = (unkToggle & 0xFFFFFFFD) | 1;
		idEvent = SetTimer(*HandleMain, idEvent, count, (TIMERPROC)TimerProc);
	}
	else {
		idEvent = timeSetEvent(count, resolution,
			(LPTIMECALLBACK)MultimediaProc, idEvent, TIME_ONESHOT);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Stop timer.
Original Offset: 00616730
Return Value: n/a
Status: Complete ; test pending
*/
void Time::stop() {
	if (idEvent) {
		if (count >= 50) {
			KillTimer(*HandleMain, idEvent);
		}
		else {
			timeKillEvent(idEvent);
		}
		idEvent = 0;
	}
	if (~unkToggle & 1) {
		flush_timer();
	}
}

/*
Purpose: Close/shutdown instance of Time.
Original Offset: 00616780
Return Value: n/a
Status: Complete ; test pending
*/
void Time::close() {
	stop();
	unkToggle = 0;
	callback1 = 0;
	callback2 = 0;
	cbParam2 = 0;
	cbParam1 = 0;
	count = 0;
	unk1 = 0;
	resolution = 5;
	unk2 = 0;
}

// 006167E0
void Time::TimerProc(HWND a2, uint32_t a3, UINT_PTR wParam, uint32_t a5) {
	/*
	if (wParam && (!*TimeModal || (Time *)wParam == *TimeModal) && (Time *)wParam->unk1) {
		PostMessageA(*HandleMain, 0x401, wParam, NULL);
		(Time)wParam->unk1 = 1;
	}
	*/
}

// 00616820
void Time::MultimediaProc(uint32_t timerID, uint32_t msg, uint32_t user, uint32_t dw1, uint32_t) {
	//
}

// global
/*
Purpose: Clear timer message queue?
Original Offset: 005FD370
Return Value: n/a
Status: Complete ; test pending
*/
void __cdecl flush_timer() {
	*MsgStatus |= 0x3F;
	MSG uMsg;
	if (PeekMessage(&uMsg, NULL, WM_USER|WM_CREATE, WM_USER|WM_CREATE, PM_REMOVE)) {
		do {
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		} while (PeekMessage(&uMsg, NULL, WM_USER | WM_CREATE, WM_USER | WM_CREATE, PM_REMOVE));
	}
	*MsgStatus = 0;
}
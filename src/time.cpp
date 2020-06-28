/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2020 Brendan Casey
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
#include "temp.h" // MsgStatus, HandleMain
#include "time.h"

Time *Time::TimeModal;
int Time::TimeInitCount;

/*
Purpose: Initialize instance of Time with single parameter callback.
Original Offset: 00616260
Return Value: n/a
Status: Complete
*/
void Time::init(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res) {
	stop();
	unkToggle = 0;
	callback1 = callback;
	callback2 = 0;
	cbParam2 = 0;
	cbParam1 = param;
	count = cnt;
	unk_1 = 0;
	resolution = res;
	unk_2 = 0;
}

/*
Purpose: Initialize instance of Time with two parameter callback.
Original Offset: 006162D0
Return Value: n/a
Status: Complete
*/
void Time::init(void(__cdecl *callback)(int, int), int param, int param2,
	uint32_t cnt, uint32_t res) {
	stop();
	unkToggle = 0;
	callback1 = 0;
	callback2 = callback;
	cbParam2 = param2;
	cbParam1 = param;
	count = cnt;
	unk_1 = 0;
	resolution = res;
	unk_2 = 0;
}

/*
Purpose: Start instance of Time with single parameter callback.
Original Offset: 00616350
Return Value: Zero success, non-zero error
Status: Complete
*/
uint32_t Time::start(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res) {
	init(callback, param, cnt, res);
	if (!callback) {
		return 7;
	}
	unkToggle = 0;
	if (count < 50) {
		idEvent = timeSetEvent(count, resolution,
			(LPTIMECALLBACK)MultimediaProc, (UINT_PTR)this, TIME_PERIODIC);
	}
	else {
		idEvent = SetTimer(*HandleMain, (UINT_PTR)this, count, (TIMERPROC)TimerProc);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Start instance of Time with two parameter callback.
Original Offset: 00616410
Return Value: Zero success, non-zero error
Status: Complete
*/
uint32_t Time::start(void(__cdecl *callback)(int, int), int param, int param2,
	uint32_t cnt, uint32_t res) {
	init(callback, param, param2, cnt, res);
	if (!callback) {
		return 7;
	}
	unkToggle = 0;
	if (count < 50) {
		idEvent = timeSetEvent(count, resolution,
			(LPTIMECALLBACK)MultimediaProc, (UINT_PTR)this, TIME_PERIODIC);
	}
	else {
		idEvent = SetTimer(*HandleMain, (UINT_PTR)this, count, (TIMERPROC)TimerProc);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Start instance of Time with single parameter callback.
Original Offset: 006164D0
Return Value: Zero success, non-zero error
Status: Complete
*/
uint32_t Time::pulse(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res) {
	init(callback, param, cnt, res);
	if (!callback) {
		return 7;
	}
	unkToggle = 0;
	if (count < 50) {
		// Bug fix: Original code had fuEvent param set to TIME_PERIODIC, the same as start().
		// Based on how the other pulse() functions work, this should be TIME_ONESHOT. It appears
		// this code branch is never used in the original game.
		idEvent = timeSetEvent(count, resolution,
			(LPTIMECALLBACK)MultimediaProc, (UINT_PTR)this, TIME_ONESHOT);
	}
	else {
		idEvent = SetTimer(*HandleMain, (UINT_PTR)this, count, (TIMERPROC)TimerProc);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Start instance of Time with two parameter callback.
Original Offset: 00616590
Return Value: Zero success, non-zero error
Status: Complete
*/
uint32_t Time::pulse(void(__cdecl *callback)(int, int), int param, int param2,
	uint32_t cnt, uint32_t res) {
	init(callback, param, param2, cnt, res);
	if (!callback) {
		return 7;
	}
	if (count < 50) {
		idEvent = timeSetEvent(count, resolution,
			(LPTIMECALLBACK)MultimediaProc, (UINT_PTR)this, TIME_ONESHOT);
	}
	else {
		unkToggle = 1;
		idEvent = SetTimer(*HandleMain, (UINT_PTR)this, count, (TIMERPROC)TimerProc);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Start instance of Time.
Original Offset: 00616650
Return Value: Zero success, non-zero error
Status: Complete
*/
uint32_t Time::start() {
	if (!callback1 && !callback2) {
		return 7;
	}
	unkToggle &= ~1;
	if (count < 50) {
		idEvent = timeSetEvent(count, resolution, (LPTIMECALLBACK)MultimediaProc,
			(UINT_PTR)this, TIME_PERIODIC);
	}
	else {
		idEvent = SetTimer(*HandleMain, (UINT_PTR)this, count, (TIMERPROC)TimerProc);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Start instance of pulse Time.
Original Offset: 006166C0
Return Value: Zero success, non-zero error
Status: Complete
*/
uint32_t Time::pulse() {
	if (!callback1 && !callback2) {
		return 7;
	}
	if (count < 50) {
		idEvent = timeSetEvent(count, resolution,
			(LPTIMECALLBACK)MultimediaProc, (UINT_PTR)this, TIME_ONESHOT);
	}
	else {
		unkToggle = (unkToggle & ~2) | 1;
		idEvent = SetTimer(*HandleMain, (UINT_PTR)this, count, (TIMERPROC)TimerProc);
	}
	return idEvent ? 0 : 2;
}

/*
Purpose: Stop timer.
Original Offset: 00616730
Return Value: n/a
Status: Complete
*/
void Time::stop() {
	if (idEvent) {
		if (count < 50) {
			timeKillEvent(idEvent);
		}
		else {
			KillTimer(*HandleMain, idEvent);
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
Status: Complete
*/
void Time::close() {
	stop();
	unkToggle = 0;
	callback1 = 0;
	callback2 = 0;
	cbParam2 = 0;
	cbParam1 = 0;
	count = 0;
	unk_1 = 0;
	resolution = 5;
	unk_2 = 0;
}

/*
Purpose: Callback function that processes WM_TIMER messages (SetTimer).
Original Offset: 006167E0
Return Value: n/a
Status: Complete
*/
void Time::TimerProc(HWND UNUSED(hwnd), uint32_t UNUSED(msg), UINT_PTR idTimer,
	DWORD UNUSED(elapsed)) {
	if (idTimer && (!TimeModal || idTimer == (UINT_PTR)TimeModal)
		&& !reinterpret_cast<Time *>(idTimer)->unk_1) {
		PostMessageA(*HandleMain, WM_USER | WM_CREATE, idTimer, 0);
		reinterpret_cast<Time *>(idTimer)->unk_1 = 1;
	}
}

/*
Purpose: Callback function for the timeSetEvent. Effectively the same as TimerProc.
Original Offset: 00616820
Return Value: n/a
Status: Complete
*/
void Time::MultimediaProc(uint32_t UNUSED(timerID), uint32_t UNUSED(msg), DWORD_PTR dwUser,
	DWORD_PTR UNUSED(dw1), DWORD_PTR UNUSED(dw2)) {
	if (dwUser && (!TimeModal || dwUser == (DWORD_PTR)TimeModal)
		&& !reinterpret_cast<Time *>(dwUser)->unk_1) {
		PostMessageA(*HandleMain, WM_USER | WM_CREATE, dwUser, 0);
		reinterpret_cast<Time *>(dwUser)->unk_1 = 1;
	}
}

// global
Time *TurnTimer = (Time *)0x00915628;
Time *LineTimer = (Time *)0x00915658;
Time *BlinkTimer = (Time *)0x00915688;
Time *Blink2Timer = (Time *)0x00939EB0;
Time *GoTimer = (Time *)0x00939E60;
Time *ConsoleTimer = (Time *)0x00939E88;

/*
Purpose: Start global timers.
Original Offset: 0050F3D0
Return Value: n/a
Status: Complete
*/
void __cdecl start_timers() {
	BlinkTimer->start(blink_timer, 1, 150, 150);
	Blink2Timer->start(blink2_timer, 2, 100, 100);
	LineTimer->start(line_timer, 3, 100, 100);
	TurnTimer->start(turn_timer, 4, 500, 500);
}

/*
Purpose: Stop global timers.
Original Offset: 0050F440
Return Value: n/a
Status: Complete
*/
void __cdecl stop_timers() {
	Blink2Timer->close();
	BlinkTimer->close();
	LineTimer->close();
	// missing MP TurnTimer. TODO: In future determine if timer should be closed on stop
}

/*
Purpose: Clear timer message queue.
Original Offset: 005FD370
Return Value: n/a
Status: Complete
*/
void __cdecl flush_timer() {
	*MsgStatus |= 0x3F;
	MSG uMsg;
	if (PeekMessage(&uMsg, NULL, WM_USER | WM_CREATE, WM_USER | WM_CREATE, PM_REMOVE)) {
		do {
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		} while (PeekMessage(&uMsg, NULL, WM_USER | WM_CREATE, WM_USER | WM_CREATE, PM_REMOVE));
	}
	*MsgStatus = 0;
}

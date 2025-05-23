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
#include "stdafx.h"
#include "temp.h" // MsgStatus, HandleMain
#include "time.h"

Time *Time::TimeModal;
int Time::TimeInitCount;

/*
Purpose: Initialize an instance of the class with a single parameter callback.
Original Offset: 00616260
Return Value: n/a
Status: Complete
*/
void Time::init(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res) {
    stop();
    unk_tgl_ = 0;
    callback1_ = callback;
    callback2_ = 0;
    cb_param2_ = 0;
    cb_param1_ = param;
    count_ = cnt;
    unk_1_ = 0;
    resolution_ = res;
    unk_2_ = 0;
}

/*
Purpose: Initialize an instance of the class with a two parameter callback.
Original Offset: 006162D0
Return Value: n/a
Status: Complete
*/
void Time::init(void(__cdecl *callback)(int, int), int param, int param2, uint32_t cnt, 
                uint32_t res) {
    stop();
    unk_tgl_ = 0;
    callback1_ = 0;
    callback2_ = callback;
    cb_param2_ = param2;
    cb_param1_ = param;
    count_ = cnt;
    unk_1_ = 0;
    resolution_ = res;
    unk_2_ = 0;
}

/*
Purpose: Start an instance of the class with a single parameter callback.
Original Offset: 00616350
Return Value: Zero on success, non-zero on error
Status: Complete
*/
uint32_t Time::start(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res) {
    init(callback, param, cnt, res);
    if (!callback) {
        return 7;
    }
    unk_tgl_ = 0;
    if (count_ < 50) {
        id_event_ = timeSetEvent(count_, resolution_, (LPTIMECALLBACK)MultimediaProc, 
            (UINT_PTR)this, TIME_PERIODIC);
    } else {
        id_event_ = SetTimer(*HandleMain, (UINT_PTR)this, count_, (TIMERPROC)TimerProc);
    }
    return id_event_ ? 0 : 2;
}

/*
Purpose: Start an instance of the class with a two parameter callback.
Original Offset: 00616410
Return Value: Zero on success, non-zero on error
Status: Complete
*/
uint32_t Time::start(void(__cdecl *callback)(int, int), int param, int param2, uint32_t cnt, 
                     uint32_t res) {
    init(callback, param, param2, cnt, res);
    if (!callback) {
        return 7;
    }
    unk_tgl_ = 0;
    if (count_ < 50) {
        id_event_ = timeSetEvent(count_, resolution_, (LPTIMECALLBACK)MultimediaProc, 
            (UINT_PTR)this, TIME_PERIODIC);
    } else {
        id_event_ = SetTimer(*HandleMain, (UINT_PTR)this, count_, (TIMERPROC)TimerProc);
    }
    return id_event_ ? 0 : 2;
}

/*
Purpose: Start a pulse instance of the class with a single parameter callback.
Original Offset: 006164D0
Return Value: Zero on success, non-zero on error
Status: Complete
*/
uint32_t Time::pulse(void(__cdecl *callback)(int), int param, uint32_t cnt, uint32_t res) {
    init(callback, param, cnt, res);
    if (!callback) {
        return 7;
    }
    unk_tgl_ = 0;
    if (count_ < 50) {
        // Bug fix: Original code had fuEvent param set to TIME_PERIODIC, the same as start().
        // Based on how the other pulse() functions work, this should be TIME_ONESHOT. It appears
        // this code branch is never used in the original game.
        id_event_ = timeSetEvent(count_, resolution_, (LPTIMECALLBACK)MultimediaProc, 
            (UINT_PTR)this, TIME_ONESHOT);
    } else {
        id_event_ = SetTimer(*HandleMain, (UINT_PTR)this, count_, (TIMERPROC)TimerProc);
    }
    return id_event_ ? 0 : 2;
}

/*
Purpose: Start a pulse instance of the class with a two parameter callback.
Original Offset: 00616590
Return Value: Zero on success, non-zero on error
Status: Complete
*/
uint32_t Time::pulse(void(__cdecl *callback)(int, int), int param, int param2, uint32_t cnt, 
                     uint32_t res) {
    init(callback, param, param2, cnt, res);
    if (!callback) {
        return 7;
    }
    if (count_ < 50) {
        id_event_ = timeSetEvent(count_, resolution_, (LPTIMECALLBACK)MultimediaProc, 
            (UINT_PTR)this, TIME_ONESHOT);
    } else {
        unk_tgl_ = 1;
        id_event_ = SetTimer(*HandleMain, (UINT_PTR)this, count_, (TIMERPROC)TimerProc);
    }
    return id_event_ ? 0 : 2;
}

/*
Purpose: Start an instance of the timer.
Original Offset: 00616650
Return Value: Zero on success, non-zero on error
Status: Complete
*/
uint32_t Time::start() {
    if (!callback1_ && !callback2_) {
        return 7;
    }
    unk_tgl_ &= ~1;
    if (count_ < 50) {
        id_event_ = timeSetEvent(count_, resolution_, (LPTIMECALLBACK)MultimediaProc,
            (UINT_PTR)this, TIME_PERIODIC);
    } else {
        id_event_ = SetTimer(*HandleMain, (UINT_PTR)this, count_, (TIMERPROC)TimerProc);
    }
    return id_event_ ? 0 : 2;
}

/*
Purpose: Start an instance of the pulse timer.
Original Offset: 006166C0
Return Value: Zero on success, non-zero on error
Status: Complete
*/
uint32_t Time::pulse() {
    if (!callback1_ && !callback2_) {
        return 7;
    }
    if (count_ < 50) {
        id_event_ = timeSetEvent(count_, resolution_, (LPTIMECALLBACK)MultimediaProc, 
            (UINT_PTR)this, TIME_ONESHOT);
    } else {
        unk_tgl_ = (unk_tgl_ & ~2) | 1;
        id_event_ = SetTimer(*HandleMain, (UINT_PTR)this, count_, (TIMERPROC)TimerProc);
    }
    return id_event_ ? 0 : 2;
}

/*
Purpose: Stop an instance of the class.
Original Offset: 00616730
Return Value: n/a
Status: Complete
*/
void Time::stop() {
    if (id_event_) {
        if (count_ < 50) {
            timeKillEvent(id_event_);
        } else {
            KillTimer(*HandleMain, id_event_);
        }
        id_event_ = 0;
    }
    if (~unk_tgl_ & 1) {
        flush_timer();
    }
}

/*
Purpose: Close out an instance of the class.
Original Offset: 00616780
Return Value: n/a
Status: Complete
*/
void Time::close() {
    stop();
    unk_tgl_ = 0;
    callback1_ = 0;
    callback2_ = 0;
    cb_param2_ = 0;
    cb_param1_ = 0;
    count_ = 0;
    unk_1_ = 0;
    resolution_ = 5;
    unk_2_ = 0;
}

/*
Purpose: Callback function that processes WM_TIMER messages (SetTimer).
Original Offset: 006167E0
Return Value: n/a
Status: Complete
*/
void Time::TimerProc(HWND UNUSED(hwnd), uint32_t UNUSED(msg), UINT_PTR id_timer, 
                     DWORD UNUSED(elapsed)) {
    if (id_timer && (!TimeModal || id_timer == (UINT_PTR)TimeModal)
        && !reinterpret_cast<Time *>(id_timer)->unk_1_) {
        PostMessageA(*HandleMain, WM_USER + 1, id_timer, 0);
        reinterpret_cast<Time *>(id_timer)->unk_1_ = 1;
    }
}

/*
Purpose: Callback function for the timeSetEvent. Effectively the same as TimerProc.
Original Offset: 00616820
Return Value: n/a
Status: Complete
*/
void Time::MultimediaProc(uint32_t UNUSED(timer_id), uint32_t UNUSED(msg), DWORD_PTR user,
                          DWORD_PTR UNUSED(dw1), DWORD_PTR UNUSED(dw2)) {
    if (user && (!TimeModal || user == (DWORD_PTR)TimeModal) 
        && !reinterpret_cast<Time *>(user)->unk_1_) {
        PostMessageA(*HandleMain, WM_USER + 1, user, 0);
        reinterpret_cast<Time *>(user)->unk_1_ = 1;
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
    MSG msg;
    while (PeekMessage(&msg, NULL, WM_USER + 1, WM_USER + 1, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    *MsgStatus = 0;
}

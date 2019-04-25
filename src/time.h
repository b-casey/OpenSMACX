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
#include <mmsystem.h>

 /*
  * Time class
  */
class OPENSMACX_API Time {

private:
	int field_0;
	UINT_PTR idEvent;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;

	void TimerFunc1(HWND, UINT, UINT_PTR, DWORD);
	TIMECALLBACK TimerFunc2;
	void JACKAL_MsgClearQueue();

public:
	Time(): field_0(0), idEvent(0), field_8(0), field_C(0), field_10(0), field_14(0), field_18(0), 
		field_1C(0), field_20(5), field_24(0) { }
	~Time() { close(); }
	/*
	int pulse(UINT_PTR nIDEvent, int unk1, int unk2, UINT uElapse, UINT uResolution);
	int pulse(UINT_PTR nIDEvent, int unk1, int unk2, int unk3, UINT uElapse, UINT uResolution);
	int pulse(UINT_PTR nIDEvent);
	int start(UINT_PTR nIDEvent, int unk1, int unk2, UINT uElapse, UINT uResolution);
	int start(UINT_PTR nIDEvent, int unk1, int unk2, int unk3, UINT uElapse, UINT uResolution);
	int start(UINT_PTR nIDEvent);
	void init(int x1, int x2, int x3, int x4);
	void init(int x1, int x2, int x3, int x4, int x5);
	void stop();
	*/
	void close();
};
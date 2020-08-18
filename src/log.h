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
#pragma once
#include "general.h" // mem_get()

 /*
  * Log class: Handles debug logging.
  */
class OPENSMACX_API Log {
	LPSTR logFile;
	BOOL isDisabled;

public:
	Log() : logFile(0), isDisabled(false) { } // 00625FB0
	Log(LPCSTR input) : logFile(0), isDisabled(false) { // 00625FC0
		if (input) {
			int len = strlen(input) + 1;
			logFile = (LPSTR)mem_get(len);
			if (logFile) {
				strcpy_s(logFile, len, input);
				reset();
			}
		}
	}
	~Log() { if (logFile) { free(logFile); logFile = 0; } } // 00626020

	int init(LPCSTR input);
	void reset();
	void say(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3);
	void say_hex(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3);

	// additional functions to assist with encapsulation
	void set_state(BOOL state) { isDisabled = state ? false : true; }
};

// global
extern Log *Logging;
extern BOOL *IsLoggingDisabled;

OPENSMACX_API void __cdecl log_logging();
OPENSMACX_API void __cdecl log_logging_exit();
OPENSMACX_API void __cdecl log_reset();
OPENSMACX_API void __cdecl log_say(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3);
OPENSMACX_API void __cdecl log_say(LPCSTR str1, int num1, int num2, int num3);
OPENSMACX_API void __cdecl log_say_hex(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3);
OPENSMACX_API void __cdecl log_say_hex(LPCSTR str1, int num1, int num2, int num3);
OPENSMACX_API void __cdecl log_set_state(BOOL state);

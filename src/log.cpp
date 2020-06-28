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
#include "temp.h"
#include "log.h"

 /*
 Purpose: Initialize log file
 Original Offset: 00626040
 Return Value: Non-zero error; Zero successful
 Status: Complete
 */
int Log::init(LPCSTR input) {
	if (!input) {
		return 16;
	}
	this->~Log();
	int len = strlen(input) + 1;
	logFile = (LPSTR)mem_get(len);
	if (!logFile) {
		return 4;
	}
	strcpy_s(logFile, len, input);
	FILE *file = env_open(input, "wt");
	if (!file) {
		return 6;
	}
	fclose(file);
	return 0;
}

/*
Purpose: Reset log by opening the file for writing with truncation then closing it.
Original Offset: 006260D0
Return Value: n/a
Status: Complete
*/
void Log::reset() {
	FILE *file = env_open(logFile, "wt");
	if (file) {
		fclose(file);
	}
}

/*
Purpose: Write parameters to log file, numbers are outputted in base 10.
Original Offset: 006260F0
Return Value: n/a
Status: Complete
*/
void Log::say(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3) {
	if (!logFile || isDisabled || *IsLoggingDisabled) {
		return;
	}
	FILE *file = env_open(logFile, "at");
	if (file) {
		str2 ? fprintf_s(file, "%s %s %d %d %d\n", str1, str2, num1, num2, num3) :
			fprintf_s(file, "%s %d %d %d\n", str1, num1, num2, num3);
		fclose(file);
	}
}

/*
Purpose: Write parameters to log file, numbers are outputted in base 16.
Original Offset: 00626190
Return Value: n/a
Status: Complete
*/
void Log::say_hex(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3) {
	if (!logFile || isDisabled || *IsLoggingDisabled) {
		return;
	}
	FILE *file = env_open(logFile, "at");
	if (file) {
		str2 ? fprintf_s(file, "%s %s %04x %04x %04x\n", str1, str2, num1, num2, num3) :
			fprintf_s(file, "%s %04x %04x %04x\n", str1, num1, num2, num3);
		fclose(file);
	}
}

// global
Log *Logging = (Log *)0x009BBFF8;
BOOL *IsLoggingDisabled = (BOOL *)0x009BC004;

void __cdecl log_logging() { // 00625F20
	*Logging = *(new Log("logfile.txt")); atexit(log_logging_exit);
}

void __cdecl log_logging_exit() { Logging->~Log(); } // 00625F90

void __cdecl log_reset() { Logging->reset(); } // 00626230

void __cdecl log_say(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3) { // 00626250
	Logging->say(str1, str2, num1, num2, num3);
}

void __cdecl log_say(LPCSTR str1, int num1, int num2, int num3) { // 006262F0
	Logging->say(str1, NULL, num1, num2, num3);
}

void __cdecl log_say_hex(LPCSTR str1, LPCSTR str2, int num1, int num2, int num3) { // 00626350
	Logging->say_hex(str1, str2, num1, num2, num3);
}

void __cdecl log_say_hex(LPCSTR str1, int num1, int num2, int num3) { // 006263F0
	Logging->say_hex(str1, NULL, num1, num2, num3);
}

void __cdecl log_set_state(BOOL state) { Logging->set_state(state); } // 00626450
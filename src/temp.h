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
#include "faction.h"
#include "general.h"
#include "maininterface.h"

 /*
  * Temp functions and offset pointers. This allows interfacing with parts of the exe not yet 
  * decompiled. Eventually, this will be removed.
  */

typedef struct char256 { char str[256]; } char256;
typedef struct char1032 { char str[1032]; } char1032;

// built-in functions
typedef void *func1(size_t);
extern func1 *_malloc;
typedef void *func2(void *);
extern func2 *_free;
typedef FILE *func3(LPCSTR, LPCSTR);
extern func3 *_fopen;
typedef void *func4(unsigned int);
extern func4 *_srand;
//typedef void *func12(void *, size_t);
//extern func12 *_realloc;
//typedef int *func18(FILE *);
//extern func18 *_fclose;
//typedef LPSTR *func19(LPSTR, int, FILE *);
//extern func19 *_fgets;

// other
typedef void *func5(int);
extern func5 *load_faction_art;
typedef void *func6(char const *, int(*)(void));
extern func6 *X_pop;

// testing
typedef int *func8(LPSTR, LPSTR);
extern func8 *parse_string_OG;

typedef BOOL func12(int, int, int, int);
extern func12 *facility_avail_OG;

typedef int func21(int, int);
extern func21 *fixed_div_OG;
typedef void func22(const void *, const void *, char);
extern func22 *memrchr_OG;

///
extern LPSTR *ParseTempPtr1_1;
extern LPSTR *ParseTempPtr2_1;
extern char1032 *stringTemp;
extern char256 *ParseStrBuffer;

extern int *BufferStrHeight;
extern int *ParseNumTable;
extern int *ParseStrPlurality;
extern int *ParseStrGender;
extern int *GenderDefault;
extern int *PluralityDefault;
extern int *Language;
extern BOOL *IsLoggingDisabled; // global toggle for logging on/off? debug vs release?
extern BOOL *MultiplayerToggle;
extern int *MsgStatus;
extern HWND *HandleMain;

extern uint32_t *UnkBitfield1;

extern Filefind *FilefindPath;
extern MainInterface *MainInterfaceVar;

OPENSMACX_API int __cdecl tester();
OPENSMACX_API void __cdecl testerMap();
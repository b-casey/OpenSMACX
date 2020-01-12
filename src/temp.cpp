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
#include "temp.h"
#include "alpha.h"
#include "faction.h"
#include "general.h"
#include "base.h"
#include "map.h"
#include "veh.h"
#include "maininterface.h"
#include "technology.h"
#include "terraforming.h"
#include "log.h"

// built-in functions > used to prevent crash from mixed alloc/free SDKs
typedef void *func1(size_t);
func1 *_malloc = (func1 *)0x006470A6;
typedef void *func2(void *);
func2 *_free = (func2 *)0x00644EF2;
typedef FILE *func3(LPCSTR, LPCSTR);
func3 *_fopen = (func3 *)0x00645646;
typedef void *func4(unsigned int);
func4 *_srand = (func4 *)0x00646010;
//typedef void *func12(void *, size_t);
//func12 *_realloc = (func12 *)0x00647132;
//typedef int *func18(FILE *);
//func18 *_fclose = (func18 *)0x00645598;
//typedef LPSTR *func19(LPSTR, int, FILE *);
//func19 *_fgets = (func19 *)0x0064726A;

// other
typedef void *func5(int);
func5 *load_faction_art = (func5 *)0x00453710;
// TODO: crash bug; X_pop > ... > BasePop::start > text_close > NULLs 009B7CF4 (FILE *Txt.textFile)
// Next call to text_get() tries to access 009B7CF4 and the game crashes.
typedef void *func6(char const *, int(*)(void));
func6 *X_pop = (func6 *)0x005BF310;

// testing
typedef int *func8(LPSTR, LPSTR);
func8 *parse_string_OG = (func8 *)0x00625880;

// Time
typedef void func30(int);
func30 *blink_timer = (func30 *)0x0050EA40;
func30 *blink2_timer = (func30 *)0x0050EE30;
func30 *line_timer = (func30 *)0x0050EE80;
func30 *turn_timer = (func30 *)0x0050EF10;

///
char1032 *stringTemp = (char1032 *)0x009B86A0;
char256 *ParseStrBuffer = (char256 *)0x009BB5E8;

//  ; int 
int *BufferStrHeight = (int *)0x009B3A9C;
int *ParseNumTable = (int *)0x009BB598;
int *ParseStrPlurality = (int *)0x009BB570;
int *ParseStrGender = (int *)0x009BB5C0;
int *GenderDefault = (int *)0x009BBFEC;
int *PluralityDefault = (int *)0x009BBFF0;
int *Language = (int *)0x009BC054;
BOOL *IsLoggingDisabled = (BOOL *)0x009BC004;
BOOL *MultiplayerToggle = (BOOL *)0x0093F660;
int *MsgStatus = (int *)0x009B7B9C;
HWND *HandleMain = (HWND *)0x009B7B28;

uint32_t *UnkBitfield1 = (uint32_t *)0x0090D91C;

Filefind *FilefindPath = (Filefind *)0x009B8198;
MainInterface *MainInterfaceVar = (MainInterface *)0x007AE820;

int __cdecl tester() {
	return 0;
}

void __cdecl testerMap() {
	/*
	for (int y = -10; y < (int)*MapVerticalBounds; y++) {
		for (int x = -1; x < (int)*MapHorizontalBounds; x++) {
			for (int f = -1; f < 10; f++) {
				int FMTemp = *MapFlatToggle;
				for (int t = 0; t < 2; t++) {
					*MapFlatToggle = t;
					BOOL MPtemp = *MultiplayerToggle;
					for (int m = 0; m < 2; m++) {
						*MultiplayerToggle = m;
						if (zoc_any_OG(x, y, f) != zoc_any(x, y, f)) {
							MessageBoxA(NULL, "zoc_any Error", "FATAL ERROR", MB_ICONWARNING);
						}
						if (zoc_veh_OG(x, y, f) != zoc_veh(x, y, f)) {
							MessageBoxA(NULL, "zoc_veh Error", "FATAL ERROR", MB_ICONWARNING);
						}
						if (zoc_sea_OG(x, y, f) != zoc_sea(x, y, f)) {
							MessageBoxA(NULL, "zoc_sea Error", "FATAL ERROR", MB_ICONWARNING);
						}
						if (zoc_move_OG(x, y, f) != zoc_move(x, y, f)) {
							MessageBoxA(NULL, "zoc_move Error", "FATAL ERROR", MB_ICONWARNING);
						}
					}
					*MultiplayerToggle = MPtemp;
				}
				*MapFlatToggle = FMTemp;
			}
		}
	}
	*/
}
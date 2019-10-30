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
#include "faction.h"
#include "general.h"
#include "base.h"
#include "map.h"
#include "veh.h"
#include "maininterface.h"
#include "technology.h"
#include "terraforming.h"

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

typedef BOOL func12(int, int, int, int);
func12 *facility_avail_OG = (func12 *)0x005BA0E0;


typedef int func21(int, int);
func21 *fixed_div_OG = (func21 *)0x00628AD0;
typedef void func22(const void *, const void *, char);
func22 *memrchr_OG = (func22 *)0x00628AF0;

///
LPSTR *ParseTempPtr1_1 = (LPSTR *)0x009B7D00;
LPSTR *ParseTempPtr2_1 = (LPSTR *)0x009B7D04;
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
	for (uint32_t i = 0xFFFF0000; i < 0xFFFFFFFF; i++) {
		if (fixed_div(i, 0xFF) != fixed_div_OG(i, 0xFF)) {
			MessageBoxA(NULL, "Error", "FATAL ERROR", MB_ICONWARNING);
		}
	}
	/*
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < *BaseCurrentCount; j++) {
			for (int k = 0; k <= FAC_EMPTY_SP_64; k++) {
				if (k != FAC_PARADISE_GARDEN) {
					if (facility_avail_OG(k, i, j, 10) != facility_avail(k, i, j, 10)) {
						char szError[150];
						wsprintfA(szError,
							"facility_avail error!\n"
							"FacilityID: %d\n"
							"FactionID: %d\n"
							"BaseID: %d",
							k, i, j);
						MessageBoxA(NULL, szError, "FATAL ERROR", MB_ICONWARNING);
					}
				}
			}
		}
	}
	return 0;
	*/
	/*
	for (int i = 0; i < MaxPlayerNum; i++) {
		for (int j = 0; j < *BaseCurrentCount; j++) {
			for (int k = 0; k < *VehCurrentCount; k++) {
				int protoID = Veh[k].protoID;
				if(veh_avail(protoID, i, j) != veh_avail_OG(protoID, i, j)) {
					char szError[150];
					wsprintfA(szError,
						"Aborting due to a heap shortage!\n"
						"ProtoID: %d\n"
						"FactionID: %d\n"
						"BaseID: %d",
						protoID, i, j);
					MessageBoxA(NULL, szError, "FATAL ERROR", MB_ICONWARNING);
				}
			}
			for (int l = 0; l < MaxVehProtoNum+5; l++) {
				if (veh_avail(l, i, j) != veh_avail_OG(l, i, j)) {
					char szError[150];
					wsprintfA(szError,
						"Aborting due to a heap shortage!\n"
						"ProtoID: %d\n"
						"FactionID: %d\n"
						"BaseID: %d",
						l, i, j);
					MessageBoxA(NULL, szError, "FATAL ERROR", MB_ICONWARNING);
				}
			}
		}
	}

	*SMACX_Enabled = false;
	for (int i = 0; i < MaxPlayerNum; i++) {
		for (int j = 0; j < *BaseCurrentCount; j++) {
			for (int k = 0; k < *VehCurrentCount; k++) {
				if (veh_avail(Veh[k].protoID, i, j) != veh_avail_OG(Veh[k].protoID, i, j)) {
					char szError[150];
					wsprintfA(szError,
						"Aborting due to a heap shortage!\n"
						"ProtoID: %d\n"
						"FactionID: %d\n"
						"BaseID: %d",
						Veh[k].protoID, i, j);
					MessageBoxA(NULL, szError, "FATAL ERROR", MB_ICONWARNING);
				}
			}
			for (int l = 0; l < MaxVehProtoNum+5; l++) {
				if (veh_avail(l, i, j) != veh_avail_OG(l, i, j)) {
					char szError[150];
					wsprintfA(szError,
						"Aborting due to a heap shortage!\n"
						"ProtoID: %d\n"
						"FactionID: %d\n"
						"BaseID: %d",
						l, i, j);
					MessageBoxA(NULL, szError, "FATAL ERROR", MB_ICONWARNING);
				}
			}
		}
	}
	*/

	// BOOL __cdecl veh_avail(int protoID, int factionID, int baseID)
	

	/*
	for (int i = 0; i < *VehCurrentCount; i++) {
		/*
		int x = Veh[i].xCoord;
		int y = Veh[i].yCoord;
		veh_drop_OG(i, 9, 19);
		veh_drop(i, x, y);
		veh_drop(i, 9, 19);
		veh_drop(i, x, y);
		veh_drop_OG(i, 17, 61);
		veh_drop(i, x, y);
		veh_drop_OG(i, 30, 56);
		veh_drop(i, x, y);
		veh_drop_OG(i, 22, 62);
		veh_drop(i, x, y);
		veh_drop_OG(i, x, y);
		veh_drop(i, x, y);
		veh_drop(i, 17, 61);
		veh_drop(i, 30, 56);
		veh_drop(i, 22, 62);
		veh_drop(i, x, y);
		*/
	//}
	return 0;
}

void __cdecl testerMap() {
	/*
	for (int y = -1; y < (int)*MapVerticalBounds; y++) {
		for (int x = -1; x < (int)*MapHorizontalBounds; x++) {
			for (int z = 0; z < 2; z++) {
				if (is_coast(x, y, z) != is_coast_OG(x, y, z)) {
					MessageBoxA(NULL, "is_coast Error", "FATAL ERROR", MB_ICONWARNING);
				}
			}
		}
	}
	*/
}
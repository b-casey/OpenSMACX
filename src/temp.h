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

enum game_rules_general_set1 {
	TGL_SCENARIO_CHEATED_FLAG = 0x20,
	TGL_SCENARIO_EDITOR = 0x40,
	TGL_OMNISCIENT_VIEW = 0x80,
	TGL_DEBUG_MODE = 0x1000,
	SCN_VICT_TERRITORY_COUNT_OBJ = 0x8000,
	SCN_VICT_ALL_ARTIFACTS_OBJ_UNIT = 0x40000,
	SCN_VICT_HIGHEST_AC_SCORE_WINS = 0x80000,
	TGL_PERIHELION_ACTIVE = 0x100000,
	RAND_FAC_LEADER_PERSONALITIES = 0x800000,
	RAND_FAC_LEADER_SOCIAL_AGENDA = 0x1000000,
	SCN_VICT_TERRAIN_ENH_COUNT_OBJ = 0x2000000,
	SCN_VICT_BASE_FACIL_COUNT_OBJ = 0x4000000,
	SCN_VICT_POPULATION_COUNT_OBJ = 0x20000000,
	SCN_VICT_TECH_COUNT_OBJ = 0x40000000,
	SCN_VICT_CREDITS_COUNT_OBJ = 0x80000000,
};


typedef struct char256 { char str[256]; } char256;
typedef struct char1032 { char str[1032]; } char1032;

// built-in functions
typedef void *func1(size_t);
extern func1 *_malloc;
typedef void *func2(void *);
extern func2 *_free;
typedef FILE *func3(LPCSTR, LPCSTR);
extern func3 *_fopen;
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
typedef DWORD func7(int, int);
extern func7 *morale_alien;

// string
typedef int *func8(LPSTR, LPSTR);
extern func8 *parse_string;

// testing
typedef DWORD func9(int);
extern func9 *veh_health_;


///
extern LPSTR *ParseTempPtr1_1;
extern LPSTR *ParseTempPtr2_1;
extern char1032 *stringTemp;
extern char256 *ParseStrBuffer;

extern int *ParseNumTable;
extern int *ParseStrPlurality;
extern int *ParseStrGender;
extern int *GenderDefault;
extern int *PluralityDefault;
extern int *Language;
extern int *TechValidCount;
extern int *TechCommerceCount;
extern BOOL *SMACX_Enabled;
extern BOOL *IsLoggingDisabled; // global toggle for logging on/off? debug vs release?
extern DWORD *GameRules; // bitfield
extern int *DiffLevelCurrent;
extern int *BaseCurrentCount;
extern int *TurnCurrentNum;
extern int *MissionYearCurrent;
extern BOOL *MultiplayerToggle;

extern DWORD *UnkBitfield1;

extern Filefind *FilefindPath;
extern MainInterface *MainInterfaceVar;

OPENSMACX_API void __cdecl tester();
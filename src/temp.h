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

typedef int *func10(void *, size_t, size_t, FILE *);
extern func10 *_fread;
extern func10 *_fwrite;
typedef int func17(FILE *);
extern func17 *_fgetc;
typedef int func18(int, FILE *);
extern func18 *_fputc;
//typedef void *func12(void *, size_t);
//extern func12 *_realloc;
//typedef int *func18(FILE *);
//extern func18 *_fclose;
//typedef LPSTR *func19(LPSTR, int, FILE *);
//extern func19 *_fgets;

// other
typedef void func5(int);
extern func5 *load_faction_art;
extern func5 *draw_map;
typedef void *func6(char const *, int(*)(void));
extern func6 *X_pop;
typedef int func7(LPCSTR, LPCSTR, int, LPCSTR, int(__cdecl *)());
extern func7 *popp;
typedef void *func9(void);
extern func9 *fixup_landmarks;
extern func9 *mapwin_terrain_fixup;
typedef BOOL func11(int, int, int);
extern func11 *wants_to_attack;
typedef int func14(int, int);
extern func14 *base_at;
typedef uint32_t func15(LPCSTR);
extern func15 *save_daemon;
typedef uint32_t func16(LPCSTR, BOOL);
extern func16 *load_daemon;
// Time
typedef void func30(int);
extern func30 *blink_timer;
extern func30 *blink2_timer;
extern func30 *line_timer;
extern func30 *turn_timer;

// testing
typedef int *func8(LPSTR, LPSTR);
extern func8 *parse_string_OG;
typedef void func12(int);
extern func12 *enemy_capabilities_OG;
///
extern char1032 *stringTemp;
extern char256 *ParseStrBuffer;

extern int *BufferStrHeight;
extern int *ParseNumTable;
extern int *ParseStrPlurality;
extern int *ParseStrGender;
extern HWND *HandleMain;

extern uint32_t *UnkBitfield1;

extern Filefind *FilefindPath;
extern MainInterface *MainInterfaceVar;

OPENSMACX_API int __cdecl tester();

// tech_val output
typedef int func13(int, int, BOOL);
extern func13 *tech_val_OG;
OPENSMACX_API void __cdecl tech_calc_output();

// message handling testing
typedef void *func_msg(void);
extern func_msg *do_video;
extern func_msg *check_net;
extern func_msg *do_net;
extern func_msg *do_non_input_;

extern uint32_t *MsgStatus;

OPENSMACX_API BOOL __cdecl do_non_input();
OPENSMACX_API void __cdecl do_all_non_input();
OPENSMACX_API BOOL __cdecl do_draw();
OPENSMACX_API void __cdecl do_all_draws();
OPENSMACX_API BOOL __cdecl do_keyboard();
OPENSMACX_API void __cdecl do_all_keyboard();

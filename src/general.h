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
#pragma once

 /*
  * General related objects, variables and functions.
  */
struct Filefind {
    char cdPath[256];   // internal
    char altPath[256];  // internal
    char lastPath[256]; // used globally
    char exeDir[256];   // internal
    char relativePath[256]; // internal
};

extern uint32_t ScenEditorUndoPosition;
extern int *GenderDefault;
extern BOOL *PluralityDefault;

OPENSMACX_API void __cdecl purge_trailing(LPSTR input);
OPENSMACX_API void __cdecl purge_leading(LPSTR input);
OPENSMACX_API void __cdecl purge_spaces(LPSTR input);
OPENSMACX_API void __cdecl kill_lf(LPSTR str);
OPENSMACX_API void __cdecl kill_nl(LPSTR str);
OPENSMACX_API void __cdecl add_lf(LPSTR str);
OPENSMACX_API int __cdecl range(int input, int min, int max);
OPENSMACX_API LPVOID __cdecl mem_get_old(size_t size);
OPENSMACX_API LPVOID __cdecl mem_get(size_t size);
OPENSMACX_API FILE *__cdecl env_open_old(LPCSTR source, LPCSTR mode);
OPENSMACX_API FILE *__cdecl env_open(LPCSTR source, LPCSTR mode);
OPENSMACX_API void __cdecl parse_set(int gender, BOOL plurality);
OPENSMACX_API int __cdecl parse_num(uint32_t id, int value);
OPENSMACX_API int __cdecl parse_say(uint32_t id, int input, int gender, int pluralality);
OPENSMACX_API int __cdecl parse_says(uint32_t id, LPCSTR input, int gender, int pluralality);
OPENSMACX_API int __cdecl btoi(LPCSTR str);
OPENSMACX_API int __cdecl htoi(LPCSTR str);
OPENSMACX_API int __cdecl stoi(LPCSTR str);
OPENSMACX_API LPSTR __cdecl findnum(LPSTR str);
OPENSMACX_API BOOL __cdecl jackal_version_check(LPCSTR version);
OPENSMACX_API char __cdecl filefind_cd_drive_letter();
OPENSMACX_API void __cdecl filefind_set_alternative(LPCSTR path);
OPENSMACX_API LPSTR __cdecl filefind_get(LPCSTR fileName);
OPENSMACX_API uint32_t __cdecl bit_count(uint32_t bitfield);
OPENSMACX_API uint32_t __cdecl bit_count_signed(int bitfield);
OPENSMACX_API void __cdecl my_srand(uint32_t reseed);
OPENSMACX_API void __cdecl swap(int *var1, int *var2);
OPENSMACX_API void __cdecl swap(uint8_t *var1, uint8_t *var2);
OPENSMACX_API int __cdecl fixed_div(int numer, int denom);
OPENSMACX_API const char *__cdecl memrchr(LPCSTR start, LPCSTR end, char value);
OPENSMACX_API int __cdecl quick_root(int input);
OPENSMACX_API void __cdecl bitmask(uint32_t input, uint32_t *offset, uint32_t *mask);
OPENSMACX_API uint8_t __cdecl checksum(uint8_t *input, uint32_t length, uint8_t seed);
OPENSMACX_API uint32_t __cdecl checksum_password(LPCSTR password);
OPENSMACX_API uint32_t __cdecl rnd(int bounds, LPSTR input);
OPENSMACX_API void __cdecl danger(LPCSTR msg1, LPCSTR msg2, int num1, int num2, int num3);
OPENSMACX_API void __cdecl kill_auto_save();
OPENSMACX_API void __cdecl auto_save();
OPENSMACX_API void __cdecl auto_save_debug();
OPENSMACX_API void __cdecl load_undo(int type);
OPENSMACX_API void __cdecl wipe_undo();
OPENSMACX_API void __cdecl auto_undo();
OPENSMACX_API void __cdecl header_check(LPSTR header, FILE *file);
OPENSMACX_API void __cdecl header_write(LPCSTR header, FILE *file);
OPENSMACX_API void __cdecl sort(uint32_t count, int *id, int *value);

// WIP
int __cdecl filefind_init(LPCSTR path, BOOL isComplete);
int __cdecl parse_string(LPSTR input, LPSTR output);

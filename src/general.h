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

DLLEXPORT void __cdecl purge_trailing(LPSTR input);
DLLEXPORT void __cdecl purge_leading(LPSTR input);
DLLEXPORT void __cdecl purge_spaces(LPSTR input);
DLLEXPORT void __cdecl kill_lf(LPSTR str);
DLLEXPORT void __cdecl kill_nl(LPSTR str);
DLLEXPORT void __cdecl add_lf(LPSTR str);
DLLEXPORT int __cdecl range(int input, int min, int max);
DLLEXPORT LPVOID __cdecl mem_get_old(size_t size);
DLLEXPORT LPVOID __cdecl mem_get(size_t size);
DLLEXPORT FILE *__cdecl env_open_old(LPCSTR source, LPCSTR mode);
DLLEXPORT FILE *__cdecl env_open(LPCSTR source, LPCSTR mode);
DLLEXPORT void __cdecl parse_set(int gender, BOOL plurality);
DLLEXPORT int __cdecl parse_num(uint32_t id, int value);
DLLEXPORT int __cdecl parse_say(uint32_t id, int input, int gender, int pluralality);
DLLEXPORT int __cdecl parse_says(uint32_t id, LPCSTR input, int gender, int pluralality);
DLLEXPORT int __cdecl btoi(LPCSTR str);
DLLEXPORT int __cdecl htoi(LPCSTR str);
DLLEXPORT int __cdecl stoi(LPCSTR str);
DLLEXPORT LPSTR __cdecl findnum(LPSTR str);
DLLEXPORT BOOL __cdecl jackal_version_check(LPCSTR version);
DLLEXPORT char __cdecl filefind_cd_drive_letter();
DLLEXPORT void __cdecl filefind_set_alternative(LPCSTR path);
DLLEXPORT LPSTR __cdecl filefind_get(LPCSTR fileName);
DLLEXPORT uint32_t __cdecl bit_count(uint32_t bitfield);
DLLEXPORT uint32_t __cdecl bit_count_signed(int bitfield);
DLLEXPORT void __cdecl my_srand(uint32_t reseed);
DLLEXPORT void __cdecl swap(int *var1, int *var2);
DLLEXPORT void __cdecl swap(uint8_t *var1, uint8_t *var2);
DLLEXPORT int __cdecl fixed_div(int numer, int denom);
DLLEXPORT const char *__cdecl memrchr(LPCSTR start, LPCSTR end, char value);
DLLEXPORT int __cdecl quick_root(int input);
DLLEXPORT void __cdecl bitmask(uint32_t input, uint32_t *offset, uint32_t *mask);
DLLEXPORT uint8_t __cdecl checksum(uint8_t *input, uint32_t length, uint8_t seed);
DLLEXPORT uint32_t __cdecl checksum_password(LPCSTR password);
DLLEXPORT uint32_t __cdecl rnd(int bounds, LPSTR input);
DLLEXPORT void __cdecl danger(LPCSTR msg1, LPCSTR msg2, int num1, int num2, int num3);
DLLEXPORT void __cdecl kill_auto_save();
DLLEXPORT void __cdecl auto_save();
DLLEXPORT void __cdecl auto_save_debug();
DLLEXPORT void __cdecl load_undo(int type);
DLLEXPORT void __cdecl wipe_undo();
DLLEXPORT void __cdecl auto_undo();
DLLEXPORT void __cdecl header_check(LPSTR header, FILE *file);
DLLEXPORT void __cdecl header_write(LPCSTR header, FILE *file);
DLLEXPORT void __cdecl sort(uint32_t count, int *id, int *value);

// WIP
int __cdecl filefind_init(LPCSTR path, BOOL isComplete);
int __cdecl parse_string(LPSTR input, LPSTR output);

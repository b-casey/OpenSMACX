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

OPENSMACX_API void __cdecl purge_trailing(LPSTR input);
OPENSMACX_API void __cdecl purge_leading(LPSTR input);
OPENSMACX_API void __cdecl purge_spaces(LPSTR input);
OPENSMACX_API void __cdecl kill_lf(LPSTR str);
OPENSMACX_API void __cdecl kill_nl(LPSTR str);
OPENSMACX_API void __cdecl add_lf(LPSTR str);
OPENSMACX_API int __cdecl range(int input, int min, int max);
OPENSMACX_API LPVOID __cdecl mem_get_old(size_t size);
OPENSMACX_API LPVOID __cdecl mem_get(size_t size);
OPENSMACX_API FILE * __cdecl env_open_old(LPCSTR source, LPCSTR mode);
OPENSMACX_API FILE * __cdecl env_open(LPCSTR source, LPCSTR mode);
OPENSMACX_API int __cdecl parse_num(int id, int value);
OPENSMACX_API int __cdecl parse_say(int id, int input, int gender, int pluralality);
OPENSMACX_API int __cdecl parse_says(int id, LPSTR input, int gender, int pluralality);
OPENSMACX_API int __cdecl btoi(LPCSTR str);
OPENSMACX_API int __cdecl htoi(LPCSTR str);
OPENSMACX_API int __cdecl stoi(LPCSTR str);
OPENSMACX_API LPSTR __cdecl findnum(LPSTR str);
OPENSMACX_API BOOL __cdecl jackal_version_check(LPCSTR version);
OPENSMACX_API int prefs_get(LPCSTR keyName, int defaultValue, BOOL useDefault);
OPENSMACX_API LPSTR prefs_get(LPCSTR keyName, LPCSTR defaultValue, BOOL useDefault);
OPENSMACX_API void prefs_put(LPCSTR keyName, LPCSTR value);
OPENSMACX_API void prefs_put(LPCSTR keyName, int value, BOOL binaryTgl);
OPENSMACX_API CHAR filefind_cd_drive_letter();
OPENSMACX_API void filefind_set_alternative(LPCSTR path);
OPENSMACX_API LPSTR filefind_get(LPCSTR fileName);
OPENSMACX_API DWORD __cdecl bit_count(DWORD bitfield);

// WIP
int filefind_init(LPCSTR path, BOOL isComplete); 
int __cdecl parse_string_(LPSTR input, LPSTR output);
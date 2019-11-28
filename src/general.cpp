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
#include "general.h"
#include "log.h" // log_say
#include "strings.h"
#include "text.h"

/*
Purpose: Trim trailing spaces inline from end of string
Original Offset: 00600780
Return Value: None
Status: Complete
*/
void __cdecl purge_trailing(LPSTR input) {
	LPSTR trim = input + strlen(input);
	while ((*(trim - 1) - ' ') == 0 && (trim != input)) {
		*trim--;
	}
	input[(trim - input)] = 0;
}

/*
Purpose: Trim leading spaces inline from start of string
Original Offset: 00600760
Return Value: None
Status: Complete
*/
void __cdecl purge_leading(LPSTR input) {
	LPSTR trim = input;
	while ((*trim - ' ') == 0 && *trim != 0) {
		*trim++;
	}
	strcpy_s(input, strlen(input) + 1, trim);
}

/*
Purpose: Trim leading and trailing spaces from string
Original Offset: 006007B0
Return Value: None
Status: Complete
*/
void __cdecl purge_spaces(LPSTR input) {
	purge_leading(input);
	purge_trailing(input);
}

/*
Purpose: Truncate string at LF. Doesn't take into account CR.
Original Offset: 00600820
Return Value: n/a
Status: Complete
*/
void __cdecl kill_lf(LPSTR str) {
	LPSTR newlineLoc = strrchr(str, '\n');
	if (newlineLoc) {
		*newlineLoc = 0;
	}
}

/*
Purpose: Remove newline (Windows: CR LF). Fixes bug with TextIndex having CR at end "HEADER\r" 
         breaking compare. This is because it uses Filemap vs fopen(x, "rt").
Original Offset: n/a
Return Value: n/a
Status: Complete
*/
void __cdecl kill_nl(LPSTR str) {
	LPSTR newlineLoc = strstr(str, "\r\n");
	if (newlineLoc) {
		*newlineLoc = 0;
	}
}

/*
Purpose: Add LF to end of string, assumes buffer has an extra byte. Doesn't take into account CR.
Original Offset: 00600840
Return Value: n/a
Status: Complete
*/
void __cdecl add_lf(LPSTR str) {
	int len = strlen(str);
	str[len] = '\n';
	str[len + 1] = 0;
}

/*
Purpose: Use min and max bounds to limit input
Original Offset: 00422F00
Return Value: Bounded input
Status: Complete
*/
int __cdecl range(int input, int min, int max) {
	if (input < min || max < min) {
		return min;
	}
	if (input > max) {
		return max;
	}
	return input;
}

/*
Purpose: Allocate memory with error checking
Original Offset: 005D4510
Return Value: Pointer to allocated memory
Status: Complete with two versions of malloc to prevent crash. Incompatibility with
		newer SDK version of malloc CRT. Revisit once more code is redirected to dll.
*/
LPVOID __cdecl mem_get_old(size_t size) {
	LPVOID result = _malloc(size);
	if (!result) {
		MessageBoxA(NULL,
			"Windows has run out of virtual memory; the game will exit. "
			"Recommendations: defragment your hard drive; use Control Panel "
			"(System/Performance/Virtual Memory) to increase your virtual memory to at least 128MB",
			"FATAL ERROR", MB_ICONWARNING);
		exit(4);
	}
	return result;
}

LPVOID __cdecl mem_get(size_t size) {
	LPVOID result = malloc(size);
	if (!result) {
		MessageBoxA(NULL,
			"Windows has run out of virtual memory; the game will exit. "
			"Recommendations: defragment your hard drive; use Control Panel "
			"(System/Performance/Virtual Memory) to increase your virtual memory to at least 128MB",
			"FATAL ERROR", MB_ICONWARNING);
		exit(4);
	}
	return result;
}
/*
Purpose: Checks file paths, opens file
Original Offset: 00634BB0
Return Value: FILE ptr
Status: Complete with two versions of fopen to prevent crash. Incompatibility with
		newer SDK version of fopen/fopen_s. Revisit once more code is redirected to dll.
*/
FILE * __cdecl env_open_old(LPCSTR source, LPCSTR mode) {
	LPCSTR srcCheck = filefind_get(source);
	if (!srcCheck) {
		srcCheck = source;
	}
	return _fopen(srcCheck, mode);
}

FILE * __cdecl env_open(LPCSTR source, LPCSTR mode) {
	LPCSTR srcCheck = filefind_get(source);
	if (!srcCheck) {
		srcCheck = source;
	}
	FILE *fileOut;
	fopen_s(&fileOut, srcCheck, mode);
	return fileOut;
}

/*
Purpose: Set global gender and plurality values used by parse functions.
Original Offset: 005A58E0
Return Value: n/a
Status: Complete
*/
void __cdecl parse_set(int gender, int plurality) {
	*GenderDefault = gender;
	*PluralityDefault = plurality;
}

/*
Purpose: Copies number value into number message buffer using id
Original Offset: 00625E30
Return Value: 0: no errors; 3: error
Status: Complete
*/
int __cdecl parse_num(int id, int value) {
	if (id > 9) {
		return 3; // error
	}
	ParseNumTable[id] = value;
	return 0;
}

/*
Purpose: Copies string into message buffer from string table
Original Offset: 00625E50
Return Value: 0: no errors; 3: error
Status: Complete
*/
int __cdecl parse_say(int id, int input, int gender, int pluralality) {
	if (id > 9) {
		return 3;
	}
	if (gender < 0) {
		gender = *GenderDefault;
	}
	ParseStrGender[id] = gender;
	if (pluralality < 0) {
		pluralality = *PluralityDefault;
	}
	ParseStrPlurality[id] = pluralality;
	strcpy_s(ParseStrBuffer[id].str, 256, StringTable->get(input));
	return 0;
}

/*
Purpose: Copies string into message buffer
Original Offset: 00625EC0
Return Value: 0: no errors; 3: error
Status: Complete
*/
int __cdecl parse_says(int id, LPSTR input, int gender, int pluralality) {
	if (!input || id > 9) {
		return 3;
	}
	if (gender < 0) {
		gender = *GenderDefault;
	}
	ParseStrGender[id] = gender;
	if (pluralality < 0) {
		pluralality = *PluralityDefault;
	}
	ParseStrPlurality[id] = pluralality;
	strcpy_s(ParseStrBuffer[id].str, 256, input);
	return 0;
}

/*
Purpose: Convert binary string to int.
Original Offset: 006288D0
Return Value: int value of string
Status: Complete
*/
int __cdecl btoi(LPCSTR str) {
	int result = 0;
	while (*str == '0' || *str == '1') {
		result = *str++ - '0' + 2 * result;
	}
	return result;
}

/*
Purpose: Convert hex string to int.
Original Offset: 006288F0
Return Value: int value of string
Status: Complete
*/
int __cdecl htoi(LPCSTR str) {
	int result = 0;
	while (isxdigit(*str)) {
		result *= 16;
		if (isdigit(*str)) {
			result += *str - '0';
		} 
		else {
			result += toupper(*str) - '7';
		}
		*str++;
	}
	return result;
}

/*
Purpose: Converts a binary, hex or decimal string to int.
Original Offset: 00628950
Return Value: int value of string
Status: Complete
*/
int __cdecl stoi(LPCSTR str) {
	if (*str == '0') {
		*str++;
		switch (*str) 
		{
			case 'B':
			case 'b':
			{
				*str++;
				return btoi(str);
			}
			case 'X':
			case 'x':
			{
				*str++;
				return htoi(str);
			}
			case 'D':
			case 'd':
			{
				*str++;
				return atoi(str);
			}
			default:
			{
				return atoi(str);
			}
		}
	}
	return atoi(str);
}

/*
Purpose: Locates first number in string
Original Offset: 00628B30
Return Value: Pointer to first number, otherwise zero
Status: Complete
*/
LPSTR __cdecl findnum(LPSTR str) {
	if (!str) {
		return 0;
	}
	while ((*str < '0') || (*str > '9')) {
		if (*str == 0) {
			return 0;
		}
		*str++;
	}
	return str;
}

/*
Purpose: Check to see if JACKAL lib version is up to date. Pretty pointless but might add OpenSMACX 
         check in future.
Original Offset: 0062D570
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL __cdecl jackal_version_check(LPCSTR version) {
	if (strcmp(version, "10.10")) {
		MessageBoxA(NULL,
			"The version of the JACKAL library you (Brian Reynolds) are attempting to use "
			"is out of sync with the library headers! Please rebuild JACKAL.LIB...Bye!",
			"Oh, such serious danger, William Robinson!!!",
			MB_OK);
		return true;
	}
	return false;
}

/*
Purpose: Attempt to read setting's value from Alpha Centauri ini file.
Original Offset: 0059DB40
Return Value: Key's integer value from ini or default if not set
Status: Complete with internal string pointers. Eventually, clean up code dependent on string 
        globals and remove these references.
*/
int prefs_get(LPCSTR keyName, int defaultValue, BOOL useDefault) {
	_itoa_s(defaultValue, stringTemp->str, 256, 10);
	if (useDefault) {
		strcpy_s(*TextBufferGetPtr, 256, stringTemp->str);
		return defaultValue;
	}
	GetPrivateProfileStringA("Alpha Centauri", keyName, stringTemp->str, *TextBufferGetPtr, 256,
		".\\Alpha Centauri.ini");
	return atoi(Txt->update());
}

/*
Purpose: Attempt to read setting's value from Alpha Centauri ini file.
Original Offset: 0059D980
Return Value: Key's string value from ini or default if not set
Status: Complete with internal string pointers. Eventually, clean up dependent code on string 
        globals and remove these references.
*/
LPSTR prefs_get(LPCSTR keyName, LPCSTR defaultValue, BOOL useDefault) {
	if (useDefault ||
		(GetPrivateProfileStringA("Alpha Centauri", "Prefs Format", "0", *TextBufferGetPtr, 256,
			".\\Alpha Centauri.ini"), atoi(*TextBufferGetPtr) != 12)) {
		strcpy_s(*TextBufferGetPtr, 256, defaultValue);
	}
	else {
		GetPrivateProfileStringA("Alpha Centauri", keyName, defaultValue, *TextBufferGetPtr, 256,
			".\\Alpha Centauri.ini");
	}
	return Txt->update();
}

/*
Purpose: Write string value to pref key of ini.
Original Offset: 0059E510
Return Value: n/a
Status: Complete
*/
void prefs_put(LPCSTR keyName, LPCSTR value) {
	WritePrivateProfileStringA("Alpha Centauri", keyName, value, ".\\Alpha Centauri.ini");
}

/*
Purpose: Write value as either integer or binary to pref key inside ini.
Original Offset: 0059E530
Return Value: n/a
Status: Complete
*/
void prefs_put(LPCSTR keyName, int value, BOOL binaryTgl) {
	char temp[33];
	if (binaryTgl) {
		temp[0] = 0;
		for (int shift = 31, nonPad = 0; shift >= 0; shift--) {
			if ((1 << shift) & value) {
				nonPad = 1;
				strcat_s(temp, 33, "1");
			}
			else if (nonPad || shift < 8) {
				strcat_s(temp, 33, "0");
			}
		}
	}
	else {
		_itoa_s(value, temp, 33, 10);
	}
	WritePrivateProfileStringA("Alpha Centauri", keyName, temp, ".\\Alpha Centauri.ini");
}

/*
Purpose: Primary string parsing function
Original Offset: 00625880
Return Value: 0: no errors; 3: error
Status: WIP
*/
int __cdecl parse_string(LPSTR input, LPSTR output) {
	if (!input || !output) {
		return 3;
	}
	LPSTR parseOut = output, var;
	do {
		var = strstr(input, "$");
		if (!var) {
			break;
		}
		switch (var[1])
		{
		case '$':
		{
			int len = (var - input) + 1;
			strncpy_s(parseOut, 1024, input, len);
			parseOut += len;
			input = var + 2;
		}
		break;
		case 'H':
		case 'N':
		{
			int nCheck = 0, nBase = 0;
			if (!strncmp(var, "$NUMBER", 7)) {
				nCheck = 7;
				nBase = 10;
			}
			else if (!strncmp(var, "$NUM", 4)) {
				nCheck = 4;
				nBase = 10;
			}
			else if (!strncmp(var, "$HEX", 4)) {
				nCheck = 4;
				nBase = 16;
			}
			if (nCheck) {
				int nNum = var[nCheck] - '0';
				if (nNum > 9 || nNum < 0) {
					return 14;
				}
				strncpy_s(parseOut, 1024, input, var - input);
				char szTemp[12];
				_itoa_s(ParseNumTable[nNum], szTemp, 12, nBase);
				strcat_s(parseOut, 1024, szTemp);
				input = var + nCheck + 1;
				parseOut += strlen(parseOut);

			}
			else {
				LPSTR pNum = 0;// findnum(var);
				if (!pNum) {
					var = 0; // prevent infinite loop
					break;
				}
				strncpy_s(parseOut, 1024, input, var - input);
				//strcat_s(pParseOut, 1024, g_parse_str_buffer[*pNum - '0']);
				input = ++pNum;
				parseOut += strlen(parseOut);
			}
		}
		break;
		case '<':
		{
			int nLen = var - input;
			strncpy_s(parseOut, 1024, input, nLen);
			LPSTR pEnd = strstr(var, ">");
			if (!pEnd) {
				input = var + 1;
			}
			var += 2;
			input = pEnd + 1;
			purge_leading(var);
			int nPlural = 0, nGender, nNum = -1;
			char szGender = tolower(var[0]);
			if (szGender == 'm') {
				nGender = 0;
			}
			else if (szGender == 'f') {
				nGender = 1;
			}
			else if (szGender == 'n') {
				nGender = 2;
			}
			else if (szGender == '#') {
				nNum = var[1] - '0';
				if (nNum > 9 || nNum < 0) {
					break;
				}
				var++;
				nPlural = (ParseNumTable[nNum] == 1) ? 0 : 1;
			}
			else {
				nNum = szGender - '0';
				if (nNum > 9 || nNum < 0) {
					break;
				}
				nPlural = ParseStrPlurality[nNum];
				nGender = ParseStrGender[nNum];
			}
			var++;
			if (nNum < 0) {
				if (isdigit(var[0])) {
					nNum = var[0] - '1';
					var++;
				}
			}
			purge_leading(var);
			if (var[0] == ':') {
				var++;
			}
			// unfinished


		}
		break;
		case 'L':
		{
			if (!strncmp(var, "$LINK<", 6)) {
				int len = (var - input) + 1;
				strncpy_s(parseOut, 1024, input, len);
				parseOut += len;
				input = var + 1;
				break;
			}
		}
		default:
		{
			LPSTR num = findnum(var);
			if (!num) {
				var = 0;
				break;
			}
			int number = num[0] - '0';
			if (number > 9) {
				return 14; // error
			}
			memcpy_s(parseOut, 1024, input, var - input + 1);
			//*input = &output + 1
			//strncpy_s(parseOut, 1024, input, (num - input));
			//strcat_s(pParseOut, 1024, g_parse_str_buffer[*pNum - '0']);
			//input = ++pNum;
			parseOut += strlen(parseOut);
		}
		break;
		}
	} while (var);

	//strcat(output, input); // replace with safe version _s

	if (*Language == 1) { // French : handling for poor translations
		do {
			//LPSTR partSpeach[] = { " de ", " le ", " la ", "De ", "Le ", "La " };
			LPSTR langParse = output;
			LPSTR searching = 0;
			for (int i = 0; i < 6; i++) {
				LPSTR partFound = 0;// strstr(langParse, partSpeach[i]);
				if (partFound && (!searching || partFound < searching)) {
					searching = partFound;
				}
			}
		} while (1);

		/*
		int nLoop = 0;
		do {


				int nOffset = (nLoop < 3) + 3;
				char szVowel = tolower(partFound[nOffset]);
				if (szVowel == 'a' || szVowel == 'e' || szVowel == 'i' 
				|| szVowel == 'o' || szVowel == 'u' || szVowel == 'y' || szVowel == 'h') {
					int nDiff = strlen(output) - strlen(partFound);
					strcpy_s(&output[nDiff + nOffset - 1], 1024, &output[nDiff + nOffset]);
					output[nDiff + nOffset - 2] = '\'';
				}
			}
			else {
				nLoop++;
			}
		} while (nLoop < 6);
		*/
	}

	return false;
}

/*
Purpose: Get drive letter of CD path
Original Offset: 006003A0
Return Value: Drive letter
Status: Complete
*/
char filefind_cd_drive_letter() { return FilefindPath->cdPath[0]; }

/*
Purpose: Sets an alternative path for filefind checks
Original Offset: 006003B0
Return Value: n/a
Status: Complete
*/
void filefind_set_alternative(LPCSTR path) {
	FilefindPath->altPath[0] = 0;
	if (path) {
		if (!strchr(path, ':') && path[0] != '\\') {
			strcat_s(FilefindPath->altPath, 256, FilefindPath->exeDir);
		}
		strcat_s(FilefindPath->altPath, 256, path);
	}
}

/*
Purpose: Initialize filefind struct and CD check with callback if not complete install. Changed 
         logic since most installs will be on HDD making CD check less important.
Original Offset: 00600400
Return Value: Zero: no errors; Non-zero: error
Status: WIP
*/
int filefind_init(LPCSTR fileCheck, BOOL isComplete) {
	FilefindPath->altPath[0] = 0;
	GetCurrentDirectoryA(256, FilefindPath->exeDir);
	strcat_s(FilefindPath->exeDir, 256, "\\");

	if (isComplete) {
		return 0; // complete install, no need for further checks
	}
	if (!fileCheck) {
		return 16; // error, fileCheck shouldn't be NULL
	}
	WIN32_FIND_DATAA findFileData;
	strcpy_s(FilefindPath->lastPath, 256, FilefindPath->exeDir);
	strcat_s(FilefindPath->lastPath, fileCheck);
	HANDLE hFileFound = FindFirstFileA(FilefindPath->lastPath, &findFileData);
	FindClose(hFileFound);
	if (hFileFound != INVALID_HANDLE_VALUE) {
		return 0; // complete install on HDD, no need for CD
	}

	/*
	JACKAL_CLASS callBack = JACKAL_init();
	if(!callBack)
	return 4;
	*/
	char szRootPath[5];
	do {
		strcpy_s(szRootPath, 5, "A:\\");
		for (int nPos = 0; nPos < 26; nPos++) {
			if (GetDriveTypeA(szRootPath) == DRIVE_CDROM) {
				// problem if drive was disconnected
				strcpy_s(FilefindPath->lastPath, 256, szRootPath);
				strcat_s(FilefindPath->lastPath, fileCheck);
				//WIN32_FIND_DATA findFileData;
				//HANDLE hFileFound = FindFirstFile(g_filefind.last_path, &findFileData);
				hFileFound = FindFirstFileA(FilefindPath->lastPath, &findFileData);
				FindClose(hFileFound);
				if (hFileFound != INVALID_HANDLE_VALUE) {
					strcpy_s(FilefindPath->cdPath, 256, szRootPath);
					// destroy JACKAL callBack
					return 0;
				}
			}
			szRootPath[0]++;
		}
		// send FILEFIND_NOCD message -> if doesn't exist (removed from jackal.txt) -> exit
		// if user response is to retry -> loop around again
		// if user response is "ok", exit loop
	} while (0);
	// destroy JACKAL callBack
	return 0;
}

/*
Purpose: Checks to see if it can find file at some other path
Original Offset: 006005D0
Return Value: File path string or 0 if not found
Status: Complete
*/
LPSTR filefind_get(LPCSTR fileName) {
	if (!fileName) {
		return 0;
	}
	if (fileName == FilefindPath->lastPath) {
		return FilefindPath->lastPath;
	}
	WIN32_FIND_DATAA findFileData;
	HANDLE hFileFound;
	if (fileName[1] == ':') {
		strcpy_s(FilefindPath->lastPath, 256, fileName);
		hFileFound = FindFirstFileA(FilefindPath->lastPath, &findFileData);
		FindClose(hFileFound);
		return (hFileFound != INVALID_HANDLE_VALUE) ? FilefindPath->lastPath : 0;
	}
	if (FilefindPath->altPath[0]) {
		strcpy_s(FilefindPath->lastPath, 256, FilefindPath->altPath);
		strcat_s(FilefindPath->lastPath, 256, fileName);
		hFileFound = FindFirstFileA(FilefindPath->lastPath, &findFileData);
		FindClose(hFileFound);
		if (hFileFound != INVALID_HANDLE_VALUE) {
			return FilefindPath->lastPath;
		}
	}
	strcpy_s(FilefindPath->lastPath, 256, FilefindPath->exeDir);
	strcat_s(FilefindPath->lastPath, 256, fileName);
	hFileFound = FindFirstFileA(FilefindPath->lastPath, &findFileData);
	FindClose(hFileFound);
	if (hFileFound != INVALID_HANDLE_VALUE) {
		return FilefindPath->lastPath;
	}
	if (FilefindPath->cdPath[0]) {
		strcpy_s(FilefindPath->lastPath, 256, FilefindPath->cdPath);
		strcat_s(FilefindPath->lastPath, 256, fileName);
		hFileFound = FindFirstFileA(FilefindPath->lastPath, &findFileData);
		FindClose(hFileFound);
		if (hFileFound != INVALID_HANDLE_VALUE) {
			return FilefindPath->lastPath;
		}
	}
	return 0;
}

/*
Purpose: Count number of bits set (unsigned). Replaced existing code with Brian Kernighan's algo.
Original Offset: 0050BA30
Return Value: Bit count
Status: Complete
*/
uint32_t __cdecl bit_count(uint32_t bitfield) {
	uint32_t count;
	for (count = 0; bitfield; count++) {
		bitfield &= bitfield - 1; // clear the least significant bit set
	}
	return count;
}

/*
Purpose: Count number of bits set (signed). Added a fix to prevent an infinite loop. Only referenced
         by one GraphicWin function.
Original Offset: 00628AB0
Return Value: Bit count
Status: Complete
*/
uint32_t __cdecl bit_count_signed(int input) {
	uint32_t count = 0;
	for (count = 0; input && count < 32; count++) {
		input >>= 1;
	}
	return count;
}

/*
Purpose: Initialize pseudo-random number generator.
Original Offset: 00538FB0
Return Value: n/a
Status: Complete with built in version of srand(). Revisit once more code is redirected to dll.
*/
void __cdecl my_srand(uint32_t reseed) {
	log_say("Reseed to", reseed, 0, 0);
	//srand(reseed);
	_srand(reseed);
}

/*
Purpose: Swap the values of two 32-bit variables. Added an additional check when swapping the same
		 memory location. Future: convert to MACRO?
Original Offset: 00628A50
Return Value: n/a
Status: Complete
*/
void __cdecl swap(int *var1, int *var2) {
	if (var1 == var2) {
		return;
	}
	int temp = *var1 ^ *var2;
	*var1 = temp;
	*var2 ^= temp;
	*var1 ^= *var2;
}

/*
Purpose: Swap the values of two 8-bit variables. Added an additional check when swapping the same
         memory location. Future: convert to MACRO?
Original Offset: 00628A80
Return Value: n/a
Status: Complete
*/
void __cdecl swap(uint8_t *var1, uint8_t *var2) {
	if (var1 == var2) {
		return;
	}
	uint8_t temp = *var1 ^ *var2;
	*var1 = temp;
	*var2 ^= temp;
	*var1 ^= *var2;
}

/*
Purpose: Shift numerator to left by 16 then divide denominator. Added a check to prevent divide by 
         zero crash.
Original Offset: 00628AD0
Return Value: Quotient
Status: Complete
*/
int __cdecl fixed_div(int numer, int denom) {
	if (!denom) {
		return 0;
	}
	return ((int64_t)numer << 16) / denom;
}

/*
Purpose: Reverse string search for last occurrence of specified character. Replaced searching logic
         with strrchr() that does same thing. End parameter can be removed in future.
Original Offset: 00628AF0
Return Value: Position of character or null if not found.
Status: Complete
*/
const char *__cdecl memrchr(const char *start, const char *end, char value) {
	if (!start || !end || start == end) {
		return 0;
	}
	return strrchr(start, value);
}

/*
Purpose: Calculate the square root of input.
Original Offset: 006290E0
Return Value: root
Status: Complete
*/
int __cdecl quick_root(int input) {
	int temp = input >> 1, root;
	if (input <= 1) {
		return input;
	}
	do {
		root = temp;
		temp = (temp + input / temp) >> 1;
	} while (temp < root);
	return root;
}
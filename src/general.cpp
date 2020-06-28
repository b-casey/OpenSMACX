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
#include "alpha.h"
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
FILE *__cdecl env_open_old(LPCSTR source, LPCSTR mode) {
	LPCSTR srcCheck = filefind_get(source);
	if (!srcCheck) {
		srcCheck = source;
	}
	return _fopen(srcCheck, mode);
}

FILE *__cdecl env_open(LPCSTR source, LPCSTR mode) {
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
int __cdecl parse_num(uint32_t id, int value) {
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
int __cdecl parse_say(uint32_t id, int input, int gender, int pluralality) {
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
int __cdecl parse_says(uint32_t id, LPCSTR input, int gender, int pluralality) {
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
		switch (*str) {
		case 'B':
		case 'b':
			*str++;
			return btoi(str);
		case 'X':
		case 'x':
			*str++;
			return htoi(str);
		case 'D':
		case 'd':
			*str++;
			return atoi(str);
		default:
			return atoi(str);
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
Purpose: Primary string parsing function
Original Offset: 00625880
Return Value: 0: no errors; 3: error
Status: WIP
*/
int __cdecl parse_string(LPSTR input, LPSTR output) {
	if (!input || !output) { // EBX || ESI
		return 3;
	}
	LPSTR outputCopy = output;
	LPSTR var; // EDI
	do {
		var = strstr(input, "$");
		if (!var) {
			break;
		}
		LPSTR parsingInput = &input[1];
		switch (var[1]) {
		case '$': // done -> needs testing
		{
			int len = (var - input) + 1;
			strncpy_s(output, 1024, input, len);
			output += len;
			input = var + 2;
			*output = 0;
		}
		break;
		case 'H': // done -> needs testing
		{
			if (strncmp(var, "$HEX", 4)) {
				int number = var[4] - '0';
				if (number > 9) {
					return 14; // parse error
				}
				int len = var - input;
				strncpy_s(output, 1024, input, len);
				output += len;
				*output = 0;
				input = var + 5;
				char outputNum[5];
				_itoa_s(ParseNumTable[number], outputNum, 5, 10);
				strcat_s(output, 1024, outputNum);
				output += strlen(output);
			}
			else {
				LPSTR num = findnum(var);
				if (!num) {
					break;
				}
				int number = num[0] - '0';
				if (number > 9) {
					return 14; // parse error
				}
				int len = (var - input) + 1;
				strncpy_s(output, 1024, input, len);
				output += len;
				*output = 0;
				input = num + 1;
				strcat_s(output, 1024, ParseStrBuffer[number].str);
				output += strlen(output);
			}
		}
		break;
		case 'N': // done -> needs testing
		{
			if (strncmp(var, "$NUMBER", 7)) {
				int number = var[7] - '0';
				if (number > 9) {
					return 14; // parse error
				}
				int len = var - input;
				strncpy_s(output, 1024, input, len);
				output += len;
				*output = 0;
				input = var + 8;
				char outputNum[5];
				_itoa_s(ParseNumTable[number], outputNum, 5, 10);
				strcat_s(output, 1024, outputNum);
				output += strlen(output);
			}
			else if (strncmp(var, "$NUM", 4)) {
				int number = var[4] - '0';
				if (number > 9) {
					return 14; // parse error
				}
				int len = var - input;
				strncpy_s(output, 1024, input, len);
				output += len;
				*output = 0;
				input = var + 5;
				char outputNum[5];
				_itoa_s(ParseNumTable[number], outputNum, 5, 10);
				strcat_s(output, 1024, outputNum);
				output += strlen(output);
			}
			else {
				LPSTR num = findnum(var);
				if (!num) {
					break;
				}
				int number = num[0] - '0';
				if (number > 9) {
					return 14; // parse error
				}
				int len = var - input;
				strncpy_s(output, 1024, input, len);
				output += len;
				*output = 0;
				input = num + 1;
				strcat_s(output, 1024, ParseStrBuffer[number].str);
				output += strlen(output);
			}
		}
		break;
		case '<':
		{
			int len = var - input;
			strncpy_s(output, 1024, input, len);
			output += len;
			*output = 0;
			LPSTR endBracket = strstr(var, ">");
			if (!endBracket) {
				input = var + 1;
				continue;
			}
			var += 2;
			input = endBracket + 1;
			purge_leading(var);
			int gender, plural = 0, num = -1;
			switch (var[0]) {
			case 'M':
			case 'm':
			{
				gender = 0;
			}
			break;
			case 'F':
			case 'f':
			{
				gender = 1;
			}
			break;
			case 'N':
			case 'n':
			{
				gender = 2;
			}
			break;
			case '#':
			{
				//
			}
			break;
			default:
			{
				//
			}
			break;
			}
			/*
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
			*/
		}
		break;
		case 'L': // done -> needs testing
		{
			if (strncmp(var, "$LINK<", 6)) {
				LPSTR num = findnum(var);
				if (!num) {
					var = 0;
					break;
				}
				int number = num[0] - '0';
				if (number > 9) {
					return 14; // parse error
				}
				int len = var - input;
				strncpy_s(output, 1024, input, len);
				output += len;
				*output = 0;
				input = num + 1;
				strcat_s(output, 1024, ParseStrBuffer[number].str);
				output += strlen(output);
			}
			else {
				int len = (var - input) + 1;
				strncpy_s(output, 1024, input, len);
				output += len;
				*output = 0;
				input = var + 1;
				break;
			}
		}
		break;
		default: // done -> needs testing
		{
			LPSTR num = findnum(var);
			if (!num) {
				var = 0;
				break;
			}
			int number = num[0] - '0';
			if (number > 9) {
				return 14; // parse error
			}
			int len = var - input;
			strncpy_s(output, 1024, input, len);
			output += len;
			*output = 0;
			input = num + 1;
			strcat_s(output, 1024, ParseStrBuffer[number].str);
			output += strlen(output);
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
	srand(reseed);
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
const char *__cdecl memrchr(LPCSTR start, LPCSTR end, char value) {
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

/*
Purpose: Calculate offset & bitmask for input.
Original Offset: 0050BA00
Return Value: n/a
Status: Complete
*/
void __cdecl bitmask(uint32_t input, uint32_t *offset, uint32_t *mask) {
	*offset = input / 8;
	*mask = 1 << (input & 7);
}

/*
Purpose: Calculate a basic XOR checksum for input data.
Original Offset: 00539090
Return Value: checksum
Status: Complete
*/
uint8_t __cdecl checksum(uint8_t *buffer, uint32_t size, uint8_t seed) {
	while (size--) seed ^= *buffer++;
	return seed;
}

/*
Purpose: Calculate a basic XOR checksum for a password string.
Original Offset: 005390C0
Return Value: checksum
Status: Complete
*/
uint32_t __cdecl checksum_password(LPCSTR password) {
	if (!strlen(password)) {
		return 0;
	}
	char buffer[256];
	strcpy_s(buffer, 256, password);
	CharUpper(buffer); // incorrect results for Turkish/Azerbaijani 'i'
	uint8_t chksum = 0;
	uint32_t len = strlen(buffer);
	if (len) {
		chksum = checksum((LPBYTE)&buffer, len, 0);
	}
	return chksum + 1;
}

/*
Purpose: Calculate a random value within provided bounds. The 2nd string parameter is unused. It
		 was possibly meant to have the random value append to it. Left in for compatibility.
Original Offset: 00579770
Return Value: Randomized value
Status: Complete
*/
uint32_t __cdecl rnd(int bounds, LPSTR UNUSED(input)) {
	return (bounds - 1 > 0) ? rand() % bounds : 0;
}

/*
Purpose: Create a debug error pop-up with both messages and numbers then write these values to log.
Original Offset: 00538F30
Return Value: n/a
Status: Complete
*/
void __cdecl danger(LPCSTR msg1, LPCSTR msg2, int num1, int num2, int num3) {
	parse_says(0, msg1, -1, -1);
	parse_says(1, msg2, -1, -1);
	parse_num(0, num1);
	parse_num(1, num2);
	parse_num(2, num3);
	log_say(msg1, msg2, num1, num2, num3);
	popp(ScriptTxtID, "DANGER", 0, "hasty_sm.pcx", 0);
}

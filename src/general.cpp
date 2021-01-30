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
#include "stdafx.h"
#include "temp.h"
#include "general.h"
#include "alpha.h"
#include "game.h"
#include "log.h" // log_say
#include "strings.h"
#include "text.h"

uint32_t ScenEditorUndoPosition = 1; // 0x00690D7C
int *GenderDefault = (int *)0x009BBFEC;
BOOL *PluralityDefault = (BOOL *)0x009BBFF0;

/*
Purpose: Trim the trailing spaces in-line from the end of the string.
Original Offset: 00600780
Return Value: n/a
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
Purpose: Trim the leading spaces in-line from the start of the string.
Original Offset: 00600760
Return Value: n/a
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
Purpose: Trim the leading and trailing spaces from the string.
Original Offset: 006007B0
Return Value: n/a
Status: Complete
*/
void __cdecl purge_spaces(LPSTR input) {
    purge_leading(input);
    purge_trailing(input);
}

/*
Purpose: Truncate the string at the line feed (LF). Doesn't take into account carriage return (CR).
Original Offset: 00600820
Return Value: n/a
Status: Complete
*/
void __cdecl kill_lf(LPSTR str) {
    LPSTR newline_loc = strrchr(str, '\n');
    if (newline_loc) {
        *newline_loc = 0;
    }
}

/*
Purpose: Remove newline (Windows: CR LF). This fixes a bug with TextIndex having CR at the end of 
         "HEADER\r" breaking the compare. This is because it uses Filemap vs fopen(x, "rt").
Original Offset: n/a
Return Value: n/a
Status: Complete
*/
void __cdecl kill_nl(LPSTR str) {
    LPSTR newline_loc = strstr(str, "\r\n");
    if (newline_loc) {
        *newline_loc = 0;
    }
}

/*
Purpose: Add a line feed (LF) to the end of a string. This assumes the buffer has an extra byte and 
         doesn't take into account a carriage return (CR).
Original Offset: 00600840
Return Value: n/a
Status: Complete
*/
void __cdecl add_lf(LPSTR str) {
    size_t len = strlen(str);
    str[len] = '\n';
    str[len + 1] = 0;
}

/*
Purpose: Use the min and max parameters to bound the input.
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
Purpose: Allocate memory with error checking.
Original Offset: 005D4510
Return Value: Pointer to allocated memory
Status: Complete with two versions of malloc to prevent crash. Incompatibility with newer SDK 
        version of malloc CRT. Revisit once more code is redirected to dll.
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
Purpose: Check the source file path and attempt to open a handle to the file.
Original Offset: 00634BB0
Return Value: FILE pointer
Status: Complete with two versions of fopen to prevent a crash. Incompatibility with newer SDK 
        version of fopen/fopen_s. Revisit once more code is redirected to dll.
*/
FILE *__cdecl env_open_old(LPCSTR source, LPCSTR mode) {
    LPCSTR src_check = filefind_get(source);
    if (!src_check) {
        src_check = source;
    }
    return _fopen(src_check, mode);
}

FILE *__cdecl env_open(LPCSTR source, LPCSTR mode) {
    LPCSTR src_check = filefind_get(source);
    if (!src_check) {
        src_check = source;
    }
    FILE *file_out;
    fopen_s(&file_out, src_check, mode);
    return file_out;
}

/*
Purpose: Set the global gender and plurality variables used by various parse functions.
Original Offset: 005A58E0
Return Value: n/a
Status: Complete
*/
void __cdecl parse_set(int gender, BOOL plurality) {
    *GenderDefault = gender;
    *PluralityDefault = plurality;
}

/*
Purpose: Copies the value into a number global message buffer using id.
Original Offset: 00625E30
Return Value: No errors (0); Error (3)
Status: Complete
*/
int __cdecl parse_num(uint32_t id, int value) {
    if (id > 9) {
        return 3;
    }
    ParseNumTable[id] = value;
    return 0;
}

/*
Purpose: Use the string table input reference to copy a string into the global message buffer.
Original Offset: 00625E50
Return Value: No errors (0); Error (3)
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
Purpose: Copies the input string into the global message buffer.
Original Offset: 00625EC0
Return Value: No errors (0); Error (3)
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
Purpose: Convert the binary string to an integer.
Original Offset: 006288D0
Return Value: Integer value of the string
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
Purpose: Convert the hex string to an integer.
Original Offset: 006288F0
Return Value: Integer value of the string
Status: Complete
*/
int __cdecl htoi(LPCSTR str) {
    int result = 0;
    while (isxdigit(*str)) {
        result *= 16;
        if (isdigit(*str)) {
            result += *str - '0';
        } else {
            result += toupper(*str) - '7';
        }
        *str++;
    }
    return result;
}

/*
Purpose: Converts a binary, hex or decimal string to an integer.
Original Offset: 00628950
Return Value: Integer value of the string
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
Purpose: Locates the first number in a string.
Original Offset: 00628B30
Return Value: Pointer to the first number, otherwise zero
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
Purpose: Checks to see if the JACKAL library version is up to date. Pretty pointless but might add 
         an OpenSMACX check in the future.
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
Purpose: This handles parsing the input string and storing it in the output.
Original Offset: 00625880
Return Value: No errors (0); Error (3)
Status: WIP
*/
int __cdecl parse_string(LPSTR input, LPSTR output) {
    if (!input || !output) { // EBX || ESI
        return 3;
    }
    //LPSTR outputCopy = output;
    LPSTR var; // EDI
    do {
        var = strstr(input, "$");
        if (!var) {
            break;
        }
        //LPSTR parsingInput = &input[1];
        switch (var[1]) {
          case '$': { // done -> needs testing
            int len = (var - input) + 1;
            strncpy_s(output, 1024, input, len);
            output += len;
            input = var + 2;
            *output = 0;
            break;
          }
          case 'H': { // done -> needs testing
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
                char output_num[5];
                _itoa_s(ParseNumTable[number], output_num, 5, 10);
                strcat_s(output, 1024, output_num);
                output += strlen(output);
            } else {
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
            break;
          }
          case 'N': { // done -> needs testing
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
                char output_num[5];
                _itoa_s(ParseNumTable[number], output_num, 5, 10);
                strcat_s(output, 1024, output_num);
                output += strlen(output);
            } else if (strncmp(var, "$NUM", 4)) {
                int number = var[4] - '0';
                if (number > 9) {
                    return 14; // parse error
                }
                int len = var - input;
                strncpy_s(output, 1024, input, len);
                output += len;
                *output = 0;
                input = var + 5;
                char output_num[5];
                _itoa_s(ParseNumTable[number], output_num, 5, 10);
                strcat_s(output, 1024, output_num);
                output += strlen(output);
            } else {
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
            break;
          }
          case '<': {
            int len = var - input;
            strncpy_s(output, 1024, input, len);
            output += len;
            *output = 0;
            LPSTR end_bracket = strstr(var, ">");
            if (!end_bracket) {
                input = var + 1;
                continue;
            }
            var += 2;
            input = end_bracket + 1;
            purge_leading(var);
            int gender;// , plural = 0, num = -1;
            switch (var[0]) {
              case 'M':
              case 'm':
                gender = 0;
                break;
              case 'F':
              case 'f':
                gender = 1;
                break;
              case 'N':
              case 'n':
                gender = 2;
                break;
              case '#':
                //
                break;
              default:
                //
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
            } else {
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
            break;
          }
          case 'L': { // done -> needs testing
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
            } else {
                int len = (var - input) + 1;
                strncpy_s(output, 1024, input, len);
                output += len;
                *output = 0;
                input = var + 1;
                break;
            }
            break;
          }
          default: { // done -> needs testing
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
            break;
          }
        }
    } while (var);

    //strcat(output, input); // replace with safe version _s

    if (*Language == 1) { // French : handling for poor translations
        do {
            //LPSTR partSpeach[] = { " de ", " le ", " la ", "De ", "Le ", "La " };
            //LPSTR langParse = output;
            LPSTR searching = 0;
            for (int i = 0; i < 6; i++) {
                LPSTR part_found = 0;// strstr(langParse, partSpeach[i]);
                if (part_found && (!searching || part_found < searching)) {
                    searching = part_found;
                }
            }
        } while (1);

        /*
        int nLoop = 0;
        do {
                int nOffset = (nLoop < 3) + 3;
                char szVowel = tolower(part_found[nOffset]);
                if (szVowel == 'a' || szVowel == 'e' || szVowel == 'i'
                || szVowel == 'o' || szVowel == 'u' || szVowel == 'y' || szVowel == 'h') {
                    int nDiff = strlen(output) - strlen(part_found);
                    strcpy_s(&output[nDiff + nOffset - 1], 1024, &output[nDiff + nOffset]);
                    output[nDiff + nOffset - 2] = '\'';
                }
            } else {
                nLoop++;
            }
        } while (nLoop < 6);
        */
    }

    return false;
}

/*
Purpose: Get the drive letter of the CD path.
Original Offset: 006003A0
Return Value: CD drive letter
Status: Complete
*/
char __cdecl filefind_cd_drive_letter() { return FilefindPath->cd_path[0]; }

/*
Purpose: Set an alternative path for the Filefind checks.
Original Offset: 006003B0
Return Value: n/a
Status: Complete
*/
void __cdecl filefind_set_alternative(LPCSTR path) {
    FilefindPath->alt_path[0] = 0;
    if (path) {
        if (!strchr(path, ':') && path[0] != '\\') {
            strcat_s(FilefindPath->alt_path, 256, FilefindPath->exe_dir);
        }
        strcat_s(FilefindPath->alt_path, 256, path);
    }
}

/*
Purpose: Initialize the Filefind global along with a CD check if there isn't a complete install. 
         Optimized logic since most installs will be on a HDD making the CD check less important.
Original Offset: 00600400
Return Value: No errors (0) otherwise error
Status: WIP
*/
int __cdecl filefind_init(LPCSTR file_check, BOOL is_complete) {
    FilefindPath->alt_path[0] = 0;
    GetCurrentDirectoryA(256, FilefindPath->exe_dir);
    strcat_s(FilefindPath->exe_dir, 256, "\\");

    if (is_complete) {
        return 0; // complete install, no need for further checks
    }
    if (!file_check) {
        return 16; // error, file_check shouldn't be NULL
    }
    WIN32_FIND_DATAA find_file_data;
    strcpy_s(FilefindPath->last_path, 256, FilefindPath->exe_dir);
    strcat_s(FilefindPath->last_path, file_check);
    HANDLE file_found = FindFirstFileA(FilefindPath->last_path, &find_file_data);
    FindClose(file_found);
    if (file_found != INVALID_HANDLE_VALUE) {
        return 0; // complete install on HDD, no need for CD
    }

    /*
    JACKAL_CLASS callBack = JACKAL_init();
    if(!callBack)
    return 4;
    */
    char root_path[5];
    do {
        strcpy_s(root_path, 5, "A:\\");
        for (int i = 0; i < 26; i++) {
            if (GetDriveTypeA(root_path) == DRIVE_CDROM) {
                // problem if drive was disconnected
                strcpy_s(FilefindPath->last_path, 256, root_path);
                strcat_s(FilefindPath->last_path, file_check);
                //WIN32_FIND_DATA find_file_data;
                //HANDLE file_found = FindFirstFile(g_filefind.last_path, &find_file_data);
                file_found = FindFirstFileA(FilefindPath->last_path, &find_file_data);
                FindClose(file_found);
                if (file_found != INVALID_HANDLE_VALUE) {
                    strcpy_s(FilefindPath->cd_path, 256, root_path);
                    // destroy JACKAL callBack
                    return 0;
                }
            }
            root_path[0]++;
        }
        // send FILEFIND_NOCD message -> if doesn't exist (removed from jackal.txt) -> exit
        // if user response is to retry -> loop around again
        // if user response is "ok", exit loop
    } while (0);
    // destroy JACKAL callBack
    return 0;
}

/*
Purpose: Check to see if the specified file can be found at some other path.
Original Offset: 006005D0
Return Value: File path string or NULL if not found
Status: Complete
*/
LPSTR __cdecl filefind_get(LPCSTR file_name) {
    if (!file_name) {
        return 0;
    }
    if (file_name == FilefindPath->last_path) {
        return FilefindPath->last_path;
    }
    WIN32_FIND_DATAA find_file_data;
    HANDLE file_found;
    if (file_name[1] == ':') {
        strcpy_s(FilefindPath->last_path, 256, file_name);
        file_found = FindFirstFileA(FilefindPath->last_path, &find_file_data);
        FindClose(file_found);
        return (file_found != INVALID_HANDLE_VALUE) ? FilefindPath->last_path : 0;
    }
    if (FilefindPath->alt_path[0]) {
        strcpy_s(FilefindPath->last_path, 256, FilefindPath->alt_path);
        strcat_s(FilefindPath->last_path, 256, file_name);
        file_found = FindFirstFileA(FilefindPath->last_path, &find_file_data);
        FindClose(file_found);
        if (file_found != INVALID_HANDLE_VALUE) {
            return FilefindPath->last_path;
        }
    }
    strcpy_s(FilefindPath->last_path, 256, FilefindPath->exe_dir);
    strcat_s(FilefindPath->last_path, 256, file_name);
    file_found = FindFirstFileA(FilefindPath->last_path, &find_file_data);
    FindClose(file_found);
    if (file_found != INVALID_HANDLE_VALUE) {
        return FilefindPath->last_path;
    }
    if (FilefindPath->cd_path[0]) {
        strcpy_s(FilefindPath->last_path, 256, FilefindPath->cd_path);
        strcat_s(FilefindPath->last_path, 256, file_name);
        file_found = FindFirstFileA(FilefindPath->last_path, &find_file_data);
        FindClose(file_found);
        if (file_found != INVALID_HANDLE_VALUE) {
            return FilefindPath->last_path;
        }
    }
    return 0;
}

/*
Purpose: Count the number of unsigned bits set. Replaced the original code with Brian Kernighan's 
         algorithm.
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
Purpose: Count the number of signed bits set. Added a fix to prevent an infinite loop.
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
Purpose: Initialize the pseudo-random number generator.
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
         memory location.
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
         memory location.
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
Purpose: Shift the numerator to the left by 16 then divide by the denominator. Added a check to 
         prevent a divide by zero crash.
Original Offset: 00628AD0
Return Value: Quotient
Status: Complete
*/
int __cdecl fixed_div(int numerator, int denominator) {
    if (!denominator) {
        return 0;
    }
    return ((int64_t)numerator << 16) / denominator;
}

/*
Purpose: Reverse string search for the last occurrence of the specified character. Replaced the
         original searching logic with strrchr() that does same thing. The end parameter can be 
         removed in the future.
Original Offset: 00628AF0
Return Value: Position of character or NULL if not found.
Status: Complete
*/
const char *__cdecl memrchr(LPCSTR start, LPCSTR end, char value) {
    if (!start || !end || start == end) {
        return 0;
    }
    return strrchr(start, value);
}

/*
Purpose: Calculate the square root of the input.
Original Offset: 006290E0
Return Value: Square root
Status: Complete
*/
int __cdecl quick_root(int input) {
    int temp = input >> 1;
    int sq_root;
    if (input <= 1) {
        return input;
    }
    do {
        sq_root = temp;
        temp = (temp + input / temp) >> 1;
    } while (temp < sq_root);
    return sq_root;
}

/*
Purpose: Calculate the offset and bitmask for the specified input.
Original Offset: 0050BA00
Return Value: n/a
Status: Complete
*/
void __cdecl bitmask(uint32_t input, uint32_t *offset, uint32_t *mask) {
    *offset = input / 8;
    *mask = 1 << (input & 7);
}

/*
Purpose: Calculate a basic XOR checksum for the data buffer.
Original Offset: 00539090
Return Value: Checksum
Status: Complete
*/
uint8_t __cdecl checksum(uint8_t *buffer, uint32_t size, uint8_t seed) {
    while (size--) seed ^= *buffer++;
    return seed;
}

/*
Purpose: Calculate a basic XOR checksum for a password string.
Original Offset: 005390C0
Return Value: Checksum
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
    size_t len = strlen(buffer);
    if (len) {
        chksum = checksum((LPBYTE)&buffer, len, 0);
    }
    return chksum + 1;
}

/*
Purpose: Calculate a random value within the provided bounds. The unused 2nd parameter was possibly 
         meant to have the random value append to it.
Original Offset: 00579770
Return Value: Bounded random value
Status: Complete
*/
uint32_t __cdecl rnd(int bounds, LPSTR UNUSED(input)) {
    return (bounds - 1 > 0) ? rand() % bounds : 0;
}

/*
Purpose: Create a debug error pop-up then write all the parameters to the log file.
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

/*
Purpose: Delete the initial auto-save game file.
Original Offset: 005ABD10
Return Value: n/a
Status: Complete
*/
void __cdecl kill_auto_save() {
    remove("saves\\auto\\Alpha Centauri Autosave 1.SAV");
}

/*
Purpose: Handle the creation and management of the auto-save game files.
Original Offset: 005ABD20
Return Value: n/a
Status: Complete
*/
void __cdecl auto_save() {
    if (!*IsMultiplayerPBEM || *IsMultiplayerNet) { // auto-saving disabled for PBEM/HotSeat games
        if (*GameRules & RULES_IRONMAN && !(*GameState & STATE_SCENARIO_EDITOR)) {
            remove("saves\\auto\\Alpha Centauri Autosave 30.SAV");
            remove("saves\\auto\\Alpha Centauri Autosave 20.SAV");
            remove("saves\\auto\\Alpha Centauri Autosave 10.SAV");
            remove("saves\\auto\\Alpha Centauri Autosave 5.SAV");
            remove("saves\\auto\\Alpha Centauri Autosave 4.SAV");
            remove("saves\\auto\\Alpha Centauri Autosave 3.SAV");
            remove("saves\\auto\\Alpha Centauri Autosave 2.SAV");
            save_daemon("saves\\auto\\Alpha Centauri Autosave 1");
        } else { // standard auto saves
            if (!(*TurnCurrentNum % 10)) {
                remove("saves\\auto\\Alpha Centauri Autosave 30.SAV");
                rename("saves\\auto\\Alpha Centauri Autosave 20.SAV",
                    "saves\\auto\\Alpha Centauri Autosave 30.SAV");
                rename("saves\\auto\\Alpha Centauri Autosave 10.SAV",
                    "saves\\auto\\Alpha Centauri Autosave 20.SAV");
                save_daemon("saves\\auto\\Alpha Centauri Autosave 10");
            }
            remove("saves\\auto\\Alpha Centauri Autosave 5.SAV");
            rename("saves\\auto\\Alpha Centauri Autosave 4.SAV",
                "saves\\auto\\Alpha Centauri Autosave 5.SAV");
            rename("saves\\auto\\Alpha Centauri Autosave 3.SAV",
                "saves\\auto\\Alpha Centauri Autosave 4.SAV");
            rename("saves\\auto\\Alpha Centauri Autosave 2.SAV",
                "saves\\auto\\Alpha Centauri Autosave 3.SAV");
            rename("saves\\auto\\Alpha Centauri Autosave 1.SAV",
                "saves\\auto\\Alpha Centauri Autosave 2.SAV");
            save_daemon("saves\\auto\\Alpha Centauri Autosave 1");
        }
    }
}

/*
Purpose: Extended auto-saving regardless of the game type or settings for debug purposes.
Original Offset: n/a
Return Value: n/a
Status: Complete
*/
void __cdecl auto_save_debug() {
    if (*TurnCurrentNum == 1) {
        remove("saves\\auto\\Alpha Centauri Autosave Turn 1.SAV");
        save_daemon("saves\\auto\\Alpha Centauri Autosave Turn 1");
    }
    if (!(*TurnCurrentNum % 10)) {
        remove("saves\\auto\\Alpha Centauri Autosave 500.SAV");
        char save_path_new[45];
        char save_path_old[45];
        for (int i = 0; i < 490; i += 10) {
            sprintf_s(save_path_old, 45, "saves\\auto\\Alpha Centauri Autosave %d.SAV", 490 - i);
            sprintf_s(save_path_new, 45, "saves\\auto\\Alpha Centauri Autosave %d.SAV", 500 - i);
            rename(save_path_old, save_path_new);
        }
        save_daemon("saves\\auto\\Alpha Centauri Autosave 10");
    }
    remove("saves\\auto\\Alpha Centauri Autosave 5.SAV");
    rename("saves\\auto\\Alpha Centauri Autosave 4.SAV",
        "saves\\auto\\Alpha Centauri Autosave 5.SAV");
    rename("saves\\auto\\Alpha Centauri Autosave 3.SAV",
        "saves\\auto\\Alpha Centauri Autosave 4.SAV");
    rename("saves\\auto\\Alpha Centauri Autosave 2.SAV",
        "saves\\auto\\Alpha Centauri Autosave 3.SAV");
    rename("saves\\auto\\Alpha Centauri Autosave 1.SAV",
        "saves\\auto\\Alpha Centauri Autosave 2.SAV");
    save_daemon("saves\\auto\\Alpha Centauri Autosave 1");
}

/*
Purpose: Load a Scenario Editor undo (type: 1) or redo (type: -1) auto-save. TODO: Revisit in the 
         future to fix some of the underlying issues with the undo/redo process.
Original Offset: 005ABE40
Return Value: n/a
Status: Complete
*/
void __cdecl load_undo(int type) {
    if (type == -1 && ScenEditorUndoPosition == 1) {
        return; // bug fix: skip redo if undo hasn't been triggered yet or on 1st undo
    }
    if (type < 0 && ScenEditorUndoPosition > 1) {
        ScenEditorUndoPosition--;
    }
    char load_path[38];
    sprintf_s(load_path, 38, "saves\\auto\\Scenario Editor Undo %d.SAV", ScenEditorUndoPosition);
    if (type > 0 && ScenEditorUndoPosition < 9) {
        ScenEditorUndoPosition++;
    }
    load_daemon(load_path, false);
    draw_map(true); // Bug fix: Map artifacts display issue; TODO: best method of refreshing map?
}

/*
Purpose: Remove all the existing Scenario Editor undo auto-saves.
Original Offset: 005ABEC0
Return Value: n/a
Status: Complete
*/
void __cdecl wipe_undo() {
    char undo_path[38];
    for (int i = 9; i >= 1; i--) {
        sprintf_s(undo_path, 38, "saves\\auto\\Scenario Editor Undo %d.SAV", i);
        remove(undo_path);
    }
}

/*
Purpose: Handle the creation of an undo auto-save when certain Scenario Editor changes are made.
Original Offset: 005ABF20
Return Value: n/a
Status: Complete
*/
void __cdecl auto_undo() {
    if (*GamePreferences & PREF_BSC_AUTOSAVE_EACH_TURN) {
        ScenEditorUndoPosition = 1;
        remove("saves\\auto\\Scenario Editor Undo 9.SAV");
        char save_path_new[38];
        char save_path_old[38];
        for (int i = 9; i >= 2; i--) {
            sprintf_s(save_path_old, 38, "saves\\auto\\Scenario Editor Undo %d.SAV", i - 1);
            sprintf_s(save_path_new, 38, "saves\\auto\\Scenario Editor Undo %d.SAV", i);
            rename(save_path_old, save_path_new);
        }
        save_daemon("saves\\auto\\Scenario Editor Undo 1");
    }
}

/*
Purpose: Read the specified header from a file. This assumes the header string buffer is at least 
         256 characters. TODO: Replace built-in versions of _fgetc and change return to std::string.
Original Offset: 0057D1F0
Return Value: n/a
Status: Complete
*/
void __cdecl header_check(LPSTR header, FILE *file) {
    int header_chr = _fgetc(file);
    *header++ = (char)header_chr;
    if (header_chr) {
        int i = 0;
        do {
            if (++i >= 256) {
                break;
            }
            header_chr = _fgetc(file);
            *header++ = (char)header_chr;
        } while (header_chr);
    }
    _fgetc(file);
}

/*
Purpose: Write the specified header to a file. TODO: Replace built-in versions of _fputc.
Original Offset: 0057D240
Return Value: n/a
Status: Complete
*/
void __cdecl header_write(LPCSTR header, FILE *file) {
    int header_chr;
    do {
        header_chr = *header++;
        _fputc(header_chr, file);
    } while (header_chr);
    _fputc(0x1A, file);
}

/*
Purpose: For the count, sort both id and value arrays by the least to greatest value (ascending).
Original Offset: 005B5690
Return Value: n/a
Status: Complete
*/
void __cdecl sort(uint32_t count, int *id, int *value) {
    int bounds = count - 1;
    BOOL has_swapped;
    do {
        has_swapped = false;
        for (int i = 0; i < bounds; i++) {
            if (value[i] > value[i + 1]) {
                has_swapped = true;
                swap(&value[i], &value[i + 1]);
                swap(&id[i], &id[i + 1]);
            }
        }
    } while (has_swapped);
}

/*
Purpose: For the count, sort both id and value arrays by the greatest to least value (descending).
Original Offset: n/a
Return Value: n/a
Status: Complete
*/
void __cdecl sort_descending(uint32_t count, int *id, int *value) {
    int bounds = count - 1;
    BOOL has_swapped;
    do {
        has_swapped = false;
        for (int i = 0; i < bounds; i++) {
            if (value[i] < value[i + 1]) {
                has_swapped = true;
                swap(&value[i], &value[i + 1]);
                swap(&id[i], &id[i + 1]);
            }
        }
    } while (has_swapped);
}

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
#include "text.h"
#include "general.h"
#include "log.h"
#include "strings.h"
#include "textindex.h"

/*
Purpose: Initialize variables and allocate memory for parse buffers
Original Offset: 005FD8D0
Return Value: Non-zero error (4); 0 no errors
Status: Complete
*/
int Text::init(size_t size) {
	shutdown();
	bufferGet = (LPSTR)mem_get(size);
	if (!bufferGet) {
		return 4;
	}
	bufferItem = (LPSTR)mem_get(size);
	if (!bufferItem) {
		return 4;
	}
	return 0;
}

/*
Purpose: Close file and free memory for Text parse buffers
Original Offset: 005FD970
Return Value: n/a
Status: Complete
*/
void Text::shutdown() {
	close();
	if (bufferGet) {
		free(bufferGet);
		bufferGet = 0;
	}
	if (bufferItem) {
		free(bufferItem);
		bufferItem = 0;
	}
}

/*
Purpose: Close only file handle
Original Offset: 005FD9D0
Return Value: n/a
Status: Complete
*/
void Text::close() {
	if (textFile) {
		fclose(textFile);
		textFile = 0;
	}
}

/*
Purpose: Open specified source txt file and copy section into buffer for parsing
Original Offset: 005FDA00
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL Text::open(LPCSTR srcFileID, LPCSTR sectionID) {
	BOOL isFileOpen = false; // open / possibly tracker for reading through whole file?
	if (srcFileID) {
		strcpy_s(fileName, 80, srcFileID);
		if (!strchr(fileName, '.')) {
			strcat_s(fileName, 80, ".txt");
		}
		close();
		textFile = env_open(fileName, "rt");
		if (!textFile) {
			return true;
		}
		strcpy_s(filePath, 256, FilefindPath->lastPath);
	}
	else if (textFile) {
		isFileOpen = true;
	}
	else {
		textFile = env_open(fileName, "rt");
		if (!textFile) {
			return true;
		}
		strcpy_s(filePath, 256, FilefindPath->lastPath);
	}
	if (!sectionID) {
		return false;
	}

	int seekAddr = text_search_index(fileName, sectionID);
	if (seekAddr >= 0) {
		log_say("Seeking to", sectionID, seekAddr, 0, 0);
		fseek(textFile, seekAddr, SEEK_SET);
		isFileOpen = true;
	}

	std::string sectHeader("#");
	sectHeader += sectionID;
	do {
		if (feof(textFile)) {
			if (isFileOpen) {
				isFileOpen = false;
				rewind(textFile);
			}
			else {
				close();
				return true;
			}
		}
		if (!fgets(bufferGet, 511, textFile)) {
			close();
			return true;
		}
		kill_lf(bufferGet);
		purge_spaces(bufferGet);
	} while (_stricmp(sectHeader.c_str(), bufferGet));
	currentPos = bufferGet;
	return false;
}

/*
Purpose: Parse until newline, copying text into buffer from open file
Original Offset: 005FDC10
Return Value: Pointer to string
Status: Complete
*/
LPSTR Text::get() {
	if (feof(textFile)) {
		bufferGet[0] = 0;
		return NULL;
	}
	if (fgets(bufferGet, 511, textFile)) {
		kill_lf(bufferGet);
		purge_spaces(bufferGet);
		currentPos = bufferGet;
	}
	else {
		bufferGet[0] = 0;
	}
	return bufferGet;
}

/*
Purpose: Get string and add it to string table
Original Offset: 005FDC80
Return Value: Pointer to string
Status: Complete
*/
LPSTR Text::string() {
	return StringTable->put(get());
}

/*
Purpose: Parse string into buffer
Original Offset: 005FDD10
Return Value: Pointer to string
Status: Complete
*/
LPSTR Text::item() {
	LPSTR lpParse = bufferItem;
	while (*currentPos != 0 && *currentPos != ',') {
		*lpParse++ = *currentPos++;
	}
	*lpParse = 0;
	if (*currentPos) {
		*currentPos++;
	}
	purge_spaces(bufferItem);
	return bufferItem;
}

/*
Purpose: Parse string into buffer and add it to string table
Original Offset: 005FDD80
Return Value: Pointer to string from string table
Status: Complete
*/
LPSTR Text::item_string() {
	return StringTable->put(item());
}

/*
Purpose: Parse number string into buffer and return integer value
Original Offset: 005FDDF0
Return Value: Pointer to string from string table
Status: Complete
*/
int Text::item_number() {
	return stoi(item());
}

/*
Purpose: Parse binary string into buffer and return integer value
Original Offset: 005FDE60
Return Value: Pointer to string from string table
Status: Complete
*/
int Text::item_binary() {
	return btoi(item());
}

/*
Purpose: Parse hex string into buffer and return integer value
Original Offset: 005FDED0
Return Value: Pointer to string from string table
Status: Complete
*/
int Text::item_hex() {
	return htoi(item());
}

// global
Text *Txt = (Text *)0x009B7BA0;
LPSTR *TextBufferGetPtr = (LPSTR *)0x009B7D00;
LPSTR *TextBufferItemPtr = (LPSTR *)0x009B7D04;

void __cdecl text_txt() { *Txt = *(new Text(512)); atexit(text_txt_exit); } // 005FD400

void __cdecl text_txt_exit() { Txt->~Text(); } // 005FD460

void __cdecl text_set_get_ptr() { *TextBufferGetPtr = Txt->getBufferGet(); } // 005FD4C0

void __cdecl text_set_item_ptr() { *TextBufferItemPtr = Txt->getBufferItem(); } // 005FD4D0

void __cdecl text_close() { Txt->close(); } // 005FD530

BOOL __cdecl text_open(LPCSTR srcID, LPCSTR sectionID) { // 005FD550
	return Txt->open(srcID, sectionID);
}

LPSTR __cdecl text_get() { return Txt->get(); } // 005FD570

LPSTR __cdecl text_string() { return Txt->string(); } // 005FD5E0

LPSTR __cdecl text_item() { return Txt->item(); } // 005FD670

LPSTR __cdecl text_item_string() { return Txt->item_string(); } // 005FD6D0

int __cdecl text_item_number() { return Txt->item_number(); } // 005FD740

int __cdecl text_item_binary() { return Txt->item_binary(); } // 005FD7A0

int __cdecl text_item_hex() { return Txt->item_hex(); } // 005FD800

int __cdecl text_get_number(int min, int max) { // 00585120
	text_get(); return range(text_item_number(), min, max);
}

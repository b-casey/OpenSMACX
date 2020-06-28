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
#include "textindex.h"
#include "general.h"
#include "filemap.h"

/*
Purpose: Read specified file in from parameter and create an index of section headers ("#EXAMPLE").
		Store the file offset of each section for improved search time.
Original Offset: 005FDF80
Return Value: n/a
Status: Complete
*/
void TextIndex::make_index(LPCSTR sourceTxt) {
	Filemap txtFileMap;
	shutdown();
	strcpy_s(fileName, 256, sourceTxt);
	if (!strchr(fileName, '.')) {
		strcat_s(fileName, 256, ".txt"); // append extension if missing
	}
	LPVOID txtAddrBase = txtFileMap.open_read(fileName, false);
	if (txtAddrBase) {
		LPVOID seekAddr = txtAddrBase;
		LPVOID eofAddr = LPVOID(uint32_t(txtAddrBase) + txtFileMap.getSize());
		heap.init(0x8000); // maximum amount of memory per section index
		while (seekAddr < eofAddr) {
			LPVOID foundAddr = memchr(seekAddr, '\n', uint32_t(eofAddr) - uint32_t(seekAddr));
			if (!foundAddr) {
				break; // invalid file, no newlines
			}
			char parseBuffer[512]; // maximum line length
			memcpy_s(parseBuffer, 512, seekAddr, uint32_t(foundAddr) - uint32_t(seekAddr) + 1);
			kill_nl(parseBuffer); // bug fix; Filemap reads in raw file including carriage return
			purge_spaces(parseBuffer);
			if (parseBuffer[0] == '#' && isupper(parseBuffer[1])) {
				uint32_t len = strlen(&parseBuffer[1]) + 1;
				LPVOID storeAddr = heap.get(len + 4); // section name + address
				*(LPDWORD)storeAddr = uint32_t(seekAddr) - uint32_t(txtAddrBase); // file offset
				strcpy_s(LPSTR(storeAddr) + 4, len, &parseBuffer[1]);
				sectionCount++;
			}
			seekAddr = LPVOID(uint32_t(foundAddr) + 1);
		}
		heap.squeeze(true);
	}
}

/*
Purpose: Search the source text file for section header text.
Original Offset: 005FE120
Return Value: File offset if found, otherwise -1
Status: Complete
*/
int TextIndex::search_index(LPCSTR sourceTxt, LPCSTR sectionTxt) {
	char fileNameCheck[MAX_PATH];
	strcpy_s(fileNameCheck, MAX_PATH, sourceTxt);
	if (!strchr(fileNameCheck, '.')) {
		strcat_s(fileNameCheck, MAX_PATH, ".txt"); // append extension if missing
	}
	if (!_stricmp(fileNameCheck, fileName)) {
		if (sectionTxt[0] == '#') {
			sectionTxt++;
		}
		LPSTR cmpAddr = LPSTR(heap.getBasePtr()) + 4;
		for (int i = sectionCount; i; i--) {
			if (!_stricmp(cmpAddr, sectionTxt)) {
				return *LPDWORD(cmpAddr - 4);
			}
			while (*cmpAddr++);
			cmpAddr += 4;
		}
	}
	return -1;
}

// global
TextIndex *TxtIndex = (TextIndex *)0x009B7D08;

void text_make_index(LPCSTR sourceTxt) { // 005FE1F0
	for (int i = 0; i < MaxTextIndexNum; i++) {
		if (!TxtIndex[i].getCount()) {
			TxtIndex[i].make_index(sourceTxt);
			break;
		}
	}
}

int text_search_index(LPCSTR sourceTxt, LPCSTR sectionTxt) { // 005FE230
	for (int i = 0; i < MaxTextIndexNum; i++) {
		if (TxtIndex[i].getCount()) {
			int addr = TxtIndex[i].search_index(sourceTxt, sectionTxt);
			if (addr >= 0) {
				return addr;
			}
		}
	}
	return -1;
}

void text_clear_index() { // 005FE270
	for (int i = 0; i < MaxTextIndexNum; i++) {
		if (TxtIndex[i].getCount()) {
			TxtIndex[i].shutdown();
		}
	}
}
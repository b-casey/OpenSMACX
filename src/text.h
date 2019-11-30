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
#include "general.h"

 /*
  * Text class: Handles basic text operations.
  */
class OPENSMACX_API Text {
	char fileName[80];  // (+0)    : stores text filename string
	char filePath[256]; // (+0x50) : stores path of last opened file
	LPSTR currentPos;   // (+0x150)
	FILE *textFile;     // (+0x154)
	LPSTR bufferGet;    // (+0x158)
	LPSTR bufferItem;   // (+0x15C)

public:
	Text() : currentPos(0), textFile(0), bufferGet(0), bufferItem(0) { fileName[0] = 0; 
			filePath[0] = 0; } // 005FD860
	Text(size_t size) : currentPos(0), textFile(0), bufferGet(0), bufferItem(0) { // 005FD880
		fileName[0] = 0; filePath[0] = 0;
		bufferGet = (LPSTR)mem_get(size);
		if (bufferGet) {
			bufferItem = (LPSTR)mem_get(size);
		}
	}
	~Text() { shutdown(); } // 00608C00

	int init(size_t size);
	void shutdown();
	void close();
	BOOL open(LPCSTR srcID, LPCSTR sectionID);
	LPSTR get();
	LPSTR string();
	LPSTR item();
	LPSTR item_string();
	int item_number();
	int item_binary();
	int item_hex();

	// additional functions to assist with encapsulation
	LPSTR update() { currentPos = bufferGet; return bufferGet; }
	LPSTR getFilePath() { return filePath; }
};

// global
extern Text *Txt;
OPENSMACX_API void __cdecl text_txt();
OPENSMACX_API void __cdecl text_txt_exit();
OPENSMACX_API void __cdecl text_close();
OPENSMACX_API BOOL __cdecl text_open(LPCSTR srcID, LPCSTR sectionID);
OPENSMACX_API LPSTR __cdecl text_get();
OPENSMACX_API LPSTR __cdecl text_string();
OPENSMACX_API LPSTR __cdecl text_item();
OPENSMACX_API LPSTR __cdecl text_item_string();
OPENSMACX_API int __cdecl text_item_number();
OPENSMACX_API int __cdecl text_item_binary();
OPENSMACX_API int __cdecl text_item_hex();
OPENSMACX_API int __cdecl text_get_number(int min, int max);
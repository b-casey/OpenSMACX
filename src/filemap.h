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

 /*
  * FileMap class: Handles file IO operations.
  */
class OPENSMACX_API Filemap {
	LPVOID mapViewAddr; // (+0)
	HANDLE hFile;       // (+4)
	HANDLE hFileMap;    // (+8)
	uint32_t fileSize;     // (+12)

public:
	Filemap() : mapViewAddr(NULL), hFile(INVALID_HANDLE_VALUE), hFileMap(NULL),
		fileSize(0) { }      // 00628380
	~Filemap() { close(); }  // 006283E0

	Filemap *init(LPCSTR fileName, BOOL isSequential);
	Filemap *init(LPCSTR fileName);
	LPVOID open_read(LPCSTR fileName, BOOL isSequential);
	LPVOID open(LPCSTR fileName, BOOL isSequential);
	LPVOID create(LPCSTR fileName, uint32_t size, BOOL isSequential);
	void close();
	void close(LPVOID newAddr);

	// additional functions to assist with encapsulation
	uint32_t getSize() { return fileSize; }
};

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
#include "filemap.h"
#include "general.h" // filefind_get

/*
Purpose: Initialize Filemap by opening a file with WRITE access. BOOL toggles if file is accessed
		sequentially or randomly.
Original Offset: 006283A0
Return Value: Pointer to initialized Filemap
Status: Complete
*/
Filemap *Filemap::init(LPCSTR fileName, BOOL isSequential) {
	open(fileName, isSequential);
	return this;
}

/*
Purpose: Initialize Filemap by opening a random access file with WRITE access.
Original Offset: 006283C0
Return Value: Pointer to initialized Filemap
Status: Complete
*/
Filemap *Filemap::init(LPCSTR fileName) {
	open(fileName, false);
	return this;
}

/*
Purpose: Open a file with READ access. BOOL toggles if file is accessed sequentially or randomly.
Original Offset: 00628430
Return Value: Memory address of mapped file or 0 on error
Status: Complete
*/
LPVOID Filemap::open_read(LPCSTR fileName, BOOL isSequential) {
	close();
	LPCSTR filePaths = filefind_get(fileName);
	if (!filePaths) {
		filePaths = fileName;
	}
	hFile = CreateFileA(filePaths, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL
		| (isSequential ? FILE_FLAG_SEQUENTIAL_SCAN : FILE_FLAG_RANDOM_ACCESS), NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		GetLastError(); // failed to read file
		return 0;
	}
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hFileMap) {
		mapViewAddr = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
		if (mapViewAddr) {
			fileSize = GetFileSize(hFile, NULL);
			return mapViewAddr;
		}
	}
	close(); // clear everything on error
	return 0;
}

/*
Purpose: Open a file with WRITE access. BOOL toggles if file is accessed sequentially or randomly.
Original Offset: 00628540
Return Value: Memory address of mapped file or 0 on error
Status: Complete
*/
LPVOID Filemap::open(LPCSTR fileName, BOOL isSequential) {
	close();
	LPCSTR filePaths = filefind_get(fileName);
	if (!filePaths) {
		filePaths = fileName;
	}
	hFile = CreateFileA(filePaths, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL |
		(isSequential ? FILE_FLAG_SEQUENTIAL_SCAN : FILE_FLAG_RANDOM_ACCESS), NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		GetLastError(); // failed to read file
		return 0;
	}
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hFileMap) {
		mapViewAddr = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (mapViewAddr) {
			fileSize = GetFileSize(hFile, NULL);
			return mapViewAddr;
		}
	}
	close(); // clear everything on error
	return 0;
}

/*
Purpose: Create a new file or overwrite an existing one with specified size filled with zeros.
Original Offset: 00628650
Return Value: Memory address of mapped file or 0 on error
Status: Complete
*/
LPVOID Filemap::create(LPCSTR fileName, uint32_t size, BOOL isSequential) {
	close();
	fileSize = size;
	hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL |
		(isSequential ? FILE_FLAG_SEQUENTIAL_SCAN : FILE_FLAG_RANDOM_ACCESS), NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		GetLastError();
		return 0;
	}
	if (SetFilePointer(hFile, size, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER) {
		SetEndOfFile(hFile);
		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER) {
			hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
			if (hFileMap) {
				mapViewAddr = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				if (mapViewAddr) {
					memset(mapViewAddr, 0, size);
					return mapViewAddr;
				}
			}
		}
	}
	close(); // clear everything on error
	return 0;
}

/*
Purpose: Close all handles and map to the file.
Original Offset: 006287C0
Return Value: n/a
Status: Complete
*/
void Filemap::close() {
	if (mapViewAddr) {
		UnmapViewOfFile(mapViewAddr);
		mapViewAddr = 0;
	}
	if (hFileMap) {
		CloseHandle(hFileMap);
		hFileMap = 0;
	}
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		hFile = 0;
	}
	fileSize = 0;
}

/*
Purpose: Close file and set end based on difference between mapViewAddr and newAddr. This can be
		 used to truncate existing tmp files. Assumes file has WRITE access.
Original Offset: 00628810
Return Value: n/a
Status: Complete
*/
void Filemap::close(LPVOID newAddr) {
	if (newAddr >= mapViewAddr) {
		LONG newSize = LONG(newAddr) - LONG(mapViewAddr);
		if (mapViewAddr) {
			UnmapViewOfFile(mapViewAddr);
			mapViewAddr = 0;
		}
		if (hFileMap) {
			CloseHandle(hFileMap);
			hFileMap = 0;
		}
		if (SetFilePointer(hFile, newSize, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER) {
			SetEndOfFile(hFile);
			if (hFile) {
				CloseHandle(hFile);
				hFile = 0;
			}
			return;
		}
	}
	close(); // clear everything on error
}
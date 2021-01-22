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
#include "filemap.h"
#include "general.h"

/*
Purpose: Initialize the class by opening a file with write permission. The boolean parameter toggles 
         if the file is accessed sequentially or randomly.
Original Offset: 006283A0
Return Value: Pointer to the initialized class
Status: Complete
*/
Filemap *Filemap::init(LPCSTR file_name, BOOL is_sequential) {
    open(file_name, is_sequential);
    return this;
}

/*
Purpose: Initialize the class by opening a random access file with write permission.
Original Offset: 006283C0
Return Value: Pointer to initialized class
Status: Complete
*/
Filemap *Filemap::init(LPCSTR file_name) {
    open(file_name, false);
    return this;
}

/*
Purpose: Open a file with read permission. The boolean parameter toggles if the file is accessed 
         sequentially or randomly.
Original Offset: 00628430
Return Value: Pointer to the mapped file or NULL on error
Status: Complete
*/
LPVOID Filemap::open_read(LPCSTR file_name, BOOL is_sequential) {
    close();
    LPCSTR file_paths = filefind_get(file_name);
    if (!file_paths) {
        file_paths = file_name;
    }
    file_ = CreateFileA(file_paths, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL
        | (is_sequential ? FILE_FLAG_SEQUENTIAL_SCAN : FILE_FLAG_RANDOM_ACCESS), NULL);
    if (file_ == INVALID_HANDLE_VALUE) {
        GetLastError(); // failed to read file
        return NULL;
    }
    file_map_ = CreateFileMapping(file_, NULL, PAGE_READONLY, 0, 0, NULL);
    if (file_map_) {
        map_view_addr_ = MapViewOfFile(file_map_, FILE_MAP_READ, 0, 0, 0);
        if (map_view_addr_) {
            file_size_ = GetFileSize(file_, NULL);
            return map_view_addr_;
        }
    }
    close(); // clear everything on error
    return NULL;
}

/*
Purpose: Open a file with write permission. The boolean parameter toggles if the file is accessed
         sequentially or randomly.
Original Offset: 00628540
Return Value: Pointer to the mapped file or NULL on error
Status: Complete
*/
LPVOID Filemap::open(LPCSTR file_name, BOOL is_sequential) {
    close();
    LPCSTR file_paths = filefind_get(file_name);
    if (!file_paths) {
        file_paths = file_name;
    }
    file_ = CreateFileA(file_paths, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL |
        (is_sequential ? FILE_FLAG_SEQUENTIAL_SCAN : FILE_FLAG_RANDOM_ACCESS), NULL);
    if (file_ == INVALID_HANDLE_VALUE) {
        GetLastError(); // failed to read file
        return NULL;
    }
    file_map_ = CreateFileMapping(file_, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (file_map_) {
        map_view_addr_ = MapViewOfFile(file_map_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (map_view_addr_) {
            file_size_ = GetFileSize(file_, NULL);
            return map_view_addr_;
        }
    }
    close(); // clear everything on error
    return NULL;
}

/*
Purpose: Create a new file or overwrite an existing one with the specified size filled with zeros.
Original Offset: 00628650
Return Value: Pointer to the mapped file or NULL on error
Status: Complete
*/
LPVOID Filemap::create(LPCSTR file_name, uint32_t size, BOOL is_sequential) {
    close();
    file_size_ = size;
    file_ = CreateFileA(file_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL |
        (is_sequential ? FILE_FLAG_SEQUENTIAL_SCAN : FILE_FLAG_RANDOM_ACCESS), NULL);
    if (file_ == INVALID_HANDLE_VALUE) {
        GetLastError();
        return NULL;
    }
    if (SetFilePointer(file_, size, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER) {
        SetEndOfFile(file_);
        if (SetFilePointer(file_, 0, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER) {
            file_map_ = CreateFileMapping(file_, NULL, PAGE_READWRITE, 0, 0, NULL);
            if (file_map_) {
                map_view_addr_ = MapViewOfFile(file_map_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
                if (map_view_addr_) {
                    ZeroMemory(map_view_addr_, size);
                    return map_view_addr_;
                }
            }
        }
    }
    close(); // clear everything on error
    return NULL;
}

/*
Purpose: Close the map and all handles to the file.
Original Offset: 006287C0
Return Value: n/a
Status: Complete
*/
void Filemap::close() {
    if (map_view_addr_) {
        UnmapViewOfFile(map_view_addr_);
        map_view_addr_ = 0;
    }
    if (file_map_) {
        CloseHandle(file_map_);
        file_map_ = 0;
    }
    if (file_ != INVALID_HANDLE_VALUE) {
        CloseHandle(file_);
        file_ = 0;
    }
    file_size_ = 0;
}

/*
Purpose: Close and set the end of the file. This can be used to truncate existing files. It also 
         assumes the file has write permission.
Original Offset: 00628810
Return Value: n/a
Status: Complete
*/
void Filemap::close(LPVOID new_addr) {
    if (new_addr >= map_view_addr_) {
        LONG new_size = LONG(new_addr) - LONG(map_view_addr_);
        if (map_view_addr_) {
            UnmapViewOfFile(map_view_addr_);
            map_view_addr_ = 0;
        }
        if (file_map_) {
            CloseHandle(file_map_);
            file_map_ = 0;
        }
        if (SetFilePointer(file_, new_size, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER) {
            SetEndOfFile(file_);
            if (file_) {
                CloseHandle(file_);
                file_ = 0;
            }
            return;
        }
    }
    close(); // clear everything on error
}

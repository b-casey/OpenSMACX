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
#include "textindex.h"
#include "general.h"
#include "filemap.h"

/*
Purpose: Read the specified file and create an index of section headers ("#EXAMPLE"). Store the file 
         offset of each section for improved search time.
Original Offset: 005FDF80
Return Value: n/a
Status: Complete
*/
void TextIndex::make_index(LPCSTR source_txt) {
    Filemap txt_file_map;
    shutdown();
    strcpy_s(file_name_, 256, source_txt);
    if (!strchr(file_name_, '.')) {
        strcat_s(file_name_, 256, ".txt"); // append extension if missing
    }
    LPVOID txt_addr_base = txt_file_map.open_read(file_name_, false);
    if (txt_addr_base) {
        LPVOID seek_addr = txt_addr_base;
        LPVOID eof_addr = LPVOID(uint32_t(txt_addr_base) + txt_file_map.get_size());
        heap_.init(0x8000); // maximum amount of memory per section index
        while (seek_addr < eof_addr) {
            LPVOID found_addr = memchr(seek_addr, '\n', uint32_t(eof_addr) - uint32_t(seek_addr));
            if (!found_addr) {
                break; // invalid file, no newlines
            }
            char parse_buffer[512]; // maximum line length
            memcpy_s(parse_buffer, 512, seek_addr, uint32_t(found_addr) - uint32_t(seek_addr) + 1);
            kill_nl(parse_buffer); // bug fix; Filemap reads in raw file including carriage return
            purge_spaces(parse_buffer);
            if (parse_buffer[0] == '#' && isupper(parse_buffer[1])) {
                size_t len = strlen(&parse_buffer[1]) + 1;
                LPVOID store_addr = heap_.get(len + 4); // section name + address
                *(LPDWORD)store_addr = uint32_t(seek_addr) - uint32_t(txt_addr_base); // file offset
                strcpy_s(LPSTR(store_addr) + 4, len, &parse_buffer[1]);
                section_count_++;
            }
            seek_addr = LPVOID(uint32_t(found_addr) + 1);
        }
        heap_.squeeze(true);
    }
}

/*
Purpose: Search the source text file for a section header.
Original Offset: 005FE120
Return Value: File offset if found, otherwise -1
Status: Complete
*/
int TextIndex::search_index(LPCSTR source_txt, LPCSTR section_txt) {
    char file_name_check[MAX_PATH];
    strcpy_s(file_name_check, MAX_PATH, source_txt);
    if (!strchr(file_name_check, '.')) {
        strcat_s(file_name_check, MAX_PATH, ".txt"); // append extension if missing
    }
    if (!_stricmp(file_name_check, file_name_)) {
        if (section_txt[0] == '#') {
            section_txt++;
        }
        LPSTR cmp_addr = LPSTR(heap_.get_base()) + 4;
        for (int i = section_count_; i; i--) {
            if (!_stricmp(cmp_addr, section_txt)) {
                return *LPDWORD(cmp_addr - 4);
            }
            while (*cmp_addr++);
            cmp_addr += 4;
        }
    }
    return -1;
}

// global
TextIndex *TxtIndex = (TextIndex *)0x009B7D08;

void __cdecl text_make_index(LPCSTR source_txt) { // 005FE1F0
    for (int i = 0; i < MaxTextIndexNum; i++) {
        if (!TxtIndex[i].get_count()) {
            TxtIndex[i].make_index(source_txt);
            break;
        }
    }
}

int __cdecl text_search_index(LPCSTR source_txt, LPCSTR section_txt) { // 005FE230
    for (int i = 0; i < MaxTextIndexNum; i++) {
        if (TxtIndex[i].get_count()) {
            int addr = TxtIndex[i].search_index(source_txt, section_txt);
            if (addr >= 0) {
                return addr;
            }
        }
    }
    return -1;
}

void __cdecl text_clear_index() { // 005FE270
    for (int i = 0; i < MaxTextIndexNum; i++) {
        if (TxtIndex[i].get_count()) {
            TxtIndex[i].shutdown();
        }
    }
}

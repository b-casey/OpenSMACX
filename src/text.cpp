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
#include "text.h"
#include "general.h"
#include "log.h"
#include "strings.h"
#include "textindex.h"

/*
Purpose: Initialize the class instance.
Original Offset: 005FD8D0
Return Value: Zero on success, non-zero on error
Status: Complete
*/
int Text::init(size_t size) {
    shutdown();
    buffer_get_ = (LPSTR)mem_get(size);
    if (!buffer_get_) {
        return 4;
    }
    buffer_item_ = (LPSTR)mem_get(size);
    if (!buffer_item_) {
        return 4;
    }
    return 0;
}

/*
Purpose: Shutdown the class instance.
Original Offset: 005FD970
Return Value: n/a
Status: Complete
*/
void Text::shutdown() {
    close();
    if (buffer_get_) {
        free(buffer_get_);
        buffer_get_ = 0;
    }
    if (buffer_item_) {
        free(buffer_item_);
        buffer_item_ = 0;
    }
}

/*
Purpose: If open, close the text file.
Original Offset: 005FD9D0
Return Value: n/a
Status: Complete
*/
void Text::close() {
    if (text_file_) {
        fclose(text_file_);
        text_file_ = 0;
    }
}

/*
Purpose: Open the specified text file and copy the section into the buffer for parsing.
Original Offset: 005FDA00
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL Text::open(LPCSTR src_file_id, LPCSTR section_id) {
    BOOL is_file_open = false; // open, possibly tracker for reading through whole file?
    if (src_file_id) {
        strcpy_s(file_name_, 80, src_file_id);
        if (!strchr(file_name_, '.')) {
            strcat_s(file_name_, 80, ".txt");
        }
        close();
        text_file_ = env_open(file_name_, "rt");
        if (!text_file_) {
            return true;
        }
        strcpy_s(file_path_, 256, FilefindPath->last_path);
    } else if (text_file_) {
        is_file_open = true;
    } else {
        text_file_ = env_open(file_name_, "rt");
        if (!text_file_) {
            return true;
        }
        strcpy_s(file_path_, 256, FilefindPath->last_path);
    }
    if (!section_id) {
        return false;
    }
    int seek_addr = text_search_index(file_name_, section_id);
    if (seek_addr >= 0) {
        log_say("Seeking to", section_id, seek_addr, 0, 0);
        fseek(text_file_, seek_addr, SEEK_SET);
        is_file_open = true;
    }
    std::string sect_header("#");
    sect_header += section_id;
    do {
        if (feof(text_file_)) {
            if (is_file_open) {
                is_file_open = false;
                rewind(text_file_);
            } else {
                close();
                return true;
            }
        }
        if (!fgets(buffer_get_, 511, text_file_)) {
            close();
            return true;
        }
        kill_lf(buffer_get_);
        purge_spaces(buffer_get_);
    } while (_stricmp(sect_header.c_str(), buffer_get_));
    current_pos_ = buffer_get_;
    return false;
}

/*
Purpose: Parse text from the opened file until a newline is reached. Copy this text into the buffer.
Original Offset: 005FDC10
Return Value: Pointer to string
Status: Complete
*/
LPSTR Text::get() {
    if (feof(text_file_)) {
        buffer_get_[0] = 0;
        return NULL;
    }
    if (fgets(buffer_get_, 511, text_file_)) {
        kill_lf(buffer_get_);
        purge_spaces(buffer_get_);
        current_pos_ = buffer_get_;
    } else {
        buffer_get_[0] = 0;
    }
    return buffer_get_;
}

/*
Purpose: Get the string and put it into the string table.
Original Offset: 005FDC80
Return Value: Pointer to string table
Status: Complete
*/
LPSTR Text::string() {
    return StringTable->put(get());
}

/*
Purpose: Parse the current item into the string buffer.
Original Offset: 005FDD10
Return Value: Pointer to string buffer
Status: Complete
*/
LPSTR Text::item() {
    LPSTR parse = buffer_item_;
    while (*current_pos_ != 0 && *current_pos_ != ',') {
        *parse++ = *current_pos_++;
    }
    *parse = 0;
    if (*current_pos_) {
        *current_pos_++;
    }
    purge_spaces(buffer_item_);
    return buffer_item_;
}

/*
Purpose: Parse the current string item into the buffer and add it to the string table.
Original Offset: 005FDD80
Return Value: Pointer to string table
Status: Complete
*/
LPSTR Text::item_string() {
    return StringTable->put(item());
}

/*
Purpose: Parse the current number item.
Original Offset: 005FDDF0
Return Value: Integer value of the number item
Status: Complete
*/
int Text::item_number() {
    return stoi(item());
}

/*
Purpose: Parse the current binary item.
Original Offset: 005FDE60
Return Value: Integer value of the binary item
Status: Complete
*/
int Text::item_binary() {
    return btoi(item());
}

/*
Purpose: Parse the current hex item.
Original Offset: 005FDED0
Return Value: Integer value of the hex item
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

void __cdecl text_set_get_ptr() { *TextBufferGetPtr = Txt->get_buffer_get(); } // 005FD4C0

void __cdecl text_set_item_ptr() { *TextBufferItemPtr = Txt->get_buffer_item(); } // 005FD4D0

void __cdecl text_close() { Txt->close(); } // 005FD530

BOOL __cdecl text_open(LPCSTR src_id, LPCSTR section_id) { // 005FD550
    return Txt->open(src_id, section_id);
}

LPSTR __cdecl text_get() { return Txt->get(); } // 005FD570

LPSTR __cdecl text_string() { return Txt->string(); } // 005FD5E0

LPSTR __cdecl text_item() { return Txt->item(); } // 005FD670

LPSTR __cdecl text_item_string() { return Txt->item_string(); } // 005FD6D0

int __cdecl text_item_number() { return Txt->item_number(); } // 005FD740

int __cdecl text_item_binary() { return Txt->item_binary(); } // 005FD7A0

int __cdecl text_item_hex() { return Txt->item_hex(); } // 005FD800

int __cdecl text_get_number(int min, int max) { // 00585120
    text_get(); 
    return range(text_item_number(), min, max);
}

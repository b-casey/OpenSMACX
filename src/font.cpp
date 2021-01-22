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
#include "temp.h" // BufferStrHeight
#include "font.h"
#include "general.h"

HDC Font::FontHDC;
int Font::FontInitCount;

/*
Purpose: Initialize the class using the font name, height and style.
Original Offset: 00618F40
Return Value: Zero on success, non-zero on error
Status: Complete
*/
int Font::init(LPCSTR font_name, int height, uint32_t style) {
    if (!font_name) {
        return 3;
    }
    if (is_fot_set_ & 1) {
        line_height_ = 0;
        height_ = 0;
        ascent_ = 0;
        descent_ = 0;
        if (font_obj_) {
            DeleteObject(font_obj_);
            font_obj_ = 0;
        }
    } else {
        close();
    }
    LOGFONT lf;
    lf.lfHeight = -height;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfUnderline = (style >> 2) & 1;
    lf.lfOrientation = 0;
    lf.lfWeight = (style & 1) ? 700 : 0;
    lf.lfItalic = (style >> 1) & 1;
    lf.lfStrikeOut = 0;
    lf.lfCharSet = 0;
    lf.lfOutPrecision = 7;
    lf.lfClipPrecision = 0;
    lf.lfQuality = 0;
    lf.lfPitchAndFamily = 0;
    strcpy_s(lf.lfFaceName, 32, font_name);
    font_obj_ = CreateFontIndirectA(&lf);
    if (!font_obj_) {
        return 13;
    }
    SelectObject(FontHDC, font_obj_);
    TEXTMETRIC tm;
    GetTextMetricsA(FontHDC, &tm);
    internal_leading_ = tm.tmInternalLeading;
    line_height_ = tm.tmHeight + tm.tmExternalLeading;
    ascent_ = tm.tmAscent;
    height_ = ascent_ - internal_leading_;
    descent_ = tm.tmDescent;
    SelectObject(FontHDC, GetStockObject(SYSTEM_FONT));
    return 0;
}

/*
Purpose: Initialize the class using the file, font name, height and style.
Original Offset: 006190D0
Return Value: Zero on success, non-zero on error
Status: Complete
*/
int Font::init(LPCSTR file, LPCSTR font_name, int height, uint32_t style) {
    close();
    if (!file || !font_name) {
        return 16;
    }
    size_t len = strlen(file) + 1;
    fot_file_name_ = (LPSTR)mem_get(len);
    if (!fot_file_name_) {
        return 4;
    }
    strcpy_s(fot_file_name_, len, file);
    fot_file_name_[len - 5] = 0; // font extension length + 1
    strcat_s(fot_file_name_, len, ".fot");
    char path[MAX_PATH + 1];
    GetCurrentDirectoryA(MAX_PATH, path);
    CreateScalableFontResourceA(0, fot_file_name_, file, path);
    if (!AddFontResourceA(fot_file_name_)) {
        GetLastError();
        return 1;
    }
    PostMessageA(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    is_fot_set_ |= 1;
    return init(font_name, height, style);
}

/*
Purpose: Close an instance of the class.
Original Offset: 00619230
Return Value: n/a
Status: Complete
*/
void Font::close() {
    unk_1_ = -1;
    line_height_ = 0;
    height_ = 0;
    ascent_ = 0;
    descent_ = 0;
    if (font_obj_) {
        DeleteObject(font_obj_);
        font_obj_ = 0;
    }
    if (fot_file_name_) {
        RemoveFontResourceA(fot_file_name_);
        free(fot_file_name_); // removed 2nd redundant check if fot_file_name_ isn't null
        fot_file_name_ = 0;
    }
}

/*
Purpose: Get the width for the input text.
Original Offset: 00619280
Return Value: Width otherwise zero on error
Status: Complete
*/
int Font::width(LPSTR input) {
    if (!input) {
        return 0;
    }
    SelectObject(FontHDC, font_obj_);
    SIZE size;
    GetTextExtentPoint32A(FontHDC, input, strlen(input), &size);
    SelectObject(FontHDC, GetStockObject(SYSTEM_FONT));
    return size.cx;
}

/*
Purpose: Get the width for the input text with a maximum length.
Original Offset: 006192F0
Return Value: Width otherwise zero on error
Status: Complete
*/
int Font::width(LPSTR input, size_t max_len) {
    if (!input) {
        return 0;
    }
    size_t len = strlen(input);
    if (len > max_len) {
        len = max_len;
    }
    SelectObject(FontHDC, font_obj_);
    SIZE size;
    GetTextExtentPoint32A(FontHDC, input, len, &size);
    SelectObject(FontHDC, GetStockObject(SYSTEM_FONT));
    return size.cx;
}

/*
Purpose: Find a space in the input string that can be used as a natural line break.
Original Offset: 00619370
Return Value: Pointer to string section after the line break or NULL if not found
Status: Complete
*/
LPSTR Font::find_line_break_l(LPSTR input, int *break_len, size_t len) {
    int search_brk = *break_len;
    LPSTR search_str = input;
    if (!len) {
        return NULL;
    }
    do {
        LPSTR space = (LPSTR)memchr(search_str + 1, ' ', len);
        if (!space) {
            search_brk -= width(search_str, len);
            if (search_brk < 0) {
                *break_len = 0;
                if (search_str != input) {
                    return search_str + 1;
                }
                return *BufferStrHeight ? search_str : NULL;
            }
            break;
        }
        search_brk -= width(search_str, space - search_str);
        if (search_brk < 0) {
            *break_len = 0;
            if (search_str != input) {
                return search_str + 1;
            }
            return *BufferStrHeight ? search_str : space + 1;
        }
        len += search_str - space;
        search_str = space;
    } while (len);
    *BufferStrHeight += *break_len - search_brk;
    *break_len = search_brk;
    return NULL;
}

/*
Purpose: Initialize the class static variables.
Original Offset: 006195B0
Return Value: Zero on success (or already initialized), non-zero on error
Status: Complete
*/
int __cdecl Font::init_font_class(Font *font) {
    if (++FontInitCount > 1) {
        return 0;
    }
    if (!font) {
        return 3;
    }
    FontHDC = CreateCompatibleDC(NULL);
    if (!FontHDC) {
        return 2;
    }
    *FontDefaultPtr = font;
    if (font->font_obj_) {
        return 0;
    }
    return font->init("Times New Roman", 12, 0);
}

/*
Purpose: Shutdown the class static variables.
Original Offset: 00619610
Return Value: n/a
Status: Complete
*/
void __cdecl Font::close_font_class() {
    if (--FontInitCount <= 0) {
        if (FontHDC) {
            DeleteDC(FontHDC);
            FontHDC = 0;
        }
        *FontDefaultPtr = NULL;
    }
}

// global
Font **FontDefaultPtr = (Font **)0x009BB484;

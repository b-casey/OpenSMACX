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
#include "stdafx.h"
#include "font.h"
#include "general.h" // mem_get()
#include "temp.h" // BufferStrHeight

Font *Font::FontDefaultPtr;
HDC Font::FontHDC;
int Font::FontInitCount;

/*
Purpose: Initialize instance of Font using font name.
Original Offset: 00618F40
Return Value: Non-zero error; Zero successful
Status: Complete - test pending
*/
int Font::init(LPCSTR fontName, int heightInit, uint32_t style) {
	if (!fontName) {
		return 3;
	}
	if (toggle & 1) {
		lineHeight = 0;
		height = 0;
		ascent = 0;
		descent = 0;
		if (fontObj) {
			DeleteObject(fontObj);
			fontObj = 0;
		}
	}
	else {
		close();
	}
	LOGFONT lf;
	lf.lfHeight = -heightInit;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfUnderline = (style >> 2) & 1;
	lf.lfOrientation = 0;
	lf.lfWeight = (style & 1) != 0 ? 700 : 0;
	lf.lfItalic = (style >> 1) & 1;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = 0;
	lf.lfOutPrecision = 7;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;
	strcpy_s(lf.lfFaceName, 32, fontName);
	fontObj = CreateFontIndirectA(&lf);
	if (!fontObj) {
		return 13;
	}
	SelectObject(FontHDC, fontObj);
	TEXTMETRIC tm;
	GetTextMetricsA(FontHDC, &tm);
	internalLeading = tm.tmInternalLeading;
	lineHeight = tm.tmHeight + tm.tmExternalLeading;
	ascent = tm.tmAscent;
	height = ascent - internalLeading;
	descent = tm.tmDescent;
	SelectObject(FontHDC, GetStockObject(SYSTEM_FONT));
	return 0;
}

/*
Purpose: Initialize instance of Font using font name and file.
Original Offset: 006190D0
Return Value: Non-zero error; Zero successful
Status: Complete - test pending
*/
int Font::init(LPCSTR file, LPCSTR fontName, int heightInit, uint32_t style) {
	close();
	if (!file || !fontName) {
		return 16;
	}
	size_t len = strlen(file) + 1;
	fotFileName = (LPSTR)mem_get(len);
	if (!fotFileName) {
		return 4;
	}
	strcpy_s(fotFileName, len, file);
	fotFileName[len - 5] = 0; // ?
	strcat_s(fotFileName, len, ".fot");
	char path[MAX_PATH + 1];
	GetCurrentDirectoryA(MAX_PATH, path);
	CreateScalableFontResourceA(0, fotFileName, file, path);
	if (!AddFontResourceA(fotFileName)) {
		GetLastError();
		return 1;
	}
	PostMessageA(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	toggle |= 1;
	return init(fontName, heightInit, style);
}

/*
Purpose: Shutdown instance of Font.
Original Offset: 00619230
Return Value: n/a
Status: Complete - test pending
*/
void Font::close() {
	unk1 = -1;
	lineHeight = 0;
	height = 0;
	ascent = 0;
	descent = 0;
	if (fontObj) {
		DeleteObject(fontObj);
		fontObj = 0;
	}
	if (fotFileName) {
		RemoveFontResourceA(fotFileName);
		free(fotFileName); // removed 2nd redundant check if fotFileName isn't null
		fotFileName = 0;
	}
}

/*
Purpose: Get width for input text.
Original Offset: 00619280
Return Value: Width, otherwise zero if error
Status: Complete - test pending
*/
int Font::width(LPSTR input) {
	if (!input) {
		return 0;
	}
	SelectObject(FontHDC, fontObj);
	SIZE sz;
	GetTextExtentPoint32A(FontHDC, input, strlen(input), &sz);
	SelectObject(FontHDC, GetStockObject(SYSTEM_FONT));
	return sz.cx;
}

/*
Purpose: Get width for input text with minimum length.
Original Offset: 006192F0
Return Value: Width, otherwise zero if error
Status: Complete - test pending
*/
int Font::width(LPSTR input, int maxLen) {
	if (!input) {
		return 0;
	}
	int len = strlen(input);
	if (len >= maxLen) {
		len = maxLen;
	}
	SelectObject(FontHDC, fontObj);
	SIZE sz;
	GetTextExtentPoint32A(FontHDC, input, len, &sz);
	SelectObject(FontHDC, GetStockObject(SYSTEM_FONT));
	return sz.cx;
}

/*
Purpose: Find line break length.
Original Offset: 00619370
Return Value: Length
Status: Complete - test pending
*/
LPSTR Font::find_line_break_l(LPSTR input, int *breakLen, size_t len) {
	int searchBrk = *breakLen;
	LPSTR searchStr = input;
	if (!len) {
		return 0;
	}
	do {
		LPSTR space = (LPSTR)memchr(searchStr + 1, ' ', len);
		if (!space) {
			break;
		}
		searchBrk -= width(searchStr, space - searchStr);
		if (input != space) {
			if (searchBrk < 0) {
				*breakLen = 0;
				return searchStr + 1;
			}
		}
		else {
			if (searchBrk < 0) {
				if (!BufferStrHeight) {
					*breakLen = 0;
					return space + 1;
				}
				else {
					*breakLen = 0;
					return input;
				}
			}
		}
	} while (true);
	searchBrk -= width(searchStr, len);
	if (searchBrk < 0) {
		*breakLen = 0;
		if (input != searchStr) {
			return searchStr + 1;
		}
		return *BufferStrHeight ? searchStr : 0;
	}
	BufferStrHeight += *breakLen - searchBrk;
	*breakLen = searchBrk;
	return 0;
}

/*
Purpose: Initialize Font class static variables.
Original Offset: 006195B0
Return Value: Non-zero error; Zero successful (or already initalized)
Status: Complete - test pending
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
	FontDefaultPtr = font;
	if (font->fontObj) {
		return 0;
	}
	return font->init("Times New Roman", 12, 0);
}

/*
Purpose: Shutdown Font class static variables.
Original Offset: 00619610
Return Value: n/a
Status: Complete - test pending
*/
void __cdecl Font::close_font_class() {
	if (--FontInitCount <= 0) {
		if (FontHDC) {
			DeleteDC(FontHDC);
			FontHDC = 0;
		}
		FontDefaultPtr = 0;
	}
}

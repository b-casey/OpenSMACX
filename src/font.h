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

 /*
  * Font class
  */
class OPENSMACX_API Font {
	int unk1; // height offset?
	int toggle; // boolean? system font? already active?
	HFONT fontObj;
	int lineHeight;
	int height;
	int internalLeading;
	int ascent;
	int descent;
	int pad; // padding?
	LPSTR fotFileName;

public:
	Font() : unk1(-1), toggle(0), fontObj(0), lineHeight(0), height(0), ascent(0), descent(0), 
		fotFileName(0)  { } // 00618EA0
	Font(LPSTR fontName, int height, int style) { init(fontName, height, style); } // 00618EC0
	~Font() { close(); } // 00618EE0

	// int UNK1(int, int, int, int) { return 1; }
	int init(LPCSTR fontName, int height, uint32_t style);
	int init(LPCSTR lpszFile, LPCSTR fontName, int height, uint32_t style);
	void close();
	int width(LPSTR input);
	int width(LPSTR input, int maxLen);
	LPSTR find_line_break_l(LPSTR input, int *breakLen, size_t len);

	// eventually make atomic for thread safety
	static Font *FontDefaultPtr;
	static HDC FontHDC;
	static int FontInitCount;
	static int __cdecl init_font_class(Font *font);
	static void __cdecl close_font_class();
};
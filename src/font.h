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
  * Font class
  */
class OPENSMACX_API Font {
	int unk_1; // height offset? set outside of class functions
	BOOL isFotSet; // used only by both init() functions
	HFONT fontObj;
	int lineHeight;
	int height;
	int internalLeading;
	int ascent;
	int descent;
	int pad; // padding? no references
	LPSTR fotFileName;

public:
	Font() : unk_1(-1), isFotSet(0), fontObj(0), lineHeight(0), height(0), ascent(0), descent(0),
		fotFileName(0) { } // 00618EA0
	Font(LPSTR fontName, int lfHeight, int style) { init(fontName, lfHeight, style); } // 00618EC0
	~Font() { close(); } // 00618EE0

	// int UNK1(int, int, int, int) { return 1; } // no direct references
	int init(LPCSTR fontName, int lfHeight, uint32_t style);
	int init(LPCSTR file, LPCSTR fontName, int lfHeight, uint32_t style);
	void close();
	int width(LPSTR input);
	int width(LPSTR input, int maxLen);
	LPSTR find_line_break_l(LPSTR input, int *breakLen, size_t len);

	// eventually make atomic for thread safety
	static HDC FontHDC;
	static int FontInitCount;
	static int __cdecl init_font_class(Font *font);
	static void __cdecl close_font_class();
};

// global
extern Font **FontDefaultPtr;

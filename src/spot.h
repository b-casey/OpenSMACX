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
  * Spot class
  */
class OPENSMACX_API Spot {
	struct SpotInternal {
		RECT rect;
		int type;
		int position;
	};

	SpotInternal *spots;
	uint32_t maxCount;
	uint32_t addCount;

public:
	Spot() : spots(0), maxCount(0), addCount(0) { } // 005FA860
	~Spot() { shutdown(); }                         // 005FA870

	void clear() { spots = 0;  maxCount = 0; addCount = 0; } // 005FA820
	void shutdown();
	void init(uint32_t count);
	void replace(int position, int type, int left, int top, int length, int width);
	int add(int position, int type, int left, int top, int length, int width);
	int add(int position, int type, RECT *rect);
	void kill_pos(int position);
	void kill_specific(int position, int type);
	void kill_type(int type);
	int check(int xCoord, int yCoord, int *spotPos, int *spotType);
	int check(int xCoord, int yCoord, int *spotPos, int *spotType, RECT *spotRect);
	int get_rect(int position, int type, RECT *spotRect);
};

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
  * Path related objects, variables and functions.
  */
class OPENSMACX_API Path {
private:
	int *mapTable;
	int16_t *xCoordTable;
	int16_t *yCoordTable;
	int index1; // specific territory count
	int index2; // overall territory count
	int factionID1;
	int xCoordDst;
	int yCoordDst;
	int field_20;
	int factionID2;
	int protoID;

public:
	Path() : mapTable(0), xCoordTable(0), yCoordTable(0), index1(0), index2(0), factionID1(0), 
		xCoordDst(0), yCoordDst(0), field_20(5), factionID2(0), protoID(0) { } // n/a
	~Path() { shutdown(); } // 0059A320

	void init();
	void shutdown();
	int zoc_path(int xCoord, int yCoord, int factionID);
	int find(int xCoordSrc, int yCoordSrc, int xCoordDstA, int yCoordDstA, int protoID_,
		int factionID, int unk1, int unk2);
	int move(int vehID, int factionID);
	void make_abstract();
	void replace(uint32_t regionOld, uint32_t regionNew);
	void territory(int xCoord, int yCoord, int region, int factionID);
	void continent(int xCoord, int yCoord, uint32_t region);
	void continents();
	BOOL sensors(int factionID, int *xCoordPtr, int *yCoordPtr);
};
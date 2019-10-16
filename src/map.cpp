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
#include "map.h"
#include "base.h"
#include "veh.h"
#include "temp.h"

rules_natural *Natural = (rules_natural *)0x0094ADE0;
uint32_t *MapHorizontalBounds = (uint32_t *)0x00949870;
uint32_t *MapVerticalBounds = (uint32_t *)0x00949874;
int *MapSeaLevel = (int *)0x0094987C;
uint32_t *MapArea = (uint32_t *)0x00949884;
uint32_t *MapAreaSqRoot = (uint32_t *)0x00949888;
BOOL *MapFlatToggle = (BOOL *)0x0094988C;
uint32_t *MapLandmarkCount = (uint32_t *)0x00949890;
landmark *MapLandmark = (landmark *)0x00949894; // [64]
uint32_t *MapHorizontal = (uint32_t *)0x0068FAF0;
map **Map = (map **)0x0094A30C;

/*
Purpose: Check whether coordinates are on the map.
Original Offset: 004712A0
Return Value: true if on map, otherwise false
Status: Complete
*/
BOOL __cdecl on_map(int xCoord, int yCoord) {
	return yCoord >= 0 && yCoord < (int)*MapVerticalBounds 
		&& xCoord >= 0 && xCoord < (int)*MapHorizontalBounds;
}

/*
Purpose: Calculate distance between two points.
Original Offset: 004F8090
Return Value: Distance
Status: Complete
*/
int __cdecl x_dist(int xCoord, int yCoord) {
	int absX = abs(xCoord);
	int absY = abs(yCoord);
	int largest = absX;
	if (absX <= absY) {
		largest = absY;
	}
	int smallest = absX;
	if (absX >= absY) {
		smallest = absY;
	}
	return largest - ((((absY + absX) >> 1) - smallest + 1) >> 1);
}

/*
Purpose: Get map tile based on coords. Optimized out of original code, helps reduce code complexity.
Original Offset: n/a
Return Value: Pointer to map tile
Status: Complete
*/
map * __cdecl map_loc(int xCoord, int yCoord) {
	return &((*Map)[((xCoord >> 1) + yCoord * *MapHorizontal)]);
}

/*
Purpose: Get temperature of tile at coordinates.
Original Offset: n/a
Return Value: Temperature
Status: Complete
*/
uint32_t __cdecl temp_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->val1 & 7;
}

/*
Purpose: Set temperature of tile at coordinates.
Original Offset: 00591AD0
Return Value: n/a
Status: Complete
*/
void __cdecl temp_set(int xCoord, int yCoord, uint8_t temperature) {
	map *tile = map_loc(xCoord, yCoord);
	tile->val1 &= 0xF8;
	tile->val1 |= temperature & 7;
}

/*
Purpose: Get climate of tile at coordinates.
Original Offset: n/a
Return Value: Climate
Status: Complete
*/
uint32_t __cdecl climate_at(int xCoord, int yCoord) {
	return (map_loc(xCoord, yCoord)->val1 >> 3) & 3;
}

/*
Purpose: Set climate of tile at coordinates.
Original Offset: 00591A80
Return Value: n/a
Status: Complete
*/
void __cdecl climate_set(int xCoord, int yCoord, uint8_t climate) {
	map *tile = map_loc(xCoord, yCoord);
	tile->val1 &= 0xE7;
	tile->val1 |= (climate & 3) << 3;
	tile->bit2 |= 0x400000; // TODO: identify value
	*UnkBitfield1 |= 1; // TODO: identify global + value
}

/*
Purpose: Get altitude of tile at coordinates.
Original Offset: 00500150
Return Value: Altitude
Status: Complete
*/
uint32_t __cdecl alt_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->val1 >> 5;
}

/*
Purpose: Get altitude details of tile at coordinates.
Original Offset: 00500180
Return Value: Altitude detail
Status: Complete
*/
uint32_t __cdecl alt_detail_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->altDetail;
}

/*
Purpose: Set altitude details of tile at coordinates.
Original Offset: 00591260
Return Value: n/a
Status: Complete
*/
void __cdecl alt_put_detail(int xCoord, int yCoord, uint8_t detail) {
	map_loc(xCoord, yCoord)->altDetail = detail;
}

/*
Purpose: Get faction owner of tile at coordinates.
Original Offset: n/a
Return Value: factionID
Status: Complete
*/
uint32_t __cdecl owner_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->val2 & 0xF;
}

/*
Purpose: Set faction owner of tile at coordinates.
Original Offset: 00591B10
Return Value: n/a
Status: Complete
*/
void __cdecl owner_set(int xCoord, int yCoord, int factionID) {
	map *tile = map_loc(xCoord, yCoord);
	tile->val2 &= 0xF0;
	tile->val2 |= factionID & 0xF;
}

/*
Purpose: Set site of tile at coordinates.
Original Offset: 00591B50
Return Value: n/a
Status: Complete
*/
void __cdecl site_set(int xCoord, int yCoord, int site) {
	map *tile = map_loc(xCoord, yCoord);
	tile->val2 &= 0x0F;
	tile->val2 |= site << 4;
}

/*
Purpose: Get region of tile at coordinates.
Original Offset: 00500220
Return Value: Region
Status: Complete
*/
uint32_t __cdecl region_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->region;
}

/*
Purpose: Set region of tile at coordinates.
Original Offset: 00591B90
Return Value: n/a
Status: Complete
*/
void __cdecl region_set(int xCoord, int yCoord, uint8_t region) {
	map_loc(xCoord, yCoord)->region = region;
}

/*
Purpose: Get faction using tile at coordinates.
Original Offset: n/a
Return Value: factionID
Status: Complete
*/
uint32_t __cdecl using_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->val3 & 7;
}

/*
Purpose: Set faction using tile at coordinates.
Original Offset: 00591C10
Return Value: n/a
Status: Complete
*/
void __cdecl using_set(int xCoord, int yCoord, int factionID) {
	map *tile = map_loc(xCoord, yCoord);
	tile->val3 &= 0xF8;
	tile->val3 |= factionID & 7;
}

/*
Purpose: Get factionID lock of tile at coordinates.
Original Offset: n/a
Return Value: factionID
Status: Complete
*/
uint32_t __cdecl lock_at(int xCoord, int yCoord) {
	return (map_loc(xCoord, yCoord)->val3 >> 3) & 7;
}

/*
Purpose: Set faction lock of tile at coordinates.
Original Offset: 00591C50
Return Value: n/a
Status: Complete
*/
void __cdecl lock_set(int xCoord, int yCoord, uint32_t factionID) {
	map *tile = map_loc(xCoord, yCoord);
	tile->val3 &= 0xC7;
	tile->val3 |= (factionID & 7) << 3;
}

/*
Purpose: Lock tile at coordinates for factionID.
Original Offset: 00591C90
Return Value: true if locked by another faction, otherwise false
Status: Complete
*/
BOOL __cdecl lock_map(int xCoord, int yCoord, uint32_t factionID) {
	uint32_t lockID = lock_at(xCoord, yCoord);
	if (lockID != factionID) {
		if (lockID) {
			return true;
		}
		lock_set(xCoord, yCoord, factionID);
	}
	return false;
}

/*
Purpose: Unlock tile at coordinates for factionID.
Original Offset: 00591CF0
Return Value: n/a
Status: Complete
*/
void __cdecl unlock_map(int xCoord, int yCoord, uint32_t factionID) {
	if (lock_at(xCoord, yCoord) == factionID) {
		map_loc(xCoord, yCoord)->val3 &= 0xC7;
	}
}

/*
Purpose: Get rockiness of tile at coordinates.
Original Offset: n/a
Return Value: Rockiness
Status: Complete
*/
uint32_t __cdecl rocky_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->val3 >> 6;
}

/*
Purpose: Set rockiness of tile at coordinates.
Original Offset: 00591BC0
Return Value: n/a
Status: Complete
*/
void __cdecl rocky_set(int xCoord, int yCoord, uint8_t rocky) {
	map *tile = map_loc(xCoord, yCoord);
	tile->val3 &= 0x3F;
	tile->val3 |= rocky << 6;
	tile->bit2 |= 0x400000; // TODO: identify value
	*UnkBitfield1 |= 1; // TODO: identify variable + value
}

/*
Purpose: Get bit of tile at coordinates. 
Original Offset: 005001B0
Return Value: Bitfield
Status: Complete
*/
uint32_t __cdecl bit_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->bit;
}

/*
Purpose: Set bit of tile at coordinates.
Original Offset: 00591D30
Return Value: n/a
Status: Complete
*/
void __cdecl bit_put(int xCoord, int yCoord, uint32_t bit) {
	map_loc(xCoord, yCoord)->bit = bit;
}

/*
Purpose: Set or unset bit of tile at coordinates.
Original Offset: 00591D60
Return Value: n/a
Status: Complete
*/
void __cdecl bit_set(int xCoord, int yCoord, uint32_t bit, BOOL set) {
	if (set) {
		map_loc(xCoord, yCoord)->bit |= bit;
	}
	else {
		map_loc(xCoord, yCoord)->bit &= ~bit;
	}
}

/*
Purpose: Get bit2 of tile at coordinates.
Original Offset: n/a
Return Value: Bitfield
Status: Complete
*/
uint32_t __cdecl bit2_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->bit2;
}

/*
Purpose: Set or unset bit2 of tile at coordinates.
Original Offset: 00591DB0
Return Value: n/a
Status: Complete
*/
void __cdecl bit2_set(int xCoord, int yCoord, uint32_t bit2, BOOL set) {
	if (set) {
		map_loc(xCoord, yCoord)->bit2 |= bit2;
	}
	else {
		map_loc(xCoord, yCoord)->bit2 &= ~bit2;
	}
}

/*
Purpose: Get code of tile at coordinates. This is the upper byte section of bit2. 
         This value keeps track of tile sequence order for landmarks.
Original Offset: n/a
Return Value: Code
Status: Complete
*/
uint32_t __cdecl code_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->bit2 >> 24;
}

/*
Purpose: Set code of tile at coordinates. This is the upper byte section of bit2.
		 This value keeps track of tile sequence order for landmarks.
Original Offset: 00591E00
Return Value: n/a
Status: Complete
*/
void __cdecl code_set(int xCoord, int yCoord, uint32_t code) {
	map *tile = map_loc(xCoord, yCoord);
	tile->bit2 &= 0xFFFFFF;
	tile->bit2 |= code << 24;
	*UnkBitfield1 |= 4; // TODO: identify variable + value
}

/*
Purpose: Synchronize actual tile bit with faction visible bit.
Original Offset: 00591E50
Return Value: n/a
Status: Complete
*/
void __cdecl synch_bit(int xCoord, int yCoord, int factionID) {
	if (factionID) {
		map_loc(xCoord, yCoord)->bitVisible[factionID-1] = bit_at(xCoord, yCoord);
	}
}

/*
Purpose: Check if coordinates are considered near or on coast. Radius (excludes actual coordinates)
         can either be all the squares directly around the coordinates or same as Base '+' radius.
Original Offset: 004E49D0
Return Value: Is tile coast? true/false
Status: Complete
*/
BOOL __cdecl is_coast(int xCoord, int yCoord, BOOL isBaseRadius) {
	uint32_t radius = isBaseRadius ? 21 : 9;
	for (uint32_t i = 1; i < radius; i++) {
		int xRadius = xCoord + xRadiusOffset[i-1];
		if (!*MapFlatToggle) { // if round map, additional parsing of coordinates
			if (xRadius >= 0) {
				if (xRadius >= (int)*MapHorizontalBounds) {
					xRadius -= *MapHorizontalBounds;
				}
			}
			else {
				xRadius += *MapHorizontalBounds;
			}
		}
		int yRadius = yCoord + yRadiusOffset[i-1];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0 
			&& xRadius < (int)*MapHorizontalBounds && is_ocean(xRadius, yRadius)) {
			return true; // modified original that would return i, all calls check return as boolean
		}
	}
	return false;
}

/*
Purpose: Check whether tile is ocean or not.
Original Offset: 005001E0
Return Value: Is tile ocean? true/false
Status: Complete
*/
BOOL __cdecl is_ocean(int xCoord, int yCoord) {
	return (map_loc(xCoord, yCoord)->val1 & 0xE0) < ALT_SHORE_LINE;
}

/*
Purpose: Get owner of tile if there is a Veh in it.
Original Offset: 00500250
Return Value: Owner/factionID or -1
Status: Complete
*/
int __cdecl veh_who(int xCoord, int yCoord) {
	map *tile = map_loc(xCoord, yCoord);
	if (tile->bit & BIT_VEH_IN_TILE) {
		uint32_t owner = tile->val2 & 0xF;
		if (owner < 8) {
			return owner;
		}
	}
	return -1;
}

/*
Purpose: Rebuild the Map's Veh related values.
Original Offset: 00532A90
Return Value: n/a
Status: Complete
*/
void __cdecl rebuild_vehicle_bits() {
	if (*MapVerticalBounds <= 0) {
		return;
	}
	for (uint32_t y = 0; y < *MapVerticalBounds; y++) {
		for (uint32_t x = y & 1; x < *MapHorizontalBounds; x += 2) {
			bit_set(x, y, BIT_VEH_IN_TILE, false);
			for (int vehID = 0; vehID < *VehCurrentCount; vehID++) {
				if (Veh[vehID].xCoord == (int)x && Veh[vehID].yCoord == (int)y) {
					bit_set(x, y, BIT_VEH_IN_TILE, true);
					if(!(bit_at(x, y) & BIT_BASE_IN_TILE)) {
						owner_set(x, y, Veh[vehID].factionID);
					}
					break;
				}
			}
		}
	}
}

/*
Purpose: Rebuild the Map's Base related values.
Original Offset: 00532B70
Return Value: n/a
Status: Complete
*/
void __cdecl rebuild_base_bits() {
	if (*MapVerticalBounds <= 0) {
		return;
	}
	for (uint32_t y = 0; y < *MapVerticalBounds; y++) {
		for (uint32_t x = y & 1; x < *MapHorizontalBounds; x += 2) {
			bit_set(x, y, BIT_BASE_IN_TILE, false);
			for (int baseID = 0; baseID < *BaseCurrentCount; baseID++) {
				if (Base[baseID].xCoord == (int)x && Base[baseID].yCoord == (int)y) {
					bit_set(x, y, BIT_BASE_IN_TILE, true);
					owner_set(x, y, Base[baseID].factionIDCurrent);
					break;
				}
			}
		}
	}
}

/*
Purpose: Get owner of tile if there is a Veh or Base in it.
Original Offset: 005798E0
Return Value: Owner/factionID or -1
Status: Complete
*/
int __cdecl anything_at(int xCoord, int yCoord) {
	map *tile = map_loc(xCoord, yCoord);
	if (tile->bit & (BIT_VEH_IN_TILE | BIT_BASE_IN_TILE)) {
		uint32_t owner = tile->val2 & 0xF;
		if (owner < 8) {
			return owner;
		}
	}
	return -1;
}

/*
Purpose: Check whether there is a sensor available in the tile.
Original Offset: 005BF010
Return Value: 0 (no sensor), 1 (sensor array via terraforming), 2 (Geosynchronous Survey Pod)
Status: Complete
*/
int __cdecl is_sensor(int xCoord, int yCoord) {
	if (bit_at(xCoord, yCoord) & BIT_SENSOR_ARRAY) {
		return 1; // Sensor Array built in tile
	}
	int baseID = base_find(xCoord, yCoord);
	if (baseID != -1) {
		int distX = abs(xCoord - Base[baseID].xCoord);
		if (!(*MapFlatToggle & 1) && distX > (int)*MapHorizontal) {
			distX = *MapHorizontalBounds - distX;
		}
		if (!distX || distX == 2) { // removed unnecessary duplicate calculation of distX
			int distY = abs(yCoord - Base[baseID].yCoord);
			if (!distY || distY == 2) {
				uint32_t geoOffset, geoMask;
				bitmask(FAC_GEOSYNC_SURVEY_POD, &geoOffset, &geoMask);
				if (Base[baseID].facilitiesPresentTable[geoOffset] & geoMask) {
					return 2; // Geosynchronous Survey Pod
				}
			}
		}
	}
	return 0; // No sensor found
}
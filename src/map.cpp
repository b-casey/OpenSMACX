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
#include "temp.h"
#include "map.h"
#include "alpha.h"
#include "base.h"
#include "faction.h"
#include "game.h"
#include "random.h"
#include "veh.h"

uint32_t *MapHorizontalBounds = (uint32_t *)0x00949870;
uint32_t *MapVerticalBounds = (uint32_t *)0x00949874;
uint32_t *MapRandSeed = (uint32_t *)0x00949878;
int *MapSeaLevel = (int *)0x0094987C;
int *MapSeaLevelCouncil = (int *)0x00949880;
uint32_t *MapArea = (uint32_t *)0x00949884;
uint32_t *MapAreaSqRoot = (uint32_t *)0x00949888;
BOOL *MapIsFlat = (BOOL *)0x0094988C;
uint32_t *MapLandmarkCount = (uint32_t *)0x00949890;
landmark *MapLandmark = (landmark *)0x00949894; // [64]
uint32_t *MapAbstractHorizBounds = (uint32_t *)0x0094A294;
uint32_t *MapAbstractVertBounds = (uint32_t *)0x0094A298;
uint32_t *MapAbstractArea = (uint32_t *)0x0094A29C;
uint32_t *MapSizePlanet = (uint32_t *)0x0094A2A0;
uint32_t *MapOceanCoverage = (uint32_t *)0x0094A2A4;
uint32_t *MapLandCoverage = (uint32_t *)0x0094A2A8;
uint32_t *MapErosiveForces = (uint32_t *)0x0094A2AC;
uint32_t *MapPlanetaryOrbit = (uint32_t *)0x0094A2B0;
uint32_t *MapCloudCover = (uint32_t *)0x0094A2B4;
uint32_t *MapNativeLifeForms = (uint32_t *)0x0094A2B8;
LPSTR *MapFilePath = (LPSTR *)0x0094A2BC;
map **Map = (map **)0x0094A30C;
uint8_t **MapAbstract = (uint8_t **)0x0094A310;

rules_natural *Natural = (rules_natural *)0x0094ADE0;
uint32_t *MapHorizontal = (uint32_t *)0x0068FAF0;
LPCSTR MapExtension = "MP";

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
Purpose: Bounds check and handling of xCoord for round maps.
Original Offset: 0048BEE0
Return Value: xCoord
Status: Complete
*/
int __cdecl xrange(int xCoord) {
	if (!*MapIsFlat) {
		if (xCoord >= 0) {
			if (xCoord >= (int)*MapHorizontalBounds) {
				xCoord -= *MapHorizontalBounds;
			}
		}
		else {
			xCoord += *MapHorizontalBounds;
		}
	}
	return xCoord;
}

/*
Purpose: Check who owns a tile. Optional parameter to get closest base.
Original Offset: 004E3EF0
Return Value: factionID of territory's owner; -1 if no owner or unknown faction
Status: Complete
*/
int __cdecl whose_territory(int factionID, int xCoord, int yCoord, int *baseID, BOOL ignoreComm) {
	int owner = map_loc(xCoord, yCoord)->territory;
	if (owner <= 0) {
		return -1; // no owner
	}
	if (factionID != owner) {
		if (!ignoreComm && !(*GameRules & TGL_OMNISCIENT_VIEW)
			&& (PlayersData[factionID].diploStatus[owner] 
				& (DSTATE_COMMLINK | DSTATE_UNK_0x8000000)) 
			!= (DSTATE_COMMLINK | DSTATE_UNK_0x8000000)) {
			return -1; // owner unknown to faction
		}
		if (baseID) {
			*baseID = base_find(xCoord, yCoord, -1, region_at(xCoord, yCoord), -1, -1);
		}
	}
	return owner;
}

/*
Purpose: Find closest base to territory owned by another faction not at war with specified faction.
Original Offset: 004E3FA0
Return Value: baseID or -1
Status: Complete
*/
int __cdecl base_territory(int factionID, int xCoord, int yCoord) {
	int baseID;
	int owner = whose_territory(factionID, xCoord, yCoord, &baseID, false);
	if (owner >= 0 && owner != factionID && 
		(FactionCurrentBitfield[0] & (1 << factionID) || FactionCurrentBitfield[0] & (1 << owner)) 
		&& !(PlayersData[factionID].diploStatus[owner] & DSTATE_VENDETTA)) {
		return baseID;
	}
	return -1;
}

/*
Purpose: Calculate the distance between two points.
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
	return &((*Map)[(xCoord >> 1) + yCoord * *MapHorizontal]);
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
Purpose: Get altitude of tile at coordinates shifting bits down.
Original Offset: 00500150
Return Value: Altitude
Status: Complete
*/
uint32_t __cdecl alt_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->val1 >> 5;
}

/*
Purpose: Get altitude of tile at coordinates without shifting bits.
Original Offset: n/a
Return Value: Altitude
Status: Complete
*/
uint32_t __cdecl altitude_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->val1 & 0xE0;
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
	return map_loc(xCoord, yCoord)->val3 & 0xC0;
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
		map_loc(xCoord, yCoord)->bitVisible[factionID - 1] = bit_at(xCoord, yCoord);
	}
}

/*
Purpose: Determine tile's mineral count that translates to rockiness.
Original Offset: 00591F00
Return Value: 0 (Flat), 1 (Rolling), 2 (Rocky)
Status: Complete
*/
uint32_t __cdecl minerals_at(int xCoord, int yCoord) {
	if (!yCoord || (int)(*MapVerticalBounds - 1) == yCoord) {
		return 2; // poles
	}
	uint32_t alt = alt_at(xCoord, yCoord);
	uint32_t avg = (xCoord + yCoord) >> 1;
	xCoord -= avg;
	int val1 = (xCoord / 2) + *MapRandSeed + (xCoord - (xCoord % 2)) + (avg - (avg % 2));
	int val2 = (val1 - 2 * (xCoord & 1) - (avg & 1)) & 3;
	int type = abs((int)alt - ALT_SHORE_LINE);
	if (alt < ALT_SHORE_LINE) {
		type--;
	}
	switch (type) {
	case 0:
	{
		switch (val2) {
		case 0:
			return 1;
		case 1:
		case 3:
			return 0;
		case 2:
			return ((val1 & 4) != 0) + 1; // 1 or 2
		default:
			return ~val2 & 1;
		}
	}
	case 1:
		return (val2 < 0 || val2 > 2) ? 2 : (val2 == 2) ? 1 : val2;
	case 2:
		return (val2 < 0 || val2 > 1) ? 2 : val2;
	case 3:
		return (val2 < 0 || val2 > 1) ? 2 : 1;
	default:
		return ~val2 & 1;
	}
}

/*
Purpose: Determine if tile has a resource bonus. Last param is unused. It's set to 1 by two calls 
         inside world_site(), otherwise all other calls have it set to  0.
Original Offset: 00592030
Return Value: 0 (no bonus), 1 (nutrient), 2 (mineral), 3 (energy)
Status: Complete
*/
uint32_t __cdecl bonus_at(int xCoord, int yCoord, int unkVal) {
	uint32_t bit = bit_at(xCoord, yCoord);
	uint32_t alt = alt_at(xCoord, yCoord);
	BOOL hasRscBonus = bit & BIT_RSC_BONUS;
	if (!hasRscBonus && (!*MapRandSeed 
		|| (alt >= ALT_SHORE_LINE && !(*GameRules2 & NO_UNITY_SCATTERING)))) {
		return 0;
	}
	uint32_t avg = (xCoord + yCoord) >> 1;
	xCoord -= avg;
	uint32_t chk = (avg & 3) + 4 * (xCoord & 3);
	if (!hasRscBonus && chk != ((*MapRandSeed + (-5 * (avg >> 2)) - 3 * (xCoord >> 2)) & 0xF)) {
		return 0;
	}
	if (alt < ALT_OCEAN_SHELF) {
		return 0;
	}
	uint32_t ret = (alt < ALT_SHORE_LINE) ? chk % 3 + 1 : (chk % 5) & 3;
	if (!ret || bit & BIT_NUTRIENT_RSC) {
		if (bit & BIT_ENERGY_RSC) {
			return 3; // energy
		}
		return ((bit & BIT_MINERAL_RSC) != 0) + 1; // nutrient or mineral
	}
	return ret;
}

/*
Purpose: Determine if tile has a supply pod and if so what type.
Original Offset: 00592140
Return Value: 0 (no supply pod), 1 (standard supply pod), 2 (unity pod?)
Status: Complete
*/
uint32_t __cdecl goody_at(int xCoord, int yCoord) {
	uint32_t bit = bit_at(xCoord, yCoord);
	if (bit & (BIT_SUPPLY_REMOVE | BIT_MONOLITH)) {
		return 0; // nothing, supply pod already opened or monolith
	}
	if (*GameRules2 & NO_UNITY_SCATTERING) {
		return (bit & (BIT_UNK_4000000|BIT_UNK_8000000)) ? 2 : 0; // ?
	}
	if (bit & BIT_SUPPLY_POD) {
		return 1; // supply pod
	}
	if (!MapRandSeed) {
		return 0; // nothing
	}
	uint32_t avg = (xCoord + yCoord) >> 1;
	int xCoordDiff = xCoord - avg;
	uint32_t cmp = (avg & 3) + 4 * (xCoordDiff & 3);
	if (!is_ocean(xCoord, yCoord) 
		&& cmp == ((-5 * (avg >> 2) - 3 * (xCoordDiff >> 2) + *MapRandSeed) & 0xF)) {
		return 2;
	}
	return cmp == ((11 * (avg / 4) + 61 * (xCoordDiff / 4) + *MapRandSeed + 8) & 0x1F); // 0 or 1
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
		int xRadius = xrange(xCoord + xRadiusOffset[i - 1]), 
			yRadius = yCoord + yRadiusOffset[i - 1];
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
	return altitude_at(xCoord, yCoord) < ALT_BIT_SHORE_LINE;
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
Purpose: Calculate distance between two xCoords with handling for round maps.
Original Offset: 00579790
Return Value: Distance
Status: Complete
*/
int __cdecl cursor_dist(int xCoord1, int xCoord2) {
	int dist = abs(xCoord1 - xCoord2);
	if (!*MapIsFlat && dist > (int)*MapHorizontal) {
		dist = *MapHorizontalBounds - dist;
	}
	return dist;
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
Purpose: Shutdown allocated map variables.
Original Offset: 00590E90
Return Value: n/a
Status: Complete
*/
void __cdecl map_shutdown() {
	if (*Map) {
		free(*Map);
	}
	if (*MapAbstract) {
		free(*MapAbstract);
	}
	*Map = 0;
	*MapAbstract = 0;
}

/*
Purpose: Initialize map variables.
Original Offset: 00590ED0
Return Value: n/a
Status: Complete
*/
BOOL __cdecl map_init() {
	sprintf_s((LPSTR)MapFilePath, 80, "maps\\%s.%s", label_get(676), MapExtension);
	*MapHorizontal = *MapHorizontalBounds / 2;
	*MapArea = *MapHorizontal * *MapVerticalBounds;
	*MapAreaSqRoot = quick_root(*MapArea);
	*Map = 0;
	*Map = (map *)mem_get(*MapArea * sizeof(map));
	if (*Map) {
		*MapAbstractHorizBounds = (*MapHorizontalBounds + 4) / 5;
		*MapAbstractVertBounds = (*MapVerticalBounds + 4) / 5;
		*MapAbstractArea = *MapAbstractVertBounds * ((*MapAbstractHorizBounds + 1) / 2);
		*MapAbstract = (uint8_t *)mem_get(*MapAbstractArea);
		if (*MapAbstract) {
			mapwin_terrain_fixup();
			return false;
		}
	}
	map_shutdown();
	return true;
}

/*
Purpose: Reset map to blank state. Doesn't wipe unk1 and territory fields.
Original Offset: 00591040
Return Value: n/a
Status: Complete
*/
void __cdecl map_wipe() {
	*MapSeaLevel = 0;
	*MapSeaLevelCouncil = 0;
	*MapLandmarkCount = 0;
	*MapRandSeed = random(0, 0x7FFF) + 1;
	for (uint32_t i = 0; i < *MapArea; i++) {
		(*Map)[i].val1 = ALT_BIT_OCEAN;
		(*Map)[i].altDetail = 20;
		(*Map)[i].val2 = 0xF;
		(*Map)[i].region = 0;
		(*Map)[i].visibility = 0;
		(*Map)[i].val3 = 0;
		(*Map)[i].bit = 0;
		(*Map)[i].bit2 = 0;
		memset((*Map)[i].bitVisible, 0, sizeof((*Map)[i].bitVisible));
	}
}

/*
Purpose: Write map data to a file.
Original Offset: 005910B0
Return Value: Did an error occur? true/false
Status: Complete
*/
BOOL __cdecl map_write(FILE *mapFile) {
	if (_fwrite(&*MapHorizontalBounds, 2724, 1, mapFile) 
		&& _fwrite(*Map, *MapArea * sizeof(map), 1, mapFile)
		&& _fwrite(*MapAbstract, *MapAbstractArea, 1, mapFile)) {
		return false;
	}
	return true;
}

/*
Purpose: Read map data from a file and write it into memory.
Original Offset: 00591130
Return Value: Did an error occur? true/false
Status: Complete
*/
BOOL __cdecl map_read(FILE *mapFile) {
	map_shutdown();
	if (!_fread(&*MapHorizontalBounds, 2724, 1, mapFile)) {
		return true;
	}
	*Map = 0;
	*MapAbstract = 0;
	if (map_init()) {
		return true;
	}
	if(!_fread(*Map, *MapArea * sizeof(map), 1, mapFile)
		|| !_fread(*MapAbstract, *MapAbstractArea, 1, mapFile)) {
		return true;
	}
	fixup_landmarks();
	return false;
}

/*
Purpose: Get abstract value for coordinates.
Original Offset: 00591210
Return Value: Abstract value
Status: Complete
*/
uint8_t __cdecl abstract_at(int xCoord, int yCoord) {
	return (*MapAbstract)[(xCoord >> 1) + yCoord * (*MapAbstractHorizBounds >> 1)];
}

/*
Purpose: Set abstract value for coordinates. 
Original Offset: 00591230
Return Value: n/a
Status: Complete
*/
void __cdecl abstract_set(int xCoord, int yCoord, uint8_t val) {
	(*MapAbstract)[(xCoord >> 1) + yCoord * (*MapAbstractHorizBounds >> 1)] = val;
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
		int distX = cursor_dist(xCoord, Base[baseID].xCoord);
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

/*
Purpose: Check if faction controls the initial tile (code offset 0) of the Manifold Nexus.
Original Offset: 005BF130
Return Value: Does faction control Nexus? true/false
Status: Complete
*/
BOOL __cdecl has_temple(int factionID) {
	for (uint32_t yCoord = 0; yCoord < *MapVerticalBounds; yCoord++) {
		for (uint32_t xCoord = yCoord & 1; xCoord < *MapHorizontalBounds; xCoord += 2) {
			uint32_t bit2 = bit2_at(xCoord, yCoord);
			if ((bit2 & (0x80000000 | LM_NEXUS)) == LM_NEXUS && !(bit2 & 0xFF000000)) {
				if (whose_territory(factionID, xCoord, yCoord, NULL, false) == factionID) {
					if (map_loc(xCoord, yCoord)->visibility & (1 << factionID)) { // tile visible
						return true;
					}
				}
			}
		}
	}
	return false;
}

/*
Purpose: Check for any type of zone of control conflicts (base and/or veh).
Original Offset: 005C89F0
Return Value: zoc: factionID + 1; no zoc: 0 (however return seems to be treated as boolean)
Status: Complete
*/
uint32_t __cdecl zoc_any(int xCoord, int yCoord, int factionID) {
	for (uint32_t i = 0; i < 8; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds) {
			int owner = anything_at(xRadius, yRadius);
			if (owner >= 0 && owner != factionID 
				&& !(PlayersData[factionID].diploStatus[owner] & DSTATE_PACT)) {
				return owner + 1;
			}
		}
	}
	return 0;
}

/*
Purpose: Check for Veh related zone of control conflicts.
Original Offset: 005C8AC0
Return Value: zoc: factionID + 1; no zoc: 0 (however return seems to be treated as boolean)
Status: Complete
*/
uint32_t __cdecl zoc_veh(int xCoord, int yCoord, int factionID) {
	uint32_t ret = 0;
	for (uint32_t i = 0; i < 8; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds) {
			int owner = veh_who(xRadius, yRadius);
			if (owner >= 0 && owner != factionID
				&& !(PlayersData[factionID].diploStatus[owner] & DSTATE_PACT)) {
				owner++;
				if (ret <= (uint32_t)owner) {
					ret = owner; // any point in continuing after 1st instance of zoc is found?
				}
			}
		}
	}
	return ret;
}

/*
Purpose: Check for Veh related zone of control conflicts taking into account land or ocean.
Original Offset: 005C8BA0
Return Value: zoc: factionID + 1; no zoc: 0 (however return seems to be treated as boolean)
Status: Complete
*/
uint32_t __cdecl zoc_sea(int xCoord, int yCoord, int factionID) {
	BOOL isOcean = is_ocean(xCoord, yCoord);
	for (uint32_t i = 0; i < 8; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds) {
			int owner = veh_who(xRadius, yRadius);
			if (owner >= 0 && owner != factionID && is_ocean(xRadius, yRadius) == isOcean
				&& !(PlayersData[factionID].diploStatus[owner] & DSTATE_PACT)) {
				for (int vehID = veh_at(xRadius, yRadius); vehID >= 0;
					vehID = Veh[vehID].nextVehIDStack) {
					if(Veh[vehID].factionID != factionID 
						&& (Veh[vehID].visibleToFaction & (1 << factionID) 
						|| (!*MultiplayerToggle && !(Veh[vehID].flags & VFLAG_INVISIBLE)))) {
						return owner + 1;
					}
				}
			}
		}
	}
	return 0;
}

/*
Purpose: Check for Veh related zone of control conflicts when moving to non-base tile.
Original Offset: 005C8D40
Return Value: zoc: factionID + 1; no zoc: 0 (however return seems to be treated as boolean)
Status: Complete
*/
uint32_t __cdecl zoc_move(int xCoord, int yCoord, int factionID) {
	int owner;
	if (!(bit_at(xCoord, yCoord) & BIT_BASE_IN_TILE)
	|| ((owner = owner_at(xCoord, yCoord)), owner >= 8 || owner < 0)) {
		return zoc_sea(xCoord, yCoord, factionID);
	}
	return 0;
}
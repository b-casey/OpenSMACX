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

continent *Continents = (continent *)0x009AA730; // [128]
rules_natural *Natural = (rules_natural *)0x0094ADE0;
uint32_t *MapHorizontal = (uint32_t *)0x0068FAF0;
uint32_t *AltNatural = (uint32_t *)0x0068FB4C;
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
		if (!ignoreComm && !(*GameState & STATE_OMNISCIENT_VIEW)
			&& (PlayersData[factionID].diploTreaties[owner]
				& (DTREATY_COMMLINK | DTREATY_UNK_8000000)) 
			!= (DTREATY_COMMLINK | DTREATY_UNK_8000000)) {
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
	if (owner >= 0 && owner != factionID && (is_human(factionID) || is_human(owner))
		&& !(PlayersData[factionID].diploTreaties[owner] & DTREATY_VENDETTA)) {
		return baseID;
	}
	return -1;
}

/*
Purpose: For the specified tile, calculate the quality of the terrain.
Original Offset: 004ECB90
Return Value: Quality of terrain, lower is better (0-2)
Status: Complete
*/
uint32_t __cdecl crappy(int xCoord, int yCoord) {
	uint32_t poorQuality = 0;
	uint32_t rainfall = map_loc(xCoord, yCoord)->climate & (RAINFALL_MOIST | RAINFALL_RAINY);
	if (!rainfall) {
		poorQuality = 1; // neither moist or rainy
	}
	uint32_t rocky = rocky_at(xCoord, yCoord);
	if (rocky == TERRAIN_ROCKY) {
		poorQuality++; // rocky
	}
	else if (rocky == TERRAIN_FLAT && rainfall < RAINFALL_RAINY) {
		poorQuality++; // flat, moist or arid
	}
	return poorQuality;
}

/*
Purpose: Take the absolute distance between two points as parameters to calculate how far out they 
         radiate. This is mainly used to determine proximity or how far away the two points are from
		 each other in a rough circle shape (see xRadiusOffset[]/yRadiusOffset[]).
Original Offset: 004F8090
Return Value: Distance radius
Status: Complete
*/
int __cdecl x_dist(int xDistance, int yDistance) {
	xDistance = abs(xDistance);
	yDistance = abs(yDistance);
	int largest = xDistance;
	if (xDistance <= yDistance) {
		largest = yDistance;
	}
	int smallest = xDistance;
	if (xDistance >= yDistance) {
		smallest = yDistance;
	}
	return largest - (((yDistance + xDistance) / 2) - smallest + 1) / 2;
}

/*
Purpose: Check whether there is a path between two regions. Seems to only take into account land
		 destinations. TODO: Revisit in the future once Continent/Path is more defined.
Original Offset: 0050DDC0
Return Value: Is there path? true/false
Status: Complete
*/
BOOL __cdecl sea_coast(uint32_t regionDst, uint32_t regionSrc) {
	uint32_t offset, mask;
	bitmask(regionSrc & RegionBounds, &offset, &mask);
	return (Continents[regionDst].unk_6[offset] & mask) != 0;
}

/*
Purpose: Count number of paths from source region. Seems to only take into account land source
		 and destination ranges. TODO: Revisit in the future once Continent/Path is more defined.
Original Offset: 0050DE00
Return Value: Sea coasts valid path count
Status: Complete
*/
uint32_t __cdecl sea_coasts(uint32_t regionSrc) {
	uint32_t seaCoastCount = 0;
	for (int i = 1; i < RegionBounds; i++) {
		if (sea_coast(i, regionSrc)) {
			seaCoastCount++;
		}
	}
	return seaCoastCount;
}

/*
Purpose: Check to see whether base is within a one tile radius of a sea tile with specified region.
		 If you pass a land region (<63) as the 2nd parameter, it is possible to get collision
		 behavior due to region bounding. TODO: Revisit in the future to see whether to remove them.
Original Offset: 0050DE50
Return Value: Is base connected to specified sea region? true/false
Status: Complete
*/
BOOL __cdecl base_on_sea(uint32_t baseID, uint32_t regionSea) {
	regionSea &= RegionBounds;
	if (regionSea >= RegionBounds) { // change to equals since already bounded?
		return false; // skips poles (land or ocean)
	}
	int xCoord = Base[baseID].xCoord, yCoord = Base[baseID].yCoord;
	for (uint32_t i = 0; i < 8; i++) {
		int xRadius = xrange(xCoord + xRadiusBase[i]), yRadius = yCoord + yRadiusBase[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds && is_ocean(xRadius, yRadius)
			&& (region_at(xRadius, yRadius) & RegionBounds) == regionSea) {
			return true;
		}
	}
	return false;
}

/*
Purpose: Determine the ocean region for coastal bases. There is an issue if a base is connected to
         more than one ocean region, it will only return the last checked region based on the base 
		 radius clockwise order. This could cause the AI to make incorrect calculations for what to
		 prioritize building at the base. An example could be a base that is connected to a small
		 water body as well as the much larger ocean. Depending on where these bodies of water are 
		 positioned, the AI might make assumptions not to prioritize building naval units. Also,
		 the Continents compare logic isn't used by anything. This might be the root cause of
		 outlined bug. TODO: Revisit in the future once more is known about Continent structure.
Original Offset: 0050DF30
Return Value: Ocean region or -1 if landlocked
Status: Complete
*/
int __cdecl base_coast(uint32_t baseID) {
	int region = -1;
	int val = 0;
	int xCoord = Base[baseID].xCoord, yCoord = Base[baseID].yCoord;
	for (uint32_t i = 0; i < 8; i++) { // is_coast()
		int xRadius = xrange(xCoord + xRadiusBase[i]), yRadius = yCoord + yRadiusBase[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds && is_ocean(xRadius, yRadius)) {
			region = region_at(xRadius, yRadius);
			int compare = (region >= 127) ? 1 : Continents[region].unk_1;
			if (compare >= val) {
				val = compare; // value isn't used?
			}
			i += (2 - (i & 1)); // skips adjacent tiles
		}
	}
	return region;
}

/*
Purpose: Check to see if a port base shares a common body of water with destination coastal region.
Original Offset: 0050E030
Return Value: Is port and coastal region accessible by water to each other? true/false
Status: Complete
*/
BOOL __cdecl port_to_coast(uint32_t baseID, uint32_t region) {
	int xCoord = Base[baseID].xCoord, yCoord = Base[baseID].yCoord;
	if (region_at(xCoord, yCoord) == region) {
		return true;
	}
	for (uint32_t i = 0; i < 8; i++) { // is_coast()
		int xRadius = xrange(xCoord + xRadiusBase[i]), yRadius = yCoord + yRadiusBase[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds && is_ocean(xRadius, yRadius)) {
			if (sea_coast(region, region_at(xRadius, yRadius))) {
				return true;
			}
			i += (2 - (i & 1)); // skips adjacent tiles
		}
	}
	return false;
}

/*
Purpose: Check to see if two port bases share a common body of water determined by region.
Original Offset: 0050E160
Return Value: Are both ports accessible by water to each other? true/false
Status: Complete
*/
BOOL __cdecl port_to_port(uint32_t baseIDSrc, uint32_t baseIDDst) {
	int xCoord = Base[baseIDSrc].xCoord, yCoord = Base[baseIDSrc].yCoord, lastRegion = -1;
	for (uint32_t i = 0; i < 8; i++) { // is_coast()
		int xRadius = xrange(xCoord + xRadiusBase[i]), yRadius = yCoord + yRadiusBase[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds && is_ocean(xRadius, yRadius)) {
			int regionSrc = region_at(xRadius, yRadius);
			if (regionSrc != lastRegion) { // reduce redundant checks especially sea bases
				lastRegion = regionSrc;
				if (base_on_sea(baseIDDst, regionSrc)) {
					return true;
				}
			}
		}
	}
	return false;
}

/*
Purpose: Determine if a base has access to ports or more than one coastal region. This helps 
         prioritize whether naval transports should be built.
Original Offset: 0050E310
Return Value: Should base build naval transports? true/false
Status: Complete
*/
BOOL __cdecl transport_base(uint32_t baseID) {
	int region = base_coast(baseID);
	if (region < 0) {
		return false; // landlocked
	}
	if (is_ocean(Base[baseID].xCoord, Base[baseID].yCoord)) {
		return true; // ocean base
	}
	return (sea_coasts(region) > 1);
}

/*
Purpose: Determine if there are other faction's ports in the vicinity of the specified base.
Original Offset: 0050E3C0
Return Value: Does base have a strategic naval importance? true/false
Status: Complete
*/
BOOL __cdecl naval_base(uint32_t baseID) {
	if (base_coast(baseID) < 0 || *BaseCurrentCount <= 0) {
		return false; // landlocked base or no bases
	}
	uint32_t factionID = Base[baseID].factionIDCurrent;
	for (int i = 0; i < *BaseCurrentCount; i++) {
		if (factionID != Base[i].factionIDCurrent) {
			if (port_to_port(baseID, i)) {
				return true;
			}
		}
	}
	return false;
}

/*
Purpose: Determine if specified Veh can set up a convoy route with specified base.
Original Offset: 0050E5C0
Return Value: Is convoy route possible? true/false
Status: Complete
*/
BOOL __cdecl convoy(uint32_t vehID, uint32_t baseID) {
	int homeBaseID = Veh[vehID].homeBaseID;
	if (homeBaseID < 0 || baseID == (uint32_t)homeBaseID) {
		return false;
	}
	uint32_t triad = Chassis[VehPrototype[Veh[vehID].protoID].chassisID].triad;
	if (triad == TRIAD_AIR) {
		return true; // air
	}
	uint32_t regionBase = region_at(Base[baseID].xCoord, Base[baseID].yCoord);
	if (region_at(Base[homeBaseID].xCoord, Base[homeBaseID].yCoord) == regionBase 
		&& ((regionBase >= 64) == (triad == TRIAD_SEA))) {
		return true; // same region, by land or sea
	}
	if (triad) {
		return port_to_port(baseID, homeBaseID); // sea
	}
	return false;
}

/*
Purpose: Validate region bounds. Bad regions include: 0, 63, 64, 127, 128.
Original Offset: 005591C0
Return Value: Is region bad? true/false
Status: Complete
*/
BOOL __cdecl bad_reg(int region) {
	return (region & RegionBounds) == RegionBounds || !(region & RegionBounds);
}

/*
Purpose: Determine whether specified Veh can physically reach the destination coordinates.
Original Offset: 0056B320
Return Value: Can Veh reach tile? true/false
Status: Complete
*/
BOOL __cdecl get_there(uint32_t vehID, int xCoordDst, int yCoordDst) {
	uint32_t triad = Chassis[VehPrototype[Veh[vehID].protoID].chassisID].triad;
	if (triad == TRIAD_AIR) {
		return true;
	}
	int xCoordSrc = Veh[vehID].xCoord, yCoordSrc = Veh[vehID].yCoord;
	uint32_t regionSrc = region_at(xCoordSrc, yCoordSrc),
		regionDst = region_at(xCoordDst, yCoordDst);
	if (!triad) { // TRIAD_LAND
		return (regionSrc == regionDst);
	}
	int baseIDSrc = base_at(xCoordSrc, yCoordSrc), baseIDDst = base_at(xCoordDst, yCoordDst);
	if (baseIDDst >= 0) {
		return (baseIDSrc < 0) ? base_on_sea(baseIDDst, regionSrc)
			: port_to_port(baseIDSrc, baseIDDst);
	}
	if (is_ocean(xCoordDst, yCoordDst)) {
		return (baseIDSrc < 0) ? (regionSrc == regionDst) : base_on_sea(baseIDSrc, regionDst);
	}
	return (baseIDSrc < 0) ? sea_coast(regionDst, regionSrc) : port_to_coast(baseIDSrc, regionDst);
}

/*
Purpose: Determine whether point A is a coast or border tile. It seems that the point B check is 
         effectively disabled since usage of this function passes same coordinates for both points.
		 Modified return value to boolean rather than returning i. It is always treated as boolean
		 and makes a lot more sense than returning iterator position.
Original Offset: 0056B480
Return Value: Is point A considered a border or coast? true/false
Status: Complete
*/
BOOL __cdecl coast_or_border(int xCoordPtA, int yCoordPtA, int xCoordPtB, int yCoordPtB,
	int factionID) {
	if (factionID != whose_territory(factionID, xCoordPtA, yCoordPtA, NULL, false)) {
		return false; // faction doesn't control point A
	}
	uint32_t regionA = region_at(xCoordPtA, yCoordPtA), regionB = region_at(xCoordPtB, yCoordPtB);
	for (uint32_t i = 1; i < 9; i++) {
		int xRadius = xrange(xCoordPtA + xRadiusOffset[i]), yRadius = yCoordPtA + yRadiusOffset[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds && (is_ocean(xRadius, yRadius)
			|| whose_territory(factionID, xRadius, yRadius, NULL, false) != factionID
				|| regionA != regionB)) { // not sure about reason for this
			return true; // modified to boolean rather than returning i
		}
	}
	return false;
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
	return map_loc(xCoord, yCoord)->climate & 7;
}

/*
Purpose: Set temperature of tile at coordinates.
Original Offset: 00591AD0
Return Value: n/a
Status: Complete
*/
void __cdecl temp_set(int xCoord, int yCoord, uint8_t temperature) {
	map *tile = map_loc(xCoord, yCoord);
	tile->climate &= 0xF8;
	tile->climate |= temperature & 7;
}

/*
Purpose: Set climate rainfall of tile at coordinates.
Original Offset: 00591A80
Return Value: n/a
Status: Complete
*/
void __cdecl climate_set(int xCoord, int yCoord, uint8_t rainfall) {
	map *tile = map_loc(xCoord, yCoord);
	tile->climate &= 0xE7;
	tile->climate |= (rainfall & 3) << 3;
	tile->bit2 |= 0x400000; // TODO: identify value
	*UnkBitfield1 |= 1; // TODO: identify global + value
}

/*
Purpose: Calculate elevation for tile at coordinates.
Original Offset: 005919C0
Return Value: Elevation (bounds: -3000 to 3500)
Status: Complete
*/
int __cdecl elev_at(int xCoord, int yCoord) {
	uint32_t contour = alt_detail_at(xCoord, yCoord);
	int elev = 50 * (contour - ElevDetail[3] - *MapSeaLevel);
	elev += (contour <= ElevDetail[alt_at(xCoord, yCoord)]) ? 10
		: (xCoord * 113 + yCoord * 217 + *MapSeaLevel * 301) % 50;
	return range(elev, -3000, 3500);
}

/*
Purpose: Calculate natural altitude of tile at coordinates.
Original Offset: 005918A0
Return Value: Natural altitude on a scale from 0 (ocean trench) to 6 (mountain tops)
Status: Complete
*/
uint32_t __cdecl alt_natural(int xCoord, int yCoord) {
	uint32_t contour = alt_detail_at(xCoord, yCoord) - *MapSeaLevel;
	uint32_t natural = 6;
	while (contour < AltNatural[natural] && natural) {
		natural--;
	}
	return natural;
}

/*
Purpose: Get altitude of tile at coordinates shifting bits down.
Original Offset: 00500150
Return Value: Altitude
Status: Complete
*/
uint32_t __cdecl alt_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->climate >> 5;
}

/*
Purpose: Get altitude of tile at coordinates without shifting bits.
Original Offset: n/a
Return Value: Altitude
Status: Complete
*/
uint32_t __cdecl altitude_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->climate & 0xE0;
}

/*
Purpose: Get altitude details of tile at coordinates.
Original Offset: 00500180
Return Value: Altitude detail
Status: Complete
*/
uint32_t __cdecl alt_detail_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->contour;
}

/*
Purpose: Set altitude details of tile at coordinates.
Original Offset: 00591260
Return Value: n/a
Status: Complete
*/
void __cdecl alt_put_detail(int xCoord, int yCoord, uint8_t detail) {
	map_loc(xCoord, yCoord)->contour = detail;
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
		|| (alt >= ALT_SHORE_LINE && !(*GameRules & RULES_NO_UNITY_SCATTERING)))) {
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
	if (*GameRules & RULES_NO_UNITY_SCATTERING) {
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
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
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
Purpose: Check whether faction can see tile at specified coordinates.
Original Offset: 00579840
Return Value: Is tile visible/known to faction? true/false
Status: Complete
*/
BOOL __cdecl is_known(uint32_t xCoord, uint32_t yCoord, uint32_t factionID) {
	return (PlayersData[factionID].playerFlags & PFLAG_MAP_REVEALED
		|| map_loc(xCoord, yCoord)->visibility & (1 << factionID));
}

/*
Purpose: Get owner of the tile if a base exists at the specified coordinates.
Original Offset: 005798A0
Return Value: Owner/factionID or -1
Status: Complete
*/
int __cdecl base_who(uint32_t xCoord, uint32_t yCoord) {
	map *tile = map_loc(xCoord, yCoord);
	if (tile->bit & BIT_BASE_IN_TILE) {
		uint32_t owner = tile->val2 & 0xF;
		if (owner < MaxPlayerNum) {
			return owner;
		}
	}
	return -1;
}

/*
Purpose: Get owner of tile if there is a Veh or Base on it.
Original Offset: 005798E0
Return Value: Owner/factionID or -1
Status: Complete
*/
int __cdecl anything_at(uint32_t xCoord, uint32_t yCoord) {
	map *tile = map_loc(xCoord, yCoord);
	if (tile->bit & (BIT_VEH_IN_TILE | BIT_BASE_IN_TILE)) {
		uint32_t owner = tile->val2 & 0xF;
		if (owner < MaxPlayerNum) {
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
Purpose: Reset map to blank state. Doesn't wipe unk_1 and territory fields.
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
		(*Map)[i].climate = ALT_BIT_OCEAN;
		(*Map)[i].contour = 20;
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
				if (has_fac_built(FAC_GEOSYNC_SURVEY_POD, baseID)) {
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
Purpose: Check for any type of zone of control conflicts (Base and/or Veh).
Original Offset: 005C89F0
Return Value: zoc: factionID + 1; no zoc: 0 (however return seems to be treated as boolean)
Status: Complete
*/
uint32_t __cdecl zoc_any(int xCoord, int yCoord, uint32_t factionID) {
	for (uint32_t i = 0; i < 8; i++) {
		int xRadius = xrange(xCoord + xRadiusBase[i]), yRadius = yCoord + yRadiusBase[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds) {
			int owner = anything_at(xRadius, yRadius);
			if (owner >= 0 && owner != (int)factionID 
				&& !(PlayersData[factionID].diploTreaties[owner] & DTREATY_PACT)) {
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
uint32_t __cdecl zoc_veh(int xCoord, int yCoord, uint32_t factionID) {
	uint32_t ret = 0;
	for (uint32_t i = 0; i < 8; i++) {
		int xRadius = xrange(xCoord + xRadiusBase[i]), yRadius = yCoord + yRadiusBase[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds) {
			int owner = veh_who(xRadius, yRadius);
			if (owner >= 0 && owner != (int)factionID
				&& !(PlayersData[factionID].diploTreaties[owner] & DTREATY_PACT)) {
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
Return Value: zoc: factionID + 1; no zoc: 0 (however return seems to mostly be treated as boolean)
Status: Complete
*/
uint32_t __cdecl zoc_sea(int xCoord, int yCoord, uint32_t factionID) {
	BOOL isOcean = is_ocean(xCoord, yCoord);
	for (uint32_t i = 0; i < 8; i++) {
		int xRadius = xrange(xCoord + xRadiusBase[i]), yRadius = yCoord + yRadiusBase[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds) {
			int owner = veh_who(xRadius, yRadius);
			if (owner >= 0 && owner != (int)factionID && is_ocean(xRadius, yRadius) == isOcean
				&& !(PlayersData[factionID].diploTreaties[owner] & DTREATY_PACT)) {
				for (int vehID = veh_at(xRadius, yRadius); vehID >= 0;
					vehID = Veh[vehID].nextVehIDStack) {
					if(Veh[vehID].factionID != factionID 
						&& (Veh[vehID].visibility & (1 << factionID)
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
uint32_t __cdecl zoc_move(int xCoord, int yCoord, uint32_t factionID) {
	int owner;
	if (!(bit_at(xCoord, yCoord) & BIT_BASE_IN_TILE)
	|| ((owner = owner_at(xCoord, yCoord)), owner >= 8 || owner < 0)) {
		return zoc_sea(xCoord, yCoord, factionID);
	}
	return 0;
}
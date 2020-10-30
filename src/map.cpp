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
#include "path.h"
#include "random.h"
#include "strings.h"
#include "veh.h"

uint32_t *MapLongitudeBounds = (uint32_t *)0x00949870;
uint32_t *MapLatitudeBounds = (uint32_t *)0x00949874;
uint32_t *MapRandSeed = (uint32_t *)0x00949878;
int *MapSeaLevel = (int *)0x0094987C;
int *MapSeaLevelCouncil = (int *)0x00949880;
uint32_t *MapArea = (uint32_t *)0x00949884;
uint32_t *MapAreaSqRoot = (uint32_t *)0x00949888;
BOOL *MapIsFlat = (BOOL *)0x0094988C;
uint32_t *MapLandmarkCount = (uint32_t *)0x00949890;
landmark *MapLandmark = (landmark *)0x00949894; // [64]
uint32_t *MapAbstractLongBounds = (uint32_t *)0x0094A294;
uint32_t *MapAbstractLatBounds = (uint32_t *)0x0094A298;
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
uint32_t *MapBaseSubmergedCount = (uint32_t *)0x009B2290; // [8]; reset each time global alt changes
int *MapBaseIDClosestSubmergedVeh = (int *)0x009B22BC; // [8]; reset each time global alt changes
uint32_t *BrushVal1 = (uint32_t *)0x009B22B0;
uint32_t *BrushVal2 = (uint32_t *)0x009B22B8;
uint32_t *WorldBuildVal1 = (uint32_t *)0x009B22B4;

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
	return yCoord >= 0 && yCoord < (int)*MapLatitudeBounds
		&& xCoord >= 0 && xCoord < (int)*MapLongitudeBounds;
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
			if (xCoord >= (int)*MapLongitudeBounds) {
				xCoord -= *MapLongitudeBounds;
			}
		}
		else {
			xCoord += *MapLongitudeBounds;
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
			&& has_treaty(factionID, owner, DTREATY_COMMLINK | DTREATY_UNK_8000000)
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
		&& !has_treaty(factionID, owner, DTREATY_VENDETTA)) {
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
	uint32_t rainfall = climate_at(xCoord, yCoord);
	if (rainfall == RAINFALL_ARID) {
		poorQuality = 1; // neither moist or rainy
	}
	uint32_t rocky = rocky_at(xCoord, yCoord);
	if (rocky == TERRAIN_BIT_ROCKY) {
		poorQuality++; // rocky
	}
	else if (rocky == ROCKINESS_FLAT && rainfall < RAINFALL_RAINY) {
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
int __cdecl vector_dist(int xDistance, int yDistance) {
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
Purpose: Take two points and calculate how far out they radiate. This is mainly used to determine
		 proximity or how far away the two points are from each other in a rough circle shape.
Original Offset: 005A5910
Return Value: Distance radius
Status: Complete
*/
int __cdecl vector_dist(int xCoordA, int yCoordA, int xCoordB, int yCoordB) {
	return vector_dist(x_dist(xCoordA, xCoordB), abs(yCoordA - yCoordB));
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
	return (Continents[regionDst].seaCoasts[offset] & mask) != 0;
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
		if (on_map(xRadius, yRadius) && is_ocean(xRadius, yRadius) 
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
		if (on_map(xRadius, yRadius) && is_ocean(xRadius, yRadius)) {
			region = region_at(xRadius, yRadius);
			int compare = (region >= 127) ? 1 : Continents[region].tiles;
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
		if (on_map(xRadius, yRadius) && is_ocean(xRadius, yRadius)) {
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
		if (on_map(xRadius, yRadius) && is_ocean(xRadius, yRadius)) {
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
	uint32_t triad = get_triad(vehID);
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
	uint32_t triad = get_triad(vehID);
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
		if (on_map(xRadius, yRadius) && (is_ocean(xRadius, yRadius)
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
map *__cdecl map_loc(int xCoord, int yCoord) {
	return &((*Map)[(xCoord >> 1) + yCoord * *MapHorizontal]);
}

/*
Purpose: Get the temperature of the specified tile.
Original Offset: n/a
Return Value: Temperature
Status: Complete
*/
uint32_t __cdecl temp_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->climate & 7;
}

/*
Purpose: Set the temperature of the specified tile.
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
Purpose: Get the rainfall (climate) of the specified tile.
Original Offset: n/a
Return Value: Rainfall (climate)
Status: Complete
*/
uint32_t __cdecl climate_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->climate & (RAINFALL_RAINY | RAINFALL_MOIST);
}

/*
Purpose: Set the rainfall (climate) of the specified tile.
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
	uint32_t natural = ALT_3_LEVELS_ABOVE_SEA;
	while (contour < AltNatural[natural] && natural) {
		natural--;
	}
	return natural;
}

/*
Purpose: Set both the altitude and natural altitude of the specified tile.
Original Offset: 005918F0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl alt_set_both(int xCoord, int yCoord, uint32_t altitude) {
	alt_set(xCoord, yCoord, altitude);
	if (alt_natural(xCoord, yCoord) != altitude) {
		alt_put_detail(xCoord, yCoord, (uint8_t)(AltNatural[altitude] + *MapSeaLevel
			+ rnd(AltNatural[altitude + 1] - AltNatural[altitude], NULL)));
	}
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
Purpose: Get the code of tile at coordinates. This is the upper byte section of bit2. This value 
         keeps track of tile sequence order for landmarks.
Original Offset: n/a
Return Value: Code
Status: Complete
*/
uint32_t __cdecl code_at(int xCoord, int yCoord) {
	return map_loc(xCoord, yCoord)->bit2 >> 24;
}

/*
Purpose: Set the code of tile at coordinates. This is the upper byte section of bit2. This value 
         keeps track of tile sequence order for landmarks.
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
	if (!yCoord || (int)(*MapLatitudeBounds - 1) == yCoord) {
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
uint32_t __cdecl bonus_at(int xCoord, int yCoord, int UNUSED(unkVal)) {
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
		return (bit & (BIT_UNK_4000000 | BIT_UNK_8000000)) ? 2 : 0; // ?
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
Purpose: Clear the map's site values in a radius from the tile. The 3rd parameter is unused.
Original Offset: 00592400
Return Value: n/a
Status: Complete
*/
void __cdecl site_radius(int xCoord, int yCoord, int UNUSED(valUnk)) {
	for (int i = 0; i < 21; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			map *tile = map_loc(xRadius, yRadius);
			tile->val2 &= 0x0F;
		}
	}
}

/*
Purpose: Search for the first landmark found within the radius range of the specified tile.
Original Offset: 00592550
Return Value: Landmark offset or -1 if none found
Status: Complete - testing
*/
int __cdecl find_landmark(int xCoord, int yCoord, uint32_t radiusRangeOffset) {
	uint32_t radius = RadiusRange[radiusRangeOffset];
	for (uint32_t i = 0; i < radius; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			for (uint32_t lm = 0; lm < *MapLandmarkCount; lm++) {
				if (xRadius == MapLandmark[lm].xCoord && yRadius == MapLandmark[lm].yCoord) {
					return lm;
				}
			}
		}
	}
	return -1; // none found
}

/*
Purpose: Set up a new landmark with the provided name at the specified tile.
Original Offset: 00592600
Return Value: Landmark offset or -1 if max landmark count is reached
Status: Complete - testing
*/
int __cdecl new_landmark(int xCoord, int yCoord, LPSTR name) {
	uint32_t landmarkOffset = *MapLandmarkCount;
	if (landmarkOffset >= MaxLandmarkNum) {
		return -1;
	}
	*MapLandmarkCount += 1;
	landmark *lm = &MapLandmark[landmarkOffset];
	lm->xCoord = xCoord;
	lm->yCoord = yCoord;
	strcpy_s(lm->name, 32, name);
	return landmarkOffset;
}

/*
Purpose: Check whether the specified faction has permission to name a landmark on the provided tile.
Original Offset: 00592650
Return Value: Does faction have control of the tile to set a landmark? true/false
Status: Complete - testing
*/
BOOL __cdecl valid_landmark(int xCoord, int yCoord, int factionID) {
	int terrFactionID = *IsMultiplayerNet ? map_loc(xCoord, yCoord)->territory 
		: whose_territory(factionID, xCoord, yCoord, NULL, false);
	if (terrFactionID == factionID) {
		return true;
	}
	if (terrFactionID > 0) {
		return false;
	}
	int baseID = base_find(xCoord, yCoord);
	return baseID >= 0 ? Base[baseID].factionIDCurrent == factionID : true;
}

/*
Purpose: Remove the landmark at the specified tile.
Original Offset: 005926F0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl kill_landmark(int xCoord, int yCoord) {
	int landmarkToKill = find_landmark(xCoord, yCoord, 1);
	if (landmarkToKill >= 0) {
		if ((uint32_t)landmarkToKill < (*MapLandmarkCount - 1)) {
			memcpy_s(&MapLandmark[landmarkToKill], sizeof(landmark) * MaxLandmarkNum,
				&MapLandmark[landmarkToKill + 1], // single memory copy replaces original loop
				sizeof(landmark) * (*MapLandmarkCount - landmarkToKill - 1));
		}
		*MapLandmarkCount -= 1;
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
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius) && is_ocean(xRadius, yRadius)) {
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
	for (uint32_t y = 0; y < *MapLatitudeBounds; y++) {
		for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
			bit_set(x, y, BIT_VEH_IN_TILE, false);
			for (int vehID = 0; vehID < *VehCurrentCount; vehID++) {
				if (Veh[vehID].xCoord == (int)x && Veh[vehID].yCoord == (int)y) {
					bit_set(x, y, BIT_VEH_IN_TILE, true);
					if (!(bit_at(x, y) & BIT_BASE_IN_TILE)) {
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
	for (uint32_t y = 0; y < *MapLatitudeBounds; y++) {
		for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
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
Purpose: Calculate the distance between two xCoords with handling for round maps.
Original Offset: 00579790
Return Value: Distance
Status: Complete
*/
int __cdecl x_dist(int xCoordA, int xCoordB) {
	int dist = abs(xCoordA - xCoordB);
	if (!*MapIsFlat && dist > (int)*MapHorizontal) {
		dist = *MapLongitudeBounds - dist;
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
	*MapHorizontal = *MapLongitudeBounds / 2;
	*MapArea = *MapHorizontal * *MapLatitudeBounds;
	*MapAreaSqRoot = quick_root(*MapArea);
	*Map = 0;
	*Map = (map *)mem_get(*MapArea * sizeof(map));
	if (*Map) {
		*MapAbstractLongBounds = (*MapLongitudeBounds + 4) / 5;
		*MapAbstractLatBounds = (*MapLatitudeBounds + 4) / 5;
		*MapAbstractArea = *MapAbstractLatBounds * ((*MapAbstractLongBounds + 1) / 2);
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
		ZeroMemory((*Map)[i].bitVisible, sizeof((*Map)[i].bitVisible));
	}
}

/*
Purpose: Write map data to a file.
Original Offset: 005910B0
Return Value: Did an error occur? true/false
Status: Complete
*/
BOOL __cdecl map_write(FILE *mapFile) {
	if (_fwrite(&*MapLongitudeBounds, 2724, 1, mapFile)
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
	if (!_fread(&*MapLongitudeBounds, 2724, 1, mapFile)) {
		return true;
	}
	*Map = 0;
	*MapAbstract = 0;
	if (map_init()) {
		return true;
	}
	if (!_fread(*Map, *MapArea * sizeof(map), 1, mapFile)
		|| !_fread(*MapAbstract, *MapAbstractArea, 1, mapFile)) {
		return true;
	}
	fixup_landmarks();
	return false;
}

/*
Purpose: Get the region value for coordinates.
Original Offset: 00591210
Return Value: Abstract value
Status: Complete
*/
uint8_t __cdecl abstract_at(int xCoord, int yCoord) {
	return (*MapAbstract)[(xCoord >> 1) + yCoord * (*MapAbstractLongBounds >> 1)];
}

/*
Purpose: Set the region value for coordinates.
Original Offset: 00591230
Return Value: n/a
Status: Complete
*/
void __cdecl abstract_set(int xCoord, int yCoord, uint8_t region) {
	(*MapAbstract)[(xCoord >> 1) + yCoord * (*MapAbstractLongBounds >> 1)] = region;
}

/*
Purpose: Quickly check for Veh related zone of control conflicts. If a ZOC conflict is found, store
		 the coordinates of the tile inside xCoordZoc/yCoordZoc pointers.
Original Offset: 00593830
Return Value: n/a
Status: Complete
*/
void __cdecl quick_zoc(int xCoordSrc, int yCoordSrc, int factionID, int xCoordDst, int yCoordDst,
	int *xCoordZoc, int *yCoordZoc) {
	BOOL isSrcOcean = is_ocean(xCoordSrc, yCoordSrc);
	int searchZoc = -1;
	for (uint32_t i = 0; i < 8; i++) {
		int xRadius = xrange(xCoordSrc + xRadiusBase[i]), yRadius = yCoordSrc + yRadiusBase[i];
		if (on_map(xRadius, yRadius)) {
			int owner = veh_who(xRadius, yRadius);
			if (owner >= 0 && owner != factionID && is_ocean(xRadius, yRadius) == isSrcOcean
				&& !has_treaty(factionID, owner, DTREATY_PACT)) {
				int proximity = vector_dist(xRadius, yRadius, xCoordDst, yCoordDst);
				if (proximity >= searchZoc) {
					searchZoc = proximity;
					*xCoordZoc = xRadius;
					*yCoordZoc = yRadius;
				}
			}
		}
	}
}

/*
Purpose: Determine if the specified offsets are within the range's radius.
Original Offset: 005A65A0
Return Value: -1 if not within range, otherwise range radius
Status: Complete
*/
int __cdecl radius_move(int xRadiusOff, int yRadiusOff, int range) {
	for (int i = 0; i < range; i++) {
		if (xRadiusOff == xRadiusOffset[i] && yRadiusOff == yRadiusOffset[i]) {
			return i; // radius
		}
	}
	return -1;
}

/*
Purpose: Determine if the specified two tiles are within the range's radius of each other.
Original Offset: 005A65D0
Return Value: -1 if not within range, otherwise range radius
Status: Complete
*/
int __cdecl radius_move(int xCoordSrc, int yCoordSrc, int xCoordDst, int yCoordDst, int range) {
	int xRadiusOff = xCoordDst - xCoordSrc;
	if (xRadiusOff < ((int)*MapLongitudeBounds / -2)) {
		xRadiusOff += *MapLongitudeBounds;
	}
	if (xRadiusOff > ((int)*MapLongitudeBounds / 2)) {
		xRadiusOff -= *MapLongitudeBounds;
	}
	return radius_move(xRadiusOff, yCoordDst - yCoordSrc, range);
}

/*
Purpose: Determine if the specified two tiles are within the radius directionally of each other.
Original Offset: 005A6630
Return Value: -1 if not within radius, otherwise radius offset
Status: Complete
*/
int __cdecl compass_move(int xCoordSrc, int yCoordSrc, int xCoordDst, int yCoordDst) {
	int xRadiusOff = xCoordDst - xCoordSrc;
	if (xRadiusOff < ((int)*MapLongitudeBounds / -2)) {
		xRadiusOff += *MapLongitudeBounds;
	}
	if (xRadiusOff > ((int)*MapLongitudeBounds / 2)) {
		xRadiusOff -= *MapLongitudeBounds;
	}
	int yRadiusOff = yCoordDst - yCoordSrc;
	int directionX = (xRadiusOff > 0) ? 1 : (xRadiusOff >= 0) - 1;
	int directionY = (yRadiusOff > 0) ? 1 : (yRadiusOff >= 0) - 1;
	for (int i = 0; i < 9; i++) {
		if (directionX == (xRadiusBase[i] > 0 ? 1 : (xRadiusBase[i] >= 0) - 1)
			&& directionY == (yRadiusBase[i] > 0 ? 1 : (yRadiusBase[i] >= 0) - 1)) {
			return i;
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
		int distX = x_dist(xCoord, Base[baseID].xCoord);
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
	for (uint32_t yCoord = 0; yCoord < *MapLatitudeBounds; yCoord++) {
		for (uint32_t xCoord = yCoord & 1; xCoord < *MapLongitudeBounds; xCoord += 2) {
			if ((bit2_at(xCoord, yCoord) & (BIT2_UNK_80000000 | BIT2_NEXUS)) == BIT2_NEXUS
				&& !code_at(xCoord, yCoord) 
				&& factionID == whose_territory(factionID, xCoord, yCoord, NULL, false)
				&& map_loc(xCoord, yCoord)->visibility & (1 << factionID)) { // tile visible
				return true;
			}
		}
	}
	return false;
}

/*
Purpose: Handling setting altitude for world.
Original Offset: 005C2020
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_alt_set(int xCoord, int yCoord, uint32_t altitude, BOOL isSetBoth) {
	ZeroMemory(MapBaseSubmergedCount, sizeof(uint32_t) * MaxPlayerNum);
	memset(MapBaseIDClosestSubmergedVeh, -1, sizeof(int) * MaxPlayerNum);
	if (on_map(xCoord, yCoord)) {
		isSetBoth ? alt_set_both(xCoord, yCoord, altitude) : alt_set(xCoord, yCoord, altitude);
	}
	BOOL hasSetAlt;
	for (uint32_t i = 1, alt = altitude - 1; i < altitude; i++, alt--) {
		hasSetAlt = false;
		for (uint32_t j = RadiusRange[i]; j < RadiusRange[i + 1]; j++) {
			int xRadius = xrange(xCoord + xRadiusOffset[j]), yRadius = yCoord + yRadiusOffset[j];
			if (on_map(xRadius, yRadius) && alt_at(xRadius, yRadius) < alt) {
				if (isSetBoth) {
					alt_set_both(xRadius, yRadius, alt);
					if (anything_at(xRadius, yRadius) < 0 && !bonus_at(xRadius, yRadius, 0)) {
						owner_set(xRadius, yRadius, -1);
					}
				}
				else {
					alt_set(xRadius, yRadius, alt);
				}
				hasSetAlt = true;
			}
		}
		if (!hasSetAlt) {
			break;
		}
	}
	uint32_t maxAlt = 8 - altitude;
	for (uint32_t i = 1, alt = i + altitude; i < maxAlt; i++, alt++) {
		hasSetAlt = false;
		for (uint32_t j = RadiusRange[i]; j < RadiusRange[i + 1]; j++) {
			int xRadius = xrange(xCoord + xRadiusOffset[j]), yRadius = yCoord + yRadiusOffset[j];
			if (on_map(xRadius, yRadius) && alt_at(xRadius, yRadius) < alt) {
				if (isSetBoth) {
					alt_set_both(xRadius, yRadius, alt);
					if (anything_at(xRadius, yRadius) < 0 && !bonus_at(xRadius, yRadius, 0)) {
						owner_set(xRadius, yRadius, -1);
					}
				}
				else {
					alt_set(xRadius, yRadius, alt);
				}
				hasSetAlt = true;
			}
		}
		if (!hasSetAlt) {
			break;
		}
	}
}

/*
Purpose: Handling raising specified tile's altitude.
Original Offset: 005C2380
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_raise_alt(int xCoord, int yCoord) {
	if (on_map(xCoord, yCoord)) {
		uint32_t altitude = alt_at(xCoord, yCoord);
		if (altitude < 6) {
			world_alt_set(xCoord, yCoord, altitude++, true);
		}
	}
}

/*
Purpose: Handling lowering specified tile's altitude.
Original Offset: 005C23E0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_lower_alt(int xCoord, int yCoord) {
	if (on_map(xCoord, yCoord)) {
		uint32_t altitude = alt_at(xCoord, yCoord);
		if (altitude) {
			world_alt_set(xCoord, yCoord, altitude--, true);
		}
	}
}

/*
Purpose: Set up brush for creating world terrain.
Original Offset: 005C2440
Return Value: n/a
Status: Complete - testing
*/
void __cdecl brush(int xCoord, int yCoord, uint32_t altitude) {
	BOOL drawRadius = false;
	for (int i = 0; i < 4; i++) {
		if (i != 2) {
			int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
			if (on_map(xRadius, yRadius) && alt_at(xRadius, yRadius) != altitude) {
				BOOL skip = false;
				for (uint32_t j = 0; j < 8; j++) {
					int xRadBase = xrange(xRadius + xRadiusBase[j]), 
						yRadBase = yRadius + yRadiusBase[j];
					if (on_map(xRadBase, yRadBase) && !is_ocean(xRadBase, yRadBase)
						&& !region_at(xRadBase, yRadBase)) {
						skip = true;
					}
				}
				if (!skip) {
					if (altitude != 3) {
						for (uint32_t j = 0, k = 0; j < 7; j += 3, k += 2) {
							int xRadBase = xrange(xRadius + xRadiusBase[k + 1]),
								yRadBase = yRadius + yRadiusBase[k + 1];
							if (on_map(xRadBase, yRadBase)
								&& altitude_at(xRadBase, yRadBase) == ALT_BIT_SHORE_LINE) {
								int xRadBase2 = xrange(xRadius + xRadiusBase[(k + 2) % 8]),
									yRadBase2 = yRadius + yRadiusBase[(k + 2) % 8];
								int xRadBase3 = xrange(xRadius + xRadiusBase[k % 8]),
									yRadBase3 = yRadius + yRadiusBase[k % 8];
								if (on_map(xRadBase2, yRadBase2) && on_map(xRadBase3, yRadBase3)
									&& altitude_at(xRadBase2, yRadBase2) < ALT_BIT_SHORE_LINE
									&& altitude_at(xRadBase3, yRadBase3) < ALT_BIT_SHORE_LINE) {
									skip = true;
									break;
								}
							}
						}
					}
					if (!skip) {
						region_set(xRadius, yCoord, 1);
						drawRadius |= alt_at(xRadius, yRadius) != altitude;
						world_alt_set(xRadius, yRadius, altitude, true);
						if (*GameState & STATE_OMNISCIENT_VIEW && altitude > 3) {
							temp_set(xRadius, yRadius, 1);
						}
						*BrushVal1 += 1;
						*BrushVal2 += 1;
					}
				}
			}
		}
	}
	if (*GameState & STATE_OMNISCIENT_VIEW && drawRadius) {
		draw_radius(xCoord, yCoord, 2, 2);
	}
}

/*
Purpose: Set up to paint land for creating world terrain.
Original Offset: 005C27F0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl paint_land(int xCoord, int yCoord, uint32_t altitude, uint32_t radius) {
	*BrushVal2 = 0;
	for (int i = 0; i < 2000 && *BrushVal2 < radius; i++) {
		int search = -1;
		uint32_t unkVal = 0;
		int xRadBase = xCoord, yRadBase = yCoord;
		do {
			brush(xRadBase, yRadBase, altitude);
			uint32_t offset = rand() % 8 | 1;
			if ((int)offset == search) {
				if (++unkVal > ((*MapLandCoverage * *MapLandCoverage) + 2)) {
					offset = (offset - (*MapLandCoverage * *MapLandCoverage) + unkVal - 2) % 8;
				}
			}
			else {
				search = offset;
				unkVal = 0;
			}
			xRadBase = xrange(xCoord + xRadiusBase[offset]), 
				yRadBase = yCoord + yRadiusBase[offset];
			i++;
		} while (i < 2000 && on_map(xRadBase, yRadBase) && *BrushVal2 < radius);
	}
}

/*
Purpose: Build out map continents.
Original Offset: 005C28F0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl build_continent(uint32_t size) {
	for (uint32_t i = 0; i < *MapArea; i++) {
		(*Map)[i].region = 0;
	}
	int coverage = *MapLandCoverage;
	if (coverage && *BrushVal1 >= *WorldBuildVal1) {
		coverage--;
	}
	int radius = WorldBuilder->ContinentMod *coverage *coverage + WorldBuilder->ContinentBase;
	int xCoord, yCoord, i = 0;
	do {
		xCoord = rnd(*MapLongitudeBounds - (*MapIsFlat * 8), NULL) + *MapIsFlat * 4;
		yCoord = rnd(*MapLatitudeBounds - 8, NULL) + 4;
		if (xCoord & 1) {
			xCoord--;
		}
		if (yCoord & 1) {
			xCoord++; // should this have been yCoord?
		}
	} while (i++ < 100 && alt_at(xCoord, yCoord) >= ALT_BIT_SHORE_LINE);
	if (alt_at(xCoord, yCoord) < ALT_BIT_SHORE_LINE) {
		uint32_t ratio = (size * 3200) / *MapArea;
		if (ratio > WorldBuilder->ContSizeRatio5 || *BrushVal1 >= *WorldBuildVal1) {
			radius /= 4;
			if (BrushVal1 >= WorldBuildVal1 && ratio > WorldBuilder->ContSizeRatio5) {
				radius = rand() % 3 + 1;
			}
		}
		else if (ratio <= WorldBuilder->ContSizeRatio4) {
			if (ratio <= WorldBuilder->ContSizeRatio3) {
				if (ratio <= WorldBuilder->ContSizeRatio2) {
					if (ratio < WorldBuilder->ContSizeRatio1 && *MapLandCoverage > 1) {
						radius += radius / 2;
					}
				}
				else  if (rnd(4 - *MapLandCoverage, NULL)) {
					radius /= 2;
				}
			}
			else {
				radius /= 2;
			}
		}
		else {
			radius /= 3;
		}
		if (*BrushVal1 < *WorldBuildVal1 && radius < 4) {
			radius = 4;
		}
		paint_land(xCoord, yCoord, ALT_SHORE_LINE, radius);
		do_all_non_input();
	}
}

/*
Purpose: Build out map hills.
Original Offset: 005C2B40
Return Value: n/a
Status: Complete - testing
*/
void __cdecl build_hills(uint32_t altitude) {
	int xCoord, yCoord, i = 0;
	BOOL keepGoing = true;
	do {
		xCoord = rnd(*MapLongitudeBounds - (*MapIsFlat * 8), NULL) + *MapIsFlat * 4;
		yCoord = rnd(*MapLatitudeBounds - 8, NULL) + 4;
		if (xCoord & 1) {
			xCoord--;
		}
		if (yCoord & 1) {
			xCoord++; // should this have been yCoord?
		}
		i += 2;
		if (i > 100) {
			return;
		}
		for (uint32_t j = 0; j < 9; j++) {
			int xRadius = xrange(xCoord + xRadiusOffset[j]), yRadius = yCoord + yRadiusOffset[j];
			if (on_map(xRadius, yRadius) && alt_at(xRadius, yRadius) < (altitude - 1)) {
				break;
			}
		}
		keepGoing = false;
		int platMod = WorldBuilder->PlateauMod * (2 - *MapOceanCoverage);
		if (altitude != ALT_1_LEVEL_ABOVE_SEA) {
			platMod /= 4;
		}
		uint32_t platBase = WorldBuilder->PlateauBase;
		if (*MapOceanCoverage >= 2) {
			platBase /= 2;
		}
		paint_land(xCoord, yCoord, altitude, platMod + platBase);
		do_all_non_input();
	} while (keepGoing); // better way to set up outer loop?
}

/*
Purpose: Build out world's river beds.
Original Offset: 005C3680
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_riverbeds() {
	for (uint32_t i = 0; i < *MapArea; i++) {
		(*Map)[i].bit &= ~(BIT_RIVERBED);
	}
	uint32_t riverbedCount = 0;
	uint32_t maxRiverbeds = (*MapArea * ((4 - *MapOceanCoverage) * (WorldBuilder->RiversBase 
		+ *MapCloudCover * WorldBuilder->RiversRainMod) / 3)) / 3200;
	for (uint32_t i = 0; i < 4000 && riverbedCount < maxRiverbeds; i++) {
		int xCoord = rnd(*MapLongitudeBounds, NULL), yCoord = rnd(*MapLatitudeBounds, NULL);
		if (xCoord & 1) {
			xCoord--;
		}
		if (yCoord & 1) {
			xCoord++;
		}
		if (!is_ocean(xCoord, yCoord) && !(bit_at(xCoord, yCoord) & BIT_RIVERBED)) {
			uint32_t searchAlt = 0;
			int xCoordSearch = -1, yCoordSearch = -1;
			for (uint32_t j = 0; j < 81; j++) {
				int xRadius = xrange(xCoord + xRadiusOffset[j]);
				int yRadius = yCoord + yRadiusOffset[j];
				if (on_map(xRadius, yRadius)) {
					if (bit_at(xRadius, yRadius) & BIT_RIVERBED) {
						break; // exit if riverbed already in search area
					}
					if (j < 49) { // limit scope of where riverbeds can be in search
						uint32_t alt = alt_at(xRadius, yRadius);
						if (alt > searchAlt) {
							searchAlt = alt;
							xCoordSearch = xRadius; // replaced storing j offset with radius values
							yCoordSearch = yRadius;
						}
					}
				}
			}
			if (xCoordSearch >= 0) {
				bit_set(xCoordSearch, yCoordSearch, BIT_RIVERBED, true);
			}
		}
	}
}

/*
Purpose: Determine if there are any issues with how the world continents are set up.
Original Offset: 005C40F0
Return Value: Are there any issues with the world's continents? true/false
Status: Complete - testing
*/
BOOL __cdecl world_validate() {
	Paths->continents();
	uint32_t val1 = 0, val2 = 0; // rename to better var descriptions
	for (uint32_t region = 1; region < MaxRegionLandNum; region++) {
		if (!bad_reg(region)) {
			uint32_t tiles = Continents[region].tiles;
			if (tiles < val2) {
				if (tiles >= val1) {
					val1 = tiles;
				}
			}
			else {
				val1 = val2;
				val2 = tiles;
			}
		}
	}
	return val1 < ((*MapLandCoverage == 1) ? ((val2 * 2) / 3) : val2 / 2);
}

/*
Purpose: Set up the world's temperature.
Original Offset: 005C4170
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_temperature() {
	random_reseed(*MapRandSeed + 17);
	uint32_t tempHeat = *MapLatitudeBounds / WorldBuilder->SolarEnergy;
	uint32_t thermalBanding = *MapLatitudeBounds / WorldBuilder->ThermalBand;
	uint32_t thermalDeviance = *MapLatitudeBounds / WorldBuilder->ThermalDeviance;
	uint32_t globalWarming = *MapLatitudeBounds / WorldBuilder->GlobalWarming;
	for (uint32_t y = 0; y < *MapLatitudeBounds; y++) {
		for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
			if ((bit2_at(x, y) & (BIT2_UNK_80000000 | BIT2_CRATER)) != BIT2_CRATER
				|| code_at(x, y) >= 21) {
				uint32_t randSeed = random(0, thermalDeviance * 2);
				uint32_t randOrbit = random(0, *MapPlanetaryOrbit + 1);
				int val1 = (*MapLatitudeBounds / 2) - randSeed - y + thermalDeviance;
				if (val1 < 0) { // abs?
					val1 = randSeed - (*MapLatitudeBounds / 2) - thermalDeviance + y;
				}
				int val2 = (thermalBanding / 2 + (val1 - tempHeat * (*MapPlanetaryOrbit - 1) 
					- *MapSeaLevelCouncil * globalWarming) * 2) / thermalBanding;
				uint32_t temperature = (val2 > 2) ? ((val2 <= 9) + 1) : 3;
				uint32_t alt = alt_at(x, y);
				for (uint32_t i = 0; i < 8; i++) {
					int xRadius = xrange(x + xRadiusOffset[i]), yRadius = y + yRadiusOffset[i];
					if (on_map(xRadius, yRadius) && bit_at(xRadius, yRadius) & BIT_THERMAL_BORE
						&& (alt <= ALT_1_LEVEL_ABOVE_SEA || alt_at(xRadius, yRadius) >= alt)) {
						temperature++;
						break;
					}
				}
				if (temperature > 1) {
					if (alt <= ALT_2_LEVELS_ABOVE_SEA) {
						if (alt > ALT_1_LEVEL_ABOVE_SEA && !randOrbit) { // ==ALT_2_LEVELS_ABOVE_SEA
							temperature--;
						}
					}
					else if (*MapPlanetaryOrbit < 2 || !random(0, 2)) {
						temperature--;
					}
				}
				temp_set(x, y, (uint8_t)range(temperature - 1, 0, 2));
			}
		}
		do_all_non_input();
	}
}

/*
Purpose: Analysis of world map.
Original Offset: 005C55C0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_analysis() {
	for (uint32_t i = 0; i < MaxContinentNum; i++) {
		Continents[i].openTerrain = 0;
	}
	for (uint32_t y = 0; y < *MapLatitudeBounds; y++) {
		for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
			uint32_t region = region_at(x, y);
			BOOL isOcean = is_ocean(x, y);
			if (!isOcean && climate_at(x, y) != RAINFALL_ARID
				&& rocky_at(x, y) < TERRAIN_BIT_ROCKY
				&& (!(bit_at(x, y) & BIT_FUNGUS || altitude_at(x, y) < ALT_BIT_OCEAN_SHELF))) {
				Continents[region].openTerrain++;
			}
			bit_set(x, y, BIT_UNK_40000000 | BIT_UNK_4000, false); // verify map is being iterated 
			if (!isOcean) {                                       // correctly for all bit_*
				int searchVal = -1;
				uint32_t countVal1 = 0, countVal2 = 0;
				for (uint32_t i = 0; i < 20; i++) {
					int xRadius = xrange(x + xRadiusOffset[i + 1]);
					int yRadius = y + yRadiusOffset[i + 1];
					if (on_map(xRadius, yRadius)) {
						if (i >= 32) {
							BOOL isOceanRadius = is_ocean(xRadius, yRadius);
							if (isOceanRadius != searchVal || i == 32) {
								searchVal = isOceanRadius;
								countVal1++;
							}
							uint32_t regionRadius;
							if (yRadius > 4 && yRadius < ((int)*MapLatitudeBounds - 4)
								&& (regionRadius = region_at(xRadius, yRadius),
									regionRadius < MaxRegionLandNum && region != regionRadius
									&& Continents[region].tiles > 40
									&& Continents[regionRadius].tiles > 40)) {
								bit_set(x, y, BIT_UNK_4000, true); // here
							}
						}
						else {
							BOOL isOceanRadius = is_ocean(xRadius, yRadius);
							if (isOceanRadius != searchVal) {
								searchVal = isOceanRadius;
								countVal2++;
								if (countVal2 >= 4 && Continents[region].tiles >= 80) {
									bit_set(x, y, BIT_UNK_40000000, true); // here
								}
							}
						}
					}
				}
				if (countVal1 < 4) {
					bit_set(x, y, BIT_UNK_40000000, false); // here
				}
			}
		}
		do_all_non_input();
	}
	for (uint32_t i = 0; i < *MapArea; i++) {
		(*Map)[i].val2 &= 0xF; // clear map sites
	}
}

/*
Purpose: TBD; better function name?
Original Offset: 005C58C0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_alt_put_detail(int xCoord, int yCoord) {
	alt_put_detail(xCoord, yCoord, (uint8_t)AltNatural[3]);
}

/*
Purpose: Set up the world's polar caps.
Original Offset: 005C58E0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_polar_caps() {
	for (uint8_t x = 0; x < *MapLongitudeBounds; x += 2) {
		world_alt_put_detail(x, 0);
		world_alt_put_detail(x - 1, *MapLatitudeBounds - 1);
	}
	uint32_t bounds = *MapLongitudeBounds / 16;
	for (uint32_t i = 0; i < bounds; i++) {
		world_alt_put_detail(rnd(*MapLongitudeBounds / 2, NULL) * 2, 0);
		world_alt_put_detail(rnd(*MapLongitudeBounds / 2, NULL) * 2 + 1, 1);
		world_alt_put_detail(rnd(*MapLongitudeBounds / 2, NULL) * 2 + 1, *MapLatitudeBounds - 1);
		world_alt_put_detail(rnd(*MapLongitudeBounds / 2, NULL) * 2, *MapLatitudeBounds - 2);
	}
}

/*
Purpose: Build out the world's contours.
Original Offset: 005C5AE0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_linearize_contours() {
	for (uint32_t y = 0; y < *MapLatitudeBounds; y++) {
		for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
			uint32_t altNat = alt_natural(x, y);
			alt_put_detail(x, y, (uint8_t)((((ElevDetail[altNat 
				+ (altNat >= ALT_3_LEVELS_ABOVE_SEA) ? 4 : 1] - ElevDetail[altNat]) 
				* (alt_detail_at(x, y) - AltNatural[altNat]))
				/ (AltNatural[altNat + 1] - AltNatural[altNat])) + ElevDetail[altNat]));
		}
		do_all_non_input();
	}
	memcpy_s(&AltNatural, sizeof(uint32_t) * 11, &ElevDetail, sizeof(uint32_t) * 11);
}

/*
Purpose: Determine if the specified tile is near a landmark.
Original Offset: 005C5BD0
Return Value: Is the tile near a landmark? true/false
Status: Complete - testing
*/
BOOL __cdecl near_landmark(int xCoord, int yCoord) {
	for (uint32_t i = 0; i < 289; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius) && code_at(xRadius, yRadius)) {
			return true;
		}
	}
	return false;
}

/*
Purpose: Setup the 'Garland Crater' landmark.
Original Offset: 005C5C70
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_crater(int xCoord, int yCoord) {
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	world_alt_set(xCoord, yCoord, ALT_OCEAN_TRENCH, true);
	world_alt_set(xCoord, yCoord, ALT_3_LEVELS_ABOVE_SEA, true);
	world_alt_set(xCoord, yCoord, ALT_SHORE_LINE, true);
	world_alt_set(xCoord, yCoord, ALT_OCEAN_SHELF, true);
	world_alt_set(xCoord, yCoord, ALT_SHORE_LINE, true);
	uint32_t temperature = temp_at(xCoord, yCoord);
	for (uint32_t i = 0; i < 49; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			bit_set(xRadius, yRadius, BIT_RIVERBED, false);
			bit2_set(xRadius, yRadius, BIT2_CRATER, true);
			code_set(xRadius, yRadius, i);
			if (i < 21) {
				rocky_set(xRadius, yRadius, ROCKINESS_ROLLING);
				temp_set(xRadius, yRadius, (uint8_t)temperature);
			}
		}
	}
	new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_CRATER].name));
}

/*
Purpose: Setup the 'Monsoon Jungle' landmark.
Original Offset: 005C5EF0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_monsoon(int xCoord, int yCoord) {
	world_rainfall();
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			uint32_t landCount;
			do {
				yCoord = *MapLatitudeBounds / 2 + rand() % 4 - 2;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				landCount = 0;
				for (uint32_t i = 0; i < 121; i++) {
					int xRadius = xrange(xCoord + xRadiusOffset[i]);
					int yRadius = yCoord + yRadiusOffset[i];
					if (on_map(xRadius, yRadius) && !is_ocean(xRadius, yRadius)) {
						landCount++;
					}
				}
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord) || !is_coast(xCoord, yCoord, true) || landCount < 40
				|| climate_at(xCoord, yCoord) <= RAINFALL_MOIST);
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	for (uint32_t i = 0; i < 121; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (abs(yRadiusOffset[i]) <= 8 &&  on_map(xRadius, yRadius)) {
			if (i < 21 && is_ocean(xRadius, yRadius)) {
				world_alt_set(xRadius, yRadius, ALT_SHORE_LINE, true);
			}
			bit2_set(xRadius, yRadius, LM_JUNGLE, true);
			code_set(xRadius, yRadius, i);
		}
	}
	new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_JUNGLE].name));
}

/*
Purpose: Setup the 'New Sargasso' landmark.
Original Offset: 005C6200
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_sargasso(int xCoord, int yCoord) {
	uint32_t locAttempts = 0;
	BOOL hasSkipped = false;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (!is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	for (uint32_t i = 0; i < 81; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius) && (is_ocean(xRadius, yRadius) || i < 21)) {
			world_alt_set(xRadius, yRadius, ALT_OCEAN_SHELF, true);
			bit2_set(xRadius, yRadius, BIT2_SARGASSO, true);
			code_set(xRadius, yRadius, i);
			if (i <= 4) {
				bit_set(xRadius, yRadius, BIT_SUPPLY_POD, true);
			}
			if ((hasSkipped || i < 25 || rand() % 24) && (i < 49 || rand() % 3)) {
				bit_set(xRadius, yRadius, BIT_FUNGUS, true);
			}
			else {
				hasSkipped = true;
			}
		}
	}
	new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_SARGASSO].name));
}

/*
Purpose: Setup 'The Ruins' landmark.
Original Offset: 005C64A0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_ruin(int xCoord, int yCoord) {
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	for (uint32_t i = 0; i < 25; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			if (i < 25 || !is_ocean(xRadius, yRadius)) { // is_ocean check redundant? i always < 25
				world_alt_set(xRadius, yRadius, ALT_SHORE_LINE, true);
			}
			if (is_ocean(xRadius, yCoord)) {
				if (i && i < 9) {
					bit_set(xRadius, yRadius, BIT_MONOLITH, true); // in ocean tiles?
				}
			}
			else {
				bit_set(xRadius, yCoord, BIT_FUNGUS | BIT_SUPPLY_REMOVE, true);
				if (!i) {
					bit_set(xRadius, yRadius, BIT_FUNGUS, false);
				}
				else if (i < 9) {
					bit_set(xRadius, yRadius, BIT_MONOLITH, true);
				}
			}
			bit2_set(xRadius, yRadius, BIT2_RUINS, true);
			code_set(xRadius, yRadius, i);
		}
	}
	new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_RUINS].name));
}

/*
Purpose: Setup the 'Great Dunes' landmark.
Original Offset: 005C6740
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_dune(int xCoord, int yCoord) {
	world_rainfall();
	uint32_t locAttempts = 0;
	uint32_t halfVertBounds = *MapLatitudeBounds / 2;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(halfVertBounds, NULL) + halfVertBounds - *MapLatitudeBounds / 4;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord) || climate_at(xCoord, yCoord) != RAINFALL_ARID);
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	for (uint32_t i = 0; i < 25; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]);
		int yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius) && (!is_ocean(xRadius, yRadius) || i < 9 || rand() % 3)) {
			if (i == 2 || i == 7) {
				world_alt_set(xRadius, yRadius, ALT_1_LEVEL_ABOVE_SEA, true);
			}
			else {
				world_alt_set(xRadius, yRadius, ALT_SHORE_LINE, true);
			}
			bit_set(xRadius, yRadius, BIT_FUNGUS, false);
			bit2_set(xRadius, yRadius, LM_DUNES, true);
			rocky_set(xRadius, yRadius, ROCKINESS_FLAT);
			code_set(xRadius, yRadius, i);
		}
	}
	new_landmark(xCoord, yCoord + 2, StringTable->get((int)Natural[LM_DUNES].name));
}

/*
Purpose: Setup the 'Uranium Flats' landmark.
Original Offset: 005C69E0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_diamond(int xCoord, int yCoord) {
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	for (uint32_t i = 0; i < 21; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			if (i < 9 || !is_ocean(xRadius, yRadius) || rand() % 3) {
				world_alt_set(xRadius, yRadius, ALT_SHORE_LINE, true);
				bit2_set(xRadius, yRadius, BIT2_URANIUM, true);
				rocky_set(xRadius, yRadius, ROCKINESS_FLAT);
				bit_set(xRadius, yRadius, BIT_FUNGUS, false);
				code_set(xRadius, yRadius, i);
			}
		}
	}
	new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_URANIUM].name));
}

/*
Purpose: Setup the 'Freshwater Sea' landmark.
Original Offset: 005C6C40
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_fresh(int xCoord, int yCoord) {
	uint32_t region;
	if (on_map(xCoord, yCoord)) {
		region = region_at(xCoord, yCoord);
		if (!is_ocean(xCoord, yCoord)) {
			return;
		}
	}
	else {
		uint32_t tileSearch = 0;
		int regionSearch = -1;
		for (uint32_t i = MaxLandmarkNum; i < MaxContinentNum; i++) {
			uint32_t tiles = Continents[i].tiles;
			if (tiles >= 3 && tiles <= 32 && tiles >= tileSearch) {
				tileSearch = tiles;
				regionSearch = i;
			}
		}
		if (regionSearch < 0) {
			return;
		}
		region = regionSearch;
	}
	int xCoordSearch = -1;
	BOOL hasSetLandmark = false;
	for (uint32_t y = *MapLatitudeBounds - 1; y >= 0 ; y--) {
		for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
			if (region_at(x, y) == region) {
				bit2_set(x, y, LM_FRESH, true);
				if (xCoordSearch < 0) {
					xCoordSearch = x;
				}
			}
			else if(!hasSetLandmark && xCoordSearch >= 0) {
				int xCoordFresh = (xCoordSearch + x - 2) / 2;
				new_landmark(((xCoordFresh ^ y) & 1) ^ xCoordFresh, y, 
					StringTable->get((int)Natural[LM_FRESH].name));
				hasSetLandmark = true;
			}
		}
	}
}

/*
Purpose: Setup the 'Mount Planet' landmark.
Original Offset: 005C6DB0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_volcano(int xCoord, int yCoord, BOOL isNotLandmark) {
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	world_alt_set(xCoord, yCoord, ALT_BIT_OCEAN_TRENCH, true); // purpose??
	world_alt_set(xCoord, yCoord, ALT_3_LEVELS_ABOVE_SEA, true);
	for (uint32_t i = 0; i < 9; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			bit2_set(xRadius, yRadius, BIT2_VOLCANO, true);
			code_set(xRadius, yRadius, i);
			uint32_t bit = bit_at(xRadius, yCoord) & ~(BIT_SUPPLY_POD | BIT_THERMAL_BORE 
				| BIT_ECH_MIRROR | BIT_CONDENSER | BIT_SOIL_ENRICHER | BIT_FARM | BIT_RIVERBED
				| BIT_SOLAR_TIDAL | BIT_FUNGUS | BIT_MINE | BIT_MAGTUBE | BIT_ROAD);
			if (!i) {
				bit &= ~(BIT_UNK_4000000 | BIT_UNK_8000000);
			}
			bit |= BIT_SUPPLY_REMOVE;
			bit_put(xRadius, yRadius, bit);
			rocky_set(xRadius, yRadius, ROCKINESS_ROCKY);
		}
	}
	*MountPlanetXCoord = xCoord;
	*MountPlanetYCoord = yCoord;
	if (!isNotLandmark) {
		new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_VOLCANO].name));
	}
}

/*
Purpose: Setup the 'Borehole Cluster' landmark. Added to SMAC in 3.0 patch.
Original Offset: 005C7020
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_borehole(int xCoord, int yCoord) {
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	uint32_t seed = timeGetTime();
	uint32_t val1 = 8;
	uint32_t val2 = ((seed % 8) / 3) + 5;
	int val3 = 3 - ((seed % 8) % 3);
	uint32_t val4 = (seed / 8) % 4;
	if (val4 & 2) {
		val2--;
		val3--;
		if (val4 & 1) {
			val2++;
			val1 = 8;
			val3++;
		}
		else {
			val2--;
			val1 = 6;
			val3--;
		}
		val1 = (val1 + 8) % 8 + 1;
		val2 = (val2 + 8) % 8 + 1;
		val3 = (val3 + 8) % 8 + 1;
	}
	for (uint32_t i = 0; i < 9; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			world_alt_set(xRadius, yRadius, ALT_SHORE_LINE, true);
			bit_set(xRadius, yRadius, BIT_SUPPLY_REMOVE, true);
			if (i == val1 || i == val2 || i == (uint32_t)val3) {
				bit_set(xRadius, yRadius, BIT_THERMAL_BORE, true);
				bit2_set(xRadius, yRadius, LM_BOREHOLE, true);
				code_set(xRadius, yRadius, i);
			}
		}
	}
	int xCoordOffsetVal1 = xrange(xCoord + xRadiusOffset[val1]); // removed from loop
	int yCoordOffsetVal1 = yCoord + yRadiusOffset[val1]; // removed from loop
	for (int i = 0; i < 8; i++) {
		int xRadius = xrange(xCoordOffsetVal1 + xRadiusOffset[i + 1]);
		int yRadius = yCoordOffsetVal1 + yRadiusOffset[i + 1];
		if (on_map(xRadius, yRadius) && is_ocean(xRadius, yRadius)) {
			world_alt_set(xRadius, yRadius, ALT_SHORE_LINE, true);
		}
	}
	int xCoordOffsetVal2 = xrange(xCoord + xRadiusOffset[val2]); // removed from loop
	int yCoordOffsetVal2 = yCoord + yRadiusOffset[val2]; // removed from loop
	for (int i = 0; i < 8; i++) {
		int xRadius = xrange(xCoordOffsetVal2 + xRadiusOffset[i + 1]);
		int yRadius = yCoordOffsetVal2 + yRadiusOffset[i + 1];
		if (on_map(xRadius, yRadius) && is_ocean(xRadius, yRadius)) {
			world_alt_set(xRadius, yRadius, ALT_SHORE_LINE, true);
		}
	}
	int xCoordOffsetVal3 = xrange(xCoord + xRadiusOffset[val3]); // removed from loop
	int yCoordOffsetVal3 = yCoord + yRadiusOffset[val3]; // removed from loop
	for (int i = 0; i < 8; i++) {
		int xRadius = xrange(xCoordOffsetVal3 + xRadiusOffset[i + 1]);
		int yRadius = yCoordOffsetVal3 + yRadiusOffset[i + 1];
		if (on_map(xRadius, yRadius) && is_ocean(xRadius, yRadius)) {
			world_alt_set(xRadius, yRadius, ALT_SHORE_LINE, true);
		}
	}
	bit2_set(xCoord, yCoord, LM_BOREHOLE, true);
	new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_BOREHOLE].name));
}

/*
Purpose: Setup 'The Manifold Nexus' landmark. Added to SMAC in 4.0 patch.
Original Offset: 005C7540
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_temple(int xCoord, int yCoord) {
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	for (uint32_t i = 0; i < 9; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			world_alt_set(xRadius, yRadius, ALT_SHORE_LINE, true);
			bit_set(xRadius, yRadius, BIT_SUPPLY_REMOVE, true);
			bit2_set(xRadius, yRadius, LM_NEXUS, true);
			code_set(xRadius, yRadius, i);
		}
	}
	new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_NEXUS].name));
}

/*
Purpose: Setup the 'Unity Wreckage' landmark (SMACX only).
Original Offset: 005C7750
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_unity(int xCoord, int yCoord) {
	if (*ExpansionEnabled) {
		uint32_t locAttempts = 0;
		if (!on_map(xCoord, yCoord)) {
			do {
				do {
					yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
					int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
					xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
					if (++locAttempts >= 1000) {
						return;
					}
				} while (is_ocean(xCoord, yCoord));
			} while (near_landmark(xCoord, yCoord));
			if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
				return;
			}
		}
		xCoord--;
		yCoord--;
		for (uint32_t i = 0; i < 25; i++) {
			int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
			if (on_map(xRadius, yRadius)) {
				world_alt_set(xRadius, yRadius, ALT_1_LEVEL_ABOVE_SEA, true);
			}
		}
		xCoord += 2;
		yCoord += 2;
		for (uint32_t i = 0; i < 25; i++) {
			int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
			if (on_map(xRadius, yRadius)) {
				world_alt_set(xRadius, yRadius, ALT_1_LEVEL_ABOVE_SEA, true);
			}
		}
		xCoord--;
		yCoord--;
		for (uint32_t i = 0; i < 25; i++) {
			int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
			if (on_map(xRadius, yRadius)) {
				rocky_set(xRadius, yRadius, ROCKINESS_FLAT);
				bit_set(xRadius, yRadius, BIT_RIVERBED, false);
				bit_set(xRadius, yRadius, BIT_FUNGUS, false);
				bit2_set(xRadius, yRadius, LM_UNITY, true);
				code_set(xRadius, yRadius, i);
				if (!i || i == 8 || i == 10 || i == 19) {
					bit_set(xRadius, yRadius, BIT_SUPPLY_POD, true);
				}
				else {
					bit_set(xRadius, yRadius, BIT_SUPPLY_REMOVE, true);
				}
			}
		}
		new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_UNITY].name));
	}
}

/*
Purpose: Setup the 'Fossil Ridge' landmark (SMACX only).
Original Offset: 005C7A80
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_fossil(int xCoord, int yCoord) {
	if (*ExpansionEnabled) { // added check similar to Unity Wreckage
		uint32_t locAttempts = 0;
		if (!on_map(xCoord, yCoord)) {
			do {
				do {
					yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
					int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
					xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
					if (++locAttempts >= 1000) {
						return;
					}
				} while (!is_ocean(xCoord, yCoord));
			} while (near_landmark(xCoord, yCoord));
			if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
				return;
			}
		}
		for (uint32_t i = 0; i < 6; i++) {
			int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
			if (on_map(xRadius, yRadius) && is_ocean(xRadius, yRadius)) {
				world_alt_set(xRadius, yRadius, ALT_OCEAN, true);
				bit2_set(xRadius, yRadius, LM_FOSSIL, true);
				code_set(xRadius, yRadius, i);
			}
		}
		new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_FOSSIL].name));
	}
}

/*
Purpose: Setup the 'Nessus Canyon' landmark.
Original Offset: 005C7CB0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_canyon(int xCoord, int yCoord) {
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	} 
	const uint32_t WorldCanyonOffsetValues[12] = { 26, 27, 9, 1, 2, 0, 3, 4, 17, 23, 36, 35 };
	for (uint32_t i = 0; i < 12; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[WorldCanyonOffsetValues[i]]);
		int yRadius = yCoord + yRadiusOffset[WorldCanyonOffsetValues[i]];
		if (on_map(xRadius, yRadius)) {
			world_alt_set(xRadius, yRadius, ALT_1_LEVEL_ABOVE_SEA, true);
			world_alt_set(xRadius, yRadius, ALT_SHORE_LINE, true);
			bit2_set(xRadius, yRadius, LM_CANYON, true); // rolled these two into single loop with
			code_set(xRadius, yRadius, WorldCanyonOffsetValues[i]); // the above world_alt_set()
		}
	}
	new_landmark(xCoord, yCoord + 2, StringTable->get((int)Natural[LM_CANYON].name));
}

/*
Purpose: Setup the 'Sunny Mesa' landmark.
Original Offset: 005C7F40
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_mesa(int xCoord, int yCoord) {
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	for (uint32_t i = 0; i < 49; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius)) {
			world_alt_set(xRadius, yRadius, (i < 25) + ALT_SHORE_LINE, true);
			bit2_set(xRadius, yRadius, LM_MESA, true);
			code_set(xRadius, yRadius, i);
		}
	}
	new_landmark(xCoord, yCoord + 2, StringTable->get((int)Natural[LM_MESA].name));
}

/*
Purpose: Setup the 'Pholus Ridge' landmark.
Original Offset: 005C8150
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_ridge(int xCoord, int yCoord) {
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	const uint32_t WorldRidgeOffsetValues[13] = { 47, 44, 24, 20, 8, 7, 0, 5, 4, 17, 23, 35, 455 };
	for (uint32_t i = 0; i < 12; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[WorldRidgeOffsetValues[i]]);
		int yRadius = yCoord + yRadiusOffset[WorldRidgeOffsetValues[i]];
		if (on_map(xRadius, yRadius)) {
			world_alt_set(xRadius, yRadius, ALT_1_LEVEL_ABOVE_SEA, true);
			bit2_set(xRadius, yRadius, LM_RIDGE, true); // rolled these two into single loop with
			code_set(xRadius, yRadius, WorldRidgeOffsetValues[i]); // the above world_alt_set()
		}
	}
	new_landmark(xCoord, yCoord + 2, StringTable->get((int)Natural[LM_RIDGE].name));
}

/*
Purpose: Setup the 'Geothermal Shallows' landmark.
Original Offset: 005C83B0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_geothermal(int xCoord, int yCoord) {
	uint32_t locAttempts = 0;
	if (!on_map(xCoord, yCoord)) {
		do {
			do {
				yCoord = rnd(*MapLatitudeBounds - 16, NULL) + 8;
				int xCoordSeed = rnd(*MapLongitudeBounds, NULL);
				xCoord = ((xCoordSeed ^ yCoord) & 1) ^ xCoordSeed;
				if (++locAttempts >= 1000) {
					return;
				}
			} while (!is_ocean(xCoord, yCoord));
		} while (near_landmark(xCoord, yCoord));
		if (locAttempts >= 1000) { // redundant check? would have already returned, remove?
			return;
		}
	}
	BOOL hasSkipped = false;
	for (uint32_t i = 0; i < 25; i++) {
		int xRadius = xrange(xCoord + xRadiusOffset[i]), yRadius = yCoord + yRadiusOffset[i];
		if (on_map(xRadius, yRadius) && (is_ocean(xRadius, yRadius) || i < 9)) {
			if ((hasSkipped || !i || rand() % 25) && (i < 9 || rand() % 3)) {
				world_alt_set(xRadius, yRadius, ALT_OCEAN_SHELF, true);
				bit2_set(xRadius, yRadius, LM_GEOTHERMAL, true);
				code_set(xRadius, yRadius, i);
			}
			else {
				hasSkipped = true;
			}

			world_alt_set(xRadius, yRadius, (i < 25) + ALT_SHORE_LINE, true);
			bit2_set(xRadius, yRadius, LM_MESA, true);
			code_set(xRadius, yRadius, i);
		}
	}
	new_landmark(xCoord, yCoord, StringTable->get((int)Natural[LM_GEOTHERMAL].name));
}

/*
Purpose: Setup all landmarks.
Original Offset: 005C8630
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_landmarks() {
	world_monsoon(-1, -1);
	world_crater(-1, -1);
	world_volcano(-1, -1, false);
	world_mesa(-1, -1);
	world_ridge(-1, -1);
	world_diamond(-1, -1);
	world_ruin(-1, -1);
	world_unity(-1, -1); // removed redundant ExpansionEnabled check
	world_fossil(-1, -1); // moved ExpansionEnabled check to inside function
	world_temple(-1, -1);
	world_borehole(-1, -1);
	world_sargasso(-1, -1);
	world_dune(-1, -1);
	world_fresh(-1, -1);
	world_geothermal(-1, -1);
	world_canyon(-1, -1); // Change: add "Nessus Canyon" to random map generation
	fixup_landmarks();
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
		if (on_map(xRadius, yRadius)) {
			int owner = anything_at(xRadius, yRadius);
			if (owner >= 0 && owner != (int)factionID
				&& !has_treaty(factionID, owner, DTREATY_PACT)) {
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
		if (on_map(xRadius, yRadius)) {
			int owner = veh_who(xRadius, yRadius);
			if (owner >= 0 && owner != (int)factionID
				&& !has_treaty(factionID, owner, DTREATY_PACT)) {
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
		if (on_map(xRadius, yRadius)) {
			int owner = veh_who(xRadius, yRadius);
			if (owner >= 0 && owner != (int)factionID && is_ocean(xRadius, yRadius) == isOcean
				&& !has_treaty(factionID, owner, DTREATY_PACT)) {
				for (int vehID = veh_at(xRadius, yRadius); vehID >= 0;
					vehID = Veh[vehID].nextVehIDStack) {
					if (Veh[vehID].factionID != factionID
						&& (Veh[vehID].visibility & (1 << factionID)
							|| (!*IsMultiplayerNet && !(Veh[vehID].flags & VFLAG_INVISIBLE)))) {
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

/*
Purpose: Take two points and calculate the cursor distance between them. Optimized out of x86 code.
Original Offset: n/a
Return Value: Cursor distance
Status: Complete
*/
int __cdecl cursor_dist(int xCoordA, int yCoordA, int xCoordB, int yCoordB) {
	return (x_dist(xCoordA, xCoordB) + abs(yCoordA - yCoordB)) / 2;
}

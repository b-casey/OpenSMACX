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
#include "path.h"
#include "alpha.h"
#include "game.h"
#include "technology.h"
#include "veh.h"
#include "map.h"

/*
Purpose: Initialize instance of Path class.
Original Offset: 0059A220
Return Value: n/a
Status: Complete
*/
void Path::init() {
	shutdown();
	xCoordDst = -1;
	yCoordDst = -1;
	mapTable = (int *)mem_get(*MapArea * 4);
	xCoordTable = (int16_t *)mem_get(*MapArea * 2);
	yCoordTable = (int16_t *)mem_get(*MapArea * 2);
	ZeroMemory(mapTable, *MapArea * 4);
}

/*
 Purpose: Shutdown instance of Path class.
 Original Offset: 0059A2D0
 Return Value: n/a
 Status: Complete
*/
void Path::shutdown() {
	if (mapTable) {
		free(mapTable);
	}
	if (xCoordTable) {
		free(xCoordTable);
	}
	if (yCoordTable) {
		free(yCoordTable);
	}
	mapTable = 0;
	xCoordTable = 0;
	yCoordTable = 0;
}

/*
 Purpose: Get the value from the map table for the specified coordinates.
 Original Offset: n/a ; Optimized out of x86 binary, present in PPC.
 Return Value: mapTable value
 Status: Complete
*/
int Path::get(int xCoord, int yCoord) {
	return mapTable[(xCoord >> 1) + yCoord * *MapLongitude];
}

/*
 Purpose: Set the value in the map table for the specified coordinates.
 Original Offset: n/a
 Return Value: n/a
 Status: Complete
*/
void Path::set(int xCoord, int yCoord, int val) {
	mapTable[(xCoord >> 1) + yCoord * *MapLongitude] = val;
}

/*
 Purpose: Check for Veh related zone of control conflicts taking into account land or ocean. This is
		  a slightly modified version of zoc_move() / zoc_sea().
 Original Offset: 0059A370
 Return Value: zoc: factionID + 1; no zoc: 0 (however return seems to be treated as boolean)
 Status: Complete
*/
int Path::zoc_path(int xCoord, int yCoord, int factionID) {
	if (bit_at(xCoord, yCoord) & BIT_BASE_IN_TILE && owner_at(xCoord, yCoord) < 8) {
		return 0;
	}
	BOOL isOcean = is_ocean(xCoord, yCoord);
	for (uint32_t i = 0; i < 8; i++) {
		int xRadius = xrange(xCoord + RadiusBaseX[i]), yRadius = yCoord + RadiusBaseY[i];
		if (on_map(xRadius, yRadius)) {
			int owner = veh_who(xRadius, yRadius);
			if (owner >= 0 && owner != factionID && is_ocean(xRadius, yRadius) == isOcean
				&& !has_treaty(factionID, owner, DTREATY_VENDETTA | DTREATY_PACT)) {
				if (!is_human(factionID)) {
					return owner + 1;
				}
				for (int vehID = veh_at(xRadius, yRadius); vehID >= 0;
					vehID = Vehs[vehID].next_veh_id_stack) {
					if ((Vehs[vehID].faction_id == factionID && ((Vehs[vehID].flags
						& (VFLAG_INVISIBLE | VFLAG_LURKER)) != (VFLAG_INVISIBLE | VFLAG_LURKER)))
						|| Vehs[vehID].visibility & (1 << factionID)) {
						return owner + 1;
					}
				}
			}
		}
	}
	return 0;
}

/*
 Purpose: Find path between points and conditions?
 Original Offset: 0059A530
 Return Value: TBD
 Status: WIP
*/
int Path::find(int xCoordSrc, int yCoordSrc, int xCoordDstA, int yCoordDstA, int protoID_,
	int factionID, int unk1, int unk2) {
	return 0;
}

/*
 Purpose: TBD
 Original Offset: 0059BC10
 Return Value: TBD
 Status: WIP
*/
int Path::move(int vehID, int factionID) {
	int factionIDVeh = Vehs[vehID].faction_id;
	uint32_t flags = 0xC0;
	if (factionIDVeh != factionID1) {
		xCoordDst = -1;
		yCoordDst = -1;
		factionID1 = factionIDVeh;
	}
	int xCoordVeh = Vehs[vehID].x, yCoordVeh = Vehs[vehID].y,
		xCoordWayPt = Vehs[vehID].waypoint_x[0], yCoordWayPt = Vehs[vehID].waypoint_y[0];

	if (!on_map(xCoordWayPt, yCoordWayPt) || (xCoordVeh == xCoordWayPt 
		&& yCoordVeh == yCoordWayPt)) {
		return -1;
	}
	BOOL isHuman = is_human(factionIDVeh);
	if (!isHuman && ((Vehs[vehID].state & (VSTATE_UNK_40000 | VSTATE_UNK_20000))
		== (VSTATE_UNK_40000 | VSTATE_UNK_20000))) {
		flags = 0xE0;
	}
	if ((Vehs[vehID].state & (VSTATE_UNK_1000000 | VSTATE_UNK_200))
		== (VSTATE_UNK_1000000 | VSTATE_UNK_200)) {
		flags &= 0xBF;
	}
	else {
		uint32_t vehMoves = veh_moves(vehID);
		if (vehMoves <= Rules->move_rate_roads) {
			flags |= 0x100;
			if (vehMoves <= (uint32_t)((Vehs[vehID].proto_id != BSC_MIND_WORMS) + 1)) {
				flags |= 0x100;
			}
		}
	}

	return 0;
}

/*
 Purpose: Populate the abstract map with the radial region value.
 Original Offset: 0059C200
 Return Value: n/a
 Status: Complete
*/
void Path::make_abstract() {
	for (uint32_t y = 0; y < *MapAbstractLatBounds; y++) {
		for (uint32_t x = y & 1; x < *MapAbstractLongBounds; x += 2) {
			uint8_t region = 0;
			for (uint32_t i = 0; i < 9; i++) {
				int xRadius = xrange(x * 5 + RadiusOffsetX[i]), yRadius = y * 5 + RadiusOffsetY[i];
				if (on_map(xRadius, yRadius) && !is_ocean(xRadius, yRadius)) {
					region = (uint8_t)region_at(xRadius, yRadius);
					break;
				}
			}
			abstract_set(x, y, region);
		}
	}
}

/*
 Purpose: Merge the old region into the new region.
 Original Offset: 0059C340
 Return Value: n/a
 Status: Complete
*/
void Path::merge(uint32_t regionOld, uint32_t regionNew) {
	Continents[regionNew].tile_count += Continents[regionOld].tile_count;
	Continents[regionOld].tile_count = 0;
	for (uint32_t i = 0; i < *MapArea; i++) {
		if ((uint32_t)(*MapTiles)[i].region == regionOld) {
			(*MapTiles)[i].region = (uint8_t)regionNew;
		}
	}
}

/*
 Purpose: Build the pathing tables using the provided coordinates to radiate outward for connected 
          land tiles owned by the specified faction. The region parameter is unused.
 Original Offset: 0059C3C0
 Return Value: n/a
 Status: Complete
*/
void Path::territory(int xCoord, int yCoord, int UNUSED(region), int factionID) {
	if (is_ocean(xCoord, yCoord)) {
		return; // skip ocean terrain
	}
	index1 = 0; index2 = 0;
	xCoordTable[index1] = (int16_t)xCoord;
	yCoordTable[index1++] = (int16_t)yCoord;
	do {
		int16_t xCoordIt = xCoordTable[index2], yCoordIt = yCoordTable[index2++];
		for (uint32_t i = 0; i < 8; i++) {
			int xRadius = xrange(xCoordIt + RadiusBaseX[i]);
			int yRadius = yCoordIt + RadiusBaseY[i];
			Map *tile;
			if (on_map(xRadius, yRadius) && yCoordIt && yCoordIt != ((int)*MapLatitudeBounds - 1)
				&& !is_ocean(xRadius, yRadius) && (tile = map_loc(xRadius, yRadius),
					!tile->unk_1 && tile->territory == factionID)) {
				tile->unk_1 = 1;
				xCoordTable[index1] = (int16_t)xRadius;
				yCoordTable[index1++] = (int16_t)yRadius;
			}
		}
	} while (index2 && index1 != index2);
	do_all_non_input();
}

/*
 Purpose: Populate the continent and path table for the specified tile and region.
 Original Offset: 0059C520
 Return Value: n/a
 Status: Complete
*/
void Path::continent(int xCoord, int yCoord, uint32_t region) {
	Continents[region].tile_count = 0;
	index1 = 0; index2 = 0;
	uint32_t freshWaterCount = 0;
	xCoordTable[index1] = (int16_t)xCoord;
	yCoordTable[index1++] = (int16_t)yCoord;
	region_set(xCoord, yCoord, (uint8_t)region);
	BOOL isOcean = is_ocean(xCoord, yCoord);
	do {
		int16_t xCoordIt = xCoordTable[index2], yCoordIt = yCoordTable[index2++];
		Continents[region].tile_count++;
		for (uint32_t i = 0; i < 8; i++) {
			int xRadius = xrange(xCoordIt + RadiusBaseX[i]);
			int yRadius = yCoordIt + RadiusBaseY[i];
			BOOL isOceanRad;
			if (on_map(xRadius, yRadius) && yCoordIt && yCoordIt != ((int)*MapLatitudeBounds - 1)
				&& (isOceanRad = is_ocean(xRadius, yRadius), isOceanRad == isOcean 
					&& !region_at(xRadius, yRadius))) {
				if (isOcean && bit2_at(xRadius, yRadius) & BIT2_FRESH && isOceanRad) {
					freshWaterCount++;
				}
				region_set(xRadius, yRadius, (uint8_t)region);
				xCoordTable[index1] = (int16_t)xRadius;
				yCoordTable[index1++] = (int16_t)yRadius;
			}
		}
	} while (index2 && index1 != index2);
	if (freshWaterCount) {
		BOOL isFreshWater = freshWaterCount >= ((Continents[region].tile_count * 3) / 4); // land locked?
		for (uint32_t y = 0; y < *MapLatitudeBounds; y++) {
			for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
				if (region_at(x, y) == region) {
					bit2_set(x, y, BIT2_FRESH, isFreshWater);
				}
			}
		}
	}
	do_all_non_input();
}

/*
 Purpose: Populate and set up all continents and path tables.
 Original Offset: 0059C790
 Return Value: n/a
 Status: Complete
*/
void Path::continents() {
	for (uint32_t i = 0; i < *MapArea; i++) {
		(*MapTiles)[i].region = 0;
	}
	uint32_t yCoordSouthPole = *MapLatitudeBounds - 1;
	uint32_t oceanRegion = 64;
	uint32_t landRegion = 0;
	for (uint32_t y = 1; y < yCoordSouthPole; y++) {
		for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
			if (!region_at(x, y)) {
				uint32_t regionCurrent, regionMin, regionMax;
				if (is_ocean(x, y)) {
					oceanRegion++;
					regionMin = 64;
					regionMax = 126;
					regionCurrent = range(oceanRegion, regionMin, regionMax);
				}
				else {
					landRegion++;
					regionMin = 0;
					regionMax = 62;
					regionCurrent = range(landRegion, regionMin, regionMax);
				}
				continent(x, y, regionCurrent);
				if (regionCurrent == regionMax) {
					uint32_t tiles = Continents[regionCurrent].tile_count;
					int searchRegion = -1;
					for (uint32_t i = regionMin; i < regionMax; i++) {
						if (Continents[i].tile_count < tiles) {
							tiles = Continents[i].tile_count;
							searchRegion = i;
						}
					}
					if (searchRegion >= 0) {
						merge(searchRegion, regionMax + 1);
						merge(regionCurrent, searchRegion);
					}
					else {
						merge(regionCurrent, regionMax + 1);
					}
				}
			}
		}
	}
	for (uint32_t x = 0; x < *MapLongitudeBounds; x += 2) { // north pole
		uint8_t poleRegion = is_ocean(x, 0) ? 127 : 63;
		region_set(x, 0, poleRegion);
		Continents[poleRegion].tile_count++;
	}
	for (uint32_t x = yCoordSouthPole & 1; x < *MapLongitudeBounds; x += 2) { // south pole
		uint8_t poleRegion = is_ocean(x, yCoordSouthPole) ? 127 : 63;
		region_set(x, yCoordSouthPole, poleRegion);
		Continents[poleRegion].tile_count++;
	}
	uint32_t mostTiles = 0, totalTiles = 0;
	for (uint32_t i = 1; i < 63; i++) {
		uint32_t tiles = Continents[i].tile_count;
		totalTiles += tiles;
		if (tiles > mostTiles) {
			mostTiles = tiles;
		}
	}
	*GameState = (mostTiles >= ((totalTiles * 4) / 5))
		? *GameState | STATE_UNK_100 : *GameState & ~STATE_UNK_100;
	for (uint32_t i = 0; i < MaxRegionLandNum; i++) {
		ZeroMemory(Continents[i].sea_coasts, 8);
	}
	for (uint32_t y = 0; y < *MapLatitudeBounds; y++) {
		for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
			uint32_t region = region_at(x, y);
			if (region < MaxRegionLandNum) {
				for (uint32_t i = 0; i < 8; i++) {
					int xRadius = xrange(x + RadiusBaseX[i]), yRadius = y + RadiusBaseY[i];
					if (on_map(xRadius, yRadius)) {
						uint32_t regionRad = region_at(xRadius, yRadius);
						if (regionRad >= MaxRegionLandNum) {
							uint32_t offset, mask;
							bitmask(regionRad - MaxRegionLandNum, &offset, &mask);
							Continents[region].sea_coasts[offset] |= mask;
							i += 2 - (i & 1);
						}
					}
				}
			}
		}
	}
	make_abstract();
	do_all_non_input();
}

/*
 Purpose: TBD
 Original Offset: 0059CCA0
 Return Value: true/false
 Status: Complete - testing
*/
BOOL Path::sensors(int factionID, int *xCoordPtr, int *yCoordPtr) {
	BOOL isSensor = true;
	ZeroMemory(mapTable, *MapArea * 4);
	xCoordDst = -1;
	yCoordDst = -1;
	int xCoord = *xCoordPtr, yCoord = *yCoordPtr;
	int search = 9999;
	uint32_t region = region_at(xCoord, yCoord);
	for (uint32_t y = 1; y < *MapAbstractLatBounds - 1; y++) {
		for (uint32_t x = y & 1; x < *MapAbstractLongBounds; x += 2) {
			int factionIDVeh;
			if (region_at(x, y) == region
				&& whose_territory(factionID, x, y, NULL, false) == factionID
				&& base_who(x, y) < 0 && (factionIDVeh = veh_who(x, y), factionIDVeh < 0 
					|| factionIDVeh == factionID
					|| has_treaty(factionID, factionIDVeh, DTREATY_PACT))) {
				int factionIDZoc = zoc_veh(x, y, factionID);
				uint32_t bit = bit_at(x, y);
				Map *tile = map_loc(x, y);
				if (factionIDZoc != 1 && (!factionIDZoc
					|| has_treaty(factionID, factionIDZoc, DTREATY_PACT))
					&& bit & (BIT_MINE | BIT_SOLAR_TIDAL)
					&& bit & (BIT_MONOLITH | BIT_CONDENSER | BIT_THERMAL_BORE)
					&& !bonus_at(x, y, 0) && ((tile->val3 & 0xC0u) > TERRAIN_BIT_ROLLING
						|| climate_at(x, y) == RAINFALL_ARID
						|| !(bit & BIT_FUNGUS) || !is_ocean(x, y)) 
					&& (!(bit & BIT_FUNGUS) || (!is_ocean(x, y) 
						&& has_tech(Rules->tech_improve_fungus_sqr, factionID)))) {
					uint32_t flags = 0;
					for (uint32_t i = 0; i < 25; i++) {
						int xRadius = xrange(x + RadiusOffsetX[i]);
						int yRadius = y + RadiusOffsetY[i];
						if (on_map(xRadius, yRadius) && !is_sensor(xRadius, yRadius)
							&& (whose_territory(factionID, xRadius, yRadius, NULL, false)
								== factionID || get(xRadius, yRadius))) {
							if (i >= 9) {
								flags |= 1;
							}
							flags |= 2;
						}
					}
					if (!(flags & 1)) {
						int proxminity = vector_dist(xCoord, yCoord, x, y);
						for (uint32_t i = 0; i < 25; i++) {
							int xRadius = xrange(x + RadiusOffsetX[i]), tileFactionID;
							int yRadius = y + RadiusOffsetY[i];
							if (on_map(xRadius, yRadius)
								&& (tileFactionID = base_who(xRadius, yRadius), 
									tileFactionID == factionID)) {
								flags |= 4;
								BOOL check = true;
								for (uint32_t j = 0; j < 25; j++) {
									int xRadius2 = xrange(xRadius + RadiusOffsetX[j]);
									int yRadius2 = yRadius + RadiusOffsetY[j];
									if (on_map(xRadius2, yRadius2) && !is_sensor(xRadius2, yRadius2) 
										&& (whose_territory(factionID, xRadius2, yRadius2, NULL,
											false) == factionID || get(xRadius2, yRadius2))) {
										check = false;
										break;
									}
								}
								if (check) {
									flags |= 8;
								}
							}
						}
						if (flags & 4) {
							proxminity += 400;
						}
						if (flags & 8) {
							proxminity += 800;
						}
						if (!(flags & 2) || flags & 8) {
							proxminity++;
							set(x, y, 0);
						}
						else {
							proxminity += 200;
						}
						if (proxminity < search) {
							*xCoordPtr = x;
							*yCoordPtr = y;
							search = proxminity;
							isSensor = false;
						}
					}
				}
			}
		}
	}
	do_all_non_input();
	return isSensor;
}

// global
Path *Paths = (Path *)0x00945B00;

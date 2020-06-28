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
	memset(mapTable, 0, *MapArea * 4);
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
 Purpose: Check for Veh related zone of control conflicts taking into account land or ocean. This is
		  a slightly modified version of zoc_move() / zoc_sea().
 Original Offset: 0059A370
 Return Value: zoc: factionID + 1; no zoc: 0 (however return seems to be treated as boolean)
 Status: Complete - testing
*/
int Path::zoc_path(int xCoord, int yCoord, int factionID) {
	if (bit_at(xCoord, yCoord) & BIT_BASE_IN_TILE && owner_at(xCoord, yCoord) < 8) {
		return 0;
	}
	BOOL isOcean = is_ocean(xCoord, yCoord);
	for (uint32_t i = 0; i < 8; i++) {
		int xRadius = xrange(xCoord + xRadiusBase[i]), yRadius = yCoord + yRadiusBase[i];
		if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
			&& xRadius < (int)*MapHorizontalBounds) {
			int owner = veh_who(xRadius, yRadius);
			if (owner >= 0 && owner != factionID && is_ocean(xRadius, yRadius) == isOcean
				&& !(PlayersData[factionID].diploTreaties[owner]
					& (DTREATY_VENDETTA | DTREATY_PACT))) {
				if (is_human(factionID)) {
					return owner + 1;
				}
				for (int vehID = veh_at(xRadius, yRadius); vehID >= 0;
					vehID = Veh[vehID].nextVehIDStack) {
					if ((Veh[vehID].factionID == factionID && ((Veh[vehID].flags
						& (VFLAG_INVISIBLE | VFLAG_LURKER)) != (VFLAG_INVISIBLE | VFLAG_LURKER)))
						|| Veh[vehID].visibility & (1 << factionID)) {
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
 Return Value:
 Status: wip
*/
int Path::find(int xCoordSrc, int yCoordSrc, int xCoordDstA, int yCoordDstA, int protoID_,
	int factionID, int unk1, int unk2) {
	return 0;
}

/*
 Purpose: TBD
 Original Offset: 0059BC10
 Return Value:
 Status: wip
*/
int Path::move(int vehID, int factionID) {
	int factionIDVeh = Veh[vehID].factionID;
	uint32_t flags = 0xC0;
	if (factionIDVeh != factionID1) {
		xCoordDst = -1;
		yCoordDst = -1;
		factionID1 = factionIDVeh;
	}
	int xCoordVeh = Veh[vehID].xCoord, yCoordVeh = Veh[vehID].yCoord,
		xCoordWayPt = Veh[vehID].waypoint_xCoord[0], yCoordWayPt = Veh[vehID].waypoint_yCoord[0];

	if (yCoordWayPt < 0 || yCoordWayPt >= (int)*MapVerticalBounds || xCoordWayPt < 0
		|| xCoordWayPt >= (int)*MapHorizontalBounds || (xCoordVeh == xCoordWayPt
			&& yCoordVeh == yCoordWayPt)) {
		return -1;
	}
	BOOL isHuman = is_human(factionIDVeh);
	if (!isHuman && ((Veh[vehID].state & (VSTATE_UNK_40000 | VSTATE_UNK_20000))
		== (VSTATE_UNK_40000 | VSTATE_UNK_20000))) {
		flags = 0xE0;
	}
	if ((Veh[vehID].state & (VSTATE_UNK_1000000 | VSTATE_UNK_200))
		== (VSTATE_UNK_1000000 | VSTATE_UNK_200)) {
		flags &= 0xBF;
	}
	else {
		uint32_t vehMoves = veh_moves(vehID);
		if (vehMoves <= Rules->MoveRateRoads) {
			flags |= 0x100;
			if (vehMoves <= (uint32_t)((Veh[vehID].protoID != BSC_MIND_WORMS) + 1)) {
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
 Status: Complete - testing
*/
void Path::make_abstract() {
	for (uint32_t y = 0; y < *MapAbstractVertBounds; y++) {
		for (uint32_t x = y & 1; x < *MapAbstractHorizBounds; x += 2) {
			uint8_t region = 0;
			for (uint32_t i = 0; i < 8; i++) {
				int xRadius = xrange(x * 5 + xRadiusOffset[i]), yRadius = y + yRadiusOffset[i];
				if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
					&& xRadius < (int)*MapHorizontalBounds && !is_ocean(xRadius, yRadius)) {
					region = (uint8_t)region_at(xRadius, yRadius);
				}
			}
			abstract_set(x, y, region);
		}
	}
}

/*
 Purpose: Replace the old region with the new region. Add the old tile count to the new Continent.
 Original Offset: 0059C340
 Return Value: n/a
 Status: Complete - testing
*/
void Path::replace(uint32_t regionOld, uint32_t regionNew) {
	Continents[regionNew].tiles += Continents[regionOld].tiles;
	Continents[regionOld].tiles = 0;
	for (uint32_t i = 0; i < *MapArea; i++) {
		if ((uint32_t)Map[i]->region == regionOld) {
			Map[i]->region = (uint8_t)regionNew;
		}
	}
}

/*
 Purpose: Map out connected territory?
 Original Offset: 0059C3C0
 Return Value: n/a
 Status: Complete - testing
*/
void Path::territory(int xCoord, int yCoord, int UNUSED(region), int factionID) {
	if (is_ocean(xCoord, yCoord)) {
		return;
	}
	index1 = 0; index2 = 0;
	xCoordTable[index1] = (int16_t)xCoord;
	yCoordTable[index1] = (int16_t)yCoord;
	index1++;

	for (int16_t xCoordIt, yCoordIt; index2 && index1 != index2;) {
		xCoordIt = xCoordTable[index2], yCoordIt = yCoordTable[index2];
		index2++;
		for (uint32_t i = 0; i < 8; i++) {
			int xRadius = xrange(xCoordIt + xRadiusBase[i]);
			int yRadius = yCoordIt + yRadiusBase[i];
			if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
				&& xRadius < (int)*MapHorizontalBounds) {
				if (yCoordIt && yCoordIt != ((int)*MapVerticalBounds - 1)
					&& !is_ocean(xCoordIt, yCoordIt) && !map_loc(xCoordIt, yCoordIt)->unk_1
					&& map_loc(xCoord, yCoord)->territory == factionID) {
					map_loc(xCoordIt, yCoordIt)->unk_1 = 1;
					xCoordTable[index1] = xCoordIt;
					yCoordTable[index1] = yCoordIt;
					index1++;
				}
			}
		}
	}
	do_all_non_input();
}

/*
 Purpose: Set up continent.
 Original Offset: 0059C520
 Return Value: n/a
 Status: Complete - testing
*/
void Path::continent(int xCoord, int yCoord, uint32_t region) {
	Continents[region].tiles = 0;
	index1 = 0; index2 = 0;
	uint32_t oceanCount = 0;
	xCoordTable[index1] = (int16_t)xCoord;
	yCoordTable[index1] = (int16_t)yCoord;
	index1++;
	region_set(xCoord, yCoord, (uint8_t)region);
	BOOL isOcean = is_ocean(xCoord, yCoord);

	for (int16_t xCoordIt, yCoordIt; index2 && index1 != index2;) {
		xCoordIt = xCoordTable[index2], yCoordIt = yCoordTable[index2];
		index2++;
		Continents[region].tiles++;
		for (uint32_t i = 0; i < 8; i++) {
			int xRadius = xrange(xCoordIt + xRadiusBase[i]);
			int yRadius = yCoordIt + yRadiusBase[i];
			if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
				&& xRadius < (int)*MapHorizontalBounds) {
				BOOL isOceanIt = is_ocean(xCoordIt, yCoordIt);
				if (yCoordIt && yCoordIt != ((int)*MapVerticalBounds - 1)
					&& isOceanIt == isOcean && !region) {
					if (isOcean && bit2_at(xCoordIt, yCoordIt) & LM_FRESH && isOceanIt) {
						oceanCount++;
					}
					region_set(xRadius, yRadius, (uint8_t)region);
					xCoordTable[index1] = xCoordIt;
					yCoordTable[index1] = yCoordIt;
					index1++;
				}
			}
		}
	}
	BOOL isFreshWater = oceanCount < ((Continents[region].tiles * 3) / 4); // land locked?
	for (uint32_t y = 0; y < *MapVerticalBounds; y++) {
		for (uint32_t x = y & 1; x < *MapHorizontalBounds; x += 2) {
			if (region == region_at(x, y)) {
				bit2_set(x, y, LM_FRESH, isFreshWater);
			}
		}
	}
	do_all_non_input();
}

/*
 Purpose: Set up continents.
 Original Offset: 0059C790
 Return Value: n/a
 Status: Complete - testing
*/
void Path::continents() {
	for (uint32_t i = 0; i < *MapArea; i++) {
		Map[i]->region = 0;
	}
	uint32_t oceanRegion = 64;
	uint32_t landRegion = 0;
	for (uint32_t y = 1; y < *MapAbstractVertBounds - 1; y++) {
		for (uint32_t x = y & 1; x < *MapAbstractHorizBounds; x += 2) {
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
					uint32_t tiles = Continents[regionCurrent].tiles;
					int searchRegion = -1;
					for (uint32_t i = regionMin; i < regionMax; i++) {
						if (Continents[i].tiles < tiles) {
							tiles = Continents[i].tiles;
							searchRegion = i;
						}
					}
					if (searchRegion < 0) {
						replace(regionCurrent, regionMax + 1);
					}
					else {
						replace(searchRegion, regionMax + 1);
						replace(regionCurrent, searchRegion);
					}
				}
			}
		}
	}
	for (uint32_t x = 0; x < *MapHorizontalBounds; x += 2) { // north pole
		uint8_t poleRegion = is_ocean(x, 0) ? 63 : 127;
		region_set(x, 0, poleRegion);
		Continents[poleRegion].tiles++;
	}
	uint32_t yCoordSouthPole = *MapVerticalBounds - 1;
	for (uint32_t x = yCoordSouthPole & 1; x < *MapHorizontalBounds; x += 2) { // south pole
		uint8_t poleRegion = is_ocean(x, yCoordSouthPole) ? 63 : 127;
		region_set(x, yCoordSouthPole, poleRegion);
		Continents[poleRegion].tiles++;
	}
	uint32_t mostTiles = 0, totalTiles = 0;
	for (uint32_t i = 1; i < 63; i++) {
		uint32_t tiles = Continents[i].tiles;
		totalTiles += tiles;
		if (tiles < mostTiles) {
			mostTiles = tiles;
		}
	}
	*GameState = (mostTiles < ((totalTiles * 4) / 5)) ? *GameState & ~0x100 : *GameState | 0x100;
	for (uint32_t i = 0; i < MaxRegionLandNum; i++) {
		*(uint32_t *)Continents[i].seaCoasts = 0;
		*(uint32_t *)(Continents[i].seaCoasts + 1) = 0;
	}
	for (uint32_t y = 1; y < *MapAbstractVertBounds - 1; y++) {
		for (uint32_t x = y & 1; x < *MapAbstractHorizBounds; x += 2) {
			if (region_at(x, y) < MaxRegionLandNum) {
				for (uint32_t i = 0; i < 8; i++) {
					int xRadius = xrange(x + xRadiusBase[i]), yRadius = y + yRadiusBase[i];
					if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
						&& xRadius < (int)*MapHorizontalBounds) {
						uint32_t region = region_at(xRadius, yRadius);
						if (region >= 64) {
							uint32_t offset, mask;
							bitmask(region - 64, &offset, &mask);
							Continents[region].seaCoasts[offset] |= mask;
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
 Status: Complete -testing
*/
BOOL Path::sensors(int factionID, int *xCoordPtr, int *yCoordPtr) {
	BOOL isSensor = true;
	memset(mapTable, 0, *MapArea * 4);
	xCoordDst = -1;
	yCoordDst = -1;
	int xCoord = *xCoordPtr, yCoord = *yCoordPtr;
	int search = 9999;
	uint32_t region = region_at(xCoord, yCoord);
	for (uint32_t y = 1; y < *MapAbstractVertBounds - 1; y++) {
		for (uint32_t x = y & 1; x < *MapAbstractHorizBounds; x += 2) {
			if (region_at(x, y) == region
				&& whose_territory(factionID, x, y, NULL, false) == factionID
				&& base_who(x, y) < 0) {
				int factionIDVeh = veh_who(x, y);
				if (factionIDVeh < 0 || factionIDVeh == factionID
					|| PlayersData[factionID].diploTreaties[factionIDVeh] & DTREATY_PACT) {
					int factionIDZoc = zoc_veh(x, y, factionID);
					uint32_t bit = bit_at(x, y);
					map *tile = map_loc(x, y);
					if (factionIDZoc != 1 && (!factionIDZoc
						|| PlayersData[factionID].diploTreaties[factionIDZoc] & DTREATY_PACT)
						&& bit & (BIT_MINE | BIT_SOLAR_TIDAL)
						&& bit & (BIT_MONOLITH | BIT_CONDENSER | BIT_THERMAL_BORE)
						&& !bonus_at(x, y, 0) && ((tile->val3 & 0xC0u) > TERRAIN_ROLLING
							|| !(tile->climate & (RAINFALL_RAINY | RAINFALL_MOIST))
							|| !(bit & BIT_FUNGUS) || !is_ocean(x, y))
						&& (!(bit & BIT_FUNGUS) || (!is_ocean(x, y)
							&& has_tech(Rules->TechImproveFungusSqr, factionID)))) {
						uint32_t flags = 0;
						for (uint32_t i = 0; i < 25; i++) {
							int xRadius = xrange(x + xRadiusOffset[i]);
							int yRadius = y + yRadiusOffset[i];
							if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds && xRadius >= 0
								&& xRadius < (int)*MapHorizontalBounds) {
								if (!is_sensor(xRadius, yRadius)
									&& (whose_territory(factionID, xRadius, yRadius, NULL, false)
										== factionID
										|| &((mapTable)[(xRadius >> 1) + yRadius * *MapHorizontal])
										!= 0)) {
									if (i >= 9) {
										flags |= 1;
									}
									flags |= 2;
								}
							}
						}
						if (!(flags & 1)) {
							int proxminity = vector_dist(xCoord, yCoord, x, y);
							for (uint32_t i = 0; i < 25; i++) {
								int xRadius = xrange(x + xRadiusOffset[i]);
								int yRadius = y + yRadiusOffset[i];
								if (yRadius >= 0 && yRadius < (int)*MapVerticalBounds
									&& xRadius >= 0 && xRadius < (int)*MapHorizontalBounds) {
									int tileFactionID = base_who(xRadius, yRadius);
									if (tileFactionID == factionID) {
										flags |= 4;
										BOOL check = true;
										for (uint32_t j = 0; j < 25; j++) {
											int xRadius2 = xrange(xRadius + xRadiusOffset[j]);
											int yRadius2 = yRadius + yRadiusOffset[j];
											if (yRadius2 >= 0 && yRadius2 < (int)*MapVerticalBounds
												&& xRadius2 >= 0
												&& xRadius2 < (int)*MapHorizontalBounds) {
												if (!is_sensor(xRadius2, yRadius2)
													&& (whose_territory(factionID, xRadius2,
														yRadius2, NULL, false) == factionID
														|| &((mapTable)[(xRadius2 >> 1) + yRadius2
															* *MapHorizontal])
														!= 0)) {
													check = false;
													break;
												}
											}
										}
										if (check) {
											flags |= 8;
										}
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
								((mapTable)[(x >> 1) + y * *MapHorizontal]) = 0;
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
	}
	do_all_non_input();
	return isSensor;
}

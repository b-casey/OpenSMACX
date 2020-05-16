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
    xCoordDst1 = -1;
    yCoordDst1 = -1;
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
				&& !(PlayersData[factionID].diploStatus[owner] & (DSTATUS_VENDETTA | DSTATUS_PACT))) {
                if (FactionCurrentBitfield[0] & (1 << factionID)) {
                    return owner + 1;
                }
				for (int vehID = veh_at(xRadius, yRadius); vehID >= 0; 
                    vehID = Veh[vehID].nextVehIDStack) {
					if ((Veh[vehID].factionID == factionID && ((Veh[vehID].flags 
                        & (VFLAG_INVISIBLE | VFLAG_LURKER)) != (VFLAG_INVISIBLE | VFLAG_LURKER))) 
                        || Veh[vehID].visibleToFaction & (1 << factionID)) {
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
int Path::find(int xCoordSrc, int yCoordSrc, int xCoordDst, int yCoordDst, int protoID, 
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
    return 0;
}

/*
 Purpose: TBD
 Original Offset: 0059C200
 Return Value: n/a
 Status: wip
*/
void Path::make_abstract() {
    for (uint32_t y = 0; y < *MapAbstractVertBounds; y++) {
        for (uint32_t x = y & 1; x < *MapAbstractHorizBounds; x+=2) {
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
 Purpose: Fixup of region values? Merge continents?
 Original Offset: 0059C340
 Return Value: n/a
 Status: wip
*/
void Path::UNK1(uint32_t region1, uint32_t region2) {
    Continents[region2].unk1 += Continents[region1].unk1;
    Continents[region1].unk1 = 0;
    for (uint32_t i = 0; i < *MapArea; i++) {
        if ((uint32_t)Map[i]->region == region1) {
            Map[i]->region = (uint8_t)region2;
        }
    }
}

/*
 Purpose: TBD
 Original Offset: 0059C3C0
 Return Value: n/a
 Status: wip
*/
void Path::territory(int xCoord, int yCoord, int region, int factionID) {
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
                    && !is_ocean(xCoordIt, yCoordIt) && !map_loc(xCoordIt, yCoordIt)->unk1
					&& map_loc(xCoord, yCoord)->territory == factionID) {
					map_loc(xCoordIt, yCoordIt)->unk1 = 1;
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
 Purpose: TBD
 Original Offset: 0059C520
 Return Value: n/a
 Status: wip
*/
void Path::continent(int xCoord, int yCoord, uint32_t region) {
    Continents[region].unk1 = 0;
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
        Continents[region].unk1++;
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
    if (oceanCount) {
        BOOL setBit = (int)oceanCount < ((Continents[region].unk1 * 3) / 4);
        for (uint32_t i = 0; i < *MapVerticalBounds; i++) {
            //
        }
    }
    do_all_non_input();
}

/*
 Purpose: TBD
 Original Offset: 0059C790
 Return Value: n/a
 Status: wip
*/
void Path::continents() {
    //
}

/*
 Purpose: TBD
 Original Offset: 0059CCA0
 Return Value: n/a
 Status: wip
*/
void Path::sensors(int factionID, int *xCoordOut, int *yCoordOut) {
    //
}
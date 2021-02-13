/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2021 Brendan Casey
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

int *MapLongitudeBounds = (int *)0x00949870;
int *MapLatitudeBounds = (int *)0x00949874;
uint32_t *MapRandSeed = (uint32_t *)0x00949878;
int *MapSeaLevel = (int *)0x0094987C;
int *MapSeaLevelCouncil = (int *)0x00949880;
uint32_t *MapArea = (uint32_t *)0x00949884;
uint32_t *MapAreaSqRoot = (uint32_t *)0x00949888;
BOOL *MapIsFlat = (BOOL *)0x0094988C;
int *MapLandmarkCount = (int *)0x00949890;
Landmark *MapLandmark = (Landmark *)0x00949894; // [64]
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
Map **MapTiles = (Map **)0x0094A30C;
uint8_t **MapAbstract = (uint8_t **)0x0094A310;
int *MapBaseSubmergedCount = (int *)0x009B2290; // [8]; reset each time global alt changes
int *MapBaseIdClosestSubmergedVeh = (int *)0x009B22BC; // [8]; reset each time global alt changes
uint32_t *BrushVal1 = (uint32_t *)0x009B22B0;
uint32_t *BrushVal2 = (uint32_t *)0x009B22B8;
uint32_t *WorldBuildVal1 = (uint32_t *)0x009B22B4;

Continent *Continents = (Continent *)0x009AA730; // [128]
RulesNatural *Natural = (RulesNatural *)0x0094ADE0;
uint32_t *MapLongitude = (uint32_t *)0x0068FAF0; // default set to 1
uint32_t *AltNatural = (uint32_t *)0x0068FB4C;
LPCSTR MapExtension = "MP";

/*
Purpose: Check whether the coordinates are on the map.
Original Offset: 004712A0
Return Value: Are the coordinates on the map? true/false
Status: Complete
*/
BOOL __cdecl on_map(int x, int y) {
    return y >= 0 && y < (int)*MapLatitudeBounds && x >= 0 && x < (int)*MapLongitudeBounds;
}

/*
Purpose: Bounds check and handling of x coordinate for round maps.
Original Offset: 0048BEE0
Return Value: X coordinate
Status: Complete
*/
int __cdecl xrange(int x) {
    if (!*MapIsFlat) {
        if (x >= 0) {
            if (x >= (int)*MapLongitudeBounds) {
                x -= *MapLongitudeBounds;
            }
        } else {
            x += *MapLongitudeBounds;
        }
    }
    return x;
}

/*
Purpose: Check who owns a tile. Optional parameter to get closest base.
Original Offset: 004E3EF0
Return Value: Faction id of the territory's owner or -1 if no owner or unknown faction
Status: Complete
*/
int __cdecl whose_territory(uint32_t faction_id, uint32_t x, uint32_t y, int *base_id, 
                            BOOL ignore_comm) {
    int owner = map_loc(x, y)->territory;
    if (owner <= 0) {
        return -1; // no owner
    }
    if (faction_id != (uint32_t)owner) {
        if (!ignore_comm && !(*GameState & STATE_OMNISCIENT_VIEW)
            && has_treaty(faction_id, owner, DTREATY_COMMLINK | DTREATY_UNK_8000000)
                != (DTREATY_COMMLINK | DTREATY_UNK_8000000)) {
            return -1; // owner unknown to faction
        }
        if (base_id) {
            *base_id = base_find(x, y, -1, region_at(x, y), -1, -1);
        }
    }
    return owner;
}

/*
Purpose: Find closest base to territory owned by another faction not at war with specified faction.
Original Offset: 004E3FA0
Return Value: Base id or -1
Status: Complete
*/
int __cdecl base_territory(uint32_t faction_id, uint32_t x, uint32_t y) {
    int base_id;
    int owner = whose_territory(faction_id, x, y, &base_id, false);
    if (owner >= 0 && (uint32_t)owner != faction_id && (is_human(faction_id) || is_human(owner))
        && !has_treaty(faction_id, owner, DTREATY_VENDETTA)) {
        return base_id;
    }
    return -1;
}

/*
Purpose: For the specified tile, calculate the quality of the terrain.
Original Offset: 004ECB90
Return Value: Quality of terrain, lower is better (0-2)
Status: Complete
*/
uint32_t __cdecl crappy(uint32_t x, uint32_t y) {
    uint32_t poor_quality = 0;
    uint32_t rainfall = climate_at(x, y);
    if (rainfall == RAINFALL_ARID) {
        poor_quality = 1; // neither moist or rainy
    }
    uint32_t rocky = rocky_at(x, y);
    if (rocky == TERRAIN_BIT_ROCKY) {
        poor_quality++; // rocky
    } else if (rocky == ROCKINESS_FLAT && rainfall < RAINFALL_RAINY) {
        poor_quality++; // flat, moist or arid
    }
    return poor_quality;
}

/*
Purpose: Take the absolute distance between two points as parameters to calculate how far out they
         radiate. This is mainly used to determine proximity or how far away the two points are from
         each other in a rough circle shape (see RadiusOffsetX[]/RadiusOffsetY[]).
Original Offset: 004F8090
Return Value: Distance radius
Status: Complete
*/
int __cdecl vector_dist(int x_distance, int y_distance) {
    x_distance = abs(x_distance);
    y_distance = abs(y_distance);
    int largest = x_distance;
    if (x_distance <= y_distance) {
        largest = y_distance;
    }
    int smallest = x_distance;
    if (x_distance >= y_distance) {
        smallest = y_distance;
    }
    return largest - (((y_distance + x_distance) / 2) - smallest + 1) / 2;
}

/*
Purpose: Take two points and calculate how far out they radiate. This is mainly used to determine
         proximity or how far away the two points are from each other in a rough circle shape.
Original Offset: 005A5910
Return Value: Distance radius
Status: Complete
*/
int __cdecl vector_dist(int x_point_a, int y_point_a, int x_point_b, int y_point_b) {
    return vector_dist(x_dist(x_point_a, x_point_b), abs(y_point_a - y_point_b));
}

/*
Purpose: Check whether there is a path between two regions. It seems to only take into account land
         destinations. TODO: Revisit in the future when Continent/Path is complete.
Original Offset: 0050DDC0
Return Value: Is there a path? true/false
Status: Complete
*/
BOOL __cdecl sea_coast(uint32_t region_dst, uint32_t region_src) {
    uint32_t offset;
    uint32_t mask;
    bitmask(region_src & RegionBounds, &offset, &mask);
    return (Continents[region_dst].sea_coasts[offset] & mask) != 0;
}

/*
Purpose: Count the number of paths from the source region. It seems to only take into account land 
         source and destination ranges. TODO: Revisit in the future when Continent/Path is complete.
Original Offset: 0050DE00
Return Value: Sea coasts valid path count
Status: Complete
*/
uint32_t __cdecl sea_coasts(uint32_t region_src) {
    uint32_t sea_coast_count = 0;
    for (int i = 1; i < RegionBounds; i++) {
        if (sea_coast(i, region_src)) {
            sea_coast_count++;
        }
    }
    return sea_coast_count;
}

/*
Purpose: Check to see whether base is within a one tile radius of a sea tile with specified region.
         If you pass a land region (<63) as the 2nd parameter, it is possible to get collision
         behavior due to region bounding. TODO: Revisit in the future to see whether to remove them.
Original Offset: 0050DE50
Return Value: Is base connected to specified sea region? true/false
Status: Complete
*/
BOOL __cdecl base_on_sea(uint32_t base_id, uint32_t region_sea) {
    region_sea &= RegionBounds;
    if (region_sea >= RegionBounds) { // change to equals since already bounded?
        return false; // skips poles (land or ocean)
    }
    int x = Bases[base_id].x;
    int y = Bases[base_id].y;
    for (uint32_t i = 0; i < 8; i++) {
        int x_radius = xrange(x + RadiusBaseX[i]);
        int y_radius = y + RadiusBaseY[i];
        if (on_map(x_radius, y_radius) && is_ocean(x_radius, y_radius)
            && (region_at(x_radius, y_radius) & RegionBounds) == region_sea) {
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
int __cdecl base_coast(uint32_t base_id) {
    int region = -1;
    int val = 0;
    int x = Bases[base_id].x;
    int y = Bases[base_id].y;
    for (uint32_t i = 0; i < 8; i++) { // is_coast()
        int x_radius = xrange(x + RadiusBaseX[i]);
        int y_radius = y + RadiusBaseY[i];
        if (on_map(x_radius, y_radius) && is_ocean(x_radius, y_radius)) {
            region = region_at(x_radius, y_radius);
            int compare = (region >= 127) ? 1 : Continents[region].tile_count;
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
BOOL __cdecl port_to_coast(uint32_t base_id, uint32_t region) {
    int x = Bases[base_id].x;
    int y = Bases[base_id].y;
    if (region_at(x, y) == region) {
        return true;
    }
    for (uint32_t i = 0; i < 8; i++) { // is_coast()
        int x_radius = xrange(x + RadiusBaseX[i]);
        int y_radius = y + RadiusBaseY[i];
        if (on_map(x_radius, y_radius) && is_ocean(x_radius, y_radius)) {
            if (sea_coast(region, region_at(x_radius, y_radius))) {
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
BOOL __cdecl port_to_port(uint32_t base_id_src, uint32_t base_id_dst) {
    int x = Bases[base_id_src].x;
    int y = Bases[base_id_src].y;
    int last_region = -1;
    for (uint32_t i = 0; i < 8; i++) { // is_coast()
        int x_radius = xrange(x + RadiusBaseX[i]);
        int y_radius = y + RadiusBaseY[i];
        if (on_map(x_radius, y_radius) && is_ocean(x_radius, y_radius)) {
            int region_src = region_at(x_radius, y_radius);
            if (region_src != last_region) { // reduce redundant checks especially sea bases
                last_region = region_src;
                if (base_on_sea(base_id_dst, region_src)) {
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
BOOL __cdecl transport_base(uint32_t base_id) {
    int region = base_coast(base_id);
    if (region < 0) {
        return false;
    }
    if (is_ocean(Bases[base_id].x, Bases[base_id].y)) {
        return true;
    }
    return (sea_coasts(region) > 1);
}

/*
Purpose: Determine if there are other faction's ports in the vicinity of the specified base.
Original Offset: 0050E3C0
Return Value: Does base have a strategic naval importance? true/false
Status: Complete
*/
BOOL __cdecl naval_base(uint32_t base_id) {
    if (base_coast(base_id) < 0 || *BaseCurrentCount <= 0) {
        return false;
    }
    uint32_t faction_id = Bases[base_id].faction_id_current;
    for (int i = 0; i < *BaseCurrentCount; i++) {
        if (faction_id != Bases[i].faction_id_current) {
            if (port_to_port(base_id, i)) {
                return true;
            }
        }
    }
    return false;
}

/*
Purpose: Determine if specified unit can set up a convoy route with specified base.
Original Offset: 0050E5C0
Return Value: Is a convoy route possible? true/false
Status: Complete
*/
BOOL __cdecl convoy(uint32_t veh_id, uint32_t base_id) {
    int home_base_id = Vehs[veh_id].home_base_id;
    if (home_base_id < 0 || base_id == (uint32_t)home_base_id) {
        return false;
    }
    uint32_t triad = get_triad(veh_id);
    if (triad == TRIAD_AIR) {
        return true;
    }
    uint32_t region_base = region_at(Bases[base_id].x, Bases[base_id].y);
    if (region_at(Bases[home_base_id].x, Bases[home_base_id].y) == region_base
        && ((region_base >= 64) == (triad == TRIAD_SEA))) {
        return true; // same region, by land or sea
    }
    if (triad) {
        return port_to_port(base_id, home_base_id); // sea
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
Purpose: Determine whether specified unit can physically reach the destination coordinates.
Original Offset: 0056B320
Return Value: Can unit reach tile? true/false
Status: Complete
*/
BOOL __cdecl get_there(uint32_t veh_id, uint32_t x_dst, uint32_t y_dst) {
    uint32_t triad = get_triad(veh_id);
    if (triad == TRIAD_AIR) {
        return true;
    }
    int x_src = Vehs[veh_id].x; 
    int y_src = Vehs[veh_id].y;
    uint32_t region_src = region_at(x_src, y_src);
    uint32_t region_dst = region_at(x_dst, y_dst);
    if (!triad) { // TRIAD_LAND
        return (region_src == region_dst);
    }
    int base_id_src = base_at(x_src, y_src);
    int base_id_dst = base_at(x_dst, y_dst);
    if (base_id_dst >= 0) {
        return (base_id_src < 0) ? base_on_sea(base_id_dst, region_src)
            : port_to_port(base_id_src, base_id_dst);
    }
    if (is_ocean(x_dst, y_dst)) {
        return (base_id_src < 0) ? (region_src == region_dst) 
            : base_on_sea(base_id_src, region_dst);
    }
    return (base_id_src < 0) ? sea_coast(region_dst, region_src)
        : port_to_coast(base_id_src, region_dst);
}

/*
Purpose: Determine whether point A is a coast or border tile. It seems that the point B check is
         effectively disabled since usage of this function passes same coordinates for both points.
         Modified return value to boolean rather than returning i. It is always treated as boolean
         and makes a lot more sense than returning the iterator position.
Original Offset: 0056B480
Return Value: Is point A considered a border or coast? true/false
Status: Complete
*/
BOOL __cdecl coast_or_border(uint32_t x_point_a, uint32_t y_point_a, uint32_t x_point_b, 
                             uint32_t y_point_b, uint32_t faction_id) {
    if ((int)faction_id != whose_territory(faction_id, x_point_a, y_point_a, NULL, false)) {
        return false; // faction doesn't control point A
    }
    uint32_t region_a = region_at(x_point_a, y_point_a);
    uint32_t region_b = region_at(x_point_b, y_point_b);
    for (uint32_t i = 1; i < 9; i++) {
        int x_radius = xrange(x_point_a + RadiusOffsetX[i]);
        int y_radius = y_point_a + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius) && (is_ocean(x_radius, y_radius)
                || whose_territory(faction_id, x_radius, y_radius, NULL, false) != (int)faction_id
                || region_a != region_b)) { // not sure about reason for this
            return true; // modified to boolean rather than returning i
        }
    }
    return false;
}

/*
Purpose: Get the map tile for the specified coordinates.
Original Offset: n/a
Return Value: Pointer to map tile
Status: Complete
*/
Map *__cdecl map_loc(uint32_t x, uint32_t y) {
    return &((*MapTiles)[(x >> 1) + y * *MapLongitude]);
}

/*
Purpose: Get the temperature of the specified tile.
Original Offset: n/a
Return Value: Temperature
Status: Complete
*/
uint32_t __cdecl temp_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->climate & 7;
}

/*
Purpose: Set the temperature for the specified tile.
Original Offset: 00591AD0
Return Value: n/a
Status: Complete
*/
void __cdecl temp_set(uint32_t x, uint32_t y, uint8_t temperature) {
    Map *tile = map_loc(x, y);
    tile->climate &= 0xF8;
    tile->climate |= temperature & 7;
}

/*
Purpose: Get the rainfall (climate) of the specified tile.
Original Offset: n/a
Return Value: Rainfall (climate)
Status: Complete
*/
uint32_t __cdecl climate_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->climate & (RAINFALL_RAINY | RAINFALL_MOIST);
}

/*
Purpose: Set the rainfall (climate) for the specified tile.
Original Offset: 00591A80
Return Value: n/a
Status: Complete
*/
void __cdecl climate_set(uint32_t x, uint32_t y, uint8_t rainfall) {
    Map *tile = map_loc(x, y);
    tile->climate &= 0xE7;
    tile->climate |= (rainfall & 3) << 3;
    tile->bit2 |= 0x400000; // TODO: identify value
    *UnkBitfield1 |= 1; // TODO: identify global + value
}

/*
Purpose: Calculate the elevation of the specified tile.
Original Offset: 005919C0
Return Value: Elevation (bounded to: -3000 to 3500)
Status: Complete
*/
int __cdecl elev_at(uint32_t x, uint32_t y) {
    uint32_t contour = alt_detail_at(x, y);
    int elev = 50 * (contour - ElevDetail[3] - *MapSeaLevel);
    elev += (contour <= ElevDetail[alt_at(x, y)]) ? 10 
        : (x * 113 + y * 217 + *MapSeaLevel * 301) % 50;
    return range(elev, -3000, 3500);
}

/*
Purpose: Calculate the natural altitude of the specified tile.
Original Offset: 005918A0
Return Value: Natural altitude on a scale from 0 (ocean trench) to 6 (mountain tops)
Status: Complete
*/
uint32_t __cdecl alt_natural(uint32_t x, uint32_t y) {
    uint32_t contour = alt_detail_at(x, y) - *MapSeaLevel;
    uint32_t natural = ALT_3_LEVELS_ABOVE_SEA;
    while (contour < AltNatural[natural] && natural) {
        natural--;
    }
    return natural;
}

/*
Purpose: Set both the altitude and natural altitude for the specified tile. The altitude_natural
         parameter can be between 0 to 9.
Original Offset: 005918F0
Return Value: n/a
Status: Complete
*/
void __cdecl alt_set_both(uint32_t x, uint32_t y, uint32_t altitude_natural) {
    alt_set(x, y, altitude_natural);
    if (alt_natural(x, y) != altitude_natural) {
        alt_put_detail(x, y, (uint8_t)(AltNatural[altitude_natural] + *MapSeaLevel
            + rnd(AltNatural[altitude_natural + 1] - AltNatural[altitude_natural], NULL)));
    }
}

/*
Purpose: Get the bit shifted (down) altitude of the specified tile.
Original Offset: 00500150
Return Value: Altitude
Status: Complete
*/
int __cdecl alt_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->climate >> 5;
}

/*
Purpose: Get the non-shifted altitude of the specified tile.
Original Offset: n/a
Return Value: Altitude
Status: Complete
*/
int __cdecl altitude_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->climate & 0xE0;
}

/*
Purpose: Get the altitude details of the specified tile.
Original Offset: 00500180
Return Value: Altitude detail
Status: Complete
*/
int __cdecl alt_detail_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->contour;
}

/*
Purpose: Set the altitude details for the specified tile.
Original Offset: 00591260
Return Value: n/a
Status: Complete
*/
void __cdecl alt_put_detail(uint32_t x, uint32_t y, uint8_t detail) {
    map_loc(x, y)->contour = detail;
}

/*
Purpose: Get the faction owner of the specified tile.
Original Offset: n/a
Return Value: Faction id
Status: Complete
*/
uint32_t __cdecl owner_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->val2 & 0xF;
}

/*
Purpose: Set the faction owner for the specified tile.
Original Offset: 00591B10
Return Value: n/a
Status: Complete
*/
void __cdecl owner_set(uint32_t x, uint32_t y, int faction_id) {
    Map *tile = map_loc(x, y);
    tile->val2 &= 0xF0;
    tile->val2 |= faction_id & 0xF;
}

/*
Purpose: Set the site for the specified tile.
Original Offset: 00591B50
Return Value: n/a
Status: Complete
*/
void __cdecl site_set(uint32_t x, uint32_t y, int site) {
    Map *tile = map_loc(x, y);
    tile->val2 &= 0x0F;
    tile->val2 |= site << 4;
}

/*
Purpose: Get the region of the specified tile.
Original Offset: 00500220
Return Value: Region
Status: Complete
*/
uint32_t __cdecl region_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->region;
}

/*
Purpose: Set the region for the specified tile.
Original Offset: 00591B90
Return Value: n/a
Status: Complete
*/
void __cdecl region_set(uint32_t x, uint32_t y, uint8_t region) {
    map_loc(x, y)->region = region;
}

/*
Purpose: Get the using faction id of the specified tile.
Original Offset: n/a
Return Value: Faction id
Status: Complete
*/
uint32_t __cdecl using_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->val3 & 7;
}

/*
Purpose: Set the using faction id for the specified tile.
Original Offset: 00591C10
Return Value: n/a
Status: Complete
*/
void __cdecl using_set(uint32_t x, uint32_t y, int faction_id) {
    Map *tile = map_loc(x, y);
    tile->val3 &= 0xF8;
    tile->val3 |= faction_id & 7;
}

/*
Purpose: Get the lock faction id of the specified tile.
Original Offset: n/a
Return Value: Faction id
Status: Complete
*/
uint32_t __cdecl lock_at(uint32_t x, uint32_t y) {
    return (map_loc(x, y)->val3 >> 3) & 7;
}

/*
Purpose: Set the lock faction id for the specified tile.
Original Offset: 00591C50
Return Value: n/a
Status: Complete
*/
void __cdecl lock_set(uint32_t x, uint32_t y, uint32_t faction_id) {
    Map *tile = map_loc(x, y);
    tile->val3 &= 0xC7;
    tile->val3 |= (faction_id & 7) << 3;
}

/*
Purpose: Lock the specified tile for the faction id.
Original Offset: 00591C90
Return Value: True if already locked by another faction, otherwise false
Status: Complete
*/
BOOL __cdecl lock_map(uint32_t x, uint32_t y, uint32_t faction_id) {
    uint32_t lock_id = lock_at(x, y);
    if (lock_id != faction_id) {
        if (lock_id) {
            return true;
        }
        lock_set(x, y, faction_id);
    }
    return false;
}

/*
Purpose: Unlock the specified tile for faction id.
Original Offset: 00591CF0
Return Value: n/a
Status: Complete
*/
void __cdecl unlock_map(uint32_t x, uint32_t y, uint32_t faction_id) {
    if (lock_at(x, y) == faction_id) {
        map_loc(x, y)->val3 &= 0xC7;
    }
}

/*
Purpose: Get the rockiness of the specified tile.
Original Offset: n/a
Return Value: Rockiness
Status: Complete
*/
uint32_t __cdecl rocky_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->val3 & 0xC0;
}

/*
Purpose: Set the rockiness for the specified tile.
Original Offset: 00591BC0
Return Value: n/a
Status: Complete
*/
void __cdecl rocky_set(uint32_t x, uint32_t y, uint8_t rocky) {
    Map *tile = map_loc(x, y);
    tile->val3 &= 0x3F;
    tile->val3 |= rocky << 6;
    tile->bit2 |= 0x400000; // TODO: identify value
    *UnkBitfield1 |= 1; // TODO: identify variable + value
}

/*
Purpose: Get the bit of the specified tile.
Original Offset: 005001B0
Return Value: Bitfield
Status: Complete
*/
uint32_t __cdecl bit_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->bit;
}

/*
Purpose: Set the bit for the specified tile.
Original Offset: 00591D30
Return Value: n/a
Status: Complete
*/
void __cdecl bit_put(uint32_t x, uint32_t y, uint32_t bit) {
    map_loc(x, y)->bit = bit;
}

/*
Purpose: Set or unset bit for the specified tile.
Original Offset: 00591D60
Return Value: n/a
Status: Complete
*/
void __cdecl bit_set(uint32_t x, uint32_t y, uint32_t bit, BOOL set) {
    if (set) {
        map_loc(x, y)->bit |= bit;
    } else {
        map_loc(x, y)->bit &= ~bit;
    }
}

/*
Purpose: Get the bit2 of the specified tile.
Original Offset: n/a
Return Value: Bitfield
Status: Complete
*/
uint32_t __cdecl bit2_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->bit2;
}

/*
Purpose: Set or unset bit2 for the specified tile.
Original Offset: 00591DB0
Return Value: n/a
Status: Complete
*/
void __cdecl bit2_set(uint32_t x, uint32_t y, uint32_t bit2, BOOL set) {
    if (set) {
        map_loc(x, y)->bit2 |= bit2;
    } else {
        map_loc(x, y)->bit2 &= ~bit2;
    }
}

/*
Purpose: Get the code of the specified tile which keeps track of tile sequence order for landmarks.
Original Offset: n/a
Return Value: Code
Status: Complete
*/
uint32_t __cdecl code_at(uint32_t x, uint32_t y) {
    return map_loc(x, y)->bit2 >> 24;
}

/*
Purpose: Set the code for the specified tile which keeps track of tile sequence order for landmarks.
Original Offset: 00591E00
Return Value: n/a
Status: Complete
*/
void __cdecl code_set(uint32_t x, uint32_t y, uint32_t code) {
    Map *tile = map_loc(x, y);
    tile->bit2 &= 0xFFFFFF;
    tile->bit2 |= code << 24;
    *UnkBitfield1 |= 4; // TODO: identify variable + value
}

/*
Purpose: Synchronize the actual tile bit with the faction visible bit.
Original Offset: 00591E50
Return Value: n/a
Status: Complete
*/
void __cdecl synch_bit(uint32_t x, uint32_t y, uint32_t faction_id) {
    if (faction_id) {
        map_loc(x, y)->bit_visible[faction_id - 1] = bit_at(x, y);
    }
}

/*
Purpose: Determine the tile's mineral count that translates to rockiness.
Original Offset: 00591F00
Return Value: 0 (Flat), 1 (Rolling), 2 (Rocky)
Status: Complete
*/
int __cdecl minerals_at(int x, int y) {
    if (!y || y == (*MapLatitudeBounds - 1)) {
        return 2; // poles
    }
    int alt = alt_at(x, y);
    int avg = (x + y) >> 1;
    x -= avg;
    int val1 = (x / 2) + *MapRandSeed + (x - (x % 2)) + (avg - (avg % 2));
    int val2 = (val1 - 2 * (x & 1) - (avg & 1)) & 3;
    int type = abs(alt - ALT_SHORE_LINE);
    if (alt < ALT_SHORE_LINE) {
        type--;
    }
    switch (type) {
      case 0: {
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
Purpose: Determine if the tile has a resource bonus. While the last parameter is unused, it's set to
         1 by two calls inside world_site(). Otherwise, all other calls have it set to 0.
Original Offset: 00592030
Return Value: 0 (no bonus), 1 (nutrient), 2 (mineral), 3 (energy)
Status: Complete
*/
uint32_t __cdecl bonus_at(uint32_t x, uint32_t y, int UNUSED(unk_val)) {
    uint32_t bit = bit_at(x, y);
    uint32_t alt = alt_at(x, y);
    BOOL has_rsc_bonus = bit & BIT_RSC_BONUS;
    if (!has_rsc_bonus && (!*MapRandSeed
        || (alt >= ALT_SHORE_LINE && !(*GameRules & RULES_NO_UNITY_SCATTERING)))) {
        return 0;
    }
    uint32_t avg = (x + y) >> 1;
    x -= avg;
    uint32_t chk = (avg & 3) + 4 * (x & 3);
    if (!has_rsc_bonus && chk != ((*MapRandSeed + (-5 * (avg >> 2)) - 3 * (x >> 2)) & 0xF)) {
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
Purpose: Determine if the tile has a supply pod and if so what type.
Original Offset: 00592140
Return Value: 0 (no supply pod), 1 (standard supply pod), 2 (unity pod?)
Status: Complete
*/
uint32_t __cdecl goody_at(uint32_t x, uint32_t y) {
    uint32_t bit = bit_at(x, y);
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
    uint32_t avg = (x + y) >> 1;
    int x_diff = x - avg;
    uint32_t cmp = (avg & 3) + 4 * (x_diff & 3);
    if (!is_ocean(x, y)
        && cmp == ((-5 * (avg >> 2) - 3 * (x_diff >> 2) + *MapRandSeed) & 0xF)) {
        return 2;
    }
    return cmp == ((11 * (avg / 4) + 61 * (x_diff / 4) + *MapRandSeed + 8) & 0x1F); // 0 or 1
}

/*
Purpose: Clear the map's site values in a radius from the tile.
Original Offset: 00592400
Return Value: n/a
Status: Complete
*/
void __cdecl site_radius(int x, int y, int UNUSED(unk_val)) {
    for (int i = 0; i < 21; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius)) {
            Map *tile = map_loc(x_radius, y_radius);
            tile->val2 &= 0x0F;
        }
    }
}

/*
Purpose: Search for the first landmark found within the radius range of the specified tile.
Original Offset: 00592550
Return Value: Landmark offset or -1 if none found
Status: Complete
*/
int __cdecl find_landmark(int x, int y, uint32_t radius_range_offset) {
    int radius = RadiusRange[radius_range_offset];
    for (int i = 0; i < radius; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius)) {
            for (int lm = 0; lm < *MapLandmarkCount; lm++) {
                if (x_radius == MapLandmark[lm].x && y_radius == MapLandmark[lm].y) {
                    return lm;
                }
            }
        }
    }
    return -1;
}

/*
Purpose: Set up a new landmark with the provided name at the specified tile.
Original Offset: 00592600
Return Value: Landmark offset or -1 if max landmark count is reached
Status: Complete
*/
int __cdecl new_landmark(int x, int y, LPCSTR name) {
    int landmark_offset = *MapLandmarkCount;
    if (landmark_offset >= MaxLandmarkNum) {
        return -1;
    }
    *MapLandmarkCount += 1;
    Landmark *lm = &MapLandmark[landmark_offset];
    lm->x = x;
    lm->y = y;
    strcpy_s(lm->name, 32, name);
    return landmark_offset;
}

/*
Purpose: Check whether the specified faction has permission to name a landmark on the provided tile.
Original Offset: 00592650
Return Value: Does the faction have control of the tile to set a landmark? true/false
Status: Complete
*/
BOOL __cdecl valid_landmark(uint32_t x, uint32_t y, int faction_id) {
    int terr_faction_id = *IsMultiplayerNet ? map_loc(x, y)->territory 
        : whose_territory(faction_id, x, y, NULL, false);
    if (terr_faction_id == faction_id) {
        return true;
    }
    if (terr_faction_id > 0) {
        return false;
    }
    int base_id = base_find(x, y);
    return base_id < 0 ? true : Bases[base_id].faction_id_current == faction_id;
}

/*
Purpose: Remove the landmark at the specified tile.
Original Offset: 005926F0
Return Value: n/a
Status: Complete
*/
void __cdecl kill_landmark(int x, int y) {
    int landmark_to_kill = find_landmark(x, y, 1);
    if (landmark_to_kill >= 0) {
        if (landmark_to_kill < (*MapLandmarkCount - 1)) {
            memcpy_s(&MapLandmark[landmark_to_kill], sizeof(Landmark) * MaxLandmarkNum,
                &MapLandmark[landmark_to_kill + 1], // single memcpy_s replaces original loop
                sizeof(Landmark) * (*MapLandmarkCount - landmark_to_kill - 1));
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
BOOL __cdecl is_coast(int x, int y, BOOL is_base_radius) {
    uint32_t radius = is_base_radius ? 21 : 9;
    for (uint32_t i = 1; i < radius; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius) && is_ocean(x_radius, y_radius)) {
            return true; // modified original that would return i, all calls check return as boolean
        }
    }
    return false;
}

/*
Purpose: Check whether the specified tile is part of an ocean.
Original Offset: 005001E0
Return Value: Is tile ocean? true/false
Status: Complete
*/
BOOL __cdecl is_ocean(uint32_t x, uint32_t y) {
    return altitude_at(x, y) < ALT_BIT_SHORE_LINE;
}

/*
Purpose: Get the owner of the specified tile if there is a unit in it.
Original Offset: 00500250
Return Value: Owner (faction id) or -1
Status: Complete
*/
int __cdecl veh_who(uint32_t x, uint32_t y) {
    Map *tile = map_loc(x, y);
    if (tile->bit & BIT_VEH_IN_TILE) {
        uint32_t owner = tile->val2 & 0xF;
        if (owner < 8) {
            return owner;
        }
    }
    return -1;
}

/*
Purpose: Rebuild the Map's unit related values.
Original Offset: 00532A90
Return Value: n/a
Status: Complete
*/
void __cdecl rebuild_vehicle_bits() {
    for (int y = 0; y < *MapLatitudeBounds; y++) {
        for (int x = y & 1; x < *MapLongitudeBounds; x += 2) {
            bit_set(x, y, BIT_VEH_IN_TILE, false);
            for (int veh_id = 0; veh_id < *VehCurrentCount; veh_id++) {
                if (Vehs[veh_id].x == (int)x && Vehs[veh_id].y == (int)y) {
                    bit_set(x, y, BIT_VEH_IN_TILE, true);
                    if (!(bit_at(x, y) & BIT_BASE_IN_TILE)) {
                        owner_set(x, y, Vehs[veh_id].faction_id);
                    }
                    break;
                }
            }
        }
    }
}

/*
Purpose: Rebuild the Map's base related values.
Original Offset: 00532B70
Return Value: n/a
Status: Complete
*/
void __cdecl rebuild_base_bits() {
    for (int y = 0; y < *MapLatitudeBounds; y++) {
        for (int x = y & 1; x < *MapLongitudeBounds; x += 2) {
            bit_set(x, y, BIT_BASE_IN_TILE, false);
            for (int base_id = 0; base_id < *BaseCurrentCount; base_id++) {
                if (Bases[base_id].x == (int)x && Bases[base_id].y == (int)y) {
                    bit_set(x, y, BIT_BASE_IN_TILE, true);
                    owner_set(x, y, Bases[base_id].faction_id_current);
                    break;
                }
            }
        }
    }
}

/*
Purpose: Calculate the distance between two x coordinates with handling for round maps.
Original Offset: 00579790
Return Value: Distance
Status: Complete
*/
int __cdecl x_dist(int x_point_a, int x_point_b) {
    int dist = abs(x_point_a - x_point_b);
    if (!*MapIsFlat && dist > (int)*MapLongitude) {
        dist = *MapLongitudeBounds - dist;
    }
    return dist;
}

/*
Purpose: Check whether a faction can see the specified tile.
Original Offset: 00579840
Return Value: Is tile visible/known to faction? true/false
Status: Complete
*/
BOOL __cdecl is_known(uint32_t x, uint32_t y, uint32_t faction_id) {
    return (PlayersData[faction_id].flags & PFLAG_MAP_REVEALED
        || map_loc(x, y)->visibility & (1 << faction_id));
}

/*
Purpose: If a base exists, get the owner of the specified tile.
Original Offset: 005798A0
Return Value: Owner (faction id) or -1
Status: Complete
*/
int __cdecl base_who(uint32_t x, uint32_t y) {
    Map *tile = map_loc(x, y);
    if (tile->bit & BIT_BASE_IN_TILE) {
        uint32_t owner = tile->val2 & 0xF;
        if (owner < MaxPlayerNum) {
            return owner;
        }
    }
    return -1;
}

/*
Purpose: If a base or unit exists, get the owner of the specified tile.
Original Offset: 005798E0
Return Value: Owner (faction id) or -1
Status: Complete
*/
int __cdecl anything_at(uint32_t x, uint32_t y) {
    Map *tile = map_loc(x, y);
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
    if (*MapTiles) {
        free(*MapTiles);
    }
    if (*MapAbstract) {
        free(*MapAbstract);
    }
    *MapTiles = 0;
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
    *MapLongitude = *MapLongitudeBounds / 2;
    *MapArea = *MapLongitude * *MapLatitudeBounds;
    *MapAreaSqRoot = quick_root(*MapArea);
    *MapTiles = 0;
    *MapTiles = (Map *)mem_get(*MapArea * sizeof(Map));
    if (*MapTiles) {
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
Purpose: Reset the map to a blank state. Doesn't wipe unk_1 and territory fields.
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
        (*MapTiles)[i].climate = ALT_BIT_OCEAN;
        (*MapTiles)[i].contour = 20;
        (*MapTiles)[i].val2 = 0xF;
        (*MapTiles)[i].region = 0;
        (*MapTiles)[i].visibility = 0;
        (*MapTiles)[i].val3 = 0;
        (*MapTiles)[i].bit = 0;
        (*MapTiles)[i].bit2 = 0;
        ZeroMemory((*MapTiles)[i].bit_visible, sizeof((*MapTiles)[i].bit_visible));
    }
}

/*
Purpose: Write map data to a file.
Original Offset: 005910B0
Return Value: Did an error occur? true/false
Status: Complete
*/
BOOL __cdecl map_write(FILE *map_file) {
    if (_fwrite(&*MapLongitudeBounds, 2724, 1, map_file)
        && _fwrite(*MapTiles, *MapArea * sizeof(Map), 1, map_file)
        && _fwrite(*MapAbstract, *MapAbstractArea, 1, map_file)) {
        return false;
    }
    return true;
}

/*
Purpose: Read the map data from a file and write it into memory.
Original Offset: 00591130
Return Value: Did an error occur? true/false
Status: Complete
*/
BOOL __cdecl map_read(FILE *map_file) {
    map_shutdown();
    if (!_fread(&*MapLongitudeBounds, 2724, 1, map_file)) {
        return true;
    }
    *MapTiles = 0;
    *MapAbstract = 0;
    if (map_init()) {
        return true;
    }
    if (!_fread(*MapTiles, *MapArea * sizeof(Map), 1, map_file)
        || !_fread(*MapAbstract, *MapAbstractArea, 1, map_file)) {
        return true;
    }
    fixup_landmarks();
    return false;
}

/*
Purpose: Get the region value for the specified tile.
Original Offset: 00591210
Return Value: Abstract value (region)
Status: Complete
*/
uint8_t __cdecl abstract_at(uint32_t x, uint32_t y) {
    return (*MapAbstract)[(x >> 1) + y * (*MapAbstractLongBounds >> 1)];
}

/*
Purpose: Set the region value for the specified tile.
Original Offset: 00591230
Return Value: n/a
Status: Complete
*/
void __cdecl abstract_set(uint32_t x, uint32_t y, uint8_t region) {
    (*MapAbstract)[(x >> 1) + y * (*MapAbstractLongBounds >> 1)] = region;
}

/*
Purpose: Quickly check for unit related zone of control conflicts. If a ZOC conflict is found, store
         the coordinates of the tile inside ZOC pointers.
Original Offset: 00593830
Return Value: n/a
Status: Complete
*/
void __cdecl quick_zoc(uint32_t x_src, uint32_t y_src, uint32_t faction_id, int x_dst, int y_dst, 
                       int *x_zoc, int *y_zoc) {
    BOOL is_src_ocean = is_ocean(x_src, y_src);
    int search_zoc = -1;
    for (uint32_t i = 0; i < 8; i++) {
        int x_radius = xrange(x_src + RadiusBaseX[i]);
        int y_radius = y_src + RadiusBaseY[i];
        if (on_map(x_radius, y_radius)) {
            int owner = veh_who(x_radius, y_radius);
            if (owner >= 0 && (uint32_t)owner != faction_id 
                && is_ocean(x_radius, y_radius) == is_src_ocean
                && !has_treaty(faction_id, owner, DTREATY_PACT)) {
                int proximity = vector_dist(x_radius, y_radius, x_dst, y_dst);
                if (proximity >= search_zoc) {
                    search_zoc = proximity;
                    *x_zoc = x_radius;
                    *y_zoc = y_radius;
                }
            }
        }
    }
}

/*
Purpose: Determine if the specified offsets are within the range radius.
Original Offset: 005A65A0
Return Value: Range radius, otherwise -1 if not within range
Status: Complete
*/
int __cdecl radius_move(int x_radius_off, int y_radius_off, int range) {
    for (int i = 0; i < range; i++) {
        if (x_radius_off == RadiusOffsetX[i] && y_radius_off == RadiusOffsetY[i]) {
            return i; // radius
        }
    }
    return -1;
}

/*
Purpose: Determine if the specified two tiles are within the range radius of each other.
Original Offset: 005A65D0
Return Value: Range radius, otherwise -1 if not within range
Status: Complete
*/
int __cdecl radius_move(int x_src, int y_src, int x_dst, int y_dst, int range) {
    int x_radius_off = x_dst - x_src;
    if (x_radius_off < (-(int)*MapLongitude)) {
        x_radius_off += *MapLongitudeBounds;
    }
    if (x_radius_off > ((int)*MapLongitude)) {
        x_radius_off -= *MapLongitudeBounds;
    }
    return radius_move(x_radius_off, y_dst - y_src, range);
}

/*
Purpose: Determine if the specified two tiles are within the radius directionally of each other.
Original Offset: 005A6630
Return Value: Range radius, otherwise -1 if not within range
Status: Complete
*/
int __cdecl compass_move(int x_src, int y_src, int x_dst, int y_dst) {
    int x_radius_off = x_dst - x_src;
    if (x_radius_off < (-(int)*MapLongitude)) {
        x_radius_off += *MapLongitudeBounds;
    }
    if (x_radius_off > ((int)*MapLongitude)) {
        x_radius_off -= *MapLongitudeBounds;
    }
    int y_radius_off = y_dst - y_src;
    int direction_x = (x_radius_off > 0) ? 1 : (x_radius_off >= 0) - 1;
    int direction_y = (y_radius_off > 0) ? 1 : (y_radius_off >= 0) - 1;
    for (int i = 0; i < 9; i++) {
        if (direction_x == (RadiusBaseX[i] > 0 ? 1 : (RadiusBaseX[i] >= 0) - 1)
            && direction_y == (RadiusBaseY[i] > 0 ? 1 : (RadiusBaseY[i] >= 0) - 1)) {
            return i;
        }
    }
    return -1;
}

/*
Purpose: Check whether there is a sensor available in the specified tile.
Original Offset: 005BF010
Return Value: 0 (no sensor), 1 (sensor array via terraforming), 2 (Geosynchronous Survey Pod)
Status: Complete
*/
int __cdecl is_sensor(uint32_t x, uint32_t y) {
    if (bit_at(x, y) & BIT_SENSOR_ARRAY) {
        return 1; // Sensor Array built in tile
    }
    int base_id = base_find(x, y);
    if (base_id != -1) {
        int dist_x = x_dist(x, Bases[base_id].x);
        if (!dist_x || dist_x == 2) { // removed unnecessary duplicate calculation of distX
            int dist_y = abs((int)y - Bases[base_id].y);
            if (!dist_y || dist_y == 2) {
                if (has_fac_built(FAC_GEOSYNC_SURVEY_POD, base_id)) {
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
BOOL __cdecl has_temple(int faction_id) {
    for (int y = 0; y < *MapLatitudeBounds; y++) {
        for (int x = y & 1; x < *MapLongitudeBounds; x += 2) {
            if ((bit2_at(x, y) & (BIT2_UNK_80000000 | BIT2_NEXUS)) == BIT2_NEXUS
                && !code_at(x, y) 
                && faction_id == whose_territory(faction_id, x, y, NULL, false)
                && map_loc(x, y)->visibility & (1 << faction_id)) { // tile visible
                return true;
            }
        }
    }
    return false;
}

/*
Purpose: Handle setting the world altitude.
Original Offset: 005C2020
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_alt_set(int x, int y, int altitude, BOOL is_set_both) {
    ZeroMemory(MapBaseSubmergedCount, sizeof(int) * MaxPlayerNum);
    memset(MapBaseIdClosestSubmergedVeh, -1, sizeof(int) * MaxPlayerNum);
    if (on_map(x, y)) {
        is_set_both ? alt_set_both(x, y, altitude) : alt_set(x, y, altitude);
    }
    BOOL has_set_alt;
    for (int i = 1, alt = altitude - 1; i < altitude; i++, alt--) {
        has_set_alt = false;
        for (int j = RadiusRange[i]; j < RadiusRange[i + 1]; j++) {
            int x_radius = xrange(x + RadiusOffsetX[j]);
            int y_radius = y + RadiusOffsetY[j];
            if (on_map(x_radius, y_radius) && alt_at(x_radius, y_radius) < alt) {
                if (is_set_both) {
                    alt_set_both(x_radius, y_radius, alt);
                    if (anything_at(x_radius, y_radius) < 0 && !bonus_at(x_radius, y_radius, 0)) {
                        owner_set(x_radius, y_radius, -1);
                    }
                } else {
                    alt_set(x_radius, y_radius, alt);
                }
                has_set_alt = true;
            }
        }
        if (!has_set_alt) {
            break;
        }
    }
    int max_alt = 8 - altitude;
    for (int i = 1, alt = i + altitude; i < max_alt; i++, alt++) {
        has_set_alt = false;
        for (int j = RadiusRange[i]; j < RadiusRange[i + 1]; j++) {
            int x_radius = xrange(x + RadiusOffsetX[j]);
            int y_radius = y + RadiusOffsetY[j];
            if (on_map(x_radius, y_radius) && alt_at(x_radius, y_radius) < alt) {
                if (is_set_both) {
                    alt_set_both(x_radius, y_radius, alt);
                    if (anything_at(x_radius, y_radius) < 0 && !bonus_at(x_radius, y_radius, 0)) {
                        owner_set(x_radius, y_radius, -1);
                    }
                } else {
                    alt_set(x_radius, y_radius, alt);
                }
                has_set_alt = true;
            }
        }
        if (!has_set_alt) {
            break;
        }
    }
}

/*
Purpose: Handle raising the altitude of the specified tile.
Original Offset: 005C2380
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_raise_alt(int x, int y) {
    if (on_map(x, y)) {
        uint32_t altitude = alt_at(x, y);
        if (altitude < 6) {
            world_alt_set(x, y, altitude++, true);
        }
    }
}

/*
Purpose: Handle lowering the altitude of the specified tile.
Original Offset: 005C23E0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_lower_alt(int x, int y) {
    if (on_map(x, y)) {
        uint32_t altitude = alt_at(x, y);
        if (altitude) {
            world_alt_set(x, y, altitude--, true);
        }
    }
}

/*
Purpose: Set up the brush for creating world terrain.
Original Offset: 005C2440
Return Value: n/a
Status: Complete - testing
*/
void __cdecl brush(int x, int y, int altitude) {
    BOOL use_draw_radius = false;
    for (int i = 0; i < 4; i++) {
        if (i != 2) {
            int x_radius = xrange(x + RadiusOffsetX[i]);
            int y_radius = y + RadiusOffsetY[i];
            if (on_map(x_radius, y_radius) && alt_at(x_radius, y_radius) != altitude) {
                BOOL skip = false;
                for (uint32_t j = 0; j < 8; j++) {
                    int x_rad_base = xrange(x_radius + RadiusBaseX[j]);
                    int y_rad_base = y_radius + RadiusBaseY[j];
                    if (on_map(x_rad_base, y_rad_base) && !is_ocean(x_rad_base, y_rad_base)
                        && !region_at(x_rad_base, y_rad_base)) {
                        skip = true;
                    }
                }
                if (!skip) {
                    if (altitude != 3) {
                        for (uint32_t j = 0, k = 0; j < 7; j += 3, k += 2) {
                            int x_rad_base = xrange(x_radius + RadiusBaseX[k + 1]);
                            int y_rad_base = y_radius + RadiusBaseY[k + 1];
                            if (on_map(x_rad_base, y_rad_base)
                                && altitude_at(x_rad_base, y_rad_base) == ALT_BIT_SHORE_LINE) {
                                int x_rad_base2 = xrange(x_radius + RadiusBaseX[(k + 2) % 8]);
                                int y_rad_base2 = y_radius + RadiusBaseY[(k + 2) % 8];
                                int x_rad_base3 = xrange(x_radius + RadiusBaseX[k % 8]);
                                int y_rad_base3 = y_radius + RadiusBaseY[k % 8];
                                if (on_map(x_rad_base2, y_rad_base2) 
                                    && on_map(x_rad_base3, y_rad_base3)
                                    && altitude_at(x_rad_base2, y_rad_base2) < ALT_BIT_SHORE_LINE
                                    && altitude_at(x_rad_base3, y_rad_base3) < ALT_BIT_SHORE_LINE) {
                                    skip = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (!skip) {
                        region_set(x_radius, y_radius, 1);
                        use_draw_radius |= alt_at(x_radius, y_radius) != altitude;
                        world_alt_set(x_radius, y_radius, altitude, true);
                        if (*GameState & STATE_OMNISCIENT_VIEW && altitude > 3) {
                            temp_set(x_radius, y_radius, 1);
                        }
                        *BrushVal1 += 1;
                        *BrushVal2 += 1;
                    }
                }
            }
        }
    }
    if (*GameState & STATE_OMNISCIENT_VIEW && use_draw_radius) {
        draw_radius(x, y, 2, 2);
    }
}

/*
Purpose: Paint land to assist in the creation of the world terrain.
Original Offset: 005C27F0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl paint_land(int x, int y, uint32_t altitude, uint32_t radius) {
    *BrushVal2 = 0;
    for (int i = 0; i < 2000 && *BrushVal2 < radius; i++) {
        int search = -1;
        uint32_t unk_val = 0;
        int x_rad_base = x;
        int y_rad_base = y;
        do {
            brush(x_rad_base, y_rad_base, altitude);
            uint32_t offset = rand() % 8 | 1;
            if ((int)offset == search) {
                if (++unk_val > ((*MapLandCoverage * *MapLandCoverage) + 2)) {
                    offset = (offset - (*MapLandCoverage * *MapLandCoverage) + unk_val - 2) % 8;
                }
            } else {
                search = offset;
                unk_val = 0;
            }
            x_rad_base = xrange(x + RadiusBaseX[offset]);
            y_rad_base = y + RadiusBaseY[offset];
            i++;
        } while (i < 2000 && on_map(x_rad_base, y_rad_base) && *BrushVal2 < radius);
    }
}

/*
Purpose: Build out the map continents.
Original Offset: 005C28F0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl build_continent(uint32_t size) {
    for (uint32_t i = 0; i < *MapArea; i++) {
        (*MapTiles)[i].region = 0;
    }
    int coverage = *MapLandCoverage;
    if (coverage && *BrushVal1 >= *WorldBuildVal1) {
        coverage--;
    }
    int radius = WorldBuilder->continent_mod * coverage * coverage + WorldBuilder->continent_base;
    int x; 
    int y;
    int i = 0;
    do {
        x = rnd(*MapLongitudeBounds - (*MapIsFlat * 8), NULL) + *MapIsFlat * 4;
        y = rnd(*MapLatitudeBounds - 8, NULL) + 4;
        if (x & 1) {
            x--;
        }
        if (y & 1) {
            x++; // validate whether this should be y and/or reasoning for x
        }
    } while (i++ < 100 && alt_at(x, y) >= ALT_BIT_SHORE_LINE);
    if (alt_at(x, y) < ALT_BIT_SHORE_LINE) {
        uint32_t ratio = (size * 3200) / *MapArea;
        if (ratio > WorldBuilder->cont_size_ratio5 || *BrushVal1 >= *WorldBuildVal1) {
            radius /= 4;
            if (BrushVal1 >= WorldBuildVal1 && ratio > WorldBuilder->cont_size_ratio5) {
                radius = rand() % 3 + 1;
            }
        } else if (ratio <= WorldBuilder->cont_size_ratio4) {
            if (ratio <= WorldBuilder->cont_size_ratio3) {
                if (ratio <= WorldBuilder->cont_size_ratio2) {
                    if (ratio < WorldBuilder->cont_size_ratio1 && *MapLandCoverage > 1) {
                        radius += radius / 2;
                    }
                } else  if (rnd(4 - *MapLandCoverage, NULL)) {
                    radius /= 2;
                }
            } else {
                radius /= 2;
            }
        } else {
            radius /= 3;
        }
        if (*BrushVal1 < *WorldBuildVal1 && radius < 4) {
            radius = 4;
        }
        paint_land(x, y, ALT_SHORE_LINE, radius);
        do_all_non_input();
    }
}

/*
Purpose: Build out the map hills.
Original Offset: 005C2B40
Return Value: n/a
Status: Complete - testing
*/
void __cdecl build_hills(int altitude) {
    int x;
    int y;
    int i = 0;
    BOOL keep_going = true;
    do {
        x = rnd(*MapLongitudeBounds - (*MapIsFlat * 8), NULL) + *MapIsFlat * 4;
        y = rnd(*MapLatitudeBounds - 8, NULL) + 4;
        if (x & 1) {
            x--;
        }
        if (y & 1) {
            x++; // should this have been y?
        }
        i += 2;
        if (i > 100) {
            return;
        }
        for (uint32_t j = 0; j < 9; j++) {
            int x_radius = xrange(x + RadiusOffsetX[j]);
            int y_radius = y + RadiusOffsetY[j];
            if (on_map(x_radius, y_radius) && alt_at(x_radius, y_radius) < (altitude - 1)) {
                break;
            }
        }
        keep_going = false;
        int plat_mod = WorldBuilder->plateau_mod * (2 - *MapOceanCoverage);
        if (altitude != ALT_1_LEVEL_ABOVE_SEA) {
            plat_mod /= 4;
        }
        uint32_t plat_base = WorldBuilder->plateau_base;
        if (*MapOceanCoverage >= 2) {
            plat_base /= 2;
        }
        paint_land(x, y, altitude, plat_mod + plat_base);
        do_all_non_input();
    } while (keep_going); // better way to set up outer loop?
}

/*
Purpose: Build out the world river beds.
Original Offset: 005C3680
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_riverbeds() {
    for (uint32_t i = 0; i < *MapArea; i++) {
        (*MapTiles)[i].bit &= ~(BIT_RIVERBED);
    }
    uint32_t riverbed_count = 0;
    uint32_t max_riverbeds = (*MapArea * ((4 - *MapOceanCoverage) * (WorldBuilder->rivers_base 
        + *MapCloudCover * WorldBuilder->rivers_rain_mod) / 3)) / 3200;
    for (uint32_t i = 0; i < 4000 && riverbed_count < max_riverbeds; i++) {
        int x = rnd(*MapLongitudeBounds, NULL);
        int y = rnd(*MapLatitudeBounds, NULL);
        if (x & 1) {
            x--;
        }
        if (y & 1) {
            x++;
        }
        if (!is_ocean(x, y) && !(bit_at(x, y) & BIT_RIVERBED)) {
            uint32_t search_alt = 0;
            int x_search = -1;
            int y_search = -1;
            for (int j = 0; j < RadiusRange[4]; j++) {
                int x_radius = xrange(x + RadiusOffsetX[j]);
                int y_radius = y + RadiusOffsetY[j];
                if (on_map(x_radius, y_radius)) {
                    if (bit_at(x_radius, y_radius) & BIT_RIVERBED) {
                        break; // exit if riverbed already in search area
                    }
                    if (j < RadiusRange[3]) { // limit scope of where riverbeds can be in search
                        uint32_t alt = alt_at(x_radius, y_radius);
                        if (alt > search_alt) {
                            search_alt = alt;
                            x_search = x_radius; // replaced storing j offset with radius values
                            y_search = y_radius;
                        }
                    }
                }
            }
            if (x_search >= 0) {
                bit_set(x_search, y_search, BIT_RIVERBED, true);
                riverbed_count++;
            }
        }
    }
}

/*
Purpose: Determine if there are any issues with how the world continents are set up.
Original Offset: 005C40F0
Return Value: Are there any issues with the world continents? true/false
Status: Complete - testing
*/
BOOL __cdecl world_validate() {
    Paths->continents();
    uint32_t val1 = 0; // rename to better var descriptions
    uint32_t val2 = 0; // rename to better var descriptions
    for (uint32_t region = 1; region < MaxRegionLandNum; region++) {
        if (!bad_reg(region)) {
            uint32_t tile_count = Continents[region].tile_count;
            if (tile_count < val2) {
                if (tile_count >= val1) {
                    val1 = tile_count;
                }
            } else {
                val1 = val2;
                val2 = tile_count;
            }
        }
    }
    return val1 < ((*MapLandCoverage == 1) ? ((val2 * 2) / 3) : val2 / 2);
}

/*
Purpose: Set up the world temperature.
Original Offset: 005C4170
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_temperature() {
    random_reseed(*MapRandSeed + 17);
    int temp_heat = *MapLatitudeBounds / WorldBuilder->solar_energy;
    int thermal_banding = *MapLatitudeBounds / WorldBuilder->thermal_band;
    int thermal_deviance = *MapLatitudeBounds / WorldBuilder->thermal_deviance;
    int global_warming = *MapLatitudeBounds / WorldBuilder->global_warming;
    for (int y = 0; y < *MapLatitudeBounds; y++) {
        for (int x = y & 1; x < *MapLongitudeBounds; x += 2) {
            if ((bit2_at(x, y) & (BIT2_UNK_80000000 | BIT2_CRATER)) != BIT2_CRATER
                || code_at(x, y) >= 21) {
                int rand_seed = random(0, thermal_deviance * 2);
                int rand_orbit = random(0, *MapPlanetaryOrbit + 1);
                int val1 = (*MapLatitudeBounds / 2) - rand_seed - y + thermal_deviance;
                if (val1 < 0) { // abs?
                    val1 = rand_seed - (*MapLatitudeBounds / 2) - thermal_deviance + y;
                }
                int val2 = (thermal_banding / 2 + (val1 - temp_heat * (*MapPlanetaryOrbit - 1)
                    - *MapSeaLevelCouncil * global_warming) * 2) / thermal_banding;
                int temperature = (val2 > 2) ? ((val2 <= 9) + 1) : 3;
                int alt = alt_at(x, y);
                for (int i = 0; i < 8; i++) {
                    int x_radius = xrange(x + RadiusOffsetX[i]);
                    int y_radius = y + RadiusOffsetY[i];
                    if (on_map(x_radius, y_radius) && bit_at(x_radius, y_radius) & BIT_THERMAL_BORE
                        && (alt <= ALT_1_LEVEL_ABOVE_SEA || alt_at(x_radius, y_radius) >= alt)) {
                        temperature++;
                        break;
                    }
                }
                if (temperature > 1) {
                    if (alt <= ALT_2_LEVELS_ABOVE_SEA) {
                        // == ALT_2_LEVELS_ABOVE_SEA, simplify?
                        if (alt > ALT_1_LEVEL_ABOVE_SEA && !rand_orbit) {
                            temperature--;
                        }
                    } else if (*MapPlanetaryOrbit < 2 || !random(0, 2)) {
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
Purpose: Analysis of the world map.
Original Offset: 005C55C0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_analysis() {
    for (uint32_t i = 0; i < MaxContinentNum; i++) {
        Continents[i].open_terrain = 0;
    }
    for (int y = 0; y < *MapLatitudeBounds; y++) {
        for (int x = y & 1; x < *MapLongitudeBounds; x += 2) {
            uint32_t region = region_at(x, y);
            BOOL is_ocean_tile = is_ocean(x, y);
            if (!is_ocean_tile && climate_at(x, y) != RAINFALL_ARID 
                && rocky_at(x, y) < TERRAIN_BIT_ROCKY
                && (!(bit_at(x, y) & BIT_FUNGUS || altitude_at(x, y) < ALT_BIT_OCEAN_SHELF))) {
                Continents[region].open_terrain++;
            }
            bit_set(x, y, BIT_UNK_40000000 | BIT_UNK_4000, false); // verify map is being iterated 
            if (!is_ocean_tile) {                                  // correctly for all bit_*
                int search_val = -1;
                uint32_t count_val1 = 0;
                uint32_t count_val2 = 0;
                for (uint32_t i = 0; i < 20; i++) {
                    int x_radius = xrange(x + RadiusOffsetX[i + 1]);
                    int y_radius = y + RadiusOffsetY[i + 1];
                    if (on_map(x_radius, y_radius)) {
                        BOOL is_ocean_radius = is_ocean(x_radius, y_radius);
                        if (i >= 32) {
                            if (is_ocean_radius != search_val || i == 32) {
                                search_val = is_ocean_radius;
                                count_val1++;
                            }
                            uint32_t region_radius;
                            if (y_radius > 4 && y_radius < ((int)*MapLatitudeBounds - 4)
                                && (region_radius = region_at(x_radius, y_radius),
                                    region_radius < MaxRegionLandNum && region != region_radius
                                    && Continents[region].tile_count > 40
                                    && Continents[region_radius].tile_count > 40)) {
                                bit_set(x, y, BIT_UNK_4000, true); // here
                            }
                        } else {
                            if (is_ocean_radius != search_val) {
                                search_val = is_ocean_radius;
                                count_val2++;
                                if (count_val2 >= 4 && Continents[region].tile_count >= 80) {
                                    bit_set(x, y, BIT_UNK_40000000, true); // here
                                }
                            }
                        }
                    }
                }
                if (count_val1 < 4) {
                    bit_set(x, y, BIT_UNK_40000000, false); // here
                }
            }
        }
        do_all_non_input();
    }
    for (uint32_t i = 0; i < *MapArea; i++) {
        (*MapTiles)[i].val2 &= 0xF; // clear map sites
    }
}

/*
Purpose: Set the default altitude details for the specified tile.
Original Offset: 005C58C0
Return Value: n/a
Status: Complete
*/
void __cdecl world_alt_put_detail(uint32_t x, uint32_t y) {
    alt_put_detail(x, y, (uint8_t)AltNatural[3]);
}

/*
Purpose: Set up the world polar caps.
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
        world_alt_put_detail(rnd(*MapLongitude, NULL) * 2, 0);
        world_alt_put_detail(rnd(*MapLongitude, NULL) * 2 + 1, 1);
        world_alt_put_detail(rnd(*MapLongitude, NULL) * 2 + 1, *MapLatitudeBounds - 1);
        world_alt_put_detail(rnd(*MapLongitude, NULL) * 2, *MapLatitudeBounds - 2);
    }
}

/*
Purpose: Set up the world contours.
Original Offset: 005C5AE0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_linearize_contours() {
    for (int y = 0; y < *MapLatitudeBounds; y++) {
        for (int x = y & 1; x < *MapLongitudeBounds; x += 2) {
            uint32_t alt_nat = alt_natural(x, y);
            alt_put_detail(x, y, (uint8_t)((((ElevDetail[alt_nat
                + (alt_nat >= ALT_3_LEVELS_ABOVE_SEA) ? 4 : 1] - ElevDetail[alt_nat])
                * (alt_detail_at(x, y) - AltNatural[alt_nat]))
                / (AltNatural[alt_nat + 1] - AltNatural[alt_nat])) + ElevDetail[alt_nat]));
        }
        do_all_non_input();
    }
    memcpy_s(&AltNatural, sizeof(uint32_t) * 11, &ElevDetail, sizeof(uint32_t) * 11);
}

/*
Purpose: Determine if the specified tile is near a landmark.
Original Offset: 005C5BD0
Return Value: Is the tile near a landmark? true/false
Status: Complete
*/
BOOL __cdecl near_landmark(int x, int y) {
    for (int i = 0; i < RadiusRange[8]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius) && code_at(x_radius, y_radius)) {
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
void __cdecl world_crater(int x, int y) {
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    world_alt_set(x, y, ALT_OCEAN_TRENCH, true);
    world_alt_set(x, y, ALT_3_LEVELS_ABOVE_SEA, true);
    world_alt_set(x, y, ALT_SHORE_LINE, true);
    world_alt_set(x, y, ALT_OCEAN_SHELF, true);
    world_alt_set(x, y, ALT_SHORE_LINE, true);
    uint32_t temperature = temp_at(x, y);
    for (int i = 0; i < RadiusRange[3]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius)) {
            bit_set(x_radius, y_radius, BIT_RIVERBED, false);
            bit2_set(x_radius, y_radius, BIT2_CRATER, true);
            code_set(x_radius, y_radius, i);
            if (i < 21) {
                rocky_set(x_radius, y_radius, ROCKINESS_ROLLING);
                temp_set(x_radius, y_radius, (uint8_t)temperature);
            }
        }
    }
    new_landmark(x, y, StringTable->get((int)Natural[LM_CRATER].name));
}

/*
Purpose: Setup the 'Monsoon Jungle' landmark.
Original Offset: 005C5EF0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_monsoon(int x, int y) {
    world_rainfall();
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            uint32_t land_count;
            do {
                y = *MapLatitudeBounds / 2 + rand() % 4 - 2;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                land_count = 0;
                for (int i = 0; i < RadiusRange[5]; i++) {
                    int x_radius = xrange(x + RadiusOffsetX[i]);
                    int y_radius = y + RadiusOffsetY[i];
                    if (on_map(x_radius, y_radius) && !is_ocean(x_radius, y_radius)) {
                        land_count++;
                    }
                }
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y) || !is_coast(x, y, true) || land_count < 40
                || climate_at(x, y) <= RAINFALL_MOIST);
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    for (int i = 0; i < RadiusRange[5]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (abs(RadiusOffsetY[i]) <= 8 &&  on_map(x_radius, y_radius)) {
            if (i < 21 && is_ocean(x_radius, y_radius)) {
                world_alt_set(x_radius, y_radius, ALT_SHORE_LINE, true);
            }
            bit2_set(x_radius, y_radius, LM_JUNGLE, true);
            code_set(x_radius, y_radius, i);
        }
    }
    new_landmark(x, y, StringTable->get((int)Natural[LM_JUNGLE].name));
}

/*
Purpose: Setup the 'New Sargasso' landmark.
Original Offset: 005C6200
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_sargasso(int x, int y) {
    uint32_t loc_attempts = 0;
    BOOL has_skipped = false;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (!is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    for (int i = 0; i < RadiusRange[4]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius) && (is_ocean(x_radius, y_radius) || i < 21)) {
            world_alt_set(x_radius, y_radius, ALT_OCEAN_SHELF, true);
            bit2_set(x_radius, y_radius, BIT2_SARGASSO, true);
            code_set(x_radius, y_radius, i);
            if (i <= 4) {
                bit_set(x_radius, y_radius, BIT_SUPPLY_POD, true);
            }
            if ((has_skipped || i < RadiusRange[2] || rand() % 24) 
                && (i < RadiusRange[3] || rand() % 3)) {
                bit_set(x_radius, y_radius, BIT_FUNGUS, true);
            } else {
                has_skipped = true;
            }
        }
    }
    new_landmark(x, y, StringTable->get((int)Natural[LM_SARGASSO].name));
}

/*
Purpose: Setup 'The Ruins' landmark.
Original Offset: 005C64A0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_ruin(int x, int y) {
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    for (int i = 0; i < RadiusRange[2]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius)) {
            if (i < RadiusRange[2] || !is_ocean(x_radius, y_radius)) { // is_ocean check pointless?
                world_alt_set(x_radius, y_radius, ALT_SHORE_LINE, true);
            }
            if (is_ocean(x_radius, y)) {
                if (i && i < 9) {
                    bit_set(x_radius, y_radius, BIT_MONOLITH, true); // in ocean tiles?
                }
            } else {
                bit_set(x_radius, y, BIT_FUNGUS | BIT_SUPPLY_REMOVE, true);
                if (!i) {
                    bit_set(x_radius, y_radius, BIT_FUNGUS, false);
                } else if (i < 9) {
                    bit_set(x_radius, y_radius, BIT_MONOLITH, true);
                }
            }
            bit2_set(x_radius, y_radius, BIT2_RUINS, true);
            code_set(x_radius, y_radius, i);
        }
    }
    new_landmark(x, y, StringTable->get((int)Natural[LM_RUINS].name));
}

/*
Purpose: Setup the 'Great Dunes' landmark.
Original Offset: 005C6740
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_dune(int x, int y) {
    world_rainfall();
    uint32_t loc_attempts = 0;
    uint32_t half_vert_bounds = *MapLatitudeBounds / 2;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(half_vert_bounds, NULL) + half_vert_bounds - *MapLatitudeBounds / 4;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y) || climate_at(x, y) != RAINFALL_ARID);
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    for (int i = 0; i < RadiusRange[2]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius) && (!is_ocean(x_radius, y_radius) || i < 9 || rand() % 3)) {
            if (i == 2 || i == 7) {
                world_alt_set(x_radius, y_radius, ALT_1_LEVEL_ABOVE_SEA, true);
            } else {
                world_alt_set(x_radius, y_radius, ALT_SHORE_LINE, true);
            }
            bit_set(x_radius, y_radius, BIT_FUNGUS, false);
            bit2_set(x_radius, y_radius, LM_DUNES, true);
            rocky_set(x_radius, y_radius, ROCKINESS_FLAT);
            code_set(x_radius, y_radius, i);
        }
    }
    new_landmark(x, y + 2, StringTable->get((int)Natural[LM_DUNES].name));
}

/*
Purpose: Setup the 'Uranium Flats' landmark.
Original Offset: 005C69E0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_diamond(int x, int y) {
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    for (uint32_t i = 0; i < 21; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius)) {
            if (i < 9 || !is_ocean(x_radius, y_radius) || rand() % 3) {
                world_alt_set(x_radius, y_radius, ALT_SHORE_LINE, true);
                bit2_set(x_radius, y_radius, BIT2_URANIUM, true);
                rocky_set(x_radius, y_radius, ROCKINESS_FLAT);
                bit_set(x_radius, y_radius, BIT_FUNGUS, false);
                code_set(x_radius, y_radius, i);
            }
        }
    }
    new_landmark(x, y, StringTable->get((int)Natural[LM_URANIUM].name));
}

/*
Purpose: Setup the 'Freshwater Sea' landmark.
Original Offset: 005C6C40
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_fresh(int x, int y) {
    uint32_t region;
    if (on_map(x, y)) {
        region = region_at(x, y);
        if (!is_ocean(x, y)) {
            return;
        }
    } else {
        uint32_t tile_search = 0;
        int region_search = -1;
        for (uint32_t i = MaxLandmarkNum; i < MaxContinentNum; i++) {
            uint32_t tile_count = Continents[i].tile_count;
            if (tile_count >= 3 && tile_count <= 32 && tile_count >= tile_search) {
                tile_search = tile_count;
                region_search = i;
            }
        }
        if (region_search < 0) {
            return;
        }
        region = region_search;
    }
    int x_search = -1;
    BOOL has_set_landmark = false;
    for (int y_it = *MapLatitudeBounds - 1; y_it >= 0 ; y_it--) {
        for (int x_it = y_it & 1; x_it < *MapLongitudeBounds; x_it += 2) {
            if (region_at(x_it, y_it) == region) {
                bit2_set(x_it, y_it, LM_FRESH, true);
                if (x_search < 0) {
                    x_search = x_it;
                }
            } else if(!has_set_landmark && x_search >= 0) {
                int x_fresh = (x_search + x_it - 2) / 2;
                new_landmark(((x_fresh ^ y_it) & 1) ^ x_fresh, y_it,
                    StringTable->get((int)Natural[LM_FRESH].name));
                has_set_landmark = true;
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
void __cdecl world_volcano(int x, int y, BOOL is_not_landmark) {
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    world_alt_set(x, y, ALT_BIT_OCEAN_TRENCH, true); // purpose??
    world_alt_set(x, y, ALT_3_LEVELS_ABOVE_SEA, true);
    for (int i = 0; i < RadiusRange[1]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius)) {
            bit2_set(x_radius, y_radius, BIT2_VOLCANO, true);
            code_set(x_radius, y_radius, i);
            uint32_t bit = bit_at(x_radius, y_radius) & ~(BIT_SUPPLY_POD | BIT_THERMAL_BORE
                | BIT_ECH_MIRROR | BIT_CONDENSER | BIT_SOIL_ENRICHER | BIT_FARM | BIT_RIVERBED
                | BIT_SOLAR_TIDAL | BIT_FUNGUS | BIT_MINE | BIT_MAGTUBE | BIT_ROAD);
            if (!i) {
                bit &= ~(BIT_UNK_4000000 | BIT_UNK_8000000);
            }
            bit |= BIT_SUPPLY_REMOVE;
            bit_put(x_radius, y_radius, bit);
            rocky_set(x_radius, y_radius, ROCKINESS_ROCKY);
        }
    }
    *MountPlanetX = x;
    *MountPlanetY = y;
    if (!is_not_landmark) {
        new_landmark(x, y, StringTable->get((int)Natural[LM_VOLCANO].name));
    }
}

/*
Purpose: Setup the 'Borehole Cluster' landmark. Added to SMAC in 3.0 patch.
Original Offset: 005C7020
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_borehole(int x, int y) {
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    uint32_t seed = timeGetTime();
    int val1 = 8;
    int val2 = ((seed % 8) / 3) + 5;
    int val3 = 3 - ((seed % 8) % 3);
    uint32_t val4 = (seed / 8) % 4;
    if (val4 & 2) {
        val2--;
        val3--;
        if (val4 & 1) {
            val2++;
            val1 = 8;
            val3++;
        } else {
            val2--;
            val1 = 6;
            val3--;
        }
        val1 = (val1 + 8) % 8 + 1;
        val2 = (val2 + 8) % 8 + 1;
        val3 = (val3 + 8) % 8 + 1;
    }
    for (int i = 0; i < RadiusRange[1]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius)) {
            world_alt_set(x_radius, y_radius, ALT_SHORE_LINE, true);
            bit_set(x_radius, y_radius, BIT_SUPPLY_REMOVE, true);
            if (i == val1 || i == val2 || i == val3) {
                bit_set(x_radius, y_radius, BIT_THERMAL_BORE, true);
                bit2_set(x_radius, y_radius, LM_BOREHOLE, true);
                code_set(x_radius, y_radius, i);
            }
        }
    }
    int x_offset_val1 = xrange(x + RadiusOffsetX[val1]); // removed from loop
    int y_offset_val1 = y + RadiusOffsetY[val1]; // removed from loop
    for (int i = 0; i < 8; i++) {
        int x_radius = xrange(x_offset_val1 + RadiusOffsetX[i + 1]);
        int y_radius = y_offset_val1 + RadiusOffsetY[i + 1];
        if (on_map(x_radius, y_radius) && is_ocean(x_radius, y_radius)) {
            world_alt_set(x_radius, y_radius, ALT_SHORE_LINE, true);
        }
    }
    int x_offset_val2 = xrange(x + RadiusOffsetX[val2]); // removed from loop
    int y_offset_val2 = y + RadiusOffsetY[val2]; // removed from loop
    for (int i = 0; i < 8; i++) {
        int x_radius = xrange(x_offset_val2 + RadiusOffsetX[i + 1]);
        int y_radius = y_offset_val2 + RadiusOffsetY[i + 1];
        if (on_map(x_radius, y_radius) && is_ocean(x_radius, y_radius)) {
            world_alt_set(x_radius, y_radius, ALT_SHORE_LINE, true);
        }
    }
    int x_offset_val3 = xrange(x + RadiusOffsetX[val3]); // removed from loop
    int y_offset_val3 = y + RadiusOffsetY[val3]; // removed from loop
    for (int i = 0; i < 8; i++) {
        int x_radius = xrange(x_offset_val3 + RadiusOffsetX[i + 1]);
        int y_radius = y_offset_val3 + RadiusOffsetY[i + 1];
        if (on_map(x_radius, y_radius) && is_ocean(x_radius, y_radius)) {
            world_alt_set(x_radius, y_radius, ALT_SHORE_LINE, true);
        }
    }
    bit2_set(x, y, LM_BOREHOLE, true);
    new_landmark(x, y, StringTable->get((int)Natural[LM_BOREHOLE].name));
}

/*
Purpose: Setup 'The Manifold Nexus' landmark. Added to SMAC in 4.0 patch.
Original Offset: 005C7540
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_temple(int x, int y) {
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    for (int i = 0; i < RadiusRange[1]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius)) {
            world_alt_set(x_radius, y_radius, ALT_SHORE_LINE, true);
            bit_set(x_radius, y_radius, BIT_SUPPLY_REMOVE, true);
            bit2_set(x_radius, y_radius, LM_NEXUS, true);
            code_set(x_radius, y_radius, i);
        }
    }
    new_landmark(x, y, StringTable->get((int)Natural[LM_NEXUS].name));
}

/*
Purpose: Setup the 'Unity Wreckage' landmark (SMACX only).
Original Offset: 005C7750
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_unity(int x, int y) {
    if (*ExpansionEnabled) {
        uint32_t loc_attempts = 0;
        if (!on_map(x, y)) {
            do {
                do {
                    y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                    int x_seed = rnd(*MapLongitudeBounds, NULL);
                    x = ((x_seed ^ y) & 1) ^ x_seed;
                    if (++loc_attempts >= 1000) {
                        return;
                    }
                } while (is_ocean(x, y));
            } while (near_landmark(x, y));
            if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
                return;
            }
        }
        x--;
        y--;
        for (int i = 0; i < RadiusRange[2]; i++) {
            int x_radius = xrange(x + RadiusOffsetX[i]);
            int y_radius = y + RadiusOffsetY[i];
            if (on_map(x_radius, y_radius)) {
                world_alt_set(x_radius, y_radius, ALT_1_LEVEL_ABOVE_SEA, true);
            }
        }
        x += 2;
        y += 2;
        for (int i = 0; i < RadiusRange[2]; i++) {
            int x_radius = xrange(x + RadiusOffsetX[i]);
            int y_radius = y + RadiusOffsetY[i];
            if (on_map(x_radius, y_radius)) {
                world_alt_set(x_radius, y_radius, ALT_1_LEVEL_ABOVE_SEA, true);
            }
        }
        x--;
        y--;
        for (int i = 0; i < RadiusRange[2]; i++) {
            int x_radius = xrange(x + RadiusOffsetX[i]);
            int y_radius = y + RadiusOffsetY[i];
            if (on_map(x_radius, y_radius)) {
                rocky_set(x_radius, y_radius, ROCKINESS_FLAT);
                bit_set(x_radius, y_radius, BIT_RIVERBED, false);
                bit_set(x_radius, y_radius, BIT_FUNGUS, false);
                bit2_set(x_radius, y_radius, LM_UNITY, true);
                code_set(x_radius, y_radius, i);
                if (!i || i == 8 || i == 10 || i == 19) {
                    bit_set(x_radius, y_radius, BIT_SUPPLY_POD, true);
                } else {
                    bit_set(x_radius, y_radius, BIT_SUPPLY_REMOVE, true);
                }
            }
        }
        new_landmark(x, y, StringTable->get((int)Natural[LM_UNITY].name));
    }
}

/*
Purpose: Setup the 'Fossil Ridge' landmark (SMACX only).
Original Offset: 005C7A80
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_fossil(int x, int y) {
    if (*ExpansionEnabled) { // added check similar to Unity Wreckage
        uint32_t loc_attempts = 0;
        if (!on_map(x, y)) {
            do {
                do {
                    y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                    int x_seed = rnd(*MapLongitudeBounds, NULL);
                    x = ((x_seed ^ y) & 1) ^ x_seed;
                    if (++loc_attempts >= 1000) {
                        return;
                    }
                } while (!is_ocean(x, y));
            } while (near_landmark(x, y));
            if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
                return;
            }
        }
        for (uint32_t i = 0; i < 6; i++) {
            int x_radius = xrange(x + RadiusOffsetX[i]);
            int y_radius = y + RadiusOffsetY[i];
            if (on_map(x_radius, y_radius) && is_ocean(x_radius, y_radius)) {
                world_alt_set(x_radius, y_radius, ALT_OCEAN, true);
                bit2_set(x_radius, y_radius, LM_FOSSIL, true);
                code_set(x_radius, y_radius, i);
            }
        }
        new_landmark(x, y, StringTable->get((int)Natural[LM_FOSSIL].name));
    }
}

/*
Purpose: Setup the 'Nessus Canyon' landmark.
Original Offset: 005C7CB0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_canyon(int x, int y) {
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    } 
    const uint32_t WorldCanyonOffsetValues[12] = { 26, 27, 9, 1, 2, 0, 3, 4, 17, 23, 36, 35 };
    for (uint32_t i = 0; i < 12; i++) {
        int x_radius = xrange(x + RadiusOffsetX[WorldCanyonOffsetValues[i]]);
        int y_radius = y + RadiusOffsetY[WorldCanyonOffsetValues[i]];
        if (on_map(x_radius, y_radius)) {
            world_alt_set(x_radius, y_radius, ALT_1_LEVEL_ABOVE_SEA, true);
            world_alt_set(x_radius, y_radius, ALT_SHORE_LINE, true);
            bit2_set(x_radius, y_radius, LM_CANYON, true); // rolled these two into single loop with
            code_set(x_radius, y_radius, WorldCanyonOffsetValues[i]); // the above world_alt_set()
        }
    }
    new_landmark(x, y + 2, StringTable->get((int)Natural[LM_CANYON].name));
}

/*
Purpose: Setup the 'Sunny Mesa' landmark.
Original Offset: 005C7F40
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_mesa(int x, int y) {
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    for (int i = 0; i < RadiusRange[3]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius)) {
            world_alt_set(x_radius, y_radius, (i < RadiusRange[2]) + ALT_SHORE_LINE, true);
            bit2_set(x_radius, y_radius, LM_MESA, true);
            code_set(x_radius, y_radius, i);
        }
    }
    new_landmark(x, y + 2, StringTable->get((int)Natural[LM_MESA].name));
}

/*
Purpose: Setup the 'Pholus Ridge' landmark.
Original Offset: 005C8150
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_ridge(int x, int y) {
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    const uint32_t world_ridge_offset_val[13] = { 47, 44, 24, 20, 8, 7, 0, 5, 4, 17, 23, 35, 455 };
    for (uint32_t i = 0; i < 12; i++) {
        int x_radius = xrange(x + RadiusOffsetX[world_ridge_offset_val[i]]);
        int y_radius = y + RadiusOffsetY[world_ridge_offset_val[i]];
        if (on_map(x_radius, y_radius)) {
            world_alt_set(x_radius, y_radius, ALT_1_LEVEL_ABOVE_SEA, true);
            bit2_set(x_radius, y_radius, LM_RIDGE, true); // rolled these two into single loop with
            code_set(x_radius, y_radius, world_ridge_offset_val[i]); // the above world_alt_set()
        }
    }
    new_landmark(x, y + 2, StringTable->get((int)Natural[LM_RIDGE].name));
}

/*
Purpose: Setup the 'Geothermal Shallows' landmark.
Original Offset: 005C83B0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl world_geothermal(int x, int y) {
    uint32_t loc_attempts = 0;
    if (!on_map(x, y)) {
        do {
            do {
                y = rnd(*MapLatitudeBounds - 16, NULL) + 8;
                int x_seed = rnd(*MapLongitudeBounds, NULL);
                x = ((x_seed ^ y) & 1) ^ x_seed;
                if (++loc_attempts >= 1000) {
                    return;
                }
            } while (!is_ocean(x, y));
        } while (near_landmark(x, y));
        if (loc_attempts >= 1000) { // redundant check? would have already returned, remove?
            return;
        }
    }
    BOOL has_skipped = false;
    for (int i = 0; i < RadiusRange[2]; i++) {
        int x_radius = xrange(x + RadiusOffsetX[i]);
        int y_radius = y + RadiusOffsetY[i];
        if (on_map(x_radius, y_radius) && (is_ocean(x_radius, y_radius) || i < 9)) {
            if ((has_skipped || !i || rand() % 25) && (i < 9 || rand() % 3)) {
                world_alt_set(x_radius, y_radius, ALT_OCEAN_SHELF, true);
                bit2_set(x_radius, y_radius, LM_GEOTHERMAL, true);
                code_set(x_radius, y_radius, i);
            } else {
                has_skipped = true;
            }
            world_alt_set(x_radius, y_radius, (i < RadiusRange[2]) + ALT_SHORE_LINE, true);
            bit2_set(x_radius, y_radius, LM_MESA, true);
            code_set(x_radius, y_radius, i);
        }
    }
    new_landmark(x, y, StringTable->get((int)Natural[LM_GEOTHERMAL].name));
}

/*
Purpose: Set up all the landmarks.
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
Purpose: Check for any type of zone of control conflicts (base and/or unit).
Original Offset: 005C89F0
Return Value: If ZOC, faction id + 1; Otherwise, 0 (however return seems to be treated as boolean)
Status: Complete
*/
uint32_t __cdecl zoc_any(int x, int y, uint32_t faction_id) {
    for (uint32_t i = 0; i < 8; i++) {
        int x_radius = xrange(x + RadiusBaseX[i]);
        int y_radius = y + RadiusBaseY[i];
        if (on_map(x_radius, y_radius)) {
            int owner = anything_at(x_radius, y_radius);
            if (owner >= 0 && (uint32_t)owner != faction_id
                && !has_treaty(faction_id, owner, DTREATY_PACT)) {
                return owner + 1;
            }
        }
    }
    return 0;
}

/*
Purpose: Check for unit related zone of control conflicts.
Original Offset: 005C8AC0
Return Value: If ZOC, faction id + 1; Otherwise, 0 (however return seems to be treated as boolean)
Status: Complete
*/
uint32_t __cdecl zoc_veh(int x, int y, uint32_t faction_id) {
    uint32_t ret = 0;
    for (uint32_t i = 0; i < 8; i++) {
        int x_radius = xrange(x + RadiusBaseX[i]);
        int y_radius = y + RadiusBaseY[i];
        if (on_map(x_radius, y_radius)) {
            int owner = veh_who(x_radius, y_radius);
            if (owner >= 0 && (uint32_t)owner != faction_id
                && !has_treaty(faction_id, owner, DTREATY_PACT)) {
                owner++;
                if (ret <= (uint32_t)owner) {
                    ret = owner; // any point in continuing after 1st instance of ZOC is found?
                }
            }
        }
    }
    return ret;
}

/*
Purpose: Check for unit related zone of control conflicts taking into account land or ocean.
Original Offset: 005C8BA0
Return Value: If ZOC, faction id + 1; Otherwise, 0 (however return seems to be treated as boolean)
Status: Complete
*/
uint32_t __cdecl zoc_sea(int x, int y, uint32_t faction_id) {
    BOOL is_ocean_tile = is_ocean(x, y);
    for (uint32_t i = 0; i < 8; i++) {
        int x_radius = xrange(x + RadiusBaseX[i]);
        int y_radius = y + RadiusBaseY[i];
        if (on_map(x_radius, y_radius)) {
            int owner = veh_who(x_radius, y_radius);
            if (owner >= 0 && (uint32_t)owner != faction_id
                && is_ocean(x_radius, y_radius) == is_ocean_tile
                && !has_treaty(faction_id, owner, DTREATY_PACT)) {
                for (int veh_id = veh_at(x_radius, y_radius); veh_id >= 0;
                    veh_id = Vehs[veh_id].next_veh_id_stack) {
                    if (Vehs[veh_id].faction_id != faction_id
                        && (Vehs[veh_id].visibility & (1 << faction_id)
                            || (!*IsMultiplayerNet && !(Vehs[veh_id].flags & VFLAG_INVISIBLE)))) {
                        return owner + 1;
                    }
                }
            }
        }
    }
    return 0;
}

/*
Purpose: Check for unit related zone of control conflicts when moving to a non-base tile.
Original Offset: 005C8D40
Return Value: If ZOC, faction id + 1; Otherwise, 0 (however return seems to be treated as boolean)
Status: Complete
*/
uint32_t __cdecl zoc_move(uint32_t x, uint32_t y, uint32_t faction_id) {
    int owner;
    if (!(bit_at(x, y) & BIT_BASE_IN_TILE)
        || ((owner = owner_at(x, y)), owner >= 8 || owner < 0)) {
        return zoc_sea(x, y, faction_id);
    }
    return 0;
}

/*
Purpose: Take two points and calculate the cursor distance between them.
Original Offset: n/a
Return Value: Cursor distance
Status: Complete
*/
int __cdecl cursor_dist(int x_point_a, int y_point_a, int x_point_b, int y_point_b) {
    return (x_dist(x_point_a, x_point_b) + abs(y_point_a - y_point_b)) / 2;
}

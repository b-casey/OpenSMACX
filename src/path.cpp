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
#include "path.h"
#include "alpha.h"
#include "game.h"
#include "technology.h"
#include "veh.h"
#include "map.h"

/*
Purpose: Initialize an instance of the class.
Original Offset: 0059A220
Return Value: n/a
Status: Complete
*/
void Path::init() {
    shutdown();
    x_dst_ = -1;
    y_dst_ = -1;
    map_table_ = (int *)mem_get(*MapArea * 4);
    x_table_ = (int16_t *)mem_get(*MapArea * 2);
    y_table_ = (int16_t *)mem_get(*MapArea * 2);
    ZeroMemory(map_table_, *MapArea * 4);
}

/*
 Purpose: Shutdown an instance of the class.
 Original Offset: 0059A2D0
 Return Value: n/a
 Status: Complete
*/
void Path::shutdown() {
    if (map_table_) {
        free(map_table_);
    }
    if (x_table_) {
        free(x_table_);
    }
    if (y_table_) {
        free(y_table_);
    }
    map_table_ = 0;
    x_table_ = 0;
    y_table_ = 0;
}

/*
 Purpose: Get the value from the map table for the specified coordinates.
 Original Offset: n/a (Optimized out of x86 binary, present in PPC)
 Return Value: Map table value
 Status: Complete
*/
int Path::get(uint32_t x, uint32_t y) {
    return map_table_[(x >> 1) + y * *MapLongitude];
}

/*
 Purpose: Set the value in the map table for the specified coordinates.
 Original Offset: n/a
 Return Value: n/a
 Status: Complete
*/
void Path::set(uint32_t x, uint32_t y, int val) {
    map_table_[(x >> 1) + y * *MapLongitude] = val;
}

/*
 Purpose: Check for unit zone of control conflicts taking into account land or ocean. This is a 
          slightly modified version of zoc_move() or zoc_sea().
 Original Offset: 0059A370
 Return Value: If ZOC, faction id + 1; Otherwise, 0 (however return seems to be treated as boolean)
 Status: Complete
*/
int Path::zoc_path(uint32_t x, uint32_t y, uint32_t faction_id) {
    if (bit_at(x, y) & BIT_BASE_IN_TILE && owner_at(x, y) < 8) {
        return 0;
    }
    BOOL is_ocean_tile = is_ocean(x, y);
    for (uint32_t i = 0; i < 8; i++) {
        int x_radius = xrange(x + RadiusBaseX[i]);
        int y_radius = y + RadiusBaseY[i];
        if (on_map(x_radius, y_radius)) {
            int owner = veh_who(x_radius, y_radius);
            if (owner >= 0 && (uint32_t)owner != faction_id 
                && is_ocean(x_radius, y_radius) == is_ocean_tile
                && !has_treaty(faction_id, owner, DTREATY_VENDETTA | DTREATY_PACT)) {
                if (!is_human(faction_id)) {
                    return owner + 1;
                }
                for (int veh_id = veh_at(x_radius, y_radius); veh_id >= 0; 
                    veh_id = Vehs[veh_id].next_veh_id_stack) {
                    if ((Vehs[veh_id].faction_id == faction_id && ((Vehs[veh_id].flags
                        & (VFLAG_INVISIBLE | VFLAG_LURKER)) != (VFLAG_INVISIBLE | VFLAG_LURKER)))
                        || Vehs[veh_id].visibility & (1 << faction_id)) {
                        return owner + 1;
                    }
                }
            }
        }
    }
    return 0;
}

/*
 Purpose: Find the path between two points that meets the conditions?
 Original Offset: 0059A530
 Return Value: TBD
 Status: WIP
*/
int Path::find(int x_src, int y_src, int x_dst, int y_dst, int proto_id, int faction_id, int unk1,
               int unk2) {
    return 0;
}

/*
 Purpose: TBD
 Original Offset: 0059BC10
 Return Value: TBD
 Status: WIP
*/
int Path::move(uint32_t veh_id, int faction_id) {
    int veh_faction_id = Vehs[veh_id].faction_id;
    uint32_t flags = 0xC0;
    if (veh_faction_id != faction_id_) {
        x_dst_ = -1;
        y_dst_ = -1;
        faction_id_ = veh_faction_id;
    }
    int veh_x = Vehs[veh_id].x;
    int veh_y = Vehs[veh_id].y;
    int x_waypoint = Vehs[veh_id].waypoint_x[0]; 
    int y_waypoint = Vehs[veh_id].waypoint_y[0];
    if (!on_map(x_waypoint, y_waypoint) || (veh_x == x_waypoint && veh_y == y_waypoint)) {
        return -1;
    }
    BOOL is_human_player = is_human(veh_faction_id);
    if (!is_human_player && ((Vehs[veh_id].state & (VSTATE_UNK_40000 | VSTATE_UNK_20000))
        == (VSTATE_UNK_40000 | VSTATE_UNK_20000))) {
        flags = 0xE0;
    }
    if ((Vehs[veh_id].state & (VSTATE_UNK_1000000 | VSTATE_UNK_200))
        == (VSTATE_UNK_1000000 | VSTATE_UNK_200)) {
        flags &= 0xBF;
    } else {
        uint32_t moves = veh_moves(veh_id);
        if (moves <= Rules->move_rate_roads) {
            flags |= 0x100;
            if (moves <= (uint32_t)((Vehs[veh_id].proto_id != BSC_MIND_WORMS) + 1)) {
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
            for (uint32_t i = 0; i < RadiusRange[1]; i++) {
                int x_radius = xrange(x * 5 + RadiusOffsetX[i]);
                int y_radius = y * 5 + RadiusOffsetY[i];
                if (on_map(x_radius, y_radius) && !is_ocean(x_radius, y_radius)) {
                    region = (uint8_t)region_at(x_radius, y_radius);
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
void Path::merge(uint32_t region_old, uint32_t region_new) {
    Continents[region_new].tile_count += Continents[region_old].tile_count;
    Continents[region_old].tile_count = 0;
    for (uint32_t i = 0; i < *MapArea; i++) {
        if ((uint32_t)(*MapTiles)[i].region == region_old) {
            (*MapTiles)[i].region = (uint8_t)region_new;
        }
    }
}

/*
 Purpose: Build the pathing tables using the provided coordinates to radiate outward for connected 
          land tiles owned by the specified faction.
 Original Offset: 0059C3C0
 Return Value: n/a
 Status: Complete
*/
void Path::territory(uint32_t x, uint32_t y, int UNUSED(region), int faction_id) {
    if (is_ocean(x, y)) {
        return; // skip ocean terrain
    }
    index1_ = 0; 
    index2_ = 0;
    x_table_[index1_] = (int16_t)x;
    y_table_[index1_++] = (int16_t)y;
    do {
        int16_t x_it = x_table_[index2_];
        int16_t y_it = y_table_[index2_++];
        for (uint32_t i = 0; i < 8; i++) {
            int x_radius = xrange(x_it + RadiusBaseX[i]);
            int y_radius = y_it + RadiusBaseY[i];
            Map *tile;
            if (on_map(x_radius, y_radius) && y_it && y_it != ((int)*MapLatitudeBounds - 1)
                && !is_ocean(x_radius, y_radius) && (tile = map_loc(x_radius, y_radius),
                    !tile->unk_1 && tile->territory == faction_id)) {
                tile->unk_1 = 1;
                x_table_[index1_] = (int16_t)x_radius;
                y_table_[index1_++] = (int16_t)y_radius;
            }
        }
    } while (index2_ && index1_ != index2_);
    do_all_non_input();
}

/*
 Purpose: Populate the continent and path table for the specified tile and region.
 Original Offset: 0059C520
 Return Value: n/a
 Status: Complete
*/
void Path::continent(uint32_t x, uint32_t y, uint32_t region) {
    Continents[region].tile_count = 0;
    index1_ = 0; 
    index2_ = 0;
    uint32_t fresh_water_count = 0;
    x_table_[index1_] = (int16_t)x;
    y_table_[index1_++] = (int16_t)y;
    region_set(x, y, (uint8_t)region);
    BOOL is_ocean_tile = is_ocean(x, y);
    do {
        int16_t x_it = x_table_[index2_];
        int16_t y_it = y_table_[index2_++];
        Continents[region].tile_count++;
        for (uint32_t i = 0; i < 8; i++) {
            int x_radius = xrange(x_it + RadiusBaseX[i]);
            int y_radius = y_it + RadiusBaseY[i];
            BOOL is_ocean_rad;
            if (on_map(x_radius, y_radius) && y_it && y_it != ((int)*MapLatitudeBounds - 1)
                && (is_ocean_rad = is_ocean(x_radius, y_radius), is_ocean_rad == is_ocean_tile 
                    && !region_at(x_radius, y_radius))) {
                if (is_ocean_tile && bit2_at(x_radius, y_radius) & BIT2_FRESH && is_ocean_rad) {
                    fresh_water_count++;
                }
                region_set(x_radius, y_radius, (uint8_t)region);
                x_table_[index1_] = (int16_t)x_radius;
                y_table_[index1_++] = (int16_t)y_radius;
            }
        }
    } while (index2_ && index1_ != index2_);
    if (fresh_water_count) {
        // land locked?
        BOOL is_fresh_water = fresh_water_count >= ((Continents[region].tile_count * 3) / 4);
        for (uint32_t y_it = 0; y_it < *MapLatitudeBounds; y_it++) {
            for (uint32_t x_it = y_it & 1; x_it < *MapLongitudeBounds; x_it += 2) {
                if (region_at(x_it, y_it) == region) {
                    bit2_set(x_it, y_it, BIT2_FRESH, is_fresh_water);
                }
            }
        }
    }
    do_all_non_input();
}

/*
 Purpose: Populate and set up all the continents and path tables.
 Original Offset: 0059C790
 Return Value: n/a
 Status: Complete
*/
void Path::continents() {
    for (uint32_t i = 0; i < *MapArea; i++) {
        (*MapTiles)[i].region = 0;
    }
    uint32_t y_south_pole = *MapLatitudeBounds - 1;
    uint32_t ocean_region = 64;
    uint32_t land_region = 0;
    for (uint32_t y = 1; y < y_south_pole; y++) {
        for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
            if (!region_at(x, y)) {
                uint32_t region_current;
                uint32_t region_min;
                uint32_t region_max;
                if (is_ocean(x, y)) {
                    ocean_region++;
                    region_min = 64;
                    region_max = 126;
                    region_current = range(ocean_region, region_min, region_max);
                } else {
                    land_region++;
                    region_min = 0;
                    region_max = 62;
                    region_current = range(land_region, region_min, region_max);
                }
                continent(x, y, region_current);
                if (region_current == region_max) {
                    uint32_t tiles = Continents[region_current].tile_count;
                    int search_region = -1;
                    for (uint32_t i = region_min; i < region_max; i++) {
                        if (Continents[i].tile_count < tiles) {
                            tiles = Continents[i].tile_count;
                            search_region = i;
                        }
                    }
                    if (search_region >= 0) {
                        merge(search_region, region_max + 1);
                        merge(region_current, search_region);
                    } else {
                        merge(region_current, region_max + 1);
                    }
                }
            }
        }
    }
    for (uint32_t x = 0; x < *MapLongitudeBounds; x += 2) { // north pole
        uint8_t pole_region = is_ocean(x, 0) ? 127 : 63;
        region_set(x, 0, pole_region);
        Continents[pole_region].tile_count++;
    }
    for (uint32_t x = y_south_pole & 1; x < *MapLongitudeBounds; x += 2) { // south pole
        uint8_t pole_region = is_ocean(x, y_south_pole) ? 127 : 63;
        region_set(x, y_south_pole, pole_region);
        Continents[pole_region].tile_count++;
    }
    uint32_t most_tiles = 0;
    uint32_t total_tiles = 0;
    for (uint32_t i = 1; i < 63; i++) {
        uint32_t tiles = Continents[i].tile_count;
        total_tiles += tiles;
        if (tiles > most_tiles) {
            most_tiles = tiles;
        }
    }
    *GameState = (most_tiles >= ((total_tiles * 4) / 5))
        ? *GameState | STATE_UNK_100 : *GameState & ~STATE_UNK_100;
    for (uint32_t i = 0; i < MaxRegionLandNum; i++) {
        ZeroMemory(Continents[i].sea_coasts, 8);
    }
    for (uint32_t y = 0; y < *MapLatitudeBounds; y++) {
        for (uint32_t x = y & 1; x < *MapLongitudeBounds; x += 2) {
            uint32_t region = region_at(x, y);
            if (region < MaxRegionLandNum) {
                for (uint32_t i = 0; i < 8; i++) {
                    int x_radius = xrange(x + RadiusBaseX[i]);
                    int y_radius = y + RadiusBaseY[i];
                    if (on_map(x_radius, y_radius)) {
                        uint32_t region_rad = region_at(x_radius, y_radius);
                        if (region_rad >= MaxRegionLandNum) {
                            uint32_t offset;
                            uint32_t mask;
                            bitmask(region_rad - MaxRegionLandNum, &offset, &mask);
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
BOOL Path::sensors(uint32_t faction_id, int *x_sensor, int *y_sensor) {
    BOOL has_sensor = true;
    ZeroMemory(map_table_, *MapArea * 4);
    x_dst_ = -1;
    y_dst_ = -1;
    int x_search = *x_sensor;
    int y_search = *y_sensor;
    int search = 9999;
    uint32_t region = region_at(x_search, y_search);
    for (uint32_t y = 1; y < *MapAbstractLatBounds - 1; y++) {
        for (uint32_t x = y & 1; x < *MapAbstractLongBounds; x += 2) {
            int veh_faction_id;
            if (region_at(x, y) == region 
                && whose_territory(faction_id, x, y, NULL, false) == (int)faction_id
                && base_who(x, y) < 0 && (veh_faction_id = veh_who(x, y), veh_faction_id < 0 
                    || veh_faction_id == (int)faction_id
                    || has_treaty(faction_id, veh_faction_id, DTREATY_PACT))) {
                int zoc_faction_id = zoc_veh(x, y, faction_id);
                uint32_t bit = bit_at(x, y);
                Map *tile = map_loc(x, y);
                if (zoc_faction_id != 1 && (!zoc_faction_id
                    || has_treaty(faction_id, zoc_faction_id, DTREATY_PACT))
                    && bit & (BIT_MINE | BIT_SOLAR_TIDAL)
                    && bit & (BIT_MONOLITH | BIT_CONDENSER | BIT_THERMAL_BORE)
                    && !bonus_at(x, y, 0) && ((tile->val3 & 0xC0u) > TERRAIN_BIT_ROLLING
                        || climate_at(x, y) == RAINFALL_ARID || !(bit & BIT_FUNGUS) 
                        || !is_ocean(x, y)) && (!(bit & BIT_FUNGUS) || (!is_ocean(x, y) 
                        && has_tech(Rules->tech_improve_fungus_sqr, faction_id)))) {
                    uint32_t flags = 0;
                    for (uint32_t i = 0; i < RadiusRange[2]; i++) {
                        int x_radius = xrange(x + RadiusOffsetX[i]);
                        int y_radius = y + RadiusOffsetY[i];
                        if (on_map(x_radius, y_radius) && !is_sensor(x_radius, y_radius)
                            && (whose_territory(faction_id, x_radius, y_radius, NULL, false)
                                == (int)faction_id || get(x_radius, y_radius))) {
                            if (i >= 9) {
                                flags |= 1;
                            }
                            flags |= 2;
                        }
                    }
                    if (!(flags & 1)) {
                        int proxminity = vector_dist(x_search, y_search, x, y);
                        for (uint32_t i = 0; i < 25; i++) {
                            int tile_faction_id;
                            int x_radius = xrange(x + RadiusOffsetX[i]);
                            int y_radius = y + RadiusOffsetY[i];
                            if (on_map(x_radius, y_radius) 
                                && (tile_faction_id = base_who(x_radius, y_radius), 
                                    tile_faction_id == (int)faction_id)) {
                                flags |= 4;
                                BOOL check = true;
                                for (uint32_t j = 0; j < RadiusRange[2]; j++) {
                                    int x_radius2 = xrange(x_radius + RadiusOffsetX[j]);
                                    int y_radius2 = y_radius + RadiusOffsetY[j];
                                    if (on_map(x_radius2, y_radius2)
                                        && !is_sensor(x_radius2, y_radius2)
                                        && (whose_territory(faction_id, x_radius2, y_radius2, NULL,
                                            false) == (int)faction_id 
                                            || get(x_radius2, y_radius2))) {
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
                        } else {
                            proxminity += 200;
                        }
                        if (proxminity < search) {
                            *x_sensor = x;
                            *y_sensor = y;
                            search = proxminity;
                            has_sensor = false;
                        }
                    }
                }
            }
        }
    }
    do_all_non_input();
    return has_sensor;
}

// global
Path *Paths = (Path *)0x00945B00;

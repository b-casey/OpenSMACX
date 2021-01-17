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
#pragma once

 /*
  * Map related objects, variables and functions.
  */
enum TerrainAltitudeBit {
    ALT_BIT_OCEAN_TRENCH = 0x0,
    ALT_BIT_OCEAN = 0x20,
    ALT_BIT_OCEAN_SHELF = 0x40,
    ALT_BIT_SHORE_LINE = 0x60,
    ALT_BIT_1_LEVEL_ABOVE_SEA = 0x80,
    ALT_BIT_2_LEVELS_ABOVE_SEA = 0xA0,
    ALT_BIT_3_LEVELS_ABOVE_SEA = 0xC0,
};

enum TerrainAltitude {
    ALT_OCEAN_TRENCH = 0,
    ALT_OCEAN = 1,
    ALT_OCEAN_SHELF = 2,
    ALT_SHORE_LINE = 3,
    ALT_1_LEVEL_ABOVE_SEA = 4,
    ALT_2_LEVELS_ABOVE_SEA = 5,
    ALT_3_LEVELS_ABOVE_SEA = 6,
};

enum TerrainRainfallBit { // land modifiers only
    RAINFALL_ARID = 0x0,
    RAINFALL_MOIST = 0x8, // can only have one value set, otherwise tile glitches
    RAINFALL_RAINY = 0x10, // " "
};

enum TerrainRockBit { // land modifiers only
    TERRAIN_BIT_ROLLING = 0x40, // can only have one value set, otherwise game will crash
    TERRAIN_BIT_ROCKY = 0x80, // " "
};

enum TerrainRockiness { // land modifiers only
    ROCKINESS_FLAT = 0,
    ROCKINESS_ROLLING = 1, // can only have one value set, otherwise game will crash
    ROCKINESS_ROCKY = 2, // " "
};

enum TerrainBit : uint32_t {
    BIT_BASE_IN_TILE = 0x1,
    BIT_VEH_IN_TILE = 0x2,
    BIT_ROAD = 0x4,
    BIT_MAGTUBE = 0x8,
    BIT_MINE = 0x10,
    BIT_FUNGUS = 0x20,
    BIT_SOLAR_TIDAL = 0x40,
    BIT_RIVER = 0x80,
    BIT_RIVERBED = 0x100, // River source for world river creation and visual effect
    BIT_RIVER_LAKE = 0x200, // Shows more water for visual effect
    BIT_RSC_BONUS = 0x400, // Mineral, Nutrient, Energy
    BIT_BUNKER = 0x800,
    BIT_BASE_RADIUS = 0x1000, // production radius; 21 tiles per base (20 surrounding + base tile)
    BIT_MONOLITH = 0x2000,
    BIT_UNK_4000 = 0x4000, // continent + single tile island off coast, used by AI; boundary?
    BIT_FARM = 0x8000,
    BIT_ENERGY_RSC = 0x10000,
    BIT_MINERAL_RSC = 0x20000,
    BIT_AIRBASE = 0x40000,
    BIT_SOIL_ENRICHER = 0x80000,
    BIT_SUPPLY_REMOVE = 0x100000, // Prevent randomly generated pods from appearing here
    BIT_FOREST = 0x200000,
    BIT_CONDENSER = 0x400000,
    BIT_ECH_MIRROR = 0x800000,
    BIT_THERMAL_BORE = 0x1000000,
    BIT_UNK_2000000 = 0x2000000, // related to monoliths
    BIT_UNK_4000000 = 0x4000000, // related to pods
    BIT_UNK_8000000 = 0x8000000, // related to pods
    BIT_SUPPLY_POD = 0x10000000,
    BIT_NUTRIENT_RSC = 0x20000000,
    BIT_UNK_40000000 = 0x40000000, // ?
    BIT_SENSOR_ARRAY = 0x80000000,
};

enum TerrainLandmarkBit2 {
    BIT2_CRATER = 0x1,
    BIT2_VOLCANO = 0x2,
    BIT2_JUNGLE = 0x4,
    BIT2_URANIUM = 0x8,
    BIT2_SARGASSO = 0x10,
    BIT2_RUINS = 0x20,
    BIT2_DUNES = 0x40,
    BIT2_FRESH = 0x80,
    BIT2_MESA = 0x100,
    BIT2_CANYON = 0x200,
    BIT2_GEOTHERMAL = 0x400,
    BIT2_RIDGE = 0x800,
    BIT2_BOREHOLE = 0x1000,
    BIT2_NEXUS = 0x2000,
    BIT2_UNITY = 0x4000,
    BIT2_FOSSIL = 0x8000,
    //
    BIT2_UNK_80000000 = 0x80000000,
};

enum TerrainLandmarkId {
    LM_CRATER = 0,
    LM_VOLCANO = 1,
    LM_JUNGLE = 2,
    LM_URANIUM = 3,
    LM_SARGASSO = 4,
    LM_RUINS = 5,
    LM_DUNES = 6,
    LM_FRESH = 7,
    LM_MESA = 8,
    LM_CANYON = 9,
    LM_GEOTHERMAL = 10,
    LM_RIDGE = 11,
    LM_BOREHOLE = 12,
    LM_NEXUS = 13,
    LM_UNITY = 14,
    LM_FOSSIL = 15,
};

enum ResourceType {
    RSC_NUTRIENTS = 0,
    RSC_MINERALS = 1,
    RSC_ENERGY = 2,
    RSC_PSI = 3, // dropped mechanic
};

/*
* Region notes:
* A tile's region is visible with debug mode in the bottom left of the main interface tile area
* Region id is used to index the [128] planning variable arrays in player_data struct
*
* 0: bad region, n/a
* 1...62: any offshoots from poles starting at 1,1 up to continents and islands
* 63: bad region - both poles land + single tile islands (this seems specific to huge Planet map)
* 64: bad region, n/a
* 65...126: start of water bodies from 1,1 onward (usually primary ocean) to smaller enclosed
* 127: bad region - both poles water
* 128: bad region, n/a
*/
struct Map {
    uint8_t climate; // 000 00 000 | altitude (3 bit) ; rainfall (2 bit) ; temperature (3 bit)
    uint8_t contour; // altitude details
    uint8_t val2; // 0000 0000 | site (0xF0) ; owner (0x0F) - last immediate control occupying tile
                                                            // or 0x0F for unoccupied
    uint8_t region; // grouping of disjoint water/land areas; see above notes for more details
    uint8_t visibility; // faction bitfield of those who can see tile (mapped: dim/bright)
    uint8_t val3; // 00 000 000 | rocky (2 bit); lock faction_id (3 bit); using faction_id (3 bit)
    uint8_t unk_1; // flags? bitfield
    int8_t territory; // physical owner factionID of geographic area or -1 for unclaimed
    uint32_t bit; // see terrain_bit
    uint32_t bit2; // bitwho[0]? FF FF FFFF | code (landmark tile sequence); unk flags; landmark id
    uint32_t bit_visible[7]; // how each faction sees tile (ex. pods another faction already got)
};

struct Landmark {
    int x;
    int y;
    char name[32];
};

struct Continent {
    uint32_t tile_count; // count of tiles in region
    uint32_t open_terrain; // count of non-rocky, non-fungus tiles (only 1 movement point to travel)
    uint32_t unk_3; // highest world_site value (0-15)
    uint32_t pods; // current count of supply and unity pods in region
    uint32_t unk_5; // padding?
    uint8_t sea_coasts[8]; // sea specific regions, connections to land regions? bitmask
};

struct RulesNatural {
    LPSTR name;
    LPSTR name_short;
};

constexpr int MaxNaturalNum = 16;
constexpr int MaxLandmarkNum = 64;
constexpr int RegionBounds = 63;
constexpr int MaxContinentNum = 128;
constexpr int MaxRegionLandNum = 64;

const uint32_t RadiusRange[] = { 1, 9, 25, 49, 81, 121, 169, 225, 289 };
const int RadiusBaseX[] = {  1, 2, 1, 0, -1, -2, -1,  0, 0 };
const int RadiusBaseY[] = { -1, 0, 1, 2,  1,  0, -1, -2, 0 };
const int RadiusOffsetX[] = {
     0,  1,  2,  1,  0, -1, -2,  -1,   0,   2,   2,  -2,  -2,   1,   3,   3,   1,  -1,  -3,  -3,
    -1,  4, -4,  0,  0,  1,  2,   3,   4,   5,   5,   4,   3,   2,   1,  -1,  -2,  -3,  -4,  -5,
    -5, -4, -3, -2, -1,  0,  6,   0,  -6,   0,   1,   2,   3,   4,   5,   6,   7,   8,   7,   6,
     5,  4,  3,  2,  1,  0, -1,  -2,  -3,  -4,  -5,  -6,  -7,  -8,  -7,  -6,  -5,  -4,  -3,  -2,
    -1,  0,  1,  2,  3,  4,  5,   6,   7,   8,   9,  10,   9,   8,   7,   6,   5,   4,   3,   2,
     1,  0, -1, -2, -3, -4, -5,  -6,  -7,  -8,  -9, -10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,
    -1,  0,  1,  2,  3,  4,  5,   6,   7,   8,   9,  10,  11,  12,  11,  10,   9,   8,   7,   6,
     5,  4,  3,  2,  1,  0, -1,  -2,  -3,  -4,  -5,  -6,  -7,  -8,  -9, -10, -11, -12, -11, -10,
    -9, -8, -7, -6, -5, -4, -3,  -2,  -1,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,
    11, 12, 13, 14, 13, 12, 11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0,  -1,  -2,
    -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -13, -12, -11, -10,  -9,  -8,  -7,  -6,
    -5, -4, -3, -2, -1,  0,  1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
    15, 16, 15, 14, 13, 12, 11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0,  -1,  -2,
    -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16, -15, -14, -13, -12, -11, -10,
    -9, -8, -7, -6, -5, -4, -3,  -2,  -1, };
const int RadiusOffsetY[] = {
     0,  -1,   0,   1,   2,   1,   0,  -1,  -2,  -2,   2,   2,  -2,  -3, -1,  1,  3,  3,  1, -1,
    -3,   0,   0,   4,  -4,  -5,  -4,  -3,  -2,  -1,   1,   2,   3,   4,  5,  5,  4,  3,  2,  1,
    -1,  -2,  -3,  -4,  -5,   6,   0,  -6,   0,  -8,  -7,  -6,  -5,  -4, -3, -2, -1,  0,  1,  2,
     3,   4,   5,   6,   7,   8,   7,   6,   5,   4,   3,   2,   1,   0, -1, -2, -3, -4, -5, -6,
    -7, -10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1,   0,   1,   2,  3,  4,  5,  6,  7,  8,
     9,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0,  -1,  -2, -3, -4, -5, -6, -7, -8,
    -9, -12, -11, -10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1,   0,  1,  2,  3,  4,  5,  6,
     7,   8,   9,  10,  11,  12,  11,  10,   9,   8,   7,   6,   5,   4,  3,  2,  1,  0, -1, -2,
    -3,  -4,  -5,  -6,  -7,  -8,  -9, -10, -11, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4,
    -3,  -2,  -1,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10, 11, 12, 13, 14, 13, 12,
    11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0,  -1,  -2, -3, -4, -5, -6, -7, -8,
    -9, -10, -11, -12, -13, -16, -15, -14, -13, -12, -11, -10,  -9,  -8, -7, -6, -5, -4, -3, -2,
    -1,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12, 13, 14, 15, 16, 15, 14,
    13,  12,  11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0, -1, -2, -3, -4, -5, -6,
    -7,  -8,  -9, -10, -11, -12, -13, -14, -15, };
const uint32_t ElevDetail[] = { 0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200 };

extern uint32_t *MapLongitudeBounds; // x
extern uint32_t *MapLatitudeBounds; // y
extern uint32_t *MapRandSeed;
extern int *MapSeaLevel;
extern int *MapSeaLevelCouncil; // Solar Shade (-3); Melt Polar Caps (+3)
extern uint32_t *MapArea;
extern uint32_t *MapAreaSqRoot;
extern BOOL *MapIsFlat;
extern uint32_t *MapLandmarkCount;
extern Landmark *MapLandmark;
extern uint32_t *MapAbstractLongBounds; // abstract x
extern uint32_t *MapAbstractLatBounds; // abstract y
extern uint32_t *MapAbstractArea;
extern uint32_t *MapSizePlanet;
extern uint32_t *MapOceanCoverage;
extern uint32_t *MapLandCoverage; // 2 - MapOceanCoverage
extern uint32_t *MapErosiveForces;
extern uint32_t *MapPlanetaryOrbit; // affects temp
extern uint32_t *MapCloudCover; // affects rainfall, rivers
extern uint32_t *MapNativeLifeForms;
extern LPSTR *MapFilePath;
extern Map **MapTiles;
extern uint8_t **MapAbstract;
extern uint32_t *MapBaseSubmergedCount;
extern int *MapBaseIdClosestSubmergedVeh;
extern uint32_t *BrushVal1; // TODO: more descriptive variable name
extern uint32_t *BrushVal2; // TODO: more descriptive variable name
extern uint32_t *WorldBuildVal1; // TODO: more descriptive variable name

extern Continent *Continents;
extern RulesNatural *Natural;
extern uint32_t *MapLongitude; // halve of MapLongitudeBounds
extern uint32_t *AltNatural; // Default: { 0, 15, 32, 45, 60,  75,  80, 100, 100, 100, 100 };

OPENSMACX_API BOOL __cdecl on_map(int x, int y);
OPENSMACX_API int __cdecl xrange(int x);
OPENSMACX_API int __cdecl whose_territory(uint32_t faction_id, uint32_t x, uint32_t y, int *base_id,
                                          BOOL ignore_comm);
OPENSMACX_API int __cdecl base_territory(uint32_t faction_id, uint32_t x, uint32_t y);
OPENSMACX_API uint32_t __cdecl crappy(uint32_t x, uint32_t y);
OPENSMACX_API int __cdecl vector_dist(int x_distance, int y_distance);
OPENSMACX_API int __cdecl vector_dist(int x_point_a, int y_point_a, int x_point_b, int y_point_b);
OPENSMACX_API BOOL __cdecl sea_coast(uint32_t region_dst, uint32_t region_src);
OPENSMACX_API uint32_t __cdecl sea_coasts(uint32_t region_src);
OPENSMACX_API BOOL __cdecl base_on_sea(uint32_t base_id, uint32_t region_sea);
OPENSMACX_API int __cdecl base_coast(uint32_t base_id);
OPENSMACX_API BOOL __cdecl port_to_coast(uint32_t base_id, uint32_t region);
OPENSMACX_API BOOL __cdecl port_to_port(uint32_t base_id_src, uint32_t base_id_dst);
OPENSMACX_API BOOL __cdecl transport_base(uint32_t base_id);
OPENSMACX_API BOOL __cdecl naval_base(uint32_t base_id);
OPENSMACX_API BOOL __cdecl convoy(uint32_t veh_id, uint32_t base_id);
OPENSMACX_API BOOL __cdecl bad_reg(int region);
OPENSMACX_API BOOL __cdecl get_there(uint32_t veh_id, uint32_t x_dst, uint32_t y_dst);
OPENSMACX_API BOOL __cdecl coast_or_border(uint32_t x_point_a, uint32_t y_point_a, 
                                           uint32_t x_point_b, uint32_t y_point_b, 
                                           uint32_t faction_id);
OPENSMACX_API Map *__cdecl map_loc(uint32_t x, uint32_t y);
OPENSMACX_API uint32_t __cdecl temp_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl temp_set(uint32_t x, uint32_t y, uint8_t temperature);
OPENSMACX_API uint32_t __cdecl climate_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl climate_set(uint32_t x, uint32_t y, uint8_t climate);
OPENSMACX_API int __cdecl elev_at(uint32_t x, uint32_t y);
OPENSMACX_API uint32_t __cdecl alt_natural(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl alt_set_both(uint32_t x, uint32_t y, uint32_t altitude);
OPENSMACX_API uint32_t __cdecl alt_at(uint32_t x, uint32_t y);
OPENSMACX_API uint32_t __cdecl altitude_at(uint32_t x, uint32_t y);
OPENSMACX_API uint32_t __cdecl alt_detail_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl alt_put_detail(uint32_t x, uint32_t y, uint8_t detail);
OPENSMACX_API uint32_t __cdecl owner_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl owner_set(uint32_t x, uint32_t y, int faction_id);
OPENSMACX_API void __cdecl site_set(uint32_t x, uint32_t y, int site);
OPENSMACX_API uint32_t __cdecl region_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl region_set(uint32_t x, uint32_t y, uint8_t region);
OPENSMACX_API uint32_t __cdecl using_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl using_set(uint32_t x, uint32_t y, int faction_id);
OPENSMACX_API uint32_t __cdecl lock_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl lock_set(uint32_t x, uint32_t y, uint32_t faction_id);
OPENSMACX_API BOOL __cdecl lock_map(uint32_t x, uint32_t y, uint32_t faction_id);
OPENSMACX_API void __cdecl unlock_map(uint32_t x, uint32_t y, uint32_t faction_id);
OPENSMACX_API uint32_t __cdecl rocky_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl rocky_set(uint32_t x, uint32_t y, uint8_t rocky);
OPENSMACX_API uint32_t __cdecl bit_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl bit_put(uint32_t x, uint32_t y, uint32_t bit);
OPENSMACX_API void __cdecl bit_set(uint32_t x, uint32_t y, uint32_t bit, BOOL set);
OPENSMACX_API uint32_t __cdecl bit2_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl bit2_set(uint32_t x, uint32_t y, uint32_t bit2, BOOL set);
OPENSMACX_API uint32_t __cdecl code_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl code_set(uint32_t x, uint32_t y, uint32_t code);
OPENSMACX_API void __cdecl synch_bit(uint32_t x, uint32_t y, uint32_t faction_id);
OPENSMACX_API uint32_t __cdecl minerals_at(uint32_t x, uint32_t y);
OPENSMACX_API uint32_t __cdecl bonus_at(uint32_t x, uint32_t y, int unk_val);
OPENSMACX_API uint32_t __cdecl goody_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl site_radius(int x, int y, int unk_val);
OPENSMACX_API int __cdecl find_landmark(int x, int y, uint32_t radius_range_offset);
OPENSMACX_API int __cdecl new_landmark(int x, int y, LPSTR name);
OPENSMACX_API BOOL __cdecl valid_landmark(uint32_t x, uint32_t y, int faction_id);
OPENSMACX_API void __cdecl kill_landmark(int x, int y);
OPENSMACX_API BOOL __cdecl is_coast(int x, int y, BOOL is_base_radius);
OPENSMACX_API BOOL __cdecl is_ocean(uint32_t x, uint32_t y);
OPENSMACX_API int __cdecl veh_who(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl rebuild_vehicle_bits();
OPENSMACX_API void __cdecl rebuild_base_bits();
OPENSMACX_API int __cdecl x_dist(int x_point_a, int x_point_b);
OPENSMACX_API BOOL __cdecl is_known(uint32_t x, uint32_t y, uint32_t faction_id);
OPENSMACX_API int __cdecl base_who(uint32_t x, uint32_t y);
OPENSMACX_API int __cdecl anything_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl map_shutdown();
OPENSMACX_API BOOL __cdecl map_init();
OPENSMACX_API void __cdecl map_wipe();
OPENSMACX_API BOOL __cdecl map_write(FILE *map_file);
OPENSMACX_API BOOL __cdecl map_read(FILE *map_file);
OPENSMACX_API uint8_t __cdecl abstract_at(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl abstract_set(uint32_t x, uint32_t y, uint8_t region);
OPENSMACX_API void __cdecl quick_zoc(uint32_t x_src, uint32_t y_src, uint32_t faction_id, int x_dst,
                                     int y_dst, int *x_zoc, int *y_zoc);
OPENSMACX_API int __cdecl radius_move(int x_radius_off, int y_radius_off, int range);
OPENSMACX_API  int __cdecl radius_move(int x_src, int y_src, int x_dst, int y_dst, int range);
OPENSMACX_API int __cdecl compass_move(int x_src, int y_src, int x_dst, int y_dst);
OPENSMACX_API int __cdecl is_sensor(uint32_t x, uint32_t y);
OPENSMACX_API BOOL __cdecl has_temple(int faction_id);
OPENSMACX_API void __cdecl world_alt_set(int x, int y, uint32_t altitude, BOOL is_set_both);
OPENSMACX_API void __cdecl world_raise_alt(int x, int y);
OPENSMACX_API void __cdecl world_lower_alt(int x, int y);
OPENSMACX_API void __cdecl brush(int x, int y, uint32_t altitude);
OPENSMACX_API void __cdecl paint_land(int x, int y, uint32_t altitude, uint32_t radius);
OPENSMACX_API void __cdecl build_continent(uint32_t size);
OPENSMACX_API void __cdecl build_hills(uint32_t altitude);
OPENSMACX_API void __cdecl world_riverbeds();
OPENSMACX_API BOOL __cdecl world_validate();
OPENSMACX_API void __cdecl world_temperature();
OPENSMACX_API void __cdecl world_analysis();
OPENSMACX_API void __cdecl world_alt_put_detail(uint32_t x, uint32_t y);
OPENSMACX_API void __cdecl world_polar_caps();
OPENSMACX_API void __cdecl world_linearize_contours();
OPENSMACX_API BOOL __cdecl near_landmark(int x, int y);
OPENSMACX_API void __cdecl world_crater(int x, int y);
OPENSMACX_API void __cdecl world_monsoon(int x, int y);
OPENSMACX_API void __cdecl world_sargasso(int x, int y);
OPENSMACX_API void __cdecl world_ruin(int x, int y);
OPENSMACX_API void __cdecl world_dune(int x, int y);
OPENSMACX_API void __cdecl world_diamond(int x, int y);
OPENSMACX_API void __cdecl world_fresh(int x, int y);
OPENSMACX_API void __cdecl world_volcano(int x, int y, BOOL is_not_landmark);
OPENSMACX_API void __cdecl world_borehole(int x, int y);
OPENSMACX_API void __cdecl world_temple(int x, int y);
OPENSMACX_API void __cdecl world_unity(int x, int y);
OPENSMACX_API void __cdecl world_fossil(int x, int y);
OPENSMACX_API void __cdecl world_canyon(int x, int y);
OPENSMACX_API void __cdecl world_mesa(int x, int y);
OPENSMACX_API void __cdecl world_ridge(int x, int y);
OPENSMACX_API void __cdecl world_geothermal(int x, int y);
OPENSMACX_API void __cdecl world_landmarks();
OPENSMACX_API uint32_t __cdecl zoc_any(int x, int y, uint32_t faction_id);
OPENSMACX_API uint32_t __cdecl zoc_veh(int x, int y, uint32_t faction_id);
OPENSMACX_API uint32_t __cdecl zoc_sea(int x, int y, uint32_t faction_id);
OPENSMACX_API uint32_t __cdecl zoc_move(uint32_t x, uint32_t y, uint32_t faction_id);
OPENSMACX_API int __cdecl cursor_dist(int x_point_a, int y_point_a, int x_point_b, int y_point_b);

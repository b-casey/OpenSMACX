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
#pragma once

 /*
  * Map related objects, variables and functions.
  */
enum terrain_altitude_bit {
	ALT_BIT_OCEAN_TRENCH = 0x0,
	ALT_BIT_OCEAN = 0x20,
	ALT_BIT_OCEAN_SHELF = 0x40,
	ALT_BIT_SHORE_LINE = 0x60,
	ALT_BIT_1_LEVEL_ABOVE_SEA = 0x80,
	ALT_BIT_2_LEVELS_ABOVE_SEA = 0xA0,
	ALT_BIT_3_LEVELS_ABOVE_SEA = 0xC0,
};

enum terrain_altitude {
	ALT_OCEAN_TRENCH = 0,
	ALT_OCEAN = 1,
	ALT_OCEAN_SHELF = 2,
	ALT_SHORE_LINE = 3,
	ALT_1_LEVEL_ABOVE_SEA = 4,
	ALT_2_LEVELS_ABOVE_SEA = 5,
	ALT_3_LEVELS_ABOVE_SEA = 6,
};

enum terrain_rainfall_bit { // land modifiers only
	RAINFALL_ARID = 0x0,
	RAINFALL_MOIST = 0x8, // can only have one value set, otherwise tile glitches
	RAINFALL_RAINY = 0x10, // " "
};

enum terrain_rock_bit { // land modifiers only
	TERRAIN_FLAT = 0x0,
	TERRAIN_ROLLING = 0x40, // can only have one value set, otherwise game will crash
	TERRAIN_ROCKY = 0x80, // " "
};

enum terrain_bit : uint32_t {
	BIT_BASE_IN_TILE = 0x1,
	BIT_VEH_IN_TILE = 0x2,
	BIT_ROAD = 0x4,
	BIT_MAGTUBE = 0x8,
	BIT_MINE = 0x10,
	BIT_FUNGUS = 0x20,
	BIT_SOLAR_TIDAL = 0x40,
	BIT_RIVER = 0x80,
	BIT_RIVER_SRC = 0x100, // River begins here for visual effect
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
	BIT_UNK_2000000 = 0x2000000, // ?
	BIT_UNK_4000000 = 0x4000000, // ?
	BIT_UNK_8000000 = 0x8000000, // ?
	BIT_SUPPLY_POD = 0x10000000,
	BIT_NUTRIENT_RSC = 0x20000000,
	BIT_UNK_40000000 = 0x40000000, // ?
	BIT_SENSOR_ARRAY = 0x80000000,
};

enum terrain_landmark_bit2 {
	LM_CRATER = 0x1,
	LM_VOLCANO = 0x2,
	LM_JUNGLE = 0x4,
	LM_URANIUM = 0x8,
	LM_SARGASSO = 0x10,
	LM_RUINS = 0x20,
	LM_DUNES = 0x40,
	LM_FRESH = 0x80,
	LM_MESA = 0x100,
	LM_CANYON = 0x200,
	LM_GEOTHERMAL = 0x400,
	LM_RIDGE = 0x800,
	LM_BOREHOLE = 0x1000,
	LM_NEXUS = 0x2000,
	LM_UNITY = 0x4000,
	LM_FOSSIL = 0x8000,
};

/*
* Region notes:
* a tile's region is visible with debug mode in the bottom left of the main interface tile area
* region id is used to index the [128] planning variable arrays in player_data struct
*
* 0: bad region, n/a
* 1...62: any offshoots from poles starting at 1,1 up to continents and islands
* 63: bad region - both poles land + single tile islands (this seems specific to huge Planet map)
* 64: bad region, n/a
* 65...126: start of water bodies from 1,1 onward (usually primary ocean) to smaller enclosed
* 127: bad region - both poles water
* 128: bad region, n/a
*/
struct map {
	uint8_t climate; // 000 00 000 | altitude (3 bit) ; rainfall (2 bit) ; temperature (3 bit)
	uint8_t contour; // altitude details
	uint8_t val2; // 0000 0000 | site (0xF0) ; owner (0x0F) - last immediate control occupying tile
															// or 0x0F for unoccupied
	uint8_t region; // grouping of disjoint water/land areas; see above notes for more details
	uint8_t visibility; // faction bitfield of those who can see tile (mapped: dim/bright)
	uint8_t val3; // 00 000 000 | rocky (2 bit); lock - factionID (3 bit); using - factionID (3 bit)
	uint8_t unk_1; // flags? bitfield
	int8_t territory; // physical owner factionID of geographic area or -1 for unclaimed
	uint32_t bit; // see terrain_bit
	uint32_t bit2; // bitwho[0]? FF FF FFFF | code (landmark tile sequence); unk flags; landmark id
	uint32_t bitVisible[7]; // what each faction sees tile as (ex. pods another faction already got)
};

struct landmark {
	int xCoord;
	int yCoord;
	char name[32];
};

struct continent {
	uint32_t tiles; // count of tiles in region
	uint32_t openTerrain; // count of non-rocky, non-fungus tiles (only 1 movement point to travel)
	uint32_t unk_3; // highest world_site value (0-15)
	uint32_t pods; // current count of supply and unity pods in region
	uint32_t unk_5; // padding?
	uint8_t seaCoasts[8]; // sea specific regions, connections to land regions? bitmask
};

struct rules_natural {
	LPSTR name;
	LPSTR nameShort;
};

constexpr int MaxNaturalNum = 16;
constexpr int MaxLandmarkNum = 64;
constexpr int RegionBounds = 63;
constexpr int MaxContinentNum = 128;
constexpr int MaxRegionLandNum = 64;

const uint32_t RadiusRange[] = { 1, 9, 25, 49, 81, 121, 169, 225, 289 };
const int xRadiusBase[] = {  1, 2, 1, 0, -1, -2, -1,  0, 0 };
const int yRadiusBase[] = { -1, 0, 1, 2,  1,  0, -1, -2, 0 };
const int xRadiusOffset[] = {
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
const int yRadiusOffset[] = {
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

extern uint32_t *MapHorizontalBounds; // x
extern uint32_t *MapVerticalBounds; // y
extern uint32_t *MapRandSeed;
extern int *MapSeaLevel;
extern int *MapSeaLevelCouncil; // Solar Shade (-3); Melt Polar Caps (+3)
extern uint32_t *MapArea;
extern uint32_t *MapAreaSqRoot;
extern BOOL *MapIsFlat;
extern uint32_t *MapLandmarkCount;
extern landmark *MapLandmark;
extern uint32_t *MapAbstractHorizBounds;
extern uint32_t *MapAbstractVertBounds;
extern uint32_t *MapAbstractArea;
extern uint32_t *MapSizePlanet;
extern uint32_t *MapOceanCoverage;
extern uint32_t *MapLandCoverage; // 2 - MapOceanCoverage
extern uint32_t *MapErosiveForces;
extern uint32_t *MapPlanetaryOrbit; // affects temp
extern uint32_t *MapCloudCover; // affects rainfall, rivers
extern uint32_t *MapNativeLifeForms;
extern LPSTR *MapFilePath;
extern map **Map;
extern uint8_t **MapAbstract;

extern continent *Continents;
extern rules_natural *Natural;
extern uint32_t *MapHorizontal;
extern uint32_t *AltNatural; // Default: { 0, 15, 32, 45, 60,  75,  80, 100, 100, 100, 100 };

OPENSMACX_API BOOL __cdecl on_map(int xCoord, int yCoord);
OPENSMACX_API int __cdecl xrange(int xCoord);
OPENSMACX_API int __cdecl whose_territory(int factionID, int xCoord, int yCoord,
	int *baseID, BOOL ignoreComm);
OPENSMACX_API int __cdecl base_territory(int factionID, int xCoord, int yCoord);
OPENSMACX_API uint32_t __cdecl crappy(int xCoord, int yCoord);
OPENSMACX_API int __cdecl vector_dist(int xDistance, int yDistance);
OPENSMACX_API int __cdecl vector_dist(int xCoordA, int yCoordA, int xCoordB, int yCoordB);
OPENSMACX_API BOOL __cdecl sea_coast(uint32_t regionDst, uint32_t regionSrc);
OPENSMACX_API uint32_t __cdecl sea_coasts(uint32_t regionSrc);
OPENSMACX_API BOOL __cdecl base_on_sea(uint32_t baseID, uint32_t regionSea);
OPENSMACX_API int __cdecl base_coast(uint32_t baseID);
OPENSMACX_API BOOL __cdecl port_to_coast(uint32_t baseID, uint32_t region);
OPENSMACX_API BOOL __cdecl port_to_port(uint32_t baseIDSrc, uint32_t baseIDDst);
OPENSMACX_API BOOL __cdecl transport_base(uint32_t baseID);
OPENSMACX_API BOOL __cdecl naval_base(uint32_t baseID);
OPENSMACX_API BOOL __cdecl convoy(uint32_t vehID, uint32_t baseID);
OPENSMACX_API BOOL __cdecl bad_reg(int region);
OPENSMACX_API BOOL __cdecl get_there(uint32_t vehID, int xCoordDst, int yCoordDst);
OPENSMACX_API BOOL __cdecl coast_or_border(int xCoordPtA, int yCoordPtA,
	int xCoordPtB, int yCoordPtB, int factionID);
OPENSMACX_API map *__cdecl map_loc(int xCoord, int yCoord);
OPENSMACX_API uint32_t __cdecl temp_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl temp_set(int xCoord, int yCoord, uint8_t temperature);
OPENSMACX_API void __cdecl climate_set(int xCoord, int yCoord, uint8_t climate);
OPENSMACX_API int __cdecl elev_at(int xCoord, int yCoord);
OPENSMACX_API uint32_t __cdecl alt_natural(int xCoord, int yCoord);
OPENSMACX_API uint32_t __cdecl alt_at(int xCoord, int yCoord);
OPENSMACX_API uint32_t __cdecl altitude_at(int xCoord, int yCoord);
OPENSMACX_API uint32_t __cdecl alt_detail_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl alt_put_detail(int xCoord, int yCoord, uint8_t detail);
OPENSMACX_API uint32_t __cdecl owner_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl owner_set(int xCoord, int yCoord, int factionID);
OPENSMACX_API void __cdecl site_set(int xCoord, int yCoord, int site);
OPENSMACX_API uint32_t __cdecl region_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl region_set(int xCoord, int yCoord, uint8_t region);
OPENSMACX_API uint32_t __cdecl using_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl using_set(int xCoord, int yCoord, int factionID);
OPENSMACX_API uint32_t __cdecl lock_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl lock_set(int xCoord, int yCoord, uint32_t factionID);
OPENSMACX_API BOOL __cdecl lock_map(int xCoord, int yCoord, uint32_t factionID);
OPENSMACX_API void __cdecl unlock_map(int xCoord, int yCoord, uint32_t factionID);
OPENSMACX_API uint32_t __cdecl rocky_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl rocky_set(int xCoord, int yCoord, uint8_t rocky);
OPENSMACX_API uint32_t __cdecl bit_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl bit_put(int xCoord, int yCoord, uint32_t bit);
OPENSMACX_API void __cdecl bit_set(int xCoord, int yCoord, uint32_t bit, BOOL set);
OPENSMACX_API uint32_t __cdecl bit2_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl bit2_set(int xCoord, int yCoord, uint32_t bit2, BOOL set);
OPENSMACX_API uint32_t __cdecl code_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl code_set(int xCoord, int yCoord, uint32_t code);
OPENSMACX_API void __cdecl synch_bit(int xCoord, int yCoord, int factionID);
OPENSMACX_API uint32_t __cdecl minerals_at(int xCoord, int yCoord);
OPENSMACX_API uint32_t __cdecl bonus_at(int xCoord, int yCoord, int unkVal);
OPENSMACX_API uint32_t __cdecl goody_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl site_radius(int xCoord, int yCoord, int valUnk);
OPENSMACX_API BOOL __cdecl is_coast(int xCoord, int yCoord, BOOL isBaseRadius);
OPENSMACX_API BOOL __cdecl is_ocean(int xCoord, int yCoord);
OPENSMACX_API int __cdecl veh_who(int xCoord, int yCoord);
OPENSMACX_API void __cdecl rebuild_vehicle_bits();
OPENSMACX_API void __cdecl rebuild_base_bits();
OPENSMACX_API int __cdecl x_dist(int xCoordA, int xCoordB);
OPENSMACX_API BOOL __cdecl is_known(uint32_t xCoord, uint32_t yCoord, uint32_t factionID);
OPENSMACX_API int __cdecl base_who(uint32_t xCoord, uint32_t yCoord);
OPENSMACX_API int __cdecl anything_at(uint32_t xCoord, uint32_t yCoord);
OPENSMACX_API void __cdecl map_shutdown();
OPENSMACX_API BOOL __cdecl map_init();
OPENSMACX_API void __cdecl map_wipe();
OPENSMACX_API BOOL __cdecl map_write(FILE *mapFile);
OPENSMACX_API BOOL __cdecl map_read(FILE *mapFile);
OPENSMACX_API uint8_t __cdecl abstract_at(int xCoord, int yCoord);
OPENSMACX_API void __cdecl abstract_set(int xCoord, int yCoord, uint8_t region);
OPENSMACX_API void __cdecl quick_zoc(int xCoordSrc, int yCoordSrc, int factionID, int xCoordDst,
	int yCoordDst, int *xCoordZoc, int *yCoordZoc);
OPENSMACX_API int __cdecl radius_move(int xRadiusOff, int yRadiusOff, int range);
OPENSMACX_API  int __cdecl radius_move(int xCoordSrc, int yCoordSrc, int xCoordDst, int yCoordDst,
	int range);
OPENSMACX_API int __cdecl compass_move(int xCoordSrc, int yCoordSrc, int xCoordDst, int yCoordDst);
OPENSMACX_API int __cdecl is_sensor(int xCoord, int yCoord);
OPENSMACX_API BOOL __cdecl has_temple(int factionID);
OPENSMACX_API uint32_t __cdecl zoc_any(int xCoord, int yCoord, uint32_t factionID);
OPENSMACX_API uint32_t __cdecl zoc_veh(int xCoord, int yCoord, uint32_t factionID);
OPENSMACX_API uint32_t __cdecl zoc_sea(int xCoord, int yCoord, uint32_t factionID);
OPENSMACX_API uint32_t __cdecl zoc_move(int xCoord, int yCoord, uint32_t factionID);
OPENSMACX_API int __cdecl cursor_dist(int xCoordA, int yCoordA, int xCoordB, int yCoordB);

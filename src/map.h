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
#pragma once

 /*
  * Map related objects, variables and functions.
  */

struct map {
	BYTE val1; // 000 00 000 | altitude (3 bit) ; climate (2 bit) ; temperature (3 bit)
	BYTE altDetail;
	BYTE val2; // 0000 0000 | site (0xF0) ; owner (0x0F)
	BYTE region;
	BYTE visibility; // (1 << factionID)
	BYTE val3; // 00 000 000 | rocky (2 bits); lock - factionID (3 bits); using - factionID (3 bits)
	BYTE unk1; // sometimes set to 0x80, seems to be unused
	CHAR territory; // factionID or -1 for unclaimed
	DWORD bit; // see terrain_bit enum
	DWORD bit2; // // FF FF FFFF | code (landmark tile sequence); unk flags; landmark id (2 bytes)
	DWORD bitVisible[7];
};

struct landmark {
	int xCoord;
	int yCoord;
	char name[32];
};

struct rules_natural {
	LPSTR name;
	LPSTR nameShort;
};

enum terrain_altitude {
	ALT_OCEAN_TRENCH = 0x0,
	ALT_OCEAN = 0x20,
	ALT_OCEAN_SHELF = 0x40,
	ALT_SHORE_LINE = 0x60,
	ALT_1_LEVEL_ABOVE_SEA = 0x80,
	ALT_2_LEVELS_ABOVE_SEA = 0xA0,
	ALT_3_LEVELS_ABOVE_SEA = 0xC0,
	ALT_4_LEVELS_ABOVE_SEA = 0xE0,
};

enum terrain_climate { // land modifiers only
	CLIMATE_ARID = 0x0,
	CLIMATE_MOIST = 0x8, // can only have one value set, otherwise tile glitches
	CLIMATE_RAINY = 0x10, // " "
};

enum terrain_rock { // land modifiers only
	TERRAIN_FLAT = 0x0,
	TERRAIN_ROLLING = 0x40, // can only have one value set, otherwise game will crash
	TERRAIN_ROCKY = 0x80, // " "
};

enum terrain_bit {
	BIT_BASE_IN_TILE  = 0x1,
	BIT_VEH_IN_TILE   = 0x2,
	BIT_ROAD          = 0x4,
	BIT_MAGTUBE       = 0x8,
	BIT_MINE          = 0x10,
	BIT_FUNGUS        = 0x20,
	BIT_SOLAR_TIDAL   = 0x40,
	BIT_RIVER         = 0x80,
	BIT_RIVER_SRC     = 0x100, // ?
	//                  0x200,
	BIT_RSC_BONUS     = 0x400, // Mineral, Nutrient, Energy
	BIT_BUNKER        = 0x800,
	//                = 0x1000, // base radius?
	BIT_MONOLITH      = 0x2000,
	//                = 0x4000,
	BIT_FARM          = 0x8000,
	BIT_ENERGY_RSC    = 0x10000,
	BIT_MINERAL_RSC   = 0x20000,
	BIT_AIRBASE       = 0x40000,
	BIT_SOIL_ENRICHER = 0x80000,
	BIT_SUPPLY_REMOVE = 0x100000, // ?
	BIT_FOREST        = 0x200000,
	BIT_CONDENSER     = 0x400000,
	BIT_ECH_MIRROR    = 0x800000,
	BIT_THERMAL_BORE  = 0x1000000,
	//                  0x2000000,
	//                  0x4000000,
	//                  0x8000000,
	BIT_SUPPLY_POD    = 0x10000000,
	BIT_NUTRIENT_RSC  = 0x20000000,
	//                  0x40000000
	BIT_SENSOR_ARRAY  = 0x80000000,
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

constexpr int MaxNaturalNum = 16;
constexpr int MaxLandmarkNum = 64;

extern rules_natural *Natural;
extern DWORD *MapVerticalBounds;
extern DWORD *MapHorizontalBounds;
extern int *MapSeaLevel;
extern DWORD *MapArea;
extern DWORD *MapAreaSqRoot;
extern BOOL *MapFlatToggle;
extern DWORD *MapLandmarkCount;
extern landmark *MapLandmark;
extern DWORD *MapHorizontal;
extern map **Map;

map * __cdecl map_loc(int xCoord, int yCoord);
DWORD __cdecl temp_at(int xCoord, int yCoord);
void __cdecl temp_set(int xCoord, int yCoord, BYTE temperature);
DWORD __cdecl climate_at(int xCoord, int yCoord);
void __cdecl climate_set(int xCoord, int yCoord, BYTE climate);
DWORD __cdecl alt_at(int xCoord, int yCoord);
DWORD __cdecl alt_detail_at(int xCoord, int yCoord);
void __cdecl alt_put_detail(int xCoord, int yCoord, BYTE detail);
DWORD __cdecl owner_at(int xCoord, int yCoord);
void __cdecl owner_set(int xCoord, int yCoord, int factionID);
void __cdecl site_set(int xCoord, int yCoord, int site);
DWORD __cdecl region_at(int xCoord, int yCoord);
void __cdecl region_set(int xCoord, int yCoord, BYTE region);
DWORD __cdecl using_at(int xCoord, int yCoord);
void __cdecl using_set(int xCoord, int yCoord, int factionID);
DWORD __cdecl lock_at(int xCoord, int yCoord);
void __cdecl lock_set(int xCoord, int yCoord, int factionID);
BOOL __cdecl lock_map(int xCoord, int yCoord, int factionID);
void __cdecl unlock_map(int xCoord, int yCoord, int factionID);
DWORD __cdecl rocky_at(int xCoord, int yCoord);
void __cdecl rocky_set(int xCoord, int yCoord, BYTE rocky);
DWORD __cdecl bit_at(int xCoord, int yCoord);
void __cdecl bit_put(int xCoord, int yCoord, DWORD bit);
void __cdecl bit_set(int xCoord, int yCoord, DWORD bit, BOOL set);
DWORD __cdecl bit2_at(int xCoord, int yCoord);
void __cdecl bit2_set(int xCoord, int yCoord, DWORD bit2, BOOL set);
DWORD __cdecl code_at(int xCoord, int yCoord);
void __cdecl code_set(int xCoord, int yCoord, DWORD code);
void __cdecl synch_bit(int xCoord, int yCoord, int factionID);
BOOL __cdecl is_ocean(int xCoord, int yCoord);
int __cdecl veh_who(int xCoord, int yCoord);
int __cdecl anything_at(int xCoord, int yCoord);
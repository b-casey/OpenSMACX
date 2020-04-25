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
  * Terraforming related objects, variables and functions.
  */
struct rules_terraforming {
	LPSTR name;
	LPSTR nameSea;
	int preqTech;
	int preqTechSea;
	uint32_t flag;
	uint32_t flagSea;
	int rate;
	LPSTR shortcuts;
};

const uint32_t flagsTerraforming[] = { // land, sea
	    0x8000,   0x200000, // farm
	   0x80000,   0x200000, // soil enricher
	      0x10, 0x81E00050, // mine
	      0x40, 0x81E00050, // solar collector / tidal harness
	  0x200000,  0x1E88070, // forest
	         4,          0, // road
	         8,          0, // magtube
	     0x800,    0x40000, // bunker
	   0x40000,      0x800, // airbase
	0x80000000,  0x1C00050, // sensor
	         0,       0x20, // fungus remove
	      0x20,   0x288050, // fungus plant
	  0x400000, 0x81E00050, // condenser
	  0x800000, 0x81E00050, // echelon mirror
	 0x1000000, 0x81E08050, // thermal borehole
	         0,          0, // aquifer
	         0,          0, // raise land
	         0,          0, // lower land 
           	 0,          0, // level terrain
	    0x2000, 0x30000400, // monolith
};

enum terraform_id {
	TERRA_FARM = 0x0,
	TERRA_SOIL_ENRICHER = 0x1,
	TERRA_MINE = 0x2,
	TERRA_SOLAR_COLLECTOR_TIDAL = 0x3,
	TERRA_FOREST = 0x4,
	TERRA_ROAD = 0x5,
	TERRA_MAGTUBE = 0x6,
	TERRA_BUNKER = 0x7,
	TERRA_AIRBASE = 0x8,
	TERRA_SENSOR = 0x9,
	TERRA_FUNGUS_REMOVE = 0xA,
	TERRA_FUNGUS_PLANT = 0xB,
	TERRA_CONDENSER = 0xC,
	TERRA_ECHELON_MIRROR = 0xD,
	TERRA_THERMAL_BOREHOLE = 0xE,
	TERRA_AQUIFER = 0xF,
	TERRA_RAISE_LAND = 0x10,
	TERRA_LOWER_LAND = 0x11,
	TERRA_LEVEL_TERRAIN = 0x12,
	TERRA_MONOLITH = 0x13,
};

constexpr int MaxTerrainNum = 20;

extern rules_terraforming *Terraforming;

OPENSMACX_API BOOL __cdecl terrain_avail(int terraformID, BOOL isSea, int factionID);
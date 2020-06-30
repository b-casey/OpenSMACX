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
#include "map.h"

 /*
  * Terraforming related objects, variables and functions.
  */
enum terraform {
	TERRA_FARM = 0, // Farm + Kelp Farm
	TERRA_SOIL_ENR = 1, // Soil Enricher
	TERRA_MINE = 2, // Mine + Mining Platform
	TERRA_SOLAR_TIDAL = 3, // Solar Collector + Tidal Harness
	TERRA_FOREST = 4,
	TERRA_ROAD = 5,
	TERRA_MAGTUBE = 6,
	TERRA_BUNKER = 7,
	TERRA_AIRBASE = 8,
	TERRA_SENSOR = 9,
	TERRA_REMOVE_FUNGUS = 10, // Remove Fungus + Sea Fungus
	TERRA_PLANT_FUNGUS = 11, // Plant Fungus + Sea Fungus
	TERRA_CONDENSER = 12,
	TERRA_ECH_MIRROR = 13, // Echelon Mirror
	TERRA_THERMAL_BORE = 14, // Thermal Borehole
	TERRA_AQUIFER = 15,
	TERRA_RAISE_LAND = 16, // Raise Land + Raise Sea Floor
	TERRA_LOWER_LAND = 17, // Lower Land + Lower Sea Floor
	TERRA_LEVEL_TERRAIN = 18,
	TERRA_MONOLITH = 19,
};

struct rules_terraforming {
	LPSTR name;
	LPSTR nameSea;
	int preqTech;
	int preqTechSea;
	uint32_t bit;
	uint32_t bitIncompatible;
	int rate;
	LPSTR shortcuts;
};

const uint32_t terraformingBits[20][2] = { // terrain enhancement, incompatible enhancements on same tile
	BIT_FARM,          BIT_FOREST, // farm
	BIT_SOIL_ENRICHER, BIT_FOREST, // soil enricher
	BIT_MINE,          BIT_MINE | BIT_SOLAR_TIDAL | BIT_FOREST | BIT_CONDENSER | BIT_ECH_MIRROR
					   | BIT_THERMAL_BORE | BIT_SENSOR_ARRAY, // mine
	BIT_SOLAR_TIDAL,   BIT_MINE | BIT_SOLAR_TIDAL | BIT_FOREST | BIT_CONDENSER | BIT_ECH_MIRROR
					   | BIT_THERMAL_BORE | BIT_SENSOR_ARRAY, // solar collector / tidal harness
	BIT_FOREST,        BIT_MINE | BIT_FUNGUS | BIT_SOLAR_TIDAL | BIT_FARM | BIT_SOIL_ENRICHER
					   | BIT_FOREST | BIT_CONDENSER | BIT_ECH_MIRROR | BIT_THERMAL_BORE, // forest
	BIT_ROAD,          0, // road
	BIT_MAGTUBE,       0, // magtube
	BIT_BUNKER,        BIT_AIRBASE, // bunker
	BIT_AIRBASE,       BIT_BUNKER, // airbase
	BIT_SENSOR_ARRAY,  BIT_MINE | BIT_SOLAR_TIDAL | BIT_CONDENSER | BIT_ECH_MIRROR
					   | BIT_THERMAL_BORE, // sensor
	0,                 BIT_FUNGUS, // fungus remove
	BIT_FUNGUS,        BIT_MINE | BIT_SOLAR_TIDAL | BIT_FARM | BIT_SOIL_ENRICHER
					   | BIT_FOREST, // fungus plant
	BIT_CONDENSER,     BIT_MINE | BIT_SOLAR_TIDAL | BIT_FOREST | BIT_CONDENSER | BIT_ECH_MIRROR
					   | BIT_THERMAL_BORE | BIT_SENSOR_ARRAY, // condenser
	BIT_ECH_MIRROR,    BIT_MINE | BIT_SOLAR_TIDAL | BIT_FOREST | BIT_CONDENSER | BIT_ECH_MIRROR
					   | BIT_THERMAL_BORE | BIT_SENSOR_ARRAY, // echelon mirror
	BIT_THERMAL_BORE,  BIT_MINE | BIT_SOLAR_TIDAL | BIT_FARM | BIT_FOREST | BIT_CONDENSER
					   | BIT_ECH_MIRROR | BIT_THERMAL_BORE | BIT_SENSOR_ARRAY, // thermal borehole
	0,                 0, // aquifer
	0,                 0, // raise land
	0,                 0, // lower land
	0,                 0, // level terrain
	BIT_MONOLITH,      BIT_SUPPLY_POD | BIT_NUTRIENT_RSC | BIT_RSC_BONUS, // monolith
};

constexpr int MaxTerrainNum = 20;

extern rules_terraforming *Terraforming;

OPENSMACX_API uint32_t __cdecl terraform_cost(int xCoord, int yCoord, uint32_t factionID);
OPENSMACX_API uint32_t __cdecl contribution(int vehID, uint32_t terraformID);
OPENSMACX_API BOOL __cdecl terrain_avail(int terraformID, BOOL isSea, int factionID);

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
#include "faction.h"

 /*
  * Parsing alpha/x.txt
  */
struct label {
	LPSTR stringsPtr;
	int count;
};

enum rules_resourceinfo_id {
	RSCINFO_OCEAN_SQ = 0,
	RSCINFO_BASE_SQ = 1,
	RSCINFO_BONUS_SQ = 2,
	RSCINFO_FOREST_SQ = 3,
	RSCINFO_RECYCLING_TANKS = 4,
	RSCINFO_IMPROVED_LAND = 5,
	RSCINFO_IMPROVED_SEA = 6,
	RSCINFO_MONOLITH = 7,
	RSCINFO_BOREHOLE_SQ = 8,
};

struct rules_resourceinfo {
	int nutrients;
	int minerals;
	int energy;
	int unused;
};

struct rules_time_control {
	LPSTR name;
	int turn;
	int base;
	int unit;
	int event;
	int extra;
	int refresh;
	int accumulated;
};

struct rules_resource {
	LPSTR nameSingular;
	LPSTR namePlural;
};

struct rules_energy {
	LPSTR abbrev;
	LPSTR name;
};

struct rules_basic {
	// Alpha(x).txt #RULES: Movement rate along roads
	// Original bounds: 1 to 100
	// Default value: 3
	int MoveRateRoads;
	
	// Alpha(x).txt #RULES: Nutrient intake requirement for citizens
	// Original bounds: 0 to 100
	// Default value: 2
	int NutrientReqCitizen;
	
	// Alpha(x).txt #RULES: Max airdrop range w/o orbital insertion
	// Original bounds: 1 to 500
	// Default value: 8
	int MaxAirdropSansOrbInsert;
	
	// Alpha(x).txt #RULES: Max artillery range (larger will break multiplayer)
	// Original bounds: 1 to 8
	// Default value: 2
	int ArtilleryMaxRng;
	
	// Alpha(x).txt #RULES: Numerator for artillery fire damage
	// Original bounds: 1 to 1000
	// Default value: 3
	int ArtilleryDmgNum;
	
	// Alpha(x).txt #RULES: Denominator for artillery fire damage
	// Original bounds: 1 to 1000
	// Default value: 2
	int ArtilleryDmgDenom;
	
	// Alpha(x).txt #RULES: Nutrient cost multiplier
	// Original bounds: 1 to 100
	// Default value: 10
	int NutrientCostMulti;

	// Alpha(x).txt #RULES: Minerals cost multiplier
	// Original bounds: 1 to 100
	// Default value: 10
	int MineralCostMulti;

	// Alpha(x).txt #RULES: Technology discovery rate as a percentage of standard
	// Original bounds: 0 to 1000
	// Default value: 100
	int TechDiscovRatePctStd;

	// Alpha(x).txt #RULES: Limits mineral increase for mine without road in square
	// Original bounds: 0 to 100
	// Default value: 1
	int LimitMineralMineSansRoad;

	// Alpha(x).txt #RULES: Nutrient effect in mine square (0 or -1)
	// Original bounds: 0 or -1
	// Default value: -1
	int TglNutrientEffectWithMine;

	// Alpha(x).txt #RULES: Minimum base size to support specialists
	// Original bounds: 0 to 100
	// Default value: 5
	int MinBaseSizeSpecialists;

	// Alpha(x).txt #RULES: Drones induced by Genejack factory
	// Original bounds: 0 to 100
	// Default value: 1
	int DronesGenejackFactory;

	// Alpha(x).txt #RULES: Population limit w/o hab complex
	// Original bounds: 1 to 100
	// Default value: 7
	int PopLimitSansHabComplex;

	// Alpha(x).txt #RULES: Population limit w/o hab dome
	// Original bounds: 1 to 100
	// Default value: 14
	int PopLimitSansHabDome;

	// Alpha(x).txt #RULES: Technology to improve fungus squares
	// Default value: CentPsi (63)
	int TechImproveFungusSqr;

	// Alpha(x).txt #RULES: Technology to ease fungus movement
	// Default value: CentPsi (63)
	int TechEaseFungusMovement;

	// Alpha(x).txt #RULES: Technology to build roads in fungus
	// Default value: CentEmp (58)
	int TechBuildRoadsFungus;

	// Alpha(x).txt #RULES: Technology to allow 2 special abilities for a unit
	// Default value: Neural (52)
	int TechTwoSpecAbilities;

	// Alpha(x).txt #RULES: Technology to allow 3 nutrients in a square
	// Default value: Gene (49)
	int TechThreeNutrientsSqr;

	// Alpha(x).txt #RULES: Technology to allow 3 minerals in a square
	// Default value: EcoEng (60)
	int TechThreeMineralsSqr;

	// Alpha(x).txt #RULES: Technology to allow 3 energy in a square
	// Default value: EnvEcon (59)
	int TechThreeEnergySqr;
	
	// Alpha(x).txt #RULES: Extra percentage cost of prototype LAND unit
	// Original bounds: 0 to 500
	// Default value: 50
	int ExtraPctCostProtoLand;

	// Alpha(x).txt #RULES: Extra percentage cost of prototype SEA unit
	// Original bounds: 0 to 500
	// Default value: 50
	int ExtraPctCostProtoSea;

	// Alpha(x).txt #RULES: Extra percentage cost of prototype AIR unit
	// Original bounds: 0 to 500
	// Default value: 50
	int ExtraPctCostProtoAir;

	// Alpha(x).txt #RULES: Psi combat offense-to-defense ratio (unit defending) Numerator
	// Original bounds: 1 to 1000
	// Default value: 3 (LAND)
	// Default value: 1 (SEA/AIR)
	int PsiCombatRatioAtk[3]; // LAND, SEA, AIR
	
	// Alpha(x).txt #RULES: Psi combat offense-to-defense ratio (unit defending) Denominator
	// Original bounds: 1 to 1000
	// Default value: 2 (LAND)
	// Default value: 1 (SEA/AIR)
	int PsiCombatRatioDef[3]; // LAND, SEA, AIR

	// Alpha(x).txt #RULES: Players' starting energy reserves
	// Original bounds: 0 to 1000
	// Default value: 10
	int PlayerStartEnergyReserve;

	// Alpha(x).txt #RULES: Combat % -> intrinsic base defense
	// Original bounds: -100 to 1000
	// Default value: 25
	int CombatPctBaseDef;

	// Alpha(x).txt #RULES: Combat % -> attacking along road
	// Original bounds: -100 to 1000
	// Default value: 0
	int CombatPctAtkRoad;

	// Alpha(x).txt #RULES: Combat % -> for attacking from higher elevation
	// Original bounds: -100 to 1000
	// Default value: 0
	int CombatPctAtkHigherElev;

	// Alpha(x).txt #RULES: Combat penalty % -> attacking from lower elevation
	// Original bounds: -100 to 1000
	// Default value: 0
	int CombatPenPctAtkLwrElev;
	
	// Alpha(x).txt #RULES: Technology to allow orbital insertion w/o Space Elevator
	// Default value: Gravity (21)
	int TechOrbInsertSansSpcElev;
	
	// Alpha(x).txt #RULES: Minimum # of turns between councils
	// Original bounds: 0 to 1000
	// Default value: 20
	int MinTurnsCouncils;

	// Alpha(x).txt #RULES: Minerals for harvesting forest
	// Original bounds: 0 to 100
	// Default value: 5
	int MineralsHarvestingForest;

	// Alpha(x).txt #RULES: Territory: max distance from base
	// Original bounds: 0 to 100
	// Default value: 8
	int TerritoryMaxDistBase;

	// Alpha(x).txt #RULES: Turns to corner Global Energy Market
	// Original bounds: 1 to 100
	// Default value: 20
	int TurnsCornerGblEnergyMrkt;
	
	// Alpha(x).txt #RULES: Technology for +1 mining platform bonus
	// Default value: EcoEng2 (62)
	int TechMiningPlatformBonus;

	// Alpha(x).txt #RULES: Technology for economic victory
	// Default value: PlaEcon (61)
	int TechEconomicVictory;
	
	// Alpha(x).txt #RULES: Combat penalty % -> attack after airdrop
	// Original bounds: -100 to 1000
	// Default value: 50
	int CombatPenPctAtkAirdrop;

	// Alpha(x).txt #RULES: Combat % -> Fanatic attack bonus
	// Original bounds: -100 to 1000
	// Default value: 25
	int CombatPctFanaticAtkBonus;

	// Alpha(x).txt #RULES: Combat % -> Land based guns vs. ship artillery bonus
	// Original bounds: -100 to 1000
	// Default value: 50
	int CombatPctLandGunVsShipArt;
	
	// Alpha(x).txt #RULES: Combat % -> Artillery bonus per level of altitude
	// Original bounds: -100 to 1000
	// Default value: 25
	int CombatPctArtBonusLvlAlt;
	
	// Alpha(x).txt #RULES: Combat % -> Mobile unit in open ground
	// Original bounds: -100 to 1000
	// Default value: 25
	int CombatPctMobileOpenGround;

	// Alpha(x).txt #RULES: Combat % -> Defend vs. mobile in rough
	// Original bounds: -100 to 1000
	// Default value: 0
	int CombatPctDefVsMobileRough;
	
	// Alpha(x).txt #RULES: Combat % -> Trance bonus defending vs. psi
	// Original bounds: -100 to 1000
	// Default value: 50
	int CombatPctTranceDefVsPsi;

	// Alpha(x).txt #RULES: Combat % -> Empath Song bonus attacking vs. psi
	// Original bounds: -100 to 1000
	// Default value: 50
	int CombatPctEmpSongAtkVsPsi;

	// Alpha(x).txt #RULES: Combat % -> Infantry vs. Base
	// Original bounds: -100 to 1000
	// Default value: 25
	int CombatPctInfantryVsBase;
	
	// Alpha(x).txt #RULES: Combat penalty % -> Air superiority unit vs. ground unit
	// Original bounds: -100 to 1000
	// Default value: 50
	int CombatPenPctAirSuprVsGrnd;

	// Alpha(x).txt #RULES: Combat % -> Air superiority unit vs. air unit
	// Original bounds: -100 to 1000
	// Default value: 100
	int CombatPctAirSuprVsAir;

	// Alpha(x).txt #RULES: Combat penalty % -> Non-combat unit defending vs. combat unit
	// Original bounds: -100 to 1000
	// Default value: 50
	int CombatPenPctNonCbtDefVsCbt;

	// Alpha(x).txt #RULES: Combat % -> Comm Jammer unit defending vs. mobile unit
	// Original bounds: -100 to 1000
	// Default value: 50
	int CombatPctComJamDefVsMobl;

	// Alpha(x).txt #RULES: Combat % -> Bonus vs. ships caught in port
	// Original bounds: -100 to 1000
	// Default value: 100
	int CombatPctBonusVsShipPort;

	// Alpha(x).txt #RULES: Combat % -> AAA bonus vs. air units
	// Original bounds: -100 to 1000
	// Default value: 100
	int CombatPctAAABonusVsAir;

	// Alpha(x).txt #RULES: Combat % -> Defend in range of friendly Sensor
	// Original bounds: -100 to 1000
	// Default value: 25
	int CombatPctDefRangeSensor;

	// Alpha(x).txt #RULES: Combat % -> Psi attack bonus/penalty per +PLANET
	// Original bounds: -100 to 1000
	// Default value: 10
	int CombatPctPsiAtkBonusPLANET;
	
	// Alpha(x).txt #RULES: Retool strictness 
	// (0 = Always Free, 1 = Free in Category, 2 = Free if Project, 3 = Never Free)
	// Original bounds: 0 to 2 (Bug Fix: Should be 0 to 3)
	// Default value: 2
	int RetoolStrictness;

	// Alpha(x).txt #RULES: Retool percent penalty for production change
	// Original bounds: 0 to 100
	// Default value: 50
	int RetoolPctPenProdChg;

	// Alpha(x).txt #RULES: Retool exemption (first X minerals not affected by penalty)
	// Original bounds: 0 to 1000
	// Default value: 10
	int RetoolExemption;

	// Alpha(x).txt #RULES: If non-zero, probe teams can steal technologies
	// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
	// Default value: 1
	int TglProbeCanStealTech;

	// Alpha(x).txt #RULES: If non-zero, humans can always contact each other in net games
	// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
	// Default value: 1
	int TglHumanAlwaysContactNet;

	// Alpha(x).txt #RULES: If non-zero, humans can always contact each other in hotseat/email games
	// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
	// Default value: 1
	int TglHumansAlwaysContactPbem;

	// Alpha(x).txt #RULES: Maximum % damage inflicted by arty versus units in base/bunker
	// Original bounds: 10 to 100
	// Default value: 50
	int MaxPctDmgArtVsUnitBaseBnkr;

	// Alpha(x).txt #RULES: Maximum % damage inflicted by arty versus units in open
	// Original bounds: 10 to 100
	// Default value: 99
	int MaxPctDmgArtVsUnitsOpen;

	// Alpha(x).txt #RULES: Maximum % damage inflicted by arty versus units in sea
	// Original bounds: 10 to 100
	// Default value: 100
	int MaxPctDmgArtVsUnitsSea;

	// Alpha(x).txt #RULES: Numerator for freq of global warming (1,2 would be "half" normal warming).
	// Original bounds: 0 to 1000
	// Default value: 1
	int FreqGlobalWarmingNum;

	// Alpha(x).txt #RULES: Denominator for freq of global warming (1,2 would be "half" normal warming).
	// Original bounds: 1 to 1000
	// Default value: 1
	int FreqGlobalWarmingDenom;

	// Alpha(x).txt #RULES: Normal starting year
	// Original bounds: 0 to 999999
	// Default value: 2100
	int NormalStartingYear;

	// Alpha(x).txt #RULES: Normal ending year for lowest 3 difficulty levels
	// Original bounds: 0 to 999999
	// Default value: 2600
	int NormalEndYearLowThreeDiff;

	// Alpha(x).txt #RULES: Normal ending year for highest 3 difficulty levels
	// Original bounds: 0 to 999999
	// Default value: 2500
	int NormalEndYearHighThreeDiff;

	// Alpha(x).txt #RULES: If non-zero, obliterating a base counts as an atrocity
	// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
	// Default value: 1
	int TglOblitBaseAtrocity;

	// Alphax.txt #RULES: Size of base for subspace generator
	// Original bounds: 1 to 999
	// Default value: 10
	int SizeBaseSubspaceGen;

	// Alphax.txt #RULES: Number of subspace generators needed
	// Original bounds: 1 to 999
	// Default value: 6
	int SubspaceGeneratorsNeeded;
};

struct rules_worldbuilder {
	// Alpha(x).txt #WORLDBUILDER: Seeded land size of a standard world
	// Original bounds: 50 to 4000
	// Default value: 384
	int LandBase;

	// Alpha(x).txt #WORLDBUILDER: Additional land from LAND selection: x0, x1, x2
	// Original bounds: 0 to 2000
	// Default value: 256
	int LandMod;

	// Alpha(x).txt #WORLDBUILDER: Base size of a land mass seed
	// Original bounds: 5 to 1000
	// Default value: 12
	int ContinentBase;

	// Alpha(x).txt #WORLDBUILDER: Increased size from LAND selection: x0, x1, x2
	// Original bounds: 5 to 1000
	// Default value: 24
	int ContinentMod;

	// Alpha(x).txt #WORLDBUILDER: Base # of extra hills
	// Original bounds: 0 to 100
	// Default value: 1
	int HillsBase;

	// Alpha(x).txt #WORLDBUILDER: Additional hills from TIDAL selection: x0, x1, x2
	// Original bounds: 0 to 100
	// Default value: 2
	int HillsMod;

	// Alpha(x).txt #WORLDBUILDER: Basic plateau size
	// Original bounds: 0 to 500
	// Default value: 4
	int PlateauBase;

	// Alpha(x).txt #WORLDBUILDER: Plateau modifier based on LAND selection: x0, x1, x2
	// Original bounds: 0 to 500
	// Default value: 8
	int PlateauMod;

	// Alpha(x).txt #WORLDBUILDER: Basic # of rivers
	// Original bounds: 0 to 100
	// Default value: 8
	int RiversBase;

	// Alpha(x).txt #WORLDBUILDER: Additional rivers based on RAIN selection
	// Original bounds: 0 to 100
	// Default value: 12
	int RiversRainMod;

	// Alpha(x).txt #WORLDBUILDER: Latitude DIVISOR for temperature based on HEAT; 
	// Smaller # increases effect of HEAT selection
	// Original bounds: 1 to 64
	// Default value: 14
	int SolarEnergy;

	// Alpha(x).txt #WORLDBUILDER: Latitude DIVISOR for thermal banding; Smaller # widens hot bands
	// Original bounds: 1 to 64
	// Default value: 14
	int ThermalBand;

	// Alpha(x).txt #WORLDBUILDER: Latitude DIVISOR for thermal deviance; Smaller # increases randomness
	// Original bounds: 1 to 64
	// Default value: 8
	int ThermalDeviance;

	// Alpha(x).txt #WORLDBUILDER: Latitude DIVISOR for global warming; 
	// Smaller # increases effect of warming
	// Original bounds: 1 to 64
	// Default value: 8
	int GlobalWarming;

	// Alpha(x).txt #WORLDBUILDER: Magnitude of sea level changes from ice cap melting/freezing
	// Original bounds: 1 to 100
	// Default value: 5
	int SeaLevelRises;

	// Alpha(x).txt #WORLDBUILDER: Size of cloud mass trapped by peaks
	// Original bounds: 0 to 20
	// Default value: 5
	int CloudmassPeaks;

	// Alpha(x).txt #WORLDBUILDER: Size of cloud mass trapped by hills
	// Original bounds: 0 to 20
	// Default value: 3
	int CloudmassHills;

	// Alpha(x).txt #WORLDBUILDER: Multiplier for rainfall belts
	// Original bounds: 0 to 8
	// Default value: 1
	int RainfallCoeff;

	// Alpha(x).txt #WORLDBUILDER: Encourages fractal to grow deep water
	// Original bounds: -100 to 100
	// Default value: 15
	int DeepWater;

	// Alpha(x).txt #WORLDBUILDER: Encourages fractal to grow shelf
	// Original bounds: -100 to 100
	// Default value: 10
	int Shelf;

	// Alpha(x).txt #WORLDBUILDER: Encourages highland plains
	// Original bounds: -100 to 100
	// Default value: 15
	int Plains;

	// Alpha(x).txt #WORLDBUILDER: Encourages wider beaches
	// Original bounds: -100 to 100
	// Default value: 10
	int Beach;

	// Alpha(x).txt #WORLDBUILDER: Encourages hills x TIDAL selection
	// Original bounds: 0 to 100
	// Default value: 10
	int Hills;

	// Alpha(x).txt #WORLDBUILDER: Encourages peaks
	// Original bounds: -100 to 100
	// Default value: 25
	int Peaks;

	// Alpha(x).txt #WORLDBUILDER: Fungus coefficient based on LIFE selection
	// Original bounds: 0 to 5
	// Default value: 1
	int Fungus;

	// Alpha(x).txt #WORLDBUILDER: Continent size ratios #1
	// Original bounds: n/a
	// Default value: 3
	int ContSizeRatio1;

	// Alpha(x).txt #WORLDBUILDER: Continent size ratios #2
	// Original bounds: n/a
	// Default value: 6
	int ContSizeRatio2;

	// Alpha(x).txt #WORLDBUILDER: Continent size ratios #3
	// Original bounds: n/a
	// Default value: 12
	int ContSizeRatio3;

	// Alpha(x).txt #WORLDBUILDER: Continent size ratios #4
	// Original bounds: n/a
	// Default value: 18	
	int ContSizeRatio4;

	// Alpha(x).txt #WORLDBUILDER: Continent size ratios #5
	// Original bounds: n/a
	// Default value: 24
	int ContSizeRatio5;

	// Alpha(x).txt #WORLDBUILDER: Higher # increases island count
	// Original bounds: 1 to 500
	// Default value: 36
	int Islands;
};

constexpr int NoneValue = -1;
constexpr int DisabledValue = -2;

constexpr int MaxResourceInfoNum = 9;
constexpr int MaxTimeControlNum = 6;
constexpr int MaxCompassNum = 8;
constexpr int MaxResourceNum = 4;
constexpr int MaxEnergyNum = 3;
constexpr int MaxDiffNum = 6;

extern label *Label;
extern rules_resourceinfo *ResourceInfo;
extern rules_time_control *TimeControl;
extern rules_resource *Resource;
extern rules_energy *Energy;
extern rules_basic *Rules;
extern rules_worldbuilder *WorldBuilder;

OPENSMACX_API int __cdecl tech_name(LPSTR techName);
OPENSMACX_API int __cdecl chas_name(LPSTR chasName);
OPENSMACX_API int __cdecl weap_name(LPSTR weapName);
OPENSMACX_API int __cdecl arm_name(LPSTR armName);
OPENSMACX_API int __cdecl tech_item();
OPENSMACX_API BOOL __cdecl read_basic_rules();
OPENSMACX_API BOOL __cdecl read_tech();
OPENSMACX_API void __cdecl clear_faction(player *Player);
OPENSMACX_API void __cdecl read_faction(int playerID);
OPENSMACX_API void __cdecl read_faction(player *Player, int toggle);
OPENSMACX_API BOOL __cdecl read_factions();
OPENSMACX_API void __cdecl noun_item(int *gender, BOOL *plurality);
OPENSMACX_API BOOL __cdecl read_units();
OPENSMACX_API BOOL __cdecl read_rules(BOOL tglAllRules);
OPENSMACX_API void __cdecl prefs_fac_load();
OPENSMACX_API BOOL __cdecl labels_init();
OPENSMACX_API void __cdecl labels_shutdown();
OPENSMACX_API void __cdecl say_label(int labelOffset);
OPENSMACX_API LPSTR __cdecl label_get(int labelOffset);
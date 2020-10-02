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

enum game_base_warnings {
	WARN_STOP_NEW_FAC_BUILT = 0x1,
	WARN_STOP_NON_COMBAT_VEH_BUILT = 0x2,
	WARN_STOP_PROTOTYPE_COMPLETE = 0x4,
	WARN_STOP_DRONE_RIOTS = 0x8,
	WARN_STOP_DRONE_RIOTS_END = 0x10,
	WARN_STOP_GOLDEN_AGE = 0x20,
	WARN_STOP_GOLDEN_AGE_END = 0x40,
	WARN_STOP_NUTRIENT_SHORTAGE = 0x80,
	WARN_STOP_UNK_100 = 0x100, // no text, not visible in PrefWin; set with default warning
	WARN_STOP_BUILD_OUT_OF_DATE = 0x200,
	WARN_STOP_COMBAT_VEH_BUILT = 0x400,
	WARN_STOP_POP_LIMIT_REACHED = 0x800,
	WARN_STOP_DELAY_IN_TRANSCEND = 0x1000,
	WARN_STOP_BUILT_VIA_GOV_QUEUE = 0x2000,
	WARN_STOP_STARVATION = 0x4000,
	WARN_STOP_MINERAL_SHORTAGE = 0x8000,
	WARN_STOP_ENERGY_SHORTAGE = 0x10000,
	WARN_STOP_RANDOM_EVENT = 0x20000,
};

enum game_preferences : uint32_t {
	PREF_BSC_PAUSE_END_TURN = 0x1,
	PREF_BSC_AUTOSAVE_EACH_TURN = 0x2,
	PREF_BSC_DONT_QUICK_MOVE_ENEMY_VEH = 0x4, // flag set when unchecked
	PREF_ADV_FAST_BATTLE_RESOLUTION = 0x8,
	PREF_UNK_10 = 0x10, // no text, not visible in PrefWin; set with default preferences
	PREF_BSC_TUTORIAL_MSGS = 0x20,
	//
	PREF_AV_MAP_ANIMATIONS = 0x80,
	PREF_MAP_SHOW_GRID = 0x100,
	PREF_MAP_SHOW_BASE_GRID = 0x200,
	PREF_AV_VOLUME_SFX_TOGGLE = 0x400,
	PREF_AV_SOUND_EFFECTS = 0x400, // not displayed
	PREF_AV_VOLUME_MUSIC_TOGGLE = 0x800,
	PREF_AV_BACKGROUND_MUSIC = 0x800, // not displayed
	PREF_BSC_MOUSE_EDGE_SCROLL_VIEW = 0x1000,
	//
	PREF_BSC_AUTO_DESIGN_VEH = 0x4000,
	PREF_BSC_DONT_QUICK_MOVE_ALLY_VEH = 0x8000, // flag set when unchecked
	PREF_AUTO_AIR_VEH_RET_HOME_FUEL_RNG = 0x10000,
	PREF_AUTO_FORMER_RAISE_LWR_TERRAIN = 0x20000,
	PREF_AV_INTERLUDES_DISABLED = 0x40000, // flag set when unchecked
	PREF_ADV_NO_CENTER_VEH_ORDERS = 0x80000,
	PREF_AUTO_END_MOVE_SPOT_VEH_PACT = 0x100000,
	PREF_AUTO_END_MOVE_SPOT_VEH_TREATY = 0x200000,
	PREF_AUTO_END_MOVE_SPOT_VEH_TRUCE = 0x400000,
	PREF_AUTO_END_MOVE_SPOT_VEH_WAR = 0x800000,
	PREF_AUTO_FORMER_PLANT_FORESTS = 0x1000000,
	PREF_AUTO_FORMER_BUILD_ADV = 0x2000000, // condensers, boreholes, etc.
	//
	PREF_AV_SLIDING_WINDOWS = 0x8000000,
	PREF_AV_SECRET_PROJECT_MOVIES = 0x10000000,
	PREF_ADV_RADIO_BTN_NOT_SEL_SING_CLK = 0x20000000, // flag set when unchecked
	PREF_AUTO_DONT_END_MOVE_DIFF_TRIAD = 0x40000000,
	PREF_AUTO_WAKE_VEH_TRANS_REACH_LAND = 0x80000000,
};

enum game_more_preferences {
	MPREF_MAP_SHOW_FOG_WAR = 0x1,
	//
	MPREF_ADV_ZOOM_BASE_NO_RECENTER_MAP = 0x4,
	MPREF_AUTO_FORMER_REMOVE_FUNGUS = 0x8,
	MPREF_ADV_PAUSE_AFTER_BATTLES = 0x10,
	MPREF_AUTO_FORMER_BUILD_SENSORS = 0x20,
	MPREF_ADV_QUICK_MOVE_VEH_ORDERS = 0x40,
	MPREF_ADV_QUICK_MOVE_ALL_VEH = 0x80,
	MPREF_ADV_RIGHT_CLICK_POPS_UP_MENU = 0x100,
	MPREF_AV_WHOLE_VEH_BLINKS = 0x200,
	MPREF_ADV_DETAIL_RIGHT_CLICK_MENUS = 0x400,
	MPREF_AUTO_ALWAYS_INSPECT_MONOLITH = 0x800,
	MPREF_MAP_SHOW_PROD_WITH_BASE_NAMES = 0x1000,
	MPREF_MAP_SHOW_BASE_NAMES = 0x2000,
	MPREF_AV_VOLUME_VOICE_TOGGLE = 0x4000,
	MPREF_AV_VOICEOVER_TECH_FAC = 0x4000, // not displayed
	MPREF_ADV_CONFIRM_ODDS_BF_ATTACKING = 0x8000,
	MPREF_MAP_SHOW_FLAT_TERRAIN = 0x10000,
	MPREF_AV_VOICEOVER_STOP_CLOSE_POPUP = 0x20000,
	MPREF_ADV_CLICK_VEH_CANCELS_ORDERS = 0x40000,
	MPREF_AV_SLIDING_SCROLLBARS = 0x80000,
	MPREF_BSC_AUTO_PRUNE_OBS_VEH = 0x100000,
	MPREF_ADV_DETAIL_MAIN_MENUS = 0x200000,
	MPREF_AUTO_FORMER_CANT_BUILD_ROADS = 0x400000, // or tubes; flag set when unchecked
	MPREF_MAP_SHOW_GRID_OCEAN_SQ = 0x800000,
	MPREF_AV_MONUMENTS_DISABLED = 0x1000000, // flag set when unchecked
	MPREF_MAP_HIDE_ACTIVE_VEH_GOTO_PATH = 0x2000000, // flag set when unchecked
	//
	//
	//
	//
	//
	//
};

struct label {
	LPSTR stringsPtr;
	int count;
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
	uint32_t MoveRateRoads;

	// Alpha(x).txt #RULES: Nutrient intake requirement for citizens
	// Original bounds: 0 to 100
	// Default value: 2
	uint32_t NutrientReqCitizen;

	// Alpha(x).txt #RULES: Max airdrop range w/o orbital insertion
	// Original bounds: 1 to 500
	// Default value: 8
	uint32_t MaxAirdropSansOrbInsert;

	// Alpha(x).txt #RULES: Max artillery range (larger will break multiplayer)
	// Original bounds: 1 to 8
	// Default value: 2
	uint32_t ArtilleryMaxRng;

	// Alpha(x).txt #RULES: Numerator for artillery fire damage
	// Original bounds: 1 to 1000
	// Default value: 3
	uint32_t ArtilleryDmgNum;

	// Alpha(x).txt #RULES: Denominator for artillery fire damage
	// Original bounds: 1 to 1000
	// Default value: 2
	uint32_t ArtilleryDmgDenom;

	// Alpha(x).txt #RULES: Nutrient cost multiplier
	// Original bounds: 1 to 100
	// Default value: 10
	uint32_t NutrientCostMulti;

	// Alpha(x).txt #RULES: Minerals cost multiplier
	// Original bounds: 1 to 100
	// Default value: 10
	uint32_t MineralCostMulti;

	// Alpha(x).txt #RULES: Technology discovery rate as a percentage of standard
	// Original bounds: 0 to 1000
	// Default value: 100
	uint32_t TechDiscoveryRatePctStd;

	// Alpha(x).txt #RULES: Limits mineral increase for mine without road in square
	// Original bounds: 0 to 100
	// Default value: 1
	uint32_t LimitMineralMineSansRoad;

	// Alpha(x).txt #RULES: Nutrient effect in mine square (0 or -1)
	// Original bounds: 0 or -1
	// Default value: -1
	int TglNutrientEffectWithMine;

	// Alpha(x).txt #RULES: Minimum base size to support specialists
	// Original bounds: 0 to 100
	// Default value: 5
	uint32_t MinBaseSizeSpecialists;

	// Alpha(x).txt #RULES: Drones induced by Genejack factory
	// Original bounds: 0 to 100
	// Default value: 1
	uint32_t DronesGenejackFactory;

	// Alpha(x).txt #RULES: Population limit w/o hab complex
	// Original bounds: 1 to 100
	// Default value: 7
	uint32_t PopLimitSansHabComplex;

	// Alpha(x).txt #RULES: Population limit w/o hab dome
	// Original bounds: 1 to 100
	// Default value: 14
	uint32_t PopLimitSansHabDome;

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
	uint32_t ExtraPctCostProtoLand;

	// Alpha(x).txt #RULES: Extra percentage cost of prototype SEA unit
	// Original bounds: 0 to 500
	// Default value: 50
	uint32_t ExtraPctCostProtoSea;

	// Alpha(x).txt #RULES: Extra percentage cost of prototype AIR unit
	// Original bounds: 0 to 500
	// Default value: 50
	uint32_t ExtraPctCostProtoAir;

	// Alpha(x).txt #RULES: Psi combat offense-to-defense ratio (unit defending) Numerator
	// Original bounds: 1 to 1000
	// Default value: 3 (LAND)
	// Default value: 1 (SEA/AIR)
	uint32_t PsiCombatRatioAtk[3]; // LAND, SEA, AIR

	// Alpha(x).txt #RULES: Psi combat offense-to-defense ratio (unit defending) Denominator
	// Original bounds: 1 to 1000
	// Default value: 2 (LAND)
	// Default value: 1 (SEA/AIR)
	uint32_t PsiCombatRatioDef[3]; // LAND, SEA, AIR

	// Alpha(x).txt #RULES: Players' starting energy reserves
	// Original bounds: 0 to 1000
	// Default value: 10
	uint32_t PlayerStartEnergyReserve;

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
	uint32_t MinTurnsCouncils;

	// Alpha(x).txt #RULES: Minerals for harvesting forest
	// Original bounds: 0 to 100
	// Default value: 5
	uint32_t MineralsHarvestingForest;

	// Alpha(x).txt #RULES: Territory: max distance from base
	// Original bounds: 0 to 100
	// Default value: 8
	uint32_t TerritoryMaxDistBase;

	// Alpha(x).txt #RULES: Turns to corner Global Energy Market
	// Original bounds: 1 to 100
	// Default value: 20
	uint32_t TurnsCornerGblEnergyMrkt;

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
	uint32_t RetoolStrictness;

	// Alpha(x).txt #RULES: Retool percent penalty for production change
	// Original bounds: 0 to 100
	// Default value: 50
	uint32_t RetoolPctPenProdChg;

	// Alpha(x).txt #RULES: Retool exemption (first X minerals not affected by penalty)
	// Original bounds: 0 to 1000
	// Default value: 10
	uint32_t RetoolExemption;

	// Alpha(x).txt #RULES: If non-zero, probe teams can steal technologies
	// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
	// Default value: 1
	BOOL TglProbeCanStealTech;

	// Alpha(x).txt #RULES: If non-zero, humans can always contact each other in net games
	// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
	// Default value: 1
	BOOL TglHumanAlwaysContactNet;

	// Alpha(x).txt #RULES: If non-zero, humans can always contact each other in hotseat/email games
	// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
	// Default value: 1
	BOOL TglHumansAlwaysContactPBEM;

	// Alpha(x).txt #RULES: Maximum % damage inflicted by arty versus units in base/bunker
	// Original bounds: 10 to 100
	// Default value: 50
	uint32_t MaxPctDmgArtVsUnitBaseBnkr;

	// Alpha(x).txt #RULES: Maximum % damage inflicted by arty versus units in open
	// Original bounds: 10 to 100
	// Default value: 99
	uint32_t MaxPctDmgArtVsUnitsOpen;

	// Alpha(x).txt #RULES: Maximum % damage inflicted by arty versus units in sea
	// Original bounds: 10 to 100
	// Default value: 100
	uint32_t MaxPctDmgArtVsUnitsSea;

	// Alpha(x).txt #RULES: Numerator for freq of global warming (1,2 would be "half" normal warming).
	// Original bounds: 0 to 1000
	// Default value: 1
	uint32_t FreqGlobalWarmingNum;

	// Alpha(x).txt #RULES: Denominator for freq of global warming (1,2 would be "half" normal warming).
	// Original bounds: 1 to 1000
	// Default value: 1
	uint32_t FreqGlobalWarmingDenom;

	// Alpha(x).txt #RULES: Normal starting year
	// Original bounds: 0 to 999999
	// Default value: 2100
	uint32_t NormalStartingYear;

	// Alpha(x).txt #RULES: Normal ending year for lowest 3 difficulty levels
	// Original bounds: 0 to 999999
	// Default value: 2600
	uint32_t NormalEndYearLowThreeDiff;

	// Alpha(x).txt #RULES: Normal ending year for highest 3 difficulty levels
	// Original bounds: 0 to 999999
	// Default value: 2500
	uint32_t NormalEndYearHighThreeDiff;

	// Alpha(x).txt #RULES: If non-zero, obliterating a base counts as an atrocity
	// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
	// Default value: 1
	BOOL TglOblitBaseAtrocity;

	// Alphax.txt #RULES: Size of base for subspace generator
	// Original bounds: 1 to 999
	// Default value: 10
	uint32_t SizeBaseSubspaceGen;

	// Alphax.txt #RULES: Number of subspace generators needed
	// Original bounds: 1 to 999
	// Default value: 6
	uint32_t SubspaceGeneratorsNeeded;
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

struct alpha_ini {
	uint32_t Preferences;
	uint32_t MorePreferences;
	uint32_t Announce;
	uint32_t Rules;
	uint32_t Semaphore;
	uint32_t TimeControls;
	uint32_t Customize;
	uint32_t CustomWorld[7];
};

struct default_pref {
	uint32_t Difficulty;
	uint32_t FactionID;
	uint32_t pad; // unused
	uint32_t MapType;
	uint32_t TopMenu;
};

constexpr int NoneValue = -1;
constexpr int DisabledValue = -2;

constexpr int MaxResourceInfoNum = 9;
constexpr int MaxTimeControlNum = 6;
constexpr int MaxCompassNum = 8;
constexpr int MaxResourceNum = 4;
constexpr int MaxEnergyNum = 3;
constexpr int MaxDiffNum = 6;

extern LPCSTR AlphaxFileID;
extern LPCSTR ScriptTxtID;
extern label *Label;
extern rules_resourceinfo *ResourceInfo;
extern rules_time_control *TimeControl;
extern rules_resource *Resource;
extern rules_energy *Energy;
extern rules_basic *Rules;
extern rules_worldbuilder *WorldBuilder;
extern alpha_ini *AlphaIni;
extern default_pref *DefaultPrefs;
extern uint32_t *Language;

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
OPENSMACX_API LPSTR __cdecl prefs_get(LPCSTR keyName, LPCSTR defaultValue, BOOL useDefault);
OPENSMACX_API uint32_t __cdecl default_prefs();
OPENSMACX_API uint32_t __cdecl default_prefs2();
OPENSMACX_API uint32_t __cdecl default_warn();
OPENSMACX_API uint32_t __cdecl default_rules();
OPENSMACX_API int __cdecl prefs_get(LPCSTR keyName, int defaultValue, BOOL useDefault);
OPENSMACX_API void __cdecl prefs_fac_load();
OPENSMACX_API void __cdecl prefs_load(BOOL useDefault);
OPENSMACX_API void __cdecl prefs_put(LPCSTR keyName, LPCSTR value);
OPENSMACX_API void __cdecl prefs_put(LPCSTR keyName, int value, BOOL binaryTgl);
OPENSMACX_API void __cdecl prefs_save(BOOL saveFactions);
OPENSMACX_API void __cdecl prefs_use();
OPENSMACX_API std::string __cdecl prefs_get_binary(int value);
OPENSMACX_API BOOL __cdecl labels_init();
OPENSMACX_API void __cdecl labels_shutdown();
OPENSMACX_API void __cdecl set_language(uint32_t language);
OPENSMACX_API void __cdecl say_label(int labelOffset);
OPENSMACX_API LPSTR __cdecl label_get(int labelOffset);

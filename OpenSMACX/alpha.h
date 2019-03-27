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
  * Parsing alpha/x.txt
  */
struct Label {
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

const char *AlphaxFileID = "ALPHAX";

// Alpha(x).txt #RULES: Movement rate along roads
// Original bounds: 1 to 100
// Default value: 3
int *RulesMoveRateRoads = (int *)0x00949738;

// Alpha(x).txt #RULES: Nutrient intake requirement for citizens
// Original bounds: 0 to 100
// Default value: 2
int *RulesNutrientReqCitizen = (int *)0x0094973C;

// Alpha(x).txt #RULES: Numerator for artillery fire damage
// Original bounds: 1 to 1000
// Default value: 3
int *RulesArtilleryDmgNum = (int *)0x00949748;

// Alpha(x).txt #RULES: Denominator for artillery fire damage
// Original bounds: 1 to 1000
// Default value: 2
int *RulesArtilleryDmgDenom = (int *)0x0094974C;

// Alpha(x).txt #RULES: Max artillery range (larger will break multiplayer)
// Original bounds: 1 to 8
// Default value: 2
int *RulesArtilleryMaxRng = (int *)0x00949744;

// Alpha(x).txt #RULES: Max airdrop range w/o orbital insertion
// Original bounds: 1 to 500
// Default value: 8
int *RulesMaxAirdropSansOrbInsert = (int *)0x00949740;

// Alpha(x).txt #RULES: Nutrient cost multiplier
// Original bounds: 1 to 100
// Default value: 10
int *RulesNutrientCostMulti = (int *)0x00949750;

// Alpha(x).txt #RULES: Minerals cost multiplier
// Original bounds: 1 to 100
// Default value: 10
int *RulesMineralCostMulti = (int *)0x00949754;

// Alpha(x).txt #RULES: Technology discovery rate as a percentage of standard
// Original bounds: 0 to 1000
// Default value: 100
int *RulesTechDiscovRatePctStd = (int *)0x00949758;

// Alpha(x).txt #RULES: Limits mineral increase for mine without road in square
// Original bounds: 0 to 100
// Default value: 1
int *RulesLimitMineralMineSansRoad = (int *)0x0094975C;

// Alpha(x).txt #RULES: Nutrient effect in mine square (0 or -1)
// Original bounds: 0 or -1
// Default value: -1
int *RulesTglNutrientEffectWithMine = (int *)0x00949760;

// Alpha(x).txt #RULES: Minimum base size to support specialists
// Original bounds: 0 to 100
// Default value: 5
int *RulesMinBaseSizeSpecialists = (int *)0x00949764;

// Alpha(x).txt #RULES: Drones induced by Genejack factory
// Original bounds: 0 to 100
// Default value: 1
int *RulesDronesGenejackFactory = (int *)0x00949768;

// Alpha(x).txt #RULES: Population limit w/o hab complex
// Original bounds: 1 to 100
// Default value: 7
int *RulesPopLimitSansHabComplex = (int *)0x0094976C;

// Alpha(x).txt #RULES: Population limit w/o hab dome
// Original bounds: 1 to 100
// Default value: 14
int *RulesPopLimitSansHabDome = (int *)0x00949770;

// Alpha(x).txt #RULES: Extra percentage cost of prototype LAND unit
// Original bounds: 0 to 500
// Default value: 50
int *RulesExtraPctCostProtoLand = (int *)0x00949790;

// Alpha(x).txt #RULES: Extra percentage cost of prototype SEA unit
// Original bounds: 0 to 500
// Default value: 50
int *RulesExtraPctCostProtoSea = (int *)0x00949794;

// Alpha(x).txt #RULES: Extra percentage cost of prototype AIR unit
// Original bounds: 0 to 500
// Default value: 50
int *RulesExtraPctCostProtoAir = (int *)0x00949798;

// Alpha(x).txt #RULES: Psi combat offense-to-defense ratio (LAND unit defending) Numerator
// Original bounds: 1 to 1000
// Default value: 3
int *RulesPsiCombatRatioLandAtk = (int *)0x0094979C;

// Alpha(x).txt #RULES: Psi combat offense-to-defense ratio (LAND unit defending) Denominator
// Original bounds: 1 to 1000
// Default value: 2
int *RulesPsiCombatRatioLandDef = (int *)0x009497A8;

// Alpha(x).txt #RULES: Psi combat offense-to-defense ratio (SEA unit defending) Numerator
// Original bounds: 1 to 1000
// Default value: 1
int *RulesPsiCombatRatioSeaAtk = (int *)0x009497A0;

// Alpha(x).txt #RULES: Psi combat offense-to-defense ratio (SEA unit defending) Denominator
// Original bounds: 1 to 1000
// Default value: 1
int *RulesPsiCombatRatioSeaDef = (int *)0x009497AC;

// Alpha(x).txt #RULES: Psi combat offense-to-defense ratio (AIR unit defending) Numerator
// Original bounds: 1 to 1000
// Default value: 1
int *RulesPsiCombatRatioAirAtk = (int *)0x009497A4;

// Alpha(x).txt #RULES: Psi combat offense-to-defense ratio (AIR unit defending) Denominator
// Original bounds: 1 to 1000
// Default value: 1
int *RulesPsiCombatRatioAirDef = (int *)0x009497B0;

// Alpha(x).txt #RULES: Players' starting energy reserves
// Original bounds: 0 to 1000
// Default value: 10
int *RulesPlayerStartEnergyReserve = (int *)0x009497B4;

// Alpha(x).txt #RULES: Combat % -> intrinsic base defense
// Original bounds: -100 to 1000
// Default value: 25
int *RulesCombatPctBaseDef = (int *)0x009497B8;

// Alpha(x).txt #RULES: Combat % -> attacking along road
// Original bounds: -100 to 1000
// Default value: 0
int *RulesCombatPctAtkRoad = (int *)0x009497BC;

// Alpha(x).txt #RULES: Combat % -> for attacking from higher elevation
// Original bounds: -100 to 1000
// Default value: 0
int *RulesCombatPctAtkHigherElev = (int *)0x009497C0;

// Alpha(x).txt #RULES: Combat penalty % -> attacking from lower elevation
// Original bounds: -100 to 1000
// Default value: 0
int *RulesCombatPenPctAtkLwrElev = (int *)0x009497C4;

// Alpha(x).txt #RULES: Combat % -> Mobile unit in open ground
// Original bounds: -100 to 1000
// Default value: 25
int *RulesCombatPctMobileOpenGround = (int *)0x009497F4;

// Alpha(x).txt #RULES: Combat % -> Defend vs. mobile in rough
// Original bounds: -100 to 1000
// Default value: 0
int *RulesCombatPctDefVsMobileRough = (int *)0x009497F8;

// Alpha(x).txt #RULES: Combat % -> Infantry vs. Base
// Original bounds: -100 to 1000
// Default value: 25
int *RulesCombatPctInfantryVsBase = (int *)0x00949804;

// Alpha(x).txt #RULES: Combat penalty % -> attack after airdrop
// Original bounds: -100 to 1000
// Default value: 50
int *RulesCombatPenPctAtkAirdrop = (int *)0x009497E4;

// Alpha(x).txt #RULES: Combat % -> Fanatic attack bonus
// Original bounds: -100 to 1000
// Default value: 25
int *RulesCombatPctFanaticAtkBonus = (int *)0x009497E8;

// Alpha(x).txt #RULES: Combat % -> Land based guns vs. ship artillery bonus
// Original bounds: -100 to 1000
// Default value: 50
int *RulesCombatPctLandGunVsShipArt = (int *)0x009497EC;

// Alpha(x).txt #RULES: Combat % -> Artillery bonus per level of altitude
// Original bounds: -100 to 1000
// Default value: 25
int *RulesCombatPctArtBonusLvlAlt = (int *)0x009497F0;

// Alpha(x).txt #RULES: Combat % -> Trance bonus defending vs. psi
// Original bounds: -100 to 1000
// Default value: 50
int *RulesCombatPctTranceDefVsPsi = (int *)0x009497FC;

// Alpha(x).txt #RULES: Combat % -> Empath Song bonus attacking vs. psi
// Original bounds: -100 to 1000
// Default value: 50
int *RulesCombatPctEmpSongAtkVsPsi = (int *)0x00949800;

// Alpha(x).txt #RULES: Combat penalty % -> Air superiority unit vs. ground unit
// Original bounds: -100 to 1000
// Default value: 50
int *RulesCombatPenPctAirSuprVsGrnd = (int *)0x00949808;

// Alpha(x).txt #RULES: Combat % -> Air superiority unit vs. air unit
// Original bounds: -100 to 1000
// Default value: 100
int *RulesCombatPctAirSuprVsAir = (int *)0x0094980C;

// Alpha(x).txt #RULES: Combat penalty % -> Non-combat unit defending vs. combat unit
// Original bounds: -100 to 1000
// Default value: 50
int *RulesCombatPenPctNonCbtDefVsCbt = (int *)0x00949810;

// Alpha(x).txt #RULES: Combat % -> Comm Jammer unit defending vs. mobile unit
// Original bounds: -100 to 1000
// Default value: 50
int *RulesCombatPctComJamDefVsMobl = (int *)0x00949814;

// Alpha(x).txt #RULES: Combat % -> Bonus vs. ships caught in port
// Original bounds: -100 to 1000
// Default value: 100
int *RulesCombatPctBonusVsShipPort = (int *)0x00949818;

// Alpha(x).txt #RULES: Combat % -> AAA bonus vs. air units
// Original bounds: -100 to 1000
// Default value: 100
int *RulesCombatPctAAABonusVsAir = (int *)0x0094981C;

// Alpha(x).txt #RULES: Combat % -> Defend in range of friendly Sensor
// Original bounds: -100 to 1000
// Default value: 25
int *RulesCombatPctDefRangeSensor = (int *)0x00949820;

// Alpha(x).txt #RULES: Combat % -> Psi attack bonus/penalty per +PLANET
// Original bounds: -100 to 1000
// Default value: 10
int *RulesCombatPctPsiAtkBonusPLANET = (int *)0x00949824;

// Alpha(x).txt #RULES: Retool percent penalty for production change
// Original bounds: 0 to 100
// Default value: 50
int *RulesRetoolPctPenProdChg = (int *)0x0094982C;

// Alpha(x).txt #RULES: Retool strictness 
// (0 = Always Free, 1 = Free in Category, 2 = Free if Project, 3 = Never Free)
// Original bounds: 0 to 2 (Bug Fix: Should be 0 to 3)
// Default value: 2
int *RulesRetoolStrictness = (int *)0x00949828;

// Alpha(x).txt #RULES: Retool exemption (first X minerals not affected by penalty)
// Original bounds: 0 to 1000
// Default value: 10
int *RulesRetoolExemption = (int *)0x00949830;

// Alpha(x).txt #RULES: Minimum # of turns between councils
// Original bounds: 0 to 1000
// Default value: 20
int *RulesMinTurnsCouncils = (int *)0x009497CC;

// Alpha(x).txt #RULES: Minerals for harvesting forest
// Original bounds: 0 to 100
// Default value: 5
int *RulesMineralsHarvestingForest = (int *)0x009497D0;

// Alpha(x).txt #RULES: Territory: max distance from base
// Original bounds: 0 to 100
// Default value: 8
int *RulesTerritoryMaxDistBase = (int *)0x009497D4;

// Alpha(x).txt #RULES: Turns to corner Global Energy Market
// Original bounds: 1 to 100
// Default value: 20
int *RulesTurnsCornerGblEnergyMrkt = (int *)0x009497D8;

// Alpha(x).txt #RULES: Technology to improve fungus squares
// Default value: CentPsi (63)
int *RulesTechImproveFungusSqr = (int *)0x00949774;

// Alpha(x).txt #RULES: Technology to ease fungus movement
// Default value: CentPsi (63)
int *RulesTechEaseFungusMovement = (int *)0x00949778;

// Alpha(x).txt #RULES: Technology to build roads in fungus
// Default value: CentEmp (58)
int *RulesTechBuildRoadsFungus = (int *)0x0094977C;

// Alpha(x).txt #RULES: Technology to allow 2 special abilities for a unit
// Default value: Neural (52)
int *RulesTechTwoSpecAbilities = (int *)0x00949780;

// Alpha(x).txt #RULES: Technology to allow 3 nutrients in a square
// Default value: Gene (49)
int *RulesTechThreeNutrientsSqr = (int *)0x00949784;

// Alpha(x).txt #RULES: Technology to allow 3 minerals in a square
// Default value: EcoEng (60)
int *RulesTechThreeMineralsSqr = (int *)0x00949788;

// Alpha(x).txt #RULES: Technology to allow 3 energy in a square
// Default value: EnvEcon (59)
int *RulesTechThreeEnergySqr = (int *)0x0094978C;

// Alpha(x).txt #RULES: Technology to allow orbital insertion w/o Space Elevator
// Default value: Gravity (21)
int *RulesTechOrbInsertSansSpcElev = (int *)0x009497C8;

// Alpha(x).txt #RULES: Technology for +1 mining platform bonus
// Default value: EcoEng2 (62)
int *RulesTechMiningPlatformBonus = (int *)0x009497DC;

// Alpha(x).txt #RULES: Technology for economic victory
// Default value: PlaEcon (61)
int *RulesTechEconomicVictory = (int *)0x009497E0;

// Alpha(x).txt #RULES: If non-zero, probe teams can steal technologies
// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
// Default value: 1
int *RulesTglProbeCanStealTech = (int *)0x00949834;

// Alpha(x).txt #RULES: If non-zero, humans can always contact each other in net games
// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
// Default value: 1
int *RulesTglHumanAlwaysContactNet = (int *)0x00949838;

// Alpha(x).txt #RULES: If non-zero, humans can always contact each other in hotseat/email games
// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
// Default value: 1
int *RulesTglHumansAlwaysContactPbem = (int *)0x0094983C;

// Alpha(x).txt #RULES: Maximum % damage inflicted by arty versus units in base/bunker
// Original bounds: 10 to 100
// Default value: 50
int *RulesMaxPctDmgArtVsUnitBaseBnkr = (int *)0x00949840;

// Alpha(x).txt #RULES: Maximum % damage inflicted by arty versus units in open
// Original bounds: 10 to 100
// Default value: 99
int *RulesMaxPctDmgArtVsUnitsOpen = (int *)0x00949844;

// Alpha(x).txt #RULES: Maximum % damage inflicted by arty versus units in sea
// Original bounds: 10 to 100
// Default value: 100
int *RulesMaxPctDmgArtVsUnitsSea = (int *)0x00949848;

// Alpha(x).txt #RULES: Numerator for freq of global warming (1,2 would be "half" normal warming).
// Original bounds: 0 to 1000
// Default value: 1
int *RulesFreqGlobalWarmingNum = (int *)0x0094984C;

// Alpha(x).txt #RULES: Denominator for freq of global warming (1,2 would be "half" normal warming).
// Original bounds: 1 to 1000
// Default value: 1
int *RulesFreqGlobalWarmingDenom = (int *)0x00949850;

// Alpha(x).txt #RULES: Normal starting year
// Original bounds: 0 to 999999
// Default value: 2100
int *RulesNormalStartingYear = (int *)0x00949854;

// Alpha(x).txt #RULES: Normal ending year for lowest 3 difficulty levels
// Original bounds: 0 to 999999
// Default value: 2600
int *RulesNormalEndYearLowThreeDiff = (int *)0x00949858;

// Alpha(x).txt #RULES: Normal ending year for highest 3 difficulty levels
// Original bounds: 0 to 999999
// Default value: 2500
int *RulesNormalEndYearHighThreeDiff = (int *)0x0094985C;

// Alpha(x).txt #RULES: If non-zero, obliterating a base counts as an atrocity
// Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
// Default value: 1
int *RulesTglOblitBaseAtrocity = (int *)0x00949860;

// Alphax.txt #RULES: Size of base for subspace generator
// Original bounds: 1 to 999
// Default value: 10
int *RulesSizeBaseSubspaceGen = (int *)0x00949864;

// Alphax.txt #RULES: Number of subspace generators needed
// Original bounds: 1 to 999
// Default value: 6
int *RulesSubspaceGeneratorsNeeded = (int *)0x00949868;

// Alpha(x).txt #WORLDBUILDER: Seeded land size of a standard world
// Original bounds: 50 to 4000
// Default value: 384
int *WorldBuilderLandBase = (int *)0x009502A8;

// Alpha(x).txt #WORLDBUILDER: Additional land from LAND selection: x0, x1, x2
// Original bounds: 0 to 2000
// Default value: 256
int *WorldBuilderLandMod = (int *)0x009502AC;

// Alpha(x).txt #WORLDBUILDER: Base size of a land mass seed
// Original bounds: 5 to 1000
// Default value: 12
int *WorldBuilderContinentBase = (int *)0x009502B0;

// Alpha(x).txt #WORLDBUILDER: Increased size from LAND selection: x0, x1, x2
// Original bounds: 5 to 1000
// Default value: 24
int *WorldBuilderContinentMod = (int *)0x009502B4;

// Alpha(x).txt #WORLDBUILDER: Base # of extra hills
// Original bounds: 0 to 100
// Default value: 1
int *WorldBuilderHillsBase = (int *)0x009502B8;

// Alpha(x).txt #WORLDBUILDER: Additional hills from TIDAL selection: x0, x1, x2
// Original bounds: 0 to 100
// Default value: 2
int *WorldBuilderHillsMod = (int *)0x009502BC;

// Alpha(x).txt #WORLDBUILDER: Basic plateau size
// Original bounds: 0 to 500
// Default value: 4
int *WorldBuilderPlateauBase = (int *)0x009502C0;

// Alpha(x).txt #WORLDBUILDER: Plateau modifier based on LAND selection: x0, x1, x2
// Original bounds: 0 to 500
// Default value: 8
int *WorldBuilderPlateauMod = (int *)0x009502C4;

// Alpha(x).txt #WORLDBUILDER: Basic # of rivers
// Original bounds: 0 to 100
// Default value: 8
int *WorldBuilderRiversBase = (int *)0x009502C8;

// Alpha(x).txt #WORLDBUILDER: Additional rivers based on RAIN selection
// Original bounds: 0 to 100
// Default value: 12
int *WorldBuilderRiversRainMod = (int *)0x009502CC;

// Alpha(x).txt #WORLDBUILDER: Latitude DIVISOR for temperature based on HEAT; 
// Smaller # increases effect of HEAT selection
// Original bounds: 1 to 64
// Default value: 14
int *WorldBuilderSolarEnergy = (int *)0x009502D0;

// Alpha(x).txt #WORLDBUILDER: Latitude DIVISOR for thermal banding; Smaller # widens hot bands
// Original bounds: 1 to 64
// Default value: 14
int *WorldBuilderThermalBand = (int *)0x009502D4;

// Alpha(x).txt #WORLDBUILDER: Latitude DIVISOR for thermal deviance; Smaller # increases randomness
// Original bounds: 1 to 64
// Default value: 8
int *WorldBuilderThermalDeviance = (int *)0x009502D8;

// Alpha(x).txt #WORLDBUILDER: Latitude DIVISOR for global warming; 
// Smaller # increases effect of warming
// Original bounds: 1 to 64
// Default value: 8
int *WorldBuilderGlobalWarming = (int *)0x009502DC;

// Alpha(x).txt #WORLDBUILDER: Magnitude of sea level changes from ice cap melting/freezing
// Original bounds: 1 to 100
// Default value: 5
int *WorldBuilderSeaLevelRises = (int *)0x009502E0;

// Alpha(x).txt #WORLDBUILDER: Size of cloud mass trapped by peaks
// Original bounds: 0 to 20
// Default value: 5
int *WorldBuilderCloudmassPeaks = (int *)0x009502E4;

// Alpha(x).txt #WORLDBUILDER: Size of cloud mass trapped by hills
// Original bounds: 0 to 20
// Default value: 3
int *WorldBuilderCloudmassHills = (int *)0x009502E8;

// Alpha(x).txt #WORLDBUILDER: Multiplier for rainfall belts
// Original bounds: 0 to 8
// Default value: 1
int *WorldBuilderRainfallCoeff = (int *)0x009502EC;

// Alpha(x).txt #WORLDBUILDER: Encourages fractal to grow deep water
// Original bounds: -100 to 100
// Default value: 15
int *WorldBuilderDeepWater = (int *)0x009502F0;

// Alpha(x).txt #WORLDBUILDER: Encourages fractal to grow shelf
// Original bounds: -100 to 100
// Default value: 10
int *WorldBuilderShelf = (int *)0x009502F4;

// Alpha(x).txt #WORLDBUILDER: Encourages highland plains
// Original bounds: -100 to 100
// Default value: 15
int *WorldBuilderPlains = (int *)0x009502F8;

// Alpha(x).txt #WORLDBUILDER: Encourages wider beaches
// Original bounds: -100 to 100
// Default value: 10
int *WorldBuilderBeach = (int *)0x009502FC;

// Alpha(x).txt #WORLDBUILDER: Encourages hills x TIDAL selection
// Original bounds: 0 to 100
// Default value: 10
int *WorldBuilderHills = (int *)0x00950300;

// Alpha(x).txt #WORLDBUILDER: Encourages peaks
// Original bounds: -100 to 100
// Default value: 25
int *WorldBuilderPeaks = (int *)0x00950304;

// Alpha(x).txt #WORLDBUILDER: Fungus coefficient based on LIFE selection
// Original bounds: 0 to 5
// Default value: 1
int *WorldBuilderFungus = (int *)0x00950308;

// Alpha(x).txt #WORLDBUILDER: Continent size ratios #1
// Original bounds: n/a
// Default value: 3
int *WorldBuilderContSizeRatio1 = (int *)0x0095030C;

// Alpha(x).txt #WORLDBUILDER: Continent size ratios #2
// Original bounds: n/a
// Default value: 6
int *WorldBuilderContSizeRatio2 = (int *)0x00950310;

// Alpha(x).txt #WORLDBUILDER: Continent size ratios #3
// Original bounds: n/a
// Default value: 12
int *WorldBuilderContSizeRatio3 = (int *)0x00950314;

// Alpha(x).txt #WORLDBUILDER: Continent size ratios #4
// Original bounds: n/a
// Default value: 18	
int *WorldBuilderContSizeRatio4 = (int *)0x00950318;

// Alpha(x).txt #WORLDBUILDER: Continent size ratios #5
// Original bounds: n/a
// Default value: 24
int *WorldBuilderContSizeRatio5 = (int *)0x0095031C;

// Alpha(x).txt #WORLDBUILDER: Higher # increases island count
// Original bounds: 1 to 500
// Default value: 36
int *WorldBuilderIslands = (int *)0x00950320;

constexpr int NoneValue = -1;
constexpr int DisabledValue = -2;

constexpr int MaxResourceInfoNum = 9;
constexpr int MaxTimeControlNum = 6;
constexpr int MaxCompassNum = 8;
constexpr int MaxResourceNum = 4;
constexpr int MaxEnergyNum = 3;
constexpr int MaxDiffNum = 6;

extern Label *Labels;
extern rules_resourceinfo *ResourceInfo;
extern rules_time_control *TimeControl;
extern rules_resource *Resource;
extern rules_energy *Energy;

OPENSMACX_API BOOL __cdecl labels_init();
OPENSMACX_API void __cdecl labels_shutdown();
OPENSMACX_API int __cdecl tech_item();
OPENSMACX_API void __cdecl noun_item(int *gender, BOOL *plurality);
OPENSMACX_API int __cdecl tech_name(LPSTR techName);
OPENSMACX_API int __cdecl chas_name(LPSTR chasName);
OPENSMACX_API int __cdecl weap_name(LPSTR weapName);
OPENSMACX_API int __cdecl arm_name(LPSTR armName);
OPENSMACX_API BOOL __cdecl read_basic_rules();
OPENSMACX_API BOOL __cdecl read_tech();
OPENSMACX_API BOOL __cdecl read_units();
OPENSMACX_API void __cdecl clear_faction(Player *player);
OPENSMACX_API void __cdecl read_faction(int playerID);
OPENSMACX_API void __cdecl read_faction(Player *player, int toggle);
OPENSMACX_API void __cdecl prefs_fac_load();
OPENSMACX_API BOOL __cdecl read_factions();
OPENSMACX_API BOOL __cdecl read_rules(BOOL tglAllRules);
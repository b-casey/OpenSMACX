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
  * Vehicle/unit related objects, variables and functions.
  */
enum veh_morale {
	MORALE_VERY_GREEN = 0,
	MORALE_GREEN = 1,
	MORALE_DISCIPLINED = 2,
	MORALE_HARDENED = 3,
	MORALE_VETERAN = 4,
	MORALE_COMMANDO = 5,
	MORALE_ELITE = 6,
};

enum veh_basic_id {
	BSC_COLONY_POD = 0,
	BSC_FORMERS = 1,
	BSC_SCOUT_PATROL = 2,
	BSC_TRANSPORT_FOIL = 3,
	BSC_SEA_FORMERS = 4,
	BSC_SUPPLY_CRAWLER = 5,
	BSC_PROBE_TEAM = 6,
	BSC_ALIEN_ARTIFACT = 7,
	BSC_MIND_WORMS = 8,
	BSC_ISLE_OF_THE_DEEP = 9,
	BSC_LOCUSTS_OF_CHIRON = 10,
	BSC_UNITY_ROVER = 11,
	BSC_UNITY_SCOUT_CHOPPER = 12,
	BSC_UNITY_FOIL = 13,
	BSC_SEALURK = 14,
	BSC_SPORE_LAUNCHER = 15,
	BSC_BATTLE_OGRE_MK1 = 16,
	BSC_BATTLE_OGRE_MK2 = 17,
	BSC_BATTLE_OGRE_MK3 = 18,
	BSC_FUNGAL_TOWER = 19,
	BSC_UNITY_MINING_LASER = 20,
	BSC_SEA_ESCAPE_POD = 21,
	BSC_UNITY_GUNSHIP = 22,
};

enum veh_triad {
	TRIAD_LAND = 0,
	TRIAD_SEA = 1,
	TRIAD_AIR = 2,
};

enum veh_abilities_bitfield {
	ABL_SUPER_TERRAFORMER = 0x1,
	ABL_DEEP_RADAR = 0x2,
	ABL_CLOAKED = 0x4,
	ABL_AMPHIBIOUS = 0x8,
	ABL_DROP_POD = 0x10,
	ABL_AIR_SUPERIORITY = 0x20,
	ABL_DEEP_PRESSURE_HULL = 0x40,
	ABL_CARRIER = 0x80,
	ABL_AAA = 0x100,
	ABL_COMM_JAMMER = 0x200,
	ABL_ANTIGRAV_STRUTS = 0x400,
	ABL_EMPATHIC = 0x800,
	ABL_POLY_ENCRYPTION = 0x1000,
	ABL_FUNGICIDAL = 0x2000,
	ABL_TRAINED = 0x4000,
	ABL_ARTILLERY = 0x8000,
	ABL_CLEAN_REACTOR = 0x10000,
	ABL_BLINK_DISPLACER = 0x20000,
	ABL_TRANCE = 0x40000,
	ABL_HEAVY_TRANSPORT = 0x80000,
	ABL_NERVE_GAS = 0x100000,
	ABL_REPAIR = 0x200000,
	ABL_POLICE_2X = 0x400000,
	ABL_SLOW = 0x800000,
	ABL_SOPORIFIC_GAS = 0x1000000,
	ABL_DISSOCIATIVE_WAVE = 0x2000000,
	ABL_MARINE_DETACHMENT = 0x4000000,
	ABL_FUEL_NANOCELLS = 0x8000000,
	ABL_ALGO_ENHANCEMENT = 0x10000000,
};

enum veh_ability_flag_bitfield {
	ALLOWED_LAND_UNIT = 0x1,
	ALLOWED_SEA_UNIT = 0x2,
	ALLOWED_AIR_UNIT = 0x4,
	ALLOWED_COMBAT_UNIT = 0x8,
	ALLOWED_TERRAFORM_UNIT = 0x10,
	ALLOWED_NONCOMBAT_UNIT = 0x20,
	NOT_ALLOWED_PROBE_TEAM = 0x40,
	NOT_ALLOWED_PSI_UNIT = 0x80,
	TRANSPORT_ONLY_UNIT = 0x100,
	NOT_ALLOWED_FAST_UNIT = 0x200,
	COST_INC_LAND_UNIT = 0x400,
	ONLY_PROBE_TEAM = 0x800,
};

enum veh_prototype_flag_bitfield {
	PROTO_ACTIVE = 0x1, // if this bit is zero, prototype has been retired
};

enum veh_orders {
	ORDER_NONE = 0,              //  -
	ORDER_SENTRY_BOARD = 1,      // (L)
	ORDER_HOLD = 2,              // (H); Hold (set 1st waypoint (-1, 0)), Hold 10 (-1, 10), On Alert
	ORDER_CONVOY = 3,            // (O)
	ORDER_FARM = 4,              // (f)
	ORDER_SOIL_ENRICHER = 5,     // (f)
	ORDER_MINE = 6,              // (M)
	ORDER_SOLAR_COLLECTOR = 7,   // (S)
	ORDER_PLANT_FOREST = 8,      // (F)
	ORDER_ROAD = 9,              // (R)
	ORDER_MAGTUBE = 10,          // (R)
	ORDER_BUNKER = 11,           // (K)
	ORDER_AIRBASE = 12,          // (.)
	ORDER_SENSOR_ARRAY = 13,     // (O)
	ORDER_REMOVE_FUNGUS = 14,    // (F)
	ORDER_PLANT_FUNGUS = 15,     // (F)
	ORDER_CONDENSER = 16,        // (N)
	ORDER_ECHELON_MIRROR = 17,   // (E)
	ORDER_THERMAL_BOREHOLE = 18, // (B)
	ORDER_DRILL_AQUIFIER = 19,   // (Q)
	ORDER_TERRAFORM_UP = 20,     // (])
	ORDER_TERRAFORM_DOWN = 21,   // ([)
	ORDER_TERRAFORM_LEVEL = 22,  // (_)
	ORDER_PLACE_MONOLITH = 23,   // (?)
	ORDER_GO_TO = 24,            // (G); Go to Base, Group go to, Patrol
	ORDER_MOVE = 25,             // (>); Only used in a few places, seems to be buggy mechanic
	ORDER_EXPLORE = 26,          // (/); not set via shortcut, AI related?
	ORDER_ROADS_TO = 27,         // (r)
	ORDER_MAGTUBE_TO = 28,       // (t)
	// 29                        // max value, seems to be unused
	ORDER_AI_GO_TO = 88,         //  - ; ORDER_GO_TO (0x18) | 0x40 > 0x58 ? only used by AI funcs
};

enum veh_orders_auto_type {
	ORDERA_CONVOY_NUTRIENTS = 0,
	ORDERA_TERRA_AUTO_FULL = 0,
	ORDERA_CONVOY_MINERALS = 1,
	ORDERA_TERRA_AUTO_ROAD = 1,
	ORDERA_CONVOY_ENERGY = 2,
	ORDERA_TERRA_AUTO_MAGTUBE = 2,
	ORDERA_CONVOY_PSI = 3,
	ORDERA_TERRA_AUTOIMPROVE_BASE = 3,
	ORDERA_TERRA_FARM_SOLAR_ROAD = 4,
	ORDERA_TERRA_MINE_SOLAR_ROAD = 5,
	ORDERA_TERRA_AUTO_FUNGUS_REM = 6,
	ORDERA_TERRA_AUTOMATIC_SENSOR = 7,
	// 8 unused?
	// 9 unused?
	ORDERA_BOMBING_RUN = 10, // air units only
	ORDERA_ON_ALERT = 11,
	ORDERA_AUTOMATE_AIR_DEFENSE = 12,
};

enum veh_flags_bitfield {
	VFLAG_PROBE_PACT_OPERATIONS = 0x4,
	VFLAG_IS_OBJECTIVE = 0x20,
	VFLAG_LURKER = 0x40,
	VFLAG_START_RAND_LOCATION = 0x80,
	VFLAG_START_RAND_MONOLITH = 0x100,
	VFLAG_START_RAND_FUNGUS = 0x200,
	VFLAG_INVISIBLE = 0x400,
};

enum veh_weapon_mode {
	WPN_MODE_PROJECTILE = 0,
	WPN_MODE_ENERGY = 1,
	WPN_MODE_MISSILE = 2,
	WPN_MODE_TRANSPORT = 7,
	WPN_MODE_COLONIST = 8,
	WPN_MODE_TERRAFORMER = 9,
	WPN_MODE_CONVOY = 10,
	WPN_MODE_INFOWAR = 11,
	WPN_MODE_ARTIFACT = 12,
};

enum veh_plan {
	PLAN_OFFENSIVE = 0,
	PLAN_COMBAT = 1,
	PLAN_DEFENSIVE = 2,
	PLAN_RECONNAISANCE = 3,
	PLAN_AIR_SUPERIORITY = 4,
	PLAN_PLANET_BUSTER = 5,
	PLAN_NAVAL_SUPERIORITY = 6,
	PLAN_NAVAL_TRANSPORT = 7,
	PLAN_COLONIZATION = 8,
	PLAN_TERRAFORMING = 9,
	PLAN_SUPPLY_CONVOY = 10,
	PLAN_INFO_WARFARE = 11,
	PLAN_ALIEN_ARTIFACT = 12,
	PLAN_TECTONIC_MISSILE = 13,
	PLAN_FUNGAL_MISSILE = 14,
};

enum veh_weapon_id {
	WPN_HAND_WEAPONS = 0,
	WPN_LASER = 1,
	WPN_PARTICLE_IMPACTOR = 2,
	WPN_GATLING_LASER = 3,
	WPN_MISSILE_LAUNCHER = 4,
	WPN_CHAOS_GUN = 5,
	WPN_FUSION_LASER = 6,
	WPN_TACHYON_BOLT = 7,
	WPN_PLASMA_SHARD = 8,
	WPN_QUANTUM_LASER = 9,
	WPN_GRAVITON_GUN = 10,
	WPN_SINGULARITY_LASER = 11,
	WPN_RESONANCE_LASER = 12,
	WPN_RESONANCE_BOLT = 13,
	WPN_STRING_DISRUPTOR = 14,
	WPN_PSI_ATTACK = 15,
	WPN_PLANET_BUSTER = 16,
	WPN_COLONY_MODULE = 17,
	WPN_TERRAFORMING_UNIT = 18,
	WPN_TROOP_TRANSPORT = 19,
	WPN_SUPPLY_TRANSPORT = 20,
	WPN_PROBE_TEAM = 21,
	WPN_ALIEN_ARTIFACT = 22,
	WPN_CONVENTIONAL_PAYLOAD = 23,
	WPN_TECTONIC_PAYLOAD = 24,
	WPN_FUNGAL_PAYLOAD = 25,
};

enum veh_armor_id {
	ARM_NO_ARMOR = 0,
	ARM_SYNTHMETAL_ARMOR = 1,
	ARM_PLASMA_STEEL_ARMOR = 2,
	ARM_SILKSTEEL_ARMOR = 3,
	ARM_PHOTON_WALL = 4,
	ARM_PROBABILITY_SHEATH = 5,
	ARM_NEUTRONIUM_ARMOR = 6,
	ARM_ANTIMATTER_PLATE = 7,
	ARM_STASIS_GENERATOR = 8,
	ARM_PSI_DEFENSE = 9,
	ARM_PULSE_3_ARMOR = 10,
	ARM_RESONANCE_3_ARMOR = 11,
	ARM_PULSE_8_ARMOR = 12,
	ARM_RESONANCE_8_ARMOR = 13,
};

enum veh_chassis_type {
	CHSI_INFANTRY = 0,
	CHSI_SPEEDER = 1,
	CHSI_HOVERTANK = 2,
	CHSI_FOIL = 3,
	CHSI_CRUISER = 4,
	CHSI_NEEDLEJET = 5,
	CHSI_COPTER = 6,
	CHSI_GRAVSHIP = 7,
	CHSI_MISSILE = 8,
};

struct rules_chassis {
	LPSTR offsv1Name;
	LPSTR offsv2Name;
	LPSTR offsvNameLrg;
	LPSTR defsv1Name;
	LPSTR defsv2Name;
	LPSTR defsvNameLrg;
	int offsv1Gender;
	int offsv2Gender;
	int offsvGenderLrg;
	int defsv1Gender;
	int defsv2Gender;
	int defsvGenderLrg;
	BOOL offsv1IsPlural;
	BOOL offsv2IsPlural;
	BOOL offsvIsPluralLrg;
	BOOL defsv1IsPlural;
	BOOL defsv2IsPlural;
	BOOL defsvIsPluralLrg;
	BYTE speed;
	BYTE triad;
	BYTE range;
	BYTE cargo;
	BYTE cost; // expand to DWORD eventually
	BYTE missile;
	BYTE spriteFlagXOff[8];
	BYTE spriteFlagYOff[8];
	BYTE spriteUnk1XOff[8];
	BYTE spriteUnk1YOff[8];
	BYTE spriteUnk2XOff[8];
	BYTE spriteUnk2YOff[8];
	BYTE spriteUnk3XOff[8];
	BYTE spriteUnk3YOff[8];
	__int16 preqTech;
};

struct rules_weapon {
	LPSTR name;
	LPSTR nameShort;
	__int8 offenseRating;
	__int8 icon;
	BYTE mode;
	BYTE cost;
	__int16 preqTech;
	__int16 padding;
};

struct rules_armor {
	LPSTR name;
	LPSTR nameShort;
	__int8 defenseRating;
	BYTE mode;
	BYTE cost;
	BYTE padding1;
	__int16 preqTech;
	__int16 padding2;
};

struct veh {
	__int16 xCoord;
	__int16 yCoord;
	DWORD state;
	WORD flags;
	__int16 protoID;
	WORD unknown1;
	BYTE factionID;
	BYTE yearEndLurking;
	BYTE dmgIncurred;
	char orders; // see veh_orders enum
	BYTE waypointCount;
	BYTE patrolCurrentPoint;
	__int16 waypoint_xCoord[4];
	__int16 waypoint_yCoord[4];
	BYTE morale;
	BYTE terraformingTurns;
	BYTE orderAutoType; // see veh_orders_auto_type enum
	BYTE visibleToFaction; // bitfield (1 << (1 to 7))
	BYTE movesExpended;
	char unknown5;
	BYTE unknown6;
	BYTE unknown7;
	BYTE unknown8;
	char unknown9;
	__int16 homeBaseID;
	__int16 nextVehIDStack;
	__int16 prevVehIDStack;
};

struct veh_prototype {
	char vehName[32];
	DWORD abilityFlags;
	BYTE chassisID;
	BYTE weaponID;
	BYTE armorID;
	BYTE reactorID;
	BYTE carryCapacity;
	BYTE cost;
	BYTE plan;
	char unk1; // some kind of internal prototype category?
	char unk2; // factions that have created prototype?
	char unk3; // which faction "knows" about unit prototype? seemed to only be used by battle_fight
			   // to set it after initial value in make_proto()
	char iconOffset;
	char padding; // unused
	WORD flags;
	__int16 preqTech; // only set by read_units() for predefined units
};

struct rules_reactor {
	LPSTR name;
	LPSTR nameShort;
	__int16 preqTech;
	WORD power; // Bug fix: this value isn't originally set
};

struct rules_ability {
	LPSTR name;
	LPSTR description;
	LPSTR abbreviation;
	int costFactor;
	int unkVal; // only referenced in NetDaemon::synch?
	DWORD flags;
	__int16 preqTech;
	WORD padding;
};

struct rules_morale {
	LPSTR name;
	LPSTR nameNative;
};

struct rules_combat_mode {
	LPSTR name;
	LPSTR hyphened;
	LPSTR abbrev;
	LPSTR letter;
};

struct rules_order {
	LPSTR order;
	LPSTR orderSea;
	LPSTR letter;
};

constexpr int MaxChassisNum = 9;
constexpr int MaxWeaponNum = 26;
constexpr int MaxArmorNum = 14;
constexpr int MaxVehProtoNum = 512;
constexpr int MaxVehProtoFactionNum = 64; // Limit per faction + max predefined from alpha/x.txt
constexpr int MaxReactorNum = 4;
constexpr int MaxAbilityNum = 29;
constexpr int MaxMoraleNum = 7;
constexpr int MaxDefenseModeNum = 3;
constexpr int MaxOffenseModeNum = 3;
constexpr int MaxOrderNum = 30;
constexpr int MaxPlanNum = 15;
constexpr int MaxTriadNum = 3;

extern veh_prototype *VehPrototype;
extern veh *Veh;
extern rules_chassis *Chassis;
extern rules_weapon *Weapon;
extern rules_armor *Armor;
extern rules_reactor *Reactor;
extern rules_ability *Ability;
extern rules_morale *Morale;
extern rules_combat_mode *DefenseModes;
extern rules_combat_mode *OffenseModes;
extern rules_order *Order;
extern LPSTR *PlansShortName;
extern LPSTR *PlansFullName;
extern LPSTR *Triad;

OPENSMACX_API int __cdecl veh_top(int vehID);
OPENSMACX_API DWORD __cdecl drop_range(int factionID);
OPENSMACX_API int __cdecl psi_factor(int combatRatio, int factionID, BOOL isAttack, 
	BOOL isFungalTower);
OPENSMACX_API DWORD __cdecl veh_moves(int vehID);
OPENSMACX_API DWORD __cdecl proto_power(int vehID);
OPENSMACX_API int __cdecl veh_lift(int vehID);
OPENSMACX_API void __cdecl sleep(int vehID);
OPENSMACX_API void __cdecl veh_clear(int vehID, int protoID, int factionID);
OPENSMACX_API BOOL __cdecl can_arty(int protoID, BOOL seaTriadRetn);
OPENSMACX_API DWORD __cdecl offense_proto(int protoID, int vehIDDef, BOOL isArtyMissile);
OPENSMACX_API DWORD __cdecl armor_proto(int protoID, int vehIDAtk, BOOL isArtyMissile);
OPENSMACX_API DWORD __cdecl speed_proto(int protoID);
OPENSMACX_API DWORD __cdecl speed(int vehID, BOOL toggle);
OPENSMACX_API DWORD __cdecl veh_cargo(int vehID);
OPENSMACX_API void __cdecl veh_skip(int vehID);
OPENSMACX_API int __cdecl veh_fake(int protoID, int factionID);
OPENSMACX_API int __cdecl veh_wake(int vehID);
OPENSMACX_API BOOL __cdecl has_abil(int protoID, int abilityID);
OPENSMACX_API void __cdecl say_stats_3(LPSTR stat, int protoID);
OPENSMACX_API void __cdecl say_stats_3(int protoID);
OPENSMACX_API void __cdecl say_stats_2(LPSTR stat, int protoID);
OPENSMACX_API void __cdecl say_stats(LPSTR output, int protoID, LPSTR customSpacer);
OPENSMACX_API int __cdecl arm_strat(int armorID, int factionID);
OPENSMACX_API int __cdecl weap_strat(int weaponID, int factionID);
OPENSMACX_API int __cdecl weap_val(int protoID, int factionID);
OPENSMACX_API int __cdecl arm_val(int armorID, int factionID);
OPENSMACX_API int __cdecl armor_val(int protoID, int factionID);
OPENSMACX_API DWORD __cdecl transport_val(DWORD chassisID, int ability, DWORD reactorID);
OPENSMACX_API DWORD __cdecl proto_cost(DWORD chassisID, DWORD weaponID, DWORD armorID,
	DWORD ability, DWORD reactorID);
OPENSMACX_API DWORD __cdecl base_cost(int protoID);
OPENSMACX_API void __cdecl make_proto(int protoID, DWORD chassisID, DWORD weaponID, DWORD armorID,
	DWORD ability, DWORD reactorID);
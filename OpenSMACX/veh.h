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
enum veh_basic_id {
	BSC_COLONY_POD = 0x0,
	BSC_FORMERS = 0x1,
	BSC_SCOUT_PATROL = 0x2,
	BSC_TRANSPORT_FOIL = 0x3,
	BSC_SEA_FORMERS = 0x4,
	BSC_SUPPLY_CRAWLER = 0x5,
	BSC_PROBE_TEAM = 0x6,
	BSC_ALIEN_ARTIFACT = 0x7,
	BSC_MIND_WORMS = 0x8,
	BSC_ISLE_OF_THE_DEEP = 0x9,
	BSC_LOCUSTS_OF_CHIRON = 0xA,
	BSC_UNITY_ROVER = 0xB,
	BSC_UNITY_SCOUT_CHOPPER = 0xC,
	BSC_UNITY_FOIL = 0xD,
	BSC_SEALURK = 0xE,
	BSC_SPORE_LAUNCHER = 0xF,
	BSC_BATTLE_OGRE_MK1 = 0x10,
	BSC_BATTLE_OGRE_MK2 = 0x11,
	BSC_BATTLE_OGRE_MK3 = 0x12,
	BSC_FUNGAL_TOWER = 0x13,
	BSC_UNITY_MINING_LASER = 0x14,
	BSC_SEA_ESCAPE_POD = 0x15,
	BSC_UNITY_GUNSHIP = 0x16,
};

enum veh_triad {
	TRIAD_LAND = 0x0,
	TRIAD_SEA = 0x1,
	TRIAD_AIR = 0x2,
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

enum veh_weapon_mode {
	WPN_MODE_Projectile = 0x0,
	WPN_MODE_Energy = 0x1,
	WPN_MODE_Missile = 0x2,
	WPN_MODE_Transport = 0x7,
	WPN_MODE_Colonist = 0x8,
	WPN_MODE_Terraformer = 0x9,
	WPN_MODE_Convoy = 0xA,
	WPN_MODE_InfoWar = 0xB,
	WPN_MODE_Artifact = 0xC,
};

enum veh_plan {
	PLAN_OFFENSIVE = 0x0,
	PLAN_COMBAT = 0x1,
	PLAN_DEFENSIVE = 0x2,
	PLAN_RECONNAISANCE = 0x3,
	PLAN_AIR_SUPERIORITY = 0x4,
	PLAN_PLANET_BUSTER = 0x5,
	PLAN_NAVAL_SUPERIORITY = 0x6,
	PLAN_NAVAL_TRANSPORT = 0x7,
	PLAN_COLONIZATION = 0x8,
	PLAN_TERRAFORMING = 0x9,
	PLAN_SUPPLY_CONVOY = 0xA,
	PLAN_INFO_WARFARE = 0xB,
	PLAN_ALIEN_ARTIFACT = 0xC,
	PLAN_TECTONIC_MISSILE = 0xD,
	PLAN_FUNGAL_MISSILE = 0xE,
	PLAN_AUTO_CALCULATE = 0xFFFFFFFF,
};

enum veh_weapon_id {
	WPN_Hand_Weapons = 0x0,
	WPN_Laser = 0x1,
	WPN_Particle_Impactor = 0x2,
	WPN_Gatling_Laser = 0x3,
	WPN_Missile_Launcher = 0x4,
	WPN_Chaos_Gun = 0x5,
	WPN_Fusion_Laser = 0x6,
	WPN_Tachyon_Bolt = 0x7,
	WPN_Plasma_Shard = 0x8,
	WPN_Quantum_Laser = 0x9,
	WPN_Graviton_Gun = 0xA,
	WPN_Singularity_Laser = 0xB,
	WPN_Resonance_Laser = 0xC,
	WPN_Resonance_Bolt = 0xD,
	WPN_String_Disruptor = 0xE,
	WPN_Psi_Attack = 0xF,
	WPN_Planet_Buster = 0x10,
	WPN_Colony_Module = 0x11,
	WPN_Terraforming_Unit = 0x12,
	WPN_Troop_Transport = 0x13,
	WPN_Supply_Transport = 0x14,
	WPN_Probe_Team = 0x15,
	WPN_Alien_Artifact = 0x16,
	WPN_Conventional_Payload = 0x17,
	WPN_Tectonic_Payload = 0x18,
	WPN_Fungal_Payload = 0x19,
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

struct veh_prototype {
	char vehName[32];
	int abilityFlags;
	char chassisType;
	char weaponType;
	char armorType;
	char reactorType;
	BYTE carryCapacity;
	BYTE cost;
	char plan;
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

OPENSMACX_API BOOL __cdecl has_abil(int protoID, int abilityID);
OPENSMACX_API DWORD __cdecl transport_val(int chassisID, int ability, int reactorID);
OPENSMACX_API DWORD __cdecl proto_cost(int chassisType, int weapType, 
													int armorType, int ability, int reactorType);
OPENSMACX_API DWORD __cdecl base_cost(int protoID);
OPENSMACX_API void __cdecl make_proto(int protoID, int chassisType, int weapType, 
													int armorType, int ability, int reactorType);
OPENSMACX_API DWORD __cdecl speed_proto(int protoID);
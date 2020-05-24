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
  * Technology related objects, variables and functions.
  */
enum technology_flag_bitfield {
	TFLAG_SECRETS = 0x1,
	TFLAG_IMPROVED_PROBES = 0x2,
	TFLAG_INC_COMMERCE = 0x4,
	TFLAG_REVEALS_MAP = 0x8,
	TFLAG_ALLOW_GENE_WARFARE = 0x10,
	TFLAG_INC_GENE_WARFARE_DEFENSE = 0x20,
	TFLAG_INC_ENERGY_FUNGUS = 0x40,
	TFLAG_INC_MINERALS_FUNGUS = 0x80,
	TFLAG_INC_NUTRIENT_FUNGUS = 0x100,
};

enum technology {
	TECH_BIOGEN = 0,
	TECH_INDUST = 1,
	TECH_INFNET = 2,
	TECH_PHYSIC = 3,
	TECH_PSYCH = 4,
	TECH_MOBILE = 5,
	TECH_ECOLOGY = 6,
	TECH_SUPER = 7,
	TECH_CHAOS = 8,
	TECH_E_MC2 = 9,
	TECH_FUSION = 10,
	TECH_ALLOYS = 11,
	TECH_SUBAT = 12,
	TECH_CHEMIST = 13,
	TECH_SURFACE = 14,
	TECH_METAL = 15,
	TECH_STRING = 16,
	TECH_MILALG = 17,
	TECH_MAGNETS = 18,
	TECH_MATCOMP = 19,
	TECH_UNIFIED = 20,
	TECH_GRAVITY = 21,
	TECH_POLY = 22,
	TECH_AGRAV = 23,
	TECH_DELETE_1 = 24,
	TECH_QUANTUM = 25,
	TECH_SINGMEC = 26,
	TECH_CONSING = 27,
	TECH_TEMPMEC = 28,
	TECH_PROBMEC = 29,
	TECH_ALGOR = 30,
	TECH_SOLIDS = 31,
	TECH_PLANETS = 32,
	TECH_DIGSENT = 33,
	TECH_HAL9000 = 34,
	TECH_DOCINIT = 35,
	TECH_DOCFLEX = 36,
	TECH_INTEG = 37,
	TECH_FOSSIL = 38,
	TECH_DOCAIR = 39,
	TECH_DOCSEC = 40,
	TECH_MINDMAC = 41,
	TECH_NANOMIN = 42,
	TECH_DOCLOY = 43,
	TECH_ETHCALC = 44,
	TECH_INDECON = 45,
	TECH_INDAUTO = 46,
	TECH_CENTMED = 47,
	TECH_BRAIN = 48,
	TECH_GENE = 49,
	TECH_BIOENG = 50,
	TECH_BIOMAC = 51,
	TECH_NEURAL = 52,
	TECH_CYBER = 53,
	TECH_EUDAIM = 54,
	TECH_WILLPOW = 55,
	TECH_THRESH = 56,
	TECH_MATTER = 57,
	TECH_CENTEMP = 58,
	TECH_ENVECON = 59,
	TECH_ECOENG = 60,
	TECH_PLAECON = 61,
	TECH_ECOENG2 = 62,
	TECH_CENTPSI = 63,
	TECH_ALPHCEN = 64,
	TECH_CREATE = 65,
	TECH_SPACE = 66,
	TECH_HOMOSUP = 67,
	TECH_SUPLUBE = 68,
	TECH_QUANMAC = 69,
	TECH_DELETED_2 = 70,
	TECH_NANEDIT = 71,
	TECH_OPTCOMP = 72,
	TECH_INDROB = 73,
	TECH_CENTGEN = 74,
	TECH_SENTECO = 75,
	TECH_VIRAL = 76,
	TECH_ORBITAL = 77,
	TECH_PRPSYCH = 78,
	TECH_FLDMOD = 79,
	TECH_ADAPDOC = 80,
	TECH_ADAPECO = 81,
	TECH_BIOADAP = 82,
	TECH_SENTRES = 83,
	TECH_SECMANI = 84,
	TECH_NEWMISS = 85,
	TECH_BFG9000 = 86,
	TECH_USER = 87,
	TECH_TRANT = 88,
};

struct rules_technology {
	uint32_t flags;
	LPSTR name;
	char id[8]; // short name up to 7 characters in length
	int padding; // unused
	int growthValue;
	int techValue;
	int wealthValue;
	int powerValue;
	int preqTech1;
	int preqTech2;
};

struct rules_mandate {
	LPSTR name;
	LPSTR nameCAPS;
};

constexpr int MaxTechnologyNum = 89;
constexpr int TechNone = -1;
constexpr int TechDisabled = -2;
constexpr int MaxMandateNum = 4;

extern rules_technology *Technology; // [89]
extern uint8_t *GameTechDiscovered; // [89]
extern rules_mandate *Mandate; // [4]
extern int *TechValidCount;
extern int *TechCommerceCount;
extern char TechName[80];

OPENSMACX_API void __cdecl say_tech(int techID, BOOL categoryLvl);
OPENSMACX_API BOOL __cdecl valid_tech_leap(uint32_t techID, int factionID);
OPENSMACX_API void __cdecl say_tech(LPSTR output, int techID, BOOL categoryLvl);
OPENSMACX_API LPSTR __cdecl tech_name(int techID, BOOL categoryLvl);
OPENSMACX_API BOOL __cdecl has_tech(int techID, int factionID);
OPENSMACX_API int __cdecl tech_recurse(int techID, int baseLevel);
OPENSMACX_API int __cdecl tech_category(int techID);
OPENSMACX_API BOOL __cdecl tech_avail(int techID, int factionID);
OPENSMACX_API void __cdecl tech_effects(int factionID);
OPENSMACX_API BOOL __cdecl tech_is_preq(int preTechID, int techID, uint32_t range);
OPENSMACX_API int __cdecl tech_val(int techID, int factionID, BOOL simpleCalc);
OPENSMACX_API int __cdecl tech_ai(int factionID);
OPENSMACX_API int __cdecl tech_mil(int techID);
OPENSMACX_API int __cdecl tech_tech(int techID);
OPENSMACX_API int __cdecl tech_infra(int techID);
OPENSMACX_API int __cdecl tech_colonize(int techID);
OPENSMACX_API uint32_t __cdecl tech_rate(uint32_t factionID);
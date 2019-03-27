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
  * Technology related objects, variables and functions.
  */
enum technology_flags {
	SECRETS = 0x1,
	IMPROVED_PROBES = 0x2,
	INC_COMMERCE = 0x4,
	REVEALS_MAP = 0x8,
	ALLOW_GENE_WARFARE = 0x10,
	INC_GENE_WARFARE_DEFENSE = 0x20,
	INC_FUNGUS_ENERGY = 0x40,
	INC_FUNGUS_MINERALS = 0x80,
	INC_FUNGUS_NUTRIENT = 0x100,
};

struct rules_technology {
	DWORD flags;
	LPSTR name;
	CHAR id[8];
	int padding; // unused value
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
extern BYTE *GameTechDiscovered; // [89]
extern rules_mandate *Mandate; // [4]

OPENSMACX_API int __cdecl tech_tech(int techID);
OPENSMACX_API int __cdecl tech_mil(int techID);
OPENSMACX_API int __cdecl tech_infra(int techID);
OPENSMACX_API int __cdecl tech_colonize(int techID);
OPENSMACX_API BOOL __cdecl has_tech(int techID, int factionID);
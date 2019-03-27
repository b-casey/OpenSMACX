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
#include "sprite.h"
#include "stdafx.h"

 /*
  * Faction related objects, variables and functions.
  */
enum player_gender {
	MALE_GENDER = 0x0,
	FEMALE_GENDER = 0x1,
	NEUTRAL_GENDER = 0x2,
};

enum faction_special_rules {
	RULE_TECH = 0x0,
	RULE_UNIT = 0x1,
	RULE_FACILITY = 0x2,
	RULE_SOCIAL = 0x3,
	RULE_IMMUNITY = 0x4,
	RULE_IMPUNITY = 0x5,
	RULE_PENALTY = 0x6,
	RULE_FUNGNUTRIENT = 0x7,
	RULE_FUNGMINERALS = 0x8,
	RULE_FUNGENERGY = 0x9,
	RULE_ROBUST = 0xA,
	RULE_VOTES = 0xB,
	RULE_FREEFAC = 0xC,
	RULE_REVOLT = 0xD,
	RULE_NODRONE = 0xE,
	RULE_FREEABIL = 0xF,
	RULE_PROBECOST = 0x10,
	RULE_DEFENSE = 0x11,
	RULE_OFFSENSE = 0x12,
};

enum player_rule_flags {
	FLAG_TECHSTEAL = 0x10,
	FLAG_TECHSHARE = 0x20,
	FLAG_WORMPOLICE = 0x40,
	FLAG_ALIEN = 0x80,
	FLAG_AQUATIC = 0x100,
	FLAG_FREEPROTO = 0x200,
	FLAG_FANATIC = 0x400,
	FLAG_MINDCONTROL = 0x800,
	FLAG_COMMFREQ = 0x1000,
	FLAG_TERRAFORM = 0x2000,
	FLAG_INTEREST = 0x4000,
	FLAG_MORALE = 0x8000,
};

struct rules_social_effect {
	char set1[24];
	char set2[24];
	char padding[56];
};

struct rules_social_category_effect {
	int economy;
	int effic;
	int support;
	int talent;
	int morale;
	int police;
	int growth;
	int planet;
	int probe;
	int industry;
	int research;
};

struct rules_social_category {
	LPSTR type;
	int preqTech[4];
	LPSTR name[4];
	rules_social_category_effect effect[4];
};

struct rules_might {
	LPSTR adjStart;
	LPSTR adj;
};

struct rules_proposal {
	LPSTR name;
	LPSTR description;
	int preqTech;
};

struct rules_bonusname {
	char key[24];
};

struct Player {
	BOOL isLeaderFemale;
	CHAR filename[24];
	CHAR searchKey[24];
	CHAR nameLeader[24];
	CHAR titleLeader[24];
	CHAR adjLeader[128];
	CHAR adjInsultLeader[128];
	CHAR adjFaction[128];
	CHAR adjInsultFaction[128];
	BYTE padding1[128];
	CHAR nounFaction[24];
	int nounGender;
	BOOL isNounPlural;
	CHAR nameAdjFaction[128]; // drops 2nd entry on line (abbreviation value?)
	CHAR nameFaction[40];
	CHAR insultLeader[24];
	CHAR descNameFaction[24];
	CHAR assistantName[24];
	CHAR scientistName[24];
	CHAR assistantCity[24];
	BYTE padding2[176];
	int ruleTechSelected;
	int ruleMorale;
	int ruleResearch;
	int ruleDrone;
	int ruleTalent;
	int ruleEnergy;
	int ruleInterest;
	int rulePopulation;
	int ruleHurry;
	int ruleTechcost;
	int rulePsi;
	int ruleSharetech;
	int ruleCommerce;
	int ruleFlags;
	int factionBonusCount;
	int factionBonusID[8];
	int factionBonusVal1[8];
	int factionBonusVal2[8];
	int AI_IntrestFight;
	BOOL AI_IntrestGrowth;
	BOOL AI_IntrestTech;
	BOOL AI_IntrestWealth;
	BOOL AI_IntrestPower;
	int socIdeologyCategory;
	int socAntiIdeologyCategory;
	int socIdeologyModel;
	int socAntiIdeologyModel;
	int socIdeologyEffect; // Emphasis
	int socAntiIdeologyEffect; // unused, never set in default factions
};

struct faction_art {
	Sprite base[6][4];
	Sprite councilLogo[2];
	Sprite councilSmall;
	Sprite councilBig;
	Sprite diplomacy;
	Sprite diplomacyLogo;
	Sprite diplomacyLandscape;
	Sprite datalink;
	Sprite reportLogoSmall[3];
	Sprite reportLogo[2];
};

constexpr int MaxSocialCatNum = 4;
constexpr int MaxSocialModelNum = 4;
constexpr int MaxSocialEffectNum = 11;
constexpr int MaxMoodNum = 9;
constexpr int MaxReputeNum = 8;
constexpr int MaxMightNum = 7;
constexpr int MaxProposalNum = 11;
constexpr int MaxBonusNameNum = 41;
constexpr int MaxPlayerNum = 8;

extern rules_social_category *SocialCategory;
extern rules_social_effect *SocialEffect;
extern LPSTR *Mood;
extern LPSTR *Repute;
extern rules_might *Might;
extern rules_proposal *Proposal;
extern rules_bonusname *BonusName;
extern Player *Players; // Players[0] is AI native life faction
extern faction_art *FactionArt;
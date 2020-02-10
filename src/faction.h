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
	MALE_GENDER = 0,
	FEMALE_GENDER = 1,
	NEUTRAL_GENDER = 2,
};

enum faction_special_rules {
	RULE_TECH = 0,
	RULE_UNIT = 1,
	RULE_FACILITY = 2,
	RULE_SOCIAL = 3,
	RULE_IMMUNITY = 4,
	RULE_IMPUNITY = 5,
	RULE_PENALTY = 6,
	RULE_FUNGNUTRIENT = 7,
	RULE_FUNGMINERALS = 8,
	RULE_FUNGENERGY = 9,
	RULE_ROBUST = 10,
	RULE_VOTES = 11,
	RULE_FREEFAC = 12,
	RULE_REVOLT = 13,
	RULE_NODRONE = 14,
	RULE_FREEABIL = 15,
	RULE_PROBECOST = 16,
	RULE_DEFENSE = 17,
	RULE_OFFSENSE = 18,
};

enum player_rule_flag_bitfield {
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

enum diplomacy_state_bitfield {
	DSTATE_PACT = 0x1,
	DSTATE_TREATY = 0x2,
	DSTATE_TRUCE = 0x4,
	DSTATE_COMMLINK = 0x8,
	DSTATE_VENDETTA = 0x10,
	DSTATE_WANT_REVENGE = 0x20,
	DSTATE_UNK_0x40 = 0x40,
	DSTATE_UNK_0x80 = 0x80,
	DSTATE_UNK_0x100 = 0x100,
	DSTATE_UNK_0x200 = 0x200,
	DSTATE_SHALL_BETRAY = 0x400,
	DSTATE_UNK_0x800 = 0x800,
	DSTATE_HAVE_INFILTRATOR = 0x1000,
	DSTATE_WANT_TO_TALK = 0x2000,
	DSTATE_UNK_0x4000 = 0x4000,
	DSTATE_UNK_0x8000 = 0x8000,
	DSTATE_UNK_0x10000 = 0x10000,
	DSTATE_UNK_0x20000 = 0x20000,
	DSTATE_ATROCITY_VICTIM = 0x40000,
	DSTATE_UNK_0x80000 = 0x80000,
	DSTATE_UNK_0x100000 = 0x100000,
	DSTATE_UNK_0x200000 = 0x200000,
	DSTATE_UNK_0x400000 = 0x400000,
	DSTATE_UNK_0x800000 = 0x800000,
	DSTATE_UNK_0x1000000 = 0x1000000,
	DSTATE_HAVE_SURRENDERED = 0x2000000,
	DSTATE_UNK_0x4000000 = 0x4000000,
	DSTATE_UNK_0x8000000 = 0x8000000,
	DSTATE_UNK_0x10000000 = 0x10000000,
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

struct goal {
	int16_t type;
	int16_t unk1;
	int xCoord;
	int yCoord;
	int baseID;
};

struct player {
	BOOL isLeaderFemale;
	char filename[24];
	char searchKey[24];
	char nameLeader[24];
	char titleLeader[24];
	char adjLeader[128];
	char adjInsultLeader[128];
	char adjFaction[128];
	char adjInsultFaction[128];
	uint8_t padding1[128];
	char nounFaction[24];
	int nounGender;
	BOOL isNounPlural;
	char nameAdjFaction[128]; // drops 2nd entry on line (abbreviation value?)
	char nameFaction[40];
	char insultLeader[24];
	char descNameFaction[24];
	char assistantName[24];
	char scientistName[24];
	char assistantCity[24];
	uint8_t padding2[176];
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

struct player_data {
	int diploFlags;
	int ranking;
	int diffLevel;
	uint32_t baseNameOffset;
	uint32_t baseSeaNameOffset;
	int tutorialMoreBases;
	int diploStatus[8];
	int diploAgenda[8];
	int diploFriction_1[8];
	int diploTurnCheck[8];
	int diploTreatyTurns[8];
	char diploFriction_2[8];
	int sanctionTurns;
	int loanYears[8];
	int loanPayment[8];
	char gap_104[32];
	int minorAtrocities;
	int globalReputation;
	int diploUnk1[8];
	int diploUnk2[8];
	int diploBackstabs[8];
	int diploUnk3[8];
	int diploUnk4[8];
	int mapTradeDone;
	int governorDefFlags;
	int unk_2;
	int majorAtrocities;
	int unk_3;
	int unk_4[8];
	int unk_5[8];
	int energyCredits;
	int energyCost;
	int SE_PoliticsPending;
	int SE_EconomicsPending;
	int SE_ValuesPending;
	int SE_FuturePending;
	int SE_Politics;
	int SE_Economics;
	int SE_Values;
	int SE_Future;
	int SE_UpheavalCostPaid;
	int SE_EconomyPending;
	int SE_EffiencyPending;
	int SE_SupportPending;
	int SE_TalentPending;
	int SE_MoralePending;
	int SE_PolicePending;
	int SE_GrowthPending;
	int SE_PlanetPending;
	int SE_ProbePending;
	int SE_IndustryPending;
	int SE_ResearchPending;
	int SE_Economy;
	int SE_Effiency;
	int SE_Support;
	int SE_Talent;
	int SE_Morale;
	int SE_Police;
	int SE_Growth;
	int SE_Planet;
	int SE_Probe;
	int SE_Industry;
	int SE_Research;
	int SE_Economy_2;
	int SE_Effiency_2;
	int SE_Support_2;
	int SE_Talent_2;
	int SE_Morale_2;
	int SE_Police_2;
	int SE_Growth_2;
	int SE_Planet_2;
	int SE_Probe_2;
	int SE_Industry_2;
	int SE_Research_2;
	int SE_EconomyBase;
	int SE_EffiencyBase;
	int SE_SupportBase;
	int SE_TalentBase;
	int SE_MoraleBase;
	int SE_PoliceBase;
	int SE_GrowthBase;
	int SE_PlanetBase;
	int SE_ProbeBase;
	int SE_IndustryBase;
	int SE_ResearchBase;
	int unk_13;
	int unk_14;
	int techCommerceBonus;
	int unk_16;
	int unk_17;
	int unk_18;
	int techFungusNutrient;
	int techFungusMineral;
	int techFungusEnergy;
	int unk_22;
	int SE_AllocPsych;
	int SE_AllocLabs;
	int unk_25;
	char gap_330[44];
	int techRanking;
	int unk_26;
	int ODP_Deployed;
	int theoryOfEverything;
	char techTradeSource[92];
	int techAccumulated;
	int techResearchingId;
	int techCost;
	int earnedTechsSaved;
	int netRandomEvent;
	int AI_Fight;
	int AI_Growth;
	int AI_Tech;
	int AI_Wealth;
	int AI_Power;
	int target_xCoord;
	int target_yCoord;
	int unk_28;
	int councilCallTurn;
	int unk_29[11];
	int unk_30[11];
	int unk_31;
	int unk_32;
	char unk_33;
	char unk_34;
	char gap_462[2];
	int unk_35;
	int unk_36;
	int unk_37;
	char gap_470[192];
	int savedQueueSize[8];
	int savedQueueItems[78];
	int unk_38;
	char unk_39[4];
	int unk_40;
	char unk_41[12];
	int unk_42;
	char unk_43[12];
	int unk_44;
	char gap_6B4[80];
	int unk_45;
	char gap_708[200];
	int unk_46;
	char gap_7D4[32];
	int unk_47;
	int unk_48;
	int unk_49;
	int unk_50;
	int labsTotal;
	int satellitesNutrient;
	int satellitesMineral;
	int satellitesEnergy;
	int ODP_Total;
	int bestWeaponValue;
	int unk_55;
	int unk_56;
	int bestArmorValue;
	int unk_58;
	int unk_59; // see enemy_capabilities()
	int unk_60; // see enemy_capabilities()
	int unk_61;
	int unk_62;
	int unk_63;
	int unk_64;
	int unk_65;
	int unk_66;
	int unk_67;
	int unk_68;
	char unk_69[4];
	uint8_t protoID_Active[512];
	uint8_t protoID_Queue[512];
	int16_t protoID_Lost[512];
	int totalMilVeh;
	int currentNumBases;
	int milStrength_1;
	int milStrength_2;
	int popTotal;
	int unk_70;
	int planetBusters;
	int unk_71;
	int unk_72;
	int16_t unk_73[128];
	char unk_74[128];
	char unk_75[128];
	int16_t unk_76[128];
	int16_t unk_77[128];
	int16_t unk_78[128];
	int16_t unk_79[128];
	int16_t unk_80[128];
	int16_t unk_81[128];
	char unk_82[128];
	char unk_83[128];
	char unk_84[128];
	goal goals_1[75];
	goal goals_2[25];
	int unk_93;
	int unk_94;
	int unk_95;
	int unk_96;
	int unk_97;
	int unk_98;
	int unk_99;
	char gap_2058[4];
	int unk_100[8];
	int cornerMarketTurn;
	int cornerMarketActive;
	int unk_101;
	int unk_102;
	int unk_103;
	int unk_104;
	int unk_105;
	int unk_106;
	int unk_107;
	int unk_108;
	int unk_109;
	int unk_110;
	char unk_111[4];
	int unk_112;
	int unk_113;
	int unk_114;
	int unk_115;
	int unk_116;
	int unk_117;
	int unk_118;
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
extern player *Players; // Players[0] is AI native life faction
extern player_data *PlayersData;
extern faction_art *FactionArt;
extern uint8_t *FactionCurrentBitfield;

OPENSMACX_API LPSTR __cdecl get_adjective(int factionID);
OPENSMACX_API LPSTR __cdecl get_noun(int factionID);
OPENSMACX_API BOOL __cdecl society_avail(int socCategory, int socModel, int factionID);
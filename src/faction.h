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
#include "sprite.h"
#include "stdafx.h"

 /*
  * Faction related objects, variables and functions.
  */
enum player_gender {
	GENDER_MALE = 0,
	GENDER_FEMALE = 1,
	GENDER_NEUTRAL = 2,
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
	RFLAG_TECHSTEAL = 0x10,
	RFLAG_TECHSHARE = 0x20,
	RFLAG_WORMPOLICE = 0x40,
	RFLAG_ALIEN = 0x80,
	RFLAG_AQUATIC = 0x100,
	RFLAG_FREEPROTO = 0x200,
	RFLAG_FANATIC = 0x400,
	RFLAG_MINDCONTROL = 0x800,
	RFLAG_COMMFREQ = 0x1000,
	RFLAG_TERRAFORM = 0x2000,
	RFLAG_INTEREST = 0x4000,
	RFLAG_MORALE = 0x8000,
};

enum diplomacy_status_bitfield {
	DSTATUS_PACT = 0x1,
	DSTATUS_TREATY = 0x2,
	DSTATUS_TRUCE = 0x4,
	DSTATUS_COMMLINK = 0x8,
	DSTATUS_VENDETTA = 0x10,
	DSTATUS_WANT_REVENGE = 0x20,
	DSTATUS_UNK_0x40 = 0x40,
	DSTATUS_UNK_0x80 = 0x80,
	DSTATUS_UNK_0x100 = 0x100,
	DSTATUS_UNK_0x200 = 0x200,
	DSTATUS_SHALL_BETRAY = 0x400,
	DSTATUS_UNK_0x800 = 0x800,
	DSTATUS_HAVE_INFILTRATOR = 0x1000,
	DSTATUS_WANT_TO_TALK = 0x2000,
	DSTATUS_UNK_0x4000 = 0x4000,
	DSTATUS_UNK_0x8000 = 0x8000,
	DSTATUS_UNK_0x10000 = 0x10000,
	DSTATUS_UNK_0x20000 = 0x20000,
	DSTATUS_ATROCITY_VICTIM = 0x40000,
	DSTATUS_UNK_0x80000 = 0x80000,
	DSTATUS_UNK_0x100000 = 0x100000,
	DSTATUS_UNK_0x200000 = 0x200000,
	DSTATUS_UNK_0x400000 = 0x400000,
	DSTATUS_UNK_0x800000 = 0x800000,
	DSTATUS_UNK_0x1000000 = 0x1000000,
	DSTATUS_HAVE_SURRENDERED = 0x2000000,
	DSTATUS_UNK_0x4000000 = 0x4000000,
	DSTATUS_UNK_0x8000000 = 0x8000000,
	DSTATUS_UNK_0x10000000 = 0x10000000,
	DSTATUS_UNK_0x20000000 = 0x20000000,
	DSTATUS_UNK_0x40000000 = 0x40000000,
	DSTATUS_UNK_0x80000000 = 0x80000000,
};

enum player_flags_bitfield {
	PFLAG_MAP_REVEALED = 0x200,
	PFLAG_STRAT_ATK_ENEMY_HQ = 0x200000,
	PFLAG_COOP_WITH_HUMAN = 0x400000,
	PFLAG_TEAM_UP_VS_HUMAN = 0x800000,
	PFLAG_COMMIT_ATROCIT_WANTONLY = 0x1000000,
	PFLAG_OBLIT_CAPTURED_BASES = 0x2000000,
	PFLAG_EMPHASIZE_LAND_POWER = 0x4000000,
	PFLAG_EMPHASIZE_SEA_POWER = 0x8000000,
	PFLAG_EMPHASIZE_AIR_POWER = 0x10000000,
	PFLAG_STRAT_SEARCH_OBJECTIVES = 0x20000000,
	PFLAG_STRAT_DEF_OBJECTIVES = 0x40000000,
	PFLAG_STRAT_ATK_OBJECTIVES = 0x80000000,
};

enum player_flags_extended_bitfield {
	PFLAGEXT_STRAT_LOTS_COLONY_PODS = 0x1,
	PFLAGEXT_STRAT_LOTS_TERRAFORMERS = 0x2,
	PFLAGEXT_STRAT_LOTS_SEA_BASES = 0x4,
	PFLAGEXT_STRAT_LOTS_PROBE_TEAMS = 0x8,
	PFLAGEXT_STRAT_LOTS_MISSILES = 0x10,
	PFLAGEXT_SHAMELESS_BETRAY_HUMANS = 0x20,
	PFLAGEXT_STRAT_LOTS_ARTILLERY = 0x40,
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
	uint32_t playerFlags;
	uint32_t ranking; // 0 (lowest) to 7 (highest)
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
	int techCommerceBonus; // Increases commerce income
	int turnCommerceIncome;
	int unk_17;
	int unk_18;
	int techFungusNutrient; // Increases NUTRIENT production in fungus
	int techFungusMineral; // Increases MINERALS production in fungus
	int techFungusEnergy; // Increases ENERGY production in fungus
	int techFungusUnk; // PSI? Dropped mechanic?
	int SE_AllocPsych;
	int SE_AllocLabs;
	int unk_25;
	char gap_330[44];
	int techRanking;
	int unk_26;
	uint32_t satODPDeployed;
	int theoryOfEverything;
	char techTradeSource[92];
	int techAccumulated;
	int techIDResearching;
	int techCost;
	int earnedTechsSaved;
	int netRandomEvent;
	int AI_Fight; // -1, 0, 1
	BOOL AI_Growth;
	BOOL AI_Tech;
	BOOL AI_Wealth;
	BOOL AI_Power;
	int target_xCoord;
	int target_yCoord;
	int unk_28;
	int councilCallTurn;
	int unk_29[11];
	int unk_30[11];
	uint8_t facilityAnnounced[4]; // bitfield - used to determine one time play of fac audio blurb
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
	uint32_t nutrientSurplusTotal;
	int labsTotal;
	uint32_t satSkyHydroLab; // +1 Nutrients per Lab
	uint32_t satNessusMiningStat; // +1 Minerals per Station
	uint32_t satOrbitalPwrTrans; // +1 Energy per Transmitter
	uint32_t satODPTotal;
	int bestWeaponVal;
	int bestPsiAtkVal;
	int bestPsiDefVal;
	int bestArmorVal;
	int bestLandSpeed;
	int enemyBestWeaponVal;
	int enemyBestArmorVal;
	int enemyBestLandSpeed;
	int enemyBestPsiAtkVal;
	int enemyBestPsiDefVal;
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
	uint32_t currentNumBases;
	int milStrength_1;
	int milStrength_2;
	int popTotal;
	int unk_70;
	int planetBusters;
	int unk_71;
	int unk_72;
	int16_t unk_73[128];
	uint8_t baseCountByRegion[128]; // 1-62 (land), 65-126 (sea)
	char unk_75[128];
	int16_t unk_76[128];
	int16_t unk_77[128];
	int16_t unk_78[128];
	int16_t unk_79[128];
	int16_t unk_80[128];
	int16_t unk_81[128];
	char unk_82[128];
	char unk_83[128];
	uint8_t basePlanByRegion[128]; // visible in map UI with omni view + debug mode under base name
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
	uint32_t playerFlagsExt;
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
constexpr int MaxBonusNameNum = 41;
constexpr int MaxPlayerNum = 8;

extern rules_social_category *SocialCategory;
extern rules_social_effect *SocialEffect;
extern LPSTR *Mood;
extern LPSTR *Repute;
extern rules_might *Might;
extern rules_bonusname *BonusName;
extern player *Players; // Players[0] is AI native life faction
extern player_data *PlayersData;
extern faction_art *FactionArt;
extern uint8_t *FactionCurrentBitfield;

OPENSMACX_API BOOL is_human(uint32_t factionID);
OPENSMACX_API LPSTR __cdecl get_adjective(int factionID);
OPENSMACX_API LPSTR __cdecl get_noun(int factionID);
OPENSMACX_API uint32_t __cdecl aah_ooga(int factionID, int pactFactionID);
OPENSMACX_API BOOL __cdecl climactic_battle();
OPENSMACX_API uint32_t __cdecl guard_check(uint32_t factionID, uint32_t region);
OPENSMACX_API void __cdecl see_map_check();
OPENSMACX_API BOOL __cdecl society_avail(int socCategory, int socModel, int factionID);
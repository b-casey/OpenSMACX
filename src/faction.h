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
	RULE_OFFENSE = 18,
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

enum diplomacy_treaty_bitfield {
	DTREATY_PACT = 0x1,
	DTREATY_TREATY = 0x2,
	DTREATY_TRUCE = 0x4,
	DTREATY_COMMLINK = 0x8,
	DTREATY_VENDETTA = 0x10,
	DTREATY_WANT_REVENGE = 0x20,
	DTREATY_UNK_40 = 0x40,
	DTREATY_UNK_80 = 0x80,
	DTREATY_UNK_100 = 0x100,
	DTREATY_UNK_200 = 0x200,
	DTREATY_SHALL_BETRAY = 0x400,
	DTREATY_UNK_800 = 0x800,
	DTREATY_HAVE_INFILTRATOR = 0x1000,
	DTREATY_WANT_TO_TALK = 0x2000,
	DTREATY_UNK_4000 = 0x4000,
	DTREATY_UNK_8000 = 0x8000,
	DTREATY_UNK_10000 = 0x10000,
	DTREATY_UNK_20000 = 0x20000,
	DTREATY_ATROCITY_VICTIM = 0x40000,
	DTREATY_UNK_80000 = 0x80000,
	DTREATY_UNK_100000 = 0x100000,
	DTREATY_UNK_200000 = 0x200000,
	DTREATY_UNK_400000 = 0x400000,
	DTREATY_UNK_800000 = 0x800000,
	DTREATY_UNK_1000000 = 0x1000000,
	DTREATY_HAVE_SURRENDERED = 0x2000000,
	DTREATY_UNK_4000000 = 0x4000000,
	DTREATY_UNK_8000000 = 0x8000000,
	DTREATY_UNK_10000000 = 0x10000000,
	DTREATY_UNK_20000000 = 0x20000000,
	DTREATY_UNK_40000000 = 0x40000000,
	DTREATY_UNK_80000000 = 0x80000000,
};

enum diplo_agenda {
	DAGENDA_UNK_1 = 0x1,
	DAGENDA_UNK_2 = 0x2,
	DAGENDA_UNK_4 = 0x4,
	DAGENDA_FIGHT_TO_DEATH = 0x8,
	DAGENDA_UNK_10 = 0x10,
	DAGENDA_UNK_20 = 0x20,
	DAGENDA_UNK_40 = 0x40,
	DAGENDA_UNK_80 = 0x80,
	DAGENDA_UNK_100 = 0x100,
	DAGENDA_UNK_200 = 0x200,
	DAGENDA_UNK_400 = 0x400,
	DAGENDA_UNK_800 = 0x800,
	DAGENDA_UNK_1000 = 0x1000,
	DAGENDA_DIPLO_PERMANENT = 0x2000,
	DAGENDA_UNK_4000 = 0x4000,
	DAGENDA_UNK_8000 = 0x8000,
};

enum player_flags_bitfield {
	PFLAG_MAP_REVEALED = 0x200,
	PFLAG_UNK_20000 = 0x20000,
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

enum social_effect_id {
	SE_ECONOMY = 0,
	SE_EFFIC = 1,
	SE_SUPPORT = 2,
	SE_TALENT = 3,
	SE_MORALE = 4,
	SE_POLICE = 5,
	SE_GROWTH = 6,
	SE_PLANET = 7,
	SE_PROBE = 8,
	SE_INDUSTRY = 9,
	SE_RESEARCH = 10,
};

enum ai_goal_types {
	AI_GOAL_UNUSED = -1,
	AI_GOAL_ATTACK = 0, // 'a', red
	AI_GOAL_DEFEND = 2, // 'd', yellow                               0000 0010
	AI_GOAL_SCOUT = 3, // 's', pink                                  0000 0011
	AI_GOAL_UNK_1 = 6, // 'n', blue ; related to tile visibility     0000 0110
	AI_GOAL_UNK_2 = 7, // related to movement                        0000 0111
	AI_GOAL_COLONIZE = 8, // 'c', teal                               0000 1000
	AI_GOAL_TERRAFORM_LAND = 9, // 'f', green ' ;                    0000 1001
	AI_GOAL_UNK_3 = 11, // related to combat                         0000 1011
	AI_GOAL_ECHELON_MIRROR = 13,                                  // 0000 1101
	AI_GOAL_PRIORITY_COMBAT = 16, // possibly artillery related      0001 0000
	AI_GOAL_TERRAFORM_WATER = 25,                                 // 0001 1001
	AI_GOAL_LANDING_SITE = 41, // '^', white                         0010 1001
	AI_GOAL_CONDENSER = 73,                                       // 0100 1001
	AI_GOAL_THERMAL_BOREHOLE = 105,                               // 0110 1001
	AI_GOAL_SENSOR_ARRAY = 121,                                   // 0111 1001
};

struct social_category {
	int politics;
	int economics;
	int values;
	int future;
};

struct social_effect {
	int economy;
	int effiency;
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

struct rules_social_effect {
	char set1[24];
	char set2[24];
	char padding[56];
};

struct rules_social_category {
	LPSTR type;
	int preqTech[4];
	LPSTR name[4];
	social_effect effect[4]; // break out into individual vars?
};

struct rules_might {
	LPSTR adjStart;
	LPSTR adj;
};

struct rules_bonusname {
	char key[24];
};

/*
Goals overlay is visible with debug mode activated. Color is based on goal type.
Shortcut: Shift-%
Format without omniscient view: <One letter type, see ai_goal_types><priority>
Format with omniscient view: <1st letter of faction><One letter type, see ai_goal_types><priority>
*/
struct goal {
	int16_t type;
	int16_t priority;
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
	char adjNameFaction[128]; // drops 2nd entry on line (abbreviation value?), non-English forms?
	char formalNameFaction[40];
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
	int AI_Fight; // willingness to use force to achieve goals (-1, 0, 1)
	BOOL AI_Growth; // interest in population growth
	BOOL AI_Tech; // interest in knowledge
	BOOL AI_Wealth; // interest in wealth
	BOOL AI_Power; // interest in power
	int socIdeologyCategory; // priority
	int socAntiIdeologyCategory; // opposition
	int socIdeologyModel; // priority
	int socAntiIdeologyModel; // opposition
	int socIdeologyEffect; // emphasis/priority
	int socAntiIdeologyEffect; // opposition ; unused, never set in default factions
};

struct player_data {
	uint32_t playerFlags; // see player_flags_bitfield
	uint32_t ranking; // 0 (lowest) to 7 (highest) rank ; used to determine #MIGHT
	int diffLevel;
	uint32_t baseNameOffset; // keep track which base names have been used
	uint32_t baseSeaNameOffset; // keep track which sea base names have been used
	int lastTurnNewBase; // turn for last built, captured or acquired (drone riot) base
	int diploTreaties[8];
	int diploAgenda[8];
	int diploFriction[8];
	int diploSpoke[8]; // Turn for the last player-to-AI communication; -1 for never
	int diploMerc[8]; // mercifulness? Possibly higher values indicate willingness for deal-making
	char diploPatience[8]; // AI-to-player
	int sanctionTurns; // Turns left for economic sanctions imposed by other factions for atrocities
	int loanBalance[8]; // Loan balance remaining this faction owes another to be paid over term
	int loanPayment[8]; // The per turn payment amount this faction owes another faction
	char gap_104[32];
	int integrityBlemishes; // Blacken
	int globalReputation; // ? integrity? related to signing treaties, freeing faction
	int diploUnk_1[8]; // ? rights (vs wrongs below)? gift, bribe; Gifts and bribes we have received
	int diploWrongs[8]; // number of times the player has double crossed (wronged) this faction
	int diploBetrayed[8]; // number of times the player has been double crossed by this faction
	int diploUnk_3[8]; // ? combat related
	int diploUnk_4[8]; // ? combat related
	int tradedMaps; // bitfield of other factions that have traded maps with faction
	int baseGovernorAdv; // default advanced Governor settings; see governor_base_bitfield
	int atrocities; // count committed by faction
	int majorAtrocities; // count committed by faction
	// mind control action value: mind/thought control base (+4) or successfully subvert unit (+1)
	// TODO: possible issue with thought control still increasing variables on failure
	uint32_t mindControlTotal; // running sum of mind control actions above against other factions
	uint32_t diploMindControl[8]; // total action value against this player by each faction
	uint32_t stolenDataCount[8]; // probe successfully procured research data (tech/map) per faction
	int energyReserves; // current energy credits
	int energyCost; // ?
	social_category socCategoryPending;
	social_category socCategoryActive;
	int socUpheavalCostPaid;
	social_effect socEffectPending;
	social_effect socEffectActive; // ?
	social_effect socEffectTemp; // maybe previous turn?
	social_effect socEffectBase; // from faction files?
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
	int techRanking; // Twice the number of techs discovered
	int unk_26;
	uint32_t satODPDeployed;
	int theoryOfEverything;
	char techTradeSource[92];
	int techAccumulated; // Research
	int techIDResearching; // Researching
	int techCost;
	int earnedTechsSaved;
	int netRandomEvent;
	int AI_Fight; // -1, 0, 1
	BOOL AI_Growth;
	BOOL AI_Tech;
	BOOL AI_Wealth;
	BOOL AI_Power;
	int xCoordTarget;
	int yCoordTarget;
	int unk_28;
	int councilCallTurn;
	int unk_29[11]; // used by council code related to buying votes; vote id?
	int unk_30[11]; // used by council code related to buying votes; factionID
	uint8_t facilityAnnounced[4]; // bitfield - used to determine one time play of fac audio blurb
	int unk_32;
	char unk_33;
	char unk_34;
	char gap_462[2];
	int planetEcology;
	int baseIDAtkTarget; // Battle planning of base to attack, -1 if not set
	int unk_37;
	char savedQueueName[8][24]; // queue template
	int savedQueueSize[8]; // queue template
	int savedQueueItems[8][10]; // queue template
	int unk_38[8];
	int unk_39[8][9];
	int unk_46[9];
	int unk_47;
	int unk_48;
	int unk_49;
	uint32_t nutrientSurplusTotal;
	int labsTotal;
	uint32_t satellitesNutrient; // +1 Nutrients per Sky Hydroponics Lab
	uint32_t satellitesMineral; // +1 Minerals per Nessus Mining Station
	uint32_t satellitesEnergy; // +1 Energy per Orbital Power Transmitter
	uint32_t satellitesODPTotal; // Orbital Defense Pod count
	int bestWeaponValue;
	int bestPsiOffense;
	int bestPsiDefense;
	int bestArmorValue;
	int bestLandSpeed;
	int enemyBestWeaponValue; // Enemy refers here to any non-pact faction
	int enemyBestArmorValue;
	int enemyBestLandSpeed;
	int enemyBestPsiOffense;
	int enemyBestPsiDefense;
	int unk_64;
	int unk_65;
	int unk_66;
	int unk_67;
	int unk_68;
	char unk_69[4];
	uint8_t protoID_Active[512];
	uint8_t protoID_Queue[512];
	int16_t protoID_Lost[512];
	int totalMilVeh; // total combat units
	uint32_t currentNumBases;
	int milStrength_1;
	int milStrength_2;
	int popTotal;
	int unk_70; // AI total TRIAD_SEA Veh?
	int planetBusters;
	int unk_71;
	int unk_72;
	/*
	* AI planning variables that relate to faction units in specific disjoint land/water areas.
	* All of these are indexed by the region value in Map struct (see for more details).
	*/
	uint16_t regionTotalCombatVehs[128];
	uint8_t regionTotalBases[128];
	uint8_t regionTotalOffensiveVehs[128];
	uint16_t regionForceRating[128]; // Combined offensive/morale rating of all units in the area
	uint16_t unk_77[128]; // Movement planning flags
	uint16_t unk_78[128]; // controlled territory by region?
	uint16_t unk_79[128]; // territory related to scenario obj or visible by region?
	uint16_t unk_80[128]; // good quality controlled territory count by region?
	uint16_t unk_81[128]; // something to do with territory / base radius by region?
	uint8_t unk_82[128]; // world site territory by region?
	uint8_t unk_83[128]; // related to goody_at()?
	uint8_t regionBasePlan[128]; // visible in map UI with omni view + debug mode under base name
	/* End of block */
	goal goals[75];
	goal sites[25]; // same struct as goals, subset of general goals?
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
constexpr int MaxGoalsNum = 75;
constexpr int MaxSitesNum = 25;

extern rules_social_category *SocialCategory;
extern rules_social_effect *SocialEffect;
extern LPSTR *Mood;
extern LPSTR *Repute;
extern rules_might *Might;
extern rules_bonusname *BonusName;
extern player *Players; // Players[0] is AI native life faction
extern player_data *PlayersData;
extern faction_art *FactionArt;
extern uint8_t *FactionsStatus;

OPENSMACX_API BOOL __cdecl is_human(uint32_t factionID);
OPENSMACX_API BOOL __cdecl is_alive(uint32_t factionID);
OPENSMACX_API LPSTR __cdecl get_adjective(int factionID);
OPENSMACX_API LPSTR __cdecl get_noun(int factionID);
OPENSMACX_API BOOL __cdecl auto_contact();
OPENSMACX_API uint32_t __cdecl aah_ooga(int factionID, int pactFactionID);
OPENSMACX_API BOOL __cdecl climactic_battle();
OPENSMACX_API uint32_t __cdecl guard_check(uint32_t factionID, uint32_t region);
OPENSMACX_API void __cdecl add_goal(uint32_t factionID, int type, int priority, int xCoord,
	int yCoord, int baseID);
OPENSMACX_API void __cdecl add_site(uint32_t factionID, int type, int priority, int xCoord,
	int yCoord);
OPENSMACX_API BOOL __cdecl at_goal(uint32_t factionID, int type, int xCoord, int yCoord);
OPENSMACX_API BOOL __cdecl at_site(uint32_t factionID, int type, int xCoord, int yCoord);
OPENSMACX_API void __cdecl wipe_goals(uint32_t factionID);
OPENSMACX_API void __cdecl init_goals(uint32_t factionID);
OPENSMACX_API void __cdecl del_site(uint32_t factionID, int type, int xCoord, int yCoord,
	int proximity);
OPENSMACX_API uint32_t __cdecl corner_market(uint32_t factionID);
OPENSMACX_API void __cdecl see_map_check();
OPENSMACX_API BOOL __cdecl society_avail(int socCategory, int socModel, int factionID);
OPENSMACX_API void __cdecl enemy_capabilities(uint32_t factionID);
OPENSMACX_API void __cdecl enemy_capabilities_t(uint32_t factionID);

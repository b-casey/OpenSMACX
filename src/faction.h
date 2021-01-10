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
enum PlayerGender {
	GENDER_MALE = 0,
	GENDER_FEMALE = 1,
	GENDER_NEUTRAL = 2,
};

enum FactionSpecialRules {
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

enum PlayerRuleFlagBitfield {
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

enum DiplomacyTreatyBitfield {
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

enum DiploAgenda {
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

enum PlayerFlagsBitfield {
	PFLAG_UNK_20 = 0x20,
	PFLAG_MAP_REVEALED = 0x200,
	PFLAG_GENETIC_PLAGUE_INTRO = 0x400, // +1 to defense against after 1st time faction experiences
	PFLAG_BEEN_ELECTED_GOVERNOR = 0x8000, // used to determine whether #GOVERNOR has been displayed
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

enum PlayerFlagsExtendedBitfield {
	PFLAGEXT_STRAT_LOTS_COLONY_PODS = 0x1,
	PFLAGEXT_STRAT_LOTS_TERRAFORMERS = 0x2,
	PFLAGEXT_STRAT_LOTS_SEA_BASES = 0x4,
	PFLAGEXT_STRAT_LOTS_PROBE_TEAMS = 0x8,
	PFLAGEXT_STRAT_LOTS_MISSILES = 0x10,
	PFLAGEXT_SHAMELESS_BETRAY_HUMANS = 0x20,
	PFLAGEXT_STRAT_LOTS_ARTILLERY = 0x40,
};

enum AiGoalTypes {
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

enum SocialCategories {
	SOCIAL_CAT_POLITICS = 0,
	SOCIAL_CAT_ECONOMICS = 1,
	SOCIAL_CAT_VALUES = 2,
	SOCIAL_CAT_FUTURE = 3,
};

enum SocialEngineeringPolitics {
	SE_FRONTIER = 0,
	SE_POLICE_STATE = 1,
	SE_DEMOCRATIC = 2,
	SE_FUNDAMENTALIST = 3,
};

enum SocialEngineeringEconomics {
	SE_SIMPLE = 0,
	SE_FREE_MARKET = 1,
	SE_PLANNED = 2,
	SE_GREEN = 3,
};

enum SocialEngineeringValues {
	SE_SURVIVAL = 0,
	SE_POWER = 1,
	SE_KNOWLEDGE = 2,
	SE_WEALTH = 3,
};

enum SocialEngineeringFuture {
	SE_NONE = 0,
	SE_CYBERNETIC = 1,
	SE_EUDAIMONIC = 2,
	SE_THOUGHT_CONTROL = 3,
};

enum PlayerMood {
	MOOD_MAGNANIMOUS = 0, // also Submissive, a special case where faction is hopelessly overmatched
	MOOD_SOLICITOUS = 1,
	MOOD_COOPERATIVE = 2,
	MOOD_NONCOMMITTAL = 3,
	MOOD_AMBIVALENT = 4,
	MOOD_OBSTINATE = 5,
	MOOD_QUARRELSOME = 6,
	MOOD_BELLIGERENT = 7,
	MOOD_SEETHING = 8,
};

struct SocialCategory {
	uint32_t politics;
	uint32_t economics;
	uint32_t values;
	uint32_t future;
};

struct SocialEffect {
	int economy;
	int efficiency;
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

struct RulesSocialEffect {
	char set1[24];
	char set2[24];
	char padding[56];
};

struct RulesSocialCategory {
	LPSTR type;
	int preq_tech[4];
	LPSTR name[4];
    SocialEffect model_effect[4];
};

struct RulesMight {
	LPSTR adj_start;
	LPSTR adj;
};

struct RulesBonusName {
	char key[24];
};

/*
Goals overlay is visible with debug mode activated. Color is based on Goal type.
Shortcut: Shift-%
Format without omniscient view: <One letter type, see ai_goal_types><priority>
Format with omniscient view: <1st letter of faction><One letter type, see ai_goal_types><priority>
*/
struct Goal {
	int16_t type;
	int16_t priority;
	int x;
	int y;
	int base_id;
};

struct Player {
	BOOL is_leader_female;
	char filename[24];
	char search_key[24];
	char name_leader[24];
	char title_leader[24];
	char adj_leader[128];
	char adj_insult_leader[128];
	char adj_faction[128];
	char adj_insult_faction[128];
	uint8_t padding1[128];
	char noun_faction[24];
	int noun_gender;
	BOOL is_noun_plural;
	char adj_name_faction[128]; // drops 2nd entry on line (abbreviation value?), non-English forms?
	char formal_name_faction[40];
	char insult_leader[24];
	char desc_name_faction[24];
	char assistant_name[24];
	char scientist_name[24];
	char assistant_city[24];
	uint8_t padding2[176];
	int rule_tech_selected;
	int rule_morale;
	int rule_research;
	int rule_drone;
	int rule_talent;
	int rule_energy;
	int rule_interest;
	int rule_population;
	int rule_hurry;
	int rule_techcost;
	int rule_psi;
	int rule_sharetech;
	int rule_commerce;
	int rule_flags;
	int faction_bonus_count;
	int faction_bonus_id[8];
	int faction_bonus_val1[8];
	int faction_bonus_val2[8];
	int ai_fight; // willingness to use force to achieve goals (-1, 0, 1)
	BOOL ai_growth; // interest in population growth
	BOOL ai_tech; // interest in knowledge
	BOOL ai_wealth; // interest in wealth
	BOOL ai_power; // interest in power
	int soc_ideology_category; // priority
	int soc_anti_ideology_category; // opposition
	int soc_ideology_model; // priority
	int soc_anti_ideology_model; // opposition
	int soc_ideology_effect; // emphasis/priority
	int soc_anti_ideology_effect; // opposition ; unused, never set in default factions
};

struct PlayerData {
	uint32_t flags; // see PlayerFlagsBitfield
	uint32_t ranking; // 0 (lowest) to 7 (highest) rank ; used to determine #MIGHT
	int diff_level;
	uint32_t base_name_offset; // keep track which base names have been used
	uint32_t base_sea_name_offset; // keep track which sea base names have been used
	int last_turn_new_base; // turn for last built, captured or acquired (drone riot) base
	uint32_t diplo_treaties[8];
	int diplo_agenda[8];
	uint32_t diplo_friction[8]; // between 0-20
	int diplo_spoke[8]; // Turn for the last player-to-AI communication; -1 for never
	int diplo_merc[8]; // mercifulness? Possibly higher values indicate willingness for deal-making
	char diplo_patience[8]; // AI-to-player
	int sanction_turns; // Turns left for other faction imposed economic sanctions for atrocities
	int loan_balance[8]; // Loan balance remaining this faction owes another to be paid over term
	int loan_payment[8]; // The per turn payment amount this faction owes another faction
	int unk_12[8];
	int integrity_blemishes; // Blacken
	int global_reputation; // ? integrity? related to signing treaties, freeing faction
	int diplo_unk1[8]; // ? rights (vs wrongs below)? gift, bribe; Gifts and bribes we have received
	int diplo_wrongs[8]; // number of times the player has double crossed (wronged) this faction
	int diplo_betrayed[8]; // number of times the player has been double crossed by this faction
	int diplo_unk3[8]; // ? combat related
	int diplo_unk4[8]; // ? combat related
	int traded_maps; // bitfield of other factions that have traded maps with faction
	int base_governor_adv; // default advanced Governor settings; see governor_base_bitfield
	int atrocities; // count committed by faction
	int major_atrocities; // count committed by faction
	// mind control action value: mind/thought control base (+4) or successfully subvert unit (+1)
	// TODO: possible issue with thought control still increasing variables on failure
	uint32_t mind_control_total; // running sum of mind control actions above against other factions
	uint32_t diplo_mind_control[8]; // total action value against this player by each faction
	uint32_t stolen_data_count[8]; // probe procured research data (tech/map) per faction
	int energy_reserves; // current energy credits
	uint32_t hurry_cost_total; // Net MP: Total paid energy to hurry production (current turn)
	SocialCategory soc_category_pending;
	SocialCategory soc_category_active;
	int soc_upheaval_cost_paid;
    SocialEffect soc_effect_pending;
    SocialEffect soc_effect_active; // ?
    SocialEffect soc_effect_temp; // maybe previous turn?
    SocialEffect soc_effect_base; // from faction files?
	int unk_13;
	uint32_t maint_cost_total; // Total maintenance costs for all bases (current turn)
	int tech_commerce_bonus; // Increases commerce income
	int turn_commerce_income;
	int unk_17;
	int unk_18;
	int tech_fungus_nutrient; // Increases NUTRIENT production in fungus
	int tech_fungus_mineral; // Increases MINERALS production in fungus
	int tech_fungus_energy; // Increases ENERGY production in fungus
	int tech_fungus_unk; // PSI? Dropped mechanic?
	int se_alloc_psych;
	int se_alloc_labs;
	uint32_t tech_pact_shared_goals[12]; // Bitfield; Suggested techID goals from pacts (TEAMTECH)
	int tech_ranking; // Twice the number of techs achieved
	int unk_26;
	uint32_t sat_odp_deployed;
	int theory_of_everything;
	char tech_trade_source[92];
	int tech_accumulated; // Research
	int tech_id_researching; // Researching
	int tech_cost;
	int earned_techs_saved;
	int net_random_event;
	int ai_fight; // -1, 0, 1
	BOOL ai_growth;
	BOOL ai_tech;
	BOOL ai_wealth;
	BOOL ai_power;
	int x_target;
	int y_target;
	int unk_28;
	int council_call_turn;
	int unk_29[11]; // used by council code related to buying votes; vote id?
	int unk_30[11]; // used by council code related to buying votes; factionID
	uint8_t facility_announced[4]; // bitfield - used to determine one time play of fac audio blurb
	int unk_32;
	char unk_33;
	char unk_34;
	char unk_35;
	char unk_36;
	int planet_ecology;
	int base_id_atk_target; // Battle planning of base to attack, -1 if not set
	int unk_37;
	char saved_queue_name[8][24]; // queue template
	int saved_queue_size[8]; // queue template
	int saved_queue_items[8][10]; // queue template
	int unk_38[8];
	int unk_39[8][9];
	int unk_46[9];
	int unk_47;
	int unk_48;
	int unk_49;
	uint32_t nutrient_surplus_total;
	int labs_total;
	uint32_t satellites_nutrient; // +1 Nutrients per Sky Hydroponics Lab
	uint32_t satellites_mineral; // +1 Minerals per Nessus Mining Station
	uint32_t satellites_energy; // +1 Energy per Orbital Power Transmitter
	uint32_t satellites_odp_total; // Orbital Defense Pod count
	int best_weapon_value;
	int best_psi_offense;
	int best_psi_defense;
	int best_armor_value;
	int best_land_speed;
	int enemy_best_weapon_value; // Enemy refers here to any non-pact faction
	int enemy_best_armor_value;
	int enemy_best_land_speed;
	int enemy_best_psi_offense;
	int enemy_best_psi_defense;
	int unk_64;
	int unk_65;
	int unk_66;
	int unk_67;
	int unk_68;
	char unk_69[4];
	uint8_t proto_id_active[512];
	uint8_t proto_id_queue[512];
	int16_t proto_id_lost[512];
	int total_mil_veh; // total combat units
	uint32_t current_num_bases;
	int mil_strength_1;
	int mil_strength_2;
	int pop_total;
	int unk_70; // AI total TRIAD_SEA Veh?
	int planet_busters;
	int unk_71;
	int unk_72;
	/*
	* AI planning variables that relate to faction units in specific disjoint land/water areas.
	* All of these are indexed by the region value in Map struct (see for more details).
	*/
	uint16_t region_total_combat_vehs[128];
	uint8_t region_total_bases[128];
	uint8_t region_total_offensive_vehs[128];
	uint16_t region_force_rating[128]; // Combined offensive/morale rating of all units in the area
	uint16_t unk_77[128]; // Movement planning flags
	uint16_t unk_78[128]; // controlled territory by region?
	uint16_t unk_79[128]; // territory related to scenario obj or visible by region?
	uint16_t unk_80[128]; // good quality controlled territory count by region?
	uint16_t unk_81[128]; // something to do with territory / base radius by region?
	uint8_t unk_82[128]; // world site territory by region?
	uint8_t unk_83[128]; // related to goody_at()?
	uint8_t region_base_plan[128]; // visible in map UI with omni view + debug mode under base name
	/* End of block */
	Goal goals[75];
	Goal sites[25]; // same struct as goals, subset of general goals?
	int unk_93;
	int unk_94;
	int unk_95;
	int unk_96;
	int unk_97;
	uint32_t tech_achieved; // count of technology faction has discovered/achieved
	int time_bonus_count; // Net MP: Each is worth amount in seconds under Time Controls Extra
	int unk_99; // unused?
	uint32_t secret_project_intel[8]; // Bitfield; News of SPs other factions are working on
	int corner_market_turn;
	int corner_market_active;
	int unk_101;
	int unk_102;
	int unk_103;
	uint32_t flags_ext;
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

struct FactionArt {
	Sprite base[6][4];
	Sprite council_logo[2];
	Sprite council_small;
	Sprite council_big;
	Sprite diplomacy;
	Sprite diplomacy_logo;
	Sprite diplomacy_landscape;
	Sprite datalink;
	Sprite report_logo_small[3];
	Sprite report_logo[2];
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

extern RulesSocialCategory *SocialCategories;
extern RulesSocialEffect *SocialEffects;
extern LPSTR *Mood;
extern LPSTR *Repute;
extern RulesMight *Might;
extern RulesBonusName *BonusName;
extern Player *Players; // Players[0] is AI native life faction
extern PlayerData *PlayersData;
extern FactionArt *FactionsArt;
extern uint8_t *FactionsStatus;
extern uint32_t *FactionRankings;
extern uint32_t *RankingFactionIDUnk1;
extern uint32_t *RankingFactionIDUnk2;
extern uint32_t *FactionRankingsUnk;
extern int *DiploFriction;
extern uint32_t *DiploFrictionFactionIDWith;
extern uint32_t *DiploFrictionFactionID;

OPENSMACX_API BOOL __cdecl is_alien_faction(uint32_t faction_id);
OPENSMACX_API BOOL __cdecl is_human(uint32_t faction_id);
OPENSMACX_API BOOL __cdecl is_alive(uint32_t faction_id);
OPENSMACX_API uint32_t __cdecl has_treaty(uint32_t faction_id, uint32_t faction_id_with, 
                                          uint32_t treaties);
OPENSMACX_API LPSTR __cdecl get_adjective(uint32_t faction_id);
OPENSMACX_API LPSTR __cdecl get_noun(uint32_t faction_id);
OPENSMACX_API BOOL __cdecl auto_contact();
OPENSMACX_API BOOL __cdecl great_beelzebub(uint32_t faction_id, BOOL is_aggressive);
OPENSMACX_API BOOL __cdecl great_satan(uint32_t faction_id, BOOL is_aggressive);
OPENSMACX_API uint32_t __cdecl aah_ooga(int faction_id, int pact_faction_id);
OPENSMACX_API BOOL __cdecl climactic_battle();
OPENSMACX_API BOOL __cdecl at_climax(uint32_t faction_id);
OPENSMACX_API void __cdecl cause_friction(uint32_t faction_id, uint32_t faction_id_with, 
                                          int friction);
OPENSMACX_API uint32_t __cdecl get_mood(int friction);
OPENSMACX_API uint32_t __cdecl reputation(uint32_t faction_id, uint32_t faction_id_with);
OPENSMACX_API int __cdecl get_patience(uint32_t faction_id_with, uint32_t faction_id);
OPENSMACX_API uint32_t __cdecl energy_value(uint32_t loan_principal);
OPENSMACX_API void __cdecl set_treaty(uint32_t faction_id, uint32_t faction_id_with, 
                                      uint32_t treaty, BOOL set);
OPENSMACX_API void __cdecl set_agenda(uint32_t faction_id, uint32_t faction_id_with, 
                                      uint32_t agenda, BOOL set);
OPENSMACX_API uint32_t __cdecl has_agenda(uint32_t faction_id, uint32_t faction_id_with,
                                          uint32_t agenda);
OPENSMACX_API BOOL __cdecl wants_to_attack(uint32_t faction_id, uint32_t faction_id_tgt,
                                           int faction_id_unk);
OPENSMACX_API uint32_t __cdecl guard_check(uint32_t faction_id, uint32_t region);
OPENSMACX_API void __cdecl add_goal(uint32_t faction_id, int type, int priority, int x, int y, 
                                    int base_id);
OPENSMACX_API void __cdecl add_site(uint32_t faction_id, int type, int priority, int x, int y);
OPENSMACX_API BOOL __cdecl at_goal(uint32_t faction_id, int type, int x, int y);
OPENSMACX_API BOOL __cdecl at_site(uint32_t faction_id, int type, int x, int y);
OPENSMACX_API void __cdecl wipe_goals(uint32_t faction_id);
OPENSMACX_API void __cdecl init_goals(uint32_t faction_id);
OPENSMACX_API void __cdecl del_site(uint32_t faction_id, int type, int x, int y, int proximity);
OPENSMACX_API uint32_t __cdecl corner_market(uint32_t faction_id);
OPENSMACX_API void __cdecl see_map_check();
OPENSMACX_API void __cdecl compute_faction_modifiers(uint32_t faction_id);
OPENSMACX_API void __cdecl social_calc(SocialCategory *category, SocialEffect *effect, 
                                       uint32_t faction_id, BOOL toggle, BOOL is_quick_calc);
OPENSMACX_API void __cdecl social_upkeep(uint32_t faction_id);
OPENSMACX_API uint32_t __cdecl social_upheaval(uint32_t faction_id, SocialCategory *category_new);
OPENSMACX_API BOOL __cdecl society_avail(int soc_category, int soc_model, uint32_t faction_id);
OPENSMACX_API void __cdecl social_ai(uint32_t faction_id, int growth_val, int tech_val, 
                                     int wealth_val, int power_val, SocialCategory *output);
OPENSMACX_API void __cdecl enemy_capabilities(uint32_t faction_id);
OPENSMACX_API void __cdecl enemy_capabilities_t(uint32_t faction_id);

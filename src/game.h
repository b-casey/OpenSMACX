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
  * Game related objects, variables and functions.
  */

enum game_rules_general_set1_bitfield {
	TGL_SCENARIO_CHEATED_FLAG = 0x20,
	TGL_SCENARIO_EDITOR = 0x40,
	TGL_OMNISCIENT_VIEW = 0x80,
	TGL_DEBUG_MODE = 0x1000,
	SCN_VICT_TERRITORY_COUNT_OBJ = 0x8000,
	SCN_VICT_ALL_ARTIFACTS_OBJ_UNIT = 0x40000,
	SCN_VICT_HIGHEST_AC_SCORE_WINS = 0x80000,
	TGL_PERIHELION_ACTIVE = 0x100000,
	RAND_FAC_LEADER_PERSONALITIES = 0x800000,
	RAND_FAC_LEADER_SOCIAL_AGENDA = 0x1000000,
	SCN_VICT_TERRAIN_ENH_COUNT_OBJ = 0x2000000,
	SCN_VICT_BASE_FACIL_COUNT_OBJ = 0x4000000,
	SCN_VICT_POPULATION_COUNT_OBJ = 0x20000000,
	SCN_VICT_TECH_COUNT_OBJ = 0x40000000,
	SCN_VICT_CREDITS_COUNT_OBJ = 0x80000000,
};

enum game_rules_general_set2_bitfield {
	DO_OR_DIE = 0x1,
	VICTORY_TOTAL_WAR = 0x2,
	VICTORY_MINE_ALL_MINE = 0x4,
	VICTORY_PEACE_IN_OUR_TIME = 0x8,
	LOOK_FIRST = 0x10,
	TECH_STAGNATION = 0x20,
	INTENSE_RIVALRY = 0x40,
	TIME_WARP = 0x80,
	NO_UNITY_SURVEY = 0x100,
	BLIND_RESEARCH = 0x200,
	IRONMAN = 0x400,
	VICTORY_HIGHER_GOAL = 0x800,
	VICTORY_ONE_FOR_ALL = 0x1000,
	NO_UNITY_SCATTERING = 0x2000,
	SPOILS_OF_WAR = 0x4000,
	BELL_CURVE = 0x8000,
	SCENRULE_UNITY_PODS_NO_RESOURCES = 0x10000,
	SCENRULE_UNITY_PODS_NO_MONOLITHS = 0x20000,
	SCENRULE_UNITY_PODS_NO_ARTIFACTS = 0x40000,
	SCN_VICT_SOLO_MISSION = 0x80000,
	SCENRULE_FORCE_PLAYER_PLAY_CURRENT_FACT = 0x100000,
	SCENRULE_NO_NATIVE_LIFE = 0x200000,
	SCENRULE_NO_COLONY_PODS = 0x400000,
	SCENRULE_NO_TERRAFORMING = 0x800000,
	SCENRULE_FORCE_CURRENT_DIFF_LEVEL = 0x1000000,
	SCENRULE_NO_TECH_TRADING = 0x2000000,
	SCENRULE_NO_TECH_ADVANCES = 0x4000000,
	SCN_VICT_OBJ_UNITS_REACH_FRIEND_OBJ_BASE = 0x8000000,
	SCN_VICT_OBJ_UNITS_REACH_FRIEND_HQ_BASE = 0x10000000,
	SCN_VICT_ALL_BASE_COUNT_OBJ = 0x20000000,
	SCN_VICT_SP_COUNT_OBJ = 0x40000000,
	SCENRULE_NO_BUILDING_SP = 0x80000000,
};

enum game_difficulty_level {
	DLVL_CITIZEN = 0,
	DLVL_SPECIALIST = 1,
	DLVL_TALENT = 2,
	DLVL_LIBRARIAN = 3,
	DLVL_THINKER = 4,
	DLVL_TRANSCEND = 5,
};

extern BOOL *SMACX_Enabled;
extern uint32_t *GameRules; // bitfield
extern uint32_t *GameRules2; // bitfield
extern int *DiffLevelCurrent;
extern int *TurnCurrentNum;
extern int *MissionYearCurrent;
extern uint32_t *TectonicDetonationCount;
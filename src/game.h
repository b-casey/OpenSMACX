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

enum game_rules_general_set1 {
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

enum game_difficulty_level {
	DLVL_CITIZEN = 0,
	DLVL_SPECIALIST = 1,
	DLVL_TALENT = 2,
	DLVL_LIBRARIAN = 3,
	DLVL_THINKER = 4,
	DLVL_TRANSCEND = 5,
};

extern uint32_t *GameRules; // bitfield
extern int *DiffLevelCurrent;
extern int *TurnCurrentNum;
extern int *MissionYearCurrent;
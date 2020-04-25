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
#include "stdafx.h"
#include "game.h"
#include "alpha.h"

BOOL *SMACX_Enabled = (BOOL *)0x009A6488;
uint32_t *GameState  = (uint32_t *)0x009A64C0;
uint32_t *GameRules = (uint32_t *)0x009A649C;
int *DiffLevelCurrent = (int *)0x009A64C4;
int *TurnCurrentNum = (int *)0x009A64D4;
uint32_t *ObjectiveReqVictory = (uint32_t *)0x0094B4C0;
uint32_t *ObjectivesSuddenDeathVictory = (uint32_t *)0x0094B4C4;
uint32_t *ObjectiveAchievePts = (uint32_t *)0x0094B4C8;
uint32_t *VictoryAchieveBonusPts = (uint32_t *)0x0094B4CC;
uint32_t *MissionYearCurrent = (uint32_t *)0x009A64D8;
uint32_t *StartingMissionYear = (uint32_t *)0x0094B4D0;
uint32_t *EndingMissionYear = (uint32_t *)0x0094B4D4;
uint32_t *TectonicDetonationCount = (uint32_t *)0x00946138; // [8]; value for each faction

int *SunspotDuration = (int *)0x009A6800;
uint32_t *MountPlanetXCoord = (uint32_t *)0x009A6804;
uint32_t *MountPlanetYCoord = (uint32_t *)0x009A6808;
int *DustCloudDuration = (int *)0x009A680C;

/*
Purpose: Clear and reset scenario game variables.
Original Offset: 005B38D0
Return Value: n/a
Status: Complete
*/
void __cdecl clear_scenario() {
	*ObjectiveReqVictory = 9999;
    *ObjectivesSuddenDeathVictory = 9999;
    *ObjectiveAchievePts = 0;
    *VictoryAchieveBonusPts = 0;
    *StartingMissionYear = Rules->NormalStartingYear;
    *EndingMissionYear = *DiffLevelCurrent < DLVL_LIBRARIAN
        ? Rules->NormalEndYearLowThreeDiff : Rules->NormalEndYearHighThreeDiff;
}

/*
Purpose: Calculate game year from start date and turn number.
Original Offset: 005C89A0
Return Value: Game year
Status: Complete
*/
uint32_t __cdecl game_year(int turn) {
	return *StartingMissionYear + turn;
}

/*
Purpose: Craft game year string. Assume buffer of at least 80. Revisit and convert to std::string.
Original Offset: 005C89B0
Return Value: n/a
Status: Complete
*/
void __cdecl say_year(LPSTR output) {
	char year[80];
	_itoa_s(game_year(*TurnCurrentNum), year, 80, 10);
	strcat_s(output, 80, year);
}
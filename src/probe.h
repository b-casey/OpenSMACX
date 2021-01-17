/*
 * OpenSMACX - an open source clone of Sid Meier's Alpha Centauri.
 * Copyright (C) 2013-2021 Brendan Casey
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
  * Probe related objects, variables and functions.
  */
enum ProbePrimaryAction {
    PRB_INFILTRATE_DATALINKS = 0,
    PRB_PROCURE_RESEARCH_DATA = 1,
    PRB_ACTIVATE_SABOTAGE_VIRUS = 2,
    PRB_DRAIN_ENERGY_RESERVES = 3,
    PRB_INCITE_DRONE_RIOTS = 4,
    PRB_ASSASSINATE_PROMINENT_RESEARCHERS = 5,
    PRB_MIND_CONTROL_CITY = 6,
    PRB_INTRODUCE_GENETIC_PLAGUE = 7,
    PRB_FREE_CAPTURED_FACTION_LEADER = 8,
    PRB_MIND_CONTROL_VEH = -1,
};

extern BOOL *ProbeHasAlgoEnhancement;
extern uint32_t *ProbeTargetFactionID;
extern BOOL *ProbeTargetHasHSA;

DLLEXPORT int __cdecl steal_energy(uint32_t base_id);
DLLEXPORT int __cdecl mind_control(uint32_t base_id, uint32_t faction_id, BOOL is_corner_market);
DLLEXPORT int __cdecl success_rates(uint32_t id, uint32_t morale, int diff_modifier, int base_id);

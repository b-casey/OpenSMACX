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

 /*
  * Council related objects, variables and functions.
  */
enum CouncilProposals {
	PROP_ELECT_PLANETARY_GOVERNOR = 0,
	PROP_UNITE_SUPREME_LEADER = 1,
	PROP_SALVAGE_UNITY_CORE = 2,
	PROP_GLOBAL_TRADE_PACT = 3,
	PROP_REPEAL_GLOBAL_TRADE_PACT = 4,
	PROP_LAUNCH_SOLAR_SHADE = 5,
	PROP_INCREASE_SOLAR_SHADE = 6,
	PROP_MELT_POLAR_CAPS = 7,
	PROP_REPEAL_UN_CHARTER = 8,
	PROP_REINSTATE_UN_CHARTER = 9,
};

struct RulesProposal {
	LPSTR name;
	LPSTR description;
	int preq_tech;
};

constexpr int MaxProposalNum = 11;

extern RulesProposal *Proposal;

OPENSMACX_API uint32_t __cdecl council_votes(uint32_t faction_id);
OPENSMACX_API BOOL __cdecl eligible(uint32_t faction_id);

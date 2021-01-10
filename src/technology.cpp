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
#include "temp.h"
#include "technology.h"
#include "alpha.h"
#include "base.h"
#include "council.h"
#include "game.h"
#include "general.h"
#include "map.h"
#include "random.h"
#include "strings.h"
#include "veh.h"

RulesTechnology *Technology = (RulesTechnology *)0x0094F358;
uint8_t *GameTechAchieved = (uint8_t *)0x009A6670;
RulesMandate *Mandate = (RulesMandate *)0x0094B4A0;
int *TechValidCount = (int *)0x00949730;
int *TechCommerceCount = (int *)0x00949734;
char TechName[80];

/*
Purpose: Get tech string for techID and store it into stringTemp buffer.
Original Offset: 005591A0
Return Value: n/a
Status: Complete
*/
void __cdecl say_tech(int tech_id, BOOL category_lvl) {
	say_tech(stringTemp->str, tech_id, category_lvl);
}

/*
Purpose: Determine whether a faction is able to jump up the tech tree for the specified tech.
Original Offset: 0057CE50
Return Value: Is a tech leap possible? true/false
Status: Complete
*/
BOOL __cdecl valid_tech_leap(uint32_t tech_id, uint32_t faction_id) {
	int preq_1 = Technology[tech_id].preq_tech_1;
	if (preq_1 >= 0 && !(has_tech(Technology[preq_1].preq_tech_1, faction_id)
		&& has_tech(Technology[preq_1].preq_tech_2, faction_id))) {
		return false; // doesn't have 1st prerequisite prerequisites
	}
	int preq_2 = Technology[tech_id].preq_tech_2;
	if (preq_2 >= 0 && !(has_tech(Technology[preq_2].preq_tech_1, faction_id)
		&& has_tech(Technology[preq_2].preq_tech_2, faction_id))) {
		return false; // doesn't have 2nd prerequisite prerequisites
	}
	if (preq_1 <= TechDisabled || preq_2 <= TechDisabled) {
		return false; // disabled
	}
	for (uint32_t i = 0; i < MaxReactorNum; i++) {
		if ((int)tech_id == Reactor[i].preq_tech) {
			return false; // leap not possible for reactor tech
		}
	}
	for (uint32_t i = 0; i < MaxWeaponNum; i++) {
		if (Weapon[i].preq_tech == (int)tech_id) { // may end early if two weapons have the same preq
			return (PlayersData[faction_id].ranking <= 2 // lowest two ranking factions
				// this line is an odd comparison (offensive rating <= best weapon id + 2)
				// however it might be to prevent leaps for later weapon tech
				&& Weapon[i].offense_rating <= (weapon_budget(faction_id, 99, false) + 2));
		}
	}
	return true;
}

/*
Purpose: Craft an output string related to a specific technology. For techIDs outside the standard
		 range, craft a string related to world map, comm links or prototypes.
Original Offset: 005B9C40
Return Value: n/a
Status: Complete
*/
void __cdecl say_tech(LPSTR output, int tech_id, BOOL category_lvl) {
	if (tech_id < -1) {
		strcat_s(output, 80, label_get(310)); // "Not Available"
	} else if (tech_id < 0) {
		strcat_s(output, 80, label_get(25)); // "NONE"
	} else if (tech_id == 9999) {
		strcat_s(output, 80, label_get(306)); // "World Map"
	} else if (tech_id < MaxTechnologyNum) {
		strcat_s(output, 80, StringTable->get((int)Technology[tech_id].name));
		if (category_lvl) {
			sprintf_s(&output[strlen(output)], 80, " (%s%d)", // 'E#', 'D#', 'B#', 'C#'
				label_get(629 + tech_category(tech_id)), tech_recurse(tech_id, 0));
		}
	} else if (tech_id < 97) {
		if (*Language) {
			sprintf_s(&output[strlen(output)], 80, "%s (%s)", label_get(487), // 'Comm Frequency'
				get_noun(tech_id - MaxTechnologyNum));
		} else {
			sprintf_s(&output[strlen(output)], 80, "%s %s", 
                Players[tech_id - MaxTechnologyNum].adj_name_faction,
				label_get(487)); // 'Comm Frequency'
		}
	} else {
		sprintf_s(&output[strlen(output)], 80, "%s %s", VehPrototypes[tech_id - 97].veh_name,
			label_get(185)); // 'Prototype'
	}
}

/*
Purpose: Get tech string for tech_id and store it into TechName buffer.
Original Offset: 005B9EF0
Return Value: Pointer to TechName
Status: Complete
*/
LPSTR __cdecl tech_name(int tech_id, BOOL category_lvl) {
	TechName[0] = 0;
	say_tech(TechName, tech_id, category_lvl);
	return TechName;
}

/*
Purpose: Check whether faction has a particular tech or not.
Original Offset: 005B9F20
Return Value: Faction has tech? true/false
Status: Complete
*/
BOOL __cdecl has_tech(int tech_id, int faction_id) {
	if (faction_id <= 0) {
		return false;
	}
	if (tech_id == TechNone) {
		return true;
	}
	if (tech_id < 0 || tech_id >= (MaxTechnologyNum - 1) // excludes 'Transcendent Thought'
		|| Technology[tech_id].preq_tech_1 == TechDisabled
		|| (Technology[tech_id].preq_tech_2 == TechDisabled
			&& Technology[tech_id].preq_tech_1 != TechNone)) {
		// "none, disable" ; valid #TECH preq_tech entry
		return false;
	}
	return ((1 << faction_id) & GameTechAchieved[tech_id]) != 0;
}

/*
Purpose: Determine technology level for tech_id.
Original Offset: 005B9F90
Return Value: Level
Status: Complete
*/
int __cdecl tech_recurse(int tech_id, int base_lvl) {
	if (tech_id < 0 || tech_id >= MaxTechnologyNum) {
		return base_lvl;
	}
	int val1 = tech_recurse(Technology[tech_id].preq_tech_1, base_lvl + 1);
	int val2 = tech_recurse(Technology[tech_id].preq_tech_2, base_lvl + 1);
	return (val1 > val2) ? val1 : val2;
}

/*
Purpose: Determine what category is dominate for tech_id. If there is a tie, the order of precedence
		 is as follows: growth > tech > wealth > power.
Original Offset: 005B9FE0
Return Value: Tech category id: growth (0), tech (1), wealth (2) or power (3).
Status: Complete
*/
int __cdecl tech_category(uint32_t tech_id) {
	uint32_t category = 0;
	int compare = Technology[tech_id].growth_value;
	if (Technology[tech_id].tech_value > compare) {
		category = 1;
		compare = Technology[tech_id].tech_value;
	}
	if (Technology[tech_id].wealth_value > compare) {
		category = 2;
		compare = Technology[tech_id].wealth_value;
	}
	return (Technology[tech_id].power_value > compare) ? 3 : category;
}

/*
Purpose: Check to see whether provided faction can research a specific technology. Checks are
		 included to prevent SMACX specific Veh from being built in SMAC mode.
Original Offset: 005BAC20
Return Value: Is tech available to faction? true/false
Status: Complete
*/
BOOL __cdecl tech_avail(uint32_t tech_id, int faction_id) {
	if (has_tech(tech_id, faction_id) || tech_id >= MaxTechnologyNum || (!*ExpansionEnabled
		&& (tech_id == TECH_PRPSYCH || tech_id == TECH_FLDMOD || tech_id == TECH_ADAPDOC
			|| tech_id == TECH_ADAPECO || tech_id == TECH_BIOADAP || tech_id == TECH_SENTRES
			|| tech_id == TECH_SECMANI || tech_id == TECH_NEWMISS || tech_id == TECH_BFG9000))) {
		return false;
	}
    int preq_tech_1 = Technology[tech_id].preq_tech_1;
    int preq_tech_2 = Technology[tech_id].preq_tech_2;
	if (preq_tech_1 < TechNone || preq_tech_2 < TechNone) {
		return false; // if either prerequisite tech is set to disabled (-2)
	}
	return (has_tech(preq_tech_1, faction_id) && has_tech(preq_tech_2, faction_id));
}

/*
Purpose: Calculate faction's tech related bonuses for commerce and resource production in fungus.
Original Offset: 005BAE60
Return Value: n/a
Status: Complete
*/
void __cdecl tech_effects(uint32_t faction_id) {
	PlayersData[faction_id].tech_commerce_bonus = Players[faction_id].rule_commerce;
	PlayersData[faction_id].tech_fungus_nutrient = 0;
	PlayersData[faction_id].tech_fungus_mineral = 0;
	PlayersData[faction_id].tech_fungus_energy = 0;
	PlayersData[faction_id].tech_fungus_unk = 0;
	for (uint32_t tech_id = 0; tech_id < MaxTechnologyNum; tech_id++) {
		if (has_tech(tech_id, faction_id)) {
			uint32_t flags = Technology[tech_id].flags;
			if (flags & TFLAG_INC_ENERGY_FUNGUS) {
				PlayersData[faction_id].tech_fungus_energy++;
			}
			if (flags & TFLAG_INC_MINERALS_FUNGUS) {
				PlayersData[faction_id].tech_fungus_mineral++;
			}
			if (flags & TFLAG_INC_NUTRIENT_FUNGUS) {
				PlayersData[faction_id].tech_fungus_nutrient++;
			}
			if (flags & TFLAG_INC_COMMERCE) {
				PlayersData[faction_id].tech_commerce_bonus++;
			}
		}
	}
	for (int i = 0; i < Players[faction_id].faction_bonus_count; i++) {
		if (Players[faction_id].faction_bonus_id[i] == RULE_FUNGNUTRIENT) {
			PlayersData[faction_id].tech_fungus_nutrient += Players[faction_id].faction_bonus_val1[i];
		} else if (Players[faction_id].faction_bonus_id[i] == RULE_FUNGMINERALS) {
			PlayersData[faction_id].tech_fungus_mineral += Players[faction_id].faction_bonus_val1[i];
		} else if (Players[faction_id].faction_bonus_id[i] == RULE_FUNGENERGY) {
			PlayersData[faction_id].tech_fungus_energy += Players[faction_id].faction_bonus_val1[i];
		}
	}
	// if values are below zero, cap at zero
	for (int *tech_fungus = &PlayersData[faction_id].tech_fungus_nutrient,
		*end = &PlayersData[faction_id].tech_fungus_unk; tech_fungus <= end; tech_fungus++) {
		if (*tech_fungus < 0) {
			*tech_fungus = 0;
		}
	}
	if (PlayersData[faction_id].soc_effect_pending.economy > 2) {
		PlayersData[faction_id].tech_commerce_bonus++;
		if (PlayersData[faction_id].soc_effect_pending.economy > 3) {
			PlayersData[faction_id].tech_commerce_bonus++;
			if (PlayersData[faction_id].soc_effect_pending.economy > 4) {
				PlayersData[faction_id].tech_commerce_bonus++;
			}
		}
	}
}

/*
Purpose: Determine if preqTechID is a prerequisite of parentTechID within descending range.
Original Offset: 005BCB60
Return Value: Is preqTechID prerequisite of parentTechID? true/false
Status: Complete
*/
BOOL __cdecl tech_is_preq(int preq_tech_id, int parent_tech_id, uint32_t range) {
	if (preq_tech_id < 0 || parent_tech_id < 0) {
		return false;
	}
	if (preq_tech_id == parent_tech_id) {
		return true;
	}
	if (!range) {
		return false;
	}
	return tech_is_preq(preq_tech_id, Technology[parent_tech_id].preq_tech_1, range - 1)
		|| tech_is_preq(preq_tech_id, Technology[parent_tech_id].preq_tech_2, range - 1);
}

/*
Purpose: Determine how valuable the specified techID is to a faction. This id either corresponds to
		 a technology (0-88), another faction (89-96) or a prototype (97-608). The 3rd parameter
		 determines whether a simplistic or extended calculation is required for a technology id.
Original Offset: 005BCBE0
Return Value: Value of techID to the specified faction
Status: Complete
*/
int __cdecl tech_val(uint32_t tech_id, int faction_id, BOOL simple_calc) {
	if (tech_id == 9999) {
		return 2;
	}
	int value_ret;
	if (tech_id < MaxTechnologyNum) {
		uint32_t vendetta_count = 0;
		for (int i = 1; i < MaxPlayerNum; i++) {
			if (i != faction_id && has_treaty(faction_id, i, DTREATY_VENDETTA)) {
                vendetta_count++;
			}
		}
		uint32_t factor_ai = 1;
		if (!simple_calc) {
            factor_ai = (*GameRules & RULES_BLIND_RESEARCH) ? 4 : 2;
		}
        BOOL ai_growth = PlayersData[faction_id].ai_growth;
        BOOL ai_power = PlayersData[faction_id].ai_power;
        BOOL ai_wealth = PlayersData[faction_id].ai_wealth;
        BOOL ai_tech = PlayersData[faction_id].ai_tech;
        int growth = Technology[tech_id].growth_value;
        int power = Technology[tech_id].power_value;
        int wealth = Technology[tech_id].wealth_value;
        int tech = Technology[tech_id].tech_value;
        value_ret = growth * (factor_ai * ai_growth + 1) + wealth * (factor_ai * ai_wealth + 1)
			+ tech * (factor_ai * ai_tech + 1) + power * (factor_ai * ai_power + 1);
		uint32_t base_count = PlayersData[faction_id].current_num_bases;
		if ((!power || !ai_power && !vendetta_count) && (!tech || !ai_tech)
			&& (!wealth || !ai_wealth) && (!growth || !ai_growth && base_count >= 4)) {
            value_ret = (value_ret + 1) / 2;
		}
		BOOL is_human_player = is_human(faction_id);
		if (!is_human_player && !has_tech(tech_id, faction_id) && simple_calc) {
			uint32_t achieved_count = bit_count(GameTechAchieved[tech_id]);
			if (achieved_count > 1) {
                value_ret += 2 - 2 * achieved_count; // increase priority if more factions have tech 
			}
			int search_lvl = 1;
			for (int i = 0; i < MaxTechnologyNum; i++) {
				if (has_tech(i, faction_id)) {
					int current_lvl = tech_recurse(i, 0);
					if (search_lvl < current_lvl) {
                        search_lvl = current_lvl;
					}
				}
			}
            int tech_id_lvl = tech_recurse(tech_id, 0);
			if (tech_id_lvl < search_lvl) {
                value_ret = value_ret * (tech_id_lvl + 1) / (search_lvl + 1);
			}
			if (value_ret < 1) {
                value_ret = 1;
			}
		}
		if (simple_calc) {
			return value_ret;
		}
		if (base_count) {
			for (int region = 1; region < MaxRegionLandNum; region++) {
				if (!bad_reg(region)) {
					uint32_t pwr_base = PlayersData[faction_id].region_total_bases[region] * power;
					uint8_t plan = PlayersData[faction_id].region_base_plan[region];
					if (plan == PLAN_NAVAL_TRANSPORT && vendetta_count && !is_human_player) {
                        value_ret += (pwr_base / base_count);
					} else if (plan == PLAN_DEFENSIVE) {
                        value_ret += (pwr_base * 4) / (base_count * (is_human_player + 1));
					} else if (plan == PLAN_OFFENSIVE) {
                        value_ret += (pwr_base * ((PlayersData[faction_id].best_weapon_value
							>= PlayersData[faction_id].enemy_best_weapon_value) ? 2 : 4))
							/ (base_count * (is_human_player + 1));
					} else {
						for (int i = 1; i < MaxPlayerNum; i++) {
							if (i != faction_id && PlayersData[i].region_total_bases[region]
								&& PlayersData[faction_id].region_total_bases[region]
								&& has_treaty(faction_id, i, DTREATY_COMMLINK)
								&& (!has_treaty(faction_id, i, DTREATY_PACT | DTREATY_TREATY)
									|| has_treaty(faction_id, i, DTREATY_WANT_REVENGE))) {
                                value_ret += (pwr_base / (base_count * (is_human_player + 1)));
							}
						}
					}
				}
			}
		}
		if (has_tech(tech_id, faction_id)) {
			return value_ret; // should this be moved further up?
		}
		if (climactic_battle()
			&& tech_is_preq(tech_id, Facility[FAC_ASCENT_TO_TRANSCENDENCE].preq_tech, 2)) {
            value_ret *= 4;
		}
		if (PlayersData[faction_id].soc_effect_base.planet > 0 && ai_growth) {
			if (tech_is_preq(tech_id, TECH_CENTMED, 9999)) {
                value_ret *= 3;
			}
			if (tech_is_preq(tech_id, TECH_PLAECON, 9999)) {
                value_ret *= 2;
			}
			if (tech_is_preq(tech_id, TECH_ALPHCEN, 3)) {
                value_ret *= 2;
			}
		}
		if (PlayersData[faction_id].soc_effect_base.probe <= 0) {
			if (tech_is_preq(tech_id, Facility[FAC_HUNTER_SEEKER_ALGO].preq_tech, ai_tech + 2)) {
				if (!ai_power) {
                    value_ret *= 2;
				}
				if (ai_tech) {
                    value_ret *= 2;
				}
			}
		}
		if (ai_growth && tech_is_preq(tech_id, TECH_DOCINIT, 2)) {
            value_ret *= 2;
		}
		if ((ai_wealth || !*MapCloudCover) && tech_is_preq(tech_id, TECH_ENVECON, 9999)) {
            value_ret *= 2;
		}
		if (Technology[tech_id].flags & TFLAG_SECRETS && !GameTechAchieved[tech_id]
			&& !(*GameRules & RULES_BLIND_RESEARCH)) {
			value_ret *= (ai_power + 1) * 2;
		}
		if (Players[faction_id].rule_psi > 0) {
			if (tech_is_preq(tech_id, Facility[FAC_DREAM_TWISTER].preq_tech, 9999)) {
				value_ret *= 2;
			}
		} else {
			int preq_tech_fusion = Reactor[RECT_FUSION - 1].preq_tech;
			if ((int)tech_id == preq_tech_fusion) {
				value_ret *= 2;
			}
			if ((int)tech_id == Reactor[RECT_QUANTUM - 1].preq_tech) {
				value_ret *= 2;
			}
			if (tech_is_preq(tech_id, preq_tech_fusion, 9999)) {
				value_ret++;
			}
			if (tech_is_preq(tech_id, preq_tech_fusion, 1)
                && !(*GameRules & RULES_BLIND_RESEARCH)) {
				value_ret *= 2;
			}
		}
		int eco_dmg_unk = PlayersData[faction_id].unk_49 / range(base_count, 1, 9999);
		if (eco_dmg_unk > ai_power 
            && (tech_is_preq(tech_id, Facility[FAC_HYBRID_FOREST].preq_tech, 9999)
			|| tech_is_preq(tech_id, Facility[FAC_TREE_FARM].preq_tech, 9999)
			|| tech_is_preq(tech_id, Facility[FAC_CENTAURI_PRESERVE].preq_tech, 9999)
			|| tech_is_preq(tech_id, Facility[FAC_TEMPLE_OF_PLANET].preq_tech, 9999))) {
			value_ret += eco_dmg_unk;
		}
		if (Players[faction_id].rule_population > 0) {
			if (tech_is_preq(tech_id, Facility[FAC_HAB_COMPLEX].preq_tech, 9999)) {
				value_ret *= 2;
			} else if (tech_is_preq(tech_id, Facility[FAC_HABITATION_DOME].preq_tech, 9999)
				&& *TurnCurrentNum > 250) {
				value_ret = (value_ret * 3) / 2;
			}
		}
		if (ai_power) {
			for (int i = 0; i < MaxWeaponNum; i++) {
				if (Weapon[i].offense_rating) {
					int weap_preq_tech = Weapon[i].preq_tech;
					if ((int)tech_id == weap_preq_tech) {
						value_ret *= (is_human_player + 3);
					} else if (tech_is_preq(tech_id, weap_preq_tech, 1)) {
						value_ret *= (is_human_player + 2);
					}
				}
			}
		}
		if (ai_tech || !ai_power) {
			for (int i = 0; i < MaxTechnologyNum; i++) {
				if (!has_tech(i, faction_id) && Technology[i].flags & TFLAG_SECRETS
					&& !GameTechAchieved[i] && tech_is_preq(tech_id, i, 1)) {
					value_ret *= 3;
				}
			}
		}
		if (tech_is_preq(tech_id, VehPrototypes[BSC_FORMERS].preq_tech, 9999)
			&& !has_tech(VehPrototypes[BSC_FORMERS].preq_tech, faction_id)) {
			value_ret *= 2;
			if (is_human_player) {
				value_ret *= 2;
			}
		}
		if (tech_is_preq(tech_id, Chassis[CHSI_FOIL].preq_tech, 9999)
			&& !has_tech(Chassis[CHSI_FOIL].preq_tech, faction_id)) {
			BOOL search = false;
			for (int region = 1; region < MaxRegionLandNum; region++) {
				if (PlayersData[faction_id].region_total_bases[region]) {
					for (int i = 1; i < MaxPlayerNum; i++) {
						if (faction_id != i && !PlayersData[i].region_total_bases[region]) {
							search = true;
							break;
						}
					}
					if (search && PlayersData[faction_id].unk_79[region] 
                        >= Continents[region].tile_count) {
						value_ret *= 3;
						if (is_human_player) {
							value_ret *= 2;
						}
						break;
					}
				}
			}
			if (search) {
				value_ret = (value_ret * 2) + 4;
			}
		}
	} else if (tech_id < 97) { // factions
        uint32_t factor = 1;
        uint32_t faction_id_2 = tech_id - MaxTechnologyNum;
		if (!wants_to_attack_(faction_id, faction_id_2, 0)) {
			factor = 2;
		}
		if (!wants_to_attack_(faction_id_2, faction_id, 0)) {
			factor++;
		}
		value_ret = factor * (factor / (PlayersData[faction_id].ai_fight + 2));
	} else {  // prototypes
		uint32_t proto_id = tech_id - 97;
		value_ret = range(Weapon[VehPrototypes[proto_id].weapon_id].offense_rating, 1, 2)
			+ range(Armor[VehPrototypes[proto_id].armor_id].defense_rating, 1, 2)
			+ range(Chassis[VehPrototypes[proto_id].chassis_id].speed, 1, 2)
			+ VehPrototypes[proto_id].reactor_id - 2;
	}
	return value_ret;
}

/*
Purpose: Determine a tech the specified faction should research.
Original Offset: 005BDC10
Return Value: techID or -1
Status: Complete
*/
int __cdecl tech_ai(uint32_t faction_id) {
    int tech_id = -1;
    int search = -999;
	BOOL is_human_player = is_human(faction_id);
	for (int i = 0; i < MaxTechnologyNum; i++) {
		if (tech_avail(i, faction_id)) {
            int tech_value = tech_val(i, faction_id, false);
            int compare;
			if (*GameRules & RULES_BLIND_RESEARCH) {
				if (is_human_player && (PlayersData[faction_id].ai_growth
					|| PlayersData[faction_id].ai_wealth)
					&& i == VehPrototypes[BSC_FORMERS].preq_tech) {
					return i; // Direct human player research toward gaining Formers
				}
				int preq = tech_recurse(i, 0);
				tech_value = preq ? (tech_value << 8) / preq : 0;
			}
			compare = is_human_player ? random(0, tech_value + 1) // unsure why humans get random()
				: rnd(tech_value + 1, NULL); // vs AI rnd() ?
			if (compare > search) {
				search = compare;
                tech_id = i;
			}
		}
	}
	return tech_id;
}

/*
Purpose: Get power_value from technology struct for tech id.
Original Offset: 005BDD70
Return Value: power_value or 0 if tech_id is greater than max
Status: Complete
*/
int __cdecl tech_mil(uint32_t tech_id) {
    // TODO: why only this one returns 0 with other three returning 1? typo/bug?
    return (tech_id >= MaxTechnologyNum) ? 0 : *(&Technology[tech_id].power_value);
}

/*
Purpose: Get tech_value from technology struct for tech id.
Original Offset: 005BDD90
Return Value: tech_value or 1 if tech_id is greater than max
Status: Complete
*/
int __cdecl tech_tech(uint32_t tech_id) {
    return (tech_id >= MaxTechnologyNum) ? 1 : *(&Technology[tech_id].tech_value);
}

/*
Purpose: Get wealth_value from technology struct for tech id.
Original Offset: 005BDDC0
Return Value: wealth_value or 1 if tech_id is greater than max
Status: Complete
*/
int __cdecl tech_infra(uint32_t tech_id) {
    return (tech_id >= MaxTechnologyNum) ? 1 : *(&Technology[tech_id].wealth_value);
}

/*
Purpose: Get growth_value from technology struct for tech id.
Original Offset: 005BDDF0
Return Value: growth_value or 1 if tech_id is greater than max
Status: Complete
*/
int __cdecl tech_colonize(uint32_t tech_id) {
    return (tech_id >= MaxTechnologyNum) ? 1 : *(&Technology[tech_id].growth_value);
}

/*
Purpose: Calculate how much researching a tech will cost the specified faction.
Original Offset: 005BE6B0
Return Value: tech rate/cost
Status: Complete
*/
uint32_t __cdecl tech_rate(uint32_t faction_id) {
	if (PlayersData[faction_id].tech_cost >= 0) {
		return PlayersData[faction_id].tech_cost; // already set
	}
	if (!Rules->TechDiscoveryRatePctStd) {
		return 999999999; // max cost
	}
	uint32_t player_factor = range(PlayersData[faction_id].earned_techs_saved * 2
		- PlayersData[faction_id].unk_26 + PlayersData[faction_id].tech_ranking, 2, 9999);
	uint32_t top_factor = 0;
	for (uint32_t i = 1; i < MaxPlayerNum; i++) {
		uint32_t compare = PlayersData[i].earned_techs_saved * 2 + PlayersData[i].tech_ranking;
		if (compare > top_factor) {
            top_factor = compare;
		}
	}
    player_factor /= 2;
    top_factor /= 2;
	BOOL is_human_player = is_human(faction_id);
	uint32_t diff_factor = is_human_player ? PlayersData[faction_id].diff_level : *DiffLevelCurrent;
    diff_factor += (diff_factor < DLVL_LIBRARIAN);
    uint32_t diff_lvl = !is_human_player ? *DiffLevelCurrent : DLVL_LIBRARIAN;
    diff_factor = is_human_player ? diff_factor * 4 + 8 : 29 - diff_factor * 3;
    diff_factor = range(diff_factor, 12 - player_factor, player_factor + 12);
	uint32_t tech_stagnation = *GameRules & RULES_TECH_STAGNATION;
	uint32_t rule_factor = tech_stagnation | 0x40; // 64 or 96
	uint32_t fin_factor = range(player_factor - (*TurnCurrentNum / (rule_factor >> 3)),
		0, (diff_factor * (rule_factor >> 5)) >> 1) + diff_factor;
	int resch_base = range(PlayersData[faction_id].soc_effect_base.research, -1, 1);
	uint32_t discovery_rate = (fin_factor
		- range((top_factor - diff_lvl - player_factor + 7) / (8 - diff_lvl),
			0, diff_lvl * fin_factor / 10 + 1))
		* range(player_factor - resch_base, 1, 99999);
	if (Rules->TechDiscoveryRatePctStd != 100) {
        discovery_rate = 100 * discovery_rate / Rules->TechDiscoveryRatePctStd;
	}
	if (Players[faction_id].rule_techcost != 100) {
        discovery_rate = discovery_rate * Players[faction_id].rule_techcost / 100;
	}
	uint32_t cost = (discovery_rate * *MapAreaSqRoot) / 56;
	if (tech_stagnation) {
		cost += cost / 2; // Slower Rate of Research Discoveries
	}
	return range(cost, 1, 99999999);
}

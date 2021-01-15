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
#include "alpha.h"
#include "base.h"
#include "council.h"
#include "game.h"
#include "general.h"
#include "map.h"
#include "random.h"
#include "strings.h"
#include "technology.h"
#include "terraforming.h"
#include "text.h"
#include "textindex.h"
#include "veh.h"

LPCSTR AlphaxFileID = "ALPHAX";
LPCSTR ScriptTxtID = "SCRIPT";
LPSTR *Compass = (LPSTR *)0x00945D48;
LPSTR *Difficulty = (LPSTR *)0x0096C85C;
Label *Labels = (Label *)0x009B90F8;
RulesResourceinfo *ResourceInfo = (RulesResourceinfo *)0x00945F50; // see resource_info_id
RulesTimeControl *TimeControl = (RulesTimeControl *)0x0094F1B8;
RulesResource *Resource = (RulesResource *)0x00946158;
RulesEnergy *Energy = (RulesEnergy *)0x0094A318;
RulesBasic *Rules = (RulesBasic *)0x00949738;
RulesWorldbuilder *WorldBuilder = (RulesWorldbuilder *)0x009502A8;
AlphaIniPref *AlphaIniPrefs = (AlphaIniPref *)0x0094B464;
DefaultPref *DefaultPrefs = (DefaultPref *)0x0094B350;
uint32_t *Language = (uint32_t *)0x009BC054;

/*
Purpose: Convert the tech name string to a numeric tech id.
Original Offset: 00584D60
Return Value: Tech id; 'None' (-1); 'Disabled' (-2); or error (-2)
Status: Complete
*/
int __cdecl tech_name(LPSTR name) {
	purge_trailing(name);
	if (!_stricmp(name, "None")) {
		return NoneValue;
	}
	if (!_stricmp(name, "Disable")) {
		return DisabledValue;
	}
	for (int tech_id = 0; tech_id < MaxTechnologyNum; tech_id++) {
		if (!_stricmp(name, Technology[tech_id].id)) {
			return tech_id;
		}
	}
	parse_says(0, Txt->getFilePath(), -1, -1);
	parse_says(1, name, -1, -1);
	parse_says(2, *TextBufferGetPtr, -1, -1);
	X_pop("BADTECHKEY", NULL); // TODO: Fix crash bug if BADTECHKEY is triggered.
	return DisabledValue;
}

/*
Purpose: Convert the chassis name string to a numeric chassis id.
Original Offset: 00584E40
Return Value: Chassis id; 'None' (-1); 'Disabled' (-2); or error (0)
Status: Complete
*/
int __cdecl chas_name(LPSTR name) {
	purge_trailing(name);
	if (!_stricmp(name, "None")) {
		return NoneValue;
	}
	if (!_stricmp(name, "Disable")) {
		return DisabledValue;
	}
	for (int chas_id = 0; chas_id < MaxChassisNum; chas_id++) {
		if (!_stricmp(name, Chassis[chas_id].offsv_1_name)) {
			return chas_id;
		}
	}
	parse_says(0, Txt->getFilePath(), -1, -1);
	parse_says(1, name, -1, -1);
	parse_says(2, *TextBufferGetPtr, -1, -1);
	X_pop("BADCHASKEY", NULL); // TODO: Fix crash bug if BADTECHKEY is triggered.
	return 0;
}

/*
Purpose: Convert the weapon name string to a numeric weapon id.
Original Offset: 00584F40
Return Value: Weapon id; 'None' (-1); 'Disabled' (-2); or error (0)
Status: Complete
*/
int __cdecl weap_name(LPSTR name) {
	purge_trailing(name);
	if (!_stricmp(name, "None")) {
		return NoneValue;
	}
	if (!_stricmp(name, "Disable")) {
		return DisabledValue;
	}
	for (int weap_id = 0; weap_id < MaxWeaponNum; weap_id++) {
		if (!_stricmp(name, Weapon[weap_id].name_short)) {
			return weap_id;
		}
	}
	parse_says(0, Txt->getFilePath(), -1, -1);
	parse_says(1, name, -1, -1);
	parse_says(2, *TextBufferGetPtr, -1, -1);
	X_pop("BADWEAPKEY", NULL); // TODO: Fix crash bug if BADTECHKEY is triggered.
	return 0;
}

/*
Purpose: Convert the armor name string to a numeric armor id.
Original Offset: 00585030
Return Value: Armor id; 'None' (-1); 'Disabled' (-2); or error (0)
Status: Complete
*/
int __cdecl arm_name(LPSTR name) {
	purge_trailing(name);
	if (!_stricmp(name, "None")) {
		return NoneValue;
	}
	if (!_stricmp(name, "Disable")) {
		return DisabledValue;
	}
	for (int arm_id = 0; arm_id < MaxArmorNum; arm_id++) {
		if (!_stricmp(name, Armor[arm_id].name_short)) {
			return arm_id;
		}
	}
	parse_says(0, Txt->getFilePath(), -1, -1);
	parse_says(1, name, -1, -1);
	parse_says(2, *TextBufferGetPtr, -1, -1);
	X_pop("BADARMKEY", NULL); // TODO: Fix crash bug if BADTECHKEY is triggered.
	return DisabledValue;
}

/*
Purpose: Parse the current tech name inside the Txt item buffer into a tech id.
Original Offset: 00585150
Return Value: Tech id
Status: Complete
*/
int __cdecl tech_item() {
	text_get();
	return tech_name(text_item());
}

/*
Purpose: Parse the #RULES & #WORLDBUILDER sections inside the alpha(x).txt.
Original Offset: 00585170
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL __cdecl read_basic_rules() {
	if (text_open(AlphaxFileID, "RULES")) {
		return true;
	}
	Rules->move_rate_roads = text_get_number(1, 100);
	Rules->nutrient_req_citizen = text_get_number(0, 100);
	text_get();
	Rules->artillery_dmg_num = range(text_item_number(), 1, 1000);
	Rules->artillery_dmg_denom = range(text_item_number(), 1, 1000);
	Rules->artillery_max_rng = text_get_number(1, 8);
	Rules->max_airdrop_sans_orb_insert = text_get_number(1, 500);
	Rules->nutrient_cost_multi = text_get_number(1, 100);
	Rules->mineral_cost_multi = text_get_number(1, 100);
	Rules->tech_discovery_rate_pct_std = text_get_number(0, 1000);
	Rules->limit_mineral_mine_sans_road = text_get_number(0, 100);
	Rules->tgl_nutrient_effect_with_mine = text_get_number(-1, 0); // Weird logic behind -1/0 value
	Rules->min_base_size_specialists = text_get_number(0, 100);
	Rules->drones_genejack_factory = text_get_number(0, 100);
	Rules->pop_limit_sans_hab_complex = text_get_number(1, 100);
	Rules->pop_limit_sans_hab_dome = text_get_number(1, 100);
	Rules->extra_pct_cost_proto_land = text_get_number(0, 500);
	Rules->extra_pct_cost_proto_sea = text_get_number(0, 500);
	Rules->extra_pct_cost_proto_air = text_get_number(0, 500);
	text_get();
	Rules->psi_combat_ratio_atk[TRIAD_LAND] = range(text_item_number(), 1, 1000);
	Rules->psi_combat_ratio_def[TRIAD_LAND] = range(text_item_number(), 1, 1000);
	text_get();
	Rules->psi_combat_ratio_atk[TRIAD_SEA] = range(text_item_number(), 1, 1000);
	Rules->psi_combat_ratio_def[TRIAD_SEA] = range(text_item_number(), 1, 1000);
	text_get();
	Rules->psi_combat_ratio_atk[TRIAD_AIR] = range(text_item_number(), 1, 1000);
	Rules->psi_combat_ratio_def[TRIAD_AIR] = range(text_item_number(), 1, 1000);
	Rules->player_start_energy_reserve = text_get_number(0, 1000);
	Rules->combat_pct_base_def = text_get_number(-100, 1000);
	Rules->combat_pct_atk_road = text_get_number(-100, 1000);
	Rules->combat_pct_atk_higher_elev = text_get_number(-100, 1000);
	Rules->combat_pen_pct_atk_lwr_elev = text_get_number(-100, 1000);
	Rules->combat_pct_mobile_open_ground = text_get_number(-100, 1000);
	Rules->combat_pct_def_vs_mobile_rough = text_get_number(-100, 1000);
	Rules->combat_pct_infantry_vs_base = text_get_number(-100, 1000);
	Rules->combat_pen_pct_atk_airdrop = text_get_number(-100, 1000);
	Rules->combat_pct_fanatic_atk_bonus = text_get_number(-100, 1000);
	Rules->combat_pct_land_gun_vs_ship_art = text_get_number(-100, 1000);
	Rules->combat_pct_art_bonus_lvl_alt = text_get_number(-100, 1000);
	Rules->combat_pct_trance_def_vs_psi = text_get_number(-100, 1000);
	Rules->combat_pct_emp_song_atk_vs_psi = text_get_number(-100, 1000);
	Rules->combat_pen_pct_air_supr_vs_grnd = text_get_number(-100, 1000);
	Rules->combat_pct_air_supr_vs_air = text_get_number(-100, 1000);
	Rules->combat_pen_pct_non_cbt_def_vs_cbt = text_get_number(-100, 1000);
	Rules->combat_pct_com_jam_def_vs_mobl = text_get_number(-100, 1000);
	Rules->combat_pct_bonus_vs_ship_port = text_get_number(-100, 1000);
	Rules->combat_pct_aaa_bonus_vs_air = text_get_number(-100, 1000);
	Rules->combat_pct_def_range_sensor = text_get_number(-100, 1000);
	Rules->combat_pct_psi_atk_bonus_planet = text_get_number(-100, 1000);
	Rules->retool_pct_pen_prod_chg = text_get_number(0, 100);
	Rules->retool_strictness = text_get_number(0, 3); // Bug fix: increased max param to 3
	Rules->retool_exemption = text_get_number(0, 1000);
	Rules->min_turns_councils = text_get_number(0, 1000);
	Rules->minerals_harvesting_forest = text_get_number(0, 100);
	Rules->territory_max_dist_base = text_get_number(0, 100);
	Rules->turns_corner_gbl_energy_mrkt = text_get_number(1, 100);
	Rules->tech_improve_fungus_sqr = tech_item();
	Rules->tech_ease_fungus_movement = tech_item();
	Rules->tech_build_roads_fungus = tech_item();
	Rules->tech_two_spec_abilities = tech_item();
	Rules->tech_three_nutrients_sqr = tech_item();
	Rules->tech_three_minerals_sqr = tech_item();
	Rules->tech_three_energy_sqr = tech_item();
	Rules->tech_orb_insert_sans_spc_elev = tech_item();
	Rules->tech_mining_platform_bonus = tech_item();
	Rules->tech_economic_victory = tech_item();
	Rules->tgl_probe_can_steal_tech = text_get_number(0, 1); // Fix: Set min param to 0
	Rules->tgl_human_always_contact_net = text_get_number(0, 1); // Fix: Set min param to 0
	Rules->tgl_humans_always_contact_pbem = text_get_number(0, 1); // Fix: Set min param to 0
	Rules->max_pct_dmg_art_vs_unit_base_bnkr = text_get_number(10, 100);
	Rules->max_pct_dmg_art_vs_units_open = text_get_number(10, 100);
	Rules->max_pct_dmg_art_vs_units_sea = text_get_number(10, 100);
	text_get();
	Rules->freq_global_warming_num = range(text_item_number(), 0, 1000);
	Rules->freq_global_warming_denom = range(text_item_number(), 1, 1000);
	Rules->normal_starting_year = text_get_number(0, 999999);
	Rules->normal_end_year_low_three_diff = text_get_number(0, 999999);
	Rules->normal_end_year_high_three_diff = text_get_number(0, 999999);
	Rules->tgl_oblit_base_atrocity = text_get_number(0, 1); // Fix: Set min param to 0
	Rules->size_base_subspace_gen = text_get_number(1, 999); // SMACX only
	Rules->subspace_generators_needed = text_get_number(1, 999); // SMACX only
	if (text_open(AlphaxFileID, "WORLDBUILDER")) {
		return 1;
	}
	WorldBuilder->land_base = text_get_number(50, 4000);
	WorldBuilder->land_mod = text_get_number(0, 2000);
	WorldBuilder->continent_base = text_get_number(5, 1000);
	WorldBuilder->continent_mod = text_get_number(5, 1000);
	WorldBuilder->hills_base = text_get_number(0, 100);
	WorldBuilder->hills_mod = text_get_number(0, 100);
	WorldBuilder->plateau_base = text_get_number(0, 500);
	WorldBuilder->plateau_mod = text_get_number(0, 500);
	WorldBuilder->rivers_base = text_get_number(0, 100);
	WorldBuilder->rivers_rain_mod = text_get_number(0, 100);
	WorldBuilder->solar_energy = text_get_number(1, 64);
	WorldBuilder->thermal_band = text_get_number(1, 64);
	WorldBuilder->thermal_deviance = text_get_number(1, 64);
	WorldBuilder->global_warming = text_get_number(1, 64);
	WorldBuilder->sea_level_rises = text_get_number(1, 100);
	WorldBuilder->cloudmass_peaks = text_get_number(0, 20);
	WorldBuilder->cloudmass_hills = text_get_number(0, 20);
	WorldBuilder->rainfall_coeff = text_get_number(0, 8);
	WorldBuilder->deep_water = text_get_number(-100, 100);
	WorldBuilder->shelf = text_get_number(-100, 100);
	WorldBuilder->plains = text_get_number(-100, 100);
	WorldBuilder->beach = text_get_number(-100, 100);
	WorldBuilder->hills = text_get_number(0, 100);
	WorldBuilder->peaks = text_get_number(-100, 100);
	WorldBuilder->fungus = text_get_number(0, 5);
	text_get();
	WorldBuilder->cont_size_ratio1 = text_item_number();
	WorldBuilder->cont_size_ratio2 = text_item_number();
	WorldBuilder->cont_size_ratio3 = text_item_number();
	WorldBuilder->cont_size_ratio4 = text_item_number();
	WorldBuilder->cont_size_ratio5 = text_item_number();
	WorldBuilder->islands = text_get_number(1, 500);
	return false;
}

/*
Purpose: Parse the #TECHNOLOGY section inside the alpha(x).txt with a duplicate entry check.
Original Offset: 00585E30
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL __cdecl read_tech() {
	if (text_open(AlphaxFileID, "TECHNOLOGY")) {
		return true;
	}
	for (int i = 0; i < MaxTechnologyNum; i++) {
		text_get();
		text_item();
		text_item();
		strncpy_s(Technology[i].id, 8, *TextBufferItemPtr, strlen(*TextBufferItemPtr));
		Technology[i].id[7] = 0;
		for (int j = 0; j < i; j++) {
			if (!strcmp(Technology[i].id, Technology[j].id)) {
				parse_num(0, i);
				parse_num(1, j);
				parse_says(0, Technology[i].id, -1, -1);
				parse_says(1, FilefindPath->lastPath, -1, -1);
				parse_says(2, *TextBufferGetPtr, -1, -1);
				X_pop("DUPLICATETECH", NULL);
			}
		}
	}
	if (text_open(AlphaxFileID, "TECHNOLOGY")) {
		return true;
	}
	*TechValidCount = 0;
	*TechCommerceCount = 0;
	for (int i = 0; i < MaxTechnologyNum; i++) {
		text_get();
		Technology[i].name = text_item_string();
		text_item();
		Technology[i].power_value = text_item_number();
		Technology[i].tech_value = text_item_number();
		Technology[i].wealth_value = text_item_number();
		Technology[i].growth_value = text_item_number();
		Technology[i].preq_tech_1 = tech_name(text_item());
		Technology[i].preq_tech_2 = tech_name(text_item());
		Technology[i].flags = text_item_binary();
        if (Technology[i].preq_tech_1 != DisabledValue
            && Technology[i].preq_tech_2 != DisabledValue) {
			*TechValidCount += 1;
			if (Technology[i].flags & TFLAG_INC_COMMERCE) {
				*TechCommerceCount += 1;
			}
		}
	}
	return false;
}

/*
Purpose: Clear the rule values for the specified player.
Original Offset: 00585FE0
Return Value: n/a
Status: Complete
*/
void __cdecl clear_faction(Player *player) {
	player->rule_tech_selected = 0;
	player->rule_morale = 0;
	player->rule_research = 0;
	player->rule_drone = 0;
	player->rule_talent = 0;
	player->rule_energy = 0;
	player->rule_interest = 0;
	player->rule_population = 0;
	player->rule_hurry = 100;
	player->rule_techcost = 100;
	player->rule_psi = 0;
	player->rule_sharetech = 0;
	player->rule_commerce = 0;
	player->rule_flags = 0;
	player->faction_bonus_count = 0;
}

/*
Purpose: Parse the faction's file and art for the specified player id.
Original Offset: 00586050
Return Value: n/a
Status: Complete
*/
void __cdecl read_faction(uint32_t player_id) {
	if (player_id) {
		read_faction(&Players[player_id], 0);
		load_faction_art(player_id);
	}
}

/*
Purpose: Parse the 1st eight lines of the specified faction's file into a player structure. The 
         toggle parameter will end the function early if set to 2 (original code never uses this).
Original Offset: 00586090
Return Value: n/a
Status: Complete
*/
void __cdecl read_faction(Player *player, int toggle) {
	clear_faction(player);
	if (text_open(player->filename, player->search_key)
		&& text_open(player->filename, player->filename)) {
		parse_says(0, player->search_key, -1, -1);
		parse_says(1, player->filename, -1, -1);
		X_pop("PLAYERFILE", NULL);
		return;
	}
	text_get();
	strcpy_s(player->formal_name_faction, 40, text_item());
	player->formal_name_faction[39] = 0;
	strcpy_s(player->desc_name_faction, 24, text_item());
	player->desc_name_faction[23] = 0;
	strcpy_s(player->noun_faction, 24, text_item());
	player->noun_faction[23] = 0;
	LPSTR gender = text_item();
	player->noun_gender = GENDER_MALE;
	if (gender[0] == 'F' || gender[0] == 'f') {
		player->noun_gender = GENDER_FEMALE;
	} else if (gender[0] == 'N' || gender[0] == 'n') {
		player->noun_gender = GENDER_NEUTRAL;
	}
	player->is_noun_plural = range(text_item_number() - 1, false, true); // original value: 1 or 2
	strcpy_s(player->name_leader, 24, text_item());
	player->name_leader[23] = 0;
	gender = text_item();
	player->is_leader_female = (gender[0] == 'F' || gender[0] == 'f') ? true : false;
	if (toggle == 2) {
		return;
	}
	player->ai_fight = text_item_number();
	player->ai_power = text_item_number();
	player->ai_tech = text_item_number();
	player->ai_wealth = text_item_number();
	player->ai_growth = text_item_number();
	text_get();
	LPSTR parse_rule_check = text_item();
	int len = strlen(parse_rule_check);
	while (len) {
		LPSTR parse_rule = new char[len + 1];
		strcpy_s(parse_rule, len + 1, parse_rule_check);
		LPSTR parse_param = text_item();
		if (!_stricmp(parse_rule, BonusName[0].key)) { // TECH
			// will have issues if custom tech abbreviations starting with numbers are used
			int player_selected = atoi(parse_param);
			if (!player_selected && player->faction_bonus_count < 8) {
				player->faction_bonus_id[player->faction_bonus_count] = RULE_TECH;
				player->faction_bonus_val1[player->faction_bonus_count] = tech_name(parse_param);
				player->faction_bonus_count++;
			} else {
				player->rule_tech_selected = player_selected;
			}
		} else if (!_stricmp(parse_rule, BonusName[1].key)) { // MORALE
			if (parse_param[0] == '+') {
				parse_param++;
			}
			player->rule_morale = atoi(parse_param);
			// 0 indicates an exemption from negative modifiers from other sources
			if (!player->rule_morale) {
				player->rule_flags |= RFLAG_MORALE;
			}
		} else if (!_stricmp(parse_rule, BonusName[3].key) && player->faction_bonus_count < 8) {
            // FACILITY
			player->faction_bonus_id[player->faction_bonus_count] = RULE_FACILITY;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[4].key)) { // RESEARCH
			player->rule_research = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[5].key)) { // DRONE
			player->rule_drone = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[6].key)) { // TALENT
			player->rule_talent = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[7].key)) { // ENERGY
			player->rule_energy = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[8].key)) { // INTEREST
			player->rule_flags |= RFLAG_INTEREST;
			player->rule_interest = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[9].key)) { // COMMERCE
			player->rule_commerce = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[10].key)) { // POPULATION
			player->rule_population = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[11].key)) { // HURRY
			player->rule_hurry = range(atoi(parse_param), 1, 1000);
		} else if (!_stricmp(parse_rule, BonusName[13].key)) { // TECHCOST
			player->rule_techcost = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[12].key) && player->faction_bonus_count < 8) {
            // UNIT
			player->faction_bonus_id[player->faction_bonus_count] = RULE_UNIT;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[2].key)) { // PSI
			player->rule_psi = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[14].key)) { // SHARETECH
			player->rule_sharetech = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[15].key)) { // TERRAFORM
			player->rule_flags |= RFLAG_TERRAFORM;
		} else if ((!_stricmp(parse_rule, BonusName[16].key) // SOCIAL, ROBUST, IMMUNITY
            || !_stricmp(parse_rule, BonusName[17].key)
			|| !_stricmp(parse_rule, BonusName[18].key)) && player->faction_bonus_count < 8) {
            // Moved faction_bonus_count check to start rather than inner loop
			int value = 0;
			while (parse_param[0] == '+' || parse_param[0] == '-') {
				(parse_param[0] == '+') ? value++ : value--;
				parse_param++;
			}
			if (!value) { // cannot be zero
				value = 1;
			}
			for (int j = 0; j < MaxSocialEffectNum; j++) {
				if (!_stricmp(SocialEffects[j].set1, parse_param)) {
					if (!_stricmp(parse_rule, BonusName[17].key)) {
						player->faction_bonus_id[player->faction_bonus_count] = RULE_ROBUST;
					} else {
						player->faction_bonus_id[player->faction_bonus_count] =
							!_stricmp(parse_rule, BonusName[16].key) ? RULE_SOCIAL : RULE_IMMUNITY;
					}
					player->faction_bonus_val1[player->faction_bonus_count] = j; // soc effect id
					player->faction_bonus_val2[player->faction_bonus_count] = value; // value mod
					player->faction_bonus_count++;
					break;
				}
			}
		} else if ((!_stricmp(parse_rule, BonusName[19].key) // IMPUNITY, PENALTY
			|| !_stricmp(parse_rule, BonusName[20].key)) && player->faction_bonus_count < 8) {
            // Moved faction_bonus_count check to start rather than inner loop
			for (int j = 0; j < MaxSocialCatNum; j++) {
				for (int k = 0; k < MaxSocialModelNum; k++) {
					if (!_stricmp(parse_param,
						StringTable->get((int)SocialCategories[j].name[k]))) {
						player->faction_bonus_id[player->faction_bonus_count] =
							!_stricmp(parse_rule, BonusName[19].key) ? RULE_IMPUNITY : RULE_PENALTY;
						player->faction_bonus_val1[player->faction_bonus_count] = j; // category id
						player->faction_bonus_val2[player->faction_bonus_count] = k; // model id
						player->faction_bonus_count++;
					}
				}
			}
		} else if (!_stricmp(parse_rule, BonusName[21].key) && player->faction_bonus_count < 8) {
            // FUNGNUTRIENT
			player->faction_bonus_id[player->faction_bonus_count] = RULE_FUNGNUTRIENT;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[22].key) && player->faction_bonus_count < 8) {
            // FUNGMINERALS
			player->faction_bonus_id[player->faction_bonus_count] = RULE_FUNGMINERALS;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[23].key) && player->faction_bonus_count < 8) {
            // FUNGENERGY
			player->faction_bonus_id[player->faction_bonus_count] = RULE_FUNGENERGY;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[24].key)) { // COMMFREQ
			player->rule_flags |= RFLAG_COMMFREQ;
		} else if (!_stricmp(parse_rule, BonusName[25].key)) { // MINDCONTROL
			player->rule_flags |= RFLAG_MINDCONTROL;
		} else if (!_stricmp(parse_rule, BonusName[26].key)) { // FANATIC
			player->rule_flags |= RFLAG_FANATIC;
		} else if (!_stricmp(parse_rule, BonusName[27].key) && player->faction_bonus_count < 8) {
            // VOTES
			player->faction_bonus_id[player->faction_bonus_count] = RULE_VOTES;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[28].key)) { // FREEPROTO
			player->rule_flags |= RFLAG_FREEPROTO;
		} else if (!_stricmp(parse_rule, BonusName[29].key)) { // AQUATIC
			player->rule_flags |= RFLAG_AQUATIC;
		} else if (!_stricmp(parse_rule, BonusName[30].key)) { // ALIEN
			player->rule_flags |= RFLAG_ALIEN;
		} else if (!_stricmp(parse_rule, BonusName[31].key) && player->faction_bonus_count < 8) {
            // FREEFAC
			player->faction_bonus_id[player->faction_bonus_count] = RULE_FREEFAC;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[32].key) && player->faction_bonus_count < 8) {
            // REVOLT
			player->faction_bonus_id[player->faction_bonus_count] = RULE_REVOLT;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[33].key) && player->faction_bonus_count < 8) {
            // NODRONE
			player->faction_bonus_id[player->faction_bonus_count] = RULE_NODRONE;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[34].key)) { // WORMPOLICE
			player->rule_flags |= RFLAG_WORMPOLICE;
		} else if (!_stricmp(parse_rule, BonusName[35].key) && player->faction_bonus_count < 8) {
            // FREEABIL
			player->faction_bonus_id[player->faction_bonus_count] = RULE_FREEABIL;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[36].key) && player->faction_bonus_count < 8) {
            // PROBECOST
			player->faction_bonus_id[player->faction_bonus_count] = RULE_PROBECOST;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[37].key) && player->faction_bonus_count < 8) {
            // DEFENSE
			player->faction_bonus_id[player->faction_bonus_count] = RULE_DEFENSE;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[38].key) && player->faction_bonus_count < 8) {
            // OFFENSE
			player->faction_bonus_id[player->faction_bonus_count] = RULE_OFFENSE;
			player->faction_bonus_val1[player->faction_bonus_count] = atoi(parse_param);
			player->faction_bonus_count++;
		} else if (!_stricmp(parse_rule, BonusName[39].key)) { // TECHSHARE
			player->rule_flags |= RFLAG_TECHSHARE;
			player->rule_sharetech = atoi(parse_param);
		} else if (!_stricmp(parse_rule, BonusName[40].key)) { // TECHSTEAL
			player->rule_flags |= RFLAG_TECHSTEAL;
		}
		parse_rule_check = text_item();
		len = strlen(parse_rule_check);
		delete [] parse_rule;
	}
	// Societal Ideology + Anti-Ideology
	for (int i = 0; i < 2; i++) {
		*(&player->soc_ideology_category + i) = -1;
		*(&player->soc_ideology_model + i) = 0;
		*(&player->soc_ideology_effect + i) = -1;
		text_get();
		LPSTR soc_category = text_item();
		for (int j = 0; j < MaxSocialCatNum; j++) {
			LPSTR check_cat_type = StringTable->get((int)SocialCategories[j].type);
			if (*Language ? !_strnicmp(soc_category, check_cat_type, 4) 
                : !_stricmp(soc_category, check_cat_type)) {
				*(&player->soc_ideology_category + i) = j;
				break;
			}
		}
		LPSTR soc_model = text_item();
		int soc_cat_num = *(&player->soc_ideology_category + i);
		if (soc_cat_num >= 0) {
			for (int j = 0; j < MaxSocialModelNum; j++) {
				LPSTR check_model = StringTable->get((int)SocialCategories[soc_cat_num].name[j]);
				if (*Language ?
					!_strnicmp(soc_model, check_model, 4) : !_stricmp(soc_model, check_model)) {
					*(&player->soc_ideology_model + i) = j;
					break;
				}
			}
		}
		LPSTR soc_effect = text_item();
		for (int j = 0; j < MaxSocialEffectNum; j++) {
			if (!_stricmp(SocialEffects[j].set1, soc_effect)) {
				// Bug fix: Original code sets this value to -1, disabling AI faction "Emphasis"
				// value. No indication this was intentional.
				*(&player->soc_ideology_effect + i) = j;
				break;
			}
		}
	}
	// Faction and Leader related strings
	text_get(); // skips 2nd value in this line, abbreviation unused?
	strcpy_s(player->adj_name_faction, 128, text_item());
	player->adj_name_faction[127] = 0;
	text_get();
	strcpy_s(player->assistant_name, 24, text_item());
	player->assistant_name[23] = 0;
	strcpy_s(player->scientist_name, 24, text_item());
	player->scientist_name[23] = 0;
	strcpy_s(player->assistant_city, 24, text_item());
	player->assistant_city[23] = 0;
	text_get();
	strcpy_s(player->title_leader, 24, text_item());
	player->title_leader[23] = 0;
	strcpy_s(player->adj_leader, 128, text_item());
	player->adj_leader[127] = 0;
	strcpy_s(player->adj_insult_leader, 128, text_item());
	player->adj_insult_leader[127] = 0;
	strcpy_s(player->adj_faction, 128, text_item());
	player->adj_faction[127] = 0;
	strcpy_s(player->adj_insult_faction, 128, text_item());
	player->adj_insult_faction[127] = 0;
	text_get();
	strcpy_s(player->insult_leader, 24, text_item());
	player->insult_leader[23] = 0;
}

/*
Purpose: Parse the #BONUSNAMES, #FACTIONS, and #NEWFACTIONS sections inside the alpha(x).txt.
Original Offset: 00586F30
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL __cdecl read_factions() {
	if (text_open(AlphaxFileID, "BONUSNAMES")) {
		return true;
	}
	for (uint32_t i = 0; i < MaxBonusNameNum; i++) {
		if (!(i % 8)) { // 8 entries per line
			text_get();
		}
		strncpy_s(BonusName[i].key, text_item(), 24);
	}
	if (text_open(AlphaxFileID, ExpansionEnabled ? "NEWFACTIONS" : "FACTIONS")) {
		return true;
	}
	for (int i = 1; i < MaxPlayerNum; i++) {
		text_get();
		strncpy_s(Players[i].filename, text_item(), 24);
		strncpy_s(Players[i].search_key, text_item(), 24);
	}
	// SMACX only: Will override any values parsed from alphax.txt #NEWFACTIONS if set in ini;
	prefs_fac_load(); // Removed an extra SMACX_Enabled check around call since there is one inside
	uint32_t faction_count = 14;
	if (!text_open(AlphaxFileID, "CUSTOMFACTIONS")) { // get count of custom factions
		text_get();
		for (LPSTR custom = text_item(); *custom; custom = text_item()) {
			faction_count++;
			text_get();
		}
	}
	for (int i = 1; i < MaxPlayerNum; i++) {
		if (!strcmp(Players[i].filename, "JENN282")) {
			int faction_id;
			do {
				int rand_val = random(0, faction_count); // replaced rand()
				uint32_t faction_set = rand_val / 7; // 0: SMAC; 1: SMACX; 2+: custom
				if (text_open(AlphaxFileID, !faction_set ? "FACTIONS" : (faction_set == 1)
					? "NEWFACTIONS" : "CUSTOMFACTIONS")) {
					return true;
				}
				faction_id = rand_val % 7;
				for (int j = faction_id; j >= 0; j--) {
					text_get();
				}
				strcpy_s(Players[i].filename, 24, text_item());
				strcpy_s(Players[i].search_key, 24, text_item()); // original copied filename twice
				for (int k = 1; k < MaxPlayerNum; k++) {
					if (i != k) {
						if (!strcmp(Players[i].filename, Players[k].filename)) {
							faction_id = -1;
							break;
						}
					}
				}
				if (faction_id != -1) { // skip Players[0] like below check
					read_faction(&Players[i], 0);
					load_faction_art(i);
				}
			} while (faction_id == -1);
		} else {
			// removed check (&Players[i] != &Players[0]) since Players[0] is already skipped
			// moved this into same loop to increase performance with random factions
			read_faction(&Players[i], 0);
			load_faction_art(i);
		}
	}
	return false;
}

/*
Purpose: Parse and set the noun item's gender and plurality from the Txt buffer.
Original Offset: 005871D0
Return Value: n/a
Status: Complete
*/
void __cdecl noun_item(uint32_t *gender, BOOL *plurality) {
	LPSTR noun = text_item();
	*gender = 0; // defaults to male ('m' || 'M')
	*plurality = false; // defaults to singular ('1')
	if (noun[0] == 'f' || noun[0] == 'F') {
		*gender = 1;
	} else if (noun[0] == 'n' || noun[0] == 'N') {
		*gender = 2;
	}
	if (noun[1] == '2') {
		*plurality = true;
	}
}

/*
Purpose: Parse the #UNITS section inside the alpha(x).txt.
Original Offset: 00587240
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL __cdecl read_units() {
	if (text_open(AlphaxFileID, "UNITS")) {
		return true;
	}
	int total_units = text_get_number(0, MaxVehProtoFactionNum);
	for (int proto_id = 0; proto_id < total_units; proto_id++) {
		text_get();
		LPSTR name = text_item();
		strncpy_s(VehPrototypes[proto_id].veh_name, 32, name, strlen(name));
		int chas_id = chas_name(text_item());
		int weap_id = weap_name(text_item());
		int armor_id = arm_name(text_item());
		int plan = text_item_number();
		int cost = text_item_number();
		int carry = text_item_number();
		VehPrototypes[proto_id].preq_tech = (int16_t)tech_name(text_item());
		int icon = text_item_number();
		int ability = text_item_binary();
		int reactor_id = text_item_number(); // Add ability to read reactor for #UNITS
		if (!reactor_id) { // if not set or 0, default behavior
            switch (proto_id) {
              // There was a pointless explicit check for BSC_BATTLE_OGRE_MK1 to set reactor to 1
              // The parameters set by check are no different than default
              case BSC_BATTLE_OGRE_MK2:
                reactor_id = RECT_FUSION;
                break;
              case BSC_BATTLE_OGRE_MK3:
                reactor_id = RECT_QUANTUM;
                break;
              default:
                reactor_id = RECT_FISSION;
                break;
            }
        }
		make_proto(proto_id, chas_id, weap_id, armor_id, ability, reactor_id);
		// If set, override auto calculated values from make_proto()
		if (plan != -1) { // plan auto calculate: -1
			VehPrototypes[proto_id].plan = (uint8_t)plan;
		}
		if (cost) { // cost auto calculate: 0
			VehPrototypes[proto_id].cost = (uint8_t)cost;
		}
		if (carry) { // carry auto calculate: 0
			VehPrototypes[proto_id].carry_capacity = (uint8_t)carry;
		}
		VehPrototypes[proto_id].icon_offset = (int8_t)icon;
	}
	return false;
}

/*
Purpose: Parse in all the game rules via alpha/x.txt. If the toggle param is set to true, parse the
         #UNITS & #FACTIONS sections. Otherwise, skip both. New game vs reload?
Original Offset: 005873C0
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL __cdecl read_rules(BOOL tgl_all_rules) {
	StringTable->init(49952);
	if (labels_init()) {
		return true;
	}
	text_clear_index();
	text_make_index(ScriptTxtID);
	text_make_index(AlphaxFileID);
	if (read_tech() || read_basic_rules() || text_open(AlphaxFileID, "TERRAIN")) {
		return true;
	}
	for (int i = 0; i < MaxTerrainNum; i++) {
		text_get();
		// Land + sea terraforming
		for (int j = 0; j < 2; j++) {
			*(&Terraforming[i].name + j) = text_item_string();
			*(&Terraforming[i].preq_tech + j) = tech_name(text_item());
			// Add in bits & incompatible bits vs hard coded constant struct
			*(&Terraforming[i].bit + j) = TerraformingBits[i][j];
		}
		Terraforming[i].rate = text_item_number();
		// Land + sea orders
		LPSTR order_str = text_item();
		for (int j = 0; j < 2; j++) {
			parse_say(0, (int)*(&Terraforming[i].name + j), -1, -1);
			stringTemp->str[0] = 0;
			parse_string_OG(order_str, stringTemp->str); // TODO: replace
			*(&Order[i + 4].order + j) = StringTable->put(stringTemp->str);
		}
		Order[i + 4].letter = text_item_string();
		Terraforming[i].shortcuts = text_item_string();
	}
	if (text_open(AlphaxFileID, "RESOURCEINFO")) {
		return true;
	}
	for (int i = 0; i < MaxResourceInfoNum; i++) {
		text_get();
		text_item();
		ResourceInfo[i].nutrients = text_item_number();
		ResourceInfo[i].minerals = text_item_number();
		ResourceInfo[i].energy = text_item_number();
		ResourceInfo[i].unused = text_item_number();
	}
	if (text_open(AlphaxFileID, "TIMECONTROLS")) {
		return true;
	}
	for (int i = 0; i < MaxTimeControlNum; i++) {
		text_get();
		TimeControl[i].name = text_item_string();
		TimeControl[i].turn = text_item_number();
		TimeControl[i].base = text_item_number();
		TimeControl[i].unit = text_item_number();
		TimeControl[i].event = text_item_number();
		TimeControl[i].extra = text_item_number();
		TimeControl[i].refresh = text_item_number();
		TimeControl[i].accumulated = text_item_number();
	}
	if (text_open(AlphaxFileID, "CHASSIS")) {
		return true;
	}
	for (int i = 0; i < MaxChassisNum; i++) {
		text_get();
		Chassis[i].offsv_1_name = text_item_string();
		noun_item(&Chassis[i].offsv_1_gender, &Chassis[i].offsv_1_plural);
		Chassis[i].offsv_2_name = text_item_string();
		noun_item(&Chassis[i].offsv_2_gender, &Chassis[i].offsv_2_plural);
		Chassis[i].defsv_1_name = text_item_string();
		noun_item(&Chassis[i].defsv_1_gender, &Chassis[i].defsv_1_plural);
		Chassis[i].defsv_2_name = text_item_string();
		noun_item(&Chassis[i].defsv_2_gender, &Chassis[i].defsv_2_plural);
		Chassis[i].speed = (uint8_t)text_item_number();
		Chassis[i].triad = (uint8_t)text_item_number();
		Chassis[i].range = (uint8_t)text_item_number();
		Chassis[i].missile = (uint8_t)text_item_number();
		Chassis[i].cargo = (uint8_t)text_item_number();
		Chassis[i].cost = (uint8_t)text_item_number();
		Chassis[i].preq_tech = (int16_t)tech_name(text_item());
		Chassis[i].offsv_lrg_name = text_item_string();
		noun_item(&Chassis[i].offsv_lrg_gender, &Chassis[i].offsv_lrg_plural);
		Chassis[i].defsv_lrg_name = text_item_string();
		noun_item(&Chassis[i].defsv_lrg_gender, &Chassis[i].defsv_lrg_plural);
	}
	if (text_open(AlphaxFileID, "REACTORS")) {
		return true;
	}
	for (int i = 0; i < MaxReactorNum; i++) {
		text_get();
		Reactor[i].name = text_item_string();
		Reactor[i].name_short = text_item_string();
		// Bug fix/Enhancement: original function skips this value and is left as zero, isn't
		// referenced elsewhere in code. Likely because default power value is sequential.
		// This will allow future modifications.
		Reactor[i].power = (uint16_t)text_item_number();
		Reactor[i].preq_tech = (int16_t)tech_name(text_item());
	}
	if (text_open(AlphaxFileID, "WEAPONS")) {
		return true;
	}
	for (int i = 0; i < MaxWeaponNum; i++) {
		text_get();
		Weapon[i].name = text_item_string();
		Weapon[i].name_short = text_item_string();
		Weapon[i].offense_rating = (int8_t)text_item_number();
		Weapon[i].mode = (uint8_t)text_item_number();
		Weapon[i].cost = (uint8_t)text_item_number();
		Weapon[i].icon = (int8_t)text_item_number();
		Weapon[i].preq_tech = (int16_t)tech_name(text_item());
	}
	if (text_open(AlphaxFileID, "DEFENSES")) { // Armor
		return true;
	}
	for (int i = 0; i < MaxArmorNum; i++) {
		text_get();
		Armor[i].name = text_item_string();
		Armor[i].name_short = text_item_string();
		Armor[i].defense_rating = (char)text_item_number();
		Armor[i].mode = (uint8_t)text_item_number();
		Armor[i].cost = (uint8_t)text_item_number();
		Armor[i].preq_tech = (int16_t)tech_name(text_item());
	}
	if (text_open(AlphaxFileID, "ABILITIES")) {
		return true;
	}
	for (int i = 0; i < MaxAbilityNum; i++) {
		text_get();
		Ability[i].name = text_item_string();
		Ability[i].cost_factor = text_item_number();
		Ability[i].preq_tech = (int16_t)tech_name(text_item());
		Ability[i].abbreviation = text_item_string();
		Ability[i].flags = text_item_binary();
		Ability[i].description = text_item_string();
	}
	if (text_open(AlphaxFileID, "MORALE")) {
		return true;
	}
	for (int i = 0; i < MaxMoraleNum; i++) {
		text_get();
		Morale[i].name = text_item_string();
		Morale[i].name_lifecycle = text_item_string();
	}
	if (text_open(AlphaxFileID, "DEFENSEMODES")) {
		return true;
	}
	for (int i = 0; i < MaxDefenseModeNum; i++) {
		text_get();
		DefenseModes[i].name = text_item_string();
		DefenseModes[i].hyphened = text_item_string();
		DefenseModes[i].abbrev = text_item_string();
		DefenseModes[i].letter = text_item_string();
	}
	if (text_open(AlphaxFileID, "OFFENSEMODES")) {
		return true;
	}
	for (int i = 0; i < MaxOffenseModeNum; i++) {
		text_get();
		OffenseModes[i].name = text_item_string();
		OffenseModes[i].hyphened = text_item_string();
		OffenseModes[i].abbrev = text_item_string();
		OffenseModes[i].letter = text_item_string();
	}
	// Units basic prototypes (only if new game param1 boolean is set)
	// TODO: Potential bug, look into issues with Veh data persisting between loaded saved games?
	if (tgl_all_rules) {
		for (int i = 0; i < MaxVehProtoNum; i++) {
			VehPrototypes[i].veh_name[0] = 0;
			VehPrototypes[i].unk_1 = 0;
			VehPrototypes[i].flags = 0;
		}
		if (read_units()) {
			return true;
		}
	}
	if (text_open(AlphaxFileID, "FACILITIES")) {
		return true;
	}
	for (int i = 1; i < MaxFacilityNum; i++) { // Facility[0] is null
		text_get();
		Facility[i].name = text_item_string();
		Facility[i].cost = text_item_number();
		Facility[i].maint = text_item_number();
		Facility[i].preq_tech = tech_name(text_item());
		/*
		Enhancement: The original code explicitly sets this value to disabled (-2) overriding
		alpha/x.txt.  It states in #FACILITIES alpha/x.txt: "Free  = No longer supported". The
		original AC manual in Appendix 2 and official strategy guide both list the specific
		facilities being free with certain tech.  However, this mechanic could have been removed
		for balance reasons. Or maybe was dropped due to time constraints. There is code that
		checks this value and sets the free facility only for new bases built after discovering the
		tech. It looks like existing bases do not get it. Will have to review this more. For now,
		this mechanic will be included as is. You can revert to vanilla behavior by modifying the
		four entries below in alpha/x.txt #FACILITIES with free parameter set to Disabled.

		Recycling Tanks, 4, 0, Biogen, EcoEng2,   > free with "Adv.Ecological Engineering"
		Recreation Commons, 4, 1, Psych, SentEco, > free with "Sentient Econometrics"
		Energy Bank, 8, 1, IndEcon, QuanMac,      > free with "Quantum Machinery"
		Network Node, 8, 1, InfNet, HAL9000,      > free with "Self - Aware Machines"
		*/
		Facility[i].free_tech = tech_name(text_item());
		Facility[i].effect = text_item_string();
		if (i >= FacilitySPStart) {
			Facility[i].sp_ai_fight = text_item_number();
			Facility[i].sp_ai_power = text_item_number();
			Facility[i].sp_ai_tech = text_item_number();
			Facility[i].sp_ai_wealth = text_item_number();
			Facility[i].sp_ai_growth = text_item_number();
		}
	}
	if (text_open(AlphaxFileID, "ORDERS")) { // Basic
		return true;
	}
	for (int i = 0; i < MaxOrderNum; i++) {
		if (i < 4 || i > 23) { // Skipping over orders set by #TERRAIN
			text_get();
			Order[i].order = text_item_string();
			// Potential enhancement: Have separate string for sea
			Order[i].order_sea = Order[i].order;
			Order[i].letter = text_item_string();
		}
	}
	if (text_open(AlphaxFileID, "COMPASS")) {
		return true;
	}
	for (int i = 0; i < MaxCompassNum; i++) {
		text_get();
		Compass[i] = text_item_string();
	}
	if (text_open(AlphaxFileID, "PLANS")) {
		return true;
	}
	for (int i = 0; i < MaxPlanNum; i++) {
		text_get();
		// Future clean-up: Create structure with both short & full name vs split memory
		PlansShortName[i] = text_item_string();
		PlansFullName[i] = text_item_string();
	}
	if (text_open(AlphaxFileID, "TRIAD")) {
		return true;
	}
	for (int i = 0; i < MaxTriadNum; i++) {
		text_get();
		Triad[i] = text_item_string();
	}
	if (text_open(AlphaxFileID, "RESOURCES")) {
		return true;
	}
	for (int i = 0; i < MaxResourceNum; i++) {
		text_get();
		Resource[i].name_singular = text_item_string();
		Resource[i].name_plural = text_item_string();
	}
	if (text_open(AlphaxFileID, "ENERGY")) {
		return true;
	}
	for (int i = 0; i < MaxEnergyNum; i++) {
		text_get();
		Energy[i].abbrev = text_item_string();
		Energy[i].name = text_item_string();
	}
	if (text_open(AlphaxFileID, "CITIZENS")) {
		return true;
	}
	for (int i = 0; i < MaxCitizenNum; i++) {
		text_get();
		Citizen[i].singular_name = text_item_string();
		Citizen[i].plural_name = text_item_string();
		if (i < 7) {
			Citizen[i].preq_tech = tech_name(text_item());
			Citizen[i].obsol_tech = tech_name(text_item());
			Citizen[i].ops_bonus = text_item_number();
			Citizen[i].psych_bonus = text_item_number();
			Citizen[i].research_bonus = text_item_number();
		}
	}
	if (text_open(AlphaxFileID, "SOCIO")) {
		return true;
	}
	text_get();
	for (int i = 0; i < MaxSocialEffectNum; i++) {
		strcpy_s(SocialEffects[i].set1, 24, text_item());
	}
	text_get();
	for (int i = 0; i < MaxSocialEffectNum; i++) {
		strcpy_s(SocialEffects[i].set2, 24, text_item());
	}
	text_get();
	for (int i = 0; i < MaxSocialCatNum; i++) {
		SocialCategories[i].type = text_item_string();
	}
	for (int i = 0; i < MaxSocialCatNum; i++) {
		for (int j = 0; j < MaxSocialModelNum; j++) {
			text_get();
			SocialCategories[i].name[j] = text_item_string();
			SocialCategories[i].preq_tech[j] = tech_name(text_item());
			ZeroMemory(&SocialCategories[i].model_effect[j], sizeof(SocialEffect));
			LPSTR mod_value = text_item();
			int mod_len = strlen(mod_value);
			while (mod_len) {
				int value = 0;
				while (mod_value[0] == '+' || mod_value[0] == '-') {
					(mod_value[0] == '+') ? value++ : value--;
					mod_value++;
				}
				for (int k = 0; k < MaxSocialEffectNum; k++) {
					if (!_stricmp(mod_value, SocialEffects[k].set1)) {
						*(&SocialCategories[i].model_effect[j].economy + k) = value;
						break;
					}
				}
				mod_value = text_item();
				mod_len = strlen(mod_value);
			}
		}
	}
	if (text_open(AlphaxFileID, "DIFF")) { // Difficulty
		return true;
	}
	for (int i = 0; i < MaxDiffNum; i++) {
		text_get();
		Difficulty[i] = text_item_string();
	}
	if (tgl_all_rules && read_factions()) {
		return true;
	}
	if (text_open(AlphaxFileID, "MANDATE")) {
		return true;
	}
	for (int i = 0; i < MaxMandateNum; i++) {
		text_get();
		Mandate[i].name = text_item_string();
		Mandate[i].name_caps = text_item_string();
	}
	if (text_open(AlphaxFileID, "MOOD")) {
		return true;
	}
	for (int i = 0; i < MaxMoodNum; i++) {
		text_get();
		Mood[i] = text_item_string();
	}
	if (text_open(AlphaxFileID, "REPUTE")) {
		return true;
	}
	for (int i = 0; i < MaxReputeNum; i++) {
		text_get();
		Repute[i] = text_item_string();
	}
	if (text_open(AlphaxFileID, "MIGHT")) {
		return true;
	}
	for (int i = 0; i < MaxMightNum; i++) {
		text_get();
		Might[i].adj_start = text_item_string();
		Might[i].adj = text_item_string();
	}
	if (text_open(AlphaxFileID, "PROPOSALS")) {
		return true;
	}
	for (int i = 0; i < MaxProposalNum; i++) {
		text_get();
		Proposal[i].name = text_item_string();
		Proposal[i].preq_tech = tech_name(text_item());
		Proposal[i].description = text_item_string();
	}
	if (text_open(AlphaxFileID, "NATURAL")) {
		return true;
	}
	for (int i = 0; i < MaxNaturalNum; i++) {
		text_get();
		Natural[i].name = text_item_string();
		Natural[i].name_short = text_item_string();
	}
	// Revised original nested for loop code to be more efficient; Logic is still same.
	// Buttons used by "Edit Map" menus.
	for (int i = 0, j = 0; i < MaxTerrainNum; i++) {
		// excludes: Fungus (removal), Aquifer, Raise Land, Lower Land, Level Terrain
		if (Terraforming[i].bit) {
			MainInterfaceVar->flatButton[j++ + 17].set_bubble_text(
				StringTable->get((int)Terraforming[i].name)); // 17-31
		}
	}
	MainInterfaceVar->flatButton[32].set_bubble_text(
		StringTable->get((int)Natural[LM_JUNGLE].name_short));
	MainInterfaceVar->flatButton[33].set_bubble_text(
		StringTable->get((int)Natural[LM_DUNES].name_short));
	MainInterfaceVar->flatButton[34].set_bubble_text(
		StringTable->get((int)Natural[LM_URANIUM].name_short));
	MainInterfaceVar->flatButton[35].set_bubble_text(
		StringTable->get((int)Natural[LM_GEOTHERMAL].name_short));
	return false;
}

/*
Purpose: Attempt to read the setting's value from the ini file.
Original Offset: 0059D980
Return Value: Key's string value from the ini or default if not set
Status: Complete
*/
LPSTR __cdecl prefs_get(LPCSTR key_name, LPCSTR default_value, BOOL use_default) {
	if (use_default ||
		(GetPrivateProfileStringA("Alpha Centauri", "Prefs Format", "0", *TextBufferGetPtr, 256,
			".\\Alpha Centauri.ini"), atoi(*TextBufferGetPtr) != 12)) {
		strcpy_s(*TextBufferGetPtr, 256, default_value);
	} else {
		GetPrivateProfileStringA("Alpha Centauri", key_name, default_value, *TextBufferGetPtr, 256,
			".\\Alpha Centauri.ini");
	}
	return Txt->update();
}

/*
Purpose: Get the default value for the 1st set of preferences.
Original Offset: 0059DA20
Return Value: Default preferences
Status: Complete
*/
uint32_t __cdecl default_prefs() {
	uint32_t base_prefs = PREF_ADV_RADIO_BTN_NOT_SEL_SING_CLK | PREF_AUTO_FORMER_BUILD_ADV
		| PREF_AUTO_FORMER_PLANT_FORESTS | PREF_AUTO_END_MOVE_SPOT_VEH_WAR
		| PREF_AUTO_END_MOVE_SPOT_VEH_TRUCE | PREF_AUTO_END_MOVE_SPOT_VEH_TREATY
		| PREF_AUTO_AIR_VEH_RET_HOME_FUEL_RNG | PREF_BSC_DONT_QUICK_MOVE_ALLY_VEH
		| PREF_BSC_AUTO_DESIGN_VEH | PREF_BSC_MOUSE_EDGE_SCROLL_VIEW | PREF_AV_BACKGROUND_MUSIC
		| PREF_AV_SOUND_EFFECTS | PREF_MAP_SHOW_GRID | PREF_UNK_10
		| PREF_BSC_DONT_QUICK_MOVE_ENEMY_VEH | PREF_BSC_AUTOSAVE_EACH_TURN
		| PREF_AUTO_WAKE_VEH_TRANS_REACH_LAND;
	return prefs_get("Laptop", 0, false) ? base_prefs : base_prefs  // 0xA3E1DD16 : 0xBBE1DD96
		| PREF_AV_SECRET_PROJECT_MOVIES | PREF_AV_SLIDING_WINDOWS | PREF_AV_MAP_ANIMATIONS;
}

/*
Purpose: Get the default value for the 2nd set of preferences.
Original Offset: 0059DAA0
Return Value: Default preferences 2nd set
Status: Complete
*/
uint32_t __cdecl default_prefs2() {
	uint32_t base_prefs2 = MPREF_ADV_DETAIL_MAIN_MENUS | MPREF_BSC_AUTO_PRUNE_OBS_VEH
		| MPREF_AV_VOICEOVER_STOP_CLOSE_POPUP | MPREF_AV_VOICEOVER_TECH_FAC
		| MPREF_MAP_SHOW_BASE_NAMES | MPREF_MAP_SHOW_PROD_WITH_BASE_NAMES
		| MPREF_ADV_RIGHT_CLICK_POPS_UP_MENU | MPREF_ADV_QUICK_MOVE_VEH_ORDERS
		| MPREF_AUTO_FORMER_BUILD_SENSORS | MPREF_AUTO_FORMER_REMOVE_FUNGUS; // 0x327168 : 0x3A7168
	return prefs_get("Laptop", 0, false) ? base_prefs2 : base_prefs2 | MPREF_AV_SLIDING_SCROLLBARS;
}

/*
Purpose: Get the default value for the warning pop-up preferences.
Original Offset: 0059DB20
Return Value: Default warning preferences
Status: Complete
*/
uint32_t __cdecl default_warn() {
	return WARN_STOP_RANDOM_EVENT | WARN_STOP_ENERGY_SHORTAGE | WARN_STOP_MINERAL_SHORTAGE
		| WARN_STOP_STARVATION | WARN_STOP_BUILD_OUT_OF_DATE | WARN_STOP_UNK_100
		| WARN_STOP_NUTRIENT_SHORTAGE | WARN_STOP_GOLDEN_AGE | WARN_STOP_DRONE_RIOTS
		| WARN_STOP_NEW_FAC_BUILT; // 0x3C3A9
}

/*
Purpose: Get the default value for the rule related preferences.
Original Offset: 0059DB30
Return Value: Default rule preferences
Status: Complete
*/
uint32_t __cdecl default_rules() {
	return RULES_VICTORY_COOPERATIVE | RULES_VICTORY_TRANSCENDENCE | RULES_BLIND_RESEARCH
		| RULES_VICTORY_DIPLOMATIC | RULES_VICTORY_ECONOMIC | RULES_VICTORY_CONQUEST; // 0x1A0E
}

/*
Purpose: Attempt to read the setting's value from the ini file.
Original Offset: 0059DB40
Return Value: Key's integer value from the ini or default if not set
Status: Complete
*/
int __cdecl prefs_get(LPCSTR key_name, int default_value, BOOL use_default) {
	_itoa_s(default_value, stringTemp->str, 256, 10);
	if (use_default) {
		strcpy_s(*TextBufferGetPtr, 256, stringTemp->str);
	} else {
		GetPrivateProfileStringA("Alpha Centauri", key_name, stringTemp->str, *TextBufferGetPtr, 
            256, ".\\Alpha Centauri.ini");
	}
	return atoi(Txt->update());
}

/*
Purpose: Read the faction filenames and search for keys from the ini file (SMACX only). This has 
         the added effect of forcing the player's search_key to be set to the filename value. 
         Rewrote almost the entire function because of how terrible the original code logic was.
Original Offset: 0059DBD0
Return Value: n/a
Status: Complete
*/
void __cdecl prefs_fac_load() {
	if (ExpansionEnabled) {
		char returned_string[256];
		GetPrivateProfileStringA("Alpha Centauri", "Prefs Format", "0", returned_string, 256, 
            ".\\Alpha Centauri.ini");
		if (atoi(returned_string) == 12) {
			for (int i = 1; i < MaxPlayerNum; i++) {
				std::string faction = "Faction ";
				faction += std::to_string(i);
				GetPrivateProfileStringA("Alpha Centauri", faction.c_str(), Players[i].filename,
					returned_string, 256, ".\\Alpha Centauri.ini");
				strncpy_s(Players[i].filename, returned_string, 24);
				strncpy_s(Players[i].search_key, returned_string, 24);
			}
		} else {
			// use separate loop rather than check "Prefs Format" value each time in single loop
			for (int i = 1; i < MaxPlayerNum; i++) {
				strncpy_s(Players[i].search_key, Players[i].filename, 24);
			}
		}
	}
}

/*
Purpose: Load the most common preferences from the game's ini to globals.
Original Offset: 0059DCF0
Return Value: n/a
Status: Complete
*/
void __cdecl prefs_load(BOOL use_default) {
	set_language(prefs_get("Language", 0, false));
	prefs_get("Difficulty", 0, false);
	DefaultPrefs->difficulty = text_item_number();
	prefs_get("Map Type", 0, false);
	DefaultPrefs->map_type = text_item_number();
	prefs_get("Top Menu", 0, false);
	DefaultPrefs->top_menu = text_item_number();
	prefs_get("Faction", 1, false);
	DefaultPrefs->faction_id = text_item_number();
	uint32_t prefs = default_prefs();
	if (DefaultPrefs->difficulty < DLVL_TALENT) {
		prefs |= PREF_BSC_TUTORIAL_MSGS;
	}
	prefs_get("Preferences", prefs_get_binary(prefs).c_str(), use_default);
	AlphaIniPrefs->preferences = text_item_binary();
	prefs_get("More Preferences", prefs_get_binary(default_prefs2()).c_str(), use_default);
	AlphaIniPrefs->more_preferences = text_item_binary();
	prefs_get("Semaphore", "00000000", use_default);
	AlphaIniPrefs->semaphore = text_item_binary();
	prefs_get("Customize", 0, false);
	AlphaIniPrefs->customize = text_item_number();
	prefs_get("Rules", prefs_get_binary(default_rules()).c_str(), use_default);
	AlphaIniPrefs->rules = text_item_binary();
	prefs_get("Announce", prefs_get_binary(default_warn()).c_str(), use_default);
	AlphaIniPrefs->announce = text_item_binary();
	std::stringstream ss;
	for (uint32_t i = 0; i < 7; i++) {
		ss << (i ? "1, " : "2, ");
	}
	std::string custom_world_def = ss.str();
	prefs_get("Custom World", custom_world_def.c_str(), use_default);
	for (uint32_t i = 0; i < 7; i++) {
		AlphaIniPrefs->custom_world[i] = text_item_number();
	}
	prefs_get("Time Controls", 1, use_default);
	AlphaIniPrefs->time_controls = text_item_number();
}

/*
Purpose: Write the string value to the pref key of the ini.
Original Offset: 0059E510
Return Value: n/a
Status: Complete
*/
void __cdecl prefs_put(LPCSTR key_name, LPCSTR value) {
	WritePrivateProfileStringA("Alpha Centauri", key_name, value, ".\\Alpha Centauri.ini");
}

/*
Purpose: Write the value as either an integer or a binary string to the pref key inside the ini.
Original Offset: 0059E530
Return Value: n/a
Status: Complete
*/
void __cdecl prefs_put(LPCSTR key_name, int value, BOOL tgl_binary) {
	char temp[33];
	tgl_binary ? strcpy_s(temp, 33, prefs_get_binary(value).c_str()) : _itoa_s(value, temp, 33, 10);
	WritePrivateProfileStringA("Alpha Centauri", key_name, temp, ".\\Alpha Centauri.ini");
}

/*
Purpose: Save the most common preferences from memory to the game's ini. 
Original Offset: 0059E5D0
Return Value: n/a
Status: Complete
*/
void __cdecl prefs_save(BOOL save_factions) {
	prefs_put("Prefs Format", 12, false);
	prefs_put("Difficulty", DefaultPrefs->difficulty, false);
	prefs_put("Map Type", DefaultPrefs->map_type, false);
	prefs_put("Top Menu", DefaultPrefs->top_menu, false);
	prefs_put("Faction", DefaultPrefs->faction_id, false);
	prefs_put("Preferences", AlphaIniPrefs->preferences, true);
	prefs_put("More Preferences", AlphaIniPrefs->more_preferences, true);
	prefs_put("Semaphore", AlphaIniPrefs->semaphore, true);
	prefs_put("Announce", AlphaIniPrefs->announce, true);
	prefs_put("Rules", AlphaIniPrefs->rules, true);
	prefs_put("Customize", AlphaIniPrefs->customize, false);
	std::stringstream ss;
	for (uint32_t i = 0; i < 7; i++) {
		if (i != 0) {
			ss << ", "; // removed last trailing comma
		}
		ss << AlphaIniPrefs->custom_world[i];
	}
	std::string custom_world_str = ss.str();
	prefs_put("Custom World", custom_world_str.c_str());
	prefs_put("Time Controls", AlphaIniPrefs->time_controls, false);
	if (save_factions && *ExpansionEnabled) {
		for (uint32_t i = 1; i < MaxPlayerNum; i++) {
			sprintf_s(stringTemp->str, "Faction %d", i);
			prefs_put(stringTemp->str, Players[i].filename);
		}
	}
}

/*
Purpose: Set the internal game preference globals from the ini setting globals.
Original Offset: 0059E950
Return Value: n/a
Status: Complete
*/
void __cdecl prefs_use() {
	*GamePreferences = AlphaIniPrefs->preferences;
	*GameMorePreferences = AlphaIniPrefs->more_preferences;
	*GameWarnings = AlphaIniPrefs->announce;
}

/*
Purpose: Convert the specified value to a binary string for use by various preferences.
Original Offset: n/a
Return Value: Binary string
Status: Complete
*/
std::string __cdecl prefs_get_binary(int value) {
	char temp[33];
	temp[0] = 0;
	for (int shift = 31, non_pad = 0; shift >= 0; shift--) {
		if ((1 << shift) & value) {
			non_pad = 1;
			strcat_s(temp, 33, "1");
		} else if (non_pad || shift < 8) {
			strcat_s(temp, 33, "0");
		}
	}
	return temp;
}

/*
Purpose: Parse the #LABELS section inside the labels.txt file.
Original Offset: 00616A00
Return Value: Was there an error? true/false
Status: Complete
*/
BOOL __cdecl labels_init() {
	labels_shutdown();
	if (text_open("labels", "labels")) {
		return true;
	}
	text_get();
	Labels->count = text_item_number();
	Labels->strings_ptr = (LPSTR)mem_get(Labels->count * 4);
	if (!Labels->strings_ptr) {
		return true;
	}
	for (int i = 0; i < Labels->count; i++) {
		*((LPSTR *)Labels->strings_ptr + i) = text_string();
	}
	text_close();
	return false;
}

/*
Purpose: Shutdown or reset the labels global variable.
Original Offset: 006169D0
Return Value: n/a
Status: Complete
*/
void __cdecl labels_shutdown() {
	if (Labels->strings_ptr) {
		free(Labels->strings_ptr);
		Labels->strings_ptr = 0;
	}
	Labels->count = 0;
}

/*
Purpose: Set the game's language.
Original Offset: 00627100
Return Value: n/a
Status: Complete
*/
void __cdecl set_language(uint32_t language) {
	*Language = language;
}

/*
Purpose: Get the label string and concatenate it to the stringTemp buffer.
Original Offset: 005A5880
Return Value: n/a
Status: Complete
*/
void __cdecl say_label(int label_offset) {
	strcat_s(stringTemp->str, 1032, label_get(label_offset));
}

/*
Purpose: Get the label string from a label offset. Created to reduce code complexity.
Original Offset: n/a
Return Value: Pointer to label string
Status: Complete
*/
LPSTR __cdecl label_get(int label_offset) {
	return StringTable->get((int)*((LPSTR *)Labels->strings_ptr + label_offset));
}

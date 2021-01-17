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
#include "faction.h"

 /*
  * Parsing alpha/x.txt, 'Alpha Centauri.ini' and labels.txt
  */
enum RulesResourceinfoId {
    RSCINFO_OCEAN_SQ = 0,
    RSCINFO_BASE_SQ = 1,
    RSCINFO_BONUS_SQ = 2,
    RSCINFO_FOREST_SQ = 3,
    RSCINFO_RECYCLING_TANKS = 4,
    RSCINFO_IMPROVED_LAND = 5,
    RSCINFO_IMPROVED_SEA = 6,
    RSCINFO_MONOLITH = 7,
    RSCINFO_BOREHOLE_SQ = 8,
};

enum GameBaseWarningsBitfield {
    WARN_STOP_NEW_FAC_BUILT = 0x1,
    WARN_STOP_NON_COMBAT_VEH_BUILT = 0x2,
    WARN_STOP_PROTOTYPE_COMPLETE = 0x4,
    WARN_STOP_DRONE_RIOTS = 0x8,
    WARN_STOP_DRONE_RIOTS_END = 0x10,
    WARN_STOP_GOLDEN_AGE = 0x20,
    WARN_STOP_GOLDEN_AGE_END = 0x40,
    WARN_STOP_NUTRIENT_SHORTAGE = 0x80,
    WARN_STOP_UNK_100 = 0x100, // no text, not visible in PrefWin; set with default warning
    WARN_STOP_BUILD_OUT_OF_DATE = 0x200,
    WARN_STOP_COMBAT_VEH_BUILT = 0x400,
    WARN_STOP_POP_LIMIT_REACHED = 0x800,
    WARN_STOP_DELAY_IN_TRANSCEND = 0x1000,
    WARN_STOP_BUILT_VIA_GOV_QUEUE = 0x2000,
    WARN_STOP_STARVATION = 0x4000,
    WARN_STOP_MINERAL_SHORTAGE = 0x8000,
    WARN_STOP_ENERGY_SHORTAGE = 0x10000,
    WARN_STOP_RANDOM_EVENT = 0x20000,
};

enum GamePreferencesBitfield : uint32_t {
    PREF_BSC_PAUSE_END_TURN = 0x1,
    PREF_BSC_AUTOSAVE_EACH_TURN = 0x2,
    PREF_BSC_DONT_QUICK_MOVE_ENEMY_VEH = 0x4, // flag set when unchecked
    PREF_ADV_FAST_BATTLE_RESOLUTION = 0x8,
    PREF_UNK_10 = 0x10, // no text, not visible in PrefWin; set with default preferences
    PREF_BSC_TUTORIAL_MSGS = 0x20,
    //
    PREF_AV_MAP_ANIMATIONS = 0x80,
    PREF_MAP_SHOW_GRID = 0x100,
    PREF_MAP_SHOW_BASE_GRID = 0x200,
    PREF_AV_VOLUME_SFX_TOGGLE = 0x400,
    PREF_AV_SOUND_EFFECTS = 0x400, // not displayed
    PREF_AV_VOLUME_MUSIC_TOGGLE = 0x800,
    PREF_AV_BACKGROUND_MUSIC = 0x800, // not displayed
    PREF_BSC_MOUSE_EDGE_SCROLL_VIEW = 0x1000,
    //
    PREF_BSC_AUTO_DESIGN_VEH = 0x4000,
    PREF_BSC_DONT_QUICK_MOVE_ALLY_VEH = 0x8000, // flag set when unchecked
    PREF_AUTO_AIR_VEH_RET_HOME_FUEL_RNG = 0x10000,
    PREF_AUTO_FORMER_RAISE_LWR_TERRAIN = 0x20000,
    PREF_AV_INTERLUDES_DISABLED = 0x40000, // flag set when unchecked
    PREF_ADV_NO_CENTER_VEH_ORDERS = 0x80000,
    PREF_AUTO_END_MOVE_SPOT_VEH_PACT = 0x100000,
    PREF_AUTO_END_MOVE_SPOT_VEH_TREATY = 0x200000,
    PREF_AUTO_END_MOVE_SPOT_VEH_TRUCE = 0x400000,
    PREF_AUTO_END_MOVE_SPOT_VEH_WAR = 0x800000,
    PREF_AUTO_FORMER_PLANT_FORESTS = 0x1000000,
    PREF_AUTO_FORMER_BUILD_ADV = 0x2000000, // condensers, boreholes, etc.
    //
    PREF_AV_SLIDING_WINDOWS = 0x8000000,
    PREF_AV_SECRET_PROJECT_MOVIES = 0x10000000,
    PREF_ADV_RADIO_BTN_NOT_SEL_SING_CLK = 0x20000000, // flag set when unchecked
    PREF_AUTO_DONT_END_MOVE_DIFF_TRIAD = 0x40000000,
    PREF_AUTO_WAKE_VEH_TRANS_REACH_LAND = 0x80000000,
};

enum GameMorePreferencesBitfield {
    MPREF_MAP_SHOW_FOG_WAR = 0x1,
    //
    MPREF_ADV_ZOOM_BASE_NO_RECENTER_MAP = 0x4,
    MPREF_AUTO_FORMER_REMOVE_FUNGUS = 0x8,
    MPREF_ADV_PAUSE_AFTER_BATTLES = 0x10,
    MPREF_AUTO_FORMER_BUILD_SENSORS = 0x20,
    MPREF_ADV_QUICK_MOVE_VEH_ORDERS = 0x40,
    MPREF_ADV_QUICK_MOVE_ALL_VEH = 0x80,
    MPREF_ADV_RIGHT_CLICK_POPS_UP_MENU = 0x100,
    MPREF_AV_WHOLE_VEH_BLINKS = 0x200,
    MPREF_ADV_DETAIL_RIGHT_CLICK_MENUS = 0x400,
    MPREF_AUTO_ALWAYS_INSPECT_MONOLITH = 0x800,
    MPREF_MAP_SHOW_PROD_WITH_BASE_NAMES = 0x1000,
    MPREF_MAP_SHOW_BASE_NAMES = 0x2000,
    MPREF_AV_VOLUME_VOICE_TOGGLE = 0x4000,
    MPREF_AV_VOICEOVER_TECH_FAC = 0x4000, // not displayed
    MPREF_ADV_CONFIRM_ODDS_BF_ATTACKING = 0x8000,
    MPREF_MAP_SHOW_FLAT_TERRAIN = 0x10000,
    MPREF_AV_VOICEOVER_STOP_CLOSE_POPUP = 0x20000,
    MPREF_ADV_CLICK_VEH_CANCELS_ORDERS = 0x40000,
    MPREF_AV_SLIDING_SCROLLBARS = 0x80000,
    MPREF_BSC_AUTO_PRUNE_OBS_VEH = 0x100000,
    MPREF_ADV_DETAIL_MAIN_MENUS = 0x200000,
    MPREF_AUTO_FORMER_CANT_BUILD_ROADS = 0x400000, // or tubes; flag set when unchecked
    MPREF_MAP_SHOW_GRID_OCEAN_SQ = 0x800000,
    MPREF_AV_MONUMENTS_DISABLED = 0x1000000, // flag set when unchecked
    MPREF_MAP_HIDE_ACTIVE_VEH_GOTO_PATH = 0x2000000, // flag set when unchecked
    //
    //
    //
    //
    //
    //
};

struct Label {
    LPSTR strings_ptr;
    int count;
};

struct RulesResourceinfo {
    int nutrients;
    int minerals;
    int energy;
    int unused;
};

struct RulesTimeControl {
    LPSTR name;
    int turn;
    int base;
    int unit;
    int event;
    int extra;
    int refresh;
    int accumulated;
};

struct RulesResource {
    LPSTR name_singular;
    LPSTR name_plural;
};

struct RulesEnergy {
    LPSTR abbrev;
    LPSTR name;
};

struct RulesBasic { // Alpha(x).txt
    // #RULES: Movement rate along roads
    // Original bounds: 1 to 100
    // Default value: 3
    uint32_t move_rate_roads;

    // #RULES: Nutrient intake requirement for citizens
    // Original bounds: 0 to 100
    // Default value: 2
    uint32_t nutrient_req_citizen;

    // #RULES: Max airdrop range w/o orbital insertion
    // Original bounds: 1 to 500
    // Default value: 8
    uint32_t max_airdrop_sans_orb_insert;

    // #RULES: Max artillery range (larger will break multiplayer)
    // Original bounds: 1 to 8
    // Default value: 2
    uint32_t artillery_max_rng;

    // #RULES: Numerator for artillery fire damage
    // Original bounds: 1 to 1000
    // Default value: 3
    uint32_t artillery_dmg_num;

    // #RULES: Denominator for artillery fire damage
    // Original bounds: 1 to 1000
    // Default value: 2
    uint32_t artillery_dmg_denom;

    // #RULES: Nutrient cost multiplier
    // Original bounds: 1 to 100
    // Default value: 10
    uint32_t nutrient_cost_multi;

    // #RULES: Minerals cost multiplier
    // Original bounds: 1 to 100
    // Default value: 10
    uint32_t mineral_cost_multi;

    // #RULES: Technology discovery rate as a percentage of standard
    // Original bounds: 0 to 1000
    // Default value: 100
    uint32_t tech_discovery_rate_pct_std;

    // #RULES: Limits mineral increase for mine without road in square
    // Original bounds: 0 to 100
    // Default value: 1
    uint32_t limit_mineral_mine_sans_road;

    // #RULES: Nutrient effect in mine square (0 or -1)
    // Original bounds: 0 or -1
    // Default value: -1
    int tgl_nutrient_effect_with_mine;

    // #RULES: Minimum base size to support specialists
    // Original bounds: 0 to 100
    // Default value: 5
    uint32_t min_base_size_specialists;

    // #RULES: Drones induced by Genejack factory
    // Original bounds: 0 to 100
    // Default value: 1
    uint32_t drones_genejack_factory;

    // #RULES: Population limit w/o hab complex
    // Original bounds: 1 to 100
    // Default value: 7
    uint32_t pop_limit_sans_hab_complex;

    // #RULES: Population limit w/o hab dome
    // Original bounds: 1 to 100
    // Default value: 14
    uint32_t pop_limit_sans_hab_dome;

    // #RULES: Technology to improve fungus squares
    // Default value: CentPsi (63)
    int tech_improve_fungus_sqr;

    // #RULES: Technology to ease fungus movement
    // Default value: CentPsi (63)
    int tech_ease_fungus_movement;

    // #RULES: Technology to build roads in fungus
    // Default value: CentEmp (58)
    int tech_build_roads_fungus;

    // #RULES: Technology to allow 2 special abilities for a unit
    // Default value: Neural (52)
    int tech_two_spec_abilities;

    // #RULES: Technology to allow 3 nutrients in a square
    // Default value: Gene (49)
    int tech_three_nutrients_sqr;

    // #RULES: Technology to allow 3 minerals in a square
    // Default value: EcoEng (60)
    int tech_three_minerals_sqr;

    // #RULES: Technology to allow 3 energy in a square
    // Default value: EnvEcon (59)
    int tech_three_energy_sqr;

    // #RULES: Extra percentage cost of prototype LAND unit
    // Original bounds: 0 to 500
    // Default value: 50
    uint32_t extra_pct_cost_proto_land;

    // #RULES: Extra percentage cost of prototype SEA unit
    // Original bounds: 0 to 500
    // Default value: 50
    uint32_t extra_pct_cost_proto_sea;

    // #RULES: Extra percentage cost of prototype AIR unit
    // Original bounds: 0 to 500
    // Default value: 50
    uint32_t extra_pct_cost_proto_air;

    // #RULES: Psi combat offense-to-defense ratio (unit defending) Numerator
    // Original bounds: 1 to 1000
    // Default value: 3 (LAND)
    // Default value: 1 (SEA/AIR)
    uint32_t psi_combat_ratio_atk[3]; // LAND, SEA, AIR

    // #RULES: Psi combat offense-to-defense ratio (unit defending) Denominator
    // Original bounds: 1 to 1000
    // Default value: 2 (LAND)
    // Default value: 1 (SEA/AIR)
    uint32_t psi_combat_ratio_def[3]; // LAND, SEA, AIR

    // #RULES: Players' starting energy reserves
    // Original bounds: 0 to 1000
    // Default value: 10
    uint32_t player_start_energy_reserve;

    // #RULES: Combat % -> intrinsic base defense
    // Original bounds: -100 to 1000
    // Default value: 25
    int combat_pct_base_def;

    // #RULES: Combat % -> attacking along road
    // Original bounds: -100 to 1000
    // Default value: 0
    int combat_pct_atk_road;

    // #RULES: Combat % -> for attacking from higher elevation
    // Original bounds: -100 to 1000
    // Default value: 0
    int combat_pct_atk_higher_elev;

    // #RULES: Combat penalty % -> attacking from lower elevation
    // Original bounds: -100 to 1000
    // Default value: 0
    int combat_pen_pct_atk_lwr_elev;

    // #RULES: Technology to allow orbital insertion w/o Space Elevator
    // Default value: Gravity (21)
    int tech_orb_insert_sans_spc_elev;

    // #RULES: Minimum # of turns between councils
    // Original bounds: 0 to 1000
    // Default value: 20
    uint32_t min_turns_councils;

    // #RULES: Minerals for harvesting forest
    // Original bounds: 0 to 100
    // Default value: 5
    uint32_t minerals_harvesting_forest;

    // #RULES: Territory: max distance from base
    // Original bounds: 0 to 100
    // Default value: 8
    uint32_t territory_max_dist_base;

    // #RULES: Turns to corner Global Energy Market
    // Original bounds: 1 to 100
    // Default value: 20
    uint32_t turns_corner_gbl_energy_mrkt;

    // #RULES: Technology for +1 mining platform bonus
    // Default value: EcoEng2 (62)
    int tech_mining_platform_bonus;

    // #RULES: Technology for economic victory
    // Default value: PlaEcon (61)
    int tech_economic_victory;

    // #RULES: Combat penalty % -> attack after airdrop
    // Original bounds: -100 to 1000
    // Default value: 50
    int combat_pen_pct_atk_airdrop;

    // #RULES: Combat % -> Fanatic attack bonus
    // Original bounds: -100 to 1000
    // Default value: 25
    int combat_pct_fanatic_atk_bonus;

    // #RULES: Combat % -> Land based guns vs. ship artillery bonus
    // Original bounds: -100 to 1000
    // Default value: 50
    int combat_pct_land_gun_vs_ship_art;

    // #RULES: Combat % -> Artillery bonus per level of altitude
    // Original bounds: -100 to 1000
    // Default value: 25
    int combat_pct_art_bonus_lvl_alt;

    // #RULES: Combat % -> Mobile unit in open ground
    // Original bounds: -100 to 1000
    // Default value: 25
    int combat_pct_mobile_open_ground;

    // #RULES: Combat % -> Defend vs. mobile in rough
    // Original bounds: -100 to 1000
    // Default value: 0
    int combat_pct_def_vs_mobile_rough;

    // #RULES: Combat % -> Trance bonus defending vs. psi
    // Original bounds: -100 to 1000
    // Default value: 50
    int combat_pct_trance_def_vs_psi;

    // #RULES: Combat % -> Empath Song bonus attacking vs. psi
    // Original bounds: -100 to 1000
    // Default value: 50
    int combat_pct_emp_song_atk_vs_psi;

    // #RULES: Combat % -> Infantry vs. Base
    // Original bounds: -100 to 1000
    // Default value: 25
    int combat_pct_infantry_vs_base;

    // #RULES: Combat penalty % -> Air superiority unit vs. ground unit
    // Original bounds: -100 to 1000
    // Default value: 50
    int combat_pen_pct_air_supr_vs_grnd;

    // #RULES: Combat % -> Air superiority unit vs. air unit
    // Original bounds: -100 to 1000
    // Default value: 100
    int combat_pct_air_supr_vs_air;

    // #RULES: Combat penalty % -> Non-combat unit defending vs. combat unit
    // Original bounds: -100 to 1000
    // Default value: 50
    int combat_pen_pct_non_cbt_def_vs_cbt;

    // #RULES: Combat % -> Comm Jammer unit defending vs. mobile unit
    // Original bounds: -100 to 1000
    // Default value: 50
    int combat_pct_com_jam_def_vs_mobl;

    // #RULES: Combat % -> Bonus vs. ships caught in port
    // Original bounds: -100 to 1000
    // Default value: 100
    int combat_pct_bonus_vs_ship_port;

    // #RULES: Combat % -> AAA bonus vs. air units
    // Original bounds: -100 to 1000
    // Default value: 100
    int combat_pct_aaa_bonus_vs_air;

    // #RULES: Combat % -> Defend in range of friendly Sensor
    // Original bounds: -100 to 1000
    // Default value: 25
    int combat_pct_def_range_sensor;

    // #RULES: Combat % -> Psi attack bonus/penalty per +PLANET
    // Original bounds: -100 to 1000
    // Default value: 10
    int combat_pct_psi_atk_bonus_planet;

    // #RULES: Retool strictness
    // (0 = Always Free, 1 = Free in Category, 2 = Free if Project, 3 = Never Free)
    // Original bounds: 0 to 2 (Bug Fix: Should be 0 to 3)
    // Default value: 2
    uint32_t retool_strictness;

    // #RULES: Retool percent penalty for production change
    // Original bounds: 0 to 100
    // Default value: 50
    uint32_t retool_pct_pen_prod_chg;

    // #RULES: Retool exemption (first X minerals not affected by penalty)
    // Original bounds: 0 to 1000
    // Default value: 10
    uint32_t retool_exemption;

    // #RULES: If non-zero, probe teams can steal technologies
    // Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
    // Default value: 1
    BOOL tgl_probe_can_steal_tech;

    // #RULES: If non-zero, humans can always contact each other in net games
    // Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
    // Default value: 1
    BOOL tgl_human_always_contact_net;

    // #RULES: If non-zero, humans can always contact each other in hotseat/email games
    // Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
    // Default value: 1
    BOOL tgl_humans_always_contact_pbem;

    // #RULES: Maximum % damage inflicted by arty versus units in base/bunker
    // Original bounds: 10 to 100
    // Default value: 50
    uint32_t max_pct_dmg_art_vs_unit_base_bnkr;

    // #RULES: Maximum % damage inflicted by arty versus units in open
    // Original bounds: 10 to 100
    // Default value: 99
    uint32_t max_pct_dmg_art_vs_units_open;

    // #RULES: Maximum % damage inflicted by arty versus units in sea
    // Original bounds: 10 to 100
    // Default value: 100
    uint32_t max_pct_dmg_art_vs_units_sea;

    // #RULES: Numerator for freq of global warming (1,2 would be "half" normal warming).
    // Original bounds: 0 to 1000
    // Default value: 1
    uint32_t freq_global_warming_num;

    // #RULES: Denominator for freq of global warming (1,2 would be "half" normal warming).
    // Original bounds: 1 to 1000
    // Default value: 1
    uint32_t freq_global_warming_denom;

    // #RULES: Normal starting year
    // Original bounds: 0 to 999999
    // Default value: 2100
    uint32_t normal_starting_year;

    // #RULES: Normal ending year for lowest 3 difficulty levels
    // Original bounds: 0 to 999999
    // Default value: 2600
    uint32_t normal_end_year_low_three_diff;

    // #RULES: Normal ending year for highest 3 difficulty levels
    // Original bounds: 0 to 999999
    // Default value: 2500
    uint32_t normal_end_year_high_three_diff;

    // #RULES: If non-zero, obliterating a base counts as an atrocity
    // Original bounds: 0, 1 or -1 (Changed to 0 or 1 since -1 is treated same as 1)
    // Default value: 1
    BOOL tgl_oblit_base_atrocity;

    // Alphax.txt #RULES: Size of base for subspace generator
    // Original bounds: 1 to 999
    // Default value: 10
    uint32_t size_base_subspace_gen;

    // Alphax.txt #RULES: Number of subspace generators needed
    // Original bounds: 1 to 999
    // Default value: 6
    uint32_t subspace_generators_needed;
};

struct RulesWorldbuilder { // Alpha(x).txt
    // #WORLDBUILDER: Seeded land size of a standard world
    // Original bounds: 50 to 4000
    // Default value: 384
    uint32_t land_base;

    // #WORLDBUILDER: Additional land from LAND selection: x0, x1, x2
    // Original bounds: 0 to 2000
    // Default value: 256
    uint32_t land_mod;

    // #WORLDBUILDER: Base size of a land mass seed
    // Original bounds: 5 to 1000
    // Default value: 12
    uint32_t continent_base;

    // #WORLDBUILDER: Increased size from LAND selection: x0, x1, x2
    // Original bounds: 5 to 1000
    // Default value: 24
    uint32_t continent_mod;

    // #WORLDBUILDER: Base # of extra hills
    // Original bounds: 0 to 100
    // Default value: 1
    uint32_t hills_base;

    // #WORLDBUILDER: Additional hills from TIDAL selection: x0, x1, x2
    // Original bounds: 0 to 100
    // Default value: 2
    uint32_t hills_mod;

    // #WORLDBUILDER: Basic plateau size
    // Original bounds: 0 to 500
    // Default value: 4
    uint32_t plateau_base;

    // #WORLDBUILDER: Plateau modifier based on LAND selection: x0, x1, x2
    // Original bounds: 0 to 500
    // Default value: 8
    uint32_t plateau_mod;

    // #WORLDBUILDER: Basic # of rivers
    // Original bounds: 0 to 100
    // Default value: 8
    uint32_t rivers_base;

    // #WORLDBUILDER: Additional rivers based on RAIN selection
    // Original bounds: 0 to 100
    // Default value: 12
    uint32_t rivers_rain_mod;

    // #WORLDBUILDER: Latitude DIVISOR for temperature based on HEAT;
    // Smaller # increases effect of HEAT selection
    // Original bounds: 1 to 64
    // Default value: 14
    uint32_t solar_energy;

    // #WORLDBUILDER: Latitude DIVISOR for thermal banding; Smaller # widens hot bands
    // Original bounds: 1 to 64
    // Default value: 14
    uint32_t thermal_band;

    // #WORLDBUILDER: Latitude DIVISOR for thermal deviance; Smaller # increases randomness
    // Original bounds: 1 to 64
    // Default value: 8
    uint32_t thermal_deviance;

    // #WORLDBUILDER: Latitude DIVISOR for global warming;
    // Smaller # increases effect of warming
    // Original bounds: 1 to 64
    // Default value: 8
    uint32_t global_warming;

    // #WORLDBUILDER: Magnitude of sea level changes from ice cap melting/freezing
    // Original bounds: 1 to 100
    // Default value: 5
    uint32_t sea_level_rises;

    // #WORLDBUILDER: Size of cloud mass trapped by peaks
    // Original bounds: 0 to 20
    // Default value: 5
    uint32_t cloudmass_peaks;

    // #WORLDBUILDER: Size of cloud mass trapped by hills
    // Original bounds: 0 to 20
    // Default value: 3
    uint32_t cloudmass_hills;

    // #WORLDBUILDER: Multiplier for rainfall belts
    // Original bounds: 0 to 8
    // Default value: 1
    uint32_t rainfall_coeff;

    // #WORLDBUILDER: Encourages fractal to grow deep water
    // Original bounds: -100 to 100
    // Default value: 15
    int deep_water;

    // #WORLDBUILDER: Encourages fractal to grow shelf
    // Original bounds: -100 to 100
    // Default value: 10
    int shelf;

    // #WORLDBUILDER: Encourages highland plains
    // Original bounds: -100 to 100
    // Default value: 15
    int plains;

    // #WORLDBUILDER: Encourages wider beaches
    // Original bounds: -100 to 100
    // Default value: 10
    int beach;

    // #WORLDBUILDER: Encourages hills x TIDAL selection
    // Original bounds: 0 to 100
    // Default value: 10
    uint32_t hills;

    // #WORLDBUILDER: Encourages peaks
    // Original bounds: -100 to 100
    // Default value: 25
    int peaks;

    // #WORLDBUILDER: Fungus coefficient based on LIFE selection
    // Original bounds: 0 to 5
    // Default value: 1
    uint32_t fungus;

    // #WORLDBUILDER: Continent size ratios #1
    // Original bounds: n/a
    // Default value: 3
    uint32_t cont_size_ratio1;

    // #WORLDBUILDER: Continent size ratios #2
    // Original bounds: n/a
    // Default value: 6
    uint32_t cont_size_ratio2;

    // #WORLDBUILDER: Continent size ratios #3
    // Original bounds: n/a
    // Default value: 12
    uint32_t cont_size_ratio3;

    // #WORLDBUILDER: Continent size ratios #4
    // Original bounds: n/a
    // Default value: 18
    uint32_t cont_size_ratio4;

    // #WORLDBUILDER: Continent size ratios #5
    // Original bounds: n/a
    // Default value: 24
    uint32_t cont_size_ratio5;

    // #WORLDBUILDER: Higher # increases island count
    // Original bounds: 1 to 500
    // Default value: 36
    uint32_t islands;
};

struct AlphaIniPref {
    uint32_t preferences;
    uint32_t more_preferences;
    uint32_t announce;
    uint32_t rules;
    uint32_t semaphore;
    uint32_t time_controls;
    uint32_t customize;
    uint32_t custom_world[7];
};

struct DefaultPref {
    uint32_t difficulty;
    uint32_t faction_id;
    uint32_t pad; // unused
    uint32_t map_type;
    uint32_t top_menu;
};

constexpr int NoneValue = -1;
constexpr int DisabledValue = -2;

constexpr int MaxResourceInfoNum = 9;
constexpr int MaxTimeControlNum = 6;
constexpr int MaxCompassNum = 8;
constexpr int MaxResourceNum = 4;
constexpr int MaxEnergyNum = 3;
constexpr int MaxDiffNum = 6;

extern LPCSTR AlphaxFileID;
extern LPCSTR ScriptTxtID;
extern Label *Labels;
extern RulesResourceinfo *ResourceInfo;
extern RulesTimeControl *TimeControl;
extern RulesResource *Resource;
extern RulesEnergy *Energy;
extern RulesBasic *Rules;
extern RulesWorldbuilder *WorldBuilder;
extern AlphaIniPref *AlphaIniPrefs;
extern DefaultPref *DefaultPrefs;
extern uint32_t *Language;

DLLEXPORT int __cdecl tech_name(LPSTR name);
DLLEXPORT int __cdecl chas_name(LPSTR name);
DLLEXPORT int __cdecl weap_name(LPSTR name);
DLLEXPORT int __cdecl arm_name(LPSTR name);
DLLEXPORT int __cdecl tech_item();
DLLEXPORT BOOL __cdecl read_basic_rules();
DLLEXPORT BOOL __cdecl read_tech();
DLLEXPORT void __cdecl clear_faction(Player *player);
DLLEXPORT void __cdecl read_faction(uint32_t player_id);
DLLEXPORT void __cdecl read_faction(Player *player, int toggle);
DLLEXPORT BOOL __cdecl read_factions();
DLLEXPORT void __cdecl noun_item(uint32_t *gender, BOOL *plurality);
DLLEXPORT BOOL __cdecl read_units();
DLLEXPORT BOOL __cdecl read_rules(BOOL tgl_all_rules);
DLLEXPORT LPSTR __cdecl prefs_get(LPCSTR key_name, LPCSTR default_value, BOOL use_default);
DLLEXPORT uint32_t __cdecl default_prefs();
DLLEXPORT uint32_t __cdecl default_prefs2();
DLLEXPORT uint32_t __cdecl default_warn();
DLLEXPORT uint32_t __cdecl default_rules();
DLLEXPORT int __cdecl prefs_get(LPCSTR key_name, int default_value, BOOL use_default);
DLLEXPORT void __cdecl prefs_fac_load();
DLLEXPORT void __cdecl prefs_load(BOOL use_default);
DLLEXPORT void __cdecl prefs_put(LPCSTR key_name, LPCSTR value);
DLLEXPORT void __cdecl prefs_put(LPCSTR key_name, int value, BOOL tgl_binary);
DLLEXPORT void __cdecl prefs_save(BOOL save_factions);
DLLEXPORT void __cdecl prefs_use();
DLLEXPORT std::string __cdecl prefs_get_binary(int value);
DLLEXPORT BOOL __cdecl labels_init();
DLLEXPORT void __cdecl labels_shutdown();
DLLEXPORT void __cdecl set_language(uint32_t language);
DLLEXPORT void __cdecl say_label(int label_offset);
DLLEXPORT LPSTR __cdecl label_get(int label_offset);

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
#include "log.h"
#include "game.h"
#include "veh.h"
#include "technology.h"
#include "path.h"
#include "map.h"
#include "base.h"
#include "alpha.h"

// built-in functions > used to prevent crash from mixed alloc/free SDKs
func1 *_malloc = (func1 *)0x006470A6;
func2 *_free = (func2 *)0x00644EF2;
func3 *_fopen = (func3 *)0x00645646;
func4 *_srand = (func4 *)0x00646010;
func10 *_fread = (func10 *)0x00646178;
func10 *_fwrite = (func10 *)0x0064603F;
func17 *_fgetc = (func17 *)0x00646855;
func18 *_fputc = (func18 *)0x00646886;
//func12 *_realloc = (func12 *)0x00647132;
//func18 *_fclose = (func18 *)0x00645598;
//func19 *_fgets = (func19 *)0x0064726A;

// other
func5 *load_faction_art = (func5 *)0x00453710;
func5 *draw_map = (func5 *)0x0046B190;
func5 *wave_it = (func5 *)0x004455F0;
// TODO: crash bug; X_pop > ... > BasePop::start > text_close > NULLs 009B7CF4 (FILE *Txt.textFile)
// Next call to text_get() tries to access 009B7CF4 and the game crashes.
func6 *X_pop = (func6 *)0x005BF310;
func7 *popp = (func7 *)0x0048C0A0;
func9 *fixup_landmarks = (func9 *)0x00592940;
func9 *mapwin_terrain_fixup = (func9 *)0x00471240;
func9 *world_rainfall = (func9 *)0x005C4470;
func11 *wants_to_attack_ = (func11 *)0x0055BC80;
func14 *base_at = (func14 *)0x004E3A50;
func15 *save_daemon = (func15 *)0x005A94F0;
func16 *load_daemon = (func16 *)0x005A9760;
func19 *social_set = (func19 *)0x005B4600;
func19 *consider_designs = (func19 *)0x00581260;
func20 *popb = (func20 *)0x0048C650;
func21 *draw_radius = (func21 *)0x0046AEF0;
func22 *alt_set = (func22 *)0x00591290;

// Time
func30 *blink_timer = (func30 *)0x0050EA40;
func30 *blink2_timer = (func30 *)0x0050EE30;
func30 *line_timer = (func30 *)0x0050EE80;
func30 *turn_timer = (func30 *)0x0050EF10;

// testing
func8 *parse_string_OG = (func8 *)0x00625880;
func12 *enemy_capabilities_OG = (func12 *)0x00560DD0;
//
funct3 *base_queue_OG = (funct3 *)0x004F06E0;
funct4 *best_defender_OG = (funct4 *)0x005044D0;
///
char1032 *stringTemp = (char1032 *)0x009B86A0;
char256 *ParseStrBuffer = (char256 *)0x009BB5E8;

//  ; int
int *BufferStrHeight = (int *)0x009B3A9C;
int *ParseNumTable = (int *)0x009BB598;
int *ParseStrPlurality = (int *)0x009BB570;
int *ParseStrGender = (int *)0x009BB5C0;
HWND *HandleMain = (HWND *)0x009B7B28;

uint32_t *UnkBitfield1 = (uint32_t *)0x0090D91C;

Filefind *FilefindPath = (Filefind *)0x009B8198;
MainInterface *MainInterfaceVar = (MainInterface *)0x007AE820;

int __cdecl tester() {
	log_set_state(true);
	log_say("Start test", 0, 0, 0);

	for (int i = 0; i < *VehCurrentCount; i++) { // test -1
		for (int j = 0; j < *VehCurrentCount; j++) {
			for (int k = 0; k < 2; k++) {
				if (best_defender_OG(j, i, k) != best_defender(j, i, k)) {
					log_say("error: ", j, i, k);
				}
			}
		}
	}

	/*
	base_queue_OG(1);
	base_queue(1);

	base_queue(2);
	base_queue_OG(2);
	*/
	
	/*
	for (int i = 0; i < *BaseCurrentCount; i++) {
		log_say(Base[i].name_string, Base[i].xCoord, Base[i].yCoord, 0);
		log_say("economy_total: ", Base[i].economy_total, 0, 0);
		log_say("psych_total: ", Base[i].psych_total, 0, 0);
		log_say("labs_total: ",  Base[i].labs_total, 0, 0);
		log_say("unk_10: ", Base[i].unk_10, 0, 0);
	}

	/*
	char szTemp[80];
	for (uint32_t y = 0; y < *MapVerticalBounds; y++) {
		for (uint32_t x = y & 1; x < *MapHorizontalBounds; x += 2) {
			map *tile = map_loc(x, y);
			if (tile->unk_1) {
				log_say("map_unk1:", x, y, tile->unk_1);
			}
			if (tile->bit2 & 0x80000000) {
				sprintf_s(szTemp, 80, "0x%08X", tile->bit2);
				log_say("map_bit2:", szTemp, x, y, 0);
			}
		}
	}
	*/
	/*
	for (uint32_t y = 0; y < *MapVerticalBounds; y++) {
		for (uint32_t x = y & 1; x < *MapHorizontalBounds; x += 2) {
			map *tile = map_loc(x, y);
			if (tile->unk_1) {
				log_say("map_unk1:", x, y, tile->unk_1);
			}
		}
	}
	//
	/*
	for (int i = 0; i < MaxContinentNum; i++) {
		//log_say("Continent_unk2_count:", i, Continents[i].unk_2, 0);
		log_say("Continent_unk3_count:", i, Continents[i].unk_3, 0);
	}
	for (uint32_t y = 0; y < *MapVerticalBounds; y++) {
		for (uint32_t x = y & 1; x < *MapHorizontalBounds; x += 2) {
			map *tile = map_loc(x, y);
			if (altitude_at(x, y) >= ALT_BIT_SHORE_LINE && (tile->climate & (RAINFALL_RAINY | RAINFALL_MOIST)) != 0
				&& (tile->val3 & 0xC0u) < TERRAIN_ROCKY && (!(tile->bit & BIT_FUNGUS) || altitude_at(x, y) < ALT_BIT_OCEAN_SHELF)) {
				//log_say("unk2_found:", tile->region, x, y);
			}
			uint32_t site = tile->val2 >> 4;
			if (site) {
				char region[10];
				sprintf_s(region, 10, "%d", tile->region);
				log_say("site_found:", region, site, x, y);
			}
		}
	}
	*/
	/*
	for (int i = -1; i < *BaseCurrentCount; i++) {
		for (int j = -2; j < 10; j++) {
			for (int k = -2; k < 10; k++) {
				for (int p = 0; p < MaxPlayerNum; p++) {
					for (int a = 0; a < 2; a++) {
						for (int b = 0; b < 2; b++) {
							//
						}
					}
				}
			}
		}
	}
	*/
	/*
	for (int i = 0; i < MaxPlayerNum; i++) {
		for (int j = 0; j < *BaseCurrentCount; j++) {
			for (int k = 0; k < 2; k++) {
				int mc1 = mind_control(j, i, k);
				int mc2 = mind_control_OG(j, i, k);
				if (mc1 != mc2) {
					log_say("MC error", Base[j].name_string, i, j, k);
				}
				else {
					char szTemp[200];
					sprintf_s(szTemp, 200, "base: %s - xCoord: %d, yCoord: %d, MC cost for %s : %d",
						Base[j].name_string, Base[j].xCoord, Base[j].yCoord, Players[i].formalNameFaction, mc1);
					log_say(szTemp, 0, 0, 0);
				}
			}
		}
	}
	*/

	/*
	for (int i = 0; i < MaxPlayerNum; i++) {
		for (int j = 0; j < 75; j++) {
			int type = PlayersData[i].goals[j].type;
			if (type != AI_GOAL_UNUSED) {
				PlayersData[i].goals[j].type = AI_GOAL_UNK_1;
				/*
				if (type == 6) {
					char szTemp[100];
					sprintf_s(szTemp, 100, "goal1 - xCoord: %d, yCoord: %d",
					PlayersData[i].goals_1[j].xCoord, PlayersData[i].goals_1[j].yCoord);
					log_say(szTemp, 0, 0, 0);
				}
				*/
				/*
						}
					}
				}
				*/
				/*
				for (int i = 0; i < MaxPlayerNum; i++) {
					log_say(Players[i].nameFaction, "global rep", PlayersData[i].globalReputation, 0, 0);
				}
				for (int i = 0; i < MaxPlayerNum; i++) {
					log_say(Players[i].nameFaction, "energy cost", PlayersData[i].energyCost, 0, 0);
				}
				/*
				for (int i = 0; i < MaxPlayerNum; i++) {
					for (int j = 0; j < MaxPlayerNum; j++) {
						int val = PlayersData[i].diploUnk3[j];
						if (val) {
							char szTemp[100];
							sprintf_s(szTemp, 100, "combat_diplo_unk3: %s for %s", Players[i].nameFaction, Players[j].nameFaction);
							log_say(szTemp, val, 0, 0);
						}
					}
				}
				for (int i = 0; i < MaxPlayerNum; i++) {
					for (int j = 0; j < MaxPlayerNum; j++) {
						int val = PlayersData[i].diploUnk4[j];
						if (val) {
							char szTemp[100];
							sprintf_s(szTemp, 100, "combat_diplo_unk4: %s for %s", Players[i].nameFaction, Players[j].nameFaction);
							log_say(szTemp, val, 0, 0);
						}
					}
				}
				for (int i = 0; i < MaxPlayerNum; i++) {
					for (int j = 0; j < MaxPlayerNum; j++) {
						int val = PlayersData[i].diploUnk1[j];
						if (val) {
							char szTemp[100];
							sprintf_s(szTemp, 100, "diploUnk1: %s for %s", Players[i].nameFaction, Players[j].nameFaction);
							log_say(szTemp, val, 0, 0);
						}
					}
				}

				//tech_calc_output();

				/*
				for (int i = 0; i < MaxPlayerNum; i++) {
					log_say(Players[i].searchKey, "PSI Atk", weap_strat(WPN_PSI_ATTACK, i), 0, 0);
					log_say(Players[i].searchKey, "PSI Def", arm_strat(ARM_PSI_DEFENSE, i), 0, 0);
				}

				for (int z = 0; z < 10; z++) {
					for (int i = 1; i < MaxPlayerNum; i++) {
						int t1 = PlayersData[i].enemyBestWeaponVal;
						int t2 = PlayersData[i].enemyBestArmorVal;
						int t3 = PlayersData[i].enemyBestLandSpeed;
						int t4 = PlayersData[i].enemyBestPsiAtkVal;
						int t5 = PlayersData[i].enemyBestPsiDefVal;
						int t6 = PlayersData[i].bestLandSpeed;
						int t7 = PlayersData[i].bestPsiDefVal;
						int t8 = PlayersData[i].bestArmorVal;
						int t9 = PlayersData[i].bestPsiAtkVal;
						int t0 = PlayersData[i].bestWeaponVal;
						enemy_capabilities_t(i);
						int a1 = PlayersData[i].enemyBestWeaponVal;
						int a2 = PlayersData[i].enemyBestArmorVal;
						int a3 = PlayersData[i].enemyBestLandSpeed;
						int a4 = PlayersData[i].enemyBestPsiAtkVal;
						int a5 = PlayersData[i].enemyBestPsiDefVal;
						int a6 = PlayersData[i].bestLandSpeed;
						int a7 = PlayersData[i].bestPsiDefVal;
						int a8 = PlayersData[i].bestArmorVal;
						int a9 = PlayersData[i].bestPsiAtkVal;
						int a0 = PlayersData[i].bestWeaponVal;
						PlayersData[i].enemyBestWeaponVal = t1;
						PlayersData[i].enemyBestArmorVal = t2;
						PlayersData[i].enemyBestLandSpeed = t3;
						PlayersData[i].enemyBestPsiAtkVal = t4;
						PlayersData[i].enemyBestPsiDefVal = t5;
						PlayersData[i].bestLandSpeed = t6;
						PlayersData[i].bestPsiDefVal = t7;
						PlayersData[i].bestArmorVal = t8;
						PlayersData[i].bestPsiAtkVal = t9;
						PlayersData[i].bestWeaponVal = t0;
						enemy_capabilities_OG(i);
						int b1 = PlayersData[i].enemyBestWeaponVal;
						int b2 = PlayersData[i].enemyBestArmorVal;
						int b3 = PlayersData[i].enemyBestLandSpeed;
						int b4 = PlayersData[i].enemyBestPsiAtkVal;
						int b5 = PlayersData[i].enemyBestPsiDefVal;
						int b6 = PlayersData[i].bestLandSpeed;
						int b7 = PlayersData[i].bestPsiDefVal;
						int b8 = PlayersData[i].bestArmorVal;
						int b9 = PlayersData[i].bestPsiAtkVal;
						int b0 = PlayersData[i].bestWeaponVal;

						if (a1 != b1) {
							log_say("enemyBestWeaponVal error", a1, b1, i);
						}
						if (a2 != b2) {
							log_say("enemyBestArmorVal error", a2, b2, i);
						}
						if (a3 != b3) {
							log_say("enemyBestLandSpeed error", a3, b3, i);
						}
						if (a4 != b4) {
							log_say("enemyBestPsiAtkVal error", a4, b4, i);
						}
						if (a5 != b5) {
							log_say("enemyBestPsiDefVal error", a5, b5, i);
						}
						if (a6 != b6) {
							log_say("bestLandSpeed error", a6, b6, i);
						}
						if (a7 != b7) {
							log_say("bestPsiDefVal error", a7, b7, i);
						}
						if (a8 != b8) {
							log_say("bestArmorVal error", a8, b8, i);
						}
						if (a9 != b9) {
							log_say("bestPsiAtkVal error", a9, b9, i);
						}
						if (a0 != b0) {
							log_say("bestWeaponVal error", a0, b0, i);
						}
						log_say("results: ", i, a1, a2);
						log_say("results: ", i, a3, a4);
						log_say("results: ", i, a5, a6);
						log_say("results: ", i, a7, a8);
						log_say("results: ", i, a9, a0);
					}
				}
				/*
				log_say("original start", 0, 0, 0);
				for (int z = 0; z < 10; z++) {
					for (int i = 1; i < MaxPlayerNum; i++) {
						int t1 = PlayersData[i].enemyBestWeaponVal;
						int t2 = PlayersData[i].enemyBestArmorVal;
						int t3 = PlayersData[i].enemyBestLandSpeed;
						int t4 = PlayersData[i].enemyBestPsiAtkVal;
						int t5 = PlayersData[i].enemyBestPsiDefVal;
						int t6 = PlayersData[i].bestLandSpeed;
						int t7 = PlayersData[i].bestPsiDefVal;
						int t8 = PlayersData[i].bestArmorVal;
						int t9 = PlayersData[i].bestPsiAtkVal;
						int t0 = PlayersData[i].bestWeaponVal;
						enemy_capabilities_OG(i);
						int a1 = PlayersData[i].enemyBestWeaponVal;
						int a2 = PlayersData[i].enemyBestArmorVal;
						int a3 = PlayersData[i].enemyBestLandSpeed;
						int a4 = PlayersData[i].enemyBestPsiAtkVal;
						int a5 = PlayersData[i].enemyBestPsiDefVal;
						int a6 = PlayersData[i].bestLandSpeed;
						int a7 = PlayersData[i].bestPsiDefVal;
						int a8 = PlayersData[i].bestArmorVal;
						int a9 = PlayersData[i].bestPsiAtkVal;
						int a0 = PlayersData[i].bestWeaponVal;
						PlayersData[i].enemyBestWeaponVal = t1;
						PlayersData[i].enemyBestArmorVal = t2;
						PlayersData[i].enemyBestLandSpeed = t3;
						PlayersData[i].enemyBestPsiAtkVal = t4;
						PlayersData[i].enemyBestPsiDefVal = t5;
						PlayersData[i].bestLandSpeed = t6;
						PlayersData[i].bestPsiDefVal = t7;
						PlayersData[i].bestArmorVal = t8;
						PlayersData[i].bestPsiAtkVal = t9;
						PlayersData[i].bestWeaponVal = t0;
						enemy_capabilities_OG(i);
						int b1 = PlayersData[i].enemyBestWeaponVal;
						int b2 = PlayersData[i].enemyBestArmorVal;
						int b3 = PlayersData[i].enemyBestLandSpeed;
						int b4 = PlayersData[i].enemyBestPsiAtkVal;
						int b5 = PlayersData[i].enemyBestPsiDefVal;
						int b6 = PlayersData[i].bestLandSpeed;
						int b7 = PlayersData[i].bestPsiDefVal;
						int b8 = PlayersData[i].bestArmorVal;
						int b9 = PlayersData[i].bestPsiAtkVal;
						int b0 = PlayersData[i].bestWeaponVal;

						if (a1 != b1) {
							log_say("enemyBestWeaponVal error", a1, b1, i);
						}
						if (a2 != b2) {
							log_say("enemyBestArmorVal error", a2, b2, i);
						}
						if (a3 != b3) {
							log_say("enemyBestLandSpeed error", a3, b3, i);
						}
						if (a4 != b4) {
							log_say("enemyBestPsiAtkVal error", a4, b4, i);
						}
						if (a5 != b5) {
							log_say("enemyBestPsiDefVal error", a5, b5, i);
						}
						if (a6 != b6) {
							log_say("bestLandSpeed error", a6, b6, i);
						}
						if (a7 != b7) {
							log_say("bestPsiDefVal error", a7, b7, i);
						}
						if (a8 != b8) {
							log_say("bestArmorVal error", a8, b8, i);
						}
						if (a9 != b9) {
							log_say("bestPsiAtkVal error", a9, b9, i);
						}
						if (a0 != b0) {
							log_say("bestWeaponVal error", a0, b0, i);
						}
						log_say("results: ", i, a1, a2);
						log_say("results: ", i, a3, a4);
						log_say("results: ", i, a5, a6);
						log_say("results: ", i, a7, a8);
						log_say("results: ", i, a9, a0);
					}
				}
				*/

				/*
				for (int i = 0; i < MaxPlayerNum; i++) {
					for (int j = 0; j < 128; j++) {
						if (PlayersData[i].baseCountByRegion[j]) {
							log_say(Players[i].searchKey, " ? baseCountByRegion ? ", j, PlayersData[i].baseCountByRegion[j], 0);
						}
					}
				}

				for (int i = 0; i < MaxPlayerNum; i++) {
					for (int j = 0; j < 128; j++) {
						if (PlayersData[i].unk_81[j]) {
							log_say(Players[i].searchKey, "? unk_81 ?", j, PlayersData[i].unk_81[j], 0);
						}
					}
				}

				for (int i = 0; i < MaxPlayerNum; i++) {
					for (int j = 0; j < 128; j++) {
						if (PlayersData[i].unk_84[j]) {
							//log_say(Players[i].searchKey, "? unk_84 ? ", j, PlayersData[i].unk_84[j], 0);
						}
					}
				}

				for (int i = 0; i < MaxPlayerNum; i++) {
					//log_say(Players[i].searchKey, "unk_70 - sea?", PlayersData[i].unk_70, 0, 0);
					//log_say(Players[i].searchKey, "unk_71", PlayersData[i].unk_72, 0, 0);
					log_say(Players[i].searchKey, " ? unk_26 | earnedTechsSaved | techRanking ? ", PlayersData[i].unk_26, PlayersData[i].earnedTechsSaved, PlayersData[i].techRanking);

					//log_say(Players[i].searchKey, "unk_48 - base support sum", PlayersData[i].unk_48, 0, 0);
					//log_say(Players[i].searchKey, "? unk_49 - 4xecon dmg sum ? ", PlayersData[i].unk_49, 0, 0);
					//log_say(Players[i].searchKey, "unk_48 - support", PlayersData[i].unk_48, 0, 0);
				}
				*/
				/*
				for (int i = 0; i < MaxBaseNum; i++) {
					char szTemp[512];
					szTemp[0] = 0;
					strcat_s(szTemp, " - ");
					strcat_s(szTemp, Players[Base[i].faction_id_current].nounFaction);
					strcat_s(szTemp, " > unk3 > ");
					log_say(Base[i].name_string, szTemp, Base[i].unk3, 0, 0);
					/*
					szTemp[0] = 0;
					strcat_s(szTemp, " - ");
					strcat_s(szTemp, Players[Base[i].faction_id_current].nounFaction);
					strcat_s(szTemp, " - unk4 - ");
					log_say(Base[i].name_string, szTemp, Base[i].unk4, 0, 0);
					szTemp[0] = 0;
					strcat_s(szTemp, " - ");
					strcat_s(szTemp, Players[Base[i].faction_id_current].nounFaction);
					strcat_s(szTemp, " - unk5 - ");
					log_say(Base[i].name_string, szTemp, Base[i].unk5, 0, 0);
					*/
					//}

					//

	log_say("End test", 0, 0, 0);
	log_set_state(false);
	/*
	for (uint32_t y = 0; y < *MapVerticalBounds; y++) {
		for (uint32_t x = y & 1; x < *MapHorizontalBounds; x += 2) {
			uint8_t att = abstract_at(x, y);
			if (att) {
				log_say("abstract_at: ", x, y, att);
				//log_say("region:      ", x, y, region_at(x,y));
			}

			//if (goody) {
			//	log_say("goody_at: ", x, y, goody);
			//}
			/*
			uint32_t bit = bit_at(x, y);
			if (bit & BIT_UNK_4000000) {
				log_say("found BIT_UNK_4000000: ", x, y, bit);
			}
			if (bit & BIT_UNK_8000000) {
				log_say("found BIT_UNK_8000000: ", x, y, bit);
			}
			/*
			for (uint32_t alt = 0; alt < 8; alt++) {
				map_loc(x, y)->val1 &= 0x1F;
				map_loc(x, y)->val1 |= alt << 5;
				uint32_t min = minerals_at_OG(x, y);
				if (minerals_at(x, y) != min) {
					log_say("minerals_at error: ", x, y, min);
				}
			}
			*/
			//for (int f = 0; f < MaxPlayerNum; f++) {
				//
			//}
			//int goody = goody_at_OG(x, y);
			//if (bonus != bonus_at_OG(x, y, 0)) {
			//	MessageBoxA(NULL, "bonus_at Error", "FATAL ERROR", MB_ICONWARNING);
			//}
			//if (goody) {
			//	log_say("goody_at: ", x, y, goody);
			//}
			/*
			uint32_t bit = bit_at(x, y);

			if (bit & BIT_UNK_200) {
				log_say("BIT_UNK_200: ", x, y, bit);
			}
			if (bit & BIT_UNK_4000) {
				log_say("BIT_UNK_4000: ", x, y, bit);
			}
			//if (bit & BIT_SUPPLY_REMOVE) {
			//	log_say("BIT_SUPPLY_REMOVE: ", x, y, bit);
			//}
			if (bit & BIT_UNK_2000000) {
				log_say("BIT_UNK_2000000: ", x, y, bit);
			}
			if (bit & BIT_UNK_4000000) {
				log_say("BIT_UNK_4000000: ", x, y, bit);
			}
			if (bit & BIT_UNK_8000000) {
				log_say("BIT_UNK_8000000: ", x, y, bit);
			}
			//if (bit & BIT_UNK_40000000) {
			//	log_say("BIT_UNK_40000000: ", x, y, bit);
			//}
			*/
			//}
		//}

		/*
		for (int y = -10; y < (int)*MapVerticalBounds; y++) {
			for (int x = -1; x < (int)*MapHorizontalBounds; x++) {
				for (int f = -1; f < 10; f++) {
					int FMTemp = *MapFlatToggle;
					for (int t = 0; t < 2; t++) {
						*MapFlatToggle = t;
						BOOL MPtemp = *MultiplayerToggle;
						for (int m = 0; m < 2; m++) {
							*MultiplayerToggle = m;
							if (zoc_any_OG(x, y, f) != zoc_any(x, y, f)) {
								MessageBoxA(NULL, "zoc_any Error", "FATAL ERROR", MB_ICONWARNING);
							}
							if (zoc_veh_OG(x, y, f) != zoc_veh(x, y, f)) {
								MessageBoxA(NULL, "zoc_veh Error", "FATAL ERROR", MB_ICONWARNING);
							}
							if (zoc_sea_OG(x, y, f) != zoc_sea(x, y, f)) {
								MessageBoxA(NULL, "zoc_sea Error", "FATAL ERROR", MB_ICONWARNING);
							}
							if (zoc_move_OG(x, y, f) != zoc_move(x, y, f)) {
								MessageBoxA(NULL, "zoc_move Error", "FATAL ERROR", MB_ICONWARNING);
							}
						}
						*MultiplayerToggle = MPtemp;
					}
					*MapFlatToggle = FMTemp;
				}
			}
		}
		*/
	return 0;
}

// tech_val output
func13 *tech_val_OG = (func13 *)0x005BCBE0;
void tech_calc_output() {
	for (int i = 0; i < MaxPlayerNum; i++) {
		for (int j = 0; j < MaxTechnologyNum; j++) {
			for (int k = 0; k < 2; k++) {
				int techVal1 = tech_val(j, i, k);
				int techVal2 = tech_val_OG(j, i, k);
				if (techVal1 != techVal2) {
					log_say("tech_val error: ", j, i, k);
					log_say("good:", techVal2, 0, 0);
					log_say("bad: ", techVal1, 0, 0);
				}
				else {
					log_say(Players[i].formalNameFaction, Technology[j].name, techVal1, k, 0);
				}
			}
		}
		for (int j = 89; j < 609; j++) {
			int techVal1 = tech_val(j, i, 0);
			int techVal2 = tech_val_OG(j, i, 0);
			if (techVal1 != techVal2) {
				log_say("tech_val error: ", j, i, 0);
				if (j < 97) {
					log_say(Players[i].formalNameFaction, Players[j - 89].formalNameFaction, techVal1, techVal2, 0);
				}
				else {
					log_say(Players[i].formalNameFaction, VehPrototype[j - 97].vehName, techVal1, techVal2, 0);
				}
			}
			else {
				if (j < 97) {
					log_say(Players[i].formalNameFaction, Players[j - 89].formalNameFaction, techVal1, 0, 0);
				}
				else {
					log_say(Players[i].formalNameFaction, VehPrototype[j - 97].vehName, techVal1, 0, 0);
				}
			}
		}
	}
}

// message handling testing
// possible issues different SDKs?
func_msg *do_video = (func_msg *)0x00636300;
func_msg *check_net = (func_msg *)0x0062D5D0;
func_msg *do_net = (func_msg *)0x0062D5B0;
func_msg *do_non_input_ = (func_msg *)0x005FCA30;

uint32_t *MsgStatus = (uint32_t *)0x009B7B9C;

/*
Purpose: Process non-input related message.
Original Offset: 005FCA30
Return Value: Is message available? true/false
Status: Complete - testing
*/
BOOL __cdecl do_non_input() {
	do_video();
	check_net();
	do_net();
	MSG msg[3];
	for (int i = 0; i < 3; i++) {
		msg[i].time = 0xFFFFFFFF;
	}
	if (!PeekMessage(&msg[0], NULL, WM_NULL, WM_INPUT, PM_NOREMOVE)
		|| !PeekMessage(&msg[1], NULL, WM_UNICHAR, WM_KEYDOWN | WM_INPUT, PM_NOREMOVE)
		|| !PeekMessage(&msg[2], NULL, WM_MOUSELAST, UNICODE_NOCHAR, PM_NOREMOVE)) {
		return false;
	}
	int a = 0;
	for (int c = 1; c < 3; c++) {
		if (msg[c].time < msg[a].time) {
			a = c;
		}
	}
	PeekMessage(&msg[0], msg[a].hwnd, msg[a].message, msg[a].message, PM_REMOVE);
	TranslateMessage(&msg[0]);
	DispatchMessage(&msg[0]);
	return true;
}

/*
Purpose: Process all non-input related messages.
Original Offset: 005FCB20
Return Value: n/a
Status: Complete - testing
*/
void __cdecl do_all_non_input() {
	do {
		*MsgStatus = 32;
	} while (do_non_input_());
	*MsgStatus = 0;
	do_net();
	check_net();
}

/*
Purpose: Process draw related message.
Original Offset: 005FCB60
Return Value: Is message available? true/false
Status: Complete - testing
*/
BOOL __cdecl do_draw() {
	do_video();
	check_net();
	do_net();
	MSG msg;
	if (PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, WM_CREATE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		return true;
	}
	return false;
}

/*
Purpose: Process all draw related messages.
Original Offset: 005FCBB0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl do_all_draws() {
	do {
		*MsgStatus = 1;
	} while (do_draw());
	*MsgStatus = 0;
}

/*
Purpose: Process keyboard related message.
Original Offset: 005FCC20
Return Value: Is message available? true/false
Status: Complete - testing
*/
BOOL __cdecl do_keyboard() {
	do_video();
	check_net();
	MSG msg;
	if (PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYLAST, WM_CREATE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		return true;
	}
	return false;
}

/*
Purpose: Process all keyboard related messages.
Original Offset: 005FCC70
Return Value: n/a
Status: Complete - testing
*/
void __cdecl do_all_keyboard() {
	do {
		*MsgStatus = 2;
	} while (do_keyboard());
	*MsgStatus = 0;
	do_net();
}

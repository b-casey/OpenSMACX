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
#include "stdafx.h"
#include "temp.h"
#include "veh.h"
#include "alpha.h"
#include "base.h"
#include "faction.h"
#include "game.h"
#include "general.h" // range
#include "log.h"
#include "map.h"
#include "strings.h"
#include "technology.h"
#include "terraforming.h"

VehPrototype *VehPrototypes = (VehPrototype *)0x009AB868; // [512]
Veh *Vehs = (Veh *)0x00952828; // [2049]
RulesChassis *Chassis = (RulesChassis *)0x0094A330; // [9]
RulesWeapon *Weapon = (RulesWeapon *)0x0094AE60; // [26]
RulesArmor *Armor = (RulesArmor *)0x0094F278; // [14]
RulesReactor *Reactor = (RulesReactor *)0x009527F8; // [4]
RulesAbility *Ability = (RulesAbility *)0x009AB538; // [29]
RulesMorale *Morale = (RulesMorale *)0x00952328; // [7]
RulesCombatMode *DefenseModes = (RulesCombatMode *)0x00946A00; // [3]
RulesCombatMode *OffenseModes = (RulesCombatMode *)0x00946178; // [3]
RulesOrder *Order = (RulesOrder *)0x0096C878; // [9]
LPSTR *PlansShortName = (LPSTR *)0x00945FE0; // [15]
LPSTR *PlansFullName = (LPSTR *)0x00952360; // [15]
LPSTR *Triad = (LPSTR *)0x0094F1A8; // [3]

int *VehCurrentCount = (int *)0x009A64C8;
int *VehDropLiftVehID = (int *)0x009B2280;
int *VehLiftX = (int *)0x009B2278;
int *VehLiftY = (int *)0x009B2284;
BOOL *VehBitError = (BOOL *)0x009B228C;
uint32_t *VehBasicBattleMorale = (uint32_t *)0x00912420; // [2] ; [0] offense, [1] defense?
int VehMoraleModifierCount; // only used by say_morale(), optimize to local var?
// Battle related globals
uint32_t *VehBattleModCount = (uint32_t *)0x00915614; // [2]
BOOL *VehBattleUnkTgl = (BOOL *)0x0091561C; // [2] ; planet_busting() + boom() + timers
int *VehBattleModifier = (int *)0x009155F0; // [2][4]
LPSTR *VehBattleDisplay = (LPSTR *)0x0090F554; // [2][4][80]
LPSTR VehBattleDisplayTerrain;

/*
Purpose: Generate an output string for the specified unit's morale.
Original Offset: 004B3FD0
Return Value: n/a
Status: Complete
*/
void __cdecl say_morale(LPSTR morale_output, uint32_t veh_id, int faction_id_vs_native) {
    // TODO: Look into inconsistencies related to morale display
    uint32_t morale = morale_veh(veh_id, false, faction_id_vs_native);
    uint32_t faction_id = Vehs[veh_id].faction_id;
    int proto_id = Vehs[veh_id].proto_id;
    std::string output = (proto_id < MaxVehProtoFactionNum &&
        (Weapon[VehPrototypes[proto_id].weapon_id].offense_rating < 0 
            || proto_id == BSC_SPORE_LAUNCHER)) 
        ? StringTable->get((int)Morale[morale].name_lifecycle)
        : StringTable->get((int)Morale[morale].name);
    if (VehPrototypes[proto_id].plan < PLAN_COLONIZATION) {
        uint32_t morale_penalty = 0;
        int home_base_id = Vehs[veh_id].home_base_id;
        if (home_base_id >= 0 && Bases[home_base_id].state & BSTATE_DRONE_RIOTS_ACTIVE && morale > 0
            && !(Players[faction_id].rule_flags & RFLAG_MORALE)) {
            output += " (-)";
            morale_penalty = 1;
        }
        VehMoraleModifierCount = 0;
        int base_id = base_at(Vehs[veh_id].x, Vehs[veh_id].y);
        if (base_id >= 0 && morale < 6) {
            if (has_fac_built(FAC_CHILDREN_CRECHE, base_id)) {
                VehMoraleModifierCount++;
                int morale_active = range(PlayersData[faction_id].soc_effect_active.morale, -4, 4);
                if (morale_active <= -2) {
                    morale_active++;
                }
                if (morale_active < 0) {
                    int morale_cap = morale + VehMoraleModifierCount;
                    do {
                        if (morale_cap >= 6) {
                            break;
                        }
                        VehMoraleModifierCount++;
                        morale_cap++;
                        morale_active++;
                    } while (morale_active < 0);
                }
                if(has_fac_built(FAC_BROOD_PIT, base_id) && proto_id < MaxVehProtoFactionNum
                    && (Weapon[VehPrototypes[proto_id].weapon_id].offense_rating < 0 
                        || proto_id == BSC_SPORE_LAUNCHER)) {
                    VehMoraleModifierCount++;
                }
            } else if (morale < 6 && has_fac_built(FAC_BROOD_PIT, base_id)
                && proto_id < MaxVehProtoFactionNum
                && (Weapon[VehPrototypes[proto_id].weapon_id].offense_rating < 0
                    || proto_id == BSC_SPORE_LAUNCHER)) {
                VehMoraleModifierCount++;
                int morale_active = range(PlayersData[faction_id].soc_effect_active.morale, -4, 4);
                if (morale_active <= -2) {
                    morale_active++;
                }
                if (morale_active < 0) {
                    int morale_cap = morale + VehMoraleModifierCount;
                    do {
                        if (morale_cap >= 6) {
                            break;
                        }
                        VehMoraleModifierCount++;
                        morale_cap++;
                        morale_active++;
                    } while (morale_active < 0);
                }
            }
        }
        int morale_pending = PlayersData[faction_id].soc_effect_pending.morale;
        if (morale_pending == 2 || morale_pending == 3) {
            VehMoraleModifierCount++;
        }
        if (!morale && !VehMoraleModifierCount) {
            VehMoraleModifierCount = 1;
        }
        if(VehMoraleModifierCount) {
            output += " (";
            for (int i = 0; i < VehMoraleModifierCount; i++) {
                output += "+";
            }
            output += ")";
        }
        VehMoraleModifierCount -= morale_penalty;
        if (Vehs[veh_id].state & VSTATE_DESIGNATE_DEFENDER) {
            output += "(d)";
        }
    }
    // TODO: assumes at least 1032 char nulled buffer (stringTemp), eventually remove
    strcat_s(morale_output, 1032, output.c_str());
}

/*
Purpose: Get the morale string for the specified unit and store it in the stringTemp buffer.
Original Offset: 004B43C0
Return Value: n/a
Status: Complete
*/
void __cdecl say_morale(uint32_t veh_id, int faction_id_vs_native) {
    say_morale(stringTemp->str, veh_id, faction_id_vs_native);
}

/*
Purpose: Calculate maximum range a faction's units can drop (air drops, Drop Pods).
Original Offset: 00500320
Return Value: Max range
Status: Complete
*/
uint32_t __cdecl drop_range(uint32_t faction_id) {
    if (!has_tech(Rules->tech_orb_insert_sans_spc_elev, faction_id) // default 'Graviton Theory'
        && !has_project(SP_SPACE_ELEVATOR, faction_id)) {
        return Rules->max_airdrop_sans_orb_insert;
    }
    return (*MapLongitudeBounds <= * MapLatitudeBounds) ? *MapLatitudeBounds : *MapLongitudeBounds;
}

/*
Purpose: Check whether the specified prototype is a planet buster.
Original Offset: 005004F0
Return Value: Reactor id if planet buster, otherwise 0
Status: Complete
*/
uint32_t __cdecl planet_buster2(int proto_id) {
    return VehPrototypes[proto_id].plan == PLAN_PLANET_BUSTER 
        ? VehPrototypes[proto_id].reactor_id : 0;
}

/*
Purpose: Check whether the specified unit is a planet buster.
Original Offset: 00500520
Return Value: Reactor id if planet buster, otherwise 0
Status: Complete
*/
uint32_t __cdecl planet_buster(int veh_id) {
    return planet_buster2(Vehs[veh_id].proto_id);
}

/*
Purpose: Calculate how defensive the specified tile is to the defending unit based on the terrain.
Original Offset: 005010C0
Return Value: Defense terrain value
Status: Complete
*/
uint32_t __cdecl defense_value(uint32_t faction_id, uint32_t x, uint32_t y, uint32_t veh_id_def, 
                               int veh_id_atk) {
    if (is_ocean(x, y) || base_who(x, y) >= 0) {
        return 2;
    }
    uint32_t bit = bit_at(x, y);
    if (bit & BIT_FUNGUS && altitude_at(x, y) >= ALT_BIT_OCEAN_SHELF && veh_id_atk >= 0
        && (!Vehs[veh_id_atk].faction_id 
            || has_project(SP_PHOLUS_MUTAGEN, Vehs[veh_id_atk].faction_id) 
            || (Weapon[VehPrototypes[Vehs[veh_id_atk].proto_id].weapon_id].offense_rating < 0
                && Vehs[veh_id_atk].proto_id < MaxVehProtoFactionNum))) {
                return 2;
    }
    uint32_t is_rocky = rocky_at(x, y) > TERRAIN_BIT_ROLLING;
    VehBattleDisplayTerrain = label_get(91); // "Rocky"
    uint32_t defense = is_rocky;
    if (bit & BIT_FUNGUS && altitude_at(x, y) >= ALT_BIT_OCEAN_SHELF && !defense
        && get_triad(veh_id_def) != TRIAD_AIR) {
        if (veh_id_atk >= 0
            && ((Weapon[VehPrototypes[Vehs[veh_id_atk].proto_id].weapon_id].offense_rating < 0
                && Vehs[veh_id_atk].proto_id < MaxVehProtoFactionNum)
            || has_project(SP_PHOLUS_MUTAGEN, Vehs[veh_id_atk].faction_id))) {
            defense = is_rocky;
        } else {
            VehBattleDisplayTerrain = label_get(338); // "Fungus"
            defense = (has_project(SP_PHOLUS_MUTAGEN, faction_id) 
                || ((Vehs[veh_id_def].proto_id < MaxVehProtoFactionNum 
                    && Weapon[VehPrototypes[Vehs[veh_id_def].proto_id].weapon_id].offense_rating < 0)))
                ? 2 : 1;
        }
    }
    if (bit & BIT_FOREST && !defense && (veh_id_atk < 0 || get_triad(veh_id_atk) == TRIAD_LAND)) {
        VehBattleDisplayTerrain = label_get(291); // "Forest"
        defense = 1;
    }
    return defense + 2;
}

/*
Purpose: Calculate the lifecycle (morale) of the specified native life unit.
Original Offset: 00501350
Return Value: Lifecycle (morale) value
Status: Complete
*/
uint32_t __cdecl morale_alien(int veh_id, int faction_id_vs_native) {
    int morale;
    // Fungal Tower specific code, shifted to start and added veh_id check to prevent crash
    if (veh_id >= 0 && Vehs[veh_id].proto_id == BSC_FUNGAL_TOWER) {
        morale = 0;
        int16_t x = Vehs[veh_id].x;
        int16_t y = Vehs[veh_id].y;
        // similar to is_coast() > except with fungus check + Ocean Shelf included
        for (uint32_t i = RadiusRange[0]; i < RadiusRange[1]; i++) {
            int x_radius = xrange(x + RadiusOffsetX[i]);
            int y_radius = y + RadiusOffsetY[i];
            if (on_map(x_radius, y_radius) && bit_at(x_radius, y_radius) & BIT_FUNGUS
                && altitude_at(x_radius, y_radius) >= ALT_BIT_OCEAN_SHELF) {
                morale++;
            }
        }
        morale -= 2;
    } else { // everything else
        if (*TurnCurrentNum < 45) {
            morale = 0;
        } else if (*TurnCurrentNum < 90) {
            morale = 1;
        } else if (*TurnCurrentNum < 170) {
            morale = 2;
        } else if (*TurnCurrentNum < 250) {
            morale = 3;
        } else if (*TurnCurrentNum < 330) {
            morale = 4;
        } else { // 330+
            morale = 6;
        }
        if (faction_id_vs_native > 0) {
            morale += (PlayersData[faction_id_vs_native].major_atrocities != 0)
                + (TectonicDetonationCount[faction_id_vs_native] != 0);
        }
        if (veh_id >= 0) {
            if (Vehs[veh_id].state & VSTATE_MONOLITH_UPGRADED) {
                morale++;
            }
            if (Vehs[veh_id].proto_id == BSC_LOCUSTS_OF_CHIRON) {
                morale++;
            }
            morale += (Vehs[veh_id].flags >> 3) & 3; // 0x8|0x10 > +1, +2, or +3; TODO: id unk flags
        }
    }
    return range(morale, 0, 6);
}

/*
Purpose: Calculate the psi combat factor for an attacking or defending unit.
Original Offset: 00501500
Return Value: Psi factor
Status: Complete
*/
int __cdecl psi_factor(int combat_ratio, uint32_t faction_id, BOOL is_attack, BOOL is_fungal_twr) {
    int rule_psi = Players[faction_id].rule_psi;
    int factor = rule_psi ? ((rule_psi + 100) * combat_ratio) / 100 : combat_ratio;
    if (is_attack) {
        if (has_project(SP_DREAM_TWISTER, faction_id)) {
            factor += factor / 2; // Psi Attack +50%
        }
    } else {
        if (has_project(SP_NEURAL_AMPLIFIER, faction_id)) {
            factor += factor / 2; // Psi Defense +50%s
        }
        if (is_fungal_twr) {
            factor += factor / 2; // SMACX only: likely +50% Fungal Tower defense bonus
        }
    }
    return factor;
}

/*
Purpose: Get the basic offense value for an attacking unit with an optional defender unit parameter.
Original Offset: 005015B0
Return Value: Basic offense
Status: Complete
*/
int __cdecl get_basic_offense(uint32_t veh_id_atk, int veh_id_def, uint32_t psi_combat_type, 
                              BOOL is_bombard, BOOL unk_tgl) { // artillery/duel related? Is offense of defender?
    uint32_t faction_id_atk = Vehs[veh_id_atk].faction_id;
    uint32_t proto_id_atk = Vehs[veh_id_atk].proto_id;
    uint32_t morale = faction_id_atk ? morale_veh(veh_id_atk, true, 0) 
        : morale_alien(veh_id_atk, veh_id_def >= 0 ? Vehs[veh_id_def].faction_id : -1);
    int base_id_atk = base_at(Vehs[veh_id_atk].x, Vehs[veh_id_atk].y);
    if (base_id_atk >= 0) {
        if (has_fac_built(FAC_CHILDREN_CRECHE, base_id_atk)) {
            morale++;
            int morale_active = range(PlayersData[faction_id_atk].soc_effect_active.morale, -4, 4);
            if (morale_active <= -2) {
                morale_active++;
            }
            morale -= morale_active;
        } else if (has_fac_built(FAC_BROOD_PIT, base_id_atk) && proto_id_atk < MaxVehProtoFactionNum
            && (Weapon[VehPrototypes[proto_id_atk].weapon_id].offense_rating < 0
                || proto_id_atk == BSC_SPORE_LAUNCHER)) {
            morale++;
            int morale_active = range(PlayersData[faction_id_atk].soc_effect_active.morale, -4, 4);
            if (morale_active <= -2) {
                morale_active++;
            }
            morale -= morale_active;
        }
        if (unk_tgl) {
            int morale_pending = PlayersData[faction_id_atk].soc_effect_pending.morale;
            if (morale_pending >= 2 && morale_pending <= 3) {
                morale++;
            }
            if (veh_id_def >= 0) {
                if (Vehs[veh_id_def].faction_id) {
                    if ((proto_id_atk >= MaxVehProtoFactionNum
                        || (Weapon[VehPrototypes[proto_id_atk].weapon_id].offense_rating >= 0
                            && proto_id_atk != BSC_SPORE_LAUNCHER))
                        && !has_abil(proto_id_atk, ABL_DISSOCIATIVE_WAVE)
                        && has_abil(Vehs[veh_id_def].proto_id, ABL_SOPORIFIC_GAS)) {
                        morale -= 2;
                    }
                } else {
                    morale++;
                }
            }
        }
    }
    if (unk_tgl) {
        morale = range(morale, 1, 6);
    }
    VehBasicBattleMorale[unk_tgl != 0] = morale; // shifted up from original
    morale += 6;
    uint32_t offense = offense_proto(proto_id_atk, veh_id_def, is_bombard);
    if (psi_combat_type) {
        offense = psi_factor(offense, faction_id_atk, true, false);
    }
    return offense * morale * 4;
}

/*
Purpose: Get the basic defense value for a defending unit with an optional attacker unit parameter.
Original Offset: 00501940
Return Value: Basic defense
Status: Complete
*/
int __cdecl get_basic_defense(uint32_t veh_id_def, int veh_id_atk, uint32_t psi_combat_type,
                              BOOL is_bombard) {
    uint32_t faction_id_def = Vehs[veh_id_def].faction_id;
    uint32_t proto_id_def = Vehs[veh_id_def].proto_id;
    uint32_t morale = faction_id_def ? morale_veh(veh_id_def, true, 0)
        : morale_alien(veh_id_def, veh_id_atk >= 0 ? Vehs[veh_id_atk].faction_id : -1);
    int base_id_def = base_at(Vehs[veh_id_def].x, Vehs[veh_id_def].y);
    if (base_id_def >= 0) {
        if (has_fac_built(FAC_CHILDREN_CRECHE, base_id_def)) {
            morale++;
            int morale_active = range(PlayersData[faction_id_def].soc_effect_active.morale, -4, 0);
            if (morale_active <= -2) {
                morale_active++;
            }
            morale -= morale_active;
        } else if (has_fac_built(FAC_BROOD_PIT, base_id_def) 
            &&  proto_id_def < MaxVehProtoFactionNum
            && (Weapon[VehPrototypes[proto_id_def].weapon_id].offense_rating < 0
                || proto_id_def == BSC_SPORE_LAUNCHER)) {
            morale++;
            int morale_active = range(PlayersData[faction_id_def].soc_effect_active.morale, -4, 4);
            if (morale_active <= -2) {
                morale_active++;
            }
            morale -= morale_active;
        }
        if (has_fac_built(FAC_HEADQUARTERS, base_id_def)) { // bug fix: per manual
            morale++; // "Units in a headquarters base automatically gain +1 Morale when defending."
        }
        int morale_pending = PlayersData[faction_id_def].soc_effect_pending.morale;
        if (morale_pending >= 2 && morale_pending <= 3) {
            morale++;
        }
        if (veh_id_atk >= 0 && !Vehs[veh_id_atk].faction_id) {
            morale++;
        }
    }
    if (veh_id_atk >= 0 && Vehs[veh_id_atk].faction_id && (proto_id_def >= MaxVehProtoFactionNum 
        || (Weapon[VehPrototypes[proto_id_def].weapon_id].offense_rating >= 0 
            && proto_id_def != BSC_SPORE_LAUNCHER)) 
        && !has_abil(proto_id_def, ABL_DISSOCIATIVE_WAVE)
        && has_abil(Vehs[veh_id_atk].proto_id, ABL_SOPORIFIC_GAS)) {
        morale -= 2;
    }
    morale = range(morale, 1, 6);
    VehBasicBattleMorale[1] = morale;
    morale += 6;
    uint32_t plan_def = VehPrototypes[proto_id_def].plan;
    if (plan_def == PLAN_ALIEN_ARTIFACT) {
        return 1;
    }
    if (plan_def == PLAN_INFO_WARFARE
        && Armor[VehPrototypes[proto_id_def].armor_id].defense_rating == 1
        // bug fix: added veh_id_atk bounds check to prevent potential arbitrary memory read
        && (veh_id_atk < 0 || VehPrototypes[Vehs[veh_id_atk].proto_id].plan != PLAN_INFO_WARFARE)) {
        return 1;
    }
    uint32_t defense = armor_proto(proto_id_def, veh_id_atk, is_bombard);
    if (psi_combat_type) {
        defense = psi_factor(defense, faction_id_def, false, proto_id_def == BSC_FUNGAL_TOWER);
    }
    return defense * morale;
}

/*
Purpose: Initialize or reset the battle related global variables.
Original Offset: 00501D30
Return Value: n/a
Status: Complete
*/
void __cdecl battle_init() {
    VehBattleModCount[0] = 0;
    VehBattleModCount[1] = 0;
    VehBattleUnkTgl[0] = false;
    VehBattleUnkTgl[1] = false;
}

/*
Purpose: Add combat battle modifier for type (offense, defense).
Original Offset: 00501D50
Return Value: n/a
Status: Complete
*/
void __cdecl add_bat(uint32_t type, int modifier, LPCSTR display_str) {
    // TODO: Revise global offsets once all references are decompiled.
    uint32_t offset = VehBattleModCount[type];
    if (modifier && offset < 4) {
        VehBattleModifier[type * 4 + offset] = modifier;
        strcpy_s((LPSTR)&VehBattleDisplay[type * 80 + offset * 20], 80, display_str);
        VehBattleModCount[type]++;
    }
}

/*
Purpose: Calculate the battle outcome between two units.
Original Offset: 00501DA0
Return Value: n/a
Status: Complete - testing
*/
void __cdecl battle_compute(int veh_id_atk, int veh_id_def, int *offense_out, int *defense_out, 
                            int combat_type) {
    int offense = 8;
    int defense = 8;
    int proto_id_atk = -1;
    int proto_id_def = -1;
    uint32_t faction_id_atk = 0;
    uint32_t faction_id_def = 0;
    if (veh_id_atk >= 0) {
        proto_id_atk = Vehs[veh_id_atk].proto_id;
        faction_id_atk = Vehs[veh_id_atk].faction_id;
    }
    if (veh_id_def >= 0) {
        proto_id_def = Vehs[veh_id_def].proto_id;
        faction_id_def = Vehs[veh_id_def].faction_id;
    }
    battle_init();
    BOOL is_artillery = combat_type & 1;
    BOOL is_bombard = (is_artillery // added veh_id_atk check
        || (veh_id_atk >= 0 && Chassis[VehPrototypes[proto_id_atk].chassis_id].missile)) ? true : false;
    uint32_t psi_combat_type = 0;
    if (!is_bombard || proto_id_atk == BSC_SPORE_LAUNCHER || proto_id_def == BSC_SPORE_LAUNCHER) {
        if (veh_id_atk >= 0
            && Weapon[VehPrototypes[proto_id_atk].weapon_id].offense_rating < 0) {
            psi_combat_type = 1; // PSI attacker
        }
        if (veh_id_def >= 0 && Armor[VehPrototypes[proto_id_def].armor_id].defense_rating < 0) {
            psi_combat_type |= 2; // PSI defender
        }
        // NOTE: PSI bonuses below are only for display purposes, actual calculation is done in 
        //       combat morale functions.
        if (psi_combat_type) {
            // Calculate PSI attack bonus
            int rule_psi_atk = Players[faction_id_atk].rule_psi;
            if (rule_psi_atk) {
                add_bat(0, rule_psi_atk, label_get(342)); // "Gaian Psi Bonus"
            }
            if (has_project(SP_DREAM_TWISTER, faction_id_atk)) {
                add_bat(0, 50, label_get(343)); // "Dream Twist"
            }
            // PSI Defense?
            if (psi_combat_type) { // shouldn't this be & 2?
                int rule_psi_def = Players[faction_id_def].rule_psi;
                if (rule_psi_def) {
                    add_bat(1, rule_psi_def, label_get(342)); // "Gaian Psi Bonus"
                }
                if (has_project(SP_NEURAL_AMPLIFIER, faction_id_def)) {
                    add_bat(1, 50, label_get(344)); // "Neural Amp"
                }
                if (veh_id_def >= 0 && proto_id_def == BSC_FUNGAL_TOWER) {
                    add_bat(1, 50, VehPrototypes[BSC_FUNGAL_TOWER].veh_name); // "Fungal Tower"
                }
            }
        }
        if (veh_id_atk >= 0) {
            offense = get_basic_offense(veh_id_atk, veh_id_def, psi_combat_type, is_bombard, false);
            if (VehPrototypes[proto_id_atk].plan != PLAN_INFO_WARFARE) {
                if (veh_id_def >= 0 && !combat_type) { // checking if isn't wpn vs wpn ; air toggle
                    int x_def = Vehs[veh_id_def].x;
                    int y_def = Vehs[veh_id_def].y;
                    if (bit_at(x_def, y_def) & BIT_FUNGUS
                        && altitude_at(x_def, y_def) >= ALT_BIT_OCEAN_SHELF
                        && ((proto_id_atk < MaxVehProtoFactionNum
                            && Weapon[VehPrototypes[proto_id_atk].weapon_id].offense_rating < 0)
                            || has_project(SP_PHOLUS_MUTAGEN, faction_id_atk))
                        && Weapon[VehPrototypes[proto_id_def].weapon_id].offense_rating >= 0) {
                        offense += offense / 2;
                        add_bat(0, 50, label_get(338)); // "Fungus"
                    }
                    if (psi_combat_type & 2
                        && (VehPrototypes[proto_id_atk].weapon_id == WPN_RESONANCE_LASER
                            || VehPrototypes[proto_id_atk].weapon_id == WPN_RESONANCE_BOLT)) {
                        offense = (offense * 125) / 100;
                        add_bat(0, 25, label_get(1110)); // "Resonance Attack"
                    }
                    if (!has_abil(proto_id_def, ABL_DISSOCIATIVE_WAVE) && psi_combat_type &&
                        has_abil(proto_id_atk, ABL_EMPATHIC) && Rules->combat_pct_emp_song_atk_vs_psi) {
                        offense = offense * (Rules->combat_pct_emp_song_atk_vs_psi + 100) / 100;
                        add_bat(0, Rules->combat_pct_emp_song_atk_vs_psi,
                            Ability[abil_index(ABL_EMPATHIC)].name);
                    }
                }
                if (Vehs[veh_id_atk].state & VSTATE_MADE_AIRDROP && has_abil(proto_id_atk, ABL_DROP_POD)
                    && Rules->combat_pen_pct_atk_airdrop) {
                    offense = (100 - Rules->combat_pct_emp_song_atk_vs_psi) * offense / 100;
                    /*
                    uint32_t dropRange;
                    if (has_tech(Rules->tech_orb_insert_sans_spc_elev, faction_id_atk)
                        || has_project(SP_SPACE_ELEVATOR, faction_id_atk)) {
                        dropRange = (*MapHorizontalBounds <= *MapLatitudeBounds)
                            ? *MapLatitudeBounds : *MapHorizontalBounds;
                    } else {
                        dropRange = Rules->max_airdrop_sans_orb_insert;
                    }
                    */
                    add_bat(0, Rules->combat_pen_pct_atk_airdrop,
                        drop_range(faction_id_atk) <= Rules->max_airdrop_sans_orb_insert 
                        ? label_get(437) // "Air Drop"
                        : label_get(438)); // "Orbital Insertion"
                }
                if (Players[faction_id_atk].rule_flags & RFLAG_FANATIC
                    && Rules->combat_pct_fanatic_atk_bonus && !combat_type && !psi_combat_type) {
                    offense = (Rules->combat_pct_fanatic_atk_bonus + 100) * offense / 100;
                    add_bat(0, Rules->combat_pct_fanatic_atk_bonus, label_get(528));
                }
                int bonus_count = Players[faction_id_atk].faction_bonus_count;
                for (int i = 0; i < bonus_count; i++) {
                    if (Players[faction_id_atk].faction_bonus_id[i] == RULE_OFFENSE) {
                        int rule_off_bonus = Players[faction_id_atk].faction_bonus_val1[i];
                        offense = offense * rule_off_bonus / 100;
                        add_bat(0, rule_off_bonus, label_get(1108)); // "Alien Offense"
                    }
                }
                if (psi_combat_type && faction_id_atk) {
                    int planet_se_atk = PlayersData[faction_id_atk].soc_effect_active.planet;
                    if (planet_se_atk && Rules->combat_pct_psi_atk_bonus_planet) {
                        int planet_modifier = planet_se_atk * Rules->combat_pct_psi_atk_bonus_planet;
                        add_bat(0, planet_modifier, label_get(625)); // "Planet"
                        offense = (planet_modifier + 100) * offense / 100;
                    }
                }
            }
        }
        if (!(combat_type & 2) && veh_id_def >= 0) {
            defense = get_basic_defense(veh_id_def, veh_id_atk, psi_combat_type, is_bombard);
            int bonus_count = Players[faction_id_def].faction_bonus_count;
            for (int i = 0; i < bonus_count; i++) {
                if (Players[faction_id_def].faction_bonus_id[i] == RULE_DEFENSE) {
                    int rule_def_bonus = Players[faction_id_def].faction_bonus_val1[i];
                    defense = defense * rule_def_bonus / 100;
                    add_bat(1, rule_def_bonus, label_get(1109)); // "Alien Defense"
                }
            }
            if (veh_id_atk >= 0 && VehPrototypes[proto_id_def].plan == PLAN_INFO_WARFARE) {
                defense *= 4;
            } else {
                int x_def = Vehs[veh_id_def].x;
                int y_def = Vehs[veh_id_def].y;
                int base_id_def = base_at(x_def, y_def);
                if (psi_combat_type) {
                    if (base_id_def >= 0 && Rules->combat_pct_base_def) {
                        add_bat(1, Rules->combat_pct_base_def, label_get(332)); // "Base"
                        defense = defense * (Rules->combat_pct_base_def + 100) / 100;
                    }
                    defense *= 4;
                } else {
                    uint32_t triad_def = get_proto_triad(proto_id_def);
                    uint32_t terrain_def = (triad_def == TRIAD_AIR) 
                        ? 2 : defense_value(faction_id_def, x_def, y_def, veh_id_def, veh_id_atk);
                    uint32_t alt_atk;
                    uint32_t alt_def;
                    if (veh_id_atk >= 0 && get_triad(veh_id_atk) == TRIAD_LAND) {
                        if (combat_type && Rules->combat_pct_art_bonus_lvl_alt && triad_def == TRIAD_LAND
                            && (alt_atk = alt_at(Vehs[veh_id_atk].x, Vehs[veh_id_atk].y),
                                alt_def = alt_at(x_def, y_def), alt_atk > alt_def)) {
                            offense = offense * (Rules->combat_pct_art_bonus_lvl_alt 
                                * (alt_atk - alt_def) + 100) / 100;
                            add_bat(0, Rules->combat_pct_art_bonus_lvl_alt, label_get(576)); //"Altitude"
                        }
                        if (Rules->combat_pct_mobile_open_ground && !combat_type && base_id_def < 0
                            && terrain_def == 2 
                            && rocky_at(x_def, y_def) < TERRAIN_BIT_ROCKY) {
                            uint32_t speed_atk = speed_proto(proto_id_atk);
                            if (speed_atk > Rules->move_rate_roads 
                                && speed_proto(proto_id_def) < speed_atk) {
                                offense = offense * (Rules->combat_pct_mobile_open_ground + 100) / 100;
                                // "Mobile in open"
                                add_bat(0, Rules->combat_pct_mobile_open_ground, label_get(611));
                            }
                        }
                        if (Rules->combat_pct_def_vs_mobile_rough && !combat_type && (terrain_def > 2
                            || base_id_def >= 0) && rocky_at(x_def, y_def) < TERRAIN_BIT_ROCKY
                            && speed_proto(proto_id_atk) > Rules->move_rate_roads) {
                            defense = defense * (Rules->combat_pct_def_vs_mobile_rough + 100) / 100;
                            // "Rough vs. Mobile" : "Mobile vs. Base"
                            add_bat(1, Rules->combat_pct_def_vs_mobile_rough, base_id_def < 0
                                ? label_get(548) : label_get(612));
                        }
                        if (Rules->combat_pct_atk_road && !combat_type) {
                            // TODO: add check road/tube Combat % -> attacking along road
                            offense = offense * (Rules->combat_pct_atk_road + 100) / 100;
                            add_bat(0, Rules->combat_pct_atk_road, label_get(606)); // "Road Attack"
                        }

                        if (Rules->combat_pen_pct_atk_lwr_elev && !combat_type 
                            && altitude_at(x_def, y_def) 
                            > altitude_at(Vehs[veh_id_atk].x, Vehs[veh_id_atk].y)
                            && !has_abil(proto_id_atk, ABL_ANTIGRAV_STRUTS)) {
                            defense = defense * (Rules->combat_pen_pct_atk_lwr_elev + 100) / 100;
                            add_bat(1, Rules->combat_pen_pct_atk_lwr_elev, label_get(441)); // "Uphill"
                        }
                        if (Rules->combat_pct_atk_higher_elev && !combat_type 
                            && altitude_at(Vehs[veh_id_atk].x, Vehs[veh_id_atk].y) 
                            > altitude_at(x_def, y_def) 
                            && !has_abil(proto_id_def, ABL_ANTIGRAV_STRUTS)) {
                            offense = offense * (Rules->combat_pct_atk_higher_elev + 100) / 100;
                            add_bat(0, Rules->combat_pct_atk_road, label_get(330)); // "Downhill"
                        }
                    }
                    defense *= terrain_def;
                    if (terrain_def > 2) {
                        std::string terrain_modifier = label_get(331); // "Terrain"
                        terrain_modifier += " (";
                        terrain_modifier += StringTable->get(int(VehBattleDisplayTerrain));
                        terrain_modifier += ")";
                        add_bat(1, 10 * (5 * terrain_def - 10), terrain_modifier.c_str());
                    }
                    LPSTR display_def; // only one is displayed
                    uint32_t def_multi = 2;
                    if (bit_at(x_def, y_def) & BIT_BUNKER && (veh_id_atk < 0
                        || get_triad(veh_id_atk) != TRIAD_AIR)) {
                        def_multi = 3;
                        display_def = label_get(358); // "Bunker"
                    }
                    if (base_id_def) {
                        display_def = label_get(332); // "Base"
                        def_multi = 2;
                        if (veh_id_atk >= 0) {
                            uint32_t triad_atk = get_triad(veh_id_atk);
                            uint32_t fac_modifier = 0;
                            switch (triad_atk) {
                              case TRIAD_LAND:
                                if (has_fac_built(FAC_PERIMETER_DEFENSE, base_id_def)
                                    || has_project(SP_CITIZENS_DEFENSE_FORCE, faction_id_def)) {
                                    display_def = label_get(354); // "Perimeter"
                                    fac_modifier = 4;
                                }
                                break;
                              case TRIAD_SEA:
                                if (has_fac_built(FAC_NAVAL_YARD, base_id_def)
                                    || has_project(SP_MARITIME_CONTROL_CENTER, faction_id_def)) {
                                    display_def = Facility[FAC_NAVAL_YARD].name;
                                    fac_modifier = 4;
                                }
                                break;
                              case TRIAD_AIR:
                                if (has_fac_built(FAC_AEROSPACE_COMPLEX, base_id_def)
                                    || has_project(SP_CLOUDBASE_ACADEMY, faction_id_def)) {
                                    display_def = Facility[FAC_AEROSPACE_COMPLEX].name;
                                    fac_modifier = 4;
                                }
                                break;
                              default:
                                fac_modifier = def_multi;
                                break;
                            }
                            if (has_fac_built(FAC_TACHYON_FIELD, base_id_def)) {
                                fac_modifier += 2;
                                display_def = label_get(357); // "Tachyon"
                            }
                            if (fac_modifier < 2 || def_multi == 2
                                && (veh_id_atk < 0 || has_abil(proto_id_atk, ABL_BLINK_DISPLACER)
                                    && Rules->combat_pct_base_def)) {
                                add_bat(1, Rules->combat_pct_base_def, label_get(332)); // "Base"
                                defense = defense * (Rules->combat_pct_base_def + 100) / 100;
                            }
                            if (veh_id_atk >= 0 && has_abil(proto_id_atk, ABL_BLINK_DISPLACER)) {
                                if (def_multi > 2) {
                                    def_multi = 2;
                                }
                                display_def = label_get(428); // "Base vs. Blink"
                            }
                            if (Rules->combat_pct_infantry_vs_base && !combat_type && base_id_def >= 0
                                && faction_id_atk && proto_id_atk > MaxVehProtoFactionNum
                                && Weapon[VehPrototypes[proto_id_atk].weapon_id].offense_rating >= 0 
                                && proto_id_atk != BSC_SPORE_LAUNCHER && !get_proto_triad(proto_id_atk)
                                && Chassis[VehPrototypes[proto_id_atk].chassis_id].speed == 1) {
                                offense = offense * (Rules->combat_pct_infantry_vs_base + 100) / 100;
                                // "Infantry vs. Base"
                                add_bat(0, Rules->combat_pct_infantry_vs_base, label_get(547));
                            }
                            uint32_t bit_def;
                            if (is_artillery && def_multi <= 2 && base_id_def < 0
                                && rocky_at(x_def, y_def) < TERRAIN_BIT_ROCKY
                                && (bit_def = bit_at(x_def, y_def),
                                    !(bit_def & BIT_FOREST)) && (bit_def & BIT_FUNGUS
                                        || altitude_at(x_def, y_def) 
                                        < ALT_BIT_OCEAN_SHELF)) {
                                def_multi = 3;
                                display_def = label_get(525); // "Open Ground"
                            }
                            defense *= def_multi;
                            if (def_multi > 2) {
                                add_bat(1, 10 * (5 * def_multi - 10), 
                                    StringTable->get(int(display_def)));
                            }
                        }
                    }
                }
                if (faction_id_def && veh_id_atk >= 0 
                    && Chassis[VehPrototypes[proto_id_atk].chassis_id].missile) {
                    for (int i = 0; i < 25; i++) {
                        int x_radius = xrange(x_def + RadiusOffsetX[i]);
                        int y_radius = y_def + RadiusOffsetY[i];
                        if (on_map(x_radius, y_radius)) {
                            int base_id_radius = base_at(x_radius, y_radius);
                            if (base_id_radius >= 0
                                && Bases[base_id_radius].faction_id_current == faction_id_def
                                && has_fac_built(FAC_FLECHETTE_DEFENSE_SYS, base_id_radius)) {
                                defense = 3 * defense / 2;
                                add_bat(1, 50, label_get(1113)); // "Flechette"
                            }
                        }
                    }
                }
                if (!is_ocean(x_def, y_def)) {
                    uint32_t sensor_def = 0;
                    if (faction_id_def) {
                        for (int i = 0; i < 25; i++) {
                            int x_radius = xrange(x_def + RadiusOffsetX[i]);
                            int y_radius = y_def + RadiusOffsetY[i];
                            uint32_t sensor_status;
                            if (on_map(x_radius, y_radius)
                                && (sensor_status = is_sensor(x_radius, y_radius), sensor_status)) {
                                BOOL has_sensor = false;
                                if (!is_ocean(x_radius, y_radius)) {
                                    int faction_id_terr = whose_territory(faction_id_def, x_radius, 
                                        y_radius, NULL, false);
                                    if (faction_id_terr < 0 
                                        || (uint32_t)faction_id_terr == faction_id_def) {
                                        has_sensor = true;
                                    }
                                } else {
                                    int base_id_find = base_find(x_radius, y_radius);
                                    // assumes will find a base?
                                    if (Bases[base_id_find].faction_id_current == faction_id_def) {
                                        has_sensor = true;
                                    }
                                }
                                if (has_sensor) {
                                    sensor_def |= 1;
                                    if (sensor_status > 1) {
                                        sensor_def |= 2;
                                    }
                                }
                            }
                        }
                    }
                    if (sensor_def & 1) {
                        defense = defense * (Rules->combat_pct_def_range_sensor + 100) / 100;
                        add_bat(1, Rules->combat_pct_def_range_sensor, label_get(613)); // "Sensor"
                    }
                    if (sensor_def & 2) {
                        defense = defense * (Rules->combat_pct_def_range_sensor + 100) / 100;
                        add_bat(1, Rules->combat_pct_def_range_sensor, label_get(1123)); // "GSP"
                    }
                }
                if (!has_abil(proto_id_atk, ABL_DISSOCIATIVE_WAVE) && Rules->combat_pct_trance_def_vs_psi
                    && veh_id_atk >= 0 && psi_combat_type & 1 && has_abil(proto_id_def, ABL_TRANCE)) {
                    defense = defense * (Rules->combat_pct_trance_def_vs_psi + 100) / 100;
                    add_bat(1, Rules->combat_pct_trance_def_vs_psi, label_get(329)); // "Trance"
                }
                uint32_t armor_id_def = VehPrototypes[proto_id_def].armor_id;
                if (psi_combat_type & 1 && veh_id_atk >= 0 && (armor_id_def == ARM_RESONANCE_3_ARMOR 
                    || armor_id_def == ARM_RESONANCE_8_ARMOR)) {
                    defense = 125 * defense / 100;
                    add_bat(1, 25, label_get(1111)); // "Resonance Def."
                }
                // add check veh_id_atk >= 0 to skip this entire code section? original jumps to end
                if (veh_id_atk >= 0 && get_proto_triad(proto_id_atk) == TRIAD_AIR
                    && has_abil(proto_id_atk, ABL_AIR_SUPERIORITY) && !psi_combat_type) {
                    if (get_proto_triad(proto_id_def) == TRIAD_AIR) {
                        int ground_strike_pen = Rules->combat_pen_pct_air_supr_vs_grnd;
                        if (ground_strike_pen) {
                            offense = offense * (100 - ground_strike_pen) / 100;
                            add_bat(0, -ground_strike_pen, label_get(448)); // "Ground Strike"
                        }
                    } else {
                        int air_to_air = Rules->combat_pct_air_supr_vs_air;
                        if (air_to_air && !has_abil(proto_id_def, ABL_AIR_SUPERIORITY)) {
                            offense = offense * (air_to_air + 100) / 100;
                            add_bat(0, air_to_air, label_get(449)); // "Air-to-Air"
                        }
                    }
                }
                if (veh_id_atk >= 0 && get_proto_triad(proto_id_atk) == TRIAD_AIR // added veh_id_atk chk
                    && get_proto_triad(proto_id_def) == TRIAD_AIR
                    && has_abil(proto_id_def, ABL_AIR_SUPERIORITY)
                    && has_abil(proto_id_atk, ABL_AIR_SUPERIORITY) && !psi_combat_type
                    && !Chassis[VehPrototypes[proto_id_atk].chassis_id].missile
                    && !Chassis[VehPrototypes[proto_id_def].chassis_id].missile
                    && Rules->combat_pct_air_supr_vs_air) {
                    defense = defense * (Rules->combat_pct_air_supr_vs_air + 100) / 100;
                    add_bat(1, Rules->combat_pct_air_supr_vs_air, label_get(449)); // "Air-to-Air"
                }
                if (!Weapon[VehPrototypes[proto_id_def].weapon_id].offense_rating
                    && Armor[VehPrototypes[proto_id_def].armor_id].defense_rating == 1 &&
                    (faction_id_atk || base_id_def < 0) && Rules->combat_pen_pct_non_cbt_def_vs_cbt) {
                    defense = defense * (100 - Rules->combat_pen_pct_non_cbt_def_vs_cbt) / 100;
                    add_bat(1, -Rules->combat_pen_pct_non_cbt_def_vs_cbt, label_get(439)); // "Non Combat"
                }
                if (veh_id_atk >= 0 // added veh_id_atk check
                    && get_proto_triad(proto_id_atk) == TRIAD_SEA
                    && get_proto_triad(proto_id_def) == TRIAD_SEA && base_id_def >= 0 
                    && Rules->combat_pct_bonus_vs_ship_port) {
                    offense = offense * (Rules->combat_pct_bonus_vs_ship_port + 100) / 100;
                    add_bat(0, Rules->combat_pct_bonus_vs_ship_port, label_get(335)); // "In Port"
                }
                if (armor_id_def == ARM_PULSE_3_ARMOR || armor_id_def == ARM_PULSE_8_ARMOR
                    && get_proto_triad(proto_id_atk) == TRIAD_LAND
                    && Chassis[VehPrototypes[proto_id_atk].chassis_id].speed > 1) {
                    defense = 125 * defense / 100;
                    add_bat(1, 25, label_get(1112)); // "Pulse Defense"
                }
                if (!has_abil(proto_id_atk, ABL_DISSOCIATIVE_WAVE)
                    && has_abil(proto_id_def, ABL_COMM_JAMMER)
                    && get_proto_triad(proto_id_atk) == TRIAD_LAND
                    && Chassis[VehPrototypes[proto_id_atk].chassis_id].speed > 1 
                    && Rules->combat_pct_com_jam_def_vs_mobl) {
                    defense = defense * (Rules->combat_pct_com_jam_def_vs_mobl + 100) / 100;
                    add_bat(1, Rules->combat_pct_com_jam_def_vs_mobl, label_get(336)); // "Jammer"
                }
                if (!has_abil(proto_id_atk, ABL_DISSOCIATIVE_WAVE) && has_abil(proto_id_def, ABL_AAA)
                    && get_proto_triad(proto_id_atk) == TRIAD_AIR && Rules->combat_pct_aaa_bonus_vs_air) {
                    defense = defense * (Rules->combat_pct_aaa_bonus_vs_air + 100) / 100;
                    add_bat(1, Rules->combat_pct_aaa_bonus_vs_air, label_get(337)); // "Tracking"
                }
            }
        } else if (veh_id_def >= 0) {
            defense = get_basic_offense(veh_id_def, veh_id_atk, psi_combat_type, is_bombard, true);
            if (!(combat_type & 0x18)) {
                uint32_t triad_atk = get_proto_triad(proto_id_atk);
                uint32_t triad_def = get_proto_triad(proto_id_def);
                if (triad_def == TRIAD_SEA && triad_atk == TRIAD_LAND) {
                    int gun_vs_ship_art = Rules->combat_pct_land_gun_vs_ship_art;
                    if (gun_vs_ship_art) {
                        offense = (gun_vs_ship_art + 100) * offense / 100;
                        add_bat(0, gun_vs_ship_art, label_get(435)); // "Land Based Guns"
                    }
                } else if (triad_atk == TRIAD_SEA && triad_def == TRIAD_LAND) {
                    int gun_vs_ship_art = Rules->combat_pct_land_gun_vs_ship_art;
                    if (gun_vs_ship_art) {
                        defense = (gun_vs_ship_art + 100) * defense / 100;
                        add_bat(1, gun_vs_ship_art, label_get(435)); // "Land Based Guns"
                    }
                } else if (Rules->combat_pct_art_bonus_lvl_alt) {
                    uint32_t alt_atk = alt_at(Vehs[veh_id_atk].x, Vehs[veh_id_atk].y);
                    uint32_t alt_def = alt_at(Vehs[veh_id_def].x, Vehs[veh_id_def].y);
                    if (alt_def >= alt_atk) {
                        if (alt_def > alt_atk) { // ???
                            int alt_modifier_def = (alt_def - alt_atk) 
                                * Rules->combat_pct_art_bonus_lvl_alt;
                            defense = defense * (alt_modifier_def + 100) / 100;
                            add_bat(1, alt_modifier_def, label_get(576)); // "Altitude"
                        }
                    } else {
                        int alt_modifier_atk = (alt_atk - alt_def) * Rules->combat_pct_art_bonus_lvl_alt;
                        offense = offense * (alt_modifier_atk + 100) / 100;
                        add_bat(0, alt_modifier_atk, label_get(576)); // "Altitude"
                    }
                }
            }
        }
    }
    if (offense_out) {
        *offense_out = offense;
    }
    if (defense_out) {
        *defense_out = defense;
    }
}

/*
Purpose: Determine the best defender in a stack.
Original Offset: 005044D0
Return Value: Unit id of the best defender
Status: Complete - testing
*/
uint32_t __cdecl best_defender(uint32_t veh_id_def, int veh_id_atk, BOOL check_artillery) {
    int offense = veh_id_atk >= 0 
        ? get_basic_offense(veh_id_atk, veh_id_def, false, false, false) : 8;
    int x_def = Vehs[veh_id_def].x;
    int y_def = Vehs[veh_id_def].y;
    BOOL is_ocean_def = is_ocean(x_def, y_def);
    int base_id_def = base_at(x_def, y_def);
    int defender_search = -999;
    uint32_t best_def_veh_id = veh_id_def;
    for (int i = veh_top(veh_id_def); i >= 0; i = Vehs[i].next_veh_id_stack) {
        uint32_t proto_id_def = Vehs[i].proto_id;
        if ((get_proto_triad(proto_id_def) != TRIAD_LAND || !is_ocean_def && base_id_def >= 0)
            // added veh_id_atk bounds check
            && (veh_id_atk < 0 || VehPrototypes[Vehs[veh_id_atk].proto_id].plan != PLAN_INFO_WARFARE
                || VehPrototypes[proto_id_def].plan == PLAN_INFO_WARFARE)) {
            uint32_t combat_type = (can_arty(proto_id_def, true)
                && (veh_id_atk || can_arty(Vehs[veh_id_atk].proto_id, true))) ? 2 : 0; // added bounds?
            if (veh_id_atk >= 0 && get_triad(veh_id_atk) != TRIAD_AIR
                && has_abil(proto_id_def, ABL_AIR_SUPERIORITY)
                && !Chassis[VehPrototypes[proto_id_def].chassis_id].missile
                && !Chassis[VehPrototypes[Vehs[veh_id_atk].proto_id].chassis_id].missile
                && Weapon[VehPrototypes[Vehs[veh_id_atk].proto_id].weapon_id].offense_rating > 0
                && Armor[VehPrototypes[Vehs[veh_id_atk].proto_id].armor_id].defense_rating > 0
                && Weapon[VehPrototypes[proto_id_def].weapon_id].offense_rating > 0) {
                combat_type |= 0xA; // air combat
                if (get_proto_triad(proto_id_def) == TRIAD_AIR) {
                    combat_type |= 0x10;
                }
            }
            int offense_output;
            int defense_output;
            battle_compute(veh_id_atk, i, &offense_output, &defense_output, combat_type);
            if (!offense_output) {
                break;
            }
            uint32_t proto_pwr_def = proto_power(i);
            int def_modifier = (((((range(proto_pwr_def - Vehs[i].dmg_incurred, 0, 9999)
                * defense_output) / proto_pwr_def) * offense) / offense_output) / 8)
                - Weapon[VehPrototypes[proto_id_def].weapon_id].offense_rating;
            uint32_t plan_def = VehPrototypes[proto_id_def].plan;
            if (plan_def < PLAN_COLONIZATION || plan_def == PLAN_TERRAFORMING) {
                def_modifier *= 16;
            }
            if (veh_id_atk >= 0 && get_triad(veh_id_atk) == TRIAD_AIR 
                && has_abil(proto_id_def, ABL_AIR_SUPERIORITY) 
                && get_proto_triad(proto_id_def) == TRIAD_AIR 
                && !Chassis[VehPrototypes[proto_id_def].chassis_id].missile
                && !Chassis[VehPrototypes[Vehs[veh_id_atk].proto_id].chassis_id].missile) {
                def_modifier += 0x80000;
            } else if (veh_id_atk >= 0 && get_triad(veh_id_atk) == TRIAD_AIR
                && has_abil(Vehs[veh_id_atk].proto_id, ABL_AIR_SUPERIORITY)
                && get_proto_triad(proto_id_def) == TRIAD_AIR && base_id_def < 0
                && !(bit_at(x_def, y_def) & BIT_AIRBASE)
                && !stack_check(veh_id_def, 6, ABL_CARRIER, -1, -1)
                && !Chassis[VehPrototypes[proto_id_def].chassis_id].missile
                && !Chassis[VehPrototypes[Vehs[veh_id_atk].proto_id].chassis_id].missile) {
                def_modifier += 0x80000;
            } else if (check_artillery) {
                if (can_arty(proto_id_def, true)) {
                    def_modifier += 0x80000;
                }
            } else if ((!stack_check(veh_id_def, 3, TRIAD_AIR, -1, -1) || base_id_def >= 0 
                || bit_at(x_def, y_def) & BIT_AIRBASE 
                || stack_check(veh_id_def, 6, ABL_CARRIER, -1, -1)) 
                && Vehs[i].state & VSTATE_DESIGNATE_DEFENDER) {
                def_modifier += 0x80000;
            }
            int defense = i + (def_modifier << 11);
            if (defense > defender_search) {
                defender_search = defense;
                best_def_veh_id = i;
            }
        }
    }
    return best_def_veh_id;
}

/*
Purpose: Determine whether any enemy naval transports are carrying land units within range to 
         attack the specified base. If so, set the units to move towards the base.
Original Offset: 00506490
Return Value: n/a
Status: Complete
*/
void __cdecl invasions(uint32_t base_id) {
    uint32_t faction_id_base = Bases[base_id].faction_id_current;
    int16_t base_x = Bases[base_id].x;
    int16_t base_y = Bases[base_id].y;
    for (int i = 0; i < *VehCurrentCount; i++) {
        uint32_t faction_id_veh = Vehs[i].faction_id;
        if (faction_id_veh && !is_human(faction_id_veh) && faction_id_veh != faction_id_base
            && !has_treaty(faction_id_veh, faction_id_base, DTREATY_TREATY)) {
            int veh_x = Vehs[i].x;
            int veh_y = Vehs[i].y;
            if (is_ocean(veh_x, veh_y) && veh_cargo(i) && stack_check(i, 3, TRIAD_LAND, -1, -1)) {
                int proximity = vector_dist(base_x, base_y, veh_x, veh_y);
                if (proximity <= (int)speed(i, false)) {
                    Vehs[i].order = ORDER_MOVE_TO;
                    Vehs[i].waypoint_x[0] = base_x;
                    Vehs[i].waypoint_y[0] = base_y;
                }
            }
        }
    }
}

/*
Purpose: Direct the unit to start moving automatically towards the specified tile.
Original Offset: 00560AD0
Return Value: n/a
Status: Complete
*/
void __cdecl go_to(uint32_t veh_id, uint8_t type, int x, int y) {
    Vehs[veh_id].order = ORDER_MOVE_TO;
    Vehs[veh_id].move_to_ai_type = type;
    Vehs[veh_id].waypoint_x[0] = (int16_t)x;
    Vehs[veh_id].waypoint_y[0] = (int16_t)y;
    if (VehPrototypes[Vehs[veh_id].proto_id].plan == PLAN_COLONIZATION) {
        Vehs[veh_id].terraforming_turns = 0;
    }
}

/*
Purpose: Get the unit on the top of the stack.
Original Offset: 00579920
Return Value: Unit id if found, otherwise -1
Status: Complete
*/
int __cdecl veh_top(int veh_id) {
    if (veh_id < 0) {
        return -1;
    }
    int top_veh_id = veh_id;
    for (int i = Vehs[top_veh_id].prev_veh_id_stack; i >= 0; i = Vehs[i].prev_veh_id_stack) {
        top_veh_id = i;
    }
    return top_veh_id;
}

/*
Purpose: Get the current moves left for the specified unit.
Original Offset: 00579960
Return Value: Remaining moves
Status: Complete
*/
uint32_t __cdecl veh_moves(uint32_t veh_id) {
    return range(speed(veh_id, false) - Vehs[veh_id].moves_expended, 0, 999);
}

/*
Purpose: Get the specified unit's reactor power value from its prototype.
Original Offset: 005799A0
Return Value: Power
Status: Complete
*/
uint32_t __cdecl proto_power(uint32_t veh_id) {
    int proto_id = Vehs[veh_id].proto_id;
    if (VehPrototypes[proto_id].plan == PLAN_ALIEN_ARTIFACT) {
        return 1;
    }
    return range(VehPrototypes[proto_id].reactor_id, 1, 100) * 10;
}

/*
Purpose: Determine whether the specified unit is eligible for a monolith morale upgrade.
Original Offset: 00579F80
Return Value: Is unit eligible for a monolith morale upgrade? true/false
Status: Complete
*/
BOOL __cdecl want_monolith(uint32_t veh_id) {
    if (!(Vehs[veh_id].state & VSTATE_MONOLITH_UPGRADED)
        && morale_veh(veh_id, true, 0) < MORALE_ELITE && Vehs[veh_id].morale < MORALE_ELITE
        && Weapon[VehPrototypes[Vehs[veh_id].proto_id].weapon_id].offense_rating) {
        return true;
    }
    return false;
}

/*
Purpose: Calculate the armor strategy for the specified armor id.
Original Offset: 0057D270
Return Value: Armor strategy
Status: Complete
*/
int __cdecl arm_strat(uint32_t armor_id, uint32_t faction_id) {
    if (!ExpansionEnabled && armor_id > ARM_PSI_DEFENSE) {
        return 1;
    }
    int defense_rating = Armor[armor_id].defense_rating;
    if (defense_rating < 0) {
        return psi_factor((Rules->psi_combat_ratio_def[TRIAD_LAND]
            * PlayersData[faction_id].enemy_best_weapon_value) / Rules->psi_combat_ratio_atk[TRIAD_LAND],
            faction_id, false, false);
    }
    return defense_rating;
}

/*
Purpose: Calculate the weapon strategy for the specified weapon id.
Original Offset: 0057D2E0
Return Value: Weapon strategy
Status: Complete
*/
int __cdecl weap_strat(uint32_t weapon_id, uint32_t faction_id) {
    if (!ExpansionEnabled && (weapon_id == WPN_RESONANCE_LASER || weapon_id == WPN_RESONANCE_BOLT
        || weapon_id == WPN_STRING_DISRUPTOR))
        return 1;
    int offense_rating = Weapon[weapon_id].offense_rating;
    if (offense_rating < 0) {
        return psi_factor((Rules->psi_combat_ratio_atk[TRIAD_LAND]
            * PlayersData[faction_id].enemy_best_armor_value) / Rules->psi_combat_ratio_def[TRIAD_LAND],
            faction_id, true, false);
    }
    return offense_rating;
}

/*
Purpose: Calculate the weapon value for the specified prototype.
Original Offset: 0057D360
Return Value: Weapon value
Status: Complete
*/
int __cdecl weap_val(uint32_t proto_id, uint32_t faction_id) {
    return weap_strat(VehPrototypes[proto_id].weapon_id, faction_id);
}

/*
Purpose: Calculate the armor value for the specified armor id.
Original Offset: 0057D3F0
Return Value: Armor value
Status: Complete
*/
int __cdecl arm_val(uint32_t armor_id, int faction_id) {
    return ((faction_id >= 0) ? arm_strat(armor_id, faction_id) : Armor[armor_id].defense_rating)
        * 2;
}

/*
Purpose: Calculate the armor value for the specified prototype.
Original Offset: 0057D480
Return Value: Armor value
Status: Complete
*/
int __cdecl armor_val(uint32_t proto_id, int faction_id) {
    return arm_val(VehPrototypes[proto_id].armor_id, faction_id);
}

/*
Purpose: Calculate the carrying/transport capacity for the specified chassis, abilities and reactor.
Original Offset: 0057D510
Return Value: Transport capacity
Status: Complete
*/
uint32_t __cdecl transport_val(uint32_t chassis_id, uint32_t ability, uint32_t reactor_id) {
    uint32_t transport = Chassis[chassis_id].cargo;
    if (Chassis[chassis_id].triad == TRIAD_SEA) {
        transport *= reactor_id;
    }
    if (ability & ABL_SLOW) {
        transport /= 2; // -50%, rounded down
    }
    if (ability & ABL_HEAVY_TRANSPORT) {
        transport = (3 * transport + 1) / 2; // +50%, rounded up
    }
    return transport;
}

/*
Purpose: Generate offensive display string for the specified prototype. Replaced existing non-safe
         strcat with string. Original function took a 2nd parameter with char buffer and didn't
         return anything.
Original Offset: 0057D560
Return Value: Offensive display string
Status: Complete
*/
std::string __cdecl say_offense(uint32_t proto_id) {
    std::string output = std::to_string(offense_proto(proto_id, -1, 0));
    if (has_abil(proto_id, ABL_DROP_POD)) {
        output += "^";
    }
    if (has_abil(proto_id, ABL_AMPHIBIOUS)) {
        output += "~";
    }
    if (has_abil(proto_id, ABL_NERVE_GAS)) {
        output += "x";
    }
    if (has_abil(proto_id, ABL_EMPATHIC)) {
        output += "e";
    }
    if (has_abil(proto_id, ABL_BLINK_DISPLACER)) {
        output += "!";
    }
    if (can_arty(proto_id, true)) {
        output.insert(output.begin(), '(');
        output += ")";
    }
    if (has_abil(proto_id, ABL_AIR_SUPERIORITY)) {
        output.insert(output.begin(), '<');
        output += ">";
    }
    uint32_t weapon_id = VehPrototypes[proto_id].weapon_id;
    if (weapon_id == WPN_RESONANCE_LASER || weapon_id == WPN_RESONANCE_BOLT) {
        output += "r";
    }
    return output;
}

/*
Purpose: Generate defense display string for the specified prototype. Replaced existing non-safe 
         strcat with string. Original function took a 2nd parameter with char buffer and didn't 
         return anything.
Original Offset: 0057D6D0
Return Value: Defense display string
Status: Complete
*/
std::string __cdecl say_defense(uint32_t proto_id) {
    std::string output = std::to_string(armor_proto(proto_id, -1, 0));
    if (has_abil(proto_id, ABL_COMM_JAMMER)) {
        output += "+";
    }
    if (has_abil(proto_id, ABL_TRANCE)) {
        output += "t";
    }
    if (has_abil(proto_id, ABL_AAA)) {
        output.insert(output.begin(), '<');
        output += ">";
    }
    uint32_t armor_id = VehPrototypes[proto_id].armor_id;
    if (armor_id == ARM_PULSE_3_ARMOR || armor_id == ARM_PULSE_8_ARMOR) {
        output += "p";
    } else if (armor_id == ARM_RESONANCE_3_ARMOR || armor_id == ARM_RESONANCE_8_ARMOR) {
        output += "r";
    }
    return output;
}

/*
Purpose: Generate stats string for specified prototype. Replaced existing non-safe strcat with 
         string. Reworked to integrate with existing C code.
Original Offset: 0057D7D0
Return Value: n/a
Status: Complete
*/
void __cdecl say_stats_3(LPSTR stat, uint32_t proto_id) {
    std::string output;
    int off_rating = Weapon[VehPrototypes[proto_id].weapon_id].offense_rating;
    if (off_rating >= 0) {
        output = (off_rating < 99) ? say_offense(proto_id) : "*";
    } else {
        output = "?"; // PSI
    }
    output += "-";
    output += (Armor[VehPrototypes[proto_id].armor_id].defense_rating) >= 0 
        ? say_defense(proto_id) : "?";
    output += "-";
    output += std::to_string(speed_proto(proto_id) / Rules->move_rate_roads);
    uint8_t reactor = VehPrototypes[proto_id].reactor_id;
    if (reactor > RECT_FISSION) {
        output += "*";
        output += std::to_string(reactor);
    }
    // assumes at least 256 char buffer, eventually remove
    // all calls but one use stringTemp (1032 buffer) > ProdPicker::calculate uses local 256 buffer
    // TODO: Eventually remove LPSTR stat param and return std::string instead
    strcat_s(stat, 256, output.c_str());
}

/*
Purpose: Generate prototype stats using stringTemp buffer.
Original Offset: 0050B9A0
Return Value: n/a
Status: Complete
*/
void __cdecl say_stats_3(uint32_t proto_id) {
    say_stats_3(stringTemp->str, proto_id);
}

/*
Purpose: Generate stats string for specified prototype. List whether prototype is psi, sea or air.
         Replaced existing non-safe strcat with string. Reworked to integrate with existing C code.
Original Offset: 0057D8E0
Return Value: n/a
Status: Complete
*/
void __cdecl say_stats_2(LPSTR stat, uint32_t proto_id) {
    std::string output;
    int off_rating = Weapon[VehPrototypes[proto_id].weapon_id].offense_rating;
    if (off_rating >= 0) {
        output = (off_rating < 99) ? say_offense(proto_id) : "*";
    } else {
        output = label_get(196); // 'Psi'
    }
    output += "-";
    output += (Armor[VehPrototypes[proto_id].armor_id].defense_rating) >= 0 
        ? say_defense(proto_id) : label_get(196); // 'Psi'
    output += "-";
    output += std::to_string(speed_proto(proto_id) / Rules->move_rate_roads);
    uint32_t triad = get_proto_triad(proto_id);
    if (triad == TRIAD_SEA) {
        output += " ";
        output += label_get(163); // 'Sea'
    } else if (triad == TRIAD_AIR) {
        output += " ";
        output += label_get(162); // 'Air'
    }
    uint32_t reactor = VehPrototypes[proto_id].reactor_id;
    if (reactor > 1) {
        output += "*";
        output += std::to_string(reactor);
    }
    // assumes at least 1032 char buffer (stringTemp), eventually remove
    // TODO: Eventually remove LPSTR stat param and return std::string instead
    strcat_s(stat, 1032, output.c_str());
}

/*
Purpose: Generate verbose stats string for specified prototype. Used by Design Workshop and Military 
         Command Nexus. Replaced existing non-safe strcat with string. Reworked to integrate with 
         existing C code.
Original Offset: 0057DAA0
Return Value: n/a
Status: Complete
*/
void __cdecl say_stats(LPSTR stat, uint32_t proto_id, LPSTR custom_spacer) {
    std::string output;
    uint8_t plan = VehPrototypes[proto_id].plan;
    uint8_t chas = VehPrototypes[proto_id].chassis_id;
    uint8_t triad = Chassis[chas].triad;
    uint8_t mode = Weapon[VehPrototypes[proto_id].weapon_id].mode;
    int off_rating = Weapon[VehPrototypes[proto_id].weapon_id].offense_rating;
    int def_rating = Armor[VehPrototypes[proto_id].armor_id].defense_rating;
    if (plan == PLAN_RECONNAISANCE && triad == TRIAD_LAND && off_rating == 1 && def_rating == 1
        && !VehPrototypes[proto_id].ability_flags) {
        output = StringTable->get((int)PlansFullName[3]); // 'Explore/Defense'
        output += ", ";
    } else if (mode < 3) { // Projectile, energy, missile
        output = (plan != PLAN_DEFENSIVE || (off_rating >= 0 && off_rating <= def_rating))
            ? StringTable->get((int)PlansShortName[plan]) : label_get(312); // 'Combat'
        output += ", ";
    }
    if (off_rating < 0 || mode < 3) {
        output += (off_rating < 0) ? label_get(196) : say_offense(proto_id); // 'Psi'
        output += custom_spacer ? custom_spacer : "/";
        output += (def_rating < 0) ? label_get(196) : say_defense(proto_id); // 'Psi'
        output += custom_spacer ? custom_spacer : "/";
        output += std::to_string(speed_proto(proto_id) / Rules->move_rate_roads);
    } else if (def_rating != 1 || VehPrototypes[proto_id].ability_flags || (Chassis[chas].speed != 1
        && (mode != WPN_MODE_TRANSPORT || chas != CHSI_FOIL))) {
        output += StringTable->get(int(PlansShortName[mode]));
        if (plan == PLAN_NAVAL_TRANSPORT) {
            output += "(";
            output += std::to_string(VehPrototypes[proto_id].carry_capacity);
            output += ")";
        }
        output += ", ";
        output += (def_rating < 0) ? label_get(196) : say_defense(proto_id); // 'Psi'
        output += custom_spacer ? custom_spacer : "/";
        output += std::to_string(speed_proto(proto_id) / Rules->move_rate_roads);
    } else {
        output += StringTable->get((int)PlansFullName[mode]);
        if (plan == PLAN_NAVAL_TRANSPORT) {
            output += "(";
            output += std::to_string(VehPrototypes[proto_id].carry_capacity);
            output += ")";
        }
        if (triad) { // sea, air
            output += ","; // Bug fix: removed extra space
        }
    }
    if (triad == TRIAD_SEA) {
        output += " ";
        output += label_get(163); // 'Sea'
    } else if (triad == TRIAD_AIR) {
        output += " ";
        output += label_get(162); // 'Air'
    }
    uint32_t reactor = VehPrototypes[proto_id].reactor_id;
    if (reactor > 1) {
        output += " (*";
        output += std::to_string(reactor);
        output += ")";
    }
    // assumes at least 1032 char buffer (stringTemp), eventually remove
    // TODO: Eventually remove LPSTR stat param and return std::string instead
    strcat_s(stat, 1032, output.c_str());
}

/*
Purpose: Check against faction's available tech for best available reactor.
Original Offset: 0057EFA0
Return Value: Best reactor available (1-4)
Status: Complete
*/
uint32_t __cdecl best_reactor(int faction_id) {
    for (int i = MaxReactorNum - 1; i >= 0; i--) {
        if (has_tech(Reactor[i].preq_tech, faction_id)) {
            return i + 1;
        }
    }
    return RECT_FISSION; // default
}

/*
Purpose: Check against faction's available tech for best available chassis meeting requirements.
Original Offset: 0057EFF0
Return Value: Best chassis available (0-8)
Status: Complete
*/
int __cdecl pick_chassis(int faction_id, int triad_chk, int speed_chk) {
    int chassis_id = -1;
    int best_speed = 0;
    for (int i = 0; i < MaxChassisNum; i++) {
        if (has_tech(Chassis[i].preq_tech, faction_id) && (triad_chk < 0 
            || triad_chk == Chassis[i].triad) && (!Chassis[i].missile || speed_chk == -1)) {
            int speed_compare = 1;
            if (speed_chk <= 0) { // 0, -1, -2
                if (!speed_chk || triad_chk != TRIAD_AIR || (speed_chk == -1 && Chassis[i].missile)
                    || (speed_chk == -2 && Chassis[i].range == 1)) {
                    speed_compare = Chassis[i].speed + 1;
                    if (speed_compare > best_speed) {
                        best_speed = speed_compare;
                        chassis_id = i;
                    }
                }
            } else if (Chassis[i].speed >= speed_chk) { // 1, 2, 3
                if (Chassis[i].speed == speed_chk) {
                    speed_compare = 2;
                }
                if (speed_compare > best_speed) {
                    best_speed = speed_compare;
                    chassis_id = i;
                }
            }
        }
    }
    return chassis_id;
}

/*
Purpose: Check against faction's available tech for the best available weapon meeting requirements.
         The condition variable has a dual purpose of either max cost or search for 1st instance of
         a particular weapon mode.
Original Offset: 0057F0B0
Return Value: Best weapon available (0-25)
Status: Complete
*/
int __cdecl weapon_budget(int faction_id, int condition, BOOL check_mode) {
    int weapon_id = 0;
    int best_offense = -1;
    for (int i = 0; i < MaxWeaponNum; i++) {
        if (has_tech(Weapon[i].preq_tech, faction_id)) {
            if (condition >= 0) {
                if ((!check_mode || Weapon[i].mode < 2) && Weapon[i].cost <= condition
                    && Weapon[i].mode < 3 && Weapon[i].offense_rating < 99
                    && i != 23) { // Exclude Conventional Payload
                    int offense_compare = Weapon[i].offense_rating * 2;
                    if (offense_compare >= best_offense) {
                        best_offense = offense_compare;
                        weapon_id = i;
                    }
                }
            } else if (Weapon[i].mode == -condition) {
                weapon_id = i;
                break;
            }
        }
    }
    return weapon_id;
}

/*
Purpose: Check against the faction's available tech for the best available armor meeting the cost 
         requirement.
Original Offset: 0057F150
Return Value: Best armor available (0-13)
Status: Complete
*/
int __cdecl armor_budget(int faction_id, int max_cost) {
    int armor_id = 0;
    int best_defense = -1;
    for (int i = 0; i < MaxArmorNum; i++) {
        if (has_tech(Armor[i].preq_tech, faction_id) && Armor[i].cost <= max_cost
            && Armor[i].defense_rating >= 0) { // excludes PSI
            int defense_compare = Armor[i].defense_rating;
            if (faction_id >= 0 && !*ExpansionEnabled && i > 9) {
                defense_compare = 1; // Pulse + Resonance in non-expansion mode
            }                       // This really only affects Pulse 8 Armor (Super Tensile Solids)
            defense_compare *= 2;
            if (defense_compare >= best_defense) {
                armor_id = i;
                best_defense = defense_compare;
            }
        }
    }
    return armor_id;
}

/*
Purpose: Get the index value of a particular ability's bitfield.
Original Offset: 00581170
Return Value: Ability index
Status: Complete
*/
int __cdecl abil_index(int ability_id) {
    int index = 0;
    for (int check = ability_id; !(check & 1); index++) {
        check >>= 1;
    }
    return index;
}

/*
Purpose: Calculate the movement penalty/cost.
Original Offset: 00593510
Return Value: Movement cost
Status: Complete
*/
int __cdecl hex_cost(int proto_id, int faction_id, uint32_t x_src, uint32_t y_src, uint32_t x_dst,
                     uint32_t y_dst, BOOL toggle) {
    uint32_t bit_dst = bit_at(x_dst, y_dst);
    if (is_ocean(x_dst, y_dst)) {
        if (bit_dst & BIT_FUNGUS && altitude_at(x_dst, y_dst) == ALT_BIT_OCEAN_SHELF
            && get_proto_triad(proto_id) == TRIAD_SEA
            && proto_id != BSC_SEALURK // Bug fix
            && proto_id != BSC_ISLE_OF_THE_DEEP && !has_project(SP_XENOEMPATYH_DOME, faction_id)) {
            return Rules->move_rate_roads * 3;
        }
        return Rules->move_rate_roads;
    }
    if (is_ocean(x_src, y_src)) {
        return Rules->move_rate_roads;
    }
    if (proto_id >= 0 && get_proto_triad(proto_id) != TRIAD_LAND) {
        return Rules->move_rate_roads;
    }
    // Land only conditions
    uint32_t bit_src = bit_at(x_src, y_src);
    if (bit_src & (BIT_MAGTUBE | BIT_BASE_IN_TILE) && bit_dst & (BIT_MAGTUBE | BIT_BASE_IN_TILE)
        && faction_id) {
        return 0;
    }
    if ((bit_src & (BIT_ROAD | BIT_BASE_IN_TILE) || bit_src & BIT_FUNGUS && faction_id > 0
        && has_project(SP_XENOEMPATYH_DOME, faction_id)) && bit_dst & (BIT_ROAD | BIT_BASE_IN_TILE)
        && faction_id) {
        return 1;
    }
    if (faction_id >= 0 && (has_project(SP_XENOEMPATYH_DOME, faction_id) || !faction_id
        || proto_id == BSC_MIND_WORMS || proto_id == BSC_SPORE_LAUNCHER) && bit_dst & BIT_FUNGUS) {
        return 1;
    }
    if (bit_src & BIT_RIVER && bit_dst & BIT_RIVER && x_dist(x_src, x_dst) == 1
        && abs((int)y_src - (int)y_dst) == 1 && faction_id) {
        return 1;
    }
    if (VehPrototypes[proto_id].chassis_id == CHSI_HOVERTANK
        || has_abil(proto_id, ABL_ANTIGRAV_STRUTS)) {
        return Rules->move_rate_roads;
    }
    uint32_t cost = Rules->move_rate_roads;
    if (rocky_at(x_dst, y_dst) > TERRAIN_BIT_ROLLING && !toggle) {
        cost += Rules->move_rate_roads;
    }
    if (bit_dst & BIT_FOREST && !toggle) {
        cost += Rules->move_rate_roads;
    }
    if (faction_id && bit_dst & BIT_FUNGUS && (proto_id >= MaxVehProtoFactionNum
        || Weapon[VehPrototypes[proto_id].weapon_id].offense_rating >= 0)) {
        uint8_t plan = VehPrototypes[proto_id].plan;
        if (plan != PLAN_TERRAFORMING && plan != PLAN_ALIEN_ARTIFACT
            && PlayersData[faction_id].soc_effect_active.planet <= 0) {
            return cost + Rules->move_rate_roads * 2;
        }
        uint32_t speed = speed_proto(proto_id);
        if (cost <= speed) {
            return speed;
        }
    }
    return cost;
}

/*
Purpose: Relocate an existing unit to the specified tile.
Original Offset: 005A59B0
Return Value: n/a
Status: Complete
*/
void __cdecl veh_put(uint32_t veh_id, int x, int y) {
    veh_drop(veh_lift(veh_id), x, y);
}

/*
Purpose: Get the current health of the specified unit factoring in damage.
Original Offset: 005A59E0
Return Value: Unit health
Status: Complete
*/
uint32_t __cdecl veh_health(uint32_t veh_id) {
    int proto_id = Vehs[veh_id].proto_id;
    int health = VehPrototypes[proto_id].plan != PLAN_ALIEN_ARTIFACT
        ? range(VehPrototypes[proto_id].reactor_id, 1, 100) * 10 : 1;
    return range(health - Vehs[veh_id].dmg_incurred, 0, 9999);
}

/*
Purpose: Calculates cost of the prototype based on various factors. Optimized logic flow from 
         the original without any differences to the final calculation.
Original Offset: 005A5A60
Return Value: Cost of prototype
Status: Complete
*/
uint32_t __cdecl proto_cost(uint32_t chassis_id, uint32_t weapon_id, uint32_t armor_id, 
                            uint32_t ability, uint32_t reactor_id) {
    uint8_t weap_cost = Weapon[weapon_id].cost;
    // PB check: moved to start vs after 1st triad checks in original > no difference in logic
    if (Chassis[chassis_id].missile && Weapon[weapon_id].offense_rating >= 99) {
        return weap_cost;
    }
    uint8_t triad = Chassis[chassis_id].triad;
    uint32_t armor_cost = Armor[armor_id].cost;
    uint32_t speed_cost = Chassis[chassis_id].cost;
    int abil_modifier = 0;
    int flags_modifier = 0;
    if (ability) {
        for (int i = 0; i < MaxAbilityNum; i++) {
            if ((1 << i) & ability) {
                if (abil_modifier) {
                    abil_modifier++; // Increased cost with more than one ability
                }
                int factor = Ability[i].cost_factor;
                if (factor > 0) { // 1+ = Straight Cost; 25% increase per unit of cost
                    abil_modifier += factor;
                } else {
                    switch (factor) {
                      case 0: // None
                      default:
                        break;
                        // Increases w/ ratio of weapon to armor: 0, 1, or 2. Rounded DOWN.
                        // Never higher than 2.
                      case -1:
                        // fixed potential crash: will never trigger in vanilla but could with mods
                        if (armor_cost) {
                            abil_modifier += range(weap_cost / armor_cost, 0, 2);
                        }
                        break;
                      case -2: // Increases w/ weapon value
                        abil_modifier += weap_cost - 1;
                        break;
                      case -3: // Increases w/ armor value
                        abil_modifier += armor_cost - 1;
                        break;
                      case -4: // Increases w/ speed value
                        abil_modifier += speed_cost - 1;
                        break;
                      case -5: // Increases w/ weapon+armor value
                        abil_modifier += weap_cost + armor_cost - 2;
                        break;
                      case -6: // Increases w/ weapon+speed value
                        abil_modifier += weap_cost + speed_cost - 2;
                        break;
                      case -7: // Increases w/ armor+speed value
                        abil_modifier += armor_cost + speed_cost - 2;
                        break;
                    }
                }
                // 010000000000 - Cost increased for land units; Deep Radar
                // Shifted flag check into main ability loop rather than its
                // own loop at 1st triad checks
                if (Ability[i].flags & AFLAG_COST_INC_LAND_UNIT && triad == TRIAD_LAND) {
                    // separate variable keeps logic same (two abilities, both with cost 0,
                    // one with cost increase flag will trigger above "if (abil_modifier)" if
                    // this is directly abil_modifier++)
                    flags_modifier++;
                }
            }
        }
        abil_modifier += flags_modifier; // adding here keeps logic consistent after optimization
    }
    if (triad == TRIAD_SEA) {
        armor_cost /= 2;
        speed_cost += reactor_id;
    } else if (triad == TRIAD_AIR) {
        if (armor_cost > 1) {
            armor_cost *= reactor_id * 2;
        }
        speed_cost += reactor_id * 2;
    } // TRIAD_LAND ability flag check moved into ability loop above
    uint32_t combat_mod = armor_cost / 2 + 1;
    if (combat_mod < weap_cost) { // which ever is greater
        combat_mod = weap_cost;
    }
    int proto_mod;
    // shifted this check to top vs at end > no difference in logic
    if (combat_mod == 1 && armor_cost == 1 && speed_cost == 1 && reactor_id == RECT_FISSION) {
        proto_mod = 1;
    } else {
        // (2 << n) == 2^(n + 1) ; (2 << n) / 2 == 2 ^ n;
        // will crash if reactor_id is 0xFF > divide by zero; not putting in error checking
        // since this is unlikely even with modding however noting for future
        proto_mod = ((speed_cost + armor_cost) * combat_mod + ((2 << reactor_id) / 2))
            / (2 << reactor_id);
        if (speed_cost == 1) {
            proto_mod = (proto_mod / 2) + 1;
        }
        if (weap_cost > 1 && Armor[armor_id].cost > 1) {
            proto_mod++;
            // moved inside nested check vs separate triad checks below > no difference in logic
            if (triad == TRIAD_LAND && speed_cost > 1) {
                proto_mod++;
            }
        }
        // excludes sea probes
        if (triad == TRIAD_SEA && Weapon[weapon_id].mode != WPN_MODE_INFOWAR) {
            proto_mod = (proto_mod + 1) / 2;
        } else if (triad == TRIAD_AIR) {
            proto_mod /= (Weapon[weapon_id].mode > WPN_MODE_MISSILE) ? 2 : 4; // Non-combat : Combat
        }
        int reactor_mod = (reactor_id * 3 + 1) / 2;
        if (proto_mod < reactor_mod) { // which ever is greater
            proto_mod = reactor_mod;
        }
    }
    return (proto_mod * (abil_modifier + 4) + 2) / 4;
}

/*
Purpose: Calculates the base cost of the specified prototype.
Original Offset: 005A5D00
Return Value: Base cost of the prototype
Status: Complete
*/
uint32_t __cdecl base_cost(uint32_t proto_id) {
    return proto_cost(VehPrototypes[proto_id].chassis_id, VehPrototypes[proto_id].weapon_id,
        VehPrototypes[proto_id].armor_id, 0, VehPrototypes[proto_id].reactor_id);
}

/*
Purpose: Create a new prototype. Sets initial values for everything except veh_name and preq_tech.
Original Offset: 005A5D40
Return Value: n/a
Status: Complete
*/
void __cdecl make_proto(int proto_id, uint32_t chassis_id, uint32_t weapon_id, uint32_t armor_id, 
                        uint32_t ability, uint32_t reactor_id) {
    int unk_local_1 = 0; // TODO: Identify
    if (proto_id >= MaxVehProtoFactionNum) {
        BOOL cond1 = false;
        BOOL cond2 = false;
        BOOL cond3 = false;
        int proto_id_loop;
        for (int i = 0; i < 128; i++) {
            proto_id_loop = i % MaxVehProtoFactionNum;
            if ((i % MaxVehProtoFactionNum) != i) {
                proto_id_loop += proto_id - proto_id % MaxVehProtoFactionNum;
            }
            int flags_check = VehPrototypes[proto_id_loop].flags;
            if (flags_check & PROTO_ACTIVE) {
                if ((proto_id_loop <= MaxVehProtoFactionNum &&
                    has_tech(VehPrototypes[proto_id_loop].preq_tech, 
                        proto_id / MaxVehProtoFactionNum))
                    || (proto_id_loop > MaxVehProtoFactionNum
                        && (flags_check & PROTO_TYPED_COMPLETE))) {
                    uint32_t loop_weapon_id = VehPrototypes[proto_id_loop].weapon_id;
                    if (loop_weapon_id == weapon_id) {
                        cond1 = true;
                    }
                    uint32_t loop_armor_id = VehPrototypes[proto_id_loop].armor_id;
                    if (loop_armor_id == armor_id) {
                        cond2 = true;
                    }
                    uint32_t loop_chassis_id = VehPrototypes[proto_id_loop].chassis_id;
                    if (loop_chassis_id == chassis_id) {
                        cond3 = true;
                    }
                    int off_rating = Weapon[weapon_id].offense_rating;
                    if (off_rating > 0 && Weapon[loop_weapon_id].offense_rating >= off_rating) {
                        cond1 = true;
                    }
                    int def_rating = Armor[armor_id].defense_rating;
                    if (def_rating > 0 && Armor[loop_armor_id].defense_rating >= def_rating) {
                        cond2 = true;
                    }
                    if (Chassis[chassis_id].triad == Chassis[loop_chassis_id].triad
                        && Chassis[loop_chassis_id].speed >= Chassis[chassis_id].speed) {
                        cond3 = true;
                    }
                }
            }
        }
        if (cond1 && cond2 && cond3) {
            unk_local_1 = (proto_id_loop >= MaxVehProtoFactionNum
                && VehPrototypes[proto_id_loop].flags & 0x10) ? 3 : 1;
        }
    }
    VehPrototypes[proto_id].chassis_id = (uint8_t)chassis_id;
    VehPrototypes[proto_id].weapon_id = (uint8_t)weapon_id;
    VehPrototypes[proto_id].armor_id = (uint8_t)armor_id;
    VehPrototypes[proto_id].ability_flags = ability;
    VehPrototypes[proto_id].reactor_id = (uint8_t)reactor_id;
    VehPrototypes[proto_id].cost = (uint8_t)proto_cost(chassis_id, weapon_id, armor_id, ability,
        reactor_id);
    VehPrototypes[proto_id].carry_capacity = (Weapon[weapon_id].mode == WPN_MODE_TRANSPORT) 
        ? (uint8_t)transport_val(chassis_id, ability, reactor_id) : 0;
    // set plan & unk1
    if (Chassis[chassis_id].missile) {
        if (Weapon[weapon_id].offense_rating < 99) { // non-PB missiles
            if (weapon_id == WPN_TECTONIC_PAYLOAD) {
                VehPrototypes[proto_id].plan = PLAN_TECTONIC_MISSILE;
                VehPrototypes[proto_id].unk_1 = 22;
            } else if (weapon_id == WPN_FUNGAL_PAYLOAD) {
                VehPrototypes[proto_id].plan = PLAN_FUNGAL_MISSILE;
                VehPrototypes[proto_id].unk_1 = 23;
            } else { // Conventional Payload
                VehPrototypes[proto_id].plan = PLAN_OFFENSIVE;
                VehPrototypes[proto_id].unk_1 = 13;
            }
        } else {
            VehPrototypes[proto_id].plan = PLAN_PLANET_BUSTER;
            VehPrototypes[proto_id].unk_1 = 14;
        }
    } else if (Weapon[weapon_id].mode >= WPN_MODE_TRANSPORT) { // Non-combat
        VehPrototypes[proto_id].plan = Weapon[weapon_id].mode;
        VehPrototypes[proto_id].unk_1 = Weapon[weapon_id].mode + 0x20;
    } else if (Chassis[chassis_id].triad == TRIAD_SEA) { // combat sea
        VehPrototypes[proto_id].plan = PLAN_NAVAL_SUPERIORITY;
        VehPrototypes[proto_id].unk_1 = 6; // same value as plan
    } else if (Chassis[chassis_id].triad == TRIAD_AIR) { // combat air
        if (has_abil(proto_id, ABL_AIR_SUPERIORITY)) {
            VehPrototypes[proto_id].plan = PLAN_AIR_SUPERIORITY;
            VehPrototypes[proto_id].unk_1 = 9;
        } else {
            VehPrototypes[proto_id].plan = PLAN_OFFENSIVE;
            VehPrototypes[proto_id].unk_1 = 8;
        }
    } else { // TRIAD_LAND combat unit
        VehPrototypes[proto_id].plan = PLAN_OFFENSIVE;
        if (Armor[armor_id].defense_rating > 1) {
            VehPrototypes[proto_id].plan = PLAN_DEFENSIVE;
        }
        if (Weapon[weapon_id].offense_rating >= Armor[armor_id].defense_rating
            && VehPrototypes[proto_id].plan == PLAN_DEFENSIVE) {
            VehPrototypes[proto_id].plan = PLAN_COMBAT;
        }
        if (Chassis[chassis_id].speed > 1
            && Weapon[weapon_id].offense_rating > Armor[armor_id].defense_rating) {
            VehPrototypes[proto_id].plan = PLAN_OFFENSIVE;
        }
        if (ability & (ABL_ARTILLERY | ABL_DROP_POD | ABL_AMPHIBIOUS)) {
            VehPrototypes[proto_id].plan = PLAN_OFFENSIVE;
        }
        VehPrototypes[proto_id].unk_1 = 3;
        if (Armor[armor_id].defense_rating > Weapon[weapon_id].offense_rating) {
            VehPrototypes[proto_id].unk_1 = 2;
        }
        if (Weapon[weapon_id].offense_rating > Armor[armor_id].defense_rating * 2) {
            VehPrototypes[proto_id].unk_1 = 1;
        }
        // changed from '>= 2' since '>= 3' will always overwrite it
        if (Chassis[chassis_id].speed == 2) {
            VehPrototypes[proto_id].unk_1 = 4;
        } else if (Chassis[chassis_id].speed >= 3) { // added else, if speed <= 2 would always skip
            VehPrototypes[proto_id].unk_1 = 5;
        }
        if (Weapon[weapon_id].offense_rating == 1 && Armor[armor_id].defense_rating == 1) {
            if (Chassis[chassis_id].speed > 1) {
                VehPrototypes[proto_id].plan = PLAN_RECONNAISANCE;
            }
            if (has_abil(proto_id, ABL_POLICE_2X)) {
                VehPrototypes[proto_id].plan = PLAN_DEFENSIVE;
            }
            VehPrototypes[proto_id].unk_1 = 10;
        }
    }
    // set final values
    VehPrototypes[proto_id].obsolete_factions = 0;
    VehPrototypes[proto_id].combat_factions = (proto_id >= MaxVehProtoFactionNum) ?
        1 << (proto_id / MaxVehProtoFactionNum) : -1;
    VehPrototypes[proto_id].flags = unk_local_1 
        ? ((unk_local_1 & 2) ? 0x115 : 0x105) : PROTO_ACTIVE;
    VehPrototypes[proto_id].icon_offset = -1;
}

/*
Purpose: Search for the 1st prototype (basic + faction specific) that matches the specified plan.
Original Offset: 005AED50
Return Value: Prototype id or -1 if not found
Status: Complete
*/
int __cdecl get_plan(uint32_t faction_id, uint32_t plan) {
    for (int i = 0; i < 128; i++) {
        uint32_t proto_id = (i < MaxVehProtoFactionNum) ? i
            : (faction_id * MaxVehProtoFactionNum) + i - MaxVehProtoFactionNum;
        if (VehPrototypes[proto_id].flags & PROTO_ACTIVE
            && !(VehPrototypes[proto_id].obsolete_factions & (1 << faction_id))
            && (proto_id >= MaxVehProtoFactionNum
                || has_tech(VehPrototypes[proto_id].preq_tech, faction_id))
            && VehPrototypes[proto_id].plan == plan) {
            return proto_id;
        }
    }
    return -1;
}

/*
Purpose: Determine if the specified unit wants to wake up based on certain conditions and 
         preferences. Optional parameter for spotted veh_id (-1 to skip).
Original Offset: 005B5EA0
Return Value: Does unit want to wake? true/false
Status: Complete
*/
BOOL __cdecl want_to_wake(uint32_t faction_id, uint32_t veh_id, int spotted_veh_id) {
    int base_faction_id;
    uint32_t triad = get_triad(veh_id);
    uint32_t veh_faction_id = Vehs[veh_id].faction_id;
    if (triad == TRIAD_AIR && Vehs[veh_id].terraforming_turns 
        && Vehs[veh_id].order == ORDER_MOVE_TO
        && (base_faction_id = base_who(Vehs[veh_id].waypoint_x[0], Vehs[veh_id].waypoint_y[0]),
            base_faction_id >= 0)) {
        if (((uint32_t)base_faction_id == veh_faction_id) 
            || has_treaty(veh_faction_id, base_faction_id, DTREATY_PACT)) {
            return false;
        }
    }
    BOOL wants_to_wake;
    if (*IsMultiplayerNet) { // restructured to be more efficient with same logic
        if (has_treaty(veh_faction_id, faction_id, DTREATY_PACT)) {
            wants_to_wake = false;
        } else if (has_treaty(veh_faction_id, faction_id, DTREATY_TREATY)) {
            wants_to_wake = true;
        } else if (has_treaty(veh_faction_id, faction_id, DTREATY_TRUCE)) {
            wants_to_wake = true;
        } else {
            wants_to_wake = true;
        }
    } else {
        if (has_treaty(veh_faction_id, faction_id, DTREATY_PACT)) {
            wants_to_wake = *GamePreferences & PREF_AUTO_END_MOVE_SPOT_VEH_PACT;
        } else if (has_treaty(veh_faction_id, faction_id, DTREATY_TREATY)) {
            wants_to_wake = *GamePreferences & PREF_AUTO_END_MOVE_SPOT_VEH_TREATY;
        } else if (has_treaty(veh_faction_id, faction_id, DTREATY_TRUCE)) {
            wants_to_wake = *GamePreferences & PREF_AUTO_END_MOVE_SPOT_VEH_TRUCE;
        } else {
            wants_to_wake = *GamePreferences & PREF_AUTO_END_MOVE_SPOT_VEH_WAR;
        }
        if (!(*GamePreferences & PREF_AUTO_DONT_END_MOVE_DIFF_TRIAD)) {
            return wants_to_wake;
        }
    }
    if (spotted_veh_id >= 0) { // removed a check if veh_id is also >= 0 (always unsigned)
        uint32_t triad_spotted = get_triad(spotted_veh_id);
        if (triad != triad_spotted && ((Vehs[veh_id].state & (VSTATE_UNK_1000000 | VSTATE_UNK_200))
            != (VSTATE_UNK_1000000 | VSTATE_UNK_200) || triad_spotted == TRIAD_LAND)) {
            wants_to_wake = false;
        }
    }
    return wants_to_wake;
}

/*
Purpose: Wake up units meeting specific conditions from the stack where specified unit is located.
Original Offset: 005B6060
Return Value: n/a
Status: Complete
*/
void __cdecl wake_stack(int veh_id) {
    for (int i = veh_top(veh_id); i >= 0; i = Vehs[i].next_veh_id_stack) {
        int veh_id_way_point;
        int x;
        int y;
        if (Vehs[i].order == ORDER_SENTRY_BOARD 
            && (get_triad(i) || (x = Vehs[i].x, y = Vehs[i].y, !is_ocean(x, y)
            && ((veh_id_way_point = Vehs[i].waypoint_x[0], veh_id_way_point < 0)
                || get_triad(veh_id_way_point) != TRIAD_AIR || base_who(x, y) >= 0
                || bit_at(x, y) & BIT_AIRBASE)))) {
            Vehs[i].order = ORDER_NONE;
            Vehs[i].state &= ~VSTATE_EXPLORE;
        }
    }
}

/*
Purpose: Move a stack of units in the same stack as the specified unit to a destination.
Original Offset: 005B8AF0
Return Value: n/a
Status: Complete
*/
void __cdecl stack_put(int veh_id, int x, int y) {
    int next_veh_id = veh_top(veh_id);
    int veh_id_loop;
    if (next_veh_id >= 0) {
        do {
            veh_id_loop = Vehs[next_veh_id].next_veh_id_stack;
            veh_put(next_veh_id, x, y);
            next_veh_id = veh_id_loop;
        } while (veh_id_loop >= 0);
    }
}

/*
Purpose: Sort a stack of units with the transports moved to the top.
Original Offset: 005B8B60
Return Value: n/a
Status: Complete
*/
void __cdecl stack_sort(uint32_t veh_id) {
    int16_t x = Vehs[veh_id].x;
    int16_t y = Vehs[veh_id].y;
    int next_veh_id = veh_top(veh_id);
    int veh_id_put = -1;
    int veh_id_loop;
    if (next_veh_id >= 0) {
        do {
            veh_id_loop = Vehs[next_veh_id].next_veh_id_stack; // removed redundant < 0 check
            if (veh_cargo(next_veh_id) || has_abil(Vehs[next_veh_id].proto_id, ABL_CARRIER)) {
                veh_id_put = next_veh_id;
                veh_put(next_veh_id, -3, -3);
            }
            next_veh_id = veh_id_loop;
        } while (veh_id_loop >= 0);
        stack_put(veh_id_put, x, y);
    }
}

/*
Purpose: Sort a stack of units with colony pods at the top followed by combat/offensive units.
Original Offset: 005B8C90
Return Value: n/a
Status: Complete
*/
void __cdecl stack_sort_2(uint32_t veh_id) {
    int16_t x = Vehs[veh_id].x;
    int16_t y = Vehs[veh_id].y;
    int next_veh_id = veh_top(veh_id);
    int veh_id_put = -1;
    int veh_id_loop;
    if (next_veh_id >= 0) {
        do {
            veh_id_loop = Vehs[next_veh_id].next_veh_id_stack; // removed redundant < 0 check
            if (VehPrototypes[Vehs[next_veh_id].proto_id].plan == PLAN_COLONIZATION) { // Colony Pod
                veh_id_put = next_veh_id;
                veh_put(next_veh_id, -3, -3);
            }
            next_veh_id = veh_id_loop;
        } while (veh_id_loop >= 0);
    }
    next_veh_id = veh_at(x, y);
    if (next_veh_id >= 0) {
        do {
            veh_id_loop = Vehs[next_veh_id].next_veh_id_stack; // removed redundant < 0 check
            if (VehPrototypes[Vehs[next_veh_id].proto_id].plan <= PLAN_COMBAT) {
                veh_id_put = next_veh_id;
                veh_put(next_veh_id, -3, -3);
            }
            next_veh_id = veh_id_loop;
        } while (veh_id_loop >= 0);
    }
    stack_put(veh_id_put, x, y);
}

/*
Purpose: Refresh and fix the stack. Used by DirectPlay multiplayer only.
Original Offset: 005B8E10
Return Value: Either the parameter unit id or unit id of the stack top; Return is checked if >= 0
Status: Complete
*/
int __cdecl stack_fix(int veh_id) {
    if (veh_id < 0 || !*IsMultiplayerNet
        || (Vehs[veh_id].next_veh_id_stack < 0 && Vehs[veh_id].prev_veh_id_stack < 0)) {
        return veh_id; // invalid veh_id, not DirectPlay MP or no stack
    }
    for (int i = 0; i < *VehCurrentCount; i++) {
        // Bug fix: Original would compare against the exact same source coordinates (both veh_id
        // instead of one being iterator) likely causing a performance hit.
        if (Vehs[veh_id].x == Vehs[i].x && Vehs[veh_id].y == Vehs[i].y) {
            veh_promote(i);
            stack_sort(veh_id);
        }
    }
    return veh_top(veh_id);
}

/*
Purpose: Various unit stack related calculations based on type parameter (0-19) and conditions.
Original Offset: 005B9580
Return Value: Dependent on type parameter
Status: Complete
*/
int __cdecl stack_check(int veh_id, uint32_t type, int cond1, int cond2, int cond3) {
    int retn_val = 0;
    uint32_t plan;
    uint32_t chas;
    for (int i = veh_top(veh_id); i >= 0; i = Vehs[i].next_veh_id_stack) {
        switch (type) {
          case 0:
            if ((cond2 < 0 || Vehs[i].faction_id == cond2) && Vehs[i].proto_id == cond1) {
                retn_val++;
            }
            break;
          case 1:
            if (cond1 < 0 || Vehs[i].faction_id == cond1) {
                retn_val++;
            }
            break;
          case 2:
            if ((cond2 < 0 || Vehs[i].faction_id == cond2) 
                && VehPrototypes[Vehs[i].proto_id].plan == cond1) {
                retn_val++;
            }
            break;
          case 3:
            if ((cond2 < 0 || Vehs[i].faction_id == cond2) && get_triad(i) == cond1) {
                retn_val++;
            }
            break;
          case 4:
            if (cond1 < 0 || Vehs[i].faction_id == cond1) {
                retn_val += Weapon[VehPrototypes[Vehs[i].proto_id].weapon_id].offense_rating;
            }
            break;
          case 5:
            if (cond1 < 0 || Vehs[i].faction_id == cond1) {
                retn_val += Armor[VehPrototypes[Vehs[i].proto_id].armor_id].defense_rating;
            }
            break;
          case 6:
            if ((cond2 < 0 || Vehs[i].faction_id == cond2) && has_abil(Vehs[i].proto_id, cond1)) {
                retn_val++;
            }
            break;
          case 7:
            if (cond1 < 0 || Vehs[i].faction_id == cond1) {
                retn_val += VehPrototypes[Vehs[i].proto_id].cost;
            }
            break;
          case 8:
            if (cond1 < 0 || Vehs[i].faction_id == cond1) {
                uint32_t triad = get_triad(i);
                if (triad == TRIAD_LAND) {
                    retn_val--;
                } else if (triad == TRIAD_SEA) {
                    retn_val += veh_cargo(i);
                }
            }
            break;
          case 9:
            if ((cond2 < 0 || Vehs[i].faction_id == cond2) && Vehs[i].order == cond1) {
                retn_val++;
            }
            break;
          case 10:
            if (Vehs[i].faction_id == cond1) {
                retn_val++;
            }
            break;
          case 11:
            if ((cond3 < 0 || Vehs[i].faction_id == cond3)
                && (Vehs[i].state & cond1) == (uint32_t)cond2) {
                retn_val++;
            }
            break;
          case 12:
            if (cond1 < 0 || Vehs[i].faction_id == cond1) {
                uint32_t faction_id = Vehs[i].faction_id;
                int proto_id = Vehs[i].proto_id;
                if (arm_strat(VehPrototypes[proto_id].armor_id, faction_id) 
                    > weap_strat(VehPrototypes[proto_id].weapon_id, faction_id)) {
                    retn_val++;
                }
            }
            break;
          case 13:
            if ((cond1 < 0 || Vehs[i].faction_id == cond1)
                && Chassis[VehPrototypes[Vehs[i].proto_id].chassis_id].missile) {
                retn_val++;
            }
            break;
          case 14:
            if ((cond1 < 0 || Vehs[i].faction_id == cond1)
                && (plan = VehPrototypes[Vehs[i].proto_id].plan,
                    plan == PLAN_DEFENSIVE || plan == PLAN_COMBAT)) {
                retn_val++;
            }
            break;
          case 15:
            if ((cond1 < 0 || Vehs[i].faction_id == cond1) && can_arty(Vehs[i].proto_id, TRUE)) {
                retn_val++;
            }
            break;
          case 16:
            if ((cond1 < 0 || Vehs[i].faction_id == cond1)
                && (plan = VehPrototypes[Vehs[i].proto_id].plan, plan == PLAN_DEFENSIVE
                    || plan == PLAN_RECONNAISANCE || plan == PLAN_COMBAT)) {
                retn_val++;
            }
            break;
          case 17:
            if ((cond2 < 0 || Vehs[i].faction_id == cond2)
                && VehPrototypes[Vehs[i].proto_id].unk_1 == cond1) {
                retn_val++;
            }
            break;
          case 18:
            if ((cond1 < 0 || Vehs[i].faction_id == cond1)
                && (chas = VehPrototypes[Vehs[i].proto_id].chassis_id,
                    Chassis[chas].triad == TRIAD_AIR && Chassis[chas].range > 1)) {
                retn_val++;
            }
            break;
          case 19:
            if ((cond1 < 0 || Vehs[i].faction_id == cond1)
                && !Weapon[VehPrototypes[Vehs[i].proto_id].weapon_id].offense_rating) {
                retn_val++;
            }
            break;
          default:
            break;
        }
    }
    return retn_val;
}

/*
Purpose: Check to see whether provided faction and base can build a specific prototype. Checks are
         included to prevent SMACX specific units from being built in SMAC mode.
Original Offset: 005BA910
Return Value: Is unit available to faction/base? true/false
Status: Complete
*/
BOOL __cdecl veh_avail(uint32_t proto_id, uint32_t faction_id, int base_id) {
    if (!(VehPrototypes[proto_id].flags & PROTO_ACTIVE)
        || (VehPrototypes[proto_id].obsolete_factions & (1 << faction_id))) {
        return false;
    }
    if (proto_id < MaxVehProtoFactionNum) {
        if (!has_tech(VehPrototypes[proto_id].preq_tech, faction_id)) {
            return false;
        }
    }
    if (VehPrototypes[proto_id].plan == PLAN_COLONIZATION 
        && *GameRules & RULES_SCN_NO_COLONY_PODS) {
        return false;
    }
    if (base_id >= 0 && get_proto_triad(proto_id) == TRIAD_SEA && !is_port(base_id, false)) {
        return false;
    }
    uint8_t weap_id;
    uint32_t abil_flag;
    if (!*ExpansionEnabled && (VehPrototypes[proto_id].armor_id > ARM_PSI_DEFENSE
        || (weap_id = VehPrototypes[proto_id].weapon_id, weap_id == WPN_RESONANCE_LASER
            || weap_id == WPN_RESONANCE_BOLT || weap_id == WPN_STRING_DISRUPTOR
            || weap_id == WPN_TECTONIC_PAYLOAD || weap_id == WPN_FUNGAL_PAYLOAD)
        || (abil_flag = VehPrototypes[proto_id].ability_flags, abil_flag == ABL_SOPORIFIC_GAS
            || abil_flag == ABL_DISSOCIATIVE_WAVE || abil_flag == ABL_MARINE_DETACHMENT
            || abil_flag == ABL_FUEL_NANOCELLS || abil_flag == ABL_ALGO_ENHANCEMENT)
        || proto_id == BSC_SEALURK || proto_id == BSC_SPORE_LAUNCHER
        || proto_id == BSC_BATTLE_OGRE_MK1 || proto_id == BSC_BATTLE_OGRE_MK2
        || proto_id == BSC_BATTLE_OGRE_MK3 || proto_id == BSC_FUNGAL_TOWER
        || proto_id == BSC_UNITY_MINING_LASER)) {
        return false;
    }
    if (proto_id < MaxVehProtoFactionNum) {
        return true;
    }
    return (proto_id / MaxVehProtoFactionNum) == faction_id;
}

/*
Purpose: Determine whether a faction wants the specified prototype based on the faction's current
         prototype designs.
Original Offset: 005BE100
Return Value: Does faction want prototype? true/false
Status: Complete
*/
BOOL __cdecl wants_prototype(uint32_t proto_id, uint32_t faction_id) {
    uint32_t flags = VehPrototypes[proto_id].flags;
    if (!(flags & PROTO_ACTIVE) || !(flags & PROTO_TYPED_COMPLETE)) {
        return false;
    }
    uint32_t proto_offset = faction_id * MaxVehProtoFactionNum;
    for (uint32_t i = 0; i < MaxVehProtoFactionNum; i++) {
        uint32_t proto_id_cmp = proto_offset + i;
        uint32_t flags_cmp = VehPrototypes[proto_id_cmp].flags;
        if (flags_cmp & PROTO_ACTIVE && flags_cmp & PROTO_TYPED_COMPLETE
            && VehPrototypes[proto_id].plan == VehPrototypes[proto_id_cmp].plan) {
            uint8_t chas = VehPrototypes[proto_id].chassis_id;
            uint8_t chas_cmp = VehPrototypes[proto_id_cmp].chassis_id;
            if (Chassis[chas].triad == Chassis[chas_cmp].triad) {
                uint8_t weap_id_cmp = VehPrototypes[proto_id_cmp].weapon_id;
                uint8_t mode_cmp = Weapon[weap_id_cmp].mode;
                int8_t off_rating_cmp;
                if ((mode_cmp > WPN_MODE_MISSILE)
                    ? (mode_cmp == Weapon[VehPrototypes[proto_id].weapon_id].mode)
                    : (off_rating_cmp = Weapon[weap_id_cmp].offense_rating, off_rating_cmp < 0)
                    ? (off_rating_cmp == Weapon[VehPrototypes[proto_id].weapon_id].offense_rating)
                    : (off_rating_cmp >= Weapon[VehPrototypes[proto_id].weapon_id].offense_rating)) {
                    uint8_t arm_id_cmp = VehPrototypes[proto_id_cmp].armor_id;
                    if ((Armor[arm_id_cmp].defense_rating <= 0)
                        ? (arm_id_cmp == VehPrototypes[proto_id].armor_id)
                        : (armor_val(proto_id_cmp, faction_id) 
                            >= armor_val(proto_id, faction_id))) {
                        if (Chassis[chas_cmp].speed >= Chassis[chas].speed) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

/*
Purpose: Check the coordinates for units and if at least one is found return the top most id.
Original Offset: 005BFE90
Return Value: Unit id or -1 if nothing is found/error
Status: Complete
*/
int __cdecl veh_at(int x, int y) {
    if (on_map(x, y) && !(bit_at(x, y) & BIT_VEH_IN_TILE)) {
        return -1; // not found
    }
    for (int veh_id = 0; veh_id < *VehCurrentCount; veh_id++) {
        if (Vehs[veh_id].x == x && Vehs[veh_id].y == y) {
            return veh_top(veh_id);
        }
    }
    if (!on_map(x, y)) {
        return -1;
    }
    if (!*VehBitError) {
        log_say("Vehicle Bit Error  (x, y)", x, y, 0);
    }
    if (*GameState & STATE_SCENARIO_EDITOR || *GameState & STATE_DEBUG_MODE || *IsMultiplayerNet) {
        if (*VehBitError) {
            return -1;
        }
        *VehBitError = true;
    }
    rebuild_vehicle_bits();
    return -1;
}

/*
Purpose: Check whether the prototype has a specific ability.
Original Offset: 005BF1F0
Return Value: Does prototype have ability? true/false
Status: Complete
*/
BOOL __cdecl has_abil(uint32_t proto_id, uint32_t ability_id) {
    if (VehPrototypes[proto_id].ability_flags & ability_id) {
        return true;
    }
    uint32_t faction_id = proto_id / MaxVehProtoFactionNum;
    if (!faction_id) {
        return false; // skip basic prototypes from #UNITS
    }
    if (is_alien_faction(faction_id) && ability_id == ABL_DEEP_RADAR) {
        return true; // Caretakers + Usurpers > "Deep Radar" ability for all units
    }
    for (int i = 0; i < Players[faction_id].faction_bonus_count; i++) {
        if (Players[faction_id].faction_bonus_id[i] == RULE_FREEABIL) {
            int abil_bonus_id = Players[faction_id].faction_bonus_val1[i];
            if (has_tech(Ability[abil_bonus_id].preq_tech, faction_id) &&
                (ability_id & (1 << abil_bonus_id))) {
                // Pirates > "Marine Detachment" ability for combat sea units with Adaptive Doctrine
                return true;
            }
        }
    }
    if (VehPrototypes[proto_id].weapon_id == WPN_PROBE_TEAM && ability_id == ABL_ALGO_ENHANCEMENT
        && has_project(SP_NETHACK_TERMINUS, faction_id)) {
        return true; // All Probe Teams act as though they have the "Algorithmic Enhancement"
    }
    return false;
}

/*
Purpose: Temporarily remove the specified unit from its current square and stack in preparation for 
         another action such as interacting with the stack, moving or killing it.
Original Offset: 005BFFA0
Return Value: Unit id
Status: Complete
*/
int __cdecl veh_lift(uint32_t veh_id) {
    BOOL prev_stack_exists = false;
    int16_t prev_veh_id = Vehs[veh_id].prev_veh_id_stack;
    int16_t next_veh_id = Vehs[veh_id].next_veh_id_stack;
    if (prev_veh_id >= 0) {
        prev_stack_exists = true;
        Vehs[prev_veh_id].next_veh_id_stack = next_veh_id;
    }
    int x = Vehs[veh_id].x;
    int y = Vehs[veh_id].y;
    if (next_veh_id >= 0) {
        Vehs[next_veh_id].prev_veh_id_stack = prev_veh_id;
    } else if (!prev_stack_exists && on_map(x, y)) {
        bit_set(x, y, BIT_VEH_IN_TILE, false);
    }
    *VehDropLiftVehID = veh_id;
    *VehLiftX = x;
    *VehLiftY = y;
    Vehs[veh_id].x = -1;
    Vehs[veh_id].y = -1;
    Vehs[veh_id].next_veh_id_stack = -1;
    Vehs[veh_id].prev_veh_id_stack = -1;
    return veh_id;
}

/*
Purpose: Move the specified unit to the provided coordinates.
Original Offset: 005C0080
Return Value: Unit id (1st param), doesn't seem to be used
Status: Complete
*/
int __cdecl veh_drop(uint32_t veh_id, int x, int y) {
    int veh_id_dest = veh_at(x, y);
    Vehs[veh_id].next_veh_id_stack = (int16_t)veh_id_dest;
    Vehs[veh_id].prev_veh_id_stack = -1;
    Vehs[veh_id].x = (int16_t)x;
    Vehs[veh_id].y = (int16_t)y;
    *VehDropLiftVehID = -1;
    if (veh_id_dest < 0) {
        if (y < 0) {
            return veh_id;
        }
        if (on_map(x, y) && !(bit_at(x, y) & BIT_BASE_IN_TILE)) {
            owner_set(x, y, Vehs[veh_id].faction_id);
        }
    } else {
        Vehs[veh_id_dest].prev_veh_id_stack = (int16_t)veh_id;
    }
    if (on_map(x, y)) {
        uint32_t flags = (Vehs[veh_id].faction_id && get_triad(veh_id) != TRIAD_AIR)
            ? BIT_SUPPLY_REMOVE | BIT_VEH_IN_TILE : BIT_VEH_IN_TILE;
        bit_set(x, y, flags, true);
    }
    return veh_id;
}

/*
Purpose: Set the unit's status to sentry/board.
Original Offset: 005C01A0
Return Value: n/a
Status: Complete
*/
void __cdecl sleep(uint32_t veh_id) {
    Vehs[veh_id].order = ORDER_SENTRY_BOARD;
    Vehs[veh_id].waypoint_x[0] = -1;
    Vehs[veh_id].waypoint_y[0] = 0;
}

/*
Purpose: Move the specified unit to the bottom of the stack.
Original Offset: 005C01D0
Return Value: n/a
Status: Complete
*/
void __cdecl veh_demote(int veh_id) {
    if (veh_id >= 0) {
        int16_t next_veh_id = Vehs[veh_id].next_veh_id_stack;
        if (next_veh_id >= 0) {
            int16_t last_veh_id;
            do {
                last_veh_id = next_veh_id;
                next_veh_id = Vehs[last_veh_id].next_veh_id_stack;
            } while (next_veh_id >= 0);
            if (last_veh_id != veh_id) {
                veh_lift(veh_id);
                Vehs[last_veh_id].next_veh_id_stack = (int16_t)veh_id;
                Vehs[veh_id].prev_veh_id_stack = last_veh_id;
                Vehs[veh_id].next_veh_id_stack = -1;
                Vehs[veh_id].x = Vehs[last_veh_id].x;
                Vehs[veh_id].y = Vehs[last_veh_id].y;
            }
        }
    }
}

/*
Purpose: Move the specified unit to the top of the stack.
Original Offset: 005C0260
Return Value: n/a
Status: Complete
*/
void __cdecl veh_promote(int veh_id) {
    int veh_id_top = veh_top(veh_id);
    if (veh_id_top >= 0 && veh_id_top != veh_id) {
        veh_put(veh_id, Vehs[veh_id_top].x, Vehs[veh_id_top].y);
    }
}

/*
Purpose: Clear the specified unit.
Original Offset: 005C02D0
Return Value: n/a
Status: Complete
*/
void __cdecl veh_clear(uint32_t veh_id, int proto_id, uint32_t faction_id) {
    Vehs[veh_id].x = -4;
    Vehs[veh_id].y = -4;
    Vehs[veh_id].year_end_lurking = 0;
    Vehs[veh_id].unk_1 = 0;
    Vehs[veh_id].flags = 0;
    Vehs[veh_id].faction_id = (uint8_t)faction_id;
    Vehs[veh_id].proto_id = (int16_t)proto_id;
    Vehs[veh_id].next_veh_id_stack = -1;
    Vehs[veh_id].prev_veh_id_stack = -1;
    Vehs[veh_id].waypoint_count = 0;
    Vehs[veh_id].patrol_current_point = 0;
    Vehs[veh_id].order = ORDER_NONE;
    for (uint32_t i = 0; i < 4; i++) {
        Vehs[veh_id].waypoint_x[i] = -1;
        Vehs[veh_id].waypoint_y[i] = -1;
    }
    Vehs[veh_id].state = 0;
    Vehs[veh_id].moves_expended = 0;
    Vehs[veh_id].dmg_incurred = 0;
    Vehs[veh_id].order_auto_type = 0;
    Vehs[veh_id].terraforming_turns = 0;
    Vehs[veh_id].unk_6 = 0;
    Vehs[veh_id].move_to_ai_type = 0;
    Vehs[veh_id].visibility = 0;
    Vehs[veh_id].home_base_id = -1;
    Vehs[veh_id].morale = (uint8_t)(Players[faction_id].rule_morale + 1);
    Vehs[veh_id].unk_5 = 2;
    Vehs[veh_id].probe_action = 0;
    Vehs[veh_id].probe_sabotage_id = 0;
}

/*
Purpose: Check if the prototype can perform artillery combat. The 2nd parameter determines how sea 
         units are treated.
Original Offset: 005C0DB0
Return Value: Has artillery ability? true/false
Status: Complete
*/
BOOL __cdecl can_arty(uint32_t proto_id, BOOL sea_triad_retn) {
    if ((Weapon[VehPrototypes[proto_id].weapon_id].offense_rating <= 0 // PSI + non-combat
        || Armor[VehPrototypes[proto_id].armor_id].defense_rating < 0) // PSI
        && proto_id != BSC_SPORE_LAUNCHER) { // Spore Launcher exception
        return false;
    }
    uint8_t triad = get_proto_triad(proto_id);
    if (triad == TRIAD_SEA) {
        return sea_triad_retn; // cursory check shows this value always being set to true
    }
    if (triad == TRIAD_AIR) {
        return false;
    }
    return has_abil(proto_id, ABL_ARTILLERY); // TRIAD_LAND
}

/*
Purpose: Calculate a unit's morale. TODO: Determine if 2nd param is a toggle for display vs actual 
         morale.
Original Offset: 005C0E40
Return Value: Morale value
Status: Complete
*/
uint32_t __cdecl morale_veh(uint32_t veh_id, BOOL check_drone_riot, int faction_id_vs_native) {
    uint32_t faction_id = Vehs[veh_id].faction_id;
    if (!faction_id) {
        return morale_alien(veh_id, faction_id_vs_native);
    }
    int16_t proto_id = Vehs[veh_id].proto_id;
    if (VehPrototypes[proto_id].plan == PLAN_INFO_WARFARE) { // probe
        int probe_morale = range(PlayersData[faction_id].soc_effect_active.probe, 0, 3);
        probe_morale += has_project(SP_TELEPATHIC_MATRIX, faction_id) ? 2 : 0;
        for (int i = 0; i < MaxTechnologyNum; i++) {
            if (Technology[i].flags & TFLAG_IMPROVED_PROBES && has_tech(i, faction_id)) {
                probe_morale++;
            }
        }
        probe_morale += Vehs[veh_id].morale;
        return range(probe_morale, 2, 6);
    }
    char offense_rating = Weapon[VehPrototypes[proto_id].weapon_id].offense_rating;
    if (proto_id < MaxVehProtoFactionNum && offense_rating < 0) {
        return range(Vehs[veh_id].morale, 0, 6); // Basic Psi Veh
    }
    // everything else
    int morale_modifier = range(PlayersData[faction_id].soc_effect_active.morale, -4, 4);
    if (morale_modifier <= -2) {
        morale_modifier++;
    } else if (morale_modifier >= 2) {
        morale_modifier--;
    }
    int rule_morale = Players[faction_id].rule_morale; // different from 'SOCIAL, MORALE'
    if (rule_morale < 0) { // negative effects 1st
        morale_modifier += rule_morale;
    }
    int home_base_id = Vehs[veh_id].home_base_id;
    if (home_base_id >= 0) { // home base countering negative effects
        if (has_fac_built(FAC_CHILDREN_CRECHE, home_base_id) && morale_modifier < 0) {
            morale_modifier /= 2;
        }
        if (has_fac_built(FAC_BROOD_PIT, home_base_id) && proto_id < MaxVehProtoFactionNum
            && (offense_rating < 0 || proto_id == BSC_SPORE_LAUNCHER) && morale_modifier < 0) {
            morale_modifier /= 2; // never reached due to above 'Basic Psi Veh' checks
        }
    }
    if (rule_morale > 0) {
        morale_modifier += rule_morale;
    }
    BOOL morale_flag = Players[faction_id].rule_flags & RFLAG_MORALE;
    if (morale_flag && morale_modifier < 0) {
        morale_modifier = 0;
    }
    if (check_drone_riot && home_base_id >= 0 
        && Bases[home_base_id].state & BSTATE_DRONE_RIOTS_ACTIVE && !morale_flag) {
        // bug fix: removed premature range bounding negating negative morale effects
        morale_modifier--;
    }
    return range(Vehs[veh_id].morale + morale_modifier, 0, 6);
}

/*
Purpose: Calculate the offense of the specified prototype. Optional param of the unit defending 
         against (-1 to ignore) as well as whether artillery or missile combat is being utilized.
Original Offset: 005C1150
Return Value: Prototype's offense
Status: Complete
*/
uint32_t __cdecl offense_proto(uint32_t proto_id, int veh_id_def, BOOL is_bombard) {
    uint32_t weapon_id = VehPrototypes[proto_id].weapon_id;
    if (Weapon[weapon_id].mode == WPN_MODE_INFOWAR && veh_id_def >= 0
        && VehPrototypes[Vehs[veh_id_def].proto_id].plan == PLAN_INFO_WARFARE) {
        return 16; // probe attacking another probe
    }
    // Bug fix: Vehs.proto_id with veh_id_def -1 could cause arbitrary memory read (Reactor 
    // struct) due to lack of bounds checking when comparing veh_id_def proto_id to Spore Launcher
    if ((is_bombard || (Weapon[weapon_id].offense_rating >= 0
        && (veh_id_def < 0 || Armor[VehPrototypes[Vehs[veh_id_def].proto_id].armor_id].defense_rating >= 0)))
        && (veh_id_def < 0 || Vehs[veh_id_def].proto_id != BSC_SPORE_LAUNCHER)
        && proto_id != BSC_SPORE_LAUNCHER) {
        int off_rating = Weapon[VehPrototypes[proto_id].weapon_id].offense_rating;
        if (off_rating < 0) {
            off_rating = -off_rating;
        }
        if (Chassis[VehPrototypes[proto_id].chassis_id].missile && off_rating < 99) {
            off_rating = (off_rating * 3) / 2;
        }
        return (veh_id_def < 0) ? off_rating : off_rating * 8; // conventional
    }
    return (veh_id_def < 0) ? Rules->psi_combat_ratio_atk[TRIAD_LAND] : // PSI
        Rules->psi_combat_ratio_atk[get_triad(veh_id_def)] * 8;
}

/*
Purpose: Calculate the defense of the specified prototype. Optional param if unit is being attacked 
         (-1 to ignore) as well as whether artillery or missile combat is being utilized.
Original Offset: 005C1290
Return Value: Prototype's defense
Status: Complete
*/
uint32_t __cdecl armor_proto(uint32_t proto_id, int veh_id_atk, BOOL is_bombard) {
    if (Weapon[VehPrototypes[proto_id].weapon_id].mode == WPN_MODE_INFOWAR && veh_id_atk >= 0
        && VehPrototypes[Vehs[veh_id_atk].proto_id].plan == PLAN_INFO_WARFARE) {
        return 16; // probe defending against another probe
    }
    // Bug fix: Vehs[].proto_id with veh_id_atk -1 could cause arbitrary memory read (Reactor 
    // struct) due to lack of bounds checking when comparing veh_id_atk protoID to Spore Launcher
    if (is_bombard && (veh_id_atk < 0 || Vehs[veh_id_atk].proto_id != BSC_SPORE_LAUNCHER)
        && proto_id != BSC_SPORE_LAUNCHER
        || (Armor[VehPrototypes[proto_id].armor_id].defense_rating >= 0 && (veh_id_atk < 0 
            || Weapon[VehPrototypes[Vehs[veh_id_atk].proto_id].weapon_id].offense_rating >= 0))) {
        uint32_t def_rating = range(Armor[VehPrototypes[proto_id].armor_id].defense_rating, 1, 9999);
        return (veh_id_atk < 0) ? def_rating : def_rating * 8; // conventional
    }
    return (veh_id_atk < 0) ? Rules->psi_combat_ratio_def[TRIAD_LAND] : // PSI
        Rules->psi_combat_ratio_def[get_proto_triad(proto_id)] * 8;
}

/*
Purpose: Calculate the speed of the specified prototype on roads.
Original Offset: 005C13B0
Return Value: Prototype's speed on roads
Status: Complete
*/
uint32_t __cdecl speed_proto(uint32_t proto_id) {
    if (proto_id == BSC_FUNGAL_TOWER) {
        return 0; // cannot move
    }
    int chas_id = VehPrototypes[proto_id].chassis_id;
    int speed_val = Chassis[chas_id].speed;
    uint8_t triad = Chassis[chas_id].triad;
    uint32_t weap_id = VehPrototypes[proto_id].weapon_id;
    if (triad == TRIAD_AIR) {
        speed_val += VehPrototypes[proto_id].reactor_id * 2;
    }
    if (Weapon[weap_id].mode == WPN_MODE_TRANSPORT) {
        speed_val--;
    }
    if (has_abil(proto_id, ABL_SLOW)) {
        speed_val--;
    }
    if (has_abil(proto_id, ABL_ANTIGRAV_STRUTS)) {
        speed_val += (triad == TRIAD_AIR) ? VehPrototypes[proto_id].reactor_id * 2 : 1;
    }
    if (triad == TRIAD_AIR) {
        if (has_abil(proto_id, ABL_FUEL_NANOCELLS)) {
            speed_val += 2;
        }
        if (has_project(SP_CLOUDBASE_ACADEMY, proto_id / MaxVehProtoFactionNum)) {
            speed_val += 2; // bonus from Aerospace Complex
        }
        if (has_abil(proto_id, ABL_AIR_SUPERIORITY)) {
            // generally -20% to -25%, in some cases higher due to lossy division rounding
            speed_val = (speed_val * 4) / 5;
        }
        if (Weapon[weap_id].mode == WPN_MODE_TRANSPORT) {
            speed_val /= 2; // 2nd penalty for air transports: -50%
        }
    }
    return range(speed_val, 1, 99) * Rules->move_rate_roads;
}

/*
Purpose: Calculate the speed of a unit on roads taking into consideration prototype speed, 
         elite morale, if the unit is damaged and other factors. The skip_morale parameter seems to 
         only be set to true for certain combat calculations in battle_fight().
Original Offset: 005C1540
Return Value: Speed
Status: Complete
*/
uint32_t __cdecl speed(uint32_t veh_id, BOOL skip_morale) {
    uint32_t proto_id = Vehs[veh_id].proto_id;
    if (proto_id == BSC_FUNGAL_TOWER) { // moved this check to top vs bottom, same logic
        return 0; // cannot move
    }
    uint32_t speed_val = speed_proto(proto_id);
    uint8_t triad = get_proto_triad(proto_id);
    if (triad == TRIAD_SEA && has_project(SP_MARITIME_CONTROL_CENTER, Vehs[veh_id].faction_id)) {
        speed_val += Rules->move_rate_roads * 2;
    }
    if (!skip_morale && morale_veh(veh_id, true, 0) == MORALE_ELITE
        && (proto_id >= MaxVehProtoFactionNum
            || Weapon[VehPrototypes[proto_id].weapon_id].offense_rating >= 0)) {
        speed_val += Rules->move_rate_roads;
    }
    if (Vehs[veh_id].dmg_incurred && triad != TRIAD_AIR) {
        // optimized code without changes to logic
        uint32_t moves = speed_val / Rules->move_rate_roads;
        uint32_t reactor_fac;
        if (VehPrototypes[proto_id].plan == PLAN_ALIEN_ARTIFACT) {
            speed_val = reactor_fac = 1;
        } else {
            reactor_fac = range(VehPrototypes[proto_id].reactor_id, 1, 100) * 10;
            speed_val = range(reactor_fac, 1, 99);
        }
        speed_val = (moves 
            * range(reactor_fac - Vehs[veh_id].dmg_incurred, 0, 9999) + speed_val - 1) / speed_val;
        speed_val = range(speed_val, (triad == TRIAD_SEA) ? 2 : 1, 999) * Rules->move_rate_roads;
    }
    return speed_val;
}

/*
Purpose: Calculate the cargo capacity of a unit. It seems Spore Launchers were considered to have 
         cargo capacity at one time.
Original Offset: 005C1760
Return Value: Cargo capacity
Status: Complete
*/
uint32_t __cdecl veh_cargo(uint32_t veh_id) {
    uint32_t proto_id = Vehs[veh_id].proto_id;
    uint32_t cargo = VehPrototypes[proto_id].carry_capacity;
    return (cargo && proto_id < MaxVehProtoFactionNum
        && (Weapon[VehPrototypes[proto_id].weapon_id].offense_rating < 0 // Isle of the Deep
            || proto_id == BSC_SPORE_LAUNCHER)) ? Vehs[veh_id].morale + 1 : cargo;
}

/*
Purpose: Determine the extra percent cost for building a prototype. Includes a check if the faction 
         has the free prototype flag set or if the player is using one of the easier difficulties.
Original Offset: 005C17D0
Return Value: Percent extra prototype cost
Status: Complete
*/
uint32_t __cdecl prototype_factor(uint32_t proto_id) {
    uint32_t faction_id = proto_id / MaxVehProtoFactionNum;
    if (Players[faction_id].rule_flags & RFLAG_FREEPROTO
        || PlayersData[faction_id].diff_level <= DLVL_SPECIALIST) {
        return 0;
    }
    uint8_t triad = get_proto_triad(proto_id);
    switch (triad) {
      case TRIAD_SEA:
        return Rules->extra_pct_cost_proto_sea;
      case TRIAD_AIR:
        return Rules->extra_pct_cost_proto_air;
      case TRIAD_LAND:
      default:
        return Rules->extra_pct_cost_proto_land;
    }
}

/*
Purpose: Calculate the specified prototype's overall cost to build. Optional output parameter
         whether there is an associated 1st time prototype cost (true) or just the base (false).
Original Offset: 005C1850
Return Value: Mineral cost
Status: Complete
*/
uint32_t __cdecl veh_cost(uint32_t proto_id, int base_id, BOOL *has_proto_cost) {
    uint32_t cost = VehPrototypes[proto_id].cost;
    if (base_id >= 0 && proto_id < MaxVehProtoFactionNum // bug fix: added base_id bounds check
        && (Weapon[VehPrototypes[proto_id].weapon_id].offense_rating < 0
        || proto_id == BSC_SPORE_LAUNCHER) && has_fac_built(FAC_BROOD_PIT, base_id)) {
        cost = (cost * 3) / 4; // Decrease the cost of alien units by 25%
    }
    if (VehPrototypes[proto_id].plan == PLAN_COLONIZATION && base_id >= 0) {
        cost = range(cost, 1, 999);
    }
    uint32_t proto_cost_first = 0;
    if (proto_id >= MaxVehProtoFactionNum 
        && !(VehPrototypes[proto_id].flags & PROTO_TYPED_COMPLETE)) {
        proto_cost_first = (base_id >= 0 && has_fac_built(FAC_SKUNKWORKS, base_id))
            ? 0 : (prototype_factor(proto_id) * base_cost(proto_id) + 50) / 100; // moved checks up
        cost += proto_cost_first;
    }
    if (has_proto_cost) {
        *has_proto_cost = proto_cost_first != 0;
    }
    return cost;
}

/*
Purpose: Check if the land unit inside an air transport can disembark. The transport must be in 
         either a base or an airbase to do so.
Original Offset: 005C1C40
Return Value: Can the specified unit disembark? true/false
Status: Complete
*/
BOOL __cdecl veh_jail(uint32_t veh_id) {
    if (get_triad(veh_id) == TRIAD_LAND && Vehs[veh_id].order == ORDER_SENTRY_BOARD
        && Vehs[veh_id].waypoint_x[0] >= 0 && get_triad(Vehs[veh_id].waypoint_x[0]) == TRIAD_AIR
        && (!(bit_at(Vehs[veh_id].x, Vehs[veh_id].y) & BIT_BASE_IN_TILE)
            || owner_at(Vehs[veh_id].x, Vehs[veh_id].y) >= 8
            || owner_at(Vehs[veh_id].x, Vehs[veh_id].y) <= 0)
        && !(bit_at(Vehs[veh_id].x, Vehs[veh_id].y) & BIT_AIRBASE)) {
        return true;
    }
    return false;
}

/*
Purpose: Sets all moves for the specified unit as expended.
Original Offset: 005C1D20
Return Value: n/a
Status: Complete
*/
void __cdecl veh_skip(uint32_t veh_id) {
    // TODO Bug: Due to size of moves_expended, speeds over 255 will be incorrect. The speed()
    //           function can return a value from 1-999. Eventually increase size to 16 bits.
    Vehs[veh_id].moves_expended = (uint8_t)speed(veh_id, false);
}

/*
Purpose: Initialize/reset the fake unit id (2048) used as a placeholder for various UI elements.
Original Offset: 005C1D50
Return Value: Fake unit id (2048)
Status: Complete
*/
int __cdecl veh_fake(int proto_id, uint32_t faction_id) {
    veh_clear(2048, proto_id, faction_id);
    return 2048;
}

/*
Purpose: Activate the specified unit and clear the current action.
Original Offset: 005C1D70
Return Value: Unit id (doesn't look to be used on return)
Status: Complete
*/
int __cdecl veh_wake(uint32_t veh_id) {
    uint8_t order = Vehs[veh_id].order;
    uint32_t state = Vehs[veh_id].state;
    if (order >= ORDER_FARM && order < ORDER_MOVE_TO && !(state & VSTATE_CRAWLING)) {
        // TODO bug fix: Issue with moves_expended size / speed return, see veh_skip()
        Vehs[veh_id].moves_expended = (uint8_t)(speed(veh_id, false) - Rules->move_rate_roads);
        int terra_turns = Vehs[veh_id].terraforming_turns;
        if (terra_turns) {
            terra_turns -= contribution(veh_id, order - 4);
            if (terra_turns < 0) {
                terra_turns = 0;
            }
            Vehs[veh_id].terraforming_turns = (uint8_t)terra_turns;
        }
    }
    if (state & VSTATE_UNK_200 && Vehs[veh_id].order_auto_type == ORDERA_ON_ALERT
        && !(state & VSTATE_UNK_4)) {
        Vehs[veh_id].moves_expended = 0;
    }
    Vehs[veh_id].order = ORDER_NONE;
    Vehs[veh_id].state &= ~(VSTATE_UNK_200 | VSTATE_EXPLORE | VSTATE_UNK_1000000 
        | VSTATE_UNK_2000000 | VSTATE_UNK_8000000); // TODO: Identify VSTATE flags
    return veh_id;
}

/*
Purpose: Get the triad for the specified prototype.
Original Offset: n/a
Return Value: Triad (see VehTriad)
Status: Complete
*/
uint8_t __cdecl get_proto_triad(uint32_t proto_id) {
    return Chassis[VehPrototypes[proto_id].chassis_id].triad;
}

/*
Purpose: Get the triad for the specified unit.
Original Offset: n/a
Return Value: Triad (see VehTriad)
Status: Complete
*/
uint8_t __cdecl get_triad(uint32_t veh_id) {
    return get_proto_triad(Vehs[veh_id].proto_id);
}

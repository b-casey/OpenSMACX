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
#include "faction.h"
#include "alpha.h"
#include "base.h"
#include "council.h"
#include "game.h"
#include "map.h"
#include "probe.h"
#include "technology.h"
#include "veh.h"

Player *Players = (Player *)0x00946A50;
PlayerData *PlayersData = (PlayerData *)0x0096C9E0;
FactionArt *FactionsArt = (FactionArt *)0x0078E978;
RulesSocialCategory *SocialCategories = (RulesSocialCategory *)0x0094B000;
RulesSocialEffect *SocialEffects = (RulesSocialEffect *)0x00946580;
LPSTR *Mood = (LPSTR *)0x0094C9E4;
LPSTR *Repute = (LPSTR *)0x00946A30;
RulesMight *Might = (RulesMight *)0x0094C558;
RulesBonusName *BonusName = (RulesBonusName *)0x009461A8;
uint8_t *FactionsStatus = (uint8_t *)0x009A64E8;
uint32_t *FactionRankings = (uint32_t *)0x009A64EC; // [8]
uint32_t *RankingFactionIDUnk1 = (uint32_t *)0x009A650C;
uint32_t *RankingFactionIDUnk2 = (uint32_t *)0x009A6510;
uint32_t *FactionRankingsUnk = (uint32_t *)0x00945DD8; // [8]
int *DiploFriction = (int *)0x0093FA74; // not always bounded, should it be 0-20?
uint32_t *DiploFrictionFactionIDWith = (uint32_t *)0x0093FABC;
uint32_t *DiploFrictionFactionID = (uint32_t *)0x0093FAC0;

/*
Purpose: Determine if the specified faction is a Progenitor alien faction (Caretakers / Usurpers).
Original Offset: n/a
Return Value: Is faction a Progenitor? true/false
Status: Complete
*/
BOOL __cdecl is_alien_faction(uint32_t faction_id) {
    return *ExpansionEnabled && (Players[faction_id].rule_flags & RFLAG_ALIEN);
}

/*
Purpose: Determine if the specified faction is controlled by a human player or computer AI.
Original Offset: n/a
Return Value: Is faction a human? true/false
Status: Complete
*/
BOOL __cdecl is_human(uint32_t faction_id) {
    return FactionsStatus[0] & (1 << faction_id);
}

/*
Purpose: Determine if the specified faction is alive or whether they've been eliminated.
Original Offset: n/a
Return Value: Is faction alive? true/false
Status: Complete
*/
BOOL __cdecl is_alive(uint32_t faction_id) {
    return FactionsStatus[1] & (1 << faction_id);
}

/*
Purpose: Check whether the primary faction has at least one of the specified treaties (bitfield) 
         with the secondary faction.
Original Offset: 005002F0
Return Value: Treaty status between the two factions, generally treated as a boolean
Status: Complete
*/
uint32_t __cdecl has_treaty(uint32_t faction_id, uint32_t faction_id_with, uint32_t treaty) {
    return PlayersData[faction_id].diplo_treaties[faction_id_with] & treaty;
}

/*
Purpose: Get the faction's name adjective.
Original Offset: 0050B910
Return Value: Faction name adjective
Status: Complete
*/
LPSTR __cdecl get_adjective(uint32_t faction_id) {
    return Players[faction_id].adj_name_faction;
}

/*
Purpose: Get the faction's noun.
Original Offset: 0050B930
Return Value: Faction noun
Status: Complete
*/
LPSTR __cdecl get_noun(uint32_t faction_id) {
    parse_set(Players[faction_id].noun_gender, Players[faction_id].is_noun_plural);
    return Players[faction_id].noun_faction;
}

/*
Purpose: Determine whether automatic contact is enabled for net or PBEM games.
Original Offset: 00539160
Return Value: Is always contact enabled? true/false
Status: Complete
*/
BOOL __cdecl auto_contact() {
    return (*IsMultiplayerNet && Rules->tgl_human_always_contact_net) ? true
        : *IsMultiplayerPBEM && Rules->tgl_humans_always_contact_pbem;
}

/*
Purpose: Determine if the overall dominant human faction is a minor threat based on base count.
Original Offset: 00539B70
Return Value: Is faction minor threat? true/false
Status: Complete
*/
BOOL __cdecl great_beelzebub(uint32_t faction_id, BOOL is_aggressive) {
    if (is_human(faction_id) && FactionRankings[7] == faction_id) {
        uint32_t bases_threat = (*TurnCurrentNum + 25) / 50;
        if (bases_threat < 4) {
            bases_threat = 4;
        }
        if (PlayersData[faction_id].current_num_bases > bases_threat
            && (PlayersData[faction_id].diff_level > DLVL_SPECIALIST
                || *GameRules & RULES_INTENSE_RIVALRY || is_aggressive)) {
            return true;
        }
    }
    return false;
}

/*
Purpose: Determine if the specified faction is considered a threat based on the game state and 
         ranking.
Original Offset: 00539C00
Return Value: Is the specified faction a threat? true/false
Status: Complete
*/
BOOL __cdecl great_satan(uint32_t faction_id, BOOL is_aggressive) {
    if (great_beelzebub(faction_id, is_aggressive)) {
        BOOL has_intense_riv = (*GameRules & RULES_INTENSE_RIVALRY);
        if (*TurnCurrentNum <= ((has_intense_riv ? 0 
            : (DLVL_TRANSCEND - PlayersData[faction_id].diff_level) * 50) + 100)) {
            return false;
        }
        if (climactic_battle() && aah_ooga(faction_id, -1) == faction_id) {
            return true;
        }
        uint32_t diff_factor;
        uint32_t factor;
        if (has_intense_riv) {
            factor = 4;
            diff_factor = DLVL_TRANSCEND;
        } else if (PlayersData[faction_id].diff_level >= DLVL_LIBRARIAN 
            || *GameRules & RULES_VICTORY_CONQUEST || *ObjectiveReqVictory <= 1000) {
            factor = 2;
            diff_factor = DLVL_LIBRARIAN;
        } else {
            factor = 1;
            diff_factor = DLVL_TALENT;
        }
        return (factor * FactionRankingsUnk[FactionRankings[7]] 
            >= diff_factor * FactionRankingsUnk[FactionRankings[6]]);
    }
    return false;
}

/*
Purpose: Check whether the specified faction is nearing the diplomatic victory requirements to be 
         able to call a Supreme Leader vote. Optional 2nd parameter (0/-1 to disable) that specifies
         a faction to skip if they have a pact with faction from the 1st parameter.
Original Offset: 00539D40
Return Value: Faction id if nearing diplomatic victory or zero
Status: Complete
*/
uint32_t __cdecl aah_ooga(int faction_id, int pact_faction_id) {
    if (!(*GameRules & RULES_VICTORY_DIPLOMATIC)) {
        return 0; // Diplomatic Victory not allowed
    }
    uint32_t votes_total = 0;
    for (uint32_t i = 1; i < MaxPlayerNum; i++) {
        votes_total += council_votes(i);
    }
    uint32_t faction_id_ret = 0;
    for (int i = 1; i < MaxPlayerNum; i++) {
        if (i != pact_faction_id
            && (pact_faction_id <= 0 || !has_treaty(i, pact_faction_id, DTREATY_PACT)
                || !(*GameRules & RULES_VICTORY_COOPERATIVE))) {
            int proposal_preq = Proposal[PROP_UNITE_SUPREME_LEADER].preq_tech;
            if ((has_tech(proposal_preq, faction_id)
                || (proposal_preq >= 0 
                    && (has_tech(Technology[proposal_preq].preq_tech_1, faction_id)
                    || has_tech(Technology[proposal_preq].preq_tech_2, faction_id))))
                && council_votes(i) * 2 >= votes_total && (!faction_id_ret || i == faction_id)) {
                faction_id_ret = i;
            }
        }
    }
    return faction_id_ret;
}

/*
Purpose: Check if the human controlled player is nearing the endgame.
Original Offset: 00539E40
Return Value: Is human player nearing endgame? true/false
Status: Complete
*/
BOOL __cdecl climactic_battle() {
    for (uint32_t i = 1; i < MaxPlayerNum; i++) {
        if (is_human(i) && PlayersData[i].corner_market_turn > *TurnCurrentNum) {
            return true; // Human controlled player initiated Corner Global Energy Market
        }
    }
    if (aah_ooga(0, -1)) { // nearing Supreme Leader, these parameters will always return false
        return true; // TODO: Revisit in future once more end game code is complete. This may have
                     //       been effectively disabled as a design decision rather than a bug.
    }
    if (ascending(0)) {
        for (uint32_t i = 1; i < MaxPlayerNum; i++) {
            if (is_human(i) && (has_tech(Facility[FAC_PSI_GATE].preq_tech, i)
                || has_tech(Facility[FAC_VOICE_OF_PLANET].preq_tech, i))) {
                return true; // Human controlled player has tech to build PSI Gates or VoP
            }
        }
    }
    return false;
}

/*
Purpose: Determine if the specified AI faction is at the game climax based on certain conditions.
Original Offset: 00539EF0
Return Value: Is AI faction at climax? true/false
Status: Complete
*/
BOOL __cdecl at_climax(uint32_t faction_id) {
    if (is_human(faction_id) || *GameState & STATE_UNK_1 || *DiffLevelCurrent == DLVL_CITIZEN
        || !climactic_battle()) {
        return false;
    }
    if (aah_ooga(faction_id, faction_id)) {
        return true;
    }
    for (uint32_t i = 1; i < MaxPlayerNum; i++) {
        if (i != faction_id && PlayersData[faction_id].corner_market_turn > *TurnCurrentNum
            && (!has_treaty(faction_id, i, DTREATY_PACT)
                || !(*GameRules & RULES_VICTORY_COOPERATIVE))) {
            return true;
        }
    }
    int trans_most_min_them = 0;
    int trans_most_min_us = 0;
    for (int i = 0; i < *BaseCurrentCount; i++) {
        if (Bases[i].queue_production_id[0] == -FAC_ASCENT_TO_TRANSCENDENCE) {
            int min_accum = Bases[i].minerals_accumulated;
            if (Bases[i].faction_id_current == faction_id) {
                if (trans_most_min_us <= min_accum) {
                    trans_most_min_us = min_accum;
                }
            } else if (trans_most_min_them <= min_accum) {
                trans_most_min_them = min_accum;
            }
        }
    }
    for (uint32_t i = 1; i < MaxPlayerNum; i++) {
        if (i != faction_id) {
            if (ascending(i) && !ascending(faction_id)) { // both return same so irrelevant check
                return true;
            }
        }
    }
    return trans_most_min_them && trans_most_min_them > trans_most_min_us;
}

/*
Purpose: Add friction between the two specified factions.
Original Offset: 0053A030
Return Value: n/a
Status: Complete
*/
void __cdecl cause_friction(uint32_t faction_id, uint32_t faction_id_with, int friction) {
    uint32_t *diplo_friction = &PlayersData[faction_id].diplo_friction[faction_id_with];
    *diplo_friction = range(*diplo_friction + friction, 0, 20);
    if (*DiploFrictionFactionID == faction_id && *DiploFrictionFactionIDWith == faction_id_with) {
        *DiploFriction += friction; // not bounded?
    }
}

/*
Purpose: Normalize the diplomatic friction value into a mood offset.
Original Offset: 0053A090
Return Value: Mood (0-8)
Status: Complete
*/
uint32_t __cdecl get_mood(int friction) {
    if (friction <= 0) {
        return MOOD_MAGNANIMOUS;
    }
    if (friction <= 2) {
        return MOOD_SOLICITOUS;
    }
    if (friction <= 4) {
        return MOOD_COOPERATIVE;
    }
    if (friction <= 8) {
        return MOOD_NONCOMMITTAL;
    }
    if (friction <= 12) {
        return MOOD_AMBIVALENT;
    }
    if (friction <= 15) {
        return MOOD_OBSTINATE;
    }
    if (friction <= 17) {
        return MOOD_QUARRELSOME;
    }
    return (friction > 19) + MOOD_BELLIGERENT; // or MOOD_SEETHING if condition is true
}

/*
Purpose: Calculate the negative reputation the specified faction has with another.
Original Offset: 0053A100
Return Value: Bad reputation
Status: Complete
*/
uint32_t __cdecl reputation(uint32_t faction_id, uint32_t faction_id_with) {
    return range(PlayersData[faction_id].integrity_blemishes
        - PlayersData[faction_id].diplo_unk1[faction_id_with], 0, 99);
}

/*
Purpose: Calculate the amount of patience the specified faction has with another.
Original Offset: 0053A150
Return Value: Patience
Status: Complete
*/
int __cdecl get_patience(uint32_t faction_id_with, uint32_t faction_id) {
    if (has_treaty(faction_id, faction_id_with, DTREATY_VENDETTA)) {
        return 1;
    }
    if (has_treaty(faction_id, faction_id_with, DTREATY_PACT)) {
        return has_treaty(faction_id, faction_id_with, DTREATY_HAVE_SURRENDERED) ? 500 : 6;
    }
    return (has_treaty(faction_id, faction_id_with, DTREATY_TREATY) != 0) 
        - ((*DiploFriction + 3) / 8) + 3;
}

/*
Purpose: Calculate the amount of goodwill a loan will generate. This is used to reduce friction.
Original Offset: 0053A1C0
Return Value: Goodwill (friction reduction amount)
Status: Complete
*/
uint32_t __cdecl energy_value(uint32_t loan_principal) {
    uint32_t goodwill = 0;
    uint32_t divisor = 2;
    for (int weight = 10, energy = loan_principal / 5; energy > 0; energy -= weight, weight = 20) {
        goodwill += ((weight >= 0) ? ((energy > weight) ? weight : energy) : 0) / divisor++;
    }
    return (goodwill + 4) / 5;
}

/*
Purpose: Set or unset the diplomatic treaty for the specified faction with another faction.
Original Offset: 0055BB30
Return Value: n/a
Status: Complete
*/
void __cdecl set_treaty(uint32_t faction_id, uint32_t faction_id_with, uint32_t treaty, BOOL set) {
    if (set) {
        PlayersData[faction_id].diplo_treaties[faction_id_with] |= treaty;
        if (treaty & DTREATY_UNK_40) {
            PlayersData[faction_id].diplo_merc[faction_id_with] = 50;
        }
    } else {
        PlayersData[faction_id].diplo_treaties[faction_id_with] &= ~treaty;
    }
}

/*
Purpose: Set or unset the diplomatic agenda for the specified faction with another faction.
Original Offset: 0055BBA0
Return Value: n/a
Status: Complete
*/
void __cdecl set_agenda(uint32_t faction_id, uint32_t faction_id_with, uint32_t agenda, BOOL set) {
    if (set) {
        PlayersData[faction_id].diplo_agenda[faction_id_with] |= agenda;
    } else {
        PlayersData[faction_id].diplo_agenda[faction_id_with] &= ~agenda;
    }
}

/*
Purpose: Check whether the primary faction has at least one of the specified agendas (bitfield)
         with the secondary faction.
Original Offset: 005591E0
Return Value: Agenda status between the two factions, generally treated as a boolean
Status: Complete - testing
*/
uint32_t __cdecl has_agenda(uint32_t faction_id, uint32_t faction_id_with, uint32_t agenda) {
    return PlayersData[faction_id].diplo_agenda[faction_id_with] & agenda;
}

/*
Purpose: Determine if the specified faction want to attack the target faction.
Original Offset: 0055BC80
Return Value: Does the faction want to attack target? true/false
Status: Complete - testing
*/
BOOL __cdecl wants_to_attack(uint32_t faction_id, uint32_t faction_id_tgt, int faction_id_unk) {
    uint32_t peace_faction_id = 0;
    BOOL unk_tgl = false;
    if (Players[faction_id].rule_flags & RFLAG_ALIEN
        && Players[faction_id_tgt].rule_flags & RFLAG_ALIEN) {
        return true;
    }
    if (has_treaty(faction_id, faction_id_tgt,
        DTREATY_WANT_REVENGE | DTREATY_UNK_40 | DTREATY_ATROCITY_VICTIM)) {
        return true;
    }
    if (PlayersData[faction_id_tgt].major_atrocities && PlayersData[faction_id].major_atrocities) {
        return true;
    }
    if (has_treaty(faction_id, faction_id_tgt, DTREATY_UNK_4000000)) {
        return false;
    }
    if (PlayersData[faction_id].current_num_bases <= 1) {
        return false;
    }
    if (!is_human(faction_id_tgt) && PlayersData[faction_id].flags & PFLAG_TEAM_UP_VS_HUMAN) {
        return false;
    }
    uint32_t want_to_attack = 0;
    for (uint32_t i = 1; i < MaxPlayerNum; i++) {
        if (i != faction_id && i != faction_id_tgt) {
            if (has_treaty(faction_id, i, DTREATY_HAVE_SURRENDERED | DTREATY_PACT)) {
                peace_faction_id = i;
            }
            if (has_treaty(faction_id, i, DTREATY_VENDETTA)
                && !has_treaty(faction_id_tgt, i, DTREATY_PACT)) {
                want_to_attack++;
                if (PlayersData[i].mil_strength_1 
                    > ((PlayersData[faction_id_tgt].mil_strength_1 * 3) / 2)) {
                    want_to_attack++;
                }
            }
            if (great_beelzebub(i, false)
                && (*TurnCurrentNum >= 100 || *GameRules & RULES_INTENSE_RIVALRY)) {
                if (has_treaty(faction_id_tgt, i, DTREATY_VENDETTA)) {
                    want_to_attack++;
                }
                if (has_treaty(faction_id_tgt, i, DTREATY_COMMLINK) 
                    && has_treaty(faction_id, i, DTREATY_COMMLINK)) {
                    want_to_attack++;
                }
            }
            if (has_treaty(faction_id, i, DTREATY_PACT) && is_human(i)) {
                if (has_treaty(faction_id_tgt, i, DTREATY_PACT)) {
                    want_to_attack += 2;
                }
                BOOL has_surrender = has_treaty(faction_id, i, DTREATY_HAVE_SURRENDERED);
                if (has_surrender && has_treaty(i, faction_id_tgt, DTREATY_PACT | DTREATY_TREATY)) {
                    return false;
                }
                if (has_treaty(faction_id_tgt, i, DTREATY_VENDETTA)) {
                    unk_tgl = true;
                    want_to_attack -= (has_surrender ? 4 : 2);
                }
            }
        }
    }
    if (peace_faction_id) {
        if (has_treaty(faction_id_tgt, peace_faction_id, DTREATY_VENDETTA)) {
            return true;
        }
        if (has_treaty(faction_id_tgt, peace_faction_id, DTREATY_PACT | DTREATY_TREATY)) {
            return false;
        }
    }
    if (PlayersData[faction_id].ai_fight < 0 && !unk_tgl && FactionRankings[7] != faction_id_tgt) {
        return false;
    }
    uint32_t region_top_base_count[8] = { 0 }; // bug fix: initialize to zero, original doesn't and
    for (int region = 1; region < MaxRegionLandNum; region++) { // compares arbitrary data on stack
        for (uint32_t f = 1; f < MaxPlayerNum; f++) {
            uint32_t total_bases = PlayersData[f].region_total_bases[region];
            if (total_bases > region_top_base_count[f]) {
                region_top_base_count[f] = total_bases;
            }
        }
    }
    for (uint32_t f = 1; f < MaxPlayerNum; f++) {
        region_top_base_count[f] -= (region_top_base_count[f] / 4);
    }
    int region_target_hq = -1;
    int region_hq = -1;
    for (int i = 0; i < *BaseCurrentCount; i++) {
        if (has_fac_built(FAC_HEADQUARTERS, i)) {
            uint32_t base_faction = Bases[i].faction_id_current;
            if (base_faction == faction_id) {
                region_hq = region_at(Bases[i].x, Bases[i].y);
            } else if (base_faction == faction_id_tgt) {
                region_target_hq = region_at(Bases[i].x, Bases[i].y);
            }
        }
    }
    uint32_t factor_force_rating = 0;
    uint32_t factor_count = 0;
    uint32_t factor_unk = 1;
    for (int region = 1; region < MaxRegionLandNum; region++) {
        if (!bad_reg(region)) {
            uint32_t force_rating = PlayersData[faction_id].region_force_rating[region];
            if (force_rating) {
                uint32_t total_cmbt_vehs 
                    = PlayersData[faction_id_tgt].region_total_combat_vehs[region];
                uint32_t total_bases_tgt = PlayersData[faction_id_tgt].region_total_bases[region];
                if (total_cmbt_vehs || total_bases_tgt) {
                    if (PlayersData[faction_id].region_total_bases[region] 
                        >= ((region_top_base_count[faction_id] / 4) * 3) || region == region_hq) {
                        uint32_t compare = force_rating +
                            PlayersData[faction_id].region_total_combat_vehs[region] +
                            (faction_id_unk > 0 
                                ? PlayersData[faction_id_unk].region_force_rating[region] / 4 : 0);
                        if (PlayersData[faction_id_tgt].region_force_rating[region] > compare) {
                            return false;
                        }
                    }
                    if (total_bases_tgt) {
                        factor_force_rating += force_rating + (faction_id_unk > 0
                            ? PlayersData[faction_id_unk].region_force_rating[region] / 2 : 0);
                    }
                    if ((total_bases_tgt >= ((region_top_base_count[faction_id_tgt] / 4) * 3)
                        || region == region_target_hq) && force_rating > total_cmbt_vehs) {
                        factor_force_rating += force_rating + (faction_id_unk > 0
                            ? PlayersData[faction_id_unk].region_force_rating[region] / 2 : 0);
                    }
                    factor_unk += total_cmbt_vehs 
                        + PlayersData[faction_id].region_total_bases[region]
                        ? PlayersData[faction_id_tgt].region_force_rating[region] / 2 : 0;
                    if (PlayersData[faction_id].region_total_bases[region]) {
                        factor_count++;
                    }
                }
            }
        }
    }
    want_to_attack -= PlayersData[faction_id].ai_fight * 2;
    int tech_comm_bonus = PlayersData[faction_id].tech_commerce_bonus;
    int tech_comm_bonus_target = PlayersData[faction_id_tgt].tech_commerce_bonus;
    if (tech_comm_bonus > ((tech_comm_bonus_target * 3) / 2)) {
        want_to_attack++;
    }
    if (tech_comm_bonus < ((tech_comm_bonus_target * 2) / 3)) {
        want_to_attack--;
    }
    int best_armor_target = PlayersData[faction_id_tgt].best_armor_value;
    int best_weapon = PlayersData[faction_id_tgt].best_weapon_value;
    if (best_weapon > (best_armor_target * 2)) {
        want_to_attack--;
    }
    if (best_weapon <= best_armor_target) {
        want_to_attack++;
    }
    if (!has_treaty(faction_id, faction_id_tgt, DTREATY_VENDETTA)) {
        want_to_attack++;
    }
    if (!has_treaty(faction_id, faction_id_tgt, DTREATY_PACT)) {
        want_to_attack++;
    }
    if (faction_id_unk > 0 && !great_satan(faction_id_unk, false)) {
        want_to_attack--;
    }
    if (has_agenda(faction_id, faction_id_tgt, DAGENDA_UNK_200)
        && *GameRules & RULES_INTENSE_RIVALRY) {
        want_to_attack--;
    }
    want_to_attack -= range((PlayersData[faction_id_tgt].integrity_blemishes 
        - PlayersData[faction_id].integrity_blemishes + 2) / 3, 0, 2);
    int morale_factor = range(PlayersData[faction_id].soc_effect_pending.morale, -4, 4) 
        + Players[faction_id].rule_morale + 16;
    int morale_factor_target 
        = range(PlayersData[faction_id_tgt].soc_effect_pending.morale, -4, 4)
        + Players[faction_id_tgt].rule_morale + 16;
    if ((factor_count || want_to_attack > 0
        || has_treaty(faction_id, faction_id_tgt, DTREATY_UNK_20000000)) &&
        ((morale_factor * factor_force_rating * 6) / (morale_factor_target * factor_unk)) 
        < (want_to_attack + 6)) {
        return false;
    }
    return true;
}

/*
Purpose: Determine the ideal unit count to protect a faction's bases in the specified land region.
Original Offset: 00560D50
Return Value: Amount of non-offensive units needed to guard region
Status: Complete
*/
uint32_t __cdecl guard_check(uint32_t faction_id, uint32_t region) {
    if (region >= MaxRegionLandNum) {
        return 0;
    }
    int factor = 2 - PlayersData[faction_id].ai_fight;
    uint32_t plan_region = PlayersData[faction_id].region_base_plan[region];
    if (plan_region == PLAN_COLONIZATION) {
        factor += 2;
    } else if (plan_region == PLAN_DEFENSIVE) {
        factor = 1; // 1-1 unit per base ratio
    }
    if (PlayersData[faction_id].flags & PFLAG_STRAT_DEF_OBJECTIVES) {
        factor = 1; // 1-1 unit per base ratio
    }
    return (PlayersData[faction_id].region_total_bases[region] + factor - 1) / factor;
}

/*
Purpose: Add the specific goal to the faction's goals for the specified tile. Optional base param.
Original Offset: 00579A30
Return Value: n/a
Status:  Complete
*/
void __cdecl add_goal(uint32_t faction_id, int type, int priority, int x, int y, int base_id) {
    if (!on_map(x, y)) {
        return;
    }
    for (int i = 0; i < MaxGoalsNum; i++) {
        Goal &goals = PlayersData[faction_id].goals[i];
        if (goals.x == x && goals.y == y && goals.type == type) {
            if (goals.priority <= priority) {
                goals.priority = (int16_t)priority;
            }
            return;
        }
    }
    int priority_search = 0;
    int goal_id = -1;
    for (int i = 0; i < MaxGoalsNum; i++) {
        Goal &goals = PlayersData[faction_id].goals[i];
        int type_cmp = goals.type;
        int priroty_cmp = goals.priority;
        if (type_cmp < 0 || priroty_cmp < priority) {
            int cmp = type_cmp >= 0 ? 0 : 1000;
            if (!cmp) {
                cmp = priroty_cmp > 0 ? 20 - priroty_cmp : priroty_cmp + 100;
            }
            if (cmp > priority_search) {
                priority_search = cmp;
                goal_id = i;
            }
        }
    }
    if (goal_id >= 0) {
        Goal &goals = PlayersData[faction_id].goals[goal_id];
        goals.type = (int16_t)type;
        goals.priority = (int16_t)priority;
        goals.x = x;
        goals.y = y;
        goals.base_id = base_id;
    }
}

/*
Purpose: Add the specific site to the faction's site goals for the specified tile.
Original Offset: 00579B70
Return Value: n/a
Status: Complete
*/
void __cdecl add_site(uint32_t faction_id, int type, int priority, int x, int y) {
    if ((x ^ y) & 1 && *GameState & STATE_DEBUG_MODE) {
        danger("Bad SITE", "", x, y, type);
    }
    for (int i = 0; i < MaxSitesNum; i++) {
        Goal &sites = PlayersData[faction_id].sites[i];
        if (sites.x == x && sites.y == y && sites.type == type) {
            if (sites.priority <= priority) {
                sites.priority = (int16_t)priority;
            }
            return;
        }
    }
    int priority_search = 0;
    int site_id = -1;
    for (int i = 0; i < MaxSitesNum; i++) {
        Goal &sites = PlayersData[faction_id].sites[i];
        int type_cmp = sites.type;
        int priroty_cmp = sites.priority;
        if (type_cmp < 0 || priroty_cmp < priority) {
            int cmp = type_cmp >= 0 ? 0 : 1000;
            if (!cmp) {
                cmp = 20 - priroty_cmp;
            }
            if (cmp > priority_search) {
                priority_search = cmp;
                site_id = i;
            }
        }
    }
    if (site_id >= 0) {
        Goal &sites = PlayersData[faction_id].sites[site_id];
        sites.type = (int16_t)type;
        sites.priority = (int16_t)priority;
        sites.x = x;
        sites.y = y;
        add_goal(faction_id, type, priority, x, y, -1);
    }
}

/*
Purpose: Check if a goal exists at the tile for the specified faction and type.
Original Offset: 00579CC0
Return Value: Does the specific goal exist for the faction at tile? true/false
Status: Complete
*/
BOOL __cdecl at_goal(uint32_t faction_id, int type, int x, int y) {
    for (int i = 0; i < MaxGoalsNum; i++) {
        Goal &goals = PlayersData[faction_id].goals[i];
        if (goals.x == x && goals.y == y && goals.type == type) {
            return true;
        }
    }
    return false;
}

/*
Purpose: Check if a site exists at the tile for the specified faction and type.
Original Offset: 00579D20
Return Value: Does specific site exist for faction at tile? true/false
Status: Complete
*/
BOOL __cdecl at_site(uint32_t faction_id, int type, int x, int y) {
    for (int i = 0; i < MaxSitesNum; i++) {
        Goal &sites = PlayersData[faction_id].sites[i];
        if (sites.x == x && sites.y == y && sites.type == type) {
            return true;
        }
    }
    return false;
}

/*
Purpose: Wipe all goals for the specified faction then recreate any active site related goals.
Original Offset: 00579D80
Return Value: n/a
Status: Complete
*/
void __cdecl wipe_goals(uint32_t faction_id) {
    for (int i = 0; i < MaxGoalsNum; i++) {
        Goal &goals = PlayersData[faction_id].goals[i];
        int16_t priority = goals.priority;
        if (priority < 0) {
            goals.type = AI_GOAL_UNUSED;
        } else {
            goals.priority = -priority;
        }
    }
    for (int i = 0; i < MaxSitesNum; i++) {
        Goal &sites = PlayersData[faction_id].sites[i];
        int16_t type = sites.type;
        if (type >= 0) {
            add_goal(faction_id, type, sites.priority, sites.x, sites.y, -1);
        }
    }
}

/*
Purpose: Initialize all goals for the specified faction.
Original Offset: 00579E00
Return Value: n/a
Status: Complete
*/
void __cdecl init_goals(uint32_t faction_id) {
    for (int i = 0; i < MaxGoalsNum; i++) {
        Goal &goals = PlayersData[faction_id].goals[i];
        goals.type = -1;
        goals.priority = 0;
        goals.x = 0;
        goals.y = 0;
        goals.base_id = 0;
    }
    for (int i = 0; i < MaxSitesNum; i++) {
        Goal &sites = PlayersData[faction_id].sites[i];
        sites.type = -1;
        sites.priority = 0;
        sites.x = 0;
        sites.y = 0;
        sites.base_id = 0;
    }
}

/*
Purpose: Delete sites of the specified type within proximity of the tile along with related goals.
Original Offset: 00579E70
Return Value: n/a
Status: Complete
*/
void __cdecl del_site(uint32_t faction_id, int type, int x, int y, int proximity) {
    for (int i = 0; i < MaxSitesNum; i++) {
        Goal &sites = PlayersData[faction_id].sites[i];
        if (sites.type == type) {
            int dist = vector_dist(x, y, sites.x, sites.y);
            if (dist <= proximity) {
                sites.type = AI_GOAL_UNUSED;
                sites.priority = 0;
                for (int j = 0; j < MaxGoalsNum; j++) {
                    Goal &goal_compare = PlayersData[faction_id].goals[j];
                    if (goal_compare.x == sites.x && goal_compare.y == sites.y &&
                        goal_compare.type == type) {
                        goal_compare.type = AI_GOAL_UNUSED;
                    }
                }
            }
        }
    }
}

/*
Purpose: Calculate the cost for the faction to corner the Global Energy Market (Economic Victory).
Original Offset: 0059EE50
Return Value: Cost to corner the Global Energy Market
Status: Complete
*/
uint32_t __cdecl corner_market(uint32_t faction_id) {
    int cost = 0;
    for (int i = 0; i < *BaseCurrentCount; i++) {
        uint32_t target_faction_id = Bases[i].faction_id_current;
        if (target_faction_id != faction_id) {
            if (!has_treaty(target_faction_id, faction_id, DTREATY_PACT)
                || !has_treaty(target_faction_id, faction_id, DTREATY_HAVE_SURRENDERED)) {
                cost += mind_control(i, faction_id, true);
            }
        }
    }
    return (cost < 1000) ? 1000 : cost;
}

/*
Purpose: Validate whether each faction meets the requirements to have the Map revealed. Added some
         minor tweaks to improve performance without changing the logic.
Original Offset: 005A96D0
Return Value: n/a
Status: Complete
*/
void __cdecl see_map_check() {
    for (int faction_id = 1; faction_id < MaxPlayerNum; faction_id++) {
        PlayersData[faction_id].flags &= ~PFLAG_MAP_REVEALED;
        uint32_t *satellites = &PlayersData[faction_id].satellites_nutrient;
        for (int i = 0; i < 4; i++, satellites++) {
            if (*satellites) {
                PlayersData[faction_id].flags |= PFLAG_MAP_REVEALED;
                break; // end satellite loop early once set
            }
        }
        if (!(PlayersData[faction_id].flags & PFLAG_MAP_REVEALED)) { // skip Tech check if set
            for (int tech_id = 0; tech_id < MaxTechnologyNum; tech_id++) {
                if (Technology[tech_id].flags & TFLAG_REVEALS_MAP 
                    && has_tech(tech_id, faction_id)) {
                    PlayersData[faction_id].flags |= PFLAG_MAP_REVEALED;
                }
            }
        }
    }
}

/*
Purpose: Calculate the basic social engineering modifiers for the specified faction.
Original Offset: 005B0D70
Return Value: n/a
Status: Complete
*/
void __cdecl compute_faction_modifiers(uint32_t faction_id) {
    ZeroMemory(&PlayersData[faction_id].soc_effect_base, sizeof(SocialEffect));
    int count = Players[faction_id].faction_bonus_count;
    for (int i = 0; i < count; i++) {
        if (Players[faction_id].faction_bonus_id[i] == RULE_SOCIAL) {
            *(&PlayersData[faction_id].soc_effect_base.economy
                + Players[faction_id].faction_bonus_val1[i]) 
                += Players[faction_id].faction_bonus_val2[i];
        }
    }
}

/*
Purpose: Calculate the social engineering effect modifiers for the specified faction.
Original Offset: 005B4210
Return Value: n/a
Status: Complete
*/
void __cdecl social_calc(SocialCategory *category, SocialEffect *effect, uint32_t faction_id,
    BOOL UNUSED(toggle), BOOL is_quick_calc) {
    ZeroMemory(effect, sizeof(SocialEffect));
    for (int cat = 0; cat < MaxSocialCatNum; cat++) {
        uint32_t model = *(&category->politics + cat);
        for (int eff = 0; eff < MaxSocialEffectNum; eff++) {
            int effect_val = *(&SocialCategories[cat].model_effect[model].economy + eff);
            if (effect_val < 0) {
                if (cat == SOCIAL_CAT_FUTURE) {
                    if (model == SE_CYBERNETIC) {
                        if (has_project(SP_NETWORK_BACKBONE, faction_id)) {
                            effect_val = 0;
                        }
                    } else if (model == SE_THOUGHT_CONTROL) {
                        if (has_project(SP_CLONING_VATS, faction_id)) {
                            effect_val = 0;
                        }
                    }
                } else if (cat == SOCIAL_CAT_VALUES && model == SE_POWER 
                    && has_project(SP_CLONING_VATS, faction_id)) {
                    effect_val = 0;
                }
                if (effect_val < 0) {
                    for (int i = 0; i < Players[faction_id].faction_bonus_count; i++) {
                        if (Players[faction_id].faction_bonus_val1[i] == cat
                            && Players[faction_id].faction_bonus_val2[i] == (int)model) {
                            if (Players[faction_id].faction_bonus_id[i] == RULE_IMPUNITY) {
                                *(&effect->economy + eff) -= effect_val; // negates neg effects
                            } else if (Players[faction_id].faction_bonus_id[i] == RULE_PENALTY) {
                                *(&effect->economy + eff) += effect_val; // doubles neg effects
                            }
                        }
                    }
                }
            }
            *(&effect->economy + eff) += effect_val;
        }
    }
    if (!is_quick_calc) {
        if (has_project(SP_ASCETIC_VIRTUES, faction_id)) {
            effect->police++;
        }
        if (has_project(SP_LIVING_REFINERY, faction_id)) {
            effect->support += 2;
        }
        if (has_temple(faction_id)) {
            effect->planet++;
            if (is_alien_faction(faction_id)) {
                effect->research++; // bonus documented in conceptsx.txt but not manual
            }
        }
        SocialEffect *effect_calc = effect;
        SocialEffect *effect_base = &PlayersData[faction_id].soc_effect_base;
        for (int eff = 0; eff < MaxSocialEffectNum; eff++) {
            *(&effect_calc->economy + eff) += *(&effect_base->economy + eff);
        }
        for (int i = 0; i < Players[faction_id].faction_bonus_count; i++) {
            if (Players[faction_id].faction_bonus_id[i] == RULE_IMMUNITY) { // cancels neg effects
                int *eff_faction_mod 
                    = (&effect->economy + Players[faction_id].faction_bonus_val1[i]);
                *eff_faction_mod = range(*eff_faction_mod, 0, 999);
            } else if (Players[faction_id].faction_bonus_id[i] == RULE_ROBUST) {
                int *eff_faction_mod 
                    = (&effect->economy + Players[faction_id].faction_bonus_val1[i]);
                if (*eff_faction_mod < 0) {
                    *eff_faction_mod /= 2; // halves neg effects
                }
            }
        }
    }
}

/*
Purpose: Handle the social engineering turn upkeep for the specified faction.
Original Offset: 005B44D0
Return Value: n/a
Status: Complete
*/
void __cdecl social_upkeep(uint32_t faction_id) {
    for (int i = 0; i < MaxSocialCatNum; i++) {
        *(&PlayersData[faction_id].soc_category_active.politics + i) =
            *(&PlayersData[faction_id].soc_category_pending.politics + i);
    }
    SocialCategory *soc_cat_pen = &PlayersData[faction_id].soc_category_pending;
    social_calc(soc_cat_pen, &PlayersData[faction_id].soc_effect_pending, faction_id, false, false);
    social_calc(soc_cat_pen, &PlayersData[faction_id].soc_effect_active, faction_id, false, false);
    social_calc(soc_cat_pen, &PlayersData[faction_id].soc_effect_temp, faction_id, true, false);
    PlayersData[faction_id].soc_upheaval_cost_paid = 0;
}

/*
Purpose: Calculate the cost of the social upheaval for the specified faction.
Original Offset: 005B4550
Return Value: Social upheaval cost
Status: Complete
*/
uint32_t __cdecl social_upheaval(uint32_t faction_id, SocialCategory *category_new) {
    uint32_t change_count = 0;
    for (int i = 0; i < MaxSocialCatNum; i++) {
        if (*(&category_new->politics + i) != 
            *(&PlayersData[faction_id].soc_category_active.politics + i)) {
            change_count++;
        }
    }
    if (!change_count) {
        return 0;
    }
    change_count++;
    uint32_t diff_lvl = is_human(faction_id) ? PlayersData[faction_id].diff_level : DLVL_LIBRARIAN;
    uint32_t cost = change_count * change_count * change_count * diff_lvl;
    if (is_alien_faction(faction_id)) {
        cost += cost / 2;
    }
    return cost;
}

/*
Purpose: Check to see whether the faction can utilize a specific social category and model.
Original Offset: 005B4730
Return Value: Is social category/model available to faction? true/false
Status: Complete
*/
BOOL __cdecl society_avail(int soc_category, int soc_model, uint32_t faction_id) {
    if (Players[faction_id].soc_anti_ideology_category == soc_category
        && Players[faction_id].soc_anti_ideology_model == soc_model) {
        return false;
    }
    return has_tech(SocialCategories[soc_category].preq_tech[soc_model], faction_id);
}

/*
Purpose: Calculate an AI faction's social engineering.
Original Offset: 005B4790
Return Value: n/a
Status: Complete - testing (likely has multiple issues due to length + complexity)
*/
void __cdecl social_ai(uint32_t faction_id, int growth_val, int tech_val, int wealth_val, 
                       int power_val, SocialCategory *output) {
    // setup
    int fight_val;
    int unk_val1 = -1;
    if (!output) {
        if (is_human(faction_id)) {
            return;
        } else {
            growth_val = PlayersData[faction_id].ai_growth;
            tech_val = PlayersData[faction_id].ai_tech;
            wealth_val = PlayersData[faction_id].ai_wealth;
            power_val = PlayersData[faction_id].ai_power;
            fight_val = PlayersData[faction_id].ai_fight;
        }
    } else if (growth_val < 0) {
        return;
    } else if (growth_val > 100) {
        unk_val1 = growth_val - 100;
        growth_val = PlayersData[faction_id].ai_growth;
        tech_val = PlayersData[faction_id].ai_tech;
        wealth_val = PlayersData[faction_id].ai_wealth;
        power_val = PlayersData[faction_id].ai_power;
        fight_val = PlayersData[faction_id].ai_fight;
    } else {
        if (!power_val) {
            fight_val = -(wealth_val != 0);
        } else {
            fight_val = 1;
        }
    }
    // purpose?? loop to set unk_val2 to 256
    uint32_t unk_val2 = 4;
    for (int i = 3; i >= 0; i--) {
        unk_val2 *= 4;
    }
    // future pop growth
    int pop_goal_growth = 0;
    for (int i = 0; i < *BaseCurrentCount; i++) {
        if (Bases[i].faction_id_current == faction_id) {
            pop_goal_growth += pop_goal(i) - Bases[i].population_size;
        }
    }
    // plan and enemy region metrics
    uint32_t unk_sum1 = 0;
    uint32_t unk_count1 = 0;
    uint32_t unk_count2 = 0;
    uint32_t unk_sum2 = 0;
    for (uint32_t region = 1; region < MaxRegionLandNum; region++) {
        if (!bad_reg(region)) {
            uint8_t base_count = PlayersData[faction_id].region_total_bases[region];
            if (base_count) {
                uint8_t plan = PlayersData[faction_id].region_base_plan[region];
                if (plan == PLAN_DEFENSIVE || PlayersData[faction_id].unk_77[region] & 0x400) {
                    unk_sum1 += PlayersData[faction_id].region_total_combat_vehs[region] * 2;
                } else if (plan == PLAN_OFFENSIVE) {
                    unk_sum1 += PlayersData[faction_id].region_total_combat_vehs[region];
                    unk_count2++;
                } else if (plan != PLAN_COLONIZATION) {
                    unk_count1++;
                }
                if (plan <= PLAN_DEFENSIVE) {
                    for (uint32_t f = 1; f < MaxPlayerNum; f++) {
                        if (f != faction_id) {
                            if (has_treaty(faction_id, f, DTREATY_VENDETTA | DTREATY_WANT_REVENGE)
                                && PlayersData[f].enemy_best_psi_offense 
                                >= PlayersData[f].enemy_best_weapon_value 
                                && PlayersData[f].proto_id_active[BSC_MIND_WORMS] > 1) {
                                unk_sum2 += base_count; // not used
                            }
                        }
                    }
                }
            }
        }
    }
    if (output && power_val && unk_sum1 < 1) {
        unk_sum1 = 1;
    }
    // tech ranking
    uint32_t unk_val3 = 1;
    if (*GameState & 0x200 // set in rankings(), related to intense riv + end game
        && PlayersData[faction_id].ranking < 6) {
        int tech_rank_diff = PlayersData[*RankingFactionIDUnk1].tech_ranking / 2
            - PlayersData[faction_id].tech_ranking / 2;
        if (tech_rank_diff > 5) {
            unk_val3 = 2;
        }
        if (tech_rank_diff > 10) {
            unk_val3++;
        }
    }
    // search / calculate
    int unk_val4 = -9999; // search val?
    int soc_cat_bits = -1;
    // unk_sum2 = 0 > used as iterator, optimization re-use, var unused
    for (uint32_t i = 0; i < unk_val2; i++) {
        SocialCategory soc_cat;
        SocialEffect soc_eff;
        int k = i;
        for (int j = 0; j < MaxSocialCatNum; j++) {
            int model = k & 3;
            *(&soc_cat.politics + i) = model;
            if (model) {
                if (Players[faction_id].soc_anti_ideology_category == j
                    || Players[faction_id].soc_anti_ideology_model == model
                    || !has_tech(SocialCategories[model].preq_tech[j], faction_id)) {
                    break;
                }
            }
            k >>= 2;
        }
        do_all_non_input();
        social_calc(&soc_cat, &soc_eff, faction_id, false, false);
        int unk_val6 = 0;
        if (unk_val1 >= 0) {
            unk_val6 = *(&soc_eff.economy + unk_val1) * 1000;
        }
        BOOL has_ideology = false;
        int ideology_cat = Players[faction_id].soc_ideology_category;
        if (ideology_cat >= 0) {
            int ideology_mod = Players[faction_id].soc_ideology_model;
            if (ideology_mod) {
                int unk_val7 = *(&soc_cat.politics + ideology_cat);
                if (!unk_val7 && unk_val7 == ideology_mod) {
                    unk_val6 += PlayersData[faction_id].current_num_bases;
                } else {
                    has_ideology = true;
                }
            }
        }
        int ideology_eff = Players[faction_id].soc_anti_ideology_effect;
        if (ideology_eff >= 0) {
            uint32_t unk_val8 = *(&soc_eff.economy + ideology_eff);
            if (unk_val8 > 0) {
                unk_val6 += PlayersData[faction_id].current_num_bases;
            }
            if (unk_val8 < 0) {
                has_ideology = true;
            }
        }
        if (!has_ideology) {
            // economy
            int effic_val = PlayersData[faction_id].unk_46[range(4 - soc_eff.efficiency, 0, 8)];
            int econ_val = soc_eff.economy;
            int econ_weight = 0;
            if (econ_val < 2) {
                if (econ_val <= 0) {
                    if (econ_val < -1) {
                        for (int region = 0; region < MaxContinentNum; region++) {
                            econ_weight -= (soc_eff.economy + 1)
                                * PlayersData[faction_id].region_total_bases[region]
                                * ((PlayersData[faction_id].region_base_plan[region] 
                                    != PLAN_DEFENSIVE) + 1);
                        }
                        econ_weight /= (unk_sum1 + 1);
                    } else {
                        econ_weight = -1;
                    }
                } else {
                    for (int region = 0; region < MaxContinentNum; region++) {
                        econ_weight += PlayersData[faction_id].region_total_bases[region]
                            * ((PlayersData[faction_id].region_base_plan[region] != PLAN_DEFENSIVE) 
                                + 1);
                    }
                    econ_weight /= (unk_sum1 + 1);
                }
            } else {
                if (econ_val > 4) {
                    econ_val = 4;
                }
                econ_weight = PlayersData[faction_id].unk_47 + econ_val * 2 - 4 / (unk_sum1 + 1);
            }
            for (; econ_weight > 0; econ_weight--, effic_val--) {
                if (effic_val <= 0) {
                    break;
                }
            }
            if (!unk_sum1) {
                econ_weight *= 2;
            }
            econ_weight *= ((PlayersData[faction_id].ai_fight < 0 && unk_sum1 < 2) 
                + tech_val * 2 + wealth_val + 1);
            if (output) {
                if (wealth_val || tech_val) {
                    econ_weight *= 2;
                }
                if (growth_val || power_val) {
                    econ_weight /= 2;
                }
            } else if (wealth_val && !power_val && !growth_val) {
                econ_weight *= 2;
            }
            unk_val6 += econ_weight / unk_val3;
            // support
            int support_val = range(soc_eff.support + 4, 0, 7);
            int support_weight = PlayersData[faction_id].unk_38[support_val];
            if (soc_eff.support <= -4) {
                support_weight *= 3;
            }
            if (unk_sum1) {
                support_weight *= 2;
                if (soc_eff.support <= -3) {
                    support_weight *= 2;
                }
                if ((soc_eff.support == -1 || soc_eff.support == -2) && soc_eff.economy < 2) {
                    support_weight += support_weight / 2;
                }
            }
            if (output) {
                if (power_val) {
                    support_weight *= 3;
                }
                if (growth_val) {
                    support_weight *= 2;
                }
                if (tech_val) {
                    support_weight /= 2;
                }
            } else {
                if (fight_val > 0) {
                    support_weight *= 2;
                }
                if (growth_val > 0) {
                    support_weight += support_weight / 2;
                }
                if (wealth_val > 0) {
                    support_weight /= 2;
                }
                if (tech_val > 0) {
                    support_weight += support_weight / -4;
                }
                if (power_val > 0) {
                    support_weight += support_weight / 2;
                }
            }
            unk_val6 -= support_weight;
            // morale
            int morale_val = range(soc_eff.morale, -4, 4) * 2;
            if (morale_val < -6) {
                morale_val++;
            }
            if (morale_val < -2) {
                morale_val++;
            }
            if (morale_val > 6) {
                morale_val--;
            }
            if (morale_val > 2) {
                morale_val--;
            }
            int morale_weight = (unk_val3 * morale_val * (fight_val + 2) * (unk_sum1 + 1)
                * (PlayersData[faction_id].current_num_bases + PlayersData[faction_id].unk_48)) / 8;
            if (output) {
                if (power_val) {
                    morale_weight *= 3;
                }
                if (growth_val || wealth_val) {
                    morale_weight /= 2;
                }
            } else {
                if (fight_val > 1) {
                    morale_weight *= 2;
                }
                if (power_val && !growth_val && !wealth_val) {
                    morale_weight *= 2;
                }
                if (fight_val < 0 && !power_val && (wealth_val || tech_val) && !unk_sum1) {
                    morale_weight /= 2;
                }
            }
            unk_val6 += morale_weight;
            // efficiency
            int effic_weight = effic_val;
            if (soc_eff.efficiency == -3) {
                effic_weight = 2 * effic_val;
            } else if (soc_eff.efficiency <= -4) {
                effic_weight = 4 * effic_val;
            }
            if (!unk_sum1) {
                effic_weight *= 2;
            }
            if (output) {
                if (tech_val) {
                    effic_weight *= 3;
                }
                if (growth_val || power_val) {
                    effic_weight /= 2;
                }
            } else {
                if (!wealth_val && !tech_val) {
                    if (power_val) {
                        effic_weight /= 2;
                    }
                } else if (power_val) {
                    if (!wealth_val) {
                        effic_weight /= 2;
                    }
                } else if (!growth_val) {
                    effic_weight *= 2;
                }
            }
            unk_val6 -= effic_weight;
            // growth
            int growth_weight = (soc_eff.growth * (unk_count1 + 1) 
                * ((growth_val + 1) * 2 - wealth_val)
                * (pop_goal_growth + PlayersData[faction_id].nutrient_surplus_total)) / 5;
            if (aah_ooga(faction_id, faction_id)) {
                growth_weight *= 2;
            }
            if (Players[faction_id].rule_population > 0) {
                growth_weight /= 2;
            }
            if (output) {
                if (growth_val || wealth_val) {
                    growth_weight *= 2;
                }
                if (tech_val) {
                    growth_weight /= 2;
                }
            } else {
                if (growth_val && power_val) {
                    growth_weight *= 2;
                }
            }
            unk_val6 += growth_weight;
            // police
            int police_weight 
                = range(soc_eff.police, -2, 2) * PlayersData[faction_id].current_num_bases 
                * (unk_count2 * 2 + 1);
            if (PlayersData[faction_id].tech_ranking 
                < PlayersData[*RankingFactionIDUnk2].tech_ranking
                && PlayersData[faction_id].ranking < PlayersData[*RankingFactionIDUnk2].ranking) {
                police_weight *= 2;
            }
            if (PlayersData[faction_id].tech_ranking 
                < PlayersData[*RankingFactionIDUnk1].tech_ranking
                && PlayersData[faction_id].ranking < PlayersData[*RankingFactionIDUnk1].ranking) {
                police_weight /= 2;
            }
            if (output && (power_val || growth_val)) {
                police_weight *= 2;
            }
            unk_val6 += police_weight / (2 - fight_val);
            // talent
            int talent_weight = PlayersData[faction_id].unk_39[range(soc_eff.talent + 4, 0, 7)]
                [range(soc_eff.police + 5, 0, 8)];
            if (unk_sum1) {
                talent_weight += soc_eff.police * PlayersData[faction_id].current_num_bases;
            }
            if (output) {
                if (power_val || growth_val) {
                    talent_weight *= 2;
                }
                if (wealth_val) {
                    talent_weight /= 2;
                }
            } else {
                if (growth_val && tech_val) {
                    talent_weight *= 2;
                }
                if (power_val && (tech_val || wealth_val)) {
                    talent_weight /= 2;
                }
            }
            unk_val6 += talent_weight;
            // planet
            int planet_weight = (4 - soc_eff.planet) * PlayersData[faction_id].unk_49 * 4 / 4; // ?
            if (PlayersData[faction_id].soc_effect_base.planet > 0
                || PlayersData[faction_id].best_psi_offense 
                > PlayersData[faction_id].best_weapon_value) {
                planet_weight += PlayersData[faction_id].total_mil_veh;
            } else if (unk_val3 > 1) {
                planet_weight /= 2;
            }
            if (output) {
                if (growth_val) {
                    planet_weight *= 2;
                }
                if (wealth_val) {
                    planet_weight /= 2;
                }
            } else {
                if (power_val) {
                    if (growth_val) {
                        planet_weight /= 2;
                    }
                    if ((tech_val || wealth_val) && !growth_val) {
                        planet_weight /= 2;
                    }
                } else {
                    if (growth_val) {
                        if (tech_val) {
                            planet_weight *= 2;
                            if (!wealth_val && growth_val) {
                                planet_weight /= 2;
                            }
                        } else {
                            if (growth_val) {
                                planet_weight /= 2;
                            }
                        }
                    } else {
                        if ((tech_val || wealth_val) && growth_val) {
                            planet_weight /= 2;
                        }
                    }
                }
                unk_val6 -= planet_weight;
                // research
                int research_weight = range(soc_eff.research, -5, 5) 
                    * PlayersData[faction_id].labs_total / 10;
                if (!unk_sum1) {
                    research_weight *= 2;
                }
                if (output) {
                    if (tech_val) {
                        research_weight *= 2;
                    }
                    if (power_val) {
                        research_weight /= 2;
                    }
                }
                unk_val6 += research_weight;
                // industry
                uint32_t unk_val3_temp = range(unk_val3, 1, 2);
                int industry_weight = soc_eff.industry * unk_val3_temp
                    * PlayersData[faction_id].pop_total * (2 * (wealth_val + unk_sum1) + 1) / 10;
                if (power_val) {
                    industry_weight *= 2;
                }
                if (wealth_val) {
                    industry_weight += industry_weight / 2;
                }
                if (tech_val) {
                    industry_weight /= 2;
                }
                if (fight_val < 0 && !wealth_val) {
                    industry_weight /= 2;
                }
                unk_val6 += industry_weight;
                // why is this before probe? significance?
                unk_val6 += social_upheaval(faction_id, &soc_cat) / -3;
                // probe
                if (soc_eff.probe) {
                    int probe_weight;
                    for (uint32_t f = 0; f < MaxPlayerNum; f++) {
                        if (f != faction_id) {
                            probe_weight = PlayersData[i].mind_control_total / 4 + 1;
                            if (has_treaty(faction_id, i, DTREATY_PACT)) {
                                probe_weight /= 2;
                            }
                            if (!has_treaty(faction_id, i, DTREATY_WANT_REVENGE)) {
                                if (has_treaty(faction_id, i, DTREATY_TREATY)) {
                                    probe_weight /= 2;
                                }
                                if (!has_treaty(faction_id, i, DTREATY_COMMLINK)) {
                                    probe_weight /= 2;
                                }
                                if (!has_treaty(faction_id, i, DTREATY_VENDETTA)) {
                                    probe_weight /= 2;
                                }
                            }
                            probe_weight = ((PlayersData[i].current_num_bases
                                + PlayersData[faction_id].current_num_bases)
                                * range(soc_eff.probe, -2, 3) * probe_weight) / 2;
                            if (tech_val || wealth_val) {
                                probe_weight *= 2;
                            }
                            unk_val6 += probe_weight;
                        }
                    }
                }
                if (unk_val6 >= unk_val4) {
                    unk_val4 = unk_val6;
                    soc_cat_bits = unk_sum2;
                }
            }
        }
    }
    if (soc_cat_bits < 0) {
        if (output) {
            for (int i = 0; i < MaxSocialCatNum; i++) {
                *(&output->politics + i)
                    = *(&PlayersData[faction_id].soc_category_pending.politics + i);
            }
        }
    } else {
        if (!output) {
            BOOL no_cat_change = true;
            for (int i = 0; i < MaxSocialCatNum; i++) {
                uint32_t cat_bit = soc_cat_bits & 3;
                if (cat_bit != *(&PlayersData[faction_id].soc_category_pending.politics + i)) {
                    *(&PlayersData[faction_id].soc_category_pending.politics + i) = cat_bit;
                    no_cat_change = false;
                }
                soc_cat_bits >>= 2;
            }
            if (!no_cat_change) {
                social_set(faction_id);
                uint32_t cost 
                    = social_upheaval(faction_id, &PlayersData[faction_id].soc_category_pending);
                PlayersData[faction_id].energy_reserves 
                    = range(PlayersData[faction_id].energy_reserves - cost, 0, 999999999);
                PlayersData[faction_id].soc_upheaval_cost_paid += cost;
                if (!is_human(faction_id)) {
                    consider_designs(faction_id);
                }
            }
        } else {
            for (int i = 0; i < MaxSocialCatNum; i++) {
                *(&output->politics + i) = soc_cat_bits & 3;
                soc_cat_bits >>= 2;
            }
        }
    }
}

/*
Purpose: Calculate specified faction's best available weapon and armor ratings as well as the
         fastest moving ground Veh chassis. Compare these capabilities to faction's best opponent
         capabilities based on current diplomacy.
Original Offset: 00560DD0
Return Value: n/a
Status: Complete - testing / WIP
*/
void __cdecl enemy_capabilities(uint32_t faction_id) {
    BOOL has_worms = veh_avail(BSC_MIND_WORMS, faction_id, -1);
    PlayersData[faction_id].best_psi_offense = has_worms 
        ? weap_strat(WPN_PSI_ATTACK, faction_id) : 0;
    PlayersData[faction_id].best_weapon_value = 1;
    for (int i = 0; i < MaxWeaponNum; i++) {
        if (has_tech(Weapon[i].preq_tech, faction_id) && Weapon[i].offense_rating < 99) {
            int weap_val = weap_strat(i, faction_id);
            if (Weapon[i].offense_rating < 0 && weap_val 
                > PlayersData[faction_id].best_psi_offense) {
                PlayersData[faction_id].best_psi_offense = weap_val;
            }
            if (weap_val > PlayersData[faction_id].best_weapon_value) {
                PlayersData[faction_id].best_weapon_value = weap_val;
            }
        }
    }
    PlayersData[faction_id].best_psi_defense = has_worms 
        ? arm_strat(ARM_PSI_DEFENSE, faction_id) : 0;
    PlayersData[faction_id].best_armor_value = 1;
    for (int i = 0; i < MaxArmorNum; i++) {
        if (has_tech(Armor[i].preq_tech, faction_id)) {
            int arm_val = arm_strat(i, faction_id);
            if (Armor[i].defense_rating < 0 && arm_val > PlayersData[faction_id].best_psi_defense) {
                PlayersData[faction_id].best_psi_defense = arm_val;
            }
            if (arm_val > PlayersData[faction_id].best_armor_value) {
                PlayersData[faction_id].best_armor_value = arm_val;
            }
        }
    }
    PlayersData[faction_id].best_land_speed = 1;
    for (int i = 0; i < MaxChassisNum; i++) {
        if (has_tech(Chassis[i].preq_tech, faction_id) && Chassis[i].triad == TRIAD_LAND) {
            if (Chassis[i].speed > PlayersData[faction_id].best_land_speed) {
                PlayersData[faction_id].best_land_speed = Chassis[i].speed;
            }
        }
    }
    PlayersData[faction_id].enemy_best_weapon_value = 0;
    PlayersData[faction_id].enemy_best_armor_value = 0;
    PlayersData[faction_id].enemy_best_land_speed = 0;
    PlayersData[faction_id].enemy_best_psi_offense = 0;
    PlayersData[faction_id].enemy_best_psi_defense = 0;
    for (int i = 0; i < 4 && !PlayersData[faction_id].enemy_best_weapon_value; i++) {
        // 1st pass: vendetta, no treaty, has commlink
        // 2nd pass: no treaty, has commlink
        // 3rd pass: has commlink
        // 4th pass: any non-pact faction
        for (uint32_t j = 1; j < MaxPlayerNum; j++) {
            if (j != faction_id
                && !has_treaty(i, j, DTREATY_PACT)
                && ((!i && has_treaty(i, j, DTREATY_VENDETTA) && !has_treaty(i, j, DTREATY_TREATY)
                    && has_treaty(i, j, DTREATY_COMMLINK)) 
                    || (i == 1 && !has_treaty(i, j, DTREATY_TREATY) 
                        && has_treaty(i, j, DTREATY_COMMLINK))
                    || (i == 2 && has_treaty(i, j, DTREATY_COMMLINK)) || (i == 3))) {
                if (PlayersData[faction_id].enemy_best_weapon_value 
                    < PlayersData[j].best_weapon_value) {
                    PlayersData[faction_id].enemy_best_weapon_value 
                        = PlayersData[j].best_weapon_value;
                }
                if (PlayersData[faction_id].enemy_best_armor_value 
                    < PlayersData[j].best_armor_value) {
                    PlayersData[faction_id].enemy_best_armor_value 
                        = PlayersData[j].best_armor_value;
                }
                if (PlayersData[faction_id].enemy_best_land_speed 
                    < PlayersData[j].best_land_speed) {
                    PlayersData[faction_id].enemy_best_land_speed = PlayersData[j].best_land_speed;
                }
                if (PlayersData[faction_id].enemy_best_psi_offense 
                    < PlayersData[j].best_psi_offense) {
                    PlayersData[faction_id].enemy_best_psi_offense 
                        = PlayersData[j].best_psi_offense;
                }
                if (PlayersData[faction_id].enemy_best_psi_defense 
                    < PlayersData[j].best_psi_defense) {
                    PlayersData[faction_id].enemy_best_psi_defense 
                        = PlayersData[j].best_psi_defense;
                }
            }
        }
    }
}

void __cdecl enemy_capabilities_t(uint32_t faction_id) {
    // * PSI could potentially be best weapon?
    // * PSI should always be last Weapon
    // * faction order will affect initial run through
    // > potential fix would be to calculate all factions at once before enemy best compares

    //BOOL has_worms = veh_avail(BSC_MIND_WORMS, faction_id, -1);
    //PlayersData[faction_id].bestPsiAtkVal = has_worms ? weap_strat(WPN_PSI_ATTACK, faction_id) : 0;
    PlayersData[faction_id].best_weapon_value = 1;
    for (int i = 0; i < MaxWeaponNum; i++) {
        if (has_tech(Weapon[i].preq_tech, faction_id) && Weapon[i].offense_rating < 99) {
            int weap_val = weap_strat(i, faction_id);
            //if (Weapon[i].offense_rating < 0 && weap_val > PlayersData[faction_id].bestPsiAtkVal) {
            //    PlayersData[faction_id].bestPsiAtkVal = weap_val;
            //}
            if (Weapon[i].offense_rating >= 0 && weap_val 
                > PlayersData[faction_id].best_weapon_value) {
                PlayersData[faction_id].best_weapon_value = weap_val;
            }
        }
    }
    //PlayersData[faction_id].bestPsiDefVal = has_worms ? arm_strat(ARM_PSI_DEFENSE, faction_id) : 0;
    PlayersData[faction_id].best_armor_value = 1;
    for (int i = 0; i < MaxArmorNum; i++) {
        if (has_tech(Armor[i].preq_tech, faction_id)) {
            int arm_val = arm_strat(i, faction_id);
            //if (Armor[i].defense_rating < 0 && arm_val > PlayersData[faction_id].bestPsiDefVal) {
            //    PlayersData[faction_id].bestPsiDefVal = arm_val;
            //}
            if (Armor[i].defense_rating >= 0 
                && arm_val > PlayersData[faction_id].best_armor_value) {
                PlayersData[faction_id].best_armor_value = arm_val;
            }
        }
    }
    PlayersData[faction_id].best_land_speed = 1;
    for (int i = 0; i < MaxChassisNum; i++) {
        if (has_tech(Chassis[i].preq_tech, faction_id) && Chassis[i].triad == TRIAD_LAND) {
            if (Chassis[i].speed > PlayersData[faction_id].best_land_speed) {
                PlayersData[faction_id].best_land_speed = Chassis[i].speed;
            }
        }
    }
    PlayersData[faction_id].enemy_best_weapon_value = 0;
    PlayersData[faction_id].enemy_best_armor_value = 0;
    PlayersData[faction_id].enemy_best_land_speed = 0;
    //PlayersData[faction_id].enemyBestPsiAtkVal = 0;
    //PlayersData[faction_id].enemyBestPsiDefVal = 0;
    for (int i = 0; i < 4 && !PlayersData[faction_id].enemy_best_weapon_value; i++) {
        // 1st pass: vendetta, no treaty, has commlink
        // 2nd pass: no treaty, has commlink
        // 3rd pass: has commlink
        // 4th pass: any non-pact faction
        for (uint32_t j = 1, treaties; j < MaxPlayerNum; j++) {
            if (j != faction_id
                && (treaties = PlayersData[i].diplo_treaties[j], !(treaties & DTREATY_PACT))
                && ((!i && treaties & DTREATY_VENDETTA && !(treaties & DTREATY_TREATY)
                    && treaties & DTREATY_COMMLINK)
                    || (i == 1 && !(treaties & DTREATY_TREATY) && treaties & DTREATY_COMMLINK)
                    || (i == 2 && treaties & DTREATY_COMMLINK) || (i == 3))) {
                if (PlayersData[faction_id].enemy_best_weapon_value 
                    < PlayersData[j].best_weapon_value) {
                    PlayersData[faction_id].enemy_best_weapon_value 
                        = PlayersData[j].best_weapon_value;
                }
                if (PlayersData[faction_id].enemy_best_armor_value 
                    < PlayersData[j].best_armor_value) {
                    PlayersData[faction_id].enemy_best_armor_value
                        = PlayersData[j].best_armor_value;
                }
                if (PlayersData[faction_id].enemy_best_land_speed 
                    < PlayersData[j].best_land_speed) {
                    PlayersData[faction_id].enemy_best_land_speed = PlayersData[j].best_land_speed;
                }
                /*
                if (PlayersData[faction_id].enemyBestPsiAtkVal < PlayersData[j].bestPsiAtkVal) {
                    PlayersData[faction_id].enemyBestPsiAtkVal = PlayersData[j].bestPsiAtkVal;
                }
                if (PlayersData[faction_id].enemyBestPsiDefVal < PlayersData[j].bestPsiDefVal) {
                    PlayersData[faction_id].enemyBestPsiDefVal = PlayersData[j].bestPsiDefVal;
                }
                */
            }
        }
    }
    // PSI
    BOOL has_worms = veh_avail(BSC_MIND_WORMS, faction_id, -1);
    PlayersData[faction_id].best_psi_offense = has_worms 
        ? weap_strat(WPN_PSI_ATTACK, faction_id) : 0;
    for (int i = 0; i < MaxWeaponNum; i++) {
        if (has_tech(Weapon[i].preq_tech, faction_id) && Weapon[i].offense_rating < 99) {
            int weap_val = weap_strat(i, faction_id);
            if (Weapon[i].offense_rating < 0 && weap_val 
                > PlayersData[faction_id].best_psi_offense) {
                PlayersData[faction_id].best_psi_offense = weap_val;
            }
        }
    }
    PlayersData[faction_id].best_psi_defense = has_worms 
        ? arm_strat(ARM_PSI_DEFENSE, faction_id) : 0;
    for (int i = 0; i < MaxArmorNum; i++) {
        if (has_tech(Armor[i].preq_tech, faction_id)) {
            int arm_val = arm_strat(i, faction_id);
            if (Armor[i].defense_rating < 0 && arm_val > PlayersData[faction_id].best_psi_defense) {
                PlayersData[faction_id].best_psi_defense = arm_val;
            }
        }
    }
    PlayersData[faction_id].enemy_best_psi_offense = 0;
    PlayersData[faction_id].enemy_best_psi_defense = 0;
    for (int i = 0; i < 4 && !PlayersData[faction_id].enemy_best_psi_offense; i++) {
        // 1st pass: vendetta, no treaty, has commlink
        // 2nd pass: no treaty, has commlink
        // 3rd pass: has commlink
        // 4th pass: any non-pact faction
        for (uint32_t j = 1, treaties; j < MaxPlayerNum; j++) {
            if (j != faction_id
                && (treaties = PlayersData[i].diplo_treaties[j], !(treaties & DTREATY_PACT))
                && ((!i && treaties & DTREATY_VENDETTA && !(treaties & DTREATY_TREATY)
                    && treaties & DTREATY_COMMLINK)
                    || (i == 1 && !(treaties & DTREATY_TREATY) && treaties & DTREATY_COMMLINK)
                    || (i == 2 && treaties & DTREATY_COMMLINK) || (i == 3))) {
                if (PlayersData[faction_id].enemy_best_psi_offense 
                    < PlayersData[j].best_psi_offense) {
                    PlayersData[faction_id].enemy_best_psi_offense 
                        = PlayersData[j].best_psi_offense;
                }
                if (PlayersData[faction_id].enemy_best_psi_defense
                    < PlayersData[j].best_psi_defense) {
                    PlayersData[faction_id].enemy_best_psi_defense 
                        = PlayersData[j].best_psi_defense;
                }
            }
        }
    }
}

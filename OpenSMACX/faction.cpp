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
#include "stdafx.h"
#include "faction.h"

rules_social_category *SocialCategory = (rules_social_category *)0x0094B000;
rules_social_effect *SocialEffect = (rules_social_effect *)0x00946580;
LPSTR *Mood = (LPSTR *)0x0094C9E4;
LPSTR *Repute = (LPSTR *)0x00946A30;
rules_might *Might = (rules_might *)0x0094C558;
rules_proposal *Proposal = (rules_proposal *)0x009A6828;
rules_bonusname *BonusName = (rules_bonusname *)0x009461A8;
Player *Players = (Player *)0x00946A50;
faction_art *FactionArt = (faction_art*)0x0078E978;